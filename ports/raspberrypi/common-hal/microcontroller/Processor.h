// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "src/rp2_common/pico_unique_id/include/pico/unique_id.h"

#define COMMON_HAL_MCU_PROCESSOR_UID_LENGTH PICO_UNIQUE_BOARD_ID_SIZE_BYTES

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // Stores no state currently.
} mcu_processor_obj_t;
