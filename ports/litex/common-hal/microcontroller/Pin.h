// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mphal.h"


typedef struct {
    mp_obj_base_t base;
    uint8_t number;
} mcu_pin_obj_t;

#define PIN(p_number)       \
    { \
        { &mcu_pin_type }, \
        .number = p_number \
    }

extern const mcu_pin_obj_t pin_TOUCH1;
extern const mcu_pin_obj_t pin_TOUCH2;
extern const mcu_pin_obj_t pin_TOUCH3;
extern const mcu_pin_obj_t pin_TOUCH4;

void reset_all_pins(void);
// reset_pin_number takes the pin number instead of the pointer so that objects don't
// need to store a full pointer.
void reset_pin_number(uint8_t pin_port, uint8_t pin_number);
void claim_pin(const mcu_pin_obj_t *pin);
bool pin_number_is_free(uint8_t pin_port, uint8_t pin_number);
void never_reset_pin_number(uint8_t pin_port, uint8_t pin_number);
// GPIO_TypeDef * pin_port(uint8_t pin_port);
uint16_t pin_mask(uint8_t pin_number);
