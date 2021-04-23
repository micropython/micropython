/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2014 Damien P. George
 * SPDX-FileCopyrightText: Copyright (c) 2015-2018 Paul Sokolovsky
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

#include <stdarg.h>
#include <stdio.h>

#include "py/emit.h"
#include "py/runtime.h"

#if MICROPY_WARNINGS

void mp_warning(const char *category, const char *msg, ...) {
    if (category == NULL) {
        category = "Warning";
    }
    mp_print_str(MICROPY_ERROR_PRINTER, category);
    mp_print_str(MICROPY_ERROR_PRINTER, ": ");

    va_list args;
    va_start(args, msg);
    mp_vprintf(MICROPY_ERROR_PRINTER, msg, args);
    mp_print_str(MICROPY_ERROR_PRINTER, "\n");
    va_end(args);
}

void mp_emitter_warning(pass_kind_t pass, const char *msg) {
    if (pass == MP_PASS_CODE_SIZE) {
        mp_warning(NULL, msg);
    }
}

#endif // MICROPY_WARNINGS
