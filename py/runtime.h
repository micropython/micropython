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
#ifndef MICROPY_INCLUDED_PY_RUNTIME_H
#define MICROPY_INCLUDED_PY_RUNTIME_H

#include "py/mpstate.h"

typedef enum {
    MP_VM_RETURN_NORMAL,
    MP_VM_RETURN_YIELD,
    MP_VM_RETURN_EXCEPTION,
} mp_vm_return_kind_t;

typedef enum {
    MP_ARG_BOOL      = 0x001,
    MP_ARG_INT       = 0x002,
    MP_ARG_OBJ       = 0x003,
    MP_ARG_KIND_MASK = 0x0ff,
    MP_ARG_REQUIRED  = 0x100,
    MP_ARG_KW_ONLY   = 0x200,
} mp_arg_flag_t;

typedef union _mp_arg_val_t {
    bool u_bool;
    mp_int_t u_int;
    mp_obj_t u_obj;
    mp_rom_obj_t u_rom_obj;
} mp_arg_val_t;

typedef struct _mp_arg_t {
    uint16_t qst;
    uint16_t flags;
    mp_arg_val_t defval;
} mp_arg_t;

// Tables mapping operator enums to qstrs, defined in objtype.c
extern const byte mp_unary_op_method_name[];
extern const byte mp_binary_op_method_name[];

void mp_init(void);
void mp_deinit(void);

void mp_handle_pending(void);
void mp_handle_pending_tail(mp_uint_t atomic_state);

#if MICROPY_ENABLE_SCHEDULER
void mp_sched_lock(void);
void mp_sched_unlock(void);
static inline unsigned int mp_sched_num_pending(void) { return MP_STATE_VM(sched_sp); }
bool mp_sched_schedule(mp_obj_t function, mp_obj_t arg);
#endif

// extra printing method specifically for mp_obj_t's which are integral type
int mp_print_mp_int(const mp_print_t *print, mp_obj_t x, int base, int base_char, int flags, char fill, int width, int prec);

void mp_arg_check_num(size_t n_args, size_t n_kw, size_t n_args_min, size_t n_args_max, bool takes_kw);
void mp_arg_parse_all(size_t n_pos, const mp_obj_t *pos, mp_map_t *kws, size_t n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals);
void mp_arg_parse_all_kw_array(size_t n_pos, size_t n_kw, const mp_obj_t *args, size_t n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals);
NORETURN void mp_arg_error_terse_mismatch(void);
NORETURN void mp_arg_error_unimpl_kw(void);

static inline mp_obj_dict_t *mp_locals_get(void) { return MP_STATE_THREAD(dict_locals); }
static inline void mp_locals_set(mp_obj_dict_t *d) { MP_STATE_THREAD(dict_locals) = d; }
static inline mp_obj_dict_t *mp_globals_get(void) { return MP_STATE_THREAD(dict_globals); }
static inline void mp_globals_set(mp_obj_dict_t *d) { MP_STATE_THREAD(dict_globals) = d; }

mp_obj_t mp_load_name(qstr qst);
mp_obj_t mp_load_global(qstr qst);
mp_obj_t mp_load_build_class(void);
void mp_store_name(qstr qst, mp_obj_t obj);
void mp_store_global(qstr qst, mp_obj_t obj);
void mp_delete_name(qstr qst);
void mp_delete_global(qstr qst);

mp_obj_t mp_unary_op(mp_unary_op_t op, mp_obj_t arg);
mp_obj_t mp_binary_op(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs);

mp_obj_t mp_call_function_0(mp_obj_t fun);
mp_obj_t mp_call_function_1(mp_obj_t fun, mp_obj_t arg);
mp_obj_t mp_call_function_2(mp_obj_t fun, mp_obj_t arg1, mp_obj_t arg2);
mp_obj_t mp_call_function_n_kw(mp_obj_t fun, size_t n_args, size_t n_kw, const mp_obj_t *args);
mp_obj_t mp_call_method_n_kw(size_t n_args, size_t n_kw, const mp_obj_t *args);
mp_obj_t mp_call_method_n_kw_var(bool have_self, size_t n_args_n_kw, const mp_obj_t *args);
mp_obj_t mp_call_method_self_n_kw(mp_obj_t meth, mp_obj_t self, size_t n_args, size_t n_kw, const mp_obj_t *args);
// Call function and catch/dump exception - for Python callbacks from C code
// (return MP_OBJ_NULL in case of exception).
mp_obj_t mp_call_function_1_protected(mp_obj_t fun, mp_obj_t arg);
mp_obj_t mp_call_function_2_protected(mp_obj_t fun, mp_obj_t arg1, mp_obj_t arg2);

typedef struct _mp_call_args_t {
    mp_obj_t fun;
    size_t n_args, n_kw, n_alloc;
    mp_obj_t *args;
} mp_call_args_t;

#if MICROPY_STACKLESS
// Takes arguments which are the most general mix of Python arg types, and
// prepares argument array suitable for passing to ->call() method of a
// function object (and mp_call_function_n_kw()).
// (Only needed in stackless mode.)
void mp_call_prepare_args_n_kw_var(bool have_self, size_t n_args_n_kw, const mp_obj_t *args, mp_call_args_t *out_args);
#endif

void mp_unpack_sequence(mp_obj_t seq, size_t num, mp_obj_t *items);
void mp_unpack_ex(mp_obj_t seq, size_t num, mp_obj_t *items);
mp_obj_t mp_store_map(mp_obj_t map, mp_obj_t key, mp_obj_t value);
mp_obj_t mp_load_attr(mp_obj_t base, qstr attr);
void mp_convert_member_lookup(mp_obj_t obj, const mp_obj_type_t *type, mp_obj_t member, mp_obj_t *dest);
void mp_load_method(mp_obj_t base, qstr attr, mp_obj_t *dest);
void mp_load_method_maybe(mp_obj_t base, qstr attr, mp_obj_t *dest);
void mp_load_super_method(qstr attr, mp_obj_t *dest);
void mp_store_attr(mp_obj_t base, qstr attr, mp_obj_t val);

mp_obj_t mp_getiter(mp_obj_t o, mp_obj_iter_buf_t *iter_buf);
mp_obj_t mp_iternext_allow_raise(mp_obj_t o); // may return MP_OBJ_STOP_ITERATION instead of raising StopIteration()
mp_obj_t mp_iternext(mp_obj_t o); // will always return MP_OBJ_STOP_ITERATION instead of raising StopIteration(...)
mp_vm_return_kind_t mp_resume(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t *ret_val);

mp_obj_t mp_make_raise_obj(mp_obj_t o);

mp_obj_t mp_import_name(qstr name, mp_obj_t fromlist, mp_obj_t level);
mp_obj_t mp_import_from(mp_obj_t module, qstr name);
void mp_import_all(mp_obj_t module);

NORETURN void mp_raise_msg(const mp_obj_type_t *exc_type, const char *msg);
//NORETURN void nlr_raise_msg_varg(const mp_obj_type_t *exc_type, const char *fmt, ...);
NORETURN void mp_raise_ValueError(const char *msg);
NORETURN void mp_raise_TypeError(const char *msg);
NORETURN void mp_raise_NotImplementedError(const char *msg);
NORETURN void mp_raise_OSError(int errno_);
NORETURN void mp_raise_recursion_depth(void);

#if MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG
#undef mp_check_self
#define mp_check_self(pred)
#else
// A port may define to raise TypeError for example
#ifndef mp_check_self
#define mp_check_self(pred) assert(pred)
#endif
#endif

// helper functions for native/viper code
mp_uint_t mp_convert_obj_to_native(mp_obj_t obj, mp_uint_t type);
mp_obj_t mp_convert_native_to_obj(mp_uint_t val, mp_uint_t type);
mp_obj_t mp_native_call_function_n_kw(mp_obj_t fun_in, size_t n_args_kw, const mp_obj_t *args);
void mp_native_raise(mp_obj_t o);

#define mp_sys_path (MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_sys_path_obj)))
#define mp_sys_argv (MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_sys_argv_obj)))

#if MICROPY_WARNINGS
void mp_warning(const char *msg, ...);
#else
#define mp_warning(...)
#endif

#endif // MICROPY_INCLUDED_PY_RUNTIME_H
