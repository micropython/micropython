/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_PERSISTENTCODE_H
#define MICROPY_INCLUDED_PY_PERSISTENTCODE_H

#include "py/mpprint.h"
#include "py/reader.h"
#include "py/emitglue.h"

// The current version of .mpy files. A bytecode-only .mpy file can be loaded
// as long as MPY_VERSION matches, but a native .mpy (i.e. one with an arch
// set) must also match MPY_SUB_VERSION. This allows 3 additional updates to
// the native ABI per bytecode revision.
#define MPY_VERSION 6
#define MPY_SUB_VERSION 3

// Macros to encode/decode sub-version to/from the feature byte. This replaces
// the bits previously used to encode the flags (map caching and unicode)
// which are no longer used starting at .mpy version 6.
#define MPY_FEATURE_ENCODE_SUB_VERSION(version) (version)
#define MPY_FEATURE_DECODE_SUB_VERSION(feat) ((feat) & 3)

// Macros to encode/decode native architecture to/from the feature byte
#define MPY_FEATURE_ENCODE_ARCH(arch) ((arch) << 2)
#define MPY_FEATURE_DECODE_ARCH(feat) ((feat) >> 2)

// Define the host architecture
#if MICROPY_EMIT_X86
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_X86)
#elif MICROPY_EMIT_X64
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_X64)
#elif MICROPY_EMIT_THUMB
    #if defined(__thumb2__)
        #if defined(__ARM_FP) && (__ARM_FP & 8) == 8
            #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_ARMV7EMDP)
        #elif defined(__ARM_FP) && (__ARM_FP & 4) == 4
            #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_ARMV7EMSP)
        #else
            #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_ARMV7EM)
        #endif
    #else
        #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_ARMV6M)
    #endif
    #define MPY_FEATURE_ARCH_TEST(x) (MP_NATIVE_ARCH_ARMV6M <= (x) && (x) <= MPY_FEATURE_ARCH)
#elif MICROPY_EMIT_ARM
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_ARMV6)
#elif MICROPY_EMIT_XTENSA
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_XTENSA)
#elif MICROPY_EMIT_XTENSAWIN
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_XTENSAWIN)
#elif MICROPY_EMIT_RV32
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_RV32IMC)
#else
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_NONE)
#endif

#ifndef MPY_FEATURE_ARCH_TEST
#define MPY_FEATURE_ARCH_TEST(x) ((x) == MPY_FEATURE_ARCH)
#endif

// 16-bit little-endian integer with the second and third bytes of supported .mpy files
#define MPY_FILE_HEADER_INT (MPY_VERSION \
    | (MPY_FEATURE_ENCODE_SUB_VERSION(MPY_SUB_VERSION) | MPY_FEATURE_ENCODE_ARCH(MPY_FEATURE_ARCH)) << 8)

enum {
    MP_NATIVE_ARCH_NONE = 0,
    MP_NATIVE_ARCH_X86,
    MP_NATIVE_ARCH_X64,
    MP_NATIVE_ARCH_ARMV6,
    MP_NATIVE_ARCH_ARMV6M,
    MP_NATIVE_ARCH_ARMV7M,
    MP_NATIVE_ARCH_ARMV7EM,
    MP_NATIVE_ARCH_ARMV7EMSP,
    MP_NATIVE_ARCH_ARMV7EMDP,
    MP_NATIVE_ARCH_XTENSA,
    MP_NATIVE_ARCH_XTENSAWIN,
    MP_NATIVE_ARCH_RV32IMC,
    MP_NATIVE_ARCH_DEBUG, // this entry should always be last
};

enum {
    MP_PERSISTENT_OBJ_FUN_TABLE = 0,
    MP_PERSISTENT_OBJ_NONE,
    MP_PERSISTENT_OBJ_FALSE,
    MP_PERSISTENT_OBJ_TRUE,
    MP_PERSISTENT_OBJ_ELLIPSIS,
    MP_PERSISTENT_OBJ_STR,
    MP_PERSISTENT_OBJ_BYTES,
    MP_PERSISTENT_OBJ_INT,
    MP_PERSISTENT_OBJ_FLOAT,
    MP_PERSISTENT_OBJ_COMPLEX,
    MP_PERSISTENT_OBJ_TUPLE,
};

void mp_raw_code_load(mp_reader_t *reader, mp_compiled_module_t *ctx);
void mp_raw_code_load_mem(const byte *buf, size_t len, mp_compiled_module_t *ctx);
void mp_raw_code_load_file(qstr filename, mp_compiled_module_t *ctx);

void mp_raw_code_save(mp_compiled_module_t *cm, mp_print_t *print);
void mp_raw_code_save_file(mp_compiled_module_t *cm, qstr filename);

void mp_native_relocate(void *reloc, uint8_t *text, uintptr_t reloc_text);

#endif // MICROPY_INCLUDED_PY_PERSISTENTCODE_H
