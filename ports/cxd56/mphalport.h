// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include <sys/types.h>

#include "shared/runtime/interrupt_char.h"
#include "supervisor/shared/tick.h"

#define mp_hal_ticks_ms()       ((mp_uint_t)supervisor_ticks_ms32())
