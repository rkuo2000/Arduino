
// set Hardware Serial pins to output Teapot packet
#include <HardwareSerial.h>
HardwareSerial hwSer(1);

#include <SparkFunMPU9250-DMP.h>
#include <Wire.h>
MPU9250_DMP imu;

//#define OUTPUT_CSV_READABLE_RAWDATA
//#define OUTPUT_READABLE_RAWDATA
//#define OUTPUT_READABLE_QUATERNION
//#define OUTPUT_READABLE_YAWPITCHROLL
#define OUTPUT_TEAPOT_PACKET

uint8_t samplerate = 10; // 1~200

// orientation/motion vars
float q[4];
float acc[3];    // [accX, accY, accZ]    Accelerator
float gyro[3];   // [gyroX, gyroY, gyroZ] Gyroscope
float mag[3];    // [magX, magY, magZ]    Magnetometer 
float euler[3];  // [psi, theta, phi]     Euler angle container
float ypr[3];    // [yaw, pitch, roll]    yaw/pitch/roll container and gravity vector

const signed char orientationMatrix[9] = {
  1, 0, 0,
  0, 1, 0,
  0, 0, 1
};
unsigned char lastOrient = 0;

// packet structure for InvenSense teapot demo (MPU9250 DMP quaternion is 32bit)
uint8_t teapotPacket[22] = { '$', 0x02, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0x00, 0x00, '\r', '\n' };

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() 
{
  Serial.begin(115200);
  hwSer.begin(115200);  // soft Serial for sample rate >10
  Wire.begin();

  // Call imu.begin() to verify communication and initialize
  while (imu.begin() != INV_SUCCESS)
  {
    Serial.println("Unable to communicate with MPU-9250");
    Serial.println("Check connections, and try again.");
    Serial.println();
    delay(1000);
  }

  // Enable 6-axis quat
  // Use gyro calibration
  // Set DMP FIFO rate to 100 Hz (Max sample rate = 200Hz)
  // SampleRate = 10 use Serial.print, = 100 need to use Soft Serial  
  imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_GYRO_CAL, samplerate);
               
  // DMP_FEATURE_LP_QUAT can also be used. It uses the accelerometer in low-power mode to estimate quat's.
  // DMP_FEATURE_LP_QUAT and 6X_LP_QUAT are mutually exclusive 
  
  // check MPU9250 setting
  Serial.println();
  Serial.print("Accel FSR = ");  Serial.println(imu.getAccelFSR()); // 2G
  Serial.print("Gyro  FSR = ");  Serial.println(imu.getGyroFSR());  // 2000 deg/s
  Serial.print("Mag   FSR = ");  Serial.println(imu.getMagFSR());   // 4915   
  Serial.print("SampleRate= ");  Serial.println(imu.getSampleRate()); // 50
  Serial.print("Compass SampleRate= ");  Serial.println(imu.getCompassSampleRate()); // 10
  Serial.println();     
}

void loop() 
{
  // Check for new data in the FIFO
  if ( imu.fifoAvailable() )
  {
    // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
    if ( imu.dmpUpdateFifo() == INV_SUCCESS)
    { 
      #ifdef OUTPUT_CSV_READABLE_RAWDATA
      q[0] = imu.calcQuat(imu.qw);
      q[1] = imu.calcQuat(imu.qx);
      q[2] = imu.calcQuat(imu.qy);
      q[3] = imu.calcQuat(imu.qz);
      imu.updateAccel();
      acc[0] = imu.calcAccel(imu.ax);
      acc[1] = imu.calcAccel(imu.ay);
      acc[2] = imu.calcAccel(imu.az);
      imu.updateGyro();
      gyro[0]= imu.calcGyro(imu.gx);
      gyro[1]= imu.calcGyro(imu.gy);
      gyro[2]= imu.calcGyro(imu.gz);
      imu.updateCompass();
      mag[0] = imu.calcMag(imu.mx);
      mag[1] = imu.calcMag(imu.my);
      mag[2] = imu.calcMag(imu.mz);
      Serial.print(millis());
      Serial.print(","); Serial.print(acc[0]);
      Serial.print(","); Serial.print(acc[1]);
      Serial.print(","); Serial.print(acc[2]);
      Serial.print(","); Serial.print(gyro[0]);
      Serial.print(","); Serial.print(gyro[1]);
      Serial.print(","); Serial.print(gyro[2]);
      Serial.print(","); Serial.print(mag[0]);
      Serial.print(","); Serial.print(mag[1]);
      Serial.print(","); Serial.print(mag[2]);
      Serial.print(","); Serial.print(q[0],6);
      Serial.print(","); Serial.print(q[1],6);
      Serial.print(","); Serial.print(q[2],6); 
      Serial.print(","); Serial.print(q[3],6); 
      Serial.println();      
      #endif
          
      #ifdef OUTPUT_READABLE_RAWDATA
      imu.updateAccel();
      acc[0] = imu.calcAccel(imu.ax);
      acc[1] = imu.calcAccel(imu.ay);
      acc[2] = imu.calcAccel(imu.az);
      imu.updateGyro();
      gyro[0]= imu.calcGyro(imu.gx);
      gyro[1]= imu.calcGyro(imu.gy);
      gyro[2]= imu.calcGyro(imu.gz);
      imu.updateCompass();
      mag[0] = imu.calcMag(imu.mx);
      mag[1] = imu.calcMag(imu.my);
      mag[2] = imu.calcMag(imu.mz);
            
      Serial.print("acc\t");
      Serial.print(acc[0]);
      Serial.print("\t");
      Serial.print(acc[1]);
      Serial.print("\t");
      Serial.print(acc[2]);
      Serial.print("\t");
      Serial.print("gyro\t");
      Serial.print(gyro[0]);
      Serial.print("\t");
      Serial.print(gyro[1]);
      Serial.print("\t");
      Serial.print(gyro[2]);
      Serial.print("\t");      
      Serial.print("mag\t");
      Serial.print(mag[0]);
      Serial.print("\t");
      Serial.print(mag[1]);
      Serial.print("\t");
      Serial.println(mag[2]);
      #endif
                            
      #ifdef OUTPUT_READABLE_QUATERNION
      // display quaternion values in easy matrix form: w x y z
      q[0] = imu.calcQuat(imu.qw);
      q[1] = imu.calcQuat(imu.qx);
      q[2] = imu.calcQuat(imu.qy);
      q[3] = imu.calcQuat(imu.qz);
      Serial.print("quat\t");
      Serial.print(q[0],6);
      Serial.print("\t");
      Serial.print(q[1],6);
      Serial.print("\t");
      Serial.print(q[2],6);
      Serial.print("\t");
      Serial.println(q[3],6);
      #endif

      #ifdef OUTPUT_READABLE_YAWPITCHROLL
      imu.computeEulerAngles(true); //compute Euler angles from Quaternions
      ypr[0] = imu.yaw;   // psi
      ypr[1] = imu.pitch; // theta
      ypr[2] = imu.roll;  // phi
      Serial.print("Yaw-Pitch-Roll\t");
      Serial.print(ypr[0]);
      Serial.print("\t");
      Serial.print(ypr[1]);
      Serial.print("\t");
      Serial.println(ypr[2]);
      #endif
              
      #ifdef OUTPUT_TEAPOT_PACKET
      teapotPacket[2] = (imu.qw >>24) & 0x000000ff;
      teapotPacket[3] = (imu.qw >>16) & 0x000000ff;
      teapotPacket[4] = (imu.qw >>8 ) & 0x000000ff;
      teapotPacket[5] =  imu.qw       & 0x000000ff;
      teapotPacket[6] = (imu.qx >>24) & 0x000000ff;
      teapotPacket[7] = (imu.qx >>16) & 0x000000ff;
      teapotPacket[8] = (imu.qx >>8 ) & 0x000000ff;
      teapotPacket[9] =  imu.qx       & 0x000000ff;
      teapotPacket[10]= (imu.qy >>24) & 0x000000ff;
      teapotPacket[11]= (imu.qy >>16) & 0x000000ff;
      teapotPacket[12]= (imu.qy >>8 ) & 0x000000ff;
      teapotPacket[13]=  imu.qy       & 0x000000ff;
      teapotPacket[14]= (imu.qz >>24) & 0x000000ff;
      teapotPacket[15]= (imu.qz >>16) & 0x000000ff;
      teapotPacket[16]= (imu.qz >>8 ) & 0x000000ff;
      teapotPacket[17]=  imu.qz       & 0x000000ff;  
      if (samplerate >10) hwSer.write(teapotPacket, 22);
      else                Serial.write(teapotPacket, 22);      
      teapotPacket[19]++; // packetCount, loops at 0xFF on purpose
      #endif  
    }
  }
}
