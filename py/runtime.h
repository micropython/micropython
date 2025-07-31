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
#include "py/pystack.h"
#include "py/cstack.h"

// For use with mp_call_function_1_from_nlr_jump_callback.
#define MP_DEFINE_NLR_JUMP_CALLBACK_FUNCTION_1(ctx, f, a) \
    nlr_jump_callback_node_call_function_1_t ctx = { \
        .func = (void (*)(void *))(f), \
        .arg = (a), \
    }

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

struct _mp_sched_node_t;

typedef void (*mp_sched_callback_t)(struct _mp_sched_node_t *);

typedef struct _mp_sched_node_t {
    mp_sched_callback_t callback;
    struct _mp_sched_node_t *next;
} mp_sched_node_t;

// For use with mp_globals_locals_set_from_nlr_jump_callback.
typedef struct _nlr_jump_callback_node_globals_locals_t {
    nlr_jump_callback_node_t callback;
    mp_obj_dict_t *globals;
    mp_obj_dict_t *locals;
} nlr_jump_callback_node_globals_locals_t;

// For use with mp_call_function_1_from_nlr_jump_callback.
typedef struct _nlr_jump_callback_node_call_function_1_t {
    nlr_jump_callback_node_t callback;
    void (*func)(void *);
    void *arg;
} nlr_jump_callback_node_call_function_1_t;

// Tables mapping operator enums to qstrs, defined in objtype.c
extern const byte mp_unary_op_method_name[];
extern const byte mp_binary_op_method_name[];

void mp_init(void);
void mp_deinit(void);

void mp_sched_exception(mp_obj_t exc);
void mp_sched_keyboard_interrupt(void);
#if MICROPY_ENABLE_VM_ABORT
void mp_sched_vm_abort(void);
#endif
void mp_handle_pending(bool raise_exc);

#if MICROPY_ENABLE_SCHEDULER
void mp_sched_lock(void);
void mp_sched_unlock(void);
#define mp_sched_num_pending() (MP_STATE_VM(sched_len))
bool mp_sched_schedule(mp_obj_t function, mp_obj_t arg);
bool mp_sched_schedule_node(mp_sched_node_t *node, mp_sched_callback_t callback);
#endif

// Handles any pending MicroPython events without waiting for an interrupt or event.
void mp_event_handle_nowait(void);

// Handles any pending MicroPython events and then suspends execution until the
// next interrupt or event.
//
// Note: on "tickless" ports this can suspend execution for a long time,
// don't call unless you know an interrupt is coming to continue execution.
// On "ticked" ports it may return early due to the tick interrupt.
void mp_event_wait_indefinite(void);

// Handle any pending MicroPython events and then suspends execution until the
// next interrupt or event, or until timeout_ms milliseconds have elapsed.
//
// On "ticked" ports it may return early due to the tick interrupt.
void mp_event_wait_ms(mp_uint_t timeout_ms);

// extra printing method specifically for mp_obj_t's which are integral type
int mp_print_mp_int(const mp_print_t *print, mp_obj_t x, int base, int base_char, int flags, char fill, int width, int prec);

void mp_arg_check_num_sig(size_t n_args, size_t n_kw, uint32_t sig);
static inline void mp_arg_check_num(size_t n_args, size_t n_kw, size_t n_args_min, size_t n_args_max, bool takes_kw) {
    mp_arg_check_num_sig(n_args, n_kw, MP_OBJ_FUN_MAKE_SIG(n_args_min, n_args_max, takes_kw));
}
void mp_arg_parse_all(size_t n_pos, const mp_obj_t *pos, mp_map_t *kws, size_t n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals);
void mp_arg_parse_all_kw_array(size_t n_pos, size_t n_kw, const mp_obj_t *args, size_t n_allowed, const mp_arg_t *allowed, mp_arg_val_t *out_vals);
NORETURN void mp_arg_error_terse_mismatch(void);
NORETURN void mp_arg_error_unimpl_kw(void);

static inline mp_obj_dict_t *mp_locals_get(void) {
    return MP_STATE_THREAD(dict_locals);
}
static inline void mp_locals_set(mp_obj_dict_t *d) {
    MP_STATE_THREAD(dict_locals) = d;
}
static inline mp_obj_dict_t *mp_globals_get(void) {
    return MP_STATE_THREAD(dict_globals);
}
static inline void mp_globals_set(mp_obj_dict_t *d) {
    MP_STATE_THREAD(dict_globals) = d;
}

void mp_globals_locals_set_from_nlr_jump_callback(void *ctx_in);
void mp_call_function_1_from_nlr_jump_callback(void *ctx_in);

#if MICROPY_PY_THREAD
static inline void mp_thread_init_state(mp_state_thread_t *ts, size_t stack_size, mp_obj_dict_t *locals, mp_obj_dict_t *globals) {
    mp_thread_set_state(ts);

    mp_cstack_init_with_top(ts + 1, stack_size); // need to include ts in root-pointer scan

    // GC starts off unlocked
    ts->gc_lock_depth = 0;

    // There are no pending jump callbacks or exceptions yet
    ts->nlr_jump_callback_top = NULL;
    ts->mp_pending_exception = MP_OBJ_NULL;

    // If locals/globals are not given, inherit from main thread
    if (locals == NULL) {
        locals = mp_state_ctx.thread.dict_locals;
    }
    if (globals == NULL) {
        globals = mp_state_ctx.thread.dict_globals;
    }
    mp_locals_set(locals);
    mp_globals_set(globals);
}
#endif

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
void mp_load_method_protected(mp_obj_t obj, qstr attr, mp_obj_t *dest, bool catch_all_exc);
void mp_load_super_method(qstr attr, mp_obj_t *dest);
void mp_store_attr(mp_obj_t base, qstr attr, mp_obj_t val);

mp_obj_t mp_getiter(mp_obj_t o, mp_obj_iter_buf_t *iter_buf);
mp_obj_t mp_iternext_allow_raise(mp_obj_t o); // may return MP_OBJ_STOP_ITERATION instead of raising StopIteration()
mp_obj_t mp_iternext(mp_obj_t o); // will always return MP_OBJ_STOP_ITERATION instead of raising StopIteration(...)
mp_vm_return_kind_t mp_resume(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t *ret_val);

static inline mp_obj_t mp_make_stop_iteration(mp_obj_t o) {
    MP_STATE_THREAD(stop_iteration_arg) = o;
    return MP_OBJ_STOP_ITERATION;
}

mp_obj_t mp_make_raise_obj(mp_obj_t o);

mp_obj_t mp_import_name(qstr name, mp_obj_t fromlist, mp_obj_t level);
mp_obj_t mp_import_from(mp_obj_t module, qstr name);
void mp_import_all(mp_obj_t module);

#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NONE
NORETURN void mp_raise_type(const mp_obj_type_t *exc_type);
NORETURN void mp_raise_ValueError_no_msg(void);
NORETURN void mp_raise_TypeError_no_msg(void);
NORETURN void mp_raise_NotImplementedError_no_msg(void);
#define mp_raise_msg(exc_type, msg) mp_raise_type(exc_type)
#define mp_raise_msg_varg(exc_type, ...) mp_raise_type(exc_type)
#define mp_raise_ValueError(msg) mp_raise_ValueError_no_msg()
#define mp_raise_TypeError(msg) mp_raise_TypeError_no_msg()
#define mp_raise_NotImplementedError(msg) mp_raise_NotImplementedError_no_msg()
#else
#define mp_raise_type(exc_type) mp_raise_msg(exc_type, NULL)
NORETURN void mp_raise_msg(const mp_obj_type_t *exc_type, mp_rom_error_text_t msg);
NORETURN void mp_raise_msg_varg(const mp_obj_type_t *exc_type, mp_rom_error_text_t fmt, ...);
NORETURN void mp_raise_ValueError(mp_rom_error_text_t msg);
NORETURN void mp_raise_TypeError(mp_rom_error_text_t msg);
NORETURN void mp_raise_NotImplementedError(mp_rom_error_text_t msg);
#endif

NORETURN void mp_raise_type_arg(const mp_obj_type_t *exc_type, mp_obj_t arg);
NORETURN void mp_raise_StopIteration(mp_obj_t arg);
NORETURN void mp_raise_TypeError_int_conversion(mp_const_obj_t arg);
NORETURN void mp_raise_OSError(int errno_);
NORETURN void mp_raise_OSError_with_filename(int errno_, const char *filename);
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
int mp_native_type_from_qstr(qstr qst);
mp_uint_t mp_native_from_obj(mp_obj_t obj, mp_uint_t type);
mp_obj_t mp_native_to_obj(mp_uint_t val, mp_uint_t type);

#if MICROPY_PY_SYS_PATH
#define mp_sys_path (MP_STATE_VM(sys_mutable[MP_SYS_MUTABLE_PATH]))
#endif

#if MICROPY_PY_SYS_ARGV
#define mp_sys_argv (MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_sys_argv_obj)))
#endif

#if MICROPY_WARNINGS
#ifndef mp_warning
void mp_warning(const char *category, const char *msg, ...);
#endif
#else
#define mp_warning(...)
#endif

#endif // MICROPY_INCLUDED_PY_RUNTIME_H
