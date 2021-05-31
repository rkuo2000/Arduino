  
// set Software Serial pins to output Teapot packet
#include <HardwareSerial.h>
HardwareSerial hwSer(1);  // UART1 (TX1, RX1)

#include <Wire.h>
#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

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

//#define OUTPUT_CSV_READABLE_RAWDATA
//#define OUTPUT_READABLE_QUATERNION
//#define OUTPUT_READABLE_EULER
//#define OUTPUT_READABLE_YAWPITCHROLL
//#define OUTPUT_READABLE_REALACCEL
//#define OUTPUT_READABLE_WORLDACCEL
#define OUTPUT_TEAPOT_PACKET

// MPU control/status vars
unsigned long lastTime = 0;
unsigned long accDelay = 100; //ms
uint8_t sampleRate = 1000/accDelay; // >10 need to use hwSer.write, <=10 use Serial.write for Teapot output
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 gyro;       // [x, y, z]            gyro sensor measurements
VectorInt16 acc;        // [x, y, z]            accel sensor measurements
VectorInt16 accReal;    // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 accWorld;   // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$',0x02, 0,0,0,0,0,0,0,0,0x00,0x00,'\r','\n' };

/*
// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
void ICACHE_RAM_ATTR dmpDataReady ();

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}
*/
//===================================================================================================================
//====== Set of useful function to access acceleratio, gyroscope, and temperature data
//===================================================================================================================

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
// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {  
    Serial.begin(115200); 
    hwSer.begin(115200);

    Wire.begin();

    // initialize device
    Serial.println("Initializing I2C devices...");
    mpu.initialize();

    // load and configure the DMP
    Serial.println("Initializing DMP...");
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println("Enabling DMP...");
        mpu.setDMPEnabled(true);

        /* Not using Interrupt pin
        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        */

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        //Serial.println("DMP ready! Waiting for first interrupt...");
        Serial.println("DMP ready!");
        dmpReady = true;
        
        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print("DMP Initialization failed (code ");
        Serial.print(devStatus);
        Serial.println(")");
    }
    Serial.print("Sample Rate: ");
    Serial.println(sampleRate);
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  // if DMP initialization failed, don't try to do anything
  if (!dmpReady) return; 
  if ((millis() - lastTime)> accDelay){
    //Serial.println(millis()-lastTime);
    lastTime = millis();    
  
	/* ------------Not using Interrupt-----------------------
    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        // other program behavior stuff here
        // .
        // .
        // .
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
        // .
        // .
        // .
    }
  	
    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
     */ 
     
    mpuIntStatus = mpu.getIntStatus();
    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println("FIFO overflow!");

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;
        
        #ifdef OUTPUT_CSV_READABLE_RAWDATA 
        // display rawdata in csv format
        mpu.dmpGetAccel(&acc, fifoBuffer);
        getAres();        
        ax = (float)acc.x * aRes;  // get actual g value, this depends on scale being set
        ay = (float)acc.y * aRes;   
        az = (float)acc.z * aRes;
        mpu.dmpGetGyro(&gyro, fifoBuffer);
        getGres();
        // Calculate the gyro value into actual degrees per second
        gx = (float)gyro.x * gRes;  // get actual gyro value, this depends on scale being set
        gy = (float)gyro.y * gRes;  
        gz = (float)gyro.z * gRes;
        // MPU6040 has no magnetometer
        mx = 0;
        my = 0;
        mz = 0;
        mpu.dmpGetQuaternion(&q, fifoBuffer);                
        Serial.print(millis());

        Serial.print(","); Serial.print(ax,2);
        Serial.print(","); Serial.print(ay,2); 
        Serial.print(","); Serial.print(az,2);      
        Serial.print(","); Serial.print(gx,2); 
        Serial.print(","); Serial.print(gy,2); 
        Serial.print(","); Serial.print(gz,2);
        Serial.print(","); Serial.print(mx,2); 
        Serial.print(","); Serial.print(my,2); 
        Serial.print(","); Serial.print(mz,2);
        Serial.print(","); Serial.print(q.w,6);
        Serial.print(","); Serial.print(q.x,6);
        Serial.print(","); Serial.print(q.y,6); 
        Serial.print(","); Serial.print(q.z,6);
        Serial.println();
        #endif
    
        #ifdef OUTPUT_READABLE_QUATERNION
        // display quaternion values in easy matrix form: w x y z
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        Serial.print("quat\t");
        Serial.print(q.w);
        Serial.print("\t");
        Serial.print(q.x);
        Serial.print("\t");
        Serial.print(q.y);
        Serial.print("\t");
        Serial.println(q.z);
        #endif

        #ifdef OUTPUT_READABLE_EULER
        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetEuler(euler, &q);
        Serial.print("euler\t");
        Serial.print(euler[0] * 180/M_PI);
        Serial.print("\t");
        Serial.print(euler[1] * 180/M_PI);
        Serial.print("\t");
        Serial.println(euler[2] * 180/M_PI);
        #endif

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        Serial.print("ypr\t");
        Serial.print(ypr[0] * 180/M_PI);
        Serial.print("\t");
        Serial.print(ypr[1] * 180/M_PI);
        Serial.print("\t");
        Serial.println(ypr[2] * 180/M_PI);
        #endif
               
        #ifdef OUTPUT_READABLE_REALACCEL
        // display real acceleration, adjusted to remove gravity
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetAccel(&acc, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccel(&accReal, &acc, &gravity);
        Serial.print("accReal\t");
        Serial.print(accReal.x);
        Serial.print("\t");
        Serial.print(accReal.y);
        Serial.print("\t");
        Serial.println(accReal.z);
        #endif

        #ifdef OUTPUT_READABLE_WORLDACCEL
        // display initial world-frame acceleration, adjusted to remove gravity
        // and rotated based on known orientation from quaternion
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetAccel(&acc, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetLinearAccel(&accReal, &acc, &gravity);
        mpu.dmpGetLinearAccelInWorld(&accWorld, &accReal, &q);
        Serial.print("accWorld\t");
        Serial.print(accWorld.x);
        Serial.print("\t");
        Serial.print(accWorld.y);
        Serial.print("\t");
        Serial.println(accWorld.z);
        #endif
    
        #ifdef OUTPUT_TEAPOT_PACKET       
        // display quaternion values in InvenSense Teapot demo format:
        teapotPacket[2] = fifoBuffer[0];
        teapotPacket[3] = fifoBuffer[1];
        teapotPacket[4] = fifoBuffer[4];
        teapotPacket[5] = fifoBuffer[5]; 
        teapotPacket[6] = fifoBuffer[8];
        teapotPacket[7] = fifoBuffer[9];
        teapotPacket[8] = fifoBuffer[12];
        teapotPacket[9] = fifoBuffer[13];
        if (sampleRate >10) hwSer.write(teapotPacket, 14);
        else                Serial.write(teapotPacket, 14);       
        teapotPacket[11]++; // packetCount, loops at 0xFF on purpose        
        #endif
    }
  }
}
