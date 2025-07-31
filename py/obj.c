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

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "py/obj.h"
#include "py/objtype.h"
#include "py/objint.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/cstack.h"
#include "py/stream.h" // for mp_obj_print

// Allocates an object and also sets type, for mp_obj_malloc{,_var} macros.
MP_NOINLINE void *mp_obj_malloc_helper(size_t num_bytes, const mp_obj_type_t *type) {
    mp_obj_base_t *base = (mp_obj_base_t *)m_malloc(num_bytes);
    base->type = type;
    return base;
}

#if MICROPY_ENABLE_FINALISER
// Allocates an object and also sets type, for mp_obj_malloc{,_var}_with_finaliser macros.
MP_NOINLINE void *mp_obj_malloc_with_finaliser_helper(size_t num_bytes, const mp_obj_type_t *type) {
    mp_obj_base_t *base = (mp_obj_base_t *)m_malloc_with_finaliser(num_bytes);
    base->type = type;
    return base;
}
#endif

const mp_obj_type_t *MICROPY_WRAP_MP_OBJ_GET_TYPE(mp_obj_get_type)(mp_const_obj_t o_in) {
    #if MICROPY_OBJ_IMMEDIATE_OBJS && MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A

    if (mp_obj_is_obj(o_in)) {
        const mp_obj_base_t *o = MP_OBJ_TO_PTR(o_in);
        return o->type;
    } else {
        static const mp_obj_type_t *const types[] = {
            NULL, &mp_type_int, &mp_type_str, &mp_type_int,
            NULL, &mp_type_int, &mp_type_NoneType, &mp_type_int,
            NULL, &mp_type_int, &mp_type_str, &mp_type_int,
            NULL, &mp_type_int, &mp_type_bool, &mp_type_int,
        };
        return types[(uintptr_t)o_in & 0xf];
    }

    #elif MICROPY_OBJ_IMMEDIATE_OBJS && MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C

    if (mp_obj_is_small_int(o_in)) {
        return &mp_type_int;
    } else if (mp_obj_is_obj(o_in)) {
        const mp_obj_base_t *o = MP_OBJ_TO_PTR(o_in);
        return o->type;
    #if MICROPY_PY_BUILTINS_FLOAT
    } else if ((((mp_uint_t)(o_in)) & 0xff800007) != 0x00000006) {
        return &mp_type_float;
    #endif
    } else {
        static const mp_obj_type_t *const types[] = {
            &mp_type_str, &mp_type_NoneType, &mp_type_str, &mp_type_bool,
        };
        return types[((uintptr_t)o_in >> 3) & 3];
    }

    #else

    if (mp_obj_is_small_int(o_in)) {
        return &mp_type_int;
    } else if (mp_obj_is_qstr(o_in)) {
        return &mp_type_str;
        #if MICROPY_PY_BUILTINS_FLOAT && ( \
            MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D)
    } else if (mp_obj_is_float(o_in)) {
        return &mp_type_float;
        #endif
    #if MICROPY_OBJ_IMMEDIATE_OBJS
    } else if (mp_obj_is_immediate_obj(o_in)) {
        static const mp_obj_type_t *const types[2] = {&mp_type_NoneType, &mp_type_bool};
        return types[MP_OBJ_IMMEDIATE_OBJ_VALUE(o_in) & 1];
    #endif
    } else {
        const mp_obj_base_t *o = MP_OBJ_TO_PTR(o_in);
        return o->type;
    }

    #endif
}

const char *mp_obj_get_type_str(mp_const_obj_t o_in) {
    return qstr_str(mp_obj_get_type(o_in)->name);
}

void mp_obj_print_helper(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    // There can be data structures nested too deep, or just recursive
    mp_cstack_check();
    #ifndef NDEBUG
    if (o_in == MP_OBJ_NULL) {
        mp_print_str(print, "(nil)");
        return;
    }
    #endif
    const mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (MP_OBJ_TYPE_HAS_SLOT(type, print)) {
        MP_OBJ_TYPE_GET_SLOT(type, print)((mp_print_t *)print, o_in, kind);
    } else {
        mp_printf(print, "<%q>", type->name);
    }
}

void mp_obj_print(mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_print_helper(MP_PYTHON_PRINTER, o_in, kind);
}

// helper function to print an exception with traceback
void mp_obj_print_exception(const mp_print_t *print, mp_obj_t exc) {
    if (mp_obj_is_exception_instance(exc)) {
        size_t n, *values;
        mp_obj_exception_get_traceback(exc, &n, &values);
        if (n > 0) {
            assert(n % 3 == 0);
            mp_print_str(print, "Traceback (most recent call last):\n");
            for (int i = n - 3; i >= 0; i -= 3) {
                #if MICROPY_ENABLE_SOURCE_LINE
                mp_printf(print, "  File \"%q\", line %d", values[i], (int)values[i + 1]);
                #else
                mp_printf(print, "  File \"%q\"", values[i]);
                #endif
                // the block name can be NULL if it's unknown
                qstr block = values[i + 2];
                if (block == MP_QSTRnull) {
                    mp_print_str(print, "\n");
                } else {
                    mp_printf(print, ", in %q\n", block);
                }
            }
        }
    }
    mp_obj_print_helper(print, exc, PRINT_EXC);
    mp_print_str(print, "\n");
}

bool mp_obj_is_true(mp_obj_t arg) {
    if (arg == mp_const_false) {
        return 0;
    } else if (arg == mp_const_true) {
        return 1;
    } else if (arg == mp_const_none) {
        return 0;
    } else if (mp_obj_is_small_int(arg)) {
        if (arg == MP_OBJ_NEW_SMALL_INT(0)) {
            return 0;
        } else {
            return 1;
        }
    } else {
        const mp_obj_type_t *type = mp_obj_get_type(arg);
        if (MP_OBJ_TYPE_HAS_SLOT(type, unary_op)) {
            mp_obj_t result = MP_OBJ_TYPE_GET_SLOT(type, unary_op)(MP_UNARY_OP_BOOL, arg);
            if (result != MP_OBJ_NULL) {
                return result == mp_const_true;
            }
        }

        mp_obj_t len = mp_obj_len_maybe(arg);
        if (len != MP_OBJ_NULL) {
            // obj has a length, truth determined if len != 0
            return len != MP_OBJ_NEW_SMALL_INT(0);
        } else {
            // any other obj is true per Python semantics
            return 1;
        }
    }
}

bool mp_obj_is_callable(mp_obj_t o_in) {
    const mp_call_fun_t call = MP_OBJ_TYPE_GET_SLOT_OR_NULL(mp_obj_get_type(o_in), call);
    if (call != mp_obj_instance_call) {
        return call != NULL;
    }
    return mp_obj_instance_is_callable(o_in);
}

// This function implements the '==' and '!=' operators.
//
// From the Python language reference:
// (https://docs.python.org/3/reference/expressions.html#not-in)
// "The objects need not have the same type. If both are numbers, they are converted
// to a common type. Otherwise, the == and != operators always consider objects of
// different types to be unequal."
//
// This means that False==0 and True==1 are true expressions.
//
// Furthermore, from the v3.4.2 code for object.c: "Practical amendments: If rich
// comparison returns NotImplemented, == and != are decided by comparing the object
// pointer."
mp_obj_t mp_obj_equal_not_equal(mp_binary_op_t op, mp_obj_t o1, mp_obj_t o2) {
    mp_obj_t local_true = (op == MP_BINARY_OP_NOT_EQUAL) ? mp_const_false : mp_const_true;
    mp_obj_t local_false = (op == MP_BINARY_OP_NOT_EQUAL) ? mp_const_true : mp_const_false;
    int pass_number = 0;

    // Shortcut for very common cases
    if (o1 == o2 &&
        (mp_obj_is_small_int(o1) || !(mp_obj_get_type(o1)->flags & MP_TYPE_FLAG_EQ_NOT_REFLEXIVE))) {
        return local_true;
    }

    // fast path for strings
    if (mp_obj_is_str(o1)) {
        if (mp_obj_is_str(o2)) {
            // both strings, use special function
            return mp_obj_str_equal(o1, o2) ? local_true : local_false;
        #if MICROPY_PY_STR_BYTES_CMP_WARN
        } else if (mp_obj_is_type(o2, &mp_type_bytes)) {
        str_bytes_cmp:
            mp_warning(MP_WARN_CAT(BytesWarning), "Comparison between bytes and str");
            return local_false;
        #endif
        } else {
            goto skip_one_pass;
        }
    #if MICROPY_PY_STR_BYTES_CMP_WARN
    } else if (mp_obj_is_str(o2) && mp_obj_is_type(o1, &mp_type_bytes)) {
        // o1 is not a string (else caught above), so the objects are not equal
        goto str_bytes_cmp;
    #endif
    }

    // fast path for small ints
    if (mp_obj_is_small_int(o1)) {
        if (mp_obj_is_small_int(o2)) {
            // both SMALL_INT, and not equal if we get here
            return local_false;
        } else {
            goto skip_one_pass;
        }
    }

    // generic type, call binary_op(MP_BINARY_OP_EQUAL)
    while (pass_number < 2) {
        const mp_obj_type_t *type = mp_obj_get_type(o1);
        // If a full equality test is not needed and the other object is a different
        // type then we don't need to bother trying the comparison.
        if (MP_OBJ_TYPE_HAS_SLOT(type, binary_op) &&
            ((type->flags & MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE) || mp_obj_get_type(o2) == type)) {
            // CPython is asymmetric: it will try __eq__ if there's no __ne__ but not the
            // other way around.  If the class doesn't need a full test we can skip __ne__.
            if (op == MP_BINARY_OP_NOT_EQUAL && (type->flags & MP_TYPE_FLAG_EQ_HAS_NEQ_TEST)) {
                mp_obj_t r = MP_OBJ_TYPE_GET_SLOT(type, binary_op)(MP_BINARY_OP_NOT_EQUAL, o1, o2);
                if (r != MP_OBJ_NULL) {
                    return r;
                }
            }

            // Try calling __eq__.
            mp_obj_t r = MP_OBJ_TYPE_GET_SLOT(type, binary_op)(MP_BINARY_OP_EQUAL, o1, o2);
            if (r != MP_OBJ_NULL) {
                if (op == MP_BINARY_OP_EQUAL) {
                    return r;
                } else {
                    return mp_obj_is_true(r) ? local_true : local_false;
                }
            }
        }

    skip_one_pass:
        // Try the other way around if none of the above worked
        ++pass_number;
        mp_obj_t temp = o1;
        o1 = o2;
        o2 = temp;
    }

    // equality not implemented, so fall back to pointer comparison
    return (o1 == o2) ? local_true : local_false;
}

bool mp_obj_equal(mp_obj_t o1, mp_obj_t o2) {
    return mp_obj_is_true(mp_obj_equal_not_equal(MP_BINARY_OP_EQUAL, o1, o2));
}

mp_int_t mp_obj_get_int(mp_const_obj_t arg) {
    // This function essentially performs implicit type conversion to int
    // Note that Python does NOT provide implicit type conversion from
    // float to int in the core expression language, try some_list[1.0].
    mp_int_t val;
    if (!mp_obj_get_int_maybe(arg, &val)) {
        mp_raise_TypeError_int_conversion(arg);
    }
    return val;
}

mp_int_t mp_obj_get_int_truncated(mp_const_obj_t arg) {
    if (mp_obj_is_int(arg)) {
        return mp_obj_int_get_truncated(arg);
    } else {
        return mp_obj_get_int(arg);
    }
}

// returns false if arg is not of integral type
// returns true and sets *value if it is of integral type
// can throw OverflowError if arg is of integral type, but doesn't fit in a mp_int_t
bool mp_obj_get_int_maybe(mp_const_obj_t arg, mp_int_t *value) {
    if (arg == mp_const_false) {
        *value = 0;
    } else if (arg == mp_const_true) {
        *value = 1;
    } else if (mp_obj_is_small_int(arg)) {
        *value = MP_OBJ_SMALL_INT_VALUE(arg);
    } else if (mp_obj_is_exact_type(arg, &mp_type_int)) {
        *value = mp_obj_int_get_checked(arg);
    } else {
        arg = mp_unary_op(MP_UNARY_OP_INT_MAYBE, (mp_obj_t)arg);
        if (arg != MP_OBJ_NULL) {
            *value = mp_obj_int_get_checked(arg);
        } else {
            return false;
        }
    }
    return true;
}

#if MICROPY_PY_BUILTINS_FLOAT
bool mp_obj_get_float_maybe(mp_obj_t arg, mp_float_t *value) {
    mp_float_t val;

    if (arg == mp_const_false) {
        val = 0;
    } else if (arg == mp_const_true) {
        val = 1;
    } else if (mp_obj_is_small_int(arg)) {
        val = (mp_float_t)MP_OBJ_SMALL_INT_VALUE(arg);
    #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
    } else if (mp_obj_is_exact_type(arg, &mp_type_int)) {
        val = mp_obj_int_as_float_impl(arg);
    #endif
    } else if (mp_obj_is_float(arg)) {
        val = mp_obj_float_get(arg);
    } else {
        arg = mp_unary_op(MP_UNARY_OP_FLOAT_MAYBE, (mp_obj_t)arg);
        if (arg != MP_OBJ_NULL && mp_obj_is_float(arg)) {
            val = mp_obj_float_get(arg);
        } else {
            return false;
        }
    }
    *value = val;
    return true;
}

mp_float_t mp_obj_get_float(mp_obj_t arg) {
    mp_float_t val;

    if (!mp_obj_get_float_maybe(arg, &val)) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_TypeError(MP_ERROR_TEXT("can't convert to float"));
        #else
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("can't convert %s to float"), mp_obj_get_type_str(arg));
        #endif
    }

    return val;
}

#if MICROPY_PY_BUILTINS_COMPLEX
bool mp_obj_get_complex_maybe(mp_obj_t arg, mp_float_t *real, mp_float_t *imag) {
    if (mp_obj_get_float_maybe(arg, real)) {
        *imag = 0;
    } else if (mp_obj_is_type(arg, &mp_type_complex)) {
        mp_obj_complex_get(arg, real, imag);
    } else {
        arg = mp_unary_op(MP_UNARY_OP_COMPLEX_MAYBE, (mp_obj_t)arg);
        if (arg != MP_OBJ_NULL && mp_obj_is_type(arg, &mp_type_complex)) {
            mp_obj_complex_get(arg, real, imag);
        } else {
            return false;
        }
    }
    return true;
}

void mp_obj_get_complex(mp_obj_t arg, mp_float_t *real, mp_float_t *imag) {
    if (!mp_obj_get_complex_maybe(arg, real, imag)) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_TypeError(MP_ERROR_TEXT("can't convert to complex"));
        #else
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("can't convert %s to complex"), mp_obj_get_type_str(arg));
        #endif
    }
}
#endif
#endif

// note: returned value in *items may point to the interior of a GC block
void mp_obj_get_array(mp_obj_t o, size_t *len, mp_obj_t **items) {
    if (mp_obj_is_type(o, &mp_type_tuple)) {
        mp_obj_tuple_get(o, len, items);
    } else if (mp_obj_is_type(o, &mp_type_list)) {
        mp_obj_list_get(o, len, items);
    } else {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_TypeError(MP_ERROR_TEXT("expected tuple/list"));
        #else
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("object '%s' isn't a tuple or list"), mp_obj_get_type_str(o));
        #endif
    }
}

// note: returned value in *items may point to the interior of a GC block
void mp_obj_get_array_fixed_n(mp_obj_t o, size_t len, mp_obj_t **items) {
    size_t seq_len;
    mp_obj_get_array(o, &seq_len, items);
    if (seq_len != len) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_ValueError(MP_ERROR_TEXT("tuple/list has wrong length"));
        #else
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("requested length %d but object has length %d"), (int)len, (int)seq_len);
        #endif
    }
}

// is_slice determines whether the index is a slice index
size_t mp_get_index(const mp_obj_type_t *type, size_t len, mp_obj_t index, bool is_slice) {
    mp_int_t i;
    if (mp_obj_is_small_int(index)) {
        i = MP_OBJ_SMALL_INT_VALUE(index);
    } else if (!mp_obj_get_int_maybe(index, &i)) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_TypeError(MP_ERROR_TEXT("indices must be integers"));
        #else
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("%q indices must be integers, not %s"),
            type->name, mp_obj_get_type_str(index));
        #endif
    }

    if (i < 0) {
        i += len;
    }
    if (is_slice) {
        if (i < 0) {
            i = 0;
        } else if ((mp_uint_t)i > len) {
            i = len;
        }
    } else {
        if (i < 0 || (mp_uint_t)i >= len) {
            #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
            mp_raise_msg(&mp_type_IndexError, MP_ERROR_TEXT("index out of range"));
            #else
            mp_raise_msg_varg(&mp_type_IndexError, MP_ERROR_TEXT("%q index out of range"), type->name);
            #endif
        }
    }

    // By this point 0 <= i <= len and so fits in a size_t
    return (size_t)i;
}

mp_obj_t mp_obj_id(mp_obj_t o_in) {
    mp_int_t id = (mp_int_t)o_in;
    if (!mp_obj_is_obj(o_in)) {
        return mp_obj_new_int(id);
    } else if (id >= 0) {
        // Many OSes and CPUs have affinity for putting "user" memories
        // into low half of address space, and "system" into upper half.
        // We're going to take advantage of that and return small int
        // (signed) for such "user" addresses.
        return MP_OBJ_NEW_SMALL_INT(id);
    } else {
        // If that didn't work, well, let's return long int, just as
        // a (big) positive value, so it will never clash with the range
        // of small int returned in previous case.
        return mp_obj_new_int_from_uint((mp_uint_t)id);
    }
}

// will raise a TypeError if object has no length
mp_obj_t mp_obj_len(mp_obj_t o_in) {
    mp_obj_t len = mp_obj_len_maybe(o_in);
    if (len == MP_OBJ_NULL) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_TypeError(MP_ERROR_TEXT("object has no len"));
        #else
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("object of type '%s' has no len()"), mp_obj_get_type_str(o_in));
        #endif
    } else {
        return len;
    }
}

// may return MP_OBJ_NULL
mp_obj_t mp_obj_len_maybe(mp_obj_t o_in) {
    if (
        #if !MICROPY_PY_BUILTINS_STR_UNICODE
        // It's simple - unicode is slow, non-unicode is fast
        mp_obj_is_str(o_in) ||
        #endif
        mp_obj_is_type(o_in, &mp_type_bytes)) {
        GET_STR_LEN(o_in, l);
        return MP_OBJ_NEW_SMALL_INT(l);
    } else {
        const mp_obj_type_t *type = mp_obj_get_type(o_in);
        if (MP_OBJ_TYPE_HAS_SLOT(type, unary_op)) {
            return MP_OBJ_TYPE_GET_SLOT(type, unary_op)(MP_UNARY_OP_LEN, o_in);
        } else {
            return MP_OBJ_NULL;
        }
    }
}

mp_obj_t mp_obj_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t value) {
    const mp_obj_type_t *type = mp_obj_get_type(base);
    if (MP_OBJ_TYPE_HAS_SLOT(type, subscr)) {
        mp_obj_t ret = MP_OBJ_TYPE_GET_SLOT(type, subscr)(base, index, value);
        if (ret != MP_OBJ_NULL) {
            return ret;
        }
        // TODO: call base classes here?
    }
    if (value == MP_OBJ_NULL) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_TypeError(MP_ERROR_TEXT("object doesn't support item deletion"));
        #else
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("'%s' object doesn't support item deletion"), mp_obj_get_type_str(base));
        #endif
    } else if (value == MP_OBJ_SENTINEL) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_TypeError(MP_ERROR_TEXT("object isn't subscriptable"));
        #else
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("'%s' object isn't subscriptable"), mp_obj_get_type_str(base));
        #endif
    } else {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_TypeError(MP_ERROR_TEXT("object doesn't support item assignment"));
        #else
        mp_raise_msg_varg(&mp_type_TypeError,
            MP_ERROR_TEXT("'%s' object doesn't support item assignment"), mp_obj_get_type_str(base));
        #endif
    }
}

// Return input argument. Useful as .getiter for objects which are
// their own iterators, etc.
mp_obj_t mp_identity(mp_obj_t self) {
    return self;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_identity_obj, mp_identity);

// mp_obj_t mp_identity_getiter(mp_obj_t self, mp_obj_iter_buf_t *iter_buf) {
//     (void)iter_buf;
//     return self;
// }

bool mp_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    const mp_obj_type_t *type = mp_obj_get_type(obj);
    if (MP_OBJ_TYPE_HAS_SLOT(type, buffer)
        && MP_OBJ_TYPE_GET_SLOT(type, buffer)(obj, bufinfo, flags & MP_BUFFER_RW) == 0) {
        return true;
    }
    if (flags & MP_BUFFER_RAISE_IF_UNSUPPORTED) {
        mp_raise_TypeError(MP_ERROR_TEXT("object with buffer protocol required"));
    }
    return false;
}
