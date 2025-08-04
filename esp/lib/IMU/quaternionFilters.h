#ifndef QUATERNIONFILTERS_H_
#define QUATERNIONFILTERS_H_

#include <Arduino.h>

void Quaternion(float q[4]);
void EulerAngles(float q[4], float &roll, float &pitch, float &yaw);
void MadgwickAHRSupdate(float g[3], float a[3], float m[3], float dT);
void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz, float dT);
void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az, float dT);

#endif // _QUATERNIONFILTERS_H_
