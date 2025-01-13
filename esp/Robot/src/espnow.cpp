#include "espnow.h"

EspNow* _espnow;

const uint32_t TIMEOUT  = 2000;
const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const char ACK[]  = "ACK";
const char PAIR[] = "PAIR";

void PrintMAC(const uint8_t* addr) {
  DEBUG_PRINTF("%02x:%02x:%02x:%02x:%02x:%02x\n\r", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);  
}

// Callback volany po prijeti dat
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {  
  _espnow->receiveData(mac, incomingData, len);
}

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t s) {
  DEBUG_PRINT("\t\tSend status: ");
  DEBUG_PRINTLN(s == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (s != ESP_NOW_SEND_SUCCESS) _espnow->status = EspNow::NOT_CONNECTED;
}

void EspNow::init() {
  EEPROM.begin(16);
  EEPROM.readBytes(MAC_ADDR, destinationAddress, MAC_LEN);
  PrintMAC(destinationAddress);

//  status = NOT_CONNECTED;
  status = PAIRING;
  dataReady = NO_DATA;

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    return;
  }

  // Registrace callback po prijeti dat
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  // Registrace callback po odeslani dat
  esp_now_register_send_cb(OnDataSent);

  _espnow = this;
}
 
void EspNow::receiveData(const uint8_t* mac, const uint8_t* incomingData, int len) {
  if (/*memcmp(esp_now_recv_info.des_addr, broadcastAddress, MAC_LEN) == 0 &&*/ memcmp(incomingData, (uint8_t*)PAIR, sizeof(PAIR)) == 0) { 
    if (status == PAIRING) {
      memcpy(destinationAddress, mac, MAC_LEN);
//      EEPROM.writeBytes(MAC_ADDR, destinationAddress, MAX_LEN);
      status = NOT_CONNECTED;
    }
    if (status == NOT_CONNECTED) {
      if (memcmp(mac, destinationAddress, MAC_LEN) == 0) { 
        addPeer(destinationAddress);

        esp_err_t result = esp_now_send(destinationAddress, (uint8_t*)ACK, sizeof(ACK));
        if (result == ESP_OK) {
          DEBUG_PRINT("Pair ok");
          lastData = millis();
          status = CONNECTED;
        } else {
          DEBUG_PRINTLN("Error sending the data");
        }
      }
    }
  } else {
    if (status == CONNECTED && memcmp(destinationAddress, mac, MAC_LEN) == 0) {
      memcpy((void*)data, incomingData, (len > MAX_LEN) ? MAX_LEN : len);
      dataReady = DATA_READY;
      lastData = millis();
    }
  }
}

int EspNow::getData(void* payload, int len) {
  if (dataReady == DATA_READY) {
    memcpy(payload, (const void*)data, len);
    dataReady = NO_DATA;
    return 1;
  } else { 
    if (millis() - lastData > TIMEOUT && status == CONNECTED) {
      status = NOT_CONNECTED;
    }
    if (status == NOT_CONNECTED) {
      return -1;
    }
    return 0;
  }
}

void EspNow::pair() {
  status = PAIRING;
  memset(destinationAddress, 0, MAC_LEN);
}

int EspNow::sendData(void* payload, int len) {
  if (status == CONNECTED) {
    esp_err_t result = esp_now_send(destinationAddress, (uint8_t*)payload, len);
    DEBUG_PRINT("Send data: ");
    DEBUG_PRINTLN((char*)payload);
    if (result == ESP_OK) {
      DEBUG_PRINT("Data sent");
    } else {
      DEBUG_PRINTLN("Error sending the data");
      status = NOT_CONNECTED;
    }  
  } 
  return status;
}

int EspNow::sendData(char* s) {
  return sendData((uint8_t*)s, strlen(s));
}  

void EspNow::addPeer(uint8_t* mac) {
  if (!esp_now_is_peer_exist(mac)) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, MAC_LEN);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;   
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      DEBUG_PRINTLN("Failed to add peer");
      return;
    } else {
      DEBUG_PRINTLN("Add peer ok");
    }
  }
}
