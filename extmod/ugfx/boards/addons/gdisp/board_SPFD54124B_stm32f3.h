/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 * 
 * Mail: fede.677387@hotmail.it
 *
 * Board: STM32F3-Discovery
 */

/**
 * @file    boards/addons/gdisp/board_SPFD54124B_stm32f3.h
 * @brief   GDISP Graphic Driver subsystem board interface for the SPFD54124B display.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define SPFD54124B_SPID         SPID2

#define SPFD54124B_SPI_PORT	    GPIOB
#define SPFD54124B_SPI_NSS		12
#define SPFD54124B_SPI_SCK		13
#define SPFD54124B_SPI_MISO	    14 // not used
#define SPFD54124B_SPI_MOSI	    15

#define SPFD54124B_PIN_PORT	    GPIOA
#define SPFD54124B_PIN_RST		5

#define SET_RST				palSetPad(SPFD54124B_PIN_PORT, SPFD54124B_PIN_RST);
#define CLR_RST				palClearPad(SPFD54124B_PIN_PORT, SPFD54124B_PIN_RST);

#define USE_SOFT_SPI            TRUE
#define USE_HARD_SPI            !(USE_SOFT_SPI)

#if USE_HARD_SPI

#if GFX_USE_OS_CHIBIOS
static int32_t thdPriority = 0;
#endif

/*
 * Maximum speed SPI configuration in 9 bit mode
 */
static const SPIConfig hs_spicfg = {
                                    NULL,                   /* Operation complete callback or @p NULL. */
                                    SPFD54124B_SPI_PORT,    /* The chip select line port */
                                    SPFD54124B_SPI_NSS,     /* The chip select line pad number */
                                    0,                      /* SPI CR1 register initialization data*/
                                    SPI_CR2_DS_3            /* SPI CR2 register initialization data 9-bit */
};
#endif

#if USE_SOFT_SPI
static GFXINLINE void soft_spi_sck(void){
  palSetPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_SCK);
  palClearPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_SCK);
}

static GFXINLINE void soft_spi_write_9bit(uint16_t data){

  uint8_t i;

  // activate lcd by low on CS pin
  palClearPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_NSS);

  for (i=0; i<9; i++){
      // setting data
      if(data & (SPFD54124B_SEND_DATA >> i)) {
        palSetPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_MOSI);
      }
      else palClearPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_MOSI);
      // clock data
      soft_spi_sck();
  }

  //deactivate lcd by high on CS pin
  palSetPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_NSS);

}
#endif

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
  (void) g;
  if(state) {
    CLR_RST;
  } else {
    SET_RST;
  }
}

static GFXINLINE void init_board(GDisplay *g) {
  // As we are not using multiple displays we set g->board to NULL as we don't use it.
  g->board = 0;

  switch(g->controllerdisplay) {
  case 0:											// Set up for Display 0
    /*
     * SPI1 I/O pins setup.
     */
    palSetPadMode(SPFD54124B_PIN_PORT, SPFD54124B_PIN_RST,  PAL_MODE_OUTPUT_PUSHPULL);            /* RESET */
    setpin_reset(g, TRUE);

#if USE_HARD_SPI
    palSetPadMode(SPFD54124B_SPI_PORT, SPFD54124B_SPI_SCK,  PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   /* SCK. */
    palSetPadMode(SPFD54124B_SPI_PORT, SPFD54124B_SPI_MISO, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   /* MISO.*/
    palSetPadMode(SPFD54124B_SPI_PORT, SPFD54124B_SPI_MOSI, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);   /* MOSI.*/
    palSetPadMode(SPFD54124B_SPI_PORT, SPFD54124B_SPI_NSS,  PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);   /* NSS  */
    palSetPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_NSS);
    palClearPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_SCK);
    palClearPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_MOSI);
    spiInit();
#endif

#if USE_SOFT_SPI
    palSetPadMode(SPFD54124B_SPI_PORT, SPFD54124B_SPI_SCK,  PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);   /* SCK. */
    palSetPadMode(SPFD54124B_SPI_PORT, SPFD54124B_SPI_MISO, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);   /* MISO.*/
    palSetPadMode(SPFD54124B_SPI_PORT, SPFD54124B_SPI_MOSI, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);   /* MOSI.*/
    palSetPadMode(SPFD54124B_SPI_PORT, SPFD54124B_SPI_NSS,  PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);   /* NSS  */
    palSetPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_NSS);
    palClearPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_SCK);
    palClearPad(SPFD54124B_SPI_PORT, SPFD54124B_SPI_MOSI);
#endif

    break;
  }
}

static GFXINLINE void acquire_bus(GDisplay *g) {
  (void) g;
#if USE_HARD_SPI
#if GFX_USE_OS_CHIBIOS
  thdPriority = (int32_t)chThdGetPriority();
  chThdSetPriority(HIGHPRIO);
#endif
  spiAcquireBus(&SPFD54124B_SPID);
#endif
}

static GFXINLINE void release_bus(GDisplay *g) {
  (void) g;
#if USE_HARD_SPI
#if GFX_USE_OS_CHIBIOS
  chThdSetPriority(thdPriority);
#endif
  spiReleaseBus(&SPFD54124B_SPID);
#endif
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
  (void) g;

  uint16_t b;

#if USE_HARD_SPI

  spiStart(&SPFD54124B_SPID, &hs_spicfg);
  spiSelect(&SPFD54124B_SPID);

  b = (data >> 0x08) | SPFD54124B_SEND_DATA;
  spiSend(&SPFD54124B_SPID, 0x01, &b);
  b = (data &  0xFF) | SPFD54124B_SEND_DATA;
  spiSend(&SPFD54124B_SPID, 0x01, &b);

  spiUnselect(&SPFD54124B_SPID);
  spiStop(&SPFD54124B_SPID);

#endif
#if USE_SOFT_SPI
  b = (data >> 0x08) | SPFD54124B_SEND_DATA;
  soft_spi_write_9bit(b);
  b = (data &  0xFF) | SPFD54124B_SEND_DATA;
  soft_spi_write_9bit(b);
#endif

}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
  (void)    g;

#if USE_HARD_SPI
  spiStart(&SPFD54124B_SPID, &hs_spicfg);
  spiSelect(&SPFD54124B_SPID);
  spiSend(&SPFD54124B_SPID, 0x01, &index);
  spiUnselect(&SPFD54124B_SPID);
  spiStop(&SPFD54124B_SPID);
#endif
#if USE_SOFT_SPI
  soft_spi_write_9bit(index);
#endif
}

static GFXINLINE void post_init_board(GDisplay *g) {
  (void) g;
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
  (void) g;
  (void) percent;
}

#endif /* _GDISP_LLD_BOARD_H */
