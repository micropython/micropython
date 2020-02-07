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

#if MICROPY_KBD_EXCEPTION
// This function may be called asynchronously at any time so only do the bare minimum.
void MICROPY_WRAP_MP_KEYBOARD_INTERRUPT(mp_keyboard_interrupt)(void) {
    MP_STATE_VM(mp_kbd_exception).traceback_data = NULL;
    MP_STATE_VM(mp_pending_exception) = MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception));
    #if MICROPY_ENABLE_SCHEDULER
    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
    #endif
}
#endif

#if MICROPY_ENABLE_SCHEDULER

#define IDX_MASK(i) ((i) & (MICROPY_SCHEDULER_DEPTH - 1))

static inline bool mp_sched_full(void) {
    MP_STATIC_ASSERT(MICROPY_SCHEDULER_DEPTH <= 255); // MICROPY_SCHEDULER_DEPTH must fit in 8 bits
    MP_STATIC_ASSERT((IDX_MASK(MICROPY_SCHEDULER_DEPTH) == 0)); // MICROPY_SCHEDULER_DEPTH must be a power of 2

    return mp_sched_num_pending() == MICROPY_SCHEDULER_DEPTH;
}

static inline bool mp_sched_empty(void) {
    return mp_sched_num_pending() == 0;
}

// A variant of this is inlined in the VM at the pending exception check
void mp_handle_pending(bool raise_exc) {
    if (MP_STATE_VM(sched_state) == MP_SCHED_PENDING) {
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
        if (obj != MP_OBJ_NULL) {
            MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
            if (!mp_sched_num_pending()) {
                MP_STATE_VM(sched_state) = MP_SCHED_IDLE;
            }
            if (raise_exc) {
                MICROPY_END_ATOMIC_SECTION(atomic_state);
                nlr_raise(obj);
            }
        }
        mp_handle_pending_tail(atomic_state);
    }
}

// This function should only be called be mp_sched_handle_pending,
// or by the VM's inlined version of that function.
void mp_handle_pending_tail(mp_uint_t atomic_state) {
    MP_STATE_VM(sched_state) = MP_SCHED_LOCKED;
    if (!mp_sched_empty()) {
        mp_sched_item_t item = MP_STATE_VM(sched_queue)[MP_STATE_VM(sched_idx)];
        MP_STATE_VM(sched_idx) = IDX_MASK(MP_STATE_VM(sched_idx) + 1);
        --MP_STATE_VM(sched_len);
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
    if (!mp_sched_full()) {
        if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
            MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
        }
        uint8_t iput = IDX_MASK(MP_STATE_VM(sched_idx) + MP_STATE_VM(sched_len)++);
        MP_STATE_VM(sched_queue)[iput].func = function;
        MP_STATE_VM(sched_queue)[iput].arg = arg;
        ret = true;
    } else {
        // schedule queue is full
        ret = false;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return ret;
}

#else // MICROPY_ENABLE_SCHEDULER

// A variant of this is inlined in the VM at the pending exception check
void mp_handle_pending(bool raise_exc) {
    if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
        mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
        MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
        if (raise_exc) {
            nlr_raise(obj);
        }
    }
}

#endif // MICROPY_ENABLE_SCHEDULER
