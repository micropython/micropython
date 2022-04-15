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

// The current version of .mpy files
#define MPY_VERSION 6

// Macros to encode/decode flags to/from the feature byte
#define MPY_FEATURE_ENCODE_FLAGS(flags) (flags)
#define MPY_FEATURE_DECODE_FLAGS(feat) ((feat) & 3)

// Macros to encode/decode native architecture to/from the feature byte
#define MPY_FEATURE_ENCODE_ARCH(arch) ((arch) << 2)
#define MPY_FEATURE_DECODE_ARCH(feat) ((feat) >> 2)

// The feature flag bits encode the compile-time config options that affect
// the generate bytecode. Note: position 0 is now unused
// (formerly MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE).
#define MPY_FEATURE_FLAGS ( \
    ((MICROPY_PY_BUILTINS_STR_UNICODE) << 1) \
    )
// This is a version of the flags that can be configured at runtime.
#define MPY_FEATURE_FLAGS_DYNAMIC ( \
    ((MICROPY_PY_BUILTINS_STR_UNICODE_DYNAMIC) << 1) \
    )

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
        #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_ARMV7M)
    #endif
    #define MPY_FEATURE_ARCH_TEST(x) (MP_NATIVE_ARCH_ARMV6M <= (x) && (x) <= MPY_FEATURE_ARCH)
#elif MICROPY_EMIT_ARM
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_ARMV6)
#elif MICROPY_EMIT_XTENSA
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_XTENSA)
#elif MICROPY_EMIT_XTENSAWIN
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_XTENSAWIN)
#else
    #define MPY_FEATURE_ARCH (MP_NATIVE_ARCH_NONE)
#endif

#ifndef MPY_FEATURE_ARCH_TEST
#define MPY_FEATURE_ARCH_TEST(x) ((x) == MPY_FEATURE_ARCH)
#endif

// 16-bit little-endian integer with the second and third bytes of supported .mpy files
#define MPY_FILE_HEADER_INT (MPY_VERSION \
    | (MPY_FEATURE_ENCODE_FLAGS(MPY_FEATURE_FLAGS) | MPY_FEATURE_ENCODE_ARCH(MPY_FEATURE_ARCH)) << 8)

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

mp_compiled_module_t mp_raw_code_load(mp_reader_t *reader, mp_module_context_t *ctx);
mp_compiled_module_t mp_raw_code_load_mem(const byte *buf, size_t len, mp_module_context_t *ctx);
mp_compiled_module_t mp_raw_code_load_file(const char *filename, mp_module_context_t *ctx);

void mp_raw_code_save(mp_compiled_module_t *cm, mp_print_t *print);
void mp_raw_code_save_file(mp_compiled_module_t *cm, const char *filename);

void mp_native_relocate(void *reloc, uint8_t *text, uintptr_t reloc_text);

#endif // MICROPY_INCLUDED_PY_PERSISTENTCODE_H
