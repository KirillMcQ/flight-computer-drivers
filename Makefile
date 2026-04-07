main.hex: main.elf
	arm-none-eabi-objcopy -O ihex main.elf main.hex

main.elf: main.o startup.o
	arm-none-eabi-gcc -g3 -mcpu=cortex-m4 --specs=nosys.specs -T linkerscript.ld main.o startup.o -o main.elf

startup.o: startup.s
	arm-none-eabi-gcc -g3 -mcpu=cortex-m4 -mthumb -c startup.s -o startup.o

main.o: main.c
	arm-none-eabi-gcc -g3 -mcpu=cortex-m4 -mthumb -c main.c -o main.o

program:
	STM32_Programmer_CLI -c port=SWD -w main.hex 

clean:
	rm *.o *.elf *.hex

.PHONY: clean program