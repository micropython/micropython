// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_BROADCOM_COMMON_HAL_BUSIO_SPI_H
#define MICROPY_INCLUDED_BROADCOM_COMMON_HAL_BUSIO_SPI_H

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    bool has_lock;
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *MOSI;
    const mcu_pin_obj_t *MISO;
    uint32_t target_frequency;
    int32_t real_frequency;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t index;
} busio_spi_obj_t;

void reset_spi(void);

#endif // MICROPY_INCLUDED_BROADCOM_COMMON_HAL_BUSIO_SPI_H
