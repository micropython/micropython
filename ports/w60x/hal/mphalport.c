/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "wm_osal.h"
#include "wm_cpu.h"
#include "wm_regs.h"
#include "wm_watchdog.h"
#include "wm_irq.h"

#include "py/obj.h"
#include "py/mpstate.h"
#include "py/mphal.h"
#include "extmod/misc.h"
#include "lib/utils/pyexec.h"
#include "mphalport.h"

#define CNT_START_VALUE (0xffffffff)
static uint32_t timer_ovfl = 0;
static uint32_t ticks_per_us = 40;
uint32_t ticks_us_max_value;

void timer_init0() {

    tls_sys_clk sysclk;
    tls_sys_clk_get(&sysclk);
    ticks_per_us = sysclk.apbclk;

    tls_reg_write32(HR_WDG_LOAD_VALUE, CNT_START_VALUE);
    tls_reg_write32(HR_WDG_CTRL, 0x1);              /* enable irq */
    ticks_us_max_value = CNT_START_VALUE / ticks_per_us;
}


uint32_t mp_hal_ticks_ms(void) {
    return tls_os_get_time() * (1000 / HZ);
}

uint32_t mp_hal_ticks_us(void) {
    return (CNT_START_VALUE - tls_reg_read32(HR_WDG_CUR_VALUE)) / ticks_per_us;
}

uint32_t mp_hal_ticks_cpu(void) {
    return CNT_START_VALUE - tls_reg_read32(HR_WDG_CUR_VALUE);
}

STATIC inline void delay(uint32_t us) {
    if (us < 20) {
        volatile int32_t loops = ((int32_t)us - 2) * 6 + us/10 * 6;
        while(loops > 0) {
            loops--;
        }
    } else {
        uint32_t start = tls_reg_read32(HR_WDG_CUR_VALUE);
        uint32_t diff = (us - 4) * ticks_per_us;
        while((start - tls_reg_read32(HR_WDG_CUR_VALUE)) < diff) {
            ;
        }
    }
}

STATIC void __mp_hal_delay_ms(uint32_t ms) {
    if (ms / (1000 / HZ) > 0) {
        tls_os_time_delay(ms / (1000 / HZ));
    } else {
        delay(ms * 1000);
    }
}

void mp_hal_delay_ms(uint32_t ms) {
    if (!tls_get_isr_count()) {
        // IRQs enabled, so can use systick counter to do the delay
        uint32_t start = tls_os_get_time();
        // Wraparound of tick is taken care of by 2's complement arithmetic.
        while ((tls_os_get_time() - start) < (ms / (1000 / HZ))) {
            // This macro will execute the necessary idle behaviour.  It may
            // raise an exception, switch threads or enter sleep mode (waiting for
            // (at least) the SysTick interrupt).
            MICROPY_EVENT_POLL_HOOK
            tls_os_time_delay(1);
        }

        if (ms < 2) {
            MICROPY_EVENT_POLL_HOOK
            tls_os_time_delay(1);
        }
    } else {
        __mp_hal_delay_ms(ms);
    }
}

void mp_hal_delay_us(uint32_t us) {
    if ((us / 1000) > 3) {
        mp_hal_delay_ms(us / 1000);
        delay(us % 1000);
    } else {
        delay(us);
    }
}

void mp_hal_delay_us_fast(uint32_t us) {
    delay(us);
}

uint32_t mp_hal_get_cpu_freq(void) {
    tls_sys_clk sysclk;
    tls_sys_clk_get(&sysclk);
    return sysclk.cpuclk;
}

