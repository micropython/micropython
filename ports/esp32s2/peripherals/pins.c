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

#define NO_ADC 0xff

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_name, p_number) \
const mcu_pin_obj_t pin_## p_name = { \
    PIN_PREFIX_VALUES \
    .number = p_number, \
}

PIN(GPIO0, 0);
PIN(GPIO1, 1);
PIN(GPIO2, 2);
PIN(GPIO3, 3);
PIN(GPIO4, 4);
PIN(GPIO5, 5);
PIN(GPIO6, 6);
PIN(GPIO7, 7);
PIN(GPIO8, 8);
PIN(GPIO9, 9);
PIN(GPIO10, 10);
PIN(GPIO11, 11);
PIN(GPIO12, 12);
PIN(GPIO13, 13);
PIN(GPIO14, 14);
PIN(GPIO15, 15);
PIN(GPIO16, 16);
PIN(GPIO17, 17);
PIN(GPIO18, 18);

PIN(GPIO19, 19);
PIN(GPIO20, 20);
PIN(GPIO21, 21);
PIN(GPIO26, 26);
PIN(GPIO27, 27);
PIN(GPIO28, 28);
PIN(GPIO29, 29);
PIN(GPIO30, 30);
PIN(GPIO31, 31);
PIN(GPIO32, 32);
PIN(GPIO33, 33);
PIN(GPIO34, 34);
PIN(GPIO35, 35);
PIN(GPIO36, 36);
PIN(GPIO37, 37);
PIN(GPIO38, 38);
PIN(GPIO39, 39);
PIN(GPIO40, 40);
PIN(GPIO41, 41);
PIN(GPIO42, 42);
PIN(GPIO43, 43);
PIN(GPIO44, 44);
PIN(GPIO45, 45);
PIN(GPIO46, 46);
