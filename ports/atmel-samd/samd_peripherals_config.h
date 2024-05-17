// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_SAMD_PERIPHERALS_CONFIG_H
#define MICROPY_INCLUDED_ATMEL_SAMD_SAMD_PERIPHERALS_CONFIG_H

#include "py/obj.h"

extern const mp_obj_type_t mcu_pin_type;

#define PIN_PREFIX_VALUES { &mcu_pin_type },
#define PIN_PREFIX_FIELDS mp_obj_base_t base;


#endif // MICROPY_INCLUDED_ATMEL_SAMD_SAMD_PERIPHERALS_CONFIG_H
