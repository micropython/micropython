/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// DO NOT include this file directly.
// Use shared-bindings/microcontroller/Pin.h instead.
// This ensures that all necessary includes are already included.

#ifndef MICROPY_INCLUDED_ESPRESSIF_PERIPHERALS_PINS_H
#define MICROPY_INCLUDED_ESPRESSIF_PERIPHERALS_PINS_H

#include "py/obj.h"

#include "components/hal/include/hal/gpio_types.h"
#include "components/hal/include/hal/adc_types.h"
#include "components/hal/include/hal/touch_sensor_types.h"

typedef struct {
    mp_obj_base_t base;
    gpio_num_t number;
    uint8_t adc_index : 2;
    uint8_t adc_channel : 6;
    touch_pad_t touch_channel;
} mcu_pin_obj_t;

extern const mp_obj_type_t mcu_pin_type;

#define NO_PIN (GPIO_NUM_NC)

#define NO_ADC 0
#define NO_ADC_CHANNEL ADC_CHANNEL_MAX

#define NO_TOUCH_CHANNEL TOUCH_PAD_MAX

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_number, p_adc_index, p_adc_channel, p_touch_channel) \
    { \
        { &mcu_pin_type }, \
        .number = p_number, \
        .adc_index = p_adc_index, \
        .adc_channel = p_adc_channel, \
        .touch_channel = p_touch_channel, \
    }

// Choose based on chip
#ifdef CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/pins.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#include "esp32s2/pins.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "esp32s3/pins.h"
#endif

#endif // MICROPY_INCLUDED_ESPRESSIF_PERIPHERALS_PINS_H
