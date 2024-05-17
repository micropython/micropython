// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft, for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT
#ifndef MICROPY_INCLUDED_SUPERVISOR_SHARED_INTERNAL_FLASH_H
#define MICROPY_INCLUDED_SUPERVISOR_SHARED_INTERNAL_FLASH_H

#include "supervisor/internal_flash.h" // This is per-port.

void port_internal_flash_flush(void);

#endif  // MICROPY_INCLUDED_SUPERVISOR_SHARED_INTERNAL_FLASH_H
