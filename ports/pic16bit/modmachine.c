/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
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

// Enhanced machine module for PIC16bit (dsPIC33) port

#include <stdint.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "board.h"

// dsPIC33 GPIO register definitions
// Note: Actual addresses depend on specific dsPIC33 variant
#define PORTA_BASE  0x02C0
#define PORTB_BASE  0x02C2
#define PORTC_BASE  0x02C4
#define PORTD_BASE  0x02C6

#define TRISA  (*(volatile uint16_t *)0x02C0)
#define TRISB  (*(volatile uint16_t *)0x02C6)
#define TRISC  (*(volatile uint16_t *)0x02CC)
#define TRISD  (*(volatile uint16_t *)0x02D2)

#define PORTA  (*(volatile uint16_t *)0x02C2)
#define PORTB  (*(volatile uint16_t *)0x02C8)
#define PORTC  (*(volatile uint16_t *)0x02CE)
#define PORTD  (*(volatile uint16_t *)0x02D4)

#define LATA   (*(volatile uint16_t *)0x02C4)
#define LATB   (*(volatile uint16_t *)0x02CA)
#define LATC   (*(volatile uint16_t *)0x02D0)
#define LATD   (*(volatile uint16_t *)0x02D6)

// ========== Pin Object ==========
typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t port;  // 0=A, 1=B, 2=C, 3=D
    uint8_t pin;   // 0-15
    uint8_t mode;  // 0=IN, 1=OUT
} machine_pin_obj_t;

static volatile uint16_t *get_tris_reg(uint8_t port) {
    switch (port) {
        case 0: return &TRISA;
        case 1: return &TRISB;
        case 2: return &TRISC;
        case 3: return &TRISD;
        default: return &TRISA;
    }
}

static volatile uint16_t *get_port_reg(uint8_t port) {
    switch (port) {
        case 0: return &PORTA;
        case 1: return &PORTB;
        case 2: return &PORTC;
        case 3: return &PORTD;
        default: return &PORTA;
    }
}

static volatile uint16_t *get_lat_reg(uint8_t port) {
    switch (port) {
        case 0: return &LATA;
        case 1: return &LATB;
        case 2: return &LATC;
        case 3: return &LATD;
        default: return &LATA;
    }
}

static mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

    machine_pin_obj_t *self = mp_obj_malloc(machine_pin_obj_t, type);

    // Parse pin number (port*16 + pin)
    mp_int_t pin_num = mp_obj_get_int(args[0]);
    self->port = pin_num / 16;
    self->pin = pin_num % 16;
    self->mode = 0;  // INPUT

    // Parse mode if provided
    if (n_args >= 2) {
        self->mode = mp_obj_get_int(args[1]);
    }

    // Configure pin direction
    volatile uint16_t *tris = get_tris_reg(self->port);
    if (self->mode == 0) {
        // Input
        *tris |= (1 << self->pin);
    } else {
        // Output
        *tris &= ~(1 << self->pin);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // Get value
        volatile uint16_t *port = get_port_reg(self->port);
        return MP_OBJ_NEW_SMALL_INT((*port >> self->pin) & 1);
    } else {
        // Set value
        volatile uint16_t *lat = get_lat_reg(self->port);
        mp_int_t value = mp_obj_get_int(args[1]);

        if (value) {
            *lat |= (1 << self->pin);
        } else {
            *lat &= ~(1 << self->pin);
        }
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    volatile uint16_t *lat = get_lat_reg(self->port);
    *lat |= (1 << self->pin);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    volatile uint16_t *lat = get_lat_reg(self->port);
    *lat &= ~(1 << self->pin);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },

    // Mode constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(1) },
};
static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_make_new,
    locals_dict, &machine_pin_locals_dict
);

// ========== Machine Module ==========

static mp_obj_t machine_reset(void) {
    // Trigger software reset on dsPIC33
    __asm__ volatile ("reset");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

static mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    // dsPIC33 typical: 40-70 MHz
    // Return configured frequency (from board config)
    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(40000000);  // 40 MHz default
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 1, machine_freq);

static mp_obj_t machine_idle(void) {
    __asm__ volatile ("pwrsav #1");  // Idle mode
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_idle_obj, machine_idle);

static const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_machine) },

    // Classes
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) },

    // Functions
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_idle), MP_ROM_PTR(&machine_idle_obj) },
};
static MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&machine_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_machine, mp_module_machine);
