// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include <nuttx/spi/spi.h>

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    struct spi_dev_s *spi_dev;
    uint32_t frequency;
    uint8_t phase;
    uint8_t polarity;
    uint8_t bits;
    bool has_lock;
    const mcu_pin_obj_t *clock_pin;
    const mcu_pin_obj_t *mosi_pin;
    const mcu_pin_obj_t *miso_pin;
} busio_spi_obj_t;
