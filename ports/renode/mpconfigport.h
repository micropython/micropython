// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_PY_SYS_PLATFORM             "Renode"

#define MICROPY_USE_INTERNAL_PRINTF         (1)

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"

#define CIRCUITPY_DEFAULT_STACK_SIZE        (24 * 1024)
