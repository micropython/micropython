This directory contains the interface for the MikroMedia Plus STM32 M4 board
running under ChibiOS.

On this board uGFX currently supports:
	- GDISP via the SSD1963 display
	- GINPUT-touch via the STMPE610 driver
	- GAUDIO via the vs1053 driver

As this is not a standard ChibiOS supported board the necessary board files have also
been provided in the ChibiOS_Board directory.

There are two examples, one for ChibiOS/RT 2.x and one for ChibiOS/RT 3.x available.

If you are using the uGFX makefiles use OPT_CPU=stm32m4 in your makefile to get the correct cpu.
