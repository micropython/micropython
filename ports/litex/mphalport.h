// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2015 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "shared/runtime/interrupt_char.h"
#include "py/mpconfig.h"
#include "supervisor/shared/tick.h"

#define mp_hal_ticks_ms()       ((mp_uint_t)supervisor_ticks_ms32())
// #define mp_hal_delay_us(us)     NRFX_DELAY_US((uint32_t) (us))

bool mp_hal_stdin_any(void);
