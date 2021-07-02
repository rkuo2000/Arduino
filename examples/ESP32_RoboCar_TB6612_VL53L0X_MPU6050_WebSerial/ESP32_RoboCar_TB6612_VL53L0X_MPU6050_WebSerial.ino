
// PWM to TB6612 dual H-bridge motor driver, PWM freq. = 1000 Hz
// I2C to VL53L0X InfraRed ranger
// I2C to MPU6050 3-axis accelerometer & 3-axis gyroscope
// WebSerial to receive command & upload data to a connected PC in python

#include <WebSerial.h>
#include <Wire.h>
#include <VL53L0X.h>
#include "ESP32_TB6612.h"
#include <MPU6050_6Axis_MotionApps20.h>

AsyncWebServer server(80);

const char* ssid = "Kuo";
const char* password = "0972211921";
//const char* PA

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
int SafeDistance;

// Motor Speed control vars
#define FULLSPEED 1023
#define USERSPEED 255
int16_t Speed = USERSPEED;

// Main Loop vars
unsigned long lastTime = 0;
unsigned long loopTime = 50; // 20 Hz 
String WiFibuf;
char WiFicmd;
int16_t Yaw, lastYaw;
VectorInt16 YPR;
VectorInt16 lastYPR;

void recvMsg(uint8_t *data, size_t len)
{
  String d = (char*)data;
  Serial.print("WebSerial Received: "); Serial.println(d);
  WiFibuf = d;
  WiFicmd = d[0];
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

void setup() {
  Serial.begin(115200);
  Wire.begin(); // default I2C clock is 100KHz

  WiFi.mode(WIFI_STA); 
  WiFi.begin(ssid, password);
     
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Could not initialize WiFi");
    return;
  }
  Serial.println(WiFi.localIP());
     
  // WebSerial is accessible at "<IP Address>/webserial" in browser  
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  
  // VL53L0X IR ranger                    
  ranger.init();
  ranger.setTimeout(500);
  ranger.startContinuous(loopTime); // desired inter-measure period in ms
  
  // set SafeDistance from Collision in mm
  if      (Speed<128) SafeDistance = 100;
  else if (Speed<256) SafeDistance = 200; 
  else if (Speed<384) SafeDistance = 300;  
  else                SafeDistance = 400;
  Serial.print("SafeDistance = "); Serial.println(SafeDistance);

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
  
  Serial.println("Wait a few seconds for stable MPU6050!!!");
  
  server.begin();
}

void loop() {
  int distance =0;
  // Loop Time = ~44ms
  //Serial.print("loopTime = "); Serial.println(millis()-lastTime);
  //lastTime = millis();

  if (millis()-lastTime>=loopTime) {
    YPR  = GetYPR(); //read YPR from IMU  
    distance = ranger.readRangeContinuousMillimeters(); // read Distance from IR Ranger
    if (ranger.timeoutOccurred()) Serial.println("IR Ranger TIMEOUT");   

    Serial.print(distance); Serial.print(", "); 
    Serial.print(YPR.x); Serial.print(", "); 
    Serial.print(YPR.y); Serial.print(", "); 
    Serial.println(YPR.z); 
    WebSerial.println(String(distance)+", "+String(YPR.x)+", "+String(YPR.y)+", "+String(YPR.z));
    lastTime = millis();
  }

  lastYPR = YPR;
  
  // check WiFi command
  if (WiFicmd!=' ') {
    Serial.print("WiFicmd = "); Serial.println(WiFicmd);    
    switch (WiFicmd) {
      case 'F': forward(motorR, motorL, Speed);
                break;
      case 'B': backward(motorR, motorL, Speed);
                break;
      case 'R': right(motorR, motorL, Speed);           
                break;
      case 'L': left(motorR, motorL, Speed);
                break;
      case 'S': brake(motorR, motorL);
                break;
    }
  }
  
  // detect collision
  if (distance < SafeDistance && WiFicmd!='B') {
    brake(motorR, motorL);
  }
  WiFicmd=' ';  
  
}
 
