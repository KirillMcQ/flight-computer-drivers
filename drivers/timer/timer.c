#include "timer.h"

#define TIM2_ADDR 0x40000000
#define TIM5_ADDR 0x40000C00
#define TIM2_CR1 *((volatile uint32_t *)(TIM2_ADDR))
#define TIM2_SR *((volatile uint32_t *)(TIM2_ADDR + 0x10))
#define TIM2_EGR *((volatile uint32_t *)(TIM2_ADDR + 0x14))
#define TIM2_CNT *((volatile uint32_t *)(TIM2_ADDR + 0x24))
#define TIM2_PSC *((volatile uint32_t *)(TIM2_ADDR + 0x28))
#define TIM2_ARR *((volatile uint32_t *)(TIM2_ADDR + 0x2C))
#define TIM5_CR1 *((volatile uint32_t *)(TIM5_ADDR))
#define TIM5_SR *((volatile uint32_t *)(TIM5_ADDR + 0x10))
#define TIM5_EGR *((volatile uint32_t *)(TIM5_ADDR + 0x14))
#define TIM5_CNT *((volatile uint32_t *)(TIM5_ADDR + 0x24))
#define TIM5_PSC *((volatile uint32_t *)(TIM5_ADDR + 0x28))
#define TIM5_ARR *((volatile uint32_t *)(TIM5_ADDR + 0x2C))

void __attribute__((optimize("O0"))) initTIM2()
{
  TIM2_CR1 &= ~(1 << 0);
  TIM2_CR1 &= ~(1 << 7);

  // Clock frequency of 1 MHz
  TIM2_PSC = 15;

  TIM2_EGR |= (1 << 0);
}

void initMicros(void)
{
  TIM5_CR1 &= ~(1 << 0);

  TIM5_CR1 &= ~(1 << 7);

  TIM5_PSC = (16000000U / 1000000U) - 1U;

  TIM5_ARR = 0xFFFFFFFFU;

  TIM5_CNT = 0;

  TIM5_EGR |= (1 << 0);

  TIM5_SR &= ~(1 << 0);

  TIM5_CR1 |= (1 << 0);
}

uint32_t micros(void)
{
  return TIM5_CNT;
}

void __attribute__((optimize("O0"))) delayMS(uint32_t ms)
{
  TIM2_ARR = (ms * 1000U) - 1;

  TIM2_EGR |= (1 << 0);
  TIM2_SR &= ~(1 << 0);

  TIM2_CNT = 0;

  TIM2_CR1 |= (1 << 0);

  TIM2_SR &= ~(1 << 0);
  while (!((1 << 0) & TIM2_SR))
    ;

  TIM2_CR1 &= ~(1 << 0);
  TIM2_SR &= ~(1 << 0);
}