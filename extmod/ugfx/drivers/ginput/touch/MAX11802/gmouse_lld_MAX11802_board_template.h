/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

// Resolution and Accuracy Settings
#define GMOUSE_MAX11802_PEN_CALIBRATE_ERROR		8
#define GMOUSE_MAX11802_PEN_CLICK_ERROR			6
#define GMOUSE_MAX11802_PEN_MOVE_ERROR			4
#define GMOUSE_MAX11802_FINGER_CALIBRATE_ERROR	14
#define GMOUSE_MAX11802_FINGER_CLICK_ERROR		18
#define GMOUSE_MAX11802_FINGER_MOVE_ERROR		14

// How much extra data to allocate at the end of the GMouse structure for the board's use
#define GMOUSE_MAX11802_BOARD_DATA_SIZE			0

/**
 *	Init the board
 *
 *	Returns TRUE on success, FALSE on failure
 */
static GFXINLINE bool_t init_board(GMouse* m, unsigned driverinstance) {

}

/**
 *	Acquire the bus
 */
static GFXINLINE void aquire_bus(GMouse* m) {

}

/**
 *	Release the bus
 */
static GFXINLINE void release_bus(GMouse* m) {

}

/**
 *	Send command (with parameter) to the MAX11802
 *
 *	Return the second byte read in case of interest
 */
static GFXINLINE uint8_t write_command(GMouse* m, uint8_t command, uint8_t value) {
}

/**
 *	Send command (no parameter) to the MAX11802
 *
 *	Return the byte read in case of interest
 */
static GFXINLINE uint8_t gfintWriteCommand(GMouse* m, uint8_t command) {
}

/*
 *	Read 2 bytes as 16-bit value (command to read must have been sent previously)
 *	Note: Analog value is in bits 15..4, tags (reading status) in bits 3..0
 */
static GFXINLINE uint16_t read_value(GMouse* m) {

}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
