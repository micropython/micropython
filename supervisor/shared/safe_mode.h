// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mpconfig.h"

typedef enum {
    SAFE_MODE_NONE = 0,
    // BROWNOUT should be lowest after SAFE_MODE_NONE.
    SAFE_MODE_BROWNOUT,
    // alphabetical from here down
    SAFE_MODE_FLASH_WRITE_FAIL,
    SAFE_MODE_GC_ALLOC_OUTSIDE_VM,
    SAFE_MODE_HARD_FAULT,
    SAFE_MODE_INTERRUPT_ERROR,
    SAFE_MODE_MANUAL,
    SAFE_MODE_NLR_JUMP_FAIL,
    SAFE_MODE_NO_CIRCUITPY,
    SAFE_MODE_NO_HEAP,
    SAFE_MODE_PROGRAMMATIC,
    SAFE_MODE_SAFEMODE_PY_ERROR,
    SAFE_MODE_SDK_FATAL_ERROR,
    SAFE_MODE_STACK_OVERFLOW,
    SAFE_MODE_USB_BOOT_DEVICE_NOT_INTERFACE_ZERO,
    SAFE_MODE_USB_TOO_MANY_ENDPOINTS,
    SAFE_MODE_USB_TOO_MANY_INTERFACE_NAMES,
    SAFE_MODE_USER,
    SAFE_MODE_WATCHDOG,
} safe_mode_t;

safe_mode_t get_safe_mode(void);
void set_safe_mode(safe_mode_t safe_mode);

safe_mode_t wait_for_safe_mode_reset(void);

void safe_mode_on_next_reset(safe_mode_t reason);
void reset_into_safe_mode(safe_mode_t reason) NORETURN;

void print_safe_mode_message(safe_mode_t reason);
