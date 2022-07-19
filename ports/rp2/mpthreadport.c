/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "py/gc.h"
#include "py/mpthread.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

#if MICROPY_PY_THREAD

extern uint8_t __StackTop, __StackBottom;

void *core_state[2];

// This will be non-NULL while Python code is execting.
STATIC void *(*core1_entry)(void *) = NULL;

STATIC void *core1_arg = NULL;
STATIC uint32_t *core1_stack = NULL;
STATIC size_t core1_stack_num_words = 0;

// Thread mutex.
STATIC mp_thread_mutex_t atomic_mutex;

uint32_t mp_thread_begin_atomic_section(void) {
    if (core1_entry) {
        // When both cores are executing, we also need to provide
        // full mutual exclusion.
        mp_thread_mutex_lock(&atomic_mutex, 1);
        // In case this atomic section is for flash access, then
        // suspend the other core.
        multicore_lockout_start_blocking();
    }

    return save_and_disable_interrupts();
}

void mp_thread_end_atomic_section(uint32_t state) {
    restore_interrupts(state);

    if (core1_entry) {
        multicore_lockout_end_blocking();
        mp_thread_mutex_unlock(&atomic_mutex);
    }
}

// Initialise threading support.
void mp_thread_init(void) {
    assert(get_core_num() == 0);

    mp_thread_mutex_init(&atomic_mutex);

    // Allow MICROPY_BEGIN_ATOMIC_SECTION to be invoked from core1.
    multicore_lockout_victim_init();

    mp_thread_set_state(&mp_state_ctx.thread);
    core1_entry = NULL;
}

// Shutdown threading support -- stops the second thread.
void mp_thread_deinit(void) {
    assert(get_core_num() == 0);
    // Must ensure that core1 is not currently holding the GC lock, otherwise
    // it will be terminated while holding the lock.
    mp_thread_mutex_lock(&MP_STATE_MEM(gc_mutex), 1);
    multicore_reset_core1();
    core1_entry = NULL;
    mp_thread_mutex_unlock(&MP_STATE_MEM(gc_mutex));
}

void mp_thread_gc_others(void) {
    if (core1_entry != NULL) {
        // Collect core1's stack if it is active.
        gc_collect_root((void **)&core1_stack, 1);
        gc_collect_root((void **)&core1_arg, 1);
    }
    if (get_core_num() == 1) {
        // GC running on core1, trace core0's stack.
        gc_collect_root((void **)&__StackBottom, (&__StackTop - &__StackBottom) / sizeof(uintptr_t));
    }
}

STATIC void core1_entry_wrapper(void) {
    // Allow MICROPY_BEGIN_ATOMIC_SECTION to be invoked from core0.
    multicore_lockout_victim_init();

    if (core1_entry) {
        core1_entry(core1_arg);
    }

    core1_entry = NULL;
    // returning from here will loop the core forever (WFI)
}

void mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size) {
    // Check if core1 is already in use.
    if (core1_entry != NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("core1 in use"));
    }

    core1_entry = entry;
    core1_arg = arg;

    if (*stack_size == 0) {
        *stack_size = 4096; // default stack size
    } else if (*stack_size < 2048) {
        *stack_size = 2048; // minimum stack size
    }

    // Round stack size to a multiple of the word size.
    core1_stack_num_words = *stack_size / sizeof(uint32_t);
    *stack_size = core1_stack_num_words * sizeof(uint32_t);

    // Allocate stack.
    core1_stack = m_new(uint32_t, core1_stack_num_words);

    // Create thread on core1.
    multicore_reset_core1();
    multicore_launch_core1_with_stack(core1_entry_wrapper, core1_stack, *stack_size);

    // Adjust stack_size to provide room to recover from hitting the limit.
    *stack_size -= 512;
}

void mp_thread_start(void) {
}

void mp_thread_finish(void) {
}

#endif // MICROPY_PY_THREAD
