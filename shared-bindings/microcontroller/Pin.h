// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t mcu_pin_type;

const mcu_pin_obj_t *validate_obj_is_pin(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_pin_in(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_pin_or_none(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_free_pin(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_free_pin_or_none(mp_obj_t obj, qstr arg_name);
void validate_no_duplicate_pins(mp_obj_t seq, qstr arg_name);
void validate_no_duplicate_pins_2(mp_obj_t seq1, mp_obj_t seq2, qstr arg_name1, qstr arg_name2);
void validate_list_is_free_pins(qstr what, const mcu_pin_obj_t **pins_out, mp_int_t max_pins, mp_obj_t seq, uint8_t *count_out);
void validate_pins(qstr what, uint8_t *pin_nos, mp_int_t max_pins, mp_obj_t seq, uint8_t *count_out);
NORETURN void raise_ValueError_invalid_pin(void);
NORETURN void raise_ValueError_invalid_pins(void);
NORETURN void raise_ValueError_invalid_pin_name(qstr pin_name);

void assert_pin_free(const mcu_pin_obj_t *pin);

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin);
void common_hal_never_reset_pin(const mcu_pin_obj_t *pin);
void common_hal_reset_pin(const mcu_pin_obj_t *pin);
uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin);
void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin);
void common_hal_mcu_pin_claim_number(uint8_t pin_no);
void common_hal_mcu_pin_reset_number(uint8_t pin_no);

void shared_bindings_microcontroller_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);

#define COMMON_HAL_MCU_NO_PIN ((uint8_t)0xff)
