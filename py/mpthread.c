/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Damien George
 * Copyright (c) 2023 Jim Mussared
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

#include <stdarg.h>

#include "py/mpthread.h"
#include "py/stackctrl.h"
#include "py/runtime.h"

#if MICROPY_PY_THREAD && MICROPY_PY_THREAD_RTOS

// When we have a GIL, we let the RTOS thread/task acquire the GIL (which
// means no MicroPython thread is currently running), and then take over the
// thread-local state of the main thread to run the callback. Essentially the
// RTOS task becomes the main MicroPython thread temporarily.

#if MICROPY_PY_THREAD_GIL

#if MICROPY_ENABLE_PYSTACK
#error not supported
#endif

void mp_thread_run_on_mp_thread(mp_run_on_thread_function_t fn, void *arg, mp_uint_t stack_size) {
    // This code may run on an existing MicroPython thread, or a non-MicroPython thread
    // that's not using the mp_thread_get_state() value.  In the former case the state
    // must be restored once this callback finishes.
    mp_state_thread_t *ts_orig = mp_thread_get_state();

    mp_state_thread_t ts;
    if (ts_orig == NULL) {
        mp_thread_set_state(&ts);
        mp_stack_set_top(&ts + 1); // need to include ts in root-pointer scan
        mp_stack_set_limit(stack_size);
        ts.gc_lock_depth = 0;
        ts.mp_pending_exception = MP_OBJ_NULL;
        mp_locals_set(mp_state_ctx.thread.dict_locals); // set from the outer context
        mp_globals_set(mp_state_ctx.thread.dict_globals); // set from the outer context
        MP_THREAD_GIL_ENTER();
    }

    mp_sched_lock();
    fn(arg);
    mp_sched_unlock();

    if (ts_orig == NULL) {
        MP_THREAD_GIL_EXIT();
        mp_thread_set_state(ts_orig);
    }
}

#else // !MICROPY_PY_THREAD_GIL

#if !MICROPY_ENABLE_SCHEDULER
#error MICROPY_PY_THREAD_RTOS requires MICROPY_ENABLE_SCHEDULER if MICROPY_PY_THREAD_GIL is not enabled.
#endif

// When we don't have a GIL, we instead schedule a callback to run on a
// MicroPython thread, and wait while it executes the callback for us.

// TODO: We could make this require the caller to pass a static scheduler
// node, with a unique node per source task.

typedef struct {
    mp_run_on_thread_function_t fn;
    void *arg;
    mp_thread_sem_t sem;
} mp_run_on_thread_context_t;

STATIC mp_obj_t run_in_scheduler(mp_obj_t context_in) {
    mp_run_on_thread_context_t *context = MP_OBJ_TO_PTR(context_in);

    // Run the callback.
    context->fn(context->arg);

    // Signal to the waiting RTOS task that we're done.
    mp_thread_sem_post(&context->sem);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(run_in_scheduler_obj, run_in_scheduler);

void mp_thread_run_on_mp_thread(mp_run_on_thread_function_t fn, void *arg, mp_uint_t stack_size) {
    (void)stack_size;

    if (mp_thread_get_state()) {
        // We're already on a MicroPython thread, just invoke the callback
        // directly.
        fn(arg);
        return;
    }

    mp_run_on_thread_context_t context = { .fn = fn, .arg = arg };
    mp_thread_sem_init(&context.sem, 0);

    if (!mp_sched_schedule(MP_OBJ_FROM_PTR(&run_in_scheduler_obj), MP_OBJ_FROM_PTR(&context))) {
        mp_printf(&mp_plat_print, "failed to schedule\n");
    }
    // TODO: How to handle fail-to-schedule? See above, perhaps should use
    // static nodes to make that impossible.

    // Wait for the scheduled task to complete.
    mp_thread_sem_wait(&context.sem, true);
}

#endif // MICROPY_PY_THREAD_GIL

#endif // MICROPY_PY_THREAD && MICROPY_PY_THREAD_RTOS
