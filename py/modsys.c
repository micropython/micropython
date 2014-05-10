/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"
#include "runtime.h"
#include "objlist.h"
#include "objtuple.h"
#include "objstr.h"

#if MICROPY_ENABLE_MOD_SYS

MP_DECLARE_CONST_FUN_OBJ(mp_sys_exit_obj);

// These should be implemented by ports, specific types don't matter,
// only addresses.
struct _dummy_t;
extern struct _dummy_t mp_sys_stdin_obj;
extern struct _dummy_t mp_sys_stdout_obj;
extern struct _dummy_t mp_sys_stderr_obj;

mp_obj_list_t mp_sys_path_obj;
mp_obj_list_t mp_sys_argv_obj;
#define I(n) MP_OBJ_NEW_SMALL_INT(n)
// TODO: CPython is now at 5-element array, but save 2 els so far...
STATIC const mp_obj_tuple_t mp_sys_version_info_obj = {{&mp_type_tuple}, 3, {I(3), I(4), I(0)}};
#undef I
STATIC const MP_DEFINE_STR_OBJ(version_obj, "3.4.0");

STATIC const mp_map_elem_t mp_module_sys_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_sys) },
    // Should be implemented by port
    { MP_OBJ_NEW_QSTR(MP_QSTR_exit), (mp_obj_t)&mp_sys_exit_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_path), (mp_obj_t)&mp_sys_path_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_argv), (mp_obj_t)&mp_sys_argv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_version), (mp_obj_t)&version_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_version_info), (mp_obj_t)&mp_sys_version_info_obj },
#if MP_ENDIANNESS_LITTLE
    { MP_OBJ_NEW_QSTR(MP_QSTR_byteorder), MP_OBJ_NEW_QSTR(MP_QSTR_little) },
#else
    { MP_OBJ_NEW_QSTR(MP_QSTR_byteorder), MP_OBJ_NEW_QSTR(MP_QSTR_big) },
#endif

#if MICROPY_MOD_SYS_STDFILES
    { MP_OBJ_NEW_QSTR(MP_QSTR_stdin), (mp_obj_t)&mp_sys_stdin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stdout), (mp_obj_t)&mp_sys_stdout_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stderr), (mp_obj_t)&mp_sys_stderr_obj },
#endif
};

STATIC const mp_obj_dict_t mp_module_sys_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = ARRAY_SIZE(mp_module_sys_globals_table),
        .alloc = ARRAY_SIZE(mp_module_sys_globals_table),
        .table = (mp_map_elem_t*)mp_module_sys_globals_table,
    },
};

const mp_obj_module_t mp_module_sys = {
    .base = { &mp_type_module },
    .name = MP_QSTR_sys,
    .globals = (mp_obj_dict_t*)&mp_module_sys_globals,
};

#endif
