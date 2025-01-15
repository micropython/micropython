/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
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

#if MICROPY_NLR_MIPS

__attribute__((used)) unsigned int nlr_push_tail(nlr_buf_t *nlr);

__asm(
    ".globl  nlr_push         \n"
    "nlr_push:                \n"
    ".ent    nlr_push         \n"
    ".frame  $29, 0, $31      \n"
    ".set    noreorder        \n"
    ".cpload $25              \n"
    ".set    reorder          \n"
    "sw $31, 8($4)            \n"    /* is the offset of regs in nlr_buf_t */
    "sw $30, 12($4)           \n"
    "sw $29, 16($4)           \n"
    "sw $28, 20($4)           \n"
    "sw $23, 24($4)           \n"
    "sw $22, 28($4)           \n"
    "sw $21, 32($4)           \n"
    "sw $20, 36($4)           \n"
    "sw $19, 40($4)           \n"
    "sw $18, 44($4)           \n"
    "sw $17, 48($4)           \n"
    "sw $16, 52($4)           \n"
#ifdef __pic__
    "la $25, nlr_push_tail    \n"
#endif
    "j nlr_push_tail          \n"
    ".end nlr_push            \n"
    );

MP_NORETURN void nlr_jump(void *val) {
    MP_NLR_JUMP_HEAD(val, top)
    __asm(
        "move $4, %0    \n"
        "lw $31, 8($4)  \n"
        "lw $30, 12($4) \n"
        "lw $29, 16($4) \n"
        "lw $28, 20($4) \n"
        "lw $23, 24($4) \n"
        "lw $22, 28($4) \n"
        "lw $21, 32($4) \n"
        "lw $20, 36($4) \n"
        "lw $19, 40($4) \n"
        "lw $18, 44($4) \n"
        "lw $17, 48($4) \n"
        "lw $16, 52($4) \n"
        "lui $2,1       \n"   // set return value 1
        "j $31          \n"
        "nop            \n"
        :
        : "r" (top)
        : "memory"
        );
    MP_UNREACHABLE
}

#endif // MICROPY_NLR_MIPS
