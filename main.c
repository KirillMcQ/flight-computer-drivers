#include "drivers/led/led.h"
#include "drivers/clocks/clocks.h"
#include "drivers/uart/uart.h"
#include "drivers/timer/timer.h"

int main()
{
  initClocks();
  initLED1();
  initTIM2();
  initUART3();

  while (1)
  {
    transmitString("Hello, World!");
    delayMS(1000);
  };
}