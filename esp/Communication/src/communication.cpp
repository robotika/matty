#include "communication.h"

Comm::Comm(HardwareSerial* serial) : Robbus(serial) {
} 

uint8_t Comm::get() {
  return receive(&rxData, sizeof(ReceivePacket));
}

void Comm::confirm(char message) {
  txData.counter = rxData.counter;    // 
  txData.message = message;           // ACK/NACK
  transmit(&txData, 2);
}

void Comm::send(uint8_t status, uint8_t mode, uint16_t voltage, uint16_t current, int16_t angle, int16_t speed, uint16_t encoder[4]) {
  txData.counter = infoCounter++;
  txData.message = 'I';
  txData.status = status;
  txData.mode = mode;
  txData.voltage = voltage;
  txData.current = current;
  txData.angle = angle;
  txData.speed = speed;
  txData.encoder[0] = encoder[0];
  txData.encoder[1] = encoder[1];
  txData.encoder[2] = encoder[2];
  txData.encoder[3] = encoder[3];
  transmit(&txData, sizeof(TransmitPacket));
}
