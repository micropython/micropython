/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MASSDRIVER EI (massdriver.space)
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

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/devicetree/pinctrl.h>
#include <zephyr/sys/util.h>

#include "modzephyr.h"
#include "zephyr_device.h"
#include "py/runtime.h"

#if defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)

#include <pinctrl_soc.h>

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
#include CONFIG_MICROPY_DYNAMIC_PINCTRL_HEADER
#include "generated_dynamic_pinctrl.h"
#define PINMUX_COUNT (ARRAY_SIZE(generated_dynamic_pinctrl_names))
#endif

#include CONFIG_MICROPY_DYNAMIC_PINCTRL_HEADER_BUILDER

const mp_obj_type_t zephyr_pinctrl_type;

typedef struct _zephyr_pinctrl_obj_t {
    mp_obj_base_t base;
    uint32_t pinmux;
    bool bias_pull_up;
    bool bias_pull_down;
    bool input_schmitt_enable;
    bool output_enable;
    bool input_enable;
    int slew_rate;
    int drive_strength;
    pinctrl_soc_pin_t pinctrl;
} zephyr_pinctrl_obj_t;

pinctrl_soc_pin_t zephyr_pinctrl_get_pinctrl(const mp_obj_t obj) {
    if (!mp_obj_is_type(obj, &zephyr_pinctrl_type)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Argument is not of type zephyr.Pinctrl"));
    }
    zephyr_pinctrl_obj_t *this = MP_OBJ_TO_PTR(obj);
    return this->pinctrl;
}

static void zephyr_pinctrl_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    zephyr_pinctrl_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Pinctrl(%d, %d, pu=%s, pd=%s, schmitt_enable=%s, \
oe=%s, ie=%s, slew=%d, drive=%d)",
        mp_dynamic_pinctrl_get_pin(self->pinmux),
        mp_dynamic_pinctrl_get_function(self->pinmux),
        self->bias_pull_up ? "True" : "False",
        self->bias_pull_down ? "True" : "False",
        self->input_schmitt_enable ? "True" : "False",
        self->output_enable ? "True" : "False",
        self->input_enable ? "True" : "False",
        self->slew_rate,
        self->drive_strength
        );
}

static void zephyr_pinctrl_make_new_kw_args(zephyr_pinctrl_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_pu, ARG_pd, ARG_schmitt_enable, ARG_oe, ARG_ie, ARG_slew, ARG_drive };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pu, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false } },
        { MP_QSTR_pd, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false } },
        { MP_QSTR_schmitt_enable, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false } },
        { MP_QSTR_oe, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false } },
        { MP_QSTR_ie, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false } },
        { MP_QSTR_slew, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0 } },
        { MP_QSTR_drive, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 2 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->bias_pull_up = args[ARG_pu].u_bool;
    self->bias_pull_down = args[ARG_pd].u_bool;
    self->input_schmitt_enable = args[ARG_schmitt_enable].u_bool;
    self->output_enable = args[ARG_oe].u_bool;
    self->input_enable = args[ARG_ie].u_bool;
    self->slew_rate = args[ARG_slew].u_int;
    self->drive_strength = args[ARG_drive].u_int;

    self->pinctrl = mp_dynamic_pinctrl_make_pinctrl_soc_pin_t(self->pinmux,
        args[ARG_pu].u_bool,
        args[ARG_pd].u_bool,
        args[ARG_schmitt_enable].u_bool,
        args[ARG_oe].u_bool,
        args[ARG_ie].u_bool,
        args[ARG_slew].u_int,
        args[ARG_drive].u_int);
}

static mp_obj_t zephyr_pinctrl_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
    size_t index = 0;
    #endif
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
    bool arbitrary = true;

    if (mp_obj_is_str(args[0])) {
        #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
        const char *name = mp_obj_str_get_str(args[0]);
        for (index = 0; index < PINMUX_COUNT; index++) {
            if (strcmp(generated_dynamic_pinctrl_names[index], name) == 0) {
                break;
            }
        }
        if (index >= PINMUX_COUNT) {
            mp_raise_ValueError(MP_ERROR_TEXT("Not a valid pinmux name"));
        }
        arbitrary = false;
        #else
        mp_raise_ValueError(MP_ERROR_TEXT("Pinmux not generated"));
        #endif
    } else if (mp_obj_is_int(args[0]) && n_args > 1) {
        if (!mp_obj_is_int(args[1])) {
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid pinmux combination"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid pinmux combination"));
    }

    zephyr_pinctrl_obj_t *self = mp_obj_malloc(zephyr_pinctrl_obj_t, &zephyr_pinctrl_type);
    if (arbitrary) {
        self->pinmux = mp_dynamic_pinctrl_make_pinmux(mp_obj_get_int(args[0]),
            mp_obj_get_int(args[1]));
    } else {
        #ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
        self->pinmux = generated_dynamic_pinctrl_pinmux[index];
        #endif
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    if (arbitrary) {
        zephyr_pinctrl_make_new_kw_args(self, n_args - 2, args + 2, &kw_args);
    } else {
        zephyr_pinctrl_make_new_kw_args(self, n_args - 1, args + 1, &kw_args);
    }
    return MP_OBJ_FROM_PTR(self);
}

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
static mp_obj_t zephyr_pinctrl_listmuxes(void) {
    mp_obj_t data[PINMUX_COUNT];
    for (int i = 0; i < PINMUX_COUNT; i++) {
        data[i] = mp_obj_new_str_from_cstr(generated_dynamic_pinctrl_names[i]);
    }
    return mp_obj_new_list(PINMUX_COUNT, data);
}

static MP_DEFINE_CONST_FUN_OBJ_0(zephyr_pinctrl_listmuxes_obj, zephyr_pinctrl_listmuxes);

static mp_obj_t zephyr_pinctrl_listmuxes_function(mp_obj_t arg) {
    mp_obj_t data[PINMUX_COUNT];
    size_t cnt = 0;
    int function = mp_obj_get_int(arg);

    for (int i = 0; i < PINMUX_COUNT; i++) {
        if (mp_dynamic_pinctrl_get_function(generated_dynamic_pinctrl_pinmux[i]) == function) {
            data[cnt] = mp_obj_new_str_from_cstr(generated_dynamic_pinctrl_names[i]);
            cnt++;
        }
    }
    return mp_obj_new_list(cnt, data);
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_pinctrl_listmuxes_function_obj, zephyr_pinctrl_listmuxes_function);

static mp_obj_t zephyr_pinctrl_listmuxes_pin(mp_obj_t arg) {
    mp_obj_t data[PINMUX_COUNT];
    size_t cnt = 0;
    int pin = mp_obj_get_int(arg);

    for (int i = 0; i < PINMUX_COUNT; i++) {
        if (mp_dynamic_pinctrl_get_pin(generated_dynamic_pinctrl_pinmux[i]) == pin) {
            data[cnt] = mp_obj_new_str_from_cstr(generated_dynamic_pinctrl_names[i]);
            cnt++;
        }
    }
    return mp_obj_new_list(cnt, data);
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_pinctrl_listmuxes_pin_obj, zephyr_pinctrl_listmuxes_pin);
#endif

static const mp_rom_map_elem_t zephyr_pinctrl_locals_dict_table[] = {
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

#ifdef CONFIG_MICROPY_DYNAMIC_PINCTRL_GENERATE_PINMUX
static const mp_rom_map_elem_t mp_module_pinmux_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pinmux) },
    { MP_ROM_QSTR(MP_QSTR_listmuxes), MP_ROM_PTR(&zephyr_pinctrl_listmuxes_obj) },
    { MP_ROM_QSTR(MP_QSTR_listmuxes_function), MP_ROM_PTR(&zephyr_pinctrl_listmuxes_function_obj) },
    { MP_ROM_QSTR(MP_QSTR_listmuxes_pin), MP_ROM_PTR(&zephyr_pinctrl_listmuxes_pin_obj) },
};

static MP_DEFINE_CONST_DICT(mp_module_pinmux_globals, mp_module_pinmux_globals_table);

const mp_obj_module_t mp_module_pinmux = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_pinmux_globals,
};

MP_REGISTER_MODULE(MP_QSTR_pinmux, mp_module_pinmux);
#endif

#endif // defined(CONFIG_MICROPY_DYNAMIC_PINCTRL)
