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

#include <stdio.h>

#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_PY_UZLIB

#include "uzlib/tinf.h"

#if 0 // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

STATIC int mod_uzlib_grow_buf(TINF_DATA *d, unsigned alloc_req) {
    if (alloc_req < 256) {
        alloc_req = 256;
    }
    DEBUG_printf("uzlib: Resizing buffer to " UINT_FMT " bytes\n", d->destSize + alloc_req);
    d->destStart = m_renew(byte, d->destStart, d->destSize, d->destSize + alloc_req);
    d->destSize += alloc_req;
    return 0;
}

STATIC mp_obj_t mod_uzlib_decompress(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_obj_t data = args[0];
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    TINF_DATA *decomp = m_new_obj(TINF_DATA);
    DEBUG_printf("sizeof(TINF_DATA)=" UINT_FMT "\n", sizeof(*decomp));

    decomp->destSize = (bufinfo.len + 15) & ~15;
    decomp->destStart = m_new(byte, decomp->destSize);
    DEBUG_printf("uzlib: Initial out buffer: " UINT_FMT " bytes\n", decomp->destSize);
    decomp->destGrow = mod_uzlib_grow_buf;
    decomp->source = bufinfo.buf;

    int st;
    if (n_args > 1 && MP_OBJ_SMALL_INT_VALUE(args[1]) < 0) {
        st = tinf_uncompress_dyn(decomp);
    } else {
        st = tinf_zlib_uncompress_dyn(decomp, bufinfo.len);
    }
    if (st != 0) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_ValueError, MP_OBJ_NEW_SMALL_INT(st)));
    }

    mp_uint_t final_sz = decomp->dest - decomp->destStart;
    DEBUG_printf("uzlib: Resizing from " UINT_FMT " to final size: " UINT_FMT " bytes\n", decomp->destSize, final_sz);
    decomp->destStart = (byte*)m_renew(byte, decomp->destStart, decomp->destSize, final_sz);
    mp_obj_t res = mp_obj_new_bytearray_by_ref(final_sz, decomp->destStart);
    m_del_obj(TINF_DATA, decomp);
    return res;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_uzlib_decompress_obj, 1, 3, mod_uzlib_decompress);

STATIC const mp_rom_map_elem_t mp_module_uzlib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uzlib) },
    { MP_ROM_QSTR(MP_QSTR_decompress), MP_ROM_PTR(&mod_uzlib_decompress_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_uzlib_globals, mp_module_uzlib_globals_table);

const mp_obj_module_t mp_module_uzlib = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uzlib,
    .globals = (mp_obj_dict_t*)&mp_module_uzlib_globals,
};

// Source files #include'd here to make sure they're compiled in
// only if module is enabled by config setting.

#include "uzlib/tinflate.c"
#include "uzlib/tinfzlib.c"
#include "uzlib/adler32.c"

#endif // MICROPY_PY_UZLIB
