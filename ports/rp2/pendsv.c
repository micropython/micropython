/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Damien P. George
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

#include <assert.h>
#include "py/mpconfig.h"
#include "py/mpthread.h"
#include "pendsv.h"
#include "hardware/irq.h"

#if PICO_RP2040
#include "RP2040.h"
#elif PICO_RP2350 && PICO_ARM
#include "RP2350.h"
#elif PICO_RISCV
#include "pico/aon_timer.h"
#endif

#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43_stats.h"
#endif

static pendsv_dispatch_t pendsv_dispatch_table[PENDSV_DISPATCH_NUM_SLOTS];

static inline void pendsv_resume_run_dispatch(void);

// PendSV IRQ priority, to run system-level tasks that preempt the main thread.
#define IRQ_PRI_PENDSV PICO_LOWEST_IRQ_PRIORITY

void PendSV_Handler(void);

#if MICROPY_PY_THREAD

// Important to use a 'nowait' mutex here as softtimer updates PendSV from the
// loop of mp_wfe_or_timeout(), where we don't want the CPU event bit to be set.
static mp_thread_recursive_mutex_t pendsv_mutex;

// Called from CPU0 during boot, but may be called later when CPU1 wakes up
void pendsv_init(void) {
    if (get_core_num() == 0) {
        mp_thread_recursive_mutex_init(&pendsv_mutex);
    }
    #if !defined(__riscv)
    NVIC_SetPriority(PendSV_IRQn, IRQ_PRI_PENDSV);
    #endif
}

void pendsv_suspend(void) {
    // Recursive Mutex here as either core may call pendsv_suspend() and expect
    // both mutual exclusion (other core can't enter pendsv_suspend() at the
    // same time), and that no PendSV handler will run.
    mp_thread_recursive_mutex_lock(&pendsv_mutex, 1);
}

void pendsv_resume(void) {
    mp_thread_recursive_mutex_unlock(&pendsv_mutex);
    pendsv_resume_run_dispatch();
}

static inline int pendsv_suspend_count(void) {
    return pendsv_mutex.mutex.enter_count;
}

#else

// Without threads we don't include any pico-sdk mutex in the build,
// but also we don't need to worry about cross-thread contention (or
// races with interrupts that update this counter).
static int pendsv_lock;

void pendsv_init(void) {
}

void pendsv_suspend(void) {
    pendsv_lock++;
}

void pendsv_resume(void) {
    assert(pendsv_lock > 0);
    pendsv_lock--;
    pendsv_resume_run_dispatch();
}

static inline int pendsv_suspend_count(void) {
    return pendsv_lock;
}

#endif

bool pendsv_is_pending(size_t slot) {
    return pendsv_dispatch_table[slot] != NULL;
}

static inline void pendsv_resume_run_dispatch(void) {
    // Run pendsv if needed.  Find an entry with a dispatch and call pendsv dispatch
    // with it.  If pendsv runs it will service all slots.
    int count = PENDSV_DISPATCH_NUM_SLOTS;
    while (count--) {
        if (pendsv_dispatch_table[count]) {
            pendsv_schedule_dispatch(count, pendsv_dispatch_table[count]);
            break;
        }
    }
}

void pendsv_schedule_dispatch(size_t slot, pendsv_dispatch_t f) {
    pendsv_dispatch_table[slot] = f;
    // There is a race here where other core calls pendsv_suspend() before ISR
    // can execute so this check fails, but dispatch will happen later when
    // other core calls pendsv_resume().
    if (pendsv_suspend_count() == 0) {
        #if PICO_ARM
        // Note this register is part of each CPU core, so setting it on CPUx
        // will set the IRQ and run PendSV_Handler on CPUx only.
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
        #elif PICO_RISCV
        struct timespec ts;
        aon_timer_get_time(&ts);
        aon_timer_enable_alarm(&ts, PendSV_Handler, false);
        #endif
    } else {
        #if MICROPY_PY_NETWORK_CYW43
        CYW43_STAT_INC(PENDSV_DISABLED_COUNT);
        #endif
    }
}

// PendSV interrupt handler to perform background processing.
//
// Handler can execute on either CPU if MICROPY_PY_THREAD is set (no code on
// CPU1 calls pendsv_schedule_dispatch(), but CPU1 can call pendsv_resume()
// which will trigger it).
void PendSV_Handler(void) {

    #if MICROPY_PY_THREAD
    if (!mp_thread_recursive_mutex_lock(&pendsv_mutex, 0)) {
        // Failure here means other core holds pendsv_mutex. ISR will
        // run again after that core calls pendsv_resume().
        return;
    }
    // This core should not already have locked pendsv_mutex
    assert(pendsv_mutex.mutex.enter_count == 1);
    #else
    assert(pendsv_suspend_count() == 0);
    #endif

    #if MICROPY_PY_NETWORK_CYW43
    CYW43_STAT_INC(PENDSV_RUN_COUNT);
    #endif

    for (size_t i = 0; i < PENDSV_DISPATCH_NUM_SLOTS; ++i) {
        if (pendsv_dispatch_table[i] != NULL) {
            pendsv_dispatch_t f = pendsv_dispatch_table[i];
            pendsv_dispatch_table[i] = NULL;
            f();
        }
    }

    #if MICROPY_PY_THREAD
    mp_thread_recursive_mutex_unlock(&pendsv_mutex);
    #endif
}
