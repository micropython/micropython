/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include <assert.h>
#include <string.h>

#include "py/runtime.h"

#if MICROPY_PY_RANDOM

// Work out if the seed will be set on import or not.
#if MICROPY_MODULE_BUILTIN_INIT && defined(MICROPY_PY_RANDOM_SEED_INIT_FUNC)
#define SEED_ON_IMPORT (1)
#else
#define SEED_ON_IMPORT (0)
#endif

// Yasmarang random number generator
// by Ilya Levin
// http://www.literatecode.com/yasmarang
// Public Domain

#if !MICROPY_ENABLE_DYNRUNTIME
#if SEED_ON_IMPORT
// If the state is seeded on import then keep these variables in the BSS.
static uint32_t yasmarang_pad, yasmarang_n, yasmarang_d;
static uint8_t yasmarang_dat;
#else
// Without seed-on-import these variables must be initialised via the data section.
static uint32_t yasmarang_pad = 0xeda4baba, yasmarang_n = 69, yasmarang_d = 233;
static uint8_t yasmarang_dat = 0;
#endif
#endif

static uint32_t yasmarang(void) {
    yasmarang_pad += yasmarang_dat + yasmarang_d * yasmarang_n;
    yasmarang_pad = (yasmarang_pad << 3) + (yasmarang_pad >> 29);
    yasmarang_n = yasmarang_pad | 2;
    yasmarang_d ^= (yasmarang_pad << 31) + (yasmarang_pad >> 1);
    yasmarang_dat ^= (char)yasmarang_pad ^ (yasmarang_d >> 8) ^ 1;

    return yasmarang_pad ^ (yasmarang_d << 5) ^ (yasmarang_pad >> 18) ^ (yasmarang_dat << 1);
}  /* yasmarang */

// End of Yasmarang

#if MICROPY_PY_RANDOM_EXTRA_FUNCS

// returns an unsigned integer below the given argument
// n must not be zero
static uint32_t yasmarang_randbelow(uint32_t n) {
    uint32_t mask = 1;
    while ((n & mask) < n) {
        mask = (mask << 1) | 1;
    }
    uint32_t r;
    do {
        r = yasmarang() & mask;
    } while (r >= n);
    return r;
}

#endif

static mp_obj_t mod_random_getrandbits(mp_obj_t num_in) {
    mp_int_t n = mp_obj_get_int(num_in);
    if (n > 32 || n < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits must be 32 or less"));
    }
    if (n == 0) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }
    uint32_t mask = ~0;
    // Beware of C undefined behavior when shifting by >= than bit size
    mask >>= (32 - n);
    return mp_obj_new_int_from_uint(yasmarang() & mask);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_random_getrandbits_obj, mod_random_getrandbits);

static mp_obj_t mod_random_seed(size_t n_args, const mp_obj_t *args) {
    mp_uint_t seed;
    if (n_args == 0 || args[0] == mp_const_none) {
        #ifdef MICROPY_PY_RANDOM_SEED_INIT_FUNC
        seed = MICROPY_PY_RANDOM_SEED_INIT_FUNC;
        #else
        mp_raise_ValueError(MP_ERROR_TEXT("no default seed"));
        #endif
    } else {
        seed = mp_obj_get_int_truncated(args[0]);
    }
    yasmarang_pad = (uint32_t)seed;
    yasmarang_n = 69;
    yasmarang_d = 233;
    yasmarang_dat = 0;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_random_seed_obj, 0, 1, mod_random_seed);

#if MICROPY_PY_RANDOM_EXTRA_FUNCS

static mp_obj_t mod_random_randrange(size_t n_args, const mp_obj_t *args) {
    mp_int_t start = mp_obj_get_int(args[0]);
    if (n_args == 1) {
        // range(stop)
        if (start > 0) {
            return mp_obj_new_int(yasmarang_randbelow((uint32_t)start));
        } else {
            goto error;
        }
    } else {
        mp_int_t stop = mp_obj_get_int(args[1]);
        if (n_args == 2) {
            // range(start, stop)
            if (start < stop) {
                return mp_obj_new_int(start + yasmarang_randbelow((uint32_t)(stop - start)));
            } else {
                goto error;
            }
        } else {
            // range(start, stop, step)
            mp_int_t step = mp_obj_get_int(args[2]);
            mp_int_t n;
            if (step > 0) {
                n = (stop - start + step - 1) / step;
            } else if (step < 0) {
                n = (stop - start + step + 1) / step;
            } else {
                goto error;
            }
            if (n > 0) {
                return mp_obj_new_int(start + step * yasmarang_randbelow((uint32_t)n));
            } else {
                goto error;
            }
        }
    }

error:
    mp_raise_ValueError(NULL);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_random_randrange_obj, 1, 3, mod_random_randrange);

static mp_obj_t mod_random_randint(mp_obj_t a_in, mp_obj_t b_in) {
    mp_int_t a = mp_obj_get_int(a_in);
    mp_int_t b = mp_obj_get_int(b_in);
    if (a <= b) {
        return mp_obj_new_int(a + yasmarang_randbelow((uint32_t)(b - a + 1)));
    } else {
        mp_raise_ValueError(NULL);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_2(mod_random_randint_obj, mod_random_randint);

static mp_obj_t mod_random_choice(mp_obj_t seq) {
    mp_int_t len = mp_obj_get_int(mp_obj_len(seq));
    if (len > 0) {
        return mp_obj_subscr(seq, mp_obj_new_int(yasmarang_randbelow((uint32_t)len)), MP_OBJ_SENTINEL);
    } else {
        mp_raise_type(&mp_type_IndexError);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(mod_random_choice_obj, mod_random_choice);

#if MICROPY_PY_BUILTINS_FLOAT

// returns a number in the range [0..1) using Yasmarang to fill in the fraction bits
static mp_float_t yasmarang_float(void) {
    mp_float_union_t u;
    u.p.sgn = 0;
    u.p.exp = (1 << (MP_FLOAT_EXP_BITS - 1)) - 1;
    if (MP_FLOAT_FRAC_BITS <= 32) {
        u.p.frc = yasmarang();
    } else {
        u.p.frc = ((uint64_t)yasmarang() << 32) | (uint64_t)yasmarang();
    }
    return u.f - 1;
}

static mp_obj_t mod_random_random(void) {
    return mp_obj_new_float(yasmarang_float());
}
static MP_DEFINE_CONST_FUN_OBJ_0(mod_random_random_obj, mod_random_random);

static mp_obj_t mod_random_uniform(mp_obj_t a_in, mp_obj_t b_in) {
    mp_float_t a = mp_obj_get_float(a_in);
    mp_float_t b = mp_obj_get_float(b_in);
    return mp_obj_new_float(a + (b - a) * yasmarang_float());
}
static MP_DEFINE_CONST_FUN_OBJ_2(mod_random_uniform_obj, mod_random_uniform);

#endif

#endif // MICROPY_PY_RANDOM_EXTRA_FUNCS

#if SEED_ON_IMPORT
static mp_obj_t mod_random___init__(void) {
    // This module may be imported by more than one name so need to ensure
    // that it's only ever seeded once.
    static bool seeded = false;
    if (!seeded) {
        seeded = true;
        mod_random_seed(0, NULL);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(mod_random___init___obj, mod_random___init__);
#endif

#if !MICROPY_ENABLE_DYNRUNTIME
static const mp_rom_map_elem_t mp_module_random_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_random) },
    #if SEED_ON_IMPORT
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&mod_random___init___obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_getrandbits), MP_ROM_PTR(&mod_random_getrandbits_obj) },
    { MP_ROM_QSTR(MP_QSTR_seed), MP_ROM_PTR(&mod_random_seed_obj) },
    #if MICROPY_PY_RANDOM_EXTRA_FUNCS
    { MP_ROM_QSTR(MP_QSTR_randrange), MP_ROM_PTR(&mod_random_randrange_obj) },
    { MP_ROM_QSTR(MP_QSTR_randint), MP_ROM_PTR(&mod_random_randint_obj) },
    { MP_ROM_QSTR(MP_QSTR_choice), MP_ROM_PTR(&mod_random_choice_obj) },
    #if MICROPY_PY_BUILTINS_FLOAT
    { MP_ROM_QSTR(MP_QSTR_random), MP_ROM_PTR(&mod_random_random_obj) },
    { MP_ROM_QSTR(MP_QSTR_uniform), MP_ROM_PTR(&mod_random_uniform_obj) },
    #endif
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_random_globals, mp_module_random_globals_table);

const mp_obj_module_t mp_module_random = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_random_globals,
};

MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_random, mp_module_random);
#endif

#endif // MICROPY_PY_RANDOM
