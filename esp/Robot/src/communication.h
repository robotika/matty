#ifndef COMM_H
#define COMM_H

#include "config.h"
#include "data.h"
#include "robbus.h"

class Comm : public Robbus {
  public:
    Comm(HardwareSerial*);
    uint8_t   get(); 
    void      confirm(char message);
    void      send(uint8_t message, void* data = NULL, uint8_t length = 0);
    void      send(uint8_t status, uint8_t mode, uint16_t voltage, uint16_t current, int16_t speed, float angle, int32_t encoder[4]);
    ReceivePacket   rxData;
  private:
    uint8_t   infoCounter = 0;
    TransmitPacket  txData;
};

#endif