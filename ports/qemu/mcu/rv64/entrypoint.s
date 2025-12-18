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

        .section .start
        .option  norvc              /* Do not emit compressed instructions. */
        .type    start, @function
        .global  start

start:
        .cfi_startproc

        .option push
        .option norelax
        la   gp, _global_pointer    /* Load global pointer register. */
        .option pop

        csrw satp, zero             /* Disable supervisor mode. */

        /* Fill stack with a canary value. */

        li   t0, 0xBBBBBBBBBBBBBBBB /* Load canary value. */
        la   t1, _sstack            /* Load stack area start address. */
        la   t2, _estack            /* Load stack area end address. */
1:
        sd   t0, (t1)               /* Write canary. */
        addi t1, t1, 8              /* Next word. */
        bltu t1, t2, 1b             /* Loop until stack is filled. */

        la   sp, _estack            /* Load stack pointer. */

        /* Clear BSS area. */

        la   t1, _sbss              /* Load BSS area start address. */
        la   t2, _ebss              /* Load BSS area end address. */
1:
        sd   zero, (t1)             /* Clear word. */
        addi t1, t1, 8              /* Next word. */
        bltu t1, t2, 1b             /* Loop until BSS is cleared. */

        /* Set program counter. */

        la   t0, _entry_point       /* Load program counter address. */
        csrw mepc, t0               /* Store it into machine exception PC. */
        tail _entry_point           /* Jump to entry point. */

        .cfi_endproc
        .end
