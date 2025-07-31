/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Daniel Campora on behalf of REMOTE TECH LTD
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

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/sys/printk.h>
#include <stdbool.h>

#include "py/mperrno.h"

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    struct device *wdt;
    int32_t channel_id;
} machine_wdt_obj_t;

static machine_wdt_obj_t wdt_default = {
    {&machine_wdt_type}, NULL, -1
};

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms) {
    if (id != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid WDT id"));
    }

    if (timeout_ms <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("watchdog timeout too short"));
    }

    wdt_default.wdt = (struct device *)DEVICE_DT_GET(DT_ALIAS(watchdog0));

    if (!device_is_ready(wdt_default.wdt)) {
        mp_raise_OSError(MP_ENODEV);
    }

    struct wdt_timeout_cfg wdt_config = {
        /* Reset SoC when watchdog timer expires. */
        .flags = WDT_FLAG_RESET_SOC,

        /* Expire watchdog after max window */
        .window.min = 0,
        .window.max = timeout_ms,
    };

    wdt_default.channel_id = wdt_install_timeout(wdt_default.wdt, &wdt_config);

    if (wdt_default.channel_id < 0) {
        mp_raise_OSError(-wdt_default.channel_id);
    }

    mp_int_t rs_code = wdt_setup(wdt_default.wdt, WDT_OPT_PAUSE_IN_SLEEP | WDT_OPT_PAUSE_HALTED_BY_DBG);

    if (rs_code < 0) {
        mp_raise_OSError(-rs_code);
    }

    return &wdt_default;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    mp_int_t rs_code = wdt_feed(self->wdt, self->channel_id);
    if (rs_code < 0) {
        mp_raise_OSError(-rs_code);
    }
}
