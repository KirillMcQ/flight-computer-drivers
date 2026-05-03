#include <math.h>
#include "sensors.h"
#include "../timer/timer.h"
#include "../i2c/i2c.h"
#include "../uart/uart.h"
#include <stdio.h>

#define DEGS_TO_RADS 0.01745329251f
#define BMI088_GYRO_ADDR 0x68
#define BMI088_ACCEL_ADDR 0x18
#define BMP390_ADDR 0x76
#define LIS2MDLTR_ADDR 0x1E
#define BMI088_ACCEL_REGISTER_ACC_PWR_CTRL 0x7D
#define BMI088_ACCEL_REGISTER_ACC_CONF 0x40
#define BMI088_ACCEL_REGISTER_ACC_RANGE 0x41
#define BMI088_ACCEL_REGISTER_ACC_PWR_CONF 0x7C
#define BMI088_GYRO_REGISTER_GYRO_RANGE 0x0F
#define BMI088_GYRO_REGISTER_GYRO_BANDWIDTH 0x10
#define BMI088_GYRO_REGISTER_GYRO_LPM1 0x11
#define BMP390_STATUS_REGISTER 0x03
#define BMP390_PRESSURE_DATA_REGISTER 0x04
#define BMP390_PWR_CTRL_REGISTER 0x1B
#define BMP390_OSR_REGISTER 0x1C
#define BMP390_ODR_REGISTER 0x1D
#define BMP390_CONFIG_REGISTER 0x1F
#define BMP390_CALIBRATION_DATA_REGISTER 0x31
#define BMP390_CMD_REGISTER 0x7E
#define LIS2MDLTR_CFG_REG_A_REGISTER 0x60
#define LIS2MDLTR_CFG_REG_B_REGISTER 0x61
#define LIS2MDLTR_CFG_REG_C_REGISTER 0x62
#define LIS2MDLTR_OUTX_L_REG 0x68

static float gyroXBias = 0;
static float gyroYBias = 0;
static float gyroZBias = 0;

static void readBMP390Calibration();
static void readBMP390Raw(uint32_t *pressureRaw, uint32_t *tempRaw);

typedef struct
{
  double par_t1, par_t2, par_t3;
  double par_p1, par_p2, par_p3, par_p4, par_p5, par_p6;
  double par_p7, par_p8, par_p9, par_p10, par_p11;
  double t_lin;
} BMP390Calib;
static BMP390Calib bmp;
static float launchPressurePa = 101325.0f;

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

  // Gyro -> 2000 dps range
  writeToRegister(BMI088_GYRO_ADDR, BMI088_GYRO_REGISTER_GYRO_RANGE, 0x00);

  // Gyro -> 400 Hz data rate and 47 Hz filter bandwidth
  writeToRegister(BMI088_GYRO_ADDR, BMI088_GYRO_REGISTER_GYRO_BANDWIDTH, 0x03);

  // Gyro -> normal power mode
  writeToRegister(BMI088_GYRO_ADDR, BMI088_GYRO_REGISTER_GYRO_LPM1, 0x00);

  delayMS(50);
}

void initBMP390()
{
  // Soft reset
  writeToRegister(BMP390_ADDR, BMP390_CMD_REGISTER, 0xB6);
  delayMS(20);

  // Pressure x8 oversampling temperature x2 oversampling
  writeToRegister(BMP390_ADDR, BMP390_OSR_REGISTER, 11);
  delayMS(5);

  // 50 Hz ODR
  writeToRegister(BMP390_ADDR, BMP390_ODR_REGISTER, 0x02);
  delayMS(5);

  // IIR filter coefficient of 3
  writeToRegister(BMP390_ADDR, BMP390_CONFIG_REGISTER, 4);
  delayMS(5);

  // Enable pressure/temp but in sleep mode (will use forced mode)
  writeToRegister(BMP390_ADDR, BMP390_PWR_CTRL_REGISTER, 3);
  delayMS(5);

  readBMP390Calibration();

  delayMS(100);
}

void initLIS2MDLTR()
{
  // Continuous mode, 100 Hz data rate, high-resolution mode, normal mode, temperature compensation enabled
  writeToRegister(LIS2MDLTR_ADDR, LIS2MDLTR_CFG_REG_A_REGISTER, 0x8C);

  // Offset cancellation enabled
  writeToRegister(LIS2MDLTR_ADDR, LIS2MDLTR_CFG_REG_B_REGISTER, 0x02);

  // Block data update enabled
  writeToRegister(LIS2MDLTR_ADDR, LIS2MDLTR_CFG_REG_C_REGISTER, 0x10);
}

void readIMUGyro(float *gyro)
{
  uint8_t gyroBuffer[6];
  burstReadFromRegister(BMI088_GYRO_ADDR, 0x02, 6, gyroBuffer);

  gyro[0] = ((int16_t)((gyroBuffer[1] * 256) + gyroBuffer[0]) * (2000.0f / 32768.0f) * DEGS_TO_RADS) - gyroXBias;
  gyro[1] = ((int16_t)((gyroBuffer[3] * 256) + gyroBuffer[2]) * (2000.0f / 32768.0f) * DEGS_TO_RADS) - gyroYBias;
  gyro[2] = ((int16_t)((gyroBuffer[5] * 256) + gyroBuffer[4]) * (2000.0f / 32768.0f) * DEGS_TO_RADS) - gyroZBias;
}

void readIMUAccel(float *accel)
{
  uint8_t accelBuffer[6];
  burstReadFromRegister(BMI088_ACCEL_ADDR, 0x12, 6, accelBuffer);

  accel[0] = ((int16_t)((accelBuffer[1] * 256) + accelBuffer[0])) * (24.0f / 32768.0f);
  accel[1] = ((int16_t)((accelBuffer[3] * 256) + accelBuffer[2])) * (24.0f / 32768.0f);
  accel[2] = ((int16_t)((accelBuffer[5] * 256) + accelBuffer[4])) * (24.0f / 32768.0f);
}

static void readBMP390Calibration()
{
  uint8_t c[21];

  burstReadFromRegister(BMP390_ADDR, BMP390_CALIBRATION_DATA_REGISTER, 21, c);

  uint16_t nvm_par_t1 = ((uint16_t)c[1] << 8) | c[0];
  uint16_t nvm_par_t2 = ((uint16_t)c[3] << 8) | c[2];
  int8_t nvm_par_t3 = (int8_t)c[4];

  int16_t nvm_par_p1 = (int16_t)(((uint16_t)c[6] << 8) | c[5]);
  int16_t nvm_par_p2 = (int16_t)(((uint16_t)c[8] << 8) | c[7]);
  int8_t nvm_par_p3 = (int8_t)c[9];
  int8_t nvm_par_p4 = (int8_t)c[10];
  uint16_t nvm_par_p5 = ((uint16_t)c[12] << 8) | c[11];
  uint16_t nvm_par_p6 = ((uint16_t)c[14] << 8) | c[13];
  int8_t nvm_par_p7 = (int8_t)c[15];
  int8_t nvm_par_p8 = (int8_t)c[16];
  int16_t nvm_par_p9 = (int16_t)(((uint16_t)c[18] << 8) | c[17]);
  int8_t nvm_par_p10 = (int8_t)c[19];
  int8_t nvm_par_p11 = (int8_t)c[20];

  bmp.par_t1 = (double)nvm_par_t1 / 0.00390625;
  bmp.par_t2 = (double)nvm_par_t2 / 1073741824.0;
  bmp.par_t3 = (double)nvm_par_t3 / 281474976710656.0;

  bmp.par_p1 = ((double)nvm_par_p1 - 16384.0) / 1048576.0;
  bmp.par_p2 = ((double)nvm_par_p2 - 16384.0) / 536870912.0;
  bmp.par_p3 = (double)nvm_par_p3 / 4294967296.0;
  bmp.par_p4 = (double)nvm_par_p4 / 137438953472.0;
  bmp.par_p5 = (double)nvm_par_p5 / 0.125;
  bmp.par_p6 = (double)nvm_par_p6 / 64.0;
  bmp.par_p7 = (double)nvm_par_p7 / 256.0;
  bmp.par_p8 = (double)nvm_par_p8 / 32768.0;
  bmp.par_p9 = (double)nvm_par_p9 / 281474976710656.0;
  bmp.par_p10 = (double)nvm_par_p10 / 281474976710656.0;
  bmp.par_p11 = (double)nvm_par_p11 / 36893488147419103232.0;
}

static void readBMP390Raw(uint32_t *pressureRaw, uint32_t *tempRaw)
{
  uint8_t b[6];
  uint8_t status = 0;

  // Trigger one forced pressure + temperature measurement
  writeToRegister(BMP390_ADDR, BMP390_PWR_CTRL_REGISTER, 0x13);

  // Wait until pressure and temperature data are ready
  while ((status & 0x60) != 0x60)
  {
    burstReadFromRegister(BMP390_ADDR, BMP390_STATUS_REGISTER, 1, &status);
  }

  burstReadFromRegister(BMP390_ADDR, BMP390_PRESSURE_DATA_REGISTER, 6, b);

  *pressureRaw = ((uint32_t)b[2] << 16) | ((uint32_t)b[1] << 8) | b[0];
  *tempRaw = ((uint32_t)b[5] << 16) | ((uint32_t)b[4] << 8) | b[3];
}

void calibrateLaunchPressure()
{
  float sum = 0.0f;

  for (int i = 0; i < 100; i++)
  {
    sum += readBMP390PressurePa();
    delayMS(20);
  }

  launchPressurePa = sum / 100.0f;
}

void calibrateGyro()
{
  float gyro[3];

  float xB = 0.0f, yB = 0.0f, zB = 0.0f;

  for (int i = 0; i < 100; i++)
  {
    readIMUGyro(gyro);
    xB += gyro[0];
    yB += gyro[1];
    zB += gyro[2];
    transmitString("Calibrating Gyroscope");
    delayMS(25);
  }

  gyroXBias = xB / 100.0f;
  gyroYBias = yB / 100.0f;
  gyroZBias = zB / 100.0f;
}

float readBMP390PressurePa()
{
  uint32_t pressureRaw;
  uint32_t tempRaw;

  readBMP390Raw(&pressureRaw, &tempRaw);

  double temp = (double)tempRaw;
  double press = (double)pressureRaw;

  double partial1 = temp - bmp.par_t1;
  double partial2 = partial1 * bmp.par_t2;

  bmp.t_lin = partial2 + (partial1 * partial1) * bmp.par_t3;

  double t = bmp.t_lin;

  double out1 = bmp.par_p5 + bmp.par_p6 * t + bmp.par_p7 * t * t + bmp.par_p8 * t * t * t;

  double out2 = press * (bmp.par_p1 + bmp.par_p2 * t + bmp.par_p3 * t * t + bmp.par_p4 * t * t * t);

  double press2 = press * press;
  double press3 = press2 * press;

  double out3 = press2 * (bmp.par_p9 + bmp.par_p10 * t) + press3 * bmp.par_p11;

  double pressurePa = out1 + out2 + out3;

  return (float)pressurePa;
}

float readBMP390AltitudeM()
{
  float pressurePa = readBMP390PressurePa();

  float altitude = 44330.0f * (1.0f - powf(pressurePa / launchPressurePa, 0.19029495f));

  return altitude;
}

void readLIS2MDLTRUt(float *mag)
{
  uint8_t raw[6];

  burstReadFromRegister(LIS2MDLTR_ADDR, LIS2MDLTR_OUTX_L_REG, 6, raw);

  int16_t mx_raw = (int16_t)((raw[1] << 8) | raw[0]);
  int16_t my_raw = (int16_t)((raw[3] << 8) | raw[2]);
  int16_t mz_raw = (int16_t)((raw[5] << 8) | raw[4]);

  mag[0] = mx_raw * 0.15f;
  mag[1] = my_raw * 0.15f;
  mag[2] = mz_raw * 0.15f;
}