#include <stdint.h>
#include "led.h"

#define GPIOA_ADDR 0x40020000
#define GPIOA_MODER *((volatile uint32_t *)(GPIOA_ADDR))
#define GPIOA_OTYPER *((volatile uint32_t *)(GPIOA_ADDR + 0x04))
#define GPIOA_OSPEEDR *((volatile uint32_t *)(GPIOA_ADDR + 0x08))
#define GPIOA_ODR *((volatile uint32_t *)(GPIOA_ADDR + 0x14))

void initLED1()
{
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