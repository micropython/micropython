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
    {{&machine_pin_type}, PIN_PA11}, // RX_D0
    {{&machine_pin_type}, PIN_PA10}, // TX_D1
    {{&machine_pin_type}, PIN_PA14}, // D2
    {{&machine_pin_type}, PIN_PB09}, // D3
    {{&machine_pin_type}, PIN_PA08}, // D4
    {{&machine_pin_type}, PIN_PA15}, // D5
    {{NULL}, -1}, // D6- not terminated on breakout.
    {{&machine_pin_type}, PIN_PA21}, // D7
    {{NULL}, -1}, // D8- not terminated on breakout.
    {{&machine_pin_type}, PIN_PA07}, // D9
    {{&machine_pin_type}, PIN_PA18}, // D10
    {{&machine_pin_type}, PIN_PA16}, // D11
    {{&machine_pin_type}, PIN_PA19}, // D12
    {{&machine_pin_type}, PIN_PA17}, // D13
    {{&machine_pin_type}, PIN_PA02}, // A0 = 14
    {{&machine_pin_type}, PIN_PB08}, // A1
    {{&machine_pin_type}, PIN_PB09}, // A2
    {{&machine_pin_type}, PIN_PA04}, // A3
    {{&machine_pin_type}, PIN_PA05}, // A4
    {{&machine_pin_type}, PIN_PB02}, // A5 = 19
    {{&machine_pin_type}, PIN_PA22}, // SDA = 20
    {{&machine_pin_type}, PIN_PA23}, // SCL = 21
    {{&machine_pin_type}, PIN_PB10}, // MOSI = 22
    {{&machine_pin_type}, PIN_PA12}, // MISO = 23
    {{&machine_pin_type}, PIN_PB11}, // SCK = 24
    {{&machine_pin_type}, PIN_PA00}, // DOTSTAR_CLK = 25
    {{&machine_pin_type}, PIN_PA01}, // DOTSTAR_DATA = 26
    {{&machine_pin_type}, PIN_PB22}, // FLASH_MOSI = 27
    {{&machine_pin_type}, PIN_PB03}, // FLASH_MISO = 28
    {{&machine_pin_type}, PIN_PB23}, // FLASH_SCK = 29
    {{&machine_pin_type}, PIN_PA27}, // FLASH_CS = 30
    {{&machine_pin_type}, PIN_PA30}, // SWCLK = 31
    {{&machine_pin_type}, PIN_PA31}, // SWDIO = 32
};

const machine_led_obj_t machine_led_obj[] = {
    {{&machine_led_type}, PIN_PA17}, // user LED
};
