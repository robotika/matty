#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "platform.h"

//#define DEBUG

// ========================================================================
//    ROBOT
// ========================================================================

#define ROBOT_FW_VERSION  8 // please increase with every change of Robot project (or common library)

#define ROBOT_TIMEOUT   250 // timeout do automatickeho zastaveni, pokud neprijde povel G
#define CONTROL_PERIOD   20 // perioda rizeni  20 ms (50 Hz)
#define SCAN_PERIOD       0 // perioda scanovani pro odometrii, default vypnuto

#define VOLTAGE_STOP_LIMIT    (int)(2.5 * 1000)  // 2.5 V ... emergency stop
#define VOLTAGE_LOW_LIMIT     (int)(10.5 * 1000) // 10.5 V

#ifndef MATTY
  #error "Not defined MATTY platform"
#else

  #if (MATTY == 00) // testovaci board ESP32
    #define ZERO_JOINT 180.00f  // nulova poloha kloubu
    #define L              320  // rozvor
    #define A              315  // rozchod
    #define D              135  // prumer kol

    // IMU kalibrace
    #define ACC_BIAS    {-0.60, -58.37, -13.21}
    #define GYRO_BIAS   {-2.15, 0.25, -0.16}
    #define MAG_BIAS    {3.99, 9.87, 6.42}
    #define MAG_CORECT  {{0.999, 0.008, 0.002}, {0.008, 1.027, 0.009}, {0.002, 0.009, 0.974}}
  #endif

  #if (MATTY == 01) // Matty M01 - modry
    #define ZERO_JOINT 178.59f  // nulova poloha kloubu Matty M01
    #define L              320  // rozvor
    #define A              311  // rozchod
    #define D              135  // prumer kol

    // IMU kalibrace
    #define ACC_BIAS    {115.55, -48.88,   8.52}
    #define GYRO_BIAS   {-2.23,   1.45,  -0.30}
    #define MAG_BIAS    {22.27, -67.19, 1.94}
    #define MAG_CORECT  {{1.050, -0.006, -0.022}, {-0.006, 1.016, 0.019}, {-0.022, 0.019, 0.939}}

    #define USE_ESP_NOW
  #endif
  
  #if (MATTY == 02) // Matty M02 - zeleny
    #define ZERO_JOINT -178.15f // nulova poloha kloubu Matty M02 - Martin Dlouhy
    #define L              320  // rozvor
    #define A              315  // rozchod
    #define D              135  // prumer kol

    // IMU kalibrace
    #define ACC_BIAS    {0, 0, 0}
    #define GYRO_BIAS   {0, 0, 0}
    #define MAG_BIAS    {0, 0, 0}
    #define MAG_CORECT  {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}
  #endif

  #if (MATTY == 03) // Matty M03 - cerveny
    #define ZERO_JOINT   0.00f  // nulova poloha kloubu Matty M03 - Martin Dlouhy
    #define L              320  // rozvor
    #define A              315  // rozchod
    #define D              135  // prumer kol

    // IMU kalibrace
    #define ACC_BIAS    {0, 0, 0}
    #define GYRO_BIAS   {0, 0, 0}
    #define MAG_BIAS    {0, 0, 0}
    #define MAG_CORECT  {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}
  #endif

  #if (MATTY == 04) // Matty M04 - ruzovy
    #define ZERO_JOINT   0.00f  // nulova poloha kloubu Matty M04 - Martin Dlouhy
    #define L              320  // rozvor
    #define A              315  // rozchod
    #define D              135  // prumer kol

    // IMU kalibrace
    #define ACC_BIAS    {0, 0, 0}
    #define GYRO_BIAS   {0, 0, 0}
    #define MAG_BIAS    {0, 0, 0}
    #define MAG_CORECT  {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}
  #endif

  #if (MATTY == 05) // Matty M05 - oranzovy
    #define ZERO_JOINT   0.00f  // nulova poloha kloubu Matty M05 - Martin Dlouhy
    #define L              320  // rozvor
    #define A              315  // rozchod
    #define D              135  // prumer kol

    // IMU kalibrace
    #define ACC_BIAS    {0, 0, 0}
    #define GYRO_BIAS   {0, 0, 0}
    #define MAG_BIAS    {0, 0, 0}
    #define MAG_CORECT  {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}
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
//    SERVO ST
// ========================================================================

// the uart used to control servos
#define SerialServo     Serial1  /* UART1 */
#define S_RXD           18
#define S_TXD           19
#define SERVO_BAUDRATE  1000000L 

// ========================================================================
//    ROBBUS
// ========================================================================

#define SerialComm      Serial   /* UART0 */
#define ESC               0x56   // escapovani znaku SYNC a ESC
#define ROBBUS_BAUDRATE 115200L 

// ========================================================================
//    GPS
// ========================================================================

#define SerialGPS     Serial2  /* UART2 */
#define GPS_RXD       16       // zelena / GPIO16 / B_C2
#define GPS_TXD       27       // modra  / GPIO17 / B_C1 
#define GPS_BAUDRATE  9600L 

// ========================================================================
//    SERVO
// ========================================================================

//#define SERVO_PIN     4        // servo GPIO4

// ========================================================================
//    NEOPIXEL LED
// ========================================================================

#define LED_PIN     4        // servo GPIO4

#endif
