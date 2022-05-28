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
    {{&machine_pin_type}, PIN_PB17}, // D0
    {{&machine_pin_type}, PIN_PB16}, // D1
    {{NULL}, -1},  // D2
    {{NULL}, -1},  // D3
    {{&machine_pin_type}, PIN_PA14}, // D4
    {{&machine_pin_type}, PIN_PA16}, // D5
    {{&machine_pin_type}, PIN_PA18}, // D6
    {{NULL}, -1}, // D7- not terminated on breakout.
    {{&machine_pin_type}, PIN_PB03}, // D8 / NEOPIXEL
    {{&machine_pin_type}, PIN_PA19}, // D9
    {{&machine_pin_type}, PIN_PA20}, // D10
    {{&machine_pin_type}, PIN_PA21}, // D11
    {{&machine_pin_type}, PIN_PA22}, // D12
    {{&machine_pin_type}, PIN_PA23}, // D13
    {{&machine_pin_type}, PIN_PA02}, // A0
    {{&machine_pin_type}, PIN_PA05}, // A1
    {{&machine_pin_type}, PIN_PB08}, // A2
    {{&machine_pin_type}, PIN_PB09}, // A3
    {{&machine_pin_type}, PIN_PA04}, // A4
    {{&machine_pin_type}, PIN_PB06}, // A5
    {{&machine_pin_type}, PIN_PA13}, // SCL
    {{&machine_pin_type}, PIN_PA12}, // SDA
    {{&machine_pin_type}, PIN_PB23}, // MOSI
    {{&machine_pin_type}, PIN_PB22}, // MISO
    {{&machine_pin_type}, PIN_PA17}, // SCK
    {{&machine_pin_type}, PIN_PB01}, // VDIV
    {{&machine_pin_type}, PIN_PA03}, // AREF
    {{&machine_pin_type}, PIN_PA30}, // SWCLK = 28
    {{&machine_pin_type}, PIN_PA31}, // SWDIO = 29
};

const machine_led_obj_t machine_led_obj[] = {
    {{&machine_led_type}, PIN_PA17}, // D13/ user LED
};
