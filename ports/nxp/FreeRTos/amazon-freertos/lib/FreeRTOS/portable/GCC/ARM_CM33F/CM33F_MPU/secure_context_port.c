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

/* Secure context includes. */
#include "secure_context.h"

/* Secure port macros. */
#include "secure_port_macros.h"

secureportNON_SECURE_CALLABLE void SecureContext_LoadContext( SecureContextHandle_t xSecureContextHandle )
{
    /* xSecureContextHandle value is in r0. */
    __asm volatile
    (
    "    .syntax unified                        \n"
    "                                           \n"
    "    mrs r1, ipsr                           \n" /* r1 = IPSR. */
    "    cbz r1, load_ctx_therad_mode           \n" /* Do nothing if the processor is running in the Thread Mode. */
    "    ldmia r0!, {r1, r2}                    \n" /* r1 = xSecureContextHandle->pucCurrentStackPointer, r2 = xSecureContextHandle->pucStackLimit. */
    "    ldmia r1!, {r3}                        \n" /* Read CONTROL register value from task's stack. r3 = CONTROL. */
    "    msr psplim, r2                         \n" /* PSPLIM = r2. */
    "    msr psp, r1                            \n" /* PSP = r1. */
    "    msr control, r3                        \n" /* CONTROL = r3. */
    "                                           \n"
    " load_ctx_therad_mode:                     \n"
    "    nop                                    \n"
    "                                           \n"
    :::"r0", "r1", "r2", "r3"
    );
}
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureContext_SaveContext( SecureContextHandle_t xSecureContextHandle )
{
    /* xSecureContextHandle value is in r0. */
    __asm volatile
    (
    "    .syntax unified                        \n"
    "                                           \n"
    "    mrs r1, ipsr                           \n" /* r1 = IPSR. */
    "    cbz r1, save_ctx_therad_mode           \n" /* Do nothing if the processor is running in the Thread Mode. */
    "    mrs r1, psp                            \n" /* r1 = PSP. */
    "    vstmdb r1!, {s0}                       \n" /* Trigger the defferred stacking of FPU registers. */
    "    vldmia r1!, {s0}                       \n" /* Nullify the effect of the pervious statement. */
    "    subs r1, r1, #4                        \n" /* Make space on the stack for the CONTROL register. */
    "    str r1, [r0]                           \n" /* Save the new top of stack in context. xSecureContextHandle->pucCurrentStackPointer = r1. */
    "    mrs r2, control                        \n" /* r2 = CONTROL. */
    "    stmia r1!, {r2}                        \n" /* Store CONTROL value on the stack. */
    "    movs r1, %0                            \n" /* r1 = securecontextNO_STACK. */
    "    msr psplim, r1                         \n" /* PSPLIM = securecontextNO_STACK. */
    "    msr psp, r1                            \n" /* PSP = securecontextNO_STACK i.e. No stack for thread mode until next task's context is loaded. */
    "                                           \n"
    " save_ctx_therad_mode:                     \n"
    "    nop                                    \n"
    "                                           \n"
    :: "i" ( securecontextNO_STACK ) : "r1", "r2", "memory"
    );
}
/*-----------------------------------------------------------*/
