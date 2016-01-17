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

//#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_PY_URANDOM

// Yasmarang random number generator
// by Ilya Levin
// http://www.literatecode.com/yasmarang
// Public Domain

STATIC uint32_t yasmarang_pad = 0xeda4baba, yasmarang_n = 69, yasmarang_d = 233;
STATIC uint8_t yasmarang_dat = 0;

STATIC uint32_t yasmarang(void)
{
   yasmarang_pad += yasmarang_dat + yasmarang_d * yasmarang_n;
   yasmarang_pad = (yasmarang_pad<<3) + (yasmarang_pad>>29);
   yasmarang_n = yasmarang_pad | 2;
   yasmarang_d ^= (yasmarang_pad<<31) + (yasmarang_pad>>1);
   yasmarang_dat ^= (char) yasmarang_pad ^ (yasmarang_d>>8) ^ 1;

   return (yasmarang_pad^(yasmarang_d<<5)^(yasmarang_pad>>18)^(yasmarang_dat<<1));
}  /* yasmarang */

// End of Yasmarang

STATIC mp_obj_t mod_urandom_getrandbits(mp_obj_t num_in) {
    int n = mp_obj_get_int(num_in);
    if (n > 32 || n == 0) {
        nlr_raise(mp_obj_new_exception(&mp_type_ValueError));
    }
    uint32_t mask = ~0;
    // Beware of C undefined behavior when shifting by >= than bit size
    mask >>= (32 - n);
    return mp_obj_new_int_from_uint(yasmarang() & mask);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_urandom_getrandbits_obj, mod_urandom_getrandbits);

STATIC mp_obj_t mod_urandom_seed(mp_obj_t seed_in) {
    mp_uint_t seed = mp_obj_get_int_truncated(seed_in);
    yasmarang_pad = seed;
    yasmarang_n = 69;
    yasmarang_d = 233;
    yasmarang_dat = 0;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_urandom_seed_obj, mod_urandom_seed);

STATIC const mp_rom_map_elem_t mp_module_urandom_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_urandom) },
    { MP_ROM_QSTR(MP_QSTR_getrandbits), MP_ROM_PTR(&mod_urandom_getrandbits_obj) },
    { MP_ROM_QSTR(MP_QSTR_seed), MP_ROM_PTR(&mod_urandom_seed_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_urandom_globals, mp_module_urandom_globals_table);

const mp_obj_module_t mp_module_urandom = {
    .base = { &mp_type_module },
    .name = MP_QSTR_urandom,
    .globals = (mp_obj_dict_t*)&mp_module_urandom_globals,
};

#endif //MICROPY_PY_URANDOM
