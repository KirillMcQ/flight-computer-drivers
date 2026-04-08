#include <stdint.h>
#include "clocks.h"

#define RCC_ADDR 0x40023800
#define RCC_AHB1ENR *((volatile uint32_t *)(RCC_ADDR + 0x30))

void initClocks()
{
  RCC_AHB1ENR |= (1 << 0);
}