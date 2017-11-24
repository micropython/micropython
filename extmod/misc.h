/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2016 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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
#ifndef MICROPY_INCLUDED_EXTMOD_MISC_H
#define MICROPY_INCLUDED_EXTMOD_MISC_H

// This file contains cumulative declarations for extmod/ .

#include <stddef.h>
#include "py/runtime.h"

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_uos_dupterm_obj);

#if MICROPY_PY_OS_DUPTERM
int mp_uos_dupterm_rx_chr(void);
void mp_uos_dupterm_tx_strn(const char *str, size_t len);
void mp_uos_deactivate(size_t dupterm_idx, const char *msg, mp_obj_t exc);
#else
#define mp_uos_dupterm_tx_strn(s, l)
#endif

#endif // MICROPY_INCLUDED_EXTMOD_MISC_H
