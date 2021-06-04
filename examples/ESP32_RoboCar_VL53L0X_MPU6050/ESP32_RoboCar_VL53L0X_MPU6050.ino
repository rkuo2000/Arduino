// PWM to DRV8833 dual H-bridge motor driver, PWM freq. = 1000 Hz
// I2C to VL53L0X InfraRed ranger

// Connect Vin to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin (UNO: A5, NodeMCU: D1, ESP32: GIOP22)
// Connect SDA to I2C data pin  (UNO: A4, NodeMCU: D2, ESP32: GIOP21)

#include <Wire.h>
#include "VL53L0X.h"
#include "ESP32MotorControl.h"
#include "MPU6050_6Axis_MotionApps20.h"

// DRV8833 pin connection
#define IN1pin 19  
#define IN2pin 18  
#define IN3pin 23 
#define IN4pin 19

#define motorR 0
#define motorL 1

#define FULLSPEED 100
#define SLOWSPEED 30

ESP32MotorControl motor;

VL53L0X ranger;

MPU6050 imu;


enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

// Specify sensor full scale
int Gscale = GFS_250DPS;
int Ascale = AFS_2G;
float aRes, gRes; // scale resolutions per LSB for the sensors

// IMU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t imuIntStatus;   // holds actual interrupt status byte from IMU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 gyro;       // [x, y, z]            gyro sensor measurements
VectorInt16 acc;        // [x, y, z]            accel sensor measurements
VectorInt16 accReal;    // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 accWorld;   // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$',0x02, 0,0,0,0,0,0,0,0,0x00,0x00,'\r','\n' };

static float  Heading, HeadingTgt;
int angle;

// IR Ranger control/status vars
int distance;
int SafeDistance = 100;

// Motor Speed control vars
int speed = SLOWSPEED;

// Main Loop vars
unsigned long lastTime = 0;
unsigned long loopDelay = 100; //ms

void getGres() {
  switch (Gscale)
  {
   // Possible gyro scales (and their register bit settings) are:
  // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case GFS_250DPS:
          gRes = 250.0/32768.0;
          break;
    case GFS_500DPS:
          gRes = 500.0/32768.0;
          break;
    case GFS_1000DPS:
          gRes = 1000.0/32768.0;
          break;
    case GFS_2000DPS:
          gRes = 2000.0/32768.0;
          break;
  }
}

void getAres() {
  switch (Ascale)
  {
   // Possible accelerometer scales (and their register bit settings) are:
  // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case AFS_2G:
          aRes = 2.0/32768.0;
          break;
    case AFS_4G:
          aRes = 4.0/32768.0;
          break;
    case AFS_8G:
          aRes = 8.0/32768.0;
          break;
    case AFS_16G:
          aRes = 16.0/32768.0;
          break;
  }
}

// Interrup Service Routine (ISR)
volatile bool imuInterrupt = false;     // indicates whether IMU interrupt pin has gone high
void dmpDataReady() {
    imuInterrupt = true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  ranger.init();
  ranger.setTimeout(500);
  ranger.startContinuous();
  
  // set SafeDistance from Collision in mm
  if      (speed<=30) SafeDistance = 100;
  else if (speed<=50) SafeDistance = 200; 
  else                SafeDistance = 500;
    
  Serial.println("Motor Pins assigned...");
  motor.attachMotors(IN1pin, IN2pin, IN3pin, IN4pin);

  motor.motorStop(motorR);
  motor.motorStop(motorL);

  // initialize IMU
  Serial.println("Initializing IMU...");
  imu.initialize();
  
   // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(imu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
 
  // load and configure the DMP
  Serial.println("Initializing DMP...");
  devStatus = imu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  imu.setXGyroOffset(220);
  imu.setYGyroOffset(76);
  imu.setZGyroOffset(-85);
  imu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println("Enabling DMP...");
    imu.setDMPEnabled(true);

    Serial.println("DMP ready!");
    dmpReady = true;
        
    // get expected DMP packet size for later comparison
    packetSize = imu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print("DMP Initialization failed (code ");
    Serial.print(devStatus);
    Serial.println(")");
  }

  //  read heading till it is stable
  Serial.println("reading IMU till it is stable...");  

  //for (int i=0;i<200;i++) {
  //    Serial.print(i);
  //    GetHeading(&Heading); 
  //    delay(100);
  //}

  Serial.println();
  Serial.println("reading IMU done!");
  angle = 0;

  // set target heading to default heading
  GetHeading(&Heading); 
  HeadingTgt = Heading + angle;
  if (HeadingTgt>=360) HeadingTgt = HeadingTgt - 360;
  else if (HeadingTgt<0) HeadingTgt = HeadingTgt + 360;
  Serial.print("Heading Target = \t");
  Serial.println(HeadingTgt);  
}

void loop() {
  if ((millis() - lastTime)> loopDelay){
    //Serial.println(millis()-lastTime);
    lastTime = millis();

    // get heading from IMU
    GetHeading(&Heading);
    Serial.print("Yaw:\t");
    Serial.print(Heading);
    Serial.print("\t");
    Serial.print(HeadingTgt);
    Serial.print("\t");
    // get distance from IR ranger 
    distance = ranger.readRangeContinuousMillimeters() - 50;
    Serial.print(distance);
    Serial.println(" mm");
  
    // control motors
    if (distance < SafeDistance) {
      motor.motorStop(motorR);
      motor.motorStop(motorL);     
    } else {
      motor.motorForward(motorR, speed);
      motor.motorForward(motorL, speed);
    }
  }
}

void  GetHeading(float *Heading)                                                                                                                                                   
{
  //calc heading from IMU
  // reset interrupt flag and get INT_STATUS byte
  imuInterrupt = false;
  imuIntStatus = imu.getIntStatus();

  // get current FIFO count
  fifoCount = imu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((imuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    imu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } 
  else if (imuIntStatus & 0x02) 
  {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = imu.getFIFOCount();

    // read a packet from FIFO
    imu.getFIFOBytes(fifoBuffer, packetSize);
        
    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;
          
    // display Euler angles in degrees
    imu.dmpGetQuaternion(&q, fifoBuffer);
    imu.dmpGetGravity(&gravity, &q);
    imu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    *Heading = int((ypr[0] * 180/M_PI)) + 180;    
  }//done
}//END GetHeading
