#ifndef ROBOT
#define ROBOT

#include "config.h"
#include "data.h"
#include "as5600.h"
#include "power.h"
#include "sts.h"

#define dT          (CONTROL_PERIOD * 1e-3f) // perioda rizeni 0.02 s
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
    float     x, y, a;  // aktualni pozice robota [mm, mm, rad]
    uint8_t   status;
    uint8_t   mode;
    int16_t   current;
    uint16_t  voltage;
    float     joint;            // uhel natoceni kloubu ve stupnich
    float     actualSpeed;      // rychlost pocitana z odometrie v mm/s
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

    void  updateEncoder(uint16_t p[SERVOS]);
    void  updateOdometry(uint32_t t);
    void  updatePower();
    void  control();  // vypocet kinematiky a rizeni pohonu
};

#endif
