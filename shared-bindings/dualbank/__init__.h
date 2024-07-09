// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/runtime.h"

extern void common_hal_dualbank_switch(void);
extern void common_hal_dualbank_flash(const void *buf, const size_t len, const size_t offset);
