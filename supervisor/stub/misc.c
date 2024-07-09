// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "stdbool.h"

#include "supervisor/port.h"
#include "py/mpconfig.h"


MP_WEAK void port_post_boot_py(bool heap_valid) {
}
