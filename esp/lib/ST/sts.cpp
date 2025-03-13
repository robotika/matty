#include "sts.h"


#define ST_PACKET_TIMEOUT   200 // us
#define ST_HEADER           0xFF
#define ST_BROADCAST        0xFE

//################ Instruction commands Set ###############################
#define INST_PING 0x01
#define INST_READ 0x02
#define INST_WRITE 0x03
#define INST_REG_WRITE 0x04
#define INST_REG_ACTION 0x05
#define INST_SYNC_READ 0x82
#define INST_SYNC_WRITE 0x83


Sts::Sts(Stream* serial) {
  this->serial = serial;
}

void Sts::transmitPacket() {
  uint8_t checksum = 0;
  clearRxBuffer();

  // txBuffer.length = instruction + num of parameters + checksum
  txBuffer.header1 = ST_HEADER; // 1 Write Header (0xFF) data 1
  txBuffer.header2 = ST_HEADER; // 2 Write Header (0xFF) data 2
  for (uint8_t i = 0; i < txBuffer.length + 1; i++) { // id + length + instruction + parameter 1 + … parameter n
    checksum += txBuffer.data[i];
  }
  txBuffer.data[txBuffer.length + 1] = ~checksum;  // Write low bit of complement checksum

  serial->write((uint8_t*)&txBuffer, txBuffer.length + 4); 
  serial->flush();
}

int8_t Sts::readPacket(uint8_t length) {
  uint32_t startTime = micros();
  uint8_t checksum = 0;

  length += 2; // + error + checksum

  // Wait for required number of data
  while (serial->available() < length + 4) { // + 2 x header + id + length
    if (micros() - startTime >= ST_PACKET_TIMEOUT) return ST_ERROR_TIMEOUT;
  }

  rxBuffer.header1 = serial->read();
  rxBuffer.header2 = serial->read();
  for (int i = 0; i < length + 2; i++) { // + id + length
    rxBuffer.data[i] = serial->read();
    checksum += rxBuffer.data[i];  
  }

  if (rxBuffer.header1 != ST_HEADER || rxBuffer.header2 != ST_HEADER) return ST_ERROR_READ;
  if (rxBuffer.length != length) return ST_ERROR_READ;
  if (checksum != 0xFF) return ST_ERROR_CHECKSUM;

  return ST_PACKET_READY;
}

void Sts::clearRxBuffer(void) {
    while (serial->read() != -1); // Clear RX buffer;
}

int16_t Sts::read(uint8_t id, uint8_t address, uint8_t* data, uint8_t dataLen) {

  txBuffer.id = id;
  txBuffer.length = 4;              // mgsLen
  txBuffer.instruction = INST_READ; // function
  txBuffer.params[0] = address;     // address
  txBuffer.params[1] = dataLen;     // lenght of reading data

  transmitPacket();
  int8_t error = readPacket(dataLen);
  if (error != ST_PACKET_READY) return error;

  for (uint8_t i = 0; i < dataLen; i++) {
    data[i] = rxBuffer.params[i];
  }
  return ST_PACKET_READY; 
}

void Sts::write(uint8_t id, uint8_t address, uint8_t data[], uint8_t dataLen) {

  txBuffer.id = id;
  txBuffer.length = 3 + dataLen;         // mgsLen
  txBuffer.instruction = INST_WRITE;     // function
  txBuffer.params[0] = address;          // address

  for (uint8_t i = 0; i < dataLen; i++) {
    txBuffer.params[i + 1] = data[i];
  }
        
  transmitPacket();
  readPacket();
}

void Sts::syncWrite(uint8_t num, uint8_t id[], uint8_t address, uint8_t data[], uint8_t dataLen) {
  txBuffer.id = ST_BROADCAST;                 // broadcast addr
  txBuffer.length = (dataLen + 1) * num + 4;  // message length
  txBuffer.instruction = INST_SYNC_WRITE;     // function
  txBuffer.params[0] = address;               // address
  txBuffer.params[1] = dataLen;               // length of data for each device

  uint8_t* bufferPtr = &txBuffer.params[2];
  for (uint8_t i = 0; i < num; i++) {
    *bufferPtr++ = id[i];
    for (uint8_t j = 0; j < dataLen; j++) {
      *bufferPtr++ = *data++;
    }
  }

  transmitPacket();
}

int16_t Sts::bulkRead(uint8_t num, uint8_t id[], uint8_t address, uint8_t data[], uint8_t dataLen) {
  int16_t result = 0;

  txBuffer.id = ST_BROADCAST;                // broadcast addr
  txBuffer.length = num + 4;                 // message length
  txBuffer.instruction = INST_SYNC_READ;     // function
  txBuffer.params[0] = address;              // address
  txBuffer.params[1] = dataLen;              // length of data for each device

  for (uint8_t j = 0; j < num; j++) {
    txBuffer.params[2 + j] = id[j];
  }

  transmitPacket();
//  delay(1); // jinak timeout u prvniho packetu odpovedi

  memset(data, 0, num * dataLen);
  for (uint8_t j = 0; j < num; j++) {
    int16_t error = readPacket(dataLen);
    if (error == ST_PACKET_READY) {
      uint8_t k = 0;
      while (id[k] != rxBuffer.id && k < num) k++;
      memcpy(&data[k * dataLen], &rxBuffer.params, dataLen);
    } else {
      result--;
    }
  }
  return result;
}

void Sts::writePosition(uint8_t ID, int16_t position, uint16_t speed) {
  uint8_t data[6];

  data[0] = position & 0xFF;
  data[1] = position >> 8;
  data[2] = 0;
  data[3] = 0;
  data[4] = speed & 0xFF;
  data[5] = speed >> 8;

  write(ID, SMS_STS_GOAL_POSITION_L, data, 6);
}

void Sts::writeSyncPosition(uint8_t num, uint8_t id[], int16_t position[]) {
  syncWrite(num, id, SMS_STS_GOAL_POSITION_L, (uint8_t*)position, 2);
}

void Sts::writeSyncSpeed(uint8_t num, uint8_t id[], int16_t speed[]) {
  for (uint8_t i = 0; i < num; i++) {
    if (speed[i] < 0) {
      speed[i] = -speed[i];
      speed[i] |= (1 << 15);
    }
  }
  syncWrite(num, id, SMS_STS_GOAL_SPEED_L, (uint8_t*)speed, 2);
}

void Sts::enableTorqueServos(uint8_t num, uint8_t id[], uint8_t enable) {
  uint8_t e[16];
  memset(e, enable, num);
  syncWrite(num, id, SMS_STS_TORQUE_ENABLE, e, 1);
}

// Divne, prozkoumat, co to presne udela
void Sts::stopServos(uint8_t num, uint8_t id[]) {
  enableTorqueServos(num, id, 0);
  delay(10);
  enableTorqueServos(num, id, 1);
  int16_t p[] = {0, 0, 0, 0};  
  writeSyncSpeed(num, id, p);
}

uint8_t  Sts::isMoving(uint8_t ID) {
  uint8_t moving;
  read(ID, SMS_STS_MOVING, &moving, 1);
  return moving;
}

int16_t  Sts::readPosition(uint8_t ID) {
  uint16_t p;  
  read(1, SMS_STS_PRESENT_POSITION_L, (uint8_t*)&p, 2);
  return p;
}

uint8_t  Sts::isSyncMoving(uint8_t num, uint8_t id[]) {
  uint8_t p[4];
  bulkRead(num, id, SMS_STS_MOVING, (uint8_t*)&p, 1);
  for (uint8_t i = 0; i < num; i++) {
    if (p[i] != 0) return 1;
  }
  return 0;
}

int16_t Sts::readSyncPosition(uint8_t num, uint8_t id[], uint16_t position[]) {
  return bulkRead(num, id, SMS_STS_PRESENT_POSITION_L, (uint8_t*)position, 2);
}

int16_t Sts::ping(uint8_t id) {
  txBuffer.id = id;
  txBuffer.length = 2;
  txBuffer.instruction = INST_PING;

  transmitPacket();
  readPacket();

  if (rxBuffer.error == 0) {                                    // If there is no status packet error return value
    return (rxBuffer.id); // Return SERVO ID
  } else {
    return (rxBuffer.error | 0xF000); // If there is a error Returns error value
  }
}