// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "src/rp2040/hardware_regs/include/hardware/platform_defs.h"
#include "peripherals/pins.h"

const mcu_pin_obj_t *mcu_get_pin_by_number(int);
