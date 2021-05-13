/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2015 Paul Sokolovsky
 * SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_FROZENMOD_H
#define MICROPY_INCLUDED_PY_FROZENMOD_H

#include "py/lexer.h"

enum {
    MP_FROZEN_NONE,
    MP_FROZEN_STR,
    MP_FROZEN_MPY,
};

// Frozen modules are in a pseudo-directory, so sys.path can control how they're found.
#define MP_FROZEN_FAKE_DIR ".frozen"
#define MP_FROZEN_FAKE_DIR_LENGTH (sizeof(MP_FROZEN_FAKE_DIR) - 1)

#define MP_FROZEN_FAKE_DIR_SLASH (MP_FROZEN_FAKE_DIR "/")
#define MP_FROZEN_FAKE_DIR_SLASH_LENGTH (sizeof(MP_FROZEN_FAKE_DIR_SLASH) - 1)

// This should match MP_FROZEN_FAKE_DIR.
#define MP_FROZEN_FAKE_DIR_QSTR MP_QSTR__dot_frozen

int mp_find_frozen_module(const char *str, size_t len, void **data);
const char *mp_find_frozen_str(const char *str, size_t str_len, size_t *len);
mp_import_stat_t mp_frozen_stat(const char *str);

#endif // MICROPY_INCLUDED_PY_FROZENMOD_H
