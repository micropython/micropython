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
#ifndef MICROPY_INCLUDED_PY_PARSENUM_H
#define MICROPY_INCLUDED_PY_PARSENUM_H

#include "py/mpconfig.h"
#include "py/lexer.h"
#include "py/obj.h"

// these functions raise a SyntaxError if lex!=NULL, else a ValueError

mp_obj_t mp_parse_num_integer(const char *restrict str, size_t len, int base, mp_lexer_t *lex);

#if MICROPY_PY_BUILTINS_COMPLEX
mp_obj_t mp_parse_num_decimal(const char *str, size_t len, bool allow_imag, bool force_complex, mp_lexer_t *lex);

static inline mp_obj_t mp_parse_num_float(const char *str, size_t len, bool allow_imag, mp_lexer_t *lex) {
    return mp_parse_num_decimal(str, len, allow_imag, false, lex);
}

static inline mp_obj_t mp_parse_num_complex(const char *str, size_t len, mp_lexer_t *lex) {
    return mp_parse_num_decimal(str, len, true, true, lex);
}
#else
mp_obj_t mp_parse_num_float(const char *str, size_t len, bool allow_imag, mp_lexer_t *lex);
#endif

#endif // MICROPY_INCLUDED_PY_PARSENUM_H
