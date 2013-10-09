/* lexer.c -- simple tokeniser for Python implementation
 */

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "misc.h"
#include "lexer.h"

#define TAB_SIZE (8)
#define CHR_EOF (-1)

struct _py_lexer_t {
    const char *name;           // (file) name of source
    bool free;                  // free source when done with it

    const char *src_beg;        // beginning of source
    const char *src_cur;        // current location in source; points to chr0
    const char *src_end;        // end (exclusive) of source
    unichar chr0, chr1, chr2;   // current characters from source

    uint line;                  // source line
    uint column;                // source column

    uint cont_line;             // continued line

    int emit_dent;
    int nested_bracket_level;

    uint alloc_indent_level;
    uint num_indent_level;
    uint16_t *indent_level;

    py_token_t tok_cur;
    py_token_t tok_next;
};

static bool py_token_is_str(const py_token_t *tok, const char *str) {
    uint i = 0;
    const char *tstr = tok->str;

    while (i < tok->len && *tstr == *str) {
        ++i;
        ++tstr;
        ++str;
    }

    return i == tok->len && *str == 0;
}

void py_token_show(const py_token_t *tok) {
    printf("(%s:%d:%d) kind:%d cont_line:%d str:%p len:%d", tok->src_name, tok->src_line, tok->src_column, tok->kind, tok->cont_line, tok->str, tok->len);
    if (tok->str != NULL && tok->len > 0) {
        const char *i = tok->str;
        const char *j = i + tok->len;
        printf(" ");
        while (i < j) {
            unichar c = g_utf8_get_char(i);
            i = g_utf8_next_char(i);
            if (g_unichar_isprint(c)) {
                printf("%c", c);
            } else {
                printf("?");
            }
        }
    }
    printf("\n");
}

void py_token_show_error_prefix(const py_token_t *tok) {
    printf("(%s:%d:%d) ", tok->src_name, tok->src_line, tok->src_column);
}

bool py_token_show_error(const py_token_t *tok, const char *msg) {
    printf("(%s:%d:%d) %s\n", tok->src_name, tok->src_line, tok->src_column, msg);
    return false;
}

static bool is_end(py_lexer_t *lex) {
    return lex->chr0 == CHR_EOF;
}

static bool is_physical_newline(py_lexer_t *lex) {
    return lex->chr0 == '\n' || lex->chr0 == '\r';
}

static bool is_char(py_lexer_t *lex, char c) {
    return lex->chr0 == c;
}

static bool is_char_or(py_lexer_t *lex, char c1, char c2) {
    return lex->chr0 == c1 || lex->chr0 == c2;
}

static bool is_char_or3(py_lexer_t *lex, char c1, char c2, char c3) {
    return lex->chr0 == c1 || lex->chr0 == c2 || lex->chr0 == c3;
}

/*
static bool is_char_following(py_lexer_t *lex, char c) {
    return lex->chr1 == c;
}
*/

static bool is_char_following_or(py_lexer_t *lex, char c1, char c2) {
    return lex->chr1 == c1 || lex->chr1 == c2;
}

static bool is_char_following_following_or(py_lexer_t *lex, char c1, char c2) {
    return lex->chr2 == c1 || lex->chr2 == c2;
}

static bool is_char_and(py_lexer_t *lex, char c1, char c2) {
    return lex->chr0 == c1 && lex->chr1 == c2;
}

static bool is_whitespace(py_lexer_t *lex) {
    return g_unichar_isspace(lex->chr0);
}

static bool is_letter(py_lexer_t *lex) {
    return g_unichar_isalpha(lex->chr0);
}

static bool is_digit(py_lexer_t *lex) {
    return g_unichar_isdigit(lex->chr0);
}

static bool is_following_digit(py_lexer_t *lex) {
    return g_unichar_isdigit(lex->chr1);
}

// TODO UNICODE include unicode characters in definition of identifiers
static bool is_head_of_identifier(py_lexer_t *lex) {
    return is_letter(lex) || lex->chr0 == '_';
}

// TODO UNICODE include unicode characters in definition of identifiers
static bool is_tail_of_identifier(py_lexer_t *lex) {
    return is_head_of_identifier(lex) || is_digit(lex);
}

static void next_char(py_lexer_t *lex) {
    if (lex->chr0 == CHR_EOF) {
        return;
    }

    int advance = 1;

    if (lex->chr0 == '\n') {
        // LF is a new line
        ++lex->line;
        lex->column = 1;
        lex->cont_line = lex->line;
    } else if (lex->chr0 == '\r') {
        // CR is a new line
        ++lex->line;
        lex->column = 1;
        lex->cont_line = lex->line;
        if (lex->chr1 == '\n') {
            // CR LF is a single new line
            advance = 2;
        }
    } else if (lex->chr0 == '\t') {
        // a tab
        lex->column = (((lex->column - 1 + TAB_SIZE) / TAB_SIZE) * TAB_SIZE) + 1;
    } else {
        // a character worth one column
        ++lex->column;
    }

    for (; advance > 0; advance--) {
        lex->chr0 = lex->chr1;
        lex->chr1 = lex->chr2;
        lex->src_cur++;
        if (lex->src_cur + 2 < lex->src_end) {
            lex->chr2 = lex->src_cur[2];
        } else {
            // EOF
            if (lex->chr1 != '\n' && lex->chr1 != '\r') {
                lex->chr2 = '\n'; // insert newline at end of file
            } else {
                lex->chr2 = CHR_EOF;
            }
        }
    }
}

void indent_push(py_lexer_t *lex, uint indent) {
    if (lex->num_indent_level >= lex->alloc_indent_level) {
        lex->alloc_indent_level *= 2;
        lex->indent_level = m_renew(uint16_t, lex->indent_level, lex->alloc_indent_level);
    }
    lex->indent_level[lex->num_indent_level++] = indent;
}

uint indent_top(py_lexer_t *lex) {
    return lex->indent_level[lex->num_indent_level - 1];
}

void indent_pop(py_lexer_t *lex) {
    lex->num_indent_level -= 1;
}

// some tricky operator encoding:
//     <op>  = begin with <op>, if this opchar matches then begin here
//     e<op> = end with <op>, if this opchar matches then end
//     E<op> = mandatory end with <op>, this opchar must match, then end
//     c<op> = continue with <op>, if this opchar matches then continue matching
// this means if the start of two ops are the same then they are equal til the last char

static const char *tok_enc =
    "()[]{},:;@~" // singles
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
    "=e="         // = ==
    "!E="         // !=
    ".c.E.";      // . ...

// TODO static assert that number of tokens is less than 256 so we can safely make this table with byte sized entries
static const uint8_t tok_enc_kind[] = {
    PY_TOKEN_DEL_PAREN_OPEN, PY_TOKEN_DEL_PAREN_CLOSE,
    PY_TOKEN_DEL_BRACKET_OPEN, PY_TOKEN_DEL_BRACKET_CLOSE,
    PY_TOKEN_DEL_BRACE_OPEN, PY_TOKEN_DEL_BRACE_CLOSE,
    PY_TOKEN_DEL_COMMA, PY_TOKEN_DEL_COLON, PY_TOKEN_DEL_SEMICOLON, PY_TOKEN_DEL_AT, PY_TOKEN_OP_TILDE,

    PY_TOKEN_OP_LESS, PY_TOKEN_OP_LESS_EQUAL, PY_TOKEN_OP_DBL_LESS, PY_TOKEN_DEL_DBL_LESS_EQUAL,
    PY_TOKEN_OP_MORE, PY_TOKEN_OP_MORE_EQUAL, PY_TOKEN_OP_DBL_MORE, PY_TOKEN_DEL_DBL_MORE_EQUAL,
    PY_TOKEN_OP_STAR, PY_TOKEN_DEL_STAR_EQUAL, PY_TOKEN_OP_DBL_STAR, PY_TOKEN_DEL_DBL_STAR_EQUAL,
    PY_TOKEN_OP_PLUS, PY_TOKEN_DEL_PLUS_EQUAL,
    PY_TOKEN_OP_MINUS, PY_TOKEN_DEL_MINUS_EQUAL, PY_TOKEN_DEL_MINUS_MORE,
    PY_TOKEN_OP_AMPERSAND, PY_TOKEN_DEL_AMPERSAND_EQUAL,
    PY_TOKEN_OP_PIPE, PY_TOKEN_DEL_PIPE_EQUAL,
    PY_TOKEN_OP_SLASH, PY_TOKEN_DEL_SLASH_EQUAL, PY_TOKEN_OP_DBL_SLASH, PY_TOKEN_DEL_DBL_SLASH_EQUAL,
    PY_TOKEN_OP_PERCENT, PY_TOKEN_DEL_PERCENT_EQUAL,
    PY_TOKEN_OP_CARET, PY_TOKEN_DEL_CARET_EQUAL,
    PY_TOKEN_DEL_EQUAL, PY_TOKEN_OP_DBL_EQUAL,
    PY_TOKEN_OP_NOT_EQUAL,
    PY_TOKEN_DEL_PERIOD, PY_TOKEN_ELLIPSES,
};

// must have the same order as enum in lexer.h
static const char *tok_kw[] = {
    "False",
    "None",
    "True",
    "and",
    "as",
    "assert",
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
    NULL,
};

static void py_lexer_next_token_into(py_lexer_t *lex, py_token_t *tok) {
    bool had_physical_newline = false;

    while (!is_end(lex)) {
        if (is_physical_newline(lex)) {
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
        } else if (is_char(lex, '\\')) {
            // backslash (outside string literals) must appear just before a physical newline
            next_char(lex);
            if (!is_physical_newline(lex)) {
                // TODO SyntaxError
                assert(0);
            } else {
                next_char(lex);
            }
        } else {
            break;
        }
    }

    tok->src_name = lex->name;
    tok->src_line = lex->line;
    tok->src_column = lex->column;
    tok->kind = PY_TOKEN_INVALID;
    tok->cont_line = lex->cont_line;
    tok->str = lex->src_cur;
    tok->len = 0;

    if (lex->emit_dent < 0) {
        tok->kind = PY_TOKEN_DEDENT;
        lex->emit_dent += 1;

    } else if (lex->emit_dent > 0) {
        tok->kind = PY_TOKEN_INDENT;
        lex->emit_dent -= 1;

    } else if (had_physical_newline && lex->nested_bracket_level == 0) {
        tok->kind = PY_TOKEN_NEWLINE;

        uint num_spaces = lex->column - 1;
        lex->emit_dent = 0;
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
                tok->kind = PY_TOKEN_DEDENT_MISMATCH;
            }
        }

    } else if (is_end(lex)) {
        if (indent_top(lex) > 0) {
            tok->kind = PY_TOKEN_NEWLINE;
            lex->emit_dent = 0;
            while (indent_top(lex) > 0) {
                indent_pop(lex);
                lex->emit_dent -= 1;
            }
        } else {
            tok->kind = PY_TOKEN_END;
        }

    } else if (is_char_or(lex, '\'', '\"')
               || (is_char_or3(lex, 'r', 'u', 'b') && is_char_following_or(lex, '\'', '\"'))
               || ((is_char_and(lex, 'r', 'b') || is_char_and(lex, 'b', 'r')) && is_char_following_following_or(lex, '\'', '\"'))) {
        // a string or bytes literal

        // parse type codes
        bool is_raw = false;
        bool is_bytes = false;
        if (is_char(lex, 'u')) {
            next_char(lex);
        } else if (is_char(lex, 'b')) {
            is_bytes = true;
            next_char(lex);
            if (is_char(lex, 'r')) {
                is_raw = true;
                next_char(lex);
            }
        } else if (is_char(lex, 'r')) {
            is_raw = true;
            next_char(lex);
            if (is_char(lex, 'b')) {
                is_bytes = true;
                next_char(lex);
            }
        }

        // set token kind
        if (is_bytes) {
            tok->kind = PY_TOKEN_BYTES;
        } else {
            tok->kind = PY_TOKEN_STRING;
        }

        // get first quoting character
        char quote_char = '\'';
        if (is_char(lex, '\"')) {
            quote_char = '\"';
        }
        next_char(lex);

        // work out if it's a single or triple quoted literal
        int num_quotes;
        if (is_char_and(lex, quote_char, quote_char)) {
            // triple quotes
            next_char(lex);
            next_char(lex);
            num_quotes = 3;
        } else {
            // single quotes
            num_quotes = 1;
        }

        // set start of token
        tok->str = lex->src_cur;

        // parse the literal
        // TODO proper escaping
        int n_closing = 0;
        while (!is_end(lex) && (num_quotes > 1 || !is_char(lex, '\n')) && n_closing < num_quotes) {
            if (is_char(lex, quote_char)) {
                n_closing += 1;
            } else {
                n_closing = 0;
                if (!is_raw && is_char(lex, '\\')) {
                    next_char(lex);
                }
            }
            next_char(lex);
        }

        // check we got the required end quotes
        if (n_closing < num_quotes) {
            tok->kind = PY_TOKEN_LONELY_STRING_OPEN;
        }

        // set token string (byte) length
        tok->len = lex->src_cur - tok->str - n_closing;

        // we set the length, return now so it's not set incorrectly below
        return;

    } else if (is_head_of_identifier(lex)) {
        tok->kind = PY_TOKEN_NAME;

        next_char(lex);

        while (!is_end(lex) && is_tail_of_identifier(lex)) {
            next_char(lex);
        }

    } else if (is_digit(lex) || (is_char(lex, '.') && is_following_digit(lex))) {
        tok->kind = PY_TOKEN_NUMBER;

        next_char(lex);

        while (!is_end(lex)) {
            if (is_char_or(lex, 'e', 'E')) {
                next_char(lex);
                if (is_char(lex, '+') || is_char(lex, '-')) {
                    next_char(lex);
                }
            } else if (is_letter(lex) || is_digit(lex) || is_char_or(lex, '_', '.')) {
                next_char(lex);
            } else {
                break;
            }
        }

    } else {
        // search for encoded delimiter or operator

        const char *t = tok_enc;
        uint tok_enc_index = 0;
        for (; *t != 0 && !is_char(lex, *t); t += 1) {
            if (*t == 'e' || *t == 'c') {
                t += 1;
            } else if (*t == 'E') {
                tok_enc_index -= 1;
                t += 1;
            }
            tok_enc_index += 1;
        }

        next_char(lex);

        if (*t == 0) {
            // didn't match any delimiter or operator characters
            tok->kind = PY_TOKEN_INVALID;

        } else {
            // matched a delimiter or operator character

            // get the maximum characters for a valid token
            t += 1;
            uint t_index = tok_enc_index;
            for (;;) {
                for (; *t == 'e'; t += 1) {
                    t += 1;
                    t_index += 1;
                    if (is_char(lex, *t)) {
                        next_char(lex);
                        tok_enc_index = t_index;
                        break;
                    }
                }

                if (*t == 'E') {
                    t += 1;
                    if (is_char(lex, *t)) {
                        next_char(lex);
                        tok_enc_index = t_index;
                    } else {
                        tok->kind = PY_TOKEN_INVALID;
                    }
                    break;
                }

                if (*t == 'c') {
                    t += 1;
                    t_index += 1;
                    if (is_char(lex, *t)) {
                        next_char(lex);
                        tok_enc_index = t_index;
                        t += 1;
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }

            // set token kind
            tok->kind = tok_enc_kind[tok_enc_index];

            // compute bracket level for implicit line joining
            if (tok->kind == PY_TOKEN_DEL_PAREN_OPEN || tok->kind == PY_TOKEN_DEL_BRACKET_OPEN || tok->kind == PY_TOKEN_DEL_BRACE_OPEN) {
                lex->nested_bracket_level += 1;
            } else if (tok->kind == PY_TOKEN_DEL_PAREN_CLOSE || tok->kind == PY_TOKEN_DEL_BRACKET_CLOSE || tok->kind == PY_TOKEN_DEL_BRACE_CLOSE) {
                lex->nested_bracket_level -= 1;
            }
        }
    }

    // set token string (byte) length
    tok->len = lex->src_cur - tok->str;

    // check for keywords (must be done after setting token string length)
    if (tok->kind == PY_TOKEN_NAME) {
        for (int i = 0; tok_kw[i] != NULL; i++) {
            if (py_token_is_str(tok, tok_kw[i])) {
                tok->kind = PY_TOKEN_KW_FALSE + i;
                break;
            }
        }
    }
}

py_lexer_t *py_lexer_from_str_len(const char *src_name, const char *str, uint len, bool free_str) {
    py_lexer_t *lex;

    lex = m_new(py_lexer_t, 1);

    //lex->name = g_strdup(src_name); // TODO
    lex->name = src_name;
    lex->free = free_str;
    lex->src_beg = str;
    lex->src_cur = str;
    lex->src_end = str + len;
    lex->line = 1;
    lex->column = 1;
    lex->cont_line = lex->line;
    lex->emit_dent = 0;
    lex->nested_bracket_level = 0;
    lex->alloc_indent_level = 16;
    lex->num_indent_level = 1;
    lex->indent_level = m_new(uint16_t, lex->alloc_indent_level);
    lex->indent_level[0] = 0;

    // preload characters
    // TODO unicode
    if (len == 0) {
        lex->chr0 = '\n'; // insert newline at end of file
        lex->chr1 = CHR_EOF;
        lex->chr2 = CHR_EOF;
    } else if (len == 1) {
        lex->chr0 = str[0];
        if (lex->chr0 != '\n' && lex->chr0 != '\r') {
            lex->chr1 = '\n'; // insert newline at end of file
        } else {
            lex->chr1 = CHR_EOF;
        }
        lex->chr2 = CHR_EOF;
    } else if (len == 2) {
        lex->chr0 = str[0];
        lex->chr1 = str[1];
        if (lex->chr1 != '\n' && lex->chr1 != '\r') {
            lex->chr2 = '\n'; // insert newline at end of file
        } else {
            lex->chr2 = CHR_EOF;
        }
    } else {
        lex->chr0 = str[0];
        lex->chr1 = str[1];
        lex->chr2 = str[2];
    }

    py_lexer_next_token_into(lex, &lex->tok_cur);

    // check that the first token is in the first column
    // (done to get equivalence with CPython)
    if (lex->tok_cur.src_line == 1 && lex->tok_cur.src_column != 1) {
        lex->tok_next = lex->tok_cur;
        lex->tok_cur.kind = PY_TOKEN_INDENT;
    } else {
        py_lexer_next_token_into(lex, &lex->tok_next);
    }

    return lex;
}

void py_lexer_free(py_lexer_t *lex) {
    if (lex == NULL) {
        return;
    }
    //m_free(lex->name);
    if (lex->free) {
        m_free((char*)lex->src_beg);
    }
    m_free(lex);
}

void py_lexer_to_next(py_lexer_t *lex) {
    lex->tok_cur = lex->tok_next;
    py_lexer_next_token_into(lex, &lex->tok_next);
}

const py_token_t *py_lexer_cur(const py_lexer_t *lex) {
    return &lex->tok_cur;
}

bool py_lexer_is_kind(py_lexer_t *lex, py_token_kind_t kind) {
    return lex->tok_cur.kind == kind;
}

/*
bool py_lexer_is_str(py_lexer_t *lex, const char *str) {
    return py_token_is_str(&lex->tok_cur, str);
}

bool py_lexer_is_next_kind(py_lexer_t *lex, py_token_kind_t kind) {
    return lex->tok_next.kind == kind;
}

bool py_lexer_is_next_str(py_lexer_t *lex, const char *str) {
    return py_token_is_str(&lex->tok_next, str);
}

bool py_lexer_opt_kind(py_lexer_t *lex, py_token_kind_t kind) {
    if (py_lexer_is_kind(lex, kind)) {
        py_lexer_to_next(lex);
        return true;
    }
    return false;
}

bool py_lexer_opt_str(py_lexer_t *lex, const char *str) {
    if (py_lexer_is_str(lex, str)) {
        py_lexer_to_next(lex);
        return true;
    }
    return false;
}
*/

bool py_lexer_show_error(py_lexer_t *lex, const char *msg) {
    return py_token_show_error(&lex->tok_cur, msg);
}

bool py_lexer_show_error_pythonic(py_lexer_t *lex, const char *msg) {
    printf("  File \"%s\", line %d column %d\n%s\n", lex->tok_cur.src_name, lex->tok_cur.src_line, lex->tok_cur.src_column, msg);
    return false;
}
