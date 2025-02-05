#include "config.h"
#include "gps.h"

GPS gps(&SerialGPS);

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RXD, GPS_TXD);
}

void loop() {
  if (Serial.available()) {
    char a = Serial.read();
    if (a == 'a') gps.config(1);
    if (a == 's') gps.config(0);
  }

  if (gps.process()) {
    uint8_t data[80];
    uint8_t length = gps.get(data);
    Serial.write(data, length);
  }
}