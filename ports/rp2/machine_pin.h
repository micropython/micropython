/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Ibrahim Abdelkader <iabdalkader@openmv.io>
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
#ifndef MICROPY_INCLUDED_RP2_MACHINE_PIN_H
#define MICROPY_INCLUDED_RP2_MACHINE_PIN_H

#include <stdint.h>
#include "py/obj.h"
#include "py/mphal.h"

enum {
    MACHINE_PIN_MODE_IN = 0,
    MACHINE_PIN_MODE_OUT = 1,
    MACHINE_PIN_MODE_OPEN_DRAIN = 2,
    MACHINE_PIN_MODE_ALT = 3,
    MACHINE_PIN_MODE_ANALOG = 4
};

typedef struct _machine_pin_af_obj_t {
    mp_obj_base_t base;
    qstr name;
    uint8_t idx     : 4;
    uint8_t fn      : 4;
    uint8_t unit    : 8;
} machine_pin_af_obj_t;

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    qstr name;
    uint8_t id                  : 6;
    #if MICROPY_HW_PIN_EXT_COUNT
    uint8_t is_ext              : 1;
    uint8_t is_output           : 1;
    uint8_t last_output_value   : 1;
    #endif
    uint8_t af_num              : 4;
    const machine_pin_af_obj_t *af;
} machine_pin_obj_t;

extern const mp_obj_type_t machine_pin_af_type;

// Include all of the individual pin objects
#include "genhdr/pins.h"

extern const machine_pin_obj_t machine_pin_obj_table[NUM_BANK0_GPIOS];

extern const mp_obj_type_t pin_cpu_pins_obj_type;
extern const mp_obj_dict_t machine_pin_cpu_pins_locals_dict;

extern const mp_obj_type_t pin_board_pins_obj_type;
extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;

void machine_pin_ext_init(void);
bool machine_pin_ext_is_adc_channel(const machine_pin_obj_t *self);
uint32_t machine_pin_ext_to_adc_channel(const machine_pin_obj_t *self);
void machine_pin_ext_set(machine_pin_obj_t *self, bool value);
bool machine_pin_ext_get(machine_pin_obj_t *self);
uint16_t machine_pin_ext_read_u16(uint32_t channel);
void machine_pin_ext_config(machine_pin_obj_t *self, int mode, int value);

const machine_pin_obj_t *machine_pin_find(mp_obj_t pin);
const machine_pin_obj_t *machine_pin_find_named(const mp_obj_dict_t *named_pins, mp_obj_t name);
const machine_pin_af_obj_t *machine_pin_find_alt(const machine_pin_obj_t *pin, uint8_t fn);
const machine_pin_af_obj_t *machine_pin_find_alt_by_index(const machine_pin_obj_t *pin, mp_uint_t af_idx);

#endif // MICROPY_INCLUDED_RP2_MACHINE_PIN_H
