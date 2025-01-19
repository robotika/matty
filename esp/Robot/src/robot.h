#ifndef ROBOT
#define ROBOT

#include "sts.h"

#include "config.h"
#include "as5600.h"
#include "power.h"

#define ROBOT_TIMEOUT   250 // timeout do automatickeho zastaveni, pokud neprijde povel G
#define CONTROL_PERIOD   20 // perioda rizeni  20 ms (50 Hz)
#define SCAN_PERIOD       0 // perioda scanovani pro odometrii, default vypnuto
#define dT              (CONTROL_PERIOD * 1e-3f) // perioda rizeni 0.02 s

#define ZERO_JOINT 185.52f  // nulova poloha kloubu Matty M02 - Martin Dlouhy
//#define ZERO_JOINT  178.3f  // nulova poloha kloubu Matty M01
#define ANGLE_MAX       45  // maximalni uhel natoceni kloubu
#define SPEED_MAX      700  // max rychlost [mm/s]
#define STEER_MAX       90  // max uhlova rychlost [st./s]
#define L              320  // rozvor
#define A              311  // rozchod
#define D              135  // prumer kol
#define ENCODER       4096
#define LIMIT       (ENCODER/2) // limit pro overflow encoder

#define STEP        (D * PI / ENCODER)    // mm/step
#define LINE        (1 / STEP)            // steps/mm

#define SERVOS      4

class Robot : public Sts {
  public:
    Robot(Stream*);
    void  init();
    bool  go(float speed, float steer); // rychlost v mm/s, uhel ve stupnich
    void  stop(bool off = 0);     // nastavi rychlost robota na 0, uhel kloubu zachova / vypne pohony
    bool  process();  // periodicky resi rizeni a cte pozici, pocita odometrii
    void  reset();
    void  setTime(uint16_t p, uint16_t t); // nastavi periodu pro odomerii a odesilani dat, timeout pro automaticke zastaveni, pokud neprijde novy povel G
    void  setLimits(uint16_t maxSpeed, uint16_t maxAngleSpeed); // nastavi maximalni rychlosti
    float x, y, a;  // aktualni pozice roobta [mm, mm, rad]
    uint16_t  current;
    uint16_t  voltage;
    int16_t   joint; // uhel natoceni kloubu
    float     actualSpeed;       // rychlost pocitana z odometrie
    int32_t   encoder[SERVOS];
  private:
    float speed;  // pozadovana rychlost
    float steer;  // pozadovany uhel kloubu
    float v_max = SPEED_MAX; // maximalni rychlost [mm/s] 
    float a_max = STEER_MAX; // maximalni rychlost otaceni [°/s]
    int16_t   enc[SERVOS];
    int32_t   delta_enc[SERVOS];
    uint32_t  robotTimeout = ROBOT_TIMEOUT;
    uint32_t  timeGo;
    uint32_t  scanPeriod = SCAN_PERIOD;
    bool  running = 0;

    void  updateEncoder(uint16_t p[SERVOS]);
    void  updateOdometry(uint32_t t);
    void  updatePower();
    void  control();  // vypocet kinematiky a rizeni pohonu
};

#endif
