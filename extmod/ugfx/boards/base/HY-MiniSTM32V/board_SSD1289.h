/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef GDISP_LLD_BOARD_H
#define GDISP_LLD_BOARD_H

/*
 * Board file for HY-MiniSTM32V board from HAOYU (China).
 * www.powermcu.com or www.hotmcu.com.
 */

/*
 * NOTE: In order to make this work you need to set:
 *   STM32_PWM_USE_TIM3     TRUE    in mcuconf.h
 *   HAL_USE_PWM            TRUE    in halconf.h
 */

/*
 * TM3 ch2 is connected to LCD BL_CNT (PB5)
 */
static const PWMConfig pwmcfg =
{
 100000,   // frequency
 100,      // period
 NULL,     // callback
 {
  {PWM_OUTPUT_DISABLED, 0},
  {PWM_OUTPUT_ACTIVE_HIGH, 0},
  {PWM_OUTPUT_DISABLED, 0},
  {PWM_OUTPUT_DISABLED, 0}
 },
 0,        // cr2
 0,        // dier
};

/*
 * LCD_RS is on A16 (PD11)
 */
#define GDISP_REG   (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
#define GDISP_RAM   (*((volatile uint16_t *) 0x60020000)) /* RS = 1 */
/*
 * STM32_DMA1_STREAM7
 * NOTE: conflicts w/ USART2_TX, TIM2_CH2, TIM2_CH4, TIM4_UP, I2C1_RX in case
 */
#define GDISP_DMA_STREAM        STM32_DMA1_STREAM7
#define FSMC_BANK               0

static GFXINLINE void init_board(GDisplay *g) {
  /*
   * As we are not using multiple displays we set g->board to NULL as we don't
   * use it.
   */
  g->board = 0;

  switch(g->controllerdisplay) {
  /*
   * Set up for Display 0
   */
  case 0:
    /* FSMC setup for F1 */
    rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

    /* Group pins for FSMC setup as alternate function */
    IOBus busD = { GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) |          \
                   (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) |    \
                   (1 << 14) | (1 << 15), 0 };

    IOBus busE = { GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) |         \
                   (1 << 11) | (1 << 12) | (1 << 13) | (1 << 14) |             \
                   (1 << 15), 0 };

    /* FSMC sa alternate function */
    palSetBusMode(&busD, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    palSetBusMode(&busE, PAL_MODE_STM32_ALTERNATE_PUSHPULL);

    /*
     * NOTE: stm32F10x.h is FAULTY on FSMC
     * NOTE: Used hardcore bit shifting below
     * NOTE: All timings for 72MHz HCLK - should be revised for lower HCLK
     */

    /* FSMC timing - Read: DATAST = 0x20; all the rest = 0.
     * 100ns cycle time for SSD1289 as per DataSheet
     */
    FSMC_Bank1->BTCR[FSMC_BANK+1] = (0x20 << 8);

    /* FSMC timing - Write: DATAST = 0x01, ADDSET = 0x01 all the rest = 0.
     * 1000ns cycle time for SSD1289 as per DataSheet
     */
    FSMC_Bank1E->BWTR[FSMC_BANK] = (0x1 << 8) | (0x01 << 0);

    /* Bank1 NOR/SRAM control register configuration
     * Note: different read and write cycle timing
     */
    FSMC_Bank1->BTCR[FSMC_BANK] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN |          \
        FSMC_BCR1_MBKEN | FSMC_BCR1_EXTMOD;

    /* DMA Setup. */
#if defined(GDISP_USE_DMA) && defined(GDISP_DMA_STREAM)
    if (dmaStreamAllocate(GDISP_DMA_STREAM, 0, 0, 0))
      gfxExit();

    dmaStreamSetMemory0(GDISP_DMA_STREAM, &GDISP_RAM);
    dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) |                    \
                     STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD |     \
                     STM32_DMA_CR_DIR_M2M);
#endif

    /* Display backlight control */
    /* TIM3 ch2 (PB5) connected to LCD BL_CNT */
    pwmStart(&PWMD3, &pwmcfg);
    palSetPadMode(GPIOB, 5, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    pwmEnableChannel(&PWMD3, 1, 100);

    break;
  }
}

static GFXINLINE void post_init_board(GDisplay *g) {
  (void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
  (void) g;

  if(state) {}
  else {}
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
  (void) g;
  if (percent > 100) { percent = 100; }
  pwmEnableChannel(&PWMD3, 1, percent);
}

static GFXINLINE void acquire_bus(GDisplay *g) {
  (void) g;
}

static GFXINLINE void release_bus(GDisplay *g) {
  (void) g;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
  (void) g;
  GDISP_REG = index;
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
  (void) g;
  GDISP_RAM = data;
}

static GFXINLINE void setreadmode(GDisplay *g) {
  (void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
  (void) g;
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
  (void) g;
  return GDISP_RAM;
}

#if defined(GDISP_USE_DMA) || defined(__DOXYGEN__)
static GFXINLINE void dma_with_noinc(GDisplay *g, color_t *buffer, int area) {
  (void) g;
  dmaStreamSetPeripheral(GDISP_DMA_STREAM, buffer);
  dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) |                      \
                   STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD |       \
                   STM32_DMA_CR_DIR_M2M);
  for (; area > 0; area -= 65535) {
    dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area > 65535 ? 65535 : area);
    dmaStreamEnable(GDISP_DMA_STREAM);
    dmaWaitCompletion(GDISP_DMA_STREAM);
  }
}

static GFXINLINE void dma_with_inc(GDisplay *g, color_t *buffer, int area) {
  (void) g;
  dmaStreamSetPeripheral(GDISP_DMA_STREAM, buffer);
  dmaStreamSetMode(GDISP_DMA_STREAM, STM32_DMA_CR_PL(0) | STM32_DMA_CR_PINC |  \
                   STM32_DMA_CR_PSIZE_HWORD | STM32_DMA_CR_MSIZE_HWORD |       \
                   STM32_DMA_CR_DIR_M2M);
  for (; area > 0; area -= 65535) {
    dmaStreamSetTransactionSize(GDISP_DMA_STREAM, area > 65535 ? 65535 : area);
    dmaStreamEnable(GDISP_DMA_STREAM);
    dmaWaitCompletion(GDISP_DMA_STREAM);
  }
}
#endif

#endif /* GDISP_LLD_BOARD_H */
