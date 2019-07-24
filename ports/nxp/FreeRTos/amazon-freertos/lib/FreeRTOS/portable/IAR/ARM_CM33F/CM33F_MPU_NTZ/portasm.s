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

    EXTERN pxCurrentTCB
    EXTERN vTaskSwitchContext
    EXTERN vPortSVCHandler_C

    PUBLIC xIsPrivileged
    PUBLIC vResetPrivilege
    PUBLIC vRestoreContextOfFirstTask
    PUBLIC vRaisePrivilege
    PUBLIC vStartFirstTask
    PUBLIC ulSetInterruptMaskFromISR
    PUBLIC vClearInterruptMaskFromISR
    PUBLIC PendSV_Handler
    PUBLIC SVC_Handler
/*-----------------------------------------------------------*/

/*---------------- Unprivileged Functions -------------------*/

/*-----------------------------------------------------------*/

    SECTION .text:CODE:NOROOT(2)
    THUMB
/*-----------------------------------------------------------*/

xIsPrivileged:
    mrs r0, control                         /* r0 = CONTROL. */
    movs r1, #1                             /* r1 = 1. */
    tst r0, r1                              /* Perform r0 & r1 (bitwise AND) and update the conditions flag. */
    beq already_running_privileged          /* If the result of previous AND operation was 0 i.e. bit 0 of CONTROL register is zero, branch. */
    movs r0, #0                             /* Return false to indicate that the processor is not privileged. */
    bx lr                                   /* Return. */
    already_running_privileged:
        movs r0, #1                         /* Return true to indicate that the processor is privileged. */
        bx lr                               /* Return. */
/*-----------------------------------------------------------*/

vResetPrivilege:
    mrs r0, control                         /* r0 = CONTROL. */
    movs r1, #1                             /* r1 = 1. */
    orrs r0, r1                             /* r0 = r0 | r1. */
    msr control, r0                         /* CONTROL = r0. */
    bx lr                                   /* Return to the caller. */
/*-----------------------------------------------------------*/

/*----------------- Privileged Functions --------------------*/

/*-----------------------------------------------------------*/

    SECTION privileged_functions:CODE:NOROOT(2)
    THUMB
/*-----------------------------------------------------------*/

vRestoreContextOfFirstTask:
    ldr  r2, =pxCurrentTCB                  /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    ldr  r1, [r2]                           /* Read pxCurrentTCB. */
    ldr  r0, [r1]                           /* Read top of stack from TCB - The first item in pxCurrentTCB is the task top of stack. */

    adds r1, #4                             /* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
    ldr  r3, [r1]                           /* r3 = *r1 i.e. r3 = MAIR0. */
    ldr  r2, =0xe000edc0                    /* r2 = 0xe000edc0 [Location of MAIR0]. */
    str  r3, [r2]                           /* Program MAIR0. */
    ldr  r2, =0xe000ed98                    /* r2 = 0xe000ed98 [Location of RNR]. */
    movs r3, #4                             /* r3 = 4. */
    str  r3, [r2]                           /* Program RNR = 4. */
    adds r1, #4                             /* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
    ldr  r2, =0xe000ed9c                    /* r2 = 0xe000ed9c [Location of RBAR]. */
    ldmia r1!, {r4-r11}                     /* Read 4 set of RBAR/RLAR registers from TCB. */
    stmia r2!, {r4-r11}                     /* Write 4 set of RBAR/RLAR registers using alias registers. */

    ldm  r0!, {r1-r3}                       /* Read from stack - r1 = PSPLIM, r2 = CONTROL and r3 = EXC_RETURN. */
    msr  psplim, r1                         /* Set this task's PSPLIM value. */
    msr  control, r2                        /* Set this task's CONTROL value. */
    adds r0, #32                            /* Discard everything up to r0. */
    msr  psp, r0                            /* This is now the new top of stack to use in the task. */
    isb
    bx   r3                                 /* Finally, branch to EXC_RETURN. */
/*-----------------------------------------------------------*/

vRaisePrivilege:
    mrs  r0, control                        /* Read the CONTROL register. */
    movs r1, #1
    bics r0, r1                             /* Clear the bit 0. */
    msr  control, r0                        /* Write back the new CONTROL value. */
    bx lr                                   /* Return to the caller. */
/*-----------------------------------------------------------*/

vStartFirstTask:
    ldr r0, =0xe000ed08                     /* Use the NVIC offset register to locate the stack. */
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
    tst lr, #0x10                           /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
    it eq
    vstmdbeq r0!, {s16-s31}                 /* Store the FPU registers which are not saved automatically. */
    mrs r1, psplim                          /* r1 = PSPLIM. */
    mrs r2, control                         /* r2 = CONTROL. */
    mov r3, lr                              /* r3 = LR/EXC_RETURN. */
    stmdb r0!, {r1-r11}                     /* Store on the stack - PSPLIM, CONTROL, LR and registers that are not automatically saved. */

    ldr r2, =pxCurrentTCB                   /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    ldr r1, [r2]                            /* Read pxCurrentTCB. */
    str r0, [r1]                            /* Save the new top of stack in TCB. */

    cpsid i
    bl vTaskSwitchContext
    cpsie i

    ldr r2, =pxCurrentTCB                   /* Read the location of pxCurrentTCB i.e. &( pxCurrentTCB ). */
    ldr r1, [r2]                            /* Read pxCurrentTCB. */
    ldr r0, [r1]                            /* The first item in pxCurrentTCB is the task top of stack. r0 now points to the top of stack. */

    adds r1, #4                             /* r1 = r1 + 4. r1 now points to MAIR0 in TCB. */
    ldr r3, [r1]                            /* r3 = *r1 i.e. r3 = MAIR0. */
    ldr r2, =0xe000edc0                     /* r2 = 0xe000edc0 [Location of MAIR0]. */
    str r3, [r2]                            /* Program MAIR0. */
    ldr r2, =0xe000ed98                     /* r2 = 0xe000ed98 [Location of RNR]. */
    movs r3, #4                             /* r3 = 4. */
    str r3, [r2]                            /* Program RNR = 4. */
    adds r1, #4                             /* r1 = r1 + 4. r1 now points to first RBAR in TCB. */
    ldr r2, =0xe000ed9c                     /* r2 = 0xe000ed9c [Location of RBAR]. */
    ldmia r1!, {r4-r11}                     /* Read 4 sets of RBAR/RLAR registers from TCB. */
    stmia r2!, {r4-r11}                     /* Write 4 set of RBAR/RLAR registers using alias registers. */

    ldmia r0!, {r1-r11}                     /* Read from stack - r1 = PSPLIM, r2 = CONTROL, r3 = LR and r4-r11 restored. */
    tst r3, #0x10                           /* Test Bit[4] in LR. Bit[4] of EXC_RETURN is 0 if the FPU is in use. */
    it eq
    vldmiaeq r0!, {s16-s31}                 /* Restore the FPU registers which are not restored automatically. */
    msr psplim, r1                          /* Restore the PSPLIM register value for the task. */
    msr control, r2                         /* Restore the CONTROL register value for the task. */
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
