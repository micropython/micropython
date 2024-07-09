// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef enum {
    RUNMODE_UF2,
    RUNMODE_NORMAL,
    RUNMODE_SAFE_MODE,
    RUNMODE_BOOTLOADER
} mcu_runmode_t;

extern const mp_obj_type_t mcu_runmode_type;

typedef struct {
    mp_obj_base_t base;
} mcu_runmode_obj_t;

extern const mcu_runmode_obj_t mcu_runmode_uf2_obj;
extern const mcu_runmode_obj_t mcu_runmode_normal_obj;
extern const mcu_runmode_obj_t mcu_runmode_safe_mode_obj;
extern const mcu_runmode_obj_t mcu_runmode_bootloader_obj;
