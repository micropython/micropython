/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
 * Copyright (c) 2016 Damien P. George
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

#if MICROPY_MODULE_FROZEN_STR

#ifndef MICROPY_MODULE_FROZEN_LEXER
#define MICROPY_MODULE_FROZEN_LEXER mp_lexer_new_from_str_len
#else
mp_lexer_t *MICROPY_MODULE_FROZEN_LEXER(qstr src_name, const char *str, mp_uint_t len, mp_uint_t free_len);
#endif

extern const char mp_frozen_str_names[];
extern const uint32_t mp_frozen_str_sizes[];
extern const char mp_frozen_str_content[];

// On input, *len contains size of name, on output - size of content
const char *mp_find_frozen_str(const char *str, size_t *len) {
    const char *name = mp_frozen_str_names;

    size_t offset = 0;
    for (int i = 0; *name != 0; i++) {
        size_t l = strlen(name);
        if (l == *len && !memcmp(str, name, l)) {
            *len = mp_frozen_str_sizes[i];
            return mp_frozen_str_content + offset;
        }
        name += l + 1;
        offset += mp_frozen_str_sizes[i] + 1;
    }
    return NULL;
}

STATIC mp_lexer_t *mp_lexer_frozen_str(const char *str, size_t len) {
    size_t name_len = len;
    const char *content = mp_find_frozen_str(str, &len);

    if (content == NULL) {
        return NULL;
    }

    qstr source = qstr_from_strn(str, name_len);
    mp_lexer_t *lex = MICROPY_MODULE_FROZEN_LEXER(source, content, len, 0);
    return lex;
}

#endif

#if MICROPY_MODULE_FROZEN_MPY

#include "py/emitglue.h"

extern const char mp_frozen_mpy_names[];
extern const mp_raw_code_t *const mp_frozen_mpy_content[];

STATIC const mp_raw_code_t *mp_find_frozen_mpy(const char *str, size_t len) {
    const char *name = mp_frozen_mpy_names;
    for (size_t i = 0; *name != 0; i++) {
        size_t l = strlen(name);
        if (l == len && !memcmp(str, name, l)) {
            return mp_frozen_mpy_content[i];
        }
        name += l + 1;
    }
    return NULL;
}

#endif

#if MICROPY_MODULE_FROZEN

STATIC mp_import_stat_t mp_frozen_stat_helper(const char *name, const char *str) {
    size_t len = strlen(str);

    for (int i = 0; *name != 0; i++) {
        size_t l = strlen(name);
        if (l >= len && !memcmp(str, name, len)) {
            if (name[len] == 0) {
                return MP_IMPORT_STAT_FILE;
            } else if (name[len] == '/') {
                return MP_IMPORT_STAT_DIR;
            }
        }
        name += l + 1;
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_import_stat_t mp_frozen_stat(const char *str) {
    mp_import_stat_t stat;

    #if MICROPY_MODULE_FROZEN_STR
    stat = mp_frozen_stat_helper(mp_frozen_str_names, str);
    if (stat != MP_IMPORT_STAT_NO_EXIST) {
        return stat;
    }
    #endif

    #if MICROPY_MODULE_FROZEN_MPY
    stat = mp_frozen_stat_helper(mp_frozen_mpy_names, str);
    if (stat != MP_IMPORT_STAT_NO_EXIST) {
        return stat;
    }
    #endif

    return MP_IMPORT_STAT_NO_EXIST;
}

int mp_find_frozen_module(const char *str, size_t len, void **data) {
    #if MICROPY_MODULE_FROZEN_STR
    mp_lexer_t *lex = mp_lexer_frozen_str(str, len);
    if (lex != NULL) {
        *data = lex;
        return MP_FROZEN_STR;
    }
    #endif
    #if MICROPY_MODULE_FROZEN_MPY
    const mp_raw_code_t *rc = mp_find_frozen_mpy(str, len);
    if (rc != NULL) {
        *data = (void*)rc;
        return MP_FROZEN_MPY;
    }
    #endif
    return MP_FROZEN_NONE;
}

#endif
