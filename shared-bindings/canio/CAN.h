// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/canio/__init__.h"
#include "shared-bindings/canio/Message.h"

extern const mp_obj_type_t canio_can_type;

typedef struct canio_can_obj canio_can_obj_t;

void common_hal_canio_can_construct(canio_can_obj_t *self, const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx, int baudrate, bool loopback, bool silent);
bool common_hal_canio_can_auto_restart_get(canio_can_obj_t *self);
bool common_hal_canio_can_deinited(canio_can_obj_t *self);
int common_hal_canio_can_baudrate_get(canio_can_obj_t *self);
bool common_hal_canio_can_loopback_get(canio_can_obj_t *self);
int common_hal_canio_can_receive_error_count_get(canio_can_obj_t *self);
canio_bus_state_t common_hal_canio_can_state_get(canio_can_obj_t *self);
bool common_hal_canio_can_silent_get(canio_can_obj_t *self);
int common_hal_canio_can_transmit_error_count_get(canio_can_obj_t *self);
void common_hal_canio_can_auto_restart_set(canio_can_obj_t *self, bool auto_restart);
void common_hal_canio_can_check_for_deinit(canio_can_obj_t *self);
void common_hal_canio_can_deinit(canio_can_obj_t *self);
void common_hal_canio_can_restart(canio_can_obj_t *self);
void common_hal_canio_can_send(canio_can_obj_t *self, mp_obj_t message);
void common_hal_canio_reset(void);
