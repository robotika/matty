#include "as5600.h"

volatile int32_t pulse;

bool cap_ISR_cb(mcpwm_unit_t mcpwm, mcpwm_capture_channel_id_t cap_channel, const cap_event_data_t *edata,void *user_data){ //this function need to be in that format to be recognized as cap_isr_cb_t type
  static volatile uint32_t pulseStart = 0;
  static volatile uint32_t pulseEnd = 0;
  static volatile uint32_t pulseWidth = 0;
  static volatile uint32_t pulsePeriod = 0;

  uint32_t x = (uint32_t) edata->cap_value; 
  mcpwm_capture_on_edge_t e = edata->cap_edge;

  if (e & MCPWM_POS_EDGE) {
    pulsePeriod = x - pulseStart;
    pulseStart = x;
    if (pulsePeriod > 0) {
      pulse = (pulseWidth * AS5600_PERIOD / pulsePeriod - AS5600_OFFS);
      if (pulse < 0) pulse = 0;
      if (pulse > AS5600_MAX) pulse = AS5600_MAX;
    }  
  }
  if (e & MCPWM_NEG_EDGE) {
    pulseEnd = x;
    pulseWidth = pulseEnd - pulseStart;
  }
  return 0; 
}

mcpwm_capture_config_t MCPWM_cap_config = { //Capture channel configuration
  .cap_edge = MCPWM_BOTH_EDGE,              /*!<Set capture edge*/
  .cap_prescale = 1,                        /*!<Prescale of capture signal, ranging from 1 to 256 */
  .capture_cb = cap_ISR_cb,                 /*!<User defined capture event callback, running under interrupt context */
  .user_data = nullptr,                     /*!<User defined ISR callback function args*/
};


AS5600::AS5600(float zero) {
  this->zero = zero;
}

void AS5600::init() {
  pinMode(AS5600_GPIO, INPUT);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, AS5600_GPIO); //set the cap input pin
  ESP_ERROR_CHECK(mcpwm_capture_enable_channel(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, &MCPWM_cap_config)); //enable and config capture channel. Inside an ESP_ERROR_CHECK() to avoid trouble
}

int16_t AS5600::width() {
  return pulse;
}

float AS5600::angle() {
  float y = pulse * (360.0f / 4096) - zero;
  if (y > 180.0f) y -= 360.0f;
  if (y < -180.0f) y += 360.0f;
  return y;
}  


