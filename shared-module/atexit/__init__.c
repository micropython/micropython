/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-module/atexit/__init__.h"

static size_t callback_len = 0;
static atexit_callback_t *callback = NULL;

void atexit_reset(void) {
    callback_len = 0;
    m_free(callback);
    callback = NULL;
}

void atexit_gc_collect(void) {
    gc_collect_ptr(callback);
}

void shared_module_atexit_register(mp_obj_t *func, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    if (!mp_obj_is_callable(func)) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("'%q' object is not callable"), mp_obj_get_type_qstr(func));
    }
    size_t n_kw_args = (kw_args) ? kw_args->used : 0;
    atexit_callback_t cb = {
        .n_pos = 0,
        .n_kw = 0,
        .func = func,
        .args = (n_args + n_kw_args) ? m_malloc((n_args + (n_kw_args * 2)) * sizeof(mp_obj_t)) : NULL
    };
    for (; cb.n_pos < n_args; cb.n_pos++) {
        cb.args[cb.n_pos] = pos_args[cb.n_pos];
    }
    for (size_t i = cb.n_pos; cb.n_kw < n_kw_args; i++, cb.n_kw++) {
        cb.args[i] = kw_args->table[cb.n_kw].key;
        cb.args[i += 1] = kw_args->table[cb.n_kw].value;
    }
    callback = (atexit_callback_t *)m_realloc(callback, (callback_len + 1) * sizeof(cb));
    callback[callback_len++] = cb;
}

void shared_module_atexit_unregister(const mp_obj_t *func) {
    for (size_t i = 0; i < callback_len; i++) {
        if (callback[i].func == *func) {
            callback[i].n_pos = 0;
            callback[i].n_kw = 0;
            callback[i].func = mp_const_none;
            callback[i].args = NULL;
        }
    }
}

void shared_module_atexit_execute(pyexec_result_t *result) {
    if (callback) {
        for (size_t i = callback_len; i-- > 0;) {
            if (callback[i].func != mp_const_none) {
                if (result != NULL) {
                    pyexec_result_t res;
                    if (pyexec_exit_handler(&callback[i], &res) == PYEXEC_DEEP_SLEEP) {
                        *result = res;
                    }
                } else {
                    pyexec_exit_handler(&callback[i], NULL);
                }
            }
        }
    }
}
