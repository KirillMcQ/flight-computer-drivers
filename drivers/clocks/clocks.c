#include <stdint.h>
#include "clocks.h"

#define RCC_ADDR 0x40023800
#define RCC_AHB1ENR *((volatile uint32_t *)(RCC_ADDR + 0x30))
#define RCC_APB1ENR *((volatile uint32_t *)(RCC_ADDR + 0x40))

void initClocks()
{
  // GPIOA
  RCC_AHB1ENR |= (1 << 0);

  // GPIOB
  RCC_AHB1ENR |= (1 << 1);

  // USART3
  RCC_APB1ENR |= (1 << 18);

  // TIM2
  RCC_APB1ENR |= (1 << 0);

  // TIM5
  RCC_APB1ENR |= (1 << 3);

  // I2C1
  RCC_APB1ENR |= (1 << 21);
}