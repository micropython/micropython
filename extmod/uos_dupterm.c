/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2017 Damien P. George
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
#include "lib/utils/interrupt_char.h"

#if MICROPY_PY_OS_DUPTERM

void mp_uos_deactivate(size_t dupterm_idx, const char *msg, mp_obj_t exc) {
    mp_obj_t term = MP_STATE_VM(dupterm_objs[dupterm_idx]);
    MP_STATE_VM(dupterm_objs[dupterm_idx]) = MP_OBJ_NULL;
    mp_printf(&mp_plat_print, msg);
    if (exc != MP_OBJ_NULL) {
        mp_obj_print_exception(&mp_plat_print, exc);
    }
    mp_stream_close(term);
}

int mp_uos_dupterm_rx_chr(void) {
    for (size_t idx = 0; idx < MICROPY_PY_OS_DUPTERM; ++idx) {
        if (MP_STATE_VM(dupterm_objs[idx]) == MP_OBJ_NULL) {
            continue;
        }

        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t readinto_m[3];
            mp_load_method(MP_STATE_VM(dupterm_objs[idx]), MP_QSTR_readinto, readinto_m);
            readinto_m[2] = MP_STATE_VM(dupterm_arr_obj);
            mp_obj_t res = mp_call_method_n_kw(1, 0, readinto_m);
            if (res == mp_const_none) {
                nlr_pop();
            } else if (res == MP_OBJ_NEW_SMALL_INT(0)) {
                mp_uos_deactivate(idx, "dupterm: EOF received, deactivating\n", MP_OBJ_NULL);
                nlr_pop();
            } else {
                mp_buffer_info_t bufinfo;
                mp_get_buffer_raise(MP_STATE_VM(dupterm_arr_obj), &bufinfo, MP_BUFFER_READ);
                nlr_pop();
                if (*(byte*)bufinfo.buf == mp_interrupt_char) {
                    // Signal keyboard interrupt to be raised as soon as the VM resumes
                    mp_keyboard_interrupt();
                    return -2;
                }
                return *(byte*)bufinfo.buf;
            }
        } else {
            mp_uos_deactivate(idx, "dupterm: Exception in read() method, deactivating: ", nlr.ret_val);
        }
    }

    // No chars available
    return -1;
}

void mp_uos_dupterm_tx_strn(const char *str, size_t len) {
    for (size_t idx = 0; idx < MICROPY_PY_OS_DUPTERM; ++idx) {
        if (MP_STATE_VM(dupterm_objs[idx]) == MP_OBJ_NULL) {
            continue;
        }
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t write_m[3];
            mp_load_method(MP_STATE_VM(dupterm_objs[idx]), MP_QSTR_write, write_m);

            mp_obj_array_t *arr = MP_OBJ_TO_PTR(MP_STATE_VM(dupterm_arr_obj));
            void *org_items = arr->items;
            arr->items = (void*)str;
            arr->len = len;
            write_m[2] = MP_STATE_VM(dupterm_arr_obj);
            mp_call_method_n_kw(1, 0, write_m);
            arr = MP_OBJ_TO_PTR(MP_STATE_VM(dupterm_arr_obj));
            arr->items = org_items;
            arr->len = 1;
            nlr_pop();
        } else {
            mp_uos_deactivate(idx, "dupterm: Exception in write() method, deactivating: ", nlr.ret_val);
        }
    }
}

STATIC mp_obj_t mp_uos_dupterm(size_t n_args, const mp_obj_t *args) {
    mp_int_t idx = 0;
    if (n_args == 2) {
        idx = mp_obj_get_int(args[1]);
    }

    if (idx < 0 || idx >= MICROPY_PY_OS_DUPTERM) {
        mp_raise_ValueError("invalid dupterm index");
    }

    mp_obj_t previous_obj = MP_STATE_VM(dupterm_objs[idx]);
    if (previous_obj == MP_OBJ_NULL) {
        previous_obj = mp_const_none;
    }
    if (args[0] == mp_const_none) {
        MP_STATE_VM(dupterm_objs[idx]) = MP_OBJ_NULL;
    } else {
        MP_STATE_VM(dupterm_objs[idx]) = args[0];
        if (MP_STATE_VM(dupterm_arr_obj) == MP_OBJ_NULL) {
            MP_STATE_VM(dupterm_arr_obj) = mp_obj_new_bytearray(1, "");
        }
    }

    return previous_obj;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_uos_dupterm_obj, 1, 2, mp_uos_dupterm);

#endif
