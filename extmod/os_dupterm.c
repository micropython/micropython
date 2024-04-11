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

#if MICROPY_PY_OS_DUPTERM

#include "shared/runtime/interrupt_char.h"

void mp_os_deactivate(size_t dupterm_idx, const char *msg, mp_obj_t exc) {
    mp_obj_t term = MP_STATE_VM(dupterm_objs[dupterm_idx]);
    MP_STATE_VM(dupterm_objs[dupterm_idx]) = MP_OBJ_NULL;
    mp_printf(&mp_plat_print, msg);
    if (exc != MP_OBJ_NULL) {
        mp_obj_print_exception(&mp_plat_print, exc);
    }
    if (term == MP_OBJ_NULL) {
        // Dupterm was already closed.
        return;
    }
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_stream_close(term);
        nlr_pop();
    } else {
        // Ignore any errors during stream closing
    }
}

uintptr_t mp_os_dupterm_poll(uintptr_t poll_flags) {
    uintptr_t poll_flags_out = 0;

    for (size_t idx = 0; idx < MICROPY_PY_OS_DUPTERM; ++idx) {
        mp_obj_t s = MP_STATE_VM(dupterm_objs[idx]);
        if (s == MP_OBJ_NULL) {
            continue;
        }

        int errcode = 0;
        mp_uint_t ret = 0;
        const mp_stream_p_t *stream_p = mp_get_stream(s);
        #if MICROPY_PY_OS_DUPTERM_BUILTIN_STREAM
        if (mp_os_dupterm_is_builtin_stream(s)) {
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

int mp_os_dupterm_rx_chr(void) {
    #if MICROPY_PY_OS_DUPTERM_NOTIFY
    // When os.dupterm_notify() is enabled it is usually called from a scheduled
    // function, via mp_os_dupterm_notify().  That can lead to recursive calls of
    // this function when this function is called from mp_hal_stdin_rx_chr():
    // - mp_hal_stdin_rx_chr()
    //  - mp_os_dupterm_rx_chr()
    //   - <Python code>
    //    - os.dupterm_notify() is scheduled via interrupt/event
    //     - <Python code> runs scheduled code (eg WebREPL -> rp2.Flash().writeblocks())
    //      - mp_os_dupterm_notify()
    //       - mp_os_dupterm_rx_chr()
    // Break that cycle by locking the scheduler during this function's duration.
    mp_sched_lock();
    #endif

    int ret = -1; // no chars available
    for (size_t idx = 0; idx < MICROPY_PY_OS_DUPTERM; ++idx) {
        if (MP_STATE_VM(dupterm_objs[idx]) == MP_OBJ_NULL) {
            continue;
        }

        #if MICROPY_PY_OS_DUPTERM_BUILTIN_STREAM
        if (mp_os_dupterm_is_builtin_stream(MP_STATE_VM(dupterm_objs[idx]))) {
            byte buf[1];
            int errcode = 0;
            const mp_stream_p_t *stream_p = mp_get_stream(MP_STATE_VM(dupterm_objs[idx]));
            mp_uint_t out_sz = stream_p->read(MP_STATE_VM(dupterm_objs[idx]), buf, 1, &errcode);
            if (errcode == 0 && out_sz != 0) {
                ret = buf[0];
                break;
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
                mp_os_deactivate(idx, "dupterm: EOF received, deactivating\n", MP_OBJ_NULL);
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
                ret = buf[0];
                if (ret == mp_interrupt_char) {
                    // Signal keyboard interrupt to be raised as soon as the VM resumes
                    mp_sched_keyboard_interrupt();
                    ret = -2;
                }
                break;
            }
        } else {
            mp_os_deactivate(idx, "dupterm: Exception in read() method, deactivating: ", MP_OBJ_FROM_PTR(nlr.ret_val));
        }
    }

    #if MICROPY_PY_OS_DUPTERM_NOTIFY
    mp_sched_unlock();
    #endif

    return ret;
}

int mp_os_dupterm_tx_strn(const char *str, size_t len) {
    // Returns the minimum successful write length, or -1 if no write is attempted.
    int ret = len;
    bool did_write = false;
    for (size_t idx = 0; idx < MICROPY_PY_OS_DUPTERM; ++idx) {
        if (MP_STATE_VM(dupterm_objs[idx]) == MP_OBJ_NULL) {
            continue;
        }
        did_write = true;

        #if MICROPY_PY_OS_DUPTERM_BUILTIN_STREAM
        if (mp_os_dupterm_is_builtin_stream(MP_STATE_VM(dupterm_objs[idx]))) {
            int errcode = 0;
            const mp_stream_p_t *stream_p = mp_get_stream(MP_STATE_VM(dupterm_objs[idx]));
            mp_uint_t written = stream_p->write(MP_STATE_VM(dupterm_objs[idx]), str, len, &errcode);
            int write_res = MAX(0, written);
            ret = MIN(write_res, ret);
            continue;
        }
        #endif

        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t written = mp_stream_write(MP_STATE_VM(dupterm_objs[idx]), str, len, MP_STREAM_RW_WRITE);
            if (written == mp_const_none) {
                ret = 0;
            } else if (mp_obj_is_small_int(written)) {
                int written_int = MAX(0, MP_OBJ_SMALL_INT_VALUE(written));
                ret = MIN(written_int, ret);
            }
            nlr_pop();
        } else {
            mp_os_deactivate(idx, "dupterm: Exception in write() method, deactivating: ", MP_OBJ_FROM_PTR(nlr.ret_val));
            ret = 0;
        }
    }
    return did_write ? ret : -1;
}

static mp_obj_t mp_os_dupterm(size_t n_args, const mp_obj_t *args) {
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

    #if MICROPY_PY_OS_DUPTERM_STREAM_DETACHED_ATTACHED
    mp_os_dupterm_stream_detached_attached(previous_obj, args[0]);
    #endif

    return previous_obj;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_os_dupterm_obj, 1, 2, mp_os_dupterm);

MP_REGISTER_ROOT_POINTER(mp_obj_t dupterm_objs[MICROPY_PY_OS_DUPTERM]);

#endif // MICROPY_PY_OS_DUPTERM
