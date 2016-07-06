/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include <Arduino.h>
#include <SPI.h>

// pin numbers
#define SSD1351_DC	14
#define SSD1351_R	15
#define SSD1351_CS	16

// Predefine the routine with "C" prototypes
extern "C" void ssd1351_init_board(void);
extern "C" void ssd1351_setpin_reset(int state);
extern "C" void ssd1351_acquire_bus(void);
extern "C" void ssd1351_release_bus(void);
extern "C" void ssd1351_write_cmd(unsigned char index);
extern "C" void ssd1351_write_data(unsigned char data);

static SPISettings settings(12000000, MSBFIRST, SPI_MODE0);

void ssd1351_init_board(void) {
	pinMode(SSD1351_R, OUTPUT);
	pinMode(SSD1351_CS, OUTPUT);
	pinMode(SSD1351_DC, OUTPUT);
	digitalWriteFast(SSD1351_R, 1);
	digitalWriteFast(SSD1351_CS, 1);
	digitalWriteFast(SSD1351_DC, 1);
}

void ssd1351_setpin_reset(int state) {
	if (state)
		digitalWriteFast(SSD1351_R, 0);
	else
		digitalWriteFast(SSD1351_R, 1);
}

void ssd1351_acquire_bus(void) {
	SPI.beginTransaction(settings);
	digitalWriteFast(SSD1351_CS, 0);
}

void ssd1351_release_bus(void) {
	digitalWriteFast(SSD1351_CS, 1);
	SPI.endTransaction();
}

void ssd1351_write_cmd(unsigned char index) {
	digitalWriteFast(SSD1351_DC, 0);
	SPI.transfer(index);
	digitalWriteFast(SSD1351_DC, 1);
}

void ssd1351_write_data(unsigned char data) {
	SPI.transfer(data);
}
