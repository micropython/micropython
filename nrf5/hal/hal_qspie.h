/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#ifndef HAL_QSPIE_H__
#define HAL_QSPIE_H__

#if NRF52840_XXAA

#include <nrf.h>

#else
#error "Device not supported."
#endif

/**
  * @brief  Quad SPI clock frequency type definition
  */
typedef enum {
    HAL_FREQ_2_Mbps,
    HAL_FREQ_4_Mbps,
    HAL_FREQ_8_Mbps,
    HAL_FREQ_16_Mbps,
    HAL_FREQ_32_Mbps
} hal_qspi_clk_freq_t;

/**
  * @brief  Quad SPI mode configuration type definition
  */
typedef enum {
    HAL_QSPI_MODE_SINGLE_LINE,
    HAL_QSPI_MODE_DUAL_LINE,
    HAL_QSPI_MODE_QUAL_LINE
} hal_qspi_mode_t;

#endif // HAL_QSPIE_H__
