#include <Wire.h>
#include <VL53L0X_IR_libraries/VL53L0X.h>
#include "I2Cdev.h"
#include <MPU6050_IMU_libraries/MPU6050_6Axis_MotionApps612.h>

VL53L0X sensor;

MPU6050 mpu;

#define OUTPUT_READABLE_YAWPITCHROLL

bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;      // set true if DMP init was successful
uint8_t mpuIntStatus;       // holds actual interrupt status byte from MPU
uint8_t devStatus;          // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;        // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;         // count of all bytes currently in FIFO
uint8_t fifoBuffer[128];    // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);    // 400kHz I2C clock. Comment this line if having compilation difficulties    

    sensor.setTimeout(500);
    if (!sensor.init()) {
        Serial.println("Failed to detect and initialize sensor!");
        while (1) {
        }
    }
    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
    // wait for ready
    delay(1000);
    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();
    //   pinMode(INTERRUPT_PIN, INPUT);

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(51);
    mpu.setYGyroOffset(8);
    mpu.setZGyroOffset(21);
    mpu.setXAccelOffset(1150);
    mpu.setYAccelOffset(-50);
    mpu.setZAccelOffset(1060);
    if (devStatus == 0) {
      // Calibration Time: generate offsets and calibrate our MPU6050
      mpu.CalibrateAccel(6);
      mpu.CalibrateGyro(6);
      Serial.println();
      mpu.PrintActiveOffsets();
      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      mpu.setDMPEnabled(true);

      // enable Arduino interrupt detection
      // Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
      // Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
      // Serial.println(F(")..."));
      // attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
      mpuIntStatus = mpu.getIntStatus();

      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;

      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
    }
    // Start continuous back-to-back mode (take readings as fast as possible).  
    // To use continuous timed mode instead, provide a desired inter-measurement period in ms 
    // (e.g. sensor.startContinuous(100)).
    sensor.startContinuous();

    // configure LED for output
    pinMode(LED_BUILTIN, OUTPUT);
    if (dmpReady) {
      blinkState = true;
    } else { 
      blinkState = false;
    }
    digitalWrite(LED_BUILTIN, blinkState);     
}

void loop()
{
  int distance = sensor.readRangeContinuousMillimeters();
  Serial.print(distance/10);
  Serial.println(" cm");
    
  if (sensor.timeoutOccurred()) { 
    Serial.println(" TIMEOUT"); 
  }

  // if programming failed, don't try to do anything
  if (!dmpReady) { 
    Serial.println("DMP not ready!!!");
  } else {
  // read a packet from FIFO
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {    // Get the Latest packet  
#ifdef OUTPUT_READABLE_YAWPITCHROLL
      // display Euler angles in degrees
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      Serial.print("ypr\t");
      Serial.print(ypr[0] * 180 / M_PI);
      Serial.print("\t");
      Serial.print(ypr[1] * 180 / M_PI);
      Serial.print("\t");
      Serial.print(ypr[2] * 180 / M_PI);
      Serial.println();
#endif
      // blink LED to indicate activity
      blinkState = !blinkState;
      digitalWrite(LED_BUILTIN, blinkState);
    }   
  }  
}
