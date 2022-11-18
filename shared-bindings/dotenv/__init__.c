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
#include "shared-bindings/dotenv/__init__.h"

//| """Functions to manage environment variables from a .env file.
//|
//|    A subset of the CPython `dotenv library <https://saurabh-kumar.com/python-dotenv/>`_. It does
//|    not support variables or double quotes.
//|
//|    Keys and values may be put in single quotes.
//|    ``\`` and ``'`` are escaped by ``\`` in single quotes. Newlines can occur in quotes for multiline values.
//|    Comments start with ``#`` and apply for the rest of the line.
//|    A ``#`` immediately following an ``=`` is part of the value, not the start of a comment,
//|    and a ``#`` embedded in a value without whitespace will be part of that value.
//|    This corresponds to how assignments and comments work in most Unix shells.
//|
//|
//|    File format example:
//|
//|    .. code-block::
//|
//|        key=value
//|          key2 = value2
//|        'key3' = 'value with spaces'
//|        # comment
//|        key4 = value3 # comment 2
//|        'key5'=value4
//|        key=value5 # overrides the first one
//|        multiline = 'hello
//|        world
//|        how are you?'
//|        # The #'s below will be included in the value. They do not start a comment.
//|        key6=#value
//|        key7=abc#def
//|
//| """
//|
//| import typing
//|

//| def get_key(dotenv_path: str, key_to_get: str) -> Optional[str]:
//|     """Get the value for the given key from the given .env file. If the key occurs multiple
//|     times in the file, then the last value will be returned.
//|
//|     Returns None if the key isn't found or doesn't have a value."""
//|     ...
//|
STATIC mp_obj_t _dotenv_get_key(mp_obj_t path_in, mp_obj_t key_to_get_in) {
    return common_hal_dotenv_get_key(mp_obj_str_get_str(path_in),
        mp_obj_str_get_str(key_to_get_in));
}
MP_DEFINE_CONST_FUN_OBJ_2(dotenv_get_key_obj, _dotenv_get_key);

//| def load_dotenv() -> None:
//|     """Does nothing in CircuitPython because os.getenv will automatically read .env when
//|     available.
//|
//|     Present in CircuitPython so CPython-compatible code can use it without error."""
//|     ...
//|
STATIC mp_obj_t dotenv_load_dotenv(void) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(dotenv_load_dotenv_obj, dotenv_load_dotenv);

STATIC const mp_rom_map_elem_t dotenv_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dotenv) },

    { MP_ROM_QSTR(MP_QSTR_get_key), MP_ROM_PTR(&dotenv_get_key_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_dotenv), MP_ROM_PTR(&dotenv_load_dotenv_obj) },
};

STATIC MP_DEFINE_CONST_DICT(dotenv_module_globals, dotenv_module_globals_table);

const mp_obj_module_t dotenv_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&dotenv_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_dotenv, dotenv_module, CIRCUITPY_DOTENV);
