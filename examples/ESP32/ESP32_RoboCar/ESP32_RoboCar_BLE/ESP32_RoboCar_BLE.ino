// PWM to DRV8833 dual H-bridge motor driver, PWM freq. = 1000 Hz
// BLE to receive/transmit data with smartphone app BLE2RC

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ESP32MotorControl.h> 

// DRV8833 pin connection
#define IN1pin 16  
#define IN2pin 17  
#define IN3pin 18 
#define IN4pin 19

#define motorR 0
#define motorL 1

#define FULLSPEED 100
#define HALFSPEED 50

ESP32MotorControl motor;

// Main Loop vars
char *BLE_RXbuf;
char BLEcmd;
bool BLE_RXflag = false;

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
        BLE_RXbuf = (char *)rxValue.c_str();
        BLEcmd = BLE_RXbuf[0];
        BLE_RXflag = true; // set BLE RX flag
      }
    }
};

void forward(int speed) {
  motor.motorForward(motorR, speed);  
  motor.motorForward(motorL, speed);
  Serial.println("Move Forward");
}

void backward(int speed) {
  motor.motorReverse(motorR, speed);
  motor.motorReverse(motorL, speed);
  Serial.println("Move Backward");     
}

void right(int speed) {
  motor.motorReverse(motorR, speed);  
  motor.motorForward(motorL, speed);
  Serial.println("Turn Right");    
}

void left(int speed) {
  motor.motorForward(motorR, speed);
  motor.motorReverse(motorL, speed);
  Serial.println("Turn Left"); 
}

void stop() {
  motor.motorStop(motorR);
  motor.motorStop(motorL);
  Serial.println("Motor Stop");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Motor Pins assigned...");
  
  // Motor control setup
  motor.attachMotors(IN1pin, IN2pin, IN3pin, IN4pin);
  motor.motorStop(motorR);
  motor.motorStop(motorL);
    
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
  String tx;
  int Speed = HALFSPEED;

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
  if (BLE_RXflag) {
    Serial.print(BLEcmd); Serial.print(" "); Serial.println(BLE_RXbuf);
    
    switch (BLEcmd) {
      case 'F': forward(Speed);
                break;
      case 'B': backward(Speed);
                break;
      case 'R': right(Speed);           
                break;
      case 'L': left(Speed);
                break;
      case 'S': stop();
                break;  
      case 'P': 
                  Serial.println(String(BLE_RXbuf)); 
                  Speed = String(BLE_RXbuf+1).toInt();
                  Serial.println(Speed);
                  break;                
    }
    BLE_RXflag = false;
  }

}
