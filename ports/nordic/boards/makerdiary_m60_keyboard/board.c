/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/board.h"
#include "supervisor/shared/board.h"
#include "mpconfigboard.h"

static void power_on(void) {
    // turn on internal battery
    nrf_gpio_cfg(POWER_SWITCH_PIN->number,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_write(POWER_SWITCH_PIN->number, true);
}

static void preserve_and_release_battery_pin(void) {
    // Preserve the battery state. The battery is enabled by default in factory bootloader.
    // Reset claimed_pins so user can control pin's state in the vm.
    // The code below doesn't actually reset the pin's state, but only set the flags.
    reset_pin_number(POWER_SWITCH_PIN->number);       // clear claimed_pins and never_reset_pins
    never_reset_pin_number(POWER_SWITCH_PIN->number); // set never_reset_pins
}

void board_init(void) {
    // As of cpy 8.1.0, board_init() runs after reset_ports() on first run. That means
    // never_reset_pins won't be set at boot, the battery pin is reset, causing system
    // shutdown.
    // So if we need to run on battery, we must enable the battery here.
    power_on();
    preserve_and_release_battery_pin();
}

void reset_board(void) {
    preserve_and_release_battery_pin();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
