// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_CXD56_MPHALPORT_H
#define MICROPY_INCLUDED_CXD56_MPHALPORT_H

#include <sys/types.h>

#include "shared/runtime/interrupt_char.h"
#include "supervisor/shared/tick.h"

#define mp_hal_ticks_ms()       ((mp_uint_t)supervisor_ticks_ms32())

#endif // MICROPY_INCLUDED_CXD56_MPHALPORT_H
