/************************************************************
MPU9250_DMP_Quaternion
 Quaternion example for MPU-9250 DMP Arduino Library 
Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library

The MPU-9250's digital motion processor (DMP) can calculate
four unit quaternions, which can be used to represent the
rotation of an object.

This exmaple demonstrates how to configure the DMP to 
calculate quaternions, and prints them out to the serial
monitor. It also calculates pitch, roll, and yaw from those
values.

Development environment specifics:
Arduino IDE 1.6.12
SparkFun 9DoF Razor IMU M0

Supported Platforms:
- ATSAMD21 (Arduino Zero, SparkFun SAMD21 Breakouts)
*************************************************************/
#include <SparkFunMPU9250-DMP.h>

MPU9250_DMP imu;

// packet structure for InvenSense teapot demo (MPU9250 DMP quaternion is 32bit)
uint8_t teapotPacket[22] = { '$', 0x02, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0x00, 0x00, '\r', '\n' };
// #define OUTPUT_TEAPOT

float q[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float pitch, yaw, roll;
  
void setup() 
{
  Serial.begin(115200);

  // Call imu.begin() to verify communication and initialize
  if (imu.begin() != INV_SUCCESS)
  {
    while (1)
    {
      Serial.println("Unable to communicate with MPU-9250");
      Serial.println("Check connections, and try again.");
      Serial.println();
      delay(5000);
    }
  }
  
  imu.dmpBegin(DMP_FEATURE_6X_LP_QUAT | // Enable 6-axis quat
               DMP_FEATURE_GYRO_CAL, // Use gyro calibration
              10); // Set DMP FIFO rate to 10 Hz
  // DMP_FEATURE_LP_QUAT can also be used. It uses the 
  // accelerometer in low-power mode to estimate quat's.
  // DMP_FEATURE_LP_QUAT and 6X_LP_QUAT are mutually exclusive
}

void loop() 
{
  // Check for new data in the FIFO
  if ( imu.fifoAvailable() )
  {
    // Use dmpUpdateFifo to update the ax, gx, mx, etc. values
    if ( imu.dmpUpdateFifo() == INV_SUCCESS)
    {
      // computeEulerAngles can be used -- after updating the
      // quaternion values -- to estimate roll, pitch, and yaw
      imu.computeEulerAngles();
      printIMUData();
    }
  }
}

void printIMUData(void)
{  
#ifdef OUTPUT_TEAPOT
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
    Serial.write(teapotPacket, 22);
    teapotPacket[19]++; // packetCount, loops at 0xFF on purpose
#else	
  // After calling dmpUpdateFifo() the ax, gx, mx, etc. values
  // are all updated.
  // Quaternion values are, by default, stored in Q30 long
  // format. calcQuat turns them into a float between -1 and 1
    q[0] = imu.calcQuat(imu.qw);
    q[1] = imu.calcQuat(imu.qx);
    q[2] = imu.calcQuat(imu.qy);
    q[3] = imu.calcQuat(imu.qz);
    yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);   
    pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
    roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
    pitch *= 180.0f / PI;
    yaw   *= 180.0f / PI; 
    yaw   -= 13.8; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
    roll  *= 180.0f / PI;
     
    Serial.print("Yaw, Pitch, Roll: ");
    Serial.print(yaw, 3);
    Serial.print(", ");
    Serial.print(pitch, 3);
    Serial.print(", ");
    Serial.println(roll, 3);
    
//  Serial.print(imu.qw);
//  Serial.print(imu.qx);
//  Serial.print(imu.qy);
//  Serial.println(imu.qz);
//  Serial.print(imu.time);
//  Serial.println("ms");
#endif
}

