#include "config.h"
#include "sts.h"

Sts robot(&ServoPort); 

void setup() {
  Serial.begin(115200);
  ServoPort.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
}

#define SERVOS 4
uint8_t idServo[SERVOS] = {1, 2, 3, 4};

void loop() {
  uint16_t p[SERVOS];
  int16_t error = robot.readSyncPosition(SERVOS, idServo, p);
  Serial.printf("Error: %d Enc1: %d Enc2: %d Enc3: %d End4: %d\n\r", error, p[0], p[1], p[2], p[3]);
  delay(1000);
}
