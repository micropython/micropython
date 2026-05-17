/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Renesas Electronics Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef PORTS_RA_RA4M1_HAL_H_
#define PORTS_RA_RA4M1_HAL_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
// #include "hal_data.h"
#include "bsp_api.h"
#include "common_data.h"
#if defined(USE_DBG_PRINT)
#include RA_CFG_H
#endif

#define SCI_CH      DEFAULT_DBG_CH
#define SCI_BAUD    115200
#define UART_CH     SCI_CH
#define UART_TxStr  sci_tx_str
#define PCLK        48000000

#define RA_PRI_SYSTICK  (0)
#define RA_PRI_UART     (1)
#define RA_PRI_SDIO     (4)
#define RA_PRI_DMA      (5)
#define RA_PRI_FLASH    (6)
#define RA_PRI_OTG_FS   (6)
#define RA_PRI_OTG_HS   (6)
#define RA_PRI_TIM5     (6)
#define RA_PRI_CAN      (7)
#define RA_PRI_SPI      (8)
#define RA_PRI_I2C      (8)
#define RA_PRI_TIMX     (13)
#define RA_PRI_EXTINT   (14)
#define RA_PRI_PENDSV   (15)
#define RA_PRI_RTC_WKUP (15)

#include "ra_config.h"
#include "ra_adc.h"
#include "ra_dac.h"
#include "ra_flash.h"
#include "ra_gpio.h"
#include "ra_gpt.h"
#include "ra_i2c.h"
#include "ra_icu.h"
#include "ra_init.h"
#include "ra_int.h"
#include "ra_rtc.h"
#include "ra_sci.h"
#include "ra_spi.h"
#include "ra_timer.h"
#include "ra_utils.h"

typedef enum {
    HAL_OK       = 0x00,
    HAL_ERROR    = 0x01,
    HAL_BUSY     = 0x02,
    HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

#define __IO    volatile

#if defined(USE_DBG_PRINT)
#if !defined(DEBUG_CH)
#define DEBUG_CH SCI_CH
#endif
#if (DEBUG_CH == 0)
#define DEBUG_TX_PIN    P411
#define DEBUG_RX_PIN    P410
#endif
#if (DEBUG_CH == 1)
#define DEBUG_TX_PIN    P401
#define DEBUG_RX_PIN    P402
#endif
#if (DEBUG_CH == 9)
#define DEBUG_TX_PIN    P203
#define DEBUG_RX_PIN    P202
#endif
#define DEBUG_TXSTR(s)  ra_sci_tx_str(DEBUG_CH, (unsigned char *)s)
#define DEBUG_TXCH(c)   ra_sci_tx_ch(DEBUG_CH, c)
#else
#define DEBUG_TXSTR(s)
#define DEBUG_TXCH(c)
#endif

#endif /* PORTS_RA_RA4M1_HAL_H_ */
