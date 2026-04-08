#include <stdint.h>
#include "led.h"

#define GPIOA_ADDR 0x40020000
#define RCC_ADDR 0x40023800
#define GPIOA_MODER *((volatile uint32_t *)(GPIOA_ADDR))
#define GPIOA_OTYPER *((volatile uint32_t *)(GPIOA_ADDR + 0x04))
#define GPIOA_OSPEEDR *((volatile uint32_t *)(GPIOA_ADDR + 0x08))
#define GPIOA_ODR *((volatile uint32_t *)(GPIOA_ADDR + 0x14))
#define RCC_AHB1ENR *((volatile uint32_t *)(RCC_ADDR + 0x30))

void initLED1()
{
  RCC_AHB1ENR |= (1 << 0);

  GPIOA_MODER |= (1 << 2);
  GPIOA_MODER &= ~(1 << 3);

  GPIOA_OTYPER &= ~(1 << 1);

  GPIOA_OSPEEDR &= ~(1 << 2);
  GPIOA_OSPEEDR |= (1 << 3);
}

void enableLED1()
{
  GPIOA_ODR |= (1 << 1);
}

void disableLED1()
{
  GPIOA_ODR &= ~(1 << 1);
}