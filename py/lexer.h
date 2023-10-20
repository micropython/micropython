/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_LEXER_H
#define MICROPY_INCLUDED_PY_LEXER_H

#include <stdint.h>

#include "py/mpconfig.h"
#include "py/qstr.h"
#include "py/reader.h"

/* lexer.h -- simple tokeniser for MicroPython
 *
 * Uses (byte) length instead of null termination.
 * Tokens are the same - UTF-8 with (byte) length.
 */

typedef enum _mp_token_kind_t {
    MP_TOKEN_END,

    MP_TOKEN_INVALID,
    MP_TOKEN_DEDENT_MISMATCH,
    MP_TOKEN_LONELY_STRING_OPEN,
    #if MICROPY_PY_FSTRINGS
    MP_TOKEN_MALFORMED_FSTRING,
    MP_TOKEN_FSTRING_RAW,
    #endif

    MP_TOKEN_NEWLINE,
    MP_TOKEN_INDENT,
    MP_TOKEN_DEDENT,

    MP_TOKEN_NAME,
    MP_TOKEN_INTEGER,
    MP_TOKEN_FLOAT_OR_IMAG,
    MP_TOKEN_STRING,
    MP_TOKEN_BYTES,

    MP_TOKEN_ELLIPSIS,

    MP_TOKEN_KW_FALSE,
    MP_TOKEN_KW_NONE,
    MP_TOKEN_KW_TRUE,
    MP_TOKEN_KW___DEBUG__,
    MP_TOKEN_KW_AND,
    MP_TOKEN_KW_AS,
    MP_TOKEN_KW_ASSERT,
    #if MICROPY_PY_ASYNC_AWAIT
    MP_TOKEN_KW_ASYNC,
    MP_TOKEN_KW_AWAIT,
    #endif
    MP_TOKEN_KW_BREAK,
    MP_TOKEN_KW_CLASS,
    MP_TOKEN_KW_CONTINUE,
    MP_TOKEN_KW_DEF,
    MP_TOKEN_KW_DEL,
    MP_TOKEN_KW_ELIF,
    MP_TOKEN_KW_ELSE,
    MP_TOKEN_KW_EXCEPT,
    MP_TOKEN_KW_FINALLY,
    MP_TOKEN_KW_FOR,
    MP_TOKEN_KW_FROM,
    MP_TOKEN_KW_GLOBAL,
    MP_TOKEN_KW_IF,
    MP_TOKEN_KW_IMPORT,
    MP_TOKEN_KW_IN,
    MP_TOKEN_KW_IS,
    MP_TOKEN_KW_LAMBDA,
    MP_TOKEN_KW_NONLOCAL,
    MP_TOKEN_KW_NOT,
    MP_TOKEN_KW_OR,
    MP_TOKEN_KW_PASS,
    MP_TOKEN_KW_RAISE,
    MP_TOKEN_KW_RETURN,
    MP_TOKEN_KW_TRY,
    MP_TOKEN_KW_WHILE,
    MP_TOKEN_KW_WITH,
    MP_TOKEN_KW_YIELD,

    MP_TOKEN_OP_ASSIGN,
    MP_TOKEN_OP_TILDE,

    // Order of these 6 matches corresponding mp_binary_op_t operator
    MP_TOKEN_OP_LESS,
    MP_TOKEN_OP_MORE,
    MP_TOKEN_OP_DBL_EQUAL,
    MP_TOKEN_OP_LESS_EQUAL,
    MP_TOKEN_OP_MORE_EQUAL,
    MP_TOKEN_OP_NOT_EQUAL,

    // Order of these 13 matches corresponding mp_binary_op_t operator
    MP_TOKEN_OP_PIPE,
    MP_TOKEN_OP_CARET,
    MP_TOKEN_OP_AMPERSAND,
    MP_TOKEN_OP_DBL_LESS,
    MP_TOKEN_OP_DBL_MORE,
    MP_TOKEN_OP_PLUS,
    MP_TOKEN_OP_MINUS,
    MP_TOKEN_OP_STAR,
    MP_TOKEN_OP_AT,
    MP_TOKEN_OP_DBL_SLASH,
    MP_TOKEN_OP_SLASH,
    MP_TOKEN_OP_PERCENT,
    MP_TOKEN_OP_DBL_STAR,

    // Order of these 13 matches corresponding mp_binary_op_t operator
    MP_TOKEN_DEL_PIPE_EQUAL,
    MP_TOKEN_DEL_CARET_EQUAL,
    MP_TOKEN_DEL_AMPERSAND_EQUAL,
    MP_TOKEN_DEL_DBL_LESS_EQUAL,
    MP_TOKEN_DEL_DBL_MORE_EQUAL,
    MP_TOKEN_DEL_PLUS_EQUAL,
    MP_TOKEN_DEL_MINUS_EQUAL,
    MP_TOKEN_DEL_STAR_EQUAL,
    MP_TOKEN_DEL_AT_EQUAL,
    MP_TOKEN_DEL_DBL_SLASH_EQUAL,
    MP_TOKEN_DEL_SLASH_EQUAL,
    MP_TOKEN_DEL_PERCENT_EQUAL,
    MP_TOKEN_DEL_DBL_STAR_EQUAL,

    MP_TOKEN_DEL_PAREN_OPEN,
    MP_TOKEN_DEL_PAREN_CLOSE,
    MP_TOKEN_DEL_BRACKET_OPEN,
    MP_TOKEN_DEL_BRACKET_CLOSE,
    MP_TOKEN_DEL_BRACE_OPEN,
    MP_TOKEN_DEL_BRACE_CLOSE,
    MP_TOKEN_DEL_COMMA,
    MP_TOKEN_DEL_COLON,
    MP_TOKEN_DEL_PERIOD,
    MP_TOKEN_DEL_SEMICOLON,
    MP_TOKEN_DEL_EQUAL,
    MP_TOKEN_DEL_MINUS_MORE,
} mp_token_kind_t;

// this data structure is exposed for efficiency
// public members are: source_name, tok_line, tok_column, tok_kind, vstr
typedef struct _mp_lexer_t {
    qstr source_name;           // name of source
    mp_reader_t reader;         // stream source

    unichar chr0, chr1, chr2;   // current cached characters from source
    #if MICROPY_PY_FSTRINGS
    unichar chr0_saved, chr1_saved, chr2_saved; // current cached characters from alt source
    #endif

    size_t line;                // current source line
    size_t column;              // current source column

    mp_int_t emit_dent;             // non-zero when there are INDENT/DEDENT tokens to emit
    mp_int_t nested_bracket_level;  // >0 when there are nested brackets over multiple lines

    size_t alloc_indent_level;
    size_t num_indent_level;
    uint16_t *indent_level;

    size_t tok_line;            // token source line
    size_t tok_column;          // token source column
    mp_token_kind_t tok_kind;   // token kind
    vstr_t vstr;                // token data
    #if MICROPY_PY_FSTRINGS
    vstr_t fstring_args;        // extracted arguments to pass to .format()
    size_t fstring_args_idx;    // how many bytes of fstring_args have been read
    #endif
} mp_lexer_t;

mp_lexer_t *mp_lexer_new(qstr src_name, mp_reader_t reader);
mp_lexer_t *mp_lexer_new_from_str_len(qstr src_name, const char *str, size_t len, size_t free_len);

// If MICROPY_READER_POSIX or MICROPY_READER_VFS aren't enabled then
// this function must be implemented by the port.
mp_lexer_t *mp_lexer_new_from_file(const char *filename);

#if MICROPY_HELPER_LEXER_UNIX
mp_lexer_t *mp_lexer_new_from_fd(qstr filename, int fd, bool close_fd);
#endif

void mp_lexer_free(mp_lexer_t *lex);
void mp_lexer_to_next(mp_lexer_t *lex);

#endif // MICROPY_INCLUDED_PY_LEXER_H
