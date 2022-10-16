// PWM to TB6612 dual H-bridge motor driver, PWM freq. = 1000 Hz
// GPIO to SR04 InfraRed ranger
// I2C to MPU6050 3-axis accelerometer & 3-axis gyroscope
// BLE to receive/transmit data with smartphone app BLE2RC

#include "Wire.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "ESP32_TB6612.h"
#include <MPU6050_6Axis_MotionApps20.h>

MPU6050 imu;

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
bool Dir = false; // forward = true, others = false

// Motor Speed control vars
#define FULLSPEED 1023
#define HALFSPEED 512
int16_t Speed = HALFSPEED;

// Main Loop vars
unsigned long lastTime = 0;
unsigned long loopTime = 50; // 20 Hz 

char *BLErxbuf;
bool BLEflag = false;

// BLE defs & vars
// See the following for generating UUIDs: https://www.uuidgenerator.net/
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        BLErxbuf = (char *)rxValue.c_str();
        BLEflag = true; // set BLE RX flag
      }
    }
};

void BLEprint(String txString){
  int txStringLen;
  uint8_t *txValue;
  
  // output text
  Serial.println(txString);
  txStringLen = txString.length();
  txValue = (uint8_t*)txString.c_str();
  
  // send BLE TX data   
  if (deviceConnected) {
    pTxCharacteristic->setValue(txValue, txStringLen);
    pTxCharacteristic->notify(); 
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent        
  } 
}
 
void read_IMU()
{ 
  imuIntStatus = imu.getIntStatus();

  // get current FIFO count
  fifoCount = imu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((imuIntStatus & 0x10) || fifoCount == 1024) {
    Serial.println("IMU FIFO overflow!");    
    imu.resetFIFO(); // reset so we can continue cleanly
    YPR = lastYPR;
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
  Wire.begin(); // default I2C clock is 100KHz

  // setup SR04 pins
  Serial.println("setup SR04 pins...");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // initialize IMU
  Serial.println("Initializing IMU...");
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
    
  // Create the BLE Device
  BLEDevice::init("RoboCar-BLE");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  SafeDistance = 20; //cm    
}

void loop() {
  String tx;
  char command;
  // Loop Time = 8~15ms
  //Serial.print("loopTime = "); Serial.println(millis()-lastTime);

  //if (millis()-lastTime >= loopTime) { // 100ms loop to read IR ranger & IMU
  //  lastTime = millis();
    distance = read_SR04(); // read Distance from IR Ranger
    read_IMU(); //read IMU to get YPR
    
    tx = String(distance)+","+String(YPR.x)+","+String(YPR.y)+","+String(YPR.z); 
    Serial.println(tx);
    lastYPR = YPR;    
  //}  
 
  // BLE disconnecting
  if (!deviceConnected && oldDeviceConnected) {
     delay(500); // give the bluetooth stack the chance to get things ready
     pServer->startAdvertising(); // restart advertising
     Serial.println("BLE start advertising");
     oldDeviceConnected = deviceConnected;
  }

  // BLE connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
    Serial.println("BLE device connected");
  }
  
  // check BLE flag & rxbuf
  command = 'X';    
  if (BLEflag) {
    Serial.print("BLE rxbuf: "); Serial.println(BLErxbuf); 
    command = BLErxbuf[0];
    BLEflag = false;
  }
  
  // check command to set motor driver
  if (command=='F') { forward(motorR, motorL, Speed); Dir=true; }
  if (command=='B') { backward(motorR, motorL, Speed); Dir=false; }
  if (command=='L') { left(motorR, motorL, Speed); Dir=false; }  
  if (command=='R') { right(motorR, motorL, Speed); Dir=false; }
  if (command=='S') { brake(motorR, motorL); Dir=false; }
      
  // check SafeDistance
  if (Dir && (distance < SafeDistance)) brake(motorR, motorL);
  
  delay(20); // ms
  
}
