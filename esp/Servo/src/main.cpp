#include "config.h"
#include "sts.h"

Sts robot(&SerialServo); 

void setup() {
  Serial.begin(115200);
  SerialServo.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
}

#define SERVOS 4
uint8_t idServo[SERVOS] = {1, 2, 3, 4};

uint32_t timeStamp = millis();
#define PERIOD 100
#define SPEED  1000

void loop() {
  int16_t p[SERVOS];
  if (millis() - timeStamp >= PERIOD) {
    timeStamp += PERIOD;
    int16_t error = robot.readSyncPosition(SERVOS, idServo, (uint16_t*)p);
    Serial.printf("Time: %6d Error: %2d Enc1: %4d Enc2: %4d Enc3: %4d End4: %4d\n\r", millis(), error, p[0], p[1], p[2], p[3]);
  }
  if (Serial.available()) {
    char a = Serial.read();
    if (a == 'a') {
      p[0] = SPEED;
      p[1] = -SPEED;
      p[2] = SPEED;
      p[3] = -SPEED;
      robot.writeSyncSpeed(SERVOS, idServo, p);
    }
    if (a == 's') {
      p[0] = 0;
      p[1] = 0;
      p[2] = 0;
      p[3] = 0;     
      robot.writeSyncSpeed(SERVOS, idServo, p);
    }
    if (a == 'd') {
      p[0] = -SPEED;
      p[1] = SPEED;
      p[2] = -SPEED;
      p[3] = SPEED;      
      robot.writeSyncSpeed(SERVOS, idServo, p);
    }
  }
}
