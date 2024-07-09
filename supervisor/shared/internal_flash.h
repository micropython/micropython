// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft, for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT
#pragma once

#include "supervisor/internal_flash.h" // This is per-port.

void port_internal_flash_flush(void);
