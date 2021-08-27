/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/parsenumbase.h"

// find real radix base, and strip preceding '0x', '0o' and '0b'
// puts base in *base, and returns number of bytes to skip the prefix
size_t mp_parse_num_base(const char *str, size_t len, int *base) {
    const byte *p = (const byte *)str;
    if (len <= 1) {
        goto no_prefix;
    }
    unichar c = *(p++);
    if ((*base == 0 || *base == 16) && c == '0') {
        c = *(p++);
        if ((c | 32) == 'x') {
            *base = 16;
        } else if (*base == 0 && (c | 32) == 'o') {
            *base = 8;
        } else if (*base == 0 && (c | 32) == 'b') {
            *base = 2;
        } else {
            if (*base == 0) {
                *base = 10;
            }
            p -= 2;
        }
    } else if (*base == 8 && c == '0') {
        c = *(p++);
        if ((c | 32) != 'o') {
            p -= 2;
        }
    } else if (*base == 2 && c == '0') {
        c = *(p++);
        if ((c | 32) != 'b') {
            p -= 2;
        }
    } else {
        p--;
    no_prefix:
        if (*base == 0) {
            *base = 10;
        }
    }
    return p - (const byte *)str;
}
