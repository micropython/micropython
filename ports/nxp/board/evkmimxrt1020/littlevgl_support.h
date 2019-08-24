/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LITTLEVGL_SUPPORT_H
#define LITTLEVGL_SUPPORT_H

#include <stdint.h>
#include "lvgl.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define LCD_FB_BYTE_PER_PIXEL 2
/* The virtual buffer for DBI panel, it should be ~1/10 screen size. */
#define LCD_VIRTUAL_BUF_SIZE (LCD_WIDTH * LCD_HEIGHT / 10)

/* Select PLL2_MAIN_CLK (528 MHz) as master lpspi clock source */
#define LPSPI_CLOCK_SOURCE_SELECT (2U)
/* Clock divider for master lpspi clock source */
#define LPSPI_CLOCK_SOURCE_DIVIDER (5U)
/* Select OSC_CLK (24 MHz) as master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_SELECT (1U)
/* Clock divider for master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_DIVIDER (1U)

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void lv_port_pre_init(void);
void lv_port_disp_init(void);
void lv_port_indev_init(void);
void DEMO_InitLcd(void);
void DEMO_InitTouch(void);
bool DEMO_ReadTouch(lv_indev_data_t *data);
void DEMO_FlushDisplay(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);



#if defined(__cplusplus)
}
#endif

#endif /*LITTLEVGL_SUPPORT_H */
