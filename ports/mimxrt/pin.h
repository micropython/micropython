/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Philipp Ebensberger
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

#ifndef MICROPY_INCLUDED_MIMXRT_PIN_H
#define MICROPY_INCLUDED_MIMXRT_PIN_H

#include <stdint.h>
#include "py/obj.h"
#include "shared/runtime/mpirq.h"
#include "fsl_gpio.h"

// ------------------------------------------------------------------------------------------------------------------ //

#define IS_GPIO_MODE(MODE) (((MODE) == PIN_MODE_IN) || \
    ((MODE) == PIN_MODE_OUT) || \
    ((MODE) == PIN_MODE_OPEN_DRAIN) || \
    ((MODE) == PIN_MODE_ALT))

#define IS_GPIO_DRIVE(DRIVE) (((DRIVE) == PIN_DRIVE_OFF) || \
    ((DRIVE) == PIN_DRIVE_0) || \
    ((DRIVE) == PIN_DRIVE_1) || \
    ((DRIVE) == PIN_DRIVE_2) || \
    ((DRIVE) == PIN_DRIVE_3) || \
    ((DRIVE) == PIN_DRIVE_4) || \
    ((DRIVE) == PIN_DRIVE_5) || \
    ((DRIVE) == PIN_DRIVE_6))

// ------------------------------------------------------------------------------------------------------------------ //

enum {
    PIN_MODE_IN = 0,
    PIN_MODE_OUT,
    PIN_MODE_OPEN_DRAIN,
    PIN_MODE_ALT,
    PIN_MODE_SKIP,
};

enum {
    PIN_AF_MODE_ALT0 = 0,
    PIN_AF_MODE_ALT1,
    PIN_AF_MODE_ALT2,
    PIN_AF_MODE_ALT3,
    PIN_AF_MODE_ALT4,
    PIN_AF_MODE_ALT5,
    PIN_AF_MODE_ALT6,
    PIN_AF_MODE_ALT7,
    PIN_AF_MODE_ALT8,
    PIN_AF_MODE_ALT9,
};

enum {
    PIN_PULL_DOWN_100K = 0,
    PIN_PULL_UP_47K,
    PIN_PULL_UP_100K,
    PIN_PULL_UP_22K,
    PIN_PULL_DISABLED,
    PIN_PULL_HOLD,
};

enum {
    PIN_DRIVE_OFF = 0b000,
    PIN_DRIVE_0, // R0 (150 Ohm @3.3V / 260 Ohm @ 1.8V)
    PIN_DRIVE_1, // R0/2
    PIN_DRIVE_2, // R0/3
    PIN_DRIVE_3, // R0/4
    PIN_DRIVE_4, // R0/5
    PIN_DRIVE_5, // R0/6
    PIN_DRIVE_6, // R0/7
};




// ------------------------------------------------------------------------------------------------------------------ //

typedef struct {
    mp_obj_base_t base;
    qstr name;  // port name
    uint8_t af_mode;  // alternate function
    uint8_t input_daisy;
    void *instance;  // pointer to peripheral instance for alternate function
    uint32_t input_register;
    uint32_t pad_config;  // pad configuration for alternate function
} machine_pin_af_obj_t;

typedef struct {
    ADC_Type *instance;
    uint8_t channel;
} machine_pin_adc_obj_t;

typedef struct {
    mp_obj_base_t base;
    qstr name;  // pad name
    GPIO_Type *gpio;  // gpio instance for pin
    uint32_t pin;  // pin number
    uint32_t muxRegister;
    uint32_t configRegister;
    uint8_t af_list_len;  // length of available alternate functions list
    uint8_t adc_list_len; // length of available ADC options list
    const machine_pin_af_obj_t *af_list;  // pointer to list with alternate functions
    const machine_pin_adc_obj_t *adc_list; // pointer to list with ADC options
} machine_pin_obj_t;

typedef struct _machine_pin_irq_obj_t {
    mp_irq_obj_t base;
    uint32_t flags;
    uint32_t trigger;
} machine_pin_irq_obj_t;

// ------------------------------------------------------------------------------------------------------------------ //

extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_pin_af_type;

// ------------------------------------------------------------------------------------------------------------------ //

// Include board specific pins
#include "genhdr/pins.h"  // pins.h must included at this location

extern const machine_pin_obj_t *machine_pin_board_pins[];
extern const uint32_t num_board_pins;

extern const mp_obj_type_t machine_pin_board_pins_obj_type;
extern const mp_obj_type_t machine_pin_cpu_pins_obj_type;

extern const mp_obj_dict_t machine_pin_cpu_pins_locals_dict;
extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;

// ------------------------------------------------------------------------------------------------------------------ //

void pin_init(void);
uint32_t pin_get_mode(const machine_pin_obj_t *pin);
uint32_t pin_get_af(const machine_pin_obj_t *pin);
const machine_pin_obj_t *pin_find(mp_obj_t user_obj);
const machine_pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name);
const machine_pin_af_obj_t *pin_find_af(const machine_pin_obj_t *pin, uint8_t fn);
const machine_pin_af_obj_t *pin_find_af_by_index(const machine_pin_obj_t *pin, mp_uint_t af_idx);
const machine_pin_af_obj_t *pin_find_af_by_name(const machine_pin_obj_t *pin, const char *name);
void machine_pin_set_mode(const machine_pin_obj_t *pin, uint8_t mode);
uint32_t pin_generate_config(const uint32_t pull, const uint32_t mode, const uint32_t drive, uint32_t config_register);

#endif // MICROPY_INCLUDED_MIMXRT_PIN_H
