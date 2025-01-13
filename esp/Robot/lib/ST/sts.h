#ifndef STS
#define STS

#include <Arduino.h>

//-------SRAM--------

#define SMS_STS_TORQUE_ENABLE 40
#define SMS_STS_ACC 41
#define SMS_STS_GOAL_POSITION_L 42
#define SMS_STS_GOAL_POSITION_H 43
#define SMS_STS_GOAL_TIME_L 44
#define SMS_STS_GOAL_TIME_H 45
#define SMS_STS_GOAL_SPEED_L 46
#define SMS_STS_GOAL_SPEED_H 47
#define SMS_STS_TORQUE_LIMIT_L 48
#define SMS_STS_TORQUE_LIMIT_H 49
#define SMS_STS_LOCK 55

#define SMS_STS_PRESENT_POSITION_L 56
#define SMS_STS_PRESENT_POSITION_H 57
#define SMS_STS_PRESENT_SPEED_L 58
#define SMS_STS_PRESENT_SPEED_H 59
#define SMS_STS_PRESENT_LOAD_L 60
#define SMS_STS_PRESENT_LOAD_H 61
#define SMS_STS_PRESENT_VOLTAGE 62
#define SMS_STS_PRESENT_TEMPERATURE 63
#define SMS_STS_MOVING 66
#define SMS_STS_PRESENT_CURRENT_L 69
#define SMS_STS_PRESENT_CURRENT_H 70

class Sts {
  public:
    Sts(Stream*);
    int16_t  ping(uint8_t);
    int16_t  read(uint8_t ID, uint8_t address, uint8_t* data, uint8_t dataLen);
    void     write(uint8_t ID, uint8_t address, uint8_t data[], uint8_t dataLen);
    void     syncWrite(uint8_t num, uint8_t id[], uint8_t address, uint8_t data[], uint8_t dataLen);
    int16_t  bulkRead(uint8_t num, uint8_t id[], uint8_t address, uint8_t data[], uint8_t dataLen);

    void     writePosition(uint8_t ID, int16_t position, uint16_t speed = 2000);
    void     writeSyncPosition(uint8_t num, uint8_t id[], int16_t position[]);
    void     writeSyncSpeed(uint8_t num, uint8_t id[], int16_t speed[]);

    uint8_t  isMoving(uint8_t ID);
    int16_t  readPosition(uint8_t ID);
    uint8_t  isSyncMoving(uint8_t num, uint8_t id[]);
    int16_t  readSyncPosition(uint8_t num, uint8_t id[], uint16_t position[]);
  private:
  	Stream* serial;
    uint8_t rxBuffer[32];
    void transmitPacket(uint8_t[]);
    uint16_t readPacket();
    void clearRxBuffer();
};

#endif