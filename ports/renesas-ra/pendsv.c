/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdlib.h>

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "pendsv.h"
#include "irq.h"

#if defined(PENDSV_DISPATCH_NUM_SLOTS)
uint32_t pendsv_dispatch_active;
pendsv_dispatch_t pendsv_dispatch_table[PENDSV_DISPATCH_NUM_SLOTS];
#endif

void pendsv_init(void) {
    #if defined(PENDSV_DISPATCH_NUM_SLOTS)
    pendsv_dispatch_active = false;
    #endif
    // set PendSV interrupt at lowest priority
    NVIC_SetPriority(PendSV_IRQn, IRQ_PRI_PENDSV);
}

#if defined(PENDSV_DISPATCH_NUM_SLOTS)
void pendsv_schedule_dispatch(size_t slot, pendsv_dispatch_t f) {
    pendsv_dispatch_table[slot] = f;
    pendsv_dispatch_active = true;
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void pendsv_dispatch_handler(void) {
    for (size_t i = 0; i < PENDSV_DISPATCH_NUM_SLOTS; ++i) {
        if (pendsv_dispatch_table[i] != NULL) {
            pendsv_dispatch_t f = pendsv_dispatch_table[i];
            pendsv_dispatch_table[i] = NULL;
            f();
        }
    }
}
#endif

__attribute__((naked)) void PendSV_Handler(void) {
    // Handle a PendSV interrupt
    //
    // Calls any pending functions in pendsv_dispatch_table.
    //
    // For the case of a thread switch, swap stacks.
    //
    // on entry to this (naked) function, stack has the following layout:
    //
    // stack layout with DEBUG disabled:
    //   sp[6]: pc=r15
    //   sp[5]: lr=r14
    //   sp[4]: r12
    //   sp[3]: r3
    //   sp[2]: r2
    //   sp[1]: r1
    //   sp[0]: r0
    //
    // stack layout with DEBUG enabled:
    //   sp[8]: pc=r15
    //   sp[7]: lr=r14
    //   sp[6]: r12
    //   sp[5]: r3
    //   sp[4]: r2
    //   sp[3]: r1
    //   sp[2]: r0
    //   sp[1]: 0xfffffff9
    //   sp[0]: ?

    __asm volatile (
        #if defined(PENDSV_DISPATCH_NUM_SLOTS)
        // Check if there are any pending calls to dispatch to
        "ldr r1, pendsv_dispatch_active_ptr\n"
        "ldr r0, [r1]\n"
        "cmp r0, #0\n"
        "beq .no_dispatch\n"
        "mov r2, #0\n"
        "str r2, [r1]\n"                // clear pendsv_dispatch_active
        "b pendsv_dispatch_handler\n"   // jump to the handler
        ".no_dispatch:\n"
        #endif

        #if MICROPY_PY_THREAD
        // Do a thread context switch
        "push {r4-r11, lr}\n"
        "vpush {s16-s31}\n"
        "mrs r5, primask\n"             // save PRIMASK in r5
        "cpsid i\n"                     // disable interrupts while we change stacks
        "mov r0, sp\n"                  // pass sp to save
        "mov r4, lr\n"                  // save lr because we are making a call
        "bl pyb_thread_next\n"          // get next thread to execute
        "mov lr, r4\n"                  // restore lr
        "mov sp, r0\n"                  // switch stacks
        "msr primask, r5\n"             // re-enable interrupts
        "vpop {s16-s31}\n"
        "pop {r4-r11, lr}\n"
        "bx lr\n"                       // return from interrupt; will return to new thread
        #else
        // Spurious pendsv, just return
        "bx lr\n"
        #endif

        // Data
        ".align 2\n"
        #if defined(PENDSV_DISPATCH_NUM_SLOTS)
        "pendsv_dispatch_active_ptr: .word pendsv_dispatch_active\n"
        #endif
        );
}
