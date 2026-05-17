/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Alessandro Gatti
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

#if MICROPY_NLR_RV64I

#undef nlr_push

__attribute__((naked)) unsigned int nlr_push(nlr_buf_t *nlr) {
    __asm volatile (
        "sd   x1, 16(x10)       \n" // Store RA.
        "sd   x8, 24(x10)       \n" // Store S0.
        "sd   x9, 32(x10)       \n" // Store S1.
        "sd  x18, 40(x10)       \n" // Store S2.
        "sd  x19, 48(x10)       \n" // Store S3.
        "sd  x20, 56(x10)       \n" // Store S4.
        "sd  x21, 64(x10)       \n" // Store S5.
        "sd  x22, 72(x10)       \n" // Store S6.
        "sd  x23, 80(x10)       \n" // Store S7.
        "sd  x24, 88(x10)       \n" // Store S8.
        "sd  x25, 96(x10)       \n" // Store S9.
        "sd  x26, 104(x10)      \n" // Store S10.
        "sd  x27, 112(x10)      \n" // Store S11.
        "sd   x2, 120(x10)      \n" // Store SP.
        "jal  x0, nlr_push_tail \n" // Jump to the C part.
        );
}

MP_NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)
    __asm volatile (
        "add  x10, x0, %0  \n" // Load nlr_buf address.
        "ld   x1, 16(x10)  \n" // Retrieve RA.
        "ld   x8, 24(x10)  \n" // Retrieve S0.
        "ld   x9, 32(x10)  \n" // Retrieve S1.
        "ld  x18, 40(x10)  \n" // Retrieve S2.
        "ld  x19, 48(x10)  \n" // Retrieve S3.
        "ld  x20, 56(x10)  \n" // Retrieve S4.
        "ld  x21, 64(x10)  \n" // Retrieve S5.
        "ld  x22, 72(x10)  \n" // Retrieve S6.
        "ld  x23, 80(x10)  \n" // Retrieve S7.
        "ld  x24, 88(x10)  \n" // Retrieve S8.
        "ld  x25, 96(x10)  \n" // Retrieve S9.
        "ld  x26, 104(x10) \n" // Retrieve S10.
        "ld  x27, 112(x10) \n" // Retrieve S11.
        "ld   x2, 120(x10) \n" // Retrieve SP.
        "addi x10, x0, 1   \n" // Return 1 for a non-local return.
        "jalr  x0, x1, 0   \n" // Return.
        :                      // Outputs.
        : "r" (top)            // Inputs.
        : "memory"             // Clobbered.
        );

    MP_UNREACHABLE
}

#endif // MICROPY_NLR_RV64I
