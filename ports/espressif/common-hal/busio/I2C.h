// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "components/hal/include/hal/i2c_types.h"
#include "FreeRTOS.h"
#include "freertos/semphr.h"
#include "py/obj.h"

#include "peripherals/i2c.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *scl_pin;
    const mcu_pin_obj_t *sda_pin;
    i2c_port_t i2c_num;
    SemaphoreHandle_t xSemaphore;
    bool has_lock;
} busio_i2c_obj_t;
