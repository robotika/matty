#include <Arduino.h>

#define S_RXD 18
#define S_TXD 19

void setup() {
  Serial.begin(115200);
  Serial1.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
  delay(1000);
}

void loop() {
  if (Serial.available()) {
    int a = Serial.read();
    Serial1.write(a);
  }
  if (Serial1.available()) {
    int a = Serial1.read();
    Serial.write(a);
  }
}
