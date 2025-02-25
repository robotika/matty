#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "platform.h"

//#define DEBUG

// ========================================================================
//    ROBOT
// ========================================================================

//#define MATTY 01

#define ROBOT_TIMEOUT   250 // timeout do automatickeho zastaveni, pokud neprijde povel G
#define CONTROL_PERIOD   20 // perioda rizeni  20 ms (50 Hz)
#define SCAN_PERIOD       0 // perioda scanovani pro odometrii, default vypnuto

#define VOLTAGE_STOP_LIMIT    (int)(0.5 * 1000)  // 0.5 V ... emergency stop
#define VOLTAGE_LOW_LIMIT     (int)(10.5 * 1000) // 10.5 V

#ifndef MATTY
  #error "Not defined MATTY platform"
#else
  #if (MATTY == 01)
    #define ZERO_JOINT 178.59f  // nulova poloha kloubu Matty M01
    #define L              320  // rozvor
    #define A              311  // rozchod
    #define D              135  // prumer kol
  #endif
  
  #if (MATTY == 02)
    #define ZERO_JOINT -178.15f  // nulova poloha kloubu Matty M02 - Martin Dlouhy
    #define L              320  // rozvor
    #define A              315  // rozchod
    #define D              135  // prumer kol
  #endif

  #if (MATTY == 03)
    #define ZERO_JOINT   0.00f  // nulova poloha kloubu Matty M03 - Martin Dlouhy
    #define L              320  // rozvor
    #define A              315  // rozchod
    #define D              135  // prumer kol
  #endif
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
#define SerialServo     Serial1  /* UART1 */
#define S_RXD           18
#define S_TXD           19
#define SERVO_BAUDRATE  1000000L 

// ========================================================================
//    ROBBUS
// ========================================================================

#define ESC               0x56      // escapovani znaku SYNC a ESC
#define ROBBUS_BAUDRATE 115200L 

// ========================================================================
//    GPS
// ========================================================================

#define SerialGPS     Serial2  /* UART2 */
#define GPS_RXD       16       // zelena / GPS16 / B_C2
#define GPS_TXD       27       // modra  / GPS17 / B_C1 
#define GPS_BAUDRATE  9600L 

#endif
