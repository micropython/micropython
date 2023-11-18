/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
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

#include "py/obj.h"
#include "py/objtuple.h"

//| """Locale support module"""
//|
//| def getlocale() -> None:
//|     """Returns the current locale setting as a tuple ``(language code, "utf-8")``
//|
//|     The language code comes from the installed translation of CircuitPython, specifically the "Language:" code specified in the translation metadata.
//|     This can be useful to allow modules coded in Python to show messages in the user's preferred language.
//|
//|     Differences from CPython: No ``LC_*`` argument is permitted.
//|     """
//|
STATIC mp_obj_t getlocale(void) {

    mp_rom_error_text_t locale_msg = MP_ERROR_TEXT("en_US");
    size_t len_with_nul = decompress_length(locale_msg);
    size_t len = len_with_nul - 1;
    char buf[len_with_nul];
    decompress(locale_msg, buf);

    mp_obj_t elements[] = {
        mp_obj_new_str(buf, len),
        MP_OBJ_NEW_QSTR(MP_QSTR_utf_hyphen_8)
    };
    return mp_obj_new_tuple(MP_ARRAY_SIZE(elements), elements);
}
MP_DEFINE_CONST_FUN_OBJ_0(getlocale_obj, getlocale);

STATIC const mp_rom_map_elem_t locale_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_locale) },
    { MP_ROM_QSTR(MP_QSTR_getlocale), MP_ROM_PTR(&getlocale_obj) },
};

STATIC MP_DEFINE_CONST_DICT(locale_module_globals, locale_module_globals_table);

const mp_obj_module_t locale_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&locale_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_locale, locale_module);
