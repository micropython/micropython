/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _BOARD_SSD1848_H
#define _BOARD_SSD1848_H

static void init_board(GDisplay *g) {	
	(void) g;
}

static void post_init_board(GDisplay *g) {
	(void) g;
}

static void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	(void) state;
}

static void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

static void acquire_bus(GDisplay *g) {
	(void) g;
}

static void release_bus(GDisplay *g) {
	(void) g;
}

static  void spi_write_cmd(GDisplay *g, uint8_t cmd) {
	(void) g;
	(void) cmd;
}

static void spi_write_data_array(GDisplay *g, uint8_t* datas, size_t length) {
	(void) g;
	(void) datas;
	(void) length;
}

static void spi_write_data(GDisplay *g, uint8_t data) {
	(void) g;
	(void) data;
}

#endif /* _BOARD_SSD1848_H */
