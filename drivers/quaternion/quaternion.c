#include <math.h>
#include "quaternion.h"

Quaternion conjugate(Quaternion q)
{
  Quaternion res = {q.q1, -q.q2, -q.q3, -q.q4};
  return res;
}

Quaternion product(Quaternion a, Quaternion b)
{
  Quaternion res;

  res.q1 = (a.q1 * b.q1) - (a.q2 * b.q2) - (a.q3 * b.q3) - (a.q4 * b.q4);
  res.q2 = (a.q1 * b.q2) + (a.q2 * b.q1) + (a.q3 * b.q4) - (a.q4 * b.q3);
  res.q3 = (a.q1 * b.q3) - (a.q2 * b.q4) + (a.q3 * b.q1) + (a.q4 * b.q2);
  res.q4 = (a.q1 * b.q4) + (a.q2 * b.q3) - (a.q3 * b.q2) + (a.q4 * b.q1);

  return res;
}

Quaternion multiplyByScalar(Quaternion q, float scalar)
{
  Quaternion res;
  res.q1 = scalar * q.q1;
  res.q2 = scalar * q.q2;
  res.q3 = scalar * q.q3;
  res.q4 = scalar * q.q4;

  return res;
}

Quaternion add(Quaternion a, Quaternion b)
{
  Quaternion res = {a.q1 + b.q1, a.q2 + b.q2, a.q3 + b.q3, a.q4 + b.q4};
  return res;
}

Quaternion normalize(Quaternion q)
{
  float norm = sqrtf((q.q1 * q.q1) + (q.q2 * q.q2) + (q.q3 * q.q3) + (q.q4 * q.q4));

  if (norm == 0)
  {
    Quaternion identity = {1, 0, 0, 0};
    return identity;
  }

  float invNorm = 1 / norm;

  Quaternion res = {q.q1 * invNorm, q.q2 * invNorm, q.q3 * invNorm, q.q4 * invNorm};
  return res;
}

float getEulerAngleRoll(Quaternion q)
{
  return atan2f((2 * q.q3 * q.q4) - (2 * q.q1 * q.q2), (2 * q.q1 * q.q1) + (2 * q.q4 * q.q4) - 1);
}

float getEulerAnglePitch(Quaternion q)
{
  return -asinf((2 * q.q2 * q.q4) + (2 * q.q1 * q.q3));
}

float getEulerAngleYaw(Quaternion q)
{
  return atan2f((2 * q.q2 * q.q3) - (2 * q.q1 * q.q4), (2 * q.q1 * q.q1) + (2 * q.q2 * q.q2) - 1);
}