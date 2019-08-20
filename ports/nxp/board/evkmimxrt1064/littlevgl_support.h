/*
 * Copyright 2018 NXP
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

#define LCD_WIDTH 480
#define LCD_HEIGHT 272
#define LCD_FB_BYTE_PER_PIXEL 2

/*******************************************************************************
 * API
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void DEMO_InitLcd(void);
void lv_port_pre_init(void);
void lv_port_disp_init(void);
void lv_port_indev_init(void);
void DEMO_FlushDisplay(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t *color_p);
void DEMO_InitTouch(void);
bool DEMO_ReadTouch(lv_indev_data_t *data);

#if defined(__cplusplus)
}
#endif

#endif /*LITTLEVGL_SUPPORT_H */
