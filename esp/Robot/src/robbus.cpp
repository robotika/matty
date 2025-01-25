#include "robbus.h"

Robbus::Robbus(HardwareSerial* serial) {
  this->serial = serial;
}

void Robbus::begin() {
  serial->begin(ROBBUS_BAUDRATE);
  status = WAIT_FOR_RECEIVE;
}

// Kontroluje, zda je prijat znak a zpracuje jej
bool Robbus::process() {
#ifdef ESC        
  static uint8_t esc = 0;
#endif
  if (status != WAIT_FOR_RECEIVE && status != RECEIVE_PROGRESS) return 1; // nemelo by nikdy nastat, ale jistota

  while (serial->available()) {
    uint8_t x = serial->read();
    if (status == WAIT_FOR_RECEIVE && x == SYNC) {
      rxIndex = 1;   // zacatek paketu
      receiveTime = millis(); // nulovani timeoutu
      status = RECEIVE_PROGRESS;
    } else {
      if (status == RECEIVE_PROGRESS) {  
#ifdef ESC      
        if (x == ESC) {
          esc = 1;
          continue;
        } else if (esc) {
          x = ~x;
          esc = 0;
        }
#endif             
        rxPacket.d[rxIndex] = x;  // ulozeni dat do buferu
        if (rxIndex > LENGTH_OFFSET) { 
          if (rxIndex == rxLength + 2) { // posledni znak paketu (CRC)
            status = PACKET_COMPLETE;
            break;
          }
        } else if (rxIndex == LENGTH_OFFSET) { // delka paketu
          rxLength = x;
          if (x > PACKET_LENGTH) { // Podezrele dlouhy packet
            status = WAIT_FOR_RECEIVE;
          }
        }
        rxIndex++;
      }
    }
  }
  if (status == RECEIVE_PROGRESS && timeOut != 0 && (millis() - receiveTime > timeOut)) {
    status = TIMEOUT_ERROR;
  }
  return (status == PACKET_COMPLETE || status == TIMEOUT_ERROR); 
}

#ifdef ESC
void Robbus::transmit(const void* data, uint8_t length) {
  Packet txPacket;
  txPacket.sync = SYNC;
  txPacket.length = length;
  memcpy(&txPacket.data, data, length);
  txPacket.data[length] = crc(&txPacket.length, length + 1); // + LEN

  uint8_t  txBuffer[2 * PACKET_LENGTH];
  uint16_t txLength = 1; // SYNC, LEN, ...
  txBuffer[0] = SYNC;
  for (int i = 1; i < txPacket.length + 3; i++) { // + CRC
    if (txPacket.d[i] == SYNC || txPacket.d[i] == ESC) {
      txPacket.d[i] = ~txPacket.d[i];
      txBuffer[txLength++] = ESC;
    }
    txBuffer[txLength++] = txPacket.d[i];
  }
  serial->write(txBuffer, txLength);
}
#else
void Robbus::transmit(const void* data, uint8_t length) {
  Packet txPacket;
  txPacket.sync = SYNC;
  txPacket.length = length;
  memcpy(&txPacket.data, data, length);
  txPacket.data[length] = crc(&txPacket.length, length + 1); // + LEN
  serial->write((uint8_t*)&txPacket, length + 3); // + SYNC, LEN, CRC
}
#endif

uint8_t Robbus::receive(void* data, uint8_t length) {
  uint8_t result = NO_DATA;
  if (status == PACKET_COMPLETE) { // data received
    if (crc(&rxPacket.length, rxPacket.length + 2) == 0) { // + LEN, CRC
      if (rxPacket.length < length) length = rxPacket.length;
      memcpy(data, &rxPacket.data, length);
      result = PACKET_READY;
    } else {
      result = PACKET_CRC_ERROR;
    }
    status = WAIT_FOR_RECEIVE;
  }
  if (status == TIMEOUT_ERROR) {
    result = PACKET_TIMEOUT;
    status = WAIT_FOR_RECEIVE;
  }
  return result;
} 

uint8_t inline Robbus::crc(uint8_t* data, uint8_t len) {
  uint8_t c = 0;
  for (uint8_t i = 0; i < len; i++) {
    c += data[i];
  } 
  return 256-c; 
}
