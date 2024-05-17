// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "bindings/espulp/__init__.h"

mp_int_t common_hal_espulp_get_rtc_gpio_number(const mcu_pin_obj_t *pin) {
    if (pin->number <= 21) {
        return pin->number;
    }
    return -1;
}
