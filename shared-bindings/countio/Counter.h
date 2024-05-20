// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by Daniel Pollard
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/countio/Counter.h"
#include "shared-bindings/countio/Edge.h"
#include "shared-bindings/digitalio/Pull.h"

extern const mp_obj_type_t countio_counter_type;

extern void common_hal_countio_counter_construct(countio_counter_obj_t *self,
    const mcu_pin_obj_t *pin, countio_edge_t edge, digitalio_pull_t pull);
extern void common_hal_countio_counter_deinit(countio_counter_obj_t *self);
extern bool common_hal_countio_counter_deinited(countio_counter_obj_t *self);
extern mp_int_t common_hal_countio_counter_get_count(countio_counter_obj_t *self);
extern void common_hal_countio_counter_set_count(countio_counter_obj_t *self,
    mp_int_t new_count);
