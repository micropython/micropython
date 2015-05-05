/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Josef Gajdusek
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

#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/obj.h"
#include "py/gc.h"
#include MICROPY_HAL_H

typedef uint32_t uint32;
typedef uint16_t uint16;
#include "spi_flash.h"

#define SCRIPT_MAGIC        0x38707365
#define SCRIPT_ADDR         0x60000

typedef struct _script_lexer_data_t {
    uint32_t at;
    uint32_t remlen;
    int bufat;
    char buff[16];
} script_lexer_data_t;

STATIC mp_uint_t script_lexer_next(void *arg) {
    script_lexer_data_t *data = arg;

    if (data->remlen == 0) {
        return MP_LEXER_EOF;
    }

    if (data->bufat == 16) {
        data->bufat = 0;
        spi_flash_read(SCRIPT_ADDR + data->at, (uint32_t *)data->buff, 16);
    }

    data->at += 1;
    data->remlen -= 1;
    return data->buff[data->bufat++];
};

STATIC void script_lexer_close(void *arg) {
};

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    if (strcmp(filename, "main.py") != 0) {
        return NULL;
    }

    uint32_t data[2];
    spi_flash_read(SCRIPT_ADDR, data, 8);

    if (data[0] != SCRIPT_MAGIC) {
        return NULL;
    }

    script_lexer_data_t *lexdata = m_new_obj(script_lexer_data_t);
    lexdata->at = 8;
    lexdata->remlen = data[1];
    lexdata->bufat = 16;

    return mp_lexer_new(qstr_from_str(filename), lexdata, script_lexer_next, script_lexer_close);
}
