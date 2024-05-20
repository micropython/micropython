// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/i2ctarget/I2CTarget.h"

typedef struct {
    mp_obj_base_t base;
    i2ctarget_i2c_target_obj_t *target;
    uint16_t address;
    bool is_read;
    bool is_restart;
} i2ctarget_i2c_target_request_obj_t;

extern const mp_obj_type_t i2ctarget_i2c_target_request_type;

extern const mp_obj_type_t i2ctarget_i2c_target_type;

extern void common_hal_i2ctarget_i2c_target_construct(i2ctarget_i2c_target_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint8_t *addresses, unsigned int num_addresses, bool smbus);
extern void common_hal_i2ctarget_i2c_target_deinit(i2ctarget_i2c_target_obj_t *self);
extern bool common_hal_i2ctarget_i2c_target_deinited(i2ctarget_i2c_target_obj_t *self);

extern int common_hal_i2ctarget_i2c_target_is_addressed(i2ctarget_i2c_target_obj_t *self,
    uint8_t *address, bool *is_read, bool *is_restart);
extern int common_hal_i2ctarget_i2c_target_read_byte(i2ctarget_i2c_target_obj_t *self, uint8_t *data);
extern int common_hal_i2ctarget_i2c_target_write_byte(i2ctarget_i2c_target_obj_t *self, uint8_t data);
extern void common_hal_i2ctarget_i2c_target_ack(i2ctarget_i2c_target_obj_t *self, bool ack);
extern void common_hal_i2ctarget_i2c_target_close(i2ctarget_i2c_target_obj_t *self);
