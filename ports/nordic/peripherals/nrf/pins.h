// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// DO NOT include this file directly. Use shared-bindings/microcontroller/Pin.h instead to ensure
// that all necessary includes are already included.

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "nrf_gpio.h"

typedef struct {
    mp_obj_base_t base;
    // These could be squeezed to fewer bits if more fields are needed.
    uint8_t number;      // port << 5 | pin number in port (0-31): 6 bits needed
    uint8_t adc_channel; // 0 is no ADC, ADC channel from 1 to 8:
                         // 4 bits needed here; 5 bits used in periph registers
} mcu_pin_obj_t;

extern const mp_obj_type_t mcu_pin_type;

// Used in device-specific pins.c
#define PIN(p_name, p_port, p_pin, p_adc_channel)       \
    { \
        { &mcu_pin_type }, \
        .number = NRF_GPIO_PIN_MAP(p_port, p_pin),      \
        .adc_channel = (p_adc_channel), \
    }

// Use illegal pin value to mark unassigned pins.
#define NO_PIN 0xff

// Choose based on chip, but not specifically revision (e.g., not NRF52840_XXAA)
#ifdef NRF52840
#include "nrf52840/pins.h"
#endif

// Choose based on chip, but not specifically revision (e.g., not NRF52840_XXAA)
#ifdef NRF52833
#include "nrf52833/pins.h"
#endif
