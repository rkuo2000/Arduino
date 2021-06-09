// PWM to DRV8833 dual H-bridge motor driver, PWM freq. = 1000 Hz
// ESP32 BLE uart to receive commands to control RoboCar
// BLE need Vin=5V (3.7V won't work)

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "ESP32MotorControl.h"

// DRV8833 pin connection
#define IN1pin 18  
#define IN2pin 5  
#define IN3pin 23 
#define IN4pin 19

#define motorR 0
#define motorL 1
#define FULLSPEED 100
#define USERSPEED 40

ESP32MotorControl motor;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

int speed = USERSPEED;
int Heading;
  
BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

char BLEcmd;

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
    char *spd;
    int i;
      if (rxValue.length() > 0) {
        Serial.print("*** BLE Received : ");
        for (int i = 0; i < rxValue.length(); i++)          
          Serial.print(rxValue[i]);
        Serial.println();
        BLEcmd = rxValue[0];
      }
    }
};

void setup() {
  Serial.begin(115200);
    
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
  
  Serial.println("Motor Pins assigned...");
  motor.attachMotors(IN1pin, IN2pin, IN3pin, IN4pin);  
}

void loop() {
  String txString;  
  int txStringLen;
  uint8_t *txValue;

  txString = "Y=" + String(Heading);
  txStringLen = txString.length();
  Serial.print(txString); Serial.print("\t"); Serial.println(txStringLen);
  txValue = (uint8_t*)txString.c_str();
  Heading++;

  // send TX data
  if (deviceConnected) {
    pTxCharacteristic->setValue(txValue, txStringLen);
    pTxCharacteristic->notify();
	  delay(10); // bluetooth stack will go into congestion, if too many packets are sent
	}
    
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
    
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
	  // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
    
  // check BLE command
  switch (BLEcmd) {
    case 'F': motor.motorForward(motorR, speed);  
              motor.motorForward(motorL, speed); 
              break;
    case 'B': motor.motorReverse(motorR, speed);
              motor.motorReverse(motorL, speed); 
              break;
    case 'R': motor.motorReverse(motorR, speed);
              motor.motorForward(motorL, speed); 
              break;
    case 'L': motor.motorForward(motorR, speed);
              motor.motorReverse(motorL, speed);
              break;
    case 'S': motor.motorStop(motorR);
              motor.motorStop(motorL); 
              break;
  }
  
  delay(100);
}
