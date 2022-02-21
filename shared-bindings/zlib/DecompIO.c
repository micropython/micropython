/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Mark Komus
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

#include <string.h>
#include "py/obj.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "shared-bindings/zlib/DecompIO.h"
#include "shared-module/zlib/DecompIO.h"

//| class DecompIO:
//|     """A stream wrapper which allows transparent decompression of compressed data in
//|        another stream. This allows to process compressed streams with data larger
//|        than available heap size.
//|
//|        *wbits* is DEFLATE dictionary window size used during compression (8-15, the
//|        dictionary size is power of 2 of that value). Additionally, if value is positive,
//|        *data* is assumed to be zlib stream (with zlib header). Otherwise, if it's
//|        negative, it's assumed to be raw DEFLATE stream. *wbits* values 24..31 (16 + 8..15)
//|        mean that input stream has gzip header."""
//|
//|     def __init__(self, stream: typing.IO, wbits: Optional[int] = 0) -> None:
//|         """Creates a DecompIO object to decompress stream data.
//|         """
//|         ...
//|

// These are standard stream methods. Code is in py/stream.c.
//
//|     def read(self, nbytes: Optional[int] = None) -> Optional[bytes]:
//|         """Read characters.  If ``nbytes`` is specified then read at most that many
//|         bytes. Otherwise, read everything that arrives until the connection
//|         times out. Providing the number of bytes expected is highly recommended
//|         because it will be faster.
//|
//|         :return: Data read
//|         :rtype: bytes or None"""
//|         ...
//|
//|     def readinto(self, buf: WriteableBuffer, nbytes: Optional[int] = None) -> Optional[bytes]:
//|         """Read bytes into the ``buf``.  If ``nbytes`` is specified then read at most
//|         that many bytes.  Otherwise, read at most ``len(buf)`` bytes.
//|
//|         :return: number of bytes read and stored into ``buf``
//|         :rtype: bytes or None"""
//|         ...
//|
//|     def readline(self, nbytes: Optional[int] = None) -> Optional[bytes]:
//|         """Read and return one line from the stream. The line terminator is always b'\\n'.
//|         If ``nbytes`` is specified then read at most that many bytes.
//|
//|         :return: Data read
//|         :rtype: bytes or None"""
//|         ...
//|

STATIC mp_obj_t zlib_decompio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    mp_get_stream_raise(args[0], MP_STREAM_OP_READ);

    mp_obj_decompio_t *self = m_new_obj(mp_obj_decompio_t);
    self->base.type = type;
    memset(&self->decomp, 0, sizeof(self->decomp));

    mp_int_t dict_opt = 0;
    if (n_args > 1) {
        dict_opt = mp_obj_get_int(args[1]);
    }

    common_hal_zlib_decompio_construct(self, args[0], dict_opt);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_uint_t zlib_decompio_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_decompio_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->eof) {
        return 0;
    }

    return common_hal_zlib_decompio_read(self, buf, size, errcode);
}

STATIC const mp_rom_map_elem_t decompio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
};

STATIC MP_DEFINE_CONST_DICT(decompio_locals_dict, decompio_locals_dict_table);

STATIC const mp_stream_p_t decompio_stream_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = zlib_decompio_read,
    .write = NULL,
    .ioctl = NULL,
    .is_text = false,
};

const mp_obj_type_t decompio_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_DecompIO,
    .make_new = zlib_decompio_make_new,
    .locals_dict = (void *)&decompio_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &decompio_stream_p,
        ),
};
