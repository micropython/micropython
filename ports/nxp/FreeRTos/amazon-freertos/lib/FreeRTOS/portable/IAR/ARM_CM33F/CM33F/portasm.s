/*
 * Amazon FreeRTOS Pre-Release V1.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

    SECTION .text:CODE:NOROOT(2)
    THUMB

    EXTERN pxCurrentTCB
    EXTERN xSecureContext
    EXTERN vTaskSwitchContext
    EXTERN vPortSVCHandler_C
    EXTERN SecureContext_SaveContext
    EXTERN SecureContext_LoadContext

    PUBLIC vRestoreContextOfFirstTask
    PUBLIC vStartFirstTask
    PUBLIC ulSetInterruptMaskFromISR
    PUBLIC vClearInterruptMaskFromISR
    PUBLIC PendSV_Handler
    PUBLIC SVC_Handler
    PUBLIC vPortAllocateSecureContext
    PUBLIC vPortFreeSecureContext
/*-----------------------------------------------------------*/

vRestoreContextOfFirstTask:
    ldr  r2, =pxCurrentTCB                  /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    ldr  r3, [r2]                           /* Read pxCurrentTCB. */
    ldr  r0, [r3]                           /* Read top of stack from TCB - The first item in pxCurrentTCB is the task top of stack. */

    ldm  r0!, {r1-r3}                       /* Read from stack - r1 = xSecureContext, r2 = PSPLIM and r3 = EXC_RETURN. */
    ldr  r4, =xSecureContext
    str  r1, [r4]                           /* Set xSecureContext to this task's value for the same. */
    msr  psplim, r2                         /* Set this task's PSPLIM value. */
    movs r1, #2                             /* r1 = 2. */
    msr  CONTROL, r1                        /* Switch to use PSP in the thread mode. */
    adds r0, #32                            /* Discard everything up to r0. */
    msr  psp, r0                            /* This is now the new top of stack to use in the task. */
    isb
    bx   r3                                 /* Finally, branch to EXC_RETURN. */
/*-----------------------------------------------------------*/

vStartFirstTask:
    ldr r0, =0xE000ED08                     /* Use the NVIC offset register to locate the stack. */
    ldr r0, [r0]                            /* Read the VTOR register which gives the address of vector table. */
    ldr r0, [r0]                            /* The first entry in vector table is stack pointer. */
    msr msp, r0                             /* Set the MSP back to the start of the stack. */
    cpsie i                                 /* Globally enable interrupts. */
    cpsie f
    dsb
    isb
    svc 2                                   /* System call to start the first task. portSVC_START_SCHEDULER = 2. */
/*-----------------------------------------------------------*/

ulSetInterruptMaskFromISR:
    mrs r0, PRIMASK
    cpsid i
    bx lr
/*-----------------------------------------------------------*/

vClearInterruptMaskFromISR:
    msr PRIMASK, r0
    bx lr
/*-----------------------------------------------------------*/

PendSV_Handler:
    mrs r1, psp                             /* Read PSP in r1. */
    ldr r2, =xSecureContext                 /* Read the location of xSecureContext i.e. &( xSecureContext ). */
    ldr r0, [r2]                            /* Read xSecureContext - Value of xSecureContext must be in r0 as it is used as a parameter later. */

    cbz r0, save_ns_context                 /* No secure context to save. */
    push {r0-r2, r14}
    bl SecureContext_SaveContext
    pop {r0-r3}                             /* LR is now in r3. */
    mov lr, r3                              /* LR = r3. */
    lsls r2, r3, #25                        /* r2 = r3 << 25. Bit[6] of EXC_RETURN is 1 if secure stack was used, 0 if non-secure stack was used to store stack frame. */
    bpl save_ns_context                     /* bpl - branch if positive or zero. If r2 >= 0 ==> Bit[6] in EXC_RETURN is 0 i.e. non-secure stack was used. */
    ldr r3, =pxCurrentTCB                   /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    ldr r2, [r3]                            /* Read pxCurrentTCB. */
    subs r1, r1, #12                        /* Make space for xSecureContext, PSPLIM and LR on the stack. */
    str r1, [r2]                            /* Save the new top of stack in TCB. */
    mrs r2, psplim                          /* r2 = PSPLIM. */
    mov r3, lr                              /* r3 = LR/EXC_RETURN. */
    stmia r1!, {r0, r2-r3}                  /* Store xSecureContext, PSPLIM and LR on the stack. */
    b select_next_task

    save_ns_context:
        ldr r3, =pxCurrentTCB               /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
        ldr r2, [r3]                        /* Read pxCurrentTCB. */
        tst lr, #0x10                       /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
        it eq
        vstmdbeq r1!, {s16-s31}             /* Store the FPU registers which are not saved automatically. */
        subs r1, r1, #44                    /* Make space for xSecureContext, PSPLIM, LR and the remaining registers on the stack. */
        str r1, [r2]                        /* Save the new top of stack in TCB. */
        adds r1, r1, #12                    /* r1 = r1 + 12. */
        stm r1, {r4-r11}                    /* Store the registers that are not saved automatically. */
        mrs r2, psplim                      /* r2 = PSPLIM. */
        mov r3, lr                          /* r3 = LR/EXC_RETURN. */
        subs r1, r1, #12                    /* r1 = r1 - 12. */
        stmia r1!, {r0, r2-r3}              /* Store xSecureContext, PSPLIM and LR on the stack. */

    select_next_task:
        cpsid i
        bl vTaskSwitchContext
        cpsie i

        ldr r2, =pxCurrentTCB               /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
        ldr r3, [r2]                        /* Read pxCurrentTCB. */
        ldr r1, [r3]                        /* The first item in pxCurrentTCB is the task top of stack. r1 now points to the top of stack. */

        ldmia r1!, {r0, r2-r3}              /* Read from stack - r0 = xSecureContext, r2 = PSPLIM and r3 = LR. */
        msr psplim, r2                      /* Restore the PSPLIM register value for the task. */
        mov lr, r3                          /* LR = r3. */
        ldr r2, =xSecureContext             /* Read the location of xSecureContext i.e. &( xSecureContext ). */
        str r0, [r2]                        /* Restore the task's xSecureContext. */
        cbz r0, restore_ns_context          /* If there is no secure context for the task, restore the non-secure context. */
        push {r1,r3}
        bl SecureContext_LoadContext        /* Restore the secure context. */
        pop {r1,r3}
        mov lr, r3                          /* LR = r3. */
        lsls r2, r3, #25                    /* r2 = r3 << 25. Bit[6] of EXC_RETURN is 1 if secure stack was used, 0 if non-secure stack was used to store stack frame. */
        bpl restore_ns_context              /* bpl - branch if positive or zero. If r2 >= 0 ==> Bit[6] in EXC_RETURN is 0 i.e. non-secure stack was used. */
        msr psp, r1                         /* Remember the new top of stack for the task. */
        bx lr

    restore_ns_context:
        ldmia r1!, {r4-r11}                 /* Restore the registers that are not automatically restored. */
        tst lr, #0x10                       /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
        it eq
        vldmiaeq r1!, {s16-s31}             /* Restore the FPU registers which are not restored automatically. */
        msr psp, r1                         /* Remember the new top of stack for the task. */
        bx lr
/*-----------------------------------------------------------*/

SVC_Handler:
    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    b vPortSVCHandler_C
/*-----------------------------------------------------------*/

vPortAllocateSecureContext:
    svc 0                                   /* Secure context is allocated in the supervisor call. portSVC_ALLOCATE_SECURE_CONTEXT = 0. */
    bx lr                                   /* Return. */
/*-----------------------------------------------------------*/

vPortFreeSecureContext:
    /* r0 = uint32_t *pulTCB. */
    ldr r1, [r0]                            /* The first item in the TCB is the top of the stack. */
    ldr r0, [r1]                            /* The first item on the stack is the task's xSecureContext. */
    cmp r0, #0                              /* Raise svc if task's xSecureContext is not NULL. */
    it ne
    svcne 1                                 /* Secure context is freed in the supervisor call. portSVC_FREE_SECURE_CONTEXT = 1. */
    bx lr                                   /* Return. */
/*-----------------------------------------------------------*/

    END
