SSD1963 driver modified to handle Displaytech INTXXX displays, which self-initialise.

To use this mode, add:

#define GDISP_SSD1963_NO_INIT     TRUE

...either in the board-specific header file, or in gfxconf.h
