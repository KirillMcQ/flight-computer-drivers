#pragma once

typedef struct
{
  float q1;
  float q2;
  float q3;
  float q4;
} Quaternion;

Quaternion conjugate(Quaternion q);
Quaternion product(Quaternion a, Quaternion b);
Quaternion multiplyByScalar(Quaternion q, float scalar);
Quaternion add(Quaternion a, Quaternion b);
Quaternion normalize(Quaternion q);
float getEulerAngleRoll(Quaternion q);
float getEulerAnglePitch(Quaternion q);
float getEulerAngleYaw(Quaternion q);