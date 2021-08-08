/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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

#include "shared-module/traceback/__init__.h"

void shared_module_traceback_print_exception(mp_obj_exception_t *exc, mp_print_t *print, mp_int_t limit) {
    // Print traceback
    if (exc->traceback != NULL) {
        size_t n = exc->traceback->len;
        size_t *values = exc->traceback->data;
        if (n > 0) {
            assert(n % 3 == 0);
            // Decompress the format strings
            const compressed_string_t *traceback = MP_ERROR_TEXT("Traceback (most recent call last):\n");
            #if MICROPY_ENABLE_SOURCE_LINE
            const compressed_string_t *frame = MP_ERROR_TEXT("  File \"%q\", line %d");
            #else
            const compressed_string_t *frame = MP_ERROR_TEXT("  File \"%q\"");
            #endif
            const compressed_string_t *block_fmt = MP_ERROR_TEXT(", in %q\n");

            // Set traceback formatting
            // Default: Print full traceback
            limit = limit * 3;
            mp_int_t i = n - 3, j;
            if (limit > 0) {
                // Print upto limit traceback
                // entries from caller's frame
                if ((unsigned)limit > n) {
                    limit = n;
                }
                limit = n - limit;
            } else if (limit < 0) {
                // Print upto limit traceback
                // entries from last
                if ((unsigned)-limit > n) {
                    limit = -n;
                }
                i = 0, limit = limit + 3;
            }

            // Print the traceback
            mp_cprintf(print, traceback);
            for (; i >= limit; i -= 3) {
                j = (i < 0) ? -i : i;
                #if MICROPY_ENABLE_SOURCE_LINE
                mp_cprintf(print, frame, values[j], (int)values[j + 1]);
                #else
                mp_printf(print, frame, values[j]);
                #endif
                // The block name can be NULL if it's unknown
                qstr block = values[j + 2];
                if (block == MP_QSTRnull) {
                    mp_print_str(print, "\n");
                } else {
                    mp_printf(print, block_fmt, block);
                }
            }
        }
    }
    // Print exception
    mp_obj_print_helper(print, exc, PRINT_EXC);
    mp_print_str(print, "\n");
}
