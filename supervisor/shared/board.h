// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SUPERVISOR_SHARED_BOARD_H
#define MICROPY_INCLUDED_SUPERVISOR_SHARED_BOARD_H

#include <stddef.h>

#include "shared-bindings/microcontroller/Pin.h"

void board_reset_user_neopixels(const mcu_pin_obj_t *pin, size_t count);

#endif  // MICROPY_INCLUDED_SUPERVISOR_SHARED_BOARD_H
