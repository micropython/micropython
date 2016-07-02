This directory contains the interface for the MikroMedia STM32 M4 board
running under ChibiOS with the ILI9341 display.

On this board uGFX currently supports:
	- GDISP via the ILI9341 display
	- GINPUT-touch via the MCU driver
	- GAUDIO (play only) via the vs1053 driver 

Note there are two variants of this board - one with the ILI9341 display
	and an older one with a different display. This one is for the ILI9341 display.
	
As this is not a standard ChibiOS supported board the necessary board files have also
been provided in the ChibiOS_Board directory.

There are two examples, one for ChibiOS/RT 2.x and one for ChibiOS/RT 3.x available.

If you are using the uGFX makefiles use OPT_CPU=stm32m4 in your makefile to get the correct cpu.
