/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019  Bernhard Boser
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

#include "shared-bindings/msgpack/__init__.h"
#include "shared-module/msgpack/__init__.h"

//| """Pack object in msgpack format
//|
//| The msgpack format is similar to json, except that the encoded data is binary.
//| See https://msgpack.org for details.
//|
//| Example:
//|    import msgpack
//|    from io import StringIO
//|
//|    s = StringIO()
//|    msgpack.pack({'list': [True, False, None, 1, 'abc'], 'str': 'blah'}, s)
//|    s.seek(0)
//|    print(msgpack.unpack(s))"""
//|

//| def pack(obj, stream):
//|     """Pack obj to stream."""
//|     ...
//|

STATIC mp_obj_t mod_msgpack_pack(mp_obj_t obj, mp_obj_t stream_obj) {
    common_hal_msgpack_pack(obj, stream_obj);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_msgpack_pack_obj, mod_msgpack_pack);


//| def pack(obj, stream) -> obj:
//|     """Unpack and return one object (in msgpack format) from stream.
//|     Call repeatedly to read multiple objects from the stream."""
//|     ...
//|

STATIC mp_obj_t mod_msgpack_unpack(mp_obj_t stream_obj) {
    return common_hal_msgpack_unpack(stream_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_msgpack_unpack_obj, mod_msgpack_unpack);


STATIC const mp_rom_map_elem_t msgpack_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_msgpack) },
    { MP_ROM_QSTR(MP_QSTR_pack), MP_ROM_PTR(&mod_msgpack_pack_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack), MP_ROM_PTR(&mod_msgpack_unpack_obj) },
};

STATIC MP_DEFINE_CONST_DICT(msgpack_module_globals, msgpack_module_globals_table);

const mp_obj_module_t msgpack_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&msgpack_module_globals,
};
