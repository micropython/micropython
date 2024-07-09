// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Mark Komus
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/builtin.h"
#include "py/objtuple.h"
#include "py/binary.h"
#include "py/parsenum.h"

#include "shared-bindings/zlib/__init__.h"

#define UZLIB_CONF_PARANOID_CHECKS (1)
#include "lib/uzlib/tinf.h"

#if 0 // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

mp_obj_t common_hal_zlib_decompress(mp_obj_t data, mp_int_t wbits) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    TINF_DATA *decomp = m_new_obj(TINF_DATA);
    memset(decomp, 0, sizeof(*decomp));
    DEBUG_printf("sizeof(TINF_DATA)=" UINT_FMT "\n", sizeof(*decomp));
    uzlib_uncompress_init(decomp, NULL, 0);
    mp_uint_t dest_buf_size = (bufinfo.len + 15) & ~15;
    byte *dest_buf = m_new(byte, dest_buf_size);

    decomp->dest = dest_buf;
    decomp->dest_limit = dest_buf + dest_buf_size;
    DEBUG_printf("zlib: Initial out buffer: " UINT_FMT " bytes\n", decomp->destSize);
    decomp->source = bufinfo.buf;
    decomp->source_limit = (unsigned char *)bufinfo.buf + bufinfo.len;
    int st;

    if (wbits >= 16) {
        st = uzlib_gzip_parse_header(decomp);
        if (st < 0) {
            goto error;
        }
    } else if (wbits >= 0) {
        st = uzlib_zlib_parse_header(decomp);
        if (st < 0) {
            goto error;
        }
    }

    while (1) {
        st = uzlib_uncompress_chksum(decomp);
        if (st < 0) {
            goto error;
        }
        if (st == TINF_DONE) {
            break;
        }
        size_t offset = decomp->dest - dest_buf;
        dest_buf = m_renew(byte, dest_buf, dest_buf_size, dest_buf_size + 256);
        dest_buf_size += 256;
        decomp->dest = dest_buf + offset;
        decomp->dest_limit = dest_buf + offset + 256;
    }

    mp_uint_t final_sz = decomp->dest - dest_buf;
    DEBUG_printf("zlib: Resizing from " UINT_FMT " to final size: " UINT_FMT " bytes\n", dest_buf_size, final_sz);
    dest_buf = (byte *)m_renew(byte, dest_buf, dest_buf_size, final_sz);
    mp_obj_t res = mp_obj_new_bytearray_by_ref(final_sz, dest_buf);
    m_del_obj(TINF_DATA, decomp);
    return res;

error:
    mp_raise_type_arg(&mp_type_ValueError, MP_OBJ_NEW_SMALL_INT(st));
}
