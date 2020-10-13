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

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_name, p_number, p_adc_index, p_adc_channel) \
const mcu_pin_obj_t pin_## p_name = { \
    PIN_PREFIX_VALUES \
    .number = p_number, \
    .adc_index = p_adc_index, \
    .adc_channel = p_adc_channel, \
}

PIN(GPIO0, 0, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO1, 1, ADC_UNIT_1, ADC_CHANNEL_0);
PIN(GPIO2, 2, ADC_UNIT_1, ADC_CHANNEL_1);
PIN(GPIO3, 3, ADC_UNIT_1, ADC_CHANNEL_2);
PIN(GPIO4, 4, ADC_UNIT_1, ADC_CHANNEL_3);
PIN(GPIO5, 5, ADC_UNIT_1, ADC_CHANNEL_4);
PIN(GPIO6, 6, ADC_UNIT_1, ADC_CHANNEL_5);
PIN(GPIO7, 7, ADC_UNIT_1, ADC_CHANNEL_6);
PIN(GPIO8, 8, ADC_UNIT_1, ADC_CHANNEL_7);
PIN(GPIO9, 9, ADC_UNIT_1, ADC_CHANNEL_8);
PIN(GPIO10, 10, ADC_UNIT_1, ADC_CHANNEL_9);
PIN(GPIO11, 11, ADC_UNIT_2, ADC_CHANNEL_0);
PIN(GPIO12, 12, ADC_UNIT_2, ADC_CHANNEL_1);
PIN(GPIO13, 13, ADC_UNIT_2, ADC_CHANNEL_2);
PIN(GPIO14, 14, ADC_UNIT_2, ADC_CHANNEL_3);
PIN(GPIO15, 15, ADC_UNIT_2, ADC_CHANNEL_4);
PIN(GPIO16, 16, ADC_UNIT_2, ADC_CHANNEL_5);
PIN(GPIO17, 17, ADC_UNIT_2, ADC_CHANNEL_6);
PIN(GPIO18, 18, ADC_UNIT_2, ADC_CHANNEL_7);
PIN(GPIO19, 19, ADC_UNIT_2, ADC_CHANNEL_8);
PIN(GPIO20, 20, ADC_UNIT_2, ADC_CHANNEL_9);
PIN(GPIO21, 21, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO26, 26, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO27, 27, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO28, 28, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO29, 29, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO30, 30, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO31, 31, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO32, 32, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO33, 33, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO34, 34, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO35, 35, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO36, 36, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO37, 37, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO38, 38, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO39, 39, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO40, 40, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO41, 41, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO42, 42, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO43, 43, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO44, 44, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO45, 45, NO_ADC, NO_ADC_CHANNEL);
PIN(GPIO46, 46, NO_ADC, NO_ADC_CHANNEL);
