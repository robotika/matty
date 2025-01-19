#include "config.h"

#define VER 1

#define HEADER 0xFF
#define BROADCAST 0xFE
#define PACKET_TIMEOUT 1 // ms

#define INST_SYNC_READ 0x82
#define SMS_STS_PRESENT_POSITION_L 56

void transmitPacket(uint8_t txBuffer[]) {
  uint8_t checksumPacket = 0;

  while (ServoPort.read() != -1); // Clear RX buffer;

  ServoPort.write(HEADER); // 1 Write Header (0xFF) data 1 to serial
  ServoPort.write(HEADER); // 2 Write Header (0xFF) data 2 to serial

  for (uint8_t i = 0; i <= txBuffer[1]; i++) {
    ServoPort.write(txBuffer[i]); // Write Instuction & Parameters (if there are any) to serial
    checksumPacket += txBuffer[i];
  }

  checksumPacket = ~checksumPacket;
  ServoPort.write(checksumPacket); // Write low bit of checksum to serial
}

bool readPacket(uint8_t length) {
  uint32_t startTime = millis();
  while (ServoPort.available() < length) {
    if (millis() - startTime >= PACKET_TIMEOUT) {
      Serial.println("Timeout error");
      return 0;
    }
  }
  for (int i = 0; i < length; i++) {
    Serial.printf("%02X ", ServoPort.read());
  }
  Serial.println();
  return 1;
}

void setup() {
  Serial.begin(115200);
  ServoPort.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
}

#define SERVOS 4
uint8_t idServo[SERVOS] = {4, 2, 3, 1};

uint32_t timeStamp = millis();
#define  PERIOD 100

void loop() {
  uint8_t num = 4; // pocet serv
  uint8_t datalen = 2;

  if (millis() - timeStamp >= PERIOD) {
    timeStamp = millis();

    uint8_t buffer[32];

    buffer[0] = BROADCAST;               // broadcast addr
    buffer[1] = num + 4;       // message length = instr + params (address + data length + servos id) + checksum
    buffer[2] = INST_SYNC_READ;          // function
    buffer[3] = SMS_STS_PRESENT_POSITION_L;                 // address
    buffer[4] = datalen;                 // lenght of data for each device
    buffer[5] = idServo[0];
    buffer[6] = idServo[1];
    buffer[7] = idServo[2];
    buffer[8] = idServo[3];
    transmitPacket(buffer);

    Serial.println();

#if (VER == 1)
  Serial.println(millis());
  for (int i = 0; i < num; i++) {
    readPacket(6 + datalen);
  }
#endif
  }

#if (VER == 0)
  if (ServoPort.available()) {
    Serial.printf("%02X ", ServoPort.read());
  }
#endif

}
