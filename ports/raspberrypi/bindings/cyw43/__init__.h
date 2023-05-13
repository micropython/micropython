/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
 * Copyright (c) 2016 Scott Shawcroft
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

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"

extern const mp_obj_type_t cyw43_pin_type;
const mcu_pin_obj_t *validate_obj_is_free_pin_including_cyw43(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_free_pin_or_gpio29(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_pin_including_cyw43(mp_obj_t obj, qstr arg_name);

#define CONSTANT_CYW43_PM_VALUE(pm_mode, pm2_sleep_ret_ms, li_beacon_period, li_dtim_period, li_assoc) \
    (li_assoc << 20 | /* listen interval sent to ap */ \
        li_dtim_period << 16 | \
        li_beacon_period << 12 | \
        (pm2_sleep_ret_ms / 10) << 4 | /* cyw43_ll_wifi_pm multiplies this by 10 */ \
        pm_mode /* CYW43_PM2_POWERSAVE_MODE etc */)

// CYW43_DEFAULT_PM (except a compile-time constant)
#define PM_STANDARD CONSTANT_CYW43_PM_VALUE(CYW43_PM2_POWERSAVE_MODE, 200, 1, 1, 10)
// CYW43_AGGRESSIVE_PM (except a compile-time constant)
#define PM_AGGRESSIVE CONSTANT_CYW43_PM_VALUE(CYW43_PM2_POWERSAVE_MODE, 2000, 1, 1, 10)
// CYW43_PERFORMANCE_PM (except a compile-time constant)
#define PM_PERFORMANCE CONSTANT_CYW43_PM_VALUE(CYW43_PM2_POWERSAVE_MODE, 20, 1, 1, 1)
// The 0xa11140 magic value
#define PM_DISABLED CONSTANT_CYW43_PM_VALUE(CYW43_NO_POWERSAVE_MODE, 200, 1, 1, 10)

extern void bindings_cyw43_wifi_enforce_pm(void);
void cyw43_enter_deep_sleep(void);
