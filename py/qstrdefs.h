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

// *FORMAT-OFF*

#include "py/mpconfig.h"

// All the qstr definitions in this file are available as constants.
// That is, they are in ROM and you can reference them simply as MP_QSTR_xxxx.

// qstr configuration passed to makeqstrdata.py of the form QCFG(key, value)
QCFG(BYTES_IN_LEN, MICROPY_QSTR_BYTES_IN_LEN)
QCFG(BYTES_IN_HASH, MICROPY_QSTR_BYTES_IN_HASH)

Q()
Q(*)
Q(_)
Q(/)
#if MICROPY_PY_BUILTINS_STR_OP_MODULO
Q(%#o)
Q(%#x)
#else
Q({:#o})
Q({:#x})
#endif
Q({:#b})
Q( )
Q(\n)
Q(maximum recursion depth exceeded)
Q(<module>)
Q(<lambda>)
Q(<listcomp>)
Q(<dictcomp>)
Q(<setcomp>)
Q(<genexpr>)
Q(<string>)
Q(<stdin>)
Q(utf-8)

#if MICROPY_ENABLE_PYSTACK
Q(pystack exhausted)
#endif
