#include "data.h"
#include "robot.h" 
#include "communication.h"
#include "gps.h"

Comm  comm(&SerialComm);
Robot robot(&SerialServo); 
GPS   gps(&SerialGPS);

#ifdef USE_ESP_NOW
  #include "espnow.h"
  EspNow espnow;
  RcReceiveData   receiveData  = {0, 0, 0};
  RcTransmitData  transmitData = {0, 0, 0};

#define SPEED_LIMIT   500   // mm/s
#define ANGLE_LIMIT    45   // 0,01Â°

void remoteControl() {
  float speed;
  float steer;

  if (int err = espnow.getData(&receiveData, sizeof(RcReceiveData))) {
    if (err > 0) {
      speed = map(receiveData.speed, -1000, 1000, -SPEED_LIMIT, SPEED_LIMIT);
      steer = map(receiveData.steer, -1000, 1000, -ANGLE_LIMIT, ANGLE_LIMIT);
      if (receiveData.mode > 0) robot.mode = receiveData.mode;

      if (robot.scanPeriod == 0) {
        robot.status &= RUNNING;
        robot.updateSystem();
      }

      if (robot.mode == REMOTE_CONTROL) {
        if ((speed >= 0) && (robot.status & BUMPER_FRONT) ||
            (speed <= 0) && (robot.status & BUMPER_BACK)) {
          robot.stop(POWEROFF);
        } else {
          robot.go(speed, steer);
        }
      } 

      transmitData.status = (robot.status & EMERGENCY_STOP) == 0 ? robot.mode : 3;
      transmitData.voltage = robot.voltage;
      transmitData.current = robot.current;
      espnow.sendData(&transmitData, sizeof(TransmitData));
    } else if (err < 0) {
//      robot.mode = STOP; // Disconnected
    }  
//    Serial.printf("mode: %1d speed: %4.0f steer: %5.1f", receiveData.mode, speed, steer);
  }
}
#endif

void receiveCommand() {
  if (comm.process()) {                             // test prijatych dat
    if (comm.get() == PACKET_READY) {               // nacteni dat
      comm.confirm('A');
      switch (comm.rxData.command) {
        case 'M': robot.mode = comm.rxData.mode;
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
        case 'R': robot.reset();
                  break;
      }
      if (robot.mode == AUTONOMOUS) {
        switch (comm.rxData.command) {
          case 'S': robot.stop(comm.rxData.mode > POWEROFF ? DEFAULT_STOP : comm.rxData.mode);
                    break;
          case 'G': robot.go(comm.rxData.speed, comm.rxData.steer / 100.0f);
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

  #ifdef USE_ESP_NOW
    espnow.init();
  #endif
}

void loop() {
  #ifdef USE_ESP_NOW
    remoteControl();
  #endif

  // test prijatych dat z PC a jejich zpracovani
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
