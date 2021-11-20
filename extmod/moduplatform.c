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
#include "genhdr/mpversion.h"

#if MICROPY_PY_UPLATFORM

// platform - Access to underlying platform's identifying data

// TODO: Add more architectures, compilers and libraries.
// See: https://sourceforge.net/p/predef/wiki/Home/

#if defined(__ARM_ARCH)
#define PLATFORM_ARCH   "arm"
#elif defined(__x86_64__) || defined(_WIN64)
#define PLATFORM_ARCH   "x86_64"
#elif defined(__i386__) || defined(_M_IX86)
#define PLATFORM_ARCH   "x86"
#else
#define PLATFORM_ARCH   ""
#endif

#if defined(__GNUC__)
#define PLATFORM_COMPILER \
    "GCC " \
    MP_STRINGIFY(__GNUC__) "." \
    MP_STRINGIFY(__GNUC_MINOR__) "." \
    MP_STRINGIFY(__GNUC_PATCHLEVEL__)
#elif defined(__ARMCC_VERSION)
#define PLATFORM_COMPILER \
    "ARMCC " \
    MP_STRINGIFY((__ARMCC_VERSION / 1000000)) "." \
    MP_STRINGIFY((__ARMCC_VERSION / 10000 % 100)) "." \
    MP_STRINGIFY((__ARMCC_VERSION % 10000))
#elif defined(_MSC_VER)
#if defined(_WIN64)
#define COMPILER_BITS "64 bit"
#elif defined(_M_IX86)
#define COMPILER_BITS "32 bit"
#else
#define COMPILER_BITS ""
#endif
#define PLATFORM_COMPILER \
    "MSC v." MP_STRINGIFY(_MSC_VER) " " COMPILER_BITS
#else
#define PLATFORM_COMPILER       ""
#endif

#if defined(__GLIBC__)
#define PLATFORM_LIBC_LIB       "glibc"
#define PLATFORM_LIBC_VER \
    MP_STRINGIFY(__GLIBC__) "." \
    MP_STRINGIFY(__GLIBC_MINOR__)
#elif defined(__NEWLIB__)
#define PLATFORM_LIBC_LIB       "newlib"
#define PLATFORM_LIBC_VER       _NEWLIB_VERSION
#else
#define PLATFORM_LIBC_LIB       ""
#define PLATFORM_LIBC_VER       ""
#endif

#if defined(__linux)
#define PLATFORM_SYSTEM     "Linux"
#elif defined(__unix__)
#define PLATFORM_SYSTEM     "Unix"
#elif defined(__CYGWIN__)
#define PLATFORM_SYSTEM     "Cygwin"
#elif defined(_WIN32)
#define PLATFORM_SYSTEM     "Windows"
#else
#define PLATFORM_SYSTEM     "MicroPython"
#endif

#ifndef MICROPY_PLATFORM_VERSION
#define MICROPY_PLATFORM_VERSION ""
#endif

STATIC const MP_DEFINE_STR_OBJ(info_platform_obj, PLATFORM_SYSTEM "-" MICROPY_VERSION_STRING "-" \
    PLATFORM_ARCH "-" MICROPY_PLATFORM_VERSION "-with-" PLATFORM_LIBC_LIB "" PLATFORM_LIBC_VER);
STATIC const MP_DEFINE_STR_OBJ(info_python_compiler_obj, PLATFORM_COMPILER);
STATIC const MP_DEFINE_STR_OBJ(info_libc_lib_obj, PLATFORM_LIBC_LIB);
STATIC const MP_DEFINE_STR_OBJ(info_libc_ver_obj, PLATFORM_LIBC_VER);
STATIC const mp_rom_obj_tuple_t info_libc_tuple_obj = {
    {&mp_type_tuple}, 2, {MP_ROM_PTR(&info_libc_lib_obj), MP_ROM_PTR(&info_libc_ver_obj)}
};

STATIC mp_obj_t platform_platform(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return MP_OBJ_FROM_PTR(&info_platform_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(platform_platform_obj, 0, platform_platform);

STATIC mp_obj_t platform_python_compiler(void) {
    return MP_OBJ_FROM_PTR(&info_python_compiler_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(platform_python_compiler_obj, platform_python_compiler);

STATIC mp_obj_t platform_libc_ver(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    return MP_OBJ_FROM_PTR(&info_libc_tuple_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(platform_libc_ver_obj, 0, platform_libc_ver);

STATIC const mp_rom_map_elem_t modplatform_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uplatform) },
    { MP_ROM_QSTR(MP_QSTR_platform), MP_ROM_PTR(&platform_platform_obj) },
    { MP_ROM_QSTR(MP_QSTR_python_compiler), MP_ROM_PTR(&platform_python_compiler_obj) },
    { MP_ROM_QSTR(MP_QSTR_libc_ver), MP_ROM_PTR(&platform_libc_ver_obj) },
};

STATIC MP_DEFINE_CONST_DICT(modplatform_globals, modplatform_globals_table);

const mp_obj_module_t mp_module_uplatform = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&modplatform_globals,
};

#endif // MICROPY_PY_UPLATFORM
