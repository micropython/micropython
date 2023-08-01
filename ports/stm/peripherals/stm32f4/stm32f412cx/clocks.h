/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
<<<<<<<< HEAD:ports/stm/peripherals/stm32f4/stm32f412cx/clocks.h
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
========
 * Copyright (c) 2013, 2014 Damien P. George
>>>>>>>> v1.19.1:ports/renesas-ra/gccollect.c
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

<<<<<<<< HEAD:ports/stm/peripherals/stm32f4/stm32f412cx/clocks.h
#include "stm32f4xx_hal.h"

// Chip:                STM32F412Cx
// Line Type:           Access Line
// Speed:               100MHz (MAX)

// Note - uses the I2S PLL for USB to enable full 100MHz operation, since USB
// can't get the right divisors from 100MHz PLL settings.

// Defaults:
#ifndef CPY_CLK_VSCALE
#define CPY_CLK_VSCALE (PWR_REGULATOR_VOLTAGE_SCALE1)
#endif
#ifndef CPY_CLK_PLLN
#define CPY_CLK_PLLN (200)
#endif
#ifndef CPY_CLK_PLLP
#define CPY_CLK_PLLP (RCC_PLLP_DIV2)
#endif
#ifndef CPY_CLK_PLLQ
#define CPY_CLK_PLLQ (7)
#endif
#ifndef CPY_CLK_AHBDIV
#define CPY_CLK_AHBDIV (RCC_SYSCLK_DIV1)
#endif
#ifndef CPY_CLK_APB1DIV
#define CPY_CLK_APB1DIV (RCC_HCLK_DIV2)
#endif
#ifndef CPY_CLK_APB2DIV
#define CPY_CLK_APB2DIV (RCC_HCLK_DIV1)
#endif
#ifndef CPY_CLK_FLASH_LATENCY
#define CPY_CLK_FLASH_LATENCY (FLASH_LATENCY_3)
#endif
#ifndef CPY_CLK_USB_USES_AUDIOPLL
#define CPY_CLK_USB_USES_AUDIOPLL (1)
#endif
#ifndef BOARD_HSE_SOURCE
#define BOARD_HSE_SOURCE (RCC_HSE_ON)
#endif
========
#include <stdio.h>
#include <stdint.h>

#include "py/mpstate.h"
#include "py/gc.h"
#include "py/mpthread.h"
#include "shared/runtime/gchelper.h"
#include "gccollect.h"
#include "softtimer.h"
#include "systick.h"

void gc_collect(void) {
    // get current time, in case we want to time the GC
    #if 0
    uint32_t start = mp_hal_ticks_us();
    #endif

    // start the GC
    gc_collect_start();

    // trace the stack and registers
    gc_helper_collect_regs_and_stack();

    // trace root pointers from any threads
    #if MICROPY_PY_THREAD
    mp_thread_gc_others();
    #endif

    // trace soft timer nodes
    soft_timer_gc_mark_all();

    // end the GC
    gc_collect_end();

    #if 0
    // print GC info
    uint32_t ticks = mp_hal_ticks_us() - start;
    gc_info_t info;
    gc_info(&info);
    printf("GC@%lu %lums\n", start, ticks);
    printf(" " UINT_FMT " total\n", info.total);
    printf(" " UINT_FMT " : " UINT_FMT "\n", info.used, info.free);
    printf(" 1=" UINT_FMT " 2=" UINT_FMT " m=" UINT_FMT "\n", info.num_1block, info.num_2block, info.max_block);
    #endif
}
>>>>>>>> v1.19.1:ports/renesas-ra/gccollect.c
