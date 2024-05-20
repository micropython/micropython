// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2015 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#define MICROPY_PY_FUNCTION_ATTRS                (1)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS       (1)

extern uint8_t _ld_default_stack_size;

// 24kiB stack
// #define CIRCUITPY_DEFAULT_STACK_SIZE            0x6000
#define CIRCUITPY_DEFAULT_STACK_SIZE            ((uint32_t)&_ld_default_stack_size)

#include "py/circuitpy_mpconfig.h"

// Board flags:
#ifndef BOARD_OVERWRITE_SWD
#define BOARD_OVERWRITE_SWD (0)
#endif
#ifndef BOARD_VTOR_DEFER
#define BOARD_VTOR_DEFER (0)
#endif
#ifndef BOARD_NO_VBUS_SENSE
#define BOARD_NO_VBUS_SENSE (0)
#endif
#ifndef BOARD_NO_USB_OTG_ID_SENSE
#define BOARD_NO_USB_OTG_ID_SENSE (0)
#endif

// Peripheral implementation counts
#define MAX_UART 10
#define MAX_I2C 4
#define MAX_SPI 6
