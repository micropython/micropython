/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
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
 *
 */

#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "py/mphal.h"
#include "extmod/modplatform.h"
#include "genhdr/mpversion.h"

#if MICROPY_PY_PLATFORM

// platform - Access to underlying platform's identifying data

static const MP_DEFINE_STR_OBJ(info_platform_obj, MICROPY_PLATFORM_SYSTEM "-" \
    MICROPY_VERSION_STRING "-" MICROPY_PLATFORM_ARCH "-" MICROPY_PLATFORM_VERSION "-with-" \
    MICROPY_PLATFORM_LIBC_LIB "" MICROPY_PLATFORM_LIBC_VER);
static const MP_DEFINE_STR_OBJ(info_python_compiler_obj, MICROPY_PLATFORM_COMPILER);
static const MP_DEFINE_STR_OBJ(info_libc_lib_obj, MICROPY_PLATFORM_LIBC_LIB);
static const MP_DEFINE_STR_OBJ(info_libc_ver_obj, MICROPY_PLATFORM_LIBC_VER);
static const mp_rom_obj_tuple_t info_libc_tuple_obj = {
    {&mp_type_tuple}, 2, {MP_ROM_PTR(&info_libc_lib_obj), MP_ROM_PTR(&info_libc_ver_obj)}
};

static mp_obj_t platform_platform(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return MP_OBJ_FROM_PTR(&info_platform_obj);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(platform_platform_obj, 0, platform_platform);

static mp_obj_t platform_python_compiler(void) {
    return MP_OBJ_FROM_PTR(&info_python_compiler_obj);
}
static MP_DEFINE_CONST_FUN_OBJ_0(platform_python_compiler_obj, platform_python_compiler);

static mp_obj_t platform_libc_ver(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return MP_OBJ_FROM_PTR(&info_libc_tuple_obj);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(platform_libc_ver_obj, 0, platform_libc_ver);

static const mp_rom_map_elem_t modplatform_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_platform) },
    { MP_ROM_QSTR(MP_QSTR_platform), MP_ROM_PTR(&platform_platform_obj) },
    { MP_ROM_QSTR(MP_QSTR_python_compiler), MP_ROM_PTR(&platform_python_compiler_obj) },
    { MP_ROM_QSTR(MP_QSTR_libc_ver), MP_ROM_PTR(&platform_libc_ver_obj) },
};

static MP_DEFINE_CONST_DICT(modplatform_globals, modplatform_globals_table);

const mp_obj_module_t mp_module_platform = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&modplatform_globals,
};

MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_platform, mp_module_platform);

#endif // MICROPY_PY_PLATFORM
