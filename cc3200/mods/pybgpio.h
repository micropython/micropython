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

// This file requires pin_defs_xxx.h (which has port specific enums and
// defines, so we include it here. It should never be included directly

#include MICROPY_PIN_DEFS_PORT_H

typedef struct {
  mp_obj_base_t base;
  qstr name;
  uint32_t port;
  uint32_t bit           : 8;
  uint32_t pin_num       : 7;
} gpio_obj_t;

extern const mp_obj_type_t gpio_type;

typedef struct {
  const char *name;
  const gpio_obj_t *gpio;
} gpio_named_pin_t;

extern const gpio_named_pin_t gpio_cpu_pins[];

typedef struct {
    mp_obj_base_t base;
    qstr name;
    const gpio_named_pin_t *named_pins;
} gpio_named_pins_obj_t;

extern const mp_obj_type_t gpio_cpu_pins_obj_type;
extern const mp_obj_dict_t gpio_cpu_pins_locals_dict;

MP_DECLARE_CONST_FUN_OBJ(gpio_init_obj);

void gpio_init0(void);
void gpio_config(const gpio_obj_t *self, uint af, uint mode, uint type, uint strength);
const gpio_obj_t *gpio_find(mp_obj_t user_obj);
const gpio_obj_t *gpio_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name);
uint32_t gpio_get_mode(const gpio_obj_t *self);
uint32_t gpio_get_type(const gpio_obj_t *self);
uint32_t gpio_get_strenght(const gpio_obj_t *self);

