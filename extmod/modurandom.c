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

STATIC uint32_t pad = 0xeda4baba, n = 69, d = 233;
STATIC uint8_t dat = 0;

uint32_t yasmarang(void)
{
   pad += dat + d * n;
   pad = (pad<<3) + (pad>>29);
   n = pad | 2;
   d ^= (pad<<31) + (pad>>1);
   dat ^= (char) pad ^ (d>>8) ^ 1;

   return (pad^(d<<5)^(pad>>18)^(dat<<1));
}  /* yasmarang */

// End of Yasmarang

STATIC mp_obj_t mod_urandom_getrandbits(mp_obj_t num_in) {
    int n = MP_OBJ_SMALL_INT_VALUE(num_in);
    if (n > 30) {
        nlr_raise(mp_obj_new_exception(&mp_type_ValueError));
    }
    return MP_OBJ_NEW_SMALL_INT(yasmarang() & ((1 << n) - 1));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_urandom_getrandbits_obj, mod_urandom_getrandbits);

STATIC mp_obj_t mod_urandom_seed(mp_obj_t seed_in) {
    mp_uint_t seed = mp_obj_get_int(seed_in);
    pad = seed;
    n = 69;
    d = 233;
    dat = 0;
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
