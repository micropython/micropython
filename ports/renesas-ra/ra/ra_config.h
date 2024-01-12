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

#ifndef RA_RA_CONFIG_H_
#define RA_RA_CONFIG_H_

#include <stdint.h>
#include "py/mpconfig.h"

#if defined(RA4M1) | defined(RA4W1)
#define SCI_CH_MAX      10
#define SCI_CH_NUM      4
#define SCI_TX_BUF_SIZE 128
#define SCI_RX_BUF_SIZE 256
#define PCLK         48000000
#elif defined(RA6M1) | defined(RA6M2)
#define SCI_CH_MAX   10
#define SCI_CH_NUM   4
#define SCI_TX_BUF_SIZE 128
#define SCI_RX_BUF_SIZE 256
#define PCLK         120000000
#elif defined(RA6M5)
#define SCI_CH_MAX   10
#define SCI_CH_NUM   4
#define SCI_TX_BUF_SIZE 128
#define SCI_RX_BUF_SIZE 256
#define PCLK         100000000
#else
#error "CMSIS MCU Series is not specified."
#endif

#if defined(MICROPY_HW_MCU_PCLK)
#undef PCLK
#define PCLK MICROPY_HW_MCU_PCLK
#endif

#define SCI_DEF_BAUD (115200)

#if !defined(RA_PRI_SYSTICK)
#define RA_PRI_SYSTICK (0)
#endif
#if !defined(RA_PRI_UART)
#define RA_PRI_UART (1)
#endif
#if !defined(RA_PRI_SDIO)
#define RA_PRI_SDIO (4)
#endif
#if !defined(RA_PRI_DMA)
#define RA_PRI_DMA (5)
#endif
#if !defined(RA_PRI_FLASH)
#define RA_PRI_FLASH (6)
#endif
#if !defined(RA_PRI_OTG_FS)
#define RA_PRI_OTG_FS (6)
#endif
#if !defined(RA_PRI_OTG_HS)
#define RA_PRI_OTG_HS (6)
#endif
#if !defined(RA_PRI_TIM5)
#define RA_PRI_TIM5 (6)
#endif
#if !defined(RA_PRI_CAN)
#define RA_PRI_CAN (7)
#endif
#if !defined(RA_PRI_SPI)
#define RA_PRI_SPI (8)
#endif
#if !defined(RA_PRI_I2C)
#define RA_PRI_I2C (8)
#endif
#if !defined(RA_PRI_TIMX)
#define RA_PRI_TIMX (13)
#endif
#if !defined(RA_PRI_EXTINT)
#define RA_PRI_EXTINT (14)
#endif
#if !defined(RA_PRI_PENDSV)
#define RA_PRI_PENDSV (15)
#endif
#if !defined(RA_PRI_RTC_WKUP)
#define RA_PRI_RTC_WKUP (15)
#endif

#if !defined(__WEAK)
#define __WEAK __attribute__((weak))
#endif

#endif /* RA_RA_CONFIG_H_ */
