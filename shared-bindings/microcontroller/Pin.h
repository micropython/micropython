/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_MICROCONTROLLER_PIN_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_MICROCONTROLLER_PIN_H

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t mcu_pin_type;

mcu_pin_obj_t *validate_obj_is_pin(mp_obj_t obj);
mcu_pin_obj_t *validate_obj_is_pin_or_none(mp_obj_t obj);
mcu_pin_obj_t *validate_obj_is_free_pin(mp_obj_t obj);
mcu_pin_obj_t *validate_obj_is_free_pin_or_none(mp_obj_t obj);
void validate_list_is_free_pins(qstr what, mcu_pin_obj_t **pins_out, mp_int_t max_pins, mp_obj_t seq, uint8_t *count_out);
void validate_pins(qstr what, uint8_t *pin_nos, mp_int_t max_pins, mp_obj_t seq, uint8_t *count_out);

void assert_pin_free(const mcu_pin_obj_t *pin);

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin);
void common_hal_never_reset_pin(const mcu_pin_obj_t *pin);
void common_hal_reset_pin(const mcu_pin_obj_t *pin);
uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin);
void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin);
void common_hal_mcu_pin_claim_number(uint8_t pin_no);
void common_hal_mcu_pin_reset_number(uint8_t pin_no);

#define COMMON_HAL_MCU_NO_PIN ((uint8_t)0xff)

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_MICROCONTROLLER_PIN_H
