// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"

extern const mp_obj_type_t digitalio_digitalinout_type;

typedef enum {
    DIGITALINOUT_OK,
    DIGITALINOUT_PIN_BUSY,
    #if CIRCUITPY_DIGITALIO_HAVE_INPUT_ONLY
    DIGITALINOUT_INPUT_ONLY,
    #endif
    #if CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL
    DIGITALINOUT_INVALID_PULL,
    #endif
    #if CIRCUITPY_DIGITALIO_HAVE_INVALID_DRIVE_MODE
    DIGITALINOUT_INVALID_DRIVE_MODE,
    #endif
} digitalinout_result_t;

typedef enum {
    DIGITALINOUT_REG_READ,
    DIGITALINOUT_REG_WRITE,
    DIGITALINOUT_REG_SET,
    DIGITALINOUT_REG_RESET,
    DIGITALINOUT_REG_TOGGLE,
} digitalinout_reg_op_t;

const mcu_pin_obj_t *common_hal_digitalio_validate_pin(mp_obj_t obj);
digitalinout_result_t common_hal_digitalio_digitalinout_construct(digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin);
void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self);
bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self);
digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_input(digitalio_digitalinout_obj_t *self, digitalio_pull_t pull);
digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(digitalio_digitalinout_obj_t *self, bool value, digitalio_drive_mode_t drive_mode);
digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(digitalio_digitalinout_obj_t *self);
void common_hal_digitalio_digitalinout_set_value(digitalio_digitalinout_obj_t *self, bool value);
bool common_hal_digitalio_digitalinout_get_value(digitalio_digitalinout_obj_t *self);
digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(digitalio_digitalinout_obj_t *self, digitalio_drive_mode_t drive_mode);
digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(digitalio_digitalinout_obj_t *self);
digitalinout_result_t common_hal_digitalio_digitalinout_set_pull(digitalio_digitalinout_obj_t *self, digitalio_pull_t pull);
digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(digitalio_digitalinout_obj_t *self);
void common_hal_digitalio_digitalinout_never_reset(digitalio_digitalinout_obj_t *self);
digitalio_digitalinout_obj_t *assert_digitalinout(mp_obj_t obj);

volatile uint32_t *common_hal_digitalio_digitalinout_get_reg(digitalio_digitalinout_obj_t *self, digitalinout_reg_op_t op, uint32_t *mask);
bool common_hal_digitalio_has_reg_op(digitalinout_reg_op_t op);
