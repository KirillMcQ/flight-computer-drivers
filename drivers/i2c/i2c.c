#include "i2c.h"

#define I2C1_ADDR 0x40005400
#define GPIOB_ADDR 0x40020400
#define GPIOB_MODER *((volatile uint32_t *)(GPIOB_ADDR))
#define GPIOB_OTYPER *((volatile uint32_t *)(GPIOB_ADDR + 0x04))
#define GPIOB_OSPEEDR *((volatile uint32_t *)(GPIOB_ADDR + 0x08))
#define GPIOB_PUPDR *((volatile uint32_t *)(GPIOB_ADDR + 0x0C))
#define GPIOB_AFRL *((volatile uint32_t *)(GPIOB_ADDR + 0x20))
#define I2C1_CR1 *((volatile uint32_t *)(I2C1_ADDR))
#define I2C1_CR2 *((volatile uint32_t *)(I2C1_ADDR + 0x04))
#define I2C1_DR *((volatile uint32_t *)(I2C1_ADDR + 0x10))
#define I2C1_SR1 *((volatile uint32_t *)(I2C1_ADDR + 0x14))
#define I2C1_SR2 *((volatile uint32_t *)(I2C1_ADDR + 0x18))
#define I2C1_CCR *((volatile uint32_t *)(I2C1_ADDR + 0x1C))
#define I2C1_TRISE *((volatile uint32_t *)(I2C1_ADDR + 0x20))

void initI2C1()
{
  I2C1_CR1 &= ~(1 << 0);
  GPIOB_MODER &= ~(1 << 12);
  GPIOB_MODER |= (1 << 13);
  GPIOB_MODER &= ~(1 << 14);
  GPIOB_MODER |= (1 << 15);

  GPIOB_OTYPER |= (1 << 6);
  GPIOB_OTYPER |= (1 << 7);

  GPIOB_OSPEEDR &= ~(1 << 12);
  GPIOB_OSPEEDR |= (1 << 13);
  GPIOB_OSPEEDR &= ~(1 << 14);
  GPIOB_OSPEEDR |= (1 << 15);

  GPIOB_PUPDR |= (1 << 12);
  GPIOB_PUPDR &= ~(1 << 13);
  GPIOB_PUPDR |= (1 << 14);
  GPIOB_PUPDR &= ~(1 << 15);

  GPIOB_AFRL &= ~(1 << 24);
  GPIOB_AFRL &= ~(1 << 25);
  GPIOB_AFRL |= (1 << 26);
  GPIOB_AFRL &= ~(1 << 27);
  GPIOB_AFRL &= ~(1 << 28);
  GPIOB_AFRL &= ~(1 << 29);
  GPIOB_AFRL |= (1 << 30);
  GPIOB_AFRL &= ~(1 << 31);

  // 16 MhZ APB1 Clock
  I2C1_CR2 = 16;

  // 100 kHz SCL Clock Speed
  I2C1_CCR = 80;

  I2C1_TRISE = 17;

  I2C1_CR1 |= (1 << 0);
}

void __attribute__((optimize("O0"))) writeToRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t data)
{
  // Generate start condition
  I2C1_CR1 |= (1 << 8);
  while (!(I2C1_SR1 & (1 << 0)))
    ;

  // Transmit slave address
  I2C1_DR = (deviceAddr << 1);
  while (!(I2C1_SR1 & (1 << 1)))
    ;

  (void)I2C1_SR1;
  (void)I2C1_SR2;

  // Send register address
  while (!(I2C1_SR1 & (1 << 7)))
    ;
  I2C1_DR = regAddr;

  // Send data
  while (!(I2C1_SR1 & (1 << 7)))
    ;
  I2C1_DR = data;
  while (!(I2C1_SR1 & (1 << 2)))
    ;

  // Stop transmission
  I2C1_CR1 |= (1 << 9);
}

void __attribute__((optimize("O0"))) burstReadFromRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t n, uint8_t *buffer)
{
  // Enable Ack
  I2C1_CR1 |= (1 << 10);

  // Generate start condition
  I2C1_CR1 |= (1 << 8);
  while (!(I2C1_SR1 & (1 << 0)))
    ;

  // Transmit slave address
  I2C1_DR = (deviceAddr << 1);
  while (!(I2C1_SR1 & (1 << 1)))
    ;

  (void)I2C1_SR1;
  (void)I2C1_SR2;

  // Send register address
  while (!(I2C1_SR1 & (1 << 7)))
    ;
  I2C1_DR = regAddr;
  while (!(I2C1_SR1 & (1 << 2)))
    ;

  // Generate repeated start condition
  I2C1_CR1 |= (1 << 8);
  while (!(I2C1_SR1 & (1 << 0)))
    ;

  // Transmit slave address again but with RW bit set to 1 (read)
  I2C1_DR = (deviceAddr << 1) | 1;
  while (!(I2C1_SR1 & (1 << 1)))
    ;

  (void)I2C1_SR1;
  (void)I2C1_SR2;

  // Read bytes
  for (int i = 0; i < n; i++)
  {
    if (i == n - 1)
    {
      I2C1_CR1 &= ~(1 << 10);
      I2C1_CR1 |= (1 << 9);
    }

    while (!(I2C1_SR1 & (1 << 6)))
      ;

    buffer[i] = I2C1_DR;
  }
}