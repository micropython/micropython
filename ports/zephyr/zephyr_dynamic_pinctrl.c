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

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/devicetree/pinctrl.h>
#include <zephyr/sys/util.h>

#include "py/runtime.h"
#include "py/objtuple.h"

#include <pinctrl_soc.h>

#include "zephyr_dynamic_pinctrl.h"

#include "dynamic_pinctrl/builder_api.h"

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_PINMUX_TYPE_UINT32_T
#include "dynamic_pinctrl/pinmux_uint32_t.c"
#else
#include CONFIG_MICROPY_DYNAMIC_PINCTRL_PINMUX_TYPE_FILE
#endif

#include CONFIG_MICROPY_DYNAMIC_PINCTRL_BUILDER_FILE

const mp_obj_type_t zephyr_pinctrl_type;

static void zephyr_pinctrl_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    zephyr_pinctrl_obj_t *self = self_in;

    vstr_t vstr;
    mp_print_t print_pinmux;
    vstr_init_print(&vstr, 16, &print_pinmux);
    mp_obj_print_helper(&print_pinmux, self->pinmux, PRINT_STR);
    mp_printf(print, "Pinctrl(%s)", vstr_null_terminated_str(&vstr));
}

static mp_obj_t zephyr_pinctrl_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *in_args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    enum { ARG_func, ARG_mode, ARG_pull, ARG_drive, ARG_schmitt_enable,  ARG_slew };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_func, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mode, MP_ARG_INT, {.u_int = 0 } },
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        { MP_QSTR_drive, MP_ARG_INT, {.u_int = 0 } },
        { MP_QSTR_schmitt_enable, MP_ARG_BOOL, {.u_bool = false } },
        { MP_QSTR_slew, MP_ARG_INT, {.u_int = 0 } },
    };


    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, in_args + n_args);
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, in_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    zephyr_pinctrl_obj_t *self = mp_obj_malloc(zephyr_pinctrl_obj_t, &zephyr_pinctrl_type);

    if (mp_obj_is_type(args[ARG_func].u_obj, &zephyr_pinmux_type)) {
        self->pinmux = args[ARG_func].u_obj;
    } else {
        mp_obj_t new_args[1] = { args[ARG_func].u_obj };
        self->pinmux = mp_call_function_n_kw(MP_OBJ_FROM_PTR(&zephyr_pinmux_type), 1, 0, new_args);
    }

    bool pull_up = args[ARG_mode].u_int & GPIO_PULL_UP;
    bool pull_down = args[ARG_mode].u_int & GPIO_PULL_DOWN;
    bool output_enable = args[ARG_mode].u_int & GPIO_OUTPUT;
    bool input_enable = args[ARG_mode].u_int & GPIO_INPUT;

    self->pinctrl = dynamic_pinctrl_builder_api.make_pinctrl(
        self->pinmux,
        pull_up,
        pull_down,
        args[ARG_schmitt_enable].u_bool,
        output_enable,
        input_enable,
        args[ARG_slew].u_int,
        args[ARG_drive].u_int);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t zephyr_pinctrl_pinmux(mp_obj_t self_in) {
    zephyr_pinctrl_obj_t *self = self_in;

    return self->pinmux;
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_pinctrl_pinmux_obj, zephyr_pinctrl_pinmux);

static const mp_rom_map_elem_t zephyr_pinctrl_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_Pinmux), MP_ROM_PTR(&zephyr_pinmux_type) },
    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
    { MP_ROM_QSTR(MP_QSTR_MUXES), MP_ROM_PTR(&generated_dynamic_pinmuxes_tobj) },
    #endif
    /* Instance method */
    { MP_ROM_QSTR(MP_QSTR_pinmux), MP_ROM_PTR(&zephyr_pinctrl_pinmux_obj) },
};
static MP_DEFINE_CONST_DICT(zephyr_pinctrl_locals_dict, zephyr_pinctrl_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_pinctrl_type,
    MP_QSTR_Pinctrl,
    MP_TYPE_FLAG_NONE,
    make_new, zephyr_pinctrl_make_new,
    print, zephyr_pinctrl_print,
    locals_dict, &zephyr_pinctrl_locals_dict
    );

#endif /* CONFIG_MICROPY_DYNAMIC_PINCTRL */
