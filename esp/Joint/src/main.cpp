#include <Arduino.h>
#include "as5600.h"

AS5600 as5600(185.52f);

/*
  Conection joint encoder - esp board
  black   - GND
  red     - 3V3
  white   - IO5 
*/

/*
bool cap_ISR_cb(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata,void *user_data){ //this function need to be in that format to be recognized as cap_isr_cb_t type
  uint32_t x = (uint32_t) edata->cap_value; //same as mcpwm_capture_signal_get_value()
  mcpwm_capture_on_edge_t e = edata->cap_edge;
  if (e & MCPWM_POS_EDGE) {
  pulseStart = x;
  }
  if (e & MCPWM_NEG_EDGE) {
    pulseEnd = x;
    pulseWidth = pulseEnd - pulseStart;
  }
  return 0; //Whether a task switch is needed after the callback function returns, this is usually due to the callback wakes up some high priority task.
}
*/

/**
 * @brief Initialize capture submodule
 *
 * @param mcpwm_num set MCPWM unit(0-1)
 * @param cap_edge set capture edge, BIT(0) - negative edge, BIT(1) - positive edge
 * @param cap_sig capture pin, which needs to be enabled
 * @param num_of_pulse count time between rising/falling edge between 2 *(pulses mentioned), counter uses APB_CLK
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */

//esp_err_t mcpwm_capture_enable(mcpwm_unit_t mcpwm_num, mcpwm_capture_signal_t cap_sig, mcpwm_capture_on_edge_t cap_edge, uint32_t num_of_pulse); 

void setup() {
  Serial.begin(115200);
  as5600.init();
}

void loop() {
  int x = pulseIn(AS5600_GPIO, 1);
  int z = as5600.angle();
  Serial.printf("Width: %5d Angle: %3d.%02d\n\r", x, z/100, abs(z%100));
  delay(100);
}