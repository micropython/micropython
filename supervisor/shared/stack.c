// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "stack.h"

#include "py/mpconfig.h"
#include "py/runtime.h"
#include "supervisor/cpu.h"
#include "supervisor/port.h"
#include "supervisor/shared/safe_mode.h"

void stack_init(void) {
    uint32_t *stack_limit = port_stack_get_limit();
    *stack_limit = STACK_CANARY_VALUE;
}

inline bool stack_ok(void) {
    uint32_t *stack_limit = port_stack_get_limit();
    return *stack_limit == STACK_CANARY_VALUE;
}

inline void assert_heap_ok(void) {
    if (!stack_ok()) {
        reset_into_safe_mode(SAFE_MODE_STACK_OVERFLOW);
    }
}
