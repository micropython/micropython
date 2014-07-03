/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

// these functions are separate from parse.c to keep parser independent of mp_obj_t

#include <stdint.h>
#include <stdio.h>

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "lexer.h"
#include "parse.h"
#include "obj.h"
#include "parsehelper.h"

#define STR_MEMORY "parser could not allocate enough memory"
#define STR_UNEXPECTED_INDENT "unexpected indent"
#define STR_UNMATCHED_UNINDENT "unindent does not match any outer indentation level"
#define STR_INVALID_SYNTAX "invalid syntax"

void mp_parse_show_exception(mp_lexer_t *lex, mp_parse_error_kind_t parse_error_kind) {
    printf("  File \"%s\", line " UINT_FMT ", column " UINT_FMT "\n", qstr_str(mp_lexer_source_name(lex)), mp_lexer_cur(lex)->src_line, mp_lexer_cur(lex)->src_column);
    switch (parse_error_kind) {
        case MP_PARSE_ERROR_MEMORY:
            printf("MemoryError: %s\n", STR_MEMORY);
            break;

        case MP_PARSE_ERROR_UNEXPECTED_INDENT:
            printf("IndentationError: %s\n", STR_UNEXPECTED_INDENT);
            break;

        case MP_PARSE_ERROR_UNMATCHED_UNINDENT:
            printf("IndentationError: %s\n", STR_UNMATCHED_UNINDENT);
            break;

        case MP_PARSE_ERROR_INVALID_SYNTAX:
        default:
            printf("SyntaxError: %s\n", STR_INVALID_SYNTAX);
            break;
    }
}

mp_obj_t mp_parse_make_exception(mp_lexer_t *lex, mp_parse_error_kind_t parse_error_kind) {
    // make exception object
    mp_obj_t exc;
    switch (parse_error_kind) {
        case MP_PARSE_ERROR_MEMORY:
            exc = mp_obj_new_exception_msg(&mp_type_MemoryError, STR_MEMORY);
            break;

        case MP_PARSE_ERROR_UNEXPECTED_INDENT:
            exc = mp_obj_new_exception_msg(&mp_type_IndentationError, STR_UNEXPECTED_INDENT);
            break;

        case MP_PARSE_ERROR_UNMATCHED_UNINDENT:
            exc = mp_obj_new_exception_msg(&mp_type_IndentationError, STR_UNMATCHED_UNINDENT);
            break;

        case MP_PARSE_ERROR_INVALID_SYNTAX:
        default:
            exc = mp_obj_new_exception_msg(&mp_type_SyntaxError, STR_INVALID_SYNTAX);
            break;
    }

    // add traceback to give info about file name and location
    // we don't have a 'block' name, so just pass the NULL qstr to indicate this
    mp_obj_exception_add_traceback(exc, mp_lexer_source_name(lex), mp_lexer_cur(lex)->src_line, MP_QSTR_NULL);

    return exc;
}
