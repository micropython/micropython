/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include STM32_HAL_H

#include "py/mpstate.h"
#include "py/runtime.h"
#include "pendsv.h"
#include "irq.h"

// This variable is used to save the exception object between a ctrl-C and the
// PENDSV call that actually raises the exception.  It must be non-static
// otherwise gcc-5 optimises it away.  It can point to the heap but is not
// traced by GC.  This is okay because we only ever set it to
// mp_const_vcp_interrupt which is in the root-pointer set.
void *pendsv_object;

void pendsv_init(void) {
    // set PendSV interrupt at lowest priority
    HAL_NVIC_SetPriority(PendSV_IRQn, IRQ_PRI_PENDSV, IRQ_SUBPRI_PENDSV);
}

// Call this function to raise a pending exception during an interrupt.
// It will first try to raise the exception "softly" by setting the
// mp_pending_exception variable and hoping that the VM will notice it.
// If this function is called a second time (ie with the mp_pending_exception
// variable already set) then it will force the exception by using the hardware
// PENDSV feature.  This will wait until all interrupts are finished then raise
// the given exception object using nlr_jump in the context of the top-level
// thread.
void pendsv_nlr_jump(void *o) {
    if (MP_STATE_VM(mp_pending_exception) == MP_OBJ_NULL) {
        MP_STATE_VM(mp_pending_exception) = o;
    } else {
        MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
        pendsv_object = o;
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    }
}

void pendsv_isr_handler(void) {
    // re-jig the stack so that when we return from this interrupt handler
    // it returns instead to nlr_jump with argument pendsv_object
    // note that stack has a different layout if DEBUG is enabled
    //
    // on entry to this (naked) function, stack has the following layout:
    //
    // stack layout with DEBUG disabled:
    //   sp[6]: pc
    //   sp[5]: ?
    //   sp[4]: ?
    //   sp[3]: ?
    //   sp[2]: ?
    //   sp[1]: ?
    //   sp[0]: r0
    //
    // stack layout with DEBUG enabled:
    //   sp[8]: pc
    //   sp[7]: lr
    //   sp[6]: ?
    //   sp[5]: ?
    //   sp[4]: ?
    //   sp[3]: ?
    //   sp[2]: r0
    //   sp[1]: 0xfffffff9
    //   sp[0]: ?

    __asm volatile (
        "ldr r0, pendsv_object_ptr\n"
        "ldr r0, [r0]\n"
#if defined(PENDSV_DEBUG)
        "str r0, [sp, #8]\n"
#else
        "str r0, [sp, #0]\n"
#endif
        "ldr r0, nlr_jump_ptr\n"
#if defined(PENDSV_DEBUG)
        "str r0, [sp, #32]\n"
#else
        "str r0, [sp, #24]\n"
#endif
        "bx lr\n"
        ".align 2\n"
        "pendsv_object_ptr: .word pendsv_object\n"
        "nlr_jump_ptr: .word nlr_jump\n"
    );

    /*
    uint32_t x[2] = {0x424242, 0xdeaddead};
    printf("PendSV: %p\n", x);
    for (uint32_t *p = (uint32_t*)(((uint32_t)x - 15) & 0xfffffff0), i = 64; i > 0; p += 4, i -= 4) {
        printf(" %p: %08x %08x %08x %08x\n", p, (uint)p[0], (uint)p[1], (uint)p[2], (uint)p[3]);
    }
    */
}
