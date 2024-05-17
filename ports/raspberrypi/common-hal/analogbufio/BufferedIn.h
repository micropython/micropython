// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Lee Atkinson, MeanStride Technology, Inc.
// SPDX-FileCopyrightText: Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_ANALOGBUFIO_BUFFEREDIN_H
#define MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_ANALOGBUFIO_BUFFEREDIN_H

#include "common-hal/microcontroller/Pin.h"
#include "src/rp2_common/hardware_dma/include/hardware/dma.h"

#include "py/obj.h"

//  This is the analogbufio object
typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint8_t chan;
    uint dma_chan;
    dma_channel_config cfg;
} analogbufio_bufferedin_obj_t;

#endif // MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_ANALOGBUFIO_BUFFEREDIN_H
