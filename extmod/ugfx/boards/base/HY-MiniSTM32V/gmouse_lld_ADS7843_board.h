/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/*
 * Board file for HY-MiniSTM32V board from HAOYU (China).
 * www.powermcu.com or www.hotmcu.com.
 */

/*
 * NOTE: In order to make this work you need to set:
 *   STM32_SPI_USE_SPI1     TRUE    in mcuconf.h
 *   HAL_USE_SPI            TRUE    in halconf.h
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
#define GMOUSE_ADS7843_BOARD_DATA_SIZE   0

static const SPIConfig spicfg = {
  0,
  GPIOA,
  GPIOA_TP_CS,
  SPI_CR1_BR_1 | SPI_CR1_BR_0,  /* Might be tweaked for faster transfer. */
};

/*
 * ADS7843 (clone chip) is connected to SPI1 w/o remap
 * TP_CS         PA4
 * SPI_SCK      PA5
 * SPI_MISO     PA6
 * SPI_MOSI     PA7
 * TP_IRQ       PB6
 */

static bool_t init_board(GMouse* m, unsigned driverinstance) {
  (void) m;
  (void) driverinstance;

  palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(GPIOA, 5, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(GPIOA, 6, PAL_MODE_INPUT_PULLUP);
  palSetPadMode(GPIOA, 7, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(GPIOB, 6, PAL_MODE_INPUT);

  spiStart(&SPID1, &spicfg);
  return true;
}

/*
 * PB6 is connected to TP_IRQ (low active).
 */
static GFXINLINE bool_t getpin_pressed(GMouse* m) {
  (void)m;
  return (!palReadPad(GPIOB, 6));
}

/*
 * PA4 is connected to TP_CS (low active):
 */
static GFXINLINE void aquire_bus(GMouse* m) {
  (void)m;
  spiAcquireBus(&SPID1);
  palClearPad(GPIOA, 4);
}

static GFXINLINE void release_bus(GMouse* m) {
  (void)m;
  palSetPad(GPIOA, 4);
  spiReleaseBus(&SPID1);
}

static GFXINLINE uint16_t read_value(GMouse* m, uint16_t port) {
  (void)m;
  static uint8_t txbuf[3] = {0};
  static uint8_t rxbuf[3] = {0};
  uint16_t ret;

  txbuf[0] = port;
  spiExchange(&SPID1, 3, txbuf, rxbuf);
  ret = (rxbuf[1] << 5) | (rxbuf[2] >> 3);
  return ret;
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
