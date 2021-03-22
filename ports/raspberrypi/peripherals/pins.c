/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "pins.h"

#include "shared-bindings/microcontroller/Pin.h"

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_number) \
    const mcu_pin_obj_t pin_GPIO##p_number = { \
        { &mcu_pin_type }, \
        .number = p_number \
    }

PIN(0);
PIN(1);
PIN(2);
PIN(3);
PIN(4);
PIN(5);
PIN(6);
PIN(7);
PIN(8);
PIN(9);
PIN(10);
PIN(11);
PIN(12);
PIN(13);
PIN(14);
PIN(15);
PIN(16);
PIN(17);
PIN(18);
PIN(19);
PIN(20);
PIN(21);
PIN(22);
PIN(23);
PIN(24);
PIN(25);
PIN(26);
PIN(27);
PIN(28);
PIN(29);
