/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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

#include <stdint.h>
#include <stdio.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "lib/utils/pyexec.h"

#include "led.h"
#include "sdcard.h"
#include "extmod/vfs.h"


STATIC mp_obj_t pyb_main(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_opt, MP_ARG_INT, {.u_int = 0} }
    };

    if(mp_obj_is_str(pos_args[0]))
    {
        MP_STATE_PORT(pyb_config_main) = pos_args[0];

        /* parse args */
        mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
        mp_arg_parse_all(n_args - 1, pos_args+1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
        MP_STATE_VM(mp_optimise_value) = args[0].u_int;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_main_obj, 1, pyb_main);


STATIC const mp_rom_map_elem_t pyb_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pyb) },
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&pin_type) },
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pyb_led_type) },
    { MP_ROM_QSTR(MP_QSTR_Sdcard), MP_ROM_PTR(&pyb_sdcard_type) },
    { MP_ROM_QSTR(MP_QSTR_main), MP_ROM_PTR(&pyb_main_obj) },
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&mp_vfs_mount_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&pyb_module_globals,
};

