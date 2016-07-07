/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

// Resolution and Accuracy Settings
#define GMOUSE_ADS7843_PEN_CALIBRATE_ERROR		8
#define GMOUSE_ADS7843_PEN_CLICK_ERROR			6
#define GMOUSE_ADS7843_PEN_MOVE_ERROR			4
#define GMOUSE_ADS7843_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_ADS7843_FINGER_CLICK_ERROR		18
#define GMOUSE_ADS7843_FINGER_MOVE_ERROR		14

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_ADS7843_BOARD_DATA_SIZE			0

static bool_t init_board(GMouse* m, unsigned driverinstance) {

}

static GFXINLINE bool_t getpin_pressed(GMouse* m) {

}

static GFXINLINE void aquire_bus(GMouse* m) {

}

static GFXINLINE void release_bus(GMouse* m) {

}

static GFXINLINE uint16_t read_value(GMouse* m, uint16_t port) {

}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
