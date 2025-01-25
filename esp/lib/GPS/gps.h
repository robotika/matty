#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

#define NMEA_LENGTH   80
#define NMEA_FILTERS  2

class GPS {
  public:
    GPS(Stream*);
    bool process();
    uint8_t get(uint8_t*); // zkopiruje obsah nactene NMEA vety a vrati jeji delku
    void    config(uint8_t);
  private:
  	Stream* serial;
    uint8_t mode = 0;
    uint8_t nmea_ready = 0;
    uint8_t nmea_index = 0;
    char    nmea_buffer[NMEA_LENGTH];
    char    nmea_filter[NMEA_FILTERS][4] = {"GGA", "RMC"};
};

#endif