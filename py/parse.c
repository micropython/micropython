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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

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
#define RULE_ARG_OPT_TOK        (0x3000)
#define RULE_ARG_OPT_RULE       (0x4000)

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
#define DEF_RULE(rule, comp, kind, ...) RULE_##rule,
#include "py/grammar.h"
#undef DEF_RULE
    RULE_maximum_number_of,
    RULE_string, // special node for non-interned string
    RULE_bytes, // special node for non-interned bytes
};

#define ident                   (RULE_ACT_ALLOW_IDENT)
#define blank                   (RULE_ACT_ADD_BLANK)
#define or(n)                   (RULE_ACT_OR | n)
#define and(n)                  (RULE_ACT_AND | n)
#define one_or_more             (RULE_ACT_LIST | 2)
#define list                    (RULE_ACT_LIST | 1)
#define list_with_end           (RULE_ACT_LIST | 3)
#define tok(t)                  (RULE_ARG_TOK | MP_TOKEN_##t)
#define rule(r)                 (RULE_ARG_RULE | RULE_##r)
#define opt_tok(t)              (RULE_ARG_OPT_TOK | MP_TOKEN_##t)
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
#undef opt_tok
#undef opt_rule
#undef one_or_more
#undef DEF_RULE

STATIC const rule_t *rules[] = {
#define DEF_RULE(rule, comp, kind, ...) &rule_##rule,
#include "py/grammar.h"
#undef DEF_RULE
};

typedef struct _rule_stack_t {
    mp_uint_t src_line : 24;
    mp_uint_t rule_id : 8;
    mp_uint_t arg_i : 32; // what should the bit-size be?
} rule_stack_t;

typedef struct _parser_t {
    bool had_memory_error;

    mp_uint_t rule_stack_alloc;
    mp_uint_t rule_stack_top;
    rule_stack_t *rule_stack;

    mp_uint_t result_stack_alloc;
    mp_uint_t result_stack_top;
    mp_parse_node_t *result_stack;

    mp_lexer_t *lexer;
} parser_t;

STATIC inline void memory_error(parser_t *parser) {
    parser->had_memory_error = true;
}

STATIC void push_rule(parser_t *parser, mp_uint_t src_line, const rule_t *rule, mp_uint_t arg_i) {
    if (parser->had_memory_error) {
        return;
    }
    if (parser->rule_stack_top >= parser->rule_stack_alloc) {
        rule_stack_t *rs = m_renew_maybe(rule_stack_t, parser->rule_stack, parser->rule_stack_alloc, parser->rule_stack_alloc + MICROPY_ALLOC_PARSE_RULE_INC);
        if (rs == NULL) {
            memory_error(parser);
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

STATIC void push_rule_from_arg(parser_t *parser, mp_uint_t arg) {
    assert((arg & RULE_ARG_KIND_MASK) == RULE_ARG_RULE || (arg & RULE_ARG_KIND_MASK) == RULE_ARG_OPT_RULE);
    mp_uint_t rule_id = arg & RULE_ARG_ARG_MASK;
    assert(rule_id < RULE_maximum_number_of);
    push_rule(parser, parser->lexer->tok_line, rules[rule_id], 0);
}

STATIC void pop_rule(parser_t *parser, const rule_t **rule, mp_uint_t *arg_i, mp_uint_t *src_line) {
    assert(!parser->had_memory_error);
    parser->rule_stack_top -= 1;
    *rule = rules[parser->rule_stack[parser->rule_stack_top].rule_id];
    *arg_i = parser->rule_stack[parser->rule_stack_top].arg_i;
    *src_line = parser->rule_stack[parser->rule_stack_top].src_line;
}

mp_parse_node_t mp_parse_node_new_leaf(mp_int_t kind, mp_int_t arg) {
    if (kind == MP_PARSE_NODE_SMALL_INT) {
        return (mp_parse_node_t)(kind | (arg << 1));
    }
    return (mp_parse_node_t)(kind | (arg << 5));
}

void mp_parse_node_free(mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
        mp_uint_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        mp_uint_t rule_id = MP_PARSE_NODE_STRUCT_KIND(pns);
        if (rule_id == RULE_string || rule_id == RULE_bytes) {
            m_del(char, (char*)pns->nodes[0], (mp_uint_t)pns->nodes[1]);
        } else {
            bool adjust = ADD_BLANK_NODE(rules[rule_id]);
            if (adjust) {
                n--;
            }
            for (mp_uint_t i = 0; i < n; i++) {
                mp_parse_node_free(pns->nodes[i]);
            }
            if (adjust) {
                n++;
            }
        }
        m_del_var(mp_parse_node_struct_t, mp_parse_node_t, n, pns);
    }
}

#if MICROPY_DEBUG_PRINTERS
void mp_parse_node_print(mp_parse_node_t pn, mp_uint_t indent) {
    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        printf("[% 4d] ", (int)((mp_parse_node_struct_t*)pn)->source_line);
    } else {
        printf("       ");
    }
    for (mp_uint_t i = 0; i < indent; i++) {
        printf(" ");
    }
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        printf("NULL\n");
    } else if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        mp_int_t arg = MP_PARSE_NODE_LEAF_SMALL_INT(pn);
        printf("int(" INT_FMT ")\n", arg);
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        mp_uint_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
        switch (MP_PARSE_NODE_LEAF_KIND(pn)) {
            case MP_PARSE_NODE_ID: printf("id(%s)\n", qstr_str(arg)); break;
            case MP_PARSE_NODE_INTEGER: printf("int(%s)\n", qstr_str(arg)); break;
            case MP_PARSE_NODE_DECIMAL: printf("dec(%s)\n", qstr_str(arg)); break;
            case MP_PARSE_NODE_STRING: printf("str(%s)\n", qstr_str(arg)); break;
            case MP_PARSE_NODE_BYTES: printf("bytes(%s)\n", qstr_str(arg)); break;
            case MP_PARSE_NODE_TOKEN: printf("tok(" INT_FMT ")\n", arg); break;
            default: assert(0);
        }
    } else {
        // node must be a mp_parse_node_struct_t
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_string) {
            printf("literal str(%.*s)\n", (int)pns->nodes[1], (char*)pns->nodes[0]);
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == RULE_bytes) {
            printf("literal bytes(%.*s)\n", (int)pns->nodes[1], (char*)pns->nodes[0]);
        } else {
            mp_uint_t n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
#ifdef USE_RULE_NAME
            printf("%s(" UINT_FMT ") (n=" UINT_FMT ")\n", rules[MP_PARSE_NODE_STRUCT_KIND(pns)]->rule_name, (mp_uint_t)MP_PARSE_NODE_STRUCT_KIND(pns), n);
#else
            printf("rule(" UINT_FMT ") (n=" UINT_FMT ")\n", (mp_uint_t)MP_PARSE_NODE_STRUCT_KIND(pns), n);
#endif
            for (mp_uint_t i = 0; i < n; i++) {
                mp_parse_node_print(pns->nodes[i], indent + 2);
            }
        }
    }
}
#endif // MICROPY_DEBUG_PRINTERS

/*
STATIC void result_stack_show(parser_t *parser) {
    printf("result stack, most recent first\n");
    for (mp_int_t i = parser->result_stack_top - 1; i >= 0; i--) {
        mp_parse_node_print(parser->result_stack[i], 0);
    }
}
*/

STATIC mp_parse_node_t pop_result(parser_t *parser) {
    if (parser->had_memory_error) {
        return MP_PARSE_NODE_NULL;
    }
    assert(parser->result_stack_top > 0);
    return parser->result_stack[--parser->result_stack_top];
}

STATIC mp_parse_node_t peek_result(parser_t *parser, mp_uint_t pos) {
    if (parser->had_memory_error) {
        return MP_PARSE_NODE_NULL;
    }
    assert(parser->result_stack_top > pos);
    return parser->result_stack[parser->result_stack_top - 1 - pos];
}

STATIC void push_result_node(parser_t *parser, mp_parse_node_t pn) {
    if (parser->had_memory_error) {
        return;
    }
    if (parser->result_stack_top >= parser->result_stack_alloc) {
        mp_parse_node_t *stack = m_renew_maybe(mp_parse_node_t, parser->result_stack, parser->result_stack_alloc, parser->result_stack_alloc + MICROPY_ALLOC_PARSE_RESULT_INC);
        if (stack == NULL) {
            memory_error(parser);
            return;
        }
        parser->result_stack = stack;
        parser->result_stack_alloc += MICROPY_ALLOC_PARSE_RESULT_INC;
    }
    parser->result_stack[parser->result_stack_top++] = pn;
}

STATIC void push_result_string_bytes(parser_t *parser, mp_uint_t src_line, mp_uint_t rule_kind, const char *str, mp_uint_t len) {
    mp_parse_node_struct_t *pn = m_new_obj_var_maybe(mp_parse_node_struct_t, mp_parse_node_t, 2);
    if (pn == NULL) {
        memory_error(parser);
        return;
    }
    pn->source_line = src_line;
    pn->kind_num_nodes = rule_kind | (2 << 8);
    char *p = m_new(char, len);
    memcpy(p, str, len);
    pn->nodes[0] = (mp_int_t)p;
    pn->nodes[1] = len;
    push_result_node(parser, (mp_parse_node_t)pn);
}

STATIC void push_result_token(parser_t *parser) {
    mp_parse_node_t pn;
    mp_lexer_t *lex = parser->lexer;
    if (lex->tok_kind == MP_TOKEN_NAME) {
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_ID, qstr_from_strn(lex->vstr.buf, lex->vstr.len));
    } else if (lex->tok_kind == MP_TOKEN_NUMBER) {
        bool dec = false;
        bool small_int = true;
        mp_int_t int_val = 0;
        mp_uint_t len = lex->vstr.len;
        const char *str = lex->vstr.buf;
        mp_uint_t base = 0;
        mp_uint_t i = mp_parse_num_base(str, len, &base);
        bool overflow = false;
        for (; i < len; i++) {
            mp_uint_t dig;
            int clower = str[i] | 0x20;
            if (unichar_isdigit(str[i]) && (mp_uint_t)(str[i] - '0') < base) {
                dig = str[i] - '0';
            } else if (base == 16 && 'a' <= clower && clower <= 'f') {
                dig = clower - 'a' + 10;
            } else if (str[i] == '.' || clower == 'e' || clower == 'j') {
                dec = true;
                break;
            } else {
                small_int = false;
                break;
            }
            // add next digi and check for overflow
            if (mp_small_int_mul_overflow(int_val, base)) {
                overflow = true;
            }
            int_val = int_val * base + dig;
            if (!MP_SMALL_INT_FITS(int_val)) {
                overflow = true;
            }
        }
        if (dec) {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_DECIMAL, qstr_from_strn(str, len));
        } else if (small_int && !overflow && MP_SMALL_INT_FITS(int_val)) {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, int_val);
        } else {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_INTEGER, qstr_from_strn(str, len));
        }
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
            push_result_string_bytes(parser, lex->tok_line, lex->tok_kind == MP_TOKEN_STRING ? RULE_string : RULE_bytes, lex->vstr.buf, lex->vstr.len);
            return;
        }
    } else {
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_TOKEN, lex->tok_kind);
    }
    push_result_node(parser, pn);
}

STATIC void push_result_rule(parser_t *parser, mp_uint_t src_line, const rule_t *rule, mp_uint_t num_args) {
    mp_parse_node_struct_t *pn = m_new_obj_var_maybe(mp_parse_node_struct_t, mp_parse_node_t, num_args);
    if (pn == NULL) {
        memory_error(parser);
        return;
    }
    pn->source_line = src_line;
    pn->kind_num_nodes = (rule->rule_id & 0xff) | (num_args << 8);
    for (mp_uint_t i = num_args; i > 0; i--) {
        pn->nodes[i - 1] = pop_result(parser);
    }
    push_result_node(parser, (mp_parse_node_t)pn);
}

mp_parse_node_t mp_parse(mp_lexer_t *lex, mp_parse_input_kind_t input_kind, mp_parse_error_kind_t *parse_error_kind_out) {

    // initialise parser and allocate memory for its stacks

    parser_t parser;

    parser.had_memory_error = false;

    parser.rule_stack_alloc = MICROPY_ALLOC_PARSE_RULE_INIT;
    parser.rule_stack_top = 0;
    parser.rule_stack = m_new_maybe(rule_stack_t, parser.rule_stack_alloc);

    parser.result_stack_alloc = MICROPY_ALLOC_PARSE_RESULT_INIT;
    parser.result_stack_top = 0;
    parser.result_stack = m_new_maybe(mp_parse_node_t, parser.result_stack_alloc);

    parser.lexer = lex;

    // check if we could allocate the stacks
    if (parser.rule_stack == NULL || parser.result_stack == NULL) {
        goto memory_error;
    }

    // work out the top-level rule to use, and push it on the stack
    mp_uint_t top_level_rule;
    switch (input_kind) {
        case MP_PARSE_SINGLE_INPUT: top_level_rule = RULE_single_input; break;
        case MP_PARSE_EVAL_INPUT: top_level_rule = RULE_eval_input; break;
        default: top_level_rule = RULE_file_input;
    }
    push_rule(&parser, lex->tok_line, rules[top_level_rule], 0);

    // parse!

    mp_uint_t n, i; // state for the current rule
    mp_uint_t rule_src_line; // source line for the first token matched by the current rule
    bool backtrack = false;
    const rule_t *rule = NULL;

    for (;;) {
        next_rule:
        if (parser.rule_stack_top == 0 || parser.had_memory_error) {
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
                for (; i < n - 1; ++i) {
                    switch (rule->arg[i] & RULE_ARG_KIND_MASK) {
                        case RULE_ARG_TOK:
                            if (lex->tok_kind == (rule->arg[i] & RULE_ARG_ARG_MASK)) {
                                push_result_token(&parser);
                                mp_lexer_to_next(lex);
                                goto next_rule;
                            }
                            break;
                        case RULE_ARG_RULE:
                        rule_or_no_other_choice:
                            push_rule(&parser, rule_src_line, rule, i + 1); // save this or-rule
                            push_rule_from_arg(&parser, rule->arg[i]); // push child of or-rule
                            goto next_rule;
                        default:
                            assert(0);
                            goto rule_or_no_other_choice; // to help flow control analysis
                    }
                }
                if ((rule->arg[i] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                    if (lex->tok_kind == (rule->arg[i] & RULE_ARG_ARG_MASK)) {
                        push_result_token(&parser);
                        mp_lexer_to_next(lex);
                    } else {
                        backtrack = true;
                        goto next_rule;
                    }
                } else {
                    push_rule_from_arg(&parser, rule->arg[i]);
                }
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
                                    push_result_token(&parser);
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

                // count number of arguments for the parse_node
                i = 0;
                bool emit_rule = false;
                for (mp_uint_t x = 0; x < n; ++x) {
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

#if !MICROPY_EMIT_CPYTHON && !MICROPY_ENABLE_DOC_STRING
                // this code discards lonely statements, such as doc strings
                if (input_kind != MP_PARSE_SINGLE_INPUT && rule->rule_id == RULE_expr_stmt && peek_result(&parser, 0) == MP_PARSE_NODE_NULL) {
                    mp_parse_node_t p = peek_result(&parser, 1);
                    if ((MP_PARSE_NODE_IS_LEAF(p) && !MP_PARSE_NODE_IS_ID(p)) || MP_PARSE_NODE_IS_STRUCT_KIND(p, RULE_string)) {
                        pop_result(&parser); // MP_PARSE_NODE_NULL
                        mp_parse_node_free(pop_result(&parser)); // RULE_string
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
                // NOTE: can't set this flag for atom_paren because we need it
                // to distinguish, for example, [a,b] from [(a,b)]
                // TODO possibly set for: varargslist_name, varargslist_equal
                if (rule->act & RULE_ACT_ALLOW_IDENT) {
                    emit_rule = false;
                }

                // always emit these rules, and add an extra blank node at the end (to be used by the compiler to store data)
                if (ADD_BLANK_NODE(rule)) {
                    emit_rule = true;
                    push_result_node(&parser, MP_PARSE_NODE_NULL);
                    i += 1;
                }

                mp_uint_t num_not_nil = 0;
                for (mp_uint_t x = 0; x < i; ++x) {
                    if (peek_result(&parser, x) != MP_PARSE_NODE_NULL) {
                        num_not_nil += 1;
                    }
                }
                //printf("done and %s n=%d i=%d notnil=%d\n", rule->rule_name, n, i, num_not_nil);
                if (emit_rule) {
                    push_result_rule(&parser, rule_src_line, rule, i);
                } else if (num_not_nil == 0) {
                    push_result_rule(&parser, rule_src_line, rule, i); // needed for, eg, atom_paren, testlist_comp_3b
                    //result_stack_show(parser);
                    //assert(0);
                } else if (num_not_nil == 1) {
                    // single result, leave it on stack
                    mp_parse_node_t pn = MP_PARSE_NODE_NULL;
                    for (mp_uint_t x = 0; x < i; ++x) {
                        mp_parse_node_t pn2 = pop_result(&parser);
                        if (pn2 != MP_PARSE_NODE_NULL) {
                            pn = pn2;
                        }
                    }
                    push_result_node(&parser, pn);
                } else {
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
                        mp_uint_t arg = rule->arg[i & 1 & n];
                        switch (arg & RULE_ARG_KIND_MASK) {
                            case RULE_ARG_TOK:
                                if (lex->tok_kind == (arg & RULE_ARG_ARG_MASK)) {
                                    if (i & 1 & n) {
                                        // separators which are tokens are not pushed to result stack
                                    } else {
                                        push_result_token(&parser);
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
                    //printf("done list %s %d %d\n", rule->rule_name, n, i);
                    push_result_rule(&parser, rule_src_line, rule, i);
                }
                break;
            }

            default:
                assert(0);
        }
    }

    mp_parse_node_t result;

    // check if we had a memory error
    if (parser.had_memory_error) {
memory_error:
        *parse_error_kind_out = MP_PARSE_ERROR_MEMORY;
        result = MP_PARSE_NODE_NULL;
        goto finished;

    }

    // check we are at the end of the token stream
    if (lex->tok_kind != MP_TOKEN_END) {
        goto syntax_error;
    }

    //printf("--------------\n");
    //result_stack_show(parser);
    //printf("rule stack alloc: %d\n", parser.rule_stack_alloc);
    //printf("result stack alloc: %d\n", parser.result_stack_alloc);
    //printf("number of parse nodes allocated: %d\n", num_parse_nodes_allocated);

    // get the root parse node that we created
    assert(parser.result_stack_top == 1);
    result = parser.result_stack[0];

finished:
    // free the memory that we don't need anymore
    m_del(rule_stack_t, parser.rule_stack, parser.rule_stack_alloc);
    m_del(mp_parse_node_t, parser.result_stack, parser.result_stack_alloc);

    // return the result
    return result;

syntax_error:
    if (lex->tok_kind == MP_TOKEN_INDENT) {
        *parse_error_kind_out = MP_PARSE_ERROR_UNEXPECTED_INDENT;
    } else if (lex->tok_kind == MP_TOKEN_DEDENT_MISMATCH) {
        *parse_error_kind_out = MP_PARSE_ERROR_UNMATCHED_UNINDENT;
    } else {
        *parse_error_kind_out = MP_PARSE_ERROR_INVALID_SYNTAX;
#ifdef USE_RULE_NAME
        // debugging: print the rule name that failed and the token
        printf("rule: %s\n", rule->rule_name);
#if MICROPY_DEBUG_PRINTERS
        mp_token_show(lex);
#endif
#endif
    }
    result = MP_PARSE_NODE_NULL;
    goto finished;
}
