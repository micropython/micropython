/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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

#ifndef MICROPY_INCLUDED_EXTMOD_MACHINE_PWM_H
#define MICROPY_INCLUDED_EXTMOD_MACHINE_PWM_H

#include "py/obj.h"

// A port must provide this type, but it's otherwise opaque.
typedef struct _machine_pwm_obj_t machine_pwm_obj_t;

// This PWM class is implemented by machine_pwm.c.
extern const mp_obj_type_t machine_pwm_type;

// A port must provide implementations of these low-level PWM functions, either as global
// linker symbols, or included directly if MICROPY_PY_MACHINE_PWM_INCLUDEFILE is defined.
#ifndef MICROPY_PY_MACHINE_PWM_INCLUDEFILE
void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
void mp_machine_pwm_deinit(machine_pwm_obj_t *self);
mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self);
void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq);
mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self);
void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty);
mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self);
void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16);
mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self);
void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns);
#endif

#endif // MICROPY_INCLUDED_EXTMOD_MACHINE_PWM_H
