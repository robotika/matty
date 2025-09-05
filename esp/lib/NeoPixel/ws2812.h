#ifndef WS2812_H
#define WS2812_H

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "driver/rmt.h"
#include "esp_check.h"

/* ===============================
WS2811: (2.5us bit time, 400Kbps)
    T0H: 0.5us <-- 0 bit
    T0L: 2.0us
    T1H: 1.2us <-- 1 bit
    T1L: 1.3us
    RES: 50us

WS2812: (1.25us bit time, 800Kbps)
    T0H: 0.35us <-- 0 bit
    T0L: 0.8us
    T1H: 0.7us <-- 1 bit
    T1L: 0.6us
    RES: 50us

WS2812b: (1.25us bit time, 800Kbps)
    T0H: 0.4us <-- 0 bit
    T0L: 0.85us
    T1H: 0.8us <-- 1 bit
    T1L: 0.45us
    RES: 50us

To calculate values for these configuration times, 
multiply the desired time in micro seconds (μs or us in data sheets) 
by the default clock rate in MHz and divide by 2. E.g., for 280ns (0.28μs) 
on an ESP32 with an 80MHz clock: 0.28 * 80 / 2 = 11.2 => 11 (rounded when needed)
=================================== */

#define NUM_LEDS	2

// Configure these based on your project needs using menuconfig ********
#define LED_RMT_TX_CHANNEL	(rmt_channel_t)0
#define LED_RMT_TX_GPIO		(gpio_num_t)4
// ****************************************************

#define BITS_PER_LED_CMD	24
#define LED_BUFFER_ITEMS	(NUM_LEDS * BITS_PER_LED_CMD)

/*
// These values are determined by measuring pulse timing with logic analyzer and adjusting to match datasheet. 
#define T0H	20  // 0 bit high time WS2811
#define T0L	80  // low time for either bit
#define T1H	48  // 1 bit high time
#define T1L	52
#define TE  2000
*/

// These values are determined by measuring pulse timing with logic analyzer and adjusting to match datasheet. 
#define T0H	14  // 0 bit high time WS2812
#define T0L	32  // low time for either bit
#define T1H	28  // 1 bit high time
#define T1L	24
#define TE  2000

enum COLOR { // BRG
  RED   = 0x00003F,
  GREEN = 0x003F00,
  BLUE  = 0x3F0000
};

class Neopixel {
  public:
    Neopixel();
    void  led(uint8_t index, uint32_t rgb);
    void  clear();

    // Setup the hardware peripheral. Only call this once.
    esp_err_t init(void);

  private:
    // This structure is used for indicating what the colors of each LED should be set to.
    // There is a 32bit value for each LED. Only the lower 3 bytes are used and they hold the
    // Red (byte 2), Green (byte 1), and Blue (byte 0) values to be set.
    uint32_t leds[NUM_LEDS];

    // Tag for log messages
    static constexpr const char *TAG = "NeoPixel WS2812 Driver";

    // This is the buffer which the hw peripheral will access while pulsing the output pin
    rmt_item32_t led_data_buffer[LED_BUFFER_ITEMS+1];

    // Update the LEDs to the new state. Call as needed.
    // This function will block the current task until the RMT peripheral is finished sending 
    // the entire sequence.
    esp_err_t write();

    void setup_rmt_data_buffer(uint32_t* leds);
};

#endif

