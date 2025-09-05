#include "IMU.h"

I2C Wire2(0);

void IMU::imuTask(void *pvParameters) {
	static_cast<IMU*>(pvParameters)->update();
}

void IMU::init(bool noTask) {
	if (getId() == 0x05) {
    configSensors();
		if (!noTask) xTaskCreate(&IMU::imuTask, "Task IMU", 2048, (void*)this, 1, NULL);
  }
}

void IMU::update() {
	uint32_t lastTime ;
	for(;;) {
		uint8_t x = readData();
		if (x) {
			uint32_t t = micros();
			float dt = (t - lastTime) * 1.0e-6;
			MadgwickAHRSupdate(g, a, m, dt);
			if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
				Quaternion(q);
				xSemaphoreGive(mutex);
			}
			lastTime = t;
		} else {
			vTaskDelay(1 / portTICK_PERIOD_MS); // 1 ms
		}
	}
}

void IMU::eulerAngles(float &roll, float &pitch, float &yaw) {
	if (xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE) {
		float qt[4];
		qt[0] = q[0];
		qt[1] = q[1];
		qt[2] = q[2];
		qt[3] = q[3];
		xSemaphoreGive(mutex);
		EulerAngles(qt, roll, pitch, yaw);
	}
}

void IMU::enableAccGyro(uint8_t enableFlags) {
	writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl7, enableFlags);
	delay(1);
}

void IMU::configSensors() {
	enableAccGyro(QMI8658_DISABLE_ALL);  
	configAcc(Qmi8658AccRange_8g, Qmi8658AccOdr_250Hz);
  configGyro(Qmi8658GyrRange_512dps, Qmi8658GyrOdr_250Hz);
  configMag(AK09918_CONTINUOUS_100HZ);
//	configAcc(Qmi8658AccRange_8g, Qmi8658AccOdr_500Hz); // Obcas hazi chybne hodnoty, pri 250 Hz uz ne
//  configGyro(Qmi8658GyrRange_512dps, Qmi8658GyrOdr_500Hz);
	enableAccGyro(0x80 | QMI8658_ACCGYR_ENABLE);
}

uint8_t IMU::getId() {
 	uint8_t chipId = readRegister(QMI8658_ADDR, Qmi8658Register_WhoAmI);
  if (chipId != 0x05)	{
    Serial.printf("Error: Qmi8658Register_WhoAmI = 0x%x\r\n", chipId);
  } else {
    Serial.printf("OK: Qmi8658Register_WhoAmI = 0x%x\r\n", chipId);
//    qmi8658_on_demand_cali();

    writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl1, 0x60); // Address autoincrement, data big endian
    writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl7, 0x00); // Disable accelerometer and gyro
//    writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl8, 0xc0);
  }
	return chipId;
}

void IMU::qmi8658_on_demand_cali(void) {
	int16_t gyroGain[3];

	Serial.print("qmi8658_on_demand_cali start\r\n");
	writeRegister(QMI8658_ADDR, Qmi8658Register_Reset, 0xb0);
	delay(10);	
	writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl9, (unsigned char)qmi8658_Ctrl9_Cmd_On_Demand_Cali);
	delay(2200);	// delay 2000ms above
	writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl9, (unsigned char)qmi8658_Ctrl9_Cmd_NOP);
	delay(100);	
  uint8_t err = readRegister(QMI8658_ADDR, 70);
  readBytes(QMI8658_ADDR, Qmi8658Register_uuid, gyroGain, 6);
  Serial.printf("Gyro gain: %3d %5d, %5d, %5d\n\r", err, gyroGain[0], gyroGain[1], gyroGain[2]);
	Serial.print("qmi8658_on_demand_cali done\r\n");
}

void IMU::autoCalibrate() {
	#define SAMPLES	500
  Serial.println("Position your ICM20948 flat and don't move it - calibrating...");
  delay(1000);

  accBias[0] = 0;
  accBias[1] = 0;
  accBias[2] = 0;
	for(int i = 0; i < SAMPLES; i++) {
		readAccRaw();
		accBias[0] += accRaw[0];
		accBias[1] += accRaw[1];
		accBias[2] += accRaw[2];
		delay(10);
	}
	
	accBias[0] *= accScale / SAMPLES;
	accBias[1] *= accScale / SAMPLES;
	accBias[2] *= accScale / SAMPLES;
	accBias[2] -= 1000.0f; // - => + Opraveno
	
  gyroBias[0] = 0;
  gyroBias[1] = 0;
  gyroBias[2] = 0;
	for(int i = 0; i < SAMPLES; i++) {
		readGyroRaw();
		gyroBias[0] += gyroRaw[0];
		gyroBias[1] += gyroRaw[1];
		gyroBias[2] += gyroRaw[2];
		delay(2);
	}
	
	gyroBias[0] *= gyroScale / SAMPLES;
	gyroBias[1] *= gyroScale / SAMPLES;
	gyroBias[2] *= gyroScale / SAMPLES;

  Serial.printf("accBias:  %6.2f, %6.2f, %6.2f\n\r", accBias[0], accBias[1], accBias[2]);
  Serial.printf("gyroBias: %6.2f, %6.2f, %6.2f\n\r", gyroBias[0], gyroBias[1], gyroBias[2]);
}

// x = -y, y = x ... otoceni os o 90 st.
void IMU::readAccRaw() {
	readBytes(QMI8658_ADDR, Qmi8658Register_Ax_L, accRaw, 6);
  int16_t x = -accRaw[1];
  accRaw[1] = accRaw[0];
  accRaw[0] = x;
}

void IMU::readGyroRaw() {
	readBytes(QMI8658_ADDR, Qmi8658Register_Gx_L, gyroRaw, 6);
  int16_t x = -gyroRaw[1];
  gyroRaw[1] = gyroRaw[0];
  gyroRaw[0] = x;
}

void IMU::readMagRaw() {
	readBytes(AK09918_ADDR, AK09918_HXL, magRaw, 6);
  readRegister(AK09918_ADDR, AK09918_ST2);
  int16_t x = -magRaw[1];
  magRaw[1] = magRaw[0];
  magRaw[0] = x;
}

void IMU::readAcc() {
  readAccRaw();
  // Now we'll calculate the accleration value into actual g's
  // This depends on scale being set
  a[0] = (float)accRaw[0] * accScale - accBias[0];
  a[1] = (float)accRaw[1] * accScale - accBias[1];
  a[2] = (float)accRaw[2] * accScale - accBias[2];
}

void IMU::readGyro() {
  readGyroRaw();
  // Calculate the gyro value into actual degrees per second
  // This depends on scale being set
  g[0] = (float)gyroRaw[0] * gyroScale - gyroBias[0];
  g[1] = (float)gyroRaw[1] * gyroScale - gyroBias[1];
  g[2] = (float)gyroRaw[2] * gyroScale - gyroBias[2];
}

void IMU::readMag() {
  uint8_t x = readRegister(AK09918_ADDR, AK09918_ST1);
#ifndef CALIBRATION
	m[0] = 0.0f; m[1] = 0.0f; m[2] = 0.0f;
#endif
  if (x & AK09918_DRDY_BIT) {
    readMagRaw();
    float x = (float)magRaw[0] * magScale - magBias[0];
    float y = (float)magRaw[1] * magScale - magBias[1];
    float z = (float)magRaw[2] * magScale - magBias[2];
    m[0] = x * magCorect[0][0] + y * magCorect[0][1] + z * magCorect[0][2];
    m[1] = x * magCorect[1][0] + y * magCorect[1][1] + z * magCorect[1][2];
    m[2] = x * magCorect[2][0] + y * magCorect[2][1] + z * magCorect[2][2];

// Dle literatury m = 47 uT pro CR, ale nameril jsem cca 54 uT
		mg = sqrtf(m[0]*m[0] + m[1]*m[1] + m[2]*m[2]);
// zatim vyrazeno
//		if (mg > 60.0f || mg < 40.0f) {
//				m[0] = 0.0f; m[1] = 0.0f; m[2] = 0.0f;
//		}

  }
}

uint8_t IMU::readData() {
	uint8_t result = 0;
	if (Wire2.lock()) {
		uint8_t status = readRegister(QMI8658_ADDR, Qmi8658Register_Status0);
		if (status & 0x03 == 0x03) {
			readAcc();
			readGyro();
			readMag();
			result = 1;
		}
		Wire2.unlock();
	}
	return result;
}

void IMU::configAcc(enum qmi8658_AccRange range, enum qmi8658_AccOdr odr) {
	unsigned char ctl_dada;

	switch(range) {
		case Qmi8658AccRange_2g:
			accScale = 1000.0f / (1<<14);
			break;
		case Qmi8658AccRange_4g:
			accScale = 1000.0f / (1<<13);
			break;
		case Qmi8658AccRange_8g:
			accScale = -1000.0f / (1<<12); // ??? z neznameho duvodu je nutne otocit osy
			break;
		case Qmi8658AccRange_16g:
			accScale = 1000.0f / (1<<11);
			break;
		default:
			range = Qmi8658AccRange_8g;
			accScale = 1000.0f / (1<<12);
	}
  ctl_dada = (uint8_t)range | (uint8_t)odr;
	writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl2, ctl_dada);

	ctl_dada = readRegister(QMI8658_ADDR, Qmi8658Register_Ctrl5);
	ctl_dada &= 0xf0;
	ctl_dada &= ~0x01; // Disable low pass filter
	writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl5, ctl_dada);
}

void IMU::configGyro(enum qmi8658_GyrRange range, enum qmi8658_GyrOdr odr) {
	uint8_t ctl_dada;

	// Store the scale factor for use when processing raw data
	switch (range) {
		case Qmi8658GyrRange_16dps:
			gyroScale = 1.0f/2048;
			break;
		case Qmi8658GyrRange_32dps:
			gyroScale = 1.0f/1024;
			break;
		case Qmi8658GyrRange_64dps:
			gyroScale = 1.0f/512;
			break;
		case Qmi8658GyrRange_128dps:
			gyroScale = 1.0f/256;
			break;
		case Qmi8658GyrRange_256dps:
			gyroScale = 1.0f/128;
			break;
		case Qmi8658GyrRange_512dps:
			gyroScale = 1.0f/64;
			break;
		case Qmi8658GyrRange_1024dps:
			gyroScale = 1.0f/32;
			break;
		case Qmi8658GyrRange_2048dps:
			gyroScale = 1.0f/16;
			break;
		default:
			range = Qmi8658GyrRange_512dps;
			gyroScale = 1.0f/64;
			break;
	}

	ctl_dada = (uint8_t)range | (uint8_t)odr;
	writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl3, ctl_dada);

	ctl_dada = readRegister(QMI8658_ADDR, Qmi8658Register_Ctrl5);
	ctl_dada &= 0x0f;
	ctl_dada &= ~0x10; // Disable low pass filter
	writeRegister(QMI8658_ADDR, Qmi8658Register_Ctrl5, ctl_dada);
}

void IMU::configMag(enum AK09918_mode_type_t odr) {
	writeRegister(AK09918_ADDR, AK09918_CNTL3, AK09918_SRST_BIT);
	writeRegister(AK09918_ADDR, AK09918_CNTL2, odr);
  magScale = 0.15f;
}

void IMU::writeRegister(uint8_t addr, uint8_t reg, uint8_t value) {
  Wire2.beginTransmission(addr);
  Wire2.write(reg);
  Wire2.write(value);
  Wire2.endTransmission();
}

uint8_t IMU::readRegister(uint8_t addr, uint8_t reg) {
	Wire2.beginTransmission(addr);
	Wire2.write(reg);
	Wire2.endTransmission();
	Wire2.requestFrom(addr, (uint8_t)1);
	uint8_t ret = Wire2.read();
	Wire2.endTransmission();

	return ret;
}

void IMU::readBytes(uint8_t addr, uint8_t reg, void* data, uint8_t count) {
  Wire2.beginTransmission(addr);
  Wire2.write(reg);
  Wire2.endTransmission();
  Wire2.requestFrom(addr, count);
	for (int i = 0; i < count; i++) {
  	((uint8_t*)data)[i] = Wire2.read();
  }
  Wire2.endTransmission();
}



