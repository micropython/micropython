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
#ifndef __MICROPY_INCLUDED_PY_BUILTIN_H__
#define __MICROPY_INCLUDED_PY_BUILTIN_H__

#include "py/obj.h"

mp_obj_t mp_builtin___import__(size_t n_args, const mp_obj_t *args);
mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);
mp_obj_t mp_micropython_mem_info(size_t n_args, const mp_obj_t *args);

MP_DECLARE_CONST_FUN_OBJ(mp_builtin___build_class___obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin___import___obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin___repl_print___obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_abs_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_all_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_any_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_bin_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_callable_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_compile_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_chr_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_dir_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_divmod_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_eval_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_exec_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_execfile_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_getattr_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_setattr_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_globals_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_hasattr_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_hash_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_hex_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_id_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_isinstance_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_issubclass_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_iter_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_len_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_list_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_locals_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_max_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_min_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_next_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_oct_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_ord_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_pow_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_print_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_repr_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_round_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_sorted_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_sum_obj);
// Defined by a port, but declared here for simplicity
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_help_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_input_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_builtin_open_obj);

MP_DECLARE_CONST_FUN_OBJ(mp_namedtuple_obj);

MP_DECLARE_CONST_FUN_OBJ(mp_op_contains_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_op_getitem_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_op_setitem_obj);
MP_DECLARE_CONST_FUN_OBJ(mp_op_delitem_obj);

extern const mp_obj_module_t mp_module___main__;
extern const mp_obj_module_t mp_module_builtins;
extern const mp_obj_module_t mp_module_array;
extern const mp_obj_module_t mp_module_collections;
extern const mp_obj_module_t mp_module_io;
extern const mp_obj_module_t mp_module_math;
extern const mp_obj_module_t mp_module_cmath;
extern const mp_obj_module_t mp_module_micropython;
extern const mp_obj_module_t mp_module_ustruct;
extern const mp_obj_module_t mp_module_sys;
extern const mp_obj_module_t mp_module_gc;
extern const mp_obj_module_t mp_module_thread;

extern const mp_obj_dict_t mp_module_builtins_globals;

// extmod modules
extern const mp_obj_module_t mp_module_uerrno;
extern const mp_obj_module_t mp_module_uctypes;
extern const mp_obj_module_t mp_module_uzlib;
extern const mp_obj_module_t mp_module_ujson;
extern const mp_obj_module_t mp_module_ure;
extern const mp_obj_module_t mp_module_uheapq;
extern const mp_obj_module_t mp_module_uhashlib;
extern const mp_obj_module_t mp_module_ubinascii;
extern const mp_obj_module_t mp_module_urandom;
extern const mp_obj_module_t mp_module_ussl;
extern const mp_obj_module_t mp_module_machine;
extern const mp_obj_module_t mp_module_lwip;
extern const mp_obj_module_t mp_module_websocket;
extern const mp_obj_module_t mp_module_webrepl;
extern const mp_obj_module_t mp_module_framebuf;
extern const mp_obj_module_t mp_module_btree;

// extmod functions
MP_DECLARE_CONST_FUN_OBJ(pyb_mount_obj);

#endif // __MICROPY_INCLUDED_PY_BUILTIN_H__
