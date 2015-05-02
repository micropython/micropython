/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#ifndef PYBPIN_H_
#define PYBPIN_H_

#define PYBPIN_ANALOG_TYPE          0xFF

enum {
  PORT_A0 = GPIOA0_BASE,
  PORT_A1 = GPIOA1_BASE,
  PORT_A2 = GPIOA2_BASE,
  PORT_A3 = GPIOA3_BASE
};

typedef struct {
    const mp_obj_base_t base;
    const qstr          name;
    const uint32_t      port;
    uint16_t            type;
    const uint8_t       bit;
    const uint8_t       pin_num;
    uint8_t             af;
    uint8_t             strength;
    uint8_t             mode;
    bool                isused;
} pin_obj_t;

extern const mp_obj_type_t pin_type;

typedef struct {
    const char *name;
    const pin_obj_t *pin;
} pin_named_pin_t;

typedef struct {
    mp_obj_base_t base;
    qstr name;
    const pin_named_pin_t *named_pins;
} pin_named_pins_obj_t;

extern const mp_obj_type_t pin_cpu_pins_obj_type;
extern const mp_obj_dict_t pin_cpu_pins_locals_dict;

void pin_init0(void);
void pin_verify_af (uint af);
void pin_config(pin_obj_t *self, uint af, uint mode, uint type, uint strength);
void pin_extint_register(pin_obj_t *self, uint32_t intmode, uint32_t priority);
pin_obj_t *pin_find(mp_obj_t user_obj);
pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name);
pin_obj_t *pin_find_pin_by_port_bit (const mp_obj_dict_t *named_pins, uint port, uint bit);
uint32_t pin_get_mode(const pin_obj_t *self);
uint32_t pin_get_type(const pin_obj_t *self);
uint32_t pin_get_strenght(const pin_obj_t *self);

#endif  // PYBPIN_H_
