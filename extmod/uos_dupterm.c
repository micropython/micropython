/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
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

#include <string.h>
#include "py/mpconfig.h"

#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/objarray.h"
#include "py/stream.h"

#if MICROPY_PY_OS_DUPTERM

void mp_uos_deactivate(const char *msg, mp_obj_t exc) {
    mp_obj_t term = MP_STATE_PORT(term_obj);
    MP_STATE_PORT(term_obj) = NULL;
    mp_printf(&mp_plat_print, msg);
    if (exc != MP_OBJ_NULL) {
        mp_obj_print_exception(&mp_plat_print, exc);
    }
    mp_stream_close(term);
}

void mp_uos_dupterm_tx_strn(const char *str, size_t len) {
    if (MP_STATE_PORT(term_obj) != MP_OBJ_NULL) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t write_m[3];
            mp_load_method(MP_STATE_PORT(term_obj), MP_QSTR_write, write_m);

            mp_obj_array_t *arr = MP_OBJ_TO_PTR(MP_STATE_PORT(dupterm_arr_obj));
            void *org_items = arr->items;
            arr->items = (void*)str;
            arr->len = len;
            write_m[2] = MP_STATE_PORT(dupterm_arr_obj);
            mp_call_method_n_kw(1, 0, write_m);
            arr = MP_OBJ_TO_PTR(MP_STATE_PORT(dupterm_arr_obj));
            arr->items = org_items;
            arr->len = 1;
            nlr_pop();
        } else {
            mp_uos_deactivate("dupterm: Exception in write() method, deactivating: ", nlr.ret_val);
        }
    }
}

STATIC mp_obj_t mp_uos_dupterm(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        if (MP_STATE_PORT(term_obj) == MP_OBJ_NULL) {
            return mp_const_none;
        } else {
            return MP_STATE_PORT(term_obj);
        }
    } else {
        if (args[0] == mp_const_none) {
            MP_STATE_PORT(term_obj) = MP_OBJ_NULL;
        } else {
            MP_STATE_PORT(term_obj) = args[0];
            if (MP_STATE_PORT(dupterm_arr_obj) == MP_OBJ_NULL) {
                MP_STATE_PORT(dupterm_arr_obj) = mp_obj_new_bytearray(1, "");
            }
        }
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_uos_dupterm_obj, 0, 1, mp_uos_dupterm);

#endif
