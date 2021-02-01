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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "py/reader.h"
#include "py/lexer.h"
#include "py/runtime.h"

#if MICROPY_ENABLE_COMPILER

#define TAB_SIZE (8)

// TODO seems that CPython allows NULL byte in the input stream
// don't know if that's intentional or not, but we don't allow it

#define MP_LEXER_EOF ((unichar)MP_READER_EOF)
#define CUR_CHAR(lex) ((lex)->chr0)

STATIC bool is_end(mp_lexer_t *lex) {
    return lex->chr0 == MP_LEXER_EOF;
}

STATIC bool is_physical_newline(mp_lexer_t *lex) {
    return lex->chr0 == '\n';
}

STATIC bool is_char(mp_lexer_t *lex, byte c) {
    return lex->chr0 == c;
}

STATIC bool is_char_or(mp_lexer_t *lex, byte c1, byte c2) {
    return lex->chr0 == c1 || lex->chr0 == c2;
}

STATIC bool is_char_or3(mp_lexer_t *lex, byte c1, byte c2, byte c3) {
    return lex->chr0 == c1 || lex->chr0 == c2 || lex->chr0 == c3;
}

STATIC bool is_char_following(mp_lexer_t *lex, byte c) {
    return lex->chr1 == c;
}

STATIC bool is_char_following_or(mp_lexer_t *lex, byte c1, byte c2) {
    return lex->chr1 == c1 || lex->chr1 == c2;
}

STATIC bool is_char_following_following_or(mp_lexer_t *lex, byte c1, byte c2) {
    return lex->chr2 == c1 || lex->chr2 == c2;
}

STATIC bool is_char_and(mp_lexer_t *lex, byte c1, byte c2) {
    return lex->chr0 == c1 && lex->chr1 == c2;
}

STATIC bool is_whitespace(mp_lexer_t *lex) {
    return unichar_isspace(lex->chr0);
}

STATIC bool is_letter(mp_lexer_t *lex) {
    return unichar_isalpha(lex->chr0);
}

STATIC bool is_digit(mp_lexer_t *lex) {
    return unichar_isdigit(lex->chr0);
}

STATIC bool is_following_digit(mp_lexer_t *lex) {
    return unichar_isdigit(lex->chr1);
}

STATIC bool is_following_base_char(mp_lexer_t *lex) {
    const unichar chr1 = lex->chr1 | 0x20;
    return chr1 == 'b' || chr1 == 'o' || chr1 == 'x';
}

STATIC bool is_following_odigit(mp_lexer_t *lex) {
    return lex->chr1 >= '0' && lex->chr1 <= '7';
}

STATIC bool is_string_or_bytes(mp_lexer_t *lex) {
    return is_char_or(lex, '\'', '\"')
           || (is_char_or3(lex, 'r', 'u', 'b') && is_char_following_or(lex, '\'', '\"'))
           || ((is_char_and(lex, 'r', 'b') || is_char_and(lex, 'b', 'r'))
               && is_char_following_following_or(lex, '\'', '\"'));
}

// to easily parse utf-8 identifiers we allow any raw byte with high bit set
STATIC bool is_head_of_identifier(mp_lexer_t *lex) {
    return is_letter(lex) || lex->chr0 == '_' || lex->chr0 >= 0x80;
}

STATIC bool is_tail_of_identifier(mp_lexer_t *lex) {
    return is_head_of_identifier(lex) || is_digit(lex);
}

STATIC void next_char(mp_lexer_t *lex) {
    if (lex->chr0 == '\n') {
        // a new line
        ++lex->line;
        lex->column = 1;
    } else if (lex->chr0 == '\t') {
        // a tab
        lex->column = (((lex->column - 1 + TAB_SIZE) / TAB_SIZE) * TAB_SIZE) + 1;
    } else {
        // a character worth one column
        ++lex->column;
    }

    lex->chr0 = lex->chr1;
    lex->chr1 = lex->chr2;
    lex->chr2 = lex->reader.readbyte(lex->reader.data);

    if (lex->chr1 == '\r') {
        // CR is a new line, converted to LF
        lex->chr1 = '\n';
        if (lex->chr2 == '\n') {
            // CR LF is a single new line, throw out the extra LF
            lex->chr2 = lex->reader.readbyte(lex->reader.data);
        }
    }

    // check if we need to insert a newline at end of file
    if (lex->chr2 == MP_LEXER_EOF && lex->chr1 != MP_LEXER_EOF && lex->chr1 != '\n') {
        lex->chr2 = '\n';
    }
}

STATIC void indent_push(mp_lexer_t *lex, size_t indent) {
    if (lex->num_indent_level >= lex->alloc_indent_level) {
        lex->indent_level = m_renew(uint16_t, lex->indent_level, lex->alloc_indent_level, lex->alloc_indent_level + MICROPY_ALLOC_LEXEL_INDENT_INC);
        lex->alloc_indent_level += MICROPY_ALLOC_LEXEL_INDENT_INC;
    }
    lex->indent_level[lex->num_indent_level++] = indent;
}

STATIC size_t indent_top(mp_lexer_t *lex) {
    return lex->indent_level[lex->num_indent_level - 1];
}

STATIC void indent_pop(mp_lexer_t *lex) {
    lex->num_indent_level -= 1;
}

// some tricky operator encoding:
//     <op>  = begin with <op>, if this opchar matches then begin here
//     e<op> = end with <op>, if this opchar matches then end
//     c<op> = continue with <op>, if this opchar matches then continue matching
// this means if the start of two ops are the same then they are equal til the last char

STATIC const char *const tok_enc =
    "()[]{},;~"   // singles
    ":e="         // : :=
    "<e=c<e="     // < <= << <<=
    ">e=c>e="     // > >= >> >>=
    "*e=c*e="     // * *= ** **=
    "+e="         // + +=
    "-e=e>"       // - -= ->
    "&e="         // & &=
    "|e="         // | |=
    "/e=c/e="     // / /= // //=
    "%e="         // % %=
    "^e="         // ^ ^=
    "@e="         // @ @=
    "=e="         // = ==
    "!.";         // start of special cases: != . ...

// TODO static assert that number of tokens is less than 256 so we can safely make this table with byte sized entries
STATIC const uint8_t tok_enc_kind[] = {
    MP_TOKEN_DEL_PAREN_OPEN, MP_TOKEN_DEL_PAREN_CLOSE,
    MP_TOKEN_DEL_BRACKET_OPEN, MP_TOKEN_DEL_BRACKET_CLOSE,
    MP_TOKEN_DEL_BRACE_OPEN, MP_TOKEN_DEL_BRACE_CLOSE,
    MP_TOKEN_DEL_COMMA, MP_TOKEN_DEL_SEMICOLON, MP_TOKEN_OP_TILDE,

    MP_TOKEN_DEL_COLON, MP_TOKEN_OP_ASSIGN,
    MP_TOKEN_OP_LESS, MP_TOKEN_OP_LESS_EQUAL, MP_TOKEN_OP_DBL_LESS, MP_TOKEN_DEL_DBL_LESS_EQUAL,
    MP_TOKEN_OP_MORE, MP_TOKEN_OP_MORE_EQUAL, MP_TOKEN_OP_DBL_MORE, MP_TOKEN_DEL_DBL_MORE_EQUAL,
    MP_TOKEN_OP_STAR, MP_TOKEN_DEL_STAR_EQUAL, MP_TOKEN_OP_DBL_STAR, MP_TOKEN_DEL_DBL_STAR_EQUAL,
    MP_TOKEN_OP_PLUS, MP_TOKEN_DEL_PLUS_EQUAL,
    MP_TOKEN_OP_MINUS, MP_TOKEN_DEL_MINUS_EQUAL, MP_TOKEN_DEL_MINUS_MORE,
    MP_TOKEN_OP_AMPERSAND, MP_TOKEN_DEL_AMPERSAND_EQUAL,
    MP_TOKEN_OP_PIPE, MP_TOKEN_DEL_PIPE_EQUAL,
    MP_TOKEN_OP_SLASH, MP_TOKEN_DEL_SLASH_EQUAL, MP_TOKEN_OP_DBL_SLASH, MP_TOKEN_DEL_DBL_SLASH_EQUAL,
    MP_TOKEN_OP_PERCENT, MP_TOKEN_DEL_PERCENT_EQUAL,
    MP_TOKEN_OP_CARET, MP_TOKEN_DEL_CARET_EQUAL,
    MP_TOKEN_OP_AT, MP_TOKEN_DEL_AT_EQUAL,
    MP_TOKEN_DEL_EQUAL, MP_TOKEN_OP_DBL_EQUAL,
};

// must have the same order as enum in lexer.h
// must be sorted according to strcmp
STATIC const char *const tok_kw[] = {
    "False",
    "None",
    "True",
    "__debug__",
    "and",
    "as",
    "assert",
    #if MICROPY_PY_ASYNC_AWAIT
    "async",
    "await",
    #endif
    "break",
    "class",
    "continue",
    "def",
    "del",
    "elif",
    "else",
    "except",
    "finally",
    "for",
    "from",
    "global",
    "if",
    "import",
    "in",
    "is",
    "lambda",
    "nonlocal",
    "not",
    "or",
    "pass",
    "raise",
    "return",
    "try",
    "while",
    "with",
    "yield",
};

// This is called with CUR_CHAR() before first hex digit, and should return with
// it pointing to last hex digit
// num_digits must be greater than zero
STATIC bool get_hex(mp_lexer_t *lex, size_t num_digits, mp_uint_t *result) {
    mp_uint_t num = 0;
    while (num_digits-- != 0) {
        next_char(lex);
        unichar c = CUR_CHAR(lex);
        if (!unichar_isxdigit(c)) {
            return false;
        }
        num = (num << 4) + unichar_xdigit_value(c);
    }
    *result = num;
    return true;
}

STATIC void parse_string_literal(mp_lexer_t *lex, bool is_raw) {
    // get first quoting character
    char quote_char = '\'';
    if (is_char(lex, '\"')) {
        quote_char = '\"';
    }
    next_char(lex);

    // work out if it's a single or triple quoted literal
    size_t num_quotes;
    if (is_char_and(lex, quote_char, quote_char)) {
        // triple quotes
        next_char(lex);
        next_char(lex);
        num_quotes = 3;
    } else {
        // single quotes
        num_quotes = 1;
    }

    size_t n_closing = 0;
    while (!is_end(lex) && (num_quotes > 1 || !is_char(lex, '\n')) && n_closing < num_quotes) {
        if (is_char(lex, quote_char)) {
            n_closing += 1;
            vstr_add_char(&lex->vstr, CUR_CHAR(lex));
        } else {
            n_closing = 0;
            if (is_char(lex, '\\')) {
                next_char(lex);
                unichar c = CUR_CHAR(lex);
                if (is_raw) {
                    // raw strings allow escaping of quotes, but the backslash is also emitted
                    vstr_add_char(&lex->vstr, '\\');
                } else {
                    switch (c) {
                        // note: "c" can never be MP_LEXER_EOF because next_char
                        // always inserts a newline at the end of the input stream
                        case '\n':
                            c = MP_LEXER_EOF;
                            break;                          // backslash escape the newline, just ignore it
                        case '\\':
                            break;
                        case '\'':
                            break;
                        case '"':
                            break;
                        case 'a':
                            c = 0x07;
                            break;
                        case 'b':
                            c = 0x08;
                            break;
                        case 't':
                            c = 0x09;
                            break;
                        case 'n':
                            c = 0x0a;
                            break;
                        case 'v':
                            c = 0x0b;
                            break;
                        case 'f':
                            c = 0x0c;
                            break;
                        case 'r':
                            c = 0x0d;
                            break;
                        case 'u':
                        case 'U':
                            if (lex->tok_kind == MP_TOKEN_BYTES) {
                                // b'\u1234' == b'\\u1234'
                                vstr_add_char(&lex->vstr, '\\');
                                break;
                            }
                            // Otherwise fall through.
                            MP_FALLTHROUGH
                        case 'x': {
                            mp_uint_t num = 0;
                            if (!get_hex(lex, (c == 'x' ? 2 : c == 'u' ? 4 : 8), &num)) {
                                // not enough hex chars for escape sequence
                                lex->tok_kind = MP_TOKEN_INVALID;
                            }
                            c = num;
                            break;
                        }
                        case 'N':
                            // Supporting '\N{LATIN SMALL LETTER A}' == 'a' would require keeping the
                            // entire Unicode name table in the core. As of Unicode 6.3.0, that's nearly
                            // 3MB of text; even gzip-compressed and with minimal structure, it'll take
                            // roughly half a meg of storage. This form of Unicode escape may be added
                            // later on, but it's definitely not a priority right now. -- CJA 20140607
                            mp_raise_NotImplementedError(MP_ERROR_TEXT("unicode name escapes"));
                            break;
                        default:
                            if (c >= '0' && c <= '7') {
                                // Octal sequence, 1-3 chars
                                size_t digits = 3;
                                mp_uint_t num = c - '0';
                                while (is_following_odigit(lex) && --digits != 0) {
                                    next_char(lex);
                                    num = num * 8 + (CUR_CHAR(lex) - '0');
                                }
                                c = num;
                            } else {
                                // unrecognised escape character; CPython lets this through verbatim as '\' and then the character
                                vstr_add_char(&lex->vstr, '\\');
                            }
                            break;
                    }
                }
                if (c != MP_LEXER_EOF) {
                    if (MICROPY_PY_BUILTINS_STR_UNICODE_DYNAMIC) {
                        if (c < 0x110000 && lex->tok_kind == MP_TOKEN_STRING) {
                            vstr_add_char(&lex->vstr, c);
                        } else if (c < 0x100 && lex->tok_kind == MP_TOKEN_BYTES) {
                            vstr_add_byte(&lex->vstr, c);
                        } else {
                            // unicode character out of range
                            // this raises a generic SyntaxError; could provide more info
                            lex->tok_kind = MP_TOKEN_INVALID;
                        }
                    } else {
                        // without unicode everything is just added as an 8-bit byte
                        if (c < 0x100) {
                            vstr_add_byte(&lex->vstr, c);
                        } else {
                            // 8-bit character out of range
                            // this raises a generic SyntaxError; could provide more info
                            lex->tok_kind = MP_TOKEN_INVALID;
                        }
                    }
                }
            } else {
                // Add the "character" as a byte so that we remain 8-bit clean.
                // This way, strings are parsed correctly whether or not they contain utf-8 chars.
                vstr_add_byte(&lex->vstr, CUR_CHAR(lex));
            }
        }
        next_char(lex);
    }

    // check we got the required end quotes
    if (n_closing < num_quotes) {
        lex->tok_kind = MP_TOKEN_LONELY_STRING_OPEN;
    }

    // cut off the end quotes from the token text
    vstr_cut_tail_bytes(&lex->vstr, n_closing);
}

STATIC bool skip_whitespace(mp_lexer_t *lex, bool stop_at_newline) {
    bool had_physical_newline = false;
    while (!is_end(lex)) {
        if (is_physical_newline(lex)) {
            if (stop_at_newline && lex->nested_bracket_level == 0) {
                break;
            }
            had_physical_newline = true;
            next_char(lex);
        } else if (is_whitespace(lex)) {
            next_char(lex);
        } else if (is_char(lex, '#')) {
            next_char(lex);
            while (!is_end(lex) && !is_physical_newline(lex)) {
                next_char(lex);
            }
            // had_physical_newline will be set on next loop
        } else if (is_char_and(lex, '\\', '\n')) {
            // line-continuation, so don't set had_physical_newline
            next_char(lex);
            next_char(lex);
        } else {
            break;
        }
    }
    return had_physical_newline;
}

void mp_lexer_to_next(mp_lexer_t *lex) {
    // start new token text
    vstr_reset(&lex->vstr);

    // skip white space and comments
    bool had_physical_newline = skip_whitespace(lex, false);

    // set token source information
    lex->tok_line = lex->line;
    lex->tok_column = lex->column;

    if (lex->emit_dent < 0) {
        lex->tok_kind = MP_TOKEN_DEDENT;
        lex->emit_dent += 1;

    } else if (lex->emit_dent > 0) {
        lex->tok_kind = MP_TOKEN_INDENT;
        lex->emit_dent -= 1;

    } else if (had_physical_newline && lex->nested_bracket_level == 0) {
        lex->tok_kind = MP_TOKEN_NEWLINE;

        size_t num_spaces = lex->column - 1;
        if (num_spaces == indent_top(lex)) {
        } else if (num_spaces > indent_top(lex)) {
            indent_push(lex, num_spaces);
            lex->emit_dent += 1;
        } else {
            while (num_spaces < indent_top(lex)) {
                indent_pop(lex);
                lex->emit_dent -= 1;
            }
            if (num_spaces != indent_top(lex)) {
                lex->tok_kind = MP_TOKEN_DEDENT_MISMATCH;
            }
        }

    } else if (is_end(lex)) {
        lex->tok_kind = MP_TOKEN_END;

    } else if (is_string_or_bytes(lex)) {
        // a string or bytes literal

        // Python requires adjacent string/bytes literals to be automatically
        // concatenated.  We do it here in the tokeniser to make efficient use of RAM,
        // because then the lexer's vstr can be used to accumulate the string literal,
        // in contrast to creating a parse tree of strings and then joining them later
        // in the compiler.  It's also more compact in code size to do it here.

        // MP_TOKEN_END is used to indicate that this is the first string token
        lex->tok_kind = MP_TOKEN_END;

        // Loop to accumulate string/bytes literals
        do {
            // parse type codes
            bool is_raw = false;
            mp_token_kind_t kind = MP_TOKEN_STRING;
            int n_char = 0;
            if (is_char(lex, 'u')) {
                n_char = 1;
            } else if (is_char(lex, 'b')) {
                kind = MP_TOKEN_BYTES;
                n_char = 1;
                if (is_char_following(lex, 'r')) {
                    is_raw = true;
                    n_char = 2;
                }
            } else if (is_char(lex, 'r')) {
                is_raw = true;
                n_char = 1;
                if (is_char_following(lex, 'b')) {
                    kind = MP_TOKEN_BYTES;
                    n_char = 2;
                }
            }

            // Set or check token kind
            if (lex->tok_kind == MP_TOKEN_END) {
                lex->tok_kind = kind;
            } else if (lex->tok_kind != kind) {
                // Can't concatenate string with bytes
                break;
            }

            // Skip any type code characters
            if (n_char != 0) {
                next_char(lex);
                if (n_char == 2) {
                    next_char(lex);
                }
            }

            // Parse the literal
            parse_string_literal(lex, is_raw);

            // Skip whitespace so we can check if there's another string following
            skip_whitespace(lex, true);

        } while (is_string_or_bytes(lex));

    } else if (is_head_of_identifier(lex)) {
        lex->tok_kind = MP_TOKEN_NAME;

        // get first char (add as byte to remain 8-bit clean and support utf-8)
        vstr_add_byte(&lex->vstr, CUR_CHAR(lex));
        next_char(lex);

        // get tail chars
        while (!is_end(lex) && is_tail_of_identifier(lex)) {
            vstr_add_byte(&lex->vstr, CUR_CHAR(lex));
            next_char(lex);
        }

        // Check if the name is a keyword.
        // We also check for __debug__ here and convert it to its value.  This is
        // so the parser gives a syntax error on, eg, x.__debug__.  Otherwise, we
        // need to check for this special token in many places in the compiler.
        const char *s = vstr_null_terminated_str(&lex->vstr);
        for (size_t i = 0; i < MP_ARRAY_SIZE(tok_kw); i++) {
            int cmp = strcmp(s, tok_kw[i]);
            if (cmp == 0) {
                lex->tok_kind = MP_TOKEN_KW_FALSE + i;
                if (lex->tok_kind == MP_TOKEN_KW___DEBUG__) {
                    lex->tok_kind = (MP_STATE_VM(mp_optimise_value) == 0 ? MP_TOKEN_KW_TRUE : MP_TOKEN_KW_FALSE);
                }
                break;
            } else if (cmp < 0) {
                // Table is sorted and comparison was less-than, so stop searching
                break;
            }
        }

    } else if (is_digit(lex) || (is_char(lex, '.') && is_following_digit(lex))) {
        bool forced_integer = false;
        if (is_char(lex, '.')) {
            lex->tok_kind = MP_TOKEN_FLOAT_OR_IMAG;
        } else {
            lex->tok_kind = MP_TOKEN_INTEGER;
            if (is_char(lex, '0') && is_following_base_char(lex)) {
                forced_integer = true;
            }
        }

        // get first char
        vstr_add_char(&lex->vstr, CUR_CHAR(lex));
        next_char(lex);

        // get tail chars
        while (!is_end(lex)) {
            if (!forced_integer && is_char_or(lex, 'e', 'E')) {
                lex->tok_kind = MP_TOKEN_FLOAT_OR_IMAG;
                vstr_add_char(&lex->vstr, 'e');
                next_char(lex);
                if (is_char(lex, '+') || is_char(lex, '-')) {
                    vstr_add_char(&lex->vstr, CUR_CHAR(lex));
                    next_char(lex);
                }
            } else if (is_letter(lex) || is_digit(lex) || is_char(lex, '.')) {
                if (is_char_or3(lex, '.', 'j', 'J')) {
                    lex->tok_kind = MP_TOKEN_FLOAT_OR_IMAG;
                }
                vstr_add_char(&lex->vstr, CUR_CHAR(lex));
                next_char(lex);
            } else if (is_char(lex, '_')) {
                next_char(lex);
            } else {
                break;
            }
        }

    } else {
        // search for encoded delimiter or operator

        const char *t = tok_enc;
        size_t tok_enc_index = 0;
        for (; *t != 0 && !is_char(lex, *t); t += 1) {
            if (*t == 'e' || *t == 'c') {
                t += 1;
            }
            tok_enc_index += 1;
        }

        next_char(lex);

        if (*t == 0) {
            // didn't match any delimiter or operator characters
            lex->tok_kind = MP_TOKEN_INVALID;

        } else if (*t == '!') {
            // "!=" is a special case because "!" is not a valid operator
            if (is_char(lex, '=')) {
                next_char(lex);
                lex->tok_kind = MP_TOKEN_OP_NOT_EQUAL;
            } else {
                lex->tok_kind = MP_TOKEN_INVALID;
            }

        } else if (*t == '.') {
            // "." and "..." are special cases because ".." is not a valid operator
            if (is_char_and(lex, '.', '.')) {
                next_char(lex);
                next_char(lex);
                lex->tok_kind = MP_TOKEN_ELLIPSIS;
            } else {
                lex->tok_kind = MP_TOKEN_DEL_PERIOD;
            }

        } else {
            // matched a delimiter or operator character

            // get the maximum characters for a valid token
            t += 1;
            size_t t_index = tok_enc_index;
            while (*t == 'c' || *t == 'e') {
                t_index += 1;
                if (is_char(lex, t[1])) {
                    next_char(lex);
                    tok_enc_index = t_index;
                    if (*t == 'e') {
                        break;
                    }
                } else if (*t == 'c') {
                    break;
                }
                t += 2;
            }

            // set token kind
            lex->tok_kind = tok_enc_kind[tok_enc_index];

            // compute bracket level for implicit line joining
            if (lex->tok_kind == MP_TOKEN_DEL_PAREN_OPEN || lex->tok_kind == MP_TOKEN_DEL_BRACKET_OPEN || lex->tok_kind == MP_TOKEN_DEL_BRACE_OPEN) {
                lex->nested_bracket_level += 1;
            } else if (lex->tok_kind == MP_TOKEN_DEL_PAREN_CLOSE || lex->tok_kind == MP_TOKEN_DEL_BRACKET_CLOSE || lex->tok_kind == MP_TOKEN_DEL_BRACE_CLOSE) {
                lex->nested_bracket_level -= 1;
            }
        }
    }
}

mp_lexer_t *mp_lexer_new(qstr src_name, mp_reader_t reader) {
    mp_lexer_t *lex = m_new_obj(mp_lexer_t);

    lex->source_name = src_name;
    lex->reader = reader;
    lex->line = 1;
    lex->column = (size_t)-2; // account for 3 dummy bytes
    lex->emit_dent = 0;
    lex->nested_bracket_level = 0;
    lex->alloc_indent_level = MICROPY_ALLOC_LEXER_INDENT_INIT;
    lex->num_indent_level = 1;
    lex->indent_level = m_new(uint16_t, lex->alloc_indent_level);
    vstr_init(&lex->vstr, 32);

    // store sentinel for first indentation level
    lex->indent_level[0] = 0;

    // load lexer with start of file, advancing lex->column to 1
    // start with dummy bytes and use next_char() for proper EOL/EOF handling
    lex->chr0 = lex->chr1 = lex->chr2 = 0;
    next_char(lex);
    next_char(lex);
    next_char(lex);

    // preload first token
    mp_lexer_to_next(lex);

    // Check that the first token is in the first column.  If it's not then we
    // convert the token kind to INDENT so that the parser gives a syntax error.
    if (lex->tok_column != 1) {
        lex->tok_kind = MP_TOKEN_INDENT;
    }

    return lex;
}

mp_lexer_t *mp_lexer_new_from_str_len(qstr src_name, const char *str, size_t len, size_t free_len) {
    mp_reader_t reader;
    mp_reader_new_mem(&reader, (const byte *)str, len, free_len);
    return mp_lexer_new(src_name, reader);
}

#if MICROPY_READER_POSIX || MICROPY_READER_VFS

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_reader_t reader;
    mp_reader_new_file(&reader, filename);
    return mp_lexer_new(qstr_from_str(filename), reader);
}

#if MICROPY_HELPER_LEXER_UNIX

mp_lexer_t *mp_lexer_new_from_fd(qstr filename, int fd, bool close_fd) {
    mp_reader_t reader;
    mp_reader_new_file_from_fd(&reader, fd, close_fd);
    return mp_lexer_new(filename, reader);
}

#endif

#endif

void mp_lexer_free(mp_lexer_t *lex) {
    if (lex) {
        lex->reader.close(lex->reader.data);
        vstr_clear(&lex->vstr);
        m_del(uint16_t, lex->indent_level, lex->alloc_indent_level);
        m_del_obj(mp_lexer_t, lex);
    }
}

#if 0
// This function is used to print the current token and should only be
// needed to debug the lexer, so it's not available via a config option.
void mp_lexer_show_token(const mp_lexer_t *lex) {
    printf("(" UINT_FMT ":" UINT_FMT ") kind:%u str:%p len:%zu", lex->tok_line, lex->tok_column, lex->tok_kind, lex->vstr.buf, lex->vstr.len);
    if (lex->vstr.len > 0) {
        const byte *i = (const byte *)lex->vstr.buf;
        const byte *j = (const byte *)i + lex->vstr.len;
        printf(" ");
        while (i < j) {
            unichar c = utf8_get_char(i);
            i = utf8_next_char(i);
            if (unichar_isprint(c)) {
                printf("%c", (int)c);
            } else {
                printf("?");
            }
        }
    }
    printf("\n");
}
#endif

#endif // MICROPY_ENABLE_COMPILER
