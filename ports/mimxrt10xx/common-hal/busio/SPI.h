// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "fsl_common.h"
#include "periph.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    LPSPI_Type *spi;
    bool has_lock;
    uint32_t baudrate;
    const mcu_periph_obj_t *clock;
    const mcu_periph_obj_t *mosi;
    const mcu_periph_obj_t *miso;
} busio_spi_obj_t;

void spi_reset(void);
