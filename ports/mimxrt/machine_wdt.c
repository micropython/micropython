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

#include "fsl_wdog.h"

#define MIN_TIMEOUT    (500)
#define MAX_TIMEOUT    (128000)

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
} machine_wdt_obj_t;

static const machine_wdt_obj_t machine_wdt = {{&machine_wdt_type}};

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms) {
    // Verify the WDT id.
    if (id != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT(%d) doesn't exist"), id);
    }

    // Start the watchdog (timeout is in milliseconds).
    wdog_config_t config;
    WDOG_GetDefaultConfig(&config);
    // confine to the valid range
    if (timeout_ms < MIN_TIMEOUT) {
        timeout_ms = MIN_TIMEOUT;
    } else if (timeout_ms > MAX_TIMEOUT) {
        timeout_ms = MAX_TIMEOUT;
    }
    config.timeoutValue = (timeout_ms / 500) - 1;
    WDOG_Init(WDOG1, &config);

    return (machine_wdt_obj_t *)&machine_wdt;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    (void)self;
    WDOG_Refresh(WDOG1);
}

static void mp_machine_wdt_timeout_ms_set(machine_wdt_obj_t *self, mp_int_t timeout_ms) {
    (void)self;

    // confine to the valid range
    if (timeout_ms < MIN_TIMEOUT) {
        timeout_ms = MIN_TIMEOUT;
    } else if (timeout_ms > MAX_TIMEOUT) {
        timeout_ms = MAX_TIMEOUT;
    }
    uint32_t timeout = (timeout_ms / 500) - 1;
    WDOG_SetTimeoutValue(WDOG1, (uint16_t)timeout);
    WDOG_Refresh(WDOG1);
}
