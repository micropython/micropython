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
 */
#ifndef MICROPY_INCLUDED_MODUPLATFORM_H
#define MICROPY_INCLUDED_MODUPLATFORM_H

#include "py/misc.h"  // For MP_STRINGIFY.
#include "py/mpconfig.h"

// Preprocessor directives identifying the platform.
// The (u)platform module itself is guarded by MICROPY_PY_UPLATFORM, see the
// .c file, but these are made available because they're generally usable.
// TODO: Add more architectures, compilers and libraries.
// See: https://sourceforge.net/p/predef/wiki/Home/

#if defined(__ARM_ARCH)
#define MICROPY_PLATFORM_ARCH   "arm"
#elif defined(__x86_64__) || defined(_M_X64)
#define MICROPY_PLATFORM_ARCH   "x86_64"
#elif defined(__i386__) || defined(_M_IX86)
#define MICROPY_PLATFORM_ARCH   "x86"
#elif defined(__xtensa__)
#define MICROPY_PLATFORM_ARCH   "xtensa"
#else
#define MICROPY_PLATFORM_ARCH   ""
#endif

#if defined(__GNUC__)
#define MICROPY_PLATFORM_COMPILER \
    "GCC " \
    MP_STRINGIFY(__GNUC__) "." \
    MP_STRINGIFY(__GNUC_MINOR__) "." \
    MP_STRINGIFY(__GNUC_PATCHLEVEL__)
#elif defined(__ARMCC_VERSION)
#define MICROPY_PLATFORM_COMPILER \
    "ARMCC " \
    MP_STRINGIFY((__ARMCC_VERSION / 1000000)) "." \
    MP_STRINGIFY((__ARMCC_VERSION / 10000 % 100)) "." \
    MP_STRINGIFY((__ARMCC_VERSION % 10000))
#elif defined(_MSC_VER)
#if defined(_WIN64)
#define MICROPY_PLATFORM_COMPILER_BITS  "64 bit"
#elif defined(_M_IX86)
#define MICROPY_PLATFORM_COMPILER_BITS  "32 bit"
#else
#define MICROPY_PLATFORM_COMPILER_BITS  ""
#endif
#define MICROPY_PLATFORM_COMPILER \
    "MSC v." MP_STRINGIFY(_MSC_VER) " " MICROPY_PLATFORM_COMPILER_BITS
#else
#define MICROPY_PLATFORM_COMPILER       ""
#endif

#if defined(__GLIBC__)
#define MICROPY_PLATFORM_LIBC_LIB       "glibc"
#define MICROPY_PLATFORM_LIBC_VER \
    MP_STRINGIFY(__GLIBC__) "." \
    MP_STRINGIFY(__GLIBC_MINOR__)
#elif defined(__NEWLIB__)
#define MICROPY_PLATFORM_LIBC_LIB       "newlib"
#define MICROPY_PLATFORM_LIBC_VER       _NEWLIB_VERSION
#else
#define MICROPY_PLATFORM_LIBC_LIB       ""
#define MICROPY_PLATFORM_LIBC_VER       ""
#endif

#if defined(__linux)
#define MICROPY_PLATFORM_SYSTEM         "Linux"
#elif defined(__unix__)
#define MICROPY_PLATFORM_SYSTEM         "Unix"
#elif defined(__CYGWIN__)
#define MICROPY_PLATFORM_SYSTEM         "Cygwin"
#elif defined(_WIN32)
#define MICROPY_PLATFORM_SYSTEM         "Windows"
#else
#define MICROPY_PLATFORM_SYSTEM         "MicroPython"
#endif

#ifndef MICROPY_PLATFORM_VERSION
#define MICROPY_PLATFORM_VERSION ""
#endif

#endif // MICROPY_INCLUDED_MODUPLATFORM_H
