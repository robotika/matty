#ifndef ROBBUS_H
#define ROBBUS_H

#include "config.h"

#define ROBBUS_TIMEOUT      20 // ms
#define TRANSMIT_PERIOD      0 // ms, default vypnuto

#define SYNC							0x55

enum TState {
  IDLE,
  WAIT_FOR_RECEIVE,
  RECEIVE_PROGRESS,
  PACKET_COMPLETE,
  TIMEOUT_ERROR
};

enum TReceive {
  NO_DATA,
  PACKET_READY,
  PACKET_TIMEOUT,
  PACKET_CRC_ERROR
};

#define PACKET_LENGTH   124
#define LENGTH_OFFSET   offsetof(Packet, length)

typedef union __attribute__((packed)) {
  struct {
    uint8_t sync;
    uint8_t length;
    uint8_t data[PACKET_LENGTH + 1]; // + CRC
  };  
  uint8_t d[PACKET_LENGTH + 3];
} Packet; 

class Robbus {
  public:
    Robbus(HardwareSerial*);
    void     begin();
    bool     update();
    void     transmit(const void* data, uint8_t length);
    bool     process(); 
    uint8_t  receive(void* data, uint8_t length = PACKET_LENGTH); 
    void     setPeriod(uint16_t t);
  private:
    int8_t   status = 0;
    Packet   rxPacket;
    uint8_t  rxLength; 
    uint8_t  rxIndex;
    HardwareSerial*  serial;
    uint32_t transmitTime;
    uint32_t transmitPeriod = TRANSMIT_PERIOD;
    uint32_t receiveTime;
    uint32_t timeOut = ROBBUS_TIMEOUT;

    uint8_t  crc(uint8_t* data, uint8_t len);
};

#endif