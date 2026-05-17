/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
 * Copyright (c) 2016 Damien P. George
 * Copyright (c) 2021 Jim Mussared
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
#include <stdint.h>

#include "py/lexer.h"
#include "py/frozenmod.h"

#if MICROPY_MODULE_FROZEN

// Null-separated frozen file names. All string-type entries are listed first,
// followed by mpy-type entries. Use mp_frozen_str_sizes to determine how
// many string entries.
extern const char mp_frozen_names[];

#if MICROPY_MODULE_FROZEN_STR

#ifndef MICROPY_MODULE_FROZEN_LEXER
#define MICROPY_MODULE_FROZEN_LEXER mp_lexer_new_from_str_len
#else
mp_lexer_t *MICROPY_MODULE_FROZEN_LEXER(qstr src_name, const char *str, mp_uint_t len, mp_uint_t free_len);
#endif

// Size in bytes of each string entry, followed by a zero (terminator).
extern const uint32_t mp_frozen_str_sizes[];
// Null-separated string content.
extern const char mp_frozen_str_content[];
#endif // MICROPY_MODULE_FROZEN_STR

#if MICROPY_MODULE_FROZEN_MPY

#include "py/emitglue.h"

extern const mp_frozen_module_t *const mp_frozen_mpy_content[];

#endif // MICROPY_MODULE_FROZEN_MPY

// Search for "str" as a frozen entry, returning the stat result
// (no-exist/file/dir), as well as the type (none/str/mpy) and data.
// frozen_type can be NULL if its value isn't needed (and then data is assumed to be NULL).
mp_import_stat_t mp_find_frozen_module(const char *str, int *frozen_type, void **data) {
    size_t len = strlen(str);
    const char *name = mp_frozen_names;

    if (frozen_type != NULL) {
        *frozen_type = MP_FROZEN_NONE;
    }

    // Count the number of str lengths we have to find how many str entries.
    size_t num_str = 0;
    #if MICROPY_MODULE_FROZEN_STR && MICROPY_MODULE_FROZEN_MPY
    for (const uint32_t *s = mp_frozen_str_sizes; *s != 0; ++s) {
        ++num_str;
    }
    #endif

    for (size_t i = 0; *name != 0; i++) {
        size_t entry_len = strlen(name);
        if (entry_len >= len && memcmp(str, name, len) == 0) {
            // Query is a prefix of the current entry.
            if (entry_len == len) {
                // Exact match --> file.

                if (frozen_type != NULL) {
                    #if MICROPY_MODULE_FROZEN_STR
                    if (i < num_str) {
                        *frozen_type = MP_FROZEN_STR;
                        // Use the size table to figure out where this index starts.
                        size_t offset = 0;
                        for (size_t j = 0; j < i; ++j) {
                            offset += mp_frozen_str_sizes[j] + 1;
                        }
                        size_t content_len = mp_frozen_str_sizes[i];
                        const char *content = &mp_frozen_str_content[offset];

                        // Note: str & len have been updated by find_frozen_entry to strip
                        // the ".frozen/" prefix (to avoid this being a distinct qstr to
                        // the original path QSTR in frozen_content.c).
                        qstr source = qstr_from_strn(str, len);
                        mp_lexer_t *lex = MICROPY_MODULE_FROZEN_LEXER(source, content, content_len, 0);
                        *data = lex;
                    }
                    #endif

                    #if MICROPY_MODULE_FROZEN_MPY
                    if (i >= num_str) {
                        *frozen_type = MP_FROZEN_MPY;
                        // Load the corresponding index as a raw_code, taking
                        // into account any string entries to offset by.
                        *data = (void *)mp_frozen_mpy_content[i - num_str];
                    }
                    #endif
                }

                return MP_IMPORT_STAT_FILE;
            } else if (name[len] == '/') {
                // Matches up to directory separator, this is a valid
                // directory path.
                return MP_IMPORT_STAT_DIR;
            }
        }
        // Skip null separator.
        name += entry_len + 1;
    }

    return MP_IMPORT_STAT_NO_EXIST;
}

#endif // MICROPY_MODULE_FROZEN
