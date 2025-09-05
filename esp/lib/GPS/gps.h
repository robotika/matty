#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

#define NMEA_LENGTH   80
#define NMEA_FILTERS  2

struct __attribute__((packed)) GPSinfo {
  uint32_t  timeFix;              // UTC time [hhmmss]  ... TODO convert to unix timestamp
	int32_t   latitude;             // in seconds * 10e6
	int32_t   longitude;            // in seconds * 10e6
	uint16_t  altitude;             // in meters * 16
	uint16_t  hdop;                 // in meters * 256
	uint8_t   numberOfSatellites;
	uint8_t   positionFixStatus;
}; 

class GPS {
  public:
    GPS(Stream*);
    bool    process();
    uint8_t get(uint8_t*); // mode: 1 .. zkopiruje obsah nactene NMEA vety a vrati jeji delku, 2 .. vrati GPSinfo
    void    config(uint8_t);
  private:
  	Stream* serial;
    uint8_t mode = 0;
    uint8_t nmeaReady = 0;
    uint8_t nmeaIndex = 0;
    char    nmeaBuffer[NMEA_LENGTH];
    char    nmeaFilter[NMEA_FILTERS][4] = {"GGA", "RMC"};
    GPSinfo gpsInfo;

    int32_t atoi(char*);
    bool    parse();
};

#endif