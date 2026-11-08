// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2015 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// 24kiB stack
#define CIRCUITPY_DEFAULT_STACK_SIZE            (24 * 1024)

#if defined(CONFIG_TRACING_PERFETTO) && defined(CONFIG_BOARD_NATIVE_SIM)
// Raising the RuntimeError for a C stack overflow allocates the exception, and
// gc_alloc() then emits a Perfetto counter whose encoder needs about 2.5kiB of
// stack. Leave room for it, or the simulator crashes instead of raising.
#define CIRCUITPY_EXCEPTION_STACK_SIZE          (8 * 1024)
#endif

#define MICROPY_PY_SYS_PLATFORM "Zephyr"

#define CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL (1)
#define DIGITALINOUT_INVALID_DRIVE_MODE (1)

#define CIRCUITPY_DEBUG_TINYUSB 0

// NVM size is determined at runtime from the Zephyr partition table.
#define CIRCUITPY_INTERNAL_NVM_SIZE 1

// Disable native _Float16 handling for host builds.
#define MICROPY_FLOAT_USE_NATIVE_FLT16 (0)

#define MICROPY_NLR_THUMB_USE_LONG_JUMP (1)

////////////////////////////////////////////////////////////////////////////////////////////////////

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"
