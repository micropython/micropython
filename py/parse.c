/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
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

#include "py/nlr.h"
#include "py/lexer.h"
#include "py/parse.h"
#include "py/parsenum.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/objint.h"
#include "py/builtin.h"

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

// (un)comment to use rule names; for debugging
//#define USE_RULE_NAME (1)

typedef struct _rule_t {
    byte rule_id;
    byte act;
#ifdef USE_RULE_NAME
    const char *rule_name;
#endif
    uint16_t arg[];
} rule_t;

enum {
#define DEF_RULE(rule, comp, kind, ...) RULE_##rule,
#include "py/grammar.h"
#undef DEF_RULE
    RULE_maximum_number_of,
    RULE_string, // special node for non-interned string
    RULE_bytes, // special node for non-interned bytes
    RULE_const_object, // special node for a constant, generic Python object
};

#define or(n)                   (RULE_ACT_OR | n)
#define and(n)                  (RULE_ACT_AND | n)
#define and_ident(n)            (RULE_ACT_AND | n | RULE_ACT_ALLOW_IDENT)
#define and_blank(n)            (RULE_ACT_AND | n | RULE_ACT_ADD_BLANK)
#define one_or_more             (RULE_ACT_LIST | 2)
#define list                    (RULE_ACT_LIST | 1)
#define list_with_end           (RULE_ACT_LIST | 3)
#define tok(t)                  (RULE_ARG_TOK | MP_TOKEN_##t)
#define rule(r)                 (RULE_ARG_RULE | RULE_##r)
#define opt_rule(r)             (RULE_ARG_OPT_RULE | RULE_##r)
#ifdef USE_RULE_NAME
#define DEF_RULE(rule, comp, kind, ...) static const rule_t rule_##rule = { RULE_##rule, kind, #rule, { __VA_ARGS__ } };
#else
#define DEF_RULE(rule, comp, kind, ...) static const rule_t rule_##rule = { RULE_##rule, kind, { __VA_ARGS__ } };
#endif
#include "py/grammar.h"
#undef or
#undef and
#undef list
#undef list_with_end
#undef tok
#undef rule
#undef opt_rule
#undef one_or_more
#undef DEF_RULE

STATIC const rule_t *const rules[] = {
#define DEF_RULE(rule, comp, kind, ...) &rule_##rule,
#include "py/grammar.h"
#undef DEF_RULE
};

typedef struct _rule_stack_t {
    size_t src_line : 8 * sizeof(size_t) - 8; // maximum bits storing source line number
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

typedef enum {
    PARSE_ERROR_NONE = 0,
    PARSE_ERROR_MEMORY,
    PARSE_ERROR_CONST,
} parse_error_t;

typedef struct _parser_t {
    parse_error_t parse_error;

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

STATIC void *parser_alloc(parser_t *parser, size_t num_bytes) {
    // use a custom memory allocator to store parse nodes sequentially in large chunks

    mp_parse_chunk_t *chunk = parser->cur_chunk;

    if (chunk != NULL && chunk->union_.used + num_bytes > chunk->alloc) {
        // not enough room at end of previously allocated chunk so try to grow
        mp_parse_chunk_t *new_data = (mp_parse_chunk_t*)m_renew_maybe(byte, chunk,
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
        chunk = (mp_parse_chunk_t*)m_new(byte, sizeof(mp_parse_chunk_t) + alloc);
        chunk->alloc = alloc;
        chunk->union_.used = 0;
        parser->cur_chunk = chunk;
    }

    byte *ret = chunk->data + chunk->union_.used;
    chunk->union_.used += num_bytes;
    return ret;
}

STATIC void push_rule(parser_t *parser, size_t src_line, const rule_t *rule, size_t arg_i) {
    if (parser->parse_error) {
        return;
    }
    if (parser->rule_stack_top >= parser->rule_stack_alloc) {
        rule_stack_t *rs = m_renew_maybe(rule_stack_t, parser->rule_stack, parser->rule_stack_alloc, parser->rule_stack_alloc + MICROPY_ALLOC_PARSE_RULE_INC, true);
        if (rs == NULL) {
            parser->parse_error = PARSE_ERROR_MEMORY;
            return;
        }
        parser->rule_stack = rs;
        parser->rule_stack_alloc += MICROPY_ALLOC_PARSE_RULE_INC;
    }
    rule_stack_t *rs = &parser->rule_stack[parser->rule_stack_top++];
    rs->src_line = src_line;
    rs->rule_id = rule->rule_id;
    rs->arg_i = arg_i;
}

STATIC void push_rule_from_arg(parser_t *parser, size_t arg) {
    assert((arg & RULE_ARG_KIND_MASK) == RULE_ARG_RULE || (arg & RULE_ARG_KIND_MASK) == RULE_ARG_OPT_RULE);
    size_t rule_id = arg & RULE_ARG_ARG_MASK;
    assert(rule_id < RULE_maximum_number_of);
    push_rule(parser, parser->lexer->tok_line, rules[rule_id], 0);
}

STATIC void pop_rule(parser_t *parser, const rule_t **rule, size_t *arg_i, size_t *src_line) {
    assert(!parser->parse_error);
    parser->rule_stack_top -= 1;
    *rule = rules[parser->rule_stack[parser->rule_stack_top].rule_id];
    *arg_i = parser->rule_stack[parser->rule_stack_top].arg_i;
    *src_line = parser->rule_stack[parser->rule_stack_top].src_line;
}

mp_parse_node_t mp_parse_node_new_leaf(size_t kind, mp_int_t arg) {
    if (kind == MP_PARSE_NODE_SMALL_INT) {
        return (mp_parse_node_t)(kind | (arg << 1));
    }
    return (mp_parse_node_t)(kind | (arg << 4));
}

bool mp_parse_node_get_int_maybe(mp_parse_node_t pn, mp_obj_t *o) {
    if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        *o = MP_OBJ_NEW_SMALL_INT(MP_PARSE_NODE_LEAF_SMALL_INT(pn));
        return true;
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, RULE_const_object)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
        // nodes are 32-bit pointers, but need to extract 64-bit object
        *o = (uint64_t)pns->nodes[0] | ((uint64_t)pns->nodes[1] << 32);
        #else
        *o = (mp_obj_t)pns->nodes[0];
        #endif
        return MP_OBJ_IS_INT(*o);
    } else {
        return false;
    }
}

int mp_parse_node_extract_list(mp_parse_node_t *pn, size_t pn_kind, mp_parse_node_t **nodes) {
    if (MP_PARSE_NODE_IS_NULL(*pn)) {
        *nodes = NULL;
        return 0;
    } else if (MP_PARSE_NODE_IS_LEAF(*pn)) {
        *nodes = pn;
        return 1;
    } else {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)(*pn);
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
void mp_parse_node_print(mp_parse_node_t pn, size_t indent) {
    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        printf("[% 4d] ", (int)((mp_parse_node_struct_t*)pn)->source_line);
    } else {
        printf("       ");
    }
    for (size_t i = 0; i < indent; i++) {
        printf(" ");
    }
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        printf("NULL\n");
    } else if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        mp_int_t arg = MP_PARSE_NODE_LEAF_SMALL_INT(pn);
        printf("int(" INT_FMT ")\n", arg);
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        uintptr_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
        switch (MP_PARSE_NODE_LEAF_KIND(pn)) {
            case MP_PARSE_NODE_ID: printf("id(%s)\n", qstr_str(arg)); break;
            case MP_PARSE_NODE_STRING: printf("str(%s)\n", qstr_str(arg)); break;
            case MP_PARSE_NODE_BYTES: printf("bytes(%s)\n", qstr_str(arg)); break;
            case MP_PARSE_NODE_TOKEN: printf("tok(%u)\n", (uint)arg); break;
            default: assert(0);
        }
    } else {
        // node must be a mp_parse_node_struct_t
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_string) {
            printf("literal str(%.*s)\n", (int)pns->nodes[1], (char*)pns->nodes[0]);
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_bytes) {
            printf("literal bytes(%.*s)\n", (int)pns->nodes[1], (char*)pns->nodes[0]);
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_const_object) {
            #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
            printf("literal const(%016llx)\n", (uint64_t)pns->nodes[0] | ((uint64_t)pns->nodes[1] << 32));
            #else
            printf("literal const(%p)\n", (mp_obj_t)pns->nodes[0]);
            #endif
        } else {
            size_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
#ifdef USE_RULE_NAME
            printf("%s(%u) (n=%u)\n", rules[MP_PARSE_NODE_STRUCT_KIND(pns)]->rule_name, (uint)MP_PARSE_NODE_STRUCT_KIND(pns), (uint)n);
#else
            printf("rule(%u) (n=%u)\n", (uint)MP_PARSE_NODE_STRUCT_KIND(pns), (uint)n);
#endif
            for (size_t i = 0; i < n; i++) {
                mp_parse_node_print(pns->nodes[i], indent + 2);
            }
        }
    }
}
#endif // MICROPY_DEBUG_PRINTERS

/*
STATIC void result_stack_show(parser_t *parser) {
    printf("result stack, most recent first\n");
    for (ssize_t i = parser->result_stack_top - 1; i >= 0; i--) {
        mp_parse_node_print(parser->result_stack[i], 0);
    }
}
*/

STATIC mp_parse_node_t pop_result(parser_t *parser) {
    if (parser->parse_error) {
        return MP_PARSE_NODE_NULL;
    }
    assert(parser->result_stack_top > 0);
    return parser->result_stack[--parser->result_stack_top];
}

STATIC mp_parse_node_t peek_result(parser_t *parser, size_t pos) {
    if (parser->parse_error) {
        return MP_PARSE_NODE_NULL;
    }
    assert(parser->result_stack_top > pos);
    return parser->result_stack[parser->result_stack_top - 1 - pos];
}

STATIC void push_result_node(parser_t *parser, mp_parse_node_t pn) {
    if (parser->parse_error) {
        return;
    }
    if (parser->result_stack_top >= parser->result_stack_alloc) {
        mp_parse_node_t *stack = m_renew_maybe(mp_parse_node_t, parser->result_stack, parser->result_stack_alloc, parser->result_stack_alloc + MICROPY_ALLOC_PARSE_RESULT_INC, true);
        if (stack == NULL) {
            parser->parse_error = PARSE_ERROR_MEMORY;
            return;
        }
        parser->result_stack = stack;
        parser->result_stack_alloc += MICROPY_ALLOC_PARSE_RESULT_INC;
    }
    parser->result_stack[parser->result_stack_top++] = pn;
}

STATIC mp_parse_node_t make_node_string_bytes(parser_t *parser, size_t src_line, size_t rule_kind, const char *str, size_t len) {
    mp_parse_node_struct_t *pn = parser_alloc(parser, sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * 2);
    if (pn == NULL) {
        parser->parse_error = PARSE_ERROR_MEMORY;
        return MP_PARSE_NODE_NULL;
    }
    pn->source_line = src_line;
    pn->kind_num_nodes = rule_kind | (2 << 8);
    char *p = m_new(char, len);
    memcpy(p, str, len);
    pn->nodes[0] = (uintptr_t)p;
    pn->nodes[1] = len;
    return (mp_parse_node_t)pn;
}

STATIC mp_parse_node_t make_node_const_object(parser_t *parser, size_t src_line, mp_obj_t obj) {
    mp_parse_node_struct_t *pn = parser_alloc(parser, sizeof(mp_parse_node_struct_t) + sizeof(mp_obj_t));
    if (pn == NULL) {
        parser->parse_error = PARSE_ERROR_MEMORY;
        return MP_PARSE_NODE_NULL;
    }
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

STATIC void push_result_token(parser_t *parser, const rule_t *rule) {
    mp_parse_node_t pn;
    mp_lexer_t *lex = parser->lexer;
    if (lex->tok_kind == MP_TOKEN_NAME) {
        qstr id = qstr_from_strn(lex->vstr.buf, lex->vstr.len);
        #if MICROPY_COMP_CONST
        // if name is a standalone identifier, look it up in the table of dynamic constants
        mp_map_elem_t *elem;
        if (rule->rule_id == RULE_atom
            && (elem = mp_map_lookup(&parser->consts, MP_OBJ_NEW_QSTR(id), MP_MAP_LOOKUP)) != NULL) {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, MP_OBJ_SMALL_INT_VALUE(elem->value));
        } else {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_ID, id);
        }
        #else
        (void)rule;
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_ID, id);
        #endif
    } else if (lex->tok_kind == MP_TOKEN_INTEGER) {
        mp_obj_t o = mp_parse_num_integer(lex->vstr.buf, lex->vstr.len, 0, lex);
        if (MP_OBJ_IS_SMALL_INT(o)) {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, MP_OBJ_SMALL_INT_VALUE(o));
        } else {
            pn = make_node_const_object(parser, lex->tok_line, o);
        }
    } else if (lex->tok_kind == MP_TOKEN_FLOAT_OR_IMAG) {
        mp_obj_t o = mp_parse_num_decimal(lex->vstr.buf, lex->vstr.len, true, false, lex);
        pn = make_node_const_object(parser, lex->tok_line, o);
    } else if (lex->tok_kind == MP_TOKEN_STRING || lex->tok_kind == MP_TOKEN_BYTES) {
        // Don't automatically intern all strings/bytes.  doc strings (which are usually large)
        // will be discarded by the compiler, and so we shouldn't intern them.
        qstr qst = MP_QSTR_NULL;
        if (lex->vstr.len <= MICROPY_ALLOC_PARSE_INTERN_STRING_LEN) {
            // intern short strings
            qst = qstr_from_strn(lex->vstr.buf, lex->vstr.len);
        } else {
            // check if this string is already interned
            qst = qstr_find_strn(lex->vstr.buf, lex->vstr.len);
        }
        if (qst != MP_QSTR_NULL) {
            // qstr exists, make a leaf node
            pn = mp_parse_node_new_leaf(lex->tok_kind == MP_TOKEN_STRING ? MP_PARSE_NODE_STRING : MP_PARSE_NODE_BYTES, qst);
        } else {
            // not interned, make a node holding a pointer to the string/bytes data
            pn = make_node_string_bytes(parser, lex->tok_line, lex->tok_kind == MP_TOKEN_STRING ? RULE_string : RULE_bytes, lex->vstr.buf, lex->vstr.len);
        }
    } else {
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_TOKEN, lex->tok_kind);
    }
    push_result_node(parser, pn);
}

#if MICROPY_COMP_MODULE_CONST
STATIC const mp_rom_map_elem_t mp_constants_table[] = {
    #if MICROPY_PY_UERRNO
    { MP_ROM_QSTR(MP_QSTR_errno), MP_ROM_PTR(&mp_module_uerrno) },
    #endif
    #if MICROPY_PY_UCTYPES
    { MP_ROM_QSTR(MP_QSTR_uctypes), MP_ROM_PTR(&mp_module_uctypes) },
    #endif
    // Extra constants as defined by a port
    MICROPY_PORT_CONSTANTS
};
STATIC MP_DEFINE_CONST_MAP(mp_constants_map, mp_constants_table);
#endif

STATIC void push_result_rule(parser_t *parser, size_t src_line, const rule_t *rule, size_t num_args);

#if MICROPY_COMP_CONST_FOLDING
STATIC bool fold_constants(parser_t *parser, const rule_t *rule, size_t num_args) {
    // this code does folding of arbitrary integer expressions, eg 1 + 2 * 3 + 4
    // it does not do partial folding, eg 1 + 2 + x -> 3 + x

    mp_obj_t arg0;
    if (rule->rule_id == RULE_expr
        || rule->rule_id == RULE_xor_expr
        || rule->rule_id == RULE_and_expr) {
        // folding for binary ops: | ^ &
        mp_parse_node_t pn = peek_result(parser, num_args - 1);
        if (!mp_parse_node_get_int_maybe(pn, &arg0)) {
            return false;
        }
        mp_binary_op_t op;
        if (rule->rule_id == RULE_expr) {
            op = MP_BINARY_OP_OR;
        } else if (rule->rule_id == RULE_xor_expr) {
            op = MP_BINARY_OP_XOR;
        } else {
            op = MP_BINARY_OP_AND;
        }
        for (ssize_t i = num_args - 2; i >= 0; --i) {
            pn = peek_result(parser, i);
            mp_obj_t arg1;
            if (!mp_parse_node_get_int_maybe(pn, &arg1)) {
                return false;
            }
            arg0 = mp_binary_op(op, arg0, arg1);
        }
    } else if (rule->rule_id == RULE_shift_expr
        || rule->rule_id == RULE_arith_expr
        || rule->rule_id == RULE_term) {
        // folding for binary ops: << >> + - * / % //
        mp_parse_node_t pn = peek_result(parser, num_args - 1);
        if (!mp_parse_node_get_int_maybe(pn, &arg0)) {
            return false;
        }
        for (ssize_t i = num_args - 2; i >= 1; i -= 2) {
            pn = peek_result(parser, i - 1);
            mp_obj_t arg1;
            if (!mp_parse_node_get_int_maybe(pn, &arg1)) {
                return false;
            }
            mp_token_kind_t tok = MP_PARSE_NODE_LEAF_ARG(peek_result(parser, i));
            static const uint8_t token_to_op[] = {
                MP_BINARY_OP_ADD,
                MP_BINARY_OP_SUBTRACT,
                MP_BINARY_OP_MULTIPLY,
                255,//MP_BINARY_OP_POWER,
                255,//MP_BINARY_OP_TRUE_DIVIDE,
                MP_BINARY_OP_FLOOR_DIVIDE,
                MP_BINARY_OP_MODULO,
                255,//MP_BINARY_OP_LESS
                MP_BINARY_OP_LSHIFT,
                255,//MP_BINARY_OP_MORE
                MP_BINARY_OP_RSHIFT,
            };
            mp_binary_op_t op = token_to_op[tok - MP_TOKEN_OP_PLUS];
            if (op == (mp_binary_op_t)255) {
                return false;
            }
            int rhs_sign = mp_obj_int_sign(arg1);
            if (op <= MP_BINARY_OP_RSHIFT) {
                // << and >> can't have negative rhs
                if (rhs_sign < 0) {
                    return false;
                }
            } else if (op >= MP_BINARY_OP_FLOOR_DIVIDE) {
                // % and // can't have zero rhs
                if (rhs_sign == 0) {
                    return false;
                }
            }
            arg0 = mp_binary_op(op, arg0, arg1);
        }
    } else if (rule->rule_id == RULE_factor_2) {
        // folding for unary ops: + - ~
        mp_parse_node_t pn = peek_result(parser, 0);
        if (!mp_parse_node_get_int_maybe(pn, &arg0)) {
            return false;
        }
        mp_token_kind_t tok = MP_PARSE_NODE_LEAF_ARG(peek_result(parser, 1));
        mp_unary_op_t op;
        if (tok == MP_TOKEN_OP_PLUS) {
            op = MP_UNARY_OP_POSITIVE;
        } else if (tok == MP_TOKEN_OP_MINUS) {
            op = MP_UNARY_OP_NEGATIVE;
        } else {
            assert(tok == MP_TOKEN_OP_TILDE); // should be
            op = MP_UNARY_OP_INVERT;
        }
        arg0 = mp_unary_op(op, arg0);

    #if MICROPY_COMP_CONST
    } else if (rule->rule_id == RULE_expr_stmt) {
        mp_parse_node_t pn1 = peek_result(parser, 0);
        if (!MP_PARSE_NODE_IS_NULL(pn1)
            && !(MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_expr_stmt_augassign)
            || MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_expr_stmt_assign_list))) {
            // this node is of the form <x> = <y>
            mp_parse_node_t pn0 = peek_result(parser, 1);
            if (MP_PARSE_NODE_IS_ID(pn0)
                && MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_atom_expr_normal)
                && MP_PARSE_NODE_IS_ID(((mp_parse_node_struct_t*)pn1)->nodes[0])
                && MP_PARSE_NODE_LEAF_ARG(((mp_parse_node_struct_t*)pn1)->nodes[0]) == MP_QSTR_const
                && MP_PARSE_NODE_IS_STRUCT_KIND(((mp_parse_node_struct_t*)pn1)->nodes[1], RULE_trailer_paren)
                ) {
                // code to assign dynamic constants: id = const(value)

                // get the id
                qstr id = MP_PARSE_NODE_LEAF_ARG(pn0);

                // get the value
                mp_parse_node_t pn_value = ((mp_parse_node_struct_t*)((mp_parse_node_struct_t*)pn1)->nodes[1])->nodes[0];
                if (!MP_PARSE_NODE_IS_SMALL_INT(pn_value)) {
                    parser->parse_error = PARSE_ERROR_CONST;
                    return false;
                }
                mp_int_t value = MP_PARSE_NODE_LEAF_SMALL_INT(pn_value);

                // store the value in the table of dynamic constants
                mp_map_elem_t *elem = mp_map_lookup(&parser->consts, MP_OBJ_NEW_QSTR(id), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
                assert(elem->value == MP_OBJ_NULL);
                elem->value = MP_OBJ_NEW_SMALL_INT(value);

                // If the constant starts with an underscore then treat it as a private
                // variable and don't emit any code to store the value to the id.
                if (qstr_str(id)[0] == '_') {
                    pop_result(parser); // pop const(value)
                    pop_result(parser); // pop id
                    push_result_rule(parser, 0, rules[RULE_pass_stmt], 0); // replace with "pass"
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
    } else if (rule->rule_id == RULE_atom_expr_normal) {
        mp_parse_node_t pn0 = peek_result(parser, 1);
        mp_parse_node_t pn1 = peek_result(parser, 0);
        if (!(MP_PARSE_NODE_IS_ID(pn0)
            && MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_trailer_period))) {
            return false;
        }
        // id1.id2
        // look it up in constant table, see if it can be replaced with an integer
        mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pn1;
        assert(MP_PARSE_NODE_IS_ID(pns1->nodes[0]));
        qstr q_base = MP_PARSE_NODE_LEAF_ARG(pn0);
        qstr q_attr = MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]);
        mp_map_elem_t *elem = mp_map_lookup((mp_map_t*)&mp_constants_map, MP_OBJ_NEW_QSTR(q_base), MP_MAP_LOOKUP);
        if (elem == NULL) {
            return false;
        }
        mp_obj_t dest[2];
        mp_load_method_maybe(elem->value, q_attr, dest);
        if (!(dest[0] != MP_OBJ_NULL && MP_OBJ_IS_INT(dest[0]) && dest[1] == MP_OBJ_NULL)) {
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
    if (MP_OBJ_IS_SMALL_INT(arg0)) {
        push_result_node(parser, mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, MP_OBJ_SMALL_INT_VALUE(arg0)));
    } else {
        // TODO reuse memory for parse node struct?
        push_result_node(parser, make_node_const_object(parser, 0, arg0));
    }

    return true;
}
#endif

STATIC void push_result_rule(parser_t *parser, size_t src_line, const rule_t *rule, size_t num_args) {
    // optimise away parenthesis around an expression if possible
    if (rule->rule_id == RULE_atom_paren) {
        // there should be just 1 arg for this rule
        mp_parse_node_t pn = peek_result(parser, 0);
        if (MP_PARSE_NODE_IS_NULL(pn)) {
            // need to keep parenthesis for ()
        } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, RULE_testlist_comp)) {
            // need to keep parenthesis for (a, b, ...)
        } else {
            // parenthesis around a single expression, so it's just the expression
            return;
        }
    }

    #if MICROPY_COMP_CONST_FOLDING
    if (fold_constants(parser, rule, num_args)) {
        // we folded this rule so return straight away
        return;
    }
    #endif

    mp_parse_node_struct_t *pn = parser_alloc(parser, sizeof(mp_parse_node_struct_t) + sizeof(mp_parse_node_t) * num_args);
    if (pn == NULL) {
        parser->parse_error = PARSE_ERROR_MEMORY;
        return;
    }
    pn->source_line = src_line;
    pn->kind_num_nodes = (rule->rule_id & 0xff) | (num_args << 8);
    for (size_t i = num_args; i > 0; i--) {
        pn->nodes[i - 1] = pop_result(parser);
    }
    push_result_node(parser, (mp_parse_node_t)pn);
}

mp_parse_tree_t mp_parse(mp_lexer_t *lex, mp_parse_input_kind_t input_kind) {

    // initialise parser and allocate memory for its stacks

    parser_t parser;

    parser.parse_error = PARSE_ERROR_NONE;

    parser.rule_stack_alloc = MICROPY_ALLOC_PARSE_RULE_INIT;
    parser.rule_stack_top = 0;
    parser.rule_stack = m_new_maybe(rule_stack_t, parser.rule_stack_alloc);

    parser.result_stack_alloc = MICROPY_ALLOC_PARSE_RESULT_INIT;
    parser.result_stack_top = 0;
    parser.result_stack = m_new_maybe(mp_parse_node_t, parser.result_stack_alloc);

    parser.lexer = lex;

    parser.tree.chunk = NULL;
    parser.cur_chunk = NULL;

    #if MICROPY_COMP_CONST
    mp_map_init(&parser.consts, 0);
    #endif

    // check if we could allocate the stacks
    if (parser.rule_stack == NULL || parser.result_stack == NULL) {
        goto memory_error;
    }

    // work out the top-level rule to use, and push it on the stack
    size_t top_level_rule;
    switch (input_kind) {
        case MP_PARSE_SINGLE_INPUT: top_level_rule = RULE_single_input; break;
        case MP_PARSE_EVAL_INPUT: top_level_rule = RULE_eval_input; break;
        default: top_level_rule = RULE_file_input;
    }
    push_rule(&parser, lex->tok_line, rules[top_level_rule], 0);

    // parse!

    size_t n, i; // state for the current rule
    size_t rule_src_line; // source line for the first token matched by the current rule
    bool backtrack = false;
    const rule_t *rule = NULL;

    for (;;) {
        next_rule:
        if (parser.rule_stack_top == 0 || parser.parse_error) {
            break;
        }

        pop_rule(&parser, &rule, &i, &rule_src_line);
        n = rule->act & RULE_ACT_ARG_MASK;

        /*
        // debugging
        printf("depth=%d ", parser.rule_stack_top);
        for (int j = 0; j < parser.rule_stack_top; ++j) {
            printf(" ");
        }
        printf("%s n=%d i=%d bt=%d\n", rule->rule_name, n, i, backtrack);
        */

        switch (rule->act & RULE_ACT_KIND_MASK) {
            case RULE_ACT_OR:
                if (i > 0 && !backtrack) {
                    goto next_rule;
                } else {
                    backtrack = false;
                }
                for (; i < n; ++i) {
                    uint16_t kind = rule->arg[i] & RULE_ARG_KIND_MASK;
                    if (kind == RULE_ARG_TOK) {
                        if (lex->tok_kind == (rule->arg[i] & RULE_ARG_ARG_MASK)) {
                            push_result_token(&parser, rule);
                            mp_lexer_to_next(lex);
                            goto next_rule;
                        }
                    } else {
                        assert(kind == RULE_ARG_RULE);
                        if (i + 1 < n) {
                            push_rule(&parser, rule_src_line, rule, i + 1); // save this or-rule
                        }
                        push_rule_from_arg(&parser, rule->arg[i]); // push child of or-rule
                        goto next_rule;
                    }
                }
                backtrack = true;
                break;

            case RULE_ACT_AND: {

                // failed, backtrack if we can, else syntax error
                if (backtrack) {
                    assert(i > 0);
                    if ((rule->arg[i - 1] & RULE_ARG_KIND_MASK) == RULE_ARG_OPT_RULE) {
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
                    switch (rule->arg[i] & RULE_ARG_KIND_MASK) {
                        case RULE_ARG_TOK: {
                            // need to match a token
                            mp_token_kind_t tok_kind = rule->arg[i] & RULE_ARG_ARG_MASK;
                            if (lex->tok_kind == tok_kind) {
                                // matched token
                                if (tok_kind == MP_TOKEN_NAME) {
                                    push_result_token(&parser, rule);
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
                            break;
                        }
                        case RULE_ARG_RULE:
                        case RULE_ARG_OPT_RULE:
                        rule_and_no_other_choice:
                            push_rule(&parser, rule_src_line, rule, i + 1); // save this and-rule
                            push_rule_from_arg(&parser, rule->arg[i]); // push child of and-rule
                            goto next_rule;
                        default:
                            assert(0);
                            goto rule_and_no_other_choice; // to help flow control analysis
                    }
                }

                assert(i == n);

                // matched the rule, so now build the corresponding parse_node

                #if !MICROPY_ENABLE_DOC_STRING
                // this code discards lonely statements, such as doc strings
                if (input_kind != MP_PARSE_SINGLE_INPUT && rule->rule_id == RULE_expr_stmt && peek_result(&parser, 0) == MP_PARSE_NODE_NULL) {
                    mp_parse_node_t p = peek_result(&parser, 1);
                    if ((MP_PARSE_NODE_IS_LEAF(p) && !MP_PARSE_NODE_IS_ID(p)) || MP_PARSE_NODE_IS_STRUCT_KIND(p, RULE_string)) {
                        pop_result(&parser); // MP_PARSE_NODE_NULL
                        mp_parse_node_t pn = pop_result(&parser); // possibly RULE_string
                        if (MP_PARSE_NODE_IS_STRUCT(pn)) {
                            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
                            if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_string) {
                                m_del(char, (char*)pns->nodes[0], (size_t)pns->nodes[1]);
                            }
                        }
                        push_result_rule(&parser, rule_src_line, rules[RULE_pass_stmt], 0);
                        break;
                    }
                }
                #endif

                // count number of arguments for the parse node
                i = 0;
                size_t num_not_nil = 0;
                for (size_t x = n; x > 0;) {
                    --x;
                    if ((rule->arg[x] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                        mp_token_kind_t tok_kind = rule->arg[x] & RULE_ARG_ARG_MASK;
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

                if (num_not_nil == 1 && (rule->act & RULE_ACT_ALLOW_IDENT)) {
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

                    if (rule->act & RULE_ACT_ADD_BLANK) {
                        // and add an extra blank node at the end (used by the compiler to store data)
                        push_result_node(&parser, MP_PARSE_NODE_NULL);
                        i += 1;
                    }

                    push_result_rule(&parser, rule_src_line, rule, i);
                }
                break;
            }

            case RULE_ACT_LIST: {
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
                        size_t arg = rule->arg[i & 1 & n];
                        switch (arg & RULE_ARG_KIND_MASK) {
                            case RULE_ARG_TOK:
                                if (lex->tok_kind == (arg & RULE_ARG_ARG_MASK)) {
                                    if (i & 1 & n) {
                                        // separators which are tokens are not pushed to result stack
                                    } else {
                                        push_result_token(&parser, rule);
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
                                break;
                            case RULE_ARG_RULE:
                            rule_list_no_other_choice:
                                push_rule(&parser, rule_src_line, rule, i + 1); // save this list-rule
                                push_rule_from_arg(&parser, arg); // push child of list-rule
                                goto next_rule;
                            default:
                                assert(0);
                                goto rule_list_no_other_choice; // to help flow control analysis
                        }
                    }
                }
                assert(i >= 1);

                // compute number of elements in list, result in i
                i -= 1;
                if ((n & 1) && (rule->arg[1] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                    // don't count separators when they are tokens
                    i = (i + 1) / 2;
                }

                if (i == 1) {
                    // list matched single item
                    if (had_trailing_sep) {
                        // if there was a trailing separator, make a list of a single item
                        push_result_rule(&parser, rule_src_line, rule, i);
                    } else {
                        // just leave single item on stack (ie don't wrap in a list)
                    }
                } else {
                    push_result_rule(&parser, rule_src_line, rule, i);
                }
                break;
            }

            default:
                assert(0);
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

    mp_obj_t exc;

    if (parser.parse_error) {
        #if MICROPY_COMP_CONST
        if (parser.parse_error == PARSE_ERROR_CONST) {
            exc = mp_obj_new_exception_msg(&mp_type_SyntaxError,
                "constant must be an integer");
        } else
        #endif
        {
            assert(parser.parse_error == PARSE_ERROR_MEMORY);
        memory_error:
            exc = mp_obj_new_exception_msg(&mp_type_MemoryError,
                "parser could not allocate enough memory");
        }
        parser.tree.root = MP_PARSE_NODE_NULL;
    } else if (
        lex->tok_kind != MP_TOKEN_END // check we are at the end of the token stream
        || parser.result_stack_top == 0 // check that we got a node (can fail on empty input)
        ) {
    syntax_error:
        if (lex->tok_kind == MP_TOKEN_INDENT) {
            exc = mp_obj_new_exception_msg(&mp_type_IndentationError,
                "unexpected indent");
        } else if (lex->tok_kind == MP_TOKEN_DEDENT_MISMATCH) {
            exc = mp_obj_new_exception_msg(&mp_type_IndentationError,
                "unindent does not match any outer indentation level");
        } else {
            exc = mp_obj_new_exception_msg(&mp_type_SyntaxError,
                "invalid syntax");
        }
        parser.tree.root = MP_PARSE_NODE_NULL;
    } else {
        // no errors

        //result_stack_show(parser);
        //printf("rule stack alloc: %d\n", parser.rule_stack_alloc);
        //printf("result stack alloc: %d\n", parser.result_stack_alloc);
        //printf("number of parse nodes allocated: %d\n", num_parse_nodes_allocated);

        // get the root parse node that we created
        assert(parser.result_stack_top == 1);
        exc = MP_OBJ_NULL;
        parser.tree.root = parser.result_stack[0];
    }

    // free the memory that we don't need anymore
    m_del(rule_stack_t, parser.rule_stack, parser.rule_stack_alloc);
    m_del(mp_parse_node_t, parser.result_stack, parser.result_stack_alloc);
    // we also free the lexer on behalf of the caller (see below)

    if (exc != MP_OBJ_NULL) {
        // had an error so raise the exception
        // add traceback to give info about file name and location
        // we don't have a 'block' name, so just pass the NULL qstr to indicate this
        mp_obj_exception_add_traceback(exc, lex->source_name, lex->tok_line, MP_QSTR_NULL);
        mp_lexer_free(lex);
        nlr_raise(exc);
    } else {
        mp_lexer_free(lex);
        return parser.tree;
    }
}

void mp_parse_tree_clear(mp_parse_tree_t *tree) {
    mp_parse_chunk_t *chunk = tree->chunk;
    while (chunk != NULL) {
        mp_parse_chunk_t *next = chunk->union_.next;
        m_del(byte, chunk, sizeof(mp_parse_chunk_t) + chunk->alloc);
        chunk = next;
    }
}

#endif // MICROPY_ENABLE_COMPILER
