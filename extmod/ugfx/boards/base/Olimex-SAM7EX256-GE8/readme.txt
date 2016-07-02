This directory contains the interface for the Olimex SAM7EX256 board
running under ChibiOS.

On this board uGFX currently supports:
	- GDISP via the Nokia6610GE8 display 
	- GADC via the AT91SAM7 driver
	- GINPUT-dials via the GADC driver
	- GINPUT-toggles via the Pal driver
	- GAUDIO recording via the GADC driver

Note there are two variants of this board - one with the GE8 display
	and one with the GE12 display. This one is for the GE8 display.
	
There is an example Makefile and project in the examples directory.

There are also board files predefined for (just add driver to the makefile):
	- TLS8204 on the UEXT connector (Olimex MOD3310)
	- SSD1306 on the UEXT connector using SPI	(copy board_SSD1306_spi.h to board_SSD1306.h in your project directory)
	- SSD1306 on the UEXT connector using I2C	(copy board_SSD1306_i2c.h to board_SSD1306.h in your project directory)
	
If you are using the uGFX makefiles use OPT_CPU=at91sam7 in your makefile to get the correct cpu.
