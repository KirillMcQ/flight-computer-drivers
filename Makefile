SRCS = main.c drivers/led/led.c drivers/clocks/clocks.c drivers/uart/uart.c drivers/timer/timer.c drivers/i2c/i2c.c drivers/sensors/sensors.c drivers/quaternion/quaternion.c
OBJS = $(SRCS:.c=.o)

main.hex: $(OBJS) startup.o
	arm-none-eabi-gcc -g3 -mcpu=cortex-m4 -mthumb --specs=nosys.specs -T linkerscript.ld $(OBJS) startup.o -o main.elf
	arm-none-eabi-objcopy -O ihex main.elf main.hex


$(OBJS): %.o : %.c
	arm-none-eabi-gcc -g3 -mcpu=cortex-m4 -mthumb -c $< -o $@

startup.o: startup.s
	arm-none-eabi-gcc -g3 -mcpu=cortex-m4 -mthumb -c startup.s -o startup.o

program:
	STM32_Programmer_CLI -c port=SWD -w main.hex 

clean:
	rm *.o *.elf *.hex $(OBJS)

.PHONY: clean program