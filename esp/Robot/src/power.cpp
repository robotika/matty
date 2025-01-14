#include "power.h"

Power::Power() : INA219_WE(INA219_ADDRESS) {
}

bool Power::init() {
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!INA219_WE::init()) {
    return 0;
  }
  setADCMode(BIT_MODE_9);
  setPGain(PG_80);
  setBusRange(BRNG_16);
  setShuntSizeInOhms(0.01); // used in INA219.
  setADCMode(SAMPLE_MODE_8);
  return 1;
}
int Power::getVoltage() {
  return getBusVoltage_V() * 1000;
}

int Power::getCurrent() {
  return getCurrent_mA();
}
