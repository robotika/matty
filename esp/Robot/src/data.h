#ifndef DATA_H
#define DATA_H

#include "config.h"

// Robbus

enum ROBOT_STATUS {
  EMERGENCY_STOP  = 0x01,
  VOLTAGE_LOW     = 0x02,
  BUMPER_FRONT    = 0x04,
  BUMPER_BACK     = 0x08,
  ERROR_ENCODER   = 0x10,
  ERROR_POWER     = 0x20,
  RUNNING         = 0x80
};

struct __attribute__((packed)) ReceivePacket {
  uint8_t counter;
  uint8_t command;
  union {
    struct {
      uint8_t mode;
    };
    struct {
      int16_t speed;      // mm/s
      int16_t steer;      // 0,01°
    };
    struct {
      uint16_t period;    // ms
      uint16_t timeout;   // ms
    };
  };
};

struct __attribute__((packed)) TransmitPacket {
  uint8_t  counter;
  uint8_t  message;
  uint8_t  status;
  uint8_t  mode;
  uint16_t voltage;       // mV   
  uint16_t current;       // mA
  int16_t  speed;         // mm/s
  int16_t  angle;         // 0,01°
  uint16_t encoder[4];    // mm
};

struct __attribute__((packed)) TransmitData {
  uint8_t  counter;
  uint8_t  message;
  uint8_t  data[128];
};

// ESP NOW

struct __attribute__((__packed__)) RcReceiveData {
  uint8_t mode;
  int16_t speed;
  int16_t steer;
};

struct __attribute__((__packed__)) RcTransmitData {
  uint8_t status;
  int16_t voltage;
  int16_t current;
};

#endif
