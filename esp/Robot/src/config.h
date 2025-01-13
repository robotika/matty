#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

//#define DEBUG

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
