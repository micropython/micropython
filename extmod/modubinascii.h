/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
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

#ifndef MICROPY_EXTMOD_MODUBINASCII
#define MICROPY_EXTMOD_MODUBINASCII

extern mp_obj_t mod_binascii_hexlify(size_t n_args, const mp_obj_t *args);
extern mp_obj_t mod_binascii_unhexlify(mp_obj_t data);
extern mp_obj_t mod_binascii_a2b_base64(mp_obj_t data);
extern mp_obj_t mod_binascii_b2a_base64(mp_obj_t data);

MP_DECLARE_CONST_FUN_OBJ(mod_binascii_hexlify_obj);
MP_DECLARE_CONST_FUN_OBJ(mod_binascii_unhexlify_obj);
MP_DECLARE_CONST_FUN_OBJ(mod_binascii_a2b_base64_obj);
MP_DECLARE_CONST_FUN_OBJ(mod_binascii_b2a_base64_obj);

#endif /* MICROPY_EXTMOD_MODUBINASCII */
