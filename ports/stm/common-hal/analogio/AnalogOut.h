// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"
#if CPY_STM32L4
#include "stm32l4xx_hal.h"
#else
#include "stm32f4xx_hal.h"
#endif
#include "peripherals/periph.h"

typedef struct {
    mp_obj_base_t base;
    #if HAS_DAC
    DAC_ChannelConfTypeDef ch_handle;
    #endif
    const mcu_pin_obj_t *pin;
    uint8_t channel;
    uint8_t dac_index : 1;
} analogio_analogout_obj_t;

void analogout_reset(void);
