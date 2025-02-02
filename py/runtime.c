/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014-2018 Paul Sokolovsky
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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "py/parsenum.h"
#include "py/compile.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/objtype.h"
#include "py/objmodule.h"
#include "py/objgenerator.h"
#include "py/smallint.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/cstack.h"
#include "py/gc.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#define DEBUG_OP_printf(...) DEBUG_printf(__VA_ARGS__)
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#define DEBUG_OP_printf(...) (void)0
#endif

const mp_obj_module_t mp_module___main__ = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&MP_STATE_VM(dict_main),
};

MP_REGISTER_MODULE(MP_QSTR___main__, mp_module___main__);

#define TYPE_HAS_ITERNEXT(type) (type->flags & (MP_TYPE_FLAG_ITER_IS_ITERNEXT | MP_TYPE_FLAG_ITER_IS_CUSTOM | MP_TYPE_FLAG_ITER_IS_STREAM))

void mp_init(void) {
    qstr_init();

    // no pending exceptions to start with
    MP_STATE_THREAD(mp_pending_exception) = MP_OBJ_NULL;
    #if MICROPY_ENABLE_SCHEDULER
    // no pending callbacks to start with
    MP_STATE_VM(sched_state) = MP_SCHED_IDLE;
    #if MICROPY_SCHEDULER_STATIC_NODES
    if (MP_STATE_VM(sched_head) != NULL) {
        // pending callbacks are on the list, eg from before a soft reset
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
    #endif
    MP_STATE_VM(sched_idx) = 0;
    MP_STATE_VM(sched_len) = 0;
    #endif

    #if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF
    mp_init_emergency_exception_buf();
    #endif

    #if MICROPY_KBD_EXCEPTION
    // initialise the exception object for raising KeyboardInterrupt
    MP_STATE_VM(mp_kbd_exception).base.type = &mp_type_KeyboardInterrupt;
    MP_STATE_VM(mp_kbd_exception).traceback_alloc = 0;
    MP_STATE_VM(mp_kbd_exception).traceback_len = 0;
    MP_STATE_VM(mp_kbd_exception).traceback_data = NULL;
    MP_STATE_VM(mp_kbd_exception).args = (mp_obj_tuple_t *)&mp_const_empty_tuple_obj;
    #endif

    #if MICROPY_ENABLE_COMPILER
    // optimization disabled by default
    MP_STATE_VM(mp_optimise_value) = 0;
    #if MICROPY_EMIT_NATIVE
    MP_STATE_VM(default_emit_opt) = MP_EMIT_OPT_NONE;
    #endif
    #endif

    // init global module dict
    mp_obj_dict_init(&MP_STATE_VM(mp_loaded_modules_dict), MICROPY_LOADED_MODULES_DICT_SIZE);

    // initialise the __main__ module
    mp_obj_dict_init(&MP_STATE_VM(dict_main), 1);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(&MP_STATE_VM(dict_main)), MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR___main__));

    // locals = globals for outer module (see Objects/frameobject.c/PyFrame_New())
    mp_locals_set(&MP_STATE_VM(dict_main));
    mp_globals_set(&MP_STATE_VM(dict_main));

    #if MICROPY_CAN_OVERRIDE_BUILTINS
    // start with no extensions to builtins
    MP_STATE_VM(mp_module_builtins_override_dict) = NULL;
    #endif

    #if MICROPY_PERSISTENT_CODE_TRACK_FUN_DATA || MICROPY_PERSISTENT_CODE_TRACK_BSS_RODATA
    MP_STATE_VM(persistent_code_root_pointers) = MP_OBJ_NULL;
    #endif

    #if MICROPY_PY_OS_DUPTERM
    for (size_t i = 0; i < MICROPY_PY_OS_DUPTERM; ++i) {
        MP_STATE_VM(dupterm_objs[i]) = MP_OBJ_NULL;
    }
    #endif

    #if MICROPY_VFS
    // initialise the VFS sub-system
    MP_STATE_VM(vfs_cur) = NULL;
    MP_STATE_VM(vfs_mount_table) = NULL;
    #endif

    #if MICROPY_PY_SYS_PATH_ARGV_DEFAULTS
    #if MICROPY_PY_SYS_PATH
    mp_sys_path = mp_obj_new_list(0, NULL);
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
    #if MICROPY_MODULE_FROZEN
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__dot_frozen));
    #endif
    #endif
    #if MICROPY_PY_SYS_ARGV
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);
    #endif
    #endif // MICROPY_PY_SYS_PATH_ARGV_DEFAULTS

    #if MICROPY_PY_SYS_ATEXIT
    MP_STATE_VM(sys_exitfunc) = mp_const_none;
    #endif

    #if MICROPY_PY_SYS_PS1_PS2
    MP_STATE_VM(sys_mutable[MP_SYS_MUTABLE_PS1]) = MP_OBJ_NEW_QSTR(MP_QSTR__gt__gt__gt__space_);
    MP_STATE_VM(sys_mutable[MP_SYS_MUTABLE_PS2]) = MP_OBJ_NEW_QSTR(MP_QSTR__dot__dot__dot__space_);
    #endif

    #if MICROPY_PY_SYS_SETTRACE
    MP_STATE_THREAD(prof_trace_callback) = MP_OBJ_NULL;
    MP_STATE_THREAD(prof_callback_is_executing) = false;
    MP_STATE_THREAD(current_code_state) = NULL;
    #endif

    #if MICROPY_PY_SYS_TRACEBACKLIMIT
    MP_STATE_VM(sys_mutable[MP_SYS_MUTABLE_TRACEBACKLIMIT]) = MP_OBJ_NEW_SMALL_INT(1000);
    #endif

    #if MICROPY_PY_BLUETOOTH
    MP_STATE_VM(bluetooth) = MP_OBJ_NULL;
    #endif

    #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
    MP_STATE_VM(usbd) = MP_OBJ_NULL;
    #endif

    #if MICROPY_PY_THREAD_GIL
    mp_thread_mutex_init(&MP_STATE_VM(gil_mutex));
    #endif

    // call port specific initialization if any
    #ifdef MICROPY_PORT_INIT_FUNC
    MICROPY_PORT_INIT_FUNC;
    #endif

    MP_THREAD_GIL_ENTER();
}

void mp_deinit(void) {
    MP_THREAD_GIL_EXIT();

    // call port specific deinitialization if any
    #ifdef MICROPY_PORT_DEINIT_FUNC
    MICROPY_PORT_DEINIT_FUNC;
    #endif
}

void mp_globals_locals_set_from_nlr_jump_callback(void *ctx_in) {
    nlr_jump_callback_node_globals_locals_t *ctx = ctx_in;
    mp_globals_set(ctx->globals);
    mp_locals_set(ctx->locals);
}

void mp_call_function_1_from_nlr_jump_callback(void *ctx_in) {
    nlr_jump_callback_node_call_function_1_t *ctx = ctx_in;
    ctx->func(ctx->arg);
}

mp_obj_t MICROPY_WRAP_MP_LOAD_NAME(mp_load_name)(qstr qst) {
    // logic: search locals, globals, builtins
    DEBUG_OP_printf("load name %s\n", qstr_str(qst));
    // If we're at the outer scope (locals == globals), dispatch to load_global right away
    if (mp_locals_get() != mp_globals_get()) {
        mp_map_elem_t *elem = mp_map_lookup(&mp_locals_get()->map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
        if (elem != NULL) {
            return elem->value;
        }
    }
    return mp_load_global(qst);
}

mp_obj_t MICROPY_WRAP_MP_LOAD_GLOBAL(mp_load_global)(qstr qst) {
    // logic: search globals, builtins
    DEBUG_OP_printf("load global %s\n", qstr_str(qst));
    mp_map_elem_t *elem = mp_map_lookup(&mp_globals_get()->map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
    if (elem == NULL) {
        #if MICROPY_CAN_OVERRIDE_BUILTINS
        if (MP_STATE_VM(mp_module_builtins_override_dict) != NULL) {
            // lookup in additional dynamic table of builtins first
            elem = mp_map_lookup(&MP_STATE_VM(mp_module_builtins_override_dict)->map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
            if (elem != NULL) {
                return elem->value;
            }
        }
        #endif
        elem = mp_map_lookup((mp_map_t *)&mp_module_builtins_globals.map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
        if (elem == NULL) {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_raise_msg(&mp_type_NameError, MP_ERROR_TEXT("name not defined"));
            #else
            mp_raise_msg_varg(&mp_type_NameError, MP_ERROR_TEXT("name '%q' isn't defined"), qst);
            #endif
        }
    }
    return elem->value;
}

mp_obj_t mp_load_build_class(void) {
    DEBUG_OP_printf("load_build_class\n");
    #if MICROPY_CAN_OVERRIDE_BUILTINS
    if (MP_STATE_VM(mp_module_builtins_override_dict) != NULL) {
        // lookup in additional dynamic table of builtins first
        mp_map_elem_t *elem = mp_map_lookup(&MP_STATE_VM(mp_module_builtins_override_dict)->map, MP_OBJ_NEW_QSTR(MP_QSTR___build_class__), MP_MAP_LOOKUP);
        if (elem != NULL) {
            return elem->value;
        }
    }
    #endif
    return MP_OBJ_FROM_PTR(&mp_builtin___build_class___obj);
}

void mp_store_name(qstr qst, mp_obj_t obj) {
    DEBUG_OP_printf("store name %s <- %p\n", qstr_str(qst), obj);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(mp_locals_get()), MP_OBJ_NEW_QSTR(qst), obj);
}

void mp_delete_name(qstr qst) {
    DEBUG_OP_printf("delete name %s\n", qstr_str(qst));
    // TODO convert KeyError to NameError if qst not found
    mp_obj_dict_delete(MP_OBJ_FROM_PTR(mp_locals_get()), MP_OBJ_NEW_QSTR(qst));
}

void mp_store_global(qstr qst, mp_obj_t obj) {
    DEBUG_OP_printf("store global %s <- %p\n", qstr_str(qst), obj);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(mp_globals_get()), MP_OBJ_NEW_QSTR(qst), obj);
}

void mp_delete_global(qstr qst) {
    DEBUG_OP_printf("delete global %s\n", qstr_str(qst));
    // TODO convert KeyError to NameError if qst not found
    mp_obj_dict_delete(MP_OBJ_FROM_PTR(mp_globals_get()), MP_OBJ_NEW_QSTR(qst));
}

mp_obj_t mp_unary_op(mp_unary_op_t op, mp_obj_t arg) {
    DEBUG_OP_printf("unary " UINT_FMT " %q %p\n", op, mp_unary_op_method_name[op], arg);

    if (op == MP_UNARY_OP_NOT) {
        // "not x" is the negative of whether "x" is true per Python semantics
        return mp_obj_new_bool(mp_obj_is_true(arg) == 0);
    } else if (mp_obj_is_small_int(arg)) {
        mp_int_t val = MP_OBJ_SMALL_INT_VALUE(arg);
        switch (op) {
            case MP_UNARY_OP_BOOL:
                return mp_obj_new_bool(val != 0);
            case MP_UNARY_OP_HASH:
                return arg;
            case MP_UNARY_OP_POSITIVE:
            case MP_UNARY_OP_INT_MAYBE:
                return arg;
            case MP_UNARY_OP_NEGATIVE:
                // check for overflow
                if (val == MP_SMALL_INT_MIN) {
                    return mp_obj_new_int(-val);
                } else {
                    return MP_OBJ_NEW_SMALL_INT(-val);
                }
            case MP_UNARY_OP_ABS:
                if (val >= 0) {
                    return arg;
                } else if (val == MP_SMALL_INT_MIN) {
                    // check for overflow
                    return mp_obj_new_int(-val);
                } else {
                    return MP_OBJ_NEW_SMALL_INT(-val);
                }
            default:
                assert(op == MP_UNARY_OP_INVERT);
                return MP_OBJ_NEW_SMALL_INT(~val);
        }
    } else if (op == MP_UNARY_OP_HASH && mp_obj_is_str_or_bytes(arg)) {
        // fast path for hashing str/bytes
        GET_STR_HASH(arg, h);
        if (h == 0) {
            GET_STR_DATA_LEN(arg, data, len);
            h = qstr_compute_hash(data, len);
        }
        return MP_OBJ_NEW_SMALL_INT(h);
    } else {
        const mp_obj_type_t *type = mp_obj_get_type(arg);
        if (MP_OBJ_TYPE_HAS_SLOT(type, unary_op)) {
            mp_obj_t result = MP_OBJ_TYPE_GET_SLOT(type, unary_op)(op, arg);
            if (result != MP_OBJ_NULL) {
                return result;
            }
        } else if (op == MP_UNARY_OP_HASH) {
            // Type doesn't have unary_op so use hash of object instance.
            return MP_OBJ_NEW_SMALL_INT((mp_uint_t)arg);
        }
        if (op == MP_UNARY_OP_BOOL) {
            // Type doesn't have unary_op (or didn't handle MP_UNARY_OP_BOOL),
            // so is implicitly True as this code path is impossible to reach
            // if arg==mp_const_none.
            return mp_const_true;
        }
        if (op == MP_UNARY_OP_INT_MAYBE
            #if MICROPY_PY_BUILTINS_FLOAT
            || op == MP_UNARY_OP_FLOAT_MAYBE
            #if MICROPY_PY_BUILTINS_COMPLEX
            || op == MP_UNARY_OP_COMPLEX_MAYBE
            #endif
            #endif
            ) {
            // These operators may return MP_OBJ_NULL if they are not supported by the type.
            return MP_OBJ_NULL;
        }
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_TypeError(MP_ERROR_TEXT("unsupported type for operator"));
        #else
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("unsupported type for %q: '%s'"),
            mp_unary_op_method_name[op], mp_obj_get_type_str(arg));
        #endif
    }
}

mp_obj_t MICROPY_WRAP_MP_BINARY_OP(mp_binary_op)(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs) {
    DEBUG_OP_printf("binary " UINT_FMT " %q %p %p\n", op, mp_binary_op_method_name[op], lhs, rhs);

    // TODO correctly distinguish inplace operators for mutable objects
    // lookup logic that CPython uses for +=:
    //   check for implemented +=
    //   then check for implemented +
    //   then check for implemented seq.inplace_concat
    //   then check for implemented seq.concat
    //   then fail
    // note that list does not implement + or +=, so that inplace_concat is reached first for +=

    // deal with is
    if (op == MP_BINARY_OP_IS) {
        return mp_obj_new_bool(lhs == rhs);
    }

    // deal with == and != for all types
    if (op == MP_BINARY_OP_EQUAL || op == MP_BINARY_OP_NOT_EQUAL) {
        // mp_obj_equal_not_equal supports a bunch of shortcuts
        return mp_obj_equal_not_equal(op, lhs, rhs);
    }

    // deal with exception_match for all types
    if (op == MP_BINARY_OP_EXCEPTION_MATCH) {
        // rhs must be issubclass(rhs, BaseException)
        if (mp_obj_is_exception_type(rhs)) {
            if (mp_obj_exception_match(lhs, rhs)) {
                return mp_const_true;
            } else {
                return mp_const_false;
            }
        } else if (mp_obj_is_type(rhs, &mp_type_tuple)) {
            mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(rhs);
            for (size_t i = 0; i < tuple->len; i++) {
                rhs = tuple->items[i];
                if (!mp_obj_is_exception_type(rhs)) {
                    goto unsupported_op;
                }
                if (mp_obj_exception_match(lhs, rhs)) {
                    return mp_const_true;
                }
            }
            return mp_const_false;
        }
        goto unsupported_op;
    }

    if (mp_obj_is_small_int(lhs)) {
        mp_int_t lhs_val = MP_OBJ_SMALL_INT_VALUE(lhs);
        if (mp_obj_is_small_int(rhs)) {
            mp_int_t rhs_val = MP_OBJ_SMALL_INT_VALUE(rhs);
            // This is a binary operation: lhs_val op rhs_val
            // We need to be careful to handle overflow; see CERT INT32-C
            // Operations that can overflow:
            //      +       result always fits in mp_int_t, then handled by SMALL_INT check
            //      -       result always fits in mp_int_t, then handled by SMALL_INT check
            //      *       checked explicitly
            //      /       if lhs=MIN and rhs=-1; result always fits in mp_int_t, then handled by SMALL_INT check
            //      %       if lhs=MIN and rhs=-1; result always fits in mp_int_t, then handled by SMALL_INT check
            //      <<      checked explicitly
            switch (op) {
                case MP_BINARY_OP_OR:
                case MP_BINARY_OP_INPLACE_OR:
                    lhs_val |= rhs_val;
                    break;
                case MP_BINARY_OP_XOR:
                case MP_BINARY_OP_INPLACE_XOR:
                    lhs_val ^= rhs_val;
                    break;
                case MP_BINARY_OP_AND:
                case MP_BINARY_OP_INPLACE_AND:
                    lhs_val &= rhs_val;
                    break;
                case MP_BINARY_OP_LSHIFT:
                case MP_BINARY_OP_INPLACE_LSHIFT: {
                    if (rhs_val < 0) {
                        // negative shift not allowed
                        mp_raise_ValueError(MP_ERROR_TEXT("negative shift count"));
                    } else if (rhs_val >= (mp_int_t)(sizeof(lhs_val) * MP_BITS_PER_BYTE)
                               || lhs_val > (MP_SMALL_INT_MAX >> rhs_val)
                               || lhs_val < (MP_SMALL_INT_MIN >> rhs_val)) {
                        // left-shift will overflow, so use higher precision integer
                        lhs = mp_obj_new_int_from_ll(lhs_val);
                        goto generic_binary_op;
                    } else {
                        // use standard precision
                        lhs_val = (mp_uint_t)lhs_val << rhs_val;
                    }
                    break;
                }
                case MP_BINARY_OP_RSHIFT:
                case MP_BINARY_OP_INPLACE_RSHIFT:
                    if (rhs_val < 0) {
                        // negative shift not allowed
                        mp_raise_ValueError(MP_ERROR_TEXT("negative shift count"));
                    } else {
                        // standard precision is enough for right-shift
                        if (rhs_val >= (mp_int_t)(sizeof(lhs_val) * MP_BITS_PER_BYTE)) {
                            // Shifting to big amounts is undefined behavior
                            // in C and is CPU-dependent; propagate sign bit.
                            rhs_val = sizeof(lhs_val) * MP_BITS_PER_BYTE - 1;
                        }
                        lhs_val >>= rhs_val;
                    }
                    break;
                case MP_BINARY_OP_ADD:
                case MP_BINARY_OP_INPLACE_ADD:
                    lhs_val += rhs_val;
                    break;
                case MP_BINARY_OP_SUBTRACT:
                case MP_BINARY_OP_INPLACE_SUBTRACT:
                    lhs_val -= rhs_val;
                    break;
                case MP_BINARY_OP_MULTIPLY:
                case MP_BINARY_OP_INPLACE_MULTIPLY: {

                    // If long long type exists and is larger than mp_int_t, then
                    // we can use the following code to perform overflow-checked multiplication.
                    // Otherwise (eg in x64 case) we must use mp_small_int_mul_overflow.
                    #if 0
                    // compute result using long long precision
                    long long res = (long long)lhs_val * (long long)rhs_val;
                    if (res > MP_SMALL_INT_MAX || res < MP_SMALL_INT_MIN) {
                        // result overflowed SMALL_INT, so return higher precision integer
                        return mp_obj_new_int_from_ll(res);
                    } else {
                        // use standard precision
                        lhs_val = (mp_int_t)res;
                    }
                    #endif

                    if (mp_small_int_mul_overflow(lhs_val, rhs_val)) {
                        // use higher precision
                        lhs = mp_obj_new_int_from_ll(lhs_val);
                        goto generic_binary_op;
                    } else {
                        // use standard precision
                        return MP_OBJ_NEW_SMALL_INT(lhs_val * rhs_val);
                    }
                }
                case MP_BINARY_OP_FLOOR_DIVIDE:
                case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE:
                    if (rhs_val == 0) {
                        goto zero_division;
                    }
                    lhs_val = mp_small_int_floor_divide(lhs_val, rhs_val);
                    break;

                #if MICROPY_PY_BUILTINS_FLOAT
                case MP_BINARY_OP_TRUE_DIVIDE:
                case MP_BINARY_OP_INPLACE_TRUE_DIVIDE:
                    if (rhs_val == 0) {
                        goto zero_division;
                    }
                    return mp_obj_new_float((mp_float_t)lhs_val / (mp_float_t)rhs_val);
                #endif

                case MP_BINARY_OP_MODULO:
                case MP_BINARY_OP_INPLACE_MODULO: {
                    if (rhs_val == 0) {
                        goto zero_division;
                    }
                    lhs_val = mp_small_int_modulo(lhs_val, rhs_val);
                    break;
                }

                case MP_BINARY_OP_POWER:
                case MP_BINARY_OP_INPLACE_POWER:
                    if (rhs_val < 0) {
                        #if MICROPY_PY_BUILTINS_FLOAT
                        return mp_obj_float_binary_op(op, (mp_float_t)lhs_val, rhs);
                        #else
                        mp_raise_ValueError(MP_ERROR_TEXT("negative power with no float support"));
                        #endif
                    } else {
                        mp_int_t ans = 1;
                        while (rhs_val > 0) {
                            if (rhs_val & 1) {
                                if (mp_small_int_mul_overflow(ans, lhs_val)) {
                                    goto power_overflow;
                                }
                                ans *= lhs_val;
                            }
                            if (rhs_val == 1) {
                                break;
                            }
                            rhs_val /= 2;
                            if (mp_small_int_mul_overflow(lhs_val, lhs_val)) {
                                goto power_overflow;
                            }
                            lhs_val *= lhs_val;
                        }
                        lhs_val = ans;
                    }
                    break;

                power_overflow:
                    // use higher precision
                    lhs = mp_obj_new_int_from_ll(MP_OBJ_SMALL_INT_VALUE(lhs));
                    goto generic_binary_op;

                case MP_BINARY_OP_DIVMOD: {
                    if (rhs_val == 0) {
                        goto zero_division;
                    }
                    // to reduce stack usage we don't pass a temp array of the 2 items
                    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
                    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(mp_small_int_floor_divide(lhs_val, rhs_val));
                    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(mp_small_int_modulo(lhs_val, rhs_val));
                    return MP_OBJ_FROM_PTR(tuple);
                }

                case MP_BINARY_OP_LESS:
                    return mp_obj_new_bool(lhs_val < rhs_val);
                case MP_BINARY_OP_MORE:
                    return mp_obj_new_bool(lhs_val > rhs_val);
                case MP_BINARY_OP_LESS_EQUAL:
                    return mp_obj_new_bool(lhs_val <= rhs_val);
                case MP_BINARY_OP_MORE_EQUAL:
                    return mp_obj_new_bool(lhs_val >= rhs_val);

                default:
                    goto unsupported_op;
            }
            // This is an inlined version of mp_obj_new_int, for speed
            if (MP_SMALL_INT_FITS(lhs_val)) {
                return MP_OBJ_NEW_SMALL_INT(lhs_val);
            } else {
                return mp_obj_new_int_from_ll(lhs_val);
            }
        #if MICROPY_PY_BUILTINS_FLOAT
        } else if (mp_obj_is_float(rhs)) {
            mp_obj_t res = mp_obj_float_binary_op(op, (mp_float_t)lhs_val, rhs);
            if (res == MP_OBJ_NULL) {
                goto unsupported_op;
            } else {
                return res;
            }
        #endif
        #if MICROPY_PY_BUILTINS_COMPLEX
        } else if (mp_obj_is_type(rhs, &mp_type_complex)) {
            mp_obj_t res = mp_obj_complex_binary_op(op, (mp_float_t)lhs_val, 0, rhs);
            if (res == MP_OBJ_NULL) {
                goto unsupported_op;
            } else {
                return res;
            }
        #endif
        }
    }

    // Convert MP_BINARY_OP_IN to MP_BINARY_OP_CONTAINS with swapped args.
    if (op == MP_BINARY_OP_IN) {
        op = MP_BINARY_OP_CONTAINS;
        mp_obj_t temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    // generic binary_op supplied by type
    const mp_obj_type_t *type;
generic_binary_op:
    type = mp_obj_get_type(lhs);
    if (MP_OBJ_TYPE_HAS_SLOT(type, binary_op)) {
        mp_obj_t result = MP_OBJ_TYPE_GET_SLOT(type, binary_op)(op, lhs, rhs);
        if (result != MP_OBJ_NULL) {
            return result;
        }
    }

    // If this was an inplace method, fallback to the corresponding normal method.
    // https://docs.python.org/3/reference/datamodel.html#object.__iadd__ :
    // "If a specific method is not defined, the augmented assignment falls back
    // to the normal methods."
    if (op >= MP_BINARY_OP_INPLACE_OR && op <= MP_BINARY_OP_INPLACE_POWER) {
        op += MP_BINARY_OP_OR - MP_BINARY_OP_INPLACE_OR;
        goto generic_binary_op;
    }

    #if MICROPY_PY_REVERSE_SPECIAL_METHODS
    if (op >= MP_BINARY_OP_OR && op <= MP_BINARY_OP_POWER) {
        mp_obj_t t = rhs;
        rhs = lhs;
        lhs = t;
        op += MP_BINARY_OP_REVERSE_OR - MP_BINARY_OP_OR;
        goto generic_binary_op;
    } else if (op >= MP_BINARY_OP_REVERSE_OR) {
        // Convert __rop__ back to __op__ for error message
        mp_obj_t t = rhs;
        rhs = lhs;
        lhs = t;
        op -= MP_BINARY_OP_REVERSE_OR - MP_BINARY_OP_OR;
    }
    #endif

    if (op == MP_BINARY_OP_CONTAINS) {
        // If type didn't support containment then explicitly walk the iterator.
        // mp_getiter will raise the appropriate exception if lhs is not iterable.
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iter = mp_getiter(lhs, &iter_buf);
        mp_obj_t next;
        while ((next = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
            if (mp_obj_equal(next, rhs)) {
                return mp_const_true;
            }
        }
        return mp_const_false;
    }

unsupported_op:
    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    mp_raise_TypeError(MP_ERROR_TEXT("unsupported type for operator"));
    #else
    mp_raise_msg_varg(&mp_type_TypeError,
        MP_ERROR_TEXT("unsupported types for %q: '%s', '%s'"),
        mp_binary_op_method_name[op], mp_obj_get_type_str(lhs), mp_obj_get_type_str(rhs));
    #endif

zero_division:
    mp_raise_msg(&mp_type_ZeroDivisionError, MP_ERROR_TEXT("divide by zero"));
}

mp_obj_t mp_call_function_0(mp_obj_t fun) {
    return mp_call_function_n_kw(fun, 0, 0, NULL);
}

mp_obj_t mp_call_function_1(mp_obj_t fun, mp_obj_t arg) {
    return mp_call_function_n_kw(fun, 1, 0, &arg);
}

mp_obj_t mp_call_function_2(mp_obj_t fun, mp_obj_t arg1, mp_obj_t arg2) {
    mp_obj_t args[2];
    args[0] = arg1;
    args[1] = arg2;
    return mp_call_function_n_kw(fun, 2, 0, args);
}

// args contains, eg: arg0  arg1  key0  value0  key1  value1
mp_obj_t mp_call_function_n_kw(mp_obj_t fun_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // TODO improve this: fun object can specify its type and we parse here the arguments,
    // passing to the function arrays of fixed and keyword arguments

    DEBUG_OP_printf("calling function %p(n_args=" UINT_FMT ", n_kw=" UINT_FMT ", args=%p)\n", fun_in, n_args, n_kw, args);

    // get the type
    const mp_obj_type_t *type = mp_obj_get_type(fun_in);

    // do the call
    if (MP_OBJ_TYPE_HAS_SLOT(type, call)) {
        return MP_OBJ_TYPE_GET_SLOT(type, call)(fun_in, n_args, n_kw, args);
    }

    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    mp_raise_TypeError(MP_ERROR_TEXT("object not callable"));
    #else
    mp_raise_msg_varg(&mp_type_TypeError,
        MP_ERROR_TEXT("'%s' object isn't callable"), mp_obj_get_type_str(fun_in));
    #endif
}

// args contains: fun  self/NULL  arg(0)  ...  arg(n_args-2)  arg(n_args-1)  kw_key(0)  kw_val(0)  ... kw_key(n_kw-1)  kw_val(n_kw-1)
// if n_args==0 and n_kw==0 then there are only fun and self/NULL
mp_obj_t mp_call_method_n_kw(size_t n_args, size_t n_kw, const mp_obj_t *args) {
    DEBUG_OP_printf("call method (fun=%p, self=%p, n_args=" UINT_FMT ", n_kw=" UINT_FMT ", args=%p)\n", args[0], args[1], n_args, n_kw, args);
    int adjust = (args[1] == MP_OBJ_NULL) ? 0 : 1;
    return mp_call_function_n_kw(args[0], n_args + adjust, n_kw, args + 2 - adjust);
}

// This function only needs to be exposed externally when in stackless mode.
#if !MICROPY_STACKLESS
static
#endif
void mp_call_prepare_args_n_kw_var(bool have_self, size_t n_args_n_kw, const mp_obj_t *args, mp_call_args_t *out_args) {
    mp_obj_t fun = *args++;
    mp_obj_t self = MP_OBJ_NULL;
    if (have_self) {
        self = *args++; // may be MP_OBJ_NULL
    }
    size_t n_args = n_args_n_kw & 0xff;
    size_t n_kw = (n_args_n_kw >> 8) & 0xff;
    mp_uint_t star_args = MP_OBJ_SMALL_INT_VALUE(args[n_args + 2 * n_kw]);

    DEBUG_OP_printf("call method var (fun=%p, self=%p, n_args=%u, n_kw=%u, args=%p, map=%u)\n", fun, self, n_args, n_kw, args, star_args);

    // We need to create the following array of objects:
    //     args[0 .. n_args]  unpacked(pos_seq)  args[n_args .. n_args + 2 * n_kw]  unpacked(kw_dict)
    // TODO: optimize one day to avoid constructing new arg array? Will be hard.

    // The new args array
    mp_obj_t *args2;
    size_t args2_alloc;
    size_t args2_len = 0;

    // Try to get a hint for unpacked * args length
    ssize_t list_len = 0;

    if (star_args != 0) {
        for (size_t i = 0; i < n_args; i++) {
            if ((star_args >> i) & 1) {
                mp_obj_t len = mp_obj_len_maybe(args[i]);
                if (len != MP_OBJ_NULL) {
                    // -1 accounts for 1 of n_args occupied by this arg
                    list_len += mp_obj_get_int(len) - 1;
                }
            }
        }
    }

    // Try to get a hint for the size of the kw_dict
    ssize_t kw_dict_len = 0;

    for (size_t i = 0; i < n_kw; i++) {
        mp_obj_t key = args[n_args + i * 2];
        mp_obj_t value = args[n_args + i * 2 + 1];
        if (key == MP_OBJ_NULL && value != MP_OBJ_NULL && mp_obj_is_type(value, &mp_type_dict)) {
            // -1 accounts for 1 of n_kw occupied by this arg
            kw_dict_len += mp_obj_dict_len(value) - 1;
        }
    }

    // Extract the pos_seq sequence to the new args array.
    // Note that it can be arbitrary iterator.
    if (star_args == 0) {
        // no star args to unpack

        // allocate memory for the new array of args
        args2_alloc = 1 + n_args + 2 * (n_kw + kw_dict_len);
        args2 = mp_nonlocal_alloc(args2_alloc * sizeof(mp_obj_t));

        // copy the self
        if (self != MP_OBJ_NULL) {
            args2[args2_len++] = self;
        }

        // copy the fixed pos args
        mp_seq_copy(args2 + args2_len, args, n_args, mp_obj_t);
        args2_len += n_args;
    } else {
        // at least one star arg to unpack

        // allocate memory for the new array of args
        args2_alloc = 1 + n_args + list_len + 2 * (n_kw + kw_dict_len);
        args2 = mp_nonlocal_alloc(args2_alloc * sizeof(mp_obj_t));

        // copy the self
        if (self != MP_OBJ_NULL) {
            args2[args2_len++] = self;
        }

        for (size_t i = 0; i < n_args; i++) {
            mp_obj_t arg = args[i];
            if ((star_args >> i) & 1) {
                // star arg
                if (mp_obj_is_type(arg, &mp_type_tuple) || mp_obj_is_type(arg, &mp_type_list)) {
                    // optimise the case of a tuple and list

                    // get the items
                    size_t len;
                    mp_obj_t *items;
                    mp_obj_get_array(arg, &len, &items);

                    // copy the items
                    assert(args2_len + len <= args2_alloc);
                    mp_seq_copy(args2 + args2_len, items, len, mp_obj_t);
                    args2_len += len;
                } else {
                    // generic iterator

                    // extract the variable position args from the iterator
                    mp_obj_iter_buf_t iter_buf;
                    mp_obj_t iterable = mp_getiter(arg, &iter_buf);
                    mp_obj_t item;
                    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
                        if (args2_len + (n_args - i) >= args2_alloc) {
                            args2 = mp_nonlocal_realloc(args2, args2_alloc * sizeof(mp_obj_t),
                                args2_alloc * 2 * sizeof(mp_obj_t));
                            args2_alloc *= 2;
                        }
                        args2[args2_len++] = item;
                    }
                }
            } else {
                // normal argument
                assert(args2_len < args2_alloc);
                args2[args2_len++] = arg;
            }
        }
    }

    // The size of the args2 array now is the number of positional args.
    size_t pos_args_len = args2_len;

    // ensure there is still enough room for kw args
    if (args2_len + 2 * (n_kw + kw_dict_len) > args2_alloc) {
        size_t new_alloc = args2_len + 2 * (n_kw + kw_dict_len);
        args2 = mp_nonlocal_realloc(args2, args2_alloc * sizeof(mp_obj_t),
            new_alloc * sizeof(mp_obj_t));
        args2_alloc = new_alloc;
    }

    // Copy the kw args.
    for (size_t i = 0; i < n_kw; i++) {
        mp_obj_t kw_key = args[n_args + i * 2];
        mp_obj_t kw_value = args[n_args + i * 2 + 1];
        if (kw_key == MP_OBJ_NULL) {
            // double-star args
            if (mp_obj_is_type(kw_value, &mp_type_dict)) {
                // dictionary
                mp_map_t *map = mp_obj_dict_get_map(kw_value);
                // should have enough, since kw_dict_len is in this case hinted correctly above
                assert(args2_len + 2 * map->used <= args2_alloc);
                for (size_t j = 0; j < map->alloc; j++) {
                    if (mp_map_slot_is_filled(map, j)) {
                        // the key must be a qstr, so intern it if it's a string
                        mp_obj_t key = map->table[j].key;
                        if (!mp_obj_is_qstr(key)) {
                            key = mp_obj_str_intern_checked(key);
                        }
                        args2[args2_len++] = key;
                        args2[args2_len++] = map->table[j].value;
                    }
                }
            } else {
                // generic mapping:
                // - call keys() to get an iterable of all keys in the mapping
                // - call __getitem__ for each key to get the corresponding value

                // get the keys iterable
                mp_obj_t dest[3];
                mp_load_method(kw_value, MP_QSTR_keys, dest);
                mp_obj_t iterable = mp_getiter(mp_call_method_n_kw(0, 0, dest), NULL);

                mp_obj_t key;
                while ((key = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
                    // expand size of args array if needed
                    if (args2_len + 1 >= args2_alloc) {
                        size_t new_alloc = args2_alloc * 2;
                        args2 = mp_nonlocal_realloc(args2, args2_alloc * sizeof(mp_obj_t), new_alloc * sizeof(mp_obj_t));
                        args2_alloc = new_alloc;
                    }

                    // the key must be a qstr, so intern it if it's a string
                    if (!mp_obj_is_qstr(key)) {
                        key = mp_obj_str_intern_checked(key);
                    }

                    // get the value corresponding to the key
                    mp_load_method(kw_value, MP_QSTR___getitem__, dest);
                    dest[2] = key;
                    mp_obj_t value = mp_call_method_n_kw(1, 0, dest);

                    // store the key/value pair in the argument array
                    args2[args2_len++] = key;
                    args2[args2_len++] = value;
                }
            }
        } else {
            // normal kwarg
            assert(args2_len + 2 <= args2_alloc);
            args2[args2_len++] = kw_key;
            args2[args2_len++] = kw_value;
        }
    }

    out_args->fun = fun;
    out_args->args = args2;
    out_args->n_args = pos_args_len;
    out_args->n_kw = (args2_len - pos_args_len) / 2;
    out_args->n_alloc = args2_alloc;
}

mp_obj_t mp_call_method_n_kw_var(bool have_self, size_t n_args_n_kw, const mp_obj_t *args) {
    mp_call_args_t out_args;
    mp_call_prepare_args_n_kw_var(have_self, n_args_n_kw, args, &out_args);

    mp_obj_t res = mp_call_function_n_kw(out_args.fun, out_args.n_args, out_args.n_kw, out_args.args);
    mp_nonlocal_free(out_args.args, out_args.n_alloc * sizeof(mp_obj_t));

    return res;
}

// unpacked items are stored in reverse order into the array pointed to by items
void mp_unpack_sequence(mp_obj_t seq_in, size_t num, mp_obj_t *items) {
    size_t seq_len;
    if (mp_obj_is_type(seq_in, &mp_type_tuple) || mp_obj_is_type(seq_in, &mp_type_list)) {
        mp_obj_t *seq_items;
        mp_obj_get_array(seq_in, &seq_len, &seq_items);
        if (seq_len < num) {
            goto too_short;
        } else if (seq_len > num) {
            goto too_long;
        }
        for (size_t i = 0; i < num; i++) {
            items[i] = seq_items[num - 1 - i];
        }
    } else {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(seq_in, &iter_buf);

        for (seq_len = 0; seq_len < num; seq_len++) {
            mp_obj_t el = mp_iternext(iterable);
            if (el == MP_OBJ_STOP_ITERATION) {
                goto too_short;
            }
            items[num - 1 - seq_len] = el;
        }
        if (mp_iternext(iterable) != MP_OBJ_STOP_ITERATION) {
            goto too_long;
        }
    }
    return;

too_short:
    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    mp_raise_ValueError(MP_ERROR_TEXT("wrong number of values to unpack"));
    #else
    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("need more than %d values to unpack"), (int)seq_len);
    #endif
too_long:
    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    mp_raise_ValueError(MP_ERROR_TEXT("wrong number of values to unpack"));
    #else
    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("too many values to unpack (expected %d)"), (int)num);
    #endif
}

// unpacked items are stored in reverse order into the array pointed to by items
void mp_unpack_ex(mp_obj_t seq_in, size_t num_in, mp_obj_t *items) {
    size_t num_left = num_in & 0xff;
    size_t num_right = (num_in >> 8) & 0xff;
    DEBUG_OP_printf("unpack ex " UINT_FMT " " UINT_FMT "\n", num_left, num_right);
    size_t seq_len;
    if (mp_obj_is_type(seq_in, &mp_type_tuple) || mp_obj_is_type(seq_in, &mp_type_list)) {
        // Make the seq variable volatile so the compiler keeps a reference to it,
        // since if it's a tuple then seq_items points to the interior of the GC cell
        // and mp_obj_new_list may trigger a GC which doesn't trace this and reclaims seq.
        volatile mp_obj_t seq = seq_in;
        mp_obj_t *seq_items;
        mp_obj_get_array(seq, &seq_len, &seq_items);
        if (seq_len < num_left + num_right) {
            goto too_short;
        }
        for (size_t i = 0; i < num_right; i++) {
            items[i] = seq_items[seq_len - 1 - i];
        }
        items[num_right] = mp_obj_new_list(seq_len - num_left - num_right, seq_items + num_left);
        for (size_t i = 0; i < num_left; i++) {
            items[num_right + 1 + i] = seq_items[num_left - 1 - i];
        }
        seq = MP_OBJ_NULL;
    } else {
        // Generic iterable; this gets a bit messy: we unpack known left length to the
        // items destination array, then the rest to a dynamically created list.  Once the
        // iterable is exhausted, we take from this list for the right part of the items.
        // TODO Improve to waste less memory in the dynamically created list.
        mp_obj_t iterable = mp_getiter(seq_in, NULL);
        mp_obj_t item;
        for (seq_len = 0; seq_len < num_left; seq_len++) {
            item = mp_iternext(iterable);
            if (item == MP_OBJ_STOP_ITERATION) {
                goto too_short;
            }
            items[num_left + num_right + 1 - 1 - seq_len] = item;
        }
        mp_obj_list_t *rest = MP_OBJ_TO_PTR(mp_obj_new_list(0, NULL));
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            mp_obj_list_append(MP_OBJ_FROM_PTR(rest), item);
        }
        if (rest->len < num_right) {
            goto too_short;
        }
        items[num_right] = MP_OBJ_FROM_PTR(rest);
        for (size_t i = 0; i < num_right; i++) {
            items[num_right - 1 - i] = rest->items[rest->len - num_right + i];
        }
        mp_obj_list_set_len(MP_OBJ_FROM_PTR(rest), rest->len - num_right);
    }
    return;

too_short:
    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    mp_raise_ValueError(MP_ERROR_TEXT("wrong number of values to unpack"));
    #else
    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("need more than %d values to unpack"), (int)seq_len);
    #endif
}

mp_obj_t mp_load_attr(mp_obj_t base, qstr attr) {
    DEBUG_OP_printf("load attr %p.%s\n", base, qstr_str(attr));
    // use load_method
    mp_obj_t dest[2];
    mp_load_method(base, attr, dest);
    if (dest[1] == MP_OBJ_NULL) {
        // load_method returned just a normal attribute
        return dest[0];
    } else {
        // load_method returned a method, so build a bound method object
        return mp_obj_new_bound_meth(dest[0], dest[1]);
    }
}

#if MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG

// The following "checked fun" type is local to the mp_convert_member_lookup
// function, and serves to check that the first argument to a builtin function
// has the correct type.

typedef struct _mp_obj_checked_fun_t {
    mp_obj_base_t base;
    const mp_obj_type_t *type;
    mp_obj_t fun;
} mp_obj_checked_fun_t;

static mp_obj_t checked_fun_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_checked_fun_t *self = MP_OBJ_TO_PTR(self_in);
    if (n_args > 0) {
        const mp_obj_type_t *arg0_type = mp_obj_get_type(args[0]);
        if (arg0_type != self->type) {
            #if MICROPY_ERROR_REPORTING != MICROPY_ERROR_REPORTING_DETAILED
            mp_raise_TypeError(MP_ERROR_TEXT("argument has wrong type"));
            #else
            mp_raise_msg_varg(&mp_type_TypeError,
                MP_ERROR_TEXT("argument should be a '%q' not a '%q'"), self->type->name, arg0_type->name);
            #endif
        }
    }
    return mp_call_function_n_kw(self->fun, n_args, n_kw, args);
}

static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_checked_fun,
    MP_QSTR_function,
    MP_TYPE_FLAG_BINDS_SELF,
    call, checked_fun_call
    );

static mp_obj_t mp_obj_new_checked_fun(const mp_obj_type_t *type, mp_obj_t fun) {
    mp_obj_checked_fun_t *o = mp_obj_malloc(mp_obj_checked_fun_t, &mp_type_checked_fun);
    o->type = type;
    o->fun = fun;
    return MP_OBJ_FROM_PTR(o);
}

#endif // MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG

// Given a member that was extracted from an instance, convert it correctly
// and put the result in the dest[] array for a possible method call.
// Conversion means dealing with static/class methods, callables, and values.
// see http://docs.python.org/3/howto/descriptor.html
// and also https://mail.python.org/pipermail/python-dev/2015-March/138950.html
void mp_convert_member_lookup(mp_obj_t self, const mp_obj_type_t *type, mp_obj_t member, mp_obj_t *dest) {
    if (mp_obj_is_obj(member)) {
        const mp_obj_type_t *m_type = ((mp_obj_base_t *)MP_OBJ_TO_PTR(member))->type;
        if (m_type->flags & MP_TYPE_FLAG_BINDS_SELF) {
            // `member` is a function that binds self as its first argument.
            if (m_type->flags & MP_TYPE_FLAG_BUILTIN_FUN) {
                // `member` is a built-in function, which has special behaviour.
                if (mp_obj_is_instance_type(type)) {
                    // Built-in functions on user types always behave like a staticmethod.
                    dest[0] = member;
                }
                #if MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG
                else if (self == MP_OBJ_NULL && type != &mp_type_object) {
                    // `member` is a built-in method without a first argument, so wrap
                    // it in a type checker that will check self when it's supplied.
                    // Note that object will do its own checking so shouldn't be wrapped.
                    dest[0] = mp_obj_new_checked_fun(type, member);
                }
                #endif
                else {
                    // Return a (built-in) bound method, with self being this object.
                    dest[0] = member;
                    dest[1] = self;
                }
            } else {
                // Return a bound method, with self being this object.
                dest[0] = member;
                dest[1] = self;
            }
        } else if (m_type == &mp_type_staticmethod) {
            // `member` is a staticmethod, return the function that it wraps.
            dest[0] = ((mp_obj_static_class_method_t *)MP_OBJ_TO_PTR(member))->fun;
        } else if (m_type == &mp_type_classmethod) {
            // `member` is a classmethod, return a bound method with self being the type of
            // this object.  This type should be the type of the original instance, not the
            // base type (which is what is passed in the `type` argument to this function).
            if (self != MP_OBJ_NULL) {
                type = mp_obj_get_type(self);
                if (type == &mp_type_type) {
                    // `self` is already a type, so use `self` directly.
                    type = MP_OBJ_TO_PTR(self);
                }
            }
            dest[0] = ((mp_obj_static_class_method_t *)MP_OBJ_TO_PTR(member))->fun;
            dest[1] = MP_OBJ_FROM_PTR(type);
        } else {
            // `member` is a value, so just return that value.
            dest[0] = member;
        }
    } else {
        // `member` is a value, so just return that value.
        dest[0] = member;
    }
}

// no attribute found, returns:     dest[0] == MP_OBJ_NULL, dest[1] == MP_OBJ_NULL
// normal attribute found, returns: dest[0] == <attribute>, dest[1] == MP_OBJ_NULL
// method attribute found, returns: dest[0] == <method>,    dest[1] == <self>
void mp_load_method_maybe(mp_obj_t obj, qstr attr, mp_obj_t *dest) {
    // clear output to indicate no attribute/method found yet
    dest[0] = MP_OBJ_NULL;
    dest[1] = MP_OBJ_NULL;

    // Note: the specific case of obj being an instance type is fast-path'ed in the VM
    // for the MP_BC_LOAD_ATTR opcode. Instance types handle type->attr and look up directly
    // in their member's map.

    // get the type
    const mp_obj_type_t *type = mp_obj_get_type(obj);

    // look for built-in names
    #if MICROPY_CPYTHON_COMPAT
    if (attr == MP_QSTR___class__) {
        // a.__class__ is equivalent to type(a)
        dest[0] = MP_OBJ_FROM_PTR(type);
        return;
    }
    #endif

    if (attr == MP_QSTR___next__ && TYPE_HAS_ITERNEXT(type)) {
        dest[0] = MP_OBJ_FROM_PTR(&mp_builtin_next_obj);
        dest[1] = obj;
        return;
    }
    if (MP_OBJ_TYPE_HAS_SLOT(type, attr)) {
        // this type can do its own load, so call it
        MP_OBJ_TYPE_GET_SLOT(type, attr)(obj, attr, dest);
        // If type->attr has set dest[1] = MP_OBJ_SENTINEL, we should proceed
        // with lookups below (i.e. in locals_dict). If not, return right away.
        if (dest[1] != MP_OBJ_SENTINEL) {
            return;
        }
        // Clear the fail flag set by type->attr so it's like it never ran.
        dest[1] = MP_OBJ_NULL;
    }
    if (MP_OBJ_TYPE_HAS_SLOT(type, locals_dict)) {
        // generic method lookup
        // this is a lookup in the object (ie not class or type)
        assert(MP_OBJ_TYPE_GET_SLOT(type, locals_dict)->base.type == &mp_type_dict); // MicroPython restriction, for now
        mp_map_t *locals_map = &MP_OBJ_TYPE_GET_SLOT(type, locals_dict)->map;
        mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        if (elem != NULL) {
            mp_convert_member_lookup(obj, type, elem->value, dest);
        }
        return;
    }
}

void mp_load_method(mp_obj_t base, qstr attr, mp_obj_t *dest) {
    DEBUG_OP_printf("load method %p.%s\n", base, qstr_str(attr));

    mp_load_method_maybe(base, attr, dest);

    if (dest[0] == MP_OBJ_NULL) {
        // no attribute/method called attr
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_msg(&mp_type_AttributeError, MP_ERROR_TEXT("no such attribute"));
        #else
        // following CPython, we give a more detailed error message for type objects
        if (mp_obj_is_type(base, &mp_type_type)) {
            mp_raise_msg_varg(&mp_type_AttributeError,
                MP_ERROR_TEXT("type object '%q' has no attribute '%q'"),
                ((mp_obj_type_t *)MP_OBJ_TO_PTR(base))->name, attr);
        } else {
            mp_raise_msg_varg(&mp_type_AttributeError,
                MP_ERROR_TEXT("'%s' object has no attribute '%q'"),
                mp_obj_get_type_str(base), attr);
        }
        #endif
    }
}

// Acts like mp_load_method_maybe but catches AttributeError, and all other exceptions if requested
void mp_load_method_protected(mp_obj_t obj, qstr attr, mp_obj_t *dest, bool catch_all_exc) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_load_method_maybe(obj, attr, dest);
        nlr_pop();
    } else {
        if (!catch_all_exc
            && !mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(((mp_obj_base_t *)nlr.ret_val)->type),
                MP_OBJ_FROM_PTR(&mp_type_AttributeError))) {
            // Re-raise the exception
            nlr_raise(MP_OBJ_FROM_PTR(nlr.ret_val));
        }
    }
}

void mp_store_attr(mp_obj_t base, qstr attr, mp_obj_t value) {
    DEBUG_OP_printf("store attr %p.%s <- %p\n", base, qstr_str(attr), value);
    const mp_obj_type_t *type = mp_obj_get_type(base);
    if (MP_OBJ_TYPE_HAS_SLOT(type, attr)) {
        mp_obj_t dest[2] = {MP_OBJ_SENTINEL, value};
        MP_OBJ_TYPE_GET_SLOT(type, attr)(base, attr, dest);
        if (dest[0] == MP_OBJ_NULL) {
            // success
            return;
        }
    }
    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    mp_raise_msg(&mp_type_AttributeError, MP_ERROR_TEXT("no such attribute"));
    #else
    mp_raise_msg_varg(&mp_type_AttributeError,
        MP_ERROR_TEXT("'%s' object has no attribute '%q'"),
        mp_obj_get_type_str(base), attr);
    #endif
}

mp_obj_t mp_getiter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    assert(o_in);
    const mp_obj_type_t *type = mp_obj_get_type(o_in);

    // Most types that use iternext just use the identity getiter. We handle this case explicitly
    // so we don't unnecessarily allocate any RAM for the iter_buf, which won't be used.
    if ((type->flags & MP_TYPE_FLAG_ITER_IS_ITERNEXT) == MP_TYPE_FLAG_ITER_IS_ITERNEXT || (type->flags & MP_TYPE_FLAG_ITER_IS_STREAM) == MP_TYPE_FLAG_ITER_IS_STREAM) {
        return o_in;
    }

    if (MP_OBJ_TYPE_HAS_SLOT(type, iter)) {
        // check for native getiter (corresponds to __iter__)
        if (iter_buf == NULL && MP_OBJ_TYPE_GET_SLOT(type, iter) != mp_obj_instance_getiter) {
            // if caller did not provide a buffer then allocate one on the heap
            // mp_obj_instance_getiter is special, it will allocate only if needed
            iter_buf = m_new_obj(mp_obj_iter_buf_t);
        }
        mp_getiter_fun_t getiter;
        if (type->flags & MP_TYPE_FLAG_ITER_IS_CUSTOM) {
            getiter = ((mp_getiter_iternext_custom_t *)MP_OBJ_TYPE_GET_SLOT(type, iter))->getiter;
        } else {
            getiter = (mp_getiter_fun_t)MP_OBJ_TYPE_GET_SLOT(type, iter);
        }
        mp_obj_t iter = getiter(o_in, iter_buf);
        if (iter != MP_OBJ_NULL) {
            return iter;
        }
    }

    // check for __getitem__
    mp_obj_t dest[2];
    mp_load_method_maybe(o_in, MP_QSTR___getitem__, dest);
    if (dest[0] != MP_OBJ_NULL) {
        // __getitem__ exists, create and return an iterator
        if (iter_buf == NULL) {
            // if caller did not provide a buffer then allocate one on the heap
            iter_buf = m_new_obj(mp_obj_iter_buf_t);
        }
        return mp_obj_new_getitem_iter(dest, iter_buf);
    }

    // object not iterable
    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    mp_raise_TypeError(MP_ERROR_TEXT("object not iterable"));
    #else
    mp_raise_msg_varg(&mp_type_TypeError,
        MP_ERROR_TEXT("'%s' object isn't iterable"), mp_obj_get_type_str(o_in));
    #endif

}

static mp_fun_1_t type_get_iternext(const mp_obj_type_t *type) {
    if ((type->flags & MP_TYPE_FLAG_ITER_IS_STREAM) == MP_TYPE_FLAG_ITER_IS_STREAM) {
        return mp_stream_unbuffered_iter;
    } else if (type->flags & MP_TYPE_FLAG_ITER_IS_ITERNEXT) {
        return (mp_fun_1_t)MP_OBJ_TYPE_GET_SLOT(type, iter);
    } else if (type->flags & MP_TYPE_FLAG_ITER_IS_CUSTOM) {
        return ((mp_getiter_iternext_custom_t *)MP_OBJ_TYPE_GET_SLOT(type, iter))->iternext;
    } else {
        return NULL;
    }
}

// may return MP_OBJ_STOP_ITERATION as an optimisation instead of raise StopIteration()
// may also raise StopIteration()
mp_obj_t mp_iternext_allow_raise(mp_obj_t o_in) {
    const mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (TYPE_HAS_ITERNEXT(type)) {
        MP_STATE_THREAD(stop_iteration_arg) = MP_OBJ_NULL;
        return type_get_iternext(type)(o_in);
    } else {
        // check for __next__ method
        mp_obj_t dest[2];
        mp_load_method_maybe(o_in, MP_QSTR___next__, dest);
        if (dest[0] != MP_OBJ_NULL) {
            // __next__ exists, call it and return its result
            return mp_call_method_n_kw(0, 0, dest);
        } else {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError(MP_ERROR_TEXT("object not an iterator"));
            #else
            mp_raise_msg_varg(&mp_type_TypeError,
                MP_ERROR_TEXT("'%s' object isn't an iterator"), mp_obj_get_type_str(o_in));
            #endif
        }
    }
}

// will always return MP_OBJ_STOP_ITERATION instead of raising StopIteration() (or any subclass thereof)
// may raise other exceptions
mp_obj_t mp_iternext(mp_obj_t o_in) {
    mp_cstack_check(); // enumerate, filter, map and zip can recursively call mp_iternext
    const mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (TYPE_HAS_ITERNEXT(type)) {
        MP_STATE_THREAD(stop_iteration_arg) = MP_OBJ_NULL;
        return type_get_iternext(type)(o_in);
    } else {
        // check for __next__ method
        mp_obj_t dest[2];
        mp_load_method_maybe(o_in, MP_QSTR___next__, dest);
        if (dest[0] != MP_OBJ_NULL) {
            // __next__ exists, call it and return its result
            nlr_buf_t nlr;
            if (nlr_push(&nlr) == 0) {
                mp_obj_t ret = mp_call_method_n_kw(0, 0, dest);
                nlr_pop();
                return ret;
            } else {
                if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(((mp_obj_base_t *)nlr.ret_val)->type), MP_OBJ_FROM_PTR(&mp_type_StopIteration))) {
                    return mp_make_stop_iteration(mp_obj_exception_get_value(MP_OBJ_FROM_PTR(nlr.ret_val)));
                } else {
                    nlr_jump(nlr.ret_val);
                }
            }
        } else {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError(MP_ERROR_TEXT("object not an iterator"));
            #else
            mp_raise_msg_varg(&mp_type_TypeError,
                MP_ERROR_TEXT("'%s' object isn't an iterator"), mp_obj_get_type_str(o_in));
            #endif
        }
    }
}

mp_vm_return_kind_t mp_resume(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t *ret_val) {
    assert((send_value != MP_OBJ_NULL) ^ (throw_value != MP_OBJ_NULL));
    const mp_obj_type_t *type = mp_obj_get_type(self_in);

    if (type == &mp_type_gen_instance) {
        return mp_obj_gen_resume(self_in, send_value, throw_value, ret_val);
    }

    if (TYPE_HAS_ITERNEXT(type) && send_value == mp_const_none) {
        MP_STATE_THREAD(stop_iteration_arg) = MP_OBJ_NULL;
        mp_obj_t ret = type_get_iternext(type)(self_in);
        *ret_val = ret;
        if (ret != MP_OBJ_STOP_ITERATION) {
            return MP_VM_RETURN_YIELD;
        } else {
            // The generator is finished.
            // This is an optimised "raise StopIteration(*ret_val)".
            *ret_val = MP_STATE_THREAD(stop_iteration_arg);
            if (*ret_val == MP_OBJ_NULL) {
                *ret_val = mp_const_none;
            }
            return MP_VM_RETURN_NORMAL;
        }
    }

    mp_obj_t dest[3]; // Reserve slot for send() arg

    // Python instance iterator protocol
    if (send_value == mp_const_none) {
        mp_load_method_maybe(self_in, MP_QSTR___next__, dest);
        if (dest[0] != MP_OBJ_NULL) {
            *ret_val = mp_call_method_n_kw(0, 0, dest);
            return MP_VM_RETURN_YIELD;
        }
    }

    // Either python instance generator protocol, or native object
    // generator protocol.
    if (send_value != MP_OBJ_NULL) {
        mp_load_method(self_in, MP_QSTR_send, dest);
        dest[2] = send_value;
        *ret_val = mp_call_method_n_kw(1, 0, dest);
        return MP_VM_RETURN_YIELD;
    }

    assert(throw_value != MP_OBJ_NULL);
    {
        if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(throw_value)), MP_OBJ_FROM_PTR(&mp_type_GeneratorExit))) {
            mp_load_method_maybe(self_in, MP_QSTR_close, dest);
            if (dest[0] != MP_OBJ_NULL) {
                // TODO: Exceptions raised in close() are not propagated,
                // printed to sys.stderr
                *ret_val = mp_call_method_n_kw(0, 0, dest);
                // We assume one can't "yield" from close()
                return MP_VM_RETURN_NORMAL;
            }
        } else {
            mp_load_method_maybe(self_in, MP_QSTR_throw, dest);
            if (dest[0] != MP_OBJ_NULL) {
                dest[2] = throw_value;
                *ret_val = mp_call_method_n_kw(1, 0, dest);
                // If .throw() method returned, we assume it's value to yield
                // - any exception would be thrown with nlr_raise().
                return MP_VM_RETURN_YIELD;
            }
        }
        // If there's nowhere to throw exception into, then we assume that object
        // is just incapable to handle it, so any exception thrown into it
        // will be propagated up. This behavior is approved by test_pep380.py
        // test_delegation_of_close_to_non_generator(),
        //  test_delegating_throw_to_non_generator()
        if (mp_obj_exception_match(throw_value, MP_OBJ_FROM_PTR(&mp_type_StopIteration))) {
            // PEP479: if StopIteration is raised inside a generator it is replaced with RuntimeError
            *ret_val = mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("generator raised StopIteration"));
        } else {
            *ret_val = mp_make_raise_obj(throw_value);
        }
        return MP_VM_RETURN_EXCEPTION;
    }
}

mp_obj_t mp_make_raise_obj(mp_obj_t o) {
    DEBUG_printf("raise %p\n", o);
    if (mp_obj_is_exception_type(o)) {
        // o is an exception type (it is derived from BaseException (or is BaseException))
        // create and return a new exception instance by calling o
        // TODO could have an option to disable traceback, then builtin exceptions (eg TypeError)
        // could have const instances in ROM which we return here instead
        o = mp_call_function_n_kw(o, 0, 0, NULL);
    }

    if (mp_obj_is_exception_instance(o)) {
        // o is an instance of an exception, so use it as the exception
        return o;
    } else {
        // o cannot be used as an exception, so return a type error (which will be raised by the caller)
        return mp_obj_new_exception_msg(&mp_type_TypeError, MP_ERROR_TEXT("exceptions must derive from BaseException"));
    }
}

mp_obj_t mp_import_name(qstr name, mp_obj_t fromlist, mp_obj_t level) {
    DEBUG_printf("import name '%s' level=%d\n", qstr_str(name), MP_OBJ_SMALL_INT_VALUE(level));

    // build args array
    mp_obj_t args[5];
    args[0] = MP_OBJ_NEW_QSTR(name);
    args[1] = mp_const_none; // TODO should be globals
    args[2] = mp_const_none; // TODO should be locals
    args[3] = fromlist;
    args[4] = level;

    #if MICROPY_CAN_OVERRIDE_BUILTINS
    // Lookup __import__ and call that if it exists
    mp_obj_dict_t *bo_dict = MP_STATE_VM(mp_module_builtins_override_dict);
    if (bo_dict != NULL) {
        mp_map_elem_t *import = mp_map_lookup(&bo_dict->map, MP_OBJ_NEW_QSTR(MP_QSTR___import__), MP_MAP_LOOKUP);
        if (import != NULL) {
            return mp_call_function_n_kw(import->value, 5, 0, args);
        }
    }
    #endif

    return mp_builtin___import__(5, args);
}

mp_obj_t mp_import_from(mp_obj_t module, qstr name) {
    DEBUG_printf("import from %p %s\n", module, qstr_str(name));

    mp_obj_t dest[2];

    mp_load_method_maybe(module, name, dest);

    if (dest[1] != MP_OBJ_NULL) {
        // Hopefully we can't import bound method from an object
    import_error:
        mp_raise_msg_varg(&mp_type_ImportError, MP_ERROR_TEXT("can't import name %q"), name);
    }

    if (dest[0] != MP_OBJ_NULL) {
        return dest[0];
    }

    #if MICROPY_ENABLE_EXTERNAL_IMPORT

    // See if it's a package, then can try FS import
    mp_load_method_maybe(module, MP_QSTR___path__, dest);
    if (dest[0] == MP_OBJ_NULL) {
        goto import_error;
    }

    mp_load_method_maybe(module, MP_QSTR___name__, dest);
    size_t pkg_name_len;
    const char *pkg_name = mp_obj_str_get_data(dest[0], &pkg_name_len);

    const uint dot_name_len = pkg_name_len + 1 + qstr_len(name);
    char *dot_name = mp_local_alloc(dot_name_len);
    memcpy(dot_name, pkg_name, pkg_name_len);
    dot_name[pkg_name_len] = '.';
    memcpy(dot_name + pkg_name_len + 1, qstr_str(name), qstr_len(name));
    qstr dot_name_q = qstr_from_strn(dot_name, dot_name_len);
    mp_local_free(dot_name);

    // For fromlist, pass sentinel "non empty" value to force returning of leaf module
    return mp_import_name(dot_name_q, mp_const_true, MP_OBJ_NEW_SMALL_INT(0));

    #else

    // Package import not supported with external imports disabled
    goto import_error;

    #endif
}

void mp_import_all(mp_obj_t module) {
    DEBUG_printf("import all %p\n", module);

    // TODO: Support __all__
    mp_map_t *map = &mp_obj_module_get_globals(module)->map;
    for (size_t i = 0; i < map->alloc; i++) {
        if (mp_map_slot_is_filled(map, i)) {
            // Entry in module global scope may be generated programmatically
            // (and thus be not a qstr for longer names). Avoid turning it in
            // qstr if it has '_' and was used exactly to save memory.
            const char *name = mp_obj_str_get_str(map->table[i].key);
            if (*name != '_') {
                qstr qname = mp_obj_str_get_qstr(map->table[i].key);
                mp_store_name(qname, map->table[i].value);
            }
        }
    }
}

#if MICROPY_ENABLE_COMPILER

mp_obj_t mp_parse_compile_execute(mp_lexer_t *lex, mp_parse_input_kind_t parse_input_kind, mp_obj_dict_t *globals, mp_obj_dict_t *locals) {
    // save context
    nlr_jump_callback_node_globals_locals_t ctx;
    ctx.globals = mp_globals_get();
    ctx.locals = mp_locals_get();

    // set new context
    mp_globals_set(globals);
    mp_locals_set(locals);

    // set exception handler to restore context if an exception is raised
    nlr_push_jump_callback(&ctx.callback, mp_globals_locals_set_from_nlr_jump_callback);

    qstr source_name = lex->source_name;
    mp_parse_tree_t parse_tree = mp_parse(lex, parse_input_kind);
    mp_obj_t module_fun = mp_compile(&parse_tree, source_name, parse_input_kind == MP_PARSE_SINGLE_INPUT);

    mp_obj_t ret;
    if (MICROPY_PY_BUILTINS_COMPILE && globals == NULL) {
        // for compile only, return value is the module function
        ret = module_fun;
    } else {
        // execute module function and get return value
        ret = mp_call_function_0(module_fun);
    }

    // deregister exception handler and restore context
    nlr_pop_jump_callback(true);

    // return value
    return ret;
}

#endif // MICROPY_ENABLE_COMPILER

NORETURN void m_malloc_fail(size_t num_bytes) {
    DEBUG_printf("memory allocation failed, allocating %u bytes\n", (uint)num_bytes);
    #if MICROPY_ENABLE_GC
    if (gc_is_locked()) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("memory allocation failed, heap is locked"));
    }
    #endif
    mp_raise_msg_varg(&mp_type_MemoryError,
        MP_ERROR_TEXT("memory allocation failed, allocating %u bytes"), (uint)num_bytes);
}

#if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_NONE

NORETURN void mp_raise_type(const mp_obj_type_t *exc_type) {
    nlr_raise(mp_obj_new_exception(exc_type));
}

NORETURN void mp_raise_ValueError_no_msg(void) {
    mp_raise_type(&mp_type_ValueError);
}

NORETURN void mp_raise_TypeError_no_msg(void) {
    mp_raise_type(&mp_type_TypeError);
}

NORETURN void mp_raise_NotImplementedError_no_msg(void) {
    mp_raise_type(&mp_type_NotImplementedError);
}

#else

NORETURN void mp_raise_msg(const mp_obj_type_t *exc_type, mp_rom_error_text_t msg) {
    if (msg == NULL) {
        nlr_raise(mp_obj_new_exception(exc_type));
    } else {
        nlr_raise(mp_obj_new_exception_msg(exc_type, msg));
    }
}

NORETURN void mp_raise_msg_varg(const mp_obj_type_t *exc_type, mp_rom_error_text_t fmt, ...) {
    va_list args;
    va_start(args, fmt);
    mp_obj_t exc = mp_obj_new_exception_msg_vlist(exc_type, fmt, args);
    va_end(args);
    nlr_raise(exc);
}

NORETURN void mp_raise_ValueError(mp_rom_error_text_t msg) {
    mp_raise_msg(&mp_type_ValueError, msg);
}

NORETURN void mp_raise_TypeError(mp_rom_error_text_t msg) {
    mp_raise_msg(&mp_type_TypeError, msg);
}

NORETURN void mp_raise_NotImplementedError(mp_rom_error_text_t msg) {
    mp_raise_msg(&mp_type_NotImplementedError, msg);
}

#endif

NORETURN void mp_raise_type_arg(const mp_obj_type_t *exc_type, mp_obj_t arg) {
    nlr_raise(mp_obj_new_exception_arg1(exc_type, arg));
}

NORETURN void mp_raise_StopIteration(mp_obj_t arg) {
    if (arg == MP_OBJ_NULL) {
        mp_raise_type(&mp_type_StopIteration);
    } else {
        mp_raise_type_arg(&mp_type_StopIteration, arg);
    }
}

NORETURN void mp_raise_TypeError_int_conversion(mp_const_obj_t arg) {
    #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
    (void)arg;
    mp_raise_TypeError(MP_ERROR_TEXT("can't convert to int"));
    #else
    mp_raise_msg_varg(&mp_type_TypeError,
        MP_ERROR_TEXT("can't convert %s to int"), mp_obj_get_type_str(arg));
    #endif
}

NORETURN void mp_raise_OSError(int errno_) {
    mp_raise_type_arg(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(errno_));
}

NORETURN void mp_raise_OSError_with_filename(int errno_, const char *filename) {
    vstr_t vstr;
    vstr_init(&vstr, 32);
    vstr_printf(&vstr, "can't open %s", filename);
    mp_obj_t o_str = mp_obj_new_str_from_vstr(&vstr);
    mp_obj_t args[2] = { MP_OBJ_NEW_SMALL_INT(errno_), MP_OBJ_FROM_PTR(o_str)};
    nlr_raise(mp_obj_exception_make_new(&mp_type_OSError, 2, 0, args));
}

#if MICROPY_STACK_CHECK || MICROPY_ENABLE_PYSTACK
NORETURN void mp_raise_recursion_depth(void) {
    mp_raise_type_arg(&mp_type_RuntimeError, MP_OBJ_NEW_QSTR(MP_QSTR_maximum_space_recursion_space_depth_space_exceeded));
}
#endif
