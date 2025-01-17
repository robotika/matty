#include "sts.h"

#define HEADER 0xFF

#define STATUS_PACKET_TIMEOUT 50 // in millis()
#define STATUS_FRAME_BUFFER 5

//################ Instruction commands Set ###############################
#define INST_PING 0x01
#define INST_READ 0x02
#define INST_WRITE 0x03
#define INST_REG_WRITE 0x04
#define INST_REG_ACTION 0x05
#define INST_SYNC_READ 0x82
#define INST_SYNC_WRITE 0x83

#define ERROR  0b10000000

Sts::Sts(Stream* serial) {
  this->serial = serial;
}

void Sts::transmitPacket(uint8_t txBuffer[]) {
    uint8_t checksumPacket = 0;

    clearRxBuffer();

    serial->write(HEADER); // 1 Write Header (0xFF) data 1 to serial
    serial->write(HEADER); // 2 Write Header (0xFF) data 2 to serial

    for (uint8_t i = 0; i <= txBuffer[1]; i++) {
        serial->write(txBuffer[i]); // Write Instuction & Parameters (if there are any) to serial
        checksumPacket += txBuffer[i];
    }

    checksumPacket = ~checksumPacket;
    serial->write(checksumPacket); // Write low bit of checksum to serial
//    readPacket(); // Reading echo
}

uint16_t Sts::readPacket() {
    uint8_t counter = 0x00;
    uint8_t firstHeader = 0x00;

    rxBuffer[0] = 0x00;
    rxBuffer[1] = 0x00;
    rxBuffer[2] = 0x00;
    rxBuffer[3] = 0x00;

    uint32_t timeCounter = STATUS_PACKET_TIMEOUT + millis(); // Setup time out error

    while (STATUS_FRAME_BUFFER >= serial->available()) { // Wait for " header + header + id + frame length + error " RX data
        if (millis() >= timeCounter) {
            return rxBuffer[2] = ERROR; // Return with Error if Serial data not received with in time limit
        }
    }

    if (serial->peek() == 0xFF && firstHeader != 0xFF) {
        firstHeader = serial->read(); // Clear 1st header from RX buffer
    } else if (serial->peek() == -1) {
        return rxBuffer[2] = ERROR; // Return with Error if two headers are not found
    }
    if (serial->peek() == 0xFF && firstHeader == 0xFF) {
        serial->read();               // Clear 2nd header from RX buffer
        rxBuffer[0] = serial->read(); // ID sent from Dynamixel
        rxBuffer[1] = serial->read(); // Frame Length of status packet
        rxBuffer[2] = serial->read(); // Error byte

        timeCounter = STATUS_PACKET_TIMEOUT + millis();
        while (rxBuffer[1] - 2 >= serial->available()) { // Wait for wait for "Para1 + ... Para X" received data
            if (millis() >= timeCounter) {
                return rxBuffer[2] = ERROR; // Return with Error if Serial data not received with in time limit
            }
        }
        do {
            rxBuffer[3 + counter] = serial->read();
            counter++;
        } while (rxBuffer[1] > counter); // Read Parameter(s) into array

        rxBuffer[counter + 4] = serial->read(); // Read Check sum
    } else {
        return rxBuffer[2] = ERROR; // Return with Error if two headers are not found
    }
}

void Sts::clearRxBuffer(void) {
    while (serial->read() != -1); // Clear RX buffer;
}

int16_t Sts::read(uint8_t ID, uint8_t address, uint8_t* data, uint8_t dataLen) {

    uint8_t buffer[8];
    buffer[0] = ID;
    buffer[1] = 4;                       // mgsLen
    buffer[2] = INST_READ;               // function
    buffer[3] = address;                 // address
    buffer[4] = dataLen;                 // lenght of reading data

    transmitPacket(buffer);
    readPacket();

    if (rxBuffer[2] == 0) {                                    // If there is no status packet error return value
        for (uint8_t i = 0; i < dataLen; i++) {
            data[i] = rxBuffer[3 + i];
        }
        return 0; 
    } else {
        return (rxBuffer[2] | 0xF000); // If there is a error Returns error value
    }
}

void Sts::write(uint8_t ID, uint8_t address, uint8_t data[], uint8_t dataLen) {

    uint8_t buffer[32];
    buffer[0] = ID;
    buffer[1] = 3 + dataLen;             // mgsLen
    buffer[2] = INST_WRITE;              // function
    buffer[3] = address;                 // address

    for (uint8_t i = 0; i < dataLen; i++) {
        buffer[4 + i] = data[i];
    }
        
    transmitPacket(buffer);
    readPacket();
}

void Sts::syncWrite(uint8_t num, uint8_t id[], uint8_t address, uint8_t data[], uint8_t dataLen) {
    uint8_t buffer[64];

    buffer[0] = 0xFE;                    // broadcast addr
    buffer[1] = (dataLen + 1) * num + 4; // message length
    buffer[2] = INST_SYNC_WRITE;         // function
    buffer[3] = address;                 // address
    buffer[4] = dataLen;                 // lenght of data for each device

    uint8_t* bufferPtr = &buffer[5];

    for (uint8_t i = 0; i < num; i++) {
        *bufferPtr++ = id[i];
        for (uint8_t j = 0; j < dataLen; j++) {
            *bufferPtr++ = *data++;
        }
    }

    transmitPacket(buffer);
}

int16_t Sts::bulkRead(uint8_t num, uint8_t id[], uint8_t address, uint8_t data[], uint8_t dataLen) {
    uint8_t buffer[64];
    int16_t result = 0;

    buffer[0] = 0xFE;                    // broadcast addr
    buffer[1] = num + 4;                 // message length
    buffer[2] = INST_SYNC_READ;          // function
    buffer[3] = address;                 // address
    buffer[4] = dataLen;                 // lenght of data for each device

    for (uint8_t j = 0; j < num; j++) {
        buffer[5 + j] = id[j];
    }

    transmitPacket(buffer);

    for (uint8_t j = 0; j < num; j++) {
        readPacket();
        for (uint8_t i = 0; i < dataLen; i++) {
            if (rxBuffer[2] == 0) {                                    // If there is no status packet error return value
                data[j*dataLen + i] = rxBuffer[3 + i];
            } else {
                data[j*dataLen + i] = 0xff; // Error read
                result -= 1;
            }
        }
    }
    return result / dataLen;
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
  uint8_t p[2];
  bulkRead(num, id, SMS_STS_MOVING, (uint8_t*)&p, 1);
  for (uint8_t i = 0; i < num; i++) {
      if (p[i] != 0) return 1;
  }
  return 0;
}

int16_t Sts::readSyncPosition(uint8_t num, uint8_t id[], uint16_t position[]) {
  return bulkRead(num, id, SMS_STS_PRESENT_POSITION_L, (uint8_t*)position, 2);
}

int16_t Sts::ping(uint8_t ID) {
    uint8_t buffer[10];
    buffer[0] = ID;
    buffer[1] = 2; // PING_LENGTH;
    buffer[2] = INST_PING;

    transmitPacket(buffer);
    readPacket();

    if (rxBuffer[2] == 0) {                                    // If there is no status packet error return value
        return (rxBuffer[0]); // Return SERVO ID
    } else {
        return (rxBuffer[2] | 0xF000); // If there is a error Returns error value
    }
}