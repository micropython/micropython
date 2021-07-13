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
#ifndef MICROPY_INCLUDED_PY_QSTR_H
#define MICROPY_INCLUDED_PY_QSTR_H

#include "py/mpconfig.h"
#include "py/misc.h"

// See qstrdefs.h for a list of qstr's that are available as constants.
// Reference them as MP_QSTR_xxxx.
//
// Note: it would be possible to define MP_QSTR_xxx as qstr_from_str("xxx")
// for qstrs that are referenced this way, but you don't want to have them in ROM.

// first entry in enum will be MP_QSTRnull=0, which indicates invalid/no qstr
enum {
    #ifndef NO_QSTR
#define QENUM(id) id,
    #include "genhdr/qstrdefs.enum.h"
#undef QENUM
    #endif
    MP_QSTRnumber_of, // no underscore so it can't clash with any of the above
};

typedef size_t qstr;

typedef struct _qstr_attr_t {
    #if MICROPY_QSTR_BYTES_IN_HASH == 1
    uint8_t hash;
    #elif MICROPY_QSTR_BYTES_IN_HASH == 2
    uint16_t hash;
    #else
    #error unimplemented qstr hash decoding
    #endif
    #if MICROPY_QSTR_BYTES_IN_LEN == 1
    uint8_t len;
    #elif MICROPY_QSTR_BYTES_IN_LEN == 2
    uint16_t len;
    #else
    #error unimplemented qstr length decoding
    #endif
} qstr_attr_t;

typedef struct _qstr_pool_t {
    const struct _qstr_pool_t *prev;
    size_t total_prev_len;
    size_t alloc;
    size_t len;
    qstr_attr_t *attrs;
    const char *qstrs[];
} qstr_pool_t;

#define QSTR_TOTAL() (MP_STATE_VM(last_pool)->total_prev_len + MP_STATE_VM(last_pool)->len)

void qstr_init(void);

mp_uint_t qstr_compute_hash(const byte *data, size_t len);
qstr qstr_find_strn(const char *str, size_t str_len); // returns MP_QSTRnull if not found

qstr qstr_from_str(const char *str);
qstr qstr_from_strn(const char *str, size_t len);

mp_uint_t qstr_hash(qstr q);
const char *qstr_str(qstr q);
size_t qstr_len(qstr q);
const byte *qstr_data(qstr q, size_t *len);

void qstr_pool_info(size_t *n_pool, size_t *n_qstr, size_t *n_str_data_bytes, size_t *n_total_bytes);
void qstr_dump_data(void);

#endif // MICROPY_INCLUDED_PY_QSTR_H
