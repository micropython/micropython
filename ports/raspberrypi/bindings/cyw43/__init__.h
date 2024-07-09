// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"
#include "lib/cyw43-driver/src/cyw43_ll.h"

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
