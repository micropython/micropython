/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MASSDRIVER EI (massdriver.space)
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

const mp_obj_type_t machine_pin_pinmux_type;

typedef struct _machine_pin_pinmux_obj_t {
    mp_obj_base_t base;
    uint32_t pinmux;
    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
    const char *key;
    #endif
} machine_pin_pinmux_obj_t;

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
#include CONFIG_MICROPY_DYNAMIC_PINCTRL_HEADER
#include "generated_dynamic_pinctrl.h"
#define PINMUX_COUNT (ARRAY_SIZE(generated_dynamic_pinmuxes_names))
#endif

static void machine_pin_pinmux_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_pinmux_obj_t *self = self_in;

    if (kind == PRINT_STR) {
        #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
        if (self->key != NULL) {
            mp_printf(print, "%s", self->key);
            return;
        }
        #endif
        mp_printf(print, "%x", self->pinmux);
    } else {
        #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
        if (self->key != NULL) {
            mp_printf(print, "Pinmux(%s = %x)", self->key, self->pinmux);
            return;
        }
        #endif
        mp_printf(print, "Pinmux(%x)", self->pinmux);
    }
}

static mp_obj_t machine_pin_pinmux_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    machine_pin_pinmux_obj_t *self;

    if (n_args == 1) {
        if (mp_obj_is_str(args[0])) {
            #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
            const char *name = mp_obj_str_get_str(args[0]);
            size_t index;
            size_t dummy_len;
            for (index = 0; index < PINMUX_COUNT; index++) {
                if (strcmp(qstr_data(generated_dynamic_pinmuxes_names[index], &dummy_len), name) == 0) {
                    break;
                }
            }
            if (index >= PINMUX_COUNT) {
                mp_raise_ValueError(MP_ERROR_TEXT("not a valid pinmux name"));
            }
            self = mp_obj_malloc(machine_pin_pinmux_obj_t, &machine_pin_pinmux_type);
            self->pinmux = mp_obj_get_uint(generated_dynamic_pinmuxes_tobj.items[index]);
            self->key = qstr_data(generated_dynamic_pinmuxes_names[index], &dummy_len);
            return MP_OBJ_FROM_PTR(self);
            #else
            mp_raise_ValueError(MP_ERROR_TEXT("pinmuxes not generated"));
            #endif
        } else if (mp_obj_is_int(args[0])) {
            self = mp_obj_malloc(machine_pin_pinmux_obj_t, &machine_pin_pinmux_type);
            self->pinmux = mp_obj_get_uint(args[0]);
            self->key = NULL;
            return MP_OBJ_FROM_PTR(self);
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid pinmux"));
        }
        return mp_const_none;
    }

    self = mp_obj_malloc(machine_pin_pinmux_obj_t, &machine_pin_pinmux_type);
    self->pinmux = dynamic_pinctrl_builder_api.make_pinmux(mp_obj_get_uint(args[0]), mp_obj_get_uint(args[1]));
    self->key = NULL;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_pin_pinmux_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    machine_pin_pinmux_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_INT_MAYBE:
            return mp_obj_new_int_from_uint(self->pinmux);
        default:
            return MP_OBJ_NULL;
    }
}

static mp_obj_t machine_pin_pinmux_pin(mp_obj_t self_in) {
    machine_pin_pinmux_obj_t *self = self_in;

    return mp_obj_new_int_from_uint(dynamic_pinctrl_builder_api.get_pin(self->pinmux));
}

static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_pinmux_pin_obj, machine_pin_pinmux_pin);

static mp_obj_t machine_pin_pinmux_function(mp_obj_t self_in) {
    machine_pin_pinmux_obj_t *self = self_in;

    return mp_obj_new_int_from_uint(dynamic_pinctrl_builder_api.get_function(self->pinmux));
}

static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_pinmux_function_obj, machine_pin_pinmux_function);

static const mp_rom_map_elem_t machine_pin_pinmux_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&machine_pin_pinmux_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_function), MP_ROM_PTR(&machine_pin_pinmux_function_obj) },
};

static MP_DEFINE_CONST_DICT(machine_pin_pinmux_locals_dict, machine_pin_pinmux_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_pinmux_type,
    MP_QSTR_Pinmux,
    MP_TYPE_FLAG_NONE,
    make_new, machine_pin_pinmux_make_new,
    print, machine_pin_pinmux_print,
    locals_dict, &machine_pin_pinmux_locals_dict,
    unary_op, machine_pin_pinmux_unary_op
    );
