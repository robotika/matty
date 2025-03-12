//#include "espnow.h"
#include "data.h"
#include "robot.h" 
#include "communication.h"
#include "gps.h"

#define REMOTE_CONTROL  1
#define AUTONOMOUS      2

//EspNow espnow;

Comm comm(&Serial);
Robot robot(&SerialServo); 
GPS gps(&SerialGPS);

//EspReceiveData  receiveData  = {0, 0, 0};
//EspTransmitData transmitData = {0, 0, 0};

void receiveCommand() {
  if (comm.process()) {                             // test prijatych dat
    if (comm.get() == PACKET_READY) {               // nacteni dat
      comm.confirm('A');
      switch (comm.rxData.command) {
        case 'S': robot.stop(1);
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
        case 'P': gps.config(comm.rxData.mode);
                  break;
        case 'V': {
          // send current version
          uint8_t buffer[2];
          buffer[0] = MATTY;  // serial number
          buffer[1] = ROBOT_FW_VERSION;
          uint8_t length = 2;
          comm.send('V', buffer, length);
          break;
        }
      }
    } else {
      comm.confirm('N');
    }
  }
}

void setup() {
  comm.begin();

  SerialServo.begin(SERVO_BAUDRATE, SERIAL_8N1, S_RXD, S_TXD);
  robot.init();

  SerialGPS.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RXD, GPS_TXD);
}

void loop() {
  // test prijatych dat a jejich zpracovani
  receiveCommand();

  if (robot.process()) { // periodicke odesilani dat
    comm.send(robot.status, robot.mode, robot.voltage, robot.current, robot.actualSpeed, robot.joint, robot.encoder);
  }

  if (gps.process()) {
    uint8_t buffer[80];
    uint8_t length = gps.get(buffer);
    comm.send('P', buffer, length);
  }
  
}
