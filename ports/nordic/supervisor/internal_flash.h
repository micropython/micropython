// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/mpconfig.h"

#define INTERNAL_FLASH_SYSTICK_MASK     (0x1ff) // 512ms
#define INTERNAL_FLASH_IDLE_TICK(tick)  (((tick) & INTERNAL_FLASH_SYSTICK_MASK) == 2)
