/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2017 Eric Poulsen
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

#include <string.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "esp_log.h"
#include "esp_task_wdt.h"

const mp_obj_type_t machine_wdt_type;

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
} machine_wdt_obj_t;

STATIC machine_wdt_obj_t wdt_default = {{&machine_wdt_type}};

STATIC mp_obj_t machine_wdt_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 2, true);
    
    enum {
        ARG_timeout, ARG_panic
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_panic, MP_ARG_BOOL, {.u_bool = true} }
    };

    mp_map_t kw_args;
    mp_arg_val_t mp_args[MP_ARRAY_SIZE(allowed_args)];

    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_arg_parse_all(n_args, args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, mp_args);

    if (mp_args[ARG_timeout].u_int == 0) {
        esp_task_wdt_add(NULL);
        ESP_LOGI("WDT init", "default WDT");
        return &wdt_default;
    } else if (mp_args[ARG_timeout].u_int > 0) {
        
        mp_int_t rs_code = esp_task_wdt_init(mp_args[ARG_timeout].u_int, mp_args[ARG_panic].u_bool);
        if (rs_code != ESP_OK) {
          mp_raise_OSError(rs_code);
        }

        ESP_LOGI("WDT init", "timeout: %ds, panic: %d", mp_args[ARG_timeout].u_int, mp_args[ARG_panic].u_bool);

        esp_task_wdt_add(NULL);
        return &wdt_default;
    } else {
        mp_raise_ValueError(NULL);
    }
}

STATIC mp_obj_t machine_wdt_feed(mp_obj_t self_in) {
    (void)self_in;
    esp_task_wdt_reset();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_wdt_feed_obj, machine_wdt_feed);

STATIC const mp_rom_map_elem_t machine_wdt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&machine_wdt_feed_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_wdt_locals_dict, machine_wdt_locals_dict_table);

const mp_obj_type_t machine_wdt_type = {
    { &mp_type_type },
    .name = MP_QSTR_WDT,
    .make_new = machine_wdt_make_new,
    .locals_dict = (mp_obj_t)&machine_wdt_locals_dict,
};
