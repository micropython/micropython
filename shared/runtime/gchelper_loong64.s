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

    .global gc_helper_get_regs_and_sp
    .type   gc_helper_get_regs_and_sp, @function

gc_helper_get_regs_and_sp:

    /* Store registers into the given array. */

    st.d  $r23, $r4, 0   /* Save S0.  */
    st.d  $r24, $r4, 8   /* Save S1.  */
    st.d  $r25, $r4, 16  /* Save S2.  */
    st.d  $r26, $r4, 24  /* Save S3.  */
    st.d  $r27, $r4, 32  /* Save S4.  */
    st.d  $r28, $r4, 40  /* Save S5.  */
    st.d  $r29, $r4, 48  /* Save S6.  */
    st.d  $r30, $r4, 56  /* Save S7.  */
    st.d  $r31, $r4, 64  /* Save S8.  */
    st.d  $r22, $r4, 72  /* Save S9.  */

    /* Return the stack pointer. */

    add.d $r4, $r0, $r3
    jirl  $r0, $r1, 0

    .size gc_helper_get_regs_and_sp, .-gc_helper_get_regs_and_sp
