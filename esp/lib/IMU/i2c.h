#ifndef I2C_H
#define I2C_H

#include <Arduino.h>
#include <Wire.h>

class I2C : public TwoWire {
  public:
    using TwoWire::TwoWire; // prebira constructory z predka
    bool lock();
    void unlock();
  private:
    xSemaphoreHandle mutex = xSemaphoreCreateMutex();
};

extern I2C Wire2;

#endif
