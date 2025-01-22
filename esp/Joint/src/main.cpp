#include <Arduino.h>
#include "as5600.h"

//AS5600 as5600(0);
AS5600 as5600(178.59f);

void setup() {
  Serial.begin(115200);
  as5600.init();
}

void loop() {
  int x = pulseIn(AS5600_GPIO, 1);
  float z = as5600.angle();
  Serial.printf("Width: %5d Angle: %6.2f\n\r", x, z);
  delay(100);
}