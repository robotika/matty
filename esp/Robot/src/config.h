#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

//#define DEBUG

// ========================================================================
//    ROBOT
// ========================================================================

#define MATTY 01

#define ROBOT_TIMEOUT   250 // timeout do automatickeho zastaveni, pokud neprijde povel G
#define CONTROL_PERIOD   20 // perioda rizeni  20 ms (50 Hz)
#define SCAN_PERIOD       0 // perioda scanovani pro odometrii, default vypnuto

#define VOLTAGE_STOP_LIMIT    (int)(0.5 * 1000)  // 0.5 V ... emergency stop
#define VOLTAGE_LOW_LIMIT     (int)(10.5 * 1000) // 10.5 V

#if (MATTY == 01)
  #define ZERO_JOINT 178.59f  // nulova poloha kloubu Matty M01
  #define L              320  // rozvor
  #define A              311  // rozchod
  #define D              135  // prumer kol
#endif

#if (MATTY == 02)
  #define ZERO_JOINT 185.52f  // nulova poloha kloubu Matty M02 - Martin Dlouhy
  #define L              320  // rozvor
  #define A              315  // rozchod
  #define D              135  // prumer kol
#endif

#define ANGLE_MAX       45  // maximalni uhel natoceni kloubu
#define SPEED_MAX      700  // max rychlost [mm/s]
#define STEER_MAX       90  // max uhlova rychlost [st./s]

#define ENCODER       4096
#define LIMIT       (ENCODER/2) // limit pro overflow encoder
#define STEP        (D * PI / ENCODER)    // mm/step
#define LINE        (1 / STEP)            // steps/mm

// ========================================================================
//    SERVO
// ========================================================================

// the uart used to control servos.
// GPIO 18 - S_RXD, GPIO 19 - S_TXD, as default.
#define S_RXD 18
#define S_TXD 19
#define ServoPort   Serial1  /* UART1 */

// ========================================================================
//    ROBBUS
// ========================================================================

#define ESC               0x56      // escapovani znaku SYNC a ESC
#define ROBBUS_BAUDRATE 115200L 

#endif
