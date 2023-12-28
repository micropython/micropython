/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Jeff Epler for Adafruit Industries
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

#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/repl.h"

STATIC const char *get_arg_str(mp_obj_t arg, qstr name) {
    return mp_obj_str_get_str(mp_arg_validate_type_string(arg, name));
}

//| """Utilities to compile possibly incomplete Python source code."""
//|

//| def compile_command(source: str, filename: str = "<input>", symbol: str = "single"):
//|     """Compile a command and determine whether it is incomplete
//|
//|     The 'completeness' determination is slightly different than in standard Python
//|     (it's whatever the internal function ``mp_repl_continue_with_input`` does).
//|     In particular, it's important that the code not end with a newline character
//|     or it is likely to be treated as a complete command."""
//|
STATIC mp_obj_t compile_command(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_source, ARG_filename, ARG_symbol };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_source, MP_ARG_REQUIRED | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_filename, MP_ARG_OBJ, { .u_obj = MP_ROM_QSTR(MP_QSTR__lt_input_gt_) } },
        { MP_QSTR_symbol, MP_ARG_OBJ, { .u_obj = MP_ROM_QSTR(MP_QSTR_single) } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *source = get_arg_str(args[ARG_source].u_obj, MP_QSTR_source);
    if (mp_repl_continue_with_input(source)) {
        return mp_const_none;
    }

    return mp_call_function_n_kw((mp_obj_t)&mp_builtin_compile_obj, 3, 0, &args[0].u_obj);
}
MP_DEFINE_CONST_FUN_OBJ_KW(compile_command_obj, 1, compile_command);

STATIC const mp_rom_map_elem_t codeop_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_codeop) },
    { MP_ROM_QSTR(MP_QSTR_compile_command),  MP_ROM_PTR(&compile_command_obj) },
};

STATIC MP_DEFINE_CONST_DICT(codeop_module_globals, codeop_module_globals_table);

const mp_obj_module_t codeop_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&codeop_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_codeop, codeop_module);
