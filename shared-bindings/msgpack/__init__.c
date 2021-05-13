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

#include <stdio.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/msgpack/__init__.h"
#include "shared-module/msgpack/__init__.h"
#include "shared-bindings/msgpack/ExtType.h"

#define MP_OBJ_IS_METH(o) (mp_obj_is_obj(o) && (((mp_obj_base_t *)MP_OBJ_TO_PTR(o))->type->name == MP_QSTR_bound_method))

//| """Pack object in msgpack format
//|
//| The msgpack format is similar to json, except that the encoded data is binary.
//| See https://msgpack.org for details. The module implements a subset of the cpython
//| module msgpack-python.
//|
//| Not implemented: 64-bit int, uint, float.
//|
//| Example 1::
//|
//|    import msgpack
//|    from io import BytesIO
//|
//|    b = BytesIO()
//|    msgpack.pack({'list': [True, False, None, 1, 3.14], 'str': 'blah'}, b)
//|    b.seek(0)
//|    print(msgpack.unpack(b))
//|
//| Example 2: handling objects::
//|
//|    from msgpack import pack, unpack, ExtType
//|    from io import BytesIO
//|
//|    class MyClass:
//|        def __init__(self, val):
//|            self.value = val
//|        def __str__(self):
//|            return str(self.value)
//|
//|    data = MyClass(b'my_value')
//|
//|    def encoder(obj):
//|        if isinstance(obj, MyClass):
//|            return ExtType(1, obj.value)
//|        return f"no encoder for {obj}"
//|
//|    def decoder(code, data):
//|        if code == 1:
//|            return MyClass(data)
//|        return f"no decoder for type {code}"
//|
//|    buffer = BytesIO()
//|    pack(data, buffer, default=encoder)
//|    buffer.seek(0)
//|    decoded = unpack(buffer, ext_hook=decoder)
//|    print(f"{data} -> {buffer.getvalue()} -> {decoded}")
//|
//| """
//|

//| def pack(obj: object, buffer: WriteableBuffer, *, default: Union[Callable[[object], None], None] = None) -> None:
//|     """Ouput object to buffer in msgpack format.
//|
//|     :param object obj: Object to convert to msgpack format.
//|     :param ~_typing.WriteableBuffer buffer: buffer to write into
//|     :param Optional[~_typing.Callable[[object], None]] default:
//|           function called for python objects that do not have
//|           a representation in msgpack format.
//|     """
//|     ...
//|
STATIC mp_obj_t mod_msgpack_pack(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_obj, ARG_buffer, ARG_default };
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_obj, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_default, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t handler = args[ARG_default].u_obj;
    if (handler != mp_const_none && !mp_obj_is_fun(handler) && !MP_OBJ_IS_METH(handler)) {
        mp_raise_ValueError(translate("default is not a function"));
    }

    common_hal_msgpack_pack(args[ARG_obj].u_obj, args[ARG_buffer].u_obj, handler);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mod_msgpack_pack_obj, 1, mod_msgpack_pack);


//| def unpack(buffer: ReadableBuffer, *, ext_hook: Union[Callable[[int, bytes], object], None] = None, use_list: bool=True) -> object:
//|     """Unpack and return one object from buffer.
//|
//|     :param ~_typing.ReadableBuffer buffer: buffer to read from
//|     :param Optional[~_typing.Callable[[int, bytes], object]] ext_hook: function called for objects in
//|            msgpack ext format.
//|     :param Optional[bool] use_list: return array as list or tuple (use_list=False).
//|
//|     :return object: object read from buffer.
//|     """
//|     ...
//|
STATIC mp_obj_t mod_msgpack_unpack(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_ext_hook, ARG_use_list };
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_ext_hook, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_use_list, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_bool = true } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t hook = args[ARG_ext_hook].u_obj;
    if (hook != mp_const_none && !mp_obj_is_fun(hook) && !MP_OBJ_IS_METH(hook)) {
        mp_raise_ValueError(translate("ext_hook is not a function"));
    }

    return common_hal_msgpack_unpack(args[ARG_buffer].u_obj, hook, args[ARG_use_list].u_bool);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mod_msgpack_unpack_obj, 1, mod_msgpack_unpack);


STATIC const mp_rom_map_elem_t msgpack_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_msgpack) },
    { MP_ROM_QSTR(MP_QSTR_ExtType), MP_ROM_PTR(&mod_msgpack_exttype_type) },
    { MP_ROM_QSTR(MP_QSTR_pack), MP_ROM_PTR(&mod_msgpack_pack_obj) },
    { MP_ROM_QSTR(MP_QSTR_unpack), MP_ROM_PTR(&mod_msgpack_unpack_obj) },
};

STATIC MP_DEFINE_CONST_DICT(msgpack_module_globals, msgpack_module_globals_table);

const mp_obj_module_t msgpack_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&msgpack_module_globals,
};
