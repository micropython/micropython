#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "objtuple.h"
#include "objmodule.h"
#include "parsenum.h"
#include "runtime0.h"
#include "runtime.h"
#include "emitglue.h"
#include "builtin.h"
#include "builtintables.h"
#include "bc.h"
#include "intdivmod.h"
#include "objgenerator.h"

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#define DEBUG_OP_printf(...) DEBUG_printf(__VA_ARGS__)
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#define DEBUG_OP_printf(...) (void)0
#endif

// locals and globals need to be pointers because they can be the same in outer module scope
STATIC mp_map_t *map_locals;
STATIC mp_map_t *map_globals;
STATIC mp_map_t map_builtins;

// a good optimising compiler will inline this if necessary
STATIC void mp_map_add_qstr(mp_map_t *map, qstr qstr, mp_obj_t value) {
    mp_map_lookup(map, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
}

void mp_init(void) {
    mp_emit_glue_init();

    // locals = globals for outer module (see Objects/frameobject.c/PyFrame_New())
    map_locals = map_globals = mp_map_new(1);

    // init built-in hash table
    mp_map_init(&map_builtins, 3);

    // init global module stuff
    mp_module_init();

    // add some builtins that can't be done in ROM
    mp_map_add_qstr(map_globals, MP_QSTR___name__, MP_OBJ_NEW_QSTR(MP_QSTR___main__));

#if MICROPY_CPYTHON_COMPAT
    // Precreate sys module, so "import sys" didn't throw exceptions.
    mp_obj_t m_sys = mp_obj_new_module(MP_QSTR_sys);
    // Avoid warning of unused var
    (void)m_sys;
#endif
    // init sys.path
    // for efficiency, left to platform-specific startup code
    //mp_sys_path = mp_obj_new_list(0, NULL);
    //mp_store_attr(m_sys, MP_QSTR_path, mp_sys_path);
}

void mp_deinit(void) {
    mp_map_free(map_globals);
    mp_map_deinit(&map_builtins);
    mp_module_deinit();
    mp_emit_glue_deinit();
}

mp_obj_t mp_load_const_dec(qstr qstr) {
    DEBUG_OP_printf("load '%s'\n", qstr_str(qstr));
    uint len;
    const byte* data = qstr_data(qstr, &len);
    return mp_parse_num_decimal((const char*)data, len, true, false);
}

mp_obj_t mp_load_const_str(qstr qstr) {
    DEBUG_OP_printf("load '%s'\n", qstr_str(qstr));
    return MP_OBJ_NEW_QSTR(qstr);
}

mp_obj_t mp_load_const_bytes(qstr qstr) {
    DEBUG_OP_printf("load b'%s'\n", qstr_str(qstr));
    uint len;
    const byte *data = qstr_data(qstr, &len);
    return mp_obj_new_bytes(data, len);
}

mp_obj_t mp_load_name(qstr qstr) {
    // logic: search locals, globals, builtins
    DEBUG_OP_printf("load name %s\n", qstr_str(qstr));
    mp_map_elem_t *elem = mp_map_lookup(map_locals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP);
    if (elem != NULL) {
        return elem->value;
    } else {
        return mp_load_global(qstr);
    }
}

mp_obj_t mp_load_global(qstr qstr) {
    // logic: search globals, builtins
    DEBUG_OP_printf("load global %s\n", qstr_str(qstr));
    mp_map_elem_t *elem = mp_map_lookup(map_globals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP);
    if (elem == NULL) {
        elem = mp_map_lookup(&map_builtins, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP);
        if (elem == NULL) {
            mp_obj_t o = mp_builtin_tables_lookup_object(qstr);
            if (o != MP_OBJ_NULL) {
                return o;
            }
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_NameError, "name '%s' is not defined", qstr_str(qstr)));
        }
    }
    return elem->value;
}

mp_obj_t mp_load_build_class(void) {
    DEBUG_OP_printf("load_build_class\n");
    // lookup __build_class__ in dynamic table of builtins first
    mp_map_elem_t *elem = mp_map_lookup(&map_builtins, MP_OBJ_NEW_QSTR(MP_QSTR___build_class__), MP_MAP_LOOKUP);
    if (elem != NULL) {
        // found user-defined __build_class__, return it
        return elem->value;
    } else {
        // no user-defined __build_class__, return builtin one
        return (mp_obj_t)&mp_builtin___build_class___obj;
    }
}

void mp_store_name(qstr qstr, mp_obj_t obj) {
    DEBUG_OP_printf("store name %s <- %p\n", qstr_str(qstr), obj);
    mp_map_lookup(map_locals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = obj;
}

void mp_delete_name(qstr qstr) {
    DEBUG_OP_printf("delete name %s\n", qstr_str(qstr));
    mp_map_lookup(map_locals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
}

void mp_store_global(qstr qstr, mp_obj_t obj) {
    DEBUG_OP_printf("store global %s <- %p\n", qstr_str(qstr), obj);
    mp_map_lookup(map_globals, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = obj;
}

mp_obj_t mp_unary_op(int op, mp_obj_t arg) {
    DEBUG_OP_printf("unary %d %p\n", op, arg);

    if (MP_OBJ_IS_SMALL_INT(arg)) {
        mp_small_int_t val = MP_OBJ_SMALL_INT_VALUE(arg);
        switch (op) {
            case MP_UNARY_OP_BOOL:
                return MP_BOOL(val != 0);
            case MP_UNARY_OP_POSITIVE:
                return arg;
            case MP_UNARY_OP_NEGATIVE:
                // check for overflow
                if (val == MP_SMALL_INT_MIN) {
                    return mp_obj_new_int(-val);
                } else {
                    return MP_OBJ_NEW_SMALL_INT(-val);
                }
            case MP_UNARY_OP_INVERT:
                return MP_OBJ_NEW_SMALL_INT(~val);
            default:
                assert(0);
                return arg;
        }
    } else {
        mp_obj_type_t *type = mp_obj_get_type(arg);
        if (type->unary_op != NULL) {
            mp_obj_t result = type->unary_op(op, arg);
            if (result != NULL) {
                return result;
            }
        }
        // TODO specify in error message what the operator is
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "bad operand type for unary operator: '%s'", mp_obj_get_type_str(arg)));
    }
}

mp_obj_t mp_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    DEBUG_OP_printf("binary %d %p %p\n", op, lhs, rhs);

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
        return MP_BOOL(lhs == rhs);
    }

    // deal with == and != for all types
    if (op == MP_BINARY_OP_EQUAL || op == MP_BINARY_OP_NOT_EQUAL) {
        if (mp_obj_equal(lhs, rhs)) {
            if (op == MP_BINARY_OP_EQUAL) {
                return mp_const_true;
            } else {
                return mp_const_false;
            }
        } else {
            if (op == MP_BINARY_OP_EQUAL) {
                return mp_const_false;
            } else {
                return mp_const_true;
            }
        }
    }

    // deal with exception_match for all types
    if (op == MP_BINARY_OP_EXCEPTION_MATCH) {
        // rhs must be issubclass(rhs, BaseException)
        if (mp_obj_is_exception_type(rhs)) {
            // if lhs is an instance of an exception, then extract and use its type
            if (mp_obj_is_exception_instance(lhs)) {
                lhs = mp_obj_get_type(lhs);
            }
            if (mp_obj_is_subclass_fast(lhs, rhs)) {
                return mp_const_true;
            } else {
                return mp_const_false;
            }
        }
        assert(0);
        return mp_const_false;
    }

    if (MP_OBJ_IS_SMALL_INT(lhs)) {
        mp_small_int_t lhs_val = MP_OBJ_SMALL_INT_VALUE(lhs);
        if (MP_OBJ_IS_SMALL_INT(rhs)) {
            mp_small_int_t rhs_val = MP_OBJ_SMALL_INT_VALUE(rhs);
            // This is a binary operation: lhs_val op rhs_val
            // We need to be careful to handle overflow; see CERT INT32-C
            // Operations that can overflow:
            //      +       result always fits in machine_int_t, then handled by SMALL_INT check
            //      -       result always fits in machine_int_t, then handled by SMALL_INT check
            //      *       checked explicitly
            //      /       if lhs=MIN and rhs=-1; result always fits in machine_int_t, then handled by SMALL_INT check
            //      %       if lhs=MIN and rhs=-1; result always fits in machine_int_t, then handled by SMALL_INT check
            //      <<      checked explicitly
            switch (op) {
                case MP_BINARY_OP_OR:
                case MP_BINARY_OP_INPLACE_OR: lhs_val |= rhs_val; break;
                case MP_BINARY_OP_XOR:
                case MP_BINARY_OP_INPLACE_XOR: lhs_val ^= rhs_val; break;
                case MP_BINARY_OP_AND:
                case MP_BINARY_OP_INPLACE_AND: lhs_val &= rhs_val; break;
                case MP_BINARY_OP_LSHIFT:
                case MP_BINARY_OP_INPLACE_LSHIFT: {
                    if (rhs_val < 0) {
                        // negative shift not allowed
                        nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "negative shift count"));
                    } else if (rhs_val >= BITS_PER_WORD || lhs_val > (MP_SMALL_INT_MAX >> rhs_val) || lhs_val < (MP_SMALL_INT_MIN >> rhs_val)) {
                        // left-shift will overflow, so use higher precision integer
                        lhs = mp_obj_new_int_from_ll(lhs_val);
                        goto generic_binary_op;
                    } else {
                        // use standard precision
                        lhs_val <<= rhs_val;
                    }
                    break;
                }
                case MP_BINARY_OP_RSHIFT:
                case MP_BINARY_OP_INPLACE_RSHIFT:
                    if (rhs_val < 0) {
                        // negative shift not allowed
                        nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "negative shift count"));
                    } else {
                        // standard precision is enough for right-shift
                        lhs_val >>= rhs_val;
                    }
                    break;
                case MP_BINARY_OP_ADD:
                case MP_BINARY_OP_INPLACE_ADD: lhs_val += rhs_val; break;
                case MP_BINARY_OP_SUBTRACT:
                case MP_BINARY_OP_INPLACE_SUBTRACT: lhs_val -= rhs_val; break;
                case MP_BINARY_OP_MULTIPLY:
                case MP_BINARY_OP_INPLACE_MULTIPLY: {

                    // If long long type exists and is larger than machine_int_t, then
                    // we can use the following code to perform overflow-checked multiplication.
                    // Otherwise (eg in x64 case) we must use the branching code below.
                    #if 0
                    // compute result using long long precision
                    long long res = (long long)lhs_val * (long long)rhs_val;
                    if (res > MP_SMALL_INT_MAX || res < MP_SMALL_INT_MIN) {
                        // result overflowed SMALL_INT, so return higher precision integer
                        return mp_obj_new_int_from_ll(res);
                    } else {
                        // use standard precision
                        lhs_val = (mp_small_int_t)res;
                    }
                    #endif

                    if (lhs_val > 0) { // lhs_val is positive
                        if (rhs_val > 0) { // lhs_val and rhs_val are positive
                            if (lhs_val > (MP_SMALL_INT_MAX / rhs_val)) {
                                goto mul_overflow;
                            }
                        } else { // lhs_val positive, rhs_val nonpositive
                            if (rhs_val < (MP_SMALL_INT_MIN / lhs_val)) {
                                goto mul_overflow;
                            }
                        } // lhs_val positive, rhs_val nonpositive
                    } else { // lhs_val is nonpositive
                        if (rhs_val > 0) { // lhs_val is nonpositive, rhs_val is positive
                            if (lhs_val < (MP_SMALL_INT_MIN / rhs_val)) {
                                goto mul_overflow;
                            }
                        } else { // lhs_val and rhs_val are nonpositive
                            if (lhs_val != 0 && rhs_val < (MP_SMALL_INT_MAX / lhs_val)) {
                                goto mul_overflow;
                            }
                        } // End if lhs_val and rhs_val are nonpositive
                    } // End if lhs_val is nonpositive

                    // use standard precision
                    return MP_OBJ_NEW_SMALL_INT(lhs_val * rhs_val);

                mul_overflow:
                    // use higher precision
                    lhs = mp_obj_new_int_from_ll(lhs_val);
                    goto generic_binary_op;

                    break;
                }
                case MP_BINARY_OP_FLOOR_DIVIDE:
                case MP_BINARY_OP_INPLACE_FLOOR_DIVIDE:
                    if (rhs_val == 0) {
                        goto zero_division;
                    }
                    lhs_val = python_floor_divide(lhs_val, rhs_val);
                    break;

                #if MICROPY_ENABLE_FLOAT
                case MP_BINARY_OP_TRUE_DIVIDE:
                case MP_BINARY_OP_INPLACE_TRUE_DIVIDE:
                    if (rhs_val == 0) {
                        goto zero_division;
                    }
                    return mp_obj_new_float((mp_float_t)lhs_val / (mp_float_t)rhs_val);
                #endif

                case MP_BINARY_OP_MODULO:
                case MP_BINARY_OP_INPLACE_MODULO:
                {
                    lhs_val = python_modulo(lhs_val, rhs_val);
                    break;
                }
                case MP_BINARY_OP_POWER:
                case MP_BINARY_OP_INPLACE_POWER:
                    if (rhs_val < 0) {
                        #if MICROPY_ENABLE_FLOAT
                        lhs = mp_obj_new_float(lhs_val);
                        goto generic_binary_op;
                        #else
                        nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "negative power with no float support"));
                        #endif
                    } else {
                        machine_int_t ans = 1;
                        while (rhs_val > 0) {
                            if (rhs_val & 1) {
                                machine_int_t old = ans;
                                ans *= lhs_val;
                                if (ans < old) {
                                    goto power_overflow;
                                }
                            }
                            if (rhs_val == 1) {
                                break;
                            }
                            rhs_val /= 2;
                            machine_int_t old = lhs_val;
                            lhs_val *= lhs_val;
                            if (lhs_val < old) {
                                goto power_overflow;
                            }
                        }
                        lhs_val = ans;
                    }
                    break;

                power_overflow:
                    // use higher precision
                    lhs = mp_obj_new_int_from_ll(MP_OBJ_SMALL_INT_VALUE(lhs));
                    goto generic_binary_op;

                case MP_BINARY_OP_LESS: return MP_BOOL(lhs_val < rhs_val); break;
                case MP_BINARY_OP_MORE: return MP_BOOL(lhs_val > rhs_val); break;
                case MP_BINARY_OP_LESS_EQUAL: return MP_BOOL(lhs_val <= rhs_val); break;
                case MP_BINARY_OP_MORE_EQUAL: return MP_BOOL(lhs_val >= rhs_val); break;

                default: assert(0);
            }
            // TODO: We just should make mp_obj_new_int() inline and use that
            if (MP_OBJ_FITS_SMALL_INT(lhs_val)) {
                return MP_OBJ_NEW_SMALL_INT(lhs_val);
            } else {
                return mp_obj_new_int(lhs_val);
            }
#if MICROPY_ENABLE_FLOAT
        } else if (MP_OBJ_IS_TYPE(rhs, &mp_type_float)) {
            return mp_obj_float_binary_op(op, lhs_val, rhs);
        } else if (MP_OBJ_IS_TYPE(rhs, &mp_type_complex)) {
            return mp_obj_complex_binary_op(op, lhs_val, 0, rhs);
#endif
        }
    }

    /* deal with `in`
     *
     * NOTE `a in b` is `b.__contains__(a)`, hence why the generic dispatch
     * needs to go below with swapped arguments
     */
    if (op == MP_BINARY_OP_IN) {
        mp_obj_type_t *type = mp_obj_get_type(rhs);
        if (type->binary_op != NULL) {
            mp_obj_t res = type->binary_op(op, rhs, lhs);
            if (res != MP_OBJ_NULL) {
                return res;
            }
        }
        if (type->getiter != NULL) {
            /* second attempt, walk the iterator */
            mp_obj_t next = NULL;
            mp_obj_t iter = mp_getiter(rhs);
            while ((next = mp_iternext(iter)) != MP_OBJ_NULL) {
                if (mp_obj_equal(next, lhs)) {
                    return mp_const_true;
                }
            }
            return mp_const_false;
        }

        nlr_jump(mp_obj_new_exception_msg_varg(
                     &mp_type_TypeError, "'%s' object is not iterable",
                     mp_obj_get_type_str(rhs)));
        return mp_const_none;
    }

    // generic binary_op supplied by type
    mp_obj_type_t *type;
generic_binary_op:
    type = mp_obj_get_type(lhs);
    if (type->binary_op != NULL) {
        mp_obj_t result = type->binary_op(op, lhs, rhs);
        if (result != MP_OBJ_NULL) {
            return result;
        }
    }

    // TODO implement dispatch for reverse binary ops

    // TODO specify in error message what the operator is
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
        "unsupported operand types for binary operator: '%s', '%s'",
        mp_obj_get_type_str(lhs), mp_obj_get_type_str(rhs)));
    return mp_const_none;

zero_division:
    nlr_jump(mp_obj_new_exception_msg(&mp_type_ZeroDivisionError, "division by zero"));
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

// wrapper that accepts n_args and n_kw in one argument
// native emitter can only pass at most 3 arguments to a function
mp_obj_t mp_call_function_n_kw_for_native(mp_obj_t fun_in, uint n_args_kw, const mp_obj_t *args) {
    return mp_call_function_n_kw(fun_in, n_args_kw & 0xff, (n_args_kw >> 8) & 0xff, args);
}

// args contains, eg: arg0  arg1  key0  value0  key1  value1
mp_obj_t mp_call_function_n_kw(mp_obj_t fun_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO improve this: fun object can specify its type and we parse here the arguments,
    // passing to the function arrays of fixed and keyword arguments

    DEBUG_OP_printf("calling function %p(n_args=%d, n_kw=%d, args=%p)\n", fun_in, n_args, n_kw, args);

    // get the type
    mp_obj_type_t *type = mp_obj_get_type(fun_in);

    // do the call
    if (type->call != NULL) {
        return type->call(fun_in, n_args, n_kw, args);
    } else {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object is not callable", mp_obj_get_type_str(fun_in)));
    }
}

// args contains: fun  self/NULL  arg(0)  ...  arg(n_args-2)  arg(n_args-1)  kw_key(0)  kw_val(0)  ... kw_key(n_kw-1)  kw_val(n_kw-1)
// if n_args==0 and n_kw==0 then there are only fun and self/NULL
mp_obj_t mp_call_method_n_kw(uint n_args, uint n_kw, const mp_obj_t *args) {
    DEBUG_OP_printf("call method (fun=%p, self=%p, n_args=%u, n_kw=%u, args=%p)\n", args[0], args[1], n_args, n_kw, args);
    int adjust = (args[1] == NULL) ? 0 : 1;
    return mp_call_function_n_kw(args[0], n_args + adjust, n_kw, args + 2 - adjust);
}

mp_obj_t mp_call_method_n_kw_var(bool have_self, uint n_args_n_kw, const mp_obj_t *args) {
    mp_obj_t fun = *args++;
    mp_obj_t self = MP_OBJ_NULL;
    if (have_self) {
        self = *args++; // may be MP_OBJ_NULL
    }
    uint n_args = n_args_n_kw & 0xff;
    uint n_kw = (n_args_n_kw >> 8) & 0xff;
    mp_obj_t pos_seq = args[n_args + 2 * n_kw]; // map be MP_OBJ_NULL
    mp_obj_t kw_dict = args[n_args + 2 * n_kw + 1]; // map be MP_OBJ_NULL

    DEBUG_OP_printf("call method var (fun=%p, self=%p, n_args=%u, n_kw=%u, args=%p, seq=%p, dict=%p)\n", fun, self, n_args, n_kw, args, pos_seq, kw_dict);

    // We need to create the following array of objects:
    //     args[0 .. n_args]  unpacked(pos_seq)  args[n_args .. n_args + 2 * n_kw]  unpacked(kw_dict)
    // TODO: optimize one day to avoid constructing new arg array? Will be hard.

    // The new args array
    mp_obj_t *args2;
    uint args2_alloc;
    uint args2_len = 0;

    // Try to get a hint for the size of the kw_dict
    uint kw_dict_len = 0;
    if (kw_dict != MP_OBJ_NULL && MP_OBJ_IS_TYPE(kw_dict, &mp_type_dict)) {
        kw_dict_len = mp_obj_dict_len(kw_dict);
    }

    // Extract the pos_seq sequence to the new args array.
    // Note that it can be arbitrary iterator.
    if (pos_seq == MP_OBJ_NULL) {
        // no sequence

        // allocate memory for the new array of args
        args2_alloc = 1 + n_args + 2 * (n_kw + kw_dict_len);
        args2 = m_new(mp_obj_t, args2_alloc);

        // copy the self
        if (self != MP_OBJ_NULL) {
            args2[args2_len++] = self;
        }

        // copy the fixed pos args
        m_seq_copy(args2 + args2_len, args, n_args, mp_obj_t);
        args2_len += n_args;

    } else if (MP_OBJ_IS_TYPE(pos_seq, &mp_type_tuple) || MP_OBJ_IS_TYPE(pos_seq, &mp_type_list)) {
        // optimise the case of a tuple and list

        // get the items
        uint len;
        mp_obj_t *items;
        mp_obj_get_array(pos_seq, &len, &items);

        // allocate memory for the new array of args
        args2_alloc = 1 + n_args + len + 2 * (n_kw + kw_dict_len);
        args2 = m_new(mp_obj_t, args2_alloc);

        // copy the self
        if (self != MP_OBJ_NULL) {
            args2[args2_len++] = self;
        }

        // copy the fixed and variable position args
        m_seq_cat(args2 + args2_len, args, n_args, items, len, mp_obj_t);
        args2_len += n_args + len;

    } else {
        // generic iterator

        // allocate memory for the new array of args
        args2_alloc = 1 + n_args + 2 * (n_kw + kw_dict_len) + 3;
        args2 = m_new(mp_obj_t, args2_alloc);

        // copy the self
        if (self != MP_OBJ_NULL) {
            args2[args2_len++] = self;
        }

        // copy the fixed position args
        m_seq_copy(args2 + args2_len, args, n_args, mp_obj_t);

        // extract the variable position args from the iterator
        mp_obj_t iterable = mp_getiter(pos_seq);
        mp_obj_t item;
        while ((item = mp_iternext(iterable)) != MP_OBJ_NULL) {
            if (args2_len >= args2_alloc) {
                args2 = m_renew(mp_obj_t, args2, args2_alloc, args2_alloc * 2);
                args2_alloc *= 2;
            }
            args2[args2_len++] = item;
        }
    }

    // The size of the args2 array now is the number of positional args.
    uint pos_args_len = args2_len;

    // Copy the fixed kw args.
    m_seq_copy(args2 + args2_len, args + n_args, 2 * n_kw, mp_obj_t);
    args2_len += 2 * n_kw;

    // Extract (key,value) pairs from kw_dict dictionary and append to args2.
    // Note that it can be arbitrary iterator.
    if (kw_dict == MP_OBJ_NULL) {
        // pass
    } else if (MP_OBJ_IS_TYPE(kw_dict, &mp_type_dict)) {
        // dictionary
        mp_map_t *map = mp_obj_dict_get_map(kw_dict);
        assert(args2_len + 2 * map->used <= args2_alloc); // should have enough, since kw_dict_len is in this case hinted correctly above
        for (uint i = 0; i < map->alloc; i++) {
            if (map->table[i].key != MP_OBJ_NULL) {
                args2[args2_len++] = map->table[i].key;
                args2[args2_len++] = map->table[i].value;
            }
        }
    } else {
        // generic mapping
        // TODO is calling 'items' on the mapping the correct thing to do here?
        mp_obj_t dest[2];
        mp_load_method(kw_dict, MP_QSTR_items, dest);
        mp_obj_t iterable = mp_getiter(mp_call_method_n_kw(0, 0, dest));
        mp_obj_t item;
        while ((item = mp_iternext(iterable)) != MP_OBJ_NULL) {
            if (args2_len + 1 >= args2_alloc) {
                uint new_alloc = args2_alloc * 2;
                if (new_alloc < 4) {
                    new_alloc = 4;
                }
                args2 = m_renew(mp_obj_t, args2, args2_alloc, new_alloc);
                args2_alloc = new_alloc;
            }
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(item, 2, &items);
            args2[args2_len++] = items[0];
            args2[args2_len++] = items[1];
        }
    }

    mp_obj_t res = mp_call_function_n_kw(fun, pos_args_len, (args2_len - pos_args_len) / 2, args2);
    m_del(mp_obj_t, args2, args2_alloc);

    return res;
}

// unpacked items are stored in reverse order into the array pointed to by items
void mp_unpack_sequence(mp_obj_t seq_in, uint num, mp_obj_t *items) {
    uint seq_len;
    if (MP_OBJ_IS_TYPE(seq_in, &mp_type_tuple) || MP_OBJ_IS_TYPE(seq_in, &mp_type_list)) {
        mp_obj_t *seq_items;
        if (MP_OBJ_IS_TYPE(seq_in, &mp_type_tuple)) {
            mp_obj_tuple_get(seq_in, &seq_len, &seq_items);
        } else {
            mp_obj_list_get(seq_in, &seq_len, &seq_items);
        }
        if (seq_len < num) {
            goto too_short;
        } else if (seq_len > num) {
            goto too_long;
        }
        for (uint i = 0; i < num; i++) {
            items[i] = seq_items[num - 1 - i];
        }
    } else {
        mp_obj_t iterable = mp_getiter(seq_in);

        for (seq_len = 0; seq_len < num; seq_len++) {
            mp_obj_t el = mp_iternext(iterable);
            if (el == MP_OBJ_NULL) {
                goto too_short;
            }
            items[num - 1 - seq_len] = el;
        }
        if (mp_iternext(iterable) != MP_OBJ_NULL) {
            goto too_long;
        }
    }
    return;

too_short:
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "need more than %d values to unpack", seq_len));
too_long:
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "too many values to unpack (expected %d)", num));
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

// no attribute found, returns:     dest[0] == MP_OBJ_NULL, dest[1] == MP_OBJ_NULL
// normal attribute found, returns: dest[0] == <attribute>, dest[1] == MP_OBJ_NULL
// method attribute found, returns: dest[0] == <method>,    dest[1] == <self>
void mp_load_method_maybe(mp_obj_t base, qstr attr, mp_obj_t *dest) {
    // clear output to indicate no attribute/method found yet
    dest[0] = MP_OBJ_NULL;
    dest[1] = MP_OBJ_NULL;

    // get the type
    mp_obj_type_t *type = mp_obj_get_type(base);

    // look for built-in names
    if (0) {
#if MICROPY_CPYTHON_COMPAT
    } else if (attr == MP_QSTR___class__) {
        // a.__class__ is equivalent to type(a)
        dest[0] = type;
#endif

    } else if (attr == MP_QSTR___next__ && type->iternext != NULL) {
        dest[0] = (mp_obj_t)&mp_builtin_next_obj;
        dest[1] = base;

    } else if (type->load_attr != NULL) {
        // this type can do its own load, so call it
        type->load_attr(base, attr, dest);

    } else if (type->locals_dict != NULL) {
        // generic method lookup
        // this is a lookup in the object (ie not class or type)
        assert(MP_OBJ_IS_TYPE(type->locals_dict, &mp_type_dict)); // Micro Python restriction, for now
        mp_map_t *locals_map = mp_obj_dict_get_map(type->locals_dict);
        mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        if (elem != NULL) {
            // check if the methods are functions, static or class methods
            // see http://docs.python.org/3.3/howto/descriptor.html
            if (MP_OBJ_IS_TYPE(elem->value, &mp_type_staticmethod)) {
                // return just the function
                dest[0] = ((mp_obj_static_class_method_t*)elem->value)->fun;
            } else if (MP_OBJ_IS_TYPE(elem->value, &mp_type_classmethod)) {
                // return a bound method, with self being the type of this object
                dest[0] = ((mp_obj_static_class_method_t*)elem->value)->fun;
                dest[1] = mp_obj_get_type(base);
            } else if (mp_obj_is_callable(elem->value)) {
                // return a bound method, with self being this object
                dest[0] = elem->value;
                dest[1] = base;
            } else {
                // class member is a value, so just return that value
                dest[0] = elem->value;
            }
        }
    }
}

void mp_load_method(mp_obj_t base, qstr attr, mp_obj_t *dest) {
    DEBUG_OP_printf("load method %p.%s\n", base, qstr_str(attr));

    mp_load_method_maybe(base, attr, dest);

    if (dest[0] == MP_OBJ_NULL) {
        // no attribute/method called attr
        // following CPython, we give a more detailed error message for type objects
        if (MP_OBJ_IS_TYPE(base, &mp_type_type)) {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_AttributeError,
                "type object '%s' has no attribute '%s'", qstr_str(((mp_obj_type_t*)base)->name), qstr_str(attr)));
        } else {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_AttributeError, "'%s' object has no attribute '%s'", mp_obj_get_type_str(base), qstr_str(attr)));
        }
    }
}

void mp_store_attr(mp_obj_t base, qstr attr, mp_obj_t value) {
    DEBUG_OP_printf("store attr %p.%s <- %p\n", base, qstr_str(attr), value);
    mp_obj_type_t *type = mp_obj_get_type(base);
    if (type->store_attr != NULL) {
        if (type->store_attr(base, attr, value)) {
            return;
        }
    }
    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_AttributeError, "'%s' object has no attribute '%s'", mp_obj_get_type_str(base), qstr_str(attr)));
}

void mp_store_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t value) {
    DEBUG_OP_printf("store subscr %p[%p] <- %p\n", base, index, value);
    if (MP_OBJ_IS_TYPE(base, &mp_type_list)) {
        // list store
        mp_obj_list_store(base, index, value);
    } else if (MP_OBJ_IS_TYPE(base, &mp_type_dict)) {
        // dict store
        mp_obj_dict_store(base, index, value);
    } else {
        mp_obj_type_t *type = mp_obj_get_type(base);
        if (type->store_item != NULL) {
            bool r = type->store_item(base, index, value);
            if (r) {
                return;
            }
            // TODO: call base classes here?
        }
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object does not support item assignment", mp_obj_get_type_str(base)));
    }
}

mp_obj_t mp_getiter(mp_obj_t o_in) {
    mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (type->getiter != NULL) {
        return type->getiter(o_in);
    } else {
        // check for __iter__ method
        mp_obj_t dest[2];
        mp_load_method_maybe(o_in, MP_QSTR___iter__, dest);
        if (dest[0] != MP_OBJ_NULL) {
            // __iter__ exists, call it and return its result
            return mp_call_method_n_kw(0, 0, dest);
        } else {
            mp_load_method_maybe(o_in, MP_QSTR___getitem__, dest);
            if (dest[0] != MP_OBJ_NULL) {
                // __getitem__ exists, create an iterator
                return mp_obj_new_getitem_iter(dest);
            } else {
                // object not iterable
                nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object is not iterable", mp_obj_get_type_str(o_in)));
            }
        }
    }
}

// may return MP_OBJ_NULL as an optimisation instead of raise StopIteration()
// may also raise StopIteration()
mp_obj_t mp_iternext_allow_raise(mp_obj_t o_in) {
    mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (type->iternext != NULL) {
        return type->iternext(o_in);
    } else {
        // check for __next__ method
        mp_obj_t dest[2];
        mp_load_method_maybe(o_in, MP_QSTR___next__, dest);
        if (dest[0] != MP_OBJ_NULL) {
            // __next__ exists, call it and return its result
            return mp_call_method_n_kw(0, 0, dest);
        } else {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object is not an iterator", mp_obj_get_type_str(o_in)));
        }
    }
}

// will always return MP_OBJ_NULL instead of raising StopIteration() (or any subclass thereof)
// may raise other exceptions
mp_obj_t mp_iternext(mp_obj_t o_in) {
    mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (type->iternext != NULL) {
        return type->iternext(o_in);
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
                if (mp_obj_is_subclass_fast(mp_obj_get_type(nlr.ret_val), &mp_type_StopIteration)) {
                    return MP_OBJ_NULL;
                } else {
                    nlr_jump(nlr.ret_val);
                }
            }
        } else {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "'%s' object is not an iterator", mp_obj_get_type_str(o_in)));
        }
    }
}

// TODO: Unclear what to do with StopIterarion exception here.
mp_vm_return_kind_t mp_resume(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t *ret_val) {
    assert((send_value != MP_OBJ_NULL) ^ (throw_value != MP_OBJ_NULL));
    mp_obj_type_t *type = mp_obj_get_type(self_in);

    if (type == &mp_type_gen_instance) {
        return mp_obj_gen_resume(self_in, send_value, throw_value, ret_val);
    }

    if (type->iternext != NULL && send_value == mp_const_none) {
        mp_obj_t ret = type->iternext(self_in);
        if (ret != MP_OBJ_NULL) {
            *ret_val = ret;
            return MP_VM_RETURN_YIELD;
        } else {
            // Emulate raise StopIteration()
            // Special case, handled in vm.c
            *ret_val = MP_OBJ_NULL;
            return MP_VM_RETURN_NORMAL;
        }
    }

    mp_obj_t dest[3]; // Reserve slot for send() arg

    if (send_value == mp_const_none) {
        mp_load_method_maybe(self_in, MP_QSTR___next__, dest);
        if (dest[0] != MP_OBJ_NULL) {
            *ret_val = mp_call_method_n_kw(0, 0, dest);
            return MP_VM_RETURN_YIELD;
        }
    }

    if (send_value != MP_OBJ_NULL) {
        mp_load_method(self_in, MP_QSTR_send, dest);
        dest[2] = send_value;
        *ret_val = mp_call_method_n_kw(1, 0, dest);
        return MP_VM_RETURN_YIELD;
    }

    if (throw_value != MP_OBJ_NULL) {
        if (mp_obj_is_subclass_fast(mp_obj_get_type(throw_value), &mp_type_GeneratorExit)) {
            mp_load_method_maybe(self_in, MP_QSTR_close, dest);
            if (dest[0] != MP_OBJ_NULL) {
                *ret_val = mp_call_method_n_kw(0, 0, dest);
                // We assume one can't "yield" from close()
                return MP_VM_RETURN_NORMAL;
            }
        }
        mp_load_method_maybe(self_in, MP_QSTR_throw, dest);
        if (dest[0] != MP_OBJ_NULL) {
            *ret_val = mp_call_method_n_kw(1, 0, &throw_value);
            // If .throw() method returned, we assume it's value to yield
            // - any exception would be thrown with nlr_jump().
            return MP_VM_RETURN_YIELD;
        }
        // If there's nowhere to throw exception into, then we assume that object
        // is just incapable to handle it, so any exception thrown into it
        // will be propagated up. This behavior is approved by test_pep380.py
        // test_delegation_of_close_to_non_generator(),
        //  test_delegating_throw_to_non_generator()
        *ret_val = throw_value;
        return MP_VM_RETURN_EXCEPTION;
    }

    assert(0);
    return MP_VM_RETURN_NORMAL; // Should be unreachable
}

mp_obj_t mp_make_raise_obj(mp_obj_t o) {
    DEBUG_printf("raise %p\n", o);
    if (mp_obj_is_exception_type(o)) {
        // o is an exception type (it is derived from BaseException (or is BaseException))
        // create and return a new exception instance by calling o
        // TODO could have an option to disable traceback, then builtin exceptions (eg TypeError)
        // could have const instances in ROM which we return here instead
        return mp_call_function_n_kw(o, 0, 0, NULL);
    } else if (mp_obj_is_exception_instance(o)) {
        // o is an instance of an exception, so use it as the exception
        return o;
    } else {
        // o cannot be used as an exception, so return a type error (which will be raised by the caller)
        return mp_obj_new_exception_msg(&mp_type_TypeError, "exceptions must derive from BaseException");
    }
}

mp_obj_t mp_import_name(qstr name, mp_obj_t fromlist, mp_obj_t level) {
    DEBUG_printf("import name %s\n", qstr_str(name));

    // build args array
    mp_obj_t args[5];
    args[0] = MP_OBJ_NEW_QSTR(name);
    args[1] = mp_const_none; // TODO should be globals
    args[2] = mp_const_none; // TODO should be locals
    args[3] = fromlist;
    args[4] = level; // must be 0; we don't yet support other values

    // TODO lookup __import__ and call that instead of going straight to builtin implementation
    return mp_builtin___import__(5, args);
}

mp_obj_t mp_import_from(mp_obj_t module, qstr name) {
    DEBUG_printf("import from %p %s\n", module, qstr_str(name));

    mp_obj_t x = mp_load_attr(module, name);
    /* TODO convert AttributeError to ImportError
    if (fail) {
        (ImportError, "cannot import name %s", qstr_str(name), NULL)
    }
    */
    return x;
}

void mp_import_all(mp_obj_t module) {
    DEBUG_printf("import all %p\n", module);

    mp_map_t *map = mp_obj_module_get_globals(module);
    for (uint i = 0; i < map->alloc; i++) {
        if (map->table[i].key != MP_OBJ_NULL) {
            mp_store_name(MP_OBJ_QSTR_VALUE(map->table[i].key), map->table[i].value);
        }
    }
}

mp_map_t *mp_locals_get(void) {
    return map_locals;
}

void mp_locals_set(mp_map_t *m) {
    DEBUG_OP_printf("mp_locals_set(%p)\n", m);
    map_locals = m;
}

mp_map_t *mp_globals_get(void) {
    return map_globals;
}

void mp_globals_set(mp_map_t *m) {
    DEBUG_OP_printf("mp_globals_set(%p)\n", m);
    map_globals = m;
}

// these must correspond to the respective enum
void *const mp_fun_table[MP_F_NUMBER_OF] = {
    mp_load_const_dec,
    mp_load_const_str,
    mp_load_name,
    mp_load_global,
    mp_load_build_class,
    mp_load_attr,
    mp_load_method,
    mp_store_name,
    mp_store_attr,
    mp_store_subscr,
    mp_obj_is_true,
    mp_unary_op,
    mp_binary_op,
    mp_obj_new_tuple,
    mp_obj_new_list,
    mp_obj_list_append,
    mp_obj_new_dict,
    mp_obj_dict_store,
    mp_obj_new_set,
    mp_obj_set_store,
    mp_make_function_from_id,
    mp_call_function_n_kw_for_native,
    mp_call_method_n_kw,
    mp_getiter,
    mp_iternext,
};

/*
void mp_f_vector(mp_fun_kind_t fun_kind) {
    (mp_f_table[fun_kind])();
}
*/
