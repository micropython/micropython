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

// See qstrdefs.h for a list of qstr's that are available as constants.
// Reference them as MP_QSTR_xxxx.
//
// Note: it would be possible to define MP_QSTR_xxx as qstr_from_str_static("xxx")
// for qstrs that are referenced this way, but you don't want to have them in ROM.

enum {
    MP_QSTR_NULL = 0, // indicates invalid/no qstr
    MP_QSTR_ = 1, // the empty qstr
#define Q(id, str) MP_QSTR_##id,
#include "genhdr/qstrdefs.generated.h"
#undef Q
    MP_QSTR_number_of,
};

typedef mp_uint_t qstr;

#define QSTR_FROM_STR_STATIC(s) (qstr_from_strn((s), strlen(s)))

void qstr_init(void);

mp_uint_t qstr_compute_hash(const byte *data, uint len);
qstr qstr_find_strn(const char *str, uint str_len); // returns MP_QSTR_NULL if not found

qstr qstr_from_str(const char *str);
qstr qstr_from_strn(const char *str, uint len);
//qstr qstr_from_str_static(const char *str);
//qstr qstr_from_strn_copy(const char *str, int len);

byte* qstr_build_start(uint len, byte **q_ptr);
qstr qstr_build_end(byte *q_ptr);

mp_uint_t qstr_hash(qstr q);
const char* qstr_str(qstr q);
uint qstr_len(qstr q);
const byte* qstr_data(qstr q, uint *len);

void qstr_pool_info(uint *n_pool, uint *n_qstr, uint *n_str_data_bytes, uint *n_total_bytes);
