// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "supervisor/shared/translate/compressed_string.h"

#ifndef NO_QSTR
#define QDEF(id, hash, len, str)
#define TRANSLATION(english_id, number) extern struct compressed_string translation##number;
#include "genhdr/qstrdefs.generated.h"
#undef TRANSLATION
#undef QDEF
#endif

#if CIRCUITPY_TRANSLATE_OBJECT == 0
static
#endif
inline
// gcc10 -flto has issues with this being always_inline for debug builds.
#if !CIRCUITPY_LTO || CIRCUITPY_DEBUG < 1
__attribute__((always_inline))
#endif
// Prevent instrumenting this because that disables the inlining we rely of for code size
// optimization.
__attribute__((no_instrument_function)) mp_rom_error_text_t MP_COMPRESSED_ROM_TEXT(const char *original) {
    #ifndef NO_QSTR
    #define QDEF(id, hash, len, str)
    #define TRANSLATION(english_id, number) if (strcmp(original, english_id) == 0) { return (mp_rom_error_text_t)&translation##number; } else
    #include "genhdr/qstrdefs.generated.h"
#undef TRANSLATION
#undef QDEF
    #endif
    return NULL;
}
