/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "py/obj.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/canio/__init__.h"
#include "shared-bindings/canio/Message.h"

extern const mp_obj_type_t canio_can_type;

typedef struct canio_can_obj canio_can_obj_t;

void common_hal_canio_can_construct(canio_can_obj_t *self, mcu_pin_obj_t *tx, mcu_pin_obj_t *rx, int baudrate, bool loopback, bool silent);
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
