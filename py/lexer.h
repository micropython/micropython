/* lexer.h -- simple tokeniser for Python implementation
 */

#ifndef INCLUDED_LEXER_H
#define INCLUDED_LEXER_H

/* uses (byte) length instead of null termination
 * tokens are the same - UTF-8 with (byte) length
 */

typedef enum _py_token_kind_t {
    PY_TOKEN_END,                   // 0

    PY_TOKEN_INVALID,
    PY_TOKEN_LONELY_STRING_OPEN,

    PY_TOKEN_NEWLINE,               // 3
    PY_TOKEN_INDENT,                // 4
    PY_TOKEN_DEDENT,                // 5

    PY_TOKEN_NAME,                  // 6
    PY_TOKEN_NUMBER,
    PY_TOKEN_STRING,
    PY_TOKEN_BYTES,

    PY_TOKEN_ELLIPSES,

    PY_TOKEN_KW_FALSE,              // 11
    PY_TOKEN_KW_NONE,
    PY_TOKEN_KW_TRUE,
    PY_TOKEN_KW_AND,
    PY_TOKEN_KW_AS,
    PY_TOKEN_KW_ASSERT,
    PY_TOKEN_KW_BREAK,
    PY_TOKEN_KW_CLASS,
    PY_TOKEN_KW_CONTINUE,
    PY_TOKEN_KW_DEF,                // 20
    PY_TOKEN_KW_DEL,
    PY_TOKEN_KW_ELIF,
    PY_TOKEN_KW_ELSE,
    PY_TOKEN_KW_EXCEPT,
    PY_TOKEN_KW_FINALLY,
    PY_TOKEN_KW_FOR,
    PY_TOKEN_KW_FROM,
    PY_TOKEN_KW_GLOBAL,
    PY_TOKEN_KW_IF,
    PY_TOKEN_KW_IMPORT,             // 30
    PY_TOKEN_KW_IN,
    PY_TOKEN_KW_IS,
    PY_TOKEN_KW_LAMBDA,
    PY_TOKEN_KW_NONLOCAL,
    PY_TOKEN_KW_NOT,
    PY_TOKEN_KW_OR,
    PY_TOKEN_KW_PASS,
    PY_TOKEN_KW_RAISE,
    PY_TOKEN_KW_RETURN,
    PY_TOKEN_KW_TRY,                // 40
    PY_TOKEN_KW_WHILE,
    PY_TOKEN_KW_WITH,
    PY_TOKEN_KW_YIELD,

    PY_TOKEN_OP_PLUS,               // 44
    PY_TOKEN_OP_MINUS,
    PY_TOKEN_OP_STAR,
    PY_TOKEN_OP_DBL_STAR,
    PY_TOKEN_OP_SLASH,
    PY_TOKEN_OP_DBL_SLASH,
    PY_TOKEN_OP_PERCENT,
    PY_TOKEN_OP_LESS,
    PY_TOKEN_OP_DBL_LESS,
    PY_TOKEN_OP_MORE,
    PY_TOKEN_OP_DBL_MORE,           // 54
    PY_TOKEN_OP_AMPERSAND,
    PY_TOKEN_OP_PIPE,
    PY_TOKEN_OP_CARET,
    PY_TOKEN_OP_TILDE,
    PY_TOKEN_OP_LESS_EQUAL,
    PY_TOKEN_OP_MORE_EQUAL,
    PY_TOKEN_OP_DBL_EQUAL,
    PY_TOKEN_OP_NOT_EQUAL,

    PY_TOKEN_DEL_PAREN_OPEN,        // 63
    PY_TOKEN_DEL_PAREN_CLOSE,
    PY_TOKEN_DEL_BRACKET_OPEN,
    PY_TOKEN_DEL_BRACKET_CLOSE,
    PY_TOKEN_DEL_BRACE_OPEN,
    PY_TOKEN_DEL_BRACE_CLOSE,
    PY_TOKEN_DEL_COMMA,
    PY_TOKEN_DEL_COLON,
    PY_TOKEN_DEL_PERIOD,
    PY_TOKEN_DEL_SEMICOLON,
    PY_TOKEN_DEL_AT,                // 73
    PY_TOKEN_DEL_EQUAL,
    PY_TOKEN_DEL_PLUS_EQUAL,
    PY_TOKEN_DEL_MINUS_EQUAL,
    PY_TOKEN_DEL_STAR_EQUAL,
    PY_TOKEN_DEL_SLASH_EQUAL,
    PY_TOKEN_DEL_DBL_SLASH_EQUAL,
    PY_TOKEN_DEL_PERCENT_EQUAL,
    PY_TOKEN_DEL_AMPERSAND_EQUAL,
    PY_TOKEN_DEL_PIPE_EQUAL,
    PY_TOKEN_DEL_CARET_EQUAL,       // 83
    PY_TOKEN_DEL_DBL_MORE_EQUAL,
    PY_TOKEN_DEL_DBL_LESS_EQUAL,
    PY_TOKEN_DEL_DBL_STAR_EQUAL,
    PY_TOKEN_DEL_MINUS_MORE,
} py_token_kind_t;

typedef struct _py_token_t {
    const char *src_name;       // (file) name of source
    uint src_line;              // actual source line
    uint src_column;            // actual source column

    py_token_kind_t kind;       // kind of token
    uint cont_line;             // token belongs to this line in a continued line
    const char *str;            // string of token
    uint len;                   // (byte) length of string of token
} py_token_t;

typedef struct _py_lexer_t py_lexer_t;

void py_token_show(const py_token_t *tok);
void py_token_show_error_prefix(const py_token_t *tok);
bool py_token_show_error(const py_token_t *tok, const char *msg);

py_lexer_t *py_lexer_from_file(const char *filename);
py_lexer_t *py_lexer_from_str_len(const char *src_name, const char *str, uint len, bool free_str);
void py_lexer_free(py_lexer_t *lex);
void py_lexer_to_next(py_lexer_t *lex);
const py_token_t *py_lexer_cur(const py_lexer_t *lex);
bool py_lexer_is_kind(py_lexer_t *lex, py_token_kind_t kind);
/* unused
bool py_lexer_is_str(py_lexer_t *lex, const char *str);
bool py_lexer_is_next_kind(py_lexer_t *lex, py_token_kind_t kind);
bool py_lexer_is_next_str(py_lexer_t *lex, const char *str);
bool py_lexer_opt_kind(py_lexer_t *lex, py_token_kind_t kind);
bool py_lexer_opt_str(py_lexer_t *lex, const char *str);
*/
bool py_lexer_show_error(py_lexer_t *lex, const char *msg);

#endif /* INCLUDED_LEXER_H */
