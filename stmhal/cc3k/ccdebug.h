/**************************************************************************/
/*!
  @file     Adafruit_CC3000.cpp
  @author   KTOWN (Kevin Townsend for Adafruit Industries)
	@license  BSD (see license.txt)

	This is a library for the Adafruit CC3000 WiFi breakout board
	This library works with the Adafruit CC3000 breakout
	----> https://www.adafruit.com/products/1469

	Check out the links above for our tutorials and wiring diagrams
	These chips use SPI to communicate.

	Adafruit invests time and resources providing this open source code,
	please support Adafruit and open-source hardware by purchasing
	products from Adafruit!

	@section  HISTORY

	v1.0    - Initial release
*/
/**************************************************************************/

//#include <Arduino.h>

#ifndef _CC3000_DEBUG
#define _CC3000_DEBUG

#define DEBUG_MODE                      (0)

#define PRINT_F(__s)         DEBUGPRINT(FLASHIFY(__s))

#if (DEBUG_MODE != 0)
#define DEBUGPRINT_F(__s)         DEBUGPRINT(FLASHIFY(__s))
#define DEBUGPRINT_DEC(x)  printDec(x)
#define DEBUGPRINT_DEC16(x)  printDec16(x)
#define DEBUGPRINT_HEX(x)  printHex(x)
#define DEBUGPRINT_HEX16(x)  printHex16(x)
#else
#define DEBUGPRINT_F(__s)         /* do nothing! */
#define DEBUGPRINT_DEC(x)
#define DEBUGPRINT_DEC16(x)
#define DEBUGPRINT_HEX(x)
#define DEBUGPRINT_HEX16(x)
#endif

#if 1 // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf(args...) printf(args)
#else // don't print debugging info
#define DEBUG_printf(args...) (void)0
#endif

int printf(const char *fmt, ...);

#endif
