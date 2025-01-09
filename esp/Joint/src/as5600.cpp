#include "as5600.h"

volatile uint32_t pulseStart = 0;
volatile uint32_t pulseEnd = 0;
volatile uint32_t pulseWidth = 0;
volatile uint32_t pulsePeriod = 0;

void IRAM_ATTR interruptAS5600() {
  uint32_t x = micros();
  bool     e = digitalRead(AS5600_GPIO);
  if (e == 1) {
    pulsePeriod = x - pulseStart;
    pulseStart = x;
  } else {
    pulseEnd = x;
    pulseWidth = pulseEnd - pulseStart;
  }
}

AS5600::AS5600(float zero) {
  this->zero = (zero + 0.5f) * 100;
}

void AS5600::init() {
  pinMode(AS5600_GPIO, INPUT);
  attachInterrupt(AS5600_GPIO, interruptAS5600, CHANGE);
}

int AS5600::angle() {
  int y = (pulseWidth * 4095 / AS5600_GAIN - AS5600_OFFS) * 36000 / 4095 - zero;
  return y;
}  

