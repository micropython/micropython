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

void PendSV_Handler(void);

// Using the nowait variant here as softtimer updates PendSV from the loop of mp_wfe_or_timeout(),
// where we don't want the CPU event bit to be set.
static mp_thread_recursive_mutex_t pendsv_mutex;

void pendsv_init(void) {
    mp_thread_recursive_mutex_init(&pendsv_mutex);
}

void pendsv_suspend(void) {
    // Recursive Mutex here as either core may call pendsv_suspend() and expect
    // both mutual exclusion (other core can't enter pendsv_suspend() at the
    // same time), and that no PendSV handler will run.
    mp_thread_recursive_mutex_lock(&pendsv_mutex, 1);
}

void pendsv_resume(void) {
    mp_thread_recursive_mutex_unlock(&pendsv_mutex);

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
    if (pendsv_mutex.mutex.enter_count == 0) {
        #if PICO_ARM
        // There is a race here where other core calls pendsv_suspend() before
        // ISR can execute, but dispatch will happen later when other core
        // calls pendsv_resume().
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
void PendSV_Handler(void) {

    if (!mp_thread_recursive_mutex_lock(&pendsv_mutex, 0)) {
        // Failure here means core 1 holds pendsv_mutex. ISR will
        // run again after core 1 calls pendsv_resume().
        return;
    }
    // Core 0 should not already have locked pendsv_mutex
    assert(pendsv_mutex.mutex.enter_count == 1);

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

    mp_thread_recursive_mutex_unlock(&pendsv_mutex);
}
