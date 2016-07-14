/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE

#define GMOUSE_DRIVER_VMT		GMOUSEVMT_STMPE610
#include "../../../../src/ginput/ginput_driver_mouse.h"

// Hardware definitions
#include "stmpe610.h"

// Get the hardware interface
#include "gmouse_lld_STMPE610_board.h"

// Extra settings for the users gfxconf.h file. See readme.txt
#ifndef GMOUSE_STMPE610_SELF_CALIBRATE
	#define GMOUSE_STMPE610_SELF_CALIBRATE	FALSE
#endif
#ifndef GMOUSE_STMPE610_READ_PRESSURE
	#define GMOUSE_STMPE610_READ_PRESSURE	FALSE
#endif
#ifndef GMOUSE_STMPE610_TEST_MODE
	#define GMOUSE_STMPE610_TEST_MODE		FALSE
#endif

/**
 * Notes:
 *
 * This chip has some problems which required careful coding to overcome.
 *
 * The interrupt pin seems to be unreliable, at least on some boards, so we at most
 * 	use the pin for filtering results to reduce cpu load.
 * 	The symptoms are that readings will just stop due to the irq not being asserted
 * 	even though there are items in the fifo. Another interrupt source such as a
 * 	touch transition will restart the irq.
 *
 * There is no fifo entry created when a touch up event occurs. We must therefore
 * 	generate a pseudo result on touch up. Fortunately the touch detection appears
 * 	reliable and so we turn off the driver GMOUSE_VFLG_POORUPDOWN setting. In practice
 * 	if touch is up we always return a pseudo event as this saves having to remember the
 * 	previous touch state.
 *
 * Z readings range from around 90 (fully touched) to around 150 (on the verge of non-touched).
 * Note the above is on the STM32F429i-Discovery board. Other boards may be different.
 * To be conservative we use 255 as touch off, anything else is a touch on.
 *
 * GMOUSE_STMPE610_TEST_MODE is designed to be used with the "touch_raw_readings" tool which shows
 * a steady stream of raw readings.
 *
 * Settings that may need tweaking on other hardware:
 * 		The settling times. We have set these conservatively at 1ms.
 * 		The reading window. We set this to 16 just to reduce noise. High-res panels may need a lower value.
 */
static bool_t MouseInit(GMouse* m, unsigned driverinstance) {
	if (!init_board(m, driverinstance))
		return FALSE;

	aquire_bus(m);

	write_reg(m, STMPE610_REG_SYS_CTRL1, 0x02);		// Software chip reset
	gfxSleepMilliseconds(10);

	write_reg(m, STMPE610_REG_SYS_CTRL2, 0x04);		// Temperature sensor clock off, GPIO clock off, touch clock on, ADC clock on

	#if GMOUSE_STMPE610_GPIO_IRQPIN
		write_reg(m, STMPE610_REG_INT_EN, 0x03);	// Interrupt on INT pin when there is a sample or a touch transition.
	#else
		write_reg(m, STMPE610_REG_INT_EN, 0x00);	// Don't Interrupt on INT pin
	#endif

	write_reg(m, STMPE610_REG_ADC_CTRL1, 0x48);		// ADC conversion time = 80 clock ticks, 12-bit ADC, internal voltage refernce
	gfxSleepMilliseconds(2);
	write_reg(m, STMPE610_REG_ADC_CTRL2, 0x01);		// ADC speed 3.25MHz
	write_reg(m, STMPE610_REG_GPIO_AF, 0x00);		// GPIO alternate function - OFF
	write_reg(m, STMPE610_REG_TSC_CFG, 0xA3);		// Averaging 4, touch detect delay 1ms, panel driver settling time 1ms
	write_reg(m, STMPE610_REG_FIFO_TH, 0x01);		// FIFO threshold = 1
	write_reg(m, STMPE610_REG_FIFO_STA, 0x01);		// FIFO reset enable
	write_reg(m, STMPE610_REG_FIFO_STA, 0x00);		// FIFO reset disable
	write_reg(m, STMPE610_REG_TSC_FRACT_XYZ, 0x07);	// Z axis data format
	write_reg(m, STMPE610_REG_TSC_I_DRIVE, 0x01);	// max 50mA touchscreen line current
	#if GMOUSE_STMPE610_READ_PRESSURE
		write_reg(m, STMPE610_REG_TSC_CTRL, 0x30);	// X&Y&Z, 16 reading window
		write_reg(m, STMPE610_REG_TSC_CTRL, 0x31);	// X&Y&Z, 16 reading window, TSC enable
	#else
		write_reg(m, STMPE610_REG_TSC_CTRL, 0x32);	// X&Y, 16 reading window
		write_reg(m, STMPE610_REG_TSC_CTRL, 0x33);	// X&Y, 16 reading window, TSC enable
	#endif
	write_reg(m, STMPE610_REG_INT_STA, 0xFF);		// Clear all interrupts
	write_reg(m, STMPE610_REG_INT_CTRL, 0x01);		// Level interrupt, enable interrupts

	release_bus(m);
	return TRUE;
}

static bool_t read_xyz(GMouse* m, GMouseReading* pdr)
{
	#if GMOUSE_STMPE610_TEST_MODE
		static GMouseReading n;
	#endif
	uint8_t		status;

	// Button information will be regenerated
	pdr->buttons = 0;

	#if GMOUSE_STMPE610_TEST_MODE
		aquire_bus(m);

		// Set the buttons to match various touch signals
		if ((read_byte(m, STMPE610_REG_TSC_CTRL) & 0x80))
			pdr->buttons |= 0x02;

		status = read_byte(m, STMPE610_REG_FIFO_STA);
		if (!(status & 0x20))
			pdr->buttons |= 0x04;

		#if GMOUSE_STMPE610_GPIO_IRQPIN
			if (getpin_irq(m))
				pdr->buttons |= 0x08;
		#endif

		if ((status & 0x20)) {
			// Nothing in the fifo - just return the last position and pressure
			pdr->x = n.x;
			pdr->y = n.y;
			pdr->z = n.z;
			#if GMOUSE_STMPE610_GPIO_IRQPIN
				write_reg(m, STMPE610_REG_INT_STA, 0xFF);
			#endif
			release_bus(m);
			return TRUE;
		}

	#else
		// Is there a new sample or a touch transition
		#if GMOUSE_STMPE610_GPIO_IRQPIN
			if(!getpin_irq(m))
				return FALSE;
		#endif

		// Is there something in the fifo
		status = read_byte(m, STMPE610_REG_FIFO_STA);
		if ((status & 0x20)) {

			// Nothing in the fifo.

			// If not touched return the pseudo result
			if (!(read_byte(m, STMPE610_REG_TSC_CTRL) & 0x80)) {

				pdr->z = gmvmt(m)->z_min;
				#if GMOUSE_STMPE610_GPIO_IRQPIN
					write_reg(m, STMPE610_REG_INT_STA, 0xFF);
				#endif
				release_bus(m);
				return TRUE;
			}

			// No new result
			#if GMOUSE_STMPE610_GPIO_IRQPIN
				write_reg(m, STMPE610_REG_INT_STA, 0xFF);
			#endif
			release_bus(m);
			return FALSE;
		}

	#endif

	// Time to get some readings
	pdr->x = (coord_t)read_word(m, STMPE610_REG_TSC_DATA_X);
	pdr->y = (coord_t)read_word(m, STMPE610_REG_TSC_DATA_Y);
	#if GMOUSE_STMPE610_READ_PRESSURE
		pdr->z = (coord_t)read_byte(m, STMPE610_REG_TSC_DATA_Z);
	#else
		pdr->z = gmvmt(m)->z_max;
	#endif

	#if !GMOUSE_STMPE610_SLOW_CPU
		if (!(status & 0xC0)) {
			// Is there more data to come
			if (!(read_byte(m, STMPE610_REG_FIFO_STA) & 0x20))
				_gmouseWakeup(m);
		} else
	#endif

	// Clear the rest of the fifo
	{
		write_reg(m, STMPE610_REG_FIFO_STA, 0x01);		// FIFO reset enable
		write_reg(m, STMPE610_REG_FIFO_STA, 0x00);		// FIFO reset disable
	}

	// All done
	#if GMOUSE_STMPE610_GPIO_IRQPIN
		write_reg(m, STMPE610_REG_INT_STA, 0xFF);
	#endif
	release_bus(m);

	#if GMOUSE_STMPE610_TEST_MODE
		// Save the result for later
		n.x = pdr->x;
		n.y = pdr->y;
		n.z = pdr->z;
	#endif

	// Rescale X,Y if we are using self-calibration
	#if GMOUSE_STMPE610_SELF_CALIBRATE
		#if GDISP_NEED_CONTROL
			switch(gdispGGetOrientation(m->display)) {
			default:
			case GDISP_ROTATE_0:
			case GDISP_ROTATE_180:
			default:
				pdr->x = pdr->x / (4096/gdispGGetWidth(m->display));
				pdr->y = pdr->y / (4096/gdispGGetHeight(m->display));
				break;
			case GDISP_ROTATE_90:
			case GDISP_ROTATE_270:
				pdr->x = pdr->x / (4096/gdispGGetHeight(m->display));
				pdr->y = pdr->y / (4096/gdispGGetWidth(m->display));
				break;
			}
		#else
			pdr->x = pdr->x / (4096/gdispGGetWidth(m->display));
			pdr->y = pdr->y / (4096/gdispGGetHeight(m->display));
		#endif
	#endif

	return TRUE;
}

const GMouseVMT const GMOUSE_DRIVER_VMT[1] = {{
	{
		GDRIVER_TYPE_TOUCH,
		#if GMOUSE_STMPE610_SELF_CALIBRATE
			GMOUSE_VFLG_TOUCH | GMOUSE_VFLG_ONLY_DOWN,
		#else
			GMOUSE_VFLG_TOUCH | GMOUSE_VFLG_ONLY_DOWN | GMOUSE_VFLG_CALIBRATE | GMOUSE_VFLG_CAL_TEST,
		#endif
		sizeof(GMouse) + GMOUSE_STMPE610_BOARD_DATA_SIZE,
		_gmouseInitDriver,
		_gmousePostInitDriver,
		_gmouseDeInitDriver
	},
	0,				// z_max	- 0 indicates full touch
	255,			// z_min
	150,			// z_touchon
	255,			// z_touchoff
	{				// pen_jitter
		GMOUSE_STMPE610_PEN_CALIBRATE_ERROR,		// calibrate
		GMOUSE_STMPE610_PEN_CLICK_ERROR,			// click
		GMOUSE_STMPE610_PEN_MOVE_ERROR				// move
	},
	{				// finger_jitter
		GMOUSE_STMPE610_FINGER_CALIBRATE_ERROR,		// calibrate
		GMOUSE_STMPE610_FINGER_CLICK_ERROR,			// click
		GMOUSE_STMPE610_FINGER_MOVE_ERROR			// move
	},
	MouseInit, 		// init
	0,				// deinit
	read_xyz,		// get
	0,				// calsave
	0				// calload
}};

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */

