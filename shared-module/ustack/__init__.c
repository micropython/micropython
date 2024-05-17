// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/mpstate.h"
#include "py/stackctrl.h"

#include "shared-bindings/ustack/__init__.h"

#if MICROPY_MAX_STACK_USAGE
uint32_t shared_module_ustack_max_stack_usage(void) {
    // Start at stack limit and move up.
    // Untouched stack was filled with a sentinel value.
    // Stop at first non-sentinel byte.
    char *p = MP_STATE_THREAD(stack_bottom);
    while (*p++ == MP_MAX_STACK_USAGE_SENTINEL_BYTE) {
    }
    return MP_STATE_THREAD(stack_top) - p;
}
#endif

uint32_t shared_module_ustack_stack_size() {
    return MP_STATE_THREAD(stack_limit);
}

uint32_t shared_module_ustack_stack_usage() {
    return mp_stack_usage();
}
