#ifndef IMU_H
#define IMU_H

#include "config.h"
#include "IMU_9DOF.h"
#include <Arduino.h>
#include "i2c.h"
#include "quaternionFilters.h"

#define CALIBRATION

class IMU {
  public:
    void init();
    void update();  // cteni a vypocet imu pro task
    static void imuTask(void* pvParameters);
    void eulerAngles(float &roll, float &pitch, float &yaw);

    uint8_t getId();
    void qmi8658_on_demand_cali();
    void autoCalibrate();

    void readAccRaw();
    void readGyroRaw();
    void readMagRaw();
    void readAcc();
    void readGyro();
    void readMag();
    uint8_t readData();

    void configAcc(enum qmi8658_AccRange range, enum qmi8658_AccOdr odr);
    void configGyro(enum qmi8658_GyrRange range, enum qmi8658_GyrOdr odr);
    void configMag(enum AK09918_mode_type_t odr);
    void configSensors();
    void enableAccGyro(unsigned char enableFlags);

    int16_t accRaw[3];    // Stores the 16-bit signed accelerometer sensor output 
    int16_t gyroRaw[3];   // Stores the 16-bit signed gyro sensor output
    int16_t magRaw[3];    // Stores the 16-bit signed magnetometer sensor output
    // Scale resolutions per LSB for the sensors
    float accScale, gyroScale, magScale;
    // Variables to hold latest sensor data values
    float a[3], g[3], m[3];
    float mg; // hodnota intenzity magnetick0ho pole
    float q[4];

// Pro kalibraci - vypnout korekce !!!    
#ifdef CALIBRATION
    // Bias corrections for gyro, accelerometer, and magnetometer
    float accBias[3] = {0, 0, 0};
    float gyroBias[3]  = {0, 0, 0};
    float magBias[3]   = {0, 0, 0};
    float magCorect[3][3]  = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; 
#else
    float accBias[3] = ACC_BIAS;
    float gyroBias[3] = GYRO_BIAS;
    float magBias[3]  = MAG_BIAS;
    float magCorect[3][3]  = MAG_CORECT; 
#endif

  private:
    void    writeRegister(uint8_t addr, uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t addr, uint8_t reg);
    void    readBytes(uint8_t addr, uint8_t reg, void* data, uint8_t count);

    xSemaphoreHandle mutex = xSemaphoreCreateMutex();

};

#endif
