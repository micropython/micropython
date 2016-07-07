/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    boards/addons/gdisp/board_ILI9341_spi.h
 * @brief   GDISP Graphic Driver subsystem board interface for the ILI9341 display.
 *
 * @note	This file contains a mix of hardware specific and operating system specific
 *			code. You will need to change it for your CPU and/or operating system.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define LCD_PORT	GPIOB
#define LCD_MOSI	15
#define LCD_MISO	14
#define LCD_SCK		13
#define LCD_CS		12
#define LCD_DC		11
#define LCD_RES		10

#define LCD_DC_CMD	palClearPad(LCD_PORT, LCD_DC)
#define LCD_DC_DATA	palSetPad(LCD_PORT, LCD_DC)
#define LCD_SCK_SET	palSetPad(LCD_PORT, LCD_SCK)
#define LCD_SCK_RES	palClearPad(LCD_PORT, LCD_SCK)
#define LCD_CS_RES	palSetPad(LCD_PORT, LCD_CS)
#define LCD_CS_SET	palClearPad(LCD_PORT, LCD_CS)

/**
 * SPI configuration structure.
 * Speed 12 MHz, CPHA=0, CPOL=0, 8bits frames, MSb transmitted first.
 * Soft slave select.
 */
static const SPIConfig spi2cfg = {
  NULL,
  LCD_PORT,
  LCD_CS,
  (SPI_CR1_MSTR | SPI_CR1_SPE | SPI_CR1_SSM | SPI_CR1_SSI)
};

static void send_data(uint16_t data);

/**
 * @brief   Initialise the board for the display.
 *
 * @param[in] g			The GDisplay structure
 *
 * @note	Set the g->board member to whatever is appropriate. For multiple
 * 			displays this might be a pointer to the appropriate register set.
 *
 * @notapi
 */
static GFXINLINE void init_board(GDisplay *g) {

  // As we are not using multiple displays we set g->board to NULL as we don't use it.
  g->board = 0;

  palSetPadMode(LCD_PORT, LCD_CS, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(LCD_PORT, LCD_DC, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(LCD_PORT, LCD_RES, PAL_MODE_OUTPUT_PUSHPULL);

  spiStart(&SPID2, &spi2cfg);
  spiSelectI(&SPID2);
}

/**
 * @brief   After the initialisation.
 *
 * @param[in] g			The GDisplay structure
 *
 * @notapi
 */
static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

/**
 * @brief   Set or clear the lcd reset pin.
 *
 * @param[in] g			The GDisplay structure
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * 
 * @notapi
 */
static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;

	if (state == TRUE) {
		palClearPad(LCD_PORT, LCD_RES);
	} else {
		palSetPad(LCD_PORT, LCD_RES);
	}
}

/**
 * @brief   Set the lcd back-light level.
 *
 * @param[in] g				The GDisplay structure
 * @param[in] percent		0 to 100%
 * 
 * @notapi
 */
static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

/**
 * @brief   Take exclusive control of the bus
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}

/**
 * @brief   Release exclusive control of the bus
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
}

/**
 * @brief   Send data to the lcd.
 *
 * @param[in] data			The data to send
 * 
 * @notapi
 */
static GFXINLINE void send_data(uint16_t data) {
// http://forum.easyelectronics.ru/viewtopic.php?p=262122#p262122
  while (!(SPI2->SR & SPI_SR_TXE)); // при входе на отправку проверяем - а пустой ли SPI_DR
  SPI2->DR = data; // загрузили в SPI_DR код команды

}

/**
 * @brief   Send data to the index register.
 *
 * @param[in] g				The GDisplay structure
 * @param[in] index			The index register to set
 *
 * @notapi
 */
static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;

  while (SPI2->SR & SPI_SR_BSY);
  LCD_CS_RES;
  LCD_DC_CMD; // переводим дисплей в режим команд
  LCD_CS_SET;
  send_data(index);
  while (SPI2->SR & SPI_SR_BSY); // пока флаг установлен (==1) -- модуль SPI занят
  /* лишний цикл ожидания окончания передачи команды позволяет в дальнейшем слать
   * байты данных без ненужных ожиданий и задержек.
   */
  LCD_DC_DATA; // переводим дисплей в режим данных
}

/**
 * @brief   Send data to the lcd with DC control.
 *
 * @param[in] g				The GDisplay structure
 * @param[in] data			The data to send
 * 
 * @notapi
 */
static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;

  send_data(data);
}

/**
 * @brief   Set the bus in read mode
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

/**
 * @brief   Set the bus back into write mode
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}

/**
 * @brief   Read data from the lcd.
 * @return	The data from the lcd
 *
 * @param[in] g				The GDisplay structure
 *
 * @notapi
 */
static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;
	return 0;
}

#endif /* _GDISP_LLD_BOARD_H */

