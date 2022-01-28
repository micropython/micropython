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
    {{&machine_pin_type}, PIN_PA16}, // RX_D0
    {{&machine_pin_type}, PIN_PA17}, // TX_D1
    {{&machine_pin_type}, PIN_PA07}, // D2
    {{&machine_pin_type}, PIN_PB22}, // D3
    {{&machine_pin_type}, PIN_PA14}, // D4
    {{&machine_pin_type}, PIN_PA15}, // D5
    {{NULL}, -1}, // D6- not terminated on breakout.
    {{&machine_pin_type}, PIN_PA18}, // D7
    {{NULL}, -1}, // D8- not terminated on breakout.
    {{&machine_pin_type}, PIN_PA19}, // D9
    {{&machine_pin_type}, PIN_PA20}, // D10
    {{&machine_pin_type}, PIN_PA21}, // D11
    {{&machine_pin_type}, PIN_PA23}, // D12
    {{&machine_pin_type}, PIN_PA22}, // D13
    {{&machine_pin_type}, PIN_PA02},     // A0
    {{&machine_pin_type}, PIN_PA05}, // A1
    {{&machine_pin_type}, PIN_PB08}, // A2
    {{&machine_pin_type}, PIN_PB09}, // A3
    {{&machine_pin_type}, PIN_PA04}, // A4
    {{&machine_pin_type}, PIN_PA06}, // A5
};

const machine_led_obj_t machine_led_obj[] = {
    {{&machine_led_type}, PIN_PA22}, // D13
};
