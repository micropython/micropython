// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
} analogio_analogin_obj_t;

static inline uint8_t stm32_adc_units(uint8_t adc_packed) {
    return adc_packed >> 5;
}

static inline uint8_t stm32_adc_channel(uint8_t adc_packed) {
    return adc_packed & 0x1f;
}
