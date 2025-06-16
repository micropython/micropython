/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
 * Copyright (c) 2017 "Tom Manning" <tom@manningetal.com>
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

#ifndef MICROPY_INCLUDED_ESP32_MACHINE_RTC_H
#define MICROPY_INCLUDED_ESP32_MACHINE_RTC_H

#include "modmachine.h"

typedef struct {
    #if SOC_PM_SUPPORT_EXT1_WAKEUP
    uint64_t ext1_pins; // set bit == pin#
    #endif
    #if SOC_PM_SUPPORT_EXT0_WAKEUP
    int8_t ext0_pin;   // just the pin#, -1 == None
    #endif
    uint64_t gpio_pins; // set bit == pin#
    #if SOC_TOUCH_SENSOR_SUPPORTED
    bool wake_on_touch : 1;
    #endif
    #if SOC_ULP_SUPPORTED
    bool wake_on_ulp : 1;
    #endif
    #if SOC_PM_SUPPORT_EXT0_WAKEUP
    bool ext0_level : 1;
    wake_type_t ext0_wake_types;
    #endif
    #if SOC_PM_SUPPORT_EXT1_WAKEUP
    bool ext1_level : 1;
    #endif
    bool gpio_level : 1;
} machine_rtc_config_t;

extern machine_rtc_config_t machine_rtc_config;

#endif
