/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython Contributors
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

// AArch64 specific stuff

#include "py/mpconfig.h"

#if MICROPY_EMIT_AARCH64

// This is defined so that the assembler exports generic assembler API macros
#define GENERIC_ASM_API (1)
#include "py/asmaarch64.h"

// Word index of REG_LOCAL_1 (x19) within nlr_buf_t.  nlr_buf_t is
// {prev, ret_val, regs[13]} and nlr_push (py/nlraarch64.c) stores
// {lr, sp, x19..x29} in regs[], so x19 is regs[2] = word index 4.
#define NLR_BUF_IDX_LOCAL_1 (4)

#define N_AARCH64 (1)
#define EXPORT_FUN(name) emit_native_aarch64_##name
#include "py/emitnative.c"

#endif
