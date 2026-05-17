/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nick Moore
 * Copyright (c) 2023 Vekatech Ltd.
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
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "pin.h"
#include "ra/ra_dac.h"
#include "modmachine.h"

#if MICROPY_PY_MACHINE_DAC

typedef struct _machine_dac_obj_t {
    mp_obj_base_t base;
    uint8_t active;
    uint8_t ch;
    uint16_t mv;
    mp_hal_pin_obj_t dac;
} machine_dac_obj_t;

static machine_dac_obj_t machine_dac_obj[] = {
    #if defined(MICROPY_HW_DAC0)
    {{&machine_dac_type}, 0, 0, 0, MICROPY_HW_DAC0},
    #endif
    #if defined(MICROPY_HW_DAC1)
    {{&machine_dac_type}, 0, 1, 0, MICROPY_HW_DAC1}
    #endif
};

static void machine_dac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);     // const char *qstr_str(qstr q);
    mp_printf(print, "DAC(DA%d [#%d], active=%u, out=%u mV)", self->ch, self->dac->pin, self->active, self->mv);
}

static mp_obj_t machine_dac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_hal_pin_obj_t pin_id = MP_OBJ_NULL;
    machine_dac_obj_t *self = MP_OBJ_NULL;

    enum { ARG_pin };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,  MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} }
    };

    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    mp_arg_val_t init_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, init_args);

    // Get GPIO and optional device to connect to DAC.
    pin_id = mp_hal_get_pin_obj(init_args[ARG_pin].u_obj);

    if (pin_id) {
        for (int i = 0; i < MP_ARRAY_SIZE(machine_dac_obj); i++) {
            if (pin_id->pin == machine_dac_obj[i].dac->pin) {
                self = &machine_dac_obj[i];
                break;
            }
        }

        if (self) {
            if (ra_dac_is_dac_pin(self->dac->pin)) {
                ra_dac_init(self->dac->pin, self->ch);
                self->active = ra_dac_is_running(self->ch);
            } else {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%d) has no DAC output"), self->dac->pin);
            }
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%d) is used with other peripheral"), pin_id->pin);
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Pin doesn't exist"));
    }

    return MP_OBJ_FROM_PTR(self);
}

// DAC.deinit()
static mp_obj_t machine_dac_deinit(mp_obj_t self_in) {
    machine_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    ra_dac_deinit(self->dac->pin, self->ch);
    self->active = ra_dac_is_running(self->ch);
    self->ch = 0;
    self->mv = 0;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_dac_deinit_obj, machine_dac_deinit);

// DAC.write(value)
static mp_obj_t machine_dac_write(mp_obj_t self_in, mp_obj_t data) { // mp_obj_t value_in
    machine_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t value = mp_obj_get_int(data);

    if (value < 0 || value > 4095) {
        mp_raise_ValueError(MP_ERROR_TEXT("value should be 0-4095"));
    } else
    if (self->active) {
        ra_dac_write(self->ch, value);
        self->mv = (value * 3300) / 4095;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_dac_write_obj, machine_dac_write);

// DAC.read()
static mp_obj_t machine_dac_read(mp_obj_t self_in) {
    machine_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(ra_dac_read(self->ch));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_dac_read_obj, machine_dac_read);

// DAC.write_mv(Vout)
static mp_obj_t machine_dac_write_mv(mp_obj_t self_in, mp_obj_t data) {  // mp_obj_t self_in, mp_obj_t value_in
    machine_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t Vout = mp_obj_get_int(data);

    if (Vout < 0 || Vout > 3300) {
        mp_raise_ValueError(MP_ERROR_TEXT("value should be 0-3300"));
    } else
    if (self->active) {
        uint16_t Dout = (Vout * 4095) / 3300;
        ra_dac_write(self->ch, Dout);
        self->mv = Vout;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_dac_write_mv_obj, machine_dac_write_mv);

// DAC.read_mv()
static mp_obj_t machine_dac_read_mv(mp_obj_t self_in) {
    machine_dac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(self->mv);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_dac_read_mv_obj, machine_dac_read_mv);

// MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_dac_write_obj, mp_machine_dac_write);

static const mp_rom_map_elem_t machine_dac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_dac_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_dac_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_dac_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_mv), MP_ROM_PTR(&machine_dac_read_mv_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_mv), MP_ROM_PTR(&machine_dac_write_mv_obj) }
};

static MP_DEFINE_CONST_DICT(machine_dac_locals_dict, machine_dac_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_dac_type,
    MP_QSTR_DAC,
    MP_TYPE_FLAG_NONE,
    make_new, machine_dac_make_new,
    print, machine_dac_print,
    locals_dict, &machine_dac_locals_dict
    );

#endif // MICROPY_PY_MACHINE_DAC
