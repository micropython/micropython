/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Blues Wireless Contributors
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

#include "stm32l4xx_hal.h"

// Chip:                STM32L4R5
// Line Type:           Foundation Line
// Speed:               120MHz (MAX)

// Defaults:
#ifndef CPY_CLK_VSCALE
#define CPY_CLK_VSCALE (PWR_REGULATOR_VOLTAGE_SCALE1_BOOST)      // up to 120MHz
#endif
#ifndef CPY_CLK_PLLM
#define CPY_CLK_PLLM (12)
#endif
#ifndef CPY_CLK_PLLN
#define CPY_CLK_PLLN (60)
#endif
#ifndef CPY_CLK_PLLP
#define CPY_CLK_PLLP (RCC_PLLP_DIV2)
#endif
#ifndef CPY_CLK_PLLQ
#define CPY_CLK_PLLQ (2)
#endif
#ifndef CPY_CLK_AHBDIV
#define CPY_CLK_AHBDIV (RCC_SYSCLK_DIV1)
#endif
#ifndef CPY_CLK_APB1DIV
#define CPY_CLK_APB1DIV (RCC_HCLK_DIV1)
#endif
#ifndef CPY_CLK_APB2DIV
#define CPY_CLK_APB2DIV (RCC_HCLK_DIV1)
#endif
#ifndef CPY_CLK_FLASH_LATENCY
#define CPY_CLK_FLASH_LATENCY (FLASH_LATENCY_5)
#endif
#ifndef CPY_CLK_USB_USES_AUDIOPLL
#define CPY_CLK_USB_USES_AUDIOPLL (0)
#endif

#ifndef BOARD_HAS_HIGH_SPEED_CRYSTAL
#define BOARD_HAS_HIGH_SPEED_CRYSTAL (1)
#endif
