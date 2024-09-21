/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jonathan Hogg
 * Copyright (c) 2023 Damien P. George
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

#include "py/runtime.h"

#if MICROPY_PY_MACHINE_ADC_BLOCK

#include "py/mphal.h"
#include "extmod/modmachine.h"

// The port must provide implementations of these low-level ADCBlock functions.
static void mp_machine_adc_block_print(const mp_print_t *print, machine_adc_block_obj_t *self);
static machine_adc_block_obj_t *mp_machine_adc_block_get(mp_int_t unit);
static void mp_machine_adc_block_bits_set(machine_adc_block_obj_t *self, mp_int_t bits);
static machine_adc_obj_t *mp_machine_adc_block_connect(machine_adc_block_obj_t *self, mp_int_t channel_id, mp_hal_pin_obj_t pin, mp_map_t *kw_args);

// The port provides implementations of the above in this file.
#include MICROPY_PY_MACHINE_ADC_BLOCK_INCLUDEFILE

static void machine_adc_block_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_adc_block_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_adc_block_print(print, self);
}

static void machine_adc_block_init_helper(machine_adc_block_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_bits,
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_pos_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t bits = args[ARG_bits].u_int;
    mp_machine_adc_block_bits_set(self, bits);
}

static mp_obj_t machine_adc_block_make_new(const mp_obj_type_t *type, size_t n_pos_args, size_t n_kw_args, const mp_obj_t *args) {
    mp_arg_check_num(n_pos_args, n_kw_args, 1, MP_OBJ_FUN_ARGS_MAX, true);
    mp_int_t unit = mp_obj_get_int(args[0]);
    machine_adc_block_obj_t *self = mp_machine_adc_block_get(unit);
    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid block id"));
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw_args, args + n_pos_args);
    machine_adc_block_init_helper(self, n_pos_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_adc_block_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_adc_block_obj_t *self = pos_args[0];
    machine_adc_block_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_adc_block_init_obj, 1, machine_adc_block_init);

static mp_obj_t machine_adc_block_connect(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_adc_block_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_int_t channel_id = -1;
    mp_hal_pin_obj_t pin = -1;
    if (n_pos_args == 2) {
        if (mp_obj_is_int(pos_args[1])) {
            channel_id = mp_obj_get_int(pos_args[1]);
        } else {
            pin = mp_hal_get_pin_obj(pos_args[1]);
        }
    } else if (n_pos_args == 3) {
        channel_id = mp_obj_get_int(pos_args[1]);
        pin = mp_hal_get_pin_obj(pos_args[2]);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("too many positional args"));
    }

    machine_adc_obj_t *adc = mp_machine_adc_block_connect(self, channel_id, pin, kw_args);
    if (adc == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("no matching ADC"));
    }

    return MP_OBJ_FROM_PTR(adc);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_adc_block_connect_obj, 2, machine_adc_block_connect);

static const mp_rom_map_elem_t machine_adc_block_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_adc_block_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&machine_adc_block_connect_obj) },
};
static MP_DEFINE_CONST_DICT(machine_adc_block_locals_dict, machine_adc_block_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adc_block_type,
    MP_QSTR_ADCBlock,
    MP_TYPE_FLAG_NONE,
    make_new, machine_adc_block_make_new,
    print, machine_adc_block_print,
    locals_dict, &machine_adc_block_locals_dict
    );

#endif // MICROPY_PY_MACHINE_ADC_BLOCK
