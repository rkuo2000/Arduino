// PWM to DRV8833 dual H-bridge motor driver, PWM freq. = 1000 Hz
// I2C to VL53L0X InfraRed ranger
// I2C to MPU6050 3-axis accelerometer & 3-axis gyroscope
// BTserial to receive command or transmit data to smartphone

#include <BluetoothSerial.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <ESP32MotorControl.h>
#include <MPU6050_6Axis_MotionApps20.h>

BluetoothSerial SerialBT;

// DRV8833 pin connection
#define IN1pin 16  
#define IN2pin 17  
#define IN3pin 18 
#define IN4pin 19

#define motorR 0  
#define motorL 1

ESP32MotorControl motor;

VL53L0X ranger;

MPU6050 imu;

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

// IR Ranger control/status vars
int SafeDistance = 200;

// Motor Speed control vars
#define FULLSPEED 100
#define USERSPEED 30 // for 3.7Vx1, 25 for 3.7Vx2
int Speed = USERSPEED;

// Main Loop vars
unsigned long lastTime = 0;
unsigned long loopTime = 50; // 20 Hz

char BTbuf[16];
char BTcmd;
bool movement;

VectorInt16 YPR;
VectorInt16 lastYPR;

void setup() {
  Serial.begin(115200);
  Wire.begin(); // default I2C clock is 100KHz
  
  if(!SerialBT.begin("ESP32_BT")){
    Serial.println("An error occurred initializing Bluetooth");
  }else{
    Serial.println("Bluetooth initialized");
  }
  
  // Motor control pins
  Serial.println("Motor Pins assigned...");
  motor.attachMotors(IN1pin, IN2pin, IN3pin, IN4pin);
  motor.motorStop(motorR);
  motor.motorStop(motorL);
  
  // Ranger                    
  ranger.init();
  ranger.setTimeout(500);
  ranger.startContinuous(loopTime);
   
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

}

void loop() { 
  int distance =0;
  char c;   
  uint8_t i;
  // Loop Time = ~33ms with reading IR Ranger & IMU
  //Serial.print("loopTime = "); Serial.println(millis()-lastTime);
  //lastTime = millis();  
  
  if (millis()-lastTime>=loopTime) {  
    YPR = GetYPR(); //get YPR from IMU        
    distance = ranger.readRangeContinuousMillimeters(); // read distance from IR Ranger
    if (ranger.timeoutOccurred()) Serial.println("IR Ranger TIMEOUT");      

    Serial.print(distance); Serial.print(", "); 
    Serial.print(YPR.x); Serial.print(", "); 
    Serial.print(YPR.y); Serial.print(", "); 
    Serial.println(YPR.z); 
    SerialBT.println(String(distance)+", "+String(YPR.x)+", "+String(YPR.y)+", "+String(YPR.z));
	  lastTime = millis(); 
  }
  
  lastYPR = YPR;

  // 
  if (SerialBT.available()) {
    Serial.println("BT receiving data...");
    i=0;
    while(c!='\n') {
      BTbuf[i]=SerialBT.read();
      i++;
    }
	  BTcmd = BTbuf[0];
  }
  
  // check BLE command
  if (BTcmd!=' ') {
    Serial.print("BTcmd = "); Serial.println(BTcmd);
    
    switch (BTcmd) {
      case 'F': motor.motorForward(motorR, Speed);  
                motor.motorForward(motorL, Speed);
                movement = true;
                break;
      case 'B': motor.motorReverse(motorR, Speed);
                motor.motorReverse(motorL, Speed); 
                movement = false;
                break;
      case 'R': motor.motorReverse(motorR, Speed);
                motor.motorForward(motorL, Speed);
                movement = true;
                break;
      case 'L': motor.motorForward(motorR, Speed);
                motor.motorReverse(motorL, Speed);
                movement = true;
                break;
      case 'S': motor.motorStop(motorR);
                motor.motorStop(motorL); 
                movement = false;
                break;
    }
  }
  BTcmd=' ';

  // set SafeDistance from Collision in mm
  if      (Speed<=12) SafeDistance = 100;
  else if (Speed<=24) SafeDistance = 200; 
  else if (Speed<=36) SafeDistance = 300;  
  else                SafeDistance = 400;
  
  // avoid collision
  if (distance < SafeDistance && movement) {
    motor.motorStop(motorR);
    motor.motorStop(motorL);
  }
}
  
VectorInt16 GetYPR()                                                                                                                                                
{ 
  VectorInt16 ypr16;
  imuIntStatus = imu.getIntStatus();

  // get current FIFO count
  fifoCount = imu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((imuIntStatus & 0x10) || fifoCount == 1024) {
    Serial.println("IMU FIFO overflow!");  
    imu.resetFIFO(); // reset so we can continue cleanly
    return lastYPR;
    // otherwise, check for DMP data ready interrupt (this should happen fr  equently)
  } 
  else if (imuIntStatus & 0x01) 
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
    ypr16.x = int((ypr[0] * 180/M_PI)) + 180;
    ypr16.y = int((ypr[1] * 180/M_PI)) + 180;
    ypr16.z = int((ypr[2] * 180/M_PI)) + 180;    
    return ypr16;
  }
}
