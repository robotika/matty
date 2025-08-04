#include "I2C.h"

bool I2C::lock() {
  return xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE;
} 

void I2C::unlock() {
	xSemaphoreGive(mutex);
}
