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

#include "py/mphal.h"
#include "py/runtime.h"

// Schedules an exception on the main thread (for exceptions "thrown" by async
// sources such as interrupts and UNIX signal handlers).
void MICROPY_WRAP_MP_SCHED_EXCEPTION(mp_sched_exception)(mp_obj_t exc) {
    MP_STATE_MAIN_THREAD(mp_pending_exception) = exc;

    #if MICROPY_ENABLE_SCHEDULER && !MICROPY_PY_THREAD
    // Optimisation for the case where we have scheduler but no threading.
    // Allows the VM to do a single check to exclude both pending exception
    // and queued tasks.
    if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
        MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
    }
    #endif
}

#if MICROPY_KBD_EXCEPTION
// This function may be called asynchronously at any time so only do the bare minimum.
void MICROPY_WRAP_MP_SCHED_KEYBOARD_INTERRUPT(mp_sched_keyboard_interrupt)(void) {
    MP_STATE_VM(mp_kbd_exception).traceback_data = NULL;
    mp_sched_exception(MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception)));
}
#endif

#if MICROPY_ENABLE_VM_ABORT
void MICROPY_WRAP_MP_SCHED_VM_ABORT(mp_sched_vm_abort)(void) {
    MP_STATE_VM(vm_abort) = true;
}
#endif

#if MICROPY_ENABLE_SCHEDULER

#define IDX_MASK(i) ((i) & (MICROPY_SCHEDULER_DEPTH - 1))

// This is a macro so it is guaranteed to be inlined in functions like
// mp_sched_schedule that may be located in a special memory region.
#define mp_sched_full() (mp_sched_num_pending() == MICROPY_SCHEDULER_DEPTH)

static inline bool mp_sched_empty(void) {
    MP_STATIC_ASSERT(MICROPY_SCHEDULER_DEPTH <= 255); // MICROPY_SCHEDULER_DEPTH must fit in 8 bits
    MP_STATIC_ASSERT((IDX_MASK(MICROPY_SCHEDULER_DEPTH) == 0)); // MICROPY_SCHEDULER_DEPTH must be a power of 2

    return mp_sched_num_pending() == 0;
}

static inline void mp_sched_run_pending(void) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    if (MP_STATE_VM(sched_state) != MP_SCHED_PENDING) {
        // Something else (e.g. hard IRQ) locked the scheduler while we
        // acquired the lock.
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        return;
    }

    // Equivalent to mp_sched_lock(), but we're already in the atomic
    // section and know that we're pending.
    MP_STATE_VM(sched_state) = MP_SCHED_LOCKED;

    #if MICROPY_SCHEDULER_STATIC_NODES
    // Run all pending C callbacks.
    mp_sched_node_t *original_tail = MP_STATE_VM(sched_tail);
    if (original_tail != NULL) {
        mp_sched_node_t *node;
        do {
            node = MP_STATE_VM(sched_head);
            MP_STATE_VM(sched_head) = node->next;
            if (MP_STATE_VM(sched_head) == NULL) {
                MP_STATE_VM(sched_tail) = NULL;
            }
            mp_sched_callback_t callback = node->callback;
            node->callback = NULL;
            MICROPY_END_ATOMIC_SECTION(atomic_state);
            callback(node);
            atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        } while (node != original_tail); // Don't execute any callbacks scheduled during this run
    }
    #endif

    // Run at most one pending Python callback.
    if (!mp_sched_empty()) {
        mp_sched_item_t item = MP_STATE_VM(sched_queue)[MP_STATE_VM(sched_idx)];
        MP_STATE_VM(sched_idx) = IDX_MASK(MP_STATE_VM(sched_idx) + 1);
        --MP_STATE_VM(sched_len);
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        mp_call_function_1_protected(item.func, item.arg);
    } else {
        MICROPY_END_ATOMIC_SECTION(atomic_state);
    }

    // Restore MP_STATE_VM(sched_state) to idle (or pending if there are still
    // tasks in the queue).
    mp_sched_unlock();
}

// Locking the scheduler prevents tasks from executing (does not prevent new
// tasks from being added). We lock the scheduler while executing scheduled
// tasks and also in hard interrupts or GC finalisers.
void mp_sched_lock(void) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    if (MP_STATE_VM(sched_state) < 0) {
        // Already locked, increment lock (recursive lock).
        --MP_STATE_VM(sched_state);
    } else {
        // Pending or idle.
        MP_STATE_VM(sched_state) = MP_SCHED_LOCKED;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

void mp_sched_unlock(void) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    assert(MP_STATE_VM(sched_state) < 0);
    if (++MP_STATE_VM(sched_state) == 0) {
        // Scheduler became unlocked. Check if there are still tasks in the
        // queue and set sched_state accordingly.
        if (
            #if !MICROPY_PY_THREAD
            // See optimisation in mp_sched_exception.
            MP_STATE_THREAD(mp_pending_exception) != MP_OBJ_NULL ||
            #endif
            #if MICROPY_SCHEDULER_STATIC_NODES
            MP_STATE_VM(sched_head) != NULL ||
            #endif
            mp_sched_num_pending()) {
            MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
        } else {
            MP_STATE_VM(sched_state) = MP_SCHED_IDLE;
        }
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

bool MICROPY_WRAP_MP_SCHED_SCHEDULE(mp_sched_schedule)(mp_obj_t function, mp_obj_t arg) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool ret;
    if (!mp_sched_full()) {
        if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
            MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
        }
        uint8_t iput = IDX_MASK(MP_STATE_VM(sched_idx) + MP_STATE_VM(sched_len)++);
        MP_STATE_VM(sched_queue)[iput].func = function;
        MP_STATE_VM(sched_queue)[iput].arg = arg;
        MICROPY_SCHED_HOOK_SCHEDULED;
        ret = true;
    } else {
        // schedule queue is full
        ret = false;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return ret;
}

#if MICROPY_SCHEDULER_STATIC_NODES
bool mp_sched_schedule_node(mp_sched_node_t *node, mp_sched_callback_t callback) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool ret;
    if (node->callback == NULL) {
        if (MP_STATE_VM(sched_state) == MP_SCHED_IDLE) {
            MP_STATE_VM(sched_state) = MP_SCHED_PENDING;
        }
        node->callback = callback;
        node->next = NULL;
        if (MP_STATE_VM(sched_tail) == NULL) {
            MP_STATE_VM(sched_head) = node;
        } else {
            MP_STATE_VM(sched_tail)->next = node;
        }
        MP_STATE_VM(sched_tail) = node;
        MICROPY_SCHED_HOOK_SCHEDULED;
        ret = true;
    } else {
        // already scheduled
        ret = false;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return ret;
}
#endif

MP_REGISTER_ROOT_POINTER(mp_sched_item_t sched_queue[MICROPY_SCHEDULER_DEPTH]);

#endif // MICROPY_ENABLE_SCHEDULER

// Called periodically from the VM or from "waiting" code (e.g. sleep) to
// process background tasks and pending exceptions (e.g. KeyboardInterrupt).
void mp_handle_pending_internal(mp_handle_pending_behaviour_t behavior) {
    bool handle_exceptions = (behavior != MP_HANDLE_PENDING_CALLBACKS_ONLY);
    bool raise_exceptions = (behavior == MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS);

    // Handle pending VM abort.
    #if MICROPY_ENABLE_VM_ABORT
    if (handle_exceptions && MP_STATE_VM(vm_abort) && mp_thread_is_main_thread()) {
        MP_STATE_VM(vm_abort) = false;
        if (raise_exceptions && nlr_get_abort() != NULL) {
            nlr_jump_abort();
        }
    }
    #endif

    // Handle any pending exception.
    if (handle_exceptions && MP_STATE_THREAD(mp_pending_exception) != MP_OBJ_NULL) {
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        mp_obj_t obj = MP_STATE_THREAD(mp_pending_exception);
        if (obj != MP_OBJ_NULL) {
            MP_STATE_THREAD(mp_pending_exception) = MP_OBJ_NULL;
            if (raise_exceptions) {
                MICROPY_END_ATOMIC_SECTION(atomic_state);
                nlr_raise(obj);
            }
        }
        MICROPY_END_ATOMIC_SECTION(atomic_state);
    }

    // Handle any pending callbacks.
    #if MICROPY_ENABLE_SCHEDULER
    bool run_scheduler = (MP_STATE_VM(sched_state) == MP_SCHED_PENDING);
    #if MICROPY_PY_THREAD && !MICROPY_PY_THREAD_GIL
    // Avoid races by running the scheduler on the main thread, only.
    // (Not needed if GIL enabled, as GIL ensures thread safety here.)
    run_scheduler = run_scheduler && mp_thread_is_main_thread();
    #endif
    if (run_scheduler) {
        mp_sched_run_pending();
    }
    #endif
}

// Handles any pending MicroPython events without waiting for an interrupt or event.
void mp_event_handle_nowait(void) {
    #if defined(MICROPY_EVENT_POLL_HOOK_FAST) && !MICROPY_PREVIEW_VERSION_2
    // For ports still using the old macros.
    MICROPY_EVENT_POLL_HOOK_FAST
    #else
    // Process any port layer (non-blocking) events.
    MICROPY_INTERNAL_EVENT_HOOK;
    mp_handle_pending(true);
    #endif
}

// Handles any pending MicroPython events and then suspends execution until the
// next interrupt or event.
void mp_event_wait_indefinite(void) {
    #if defined(MICROPY_EVENT_POLL_HOOK) && !MICROPY_PREVIEW_VERSION_2
    // For ports still using the old macros.
    MICROPY_EVENT_POLL_HOOK
    #else
    mp_event_handle_nowait();
    MICROPY_INTERNAL_WFE(-1);
    #endif
}

// Handle any pending MicroPython events and then suspends execution until the
// next interrupt or event, or until timeout_ms milliseconds have elapsed.
void mp_event_wait_ms(mp_uint_t timeout_ms) {
    #if defined(MICROPY_EVENT_POLL_HOOK) && !MICROPY_PREVIEW_VERSION_2
    // For ports still using the old macros.
    MICROPY_EVENT_POLL_HOOK
    #else
    mp_event_handle_nowait();
    MICROPY_INTERNAL_WFE(timeout_ms);
    #endif
}
