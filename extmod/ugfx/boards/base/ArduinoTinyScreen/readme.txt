This is a working Arduino project for the TinyScreen ATmega board.

To use this with the Arduino IDE Development Environment follow these steps...

1/ Copy the gfx directory from this directory to your Arduino library. eg <Documents>/Arduino/libraries/gfx
3/ Create a subdirectory under "gfx" called "ugfx" and copy the entire ugfx system to that directory.
4/ In the "gfx" directory adjust the gfxconf.h for your project.
5/ In the gfx directory create a .c file for each driver that you want to use that contains a single line
	to #include the driver source in the repository file. For a GDISP driver you will also need to create
	a gdisp_lld_config.h file that contains a single line that #include's the gdisp_lld_config.h file for
	the GDISP driver. Don't forget to add the board files for your drivers.
	This example has the files for the SSD1331 TinyScreen display.
6/ Copy the example ugfx_test.ino file to your Arduino projects directory eg <Documents>/Arduino/ugfx_test
7/ Modify the ugfx_test.ino project to suit.
8/ Remember that for ATmega platforms RAM and FLASH are very limited. Be careful which ugfx options you turn on.
	Arduino ARM based boards are much less limited.

