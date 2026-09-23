// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "common-hal/microcontroller/Pin.h"
#include "hardware/gpio.h"

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

static void assert_power_hold(void) {
    // Doing this (rather than gpio_init()) in this specific order ensures no
    // glitch if the pin was already configured as a high output. gpio_init()
    // temporarily configures the pin as an input, which would release the
    // latch and power the board off.
    gpio_put(MICROPY_HW_POWER_HOLD_PIN_NUMBER, 1);
    gpio_set_dir(MICROPY_HW_POWER_HOLD_PIN_NUMBER, GPIO_OUT);
    gpio_set_function(MICROPY_HW_POWER_HOLD_PIN_NUMBER, GPIO_FUNC_SIO);
}

// Forward declaration to satisfy -Wmissing-prototypes
static void preinit_power_hold(void) __attribute__((constructor(101)));

// Runs before main(), so the latch is set as early as it can possibly be.
static void preinit_power_hold(void) {
    assert_power_hold();
}

// The EP-2350 has no power switch: pressing the handle applies power directly,
// and firmware must set the power hold latch on GPIO2 before the handle is
// released or the unit dies. The latch is a true set/reset latch, so a single
// high pulse is enough, but the pin must never be left low.
//
// reset_pin_number() runs this whenever the pin is reset, so the latch is
// re-asserted instead of being reset to an input. The pin is deliberately not
// claimed, so user code can still take board.POWER_HOLD and drive it low to
// power the unit off.
bool board_reset_pin_number(uint8_t pin_number) {
    if (pin_number == MICROPY_HW_POWER_HOLD_PIN_NUMBER) {
        assert_power_hold();
        return true;
    }
    return false;
}
