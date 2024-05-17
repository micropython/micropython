// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Milind Movasha
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ESP32_COMMON_HAL_ANALOGBUFIO_BUFFEREDIN_H
#define MICROPY_INCLUDED_ESP32_COMMON_HAL_ANALOGBUFIO_BUFFEREDIN_H

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

#include "components/esp_adc/include/esp_adc/adc_continuous.h"

//  This is the analogbufio object
typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint32_t sample_rate;
    adc_continuous_handle_t handle;
    bool started;
} analogbufio_bufferedin_obj_t;

#endif // MICROPY_INCLUDED_ESP32_COMMON_HAL_ANALOGBUFIO_BUFFEREDIN_H
