#include <Wire.h>
#include "IMU.h"

#define S_SCL   33
#define S_SDA   32

#define CALIBRATION

IMU imu;

void setup() { 
  Wire2.begin(S_SDA, S_SCL, 400000);
  Serial.begin(115200);
  delay(1000);

  #ifdef CALIBRATION
    imu.init(1);
    imu.autoCalibrate();
  #else
    imu.init();
  #endif  
  Serial.println("/-------------------------------------------------------------/");
}

#define PERIOD  100
uint32_t timeStamp;

float roll, pitch, yaw;

uint32_t lastTime = 0;

void loop() {
  if (millis() - timeStamp >= PERIOD) {
    imu.eulerAngles(roll, pitch, yaw);
    timeStamp = millis();
#ifndef CALIBRATION
    Serial.printf("Roll: %6.2f Pitch: %6.2f Yaw: %6.2f Mag: %6.2f\n\r", roll, pitch, yaw, imu.mg);
#else
//  Data for magnetometer calibration by MotionCal
//  "Raw:0,0,0,0,0,0,mx,my,mz\n\r" mx, my, mz 0.1uT
    uint8_t x = imu.readData();
    if (x) {
      Serial.printf("Raw:0,0,0,0,0,0,%d,%d,%d\n\r", (int)(imu.m[0]*10), (int)(imu.m[1]*10), (int)(imu.m[2]*10));
    }
#endif
  }
}
