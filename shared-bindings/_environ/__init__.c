/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

#include "extmod/vfs.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "py/mpstate.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/_environ/__init__.h"

//| """Functions to manage environment variables from a settings.toml file.
//|
//|    This library can read a subset of `toml files <https://toml.io/>`_.
//|
//|    It is recommended to not use this module directly. Instead, retrieve string
//|    values using `os.getenv`.
//|
//|    Due to technical limitations it probably also accepts some files that are
//|    not valid TOML files; bugs of this nature are subject to change (i.e., be
//|    fixed) without the usual deprecation period for incompatible changes.
//|
//|    Details of the format understood by this module:
//|     * the content is required to be in UTF-8 encoding
//|     * the supported data types are string and integer
//|     * only integers and basic strings are supported
//|     * only integers supported by strtol are supported (no 0o, no 0b, no
//|       underscores 1_000, 011 is 9, not 11)
//|     * In quoted strings, all standard eescape codes, including ``\\uxxxx``
//|       and ``\\Uxxxxxxxx``, are supported
//|     * only bare keys are supported
//|     * duplicate keys are not diagnosed.
//|     * comments are supported
//|     * only values from the "root table" can be retrieved (parsing ends when it
//|       encounters a line starting with [)
//|     * due to technical limitations, the content of multi-line
//|       strings can erroneously be parsed as a value.
//|
//|    File format example:
//|
//|    .. code-block::
//|
//|        str_key="Hello world" # with trailing comment
//|        int_key = 7
//|        unicode_key="👨"
//|        unicode_key2="\\U0001f468" # same as above
//|        escape_codes="supported, including \\r\\n\\"\\\\"
//|        # comment
//|        [subtable]
//|        subvalue="cannot retrieve this using _environ or getenv"
//|
//| """
//|
//| import typing
//|

//| def get_key(_environ_path: str, key_to_get: str) -> Union[str, int, None]:
//|     """Get the value for the given key from the given .env file.
//|
//|     Returns None if the key isn't found"""
//|     ...
//|
STATIC mp_obj_t __environ_get_key(mp_obj_t path_in, mp_obj_t key_to_get_in) {
    return common_hal__environ_get_key_path(mp_obj_str_get_str(path_in),
        mp_obj_str_get_str(key_to_get_in));
}
MP_DEFINE_CONST_FUN_OBJ_2(_environ_get_key_obj, __environ_get_key);

STATIC const mp_rom_map_elem_t _environ_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__environ) },

    { MP_ROM_QSTR(MP_QSTR_get_key), MP_ROM_PTR(&_environ_get_key_obj) },
};

STATIC MP_DEFINE_CONST_DICT(_environ_module_globals, _environ_module_globals_table);

const mp_obj_module_t _environ_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&_environ_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__environ, _environ_module, CIRCUITPY_ENVIRON);
