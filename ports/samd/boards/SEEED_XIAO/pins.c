/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Peter van der Burg
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
 *
 * Used by machine_pin.c. Holds Board/MCU specific Pin allocations.
 */

#include "modmachine.h"
#include "pins.h"

// Ensure Declaration in 'pins.h' reflects # of Pins defined here.
const machine_pin_obj_t machine_pin_obj[] = {
    {{&machine_pin_type}, PIN_PA02}, // A0/D0
    {{&machine_pin_type}, PIN_PA04}, // A1/D1
    {{&machine_pin_type}, PIN_PA10}, // A2/D2
    {{&machine_pin_type}, PIN_PA11}, // A3/D3
    {{&machine_pin_type}, PIN_PA08}, // A4/D4
    {{&machine_pin_type}, PIN_PA09}, // A5/D5
    {{&machine_pin_type}, PIN_PB08}, // A6/D6
    {{&machine_pin_type}, PIN_PB09}, // A7/D7
    {{&machine_pin_type}, PIN_PA07}, // A8/D8
    {{&machine_pin_type}, PIN_PA05}, // A9/D9
    {{&machine_pin_type}, PIN_PA06}, // A10/D10
};

const machine_led_obj_t machine_led_obj[] = {
// XIAO: Just the available LED Pins: User LED (PA17), Rx & Tx.
    {{&machine_led_type}, PIN_PA17}, // W13
    {{&machine_led_type}, PIN_PA18}, // RX_LED
    {{&machine_led_type}, PIN_PA19}, // TX_LED
};
