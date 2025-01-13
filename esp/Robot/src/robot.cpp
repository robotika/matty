#include "robot.h"

uint8_t idServo[SERVOS] = {1, 2, 3, 4};

AS5600 as5600(ZERO_JOINT);  // zero joint position
Power power;

Robot::Robot(Stream* stSerial) : Sts(stSerial) {
}

void Robot::init() {
  as5600.init();
  power.init();
  stop();
  process();
  reset();
}

void Robot::reset() {
  encoder[0] = 0;
  encoder[1] = 0;
  encoder[2] = 0;
  encoder[3] = 0;  
  x = 0;
  y = 0;
  a = PI/2;
}

bool  Robot::go(float speed, float steer) {
  if (speed >  v_max) speed =  v_max; // omezeni rychlosti
  if (speed < -v_max) speed = -v_max;
  if (speed > SPEED_MAX || speed < -SPEED_MAX) return 0;
  if (steer > ANGLE_MAX || steer < -ANGLE_MAX) return 0;
  this->speed = speed;
  this->steer = steer;
  timeGo = millis();
  return 1;
}

void  Robot::stop() {
  speed = 0;
  steer = as5600.angle() / 100.0f;
}

void  Robot::control() {
  joint = as5600.angle();                // aktualni uhel kloubu ve °
  float fi = joint * PI / 180;           // uhel kloubu v rad
  float ro = 0;
  float dr = 0;
  float s  = L/2;
  if (abs(fi) > 0.01f) {              // uhel kloubu musi byt vetsi nez 0.57°
    ro = L / (2 * tanf(fi/2));
    dr = A / (2 * ro);
    s  = ro * fi / 2;
  }
  // BRAKE_GAIN 10 ... odpovida doba brzdne rampy 0,1s (pasmo proporcionality])
  #define BRAKE_GAIN  10
  float dfi = (steer - joint) * BRAKE_GAIN * dT;  // pozadovana zmena natoceni kloubu ve °
  if (dfi > a_max * dT) {         // omezeni maximalni rychlosti nataceni kloubu
    dfi = a_max * dT;
  }
  if (dfi < -a_max * dT) {        // omezeni maximalni rychlosti nataceni kloubu
    dfi = -a_max * dT;
  }
  dfi *= PI / 180;                    // pozadovana zmena polohy kloubu v rad

  float fi1 = fi + dfi;               // cilova poloha kloubu v rad
  float ro1 = 0;
  float s1  = L/2;
  if (abs(fi1) > 0.01f) {             // uhel kloubu musi byt vetsi nez 0.57°
    ro1 = L / (2 * tanf(fi1/2));
    s1  = ro1 * fi1 / 2;
  }
  float dv = (s1 - s) / dT;           // rychlost "vzdalovani" naprav

  float dw = dfi * A / (4 * dT);      // rychlost otaceni napravy

  float vl = speed * (1 - dr);        // dopredna rychlost levych kol
  float vr = speed * (1 + dr);        // dopredna rychlost pravych kol

// TODO dodelat omezeni maximalni rychlosti

//  Serial.printf("steer: %5.1f f0: %5.1f dfi: %5.2f ro: %5.1f ro1: %5.1f s: %5.1f s1: %5.1f dv: %5.1f dw: %5.1f\n\r", steer, f0, dfi * 180 / PI, ro, ro1, s, s1, dv, dw);

  int16_t p[SERVOS];  
  p[0] = -(vl + dv - dw)  * LINE;
  p[1] = (vr + dv + dw) * LINE;
  p[2] = -(vl - dv + dw) * LINE;
  p[3] = (vr - dv - dw) * LINE;

  writeSyncSpeed(SERVOS, idServo, p);
}

void Robot::updateEncoder(uint16_t p[SERVOS]) {
  for (int i = 0; i < SERVOS; i++) {
    delta_enc[i] = p[i] - enc[i];
    if (i % 2 == 0) delta_enc[i] = -delta_enc[i]; // leva kola opacne
    enc[i] = p[i];
    if (delta_enc[i] >  LIMIT) delta_enc[i] -= ENCODER;
    if (delta_enc[i] < -LIMIT) delta_enc[i] += ENCODER;
    encoder[i] += delta_enc[i];
  }
}

void Robot::updateOdometry(uint32_t t) {
  actualSpeed = STEP * (delta_enc[0] + delta_enc[1] + delta_enc[2] + delta_enc[3]) / SERVOS * 1000.0f / t;
}

bool Robot::process() {
  static uint32_t timeControl;   // cas posleniho rizeni
  uint32_t t = millis();
  if ((int32_t)t - (int32_t)timeGo >= (int32_t)robotTimeout) {
    stop();
    timeGo = t + 36000000;
  }
  if (t - timeControl >= CONTROL_PERIOD) {
    timeControl = t;
    control();
  }

  static uint32_t timeScan;   // cas posleniho scanu, odometrie
  if (scanPeriod != 0 && t - timeScan >= scanPeriod) {
    uint16_t p[SERVOS];
    if (readSyncPosition(SERVOS, idServo, p) == -1) {
      return 0; // chyba cteni
    }
    updateEncoder(p);
    updatePower();
    updateOdometry(t - timeScan);
    timeScan = t;
    return 1;
  }
  return 0;
}

void Robot::updatePower() {
  uint16_t currentRaw[SERVOS];
  int16_t result = bulkRead(SERVOS, idServo, SMS_STS_PRESENT_CURRENT_L, (uint8_t*)currentRaw, 2);
  if (result != 0) {
    current = 0;
  } else {
    current = (currentRaw[0] + currentRaw[1] + currentRaw[2] + currentRaw[3]) * 10.0f;
  }
  voltage = power.getBusVoltage_V() * 1000.0F;
}

void Robot::setTime(uint16_t period, uint16_t t) { // nastavi periodu pro cteni odomerie a odesilani dat, timeout pro automaticke zastaveni, pokud neprijde novy povel G
  scanPeriod = period;
  robotTimeout = t;
}

void Robot::setLimits(uint16_t maxSpeed, uint16_t maxAngleSpeed) { // nastavi maximalni rychlosti
  if (maxSpeed <= SPEED_MAX) v_max = maxSpeed;
  if (maxAngleSpeed <= STEER_MAX) a_max = maxAngleSpeed;
}
