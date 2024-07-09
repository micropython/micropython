// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// DO NOT include this file directly.
// Use shared-bindings/microcontroller/Pin.h instead.
// This ensures that all necessary includes are already included.

#pragma once

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

#define NO_ADC SOC_ADC_PERIPH_NUM
#define NO_ADC_CHANNEL SOC_ADC_MAX_CHANNEL_NUM

#define NO_TOUCH_CHANNEL TOUCH_PAD_MAX

// This macro is used to simplify pin definition in peripherals/<chip>/pins.c
#define PIN(p_number, p_adc_index, p_adc_channel, p_touch_channel) \
    { \
        { &mcu_pin_type }, \
        .number = p_number, \
        .adc_index = p_adc_index, \
        .adc_channel = p_adc_channel, \
        .touch_channel = p_touch_channel, \
    }

// Choose based on chip
#if defined(CONFIG_IDF_TARGET_ESP32)
#include "esp32/pins.h"
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
#include "esp32c2/pins.h"
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#include "esp32c3/pins.h"
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#include "esp32c6/pins.h"
#elif defined(CONFIG_IDF_TARGET_ESP32H2)
#include "esp32h2/pins.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#include "esp32s2/pins.h"
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "esp32s3/pins.h"
#endif
