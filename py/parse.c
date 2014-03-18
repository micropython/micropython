#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "parsenumbase.h"
#include "parse.h"

#define RULE_ACT_KIND_MASK      (0xf0)
#define RULE_ACT_ARG_MASK       (0x0f)
#define RULE_ACT_OR             (0x10)
#define RULE_ACT_AND            (0x20)
#define RULE_ACT_LIST           (0x30)

#define RULE_ARG_BLANK          (0x0000)
#define RULE_ARG_KIND_MASK      (0xf000)
#define RULE_ARG_ARG_MASK       (0x0fff)
#define RULE_ARG_TOK            (0x1000)
#define RULE_ARG_RULE           (0x2000)
#define RULE_ARG_OPT_TOK        (0x3000)
#define RULE_ARG_OPT_RULE       (0x4000)

#define ADD_BLANK_NODE(rule_id) ((rule_id) == RULE_funcdef || (rule_id) == RULE_classdef || (rule_id) == RULE_comp_for || (rule_id) == RULE_lambdef || (rule_id) == RULE_lambdef_nocond)

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
    RULE_none = 0,
#define DEF_RULE(rule, comp, kind, ...) RULE_##rule,
#include "grammar.h"
#undef DEF_RULE
    RULE_maximum_number_of,
};

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
#include "grammar.h"
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
    NULL,
#define DEF_RULE(rule, comp, kind, ...) &rule_##rule,
#include "grammar.h"
#undef DEF_RULE
};

typedef struct _rule_stack_t {
    unsigned int src_line : 24;
    unsigned int rule_id : 8;
    int32_t arg_i; // what should be the size and signedness?
} rule_stack_t;

typedef struct _parser_t {
    uint rule_stack_alloc;
    uint rule_stack_top;
    rule_stack_t *rule_stack;

    uint result_stack_alloc;
    uint result_stack_top;
    mp_parse_node_t *result_stack;

    mp_lexer_t *lexer;
} parser_t;

STATIC void push_rule(parser_t *parser, int src_line, const rule_t *rule, int arg_i) {
    if (parser->rule_stack_top >= parser->rule_stack_alloc) {
        parser->rule_stack = m_renew(rule_stack_t, parser->rule_stack, parser->rule_stack_alloc, parser->rule_stack_alloc * 2);
        parser->rule_stack_alloc *= 2;
    }
    rule_stack_t *rs = &parser->rule_stack[parser->rule_stack_top++];
    rs->src_line = src_line;
    rs->rule_id = rule->rule_id;
    rs->arg_i = arg_i;
}

STATIC void push_rule_from_arg(parser_t *parser, uint arg) {
    assert((arg & RULE_ARG_KIND_MASK) == RULE_ARG_RULE || (arg & RULE_ARG_KIND_MASK) == RULE_ARG_OPT_RULE);
    uint rule_id = arg & RULE_ARG_ARG_MASK;
    assert(rule_id < RULE_maximum_number_of);
    push_rule(parser, mp_lexer_cur(parser->lexer)->src_line, rules[rule_id], 0);
}

STATIC void pop_rule(parser_t *parser, const rule_t **rule, uint *arg_i, uint *src_line) {
    parser->rule_stack_top -= 1;
    *rule = rules[parser->rule_stack[parser->rule_stack_top].rule_id];
    *arg_i = parser->rule_stack[parser->rule_stack_top].arg_i;
    *src_line = parser->rule_stack[parser->rule_stack_top].src_line;
}

mp_parse_node_t mp_parse_node_new_leaf(machine_int_t kind, machine_int_t arg) {
    if (kind == MP_PARSE_NODE_SMALL_INT) {
        return (mp_parse_node_t)(kind | (arg << 1));
    }
    return (mp_parse_node_t)(kind | (arg << 5));
}

//int num_parse_nodes_allocated = 0;
mp_parse_node_struct_t *parse_node_new_struct(int src_line, int rule_id, int num_args) {
    mp_parse_node_struct_t *pn = m_new_obj_var(mp_parse_node_struct_t, mp_parse_node_t, num_args);
    pn->source_line = src_line;
    pn->kind_num_nodes = (rule_id & 0xff) | (num_args << 8);
    //num_parse_nodes_allocated += 1;
    return pn;
}

uint mp_parse_node_free(mp_parse_node_t pn) {
    uint cnt = 0;
    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t *)pn;
        uint n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        uint rule_id = MP_PARSE_NODE_STRUCT_KIND(pns);
        bool adjust = ADD_BLANK_NODE(rule_id);
        if (adjust) {
            n--;
        }
        for (uint i = 0; i < n; i++) {
            cnt += mp_parse_node_free(pns->nodes[i]);
        }
        if (adjust) {
            n++;
        }
        m_del_var(mp_parse_node_struct_t, mp_parse_node_t, n, pns);
        cnt++;
    }
    return cnt;
}

#if MICROPY_DEBUG_PRINTERS
void mp_parse_node_print(mp_parse_node_t pn, int indent) {
    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        printf("[% 4d] ", (int)((mp_parse_node_struct_t*)pn)->source_line);
    } else {
        printf("       ");
    }
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        printf("NULL\n");
    } else if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        machine_int_t arg = MP_PARSE_NODE_LEAF_SMALL_INT(pn);
        printf("int(" INT_FMT ")\n", arg);
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        machine_uint_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
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
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        uint n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
#ifdef USE_RULE_NAME
        printf("%s(%d) (n=%d)\n", rules[MP_PARSE_NODE_STRUCT_KIND(pns)]->rule_name, MP_PARSE_NODE_STRUCT_KIND(pns), n);
#else
        printf("rule(%u) (n=%d)\n", (uint)MP_PARSE_NODE_STRUCT_KIND(pns), n);
#endif
        for (uint i = 0; i < n; i++) {
            mp_parse_node_print(pns->nodes[i], indent + 2);
        }
    }
}
#endif // MICROPY_DEBUG_PRINTERS

/*
STATIC void result_stack_show(parser_t *parser) {
    printf("result stack, most recent first\n");
    for (int i = parser->result_stack_top - 1; i >= 0; i--) {
        mp_parse_node_print(parser->result_stack[i], 0);
    }
}
*/

STATIC mp_parse_node_t pop_result(parser_t *parser) {
    assert(parser->result_stack_top > 0);
    return parser->result_stack[--parser->result_stack_top];
}

STATIC mp_parse_node_t peek_result(parser_t *parser, int pos) {
    assert(parser->result_stack_top > pos);
    return parser->result_stack[parser->result_stack_top - 1 - pos];
}

STATIC void push_result_node(parser_t *parser, mp_parse_node_t pn) {
    if (parser->result_stack_top >= parser->result_stack_alloc) {
        parser->result_stack = m_renew(mp_parse_node_t, parser->result_stack, parser->result_stack_alloc, parser->result_stack_alloc * 2);
        parser->result_stack_alloc *= 2;
    }
    parser->result_stack[parser->result_stack_top++] = pn;
}

STATIC void push_result_token(parser_t *parser, const mp_lexer_t *lex) {
    const mp_token_t *tok = mp_lexer_cur(lex);
    mp_parse_node_t pn;
    if (tok->kind == MP_TOKEN_NAME) {
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_ID, qstr_from_strn(tok->str, tok->len));
    } else if (tok->kind == MP_TOKEN_NUMBER) {
        bool dec = false;
        bool small_int = true;
        machine_int_t int_val = 0;
        int len = tok->len;
        const char *str = tok->str;
        int base = 0;
        int i = mp_parse_num_base(str, len, &base);
        bool overflow = false;
        for (; i < len; i++) {
            machine_int_t old_val = int_val;
            if (unichar_isdigit(str[i]) && str[i] - '0' < base) {
                int_val = base * int_val + str[i] - '0';
            } else if (base == 16 && 'a' <= str[i] && str[i] <= 'f') {
                int_val = base * int_val + str[i] - 'a' + 10;
            } else if (base == 16 && 'A' <= str[i] && str[i] <= 'F') {
                int_val = base * int_val + str[i] - 'A' + 10;
            } else if (str[i] == '.' || str[i] == 'e' || str[i] == 'E' || str[i] == 'j' || str[i] == 'J') {
                dec = true;
                break;
            } else {
                small_int = false;
                break;
            }
            if (int_val < old_val) {
                // If new value became less than previous, it's overflow
                overflow = true;
            } else if ((old_val ^ int_val) & WORD_MSBIT_HIGH) {
                // If signed number changed sign - it's overflow
                overflow = true;
            }
        }
        if (dec) {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_DECIMAL, qstr_from_strn(str, len));
        } else if (small_int && !overflow && MP_PARSE_FITS_SMALL_INT(int_val)) {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, int_val);
        } else {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_INTEGER, qstr_from_strn(str, len));
        }
    } else if (tok->kind == MP_TOKEN_STRING) {
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_STRING, qstr_from_strn(tok->str, tok->len));
    } else if (tok->kind == MP_TOKEN_BYTES) {
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_BYTES, qstr_from_strn(tok->str, tok->len));
    } else {
        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_TOKEN, tok->kind);
    }
    push_result_node(parser, pn);
}

STATIC void push_result_rule(parser_t *parser, int src_line, const rule_t *rule, int num_args) {
    mp_parse_node_struct_t *pn = parse_node_new_struct(src_line, rule->rule_id, num_args);
    for (int i = num_args; i > 0; i--) {
        pn->nodes[i - 1] = pop_result(parser);
    }
    push_result_node(parser, (mp_parse_node_t)pn);
}

mp_parse_node_t mp_parse(mp_lexer_t *lex, mp_parse_input_kind_t input_kind, mp_parse_error_kind_t *parse_error_kind_out) {

    // allocate memory for the parser and its stacks

    parser_t *parser = m_new_obj(parser_t);

    parser->rule_stack_alloc = 64;
    parser->rule_stack_top = 0;
    parser->rule_stack = m_new(rule_stack_t, parser->rule_stack_alloc);

    parser->result_stack_alloc = 64;
    parser->result_stack_top = 0;
    parser->result_stack = m_new(mp_parse_node_t, parser->result_stack_alloc);

    parser->lexer = lex;

    // work out the top-level rule to use, and push it on the stack
    int top_level_rule;
    switch (input_kind) {
        case MP_PARSE_SINGLE_INPUT: top_level_rule = RULE_single_input; break;
        case MP_PARSE_EVAL_INPUT: top_level_rule = RULE_eval_input; break;
        default: top_level_rule = RULE_file_input;
    }
    push_rule(parser, mp_lexer_cur(lex)->src_line, rules[top_level_rule], 0);

    // parse!

    uint n, i; // state for the current rule
    uint rule_src_line; // source line for the first token matched by the current rule
    bool backtrack = false;
    const rule_t *rule = NULL;
    mp_token_kind_t tok_kind;
    bool emit_rule;
    bool had_trailing_sep;

    for (;;) {
        next_rule:
        if (parser->rule_stack_top == 0) {
            break;
        }

        pop_rule(parser, &rule, &i, &rule_src_line);
        n = rule->act & RULE_ACT_ARG_MASK;

        /*
        // debugging
        printf("depth=%d ", parser->rule_stack_top);
        for (int j = 0; j < parser->rule_stack_top; ++j) {
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
                            if (mp_lexer_is_kind(lex, rule->arg[i] & RULE_ARG_ARG_MASK)) {
                                push_result_token(parser, lex);
                                mp_lexer_to_next(lex);
                                goto next_rule;
                            }
                            break;
                        case RULE_ARG_RULE:
                            push_rule(parser, rule_src_line, rule, i + 1); // save this or-rule
                            push_rule_from_arg(parser, rule->arg[i]); // push child of or-rule
                            goto next_rule;
                        default:
                            assert(0);
                    }
                }
                if ((rule->arg[i] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                    if (mp_lexer_is_kind(lex, rule->arg[i] & RULE_ARG_ARG_MASK)) {
                        push_result_token(parser, lex);
                        mp_lexer_to_next(lex);
                    } else {
                        backtrack = true;
                        goto next_rule;
                    }
                } else {
                    push_rule_from_arg(parser, rule->arg[i]);
                }
                break;

            case RULE_ACT_AND:

                // failed, backtrack if we can, else syntax error
                if (backtrack) {
                    assert(i > 0);
                    if ((rule->arg[i - 1] & RULE_ARG_KIND_MASK) == RULE_ARG_OPT_RULE) {
                        // an optional rule that failed, so continue with next arg
                        push_result_node(parser, MP_PARSE_NODE_NULL);
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
                        case RULE_ARG_TOK:
                            // need to match a token
                            tok_kind = rule->arg[i] & RULE_ARG_ARG_MASK;
                            if (mp_lexer_is_kind(lex, tok_kind)) {
                                // matched token
                                if (tok_kind == MP_TOKEN_NAME) {
                                    push_result_token(parser, lex);
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
                        case RULE_ARG_RULE:
                        case RULE_ARG_OPT_RULE:
                            push_rule(parser, rule_src_line, rule, i + 1); // save this and-rule
                            push_rule_from_arg(parser, rule->arg[i]); // push child of and-rule
                            goto next_rule;
                        default:
                            assert(0);
                    }
                }

                assert(i == n);

                // matched the rule, so now build the corresponding parse_node

                // count number of arguments for the parse_node
                i = 0;
                emit_rule = false;
                for (int x = 0; x < n; ++x) {
                    if ((rule->arg[x] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                        tok_kind = rule->arg[x] & RULE_ARG_ARG_MASK;
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

                // always emit these rules, even if they have only 1 argument
                if (rule->rule_id == RULE_expr_stmt || rule->rule_id == RULE_yield_stmt) {
                    emit_rule = true;
                }

                // never emit these rules if they have only 1 argument
                // NOTE: can't put atom_paren here because we need it to distinguisg, for example, [a,b] from [(a,b)]
                // TODO possibly put varargslist_name, varargslist_equal here as well
                if (rule->rule_id == RULE_else_stmt || rule->rule_id == RULE_testlist_comp_3b || rule->rule_id == RULE_import_as_names_paren || rule->rule_id == RULE_typedargslist_name || rule->rule_id == RULE_typedargslist_colon || rule->rule_id == RULE_typedargslist_equal || rule->rule_id == RULE_dictorsetmaker_colon || rule->rule_id == RULE_classdef_2 || rule->rule_id == RULE_with_item_as || rule->rule_id == RULE_assert_stmt_extra || rule->rule_id == RULE_as_name || rule->rule_id == RULE_raise_stmt_from || rule->rule_id == RULE_vfpdef) {
                    emit_rule = false;
                }

                // always emit these rules, and add an extra blank node at the end (to be used by the compiler to store data)
                if (ADD_BLANK_NODE(rule->rule_id)) {
                    emit_rule = true;
                    push_result_node(parser, MP_PARSE_NODE_NULL);
                    i += 1;
                }

                int num_not_nil = 0;
                for (int x = 0; x < i; ++x) {
                    if (peek_result(parser, x) != MP_PARSE_NODE_NULL) {
                        num_not_nil += 1;
                    }
                }
                //printf("done and %s n=%d i=%d notnil=%d\n", rule->rule_name, n, i, num_not_nil);
                if (emit_rule) {
                    push_result_rule(parser, rule_src_line, rule, i);
                } else if (num_not_nil == 0) {
                    push_result_rule(parser, rule_src_line, rule, i); // needed for, eg, atom_paren, testlist_comp_3b
                    //result_stack_show(parser);
                    //assert(0);
                } else if (num_not_nil == 1) {
                    // single result, leave it on stack
                    mp_parse_node_t pn = MP_PARSE_NODE_NULL;
                    for (int x = 0; x < i; ++x) {
                        mp_parse_node_t pn2 = pop_result(parser);
                        if (pn2 != MP_PARSE_NODE_NULL) {
                            pn = pn2;
                        }
                    }
                    push_result_node(parser, pn);
                } else {
                    push_result_rule(parser, rule_src_line, rule, i);
                }
                break;

            case RULE_ACT_LIST:
                // n=2 is: item item*
                // n=1 is: item (sep item)*
                // n=3 is: item (sep item)* [sep]
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
                        uint arg = rule->arg[i & 1 & n];
                        switch (arg & RULE_ARG_KIND_MASK) {
                            case RULE_ARG_TOK:
                                if (mp_lexer_is_kind(lex, arg & RULE_ARG_ARG_MASK)) {
                                    if (i & 1 & n) {
                                        // separators which are tokens are not pushed to result stack
                                    } else {
                                        push_result_token(parser, lex);
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
                                push_rule(parser, rule_src_line, rule, i + 1); // save this list-rule
                                push_rule_from_arg(parser, arg); // push child of list-rule
                                goto next_rule;
                            default:
                                assert(0);
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
                        push_result_rule(parser, rule_src_line, rule, i);
                    } else {
                        // just leave single item on stack (ie don't wrap in a list)
                    }
                } else {
                    //printf("done list %s %d %d\n", rule->rule_name, n, i);
                    push_result_rule(parser, rule_src_line, rule, i);
                }
                break;

            default:
                assert(0);
        }
    }

    // check we are at the end of the token stream
    if (!mp_lexer_is_kind(lex, MP_TOKEN_END)) {
        goto syntax_error;
    }

    //printf("--------------\n");
    //result_stack_show(parser);
    //printf("rule stack alloc: %d\n", parser->rule_stack_alloc);
    //printf("result stack alloc: %d\n", parser->result_stack_alloc);
    //printf("number of parse nodes allocated: %d\n", num_parse_nodes_allocated);

    // get the root parse node that we created
    assert(parser->result_stack_top == 1);
    mp_parse_node_t result = parser->result_stack[0];

finished:
    // free the memory that we don't need anymore
    m_del(rule_stack_t, parser->rule_stack, parser->rule_stack_alloc);
    m_del(mp_parse_node_t, parser->result_stack, parser->result_stack_alloc);
    m_del_obj(parser_t, parser);

    // return the result
    return result;

syntax_error:
    if (mp_lexer_is_kind(lex, MP_TOKEN_INDENT)) {
        *parse_error_kind_out = MP_PARSE_ERROR_UNEXPECTED_INDENT;
    } else if (mp_lexer_is_kind(lex, MP_TOKEN_DEDENT_MISMATCH)) {
        *parse_error_kind_out = MP_PARSE_ERROR_UNMATCHED_UNINDENT;
    } else {
        *parse_error_kind_out = MP_PARSE_ERROR_INVALID_SYNTAX;
#ifdef USE_RULE_NAME
        // debugging: print the rule name that failed and the token
        printf("rule: %s\n", rule->rule_name);
#if MICROPY_DEBUG_PRINTERS
        mp_token_show(mp_lexer_cur(lex));
#endif
#endif
    }
    result = MP_PARSE_NODE_NULL;
    goto finished;
}
