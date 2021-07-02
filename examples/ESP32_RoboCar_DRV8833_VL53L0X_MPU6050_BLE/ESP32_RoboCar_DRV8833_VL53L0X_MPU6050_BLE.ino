// PWM to DRV8833 dual H-bridge motor driver, PWM freq. = 1000 Hz
// I2C to VL53L0X InfraRed ranger
// I2C to MPU6050 3-axis accelerometer & 3-axis gyroscope
// BLE to receive command from BLE2RC App

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <VL53L0X.h>                    // VL53L0X
#include <ESP32MotorControl.h>          // DRV8833
#include <MPU6050_6Axis_MotionApps20.h> // MPU6050

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
unsigned long loopTime = 100; // 10 Hz

char tmp[16];
char BLEcmd;
bool movement;

int16_t Distance;
int16_t Yaw, lastYaw, prevYaw;

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
        Serial.print("*** BLE Received : ");
        for (int i = 0; i < rxValue.length(); i++){
          if (i>0) tmp[i-1]=rxValue[i];
          Serial.print(rxValue[i]);
        }
        Serial.println();        
        BLEcmd = rxValue[0];
      }
    }
};

// Interrup Service Routine (ISR)
volatile bool imuInterrupt = false;     // indicates whether IMU interrupt pin has gone high
void dmpDataReady() {
    imuInterrupt = true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(); // default I2C clock is 100KHz
  
  // Motor control pins
  Serial.println("Motor Pins assigned...");
  motor.attachMotors(IN1pin, IN2pin, IN3pin, IN4pin);
  motor.motorStop(motorR);
  motor.motorStop(motorL);
  
  // VL53L0X IR ranger                    
  ranger.init();
  ranger.setTimeout(500);
  ranger.startContinuous();
   
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

  // Create the BLE Device
  BLEDevice::init("ESP32-UART");

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
}

void loop() {      
  // Loop Time = ~33ms with reading IR Ranger & IMU
  //Serial.print("loopTime = "); Serial.println(millis()-lastTime);
  //lastTime = millis();  
  
  if ((millis()-lastTime) >= loopTime) { // 100ms loop to read IR Ranger & IMU
    Yaw  = GetYaw(); //read Yaw from IMU
    delay(100);
    Distance = ranger.readRangeSingleMillimeters(); // read Distance from IR Ranger
    if (ranger.timeoutOccurred()) Serial.println("IR Ranger TIMEOUT");      
    lastTime = millis();
  }
  
  if (Yaw!=lastYaw) { 
    Serial.print("D=");Serial.print(Distance);Serial.print(", Y=");Serial.println(Yaw);
    BLEsend(Distance, Yaw);
  }
  lastYaw = Yaw;
    
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
 
    // check BLE command
  if (BLEcmd!=' ') {
    Serial.print("BLEcmd = "); Serial.println(BLEcmd); 
    if (BLEcmd=='S') BLEsend(Distance, Yaw);
    if (BLEcmd=='P') Speed = String(tmp).toInt();
    
    switch (BLEcmd) {
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
  BLEcmd=' ';

  // set SafeDistance from Collision in mm
  if      (Speed<=12) SafeDistance = 100;
  else if (Speed<=24) SafeDistance = 200; 
  else if (Speed<=36) SafeDistance = 300;  
  else                SafeDistance = 400;
  
  // avoid collision
  if (Distance < SafeDistance && movement) {
    motor.motorStop(motorR);
    motor.motorStop(motorL);
  }
}

void BLEsend(int D, int Y){
  String txString;  
  int txStringLen;
  uint8_t *txValue;
  
  // output text
  txString = "D=" + String(D) +",Y=" +String(Y);
  txStringLen = txString.length();
  Serial.print(txString); Serial.print("\t"); Serial.println(txStringLen);
  txValue = (uint8_t*)txString.c_str();
  
  // send BLE TX data   
  if (deviceConnected) {
    pTxCharacteristic->setValue(txValue, txStringLen);
    pTxCharacteristic->notify(); 
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent        
  } 
}
  
int GetYaw()                                                                                                                                                
{ 
  int y;
  // reset interrupt flag and get INT_STATUS byte
  imuInterrupt = false;
  imuIntStatus = imu.getIntStatus();

  // get current FIFO count
  fifoCount = imu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((imuIntStatus & 0x10) || fifoCount == 1024) {
    Serial.println("IMU FIFO overflow!");  
    imu.resetFIFO(); // reset so we can continue cleanly
    return lastYaw;
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
    y = int((ypr[0] * 180/M_PI)) + 180;    
    return y;
  }
}
