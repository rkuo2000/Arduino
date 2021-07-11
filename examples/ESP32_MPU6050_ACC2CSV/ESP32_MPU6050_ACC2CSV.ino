//
// ESP32 using MPU6050 to record acc for motion recognition
//
// I2C to MPU6050 (3-axis accelerometer & 3-axis gyroscope)

#include <WebSerial.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>

MPU6050 imu;

// Set initial input parameters
enum Ascale {
  AFS_2G = 0,
  AFS_4G,
  AFS_8G,
  AFS_16G
};

enum Gscale {
  GFS_250DPS = 0,
  GFS_500DPS,
  GFS_1000DPS,
  GFS_2000DPS
};

// Specify sensor full scale
int Gscale = GFS_250DPS;
int Ascale = AFS_2G;
float aRes, gRes; // scale resolutions per LSB for the sensors

// IMU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t imuIntStatus;   // holds actual interrupt status byte from IMU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars

//Quaternion q;           // [w, x, y, z]         quaternion container
//VectorFloat gravity;    // [x, y, z]            gravity vector
//float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
//VectorInt16 YPR, lastYPR;

VectorInt16 acc;          // [x, y, z]            accel sensor measurements
VectorInt16 lastacc;
float accM, accMavg;      // sqrt(x^2 + y^2 +z^2) accel magnitude
VectorFloat accReal;
VectorFloat accBuf[30];   // circular buffer of YPR

// Main Loop vars
unsigned long lastTime = 0;
unsigned long loopTime = 50;     //ms = 20Hz
unsigned long sampleTime = 1500; //ms = 1.5sec data recording
uint8_t i =0;

void getGres() {
  switch (Gscale)
  {
   // Possible gyro scales (and their register bit settings) are:
  // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
    case GFS_250DPS:
          gRes = 250.0/32768.0;
          break;
    case GFS_500DPS:
          gRes = 500.0/32768.0;
          break;
    case GFS_1000DPS:
          gRes = 1000.0/32768.0;
          break;
    case GFS_2000DPS:
          gRes = 2000.0/32768.0;
          break;
  }
}

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
    accReal.x = (float)acc.x * aRes;
    accReal.y = (float)acc.y * aRes;   
    accReal.z = (float)acc.z * aRes;
            
    //imu.dmpGetQuaternion(&q, fifoBuffer);
    //imu.dmpGetGravity(&gravity, &q);
    //imu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    //YPR.x = int((ypr[0] * 180/M_PI)) + 180;
    //YPR.y = int((ypr[1] * 180/M_PI)) + 180;
    //YPR.z = int((ypr[2] * 180/M_PI)) + 180;     
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(); // default I2C clock is 100KHz
    
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


  // keep reading MPU6050 till it is stable
  Serial.print("keep reading MPU6050 till it is stable...");   
  for (i=0; i<30; i++) { 
    readIMU(); 
    Serial.print(accReal.x); Serial.print(","); Serial.print(accReal.y); Serial.print(","); Serial.print(accReal.z); Serial.print(","); Serial.println(i);
    while( millis()-lastTime <loopTime) lastTime = millis();
  }
  Serial.println("MPU6050 is stable!");

  // calculate average acc magnitude
  Serial.println("Hold Steady, calculating average acc magnitude for not-moving");
  for (i=0; i<10; i++) { 
    readIMU(); 
    Serial.println(accM);
    accMavg+= accM; 
    while(millis()-lastTime <loopTime) lastTime = millis(); 
  }

  accMavg = accMavg/10;
  Serial.print("Average Accelero Magnitude is "); Serial.println(accMavg);

  // check if accM has changed
  Serial.println("Keep reading MPU6050 till it is moving");  
  while ( abs(accM - accMavg) <300 ){
    readIMU(); 
    while(millis()-lastTime <loopTime) lastTime = millis();
  }  
  Serial.println("Start Recording Accelero for 1500ms in every 20ms");
  i =0;
}

void loop() {
  String tx;
  // Loop Time = ~15ms
  //Serial.print("loopTime = "); Serial.println(millis()-lastTime);
  //lastTime = millis();
  while(i<30) { // circular buffer to record acc
    if (millis()-lastTime >=loopTime) {
      readIMU();
      lastacc = acc;        
      accBuf[i] = accReal;      
      i++;
      
      tx = String(accReal.x)+","+String(accReal.y)+","+String(accReal.z);
      Serial.println(tx); 
      lastTime = millis();
    }
  }
}
 
