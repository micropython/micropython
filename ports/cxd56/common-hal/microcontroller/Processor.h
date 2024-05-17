// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_CXD56_COMMON_HAL_MICROCONTROLLER_PROCESSOR_H
#define MICROPY_INCLUDED_CXD56_COMMON_HAL_MICROCONTROLLER_PROCESSOR_H

#define COMMON_HAL_MCU_PROCESSOR_UID_LENGTH CONFIG_BOARDCTL_UNIQUEID_SIZE

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
} mcu_processor_obj_t;

extern const mp_obj_type_t mcu_processor_type;

#endif // MICROPY_INCLUDED_CXD56_COMMON_HAL_MICROCONTROLLER_PROCESSOR_H
