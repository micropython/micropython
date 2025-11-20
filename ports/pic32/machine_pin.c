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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"

// PIC32MX GPIO Register Definitions
// Base addresses for PIC32MX795F512L
#define PORTA_BASE  0xBF886000
#define PORTB_BASE  0xBF886100
#define PORTC_BASE  0xBF886200
#define PORTD_BASE  0xBF886300
#define PORTE_BASE  0xBF886400
#define PORTF_BASE  0xBF886500
#define PORTG_BASE  0xBF886600

// Register offsets
#define TRIS_OFFSET    0x00  // Direction (1=input, 0=output)
#define PORT_OFFSET    0x10  // Read pin state
#define LAT_OFFSET     0x20  // Output latch
#define ODCSET_OFFSET  0x30  // Open drain config set
#define ANSEL_OFFSET   0x40  // Analog select (1=analog, 0=digital)

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint8_t port;      // 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G
    uint8_t pin;       // 0-15
    uint8_t mode;      // 0=IN, 1=OUT
    uint8_t pull;      // 0=NONE (not available on PIC32)
    uint8_t value;     // Current value
} machine_pin_obj_t;

STATIC uint32_t get_port_base(uint8_t port) {
    switch (port) {
        case 0: return PORTA_BASE;
        case 1: return PORTB_BASE;
        case 2: return PORTC_BASE;
        case 3: return PORTD_BASE;
        case 4: return PORTE_BASE;
        case 5: return PORTF_BASE;
        case 6: return PORTG_BASE;
        default: return PORTA_BASE;
    }
}

STATIC void pin_set_mode(machine_pin_obj_t *self, uint8_t mode) {
    uint32_t base = get_port_base(self->port);
    volatile uint32_t *tris = (volatile uint32_t *)(base + TRIS_OFFSET);
    volatile uint32_t *ansel = (volatile uint32_t *)(base + ANSEL_OFFSET);

    // Configure as digital I/O
    *ansel &= ~(1 << self->pin);

    if (mode == 0) {
        // Input mode
        *tris |= (1 << self->pin);
    } else {
        // Output mode
        *tris &= ~(1 << self->pin);
    }

    self->mode = mode;
}

// Pin(id, mode=Pin.IN, pull=None, value=None)
STATIC mp_obj_t machine_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 4, false);

    // Get pin number
    mp_int_t pin_num = mp_obj_get_int(args[0]);

    // Validate pin number (0-111 for ports A-G, 16 pins each)
    if (pin_num < 0 || pin_num >= 112) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
    }

    machine_pin_obj_t *self = mp_obj_malloc(machine_pin_obj_t, type);
    self->port = pin_num / 16;
    self->pin = pin_num % 16;
    self->mode = 0;  // Default to INPUT
    self->pull = 0;  // Not supported on PIC32
    self->value = 0;

    // Get mode if provided
    if (n_args >= 2) {
        self->mode = mp_obj_get_int(args[1]);
    }

    // Configure pin
    pin_set_mode(self, self->mode);

    // Set initial value if provided and mode is OUTPUT
    if (n_args >= 4 && self->mode == 1) {
        mp_int_t value = mp_obj_get_int(args[3]);
        uint32_t base = get_port_base(self->port);
        volatile uint32_t *lat = (volatile uint32_t *)(base + LAT_OFFSET);

        if (value) {
            *lat |= (1 << self->pin);
        } else {
            *lat &= ~(1 << self->pin);
        }
        self->value = value ? 1 : 0;
    }

    return MP_OBJ_FROM_PTR(self);
}

// pin.value([val])
STATIC mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t base = get_port_base(self->port);

    if (n_args == 1) {
        // Get value
        volatile uint32_t *port = (volatile uint32_t *)(base + PORT_OFFSET);
        return MP_OBJ_NEW_SMALL_INT((*port >> self->pin) & 1);
    } else {
        // Set value
        mp_int_t value = mp_obj_get_int(args[1]);
        volatile uint32_t *lat = (volatile uint32_t *)(base + LAT_OFFSET);

        if (value) {
            *lat |= (1 << self->pin);
            self->value = 1;
        } else {
            *lat &= ~(1 << self->pin);
            self->value = 0;
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

// pin.on()
STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t base = get_port_base(self->port);
    volatile uint32_t *lat = (volatile uint32_t *)(base + LAT_OFFSET);
    *lat |= (1 << self->pin);
    self->value = 1;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

// pin.off()
STATIC mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t base = get_port_base(self->port);
    volatile uint32_t *lat = (volatile uint32_t *)(base + LAT_OFFSET);
    *lat &= ~(1 << self->pin);
    self->value = 0;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

// pin.mode([mode])
STATIC mp_obj_t machine_pin_mode(size_t n_args, const mp_obj_t *args) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(self->mode);
    } else {
        mp_int_t mode = mp_obj_get_int(args[1]);
        pin_set_mode(self, mode);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_mode_obj, 1, 2, machine_pin_mode);

STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode), MP_ROM_PTR(&machine_pin_mode_obj) },

    // Mode constants
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN), MP_ROM_INT(2) },

    // Pull constants (not supported but defined for compatibility)
    { MP_ROM_QSTR(MP_QSTR_PULL_NONE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_PULL_UP), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_PULL_DOWN), MP_ROM_INT(2) },
};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

STATIC const mp_pin_p_t machine_pin_obj_protocol = {
    .ioctl = NULL,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_make_new,
    locals_dict, &machine_pin_locals_dict,
    protocol, &machine_pin_obj_protocol
);
