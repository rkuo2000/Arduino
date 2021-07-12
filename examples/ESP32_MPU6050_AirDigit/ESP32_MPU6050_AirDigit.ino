#include <EloquentTinyML.h>
#include "tinyml_airdigit.h"
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>

#define NUMBER_OF_INPUTS  90 // 30*3
#define NUMBER_OF_OUTPUTS 10
#define TENSOR_ARENA_SIZE 16*1024 // preallocate a certain amount of memory for input, output, and intermediate arrays.

Eloquent::TinyML::TfLite<NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, TENSOR_ARENA_SIZE> ml;

MPU6050 imu;

// Set initial input parameters
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

int Ascale = AFS_2G;
float aRes; // scale resolutions per LSB for the sensors

// IMU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t imuIntStatus;   // holds actual interrupt status byte from IMU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

VectorInt16 acc;          // [x, y, z]            accel sensor measurements
VectorInt16 lastacc;
float accM, accMavg;      // sqrt(x^2 + y^2 +z^2) accel magnitude
float accReal[3];
float accBuf[90];

// Main Loop vars
unsigned long lastTime = 0;
unsigned long loopTime = 50;     //ms = 20Hz
unsigned long sampleTime = 1500; //ms = 1.5sec data recording
uint8_t i =0;

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

void readIMU() 
{ 
  imuIntStatus = imu.getIntStatus();

  // get current FIFO count
  fifoCount = imu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((imuIntStatus & 0x10) || fifoCount == 1024) {
    // Serial.println("IMU FIFO overflow!");    
    imu.resetFIFO(); // reset so we can continue cleanly
    //YPR = lastYPR;    
    acc = lastacc;

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
    imu.dmpGetAccel(&acc, fifoBuffer); 
    accM = sqrt(pow(acc.x, 2)+pow(acc.y, 2)+pow(acc.z, 2));
    getAres();
    accReal[0] = (float)acc.x * aRes;
    accReal[1] = (float)acc.y * aRes;   
    accReal[2] = (float)acc.z * aRes;
        
  }
}

void setup() {  
  Serial.begin(115200);
  Wire.begin(); // default I2C clock is 100KHz    
  ml.begin(tinyml_airdigit);
  
  // initialize IMU
  Serial.println("Initializing IMU...");
  imu.initialize();
    
  Serial.println(F("Testing device connections...")); // verify connection
  Serial.println(imu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
   
  Serial.println("Initializing DMP..."); // load and configure the DMP
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
    packetSize = imu.dmpGetFIFOPacketSize(); // get expected DMP packet size for later comparison
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
  int j =0;
  float y_pred[10] ={0};
  uint32_t inferTime, lastInferTime;
  String tx;

  accMavg = 0;
  // calculate average acc magnitude
  Serial.println("Hold Steady !!!");
  for (i=0; i<10; i++) { 
    readIMU();
    accMavg+= accM; 
    while(millis()-lastTime <loopTime) lastTime = millis(); 
  }

  accMavg = accMavg/10;
  Serial.print("Average Accelero Magnitude is "); Serial.println(accMavg);
  // check if accM has changed
  Serial.println("keep reading MPU6050 till moving...");  
  while ( abs(accM - accMavg) <500 ){
    readIMU(); 
    while(millis()-lastTime <loopTime) lastTime = millis();
  }
  
  Serial.println("Start Recording Accelero...");
  while(j<30) { // circular buffer to record acc
    if (millis()-lastTime >=loopTime) {
      readIMU();
      lastacc = acc;        
      accBuf[j*3]   = accReal[0];
      accBuf[j*3+1] = accReal[1]; 
      accBuf[j*3+2] = accReal[2]; 
      j++;   
      lastTime = millis();
      Serial.print(" ...");
    }
  }
  Serial.println();
  
  lastInferTime = micros();    
  ml.predict(accBuf, y_pred);
  inferTime = micros() - lastInferTime;

  Serial.print("inference time: "); Serial.print(inferTime); Serial.println(" us");

  for (int j=0; j<10; j++) {
    Serial.print(y_pred[j]);
    Serial.print(j==9 ? '\n' : ',');
  }
  Serial.println();    
  Serial.print("Prediction :");
  Serial.println(ml.probaToClass(y_pred));
  
  Serial.println();
  Serial.println("Move to the starting point !");
  Serial.println();  
  delay(2000); 
}
