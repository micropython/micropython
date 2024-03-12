/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Josef Gajdusek
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_adc.c via MICROPY_PY_MACHINE_ADC_INCLUDEFILE.

#include "user_interface.h"

// The ADC class doesn't have any constants for this port.
#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    bool isvdd;
} machine_adc_obj_t;

static machine_adc_obj_t machine_adc_vdd3 = {{&machine_adc_type}, true};
static machine_adc_obj_t machine_adc_adc = {{&machine_adc_type}, false};

static uint16_t adc_read(machine_adc_obj_t *self) {
    if (self->isvdd) {
        return system_get_vdd33();
    } else {
        return system_adc_read();
    }
}

static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ADC(%u)", self->isvdd);
}

static mp_obj_t mp_machine_adc_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_int_t chn = mp_obj_get_int(args[0]);

    switch (chn) {
        case 0:
            return &machine_adc_adc;
        case 1:
            return &machine_adc_vdd3;
        default:
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("ADC(%d) doesn't exist"), chn);
    }
}

// read_u16()
static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
    uint32_t value = adc_read(self);
    return value * 65535 / 1024;
}

// Legacy method
static mp_int_t mp_machine_adc_read(machine_adc_obj_t *self) {
    return adc_read(self);
}
