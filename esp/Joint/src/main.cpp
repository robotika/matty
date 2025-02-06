#include <Arduino.h>
#include "as5600.h"
#include "config.h"

AS5600 as5600(0);
//AS5600 as5600(178.59f);
//AS5600 as5600(ZERO_JOINT);

void setup() {
  Serial.begin(115200);
  as5600.init();
}

void loop() {
  int   x = as5600.width();
  float z = as5600.angle();
  Serial.printf("Width: %5d Angle: %6.2f\n\r", x, z);
  delay(100);
}