#ifndef ESPNOW_H
#define ESPNOW_H

#include "config.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <EEPROM.h>

#ifdef DEBUG
  #define DEBUG_BEGIN(...) { Serial.begin(__VA_ARGS__); }
  #define DEBUG_PRINT(...) { Serial.print(__VA_ARGS__); }
  #define DEBUG_PRINTF(...) { Serial.printf(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { Serial.println(__VA_ARGS__); }
#else
  #define DEBUG_BEGIN(...) {}
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTF(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif  

#define MAC_LEN   6
#define MAC_ADDR  0

class EspNow {
  public:
    enum {NOT_CONNECTED = -1, CONNECTED = 0, PAIRING = 1};
    enum {NO_DATA = 0, DATA_READY = 1};
    volatile int     status;

    void init();
    void pair();
    int  sendData(char*);
    int  sendData(void* = NULL, int = 0);
    int  getStatus();
    int  getData(void*, int);
    void receiveData(const uint8_t* mac, const uint8_t* incomingData, int len);
  private:    
    static const int MAX_LEN = 32;
    volatile uint8_t data[MAX_LEN];
    volatile int     dataReady;
    uint8_t   destinationAddress[MAC_LEN];
    uint32_t  lastData;
    void addPeer(uint8_t* mac);
};

#endif