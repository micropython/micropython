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
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

#if MICROPY_PY_ZLIBD

#include "miniz/tinfl.c"

#if 0 // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

STATIC mp_obj_t mod_zlibd_decompress(uint n_args, mp_obj_t *args) {
    mp_obj_t data = args[0];
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
    tinfl_decompressor *decomp = m_new_obj(tinfl_decompressor);
    tinfl_init(decomp);
    DEBUG_printf("sizeof(tinfl_decompressor)=" UINT_FMT "\n", sizeof(tinfl_decompressor));

    byte *out = m_new(byte, bufinfo.len);
    size_t out_len = bufinfo.len;
    size_t in_buf_ofs = 0, dst_buf_ofs = 0;
    size_t dst_buf_sz = bufinfo.len;

    while (1) {
        size_t in_buf_sz = bufinfo.len - in_buf_ofs;
        DEBUG_printf("tinfl in: in_ofs=%d in_sz=%d dst_ofs=%d, dst_sz=%d\n", in_buf_ofs, in_buf_sz, dst_buf_ofs, dst_buf_sz);
        tinfl_status st = tinfl_decompress(decomp,
            (mz_uint8*) bufinfo.buf + in_buf_ofs, &in_buf_sz,
            out, out + dst_buf_ofs, &dst_buf_sz,
            TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF | TINFL_FLAG_PARSE_ZLIB_HEADER);
        DEBUG_printf("tinfl out: st=%d, in_sz=%d, out_sz=%d\n", st, in_buf_sz, dst_buf_sz);
        in_buf_ofs += in_buf_sz;
        dst_buf_ofs += dst_buf_sz;
        if (st != TINFL_STATUS_HAS_MORE_OUTPUT) {
            break;
        }
        out = m_renew(byte, out, out_len, dst_buf_ofs + 256);
        out_len = dst_buf_ofs + 256;
        dst_buf_sz = out_len - dst_buf_ofs;
    }

    m_del_obj(tinfl_decompressor, decomp);
    return mp_obj_new_bytearray_by_ref(dst_buf_ofs, out);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_zlibd_decompress_obj, 1, 3, mod_zlibd_decompress);

STATIC const mp_map_elem_t mp_module_zlibd_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_zlibd) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_decompress), (mp_obj_t)&mod_zlibd_decompress_obj },
};

STATIC const mp_obj_dict_t mp_module_zlibd_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(mp_module_zlibd_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_zlibd_globals_table),
        .table = (mp_map_elem_t*)mp_module_zlibd_globals_table,
    },
};

const mp_obj_module_t mp_module_zlibd = {
    .base = { &mp_type_module },
    .name = MP_QSTR_zlibd,
    .globals = (mp_obj_dict_t*)&mp_module_zlibd_globals,
};

#endif //MICROPY_PY_ZLIBD
