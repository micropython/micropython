// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DUALBANK___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DUALBANK___INIT___H

#include "py/runtime.h"

extern void common_hal_dualbank_switch(void);
extern void common_hal_dualbank_flash(const void *buf, const size_t len, const size_t offset);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_DUALBANK___INIT___H
