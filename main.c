#include "drivers/led/led.h"
#include "drivers/clocks/clocks.h"
#include "drivers/uart/uart.h"
#include "drivers/timer/timer.h"
#include "drivers/i2c/i2c.h"
#include "drivers/imu/imu.h"
#include <stdio.h>

int main()
{
  initClocks();
  initLED1();
  initTIM2();
  initUART3();
  initI2C1();
  initIMU();

  float accel[3];
  float gyro[3];

  while (1)
  {
    readAccel(accel);
    readGyro(gyro);

    float ax = accel[0], ay = accel[1], az = accel[2];
    float gx = gyro[0], gy = gyro[1], gz = gyro[2];

    char axStr[64];
    sprintf(axStr, "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", ax, ay, az, gx, gy, gz);

    transmitString(axStr);
    delayMS(50);
  };
}