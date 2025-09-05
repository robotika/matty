#ifndef POWER_H
#define POWER_H

#define I2C_SDA 32
#define I2C_SCL 33

#include <INA219_WE.h>
#include <i2c.h>

#define INA219_ADDRESS 0x42

class Power : public INA219_WE {
  public:
    Power();
    bool init();
    int  getVoltage();
    int  getCurrent();
  private:
};

#endif