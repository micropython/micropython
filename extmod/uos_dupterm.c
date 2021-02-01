/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2017-2019 Damien P. George
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
#include "extmod/misc.h"
#include "lib/utils/interrupt_char.h"

#if MICROPY_PY_OS_DUPTERM

void mp_uos_deactivate(size_t dupterm_idx, const char *msg, mp_obj_t exc) {
    mp_obj_t term = MP_STATE_VM(dupterm_objs[dupterm_idx]);
    MP_STATE_VM(dupterm_objs[dupterm_idx]) = MP_OBJ_NULL;
    mp_printf(&mp_plat_print, msg);
    if (exc != MP_OBJ_NULL) {
        mp_obj_print_exception(&mp_plat_print, exc);
    }
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_stream_close(term);
        nlr_pop();
    } else {
        // Ignore any errors during stream closing
    }
}

uintptr_t mp_uos_dupterm_poll(uintptr_t poll_flags) {
    uintptr_t poll_flags_out = 0;

    for (size_t idx = 0; idx < MICROPY_PY_OS_DUPTERM; ++idx) {
        mp_obj_t s = MP_STATE_VM(dupterm_objs[idx]);
        if (s == MP_OBJ_NULL) {
            continue;
        }

        int errcode = 0;
        mp_uint_t ret = 0;
        const mp_stream_p_t *stream_p = mp_get_stream(s);
        #if MICROPY_PY_UOS_DUPTERM_BUILTIN_STREAM
        if (mp_uos_dupterm_is_builtin_stream(s)) {
            ret = stream_p->ioctl(s, MP_STREAM_POLL, poll_flags, &errcode);
        } else
        #endif
        {
            nlr_buf_t nlr;
            if (nlr_push(&nlr) == 0) {
                ret = stream_p->ioctl(s, MP_STREAM_POLL, poll_flags, &errcode);
                nlr_pop();
            } else {
                // Ignore error with ioctl
            }
        }

        if (ret != MP_STREAM_ERROR) {
            poll_flags_out |= ret;
            if (poll_flags_out == poll_flags) {
                // Finish early if all requested flags are set
                break;
            }
        }
    }

    return poll_flags_out;
}

int mp_uos_dupterm_rx_chr(void) {
    for (size_t idx = 0; idx < MICROPY_PY_OS_DUPTERM; ++idx) {
        if (MP_STATE_VM(dupterm_objs[idx]) == MP_OBJ_NULL) {
            continue;
        }

        #if MICROPY_PY_UOS_DUPTERM_BUILTIN_STREAM
        if (mp_uos_dupterm_is_builtin_stream(MP_STATE_VM(dupterm_objs[idx]))) {
            byte buf[1];
            int errcode = 0;
            const mp_stream_p_t *stream_p = mp_get_stream(MP_STATE_VM(dupterm_objs[idx]));
            mp_uint_t out_sz = stream_p->read(MP_STATE_VM(dupterm_objs[idx]), buf, 1, &errcode);
            if (errcode == 0 && out_sz != 0) {
                return buf[0];
            } else {
                continue;
            }
        }
        #endif

        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            byte buf[1];
            int errcode;
            const mp_stream_p_t *stream_p = mp_get_stream(MP_STATE_VM(dupterm_objs[idx]));
            mp_uint_t out_sz = stream_p->read(MP_STATE_VM(dupterm_objs[idx]), buf, 1, &errcode);
            if (out_sz == 0) {
                nlr_pop();
                mp_uos_deactivate(idx, "dupterm: EOF received, deactivating\n", MP_OBJ_NULL);
            } else if (out_sz == MP_STREAM_ERROR) {
                // errcode is valid
                if (mp_is_nonblocking_error(errcode)) {
                    nlr_pop();
                } else {
                    mp_raise_OSError(errcode);
                }
            } else {
                // read 1 byte
                nlr_pop();
                if (buf[0] == mp_interrupt_char) {
                    // Signal keyboard interrupt to be raised as soon as the VM resumes
                    mp_keyboard_interrupt();
                    return -2;
                }
                return buf[0];
            }
        } else {
            mp_uos_deactivate(idx, "dupterm: Exception in read() method, deactivating: ", MP_OBJ_FROM_PTR(nlr.ret_val));
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

        #if MICROPY_PY_UOS_DUPTERM_BUILTIN_STREAM
        if (mp_uos_dupterm_is_builtin_stream(MP_STATE_VM(dupterm_objs[idx]))) {
            int errcode = 0;
            const mp_stream_p_t *stream_p = mp_get_stream(MP_STATE_VM(dupterm_objs[idx]));
            stream_p->write(MP_STATE_VM(dupterm_objs[idx]), str, len, &errcode);
            continue;
        }
        #endif

        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_stream_write(MP_STATE_VM(dupterm_objs[idx]), str, len, MP_STREAM_RW_WRITE);
            nlr_pop();
        } else {
            mp_uos_deactivate(idx, "dupterm: Exception in write() method, deactivating: ", MP_OBJ_FROM_PTR(nlr.ret_val));
        }
    }
}

STATIC mp_obj_t mp_uos_dupterm(size_t n_args, const mp_obj_t *args) {
    mp_int_t idx = 0;
    if (n_args == 2) {
        idx = mp_obj_get_int(args[1]);
    }

    if (idx < 0 || idx >= MICROPY_PY_OS_DUPTERM) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid dupterm index"));
    }

    mp_obj_t previous_obj = MP_STATE_VM(dupterm_objs[idx]);
    if (previous_obj == MP_OBJ_NULL) {
        previous_obj = mp_const_none;
    }
    if (args[0] == mp_const_none) {
        MP_STATE_VM(dupterm_objs[idx]) = MP_OBJ_NULL;
    } else {
        mp_get_stream_raise(args[0], MP_STREAM_OP_READ | MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);
        MP_STATE_VM(dupterm_objs[idx]) = args[0];
    }

    return previous_obj;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_uos_dupterm_obj, 1, 2, mp_uos_dupterm);

#endif
