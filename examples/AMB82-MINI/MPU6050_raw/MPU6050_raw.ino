// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

#define OUTPUT_READABLE_ACCGYRO
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 imu;
//MPU6050 imu(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;

#define LED_PIN LED_BUILTIN
bool blinkState = false;

void setup() {
    Wire.begin();
    Serial.begin(115200);

    // initialize device
    Serial.println("Initializing I2C devices...");
    imu.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(imu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // use the code below to change accel/gyro offset values
    /*
    Serial.println("Updating internal sensor offsets...");
    // -76	-2359	1688	0	0	0
    Serial.print(imu.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(imu.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(imu.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(imu.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(imu.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(imu.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    imu.setXGyroOffset(220);
    imu.setYGyroOffset(76);
    imu.setZGyroOffset(-85);
    Serial.print(imu.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(imu.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(imu.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(imu.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(imu.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(imu.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    */

    // configure Arduino LED pin for output
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // read raw accel/gyro measurements from device
    imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //imu.getAcceleration(&ax, &ay, &az);
    //imu.getRotation(&gx, &gy, &gz);

    #ifdef OUTPUT_READABLE_ACCGYRO
        Serial.print(ax); Serial.print(",");
        Serial.print(ay); Serial.print(",");
        Serial.print(az); Serial.print(",");
        Serial.print(gx); Serial.print(",");
        Serial.print(gy); Serial.print(",");
        Serial.println(gz);
    #endif

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}
