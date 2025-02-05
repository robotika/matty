#include <Arduino.h>

#define GPS_RXD   16       // zelena / GPS16 / B_C2
#define GPS_TXD   27       // modra  / GPS17 / B_C1 

#define SerialGPS Serial2

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RXD, GPS_TXD);
  delay(1000);
}

void loop() {
  if (Serial.available()) {
    int a = Serial.read();
    SerialGPS.write(a);
  }

  if (SerialGPS.available()) {
    int a = SerialGPS.read();
    Serial.write(a);
  }
}