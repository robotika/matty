#ifndef AS5600_H
#define AS5600_H

#include <Arduino.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "soc/rtc.h"

#define AS5600_GPIO   5
#define AS5600_PERIOD 4351
#define AS5600_OFFS   127
#define AS5600_MAX    4095

class AS5600 {
  public:
    AS5600(float zero = 180.0f);
    void  init();
    int16_t width();
    float angle();
  private:
    float zero;
};

#endif