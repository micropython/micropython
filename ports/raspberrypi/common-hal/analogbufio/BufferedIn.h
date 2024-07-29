// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Lee Atkinson, MeanStride Technology, Inc.
// SPDX-FileCopyrightText: Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "src/rp2_common/hardware_dma/include/hardware/dma.h"

#include "py/obj.h"

//  This is the analogbufio object
typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint8_t chan;
    uint dma_chan[2];
    dma_channel_config cfg[2];
} analogbufio_bufferedin_obj_t;
