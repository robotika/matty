//#include "espnow.h"
#include "data.h"
#include "robot.h" 
#include "communication.h"

#define SPEED_LIMIT   500   // mm/s
#define ANGLE_LIMIT    45   // 0,01°

#define VOLTAGE_LIMIT  (5 * 1000) // 5 V
#define REMOTE_CONTROL  1
#define AUTONOMOUS      2
#define EMERGENCY_STOP  3

//EspNow espnow;

Comm comm(&Serial);
Robot robot(&ServoPort); 

//EspReceiveData  receiveData  = {0, 0, 0};
//EspTransmitData transmitData = {0, 0, 0};

void receiveCommand() {
  if (comm.process()) {                             // test prijatych dat
    if (comm.get() == PACKET_READY) {               // nacteni dat
      comm.confirm('A');
      switch (comm.rxData.command) {
        case 'S': robot.stop();
                  break;
        case 'R': robot.reset();
                  break;
        case 'M': break;        
        case 'G': robot.go(comm.rxData.speed, comm.rxData.steer / 100.0f);
                  break;
        case 'L': robot.setLimits(comm.rxData.speed, comm.rxData.steer);
                  break;
        case 'T': robot.setTime(comm.rxData.period, comm.rxData.timeout);
                  break;
      }
    } else {
      comm.confirm('N');
    }
  }
}

void setup() {
  comm.begin();

  ServoPort.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
  robot.init();
}

void loop() {
  // test prijatych dat a jejich zpracovani
  receiveCommand();

  if (robot.process()) { // periodicke odesilani dat
    comm.send(1, 2, robot.voltage, robot.current, robot.actualSpeed, robot.joint, robot.encoder);
  }
}
