/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h> // for ssize_t
#include <assert.h>
#include <string.h>

#include "py/lexer.h"
#include "py/parse.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "py/objint.h"
#include "py/objstr.h"
#include "py/objlist.h"
#include "py/objtuple.h"
#include "py/builtin.h"
#include "py/compile.h"

#if MICROPY_PY_TSTRINGS
#include "py/tstring_expr_parser.h"

// Forward declaration
struct _parser_t;
static void *parser_alloc(struct _parser_t *parser, size_t num_bytes);

// Wrapper function for parser_alloc to match the allocator interface
static void *tstring_parser_alloc_wrapper(void *ctx, size_t num_bytes) {
    return parser_alloc((struct _parser_t *)ctx, num_bytes);
}
#endif

#if MICROPY_ENABLE_COMPILER

#define RULE_ACT_ARG_MASK       (0x0f)
#define RULE_ACT_KIND_MASK      (0x30)
#define RULE_ACT_ALLOW_IDENT    (0x40)
#define RULE_ACT_ADD_BLANK      (0x80)
#define RULE_ACT_OR             (0x10)
#define RULE_ACT_AND            (0x20)
#define RULE_ACT_LIST           (0x30)

#define RULE_ARG_KIND_MASK      (0xf000)
#define RULE_ARG_ARG_MASK       (0x0fff)
#define RULE_ARG_TOK            (0x1000)
#define RULE_ARG_RULE           (0x2000)
#define RULE_ARG_OPT_RULE       (0x3000)

// *FORMAT-OFF*

enum {
// define rules with a compile function
#define DEF_RULE(rule, comp, kind, ...) RULE_##rule,
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
    RULE_const_object, // special node for a constant, generic Python object

// define rules without a compile function
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) RULE_##rule,
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
};

// Define an array of actions corresponding to each rule
static const uint8_t rule_act_table[] = {
#define or(n)                   (RULE_ACT_OR | n)
#define and(n)                  (RULE_ACT_AND | n)
#define and_ident(n)            (RULE_ACT_AND | n | RULE_ACT_ALLOW_IDENT)
#define and_blank(n)            (RULE_ACT_AND | n | RULE_ACT_ADD_BLANK)
#define one_or_more             (RULE_ACT_LIST | 2)
#define list                    (RULE_ACT_LIST | 1)
#define list_with_end           (RULE_ACT_LIST | 3)

#define DEF_RULE(rule, comp, kind, ...) kind,
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC

    0, // RULE_const_object

#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) kind,
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC

#undef or
#undef and
#undef and_ident
#undef and_blank
#undef one_or_more
#undef list
#undef list_with_end
};

// Define the argument data for each rule, as a combined array
static const uint16_t rule_arg_combined_table[] = {
#define tok(t)                  (RULE_ARG_TOK | MP_TOKEN_##t)
#define rule(r)                 (RULE_ARG_RULE | RULE_##r)
#define opt_rule(r)             (RULE_ARG_OPT_RULE | RULE_##r)

#define DEF_RULE(rule, comp, kind, ...) __VA_ARGS__,
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC

#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...)  __VA_ARGS__,
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC

#undef tok
#undef rule
#undef opt_rule
};

// Macro to create a list of N identifiers where N is the number of variable arguments to the macro
#define RULE_EXPAND(x) x
#define RULE_PADDING(rule, ...) RULE_PADDING2(rule, __VA_ARGS__, RULE_PADDING_IDS(rule))
#define RULE_PADDING2(rule, ...) RULE_EXPAND(RULE_PADDING3(rule, __VA_ARGS__))
#define RULE_PADDING3(rule, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, ...) __VA_ARGS__
#define RULE_PADDING_IDS(r) PAD13_##r, PAD12_##r, PAD11_##r, PAD10_##r, PAD9_##r, PAD8_##r, PAD7_##r, PAD6_##r, PAD5_##r, PAD4_##r, PAD3_##r, PAD2_##r, PAD1_##r,

// Use an enum to create constants specifying how much room a rule takes in rule_arg_combined_table
enum {
#define DEF_RULE(rule, comp, kind, ...) RULE_PADDING(rule, __VA_ARGS__)
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) RULE_PADDING(rule, __VA_ARGS__)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
};

// Macro to compute the start of a rule in rule_arg_combined_table
#define RULE_ARG_OFFSET(rule, ...) RULE_ARG_OFFSET2(rule, __VA_ARGS__, RULE_ARG_OFFSET_IDS(rule))
#define RULE_ARG_OFFSET2(rule, ...) RULE_EXPAND(RULE_ARG_OFFSET3(rule, __VA_ARGS__))
#define RULE_ARG_OFFSET3(rule, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, ...) _14
#define RULE_ARG_OFFSET_IDS(r) PAD13_##r, PAD12_##r, PAD11_##r, PAD10_##r, PAD9_##r, PAD8_##r, PAD7_##r, PAD6_##r, PAD5_##r, PAD4_##r, PAD3_##r, PAD2_##r, PAD1_##r, PAD0_##r,

// Use the above enum values to create a table of offsets for each rule's arg
// data, which indexes rule_arg_combined_table.  The offsets require 9 bits of
// storage but only the lower 8 bits are stored here.  The 9th bit is computed
// in get_rule_arg using the FIRST_RULE_WITH_OFFSET_ABOVE_255 constant.
static const uint8_t rule_arg_offset_table[] = {
#define DEF_RULE(rule, comp, kind, ...) RULE_ARG_OFFSET(rule, __VA_ARGS__) & 0xff,
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
    0, // RULE_const_object
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) RULE_ARG_OFFSET(rule, __VA_ARGS__) & 0xff,
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
};

// Define a constant that's used to determine the 9th bit of the values in rule_arg_offset_table
static const size_t FIRST_RULE_WITH_OFFSET_ABOVE_255 =
#define DEF_RULE(rule, comp, kind, ...) RULE_ARG_OFFSET(rule, __VA_ARGS__) >= 0x100 ? RULE_##rule :
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) RULE_ARG_OFFSET(rule, __VA_ARGS__) >= 0x100 ? RULE_##rule :
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
0;

#if MICROPY_DEBUG_PARSE_RULE_NAME
// Define an array of rule names corresponding to each rule
static const char *const rule_name_table[] = {
#define DEF_RULE(rule, comp, kind, ...) #rule,
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
    "", // RULE_const_object
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) #rule,
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
};
#endif

// *FORMAT-ON*

typedef struct _rule_stack_t {
    size_t src_line : (8 * sizeof(size_t) - 8); // maximum bits storing source line number
    size_t rule_id : 8; // this must be large enough to fit largest rule number
    size_t arg_i; // this dictates the maximum nodes in a "list" of things
} rule_stack_t;

typedef struct _mp_parse_chunk_t {
    size_t alloc;
    union {
        size_t used;
        struct _mp_parse_chunk_t *next;
    } union_;
    byte data[];
} mp_parse_chunk_t;

typedef struct _parser_t {
    size_t rule_stack_alloc;
    size_t rule_stack_top;
    rule_stack_t *rule_stack;

    size_t result_stack_alloc;
    size_t result_stack_top;
    mp_parse_node_t *result_stack;

    mp_lexer_t *lexer;

    mp_parse_tree_t tree;
    mp_parse_chunk_t *cur_chunk;

    #if MICROPY_COMP_CONST
    mp_map_t consts;
    #endif
} parser_t;

static void push_result_rule(parser_t *parser, size_t src_line, uint8_t rule_id, size_t num_args);
static void push_result_token(parser_t *parser, uint8_t rule_id);

static const uint16_t *get_rule_arg(uint8_t r_id) {
    size_t off = rule_arg_offset_table[r_id];
    if (r_id >= FIRST_RULE_WITH_OFFSET_ABOVE_255) {
        off |= 0x100;
    }
    return &rule_arg_combined_table[off];
}

static void *parser_alloc(parser_t *parser, size_t num_bytes) {
    // use a custom memory allocator to store parse nodes sequentially in large chunks

    mp_parse_chunk_t *chunk = parser->cur_chunk;

    if (chunk != NULL && chunk->union_.used + num_bytes > chunk->alloc) {
        // not enough room at end of previously allocated chunk so try to grow
        mp_parse_chunk_t *new_data = (mp_parse_chunk_t *)m_renew_maybe(byte, chunk,
            sizeof(mp_parse_chunk_t) + chunk->alloc,
            sizeof(mp_parse_chunk_t) + chunk->alloc + num_bytes, false);
        if (new_data == NULL) {
            // could not grow existing memory; shrink it to fit previous
            (void)m_renew_maybe(byte, chunk, sizeof(mp_parse_chunk_t) + chunk->alloc,
                sizeof(mp_parse_chunk_t) + chunk->union_.used, false);
            chunk->alloc = chunk->union_.used;
            chunk->union_.next = parser->tree.chunk;
            parser->tree.chunk = chunk;
            chunk = NULL;
        } else {
            // could grow existing memory
            chunk->alloc += num_bytes;
        }
    }

    if (chunk == NULL) {
        // no previous chunk, allocate a new chunk
        size_t alloc = MICROPY_ALLOC_PARSE_CHUNK_INIT;
        if (alloc < num_bytes) {
            alloc = num_bytes;
        }
        chunk = (mp_parse_chunk_t *)m_new(byte, sizeof(mp_parse_chunk_t) + alloc);
        chunk->alloc = alloc;
        chunk->union_.used = 0;
        parser->cur_chunk = chunk;
    }

    byte *ret = chunk->data + chunk->union_.used;
    chunk->union_.used += num_bytes;
    return ret;
}

#if MICROPY_COMP_CONST_TUPLE
static void parser_free_parse_node_struct(parser_t *parser, mp_parse_node_struct_t *pns) {
    mp_parse_chunk_t *chunk = parser->cur_chunk;
    if (chunk->data <= (byte *)pns && (byte *)pns < chunk->data + chunk->union_.used) {
        size_t num_bytes = sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        chunk->union_.used -= num_bytes;
    }
}
#endif

static void push_rule(parser_t *parser, size_t src_line, uint8_t rule_id, size_t arg_i) {
    if (parser->rule_stack_top >= parser->rule_stack_alloc) {
        rule_stack_t *rs = m_renew(rule_stack_t, parser->rule_stack, parser->rule_stack_alloc, parser->rule_stack_alloc + MICROPY_ALLOC_PARSE_RULE_INC);
        parser->rule_stack = rs;
        parser->rule_stack_alloc += MICROPY_ALLOC_PARSE_RULE_INC;
    }
    rule_stack_t *rs = &parser->rule_stack[parser->rule_stack_top++];
    rs->src_line = src_line;
    rs->rule_id = rule_id;
    rs->arg_i = arg_i;
}

static void push_rule_from_arg(parser_t *parser, size_t arg) {
    assert((arg & RULE_ARG_KIND_MASK) == RULE_ARG_RULE || (arg & RULE_ARG_KIND_MASK) == RULE_ARG_OPT_RULE);
    size_t rule_id = arg & RULE_ARG_ARG_MASK;
    push_rule(parser, parser->lexer->tok_line, rule_id, 0);
}

static uint8_t pop_rule(parser_t *parser, size_t *arg_i, size_t *src_line) {
    parser->rule_stack_top -= 1;
    uint8_t rule_id = parser->rule_stack[parser->rule_stack_top].rule_id;
    *arg_i = parser->rule_stack[parser->rule_stack_top].arg_i;
    *src_line = parser->rule_stack[parser->rule_stack_top].src_line;
    return rule_id;
}

#if MICROPY_COMP_CONST_TUPLE
static uint8_t peek_rule(parser_t *parser, size_t n) {
    assert(parser->rule_stack_top > n);
    return parser->rule_stack[parser->rule_stack_top - 1 - n].rule_id;
}
#endif

#if MICROPY_COMP_CONST_FOLDING || MICROPY_EMIT_INLINE_ASM
static bool mp_parse_node_get_number_maybe(mp_parse_node_t pn, mp_obj_t *o) {
    if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        *o = MP_OBJ_NEW_SMALL_INT(MP_PARSE_NODE_LEAF_SMALL_INT(pn));
        return true;
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, RULE_const_object)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
        *o = mp_parse_node_extract_const_object(pns);
        return mp_obj_is_int(*o)
               #if MICROPY_COMP_CONST_FLOAT
               || mp_obj_is_float(*o)
               #endif
        ;
    } else {
        return false;
    }
}
#endif

#if MICROPY_EMIT_INLINE_ASM
bool mp_parse_node_get_int_maybe(mp_parse_node_t pn, mp_obj_t *o) {
    return mp_parse_node_get_number_maybe(pn, o)
           #if MICROPY_COMP_CONST_FLOAT
           && mp_obj_is_int(*o)
           #endif
    ;
}
#endif

#if MICROPY_COMP_CONST_TUPLE || MICROPY_COMP_CONST
static bool mp_parse_node_is_const(mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        // Small integer.
        return true;
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        // Possible str, or constant literal.
        uintptr_t kind = MP_PARSE_NODE_LEAF_KIND(pn);
        if (kind == MP_PARSE_NODE_STRING) {
            return true;
        } else if (kind == MP_PARSE_NODE_TOKEN) {
            uintptr_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
            return arg == MP_TOKEN_KW_NONE
                   || arg == MP_TOKEN_KW_FALSE
                   || arg == MP_TOKEN_KW_TRUE
                   || arg == MP_TOKEN_ELLIPSIS;
        }
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, RULE_const_object)) {
        // Constant object.
        return true;
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, RULE_atom_paren)) {
        // Possible empty tuple.
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
        return MP_PARSE_NODE_IS_NULL(pns->nodes[0]);
    }
    return false;
}

static mp_obj_t mp_parse_node_convert_to_obj(mp_parse_node_t pn) {
    assert(mp_parse_node_is_const(pn));
    if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        mp_int_t arg = MP_PARSE_NODE_LEAF_SMALL_INT(pn);
        #if MICROPY_DYNAMIC_COMPILER
        mp_uint_t sign_mask = -((mp_uint_t)1 << (mp_dynamic_compiler.small_int_bits - 1));
        if (!((arg & sign_mask) == 0 || (arg & sign_mask) == sign_mask)) {
            // Integer doesn't fit in a small-int, so create a multi-precision int object.
            return mp_obj_new_int_from_ll(arg);
        }
        #endif
        return MP_OBJ_NEW_SMALL_INT(arg);
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        uintptr_t kind = MP_PARSE_NODE_LEAF_KIND(pn);
        uintptr_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
        if (kind == MP_PARSE_NODE_STRING) {
            return MP_OBJ_NEW_QSTR(arg);
        } else {
            assert(MP_PARSE_NODE_LEAF_KIND(pn) == MP_PARSE_NODE_TOKEN);
            switch (arg) {
                case MP_TOKEN_KW_NONE:
                    return mp_const_none;
                case MP_TOKEN_KW_FALSE:
                    return mp_const_false;
                case MP_TOKEN_KW_TRUE:
                    return mp_const_true;
                default:
                    assert(arg == MP_TOKEN_ELLIPSIS);
                    return MP_OBJ_FROM_PTR(&mp_const_ellipsis_obj);
            }
        }
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, RULE_const_object)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
        return mp_parse_node_extract_const_object(pns);
    } else {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn, RULE_atom_paren));
        assert(MP_PARSE_NODE_IS_NULL(((mp_parse_node_struct_t *)pn)->nodes[0]));
        return mp_const_empty_tuple;
    }
}
#endif

static bool parse_node_is_const_bool(mp_parse_node_t pn, bool value) {
    // Returns true if 'pn' is a constant whose boolean value is equivalent to 'value'
    #if MICROPY_COMP_CONST_TUPLE || MICROPY_COMP_CONST
    return mp_parse_node_is_const(pn) && mp_obj_is_true(mp_parse_node_convert_to_obj(pn)) == value;
    #else
    return MP_PARSE_NODE_IS_TOKEN_KIND(pn, value ? MP_TOKEN_KW_TRUE : MP_TOKEN_KW_FALSE)
           || (MP_PARSE_NODE_IS_SMALL_INT(pn) && !!MP_PARSE_NODE_LEAF_SMALL_INT(pn) == value);
    #endif
}

bool mp_parse_node_is_const_false(mp_parse_node_t pn) {
    return parse_node_is_const_bool(pn, false);
}

bool mp_parse_node_is_const_true(mp_parse_node_t pn) {
    return parse_node_is_const_bool(pn, true);
}

size_t mp_parse_node_extract_list(mp_parse_node_t *pn, size_t pn_kind, mp_parse_node_t **nodes) {
    if (MP_PARSE_NODE_IS_NULL(*pn)) {
        *nodes = NULL;
        return 0;
    } else if (MP_PARSE_NODE_IS_LEAF(*pn)) {
        *nodes = pn;
        return 1;
    } else {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)(*pn);
        if (MP_PARSE_NODE_STRUCT_KIND(pns) != pn_kind) {
            *nodes = pn;
            return 1;
        } else {
            *nodes = pns->nodes;
            return MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        }
    }
}

#if MICROPY_DEBUG_PRINTERS
void mp_parse_node_print(const mp_print_t *print, mp_parse_node_t pn, size_t indent) {
    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_printf(print, "[% 4d] ", (int)((mp_parse_node_struct_t *)pn)->source_line);
    } else {
        mp_printf(print, "       ");
    }
    for (size_t i = 0; i < indent; i++) {
        mp_printf(print, " ");
    }
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        mp_printf(print, "NULL\n");
    } else if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        mp_int_t arg = MP_PARSE_NODE_LEAF_SMALL_INT(pn);
        mp_printf(print, "int(" INT_FMT ")\n", arg);
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        uintptr_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
        switch (MP_PARSE_NODE_LEAF_KIND(pn)) {
            case MP_PARSE_NODE_ID:
                mp_printf(print, "id(%s)\n", qstr_str(arg));
                break;
            case MP_PARSE_NODE_STRING:
                mp_printf(print, "str(%s)\n", qstr_str(arg));
                break;
            default:
                assert(MP_PARSE_NODE_LEAF_KIND(pn) == MP_PARSE_NODE_TOKEN);
                mp_printf(print, "tok(%u)\n", (uint)arg);
                break;
        }
    } else {
        // node must be a mp_parse_node_struct_t
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_const_object) {
            mp_obj_t obj = mp_parse_node_extract_const_object(pns);
            #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
            mp_printf(print, "literal const(%016llx)=", obj);
            #else
            mp_printf(print, "literal const(%p)=", obj);
            #endif
            mp_obj_print_helper(print, obj, PRINT_REPR);
            mp_printf(print, "\n");
        } else {
            size_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
            #if MICROPY_DEBUG_PARSE_RULE_NAME
            mp_printf(print, "%s(%u) (n=%u)\n", rule_name_table[MP_PARSE_NODE_STRUCT_KIND(pns)], (uint)MP_PARSE_NODE_STRUCT_KIND(pns), (uint)n);
            #else
            mp_printf(print, "rule(%u) (n=%u)\n", (uint)MP_PARSE_NODE_STRUCT_KIND(pns), (uint)n);
            #endif
            for (size_t i = 0; i < n; i++) {
                mp_parse_node_print(print, pns->nodes[i], indent + 2);
            }
        }
    }
}
#endif // MICROPY_DEBUG_PRINTERS

/*
static void result_stack_show(const mp_print_t *print, parser_t *parser) {
    mp_printf(print, "result stack, most recent first\n");
    for (ssize_t i = parser->result_stack_top - 1; i >= 0; i--) {
        mp_parse_node_print(print, parser->result_stack[i], 0);
    }
}
*/

static mp_parse_node_t pop_result(parser_t *parser) {
    assert(parser->result_stack_top > 0);
    return parser->result_stack[--parser->result_stack_top];
}

static mp_parse_node_t peek_result(parser_t *parser, size_t pos) {
    assert(parser->result_stack_top > pos);
    return parser->result_stack[parser->result_stack_top - 1 - pos];
}

static void push_result_node(parser_t *parser, mp_parse_node_t pn) {
    if (parser->result_stack_top >= parser->result_stack_alloc) {
        mp_parse_node_t *stack = m_renew(mp_parse_node_t, parser->result_stack, parser->result_stack_alloc, parser->result_stack_alloc + MICROPY_ALLOC_PARSE_RESULT_INC);
        parser->result_stack = stack;
        parser->result_stack_alloc += MICROPY_ALLOC_PARSE_RESULT_INC;
    }
    parser->result_stack[parser->result_stack_top++] = pn;
}

static mp_parse_node_t make_node_const_object(parser_t *parser, size_t src_line, mp_obj_t obj) {
    mp_parse_node_struct_t *pn = parser_alloc(parser, sizeof(mp_parse_node_struct_t) + sizeof(mp_obj_t));
    pn->source_line = src_line;
    #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
    // nodes are 32-bit pointers, but need to store 64-bit object
    pn->kind_num_nodes = RULE_const_object | (2 << 8);
    pn->nodes[0] = (uint64_t)obj;
    pn->nodes[1] = (uint64_t)obj >> 32;
    #else
    pn->kind_num_nodes = RULE_const_object | (1 << 8);
    pn->nodes[0] = (uintptr_t)obj;
    #endif
    return (mp_parse_node_t)pn;
}

// Create a parse node representing a constant object, possibly optimising the case of
// an integer, by putting the (small) integer value directly in the parse node itself.
static mp_parse_node_t make_node_const_object_optimised(parser_t *parser, size_t src_line, mp_obj_t obj) {
    if (mp_obj_is_small_int(obj)) {
        mp_int_t val = MP_OBJ_SMALL_INT_VALUE(obj);
        #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
        // A parse node is only 32-bits and the small-int value must fit in 31-bits
        if (((val ^ (val << 1)) & 0xffffffff80000000) != 0) {
            return make_node_const_object(parser, src_line, obj);
        }
        #endif
        #if MICROPY_DYNAMIC_COMPILER
        // Check that the integer value fits in target runtime's small-int
        mp_uint_t sign_mask = -((mp_uint_t)1 << (mp_dynamic_compiler.small_int_bits - 1));
        if (!((val & sign_mask) == 0 || (val & sign_mask) == sign_mask)) {
            return make_node_const_object(parser, src_line, obj);
        }
        #endif
        return mp_parse_node_new_small_int(val);
    } else {
        return make_node_const_object(parser, src_line, obj);
    }
}

static void push_result_token(parser_t *parser, uint8_t rule_id) {
    mp_parse_node_t pn;
    mp_lexer_t *lex = parser->lexer;
    if (lex->tok_kind == MP_TOKEN_NAME) {
        qstr id = qstr_from_strn(lex->vstr.buf, lex->vstr.len);
        #if MICROPY_COMP_CONST
        // if name is a standalone identifier, look it up in the table of dynamic constants
        mp_map_elem_t *elem;
        if (rule_id == RULE_atom
            && (elem = mp_map_lookup(&parser->consts, MP_OBJ_NEW_QSTR(id), MP_MAP_LOOKUP)) != NULL) {
            pn = make_node_const_object_optimised(parser, lex->tok_line, elem->value);
        } else {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_ID, id);
        }
        #else
        (void)rule_id;
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_ID, id);
        #endif
    } else if (lex->tok_kind == MP_TOKEN_INTEGER) {
        mp_obj_t o = mp_parse_num_integer(lex->vstr.buf, lex->vstr.len, 0, lex);
        pn = make_node_const_object_optimised(parser, lex->tok_line, o);
    } else if (lex->tok_kind == MP_TOKEN_FLOAT_OR_IMAG) {
        mp_obj_t o = mp_parse_num_float(lex->vstr.buf, lex->vstr.len, true, lex);
        pn = make_node_const_object(parser, lex->tok_line, o);
    } else if (lex->tok_kind == MP_TOKEN_STRING) {
        // Don't automatically intern all strings.  Doc strings (which are usually large)
        // will be discarded by the compiler, and so we shouldn't intern them.
        qstr qst = MP_QSTRnull;
        if (lex->vstr.len <= MICROPY_ALLOC_PARSE_INTERN_STRING_LEN) {
            // intern short strings
            qst = qstr_from_strn(lex->vstr.buf, lex->vstr.len);
        } else {
            // check if this string is already interned
            qst = qstr_find_strn(lex->vstr.buf, lex->vstr.len);
        }
        if (qst != MP_QSTRnull) {
            // qstr exists, make a leaf node
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, qst);
        } else {
            // not interned, make a node holding a pointer to the string object
            mp_obj_t o = mp_obj_new_str_copy(&mp_type_str, (const byte *)lex->vstr.buf, lex->vstr.len);
            pn = make_node_const_object(parser, lex->tok_line, o);
        }
    } else if (lex->tok_kind == MP_TOKEN_BYTES) {
        // make a node holding a pointer to the bytes object
        mp_obj_t o = mp_obj_new_bytes((const byte *)lex->vstr.buf, lex->vstr.len);
        pn = make_node_const_object(parser, lex->tok_line, o);
    }
    #if MICROPY_PY_TSTRINGS
    else if (lex->tok_kind == MP_TOKEN_TSTRING || lex->tok_kind == MP_TOKEN_TSTRING_RAW) {
        // Generate AST for template string construction
        // This will create code that calls __template__(strings, interpolations)

        const byte *str = (const byte *)lex->vstr.buf;
        size_t len = lex->vstr.len;

        // Process TSTRING content
        // Use parser_alloc for all allocations to ensure GC safety

        // Allocate initial arrays using parser memory
        mp_parse_node_t *strings = NULL;
        mp_parse_node_t *interps = NULL;
        size_t strings_len = 0;
        size_t interps_len = 0;
        size_t strings_alloc = 0;
        size_t interps_alloc = 0;

        // Helper macros for dynamic array growth
        #define ADD_NODE_STRINGS(node) do { \
        if (strings_len >= strings_alloc) { \
            size_t new_alloc = strings_alloc ? strings_alloc * 2 : 4; \
            if (new_alloc > 0xFFF) { \
                goto tstring_overflow; \
            } \
            mp_parse_node_t *new_items = parser_alloc(parser, new_alloc * sizeof(mp_parse_node_t)); \
            if (strings) { \
                memcpy(new_items, strings, strings_len * sizeof(mp_parse_node_t)); \
            } \
            strings = new_items; \
            strings_alloc = new_alloc; \
        } \
        strings[strings_len++] = node; \
} \
    while (0)

        #define ADD_NODE_INTERPS(node) do { \
        if (interps_len >= interps_alloc) { \
            size_t new_alloc = interps_alloc ? interps_alloc * 2 : 4; \
            if (new_alloc > 0xFFF) { \
                goto tstring_overflow; \
            } \
            mp_parse_node_t *new_items = parser_alloc(parser, new_alloc * sizeof(mp_parse_node_t)); \
            if (interps) { \
                memcpy(new_items, interps, interps_len * sizeof(mp_parse_node_t)); \
            } \
            interps = new_items; \
            interps_alloc = new_alloc; \
        } \
        interps[interps_len++] = node; \
} while (0)

        // Use a vstr to accumulate the current string part
        vstr_t vstr;
        vstr_init(&vstr, 16);

        size_t i = 0;
        while (i < len) {
            if (i < len - 1 && str[i] == '{' && str[i + 1] == '{') {
                // Handle escaped braces {{
                vstr_add_byte(&vstr, '{');
                i += 2;
            } else if (i < len - 1 && str[i] == '}' && str[i + 1] == '}') {
                // Handle escaped braces }}
                vstr_add_byte(&vstr, '}');
                i += 2;
            } else if (str[i] == '}') {
                // Lone } without matching }} - syntax error
                vstr_clear(&vstr);
                mp_raise_msg(&mp_type_SyntaxError, MP_ERROR_TEXT("t-string: single '}' is not allowed"));
            } else if (str[i] == '{') {
                // Find the end of the interpolation first to check for debug format
                i++;
                size_t expr_start = i;
                int brace_depth = 1;
                size_t conversion_pos = 0;
                size_t format_spec_pos = 0;
                bool is_debug_format = false;

                // Quick scan to check for debug format
                size_t j = i;
                int bd = 0, pd = 0;     // bracket and paren depth
                char in_str = 0;     // Track string delimiter

                while (j < len) {
                    if (in_str) {
                        if (str[j] == in_str) {
                            in_str = 0;
                        }
                    } else {
                        if (str[j] == '"' || str[j] == '\'') {
                            in_str = str[j];
                        } else if (str[j] == '}') {
                            break;
                        } else if (str[j] == '[') {
                            bd++;
                        } else if (str[j] == ']') {
                            bd--;
                        } else if (str[j] == '(') {
                            pd++;
                        } else if (str[j] == ')') {
                            pd--;
                        } else if (bd == 0 && pd == 0 && (str[j] == '!' || str[j] == ':')) {
                            break;
                        }
                    }
                    j++;
                }
                // Check for debug format (expr=)
                // Only check if we found a valid terminator (not EOF)
                if (j < len) {
                    size_t check_pos = j - 1;
                    // Skip trailing whitespace to find '='
                    while (check_pos > i && check_pos < len &&
                           (str[check_pos] == ' ' || str[check_pos] == '\t')) {
                        check_pos--;
                    }
                    // Check if it's a debug format
                    if (check_pos > i && check_pos < len && str[check_pos] == '=' &&
                        (str[j] == '}' || str[j] == '!' || str[j] == ':')) {
                        is_debug_format = true;

                        // For debug format, add "expr=" with whitespace to the current string
                        for (size_t k = expr_start; k < j && k < len; k++) {
                            vstr_add_byte(&vstr, str[k]);
                        }
                    }
                }

                int bracket_depth = 0;     // Track [] nesting
                int paren_depth = 0;     // Track () nesting
                char in_string = 0;     // Track string delimiter (0, '"', or '\'')

                while (i < len && brace_depth > 0) {
                    if (in_string) {
                        if (str[i] == in_string) {
                            in_string = 0;
                        }
                    } else {
                        if (str[i] == '"' || str[i] == '\'') {
                            in_string = str[i];
                        } else if (str[i] == '{') {
                            brace_depth++;
                        } else if (str[i] == '}') {
                            brace_depth--;
                            if (brace_depth == 0) {
                                break;
                            }
                        } else if (str[i] == '[') {
                            bracket_depth++;
                        } else if (str[i] == ']') {
                            if (bracket_depth > 0) {
                                bracket_depth--;
                            }
                        } else if (str[i] == '(') {
                            paren_depth++;
                        } else if (str[i] == ')') {
                            if (paren_depth > 0) {
                                paren_depth--;
                            }
                        } else if (brace_depth == 1 && bracket_depth == 0 && paren_depth == 0 && str[i] == '!' && conversion_pos == 0 && format_spec_pos == 0) {
                            conversion_pos = i;
                        } else if (brace_depth == 1 && bracket_depth == 0 && paren_depth == 0 && str[i] == ':' && format_spec_pos == 0) {
                            format_spec_pos = i;
                        }
                    }
                    i++;
                }

                if (brace_depth == 0) {
                    // Successfully found closing brace
                    size_t expr_end = i;
                    size_t expr_len = expr_end - expr_start;

                    // Commit literal prefix (including any debug prefix) now that interpolation parsed.
                    mp_obj_t str_obj = mp_obj_new_str_copy(&mp_type_str, (const byte *)vstr.buf, vstr.len);
                    ADD_NODE_STRINGS(make_node_const_object(parser, lex->tok_line, str_obj));
                    vstr_reset(&vstr);

                    if (is_debug_format) {
                        // Find the position of '=' to determine expression length
                        size_t eq_pos = expr_start;
                        while (eq_pos < expr_end && str[eq_pos] != '=') {
                            eq_pos++;
                        }
                        // Trim trailing whitespace before '='
                        while (eq_pos > expr_start &&
                               (str[eq_pos - 1] == ' ' || str[eq_pos - 1] == '\t' ||
                                str[eq_pos - 1] == '\n' || str[eq_pos - 1] == '\r')) {
                            eq_pos--;
                        }
                        expr_len = eq_pos - expr_start;
                    } else if (conversion_pos) {
                        expr_len = conversion_pos - expr_start;
                    } else if (format_spec_pos) {
                        expr_len = format_spec_pos - expr_start;
                    }

                    // Parse the expression to generate proper AST nodes
                    mp_parse_node_t expr_node;
                    if (expr_len > 0) {
                        #if MICROPY_PY_TSTRINGS
                        // Use the dedicated expression parser
                        expr_node = parse_tstring_expression(parser, tstring_parser_alloc_wrapper, (const char *)&str[expr_start], expr_len);
                        #else
                        // Fallback: store as string
                        qstr expr_q = qstr_from_strn((const char *)&str[expr_start], expr_len);
                        expr_node = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, expr_q);
                        #endif
                    } else {
                        // Empty expression is not allowed in template strings
                        goto tstring_empty_expr;
                    }

                    // Create expression text string
                    mp_parse_node_t expr_text_node;
                    if (expr_len <= MICROPY_ALLOC_PARSE_INTERN_STRING_LEN) {
                        // intern short strings
                        qstr expr_text_q = qstr_from_strn((const char *)&str[expr_start], expr_len);
                        expr_text_node = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, expr_text_q);
                    } else {
                        // check if this string is already interned
                        qstr expr_text_q = qstr_find_strn((const char *)&str[expr_start], expr_len);
                        if (expr_text_q != MP_QSTRnull) {
                            expr_text_node = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, expr_text_q);
                        } else {
                            // not interned, make a node holding a pointer to the string object
                            mp_obj_t o = mp_obj_new_str_copy(&mp_type_str, (const byte *)&str[expr_start], expr_len);
                            expr_text_node = make_node_const_object(parser, lex->tok_line, o);
                        }
                    }

                    // Extract conversion
                    mp_parse_node_t conversion_node = mp_parse_node_new_leaf(MP_PARSE_NODE_TOKEN, MP_TOKEN_KW_NONE);

                    if (conversion_pos) {
                        if (conversion_pos + 1 >= expr_end) {
                            vstr_clear(&vstr);
                            mp_raise_msg(&mp_type_SyntaxError, MP_ERROR_TEXT("t-string: missing conversion character"));
                        }
                        char conv_char = str[conversion_pos + 1];
                        if (conv_char != 's' && conv_char != 'r' && conv_char != 'a') {
                            vstr_clear(&vstr);
                            mp_raise_msg_varg(&mp_type_SyntaxError,
                                MP_ERROR_TEXT("t-string: invalid conversion character '%c': expected 's', 'r', or 'a'"),
                                conv_char);
                        }
                        size_t check_start = conversion_pos + 2;
                        size_t check_end = format_spec_pos ? format_spec_pos : expr_end;
                        for (size_t k = check_start; k < check_end; k++) {
                            char c = str[k];
                            if (c == '\x0b') {
                                vstr_clear(&vstr);
                                mp_raise_msg_varg(&mp_type_SyntaxError,
                                    MP_ERROR_TEXT("invalid non-printable character U+000B (%s, line %d)"),
                                    qstr_str(lex->source_name), lex->tok_line);
                            }
                            if (c != ' ' && c != '\t' && c != '\n' && c != '\r' && c != '\f') {
                                vstr_clear(&vstr);
                                mp_raise_msg_varg(&mp_type_SyntaxError,
                                    MP_ERROR_TEXT("t-string: invalid conversion character '%.*s': expected 's', 'r', or 'a'"),
                                    (int)(check_end - conversion_pos - 1), (const char *)&str[conversion_pos + 1]);
                            }
                        }
                        qstr conv_q = qstr_from_strn((const char *)&str[conversion_pos + 1], 1);
                        conversion_node = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, conv_q);
                    } else if (is_debug_format) {
                        if (format_spec_pos) {
                            conversion_node = mp_parse_node_new_leaf(MP_PARSE_NODE_TOKEN, MP_TOKEN_KW_NONE);
                        } else {
                            qstr conv_q = MP_QSTR_r;
                            conversion_node = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, conv_q);
                        }
                    }

                    // Extract format spec
                    mp_parse_node_t format_spec_node;
                    if (format_spec_pos && format_spec_pos + 1 < expr_end) {
                        size_t fmt_end = expr_end;
                        if (fmt_end > format_spec_pos + 1) {
                            const byte *fmt_start = &str[format_spec_pos + 1];
                            size_t fmt_len = fmt_end - format_spec_pos - 1;

                            #if MICROPY_PY_FSTRINGS
                            // Check if format spec contains expressions (has '{')
                            bool has_expr = false;
                            for (size_t k = 0; k < fmt_len; k++) {
                                byte c = fmt_start[k];
                                if (c == '{' && (k + 1 >= fmt_len || fmt_start[k + 1] != '{')) {
                                    has_expr = true;
                                    break;
                                }
                            }

                            if (has_expr) {
                                // Choose f-string wrapper to avoid collision with format spec content
                                bool has_triple_double = false;
                                bool has_triple_single = false;
                                for (size_t k = 0; k + 2 < fmt_len; k++) {
                                    if (fmt_start[k] == '"' && fmt_start[k + 1] == '"' && fmt_start[k + 2] == '"') {
                                        has_triple_double = true;
                                    }
                                    if (fmt_start[k] == '\'' && fmt_start[k + 1] == '\'' && fmt_start[k + 2] == '\'') {
                                        has_triple_single = true;
                                    }
                                }

                                const char *wrapper_start, *wrapper_end;
                                if (has_triple_double && has_triple_single) {
                                    // Both present: use ''' (''' less common than """)
                                    wrapper_start = "f'''";
                                    wrapper_end = "'''";
                                } else if (has_triple_double) {
                                    wrapper_start = "f'''";
                                    wrapper_end = "'''";
                                } else {
                                    wrapper_start = "f\"\"\"";
                                    wrapper_end = "\"\"\"";
                                }

                                vstr_t fstring_vstr;
                                vstr_init(&fstring_vstr, fmt_len * 2 + 8);
                                vstr_add_str(&fstring_vstr, wrapper_start);

                                // Track string literal type: 0=none, 1=', 2=", 3=''', 4="""
                                int string_type = 0;
                                for (size_t k = 0; k < fmt_len; k++) {
                                    byte c = fmt_start[k];

                                    if (string_type == 0) {
                                        // Outside string literals: check for string start and special chars
                                        if (c == '"' && k + 2 < fmt_len && fmt_start[k + 1] == '"' && fmt_start[k + 2] == '"') {
                                            string_type = 4;
                                            vstr_add_byte(&fstring_vstr, '"');
                                            vstr_add_byte(&fstring_vstr, '"');
                                            vstr_add_byte(&fstring_vstr, '"');
                                            k += 2;
                                        } else if (c == '\'' && k + 2 < fmt_len && fmt_start[k + 1] == '\'' && fmt_start[k + 2] == '\'') {
                                            string_type = 3;
                                            vstr_add_byte(&fstring_vstr, '\'');
                                            vstr_add_byte(&fstring_vstr, '\'');
                                            vstr_add_byte(&fstring_vstr, '\'');
                                            k += 2;
                                        } else if (c == '"') {
                                            string_type = 2;
                                            vstr_add_byte(&fstring_vstr, c);
                                        } else if (c == '\'') {
                                            string_type = 1;
                                            vstr_add_byte(&fstring_vstr, c);
                                        } else if (c == '\\') {
                                            // Escape backslash to prevent escaping wrapper quotes
                                            vstr_add_byte(&fstring_vstr, '\\');
                                            vstr_add_byte(&fstring_vstr, '\\');
                                        } else if (c == '\r') {
                                            // Escape CR to avoid lexer normalization to LF
                                            vstr_add_str(&fstring_vstr, "\\r");
                                        } else {
                                            vstr_add_byte(&fstring_vstr, c);
                                        }
                                    } else {
                                        if (c == '\\') {
                                            // Copy backslash and next char to preserve escape sequences
                                            vstr_add_byte(&fstring_vstr, c);
                                            k++;
                                            if (k < fmt_len) {
                                                vstr_add_byte(&fstring_vstr, fmt_start[k]);
                                            }
                                        } else if (string_type == 4) {
                                            vstr_add_byte(&fstring_vstr, c);
                                            if (c == '"' && k + 2 < fmt_len && fmt_start[k + 1] == '"' && fmt_start[k + 2] == '"') {
                                                vstr_add_byte(&fstring_vstr, '"');
                                                vstr_add_byte(&fstring_vstr, '"');
                                                k += 2;
                                                string_type = 0;
                                            }
                                        } else if (string_type == 3) {
                                            vstr_add_byte(&fstring_vstr, c);
                                            if (c == '\'' && k + 2 < fmt_len && fmt_start[k + 1] == '\'' && fmt_start[k + 2] == '\'') {
                                                vstr_add_byte(&fstring_vstr, '\'');
                                                vstr_add_byte(&fstring_vstr, '\'');
                                                k += 2;
                                                string_type = 0;
                                            }
                                        } else if (string_type == 2) {
                                            vstr_add_byte(&fstring_vstr, c);
                                            if (c == '"') {
                                                string_type = 0;
                                            }
                                        } else if (string_type == 1) {
                                            vstr_add_byte(&fstring_vstr, c);
                                            if (c == '\'') {
                                                string_type = 0;
                                            }
                                        }
                                    }
                                }

                                vstr_add_str(&fstring_vstr, wrapper_end);

                                // Use nlr to ensure cleanup even if parsing/copying fails
                                nlr_buf_t nlr;
                                mp_parse_tree_t fstring_tree;
                                bool tree_needs_cleanup = false;
                                if (nlr_push(&nlr) == 0) {
                                    mp_lexer_t *fstring_lex = mp_lexer_new_from_str_len(
                                        MP_QSTR__lt_format_spec_gt_,
                                        vstr_str(&fstring_vstr),
                                        vstr_len(&fstring_vstr),
                                        0
                                        );

                                    fstring_tree = mp_parse(fstring_lex, MP_PARSE_EVAL_INPUT);
                                    tree_needs_cleanup = true;  // From here on, tree must be cleaned up
                                    mp_parse_node_t fstring_root = fstring_tree.root;

                                    if (MP_PARSE_NODE_IS_STRUCT(fstring_root)) {
                                        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)fstring_root;
                                        if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_eval_input) {
                                            fstring_root = pns->nodes[0];
                                        }
                                    }

                                    format_spec_node = copy_parse_node(parser, tstring_parser_alloc_wrapper, fstring_root);

                                    mp_parse_tree_clear(&fstring_tree);
                                    nlr_pop();
                                    vstr_clear(&fstring_vstr);
                                } else {
                                    // Exception occurred - cleanup allocated resources
                                    if (tree_needs_cleanup) {
                                        mp_parse_tree_clear(&fstring_tree);
                                    }
                                    vstr_clear(&fstring_vstr);
                                    nlr_raise(nlr.ret_val);
                                }
                            } else
                            #endif
                            {
                                qstr fmt_q = qstr_from_strn((const char *)fmt_start, fmt_len);
                                format_spec_node = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, fmt_q);
                            }
                        } else {
                            format_spec_node = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, MP_QSTR_);
                        }
                    } else {
                        format_spec_node = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, MP_QSTR_);
                    }

                    // Build interpolation tuple node: (expr, text, conv, fmt)
                    mp_parse_node_struct_t *testlist_comp = parser_alloc(parser,
                        sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * 4);
                    testlist_comp->source_line = lex->tok_line;
                    testlist_comp->kind_num_nodes = RULE_testlist_comp | (4 << 8);
                    testlist_comp->nodes[0] = expr_node;
                    testlist_comp->nodes[1] = expr_text_node;
                    testlist_comp->nodes[2] = conversion_node;
                    testlist_comp->nodes[3] = format_spec_node;

                    // Wrap in atom_paren to create a runtime tuple
                    mp_parse_node_struct_t *interp_tuple = parser_alloc(parser,
                        sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * 1);
                    interp_tuple->source_line = lex->tok_line;
                    interp_tuple->kind_num_nodes = RULE_atom_paren | (1 << 8);
                    interp_tuple->nodes[0] = (mp_parse_node_t)testlist_comp;

                    ADD_NODE_INTERPS((mp_parse_node_t)interp_tuple);
                    // Added interpolation
                    i++;     // Skip the closing brace
                }
            } else if (false && str[i] == '\\' && i + 1 < len) {
                i++;
                unsigned char c = str[i];
                switch (c) {
                    case 'n':
                        vstr_add_byte(&vstr, '\n');
                        break;
                    case 't':
                        vstr_add_byte(&vstr, '\t');
                        break;
                    case 'r':
                        vstr_add_byte(&vstr, '\r');
                        break;
                    case 'b':
                        vstr_add_byte(&vstr, '\b');
                        break;
                    case 'f':
                        vstr_add_byte(&vstr, '\f');
                        break;
                    case 'v':
                        vstr_add_byte(&vstr, '\v');
                        break;
                    case 'a':
                        vstr_add_byte(&vstr, '\a');
                        break;
                    case '\\':
                        vstr_add_byte(&vstr, '\\');
                        break;
                    case '\'':
                        vstr_add_byte(&vstr, '\'');
                        break;
                    case '"':
                        vstr_add_byte(&vstr, '"');
                        break;
                    case 'x': {
                        if (i + 2 < len && unichar_isxdigit(str[i + 1]) && unichar_isxdigit(str[i + 2])) {
                            int val = (unichar_xdigit_value(str[i + 1]) << 4) | unichar_xdigit_value(str[i + 2]);
                            vstr_add_byte(&vstr, val);
                            i += 2;
                        } else {
                            vstr_add_byte(&vstr, '\\');
                            vstr_add_byte(&vstr, 'x');
                        }
                        break;
                    }
                    case 'u':
                    case 'U': {
                        int digits = (c == 'u') ? 4 : 8;
                        bool valid = i + digits < len;
                        mp_uint_t val = 0;
                        if (valid) {
                            for (int j = 0; j < digits; j++) {
                                char h = str[i + 1 + j];
                                if (!unichar_isxdigit(h)) {
                                    valid = false;
                                    break;
                                }
                                val = val * 16 + unichar_xdigit_value(h);
                            }
                        }
                        if (valid) {
                            if (val < 0x80) {
                                vstr_add_byte(&vstr, val);
                            } else if (val < 0x800) {
                                vstr_add_byte(&vstr, 0xC0 | (val >> 6));
                                vstr_add_byte(&vstr, 0x80 | (val & 0x3F));
                            } else if (val < 0x10000) {
                                vstr_add_byte(&vstr, 0xE0 | (val >> 12));
                                vstr_add_byte(&vstr, 0x80 | ((val >> 6) & 0x3F));
                                vstr_add_byte(&vstr, 0x80 | (val & 0x3F));
                            } else if (val < 0x110000) {
                                vstr_add_byte(&vstr, 0xF0 | (val >> 18));
                                vstr_add_byte(&vstr, 0x80 | ((val >> 12) & 0x3F));
                                vstr_add_byte(&vstr, 0x80 | ((val >> 6) & 0x3F));
                                vstr_add_byte(&vstr, 0x80 | (val & 0x3F));
                            } else {
                                valid = false;
                            }
                            if (valid) {
                                i += digits;
                            }
                        }
                        if (!valid) {
                            vstr_add_byte(&vstr, '\\');
                            vstr_add_byte(&vstr, c);
                        }
                        break;
                    }
                    default:
                        if (c >= '0' && c <= '7') {
                            int val = c - '0';
                            int digits = 1;
                            while (digits < 3 && i + 1 < len && str[i + 1] >= '0' && str[i + 1] <= '7') {
                                i++;
                                val = val * 8 + (str[i] - '0');
                                digits++;
                            }
                            vstr_add_byte(&vstr, val);
                        } else {
                            vstr_add_byte(&vstr, '\\');
                            vstr_add_byte(&vstr, c);
                        }
                        break;
                }
                i++;
            } else {
                vstr_add_byte(&vstr, str[i]);
                i++;
            }
        }

        mp_obj_t str_obj = mp_obj_new_str_copy(&mp_type_str, (const byte *)vstr.buf, vstr.len);
        ADD_NODE_STRINGS(make_node_const_object(parser, lex->tok_line, str_obj));


        size_t seg_cnt = strings_len;
        size_t interp_cnt = interps_len;

        // Counts are limited by ADD_NODE_* doubling logic, which aborts before exceeding 0xFFF.
        assert(seg_cnt <= 0xFFF && interp_cnt <= 0xFFF);

        // Integer overflow is impossible here since both values are <= 0xFFF
        size_t total = seg_cnt + interp_cnt;

        // Allocate template node. GC safety: We copy the array contents
        // immediately after allocation to avoid issues if parser_alloc triggers GC.
        mp_parse_node_struct_t *templ =
            parser_alloc(parser,
                sizeof(*templ) + sizeof(mp_parse_node_t) * total);

        templ->source_line = lex->tok_line;
        templ->kind_num_nodes = MP_PARSE_TSTR_HDR_MAKE(seg_cnt, interp_cnt);

        // Copy nodes - safe because both arrays are in parser memory
        if (seg_cnt > 0) {
            assert(strings != NULL);
            memcpy(&templ->nodes[0], strings,
                seg_cnt * sizeof(mp_parse_node_t));
        }
        if (interp_cnt > 0) {
            assert(interps != NULL);
            memcpy(&templ->nodes[seg_cnt], interps,
                interp_cnt * sizeof(mp_parse_node_t));
        }

        pn = (mp_parse_node_t)templ;

        // Cleanup vstr - parser memory is automatically managed
        vstr_clear(&vstr);

#undef ADD_NODE_STRINGS
#undef ADD_NODE_INTERPS

        // Jump here from ADD_NODE_* macros on overflow
        if (0) {
        tstring_overflow:
            vstr_clear(&vstr);
            mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("template string too large for header format"));
        tstring_empty_expr:
            vstr_clear(&vstr);
            mp_raise_msg(&mp_type_SyntaxError, MP_ERROR_TEXT("t-string: valid expression required before '}'"));
        }
    }
    #endif
    else {
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_TOKEN, lex->tok_kind);
    }
    push_result_node(parser, pn);
}

#if MICROPY_COMP_CONST_FOLDING

#if MICROPY_COMP_MODULE_CONST
static const mp_rom_map_elem_t mp_constants_table[] = {
    #if MICROPY_PY_ERRNO
    { MP_ROM_QSTR(MP_QSTR_errno), MP_ROM_PTR(&mp_module_errno) },
    #endif
    #if MICROPY_PY_UCTYPES
    { MP_ROM_QSTR(MP_QSTR_uctypes), MP_ROM_PTR(&mp_module_uctypes) },
    #endif
    #if MICROPY_PY_BUILTINS_FLOAT && MICROPY_PY_MATH && MICROPY_COMP_CONST_FLOAT
    { MP_ROM_QSTR(MP_QSTR_math), MP_ROM_PTR(&mp_module_math) },
    #endif
    // Extra constants as defined by a port
    MICROPY_PORT_CONSTANTS
};
static MP_DEFINE_CONST_MAP(mp_constants_map, mp_constants_table);
#endif

static bool binary_op_maybe(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs, mp_obj_t *res) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t tmp = mp_binary_op(op, lhs, rhs);
        nlr_pop();
        #if MICROPY_PY_BUILTINS_COMPLEX
        if (mp_obj_is_type(tmp, &mp_type_complex)) {
            return false;
        }
        #endif
        *res = tmp;
        return true;
    } else {
        return false;
    }
}

static bool fold_logical_constants(parser_t *parser, uint8_t rule_id, size_t *num_args) {
    if (rule_id == RULE_or_test
        || rule_id == RULE_and_test) {
        // folding for binary logical ops: or and
        size_t copy_to = *num_args;
        for (size_t i = copy_to; i > 0;) {
            mp_parse_node_t pn = peek_result(parser, --i);
            parser->result_stack[parser->result_stack_top - copy_to] = pn;
            if (i == 0) {
                // always need to keep the last value
                break;
            }
            if (rule_id == RULE_or_test) {
                if (mp_parse_node_is_const_true(pn)) {
                    //
                    break;
                } else if (!mp_parse_node_is_const_false(pn)) {
                    copy_to -= 1;
                }
            } else {
                // RULE_and_test
                if (mp_parse_node_is_const_false(pn)) {
                    break;
                } else if (!mp_parse_node_is_const_true(pn)) {
                    copy_to -= 1;
                }
            }
        }
        copy_to -= 1; // copy_to now contains number of args to pop

        // pop and discard all the short-circuited expressions
        for (size_t i = 0; i < copy_to; ++i) {
            pop_result(parser);
        }
        *num_args -= copy_to;

        // we did a complete folding if there's only 1 arg left
        return *num_args == 1;

    } else if (rule_id == RULE_not_test_2) {
        // folding for unary logical op: not
        mp_parse_node_t pn = peek_result(parser, 0);
        if (mp_parse_node_is_const_false(pn)) {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_TOKEN, MP_TOKEN_KW_TRUE);
        } else if (mp_parse_node_is_const_true(pn)) {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_TOKEN, MP_TOKEN_KW_FALSE);
        } else {
            return false;
        }
        pop_result(parser);
        push_result_node(parser, pn);
        return true;
    }

    return false;
}

static bool fold_constants(parser_t *parser, uint8_t rule_id, size_t num_args) {
    // this code does folding of arbitrary numeric expressions, eg 1 + 2 * 3 + 4
    // it does not do partial folding, eg 1 + 2 + x -> 3 + x

    mp_obj_t arg0;
    if (rule_id == RULE_expr
        || rule_id == RULE_xor_expr
        || rule_id == RULE_and_expr
        || rule_id == RULE_power) {
        // folding for binary ops: | ^ & **
        mp_parse_node_t pn = peek_result(parser, num_args - 1);
        if (!mp_parse_node_get_number_maybe(pn, &arg0)) {
            return false;
        }
        mp_binary_op_t op;
        if (rule_id == RULE_expr) {
            op = MP_BINARY_OP_OR;
        } else if (rule_id == RULE_xor_expr) {
            op = MP_BINARY_OP_XOR;
        } else if (rule_id == RULE_and_expr) {
            op = MP_BINARY_OP_AND;
        } else {
            op = MP_BINARY_OP_POWER;
        }
        for (ssize_t i = num_args - 2; i >= 0; --i) {
            pn = peek_result(parser, i);
            mp_obj_t arg1;
            if (!mp_parse_node_get_number_maybe(pn, &arg1)) {
                return false;
            }
            if (!binary_op_maybe(op, arg0, arg1, &arg0)) {
                return false;
            }
        }
    } else if (rule_id == RULE_shift_expr
               || rule_id == RULE_arith_expr
               || rule_id == RULE_term) {
        // folding for binary ops: << >> + - * @ / % //
        mp_parse_node_t pn = peek_result(parser, num_args - 1);
        if (!mp_parse_node_get_number_maybe(pn, &arg0)) {
            return false;
        }
        for (ssize_t i = num_args - 2; i >= 1; i -= 2) {
            pn = peek_result(parser, i - 1);
            mp_obj_t arg1;
            if (!mp_parse_node_get_number_maybe(pn, &arg1)) {
                return false;
            }
            mp_token_kind_t tok = MP_PARSE_NODE_LEAF_ARG(peek_result(parser, i));
            mp_binary_op_t op = MP_BINARY_OP_LSHIFT + (tok - MP_TOKEN_OP_DBL_LESS);
            if (!binary_op_maybe(op, arg0, arg1, &arg0)) {
                return false;
            }
        }
    } else if (rule_id == RULE_factor_2) {
        // folding for unary ops: + - ~
        mp_parse_node_t pn = peek_result(parser, 0);
        if (!mp_parse_node_get_number_maybe(pn, &arg0)) {
            return false;
        }
        mp_token_kind_t tok = MP_PARSE_NODE_LEAF_ARG(peek_result(parser, 1));
        mp_unary_op_t op;
        if (tok == MP_TOKEN_OP_TILDE) {
            if (!mp_obj_is_int(arg0)) {
                return false;
            }
            op = MP_UNARY_OP_INVERT;
        } else {
            assert(tok == MP_TOKEN_OP_PLUS || tok == MP_TOKEN_OP_MINUS); // should be
            op = MP_UNARY_OP_POSITIVE + (tok - MP_TOKEN_OP_PLUS);
        }
        arg0 = mp_unary_op(op, arg0);

    #if MICROPY_COMP_CONST
    } else if (rule_id == RULE_expr_stmt) {
        mp_parse_node_t pn1 = peek_result(parser, 0);
        if (!MP_PARSE_NODE_IS_NULL(pn1)
            && !(MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_expr_stmt_augassign)
                 || MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_expr_stmt_assign_list))) {
            // this node is of the form <x> = <y>
            mp_parse_node_t pn0 = peek_result(parser, 1);
            if (MP_PARSE_NODE_IS_ID(pn0)
                && MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_atom_expr_normal)
                && MP_PARSE_NODE_IS_ID(((mp_parse_node_struct_t *)pn1)->nodes[0])
                && MP_PARSE_NODE_LEAF_ARG(((mp_parse_node_struct_t *)pn1)->nodes[0]) == MP_QSTR_const
                && MP_PARSE_NODE_IS_STRUCT_KIND(((mp_parse_node_struct_t *)pn1)->nodes[1], RULE_trailer_paren)
                ) {
                // code to assign dynamic constants: id = const(value)

                // get the id
                qstr id = MP_PARSE_NODE_LEAF_ARG(pn0);

                // get the value
                mp_parse_node_t pn_value = ((mp_parse_node_struct_t *)((mp_parse_node_struct_t *)pn1)->nodes[1])->nodes[0];
                if (!mp_parse_node_is_const(pn_value)) {
                    mp_obj_t exc = mp_obj_new_exception_msg(&mp_type_SyntaxError,
                        MP_ERROR_TEXT("not a constant"));
                    mp_obj_exception_add_traceback(exc, parser->lexer->source_name,
                        ((mp_parse_node_struct_t *)pn1)->source_line, MP_QSTRnull);
                    nlr_raise(exc);
                }
                mp_obj_t value = mp_parse_node_convert_to_obj(pn_value);

                // store the value in the table of dynamic constants
                mp_map_elem_t *elem = mp_map_lookup(&parser->consts, MP_OBJ_NEW_QSTR(id), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
                assert(elem->value == MP_OBJ_NULL);
                elem->value = value;

                // If the constant starts with an underscore then treat it as a private
                // variable and don't emit any code to store the value to the id.
                if (qstr_str(id)[0] == '_') {
                    pop_result(parser); // pop const(value)
                    pop_result(parser); // pop id
                    push_result_rule(parser, 0, RULE_pass_stmt, 0); // replace with "pass"
                    return true;
                }

                // replace const(value) with value
                pop_result(parser);
                push_result_node(parser, pn_value);

                // finished folding this assignment, but we still want it to be part of the tree
                return false;
            }
        }
        return false;
    #endif

    #if MICROPY_COMP_MODULE_CONST
    } else if (rule_id == RULE_atom_expr_normal) {
        mp_parse_node_t pn0 = peek_result(parser, 1);
        mp_parse_node_t pn1 = peek_result(parser, 0);
        if (!(MP_PARSE_NODE_IS_ID(pn0)
              && MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_trailer_period))) {
            return false;
        }
        // id1.id2
        // look it up in constant table, see if it can be replaced with an integer or a float
        mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t *)pn1;
        assert(MP_PARSE_NODE_IS_ID(pns1->nodes[0]));
        qstr q_base = MP_PARSE_NODE_LEAF_ARG(pn0);
        qstr q_attr = MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]);
        mp_map_elem_t *elem = mp_map_lookup((mp_map_t *)&mp_constants_map, MP_OBJ_NEW_QSTR(q_base), MP_MAP_LOOKUP);
        if (elem == NULL) {
            return false;
        }
        mp_obj_t dest[2];
        mp_load_method_maybe(elem->value, q_attr, dest);
        if (!(dest[0] != MP_OBJ_NULL && (mp_obj_is_int(dest[0]) || mp_obj_is_float(dest[0])) && dest[1] == MP_OBJ_NULL)) {
            return false;
        }
        arg0 = dest[0];
    #endif

    } else {
        return false;
    }

    // success folding this rule

    for (size_t i = num_args; i > 0; i--) {
        pop_result(parser);
    }
    push_result_node(parser, make_node_const_object_optimised(parser, 0, arg0));

    return true;
}

#endif // MICROPY_COMP_CONST_FOLDING

#if MICROPY_COMP_CONST_TUPLE
static bool build_tuple_from_stack(parser_t *parser, size_t src_line, size_t num_args) {
    for (size_t i = num_args; i > 0;) {
        mp_parse_node_t pn = peek_result(parser, --i);
        if (!mp_parse_node_is_const(pn)) {
            return false;
        }
    }
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(num_args, NULL));
    for (size_t i = num_args; i > 0;) {
        mp_parse_node_t pn = pop_result(parser);
        tuple->items[--i] = mp_parse_node_convert_to_obj(pn);
        if (MP_PARSE_NODE_IS_STRUCT(pn)) {
            parser_free_parse_node_struct(parser, (mp_parse_node_struct_t *)pn);
        }
    }
    push_result_node(parser, make_node_const_object(parser, src_line, MP_OBJ_FROM_PTR(tuple)));
    return true;
}

static bool build_tuple(parser_t *parser, size_t src_line, uint8_t rule_id, size_t num_args) {
    if (rule_id == RULE_testlist_comp) {
        if (peek_rule(parser, 0) == RULE_atom_paren) {
            // Tuple of the form "(a,)".
            return build_tuple_from_stack(parser, src_line, num_args);
        }
    }
    if (rule_id == RULE_testlist_comp_3c) {
        assert(peek_rule(parser, 0) == RULE_testlist_comp_3b);
        assert(peek_rule(parser, 1) == RULE_testlist_comp);
        if (peek_rule(parser, 2) == RULE_atom_paren) {
            // Tuple of the form "(a, b)".
            if (build_tuple_from_stack(parser, src_line, num_args)) {
                parser->rule_stack_top -= 2; // discard 2 rules
                return true;
            }
        }
    }
    if (rule_id == RULE_testlist_star_expr
        || rule_id == RULE_testlist
        || rule_id == RULE_subscriptlist) {
        // Tuple of the form:
        //  - x = a, b
        //  - return a, b
        //  - for x in a, b: pass
        //  - x[a, b]
        return build_tuple_from_stack(parser, src_line, num_args);
    }

    return false;
}
#endif

static void push_result_rule(parser_t *parser, size_t src_line, uint8_t rule_id, size_t num_args) {
    // Simplify and optimise certain rules, to reduce memory usage and simplify the compiler.
    if (rule_id == RULE_atom_paren) {
        // Remove parenthesis around a single expression if possible.
        // This atom_paren rule always has a single argument, and after this
        // optimisation that argument is either NULL or testlist_comp.
        mp_parse_node_t pn = peek_result(parser, 0);
        if (MP_PARSE_NODE_IS_NULL(pn)) {
            // need to keep parenthesis for ()
        } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, RULE_testlist_comp)) {
            // need to keep parenthesis for (a, b, ...)
        } else {
            // parenthesis around a single expression, so it's just the expression
            return;
        }
    } else if (rule_id == RULE_testlist_comp) {
        // The testlist_comp rule can be the sole argument to either atom_parent
        // or atom_bracket, for (...) and [...] respectively.
        assert(num_args == 2);
        mp_parse_node_t pn = peek_result(parser, 0);
        if (MP_PARSE_NODE_IS_STRUCT(pn)) {
            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
            if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_testlist_comp_3b) {
                // tuple of one item, with trailing comma
                pop_result(parser);
                --num_args;
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_testlist_comp_3c) {
                // tuple of many items, convert testlist_comp_3c to testlist_comp
                pop_result(parser);
                assert(pn == peek_result(parser, 0));
                pns->kind_num_nodes = rule_id | MP_PARSE_NODE_STRUCT_NUM_NODES(pns) << 8;
                return;
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_comp_for) {
                // generator expression
            } else {
                // tuple with 2 items
            }
        } else {
            // tuple with 2 items
        }
    } else if (rule_id == RULE_testlist_comp_3c) {
        // steal first arg of outer testlist_comp rule
        ++num_args;
    }

    #if MICROPY_COMP_CONST_FOLDING
    if (fold_logical_constants(parser, rule_id, &num_args)) {
        // we folded this rule so return straight away
        return;
    }
    if (fold_constants(parser, rule_id, num_args)) {
        // we folded this rule so return straight away
        return;
    }
    #endif

    #if MICROPY_COMP_CONST_TUPLE
    if (build_tuple(parser, src_line, rule_id, num_args)) {
        // we built a tuple from this rule so return straight away
        return;
    }
    #endif

    mp_parse_node_struct_t *pn = parser_alloc(parser, sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * num_args);
    pn->source_line = src_line;
    pn->kind_num_nodes = (rule_id & 0xff) | (num_args << 8);
    for (size_t i = num_args; i > 0; i--) {
        pn->nodes[i - 1] = pop_result(parser);
    }
    if (rule_id == RULE_testlist_comp_3c) {
        // need to push something non-null to replace stolen first arg of testlist_comp
        push_result_node(parser, (mp_parse_node_t)pn);
    }
    push_result_node(parser, (mp_parse_node_t)pn);
}

mp_parse_tree_t mp_parse(mp_lexer_t *lex, mp_parse_input_kind_t input_kind) {
    // Set exception handler to free the lexer if an exception is raised.
    MP_DEFINE_NLR_JUMP_CALLBACK_FUNCTION_1(ctx, mp_lexer_free, lex);
    nlr_push_jump_callback(&ctx.callback, mp_call_function_1_from_nlr_jump_callback);

    // initialise parser and allocate memory for its stacks

    parser_t parser;

    parser.rule_stack_alloc = MICROPY_ALLOC_PARSE_RULE_INIT;
    parser.rule_stack_top = 0;
    parser.rule_stack = m_new(rule_stack_t, parser.rule_stack_alloc);

    parser.result_stack_alloc = MICROPY_ALLOC_PARSE_RESULT_INIT;
    parser.result_stack_top = 0;
    parser.result_stack = m_new(mp_parse_node_t, parser.result_stack_alloc);

    parser.lexer = lex;

    parser.tree.chunk = NULL;
    parser.cur_chunk = NULL;

    #if MICROPY_COMP_CONST
    mp_map_init(&parser.consts, 0);
    #endif

    // work out the top-level rule to use, and push it on the stack
    size_t top_level_rule;
    switch (input_kind) {
        case MP_PARSE_SINGLE_INPUT:
            top_level_rule = RULE_single_input;
            break;
        case MP_PARSE_EVAL_INPUT:
            top_level_rule = RULE_eval_input;
            break;
        default:
            top_level_rule = RULE_file_input;
    }
    push_rule(&parser, lex->tok_line, top_level_rule, 0);

    // parse!

    bool backtrack = false;

    for (;;) {
    next_rule:
        if (parser.rule_stack_top == 0) {
            break;
        }

        // Pop the next rule to process it
        size_t i; // state for the current rule
        size_t rule_src_line; // source line for the first token matched by the current rule
        uint8_t rule_id = pop_rule(&parser, &i, &rule_src_line);
        uint8_t rule_act = rule_act_table[rule_id];
        const uint16_t *rule_arg = get_rule_arg(rule_id);
        size_t n = rule_act & RULE_ACT_ARG_MASK;

        #if 0
        // debugging
        printf("depth=" UINT_FMT " ", parser.rule_stack_top);
        for (int j = 0; j < parser.rule_stack_top; ++j) {
            printf(" ");
        }
        printf("%s n=" UINT_FMT " i=" UINT_FMT " bt=%d\n", rule_name_table[rule_id], n, i, backtrack);
        #endif

        switch (rule_act & RULE_ACT_KIND_MASK) {
            case RULE_ACT_OR:
                if (i > 0 && !backtrack) {
                    goto next_rule;
                } else {
                    backtrack = false;
                }
                for (; i < n; ++i) {
                    uint16_t kind = rule_arg[i] & RULE_ARG_KIND_MASK;
                    if (kind == RULE_ARG_TOK) {
                        if (lex->tok_kind == (rule_arg[i] & RULE_ARG_ARG_MASK)) {
                            push_result_token(&parser, rule_id);
                            mp_lexer_to_next(lex);
                            goto next_rule;
                        }
                    } else {
                        assert(kind == RULE_ARG_RULE);
                        if (i + 1 < n) {
                            push_rule(&parser, rule_src_line, rule_id, i + 1); // save this or-rule
                        }
                        push_rule_from_arg(&parser, rule_arg[i]); // push child of or-rule
                        goto next_rule;
                    }
                }
                backtrack = true;
                break;

            case RULE_ACT_AND: {

                // failed, backtrack if we can, else syntax error
                if (backtrack) {
                    assert(i > 0);
                    if ((rule_arg[i - 1] & RULE_ARG_KIND_MASK) == RULE_ARG_OPT_RULE) {
                        // an optional rule that failed, so continue with next arg
                        push_result_node(&parser, MP_PARSE_NODE_NULL);
                        backtrack = false;
                    } else {
                        // a mandatory rule that failed, so propagate backtrack
                        if (i > 1) {
                            // already eaten tokens so can't backtrack
                            goto syntax_error;
                        } else {
                            goto next_rule;
                        }
                    }
                }

                // progress through the rule
                for (; i < n; ++i) {
                    if ((rule_arg[i] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                        // need to match a token
                        mp_token_kind_t tok_kind = rule_arg[i] & RULE_ARG_ARG_MASK;
                        if (lex->tok_kind == tok_kind) {
                            // matched token
                            if (tok_kind == MP_TOKEN_NAME) {
                                push_result_token(&parser, rule_id);
                            }
                            mp_lexer_to_next(lex);
                        } else {
                            // failed to match token
                            if (i > 0) {
                                // already eaten tokens so can't backtrack
                                goto syntax_error;
                            } else {
                                // this rule failed, so backtrack
                                backtrack = true;
                                goto next_rule;
                            }
                        }
                    } else {
                        push_rule(&parser, rule_src_line, rule_id, i + 1); // save this and-rule
                        push_rule_from_arg(&parser, rule_arg[i]); // push child of and-rule
                        goto next_rule;
                    }
                }

                assert(i == n);

                // matched the rule, so now build the corresponding parse_node

                #if !MICROPY_ENABLE_DOC_STRING
                // this code discards lonely statements, such as doc strings
                if (input_kind != MP_PARSE_SINGLE_INPUT && rule_id == RULE_expr_stmt && peek_result(&parser, 0) == MP_PARSE_NODE_NULL) {
                    mp_parse_node_t p = peek_result(&parser, 1);
                    if ((MP_PARSE_NODE_IS_LEAF(p) && !MP_PARSE_NODE_IS_ID(p))
                        || MP_PARSE_NODE_IS_STRUCT_KIND(p, RULE_const_object)) {
                        pop_result(&parser); // MP_PARSE_NODE_NULL
                        pop_result(&parser); // const expression (leaf or RULE_const_object)
                        // Pushing the "pass" rule here will overwrite any RULE_const_object
                        // entry that was on the result stack, allowing the GC to reclaim
                        // the memory from the const object when needed.
                        push_result_rule(&parser, rule_src_line, RULE_pass_stmt, 0);
                        break;
                    }
                }
                #endif

                // count number of arguments for the parse node
                i = 0;
                size_t num_not_nil = 0;
                for (size_t x = n; x > 0;) {
                    --x;
                    if ((rule_arg[x] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                        mp_token_kind_t tok_kind = rule_arg[x] & RULE_ARG_ARG_MASK;
                        if (tok_kind == MP_TOKEN_NAME) {
                            // only tokens which were names are pushed to stack
                            i += 1;
                            num_not_nil += 1;
                        }
                    } else {
                        // rules are always pushed
                        if (peek_result(&parser, i) != MP_PARSE_NODE_NULL) {
                            num_not_nil += 1;
                        }
                        i += 1;
                    }
                }

                if (num_not_nil == 1 && (rule_act & RULE_ACT_ALLOW_IDENT)) {
                    // this rule has only 1 argument and should not be emitted
                    mp_parse_node_t pn = MP_PARSE_NODE_NULL;
                    for (size_t x = 0; x < i; ++x) {
                        mp_parse_node_t pn2 = pop_result(&parser);
                        if (pn2 != MP_PARSE_NODE_NULL) {
                            pn = pn2;
                        }
                    }
                    push_result_node(&parser, pn);
                } else {
                    // this rule must be emitted

                    if (rule_act & RULE_ACT_ADD_BLANK) {
                        // and add an extra blank node at the end (used by the compiler to store data)
                        push_result_node(&parser, MP_PARSE_NODE_NULL);
                        i += 1;
                    }

                    push_result_rule(&parser, rule_src_line, rule_id, i);
                }
                break;
            }

            default: {
                assert((rule_act & RULE_ACT_KIND_MASK) == RULE_ACT_LIST);

                // n=2 is: item item*
                // n=1 is: item (sep item)*
                // n=3 is: item (sep item)* [sep]
                bool had_trailing_sep;
                if (backtrack) {
                list_backtrack:
                    had_trailing_sep = false;
                    if (n == 2) {
                        if (i == 1) {
                            // fail on item, first time round; propagate backtrack
                            goto next_rule;
                        } else {
                            // fail on item, in later rounds; finish with this rule
                            backtrack = false;
                        }
                    } else {
                        if (i == 1) {
                            // fail on item, first time round; propagate backtrack
                            goto next_rule;
                        } else if ((i & 1) == 1) {
                            // fail on item, in later rounds; have eaten tokens so can't backtrack
                            if (n == 3) {
                                // list allows trailing separator; finish parsing list
                                had_trailing_sep = true;
                                backtrack = false;
                            } else {
                                // list doesn't allowing trailing separator; fail
                                goto syntax_error;
                            }
                        } else {
                            // fail on separator; finish parsing list
                            backtrack = false;
                        }
                    }
                } else {
                    for (;;) {
                        size_t arg = rule_arg[i & 1 & n];
                        if ((arg & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                            if (lex->tok_kind == (arg & RULE_ARG_ARG_MASK)) {
                                if (i & 1 & n) {
                                    // separators which are tokens are not pushed to result stack
                                } else {
                                    push_result_token(&parser, rule_id);
                                }
                                mp_lexer_to_next(lex);
                                // got element of list, so continue parsing list
                                i += 1;
                            } else {
                                // couldn't get element of list
                                i += 1;
                                backtrack = true;
                                goto list_backtrack;
                            }
                        } else {
                            assert((arg & RULE_ARG_KIND_MASK) == RULE_ARG_RULE);
                            push_rule(&parser, rule_src_line, rule_id, i + 1); // save this list-rule
                            push_rule_from_arg(&parser, arg); // push child of list-rule
                            goto next_rule;
                        }
                    }
                }
                assert(i >= 1);

                // compute number of elements in list, result in i
                i -= 1;
                if ((n & 1) && (rule_arg[1] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                    // don't count separators when they are tokens
                    i = (i + 1) / 2;
                }

                if (i == 1) {
                    // list matched single item
                    if (had_trailing_sep) {
                        // if there was a trailing separator, make a list of a single item
                        push_result_rule(&parser, rule_src_line, rule_id, i);
                    } else {
                        // just leave single item on stack (ie don't wrap in a list)
                    }
                } else {
                    push_result_rule(&parser, rule_src_line, rule_id, i);
                }
                break;
            }
        }
    }

    #if MICROPY_COMP_CONST
    mp_map_deinit(&parser.consts);
    #endif

    // truncate final chunk and link into chain of chunks
    if (parser.cur_chunk != NULL) {
        (void)m_renew_maybe(byte, parser.cur_chunk,
            sizeof(mp_parse_chunk_t) + parser.cur_chunk->alloc,
            sizeof(mp_parse_chunk_t) + parser.cur_chunk->union_.used,
            false);
        parser.cur_chunk->alloc = parser.cur_chunk->union_.used;
        parser.cur_chunk->union_.next = parser.tree.chunk;
        parser.tree.chunk = parser.cur_chunk;
    }

    if (
        lex->tok_kind != MP_TOKEN_END // check we are at the end of the token stream
        || parser.result_stack_top == 0 // check that we got a node (can fail on empty input)
        ) {
    syntax_error:;
        mp_obj_t exc;
        if (lex->tok_kind == MP_TOKEN_INDENT) {
            exc = mp_obj_new_exception_msg(&mp_type_IndentationError,
                MP_ERROR_TEXT("unexpected indent"));
        } else if (lex->tok_kind == MP_TOKEN_DEDENT_MISMATCH) {
            exc = mp_obj_new_exception_msg(&mp_type_IndentationError,
                MP_ERROR_TEXT("unindent doesn't match any outer indent level"));
        #if MICROPY_PY_FSTRINGS
        } else if (lex->tok_kind == MP_TOKEN_MALFORMED_FSTRING) {
            exc = mp_obj_new_exception_msg(&mp_type_SyntaxError,
                MP_ERROR_TEXT("malformed f-string"));
        #endif
        } else {
            exc = mp_obj_new_exception_msg(&mp_type_SyntaxError,
                MP_ERROR_TEXT("invalid syntax"));
        }
        // add traceback to give info about file name and location
        // we don't have a 'block' name, so just pass the NULL qstr to indicate this
        mp_obj_exception_add_traceback(exc, lex->source_name, lex->tok_line, MP_QSTRnull);
        nlr_raise(exc);
    }

    // get the root parse node that we created
    assert(parser.result_stack_top == 1);
    parser.tree.root = parser.result_stack[0];

    // free the memory that we don't need anymore
    m_del(rule_stack_t, parser.rule_stack, parser.rule_stack_alloc);
    m_del(mp_parse_node_t, parser.result_stack, parser.result_stack_alloc);

    // Deregister exception handler and free the lexer.
    nlr_pop_jump_callback(true);

    return parser.tree;
}

void mp_parse_tree_clear(mp_parse_tree_t *tree) {
    mp_parse_chunk_t *chunk = tree->chunk;
    while (chunk != NULL) {
        mp_parse_chunk_t *next = chunk->union_.next;
        m_del(byte, chunk, sizeof(mp_parse_chunk_t) + chunk->alloc);
        chunk = next;
    }
    tree->chunk = NULL; // Avoid dangling pointer that may live on stack
}

#endif // MICROPY_ENABLE_COMPILER
