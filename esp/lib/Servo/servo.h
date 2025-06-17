#ifndef SERVO_H
#define SERVO_H

#include <Arduino.h>

#define PWM_CHANNEL      1
#define PWM_FREQ        50
#define PWM_RESOLUTION  16

class Servo {
  public:
    Servo(uint8_t channel = PWM_CHANNEL);
    void attach(uint8_t pin);
    void detach();
    void write(int pos);
    ~Servo();
  private:
    uint8_t channel;
    uint8_t pin;
};

#endif