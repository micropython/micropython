/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The music encoded herein is either in the public domain, composed by
 * Nicholas H.Tollervey or the composer is untraceable and covered by fair
 * (educational) use.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
 * Copyright (c) 2015 Nicholas H. Tollervey
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

#include "py/mphal.h"
#include "py/objtuple.h"

#if MICROPY_PY_MUSIC

#define N(q) MP_ROM_QSTR(MP_QSTR_ ## q)
#define T(name, ...) const mp_obj_tuple_t microbit_music_tune_ ## name ## _obj = {{&mp_type_tuple}, .len = (sizeof((mp_obj_t[]){__VA_ARGS__})/sizeof(mp_obj_t)), .items = {__VA_ARGS__}};


T(dadadadum,
    N(r4_colon_2), N(g), N(g), N(g), N(eb_colon_8), N(r_colon_2), N(f), N(f),
    N(f), N(d_colon_8));

T(entertainer,
    N(d4_colon_1), N(d_hash_), N(e), N(c5_colon_2), N(e4_colon_1),
    N(c5_colon_2), N(e4_colon_1), N(c5_colon_3), N(c_colon_1), N(d),
    N(d_hash_), N(e), N(c), N(d), N(e_colon_2), N(b4_colon_1), N(d5_colon_2),
    N(c_colon_4));

T(prelude,
    N(c4_colon_1), N(e), N(g), N(c5), N(e), N(g4), N(c5), N(e), N(c4), N(e),
    N(g), N(c5), N(e), N(g4), N(c5), N(e), N(c4), N(d), N(g), N(d5), N(f),
    N(g4), N(d5), N(f), N(c4), N(d), N(g), N(d5), N(f), N(g4), N(d5), N(f),
    N(b3), N(d4), N(g), N(d5), N(f), N(g4), N(d5), N(f), N(b3), N(d4), N(g),
    N(d5), N(f), N(g4), N(d5), N(f), N(c4), N(e), N(g), N(c5), N(e), N(g4),
    N(c5), N(e), N(c4), N(e), N(g), N(c5), N(e), N(g4), N(c5), N(e));

T(ode,
    N(e4), N(e), N(f), N(g), N(g), N(f), N(e), N(d), N(c), N(c), N(d), N(e),
    N(e_colon_6), N(d_colon_2), N(d_colon_8), N(e_colon_4), N(e), N(f), N(g),
    N(g), N(f), N(e), N(d), N(c), N(c), N(d), N(e), N(d_colon_6),
    N(c_colon_2), N(c_colon_8));

T(nyan,
    N(f_hash_5_colon_2), N(g_hash_), N(c_hash__colon_1), N(d_hash__colon_2),
    N(b4_colon_1), N(d5_colon_1), N(c_hash_), N(b4_colon_2), N(b),
    N(c_hash_5), N(d), N(d_colon_1), N(c_hash_), N(b4_colon_1),
    N(c_hash_5_colon_1), N(d_hash_), N(f_hash_), N(g_hash_), N(d_hash_),
    N(f_hash_), N(c_hash_), N(d), N(b4), N(c_hash_5), N(b4),
    N(d_hash_5_colon_2), N(f_hash_), N(g_hash__colon_1), N(d_hash_),
    N(f_hash_), N(c_hash_), N(d_hash_), N(b4), N(d5), N(d_hash_), N(d),
    N(c_hash_), N(b4), N(c_hash_5), N(d_colon_2), N(b4_colon_1), N(c_hash_5),
    N(d_hash_), N(f_hash_), N(c_hash_), N(d), N(c_hash_), N(b4),
    N(c_hash_5_colon_2), N(b4), N(c_hash_5), N(b4), N(f_hash__colon_1),
    N(g_hash_), N(b_colon_2), N(f_hash__colon_1), N(g_hash_), N(b),
    N(c_hash_5), N(d_hash_), N(b4), N(e5), N(d_hash_), N(e), N(f_hash_),
    N(b4_colon_2), N(b), N(f_hash__colon_1), N(g_hash_), N(b), N(f_hash_),
    N(e5), N(d_hash_), N(c_hash_), N(b4), N(f_hash_), N(d_hash_), N(e),
    N(f_hash_), N(b_colon_2), N(f_hash__colon_1), N(g_hash_), N(b_colon_2),
    N(f_hash__colon_1), N(g_hash_), N(b), N(b), N(c_hash_5), N(d_hash_),
    N(b4), N(f_hash_), N(g_hash_), N(f_hash_), N(b_colon_2), N(b_colon_1),
    N(a_hash_), N(b), N(f_hash_), N(g_hash_), N(b), N(e5), N(d_hash_), N(e),
    N(f_hash_), N(b4_colon_2), N(c_hash_5));

T(ringtone,
    N(c4_colon_1), N(d), N(e_colon_2), N(g), N(d_colon_1), N(e), N(f_colon_2),
    N(a), N(e_colon_1), N(f), N(g_colon_2), N(b), N(c5_colon_4));

T(funk,
    N(c2_colon_2), N(c), N(d_hash_), N(c_colon_1), N(f_colon_2), N(c_colon_1),
    N(f_colon_2), N(f_hash_), N(g), N(c), N(c), N(g), N(c_colon_1),
    N(f_hash__colon_2), N(c_colon_1), N(f_hash__colon_2), N(f), N(d_hash_));

T(blues,
    N(c2_colon_2), N(e), N(g), N(a), N(a_hash_), N(a), N(g), N(e),
    N(c2_colon_2), N(e), N(g), N(a), N(a_hash_), N(a), N(g), N(e), N(f), N(a),
    N(c3), N(d), N(d_hash_), N(d), N(c), N(a2), N(c2_colon_2), N(e), N(g),
    N(a), N(a_hash_), N(a), N(g), N(e), N(g), N(b), N(d3), N(f), N(f2), N(a),
    N(c3), N(d_hash_), N(c2_colon_2), N(e), N(g), N(e), N(g), N(f), N(e),
    N(d));

T(birthday,
    N(c4_colon_3), N(c_colon_1), N(d_colon_4), N(c_colon_4), N(f),
    N(e_colon_8), N(c_colon_3), N(c_colon_1), N(d_colon_4), N(c_colon_4),
    N(g), N(f_colon_8), N(c_colon_3), N(c_colon_1), N(c5_colon_4), N(a4),
    N(f), N(e), N(d), N(a_hash__colon_3), N(a_hash__colon_1), N(a_colon_4),
    N(f), N(g), N(f_colon_8));

T(wedding,
    N(c4_colon_4), N(f_colon_3), N(f_colon_1), N(f_colon_8), N(c_colon_4),
    N(g_colon_3), N(e_colon_1), N(f_colon_8), N(c_colon_4), N(f_colon_3),
    N(a_colon_1), N(c5_colon_4), N(a4_colon_3), N(f_colon_1), N(f_colon_4),
    N(e_colon_3), N(f_colon_1), N(g_colon_8));

T(funeral,
    N(c3_colon_4), N(c_colon_3), N(c_colon_1), N(c_colon_4),
    N(d_hash__colon_3), N(d_colon_1), N(d_colon_3), N(c_colon_1),
    N(c_colon_3), N(b2_colon_1), N(c3_colon_4));

T(punchline,
    N(c4_colon_3), N(g3_colon_1), N(f_hash_), N(g), N(g_hash__colon_3), N(g),
    N(r), N(b), N(c4));

T(python,
    N(d5_colon_1), N(b4), N(r), N(b), N(b), N(a_hash_), N(b), N(g5), N(r),
    N(d), N(d), N(r), N(b4), N(c5), N(r), N(c), N(c), N(r), N(d),
    N(e_colon_5), N(c_colon_1), N(a4), N(r), N(a), N(a), N(g_hash_), N(a),
    N(f_hash_5), N(r), N(e), N(e), N(r), N(c), N(b4), N(r), N(b), N(b), N(r),
    N(c5), N(d_colon_5), N(d_colon_1), N(b4), N(r), N(b), N(b), N(a_hash_),
    N(b), N(b5), N(r), N(g), N(g), N(r), N(d), N(c_hash_), N(r), N(a), N(a),
    N(r), N(a), N(a_colon_5), N(g_colon_1), N(f_hash__colon_2), N(a_colon_1),
    N(a), N(g_hash_), N(a), N(e_colon_2), N(a_colon_1), N(a), N(g_hash_),
    N(a), N(d), N(r), N(c_hash_), N(d), N(r), N(c_hash_), N(d_colon_2),
    N(r_colon_3));

T(baddy,
    N(c3_colon_3), N(r), N(d_colon_2), N(d_hash_), N(r), N(c), N(r), N(f_hash__colon_8), );

T(chase,
    N(a4_colon_1), N(b), N(c5), N(b4), N(a_colon_2), N(r), N(a_colon_1), N(b), N(c5), N(b4), N(a_colon_2), N(r), N(a_colon_2), N(e5), N(d_hash_), N(e), N(f), N(e), N(d_hash_), N(e), N(b4_colon_1), N(c5), N(d), N(c), N(b4_colon_2), N(r), N(b_colon_1), N(c5), N(d), N(c), N(b4_colon_2), N(r), N(b_colon_2), N(e5), N(d_hash_), N(e), N(f), N(e), N(d_hash_), N(e), );

T(ba_ding,
    N(b5_colon_1), N(e6_colon_3), );

T(wawawawaa,
    N(e3_colon_3), N(r_colon_1), N(d_hash__colon_3), N(r_colon_1), N(d_colon_4), N(r_colon_1), N(c_hash__colon_8), );

T(jump_up,
    N(c5_colon_1), N(d), N(e), N(f), N(g), );

T(jump_down,
    N(g5_colon_1), N(f), N(e), N(d), N(c), );

T(power_up,
    N(g4_colon_1), N(c5), N(e), N(g_colon_2), N(e_colon_1), N(g_colon_3), );

T(power_down,
    N(g5_colon_1), N(d_hash_), N(c), N(g4_colon_2), N(b_colon_1), N(c5_colon_3), );

#undef N
#undef T

#endif // MICROPY_PY_MUSIC
