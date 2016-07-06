/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    boards/addons/gdisp/board_SSD1963_gpio.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SSD1963 display.
 *
 * @note	This file contains a mix of hardware specific and operating system specific
 *			code. You will need to change it for your CPU and/or operating system.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

static const LCD_Parameters	DisplayTimings[] = {
	// You need one of these array elements per display
	{
		480, 272,								// Panel width and height
		2, 2, 41,								// Horizontal Timings (back porch, front porch, pulse)
		CALC_PERIOD(480,2,2,41),				// Total Horizontal Period (calculated from above line)
		2, 2, 10,								// Vertical Timings (back porch, front porch, pulse)
		CALC_PERIOD(272,2,2,10),				// Total Vertical Period (calculated from above line)
		CALC_FPR(480,272,2,2,41,2,2,10,60ULL)	// FPR - the 60ULL is the frames per second. Note the ULL!
	},
};

// For a multiple display configuration we would put all this in a structure and then
//	set g->board to that structure.

/**
 * @brief	The GPIO pin config
 *
 * @details	This block of defines tell your driver how you wired your display
 *			controller to your MCU. Please change them accordingly.
 */
#define GDISP_CMD_PORT		GPIOC
#define GDISP_DATA_PORT		GPIOD
#define GDISP_CS			0
#define GDISP_RS			1
#define GDISP_WR			2
#define GDISP_RD			3

#define Set_CS		palSetPad(GDISP_CMD_PORT, GDISP_CS);
#define Clr_CS		palClearPad(GDISP_CMD_PORT, GDISP_CS);
#define Set_RS		palSetPad(GDISP_CMD_PORT, GDISP_RS);
#define Clr_RS		palClearPad(GDISP_CMD_PORT, GDISP_RS);
#define Set_WR		palSetPad(GDISP_CMD_PORT, GDISP_WR);
#define Clr_WR		palClearPad(GDISP_CMD_PORT, GDISP_WR);
#define Set_RD		palSetPad(GDISP_CMD_PORT, GDISP_RD);
#define Clr_RD		palClearPad(GDISP_CMD_PORT, GDISP_RD);

static GFXINLINE void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		IOBus busCMD = {GDISP_CMD_PORT, (1 << GDISP_CS) | (1 << GDISP_RS) | (1 << GDISP_WR) | (1 << GDISP_RD), 0};
		IOBus busDATA = {GDISP_CMD_PORT, 0xFFFFF, 0};
		palSetBusMode(&busCMD, PAL_MODE_OUTPUT_PUSHPULL);
		palSetBusMode(&busDATA, PAL_MODE_OUTPUT_PUSHPULL);
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
	Set_CS;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	Clr_CS;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;
	Set_RS; Clr_RD; Set_WR;
	palWritePort(GDISP_DATA_PORT, index);
	Clr_WR;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;
	Clr_RS; Clr_RD; Set_WR;
	palWritePort(GDISP_DATA_PORT, data);
	Clr_WR;
}

#endif /* _GDISP_LLD_BOARD_H */

