#include <stdio.h>
#include "drivers/led/led.h"
#include "drivers/clocks/clocks.h"
#include "drivers/uart/uart.h"
#include "drivers/timer/timer.h"
#include "drivers/i2c/i2c.h"
#include "drivers/sensors/sensors.h"
#include "drivers/quaternion/quaternion.h"

#define RADIANS_TO_DEGREES 57.2957795131

int main()
{
  initClocks();
  initLED1();
  initTIM2();
  initMicros();
  initUART3();
  initI2C1();
  initIMU();
  initBMP390();
  calibrateLaunchPressure();
  initLIS2MDLTR();

  float accel[3];
  float gyro[3];
  float mag[3];

  uint32_t lastUs = micros();
  Quaternion q = {1, 0, 0, 0};

  while (1)
  {
    uint32_t curUs = micros();

    // 200 Hz
    if ((uint32_t)(curUs - lastUs) >= 5000U)
    {
      float dt = (uint32_t)(curUs - lastUs) * 0.000001f; // Seconds
      lastUs = curUs;

      readIMUAccel(accel);
      readIMUGyro(gyro);
      readLIS2MDLTRUt(mag);

      float ax = accel[0], ay = accel[1], az = accel[2];
      float gx = gyro[0], gy = gyro[1], gz = gyro[2];
      float altitude = readBMP390AltitudeM();
      float mx = mag[0], my = mag[1], mz = mag[2];

      Quaternion sw = {0, gyro[0], gyro[1], gyro[2]};
      Quaternion halfQ = multiplyByScalar(q, 0.5f);
      Quaternion qDot = product(halfQ, sw);
      q = add(q, multiplyByScalar(qDot, dt));
      q = normalize(q); // Orientation

      // Euler angle orientation
      float yaw = getEulerAngleYaw(q) * RADIANS_TO_DEGREES, pitch = getEulerAnglePitch(q) * RADIANS_TO_DEGREES, roll = getEulerAngleRoll(q) * RADIANS_TO_DEGREES;

      char dataStr[64];
      sprintf(dataStr, "Yaw: %.3f, Pitch: %.3f, Roll: %.3f (degrees)", yaw, pitch, roll);
      transmitString(dataStr);
    }
  };
}