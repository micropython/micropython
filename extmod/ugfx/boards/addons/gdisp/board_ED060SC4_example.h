/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/addons/gdisp/board_ED060SC4_example.h
 * @brief   GDISP Graphic Driver subsystem board interface for the ED060SC4 display.
 *
 * This file corresponds to the connections shown in example_schematics.png,
 * and is designed to interface with ChibiOS/RT.
 * 
 * Please note that this file has never been tested in exactly this pin
 * configuration, because the actual boards I have are slightly different.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include <hal.h>

/*
 * IO pins assignments.
 */
#define GPIOB_EINK_VDD      0
#define GPIOB_EINK_GMODE    1
#define GPIOB_EINK_SPV      2
#define GPIOB_EINK_CKV      3
#define GPIOB_EINK_CL       4
#define GPIOB_EINK_LE       5
#define GPIOB_EINK_OE       6
#define GPIOB_EINK_SPH      7
#define GPIOB_EINK_D0       8
#define GPIOB_EINK_D1       9
#define GPIOB_EINK_D2       10
#define GPIOB_EINK_D3       11
#define GPIOB_EINK_D4       12
#define GPIOB_EINK_D5       13
#define GPIOB_EINK_D6       14
#define GPIOB_EINK_D7       15

#define GPIOC_SMPS_CTRL     13
#define GPIOC_VPOS_CTRL     14
#define GPIOC_VNEG_CTRL     15

static GFXINLINE void init_board(GDisplay *g) {

	// As we are not using multiple displays we set g->board to NULL as we don't use it.
	g->board = 0;

	switch(g->controllerdisplay) {
	case 0:											// Set up for Display 0
		/* Main SMPS power control, active low
		 * (open collector so that MOSFET gate can be pulled up to Vbat) */
		palWritePad(GPIOC, GPIOC_SMPS_CTRL, true);
		palSetPadMode(GPIOC, GPIOC_SMPS_CTRL, PAL_MODE_OUTPUT_OPENDRAIN);

		/* Power control for the positive & negative side */
		palWritePad(GPIOC, GPIOC_VPOS_CTRL, false);
		palSetPadMode(GPIOC, GPIOC_VPOS_CTRL, PAL_MODE_OUTPUT_PUSHPULL);
		palWritePad(GPIOC, GPIOC_VNEG_CTRL, false);
		palSetPadMode(GPIOC, GPIOC_VNEG_CTRL, PAL_MODE_OUTPUT_PUSHPULL);

		/* Main data bus */
		palWritePort(GPIOB, 0);
		palSetGroupMode(GPIOB, 0xFFFF, 0, PAL_MODE_OUTPUT_PUSHPULL);
		break;
	}
}

/* Delay for display waveforms. Should be an accurate microsecond delay. */
static void eink_delay(int us) {
	halPolledDelay(US2RTT(us));
}

/* Turn the E-ink panel Vdd supply (+3.3V) on or off. */
static GFXINLINE void setpower_vdd(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOB, GPIOB_SMPS_CTRL, !on);
	palWritePad(GPIOA, GPIOA_EINK_VDD, on);
}

/* Turn the E-ink panel negative supplies (-15V, -20V) on or off. */
static GFXINLINE void setpower_vneg(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOA, GPIOA_VNEG_CTRL, on);
}

/* Turn the E-ink panel positive supplies (-15V, -20V) on or off. */
static GFXINLINE void setpower_vpos(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOA, GPIOA_VPOS_CTRL, on);
}

/* Set the state of the LE (source driver Latch Enable) pin. */
static GFXINLINE void setpin_le(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOB, GPIOB_EINK_LE, on);
}

/* Set the state of the OE (source driver Output Enable) pin. */
static GFXINLINE void setpin_oe(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOB, GPIOB_EINK_OE, on);
}

/* Set the state of the CL (source driver Clock) pin. */
static GFXINLINE void setpin_cl(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOB, GPIOB_EINK_CL, on);
}

/* Set the state of the SPH (source driver Start Pulse Horizontal) pin. */
static GFXINLINE void setpin_sph(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOB, GPIOB_EINK_SPH, on);
}

/* Set the state of the D0-D7 (source driver Data) pins. */
static GFXINLINE void setpins_data(GDisplay *g, uint8_t value) {
	(void) g;
	palWriteGroup(GPIOB, 0xFF, GPIOB_EINK_D0, value);
}

/* Set the state of the CKV (gate driver Clock Vertical) pin. */
static GFXINLINE void setpin_ckv(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOB, GPIOB_EINK_CKV, on);
}

/* Set the state of the GMODE (gate driver Gate Mode) pin. */
static GFXINLINE void setpin_gmode(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOC, GPIOC_EINK_GMODE, on);
}

/* Set the state of the SPV (gate driver Start Pulse Vertical) pin. */
static GFXINLINE void setpin_spv(GDisplay *g, bool_t on) {
	(void) g;
	palWritePad(GPIOB, GPIOB_EINK_SPV, on);
}

#endif
