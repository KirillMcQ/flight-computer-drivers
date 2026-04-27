#include "imu.h"
#include "../timer/timer.h"
#include "../i2c/i2c.h"

#define BMI088_GYRO_ADDR 0x68
#define BMI088_ACCEL_ADDR 0x18
#define BMI088_ACCEL_REGISTER_ACC_PWR_CTRL 0x7D
#define BMI088_ACCEL_REGISTER_ACC_CONF 0x40
#define BMI088_ACCEL_REGISTER_ACC_RANGE 0x41
#define BMI088_ACCEL_REGISTER_ACC_PWR_CONF 0x7C
#define BMI088_GYRO_REGISTER_GYRO_RANGE 0x0F
#define BMI088_GYRO_REGISTER_GYRO_BANDWIDTH 0x10
#define BMI088_GYRO_REGISTER_GYRO_LPM1 0x11

void initIMU()
{
  // Accel -> normal mode and enabled
  writeToRegister(BMI088_ACCEL_ADDR, BMI088_ACCEL_REGISTER_ACC_PWR_CTRL, 0x04);
  delayMS(50);

  writeToRegister(BMI088_ACCEL_ADDR, BMI088_ACCEL_REGISTER_ACC_PWR_CONF, 0x00);
  delayMS(50);

  // Accel -> 1600 Hz data rate
  writeToRegister(BMI088_ACCEL_ADDR, BMI088_ACCEL_REGISTER_ACC_CONF, (0x0A << 4) | 0x0C);

  // Accel -> +-24g range
  writeToRegister(BMI088_ACCEL_ADDR, BMI088_ACCEL_REGISTER_ACC_RANGE, 0x03);

  // Gyro -> 2000 range
  writeToRegister(BMI088_GYRO_ADDR, BMI088_GYRO_REGISTER_GYRO_RANGE, 0x00);

  // Gyro -> 2000 Hz data rate and 532 Hz filter bandwidth
  writeToRegister(BMI088_GYRO_ADDR, BMI088_GYRO_REGISTER_GYRO_BANDWIDTH, 0x00);

  // Gyro -> normal power mode
  writeToRegister(BMI088_GYRO_ADDR, BMI088_GYRO_REGISTER_GYRO_LPM1, 0x00);

  delayMS(50);
}

void readGyro(float *gyro)
{
  uint8_t gyroBuffer[6];
  burstReadFromRegister(BMI088_GYRO_ADDR, 0x02, 6, gyroBuffer);

  gyro[0] = (int16_t)((gyroBuffer[1] * 256) + gyroBuffer[0]) * (2000.0f / 32768.0f);
  gyro[1] = (int16_t)((gyroBuffer[3] * 256) + gyroBuffer[2]) * (2000.0f / 32768.0f);
  gyro[2] = (int16_t)((gyroBuffer[5] * 256) + gyroBuffer[4]) * (2000.0f / 32768.0f);
}

void readAccel(float *accel)
{
  uint8_t accelBuffer[6];
  burstReadFromRegister(BMI088_ACCEL_ADDR, 0x12, 6, accelBuffer);

  accel[0] = ((int16_t)((accelBuffer[1] * 256) + accelBuffer[0])) * (24.0f / 32768.0f);
  accel[1] = ((int16_t)((accelBuffer[3] * 256) + accelBuffer[2])) * (24.0f / 32768.0f);
  accel[2] = ((int16_t)((accelBuffer[5] * 256) + accelBuffer[4])) * (24.0f / 32768.0f);
}