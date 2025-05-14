/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.

#include "sam.h"

#define MIN_TIMEOUT         512
#define MAX_TIMEOUT         16384

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
} machine_wdt_obj_t;

// Calculate the floor value of log2(n)
mp_int_t log2i(mp_int_t num) {
    mp_int_t res = 0;
    for (; num > 1; num >>= 1) {
        res += 1;
    }
    return res;
}

static const machine_wdt_obj_t machine_wdt = {{&machine_wdt_type}};

static void set_timeout(uint32_t timeout) {
    // Set new timeout. Have to disable WDT first.

    // Confine to the valid range
    if (timeout < MIN_TIMEOUT) {
        timeout = MIN_TIMEOUT;
    } else if (timeout > MAX_TIMEOUT) {
        timeout = MAX_TIMEOUT;
    }

    #if defined(MCU_SAMD21)
    WDT->CTRL.reg = 0;
    while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY) {
    }
    WDT->CONFIG.reg = log2i(timeout) - 3;
    WDT->CTRL.reg = WDT_CTRL_ENABLE;
    while (WDT->STATUS.reg & WDT_STATUS_SYNCBUSY) {
    }
    #elif defined(MCU_SAMD51)
    WDT->CTRLA.reg = 0;
    while (WDT->SYNCBUSY.reg & WDT_SYNCBUSY_ENABLE) {
    }
    WDT->CONFIG.reg = log2i(timeout) - 3;
    WDT->CTRLA.reg = WDT_CTRLA_ENABLE;
    while (WDT->SYNCBUSY.reg & WDT_SYNCBUSY_ENABLE) {
    }
    #endif
}

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms) {
    #if defined(MCU_SAMD51)
    // Verify the WDT id. SAMD51 only, saving a few bytes for SAMD21
    if (id != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT(%d) doesn't exist"), id);
    }
    #endif

    // Start the watchdog (timeout is in milliseconds).
    // Configure the WDT
    #if defined(MCU_SAMD21)

    // Enable APBx clocks and GCLK clock
    PM->APBAMASK.reg |= PM_APBAMASK_WDT;
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK8 | GCLK_CLKCTRL_ID_WDT;

    #elif defined(MCU_SAMD51)

    // Enable APBx clocks and 1kHz clock
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_WDT;
    OSC32KCTRL->OSCULP32K.bit.EN1K = 1;

    #endif

    set_timeout(timeout_ms);

    return (machine_wdt_obj_t *)&machine_wdt;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    (void)self;
    WDT->CLEAR.reg = 0xa5;
}

static void mp_machine_wdt_timeout_ms_set(machine_wdt_obj_t *self, mp_int_t timeout_ms) {
    (void)self;
    set_timeout(timeout_ms);
}
