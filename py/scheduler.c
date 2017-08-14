/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include <stdio.h>

#include "py/runtime.h"

#if MICROPY_ENABLE_SCHEDULER

// A variant of this is inlined in the VM at the pending exception check
void mp_handle_pending(void) {
    if (MP_STATE_VM(sched_state) == MP_SCHED_PENDING) {
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
        if (obj != MP_OBJ_NULL) {
            MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
            if (!mp_sched_num_pending()) {
                MP_STATE_VM(sched_state) = MP_SCHED_IDLE;
            }
            MICROPY_END_ATOMIC_SECTION(atomic_state);
            nlr_raise(obj);
        }
        mp_handle_pending_tail(atomic_state);
    }
}

// This function should only be called be mp_sched_handle_pending,
// or by the VM's inlined version of that function.
void mp_handle_pending_tail(mp_uint_t atomic_state) {
    MP_STATE_VM(sched_state) = MP_SCHED_LOCKED;
    if (MP_STATE_VM(sched_sp) > 0) {
        mp_sched_item_t item = MP_STATE_VM(sched_stack)[--MP_STATE_VM(sched_sp)];
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        mp_call_function_1_protected(item.func, item.arg);
    } else {
        MICROPY_END_ATOMIC_SECTION(atomic_state);
    }
    mp_sched_unlock();
}

void mp_sched_lock(void) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    if (MP_STATE_VM(sched_state) < 0) {
        --MP_STATE_VM(sched_state);
    } else {
        MP_STATE_VM(sched_state) = MP_SCHED_LOCKED;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

void mp_sched_unlock(void) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    if (++MP_STATE_VM(sched_state) == 0) {
        // vm became unlocked
        if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL || mp_sched_num_pending()) {
            MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
        } else {
            MP_STATE_VM(sched_state) = MP_SCHED_IDLE;
        }
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

bool mp_sched_schedule(mp_obj_t function, mp_obj_t arg) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool ret;
    if (MP_STATE_VM(sched_sp) < MICROPY_SCHEDULER_DEPTH) {
        if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
            MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
        }
        MP_STATE_VM(sched_stack)[MP_STATE_VM(sched_sp)].func = function;
        MP_STATE_VM(sched_stack)[MP_STATE_VM(sched_sp)].arg = arg;
        ++MP_STATE_VM(sched_sp);
        ret = true;
    } else {
        // schedule stack is full
        ret = false;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return ret;
}

#else // MICROPY_ENABLE_SCHEDULER

// A variant of this is inlined in the VM at the pending exception check
void mp_handle_pending(void) {
    if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
        mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
        MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
        nlr_raise(obj);
    }
}

#endif // MICROPY_ENABLE_SCHEDULER
