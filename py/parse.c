#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "lexer.h"
#include "machine.h"
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
#define DEF_RULE(rule, comp, kind, arg...) RULE_##rule,
#include "grammar.h"
#undef DEF_RULE
    RULE_maximum_number_of,
};

#define or(n)                   (RULE_ACT_OR | n)
#define and(n)                  (RULE_ACT_AND | n)
#define one_or_more             (RULE_ACT_LIST | 2)
#define list                    (RULE_ACT_LIST | 1)
#define list_with_end           (RULE_ACT_LIST | 3)
#define tok(t)                  (RULE_ARG_TOK | PY_TOKEN_##t)
#define rule(r)                 (RULE_ARG_RULE | RULE_##r)
#define opt_tok(t)              (RULE_ARG_OPT_TOK | PY_TOKEN_##t)
#define opt_rule(r)             (RULE_ARG_OPT_RULE | RULE_##r)
#ifdef USE_RULE_NAME
#define DEF_RULE(rule, comp, kind, arg...) static rule_t rule_##rule = { RULE_##rule, kind, #rule, { arg } };
#else
#define DEF_RULE(rule, comp, kind, arg...) static rule_t rule_##rule = { RULE_##rule, kind, { arg } };
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

static rule_t *rules[] = {
    NULL,
#define DEF_RULE(rule, comp, kind, arg...) &rule_##rule,
#include "grammar.h"
#undef DEF_RULE
};

typedef struct _rule_stack_t {
    byte rule_id;
    int32_t arg_i; // what should be the size and signedness?
} rule_stack_t;

typedef struct _parser_t {
    uint rule_stack_alloc;
    uint rule_stack_top;
    rule_stack_t *rule_stack;

    uint result_stack_top;
    py_parse_node_t *result_stack;
} parser_t;

static void push_rule(parser_t *parser, rule_t *rule, int arg_i) {
    if (parser->rule_stack_top >= parser->rule_stack_alloc) {
        parser->rule_stack_alloc *= 2;
        parser->rule_stack = m_renew(rule_stack_t, parser->rule_stack, parser->rule_stack_alloc);
    }
    parser->rule_stack[parser->rule_stack_top].rule_id = rule->rule_id;
    parser->rule_stack[parser->rule_stack_top].arg_i = arg_i;
    parser->rule_stack_top += 1;
}

static void push_rule_from_arg(parser_t *parser, uint arg) {
    assert((arg & RULE_ARG_KIND_MASK) == RULE_ARG_RULE || (arg & RULE_ARG_KIND_MASK) == RULE_ARG_OPT_RULE);
    uint rule_id = arg & RULE_ARG_ARG_MASK;
    assert(rule_id < RULE_maximum_number_of);
    push_rule(parser, rules[rule_id], 0);
}

static void pop_rule(parser_t *parser, rule_t **rule, uint *arg_i) {
    parser->rule_stack_top -= 1;
    *rule = rules[parser->rule_stack[parser->rule_stack_top].rule_id];
    *arg_i = parser->rule_stack[parser->rule_stack_top].arg_i;
}

py_parse_node_t py_parse_node_new_leaf(machine_int_t kind, machine_int_t arg) {
    return (py_parse_node_t)(kind | (arg << 4));
}

int num_parse_nodes_allocated = 0;
py_parse_node_struct_t *parse_node_new_struct(int rule_id, int num_args) {
    py_parse_node_struct_t *pn = m_malloc(sizeof(py_parse_node_struct_t) + num_args * sizeof(py_parse_node_t));
    pn->source = 0; // TODO
    pn->kind_num_nodes = (rule_id & 0xff) | (num_args << 8);
    num_parse_nodes_allocated += 1;
    return pn;
}

void parse_node_show(py_parse_node_t pn, int indent) {
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }
    if (PY_PARSE_NODE_IS_NULL(pn)) {
        printf("NULL\n");
    } else if (PY_PARSE_NODE_IS_LEAF(pn)) {
        int arg = PY_PARSE_NODE_LEAF_ARG(pn);
        switch (PY_PARSE_NODE_LEAF_KIND(pn)) {
            case PY_PARSE_NODE_ID: printf("id(%s)\n", qstr_str(arg)); break;
            case PY_PARSE_NODE_SMALL_INT: printf("int(%d)\n", arg); break;
            case PY_PARSE_NODE_INTEGER: printf("int(%s)\n", qstr_str(arg)); break;
            case PY_PARSE_NODE_DECIMAL: printf("dec(%s)\n", qstr_str(arg)); break;
            case PY_PARSE_NODE_STRING: printf("str(%s)\n", qstr_str(arg)); break;
            case PY_PARSE_NODE_BYTES: printf("bytes(%s)\n", qstr_str(arg)); break;
            case PY_PARSE_NODE_TOKEN: printf("tok(%d)\n", arg); break;
            default: assert(0);
        }
    } else {
        py_parse_node_struct_t *pns2 = (py_parse_node_struct_t*)pn;
        int n = pns2->kind_num_nodes >> 8;
#ifdef USE_RULE_NAME
        printf("%s(%d) (n=%d)\n", rules[PY_PARSE_NODE_STRUCT_KIND(pns2)]->rule_name, PY_PARSE_NODE_STRUCT_KIND(pns2), n);
#else
        printf("rule(%u) (n=%d)\n", (uint)PY_PARSE_NODE_STRUCT_KIND(pns2), n);
#endif
        for (int i = 0; i < n; i++) {
            parse_node_show(pns2->nodes[i], indent + 2);
        }
    }
}

/*
static void result_stack_show(parser_t *parser) {
    printf("result stack, most recent first\n");
    for (int i = parser->result_stack_top - 1; i >= 0; i--) {
        parse_node_show(parser->result_stack[i], 0);
    }
}
*/

static py_parse_node_t pop_result(parser_t *parser) {
    assert(parser->result_stack_top > 0);
    return parser->result_stack[--parser->result_stack_top];
}

static py_parse_node_t peek_result(parser_t *parser, int pos) {
    assert(parser->result_stack_top > pos);
    return parser->result_stack[parser->result_stack_top - 1 - pos];
}

static void push_result_node(parser_t *parser, py_parse_node_t pn) {
    parser->result_stack[parser->result_stack_top++] = pn;
}

static void push_result_token(parser_t *parser, const py_lexer_t *lex) {
    const py_token_t *tok = py_lexer_cur(lex);
    py_parse_node_t pn;
    if (tok->kind == PY_TOKEN_NAME) {
        pn = py_parse_node_new_leaf(PY_PARSE_NODE_ID, qstr_from_strn_copy(tok->str, tok->len));
    } else if (tok->kind == PY_TOKEN_NUMBER) {
        bool dec = false;
        bool small_int = true;
        int int_val = 0;
        int len = tok->len;
        const char *str = tok->str;
        int base = 10;
        int i = 0;
        if (len >= 3 && str[0] == '0') {
            if (str[1] == 'o' || str[1] == 'O') {
                // octal
                base = 8;
                i = 2;
            } else if (str[1] == 'x' || str[1] == 'X') {
                // hexadecimal
                base = 16;
                i = 2;
            } else if (str[1] == 'b' || str[1] == 'B') {
                // binary
                base = 2;
                i = 2;
            }
        }
        for (; i < len; i++) {
            if (g_unichar_isdigit(str[i]) && str[i] - '0' < base) {
                int_val = base * int_val + str[i] - '0';
            } else if (base == 16 && 'a' <= str[i] && str[i] <= 'f') {
                int_val = base * int_val + str[i] - 'a' + 10;
            } else if (base == 16 && 'F' <= str[i] && str[i] <= 'F') {
                int_val = base * int_val + str[i] - 'A' + 10;
            } else if (str[i] == '.' || str[i] == 'e' || str[i] == 'E') {
                dec = true;
                break;
            } else {
                small_int = false;
                break;
            }
        }
        if (dec) {
            pn = py_parse_node_new_leaf(PY_PARSE_NODE_DECIMAL, qstr_from_strn_copy(str, len));
        } else if (small_int && -0x800000 <= int_val && int_val <= 0x7fffff) { // XXX check this range formula!
            pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, int_val);
        } else {
            pn = py_parse_node_new_leaf(PY_PARSE_NODE_INTEGER, qstr_from_strn_copy(str, len));
        }
    } else if (tok->kind == PY_TOKEN_STRING) {
        pn = py_parse_node_new_leaf(PY_PARSE_NODE_STRING, qstr_from_strn_copy(tok->str, tok->len));
    } else if (tok->kind == PY_TOKEN_BYTES) {
        pn = py_parse_node_new_leaf(PY_PARSE_NODE_BYTES, qstr_from_strn_copy(tok->str, tok->len));
    } else {
        pn = py_parse_node_new_leaf(PY_PARSE_NODE_TOKEN, tok->kind);
    }
    push_result_node(parser, pn);
}

static void push_result_rule(parser_t *parser, rule_t *rule, int num_args) {
    py_parse_node_struct_t *pn = parse_node_new_struct(rule->rule_id, num_args);
    for (int i = num_args; i > 0; i--) {
        pn->nodes[i - 1] = pop_result(parser);
    }
    push_result_node(parser, (py_parse_node_t)pn);
}

py_parse_node_t py_parse(py_lexer_t *lex, int wanted_rule) {
    wanted_rule = RULE_file_input;
    parser_t *parser = m_new(parser_t, 1);
    parser->rule_stack_alloc = 64;
    parser->rule_stack_top = 0;
    parser->rule_stack = m_new(rule_stack_t, parser->rule_stack_alloc);

    parser->result_stack = m_new(py_parse_node_t, 1000);
    parser->result_stack_top = 0;

    push_rule(parser, rules[wanted_rule], 0);

    uint n, i;
    bool backtrack = false;
    rule_t *rule;
    py_token_kind_t tok_kind;
    bool emit_rule;
    bool had_trailing_sep;

    for (;;) {
        next_rule:
        if (parser->rule_stack_top == 0) {
            break;
        }

        pop_rule(parser, &rule, &i);
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
                            if (py_lexer_is_kind(lex, rule->arg[i] & RULE_ARG_ARG_MASK)) {
                                push_result_token(parser, lex);
                                py_lexer_to_next(lex);
                                goto next_rule;
                            }
                            break;
                        case RULE_ARG_RULE:
                            push_rule(parser, rule, i + 1);
                            push_rule_from_arg(parser, rule->arg[i]);
                            goto next_rule;
                        default:
                            assert(0);
                    }
                }
                if ((rule->arg[i] & RULE_ARG_KIND_MASK) == RULE_ARG_TOK) {
                    if (py_lexer_is_kind(lex, rule->arg[i] & RULE_ARG_ARG_MASK)) {
                        push_result_token(parser, lex);
                        py_lexer_to_next(lex);
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
                        push_result_node(parser, PY_PARSE_NODE_NULL);
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
                            if (py_lexer_is_kind(lex, tok_kind)) {
                                // matched token
                                if (tok_kind == PY_TOKEN_NAME) {
                                    push_result_token(parser, lex);
                                }
                                py_lexer_to_next(lex);
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
                            //if (i + 1 < n) {
                                push_rule(parser, rule, i + 1);
                            //}
                            push_rule_from_arg(parser, rule->arg[i]);
                            goto next_rule;
                        case RULE_ARG_OPT_RULE:
                            push_rule(parser, rule, i + 1);
                            push_rule_from_arg(parser, rule->arg[i]);
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
                        if (tok_kind >= PY_TOKEN_NAME) {
                            emit_rule = true;
                        }
                        if (tok_kind == PY_TOKEN_NAME) {
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
                if (rule->rule_id == RULE_else_stmt || rule->rule_id == RULE_testlist_comp_3b || rule->rule_id == RULE_import_as_names_paren || rule->rule_id == RULE_typedargslist_colon || rule->rule_id == RULE_typedargslist_equal || rule->rule_id == RULE_dictorsetmaker_colon || rule->rule_id == RULE_classdef_2 || rule->rule_id == RULE_with_item_as || rule->rule_id == RULE_assert_stmt_extra || rule->rule_id == RULE_as_name || rule->rule_id == RULE_raise_stmt_from || rule->rule_id == RULE_vfpdef) {
                    emit_rule = false;
                }

                // always emit these rules, and add an extra blank node at the end (to be used by the compiler to store data)
                if (rule->rule_id == RULE_funcdef || rule->rule_id == RULE_classdef || rule->rule_id == RULE_comp_for || rule->rule_id == RULE_lambdef || rule->rule_id == RULE_lambdef_nocond) {
                    emit_rule = true;
                    push_result_node(parser, PY_PARSE_NODE_NULL);
                    i += 1;
                }

                int num_not_nil = 0;
                for (int x = 0; x < i; ++x) {
                    if (peek_result(parser, x) != PY_PARSE_NODE_NULL) {
                        num_not_nil += 1;
                    }
                }
                //printf("done and %s n=%d i=%d notnil=%d\n", rule->rule_name, n, i, num_not_nil);
                if (emit_rule) {
                    push_result_rule(parser, rule, i);
                } else if (num_not_nil == 0) {
                    push_result_rule(parser, rule, i); // needed for, eg, atom_paren, testlist_comp_3b
                    //result_stack_show(parser);
                    //assert(0);
                } else if (num_not_nil == 1) {
                    // single result, leave it on stack
                    py_parse_node_t pn = PY_PARSE_NODE_NULL;
                    for (int x = 0; x < i; ++x) {
                        py_parse_node_t pn2 = pop_result(parser);
                        if (pn2 != PY_PARSE_NODE_NULL) {
                            pn = pn2;
                        }
                    }
                    push_result_node(parser, pn);
                } else {
                    push_result_rule(parser, rule, i);
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
                                if (py_lexer_is_kind(lex, arg & RULE_ARG_ARG_MASK)) {
                                    if (i & 1 & n) {
                                        // separators which are tokens are not pushed to result stack
                                    } else {
                                        push_result_token(parser, lex);
                                    }
                                    py_lexer_to_next(lex);
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
                                push_rule(parser, rule, i + 1);
                                push_rule_from_arg(parser, arg);
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
                        push_result_rule(parser, rule, i);
                    } else {
                        // just leave single item on stack (ie don't wrap in a list)
                    }
                } else {
                    //printf("done list %s %d %d\n", rule->rule_name, n, i);
                    push_result_rule(parser, rule, i);
                }
                break;

            default:
                assert(0);
        }
    }
    if (!py_lexer_is_kind(lex, PY_TOKEN_END)) {
        py_lexer_show_error(lex, "unexpected token at end:");
        py_token_show(py_lexer_cur(lex));
    }
    //printf("--------------\n");
    //result_stack_show(parser);
    assert(parser->result_stack_top == 1);
    //printf("maximum depth: %d\n", parser->rule_stack_alloc);
    //printf("number of parse nodes allocated: %d\n", num_parse_nodes_allocated);
    return parser->result_stack[0];

syntax_error:
    py_lexer_show_error(lex, "syntax error:");
#ifdef USE_RULE_NAME
    py_lexer_show_error(lex, rule->rule_name);
#endif
    py_token_show(py_lexer_cur(lex));
    return PY_PARSE_NODE_NULL;
}
