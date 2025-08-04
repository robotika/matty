#include "servo.h"

Servo::Servo(uint8_t channel) {
  this->channel = channel;
  ledcSetup(channel, PWM_FREQ, PWM_RESOLUTION);    // nastaveni PWM vystupu
//  write(1500);
}

void Servo::attach(uint8_t pin) {
  this->pin = pin;
  ledcAttachPin(pin, channel);  
}

void Servo::detach() {
  ledcDetachPin(pin);
}

void Servo::write(int pos) {
  pos = 65536 * pos / 20000;
  ledcWrite(channel, pos);    // nastaveni vystupni hodnoty na vystup
}

Servo::~Servo() {
  detach();
}
