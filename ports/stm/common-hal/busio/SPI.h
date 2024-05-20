// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "peripherals/periph.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    SPI_HandleTypeDef handle;
    bool has_lock;
    const mcu_periph_obj_t *sck;
    const mcu_periph_obj_t *mosi;
    const mcu_periph_obj_t *miso;
    const mcu_periph_obj_t *nss;
    uint32_t baudrate;
    uint16_t prescaler;
    bool half_duplex;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
} busio_spi_obj_t;

void spi_reset(void);
