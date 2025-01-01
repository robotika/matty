#include "communication.h"

Comm comm(&Serial);

int16_t speed, steer;

void receiveCommand() {
  if (comm.process()) {                             // test prijatych dat
    if (comm.get() == PACKET_READY) {               // nacteni dat
      comm.confirm('A');
      switch (comm.rxData.command) {
        case 'S': speed = 0;
                  break;
        case 'R': break;
        case 'M': break;
        case 'G': speed = comm.rxData.speed;
                  steer = comm.rxData.steer;
                  break;
        case 'L': break;
        case 'T': comm.setPeriod(comm.rxData.period);
                  //
                  break;
      }
    } else {
      comm.confirm('N');
    }
  }
}

void sendData() {
  if (comm.update()) {                              // test casu odesilani dat
    uint16_t enc[] = {101, 23, 1501, 234};
    comm.send(1, 2, 7.26f * 1000, 0.123f * 1000, steer, speed, enc);
  }
}

void setup() {
  comm.begin();
}


void loop() {
  // test prijatych dat a jejich zpracovani
  receiveCommand();

  // periodicke odesilani dat
  sendData();
}