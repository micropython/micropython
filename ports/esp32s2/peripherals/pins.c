/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "peripherals/pins.h"

#define NO_ADC 0
#define NO_ADC_CHANNEL ADC_CHANNEL_MAX

#define TOUCH \
    .has_touch = true,

#define NO_TOUCH \
    .has_touch = false,

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_name, p_number, p_adc_index, p_adc_channel, p_touch_channel) \
const mcu_pin_obj_t pin_## p_name = { \
    PIN_PREFIX_VALUES \
    .number = p_number, \
    .adc_index = p_adc_index, \
    .adc_channel = p_adc_channel, \
    p_touch_channel \
}

PIN(GPIO0, 0, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO1, 1, ADC_UNIT_1, ADC_CHANNEL_0, TOUCH);
PIN(GPIO2, 2, ADC_UNIT_1, ADC_CHANNEL_1, TOUCH);
PIN(GPIO3, 3, ADC_UNIT_1, ADC_CHANNEL_2, TOUCH);
PIN(GPIO4, 4, ADC_UNIT_1, ADC_CHANNEL_3, TOUCH);
PIN(GPIO5, 5, ADC_UNIT_1, ADC_CHANNEL_4, TOUCH);
PIN(GPIO6, 6, ADC_UNIT_1, ADC_CHANNEL_5, TOUCH);
PIN(GPIO7, 7, ADC_UNIT_1, ADC_CHANNEL_6, TOUCH);
PIN(GPIO8, 8, ADC_UNIT_1, ADC_CHANNEL_7, TOUCH);
PIN(GPIO9, 9, ADC_UNIT_1, ADC_CHANNEL_8, TOUCH);
PIN(GPIO10, 10, ADC_UNIT_1, ADC_CHANNEL_9, TOUCH);
PIN(GPIO11, 11, ADC_UNIT_2, ADC_CHANNEL_0, TOUCH);
PIN(GPIO12, 12, ADC_UNIT_2, ADC_CHANNEL_1, TOUCH);
PIN(GPIO13, 13, ADC_UNIT_2, ADC_CHANNEL_2, TOUCH);
PIN(GPIO14, 14, ADC_UNIT_2, ADC_CHANNEL_3, TOUCH);
PIN(GPIO15, 15, ADC_UNIT_2, ADC_CHANNEL_4, NO_TOUCH);
PIN(GPIO16, 16, ADC_UNIT_2, ADC_CHANNEL_5, NO_TOUCH);
PIN(GPIO17, 17, ADC_UNIT_2, ADC_CHANNEL_6, NO_TOUCH);
PIN(GPIO18, 18, ADC_UNIT_2, ADC_CHANNEL_7, NO_TOUCH);
PIN(GPIO19, 19, ADC_UNIT_2, ADC_CHANNEL_8, NO_TOUCH);
PIN(GPIO20, 20, ADC_UNIT_2, ADC_CHANNEL_9, NO_TOUCH);
PIN(GPIO21, 21, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO26, 26, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO27, 27, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO28, 28, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO29, 29, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO30, 30, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO31, 31, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO32, 32, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO33, 33, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO34, 34, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO35, 35, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO36, 36, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO37, 37, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO38, 38, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO39, 39, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO40, 40, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO41, 41, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO42, 42, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO43, 43, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO44, 44, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO45, 45, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
PIN(GPIO46, 46, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH);
