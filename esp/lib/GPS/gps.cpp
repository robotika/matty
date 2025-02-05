#include "gps.h"

#define START   '$'
#define CR      13
#define LF      10

GPS::GPS(Stream* serial) {
  this->serial = serial;
}

void GPS::config(uint8_t mode) {
  this->mode = mode;
}

bool GPS::process() {
  while (serial->available()) {
    uint8_t a = serial->read();
    if (a == '$') { // zacatek vety
      nmea_ready = 0;
      nmea_index = 0;
      nmea_buffer[nmea_index++] = a;
    } else if (nmea_index > 0 && nmea_index < NMEA_LENGTH) {
      nmea_buffer[nmea_index++] = a;
      if (a == LF) { // konec vety
        if (strncmp(&nmea_buffer[3], nmea_filter[0], 3) == 0 || strncmp(&nmea_buffer[3], nmea_filter[1], 3) == 0) {
          nmea_ready = 1;
          return (mode != 0);
        }
      }
    }
  }
  return 0;
}

uint8_t GPS::get(uint8_t* buffer) {
  if (nmea_ready) {
    memcpy(buffer, nmea_buffer, nmea_index);
    nmea_ready = 0;
    return nmea_index;
  } 
  return 0;
}
