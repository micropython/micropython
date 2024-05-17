// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/microcontroller/Pin.h"

void espulp_reset(void);

mp_int_t common_hal_espulp_get_rtc_gpio_number(const mcu_pin_obj_t *pin);
