// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Elvis Pfutzenreuter <epxx@epxx.co>
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/ps2io/Ps2.h"

extern const mp_obj_type_t ps2io_ps2_type;

extern void common_hal_ps2io_ps2_construct(ps2io_ps2_obj_t *self,
    const mcu_pin_obj_t *data_pin, const mcu_pin_obj_t *clock_pin);
extern void common_hal_ps2io_ps2_deinit(ps2io_ps2_obj_t *self);
extern bool common_hal_ps2io_ps2_deinited(ps2io_ps2_obj_t *self);
extern uint16_t common_hal_ps2io_ps2_get_len(ps2io_ps2_obj_t *self);
extern int16_t common_hal_ps2io_ps2_popleft(ps2io_ps2_obj_t *self);
extern int16_t common_hal_ps2io_ps2_sendcmd(ps2io_ps2_obj_t *self, uint8_t b);
extern uint16_t common_hal_ps2io_ps2_clear_errors(ps2io_ps2_obj_t *self);
