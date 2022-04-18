// ESP32 BLE remote control LED5050 strip
#include <string.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define pinBLUE  27
#define pinRED   14
#define pinGREEN 12

// PWM channel no.  
#define chRED   0
#define chGREEN 1
#define chBLUE  2

#define PWMFREQ 1000
#define PWMRESOLUTION 8 // 8-bit resolution
#define PWMMAXPOWER 255

#define ledTime 20
#define colorTime 2000

// BLE defs & vars
// See the following for generating UUIDs: https://www.uuidgenerator.net/
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Main loop vars
char *BLE_RXbuf;
bool BLE_RXflag = false;
int r,g,b;

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
        BLE_RXflag = true; // set BLE RX flag
      }
    }
};

void setup() {
  Serial.begin(115200);
  
  // setup PWM pin and its channel
  pinMode(pinRED,   OUTPUT);
  pinMode(pinGREEN, OUTPUT);
  pinMode(pinBLUE,  OUTPUT);
  ledcSetup(chRED,  PWMFREQ, PWMRESOLUTION);
  ledcSetup(chGREEN,PWMFREQ, PWMRESOLUTION); 
  ledcSetup(chBLUE, PWMFREQ, PWMRESOLUTION); 
  ledcAttachPin(pinRED,   chRED); 
  ledcAttachPin(pinGREEN, chGREEN); 
  ledcAttachPin(pinBLUE,  chBLUE);
  
  turnOff(); // output Color=0 0 0 to LED5050
  
  // Create the BLE Device
  BLEDevice::init("ESP32-LED5050");

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
  if (BLE_RXflag) {
    // display BLE received string
    Serial.println(BLE_RXbuf);

    // split string
    splitCommand(BLE_RXbuf, " "); 
    Serial.print(r); Serial.print("\t"); Serial.print(g); Serial.print("\t"); Serial.println(b);

    // set color to LED5050
    setColor(r,g,b);
    BLE_RXflag = false; // clear BLE RX flag
  }
  delay(colorTime);  // add delay before setting another color
    
}

void turnOff() {
  ledcWrite(chRED, 0);
  ledcWrite(chGREEN,0);
  ledcWrite(chBLUE, 0);
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(chRED,   r);
  ledcWrite(chGREEN, g);
  ledcWrite(chBLUE,  b);
  delay(ledTime);
}

// command = "r=255 g=255 b=255"
void splitCommand(char * command, char * delimiter) {
  char * ptr = command;
  char * eq = NULL; //locate assmts
  int * num = NULL; //just for starters
  while (1){
    eq = strchr(ptr, '=');
    ptr = eq; // update the pointer
    if (ptr == NULL) // found no = chars
      break;
    switch (*(ptr - 1)){ 
      case 'r':    //all the possible variables
        num = &r; break;
      case 'g': 
        num = &g; break;
      case 'b': 
        num = &b; break;
      default:   //unknown variable
        num = NULL;
    }
    ptr++;
    if (num == NULL) //unrecognized var
      continue;   // locate next = char
    *num = 0;
    while (*ptr && (*ptr != ' ')){  // while space or end of string not yet reached
      *num *= 10;  // extract each int
      *num += *ptr - '0';
      ptr++;
    }
  }
}
