#pragma once
#include <stdint.h>

void initI2C1();
void __attribute__((optimize("O0"))) writeToRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t data);
void __attribute__((optimize("O0"))) burstReadFromRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t n, uint8_t *buffer);