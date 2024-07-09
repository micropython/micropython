// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"

#include "tick.h"

uint64_t common_hal_time_monotonic_ms(void) {
    return supervisor_ticks_ms64();
}

uint64_t common_hal_time_monotonic_ns(void) {
    uint64_t ms;
    uint32_t us_until_ms;
    current_tick(&ms, &us_until_ms);
    // us counts down.
    return 1000 * (ms * 1000 + (1000 - us_until_ms));
}

void common_hal_time_delay_ms(uint32_t delay) {
    mp_hal_delay_ms(delay);
}
