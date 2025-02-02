/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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
#ifndef MICROPY_INCLUDED_ESP32_MACHINE_PIN_H
#define MICROPY_INCLUDED_ESP32_MACHINE_PIN_H

#include "py/obj.h"
#include "hal/gpio_types.h"

// Define which pins are enabled for a given SoC and configuration.

#if CONFIG_IDF_TARGET_ESP32

#define MICROPY_HW_ENABLE_GPIO0 (1)
#define MICROPY_HW_ENABLE_GPIO1 (1)
#define MICROPY_HW_ENABLE_GPIO2 (1)
#define MICROPY_HW_ENABLE_GPIO3 (1)
#define MICROPY_HW_ENABLE_GPIO4 (1)
#define MICROPY_HW_ENABLE_GPIO5 (1)
#define MICROPY_HW_ENABLE_GPIO6 (1)
#define MICROPY_HW_ENABLE_GPIO7 (1)
#define MICROPY_HW_ENABLE_GPIO8 (1)
#define MICROPY_HW_ENABLE_GPIO9 (1)
#define MICROPY_HW_ENABLE_GPIO10 (1)
#define MICROPY_HW_ENABLE_GPIO11 (1)
#define MICROPY_HW_ENABLE_GPIO12 (1)
#define MICROPY_HW_ENABLE_GPIO13 (1)
#define MICROPY_HW_ENABLE_GPIO14 (1)
#define MICROPY_HW_ENABLE_GPIO15 (1)
#if !CONFIG_ESP32_SPIRAM_SUPPORT
#define MICROPY_HW_ENABLE_GPIO16 (1)
#define MICROPY_HW_ENABLE_GPIO17 (1)
#endif
#define MICROPY_HW_ENABLE_GPIO18 (1)
#define MICROPY_HW_ENABLE_GPIO19 (1)
#define MICROPY_HW_ENABLE_GPIO20 (1)
#define MICROPY_HW_ENABLE_GPIO21 (1)
#define MICROPY_HW_ENABLE_GPIO22 (1)
#define MICROPY_HW_ENABLE_GPIO23 (1)
#define MICROPY_HW_ENABLE_GPIO25 (1)
#define MICROPY_HW_ENABLE_GPIO26 (1)
#define MICROPY_HW_ENABLE_GPIO27 (1)
#define MICROPY_HW_ENABLE_GPIO32 (1)
#define MICROPY_HW_ENABLE_GPIO33 (1)
#define MICROPY_HW_ENABLE_GPIO34 (1)
#define MICROPY_HW_ENABLE_GPIO35 (1)
#define MICROPY_HW_ENABLE_GPIO36 (1)
#define MICROPY_HW_ENABLE_GPIO37 (1)
#define MICROPY_HW_ENABLE_GPIO38 (1)
#define MICROPY_HW_ENABLE_GPIO39 (1)

#elif CONFIG_IDF_TARGET_ESP32C3

#define MICROPY_HW_ENABLE_GPIO0 (1)
#define MICROPY_HW_ENABLE_GPIO1 (1)
#define MICROPY_HW_ENABLE_GPIO2 (1)
#define MICROPY_HW_ENABLE_GPIO3 (1)
#define MICROPY_HW_ENABLE_GPIO4 (1)
#define MICROPY_HW_ENABLE_GPIO5 (1)
#define MICROPY_HW_ENABLE_GPIO6 (1)
#define MICROPY_HW_ENABLE_GPIO7 (1)
#define MICROPY_HW_ENABLE_GPIO8 (1)
#define MICROPY_HW_ENABLE_GPIO9 (1)
#define MICROPY_HW_ENABLE_GPIO10 (1)
#define MICROPY_HW_ENABLE_GPIO11 (1)
#define MICROPY_HW_ENABLE_GPIO12 (1)
#define MICROPY_HW_ENABLE_GPIO13 (1)
#if !MICROPY_HW_ESP_USB_SERIAL_JTAG
#define MICROPY_HW_ENABLE_GPIO18 (1)
#define MICROPY_HW_ENABLE_GPIO19 (1)
#endif
#define MICROPY_HW_ENABLE_GPIO20 (1)
#define MICROPY_HW_ENABLE_GPIO21 (1)

#elif CONFIG_IDF_TARGET_ESP32C6

#define MICROPY_HW_ENABLE_GPIO0 (1)
#define MICROPY_HW_ENABLE_GPIO1 (1)
#define MICROPY_HW_ENABLE_GPIO2 (1)
#define MICROPY_HW_ENABLE_GPIO3 (1)
#define MICROPY_HW_ENABLE_GPIO4 (1)
#define MICROPY_HW_ENABLE_GPIO5 (1)
#define MICROPY_HW_ENABLE_GPIO6 (1)
#define MICROPY_HW_ENABLE_GPIO7 (1)
#define MICROPY_HW_ENABLE_GPIO8 (1)
#define MICROPY_HW_ENABLE_GPIO9 (1)
#define MICROPY_HW_ENABLE_GPIO10 (1)
#define MICROPY_HW_ENABLE_GPIO11 (1)
#if !CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#define MICROPY_HW_ENABLE_GPIO12 (1)
#define MICROPY_HW_ENABLE_GPIO13 (1)
#endif
#define MICROPY_HW_ENABLE_GPIO14 (1)
#define MICROPY_HW_ENABLE_GPIO15 (1)
#define MICROPY_HW_ENABLE_GPIO16 (1)
#define MICROPY_HW_ENABLE_GPIO17 (1)
#define MICROPY_HW_ENABLE_GPIO18 (1)
#define MICROPY_HW_ENABLE_GPIO19 (1)
#define MICROPY_HW_ENABLE_GPIO20 (1)
#define MICROPY_HW_ENABLE_GPIO21 (1)
#define MICROPY_HW_ENABLE_GPIO22 (1)
#define MICROPY_HW_ENABLE_GPIO23 (1)
// GPIO 24-30 are used for spi/sip flash.

#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3

#define MICROPY_HW_ENABLE_GPIO0 (1)
#define MICROPY_HW_ENABLE_GPIO1 (1)
#define MICROPY_HW_ENABLE_GPIO2 (1)
#define MICROPY_HW_ENABLE_GPIO3 (1)
#define MICROPY_HW_ENABLE_GPIO4 (1)
#define MICROPY_HW_ENABLE_GPIO5 (1)
#define MICROPY_HW_ENABLE_GPIO6 (1)
#define MICROPY_HW_ENABLE_GPIO7 (1)
#define MICROPY_HW_ENABLE_GPIO8 (1)
#define MICROPY_HW_ENABLE_GPIO9 (1)
#define MICROPY_HW_ENABLE_GPIO10 (1)
#define MICROPY_HW_ENABLE_GPIO11 (1)
#define MICROPY_HW_ENABLE_GPIO12 (1)
#define MICROPY_HW_ENABLE_GPIO13 (1)
#define MICROPY_HW_ENABLE_GPIO14 (1)
#define MICROPY_HW_ENABLE_GPIO15 (1)
#define MICROPY_HW_ENABLE_GPIO16 (1)
#define MICROPY_HW_ENABLE_GPIO17 (1)
#define MICROPY_HW_ENABLE_GPIO18 (1)
#if !CONFIG_USB_CDC_ENABLED
#define MICROPY_HW_ENABLE_GPIO19 (1)
#define MICROPY_HW_ENABLE_GPIO20 (1)
#endif
#define MICROPY_HW_ENABLE_GPIO21 (1)
#if !CONFIG_SPIRAM
#define MICROPY_HW_ENABLE_GPIO26 (1)
#endif
#if !CONFIG_SPIRAM_MODE_OCT
#define MICROPY_HW_ENABLE_GPIO33 (1)
#define MICROPY_HW_ENABLE_GPIO34 (1)
#define MICROPY_HW_ENABLE_GPIO35 (1)
#define MICROPY_HW_ENABLE_GPIO36 (1)
#define MICROPY_HW_ENABLE_GPIO37 (1)
#endif
#define MICROPY_HW_ENABLE_GPIO38 (1)
#define MICROPY_HW_ENABLE_GPIO39 (1)
#define MICROPY_HW_ENABLE_GPIO40 (1)
#define MICROPY_HW_ENABLE_GPIO41 (1)
#define MICROPY_HW_ENABLE_GPIO42 (1)
#define MICROPY_HW_ENABLE_GPIO43 (1)
#define MICROPY_HW_ENABLE_GPIO44 (1)
#define MICROPY_HW_ENABLE_GPIO45 (1)
#define MICROPY_HW_ENABLE_GPIO46 (1)
#if CONFIG_IDF_TARGET_ESP32S3 && MICROPY_HW_ESP32S3_EXTENDED_IO
#define MICROPY_HW_ENABLE_GPIO47 (1)
#define MICROPY_HW_ENABLE_GPIO48 (1)
#endif

#endif

typedef struct _machine_pin_irq_obj_t {
    mp_obj_base_t base;
} machine_pin_irq_obj_t;

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    machine_pin_irq_obj_t irq;
} machine_pin_obj_t;

extern const mp_obj_type_t machine_pin_irq_type;

extern const machine_pin_obj_t machine_pin_obj_table[GPIO_NUM_MAX];

extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;

#endif // MICROPY_INCLUDED_ESP32_MACHINE_PIN_H
