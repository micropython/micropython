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
    EXTERN vTaskSwitchContext
    EXTERN vPortSVCHandler_C

    PUBLIC vRestoreContextOfFirstTask
    PUBLIC vStartFirstTask
    PUBLIC ulSetInterruptMaskFromISR
    PUBLIC vClearInterruptMaskFromISR
    PUBLIC PendSV_Handler
    PUBLIC SVC_Handler
/*-----------------------------------------------------------*/

vRestoreContextOfFirstTask:
    ldr  r2, =pxCurrentTCB                  /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    ldr  r3, [r2]                           /* Read pxCurrentTCB. */
    ldr  r0, [r3]                           /* Read top of stack from TCB - The first item in pxCurrentTCB is the task top of stack. */

    ldm  r0!, {r1-r2}                       /* Read from stack - r1 = PSPLIM and r2 = EXC_RETURN. */
    msr  psplim, r1                         /* Set this task's PSPLIM value. */
    movs r1, #2                             /* r1 = 2. */
    msr  CONTROL, r1                        /* Switch to use PSP in the thread mode. */
    adds r0, #32                            /* Discard everything up to r0. */
    msr  psp, r0                            /* This is now the new top of stack to use in the task. */
    isb
    bx   r2                                 /* Finally, branch to EXC_RETURN. */
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
    mrs r0, psp                             /* Read PSP in r0. */
    ldr r2, =pxCurrentTCB                   /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    ldr r1, [r2]                            /* Read pxCurrentTCB. */

    tst lr, #0x10                           /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
    it eq
    vstmdbeq r0!, {s16-s31}                 /* Store the FPU registers which are not saved automatically. */
    mrs r2, psplim                          /* r2 = PSPLIM. */
    mov r3, lr                              /* r3 = LR/EXC_RETURN. */
    stmdb r0!, {r2-r11}                     /* Store PSPLIM, LR and registers that are not automatically saved on the stack. */
    str r0, [r1]                            /* Save the new top of stack in TCB. */

    cpsid i
    bl vTaskSwitchContext
    cpsie i

    ldr r2, =pxCurrentTCB                   /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    ldr r1, [r2]                            /* Read pxCurrentTCB. */
    ldr r0, [r1]                            /* The first item in pxCurrentTCB is the task top of stack. r0 now points to the top of stack. */

    ldmia r0!, {r2-r11}                     /* Read from stack - r2 = PSPLIM, r3 = LR and r4-r11 restored. */
    tst r3, #0x10                           /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
    it eq
    vldmiaeq r0!, {s16-s31}                 /* Restore the FPU registers which are not restored automatically. */
    msr psplim, r2                          /* Restore the PSPLIM register value for the task. */
    msr psp, r0                             /* Remember the new top of stack for the task. */
    bx r3
/*-----------------------------------------------------------*/

SVC_Handler:
    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    b vPortSVCHandler_C
/*-----------------------------------------------------------*/

    END
