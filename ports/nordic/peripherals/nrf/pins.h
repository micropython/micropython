/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

// DO NOT include this file directly. Use shared-bindings/microcontroller/Pin.h instead to ensure
// that all necessary includes are already included.

#ifndef __MICROPY_INCLUDED_NRF_PERIPHERALS_PINS_H__
#define __MICROPY_INCLUDED_NRF_PERIPHERALS_PINS_H__

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

#endif // __MICROPY_INCLUDED_NRF_PERIPHERALS_PINS_H__
