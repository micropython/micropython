/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Emil Renner Berthing
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

#if MICROPY_NLR_RISCV32

#undef nlr_push

// For reference, riscv callee save regs are:
//      s0-s11, ra and sp

__attribute__((naked)) unsigned int nlr_push(nlr_buf_t *nlr) {

    __asm volatile (
        "sw    s0,     0+%0    \n" // store s0 into nlr->regs
        "sw    s1,     4+%0    \n" // store s1 into nlr->regs
        "sw    s2,     8+%0    \n" // store s2 into nlr->regs
        "sw    s3,    12+%0    \n" // store s3 into nlr->regs
        "sw    s4,    16+%0    \n" // store s4 into nlr->regs
        "sw    s5,    20+%0    \n" // store s5 into nlr->regs
        "sw    s6,    24+%0    \n" // store s6 into nlr->regs
        "sw    s7,    28+%0    \n" // store s7 into nlr->regs
        "sw    s8,    32+%0    \n" // store s8 into nlr->regs
        "sw    s9,    36+%0    \n" // store s9 into nlr->regs
        "sw    s10,   40+%0    \n" // store s10 into nlr->regs
        "sw    s11,   44+%0    \n" // store s11 into nlr->regs
        "sw    ra,    48+%0    \n" // store ra into nlr->regs
        "sw    sp,    52+%0    \n" // store sp into nlr->regs
        "tail  nlr_push_tail   \n" // do the rest in C
        : "=o" (nlr->regs)
        );
}

NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)

    /* WARNING: if the above macro grows to call a function
     * (that returns) after loading top, the compiler might
     * decide to stash top in an s-register and the assembly
     * below will break.
     */
    __asm volatile (
        "lw    s0,     0+%0    \n" // load s0 from top->regs
        "lw    s1,     4+%0    \n" // load s1 from top->regs
        "lw    s2,     8+%0    \n" // load s2 from top->regs
        "lw    s3,    12+%0    \n" // load s3 from top->regs
        "lw    s4,    16+%0    \n" // load s4 from top->regs
        "lw    s5,    20+%0    \n" // load s5 from top->regs
        "lw    s6,    24+%0    \n" // load s6 from top->regs
        "lw    s7,    28+%0    \n" // load s7 from top->regs
        "lw    s8,    32+%0    \n" // load s8 from top->regs
        "lw    s9,    36+%0    \n" // load s9 from top->regs
        "lw    s10,   40+%0    \n" // load s10 from top->regs
        "lw    s11,   44+%0    \n" // load s11 from top->regs
        "lw    ra,    48+%0    \n" // load ra from top->regs
        "lw    sp,    52+%0    \n" // load sp from top->regs
        "li    a0,   1         \n" // return 1, non-local return
        "ret                   \n" // return
        :: "o" (top->regs)
        );

    MP_UNREACHABLE
}

#endif // MICROPY_NLR_RISCV32
