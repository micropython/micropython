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

#if defined (MICROPY_PY_LED) && MICROPY_PY_LED
#include "led.h"
#endif
#include "sdcard.h"
#include "extmod/vfs.h"
#if defined (MICROPY_PY_LPI2C) && MICROPY_PY_LPI2C  
#include "pybi2c.h"
#endif
#if defined (MICROPY_PY_GPIO) && MICROPY_PY_GPIO
#include "pybgpio.h"
#endif
#if defined (MICROPY_PY_GPT) && MICROPY_PY_GPT
#include "pybgpt.h"
#endif
#if defined (MICROPY_PY_PINMUX) && MICROPY_PY_PINMUX
#include "pybpinmux.h"
#endif

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
#if defined (MICROPY_PY_LED) && MICROPY_PY_LED
    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&pin_type) },
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pyb_led_type) },
#endif
    { MP_ROM_QSTR(MP_QSTR_Sdcard), MP_ROM_PTR(&pyb_sdcard_type) },
    { MP_ROM_QSTR(MP_QSTR_main), MP_ROM_PTR(&pyb_main_obj) },
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&mp_vfs_mount_obj) },
#if defined (MICROPY_PY_LPI2C) && MICROPY_PY_LPI2C    
    { MP_ROM_QSTR(MP_QSTR_LPI2C), MP_ROM_PTR(&pyb_i2c_type) },
#endif
#if defined (MICROPY_PY_PINMUX) && MICROPY_PY_PINMUX
    { MP_ROM_QSTR(MP_QSTR_port), MP_ROM_PTR(&port_type) },
    { MP_ROM_QSTR(MP_QSTR_pinmux), MP_ROM_PTR(&pinmux_type) },
#endif
#if defined (MICROPY_PY_GPIO) && MICROPY_PY_GPIO
    { MP_ROM_QSTR(MP_QSTR_gpio), MP_ROM_PTR(&pyb_gpio_type) },
#endif
#if defined (MICROPY_PY_GPT) && MICROPY_PY_GPT
    { MP_ROM_QSTR(MP_QSTR_GPT), MP_ROM_PTR(&pyb_gpt_type) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&pyb_module_globals,
};

