// Helper functions for parsing t-string expressions
#ifndef MICROPY_INCLUDED_PY_TSTRING_EXPR_PARSER_H
#define MICROPY_INCLUDED_PY_TSTRING_EXPR_PARSER_H

#include "py/parse.h"

// Memory allocator function type for parser
typedef void *(*mp_parse_allocator_t)(void *ctx, size_t num_bytes);

#if MICROPY_PY_TSTRINGS
// Parse a t-string expression and return an AST node
// Uses MicroPython's existing parser infrastructure
mp_parse_node_t parse_tstring_expression(void *alloc_ctx, mp_parse_allocator_t allocator,
    const char *expr, size_t len);

// Copy a parse node tree using the provided allocator
mp_parse_node_t copy_parse_node(void *alloc_ctx, mp_parse_allocator_t allocator, mp_parse_node_t node);
#endif

#endif // MICROPY_INCLUDED_PY_TSTRING_EXPR_PARSER_H
