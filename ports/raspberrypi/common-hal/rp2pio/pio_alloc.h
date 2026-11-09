// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Shared PIO allocator declarations that let external drivers (including the
// vendored C++ PioSdioCard driver) cooperate with CircuitPython's rp2pio PIO
// allocator instead of seizing whole PIO blocks through the raw SDK. The
// definitions live in common-hal/rp2pio/StateMachine.c. This header
// deliberately does NOT include the MicroPython object headers pulled in by the
// full rp2pio StateMachine.h, so it can be included from C++ translation units.
// The declarations are given C linkage for that reason.

#pragma once

#include "hardware/pio.h"

#ifdef __cplusplus
extern "C" {
#endif

// See common-hal/rp2pio/StateMachine.c. Returns a PIO index (0..NUM_PIOS-1) with
// room for program_size instructions and sm_count free state machines, or
// NUM_PIOS if none qualifies.
uint8_t rp2pio_statemachine_find_pio(int program_size, int sm_count);

#ifdef __cplusplus
}
#endif
