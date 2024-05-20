// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2020 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include <nuttx/fs/fs.h>

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    struct inode *inode;
    uint32_t frequency;
    uint32_t count;
    uint8_t width;
    const mcu_pin_obj_t *command_pin;
    const mcu_pin_obj_t *clock_pin;
    const mcu_pin_obj_t *data_pins[4];
} sdioio_sdcard_obj_t;
