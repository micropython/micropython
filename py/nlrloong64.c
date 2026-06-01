/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Alessandro Gatti
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

#include "py/mpstate.h"

#if MICROPY_NLR_LOONG64

__attribute__((used)) unsigned int nlr_push_tail(nlr_buf_t *nlr);

__asm(
    ".globl nlr_push     \n"
    "nlr_push:           \n"
    ".cfi_startproc      \n"
    "st.d $r1,  $r4, 16  \n" /* Store RA. */
    "st.d $r23, $r4, 24  \n" /* Store S0. */
    "st.d $r24, $r4, 32  \n" /* Store S1. */
    "st.d $r25, $r4, 40  \n" /* Store S2. */
    "st.d $r26, $r4, 48  \n" /* Store S3. */
    "st.d $r27, $r4, 56  \n" /* Store S4. */
    "st.d $r28, $r4, 64  \n" /* Store S5. */
    "st.d $r29, $r4, 72  \n" /* Store S6. */
    "st.d $r30, $r4, 80  \n" /* Store S7. */
    "st.d $r31, $r4, 88  \n" /* Store S8. */
    "st.d $r22, $r4, 96  \n" /* Store S9. */
    "st.d $r21, $r4, 104 \n" /* Marked as reserved in the ABI. */
    "st.d $r3,  $r4, 112 \n" /* Store SP. */
    "b    nlr_push_tail  \n" /* Jump to the C part. */
    ".cfi_endproc        \n"
    );

MP_NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)
    __asm volatile (
        "add.d  $r4,  $r0, %0  \n"
        "ld.d   $r1,  $r4, 16  \n" /* Retrieve RA. */
        "ld.d   $r23, $r4, 24  \n" /* Retrieve S0. */
        "ld.d   $r24, $r4, 32  \n" /* Retrieve S1. */
        "ld.d   $r25, $r4, 40  \n" /* Retrieve S2. */
        "ld.d   $r26, $r4, 48  \n" /* Retrieve S3. */
        "ld.d   $r27, $r4, 56  \n" /* Retrieve S4. */
        "ld.d   $r28, $r4, 64  \n" /* Retrieve S5. */
        "ld.d   $r29, $r4, 72  \n" /* Retrieve S6. */
        "ld.d   $r30, $r4, 80  \n" /* Retrieve S7. */
        "ld.d   $r31, $r4, 88  \n" /* Retrieve S8. */
        "ld.d   $r22, $r4, 96  \n" /* Retrieve S9. */
        "ld.d   $r21, $r4, 104 \n"
        "ld.d   $r3,  $r4, 112 \n" /* Retrieve SP. */
        "addi.d $r4,  $r0, 1   \n" /* Return 1 for a non-local return. */
        "ret                   \n" /* Return. */
        :
        : "r" (top)
        : "memory"
        );
    MP_UNREACHABLE
}

#endif
