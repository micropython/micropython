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

#if MICROPY_NLR_RV32I

#undef nlr_push

__attribute__((naked)) unsigned int nlr_push(nlr_buf_t *nlr) {
    __asm volatile (
        "sw   x1,  8(x10)       \n" // Store RA.
        "sw   x8, 12(x10)       \n" // Store S0.
        "sw   x9, 16(x10)       \n" // Store S1.
        "sw  x18, 20(x10)       \n" // Store S2.
        "sw  x19, 24(x10)       \n" // Store S3.
        "sw  x20, 28(x10)       \n" // Store S4.
        "sw  x21, 32(x10)       \n" // Store S5.
        "sw  x22, 36(x10)       \n" // Store S6.
        "sw  x23, 40(x10)       \n" // Store S7.
        "sw  x24, 44(x10)       \n" // Store S8.
        "sw  x25, 48(x10)       \n" // Store S9.
        "sw  x26, 52(x10)       \n" // Store S10.
        "sw  x27, 56(x10)       \n" // Store S11.
        "sw   x2, 60(x10)       \n" // Store SP.
        "jal  x0, nlr_push_tail \n" // Jump to the C part.
        );
}

NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)
    __asm volatile (
        "add  x10, x0, %0  \n" // Load nlr_buf address.
        "lw   x1,  8(x10)  \n" // Retrieve RA.
        "lw   x8, 12(x10)  \n" // Retrieve S0.
        "lw   x9, 16(x10)  \n" // Retrieve S1.
        "lw  x18, 20(x10)  \n" // Retrieve S2.
        "lw  x19, 24(x10)  \n" // Retrieve S3.
        "lw  x20, 28(x10)  \n" // Retrieve S4.
        "lw  x21, 32(x10)  \n" // Retrieve S5.
        "lw  x22, 36(x10)  \n" // Retrieve S6.
        "lw  x23, 40(x10)  \n" // Retrieve S7.
        "lw  x24, 44(x10)  \n" // Retrieve S8.
        "lw  x25, 48(x10)  \n" // Retrieve S9.
        "lw  x26, 52(x10)  \n" // Retrieve S10.
        "lw  x27, 56(x10)  \n" // Retrieve S11.
        "lw   x2, 60(x10)  \n" // Retrieve SP.
        "addi x10, x0, 1   \n" // Return 1 for a non-local return.
        "jalr  x0, x1, 0   \n" // Return.
        :                      // Outputs.
        : "r" (top)            // Inputs.
        : "memory"             // Clobbered.
        );

    MP_UNREACHABLE
}

#endif // MICROPY_NLR_RV32I
