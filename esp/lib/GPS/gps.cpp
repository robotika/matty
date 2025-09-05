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
      nmeaReady = 0;
      nmeaIndex = 0;
      nmeaBuffer[nmeaIndex++] = a;
    } else if (nmeaIndex > 0 && nmeaIndex < NMEA_LENGTH) {
      nmeaBuffer[nmeaIndex++] = a;
      if (a == LF) { // konec vety
        if (strncmp(&nmeaBuffer[3], nmeaFilter[0], 3) == 0 || strncmp(&nmeaBuffer[3], nmeaFilter[1], 3) == 0) {
          nmeaReady = 1;
          return (mode != 0);
        }
      }
    }
  }
  return 0;
}

uint8_t GPS::get(uint8_t* buffer) {
  if (nmeaReady) {
    if (mode == 1) { // raw data
      memcpy(buffer, nmeaBuffer, nmeaIndex);
      nmeaReady = 0;
      return nmeaIndex;
    } else {
      if (parse()) {
        memcpy(buffer, &gpsInfo, sizeof(GPSinfo));      
        return sizeof(GPSinfo);
      }
    }
  } 
  return 0;
}

int32_t GPS::atoi(char* p) {
  int32_t x = 0;
  int8_t sign = 0;
  if (*p == '-') {
    sign = 1;
    p++;
  }
  while (*p >= '0' && *p <= '9') {
    x = x * 10 + (*p - '0');
    p++;
  }
  if (sign) x = -x;
  return x;
} 

bool GPS::parse() {
	uint8_t i;
	char* endptr;
	int32_t  whole, frac;

  memset(&gpsInfo, 0, sizeof(GPSinfo));
  
  #define next(a)		while(nmeaBuffer[i++] != a);
  #define decimal() { whole = atoi(&nmeaBuffer[i]); \
		                  if (whole != 0)	{ \
			                  next('.'); \
			                  frac = atoi(&nmeaBuffer[i]); \
		                  } \
                    }

  if (strncmp(&nmeaBuffer[3], "GGA", 3) == 0) { // GGA
		i = 7;  		                              // start parsing just after "GGA"		
		if(nmeaBuffer[i] == ',' && nmeaBuffer[i+1] == ',') // attempt to reject empty packets right away
			return 0;
			
		gpsInfo.timeFix = atoi(&nmeaBuffer[i]);   // get UTC time [hhmmss]   ... TODO convert to unix timestamp

		next(',');				                        // next field: latitude
    decimal();                                // get latitude [ddmm.mmmmm]
		gpsInfo.latitude = ((whole/100)*600000 + ((whole%100)*10000 + (frac+5)/10))*6; // convert to seconds [ssssssss*10e3] format
		next(',');				                        // next field: N/S indicator
		if (nmeaBuffer[i] == 'S') gpsInfo.latitude = -gpsInfo.latitude;  		// correct latitute for N/S

		next(',');				                        // next field: longitude	
    decimal();                                // get longitude [ddmm.mmmmm]
		gpsInfo.longitude = ((whole/100)*600000 + ((whole%100)*10000 + (frac+5)/10))*6; // convert to seconds [ssssssss*10e3] format
		next(',');				                        // next field: E/W indicator
		if (nmeaBuffer[i] == 'W') gpsInfo.longitude = -gpsInfo.longitude; // correct latitute for E/W

		next(',');				                        // next field: position fix status
		gpsInfo.positionFixStatus = atoi(&nmeaBuffer[i]); // position fix status  0 = Invalid, 1 = Valid SPS, 2 = Valid DGPS, 3 = Valid PPS
	
		next(',');	                        			// next field: satellites used
		gpsInfo.numberOfSatellites = atoi(&nmeaBuffer[i]); // get number of satellites used in GPS solution

		next(',');				                        // next field: HDOP (horizontal dilution of precision)
    decimal();                                // get HDOP
    gpsInfo.hdop = (256 * whole + frac);      // get HDOP (horizontal dilution of precisiom) (in meters * 256)

		next(',');				                        // next field: altitude
    decimal();
    gpsInfo.altitude = (16 * whole + frac / 16);  // get altitude (in meters * 16)
	
    next(',');				// next field: altitude units, always 'M'
    next(',');				// next field: geoid seperation
    next(',');				// next field: seperation units
    next(',');				// next field: DGPS age
    next(',');				// next field: DGPS station ID
    next('*');				// next field: checksum

//    printf("Lat: %f Long: %f\n", gpsInfo.latitude/3.6e6, gpsInfo.longitude/3.6e6);
		return 1;
	}
  return 0;
}
