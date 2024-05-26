// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2015 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define CIRCUITPY_INTERNAL_NVM_SIZE         (0)
#define MICROPY_NLR_THUMB                   (0)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS  (1)

#include "py/circuitpy_mpconfig.h"

#define MICROPY_NLR_SETJMP                  (1)
#define CIRCUITPY_DEFAULT_STACK_SIZE        0x6000
