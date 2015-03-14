/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#ifndef __MICROPY_INCLUDED_PY_PFENV_H__
#define __MICROPY_INCLUDED_PY_PFENV_H__

#include <stdarg.h>

#include "py/obj.h"

#define PF_FLAG_LEFT_ADJUST       (0x001)
#define PF_FLAG_SHOW_SIGN         (0x002)
#define PF_FLAG_SPACE_SIGN        (0x004)
#define PF_FLAG_NO_TRAILZ         (0x008)
#define PF_FLAG_SHOW_PREFIX       (0x010)
#define PF_FLAG_SHOW_COMMA        (0x020)
#define PF_FLAG_PAD_AFTER_SIGN    (0x040)
#define PF_FLAG_CENTER_ADJUST     (0x080)
#define PF_FLAG_ADD_PERCENT       (0x100)
#define PF_FLAG_PAD_NAN_INF       (0x200)
#define PF_FLAG_SHOW_OCTAL_LETTER (0x400)

typedef struct _pfenv_t {
    void *data;
    void (*print_strn)(void *, const char *str, mp_uint_t len);
} pfenv_t;

void pfenv_vstr_add_strn(void *data, const char *str, mp_uint_t len);

int pfenv_print_strn(const pfenv_t *pfenv, const char *str, mp_uint_t len, int flags, char fill, int width);
int pfenv_print_int(const pfenv_t *pfenv, mp_uint_t x, int sgn, int base, int base_char, int flags, char fill, int width);
int pfenv_print_mp_int(const pfenv_t *pfenv, mp_obj_t x, int base, int base_char, int flags, char fill, int width, int prec);
#if MICROPY_PY_BUILTINS_FLOAT
int pfenv_print_float(const pfenv_t *pfenv, mp_float_t f, char fmt, int flags, char fill, int width, int prec);
#endif

int pfenv_vprintf(const pfenv_t *pfenv, const char *fmt, va_list args);
int pfenv_printf(const pfenv_t *pfenv, const char *fmt, ...);

// Wrapper for system printf
void printf_wrapper(void *env, const char *fmt, ...);

#endif // __MICROPY_INCLUDED_PY_PFENV_H__
