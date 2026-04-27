#pragma once
#include <stdint.h>

void initIMU();
void readGyro(float *gyro);
void readAccel(float *accel);