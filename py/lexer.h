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

/* lexer.h -- simple tokeniser for Micro Python
 *
 * Uses (byte) length instead of null termination.
 * Tokens are the same - UTF-8 with (byte) length.
 */

typedef enum _mp_token_kind_t {
    MP_TOKEN_END,                   // 0

    MP_TOKEN_INVALID,
    MP_TOKEN_DEDENT_MISMATCH,
    MP_TOKEN_LONELY_STRING_OPEN,
    MP_TOKEN_BAD_LINE_CONTINUATION,

    MP_TOKEN_NEWLINE,               // 5
    MP_TOKEN_INDENT,                // 6
    MP_TOKEN_DEDENT,                // 7

    MP_TOKEN_NAME,                  // 8
    MP_TOKEN_NUMBER,
    MP_TOKEN_STRING,
    MP_TOKEN_BYTES,

    MP_TOKEN_ELLIPSIS,

    MP_TOKEN_KW_FALSE,              // 13
    MP_TOKEN_KW_NONE,
    MP_TOKEN_KW_TRUE,
    MP_TOKEN_KW_AND,
    MP_TOKEN_KW_AS,
    MP_TOKEN_KW_ASSERT,
    MP_TOKEN_KW_BREAK,
    MP_TOKEN_KW_CLASS,
    MP_TOKEN_KW_CONTINUE,
    MP_TOKEN_KW_DEF,                // 22
    MP_TOKEN_KW_DEL,
    MP_TOKEN_KW_ELIF,
    MP_TOKEN_KW_ELSE,
    MP_TOKEN_KW_EXCEPT,
    MP_TOKEN_KW_FINALLY,
    MP_TOKEN_KW_FOR,
    MP_TOKEN_KW_FROM,
    MP_TOKEN_KW_GLOBAL,
    MP_TOKEN_KW_IF,
    MP_TOKEN_KW_IMPORT,             // 32
    MP_TOKEN_KW_IN,
    MP_TOKEN_KW_IS,
    MP_TOKEN_KW_LAMBDA,
    MP_TOKEN_KW_NONLOCAL,
    MP_TOKEN_KW_NOT,
    MP_TOKEN_KW_OR,
    MP_TOKEN_KW_PASS,
    MP_TOKEN_KW_RAISE,
    MP_TOKEN_KW_RETURN,
    MP_TOKEN_KW_TRY,                // 42
    MP_TOKEN_KW_WHILE,
    MP_TOKEN_KW_WITH,
    MP_TOKEN_KW_YIELD,

    MP_TOKEN_OP_PLUS,               // 46
    MP_TOKEN_OP_MINUS,
    MP_TOKEN_OP_STAR,
    MP_TOKEN_OP_DBL_STAR,
    MP_TOKEN_OP_SLASH,
    MP_TOKEN_OP_DBL_SLASH,
    MP_TOKEN_OP_PERCENT,
    MP_TOKEN_OP_LESS,
    MP_TOKEN_OP_DBL_LESS,
    MP_TOKEN_OP_MORE,
    MP_TOKEN_OP_DBL_MORE,           // 56
    MP_TOKEN_OP_AMPERSAND,
    MP_TOKEN_OP_PIPE,
    MP_TOKEN_OP_CARET,
    MP_TOKEN_OP_TILDE,
    MP_TOKEN_OP_LESS_EQUAL,
    MP_TOKEN_OP_MORE_EQUAL,
    MP_TOKEN_OP_DBL_EQUAL,
    MP_TOKEN_OP_NOT_EQUAL,

    MP_TOKEN_DEL_PAREN_OPEN,        // 65
    MP_TOKEN_DEL_PAREN_CLOSE,
    MP_TOKEN_DEL_BRACKET_OPEN,
    MP_TOKEN_DEL_BRACKET_CLOSE,
    MP_TOKEN_DEL_BRACE_OPEN,
    MP_TOKEN_DEL_BRACE_CLOSE,
    MP_TOKEN_DEL_COMMA,
    MP_TOKEN_DEL_COLON,
    MP_TOKEN_DEL_PERIOD,
    MP_TOKEN_DEL_SEMICOLON,
    MP_TOKEN_DEL_AT,                // 75
    MP_TOKEN_DEL_EQUAL,
    MP_TOKEN_DEL_PLUS_EQUAL,
    MP_TOKEN_DEL_MINUS_EQUAL,
    MP_TOKEN_DEL_STAR_EQUAL,
    MP_TOKEN_DEL_SLASH_EQUAL,
    MP_TOKEN_DEL_DBL_SLASH_EQUAL,
    MP_TOKEN_DEL_PERCENT_EQUAL,
    MP_TOKEN_DEL_AMPERSAND_EQUAL,
    MP_TOKEN_DEL_PIPE_EQUAL,
    MP_TOKEN_DEL_CARET_EQUAL,       // 85
    MP_TOKEN_DEL_DBL_MORE_EQUAL,
    MP_TOKEN_DEL_DBL_LESS_EQUAL,
    MP_TOKEN_DEL_DBL_STAR_EQUAL,
    MP_TOKEN_DEL_MINUS_MORE,
} mp_token_kind_t;

typedef struct _mp_token_t {
    mp_uint_t src_line;         // source line
    mp_uint_t src_column;       // source column

    mp_token_kind_t kind;       // kind of token
    const char *str;            // string of token (valid only while this token is current token)
    mp_uint_t len;              // (byte) length of string of token
} mp_token_t;

// the next-byte function must return the next byte in the stream
// it must return MP_LEXER_EOF if end of stream
// it can be called again after returning MP_LEXER_EOF, and in that case must return MP_LEXER_EOF
#define MP_LEXER_EOF (-1)
typedef mp_uint_t (*mp_lexer_stream_next_byte_t)(void*);
typedef void (*mp_lexer_stream_close_t)(void*);

typedef struct _mp_lexer_t mp_lexer_t;

void mp_token_show(const mp_token_t *tok);

mp_lexer_t *mp_lexer_new(qstr src_name, void *stream_data, mp_lexer_stream_next_byte_t stream_next_byte, mp_lexer_stream_close_t stream_close);
mp_lexer_t *mp_lexer_new_from_str_len(qstr src_name, const char *str, mp_uint_t len, mp_uint_t free_len);

void mp_lexer_free(mp_lexer_t *lex);
qstr mp_lexer_source_name(mp_lexer_t *lex);
void mp_lexer_to_next(mp_lexer_t *lex);
const mp_token_t *mp_lexer_cur(const mp_lexer_t *lex);
bool mp_lexer_is_kind(mp_lexer_t *lex, mp_token_kind_t kind);

bool mp_lexer_show_error_pythonic_prefix(mp_lexer_t *lex);
bool mp_lexer_show_error_pythonic(mp_lexer_t *lex, const char *msg);

/******************************************************************/
// platform specific import function; must be implemented for a specific port
// TODO tidy up, rename, or put elsewhere

//mp_lexer_t *mp_import_open_file(qstr mod_name);

typedef enum {
    MP_IMPORT_STAT_NO_EXIST,
    MP_IMPORT_STAT_DIR,
    MP_IMPORT_STAT_FILE,
} mp_import_stat_t;

mp_import_stat_t mp_import_stat(const char *path);
mp_lexer_t *mp_lexer_new_from_file(const char *filename);

extern mp_uint_t mp_optimise_value;
