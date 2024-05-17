// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>


// Map MicroPython's error messages to our translations.
#if !defined(MICROPY_ENABLE_DYNRUNTIME) || !MICROPY_ENABLE_DYNRUNTIME
#include "supervisor/shared/translate/compressed_string.h"

// MP_ERROR_TEXT() is a giant function with many strcmp calls. The assumption is
// that the build process will optimize this away and replace it with the
// appropriate compressed data for each call site.

#if CIRCUITPY_TRANSLATE_OBJECT == 0
// Without LTO, we need to include a copy of this function in each compilation
// unit so that the compile stage can do the optimization. Otherwise the linker
// will leave this as a giant function and have each call site call into it.
#include "supervisor/shared/translate/translate_impl.h"
#else
// In link time optimized (LTO) builds, we can compile this once into a .o and
// at link time the calls will be optimized.
mp_rom_error_text_t MP_COMPRESSED_ROM_TEXT(const char *c);
#endif

#else
typedef const char *mp_rom_error_text_t;
#define MP_COMPRESSED_ROM_TEXT(x) x
#endif
