# RobotCat
![](https://github.com/rkuo2000/Robotics/blob/gh-pages/images/RobotCat.jpg?raw=true)

## Hardware
1. ESP32 (NodeMCU32-S)
2. MPU6050 (3-axis accelerometer & 3-axis gyroscope)
3. SR04 (Ultrasound Ranger)
4. PCA9685 (I2C-PWM 16 channels)
5. SG90 x12 (DCservo)
6. 7805CT (5V voltage regulator)
7. 18650 x2 (LiPo battery, 3.7V*2=7.4V)

## Software
* ESP32_MPU6050_DMP6   : get Yaw, Pitch, Roll
* ESP32_PCA9685_SG90x1 : sweep test a SG90 on connector 15
* ESP32_PCA9685_SG90x4 : sweep test four SG90 on connector 3,7,11,15
* ESP32_PCA9685_Async_OTA_WebServer : Async OTA WebServer to set pos of SG90 x12
![](https://github.com/rkuo2000/Robotics/blob/gh-pages/images/ESP32_PCA9685_Async_OTA_WebServer.png?raw=true)
