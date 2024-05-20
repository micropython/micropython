// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "hal/include/hal_spi_m_sync.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    struct spi_m_sync_descriptor spi_desc;
    bool has_lock;
    uint8_t clock_pin;
    uint8_t MOSI_pin;
    uint8_t MISO_pin;
} busio_spi_obj_t;
