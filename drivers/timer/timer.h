#pragma once
#include <stdint.h>

void __attribute__((optimize("O0"))) initTIM2();

void __attribute__((optimize("O0"))) delayMS(uint32_t ms);

void initMicros(void);

uint32_t micros(void);