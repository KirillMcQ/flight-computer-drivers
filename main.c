#include <stdio.h>
#include "drivers/led/led.h"
#include "drivers/clocks/clocks.h"
#include "drivers/uart/uart.h"
#include "drivers/timer/timer.h"
#include "drivers/i2c/i2c.h"
#include "drivers/sensors/sensors.h"

int main()
{
  initClocks();
  initLED1();
  initTIM2();
  initUART3();
  initI2C1();
  initIMU();
  initBMP390();
  calibrateLaunchPressure();

  float accel[3];
  float gyro[3];

  while (1)
  {
    readIMUAccel(accel);
    readIMUGyro(gyro);

    float ax = accel[0], ay = accel[1], az = accel[2];
    float gx = gyro[0], gy = gyro[1], gz = gyro[2];
    float altitude = readBMP390AltitudeM();

    char dataStr[64];
    sprintf(dataStr, "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", ax, ay, az, gx, gy, gz, altitude);

    transmitString(dataStr);
    delayMS(25);
  };
}