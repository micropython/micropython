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

#include <assert.h>

#include "py/emit.h"

STATIC void emit_pass1_dummy(emit_t *emit) {
    (void)emit;
}

STATIC void emit_pass1_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    assert(pass == MP_PASS_SCOPE);
}

STATIC bool emit_pass1_last_emit_was_return_value(emit_t *emit) {
    (void)emit;
    return false;
}

const emit_method_table_t emit_pass1_method_table = {
    (void*)emit_pass1_dummy,
    emit_pass1_start_pass,
    emit_pass1_dummy,
    emit_pass1_last_emit_was_return_value,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,

    {
        (void*)emit_pass1_dummy,
        (void*)emit_pass1_dummy,
        (void*)emit_pass1_dummy,
        (void*)emit_pass1_dummy,
    },
    {
        (void*)emit_pass1_dummy,
        (void*)emit_pass1_dummy,
        (void*)emit_pass1_dummy,
        (void*)emit_pass1_dummy,
    },
    {
        (void*)emit_pass1_dummy,
        (void*)emit_pass1_dummy,
        (void*)emit_pass1_dummy,
        (void*)emit_pass1_dummy,
    },

    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    #if MICROPY_PY_BUILTINS_SET
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    #endif
    #if MICROPY_PY_BUILTINS_SLICE
    (void*)emit_pass1_dummy,
    #endif
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,

    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,

#if MICROPY_EMIT_CPYTHON
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
#endif
};
