/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef GAUDIO_PLAY_BOARD_H
#define GAUDIO_PLAY_BOARD_H

// Initialise the board
static void board_init(void) {

}

// Chip is initialised enough so we can talk fast to it
static void board_init_end(void) {

}

// Reset the board
static void board_reset(void) {

}

// Returns the state of the dreq pin
static bool board_dreq(void) {

}

// Start a command write
static void board_startcmdwrite(void) {

}

// End a command write
static void board_endcmdwrite(void) {

}

// Start a command read
static void board_startcmdread(void) {

}

// End a command read
static void board_endcmdread(void) {

}

// Start a data write
static void board_startdatawrite(void) {

}

// End a data write
static void board_enddatawrite(void) {

}

// Write data to the SPI port
static void board_spiwrite(const uint8_t *buf, unsigned len) {

}

// Read data from the SPI port
static void board_spiread(uint8_t *buf, unsigned len) {

}

#endif	/* GAUDIO_PLAY_BOARD_H */
