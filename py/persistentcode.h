/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_PERSISTENTCODE_H
#define MICROPY_INCLUDED_PY_PERSISTENTCODE_H

#include "py/mpprint.h"
#include "py/reader.h"
#include "py/emitglue.h"

// The current version of .mpy files
#define MPY_VERSION 4

enum {
    MP_NATIVE_ARCH_NONE = 0,
    MP_NATIVE_ARCH_X86,
    MP_NATIVE_ARCH_X64,
    MP_NATIVE_ARCH_ARMV6,
    MP_NATIVE_ARCH_ARMV6M,
    MP_NATIVE_ARCH_ARMV7M,
    MP_NATIVE_ARCH_ARMV7EM,
    MP_NATIVE_ARCH_ARMV7EMSP,
    MP_NATIVE_ARCH_ARMV7EMDP,
    MP_NATIVE_ARCH_XTENSA,
};

mp_raw_code_t *mp_raw_code_load(mp_reader_t *reader);
mp_raw_code_t *mp_raw_code_load_mem(const byte *buf, size_t len);
mp_raw_code_t *mp_raw_code_load_file(const char *filename);

void mp_raw_code_save(mp_raw_code_t *rc, mp_print_t *print);
void mp_raw_code_save_file(mp_raw_code_t *rc, const char *filename);

#endif // MICROPY_INCLUDED_PY_PERSISTENTCODE_H
