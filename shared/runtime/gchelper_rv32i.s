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

    sw    x8,  0(x10)  /* Save S0.  */
    sw    x9,  4(x10)  /* Save S1.  */
    sw   x18,  8(x10)  /* Save S2.  */
    sw   x19, 12(x10)  /* Save S3.  */
    sw   x20, 16(x10)  /* Save S4.  */
    sw   x21, 20(x10)  /* Save S5.  */
    sw   x22, 24(x10)  /* Save S6.  */
    sw   x23, 28(x10)  /* Save S7.  */
    sw   x24, 32(x10)  /* Save S8.  */
    sw   x25, 36(x10)  /* Save S9.  */
    sw   x26, 40(x10)  /* Save S10. */
    sw   x27, 44(x10)  /* Save S11. */

    /* Return the stack pointer. */

    add  x10, x0, x2
    jalr  x0, x1, 0

    .size gc_helper_get_regs_and_sp, .-gc_helper_get_regs_and_sp
