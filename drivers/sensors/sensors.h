#pragma once
#include <stdint.h>

void initIMU();
void initBMP390();
void readIMUGyro(float *gyro);
void readIMUAccel(float *accel);
void calibrateLaunchPressure();
float readBMP390PressurePa();
float readBMP390AltitudeM();