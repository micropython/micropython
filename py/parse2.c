/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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
#include <assert.h>
#include <string.h>

#include "py/nlr.h"
#include "py/lexer.h"
#include "py/parse.h"
#include "py/parsenum.h"
#include "py/smallint.h"

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

#define ADD_BLANK_NODE(rule) ((rule->act & RULE_ACT_ADD_BLANK) != 0)

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
#define DEF_RULE_NC(rule, kind, ...) static const rule_t rule_##rule = { RULE_##rule, kind, #rule, { __VA_ARGS__ } };
#else
#define DEF_RULE(rule, comp, kind, ...) static const rule_t rule_##rule = { RULE_##rule, kind, { __VA_ARGS__ } };
#define DEF_RULE_NC(rule, kind, ...) static const rule_t rule_##rule = { RULE_##rule, kind, { __VA_ARGS__ } };
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
#undef DEF_RULE_NC

STATIC const rule_t *rules[] = {
// define rules with a compile function
#define DEF_RULE(rule, comp, kind, ...) &rule_##rule,
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
    NULL, // RULE_const_object

// define rules without a compile function
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) &rule_##rule,
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
};

typedef struct _rule_stack_t {
    size_t src_line : 8 * sizeof(size_t) - 8; // maximum bits storing source line number
    size_t rule_id : 8; // this must be large enough to fit largest rule number
    size_t arg_i; // this dictates the maximum nodes in a "list" of things
    size_t pt_off;
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

    mp_uint_t cur_scope_id;

    size_t co_alloc;
    size_t co_used;
    mp_uint_t *co_data;

    mp_lexer_t *lexer;

    mp_parse_tree_t tree;

    #if MICROPY_COMP_CONST
    mp_map_t consts;
    #endif
} parser_t;

STATIC void push_rule(parser_t *parser, size_t src_line, const rule_t *rule, size_t arg_i, size_t pt_off) {
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
    rs->pt_off = pt_off;
}

STATIC void push_rule_from_arg(parser_t *parser, size_t arg) {
    assert((arg & RULE_ARG_KIND_MASK) == RULE_ARG_RULE || (arg & RULE_ARG_KIND_MASK) == RULE_ARG_OPT_RULE);
    size_t rule_id = arg & RULE_ARG_ARG_MASK;
    push_rule(parser, parser->lexer->tok_line, rules[rule_id], 0, 0);
}

STATIC void pop_rule(parser_t *parser, const rule_t **rule, size_t *arg_i, size_t *src_line, size_t *pt_off) {
    assert(!parser->parse_error);
    parser->rule_stack_top -= 1;
    *rule = rules[parser->rule_stack[parser->rule_stack_top].rule_id];
    *arg_i = parser->rule_stack[parser->rule_stack_top].arg_i;
    *src_line = parser->rule_stack[parser->rule_stack_top].src_line;
    *pt_off = parser->rule_stack[parser->rule_stack_top].pt_off;
}

typedef struct _pt_t {
    vstr_t vv;
} pt_t;

STATIC pt_t *pt_new(void) {
    pt_t *pt = m_new_obj(pt_t);
    vstr_init(&pt->vv, 16);
    return pt;
}

STATIC byte *pt_raw_add_blank(pt_t *pt, size_t nbytes) {
    return (byte*)vstr_add_len(&pt->vv, nbytes);
}

STATIC byte *pt_raw_ins_blank(pt_t *pt, size_t pt_off, size_t nbytes) {
    return (byte*)vstr_ins_blank_bytes(&pt->vv, pt_off, nbytes);
}

STATIC void pt_raw_truncate_at(pt_t *pt, size_t pt_off) {
    pt->vv.len = pt_off;
}

STATIC int vuint_nbytes(size_t val) {
    int n = 0;
    do {
        n += 1;
        val >>= 7;
    } while (val != 0);
    return n;
}

STATIC void vuint_store(byte *p, int nbytes, size_t val) {
    p += nbytes;
    *--p = val & 0x7f;
    for (--nbytes; nbytes > 0; --nbytes) {
        val >>= 7;
        *--p = 0x80 | (val & 0x7f);
    }
}

STATIC size_t vuint_load(const byte **p_in) {
    const byte *p = *p_in;
    size_t val = 0;
    do {
        val = (val << 7) + (*p & 0x7f);
    } while ((*p++ & 0x80) != 0);
    *p_in = p;
    return val;
}

STATIC byte *pt_advance(const byte *p, bool full_rule) {
    switch (*p++) {
        case MP_PT_NULL:
            break;
        case MP_PT_TOKEN:
            p += 1;
            break;
        case MP_PT_SMALL_INT:
            p += BYTES_PER_WORD;
            break;
        case MP_PT_STRING:
            p += 2;
            break;
        case MP_PT_BYTES:
            p += 2;
            break;
        case MP_PT_CONST_OBJECT:
            vuint_load(&p);
            break;
        default:
            if (p[-1] < MP_PT_RULE_BASE) {
                // MP_PT_ID_BASE
                p += 1;
            } else {
                // MP_PT_RULE_BASE
                vuint_load(&p);
                uint32_t n = vuint_load(&p);
                if (full_rule) {
                    p += n;
                }
            }
            break;
    }
    return (byte*)p;
}

bool mp_parse_node_get_int_maybe(const byte *p, mp_obj_t *o) {
    if (pt_is_small_int(p)) {
        *o = MP_OBJ_NEW_SMALL_INT(pt_small_int_value(p));
        return true;
    #if 0 // TODO
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, RULE_const_object)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
        // nodes are 32-bit pointers, but need to extract 64-bit object
        *o = (uint64_t)pns->nodes[0] | ((uint64_t)pns->nodes[1] << 32);
        #else
        *o = (mp_obj_t)pns->nodes[0];
        #endif
        return MP_OBJ_IS_INT(*o);
    #endif
    } else {
        return false;
    }
}

// TODO this could perhaps allow *p to be null and in that case return null?
const byte *mp_parse_node_extract_list(const byte **p, size_t pn_kind) {
    if (pt_is_null(*p)) {
        *p += 1;
        return *p;
    } else if (!pt_is_any_rule(*p)) {
        return pt_advance(*p, true);
    } else {
        if (!pt_is_rule(*p, pn_kind)) {
            return pt_advance(*p, true);
        } else {
            const byte *ptop;
            *p = pt_rule_extract_top(*p, &ptop);
            return ptop;
        }
    }
}

/*
const byte *pt_extract_id(const byte *p, qstr *qst) {
    //assert(*p == MP_PT_ID_BASE);
    *qst = p[1] | ((p[0] - MP_PT_ID_BASE) << 8);
    return p + 2;
}
*/

const byte *pt_extract_const_obj(const byte *p, size_t *idx) {
    assert(*p == MP_PT_CONST_OBJECT);
    p += 1;
    *idx = vuint_load(&p);
    return p;
}

const byte *pt_get_small_int(const byte *p, mp_int_t *val) {
    assert(*p == MP_PT_SMALL_INT);
    *val = 0;
    for (size_t i = 0; i < BYTES_PER_WORD; i++) {
        *val |= (mp_int_t)*++p << (8 * i);
    }
    return p + 1;
}

mp_int_t pt_small_int_value(const byte *p) {
    mp_int_t val;
    pt_get_small_int(p, &val);
    return val;
}

int pt_num_nodes(const byte *p, const byte *ptop) {
    int n = 0;
    while (p < ptop) {
        n += 1;
        p = pt_advance(p, true);
    }
    return n;
}

const byte *pt_next(const byte *p) {
    return pt_advance(p, true);
}

const byte *pt_rule_first(const byte *p) {
    return pt_advance(p, false);
}

#if 0
void pt_show(pt_t *pt) {
    const byte *top = (byte*)pt->vv.buf + pt->vv.len;
    /*
    for (const byte *p = (byte*)pt->buf; p < top; ++p) {
        if ((p - (byte*)pt->buf) % 20 == 0) {
            printf("\n");
        }
        printf("%02x ", *p);
    }
    printf("\n");
    */
    for (const byte *p = (byte*)pt->vv.buf; p < top;) {
        printf("%04u  ", (uint)(p - (byte*)pt->vv.buf));
        const byte *p2 = pt_advance(p, false);
        for (const byte *p3 = p; p3 < p2; ++p3) {
            printf("%02x ", *p3);
        }
        for (int i = 8 - (p2 - p); i > 0; --i) {
            printf("   ");
        }
        switch (*p) {
            case MP_PT_NULL:
                printf("NULL\n");
                break;
            case MP_PT_TOKEN:
                printf("TOKEN %u\n", p[1]);
                break;
            case MP_PT_SMALL_INT:
                printf("SMALL_INT " INT_FMT "\n", pt_small_int_value(p));
                break;
            case MP_PT_STRING:
                printf("STRING %s\n", qstr_str(p[1] | (p[2] << 8)));
                break;
            case MP_PT_BYTES:
                printf("BYTES %s\n", qstr_str(p[1] | (p[2] << 8)));
                break;
            case MP_PT_CONST_OBJECT:
                printf("CONST_OBJECT\n");
                break;
            default:
                if (p[0] < MP_PT_RULE_BASE) {
                    // MP_PT_ID_BASE
                    printf("ID %s\n", qstr_str(p[1] | ((p[0] - MP_PT_ID_BASE) << 8)));
                } else {
                    // MP_PT_RULE_BASE
                    byte rule_id = p[0] - MP_PT_RULE_BASE;
                    const byte *p4 = p + 1;
                    uint32_t src_line = vuint_load(&p4);
                    uint32_t n = vuint_load(&p4);
                    #if USE_RULE_NAME
                    printf("RULE %s line=%u bytes=%u\n", rules[rule_id]->rule_name, src_line, n);
                    #else
                    printf("RULE %d line=%u bytes=%u\n", rule_id, src_line, n);
                    #endif
                }
                break;
        }
        p = p2;
    }
}
#endif

STATIC void pt_add_null(pt_t *pt) {
    *pt_raw_add_blank(pt, 1) = MP_PT_NULL;
}

STATIC void pt_add_kind_byte(pt_t *pt, byte kind, byte b) {
    byte *buf = pt_raw_add_blank(pt, 2);
    buf[0] = kind;
    buf[1] = b;
}

STATIC void pt_add_kind_qstr(pt_t *pt, byte kind, qstr qst) {
    if (kind == MP_PT_ID_BASE) {
        assert((qst >> 12) == 0);
        byte *buf = pt_raw_add_blank(pt, 2);
        buf[0] = MP_PT_ID_BASE + (qst >> 8);
        buf[1] = qst;
    } else {
        assert((qst >> 16) == 0);
        byte *buf = pt_raw_add_blank(pt, 3);
        buf[0] = kind;
        buf[1] = qst;
        buf[2] = qst >> 8;
    }
}

// valid for up to BYTES_PER_WORD=8
const byte pt_const_int0[] = {MP_PT_SMALL_INT, 0, 0, 0, 0, 0, 0, 0, 0};

STATIC void pt_add_kind_int(pt_t *pt, byte kind, mp_int_t val) {
    byte *buf = pt_raw_add_blank(pt, 1 + BYTES_PER_WORD);
    buf[0] = kind;
    for (size_t i = 0; i < BYTES_PER_WORD; ++i) {
        buf[i + 1] = val;
        val >>= 8;
    }
}

STATIC void pt_del_tail_bytes(pt_t *pt, size_t nbytes) {
    vstr_cut_tail_bytes(&pt->vv, nbytes);
}

STATIC const byte *pt_del_byte(pt_t *pt, const byte *p) {
    vstr_cut_out_bytes(&pt->vv, p - (byte*)pt->vv.buf, 1);
    return p;
}

#if MICROPY_COMP_MODULE_CONST
#include "py/builtin.h"
STATIC const mp_map_elem_t mp_constants_table[] = {
    #if MICROPY_PY_UCTYPES
    { MP_OBJ_NEW_QSTR(MP_QSTR_uctypes), (mp_obj_t)&mp_module_uctypes },
    #endif
    // Extra constants as defined by a port
    MICROPY_PORT_CONSTANTS
};
STATIC MP_DEFINE_CONST_MAP(mp_constants_map, mp_constants_table);
#endif

#if MICROPY_COMP_CONST_FOLDING
STATIC bool fold_constants(parser_t *parser, pt_t *pt, size_t pt_off, const rule_t *rule) {
    (void)parser;

    // this code does folding of arbitrary integer expressions, eg 1 + 2 * 3 + 4
    // it does not do partial folding, eg 1 + 2 + x -> 3 + x

    mp_int_t arg0;
    if (rule->rule_id == RULE_expr
        || rule->rule_id == RULE_xor_expr
        || rule->rule_id == RULE_and_expr) {
        // folding for binary ops: | ^ &
        const byte *p = (byte*)pt->vv.buf + pt_off;
        const byte *ptop = (byte*)pt->vv.buf + pt->vv.len;
        if (*p != MP_PT_SMALL_INT) {
            return false;
        }
        p = pt_get_small_int(p, &arg0);
        while (p != ptop) {
            if (*p != MP_PT_SMALL_INT) {
                return false;
            }
            mp_int_t arg1;
            p = pt_get_small_int(p, &arg1);
            if (rule->rule_id == RULE_expr) {
                // int | int
                arg0 |= arg1;
            } else if (rule->rule_id == RULE_xor_expr) {
                // int ^ int
                arg0 ^= arg1;
            } else if (rule->rule_id == RULE_and_expr) {
                // int & int
                arg0 &= arg1;
            }
        }
    } else if (rule->rule_id == RULE_shift_expr
        || rule->rule_id == RULE_arith_expr
        || rule->rule_id == RULE_term) {
        // folding for binary ops: << >> + - * / % //
        const byte *p = (byte*)pt->vv.buf + pt_off;
        const byte *ptop = (byte*)pt->vv.buf + pt->vv.len;
        if (*p != MP_PT_SMALL_INT) {
            return false;
        }
        p = pt_get_small_int(p, &arg0);
        while (p != ptop) {
            p += 1; // it's a token
            byte tok = *p++;
            if (*p != MP_PT_SMALL_INT) {
                return false;
            }
            mp_int_t arg1;
            p = pt_get_small_int(p, &arg1);
            if (tok == MP_TOKEN_OP_DBL_LESS) {
                // int << int
                if (arg1 >= (mp_int_t)BITS_PER_WORD
                    || arg0 > (MP_SMALL_INT_MAX >> arg1)
                    || arg0 < (MP_SMALL_INT_MIN >> arg1)) {
                    return false;
                }
                arg0 <<= arg1;
            } else if (tok == MP_TOKEN_OP_DBL_MORE) {
                // int >> int
                if (arg1 >= (mp_int_t)BITS_PER_WORD) {
                    // Shifting to big amounts is underfined behavior
                    // in C and is CPU-dependent; propagate sign bit.
                    arg1 = BITS_PER_WORD - 1;
                }
                arg0 >>= arg1;
            } else if (tok == MP_TOKEN_OP_PLUS) {
                // int + int
                arg0 += arg1;
            } else if (tok == MP_TOKEN_OP_MINUS) {
                // int - int
                arg0 -= arg1;
            } else if (tok == MP_TOKEN_OP_STAR) {
                // int * int
                if (mp_small_int_mul_overflow(arg0, arg1)) {
                    return false;
                }
                arg0 *= arg1;
            } else if (tok == MP_TOKEN_OP_SLASH) {
                // int / int
                return false;
            } else if (tok == MP_TOKEN_OP_PERCENT) {
                // int % int
                if (arg1 == 0) {
                    return false;
                }
                arg0 = mp_small_int_modulo(arg0, arg1);
            } else {
                assert(tok == MP_TOKEN_OP_DBL_SLASH); // should be
                // int // int
                if (arg1 == 0) {
                    return false;
                }
                arg0 = mp_small_int_floor_divide(arg0, arg1);
            }
            if (!MP_SMALL_INT_FITS(arg0)) {
                return false;
            }
        }
    } else if (rule->rule_id == RULE_factor_2) {
        // folding for unary ops: + - ~
        const byte *p = (byte*)pt->vv.buf + pt_off;
        p += 1; // it's a token
        byte tok = *p++;
        if (*p != MP_PT_SMALL_INT) {
            return false;
        }
        arg0 = pt_small_int_value(p);
        if (tok == MP_TOKEN_OP_PLUS) {
            // +int
        } else if (tok == MP_TOKEN_OP_MINUS) {
            // -int
            arg0 = -arg0;
            if (!MP_SMALL_INT_FITS(arg0)) {
                return false;
            }
        } else {
            assert(tok == MP_TOKEN_OP_TILDE); // should be
            // ~int
            arg0 = ~arg0;
        }

    #if 0&&MICROPY_COMP_CONST
    } else if (rule->rule_id == RULE_expr_stmt) {
        mp_parse_node_t pn1 = peek_result(parser, 0);
        if (!MP_PARSE_NODE_IS_NULL(pn1)
            && !(MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_expr_stmt_augassign)
            || MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_expr_stmt_assign_list))) {
            // this node is of the form <x> = <y>
            mp_parse_node_t pn0 = peek_result(parser, 1);
            if (MP_PARSE_NODE_IS_ID(pn0)
                && MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_power)
                && MP_PARSE_NODE_IS_ID(((mp_parse_node_struct_t*)pn1)->nodes[0])
                && MP_PARSE_NODE_LEAF_ARG(((mp_parse_node_struct_t*)pn1)->nodes[0]) == MP_QSTR_const
                && MP_PARSE_NODE_IS_STRUCT_KIND(((mp_parse_node_struct_t*)pn1)->nodes[1], RULE_trailer_paren)
                && MP_PARSE_NODE_IS_NULL(((mp_parse_node_struct_t*)pn1)->nodes[2])
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

                // replace const(value) with value
                pop_result(parser);
                push_result_node(parser, pn_value);

                // finished folding this assignment, but we still want it to be part of the tree
                return false;
            }
        }
        return false;
    #endif

    #if 0&&MICROPY_COMP_MODULE_CONST
    } else if (rule->rule_id == RULE_power) {
        mp_parse_node_t pn0 = peek_result(parser, 2);
        mp_parse_node_t pn1 = peek_result(parser, 1);
        mp_parse_node_t pn2 = peek_result(parser, 0);
        if (!(MP_PARSE_NODE_IS_ID(pn0)
            && MP_PARSE_NODE_IS_STRUCT_KIND(pn1, RULE_trailer_period)
            && MP_PARSE_NODE_IS_NULL(pn2))) {
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
        if (!(MP_OBJ_IS_SMALL_INT(dest[0]) && dest[1] == MP_OBJ_NULL)) {
            return false;
        }
        arg0 = MP_OBJ_SMALL_INT_VALUE(dest[0]);
    #endif

    } else {
        return false;
    }

    // success folding this rule

    pt_raw_truncate_at(pt, pt_off);
    pt_add_kind_int(pt, MP_PT_SMALL_INT, arg0);

    return true;
}
#endif

STATIC void pt_ins_rule(parser_t *parser, pt_t *pt, size_t pt_off, size_t src_line, const rule_t *rule, size_t num_args) {
    (void)num_args;

    // optimise away parenthesis around an expression if possible
    if (rule->rule_id == RULE_atom_paren) {
        // there should be just 1 arg for this rule
        const byte *p = (byte*)pt->vv.buf + pt_off;
        if (pt_is_null(p)) {
            // need to keep parenthesis for ()
        } else if (pt_is_rule(p, RULE_testlist_comp)) {
            // need to keep parenthesis for (a, b, ...)
        } else {
            // parenthesis around a single expression, so it's just the expression
            //printf("opt!\n");
            return;
        }
    }

    #if MICROPY_COMP_CONST_FOLDING
    if (fold_constants(parser, pt, pt_off, rule)) {
        // we folded this rule so return straight away
        return;
    }
    #endif

#if 0
    // TODO partial folding, eg 1 + 2 + x -> 3 + x
    mp_int_t arg0;
    if (rule->rule_id == RULE_expr
        || rule->rule_id == RULE_xor_expr
        || rule->rule_id == RULE_and_expr) {
        // combined node folding for these rules
        const byte *p = (byte*)pt->vv.buf + pt_off;
        const byte *ptop = (byte*)pt->vv.buf + pt->vv.len;
        if (*p != MP_PT_SMALL_INT) {
            goto folding_fail;
        }
        p = pt_get_small_int(p, &arg0);
        while (p != ptop) {
            if (*p != MP_PT_SMALL_INT) {
                goto folding_fail;
            }
            mp_int_t arg1;
            p = pt_get_small_int(p, &arg1);
            if (rule->rule_id == RULE_expr) {
                // int | int
                arg0 |= arg1;
            } else if (rule->rule_id == RULE_xor_expr) {
                // int ^ int
                arg0 ^= arg1;
            } else if (rule->rule_id == RULE_and_expr) {
                // int & int
                arg0 &= arg1;
            }
            if (!MP_SMALL_INT_FITS(arg0)) { // check needed?
                goto folding_fail;
            }
        }
    } else if (rule->rule_id == RULE_shift_expr
        || rule->rule_id == RULE_arith_expr
        || rule->rule_id == RULE_term) {
        // combined node folding for these rules
        const byte *p = (byte*)pt->vv.buf + pt_off;
        const byte *ptop = (byte*)pt->vv.buf + pt->vv.len;
        if (*p != MP_PT_SMALL_INT) {
            goto folding_fail;
        }
        p = pt_get_small_int(p, &arg0);
        while (p != ptop) {
            p += 1; // it's a token
            byte tok = *p++;
            if (*p != MP_PT_SMALL_INT) {
                goto folding_fail;
            }
            mp_int_t arg1;
            p = pt_get_small_int(p, &arg1);
            if (tok == MP_TOKEN_OP_DBL_LESS) {
                // int << int
                if (arg1 >= (mp_int_t)BITS_PER_WORD
                    || arg0 > (MP_SMALL_INT_MAX >> arg1)
                    || arg0 < (MP_SMALL_INT_MIN >> arg1)) {
                    goto folding_fail;
                }
                arg0 <<= arg1;
            } else if (tok == MP_TOKEN_OP_DBL_MORE) {
                // int >> int
                if (arg1 >= (mp_int_t)BITS_PER_WORD) {
                    // Shifting to big amounts is underfined behavior
                    // in C and is CPU-dependent; propagate sign bit.
                    arg1 = BITS_PER_WORD - 1;
                }
                arg0 >>= arg1;
            } else if (tok == MP_TOKEN_OP_PLUS) {
                // int + int
                arg0 += arg1;
            } else if (tok == MP_TOKEN_OP_MINUS) {
                // int - int
                arg0 -= arg1;
            } else if (tok == MP_TOKEN_OP_STAR) {
                // int * int
                if (mp_small_int_mul_overflow(arg0, arg1)) {
                    goto folding_fail;
                }
                arg0 *= arg1;
            } else if (tok == MP_TOKEN_OP_SLASH) {
                // int / int
                goto folding_fail;
            } else if (tok == MP_TOKEN_OP_PERCENT) {
                // int % int
                if (arg1 == 0) {
                    goto folding_fail;
                }
                arg0 = mp_small_int_modulo(arg0, arg1);
            } else {
                assert(tok == MP_TOKEN_OP_DBL_SLASH); // should be
                // int // int
                if (arg1 == 0) {
                    goto folding_fail;
                }
                arg0 = mp_small_int_floor_divide(arg0, arg1);
            }
            if (!MP_SMALL_INT_FITS(arg0)) {
                goto folding_fail;
            }
        }
    } else if (rule->rule_id == RULE_factor_2) {
        const byte *p = (byte*)pt->vv.buf + pt_off;
        p += 1; // it's a token
        byte tok = *p++;
        if (*p != MP_PT_SMALL_INT) {
            goto folding_fail;
        }
        arg0 = pt_small_int_value(p);
        if (tok == MP_TOKEN_OP_PLUS) {
            // +int
        } else if (tok == MP_TOKEN_OP_MINUS) {
            // -int
            arg0 = -arg0;
            if (!MP_SMALL_INT_FITS(arg0)) {
                goto folding_fail;
            }
        } else {
            assert(tok == MP_TOKEN_OP_TILDE); // should be
            // ~int
            arg0 = ~arg0;
        }
    } else {
        goto folding_fail;
    }

    // success folding this rule
    pt_raw_truncate_at(pt, pt_off);
    pt_add_kind_int(pt, MP_PT_SMALL_INT, arg0);
    return;

folding_fail:;
#endif

    int extra_node = 0;
    /*
    if (ADD_BLANK_NODE(rule)) {
        extra_node = 1 + BYTES_PER_WORD; // for small int node
    }
    */

    size_t nbytes = pt->vv.len + extra_node - pt_off;
    int nb1 = vuint_nbytes(src_line);
    int nb2 = vuint_nbytes(nbytes);
    byte *dest = (byte*)pt_raw_ins_blank(pt, pt_off, 1 + nb1 + nb2 + extra_node);
    dest[0] = MP_PT_RULE_BASE + rule->rule_id;
    vuint_store(dest + 1, nb1, src_line);
    vuint_store(dest + 1 + nb1, nb2, nbytes);

    // insert small int node for scope index
    if (extra_node != 0) {
        dest[1 + nb1 + nb2] = MP_PT_SMALL_INT;
        size_t val = ++parser->cur_scope_id;
        for (size_t i = 0; i < BYTES_PER_WORD; ++i) {
            dest[1 + nb1 + nb2 + 1 + i] = val;
            val >>= 8;
        }
    }
}

STATIC void make_node_const_object(parser_t *parser, pt_t *pt, mp_obj_t obj) {
    int nb = vuint_nbytes(parser->co_used);
    byte *buf = pt_raw_add_blank(pt, 1 + nb);
    buf[0] = MP_PT_CONST_OBJECT;
    vuint_store(buf + 1, nb, parser->co_used);
    if (parser->co_used >= parser->co_alloc) {
        // TODO use m_renew_maybe
        size_t alloc = parser->co_alloc + 8;
        parser->co_data = m_renew(mp_uint_t, parser->co_data, parser->co_alloc, alloc);
        parser->co_alloc = alloc;
    }
    parser->co_data[parser->co_used++] = (mp_uint_t)obj;
}

STATIC void make_node_string_bytes(parser_t *parser, pt_t *pt, mp_token_kind_t tok, const char *str, size_t len) {
    mp_obj_t o;
    if (tok == MP_TOKEN_STRING) {
        o = mp_obj_new_str(str, len, false);
    } else {
        o = mp_obj_new_bytes((const byte*)str, len);
    }
    make_node_const_object(parser, pt, o);
}

STATIC bool pt_add_token(parser_t *parser, pt_t *pt) {
    mp_lexer_t *lex = parser->lexer;
    if (lex->tok_kind == MP_TOKEN_NAME) {
        qstr id = qstr_from_strn(lex->vstr.buf, lex->vstr.len);
        #if MICROPY_COMP_CONST
        // lookup identifier in table of dynamic constants
        mp_map_elem_t *elem = mp_map_lookup(&parser->consts, MP_OBJ_NEW_QSTR(id), MP_MAP_LOOKUP);
        if (elem != NULL) {
            pt_add_kind_int(pt, MP_PT_SMALL_INT, MP_OBJ_SMALL_INT_VALUE(elem->value));
        } else
        #endif
        {
            pt_add_kind_qstr(pt, MP_PT_ID_BASE, id);
        }
    } else if (lex->tok_kind == MP_TOKEN_INTEGER) {
        mp_obj_t o = mp_parse_num_integer(lex->vstr.buf, lex->vstr.len, 0, lex);
        if (MP_OBJ_IS_SMALL_INT(o)) {
            pt_add_kind_int(pt, MP_PT_SMALL_INT, MP_OBJ_SMALL_INT_VALUE(o));
        } else {
            make_node_const_object(parser, pt, o);
        }
    } else if (lex->tok_kind == MP_TOKEN_FLOAT_OR_IMAG) {
        mp_obj_t o = mp_parse_num_decimal(lex->vstr.buf, lex->vstr.len, true, false, lex);
        make_node_const_object(parser, pt, o);
    } else if (lex->tok_kind == MP_TOKEN_STRING || lex->tok_kind == MP_TOKEN_BYTES) {
        // join adjacent string/bytes literals
        mp_token_kind_t tok_kind = lex->tok_kind;
        vstr_t vstr;
        vstr_init(&vstr, lex->vstr.len);
        do {
            vstr_add_strn(&vstr, lex->vstr.buf, lex->vstr.len);
            mp_lexer_to_next(lex);
        } while (lex->tok_kind == tok_kind);

        if (lex->tok_kind == MP_TOKEN_STRING || lex->tok_kind == MP_TOKEN_BYTES) {
            return false;
        }

        // Don't automatically intern all strings/bytes.  doc strings (which are usually large)
        // will be discarded by the compiler, and so we shouldn't intern them.
        qstr qst = MP_QSTR_NULL;
        if (vstr.len <= MICROPY_ALLOC_PARSE_INTERN_STRING_LEN) {
            // intern short strings
            qst = qstr_from_strn(vstr.buf, vstr.len);
        } else {
            // check if this string is already interned
            qst = qstr_find_strn(vstr.buf, vstr.len);
        }
        if (qst != MP_QSTR_NULL) {
            // qstr exists, make a leaf node
            pt_add_kind_qstr(pt, tok_kind == MP_TOKEN_STRING ? MP_PT_STRING : MP_PT_BYTES, qst);
        } else {
            // not interned, make a node holding a pointer to the string/bytes data
            make_node_string_bytes(parser, pt, tok_kind, vstr.buf, vstr.len);
        }
        vstr_clear(&vstr);
        return true;
    } else {
        pt_add_kind_byte(pt, MP_PT_TOKEN, lex->tok_kind);
    }
    mp_lexer_to_next(lex);
    return true;
}

const byte *pt_rule_extract_top(const byte *p, const byte **ptop) {
    assert(*p >= MP_PT_RULE_BASE);
    p++;
    vuint_load(&p);
    size_t nbytes = vuint_load(&p);
    *ptop = p + nbytes;
    return p;
}

const byte *pt_rule_extract(const byte *p, size_t *rule_id, size_t *src_line, const byte **ptop) {
    assert(*p >= MP_PT_RULE_BASE);
    *rule_id = *p++ - MP_PT_RULE_BASE;
    *src_line = vuint_load(&p);
    size_t nbytes = vuint_load(&p);
    *ptop = p + nbytes;
    return p;
}

bool pt_is_rule_empty(const byte *p) {
    const byte *ptop;
    p = pt_rule_extract_top(p, &ptop);
    return p == ptop;
}

mp_parse_tree_t mp_parse(mp_lexer_t *lex, mp_parse_input_kind_t input_kind) {

    // initialise parser and allocate memory for its stacks
    parser_t parser;

    parser.parse_error = PARSE_ERROR_NONE;

    parser.rule_stack_alloc = MICROPY_ALLOC_PARSE_RULE_INIT;
    parser.rule_stack_top = 0;
    parser.rule_stack = m_new_maybe(rule_stack_t, parser.rule_stack_alloc);

    parser.cur_scope_id = 0;

    parser.co_alloc = 0;
    parser.co_used = 0;
    parser.co_data = NULL;

    parser.lexer = lex;

    parser.tree.chunk = NULL;

    #if MICROPY_COMP_CONST
    mp_map_init(&parser.consts, 0);
    #endif

    // check if we could allocate the stacks
    if (parser.rule_stack == NULL) {
        goto memory_error;
    }

    // work out the top-level rule to use, and push it on the stack
    size_t top_level_rule;
    switch (input_kind) {
        case MP_PARSE_SINGLE_INPUT: top_level_rule = RULE_single_input; break;
        case MP_PARSE_EVAL_INPUT: top_level_rule = RULE_eval_input; break;
        default: top_level_rule = RULE_file_input;
    }
    push_rule(&parser, lex->tok_line, rules[top_level_rule], 0, 0);

    // parse!

    size_t n, i; // state for the current rule
    size_t pt_off = 0; // state for the current rule
    size_t rule_src_line; // source line for the first token matched by the current rule
    bool backtrack = false;
    const rule_t *rule = NULL;
    pt_t *pt = pt_new();

    for (;;) {
        next_rule:
        if (parser.rule_stack_top == 0 || parser.parse_error) {
            break;
        }

        pop_rule(&parser, &rule, &i, &rule_src_line, &pt_off);
        n = rule->act & RULE_ACT_ARG_MASK;

        if (i == 0) {
            pt_off = pt->vv.len;
        }

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
                            if (!pt_add_token(&parser, pt)) {
                                goto syntax_error;
                            }
                            goto next_rule;
                        }
                    } else {
                        assert(kind == RULE_ARG_RULE);
                        if (i + 1 < n) {
                            push_rule(&parser, rule_src_line, rule, i + 1, pt_off); // save this or-rule
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
                        pt_add_null(pt);
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
                                    pt_add_kind_qstr(pt, MP_PT_ID_BASE, qstr_from_strn(lex->vstr.buf, lex->vstr.len));
                                }
                                if (i == 0 && ADD_BLANK_NODE(rule)) {
                                    pt_add_kind_int(pt, MP_PT_SMALL_INT, ++parser.cur_scope_id);
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
                        default:
                            push_rule(&parser, rule_src_line, rule, i + 1, pt_off); // save this and-rule
                            push_rule_from_arg(&parser, rule->arg[i]); // push child of and-rule
                            goto next_rule;
                    }
                }

                assert(i == n);

                // matched the rule, so now build the corresponding parse_node

                // count number of arguments for the parse_node
                i = 0;
                bool emit_rule = false;
                /*
                for (size_t x = 0; x < n; ++x) {
                    if ((rule->arg[x] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                        mp_token_kind_t tok_kind = rule->arg[x] & RULE_ARG_ARG_MASK;
                        if (tok_kind >= MP_TOKEN_NAME) {
                            emit_rule = true;
                        }
                        if (tok_kind == MP_TOKEN_NAME) {
                            // only tokens which were names are pushed to stack
                            i += 1;
                        }
                    } else {
                        // rules are always pushed
                        i += 1;
                    }
                }
                */
                for (size_t x = 0; x < n; ++x) {
                    if ((rule->arg[x] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                        mp_token_kind_t tok_kind = rule->arg[x] & RULE_ARG_ARG_MASK;
                        if (tok_kind >= MP_TOKEN_NAME) {
                            emit_rule = true;
                        }
                    }
                }
                for (const byte *p = (byte*)pt->vv.buf + pt_off; p < (byte*)pt->vv.buf + pt->vv.len;) {
                    i += 1;
                    p = pt_advance(p, true);
                }

                #if 0 && !MICROPY_ENABLE_DOC_STRING
                // this code discards lonely statements, such as doc strings
                if (input_kind != MP_PARSE_SINGLE_INPUT && rule->rule_id == RULE_expr_stmt && peek_result(&parser, 0) == MP_PARSE_NODE_NULL) {
                    mp_parse_node_t p = peek_result(&parser, 1);
                    if ((MP_PARSE_NODE_IS_LEAF(p) && !MP_PARSE_NODE_IS_ID(p)) || MP_PARSE_NODE_IS_STRUCT_KIND(p, RULE_string)) {
                        pop_result(&parser); // MP_PARSE_NODE_NULL
                        mp_parse_node_t pn = pop_result(&parser); // possibly RULE_string
                        if (MP_PARSE_NODE_IS_STRUCT(pn)) {
                            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
                            if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_string) {
                                m_del(char, (char*)pns->nodes[0], (mp_uint_t)pns->nodes[1]);
                            }
                        }
                        push_result_rule(&parser, rule_src_line, rules[RULE_pass_stmt], 0);
                        break;
                    }
                }
                #endif

                // always emit these rules, even if they have only 1 argument
                if (rule->rule_id == RULE_expr_stmt || rule->rule_id == RULE_yield_stmt) {
                    emit_rule = true;
                }

                // if a rule has the RULE_ACT_ALLOW_IDENT bit set then this
                // rule should not be emitted if it has only 1 argument
                if (rule->act & RULE_ACT_ALLOW_IDENT) {
                    emit_rule = false;
                }

                // always emit these rules, and add an extra blank node at the end (to be used by the compiler to store data)
                if (ADD_BLANK_NODE(rule)) {
                    emit_rule = true;
                    // TODO
                    //add_result_node(&parser, MP_PARSE_NODE_NULL);
                    //i += 1;
                }

                // count number of non-null nodes
                size_t num_not_null = 0;
                size_t num_trail_null = 0;
                { const byte *p = (byte*)pt->vv.buf + pt_off;
                for (size_t x = 0; x < i; ++x) {
                    if (*p != MP_PT_NULL) {
                        num_not_null += 1;
                        num_trail_null = 0;
                    } else {
                        num_trail_null += 1;
                    }
                    p = pt_advance(p, true);
                }}

                if (emit_rule || num_not_null != 1) {
                    // need to add rule when num_not_null == 0 for, eg, atom_paren, testlist_comp_3b
                    pt_del_tail_bytes(pt, num_trail_null); // remove trailing null nodes, they are store implicitly
                    pt_ins_rule(&parser, pt, pt_off, rule_src_line, rule, i - num_trail_null);
                } else {
                    // single result, leave it on stack
                    const byte *p = (byte*)pt->vv.buf + pt_off;
                    for (size_t x = 0; x < i; ++x) {
                        if (*p == MP_PT_NULL) {
                            p = pt_del_byte(pt, p);
                        } else {
                            p = pt_advance(p, true);
                        }
                    }
                }
                break;
            }

            case RULE_ACT_LIST:
            default: // nothing else
            {
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
                                        mp_lexer_to_next(lex);
                                    } else {
                                        pt_add_token(&parser, pt);
                                    }
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
                                push_rule(&parser, rule_src_line, rule, i + 1, pt_off); // save this list-rule
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
                        pt_ins_rule(&parser, pt, pt_off, rule_src_line, rule, i);
                    } else {
                        // just leave single item on stack (ie don't wrap in a list)
                    }
                } else {
                    pt_ins_rule(&parser, pt, pt_off, rule_src_line, rule, i);
                }
                break;
            }
        }
    }

    #if MICROPY_COMP_CONST
    mp_map_deinit(&parser.consts);
    #endif

    #if 0
    pt_show(pt);

    {
        size_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        printf("qstr pool: n_pool=" UINT_FMT ", n_qstr=" UINT_FMT ", n_str_data_bytes="
            UINT_FMT ", n_total_bytes=" UINT_FMT "\n",
            n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }
    #endif

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
        parser.tree.root = NULL;
    } else if (
        lex->tok_kind != MP_TOKEN_END // check we are at the end of the token stream
        || pt->vv.len == 0 // check that we got a node (can fail on empty input)
        ) {
    syntax_error:
        if (lex->tok_kind == MP_TOKEN_INDENT) {
            exc = mp_obj_new_exception_msg(&mp_type_IndentationError,
                "unexpected indent");
        } else if (lex->tok_kind == MP_TOKEN_DEDENT_MISMATCH) {
            exc = mp_obj_new_exception_msg(&mp_type_IndentationError,
                "unindent does not match any outer indentation level");
        } else if (lex->tok_kind == MP_TOKEN_STRING || lex->tok_kind == MP_TOKEN_BYTES) {
            exc = mp_obj_new_exception_msg(&mp_type_SyntaxError,
                "cannot mix bytes and nonbytes literals");
        } else {
            exc = mp_obj_new_exception_msg(&mp_type_SyntaxError,
                "invalid syntax");
        }
        parser.tree.root = NULL;
    } else {
        // no errors

        //result_stack_show(parser);
        //printf("rule stack alloc: %d\n", parser.rule_stack_alloc);
        //printf("result stack alloc: %d\n", parser.result_stack_alloc);
        //printf("number of parse nodes allocated: %d\n", num_parse_nodes_allocated);

        // add number of scopes
        pt_add_kind_int(pt, MP_PT_SMALL_INT, parser.cur_scope_id + 1);

        // get the root parse node that we created
        //assert(parser.result_stack_top == 1);
        exc = MP_OBJ_NULL;
        parser.tree.root = (byte*)pt->vv.buf;
        parser.tree.co_data = parser.co_data;
    }

    // free the memory that we don't need anymore
    m_del(rule_stack_t, parser.rule_stack, parser.rule_stack_alloc);
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
