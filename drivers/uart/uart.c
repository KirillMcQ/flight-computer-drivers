#include <stdint.h>
#include "uart.h"

#define USART3_ADDR 0x40004800
#define GPIOB_ADDR 0x40020400
#define GPIOB_MODER *((volatile uint32_t *)(GPIOB_ADDR))
#define GPIOB_OTYPER *((volatile uint32_t *)(GPIOB_ADDR + 0x04))
#define GPIOB_OSPEEDR *((volatile uint32_t *)(GPIOB_ADDR + 0x08))
#define GPIOB_AFRH *((volatile uint32_t *)(GPIOB_ADDR + 0x24))
#define USART3_SR *((volatile uint32_t *)(USART3_ADDR))
#define USART3_DR *((volatile uint32_t *)(USART3_ADDR + 0x04))
#define USART3_BRR *((volatile uint32_t *)(USART3_ADDR + 0x08))
#define USART3_CR1 *((volatile uint32_t *)(USART3_ADDR + 0x0C))
#define USART3_CR2 *((volatile uint32_t *)(USART3_ADDR + 0x10))

void initUART3()
{
  GPIOB_MODER &= ~(1 << 20);
  GPIOB_MODER |= (1 << 21);

  GPIOB_MODER &= ~(1 << 22);
  GPIOB_MODER |= (1 << 23);

  GPIOB_OTYPER &= ~(1 << 10);
  GPIOB_OTYPER &= ~(1 << 11);

  GPIOB_OSPEEDR &= ~(1 << 20);
  GPIOB_OSPEEDR |= (1 << 21);

  GPIOB_OSPEEDR &= ~(1 << 22);
  GPIOB_OSPEEDR |= (1 << 23);

  GPIOB_AFRH |= (1 << 8);
  GPIOB_AFRH |= (1 << 9);
  GPIOB_AFRH |= (1 << 10);
  GPIOB_AFRH &= ~(1 << 11);

  GPIOB_AFRH |= (1 << 12);
  GPIOB_AFRH |= (1 << 13);
  GPIOB_AFRH |= (1 << 14);
  GPIOB_AFRH &= ~(1 << 15);

  // Disable USART3 for configuration
  USART3_CR1 &= ~(1 << 13);

  // 1 start bit, 8 data bits, 0 stop bits
  USART3_CR1 &= ~(1 << 12);
  USART3_CR2 &= ~(3 << 12);

  // OVER8 = 0
  USART3_CR1 &= ~(1 << 15);

  // Baudrate = 115200
  USART3_BRR = 11 | (8 << 4);

  // Enable USART3
  USART3_CR1 |= (1 << 13);

  // TE Enabled
  USART3_CR1 |= (1 << 3);
}

static void __attribute__((optimize("O0"))) transmitCharacter(char c)
{
  USART3_DR = c;
  while (!((1 << 6) & USART3_SR))
    ;
}

void __attribute__((optimize("O0"))) transmitString(char str[])
{
  for (int i = 0; i < sizeof(str) / sizeof(str[0]); i++)
  {
    transmitCharacter(str[i]);
  }
  transmitCharacter('\r');
  transmitCharacter('\n');
}