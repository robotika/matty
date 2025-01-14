#ifndef AS5600_H
#define AS5600_H

#include <Arduino.h>

#define AS5600_GPIO   5
#define AS5600_ZERO   200
#define AS5600_GAIN   4131
#define AS5600_OFFS   127

class AS5600 {
  public:
    AS5600(float zero = 180.0f);
    void  init();
    int   angle();
  private:
    int zero;
};

#endif