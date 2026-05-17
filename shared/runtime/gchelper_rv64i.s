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

    .global gc_helper_get_regs_and_sp
    .type   gc_helper_get_regs_and_sp, @function

gc_helper_get_regs_and_sp:

    /* Store registers into the given array. */

    sd    x8,  0(x10)  /* Save S0.  */
    sd    x9,  8(x10)  /* Save S1.  */
    sd   x18, 16(x10)  /* Save S2.  */
    sd   x19, 24(x10)  /* Save S3.  */
    sd   x20, 32(x10)  /* Save S4.  */
    sd   x21, 40(x10)  /* Save S5.  */
    sd   x22, 48(x10)  /* Save S6.  */
    sd   x23, 56(x10)  /* Save S7.  */
    sd   x24, 64(x10)  /* Save S8.  */
    sd   x25, 72(x10)  /* Save S9.  */
    sd   x26, 80(x10)  /* Save S10. */
    sd   x27, 88(x10)  /* Save S11. */

    /* Return the stack pointer. */

    add  x10, x0, x2
    jalr  x0, x1, 0

    .size gc_helper_get_regs_and_sp, .-gc_helper_get_regs_and_sp
