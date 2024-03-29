// PWM to TB6612 dual H-bridge motor driver, PWM freq. = 1000 Hz
// GPIO to HC-SR04 InfraRed ranger
// I2C to MPU6050 3-axis accelerometer & 3-axis gyroscope
// BTserial to receive command or transmit data to smartphone

#include "Wire.h"
#include <BluetoothSerial.h>
#include "ESP32_TB6612.h"
#include <MPU6050_6Axis_MotionApps20.h>

MPU6050 imu;
BluetoothSerial SerialBT;

// TB6612 pin connection
#define STBY 23
#define PWMA  5   // 1KHz
#define AIN2 19
#define AIN1 18
#define BIN1 17
#define BIN2 16
#define PWMB  4

// value 1 or -1 for motor spining default
const int offsetA = 1;
const int offsetB = 1;

Motor motorR = Motor(AIN1, AIN2, PWMA, 0, offsetA, STBY);
Motor motorL = Motor(BIN1, BIN2, PWMB, 1, offsetB, STBY);

const int trigPin = 13;
const int echoPin = 12;

// IMU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t imuIntStatus;   // holds actual interrupt status byte from IMU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
VectorInt16 YPR, lastYPR;

// IR Ranger control/status vars
int16_t SafeDistance;
int16_t distance; 
bool Dir = false;
  
// Motor Speed control vars
#define FULLSPEED 1023
#define HALFSPEED 512
int16_t Speed = HALFSPEED;

// Main Loop vars
unsigned long lastTime = 0;
unsigned long loopTime = 50; // 20 Hz 

void read_IMU()                                                                                                                                                
{ 
  imuIntStatus = imu.getIntStatus();

  // get current FIFO count
  fifoCount = imu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((imuIntStatus & 0x10) || fifoCount == 1024) {  
    imu.resetFIFO(); // reset so we can continue cleanly
    YPR = lastYPR;
    Serial.println("IMU FIFO overflow!"); 
    // otherwise, check for DMP data ready interrupt (this should happen fr  equently)
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
    YPR.x = int((ypr[0] * 180/M_PI)) + 180;
    YPR.y = int((ypr[1] * 180/M_PI)) + 180;
    YPR.z = int((ypr[2] * 180/M_PI)) + 180;   
  }
}


float read_SR04()
{
    float duration, distance;

    // trigger a 10us HIGH pulse at trigger pin
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // measure time cost of pulse HIGH at echo pin
    duration = pulseIn (echoPin, HIGH);

    // calculate the distance from duration
    distance = duration / 58;

    Serial.print(distance);
    Serial.println(" cm");
    return distance;
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("RoboCar_BT");
  Wire.begin();
  
  // setup SR04 pins
  Serial.println("setup SR04 pins...");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  

  // initialize IMU
  Serial.println("Initializing I2C devices...");
  imu.initialize();
  
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

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    //Serial.println("DMP ready! Waiting for first interrupt...");
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
  
  SafeDistance = 20; //cm
}

void loop() {
  uint8_t i, buflen;
  char BTbuf[16];
  char command;
  // Loop Time = 8~15ms with reading Ultra-Sound Ranger & IMU
  //Serial.print("loopTime = "); Serial.println(millis()-lastTime); 

  //if (millis()-lastTime >= loopTime) {  
  //  lastTime = millis();
    distance =  read_SR04(); // read distance from SR04 
    read_IMU(); //read IMU (MPU6050) to get YPR      
    Serial.print(distance); Serial.print(", "); 
    Serial.print(YPR.x); Serial.print(", "); 
    Serial.print(YPR.y); Serial.print(", "); 
    Serial.println(YPR.z); 
    lastYPR = YPR;    
  //}  
 
  // check Bluetooth
  i = 0;
  command = 'X';
  while (SerialBT.available()>0) {
      BTbuf[i]=SerialBT.read();
      i++;
  }
  if (i>0) {
    command = BTbuf[0];
    buflen = i;
    for (i=0; i<buflen; i++) Serial.write(BTbuf[i]); 
  }  
  Serial.print("Command = "); Serial.println(command);
  
  // check command to set motor driver
  if (command=='F') { forward(motorR, motorL, Speed); Dir=true; }
  if (command=='B') { backward(motorR, motorL, Speed); Dir=false; }
  if (command=='L') { left(motorR, motorL, Speed); Dir=false; }  
  if (command=='R') { right(motorR, motorL, Speed); Dir=false; }
  if (command=='S') { brake(motorR, motorL); Dir=false; }

  if (Dir && distance < SafeDistance) brake(motorR, motorL);
          
  delay(20); // ms
}
 
