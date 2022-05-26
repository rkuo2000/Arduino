//
// using MPU6050 to record acc for motion recognition
//
#include "I2Cdev.h" 
#include "Wire.h"
#include <MPU6050.h>

MPU6050 imu;
float ax, ay, az;
int16_t accX, accY, accZ;

float sumX=0;
float sumY=0;
float sumZ=0;
float offsetX=0;
float offsetY=0;
float offsetZ=0;

#define sampleTime 20 // 20ms = 50Hz
#define periodTime 1500 // 1500ms
long loopTime, lastTime;
int samples = periodTime / sampleTime; // 1500/20 = 75 samples
int counts;
bool sampleEn = false;

enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

int Ascale = AFS_2G;
float aRes; // scale resolutions per LSB for the sensors

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

void getAcc() {
    imu.getAcceleration(&accX, &accY, &accZ);
    getAres();        
    ax = (float)accX * aRes -offsetX;  // get actual g value, this depends on scale being set
    ay = (float)accY * aRes -offsetY;   
    az = (float)accZ * aRes -offsetZ;   
}

void setup() {
  Serial.begin(115200);
  Wire.begin(); // default I2C clock is 100KHz
  pinMode(LED_BUILTIN, OUTPUT);
    
  // initialize IMU
  Serial.println("Initializing IMU...");
  imu.initialize();
  
  // verify connection
  Serial.println(imu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));  

  // calibration
  for (int i=0; i<100; i++){
    getAcc();
    sumX += ax;
    sumY += ay;
    sumZ += az;
  }
  offsetX = sumX/100;
  offsetY = sumY/100;
  offsetZ = sumZ/100;
  Serial.print("Offset: ");
  Serial.print(offsetX); Serial.print("\t");
  Serial.print(offsetY); Serial.print("\t");
  Serial.print(offsetZ); Serial.println();  

  // pick up IMU to start sampling  
  while(!sampleEn) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);   
    getAcc();
    if (abs(az)>0.1) sampleEn=true;
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);      
  }
  
  lastTime = millis();
}

void loop() {
    loopTime = millis() - lastTime;
    if (loopTime >= sampleTime) {
      getAcc();
      lastTime = millis();  
      Serial.print(ax); Serial.print(","); 
      Serial.print(ay); Serial.print(",");
      Serial.print(az); Serial.println();
      counts++;
    }
    if (counts>=samples) {
      counts=0;
      Serial.println(); Serial.println(); Serial.println();
    }
}
 
