/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

static struct k_poll_signal wait_signal;

void mp_hal_init(void) {
    k_poll_signal_init(&wait_signal);
}

void mp_hal_signal_event(void) {
    k_poll_signal_raise(&wait_signal, 0);
}

void mp_hal_delay_ms(mp_uint_t ms) {
    uint64_t dt;
    uint32_t t0 = mp_hal_ticks_ms();
    for (;;) {
        mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS);
        MP_THREAD_GIL_EXIT();
        uint32_t t1 = mp_hal_ticks_ms();
        dt = t1 - t0;
        if (dt + 1 >= ms) {
            // doing a k_msleep would take us beyond requested delay time
            k_yield();
            MP_THREAD_GIL_ENTER();
            t1 = mp_hal_ticks_ms();
            dt = t1 - t0;
            break;
        } else {
            k_msleep(1);
            MP_THREAD_GIL_ENTER();
        }
    }
    if (dt < ms) {
        mp_hal_delay_us(500);
    }
}

mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t pin_in) {
    if (mp_obj_is_type(pin_in, &machine_pin_type)) {
        return MP_OBJ_TO_PTR(pin_in);
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
}
