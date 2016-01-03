/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
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

extern const char mp_frozen_names[];
extern const uint32_t mp_frozen_sizes[];
extern const char mp_frozen_content[];

mp_lexer_t *mp_find_frozen_module(const char *str, int len) {
    const char *name = mp_frozen_names;

    size_t offset = 0;
    for (int i = 0; *name != 0; i++) {
        int l = strlen(name);
        if (l == len && !memcmp(str, name, l)) {
            mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR_, mp_frozen_content + offset, mp_frozen_sizes[i], 0);
            return lex;
        }
        name += l + 1;
        offset += mp_frozen_sizes[i] + 1;
    }
    return NULL;
}

#endif // MICROPY_MODULE_FROZEN
