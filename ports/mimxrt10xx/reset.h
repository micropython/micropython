// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/mpconfig.h"

// Copied from inc/uf2.h in https://github.com/Microsoft/uf2-samd21
#define DBL_TAP_REG              SNVS->LPGPR[3]
#define DBL_TAP_MAGIC 0xf01669ef // Randomly selected, adjusted to have first and last bit set
#define DBL_TAP_MAGIC_QUICK_BOOT 0xf02669ef

void reset_to_bootloader(void) NORETURN;
void reset(void) NORETURN;
bool bootloader_available(void);
