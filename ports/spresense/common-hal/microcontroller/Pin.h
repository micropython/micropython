/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#ifndef MICROPY_INCLUDED_SPRESENSE_COMMON_HAL_MICROCONTROLLER_PIN_H
#define MICROPY_INCLUDED_SPRESENSE_COMMON_HAL_MICROCONTROLLER_PIN_H

#include "py/obj.h"

extern const mp_obj_type_t mcu_pin_type;

#define PIN(pin, a) \
{ \
    { &mcu_pin_type }, \
    .number = (pin), \
    .analog = (a) \
}

typedef struct {
    mp_obj_base_t base;
    uint8_t number;
    bool analog;
} mcu_pin_obj_t;

extern const mcu_pin_obj_t pin_D0;
extern const mcu_pin_obj_t pin_D1;
extern const mcu_pin_obj_t pin_D2;
extern const mcu_pin_obj_t pin_D3;
extern const mcu_pin_obj_t pin_D4;
extern const mcu_pin_obj_t pin_D5;
extern const mcu_pin_obj_t pin_D6;
extern const mcu_pin_obj_t pin_D7;
extern const mcu_pin_obj_t pin_D8;
extern const mcu_pin_obj_t pin_D9;
extern const mcu_pin_obj_t pin_D10;
extern const mcu_pin_obj_t pin_D11;
extern const mcu_pin_obj_t pin_D12;
extern const mcu_pin_obj_t pin_D13;
extern const mcu_pin_obj_t pin_D14;
extern const mcu_pin_obj_t pin_D15;
extern const mcu_pin_obj_t pin_D16;
extern const mcu_pin_obj_t pin_D17;
extern const mcu_pin_obj_t pin_D18;
extern const mcu_pin_obj_t pin_D19;
extern const mcu_pin_obj_t pin_D20;
extern const mcu_pin_obj_t pin_D21;
extern const mcu_pin_obj_t pin_D22;
extern const mcu_pin_obj_t pin_D23;
extern const mcu_pin_obj_t pin_D24;
extern const mcu_pin_obj_t pin_D25;
extern const mcu_pin_obj_t pin_D26;
extern const mcu_pin_obj_t pin_D27;
extern const mcu_pin_obj_t pin_D28;
extern const mcu_pin_obj_t pin_LED0;
extern const mcu_pin_obj_t pin_LED1;
extern const mcu_pin_obj_t pin_LED2;
extern const mcu_pin_obj_t pin_LED3;
extern const mcu_pin_obj_t pin_A0;
extern const mcu_pin_obj_t pin_A1;
extern const mcu_pin_obj_t pin_A2;
extern const mcu_pin_obj_t pin_A3;
extern const mcu_pin_obj_t pin_A4;
extern const mcu_pin_obj_t pin_A5;

void never_reset_pin_number(uint8_t pin_number);
void reset_pin_number(uint8_t pin_number);
void reset_all_pins(void);
void claim_pin(const mcu_pin_obj_t* pin);

#endif // MICROPY_INCLUDED_SPRESENSE_COMMON_HAL_MICROCONTROLLER_PIN_H
