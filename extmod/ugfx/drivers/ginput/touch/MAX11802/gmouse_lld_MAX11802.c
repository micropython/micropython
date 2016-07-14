/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE

#define GMOUSE_DRIVER_VMT		GMOUSEVMT_MAX11802
#include "../../../../src/ginput/ginput_driver_mouse.h"

// Hardware definitions
#include "max11802.h"

// Get the hardware interface
#include "gmouse_lld_MAX11802_board.h"

// Register values to set
#define MAX11802_MODE    0x0E		// Direct conversion with averaging
#define MAX11802_AVG     0x55
#define MAX11802_TIMING  0x77
#define MAX11802_DELAY   0x55

#define Z_MIN		0
#define Z_MAX		1

static bool_t MouseInit(GMouse* m, unsigned driverinstance)
{
	const uint8_t *p;

	static const uint8_t commandList[] = {
		MAX11802_CMD_GEN_WR, 0xf0,					// General config - leave TIRQ enabled, even though we ignore it ATM
		MAX11802_CMD_RES_WR, 0x00,					// A-D resolution, hardware config - rewriting default; all 12-bit resolution
		MAX11802_CMD_AVG_WR, MAX11802_AVG,			// A-D averaging - 8 samples, average four median samples
		MAX11802_CMD_SAMPLE_WR, 0x00,				// A-D sample time - use default
		MAX11802_CMD_TIMING_WR, MAX11802_TIMING,	// Setup timing
		MAX11802_CMD_DELAY_WR, MAX11802_DELAY,		// Conversion delays
		MAX11802_CMD_TDPULL_WR, 0x00,				// A-D resolution, hardware config - rewrite default
		// MAX11802_CMD_MDTIM_WR, 0x00,				// Autonomous mode timing - write default - only for MAX11800, MAX11801
		// MAX11802_CMD_APCONF_WR, 0x00,			// Aperture config register - rewrite default - only for MAX11800, MAX11801
													// Ignore aux config register - not used
		MAX11802_CMD_MODE_WR, MAX11802_MODE			// Set mode last
	};

	if (!init_board(m, driverinstance))
		return FALSE;

	aquire_bus(m);
	for (p = commandList; p < commandList+sizeof(commandList); p += 2)
		write_command(m, p[0], p[1]);
    release_bus(m);             // Need to release bus after each access to reset interface in chip
                                // (and in some cases, to allow sharing of SPI bus if board logic/OS manages it)
    // Setup complete here


	// Read something as a test
    aquire_bus(m);
	if (write_command(m, MAX11802_CMD_MODE_RD, 0) != MAX11802_MODE) {
		release_bus(m);
		return FALSE;
	}

	release_bus(m);

	return TRUE;
}

static bool_t read_xyz(GMouse* m, GMouseReading* pdr)
{
	uint8_t readyCount;
	uint8_t notReadyCount;

	// Assume not touched.
	pdr->buttons = 0;
	pdr->z = 0;
	
	aquire_bus(m);

	// Start the conversion
	gfintWriteCommand(m, MAX11802_CMD_MEASUREXY);		// just write command
    release_bus(m);

    /**
	 * put a delay in here, since conversion will take a finite time - longer if we were reading Z as well
	 * Potentially at least 1msec for 3 axes with 8us conversion time per sample, 8 samples per average
	 * Note Maxim AN5435-software to do calculation (www.maximintegrated.com/design/tools/appnotes/5435/AN5435-software.zip)
	 *
	 * We'll just use a fixed delay to avoid too much polling/bus activity
	 */
	gfxSleepMilliseconds(2);            // Was 1 - try 2

	/* Wait for data ready
	* Note: MAX11802 signals the readiness of the results using the lowest 4 bits of the result. However, these are the
	* last bits to be read out of the device. It is possible for the hardware value to change in the middle of the read,
	* causing the analog value to still be invalid even though the tags indicate a valid result.
	*
	* We work around this by reading the registers once more after the tags indicate they are ready.
	* There's also a separate counter to guard against never getting valid readings.
	*
	* Read the two readings required in a single burst, swapping x and y order if necessary
	*
	* Reading Z is possible but complicated requiring two z readings, multiplication and division, various constant ratio's,
	* 	and interpolation in relation to the X and Y readings. It is not a simple pressure reading.
	* 	In other words, don't bother trying.
	*/

	readyCount = notReadyCount = 0;
	do {
		// Get a set of readings
	    aquire_bus(m);
	    gfintWriteCommand(m, MAX11802_CMD_XPOSITION);
		#if defined(GINPUT_MOUSE_YX_INVERTED) && GINPUT_MOUSE_YX_INVERTED
			pdr->y = read_value(m);
			pdr->x = read_value(m);
		#else
			pdr->x = read_value(m);
			pdr->y = read_value(m);
		#endif
        release_bus(m);

		// Test the result validity
		if (((pdr->x | pdr->y) & 0x03) == 0x03) {

			// Has it been too long? If so give up
			if (++notReadyCount >= 5) {
				return FALSE;
			}

			// Give up the time slice to someone else and then try again
			gfxYield();
			continue;
		}

		// We have a result but we need two valid results to believe it
		readyCount++;

	} while (readyCount < 2);

	/**
	 *	format of each value returned by MAX11802:
	 *		Bits 15..4 - analog value
	 *		Bits 3..2 - tag value - measurement type (X, Y, Z1, Z2)
	 *		Bits 1..0 - tag value - event type (00 = valid touch, 10 - no touch, 11 - measurement in progress)
	 */

    // Was there a valid touch?
    if (((pdr->x | pdr->y) & 0x03) != 0x0) {
    	pdr->z = Z_MIN;
    	return TRUE;
    }

    // Strip the tags (we need to take care because coord_t is signed - and sign bit gets extended on shift!)
    pdr->x = (uint16_t)(pdr->x) >> 4;
    pdr->y = (uint16_t)(pdr->y) >> 4;
   	pdr->z = Z_MAX;

    return TRUE;
}

const GMouseVMT const GMOUSE_DRIVER_VMT[1] = {{
	{
		GDRIVER_TYPE_TOUCH,
		GMOUSE_VFLG_TOUCH | GMOUSE_VFLG_ONLY_DOWN | GMOUSE_VFLG_POORUPDOWN | GMOUSE_VFLG_CALIBRATE | GMOUSE_VFLG_CAL_TEST,
		sizeof(GMouse) + GMOUSE_MAX11802_BOARD_DATA_SIZE,
		_gmouseInitDriver,
		_gmousePostInitDriver,
		_gmouseDeInitDriver
	},
	Z_MAX,			// z_max
	Z_MIN,			// z_min
	Z_MAX,  		// z_touchon
	Z_MIN,			// z_touchoff
	{				// pen_jitter
		GMOUSE_MAX11802_PEN_CALIBRATE_ERROR,		// calibrate
		GMOUSE_MAX11802_PEN_CLICK_ERROR,			// click
		GMOUSE_MAX11802_PEN_MOVE_ERROR				// move
	},
	{				// finger_jitter
		GMOUSE_MAX11802_FINGER_CALIBRATE_ERROR,		// calibrate
		GMOUSE_MAX11802_FINGER_CLICK_ERROR,			// click
		GMOUSE_MAX11802_FINGER_MOVE_ERROR			// move
	},
	MouseInit, 		// init
	0,				// deinit
	read_xyz,		// get
	0,				// calsave
	0				// calload
}};

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */
