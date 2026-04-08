#include "drivers/led/led.h"
#include "drivers/clocks/clocks.h"

int main()
{
  initClocks();
  initLED1();
  enableLED1();

  while (1)
  {
  };
}