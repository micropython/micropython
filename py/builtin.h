/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_BUILTIN_H
#define MICROPY_INCLUDED_PY_BUILTIN_H

#include "py/obj.h"

typedef enum {
    MP_IMPORT_STAT_NO_EXIST,
    MP_IMPORT_STAT_DIR,
    MP_IMPORT_STAT_FILE,
} mp_import_stat_t;

#if MICROPY_VFS

// Delegate to the VFS for import stat and builtin open.

#define mp_builtin_open_obj mp_vfs_open_obj

mp_import_stat_t mp_vfs_import_stat(const char *path);
mp_obj_t mp_vfs_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);

MP_DECLARE_CONST_FUN_OBJ_KW(mp_vfs_open_obj);

static inline mp_import_stat_t mp_import_stat(const char *path) {
    return mp_vfs_import_stat(path);
}

static inline mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_vfs_open(n_args, args, kwargs);
}

#else

// A port can provide implementations of these functions.
mp_import_stat_t mp_import_stat(const char *path);
mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);

// A port can provide this object.
MP_DECLARE_CONST_FUN_OBJ_KW(mp_builtin_open_obj);

#endif

// A port can provide its own import handler by defining mp_builtin___import__.
#ifndef mp_builtin___import__
#define mp_builtin___import__ mp_builtin___import___default
#endif
mp_obj_t mp_builtin___import__(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_builtin___import___default(size_t n_args, const mp_obj_t *args);

mp_obj_t mp_micropython_mem_info(size_t n_args, const mp_obj_t *args);

MP_DECLARE_CONST_FUN_OBJ_VAR(mp_builtin___build_class___obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin___import___obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin___repl_print___obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_abs_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_all_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_any_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_bin_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_callable_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_compile_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_chr_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_builtin_delattr_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_dir_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_builtin_divmod_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_eval_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_exec_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_execfile_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_getattr_obj);
MP_DECLARE_CONST_FUN_OBJ_3(mp_builtin_setattr_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_builtin_globals_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_builtin_hasattr_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_hash_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_help_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_hex_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_id_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_builtin_isinstance_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_builtin_issubclass_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_iter_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_len_obj);
MP_DECLARE_CONST_FUN_OBJ_0(mp_builtin_locals_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(mp_builtin_max_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(mp_builtin_min_obj);
#if MICROPY_PY_BUILTINS_NEXT2
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_next_obj);
#else
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_next_obj);
#endif
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_oct_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_ord_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_pow_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(mp_builtin_print_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_builtin_repr_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_round_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(mp_builtin_sorted_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_sum_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_input_obj);

MP_DECLARE_CONST_FUN_OBJ_2(mp_namedtuple_obj);

MP_DECLARE_CONST_FUN_OBJ_2(mp_op_contains_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_op_getitem_obj);
MP_DECLARE_CONST_FUN_OBJ_3(mp_op_setitem_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_op_delitem_obj);

// Modules needed by the runtime.
extern const mp_obj_dict_t mp_module_builtins_globals;
extern const mp_obj_module_t mp_module___main__;
extern const mp_obj_module_t mp_module_builtins;
extern const mp_obj_module_t mp_module_sys;

// Modules needed by the parser when MICROPY_COMP_MODULE_CONST is enabled.
extern const mp_obj_module_t mp_module_errno;
extern const mp_obj_module_t mp_module_uctypes;
extern const mp_obj_module_t mp_module_machine;

extern const char MICROPY_PY_BUILTINS_HELP_TEXT[];

#endif // MICROPY_INCLUDED_PY_BUILTIN_H
