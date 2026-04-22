#include "drivers/led/led.h"
#include "drivers/clocks/clocks.h"
#include "drivers/uart/uart.h"
#include "drivers/timer/timer.h"

int main()
{
  initClocks();
  initLED1();
  initTIM2();

  while (1)
  {
    enableLED1();
    transmitString("Hello, World!");
    delayMS(1000);
    disableLED1();
    delayMS(1000);
  };
}