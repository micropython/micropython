/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#include "lib/utils/interrupt_char.h"
#include "py/obj.h"
#include "py/objtype.h"
#include "py/objint.h"
#include "py/objstr.h"
#include "py/qstr.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "py/stream.h" // for mp_obj_print

#include "supervisor/linker.h"
#include "supervisor/shared/stack.h"
#include "supervisor/shared/translate.h"

mp_obj_type_t *mp_obj_get_type(mp_const_obj_t o_in) {
    if (MP_OBJ_IS_SMALL_INT(o_in)) {
        return (mp_obj_type_t*)&mp_type_int;
    } else if (MP_OBJ_IS_QSTR(o_in)) {
        return (mp_obj_type_t*)&mp_type_str;
    #if MICROPY_PY_BUILTINS_FLOAT
    } else if (mp_obj_is_float(o_in)) {
        return (mp_obj_type_t*)&mp_type_float;
    #endif
    } else {
        const mp_obj_base_t *o = MP_OBJ_TO_PTR(o_in);
        return (mp_obj_type_t*)o->type;
    }
}

const char *mp_obj_get_type_str(mp_const_obj_t o_in) {
    return qstr_str(mp_obj_get_type_qstr(o_in));
}

void mp_obj_print_helper(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    // There can be data structures nested too deep, or just recursive
    MP_STACK_CHECK();
    #ifdef RUN_BACKGROUND_TASKS
    RUN_BACKGROUND_TASKS;
    #endif
    // Stop printing if we've been interrupted.
    if (mp_hal_is_interrupted()) {
        return;
    }

#ifndef NDEBUG
    if (o_in == MP_OBJ_NULL) {
        mp_print_str(print, "(nil)");
        return;
    }
#endif
    mp_obj_type_t *type = mp_obj_get_type(o_in);
    if (type->print != NULL) {
        type->print((mp_print_t*)print, o_in, kind);
    } else {
        mp_printf(print, "<%q>", type->name);
    }
}

void mp_obj_print(mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_print_helper(MP_PYTHON_PRINTER, o_in, kind);
}

// helper function to print an exception with traceback
void mp_obj_print_exception(const mp_print_t *print, mp_obj_t exc) {
    if (mp_obj_is_exception_instance(exc) && stack_ok()) {
        size_t n, *values;
        mp_obj_exception_get_traceback(exc, &n, &values);
        if (n > 0) {
            assert(n % 3 == 0);
            // Decompress the format strings
            const compressed_string_t* traceback = translate("Traceback (most recent call last):\n");
            char decompressed[decompress_length(traceback)];
            decompress(traceback, decompressed);
#if MICROPY_ENABLE_SOURCE_LINE
            const compressed_string_t* frame = translate("  File \"%q\", line %d");
#else
            const compressed_string_t* frame = translate("  File \"%q\"");
#endif
            char decompressed_frame[decompress_length(frame)];
            decompress(frame, decompressed_frame);
            const compressed_string_t* block_fmt = translate(", in %q\n");
            char decompressed_block[decompress_length(block_fmt)];
            decompress(block_fmt, decompressed_block);

            // Print the traceback
            mp_print_str(print, decompressed);
            for (int i = n - 3; i >= 0; i -= 3) {
#if MICROPY_ENABLE_SOURCE_LINE
                mp_printf(print, decompressed_frame, values[i], (int)values[i + 1]);
#else
                mp_printf(print, decompressed_frame, values[i]);
#endif
                // the block name can be NULL if it's unknown
                qstr block = values[i + 2];
                if (block == MP_QSTR_NULL) {
                    mp_print_str(print, "\n");
                } else {
                    mp_printf(print, decompressed_block, block);
                }
            }
        }
    }
    mp_obj_print_helper(print, exc, PRINT_EXC);
    mp_print_str(print, "\n");
}

bool PLACE_IN_ITCM(mp_obj_is_true)(mp_obj_t arg) {
    if (arg == mp_const_false) {
        return 0;
    } else if (arg == mp_const_true) {
        return 1;
    } else if (arg == mp_const_none) {
        return 0;
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        if (MP_OBJ_SMALL_INT_VALUE(arg) == 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        mp_obj_type_t *type = mp_obj_get_type(arg);
        if (type->unary_op != NULL) {
            mp_obj_t result = type->unary_op(MP_UNARY_OP_BOOL, arg);
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
    mp_call_fun_t call = mp_obj_get_type(o_in)->call;
    if (call != mp_obj_instance_call) {
        return call != NULL;
    }
    return mp_obj_instance_is_callable(o_in);
}

// This function implements the '==' operator (and so the inverse of '!=').
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
bool mp_obj_equal(mp_obj_t o1, mp_obj_t o2) {
    // Float (and complex) NaN is never equal to anything, not even itself,
    // so we must have a special check here to cover those cases.
    if (o1 == o2
        #if MICROPY_PY_BUILTINS_FLOAT
        && !mp_obj_is_float(o1)
        #endif
        #if MICROPY_PY_BUILTINS_COMPLEX
        && !MP_OBJ_IS_TYPE(o1, &mp_type_complex)
        #endif
        ) {
        return true;
    }
    if (o1 == mp_const_none || o2 == mp_const_none) {
        return false;
    }

    // fast path for small ints
    if (MP_OBJ_IS_SMALL_INT(o1)) {
        if (MP_OBJ_IS_SMALL_INT(o2)) {
            // both SMALL_INT, and not equal if we get here
            return false;
        } else {
            mp_obj_t temp = o2; o2 = o1; o1 = temp;
            // o2 is now the SMALL_INT, o1 is not
            // fall through to generic op
        }
    }

    // fast path for strings
    if (MP_OBJ_IS_STR(o1)) {
        if (MP_OBJ_IS_STR(o2)) {
            // both strings, use special function
            return mp_obj_str_equal(o1, o2);
        } else {
            // a string is never equal to anything else
            goto str_cmp_err;
        }
    } else if (MP_OBJ_IS_STR(o2)) {
        // o1 is not a string (else caught above), so the objects are not equal
    str_cmp_err:
        #if MICROPY_PY_STR_BYTES_CMP_WARN
        if (MP_OBJ_IS_TYPE(o1, &mp_type_bytes) || MP_OBJ_IS_TYPE(o2, &mp_type_bytes)) {
            mp_warning("Comparison between bytes and str");
        }
        #endif
        return false;
    }

    // generic type, call binary_op(MP_BINARY_OP_EQUAL)
    mp_obj_type_t *type = mp_obj_get_type(o1);
    if (type->binary_op != NULL) {
        mp_obj_t r = type->binary_op(MP_BINARY_OP_EQUAL, o1, o2);
        if (r != MP_OBJ_NULL) {
            return r == mp_const_true ? true : false;
        }
    }

    // equality not implemented, and objects are not the same object, so
    // they are defined as not equal
    return false;
}

mp_int_t mp_obj_get_int(mp_const_obj_t arg) {
    // This function essentially performs implicit type conversion to int
    // Note that Python does NOT provide implicit type conversion from
    // float to int in the core expression language, try some_list[1.0].
    if (arg == mp_const_false) {
        return 0;
    } else if (arg == mp_const_true) {
        return 1;
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        return MP_OBJ_SMALL_INT_VALUE(arg);
    } else if (MP_OBJ_IS_TYPE(arg, &mp_type_int)) {
        return mp_obj_int_get_checked(arg);
    } else {
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError_varg(translate("can't convert to %q"), MP_QSTR_int);
        #else
            mp_raise_TypeError_varg(
                translate("can't convert %q to %q"), mp_obj_get_type_qstr(arg), MP_QSTR_int);
        #endif
    }
}

mp_int_t mp_obj_get_int_truncated(mp_const_obj_t arg) {
    if (MP_OBJ_IS_INT(arg)) {
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
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        *value = MP_OBJ_SMALL_INT_VALUE(arg);
    } else if (MP_OBJ_IS_TYPE(arg, &mp_type_int)) {
        *value = mp_obj_int_get_checked(arg);
    } else {
        return false;
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
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        val = MP_OBJ_SMALL_INT_VALUE(arg);
    #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
    } else if (MP_OBJ_IS_TYPE(arg, &mp_type_int)) {
        val = mp_obj_int_as_float_impl(arg);
    #endif
    } else if (mp_obj_is_float(arg)) {
        val = mp_obj_float_get(arg);
    } else {
        return false;
    }

    *value = val;
    return true;
}

mp_float_t mp_obj_get_float(mp_obj_t arg) {
    mp_float_t val;

    if (!mp_obj_get_float_maybe(arg, &val)) {
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError_varg(translate("can't convert to %q"), MP_QSTR_float);
        #else
            mp_raise_TypeError_varg(
                translate("can't convert %q to %q"), mp_obj_get_type_qstr(arg), MP_QSTR_float);
        #endif
    }

    return val;
}

#if MICROPY_PY_BUILTINS_COMPLEX
void mp_obj_get_complex(mp_obj_t arg, mp_float_t *real, mp_float_t *imag) {
    if (arg == mp_const_false) {
        *real = 0;
        *imag = 0;
    } else if (arg == mp_const_true) {
        *real = 1;
        *imag = 0;
    } else if (MP_OBJ_IS_SMALL_INT(arg)) {
        *real = MP_OBJ_SMALL_INT_VALUE(arg);
        *imag = 0;
    #if MICROPY_LONGINT_IMPL != MICROPY_LONGINT_IMPL_NONE
    } else if (MP_OBJ_IS_TYPE(arg, &mp_type_int)) {
        *real = mp_obj_int_as_float_impl(arg);
        *imag = 0;
    #endif
    } else if (mp_obj_is_float(arg)) {
        *real = mp_obj_float_get(arg);
        *imag = 0;
    } else if (MP_OBJ_IS_TYPE(arg, &mp_type_complex)) {
        mp_obj_complex_get(arg, real, imag);
    } else {
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError_varg(translate("can't convert to %q"), MP_QSTR_complex);
        #else
            mp_raise_TypeError_varg(
                translate("can't convert %q to %q"), mp_obj_get_type_qstr(arg), MP_QSTR_complex);
        #endif
    }
}
#endif
#endif

// note: returned value in *items may point to the interior of a GC block
void mp_obj_get_array(mp_obj_t o, size_t *len, mp_obj_t **items) {
    if (MP_OBJ_IS_TYPE(o, &mp_type_tuple)) {
        mp_obj_tuple_get(o, len, items);
    } else if (MP_OBJ_IS_TYPE(o, &mp_type_list)) {
        mp_obj_list_get(o, len, items);
    } else {
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError(translate("expected tuple/list"));
        #else
            mp_raise_TypeError_varg(
                translate("object '%q' is not a tuple or list"), mp_obj_get_type_qstr(o));
        #endif
    }
}

// note: returned value in *items may point to the interior of a GC block
void mp_obj_get_array_fixed_n(mp_obj_t o, size_t len, mp_obj_t **items) {
    size_t seq_len;
    mp_obj_get_array(o, &seq_len, items);
    if (seq_len != len) {
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_ValueError(translate("tuple/list has wrong length"));
        #else
            mp_raise_ValueError_varg(translate("requested length %d but object has length %d"),
                (int)len, (int)seq_len);
        #endif
    }
}

// is_slice determines whether the index is a slice index
size_t mp_get_index(const mp_obj_type_t *type, size_t len, mp_obj_t index, bool is_slice) {
    mp_int_t i;
    if (MP_OBJ_IS_SMALL_INT(index)) {
        i = MP_OBJ_SMALL_INT_VALUE(index);
    } else if (!mp_obj_get_int_maybe(index, &i)) {
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError(translate("indices must be integers"));
        #else
            mp_raise_TypeError_varg(
                translate("%q indices must be integers, not %q"),
                type->name, mp_obj_get_type_qstr(index));
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
            #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
                mp_raise_IndexError(translate("index out of range"));
            #else
                mp_raise_msg_varg(&mp_type_IndexError,
                    translate("%q index out of range"), type->name);
            #endif
        }
    }

    // By this point 0 <= i <= len and so fits in a size_t
    return (size_t)i;
}

mp_obj_t mp_obj_id(mp_obj_t o_in) {
    mp_int_t id = (mp_int_t)o_in;
    if (!MP_OBJ_IS_OBJ(o_in)) {
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
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError(translate("object has no len"));
        #else
            mp_raise_TypeError_varg(
                translate("object of type '%q' has no len()"), mp_obj_get_type_qstr(o_in));
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
        MP_OBJ_IS_STR(o_in) ||
#endif
        MP_OBJ_IS_TYPE(o_in, &mp_type_bytes)) {
        GET_STR_LEN(o_in, l);
        return MP_OBJ_NEW_SMALL_INT(l);
    } else {
        mp_obj_type_t *type = mp_obj_get_type(o_in);
        if (type->unary_op != NULL) {
            return type->unary_op(MP_UNARY_OP_LEN, o_in);
        } else {
            return MP_OBJ_NULL;
        }
    }
}

mp_obj_t mp_obj_subscr(mp_obj_t base, mp_obj_t index, mp_obj_t value) {
    mp_obj_type_t *type = mp_obj_get_type(base);

    if (type->subscr != NULL) {
        mp_obj_t ret = type->subscr(base, index, value);
        // May have called port specific C code. Make sure it didn't mess up the heap.
        assert_heap_ok();
        if (ret != MP_OBJ_NULL) {
            return ret;
        }
    }
    if (value == MP_OBJ_NULL) {
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError(translate("object does not support item deletion"));
        #else
            mp_raise_TypeError_varg(
                translate("'%q' object does not support item deletion"), mp_obj_get_type_qstr(base));
        #endif
    } else if (value == MP_OBJ_SENTINEL) {
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError(translate("object is not subscriptable"));
        #else
            mp_raise_TypeError_varg(
                translate("'%q' object is not subscriptable"), mp_obj_get_type_qstr(base));
        #endif
    } else {
        #if MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE
            mp_raise_TypeError(translate("object does not support item assignment"));
        #else
            mp_raise_TypeError_varg(
                translate("'%q' object does not support item assignment"), mp_obj_get_type_qstr(base));
        #endif
    }
}

// Return input argument. Useful as .getiter for objects which are
// their own iterators, etc.
mp_obj_t mp_identity(mp_obj_t self) {
    return self;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_identity_obj, mp_identity);

mp_obj_t mp_identity_getiter(mp_obj_t self, mp_obj_iter_buf_t *iter_buf) {
    (void)iter_buf;
    return self;
}

typedef struct {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_t obj;
    mp_int_t cur;
} mp_obj_generic_it_t;

STATIC mp_obj_t generic_it_iternext(mp_obj_t self_in) {
    mp_obj_generic_it_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_type_t *type = mp_obj_get_type(self->obj);
    mp_obj_t current_length = type->unary_op(MP_UNARY_OP_LEN, self->obj);
    if (self->cur < MP_OBJ_SMALL_INT_VALUE(current_length)) {
        mp_obj_t o_out = type->subscr(self->obj, MP_OBJ_NEW_SMALL_INT(self->cur), MP_OBJ_SENTINEL);
        self->cur += 1;
        return o_out;
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

mp_obj_t mp_obj_new_generic_iterator(mp_obj_t obj, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(mp_obj_generic_it_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_generic_it_t *o = (mp_obj_generic_it_t*)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = generic_it_iternext;
    o->obj = obj;
    o->cur = 0;
    return MP_OBJ_FROM_PTR(o);
}

bool mp_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_type_t *type = mp_obj_get_type(obj);
    if (type->buffer_p.get_buffer == NULL) {
        return false;
    }
    int ret = type->buffer_p.get_buffer(obj, bufinfo, flags);
    if (ret != 0) {
        return false;
    }
    return true;
}

void mp_get_buffer_raise(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    if (!mp_get_buffer(obj, bufinfo, flags)) {
        mp_raise_TypeError(translate("object with buffer protocol required"));
    }
}

mp_obj_t mp_generic_unary_op(mp_unary_op_t op, mp_obj_t o_in) {
    switch (op) {
        case MP_UNARY_OP_HASH: return MP_OBJ_NEW_SMALL_INT((mp_uint_t)o_in);
        default: return MP_OBJ_NULL; // op not supported
    }
}
