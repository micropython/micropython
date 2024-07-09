// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "shared-bindings/time/__init__.h"

uint64_t common_hal_time_monotonic_ms(void) {
    return supervisor_ticks_ms64();
}

uint64_t common_hal_time_monotonic_ns(void) {
    uint8_t subticks = 0;
    uint64_t ticks = port_get_raw_ticks(&subticks);
    // A tick is 976562.5 nanoseconds so multiply it by the base and add half instead of doing float
    // math.
    // A subtick is 1/32 of a tick.
    // 30518 is 1e9 / 32768
    return 976562 * ticks + ticks / 2 + 30518 * subticks;
}

void common_hal_time_delay_ms(uint32_t delay) {
    mp_hal_delay_ms(delay);
}
