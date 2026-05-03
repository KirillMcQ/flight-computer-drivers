#pragma once
#include <stdint.h>

void initIMU();
void initBMP390();
void initLIS2MDLTR();
void readIMUGyro(float *gyro); // RAD/s
void readIMUAccel(float *accel);
void calibrateLaunchPressure();
void calibrateGyro(); // Should be called after initialization
float readBMP390PressurePa();
float readBMP390AltitudeM();
void readLIS2MDLTRUt(float *mag);