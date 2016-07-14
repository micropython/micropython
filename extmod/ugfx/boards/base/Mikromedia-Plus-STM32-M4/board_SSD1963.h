/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
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
		CALC_PERIOD(272,2,2,10), 				// Total Vertical Period (calculated from above line)
		CALC_FPR(480,272,2,2,41,2,2,10,73ULL),	// FPR - the 60ULL is the frames per second. Note the ULL!
		LCD_PANEL_DATA_WIDTH_24BIT | LCD_PANEL_ENABLE_DITHERING
	},
};

/* GPIO Pin Config */
#define GDISP_CMD_PORT          GPIOF
#define GDISP_DATA_LO_PORT      GPIOG
#define GDISP_DATA_HI_PORT      GPIOE

#define GDISP_WR                11
#define GDISP_RD                12
#define GDISP_CS                13
#define GDISP_RST               14
#define GDISP_DC                15

#define SET_CS                  palSetPad(GDISP_CMD_PORT, GDISP_CS);
#define CLR_CS                  palClearPad(GDISP_CMD_PORT, GDISP_CS);
#define SET_RST                 palSetPad(GDISP_CMD_PORT, GDISP_RST);
#define CLR_RST                 palClearPad(GDISP_CMD_PORT, GDISP_RST);
#define SET_WR                  palSetPad(GDISP_CMD_PORT, GDISP_WR);
#define CLR_WR                  palClearPad(GDISP_CMD_PORT, GDISP_WR);
#define SET_RD                  palSetPad(GDISP_CMD_PORT, GDISP_RD);
#define CLR_RD                  palClearPad(GDISP_CMD_PORT, GDISP_RD);
#define SET_DC                  palSetPad(GDISP_CMD_PORT, GDISP_DC);
#define CLR_DC                  palClearPad(GDISP_CMD_PORT, GDISP_DC);

#define writeStrobe             { CLR_WR; /*asm volatile ("nop;");*/ SET_WR; }

IOBus busCMD    = { GDISP_CMD_PORT, (1 << GDISP_CS) | (1 << GDISP_RST) | (1 << GDISP_WR) | (1 << GDISP_RD) | (1 << GDISP_DC), 11 };
IOBus busDataLo = { GDISP_DATA_LO_PORT, 0xFF, 0 };
IOBus busDataHi = { GDISP_DATA_HI_PORT, 0xFF, 8 };


static GFXINLINE void init_board(GDisplay *g) {
	g->board = 0;
	switch(g->controllerdisplay) {
	case 0:
		palSetBusMode(&busCMD, PAL_MODE_OUTPUT_PUSHPULL);
		palSetBusMode(&busDataLo, PAL_MODE_OUTPUT_PUSHPULL);
		palSetBusMode(&busDataHi, PAL_MODE_OUTPUT_PUSHPULL);
		SET_CS; SET_WR; SET_RD; SET_DC; SET_RST;
		break;
	default:
		break;
	}
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if (state) {
		CLR_RST;
	} else {
		SET_RST;
	}
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
	CLR_CS;
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	SET_CS;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;

	CLR_DC;
	palWriteBus(&busDataLo, (index & 0xFF));
	palWriteBus(&busDataHi, (index >> 8));
	writeStrobe;
	SET_DC;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;

	palWriteBus(&busDataLo, (data & 0xFF));
	palWriteBus(&busDataHi, (data >> 8));
	writeStrobe;
}

static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
	palSetBusMode(&busDataLo, PAL_MODE_INPUT_PULLUP);
	palSetBusMode(&busDataHi, PAL_MODE_INPUT_PULLUP);
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
	palSetBusMode(&busDataLo, PAL_MODE_OUTPUT_PUSHPULL);
	palSetBusMode(&busDataHi, PAL_MODE_OUTPUT_PUSHPULL);
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	uint16_t data;
	(void) g;

	CLR_RD;
	data = (palReadBus(&busDataHi) << 8) | palReadBus(&busDataLo);
	SET_RD;
	return data;
}

#endif /* _GDISP_LLD_BOARD_H */
