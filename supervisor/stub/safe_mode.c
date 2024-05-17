// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/safe_mode.h"

#include <stdlib.h>

safe_mode_t wait_for_safe_mode_reset(void) {
    return SAFE_MODE_NONE;
}

void reset_into_safe_mode(safe_mode_t reason) {
    (void)reason;
    abort();
}

void print_safe_mode_message(safe_mode_t reason) {
    (void)reason;
}
