/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_REPL_H
#define MICROPY_INCLUDED_PY_REPL_H

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/mpprint.h"

#if MICROPY_HELPER_REPL

#if MICROPY_PY_SYS_PS1_PS2

const char *mp_repl_get_psx(unsigned int entry);

static inline const char *mp_repl_get_ps1(void) {
    return mp_repl_get_psx(MP_SYS_MUTABLE_PS1);
}

static inline const char *mp_repl_get_ps2(void) {
    return mp_repl_get_psx(MP_SYS_MUTABLE_PS2);
}

#else

static inline const char *mp_repl_get_ps1(void) {
    return ">>> ";
}

static inline const char *mp_repl_get_ps2(void) {
    return "... ";
}

#endif

bool mp_repl_continue_with_input(const char *input);
size_t mp_repl_autocomplete(const char *str, size_t len, const mp_print_t *print, const char **compl_str);

#endif

#endif // MICROPY_INCLUDED_PY_REPL_H
