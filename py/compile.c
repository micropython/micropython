#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "lexer.h"
#include "machine.h"
#include "parse.h"
#include "scope.h"
#include "compile.h"
#include "runtime.h"
#include "emit.h"

// TODO need to mangle __attr names

typedef enum {
    PN_none = 0,
#define DEF_RULE(rule, comp, kind, arg...) PN_##rule,
#include "grammar.h"
#undef DEF_RULE
    PN_maximum_number_of,
} pn_kind_t;

#define EMIT(fun, arg...) (emit_##fun(comp->emit, ##arg))
#define EMIT_COMMON(fun, arg...) (emit_common_##fun(comp->pass, comp->scope_cur, ##arg))

typedef struct _compiler_t {
    qstr qstr___class__;
    qstr qstr___locals__;
    qstr qstr___name__;
    qstr qstr___module__;
    qstr qstr___qualname__;
    qstr qstr___doc__;
    qstr qstr_assertion_error;

    pass_kind_t pass;

    int break_label;
    int continue_label;
    int except_nest_level;

    int n_arg_keyword;
    bool have_star_arg;
    bool have_dbl_star_arg;
    bool have_bare_star;
    int param_pass;
    int param_pass_num_dict_params;
    int param_pass_num_default_params;

    scope_t *scope_head;
    scope_t *scope_cur;

    emitter_t *emit;
} compiler_t;

py_parse_node_t fold_constants(py_parse_node_t pn) {
    if (PY_PARSE_NODE_IS_STRUCT(pn)) {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
        int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);

        // fold arguments first
        for (int i = 0; i < n; i++) {
            pns->nodes[i] = fold_constants(pns->nodes[i]);
        }

        switch (PY_PARSE_NODE_STRUCT_KIND(pns)) {
            case PN_shift_expr:
                if (n == 3 && PY_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && PY_PARSE_NODE_IS_SMALL_INT(pns->nodes[2])) {
                    int arg0 = PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                    int arg1 = PY_PARSE_NODE_LEAF_ARG(pns->nodes[2]);
                    if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], PY_TOKEN_OP_DBL_LESS)) {
                        pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, arg0 << arg1); // XXX can overflow; enabled only to compare with CPython
                    } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], PY_TOKEN_OP_DBL_MORE)) {
                        pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, arg0 >> arg1);
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

            case PN_arith_expr:
                // XXX can overflow; enabled only to compare with CPython
                if (n == 3 && PY_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && PY_PARSE_NODE_IS_SMALL_INT(pns->nodes[2])) {
                    int arg0 = PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                    int arg1 = PY_PARSE_NODE_LEAF_ARG(pns->nodes[2]);
                    if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], PY_TOKEN_OP_PLUS)) {
                        pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, arg0 + arg1);
                    } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], PY_TOKEN_OP_MINUS)) {
                        pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, arg0 - arg1);
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

            case PN_term:
                // XXX can overflow; enabled only to compare with CPython
                if (n == 3 && PY_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && PY_PARSE_NODE_IS_SMALL_INT(pns->nodes[2])) {
                    int arg0 = PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                    int arg1 = PY_PARSE_NODE_LEAF_ARG(pns->nodes[2]);
                    if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], PY_TOKEN_OP_STAR)) {
                        pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, arg0 * arg1);
                    } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], PY_TOKEN_OP_SLASH)) {
                        ; // pass
                    //} else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], PY_TOKEN_OP_)) {
                        //pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, arg0 - arg1);
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

            case PN_factor_2:
                if (PY_PARSE_NODE_IS_SMALL_INT(pns->nodes[1])) {
                    machine_int_t arg = PY_PARSE_NODE_LEAF_ARG(pns->nodes[1]);
                    if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], PY_TOKEN_OP_PLUS)) {
                        pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, arg);
                    } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], PY_TOKEN_OP_MINUS)) {
                        pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, -arg);
                    } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], PY_TOKEN_OP_TILDE)) {
                        pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, ~arg);
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

            case PN_power:
                // XXX can overflow; enabled only to compare with CPython
                if (PY_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && PY_PARSE_NODE_IS_NULL(pns->nodes[1]) && !PY_PARSE_NODE_IS_NULL(pns->nodes[2])) {
                    py_parse_node_struct_t* pns2 = (py_parse_node_struct_t*)pns->nodes[2];
                    if (PY_PARSE_NODE_IS_SMALL_INT(pns2->nodes[0])) {
                        int power = PY_PARSE_NODE_LEAF_ARG(pns2->nodes[0]);
                        if (power >= 0) {
                            int ans = 1;
                            int base = PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                            for (; power > 0; power--) {
                                ans *= base;
                            }
                            pn = py_parse_node_new_leaf(PY_PARSE_NODE_SMALL_INT, ans);
                        }
                    }
                }
                break;
        }
    }

    return pn;
}

void compile_node(compiler_t *comp, py_parse_node_t pn);

scope_t *scope_new_and_link(compiler_t *comp, scope_kind_t kind, py_parse_node_t pn) {
    scope_t *scope = scope_new(kind, pn);
    scope->parent = comp->scope_cur;
    scope->next = NULL;
    if (comp->scope_head == NULL) {
        comp->scope_head = scope;
    } else {
        scope_t *s = comp->scope_head;
        while (s->next != NULL) {
            s = s->next;
        }
        s->next = scope;
    }
    return scope;
}

int list_len(py_parse_node_t pn, int pn_kind) {
    if (PY_PARSE_NODE_IS_NULL(pn)) {
        return 0;
    } else if (PY_PARSE_NODE_IS_LEAF(pn)) {
        return 1;
    } else {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
        if (PY_PARSE_NODE_STRUCT_KIND(pns) != pn_kind) {
            return 1;
        } else {
            return PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
        }
    }
}

void apply_to_single_or_list(compiler_t *comp, py_parse_node_t pn, int pn_list_kind, void (*f)(compiler_t*, py_parse_node_t)) {
    if (PY_PARSE_NODE_IS_STRUCT(pn) && PY_PARSE_NODE_STRUCT_KIND((py_parse_node_struct_t*)pn) == pn_list_kind) {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
        int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
        for (int i = 0; i < num_nodes; i++) {
            f(comp, pns->nodes[i]);
        }
    } else if (!PY_PARSE_NODE_IS_NULL(pn)) {
        f(comp, pn);
    }
}

int list_get(py_parse_node_t *pn, int pn_kind, py_parse_node_t **nodes) {
    if (PY_PARSE_NODE_IS_NULL(*pn)) {
        *nodes = NULL;
        return 0;
    } else if (PY_PARSE_NODE_IS_LEAF(*pn)) {
        *nodes = pn;
        return 1;
    } else {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)(*pn);
        if (PY_PARSE_NODE_STRUCT_KIND(pns) != pn_kind) {
            *nodes = pn;
            return 1;
        } else {
            *nodes = pns->nodes;
            return PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
        }
    }
}

void compile_do_nothing(compiler_t *comp, py_parse_node_struct_t *pns) {
}

void compile_generic_all_nodes(compiler_t *comp, py_parse_node_struct_t *pns) {
    int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < num_nodes; i++) {
        compile_node(comp, pns->nodes[i]);
    }
}

bool c_tuple_is_const(py_parse_node_t pn) {
    if (!PY_PARSE_NODE_IS_LEAF(pn)) {
        return false;
    }
    if (PY_PARSE_NODE_IS_ID(pn)) {
        return false;
    }
    return true;
}

void c_tuple_emit_const(compiler_t *comp, py_parse_node_t pn) {
    assert(PY_PARSE_NODE_IS_LEAF(pn));
    int arg = PY_PARSE_NODE_LEAF_ARG(pn);
    switch (PY_PARSE_NODE_LEAF_KIND(pn)) {
        case PY_PARSE_NODE_ID: assert(0);
        case PY_PARSE_NODE_SMALL_INT: EMIT(load_const_verbatim_int, arg); break;
        case PY_PARSE_NODE_INTEGER: EMIT(load_const_verbatim_str, qstr_str(arg)); break;
        case PY_PARSE_NODE_DECIMAL: EMIT(load_const_verbatim_str, qstr_str(arg)); break;
        case PY_PARSE_NODE_STRING: EMIT(load_const_verbatim_quoted_str, arg, false); break;
        case PY_PARSE_NODE_BYTES: EMIT(load_const_verbatim_quoted_str, arg, true); break;
        case PY_PARSE_NODE_TOKEN:
            switch (arg) {
                case PY_TOKEN_KW_FALSE: EMIT(load_const_verbatim_str, "False"); break;
                case PY_TOKEN_KW_NONE: EMIT(load_const_verbatim_str, "None"); break;
                case PY_TOKEN_KW_TRUE: EMIT(load_const_verbatim_str, "True"); break;
                default: assert(0);
            }
            break;
        default: assert(0);
    }
}

// funnelling all tuple creations through this function and all this constant stuff is purely to agree with CPython
void c_tuple(compiler_t *comp, py_parse_node_t pn, py_parse_node_struct_t *pns_list) {
    int n = 0;
    if (pns_list != NULL) {
        n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns_list);
    }
    int total = n;
    bool is_const = true;
    if (!PY_PARSE_NODE_IS_NULL(pn)) {
        total += 1;
        if (!c_tuple_is_const(pn)) {
            is_const = false;
        }
    }
    for (int i = 0; i < n; i++) {
        if (!c_tuple_is_const(pns_list->nodes[i])) {
            is_const = false;
            break;
        }
    }
    if (total > 0 && is_const) {
        bool need_comma = false;
        EMIT(load_const_verbatim_start);
        EMIT(load_const_verbatim_str, "(");
        if (!PY_PARSE_NODE_IS_NULL(pn)) {
            c_tuple_emit_const(comp, pn);
            need_comma = true;
        }
        for (int i = 0; i < n; i++) {
            if (need_comma) {
                EMIT(load_const_verbatim_str, ", ");
            }
            c_tuple_emit_const(comp, pns_list->nodes[i]);
            need_comma = true;
        }
        if (total == 1) {
            EMIT(load_const_verbatim_str, ",)");
        } else {
            EMIT(load_const_verbatim_str, ")");
        }
        EMIT(load_const_verbatim_end);
    } else {
        if (!PY_PARSE_NODE_IS_NULL(pn)) {
            compile_node(comp, pn);
        }
        for (int i = 0; i < n; i++) {
            compile_node(comp, pns_list->nodes[i]);
        }
        EMIT(build_tuple, total);
    }
}

void compile_generic_tuple(compiler_t *comp, py_parse_node_struct_t *pns) {
    // a simple tuple expression
    /*
    int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < n; i++) {
        compile_node(comp, pns->nodes[i]);
    }
    EMIT(build_tuple, n);
    */
    c_tuple(comp, PY_PARSE_NODE_NULL, pns);
}

bool node_is_const_false(py_parse_node_t pn) {
    return PY_PARSE_NODE_IS_TOKEN_KIND(pn, PY_TOKEN_KW_FALSE);
    // untested: || (PY_PARSE_NODE_IS_SMALL_INT(pn) && PY_PARSE_NODE_LEAF_ARG(pn) == 1);
}

bool node_is_const_true(py_parse_node_t pn) {
    return PY_PARSE_NODE_IS_TOKEN_KIND(pn, PY_TOKEN_KW_TRUE) || (PY_PARSE_NODE_IS_SMALL_INT(pn) && PY_PARSE_NODE_LEAF_ARG(pn) == 1);
}

// having c_if_cond_2 and the is_nested variable is purely to match with CPython, which doesn't fully optimise not's
void c_if_cond_2(compiler_t *comp, py_parse_node_t pn, bool jump_if, int label, bool is_nested) {
    if (node_is_const_false(pn)) {
        if (jump_if == false) {
            EMIT(jump, label);
        }
        return;
    } else if (node_is_const_true(pn)) {
        if (jump_if == true) {
            EMIT(jump, label);
        }
        return;
    } else if (PY_PARSE_NODE_IS_STRUCT(pn)) {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
        int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
        if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_or_test) {
            if (jump_if == false) {
                int label2 = EMIT(label_new);
                for (int i = 0; i < n - 1; i++) {
                    c_if_cond_2(comp, pns->nodes[i], true, label2, true);
                }
                c_if_cond_2(comp, pns->nodes[n - 1], false, label, true);
                EMIT(label_assign, label2);
            } else {
                for (int i = 0; i < n; i++) {
                    c_if_cond_2(comp, pns->nodes[i], true, label, true);
                }
            }
            return;
        } else if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_and_test) {
            if (jump_if == false) {
                for (int i = 0; i < n; i++) {
                    c_if_cond_2(comp, pns->nodes[i], false, label, true);
                }
            } else {
                int label2 = EMIT(label_new);
                for (int i = 0; i < n - 1; i++) {
                    c_if_cond_2(comp, pns->nodes[i], false, label2, true);
                }
                c_if_cond_2(comp, pns->nodes[n - 1], true, label, true);
                EMIT(label_assign, label2);
            }
            return;
        } else if (!is_nested && PY_PARSE_NODE_STRUCT_KIND(pns) == PN_not_test_2) {
            c_if_cond_2(comp, pns->nodes[0], !jump_if, label, true);
            return;
        }
    }

    // nothing special, fall back to default compiling for node and jump
    compile_node(comp, pn);
    if (jump_if == false) {
        EMIT(pop_jump_if_false, label);
    } else {
        EMIT(pop_jump_if_true, label);
    }
}

void c_if_cond(compiler_t *comp, py_parse_node_t pn, bool jump_if, int label) {
    c_if_cond_2(comp, pn, jump_if, label, false);
}

typedef enum { ASSIGN_STORE, ASSIGN_AUG_LOAD, ASSIGN_AUG_STORE } assign_kind_t;
void c_assign(compiler_t *comp, py_parse_node_t pn, assign_kind_t kind);

void c_assign_power(compiler_t *comp, py_parse_node_struct_t *pns, assign_kind_t assign_kind) {
    if (assign_kind != ASSIGN_AUG_STORE) {
        compile_node(comp, pns->nodes[0]);
    }

    if (PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
        py_parse_node_struct_t *pns1 = (py_parse_node_struct_t*)pns->nodes[1];
        if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_power_trailers) {
            int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns1);
            if (assign_kind != ASSIGN_AUG_STORE) {
                for (int i = 0; i < n - 1; i++) {
                    compile_node(comp, pns1->nodes[i]);
                }
            }
            assert(PY_PARSE_NODE_IS_STRUCT(pns1->nodes[n - 1]));
            pns1 = (py_parse_node_struct_t*)pns1->nodes[n - 1];
        }
        if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_paren) {
            printf("SyntaxError: can't assign to function call\n");
            return;
        } else if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_bracket) {
            if (assign_kind == ASSIGN_AUG_STORE) {
                EMIT(rot_three);
                EMIT(store_subscr);
            } else {
                compile_node(comp, pns1->nodes[0]);
                if (assign_kind == ASSIGN_AUG_LOAD) {
                    EMIT(dup_top_two);
                    EMIT(binary_op, RT_BINARY_OP_SUBSCR);
                } else {
                    EMIT(store_subscr);
                }
            }
        } else if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_period) {
            assert(PY_PARSE_NODE_IS_ID(pns1->nodes[0]));
            if (assign_kind == ASSIGN_AUG_LOAD) {
                EMIT(dup_top);
                EMIT(load_attr, PY_PARSE_NODE_LEAF_ARG(pns1->nodes[0]));
            } else {
                if (assign_kind == ASSIGN_AUG_STORE) {
                    EMIT(rot_two);
                }
                EMIT(store_attr, PY_PARSE_NODE_LEAF_ARG(pns1->nodes[0]));
            }
        } else {
            // shouldn't happen
            assert(0);
        }
    } else {
        // shouldn't happen
        assert(0);
    }

    if (!PY_PARSE_NODE_IS_NULL(pns->nodes[2])) {
        // SyntaxError, cannot assign
        assert(0);
    }
}

void c_assign_tuple(compiler_t *comp, int n, py_parse_node_t *nodes) {
    assert(n >= 0);
    int have_star_index = -1;
    for (int i = 0; i < n; i++) {
        if (PY_PARSE_NODE_IS_STRUCT_KIND(nodes[i], PN_star_expr)) {
            if (have_star_index < 0) {
                EMIT(unpack_ex, i, n - i - 1);
                have_star_index = i;
            } else {
                printf("SyntaxError: two starred expressions in assignment\n");
                return;
            }
        }
    }
    if (have_star_index < 0) {
        EMIT(unpack_sequence, n);
    }
    for (int i = 0; i < n; i++) {
        if (i == have_star_index) {
            c_assign(comp, ((py_parse_node_struct_t*)nodes[i])->nodes[0], ASSIGN_STORE);
        } else {
            c_assign(comp, nodes[i], ASSIGN_STORE);
        }
    }
}

// assigns top of stack to pn
void c_assign(compiler_t *comp, py_parse_node_t pn, assign_kind_t assign_kind) {
    tail_recursion:
    if (PY_PARSE_NODE_IS_NULL(pn)) {
        assert(0);
    } else if (PY_PARSE_NODE_IS_LEAF(pn)) {
        if (PY_PARSE_NODE_IS_ID(pn)) {
            int arg = PY_PARSE_NODE_LEAF_ARG(pn);
            switch (assign_kind) {
                case ASSIGN_STORE:
                case ASSIGN_AUG_STORE:
                    EMIT_COMMON(store_id, comp->emit, arg);
                    break;
                case ASSIGN_AUG_LOAD:
                    EMIT_COMMON(load_id, comp->qstr___class__, comp->emit, arg);
                    break;
            }
        } else {
            printf("SyntaxError: can't assign to literal\n");
            return;
        }
    } else {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
        switch (PY_PARSE_NODE_STRUCT_KIND(pns)) {
            case PN_power:
                // lhs is an index or attribute
                c_assign_power(comp, pns, assign_kind);
                break;

            case PN_testlist_star_expr:
            case PN_exprlist:
                // lhs is a tuple
                if (assign_kind != ASSIGN_STORE) {
                    goto bad_aug;
                }
                c_assign_tuple(comp, PY_PARSE_NODE_STRUCT_NUM_NODES(pns), pns->nodes);
                break;

            case PN_atom_paren:
                // lhs is something in parenthesis
                if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                    // empty tuple
                    printf("SyntaxError: can't assign to ()\n");
                    return;
                } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
                    pns = (py_parse_node_struct_t*)pns->nodes[0];
                    goto testlist_comp;
                } else {
                    // parenthesis around 1 item, is just that item
                    pn = pns->nodes[0];
                    goto tail_recursion;
                }
                break;

            case PN_atom_bracket:
                // lhs is something in brackets
                if (assign_kind != ASSIGN_STORE) {
                    goto bad_aug;
                }
                if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                    // empty list, assignment allowed
                    c_assign_tuple(comp, 0, NULL);
                } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
                    pns = (py_parse_node_struct_t*)pns->nodes[0];
                    goto testlist_comp;
                } else {
                    // brackets around 1 item
                    c_assign_tuple(comp, 1, &pns->nodes[0]);
                }
                break;

            default:
                printf("unknown assign, %u\n", (uint)PY_PARSE_NODE_STRUCT_KIND(pns));
                assert(0);
        }
        return;

        testlist_comp:
        // lhs is a sequence
        if (PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
            py_parse_node_struct_t *pns2 = (py_parse_node_struct_t*)pns->nodes[1];
            if (PY_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3b) {
                // sequence of one item, with trailing comma
                assert(PY_PARSE_NODE_IS_NULL(pns2->nodes[0]));
                c_assign_tuple(comp, 1, &pns->nodes[0]);
            } else if (PY_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3c) {
                // sequence of many items
                // TODO call c_assign_tuple instead
                int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns2);
                EMIT(unpack_sequence, 1 + n);
                c_assign(comp, pns->nodes[0], ASSIGN_STORE);
                for (int i = 0; i < n; i++) {
                    c_assign(comp, pns2->nodes[i], ASSIGN_STORE);
                }
            } else if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_comp_for) {
                // TODO not implemented
                assert(0);
            } else {
                // sequence with 2 items
                goto sequence_with_2_items;
            }
        } else {
            // sequence with 2 items
            sequence_with_2_items:
            c_assign_tuple(comp, 2, pns->nodes);
        }
        return;
    }
    return;

    bad_aug:
    printf("SyntaxError: illegal expression for augmented assignment\n");
}

// stuff for lambda and comprehensions and generators
void close_over_variables_etc(compiler_t *comp, scope_t *this_scope, int n_dict_params, int n_default_params) {
    // make closed over variables, if any
    int nfree = 0;
    if (comp->scope_cur->kind != SCOPE_MODULE) {
        for (int i = 0; i < this_scope->id_info_len; i++) {
            id_info_t *id_info = &this_scope->id_info[i];
            if (id_info->kind == ID_INFO_KIND_FREE) {
                EMIT(load_closure, id_info->qstr);
                nfree += 1;
            }
        }
    }
    if (nfree > 0) {
        EMIT(build_tuple, nfree);
    }

    // make the function/closure
    if (nfree == 0) {
        EMIT(make_function, this_scope, n_dict_params, n_default_params);
    } else {
        EMIT(make_closure, this_scope, n_dict_params, n_default_params);
    }
}

void compile_funcdef_param(compiler_t *comp, py_parse_node_t pn) {
    assert(PY_PARSE_NODE_IS_STRUCT(pn));
    py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
    if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_typedargslist_name) {
        if (!PY_PARSE_NODE_IS_NULL(pns->nodes[2])) {
            // this parameter has a default value
            // in CPython, None (and True, False?) as default parameters are loaded with LOAD_NAME; don't understandy why
            if (comp->have_bare_star) {
                comp->param_pass_num_dict_params += 1;
                if (comp->param_pass == 1) {
                    EMIT(load_const_id, PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]));
                    compile_node(comp, pns->nodes[2]);
                }
            } else {
                comp->param_pass_num_default_params += 1;
                if (comp->param_pass == 2) {
                    compile_node(comp, pns->nodes[2]);
                }
            }
        }
    } else if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_typedargslist_star) {
        if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
            // bare star
            comp->have_bare_star = true;
        }
    }
}

// leaves function object on stack
// returns function name
qstr compile_funcdef_helper(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (comp->pass == PASS_1) {
        // create a new scope for this function
        scope_t *s = scope_new_and_link(comp, SCOPE_FUNCTION, (py_parse_node_t)pns);
        // store the function scope so the compiling function can use it at each pass
        pns->nodes[4] = (py_parse_node_t)s;
    }

    // save variables (probably don't need to do this, since we can't have nested definitions..?)
    bool old_have_bare_star = comp->have_bare_star;
    int old_param_pass = comp->param_pass;
    int old_param_pass_num_dict_params = comp->param_pass_num_dict_params;
    int old_param_pass_num_default_params = comp->param_pass_num_default_params;

    // compile default parameters
    comp->have_bare_star = false;
    comp->param_pass = 1; // pass 1 does any default parameters after bare star
    comp->param_pass_num_dict_params = 0;
    comp->param_pass_num_default_params = 0;
    apply_to_single_or_list(comp, pns->nodes[1], PN_typedargslist, compile_funcdef_param);
    comp->have_bare_star = false;
    comp->param_pass = 2; // pass 2 does any default parameters before bare star
    comp->param_pass_num_dict_params = 0;
    comp->param_pass_num_default_params = 0;
    apply_to_single_or_list(comp, pns->nodes[1], PN_typedargslist, compile_funcdef_param);

    // get the scope for this function
    scope_t *fscope = (scope_t*)pns->nodes[4];

    // make the function
    close_over_variables_etc(comp, fscope, comp->param_pass_num_dict_params, comp->param_pass_num_default_params);

    // restore variables
    comp->have_bare_star = old_have_bare_star;
    comp->param_pass = old_param_pass;
    comp->param_pass_num_dict_params = old_param_pass_num_dict_params;
    comp->param_pass_num_default_params = old_param_pass_num_default_params;

    // return its name (the 'f' in "def f(...):")
    return fscope->simple_name;
}

// leaves class object on stack
// returns class name
qstr compile_classdef_helper(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (comp->pass == PASS_1) {
        // create a new scope for this class
        scope_t *s = scope_new_and_link(comp, SCOPE_CLASS, (py_parse_node_t)pns);
        // store the class scope so the compiling function can use it at each pass
        pns->nodes[3] = (py_parse_node_t)s;
    }

    EMIT(load_build_class);

    // scope for this class
    scope_t *cscope = (scope_t*)pns->nodes[3];

    // compile the class
    close_over_variables_etc(comp, cscope, 0, 0);

    // get its name
    EMIT(load_const_id, cscope->simple_name);

    // nodes[1] has parent classes, if any
    if (PY_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        // no parent classes
        EMIT(call_function, 2, 0, false, false);
    } else {
        // have a parent class or classes
        // TODO what if we have, eg, *a or **a in the parent list?
        compile_node(comp, pns->nodes[1]);
        EMIT(call_function, 2 + list_len(pns->nodes[1], PN_arglist), 0, false, false);
    }

    // return its name (the 'C' in class C(...):")
    return cscope->simple_name;
}

void compile_decorated(compiler_t *comp, py_parse_node_struct_t *pns) {
    // get the list of decorators
    py_parse_node_t *nodes;
    int n = list_get(&pns->nodes[0], PN_decorators, &nodes);

    // load each decorator
    for (int i = 0; i < n; i++) {
        assert(PY_PARSE_NODE_IS_STRUCT_KIND(nodes[i], PN_decorator)); // should be
        py_parse_node_struct_t *pns_decorator = (py_parse_node_struct_t*)nodes[i];
        py_parse_node_t *nodes2;
        int n2 = list_get(&pns_decorator->nodes[0], PN_dotted_name, &nodes2);
        compile_node(comp, nodes2[0]);
        for (int i = 1; i < n2; i++) {
            EMIT(load_attr, PY_PARSE_NODE_LEAF_ARG(nodes2[i]));
        }
        if (!PY_PARSE_NODE_IS_NULL(pns_decorator->nodes[1])) {
            // first call the function with these arguments
            compile_node(comp, pns_decorator->nodes[1]);
        }
    }

    // compile the body (funcdef or classdef) and get its name
    py_parse_node_struct_t *pns_body = (py_parse_node_struct_t*)pns->nodes[1];
    qstr body_name = 0;
    if (PY_PARSE_NODE_STRUCT_KIND(pns_body) == PN_funcdef) {
        body_name = compile_funcdef_helper(comp, pns_body);
    } else if (PY_PARSE_NODE_STRUCT_KIND(pns_body) == PN_classdef) {
        body_name = compile_classdef_helper(comp, pns_body);
    } else {
        // shouldn't happen
        assert(0);
    }

    // call each decorator
    for (int i = 0; i < n; i++) {
        EMIT(call_function, 1, 0, false, false);
    }

    // store func/class object into name
    EMIT_COMMON(store_id, comp->emit, body_name);
}

void compile_funcdef(compiler_t *comp, py_parse_node_struct_t *pns) {
    qstr fname = compile_funcdef_helper(comp, pns);
    // store function object into function name
    EMIT_COMMON(store_id, comp->emit, fname);
}

void c_del_stmt(compiler_t *comp, py_parse_node_t pn) {
    if (PY_PARSE_NODE_IS_ID(pn)) {
        EMIT_COMMON(delete_id, comp->emit, PY_PARSE_NODE_LEAF_ARG(pn));
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pn, PN_power)) {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;

        compile_node(comp, pns->nodes[0]); // base of the power node

        if (PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
            py_parse_node_struct_t *pns1 = (py_parse_node_struct_t*)pns->nodes[1];
            if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_power_trailers) {
                int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns1);
                for (int i = 0; i < n - 1; i++) {
                    compile_node(comp, pns1->nodes[i]);
                }
                assert(PY_PARSE_NODE_IS_STRUCT(pns1->nodes[n - 1]));
                pns1 = (py_parse_node_struct_t*)pns1->nodes[n - 1];
            }
            if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_paren) {
                // SyntaxError: can't delete a function call
                assert(0);
            } else if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_bracket) {
                compile_node(comp, pns1->nodes[0]);
                EMIT(delete_subscr);
            } else if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_period) {
                assert(PY_PARSE_NODE_IS_ID(pns1->nodes[0]));
                EMIT(delete_attr, PY_PARSE_NODE_LEAF_ARG(pns1->nodes[0]));
            } else {
                // shouldn't happen
                assert(0);
            }
        } else {
            // shouldn't happen
            assert(0);
        }

        if (!PY_PARSE_NODE_IS_NULL(pns->nodes[2])) {
            // SyntaxError, cannot delete
            assert(0);
        }
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pn, PN_atom_paren)) {
        pn = ((py_parse_node_struct_t*)pn)->nodes[0];
        if (PY_PARSE_NODE_IS_STRUCT_KIND(pn, PN_testlist_comp)) {
            py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
            // TODO perhaps factorise testlist_comp code with other uses of PN_testlist_comp

            if (PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
                py_parse_node_struct_t *pns1 = (py_parse_node_struct_t*)pns->nodes[1];
                if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_testlist_comp_3b) {
                    // sequence of one item, with trailing comma
                    assert(PY_PARSE_NODE_IS_NULL(pns1->nodes[0]));
                    c_del_stmt(comp, pns->nodes[0]);
                } else if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_testlist_comp_3c) {
                    // sequence of many items
                    int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns1);
                    c_del_stmt(comp, pns->nodes[0]);
                    for (int i = 0; i < n; i++) {
                        c_del_stmt(comp, pns1->nodes[i]);
                    }
                } else if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_comp_for) {
                    // TODO not implemented; can't del comprehension?
                    assert(0);
                } else {
                    // sequence with 2 items
                    goto sequence_with_2_items;
                }
            } else {
                // sequence with 2 items
                sequence_with_2_items:
                c_del_stmt(comp, pns->nodes[0]);
                c_del_stmt(comp, pns->nodes[1]);
            }
        } else {
            // tuple with 1 element
            c_del_stmt(comp, pn);
        }
    } else {
        // not implemented
        assert(0);
    }
}

void compile_del_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    apply_to_single_or_list(comp, pns->nodes[0], PN_exprlist, c_del_stmt);
}

void compile_break_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (comp->break_label == 0) {
        printf("ERROR: cannot break from here\n");
    }
    EMIT(break_loop, comp->break_label);
}

void compile_continue_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (comp->continue_label == 0) {
        printf("ERROR: cannot continue from here\n");
    }
    if (comp->except_nest_level > 0) {
        EMIT(continue_loop, comp->continue_label);
    } else {
        EMIT(jump, comp->continue_label);
    }
}

void compile_return_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        EMIT(load_const_tok, PY_TOKEN_KW_NONE);
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_test_if_expr)) {
        // special case when returning an if-expression; to match CPython optimisation
        py_parse_node_struct_t *pns_test_if_expr = (py_parse_node_struct_t*)pns->nodes[0];
        py_parse_node_struct_t *pns_test_if_else = (py_parse_node_struct_t*)pns_test_if_expr->nodes[1];

        int l_fail = EMIT(label_new);
        c_if_cond(comp, pns_test_if_else->nodes[0], false, l_fail); // condition
        compile_node(comp, pns_test_if_expr->nodes[0]); // success value
        EMIT(return_value);
        EMIT(label_assign, l_fail);
        compile_node(comp, pns_test_if_else->nodes[1]); // failure value
    } else {
        compile_node(comp, pns->nodes[0]);
    }
    EMIT(return_value);
}

void compile_yield_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]);
    EMIT(pop_top);
}

void compile_raise_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // raise
        EMIT(raise_varargs, 0);
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_raise_stmt_arg)) {
        // raise x from y
        pns = (py_parse_node_struct_t*)pns->nodes[0];
        compile_node(comp, pns->nodes[0]);
        compile_node(comp, pns->nodes[1]);
        EMIT(raise_varargs, 2);
    } else {
        // raise x
        compile_node(comp, pns->nodes[0]);
        EMIT(raise_varargs, 1);
    }
}

// q1 holds the base, q2 the full name
// eg   a -> q1=q2=a
//      a.b.c -> q1=a, q2=a.b.c
void do_import_name(compiler_t *comp, py_parse_node_t pn, qstr *q1, qstr *q2) {
    bool is_as = false;
    if (PY_PARSE_NODE_IS_STRUCT_KIND(pn, PN_dotted_as_name)) {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
        // a name of the form x as y; unwrap it
        *q1 = PY_PARSE_NODE_LEAF_ARG(pns->nodes[1]);
        pn = pns->nodes[0];
        is_as = true;
    }
    if (PY_PARSE_NODE_IS_ID(pn)) {
        // just a simple name
        *q2 = PY_PARSE_NODE_LEAF_ARG(pn);
        if (!is_as) {
            *q1 = *q2;
        }
        EMIT(import_name, *q2);
    } else if (PY_PARSE_NODE_IS_STRUCT(pn)) {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
        if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_dotted_name) {
            // a name of the form a.b.c
            if (!is_as) {
                *q1 = PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            }
            int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
            int len = n - 1;
            for (int i = 0; i < n; i++) {
                len += strlen(qstr_str(PY_PARSE_NODE_LEAF_ARG(pns->nodes[i])));
            }
            char *str = m_new(char, len + 1);
            str[0] = 0;
            for (int i = 0; i < n; i++) {
                if (i > 0) {
                    strcat(str, ".");
                }
                strcat(str, qstr_str(PY_PARSE_NODE_LEAF_ARG(pns->nodes[i])));
            }
            *q2 = qstr_from_str_take(str);
            EMIT(import_name, *q2);
            if (is_as) {
                for (int i = 1; i < n; i++) {
                    EMIT(load_attr, PY_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
                }
            }
        } else {
            // TODO not implemented
            assert(0);
        }
    } else {
        // TODO not implemented
        assert(0);
    }
}

void compile_dotted_as_name(compiler_t *comp, py_parse_node_t pn) {
    EMIT(load_const_small_int, 0); // ??
    EMIT(load_const_tok, PY_TOKEN_KW_NONE);
    qstr q1, q2;
    do_import_name(comp, pn, &q1, &q2);
    EMIT_COMMON(store_id, comp->emit, q1);
}

void compile_import_name(compiler_t *comp, py_parse_node_struct_t *pns) {
    apply_to_single_or_list(comp, pns->nodes[0], PN_dotted_as_names, compile_dotted_as_name);
}

void compile_import_from(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], PY_TOKEN_OP_STAR)) {
        EMIT(load_const_small_int, 0); // what's this for??
        EMIT(load_const_verbatim_start);
        EMIT(load_const_verbatim_str, "('*',)");
        EMIT(load_const_verbatim_end);
        qstr dummy_q, id1;
        do_import_name(comp, pns->nodes[0], &dummy_q, &id1);
        EMIT(import_star);
    } else {
        py_parse_node_t *pn_nodes;
        int n = list_get(&pns->nodes[1], PN_import_as_names, &pn_nodes);

        EMIT(load_const_small_int, 0); // what's this for??
        EMIT(load_const_verbatim_start);
        EMIT(load_const_verbatim_str, "(");
        for (int i = 0; i < n; i++) {
            assert(PY_PARSE_NODE_IS_STRUCT_KIND(pn_nodes[i], PN_import_as_name));
            py_parse_node_struct_t *pns3 = (py_parse_node_struct_t*)pn_nodes[i];
            qstr id2 = PY_PARSE_NODE_LEAF_ARG(pns3->nodes[0]); // should be id
            if (i > 0) {
                EMIT(load_const_verbatim_str, ", ");
            }
            EMIT(load_const_verbatim_str, "'");
            EMIT(load_const_verbatim_str, qstr_str(id2));
            EMIT(load_const_verbatim_str, "'");
        }
        if (n == 1) {
            EMIT(load_const_verbatim_str, ",");
        }
        EMIT(load_const_verbatim_str, ")");
        EMIT(load_const_verbatim_end);
        qstr dummy_q, id1;
        do_import_name(comp, pns->nodes[0], &dummy_q, &id1);
        for (int i = 0; i < n; i++) {
            assert(PY_PARSE_NODE_IS_STRUCT_KIND(pn_nodes[i], PN_import_as_name));
            py_parse_node_struct_t *pns3 = (py_parse_node_struct_t*)pn_nodes[i];
            qstr id2 = PY_PARSE_NODE_LEAF_ARG(pns3->nodes[0]); // should be id
            EMIT(import_from, id2);
            if (PY_PARSE_NODE_IS_NULL(pns3->nodes[1])) {
                EMIT_COMMON(store_id, comp->emit, id2);
            } else {
                EMIT_COMMON(store_id, comp->emit, PY_PARSE_NODE_LEAF_ARG(pns3->nodes[1]));
            }
        }
        EMIT(pop_top);
    }
}

void compile_global_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (PY_PARSE_NODE_IS_LEAF(pns->nodes[0])) {
        EMIT_COMMON(declare_global, PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]));
    } else {
        pns = (py_parse_node_struct_t*)pns->nodes[0];
        int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
        for (int i = 0; i < num_nodes; i++) {
            EMIT_COMMON(declare_global, PY_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
        }
    }
}

void compile_nonlocal_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (PY_PARSE_NODE_IS_LEAF(pns->nodes[0])) {
        EMIT_COMMON(declare_nonlocal, PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]));
    } else {
        pns = (py_parse_node_struct_t*)pns->nodes[0];
        int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
        for (int i = 0; i < num_nodes; i++) {
            EMIT_COMMON(declare_nonlocal, PY_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
        }
    }
}

void compile_assert_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    int l_end = EMIT(label_new);
    c_if_cond(comp, pns->nodes[0], true, l_end);
    EMIT_COMMON(load_id, comp->qstr___class__, comp->emit, comp->qstr_assertion_error);
    if (!PY_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        // assertion message
        compile_node(comp, pns->nodes[1]);
        EMIT(call_function, 1, 0, false, false);
    }
    EMIT(raise_varargs, 1);
    EMIT(label_assign, l_end);
}

void compile_if_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    // TODO proper and/or short circuiting

    int l_end = EMIT(label_new);

    int l_fail = EMIT(label_new);
    c_if_cond(comp, pns->nodes[0], false, l_fail); // if condition

    compile_node(comp, pns->nodes[1]); // if block
    //if (!(PY_PARSE_NODE_IS_NULL(pns->nodes[2]) && PY_PARSE_NODE_IS_NULL(pns->nodes[3]))) { // optimisation; doesn't align with CPython
        // jump over elif/else blocks if they exist
        if (!emit_last_emit_was_return_value(comp->emit)) { // simple optimisation to align with CPython
            EMIT(jump, l_end);
        }
    //}
    EMIT(label_assign, l_fail);

    if (!PY_PARSE_NODE_IS_NULL(pns->nodes[2])) {
        // compile elif blocks

        py_parse_node_struct_t *pns_elif = (py_parse_node_struct_t*)pns->nodes[2];

        if (PY_PARSE_NODE_STRUCT_KIND(pns_elif) == PN_if_stmt_elif_list) {
            // multiple elif blocks

            int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns_elif);
            for (int i = 0; i < n; i++) {
                py_parse_node_struct_t *pns_elif2 = (py_parse_node_struct_t*)pns_elif->nodes[i];
                l_fail = EMIT(label_new);
                c_if_cond(comp, pns_elif2->nodes[0], false, l_fail); // elif condition

                compile_node(comp, pns_elif2->nodes[1]); // elif block
                if (!emit_last_emit_was_return_value(comp->emit)) { // simple optimisation to align with CPython
                    EMIT(jump, l_end);
                }
                EMIT(label_assign, l_fail);
            }

        } else {
            // a single elif block

            l_fail = EMIT(label_new);
            c_if_cond(comp, pns_elif->nodes[0], false, l_fail); // elif condition

            compile_node(comp, pns_elif->nodes[1]); // elif block
            if (!emit_last_emit_was_return_value(comp->emit)) { // simple optimisation to align with CPython
                EMIT(jump, l_end);
            }
            EMIT(label_assign, l_fail);
        }
    }

    // compile else block
    compile_node(comp, pns->nodes[3]); // can be null

    EMIT(label_assign, l_end);
}

void compile_while_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    int old_break_label = comp->break_label;
    int old_continue_label = comp->continue_label;

    int done_label = EMIT(label_new);
    int end_label = EMIT(label_new);
    int break_label = EMIT(label_new);
    int continue_label = EMIT(label_new);

    comp->break_label = break_label;
    comp->continue_label = continue_label;

    EMIT(setup_loop, end_label);
    EMIT(label_assign, continue_label);
    c_if_cond(comp, pns->nodes[0], false, done_label); // condition
    compile_node(comp, pns->nodes[1]); // body
    if (!emit_last_emit_was_return_value(comp->emit)) {
        EMIT(jump, continue_label);
    }
    EMIT(label_assign, done_label);

    // break/continue apply to outer loop (if any) in the else block
    comp->break_label = old_break_label;
    comp->continue_label = old_continue_label;

    // CPython does not emit POP_BLOCK if the condition was a constant; don't undertand why
    // this is a small hack to agree with CPython
    if (!node_is_const_true(pns->nodes[0])) {
        EMIT(pop_block);
    }

    compile_node(comp, pns->nodes[2]); // else

    EMIT(label_assign, break_label);
    EMIT(label_assign, end_label);
}

void compile_for_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    int old_break_label = comp->break_label;
    int old_continue_label = comp->continue_label;

    int for_label = EMIT(label_new);
    int pop_label = EMIT(label_new);
    int end_label = EMIT(label_new);

    int break_label = EMIT(label_new);

    comp->continue_label = for_label;
    comp->break_label = break_label;

    EMIT(setup_loop, end_label);
    compile_node(comp, pns->nodes[1]); // iterator
    EMIT(get_iter);
    EMIT(label_assign, for_label);
    EMIT(for_iter, pop_label);
    c_assign(comp, pns->nodes[0], ASSIGN_STORE); // variable
    compile_node(comp, pns->nodes[2]); // body
    if (!emit_last_emit_was_return_value(comp->emit)) {
        EMIT(jump, for_label);
    }
    EMIT(label_assign, pop_label);
    EMIT(for_iter_end);

    // break/continue apply to outer loop (if any) in the else block
    comp->break_label = old_break_label;
    comp->continue_label = old_continue_label;

    EMIT(pop_block);

    compile_node(comp, pns->nodes[3]); // else (not tested)

    EMIT(label_assign, break_label);
    EMIT(label_assign, end_label);
}

void compile_try_except(compiler_t *comp, py_parse_node_t pn_body, int n_except, py_parse_node_t *pn_excepts, py_parse_node_t pn_else) {
    // this function is a bit of a hack at the moment
    // don't understand how the stack works with exceptions, so we force it to return to the correct value

    // setup code
    int stack_size = EMIT(get_stack_size);
    int l1 = EMIT(label_new);
    int success_label = EMIT(label_new);
    comp->except_nest_level += 1; // for correct handling of continue
    EMIT(setup_except, l1);
    compile_node(comp, pn_body); // body
    EMIT(pop_block);
    EMIT(jump, success_label);
    EMIT(label_assign, l1);
    int l2 = EMIT(label_new);

    for (int i = 0; i < n_except; i++) {
        assert(PY_PARSE_NODE_IS_STRUCT_KIND(pn_excepts[i], PN_try_stmt_except)); // should be
        py_parse_node_struct_t *pns_except = (py_parse_node_struct_t*)pn_excepts[i];

        qstr qstr_exception_local = 0;
        int end_finally_label = EMIT(label_new);

        if (PY_PARSE_NODE_IS_NULL(pns_except->nodes[0])) {
            // this is a catch all exception handler
            if (i + 1 != n_except) {
                printf("SyntaxError: default 'except:' must be last\n");
                return;
            }
        } else {
            // this exception handler requires a match to a certain type of exception
            py_parse_node_t pns_exception_expr = pns_except->nodes[0];
            if (PY_PARSE_NODE_IS_STRUCT(pns_exception_expr)) {
                py_parse_node_struct_t *pns3 = (py_parse_node_struct_t*)pns_exception_expr;
                if (PY_PARSE_NODE_STRUCT_KIND(pns3) == PN_try_stmt_as_name) {
                    // handler binds the exception to a local
                    pns_exception_expr = pns3->nodes[0];
                    qstr_exception_local = PY_PARSE_NODE_LEAF_ARG(pns3->nodes[1]);
                }
            }
            EMIT(dup_top);
            compile_node(comp, pns_exception_expr);
            EMIT(compare_op, RT_COMPARE_OP_EXCEPTION_MATCH);
            EMIT(pop_jump_if_false, end_finally_label);
        }

        EMIT(pop_top);

        if (qstr_exception_local == 0) {
            EMIT(pop_top);
        } else {
            EMIT_COMMON(store_id, comp->emit, qstr_exception_local);
        }

        EMIT(pop_top);

        int l3;
        if (qstr_exception_local != 0) {
            l3 = EMIT(label_new);
            EMIT(setup_finally, l3);
        }
        compile_node(comp, pns_except->nodes[1]);
        if (qstr_exception_local != 0) {
            EMIT(pop_block);
        }
        EMIT(pop_except);
        if (qstr_exception_local != 0) {
            EMIT(load_const_tok, PY_TOKEN_KW_NONE);
            EMIT(label_assign, l3);
            EMIT(load_const_tok, PY_TOKEN_KW_NONE);
            EMIT_COMMON(store_id, comp->emit, qstr_exception_local);
            EMIT_COMMON(delete_id, comp->emit, qstr_exception_local);
            EMIT(end_finally);
        }
        EMIT(jump, l2);
        EMIT(label_assign, end_finally_label);
    }

    EMIT(end_finally);
    EMIT(label_assign, success_label);
    comp->except_nest_level -= 1;
    compile_node(comp, pn_else); // else block, can be null
    EMIT(label_assign, l2);
    EMIT(set_stack_size, stack_size);
}

void compile_try_finally(compiler_t *comp, py_parse_node_t pn_body, int n_except, py_parse_node_t *pn_except, py_parse_node_t pn_else, py_parse_node_t pn_finally) {
    // don't understand how the stack works with exceptions, so we force it to return to the correct value
    int stack_size = EMIT(get_stack_size);
    int l_finally_block = EMIT(label_new);
    EMIT(setup_finally, l_finally_block);
    if (n_except == 0) {
        assert(PY_PARSE_NODE_IS_NULL(pn_else));
        compile_node(comp, pn_body);
    } else {
        compile_try_except(comp, pn_body, n_except, pn_except, pn_else);
    }
    EMIT(pop_block);
    EMIT(load_const_tok, PY_TOKEN_KW_NONE);
    EMIT(label_assign, l_finally_block);
    compile_node(comp, pn_finally);
    EMIT(end_finally);
    EMIT(set_stack_size, stack_size);
}

void compile_try_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
        py_parse_node_struct_t *pns2 = (py_parse_node_struct_t*)pns->nodes[1];
        if (PY_PARSE_NODE_STRUCT_KIND(pns2) == PN_try_stmt_finally) {
            // just try-finally
            compile_try_finally(comp, pns->nodes[0], 0, NULL, PY_PARSE_NODE_NULL, pns2->nodes[0]);
        } else if (PY_PARSE_NODE_STRUCT_KIND(pns2) == PN_try_stmt_except_and_more) {
            // try-except and possibly else and/or finally
            py_parse_node_t *pn_excepts;
            int n_except = list_get(&pns2->nodes[0], PN_try_stmt_except_list, &pn_excepts);
            if (PY_PARSE_NODE_IS_NULL(pns2->nodes[2])) {
                // no finally
                compile_try_except(comp, pns->nodes[0], n_except, pn_excepts, pns2->nodes[1]);
            } else {
                // have finally
                compile_try_finally(comp, pns->nodes[0], n_except, pn_excepts, pns2->nodes[1], ((py_parse_node_struct_t*)pns2->nodes[2])->nodes[0]);
            }
        } else {
            // just try-except
            py_parse_node_t *pn_excepts;
            int n_except = list_get(&pns->nodes[1], PN_try_stmt_except_list, &pn_excepts);
            compile_try_except(comp, pns->nodes[0], n_except, pn_excepts, PY_PARSE_NODE_NULL);
        }
    } else {
        // shouldn't happen
        assert(0);
    }
}

void compile_with_stmt_helper(compiler_t *comp, int n, py_parse_node_t *nodes, py_parse_node_t body) {
    if (n == 0) {
        // no more pre-bits, compile the body of the with
        compile_node(comp, body);
    } else {
        int l_end = EMIT(label_new);
        if (PY_PARSE_NODE_IS_STRUCT_KIND(nodes[0], PN_with_item)) {
            // this pre-bit is of the form "a as b"
            py_parse_node_struct_t *pns = (py_parse_node_struct_t*)nodes[0];
            compile_node(comp, pns->nodes[0]);
            EMIT(setup_with, l_end);
            c_assign(comp, pns->nodes[1], ASSIGN_STORE);
        } else {
            // this pre-bit is just an expression
            compile_node(comp, nodes[0]);
            EMIT(setup_with, l_end);
            EMIT(pop_top);
        }
        // compile additional pre-bits and the body
        compile_with_stmt_helper(comp, n - 1, nodes + 1, body);
        // finish this with block
        EMIT(pop_block);
        EMIT(load_const_tok, PY_TOKEN_KW_NONE);
        EMIT(label_assign, l_end);
        EMIT(with_cleanup);
        EMIT(end_finally);
    }
}

void compile_with_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    // get the nodes for the pre-bit of the with (the a as b, c as d, ... bit)
    py_parse_node_t *nodes;
    int n = list_get(&pns->nodes[0], PN_with_stmt_list, &nodes);
    assert(n > 0);

    // compile in a nested fashion
    compile_with_stmt_helper(comp, n, nodes, pns->nodes[1]);
}

void compile_expr_stmt(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (PY_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        if (PY_PARSE_NODE_IS_LEAF(pns->nodes[0]) && !PY_PARSE_NODE_IS_ID(pns->nodes[0])) {
            // do nothing with a lonely constant
        } else {
            compile_node(comp, pns->nodes[0]); // just an expression
            EMIT(pop_top); // discard last result since this is a statement and leaves nothing on the stack
        }
    } else {
        py_parse_node_struct_t *pns1 = (py_parse_node_struct_t*)pns->nodes[1];
        int kind = PY_PARSE_NODE_STRUCT_KIND(pns1);
        if (kind == PN_expr_stmt_augassign) {
            c_assign(comp, pns->nodes[0], ASSIGN_AUG_LOAD); // lhs load for aug assign
            compile_node(comp, pns1->nodes[1]); // rhs
            assert(PY_PARSE_NODE_IS_TOKEN(pns1->nodes[0]));
            // note that we don't really need to implement separate inplace ops, just normal binary ops will suffice
            switch (PY_PARSE_NODE_LEAF_ARG(pns1->nodes[0])) {
                case PY_TOKEN_DEL_PIPE_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_OR); break;
                case PY_TOKEN_DEL_CARET_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_XOR); break;
                case PY_TOKEN_DEL_AMPERSAND_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_AND); break;
                case PY_TOKEN_DEL_DBL_LESS_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_LSHIFT); break;
                case PY_TOKEN_DEL_DBL_MORE_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_RSHIFT); break;
                case PY_TOKEN_DEL_PLUS_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_ADD); break;
                case PY_TOKEN_DEL_MINUS_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_SUBTRACT); break;
                case PY_TOKEN_DEL_STAR_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_MULTIPLY); break;
                case PY_TOKEN_DEL_DBL_SLASH_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_FLOOR_DIVIDE); break;
                case PY_TOKEN_DEL_SLASH_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_TRUE_DIVIDE); break;
                case PY_TOKEN_DEL_PERCENT_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_MODULO); break;
                case PY_TOKEN_DEL_DBL_STAR_EQUAL: EMIT(binary_op, RT_BINARY_OP_INPLACE_POWER); break;
                default: assert(0); // shouldn't happen
            }
            c_assign(comp, pns->nodes[0], ASSIGN_AUG_STORE); // lhs store for aug assign
        } else if (kind == PN_expr_stmt_assign_list) {
            int rhs = PY_PARSE_NODE_STRUCT_NUM_NODES(pns1) - 1;
            compile_node(comp, ((py_parse_node_struct_t*)pns1->nodes[rhs])->nodes[0]); // rhs
            // following CPython, we store left-most first
            if (rhs > 0) {
                EMIT(dup_top);
            }
            c_assign(comp, pns->nodes[0], ASSIGN_STORE); // lhs store
            for (int i = 0; i < rhs; i++) {
                if (i + 1 < rhs) {
                    EMIT(dup_top);
                }
                c_assign(comp, ((py_parse_node_struct_t*)pns1->nodes[i])->nodes[0], ASSIGN_STORE); // middle store
            }
        } else if (kind == PN_expr_stmt_assign) {
            if (PY_PARSE_NODE_IS_STRUCT_KIND(pns1->nodes[0], PN_testlist_star_expr)
                && PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_star_expr)
                && PY_PARSE_NODE_STRUCT_NUM_NODES((py_parse_node_struct_t*)pns1->nodes[0]) == 2
                && PY_PARSE_NODE_STRUCT_NUM_NODES((py_parse_node_struct_t*)pns->nodes[0]) == 2) {
                // optimisation for a, b = c, d; to match CPython's optimisation
                py_parse_node_struct_t* pns10 = (py_parse_node_struct_t*)pns1->nodes[0];
                py_parse_node_struct_t* pns0 = (py_parse_node_struct_t*)pns->nodes[0];
                compile_node(comp, pns10->nodes[0]); // rhs
                compile_node(comp, pns10->nodes[1]); // rhs
                EMIT(rot_two);
                c_assign(comp, pns0->nodes[0], ASSIGN_STORE); // lhs store
                c_assign(comp, pns0->nodes[1], ASSIGN_STORE); // lhs store
            } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pns1->nodes[0], PN_testlist_star_expr)
                && PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_star_expr)
                && PY_PARSE_NODE_STRUCT_NUM_NODES((py_parse_node_struct_t*)pns1->nodes[0]) == 3
                && PY_PARSE_NODE_STRUCT_NUM_NODES((py_parse_node_struct_t*)pns->nodes[0]) == 3) {
                // optimisation for a, b, c = d, e, f; to match CPython's optimisation
                py_parse_node_struct_t* pns10 = (py_parse_node_struct_t*)pns1->nodes[0];
                py_parse_node_struct_t* pns0 = (py_parse_node_struct_t*)pns->nodes[0];
                compile_node(comp, pns10->nodes[0]); // rhs
                compile_node(comp, pns10->nodes[1]); // rhs
                compile_node(comp, pns10->nodes[2]); // rhs
                EMIT(rot_three);
                EMIT(rot_two);
                c_assign(comp, pns0->nodes[0], ASSIGN_STORE); // lhs store
                c_assign(comp, pns0->nodes[1], ASSIGN_STORE); // lhs store
                c_assign(comp, pns0->nodes[2], ASSIGN_STORE); // lhs store
            } else {
                compile_node(comp, pns1->nodes[0]); // rhs
                c_assign(comp, pns->nodes[0], ASSIGN_STORE); // lhs store
            }
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

void c_binary_op(compiler_t *comp, py_parse_node_struct_t *pns, rt_binary_op_t binary_op) {
    int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i < num_nodes; i += 1) {
        compile_node(comp, pns->nodes[i]);
        EMIT(binary_op, binary_op);
    }
}

void compile_test_if_expr(compiler_t *comp, py_parse_node_struct_t *pns) {
    assert(PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_test_if_else));
    py_parse_node_struct_t *pns_test_if_else = (py_parse_node_struct_t*)pns->nodes[1];

    int stack_size = EMIT(get_stack_size);
    int l_fail = EMIT(label_new);
    int l_end = EMIT(label_new);
    c_if_cond(comp, pns_test_if_else->nodes[0], false, l_fail); // condition
    compile_node(comp, pns->nodes[0]); // success value
    EMIT(jump, l_end);
    EMIT(label_assign, l_fail);
    EMIT(set_stack_size, stack_size); // force stack size reset
    compile_node(comp, pns_test_if_else->nodes[1]); // failure value
    EMIT(label_assign, l_end);
}

void compile_lambdef(compiler_t *comp, py_parse_node_struct_t *pns) {
    // TODO default params etc for lambda; possibly just use funcdef code
    //py_parse_node_t pn_params = pns->nodes[0];
    //py_parse_node_t pn_body = pns->nodes[1];

    if (comp->pass == PASS_1) {
        // create a new scope for this lambda
        scope_t *s = scope_new_and_link(comp, SCOPE_LAMBDA, (py_parse_node_t)pns);
        // store the lambda scope so the compiling function (this one) can use it at each pass
        pns->nodes[2] = (py_parse_node_t)s;
    }

    // get the scope for this lambda
    scope_t *this_scope = (scope_t*)pns->nodes[2];

    // make the lambda
    close_over_variables_etc(comp, this_scope, 0, 0);
}

void compile_or_test(compiler_t *comp, py_parse_node_struct_t *pns) {
    int l_end = EMIT(label_new);
    int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < n; i += 1) {
        compile_node(comp, pns->nodes[i]);
        if (i + 1 < n) {
            EMIT(jump_if_true_or_pop, l_end);
        }
    }
    EMIT(label_assign, l_end);
}

void compile_and_test(compiler_t *comp, py_parse_node_struct_t *pns) {
    int l_end = EMIT(label_new);
    int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < n; i += 1) {
        compile_node(comp, pns->nodes[i]);
        if (i + 1 < n) {
            EMIT(jump_if_false_or_pop, l_end);
        }
    }
    EMIT(label_assign, l_end);
}

void compile_not_test_2(compiler_t *comp, py_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]);
    EMIT(unary_op, RT_UNARY_OP_NOT);
}

void compile_comparison(compiler_t *comp, py_parse_node_struct_t *pns) {
    int stack_size = EMIT(get_stack_size);
    int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    bool multi = (num_nodes > 3);
    int l_fail = 0;
    if (multi) {
        l_fail = EMIT(label_new);
    }
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (i + 2 < num_nodes) {
            EMIT(dup_top);
            EMIT(rot_three);
        }
        if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_LESS)) {
            EMIT(compare_op, RT_COMPARE_OP_LESS);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_MORE)) {
            EMIT(compare_op, RT_COMPARE_OP_MORE);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_DBL_EQUAL)) {
            EMIT(compare_op, RT_COMPARE_OP_EQUAL);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_LESS_EQUAL)) {
            EMIT(compare_op, RT_COMPARE_OP_LESS_EQUAL);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_MORE_EQUAL)) {
            EMIT(compare_op, RT_COMPARE_OP_MORE_EQUAL);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_NOT_EQUAL)) {
            EMIT(compare_op, RT_COMPARE_OP_NOT_EQUAL);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_KW_IN)) {
            EMIT(compare_op, RT_COMPARE_OP_IN);
        } else if (PY_PARSE_NODE_IS_STRUCT(pns->nodes[i])) {
            py_parse_node_struct_t *pns2 = (py_parse_node_struct_t*)pns->nodes[i];
            int kind = PY_PARSE_NODE_STRUCT_KIND(pns2);
            if (kind == PN_comp_op_not_in) {
                EMIT(compare_op, RT_COMPARE_OP_NOT_IN);
            } else if (kind == PN_comp_op_is) {
                if (PY_PARSE_NODE_IS_NULL(pns2->nodes[0])) {
                    EMIT(compare_op, RT_COMPARE_OP_IS);
                } else {
                    EMIT(compare_op, RT_COMPARE_OP_IS_NOT);
                }
            } else {
                // shouldn't happen
                assert(0);
            }
        } else {
            // shouldn't happen
            assert(0);
        }
        if (i + 2 < num_nodes) {
            EMIT(jump_if_false_or_pop, l_fail);
        }
    }
    if (multi) {
        int l_end = EMIT(label_new);
        EMIT(jump, l_end);
        EMIT(label_assign, l_fail);
        EMIT(rot_two);
        EMIT(pop_top);
        EMIT(label_assign, l_end);
        EMIT(set_stack_size, stack_size + 1); // force stack size
    }
}

void compile_star_expr(compiler_t *comp, py_parse_node_struct_t *pns) {
    // TODO
    assert(0);
    compile_node(comp, pns->nodes[0]);
    //EMIT(unary_op, "UNARY_STAR");
}

void compile_expr(compiler_t *comp, py_parse_node_struct_t *pns) {
    c_binary_op(comp, pns, RT_BINARY_OP_OR);
}

void compile_xor_expr(compiler_t *comp, py_parse_node_struct_t *pns) {
    c_binary_op(comp, pns, RT_BINARY_OP_XOR);
}

void compile_and_expr(compiler_t *comp, py_parse_node_struct_t *pns) {
    c_binary_op(comp, pns, RT_BINARY_OP_AND);
}

void compile_shift_expr(compiler_t *comp, py_parse_node_struct_t *pns) {
    int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_DBL_LESS)) {
            EMIT(binary_op, RT_BINARY_OP_LSHIFT);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_DBL_MORE)) {
            EMIT(binary_op, RT_BINARY_OP_RSHIFT);
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

void compile_arith_expr(compiler_t *comp, py_parse_node_struct_t *pns) {
    int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_PLUS)) {
            EMIT(binary_op, RT_BINARY_OP_ADD);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_MINUS)) {
            EMIT(binary_op, RT_BINARY_OP_SUBTRACT);
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

void compile_term(compiler_t *comp, py_parse_node_struct_t *pns) {
    int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_STAR)) {
            EMIT(binary_op, RT_BINARY_OP_MULTIPLY);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_DBL_SLASH)) {
            EMIT(binary_op, RT_BINARY_OP_FLOOR_DIVIDE);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_SLASH)) {
            EMIT(binary_op, RT_BINARY_OP_TRUE_DIVIDE);
        } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], PY_TOKEN_OP_PERCENT)) {
            EMIT(binary_op, RT_BINARY_OP_MODULO);
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

void compile_factor_2(compiler_t *comp, py_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[1]);
    if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], PY_TOKEN_OP_PLUS)) {
        EMIT(unary_op, RT_UNARY_OP_POSITIVE);
    } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], PY_TOKEN_OP_MINUS)) {
        EMIT(unary_op, RT_UNARY_OP_NEGATIVE);
    } else if (PY_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], PY_TOKEN_OP_TILDE)) {
        EMIT(unary_op, RT_UNARY_OP_INVERT);
    } else {
        // shouldn't happen
        assert(0);
    }
}

void compile_trailer_paren_helper(compiler_t *comp, py_parse_node_struct_t *pns, bool is_method_call) {
    // function to call is on top of stack

    int old_n_arg_keyword = comp->n_arg_keyword;
    bool old_have_star_arg = comp->have_star_arg;
    bool old_have_dbl_star_arg = comp->have_dbl_star_arg;
    comp->n_arg_keyword = 0;
    comp->have_star_arg = false;
    comp->have_dbl_star_arg = false;

    compile_node(comp, pns->nodes[0]); // arguments to function call; can be null

    // compute number of positional arguments
    int n_positional = list_len(pns->nodes[0], PN_arglist) - comp->n_arg_keyword;
    if (comp->have_star_arg) {
        n_positional -= 1;
    }
    if (comp->have_dbl_star_arg) {
        n_positional -= 1;
    }

    if (is_method_call) {
        EMIT(call_method, n_positional, comp->n_arg_keyword, comp->have_star_arg, comp->have_dbl_star_arg);
    } else {
        EMIT(call_function, n_positional, comp->n_arg_keyword, comp->have_star_arg, comp->have_dbl_star_arg);
    }

    comp->n_arg_keyword = old_n_arg_keyword;
    comp->have_star_arg = old_have_star_arg;
    comp->have_dbl_star_arg = old_have_dbl_star_arg;
}

void compile_power_trailers(compiler_t *comp, py_parse_node_struct_t *pns) {
    int num_nodes = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < num_nodes; i++) {
        if (i + 1 < num_nodes && PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[i], PN_trailer_period) && PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[i + 1], PN_trailer_paren)) {
            // optimisation for method calls a.f(...), following PyPy
            py_parse_node_struct_t *pns_period = (py_parse_node_struct_t*)pns->nodes[i];
            py_parse_node_struct_t *pns_paren = (py_parse_node_struct_t*)pns->nodes[i + 1];
            EMIT(load_method, PY_PARSE_NODE_LEAF_ARG(pns_period->nodes[0])); // get the method
            compile_trailer_paren_helper(comp, pns_paren, true);
            i += 1;
        } else {
            compile_node(comp, pns->nodes[i]);
        }
    }
}

void compile_power_dbl_star(compiler_t *comp, py_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]);
    EMIT(binary_op, RT_BINARY_OP_POWER);
}

void compile_atom_string(compiler_t *comp, py_parse_node_struct_t *pns) {
    // a list of strings
    EMIT(load_const_verbatim_start);
    EMIT(load_const_verbatim_str, "'");
    int n = PY_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < n; i++) {
        // TODO allow concatenation of either strings or bytes, but not mixed
        assert(PY_PARSE_NODE_IS_LEAF(pns->nodes[i]));
        assert(PY_PARSE_NODE_LEAF_KIND(pns->nodes[i]) == PY_PARSE_NODE_STRING);
        const char *str = qstr_str(PY_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
        EMIT(load_const_verbatim_strn, str, strlen(str));
    }
    EMIT(load_const_verbatim_str, "'");
    EMIT(load_const_verbatim_end);
}

// pns needs to have 2 nodes, first is lhs of comprehension, second is PN_comp_for node
void compile_comprehension(compiler_t *comp, py_parse_node_struct_t *pns, scope_kind_t kind) {
    assert(PY_PARSE_NODE_STRUCT_NUM_NODES(pns) == 2);
    assert(PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_comp_for));
    py_parse_node_struct_t *pns_comp_for = (py_parse_node_struct_t*)pns->nodes[1];

    if (comp->pass == PASS_1) {
        // create a new scope for this comprehension
        scope_t *s = scope_new_and_link(comp, kind, (py_parse_node_t)pns);
        // store the comprehension scope so the compiling function (this one) can use it at each pass
        pns_comp_for->nodes[3] = (py_parse_node_t)s;
    }

    // get the scope for this comprehension
    scope_t *this_scope = (scope_t*)pns_comp_for->nodes[3];

    // compile the comprehension
    close_over_variables_etc(comp, this_scope, 0, 0);

    compile_node(comp, pns_comp_for->nodes[1]); // source of the iterator
    EMIT(get_iter);
    EMIT(call_function, 1, 0, false, false);
}

void compile_atom_paren(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // an empty tuple
        /*
        EMIT(build_tuple, 0);
        */
        c_tuple(comp, PY_PARSE_NODE_NULL, NULL);
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
        pns = (py_parse_node_struct_t*)pns->nodes[0];
        assert(!PY_PARSE_NODE_IS_NULL(pns->nodes[1]));
        if (PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
            py_parse_node_struct_t *pns2 = (py_parse_node_struct_t*)pns->nodes[1];
            if (PY_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3b) {
                // tuple of one item, with trailing comma
                assert(PY_PARSE_NODE_IS_NULL(pns2->nodes[0]));
                /*
                compile_node(comp, pns->nodes[0]);
                EMIT(build_tuple, 1);
                */
                c_tuple(comp, pns->nodes[0], NULL);
            } else if (PY_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3c) {
                // tuple of many items
                /*
                compile_node(comp, pns->nodes[0]);
                compile_generic_all_nodes(comp, pns2);
                EMIT(build_tuple, 1 + PY_PARSE_NODE_STRUCT_NUM_NODES(pns2));
                */
                c_tuple(comp, pns->nodes[0], pns2);
            } else if (PY_PARSE_NODE_STRUCT_KIND(pns2) == PN_comp_for) {
                // generator expression
                compile_comprehension(comp, pns, SCOPE_GEN_EXPR);
            } else {
                // tuple with 2 items
                goto tuple_with_2_items;
            }
        } else {
            // tuple with 2 items
            tuple_with_2_items:
            /*
            compile_node(comp, pns->nodes[0]);
            compile_node(comp, pns->nodes[1]);
            EMIT(build_tuple, 2);
            */
            c_tuple(comp, PY_PARSE_NODE_NULL, pns);
        }
    } else {
        // parenthesis around a single item, is just that item
        compile_node(comp, pns->nodes[0]);
    }
}

void compile_atom_bracket(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // empty list
        EMIT(build_list, 0);
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
        py_parse_node_struct_t *pns2 = (py_parse_node_struct_t*)pns->nodes[0];
        if (PY_PARSE_NODE_IS_STRUCT(pns2->nodes[1])) {
            py_parse_node_struct_t *pns3 = (py_parse_node_struct_t*)pns2->nodes[1];
            if (PY_PARSE_NODE_STRUCT_KIND(pns3) == PN_testlist_comp_3b) {
                // list of one item, with trailing comma
                assert(PY_PARSE_NODE_IS_NULL(pns3->nodes[0]));
                compile_node(comp, pns2->nodes[0]);
                EMIT(build_list, 1);
            } else if (PY_PARSE_NODE_STRUCT_KIND(pns3) == PN_testlist_comp_3c) {
                // list of many items
                compile_node(comp, pns2->nodes[0]);
                compile_generic_all_nodes(comp, pns3);
                EMIT(build_list, 1 + PY_PARSE_NODE_STRUCT_NUM_NODES(pns3));
            } else if (PY_PARSE_NODE_STRUCT_KIND(pns3) == PN_comp_for) {
                // list comprehension
                compile_comprehension(comp, pns2, SCOPE_LIST_COMP);
            } else {
                // list with 2 items
                goto list_with_2_items;
            }
        } else {
            // list with 2 items
            list_with_2_items:
            compile_node(comp, pns2->nodes[0]);
            compile_node(comp, pns2->nodes[1]);
            EMIT(build_list, 2);
        }
    } else {
        // list with 1 item
        compile_node(comp, pns->nodes[0]);
        EMIT(build_list, 1);
    }
}

void compile_atom_brace(compiler_t *comp, py_parse_node_struct_t *pns) {
    py_parse_node_t pn = pns->nodes[0];
    if (PY_PARSE_NODE_IS_NULL(pn)) {
        // empty dict
        EMIT(build_map, 0);
    } else if (PY_PARSE_NODE_IS_STRUCT(pn)) {
        pns = (py_parse_node_struct_t*)pn;
        if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_dictorsetmaker_item) {
            // dict with one element
            EMIT(build_map, 1);
            compile_node(comp, pn);
            EMIT(store_map);
        } else if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_dictorsetmaker) {
            assert(PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should succeed
            py_parse_node_struct_t *pns1 = (py_parse_node_struct_t*)pns->nodes[1];
            if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_dictorsetmaker_list) {
                // dict/set with multiple elements

                // get tail elements (2nd, 3rd, ...)
                py_parse_node_t *nodes;
                int n = list_get(&pns1->nodes[0], PN_dictorsetmaker_list2, &nodes);

                // first element sets whether it's a dict or set
                bool is_dict;
                if (PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_dictorsetmaker_item)) {
                    // a dictionary
                    EMIT(build_map, 1 + n);
                    compile_node(comp, pns->nodes[0]);
                    EMIT(store_map);
                    is_dict = true;
                } else {
                    // a set
                    compile_node(comp, pns->nodes[0]); // 1st value of set
                    is_dict = false;
                }

                // process rest of elements
                for (int i = 0; i < n; i++) {
                    py_parse_node_t pn = nodes[i];
                    bool is_key_value = PY_PARSE_NODE_IS_STRUCT_KIND(pn, PN_dictorsetmaker_item);
                    compile_node(comp, pn);
                    if (is_dict) {
                        if (!is_key_value) {
                            printf("SyntaxError?: expecting key:value for dictionary");
                            return;
                        }
                        EMIT(store_map);
                    } else {
                        if (is_key_value) {
                            printf("SyntaxError?: expecting just a value for set");
                            return;
                        }
                    }
                }

                // if it's a set, build it
                if (!is_dict) {
                    EMIT(build_set, 1 + n);
                }
            } else if (PY_PARSE_NODE_STRUCT_KIND(pns1) == PN_comp_for) {
                // dict/set comprehension
                if (PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_dictorsetmaker_item)) {
                    // a dictionary comprehension
                    compile_comprehension(comp, pns, SCOPE_DICT_COMP);
                } else {
                    // a set comprehension
                    compile_comprehension(comp, pns, SCOPE_SET_COMP);
                }
            } else {
                // shouldn't happen
                assert(0);
            }
        } else {
            // set with one element
            goto set_with_one_element;
        }
    } else {
        // set with one element
        set_with_one_element:
        compile_node(comp, pn);
        EMIT(build_set, 1);
    }
}

void compile_trailer_paren(compiler_t *comp, py_parse_node_struct_t *pns) {
    compile_trailer_paren_helper(comp, pns, false);
}

void compile_trailer_bracket(compiler_t *comp, py_parse_node_struct_t *pns) {
    // object who's index we want is on top of stack
    compile_node(comp, pns->nodes[0]); // the index
    EMIT(binary_op, RT_BINARY_OP_SUBSCR);
}

void compile_trailer_period(compiler_t *comp, py_parse_node_struct_t *pns) {
    // object who's attribute we want is on top of stack
    EMIT(load_attr, PY_PARSE_NODE_LEAF_ARG(pns->nodes[0])); // attribute to get
}

void compile_subscript_3_helper(compiler_t *comp, py_parse_node_struct_t *pns) {
    assert(PY_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_3); // should always be
    py_parse_node_t pn = pns->nodes[0];
    if (PY_PARSE_NODE_IS_NULL(pn)) {
        // [?:]
        EMIT(load_const_tok, PY_TOKEN_KW_NONE);
        EMIT(build_slice, 2);
    } else if (PY_PARSE_NODE_IS_STRUCT(pn)) {
        pns = (py_parse_node_struct_t*)pn;
        if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_3c) {
            EMIT(load_const_tok, PY_TOKEN_KW_NONE);
            pn = pns->nodes[0];
            if (PY_PARSE_NODE_IS_NULL(pn)) {
                // [?::]
                EMIT(build_slice, 2);
            } else {
                // [?::x]
                compile_node(comp, pn);
                EMIT(build_slice, 3);
            }
        } else if (PY_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_3d) {
            compile_node(comp, pns->nodes[0]);
            assert(PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should always be
            pns = (py_parse_node_struct_t*)pns->nodes[1];
            assert(PY_PARSE_NODE_STRUCT_KIND(pns) == PN_sliceop); // should always be
            if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                // [?:x:]
                EMIT(build_slice, 2);
            } else {
                // [?:x:x]
                compile_node(comp, pns->nodes[0]);
                EMIT(build_slice, 3);
            }
        } else {
            // [?:x]
            compile_node(comp, pn);
            EMIT(build_slice, 2);
        }
    } else {
        // [?:x]
        compile_node(comp, pn);
        EMIT(build_slice, 2);
    }
}

void compile_subscript_2(compiler_t *comp, py_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]); // start of slice
    assert(PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should always be
    compile_subscript_3_helper(comp, (py_parse_node_struct_t*)pns->nodes[1]);
}

void compile_subscript_3(compiler_t *comp, py_parse_node_struct_t *pns) {
    EMIT(load_const_tok, PY_TOKEN_KW_NONE);
    compile_subscript_3_helper(comp, pns);
}

void compile_dictorsetmaker_item(compiler_t *comp, py_parse_node_struct_t *pns) {
    // if this is called then we are compiling a dict key:value pair
    compile_node(comp, pns->nodes[1]); // value
    compile_node(comp, pns->nodes[0]); // key
}

void compile_classdef(compiler_t *comp, py_parse_node_struct_t *pns) {
    qstr cname = compile_classdef_helper(comp, pns);
    // store class object into class name
    EMIT_COMMON(store_id, comp->emit, cname);
}

void compile_arglist_star(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (comp->have_star_arg) {
        printf("SyntaxError?: can't have multiple *x\n");
        return;
    }
    comp->have_star_arg = true;
    compile_node(comp, pns->nodes[0]);
}

void compile_arglist_dbl_star(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (comp->have_dbl_star_arg) {
        printf("SyntaxError?: can't have multiple **x\n");
        return;
    }
    comp->have_dbl_star_arg = true;
    compile_node(comp, pns->nodes[0]);
}

void compile_argument(compiler_t *comp, py_parse_node_struct_t *pns) {
    assert(PY_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should always be
    py_parse_node_struct_t *pns2 = (py_parse_node_struct_t*)pns->nodes[1];
    if (PY_PARSE_NODE_STRUCT_KIND(pns2) == PN_argument_3) {
        if (!PY_PARSE_NODE_IS_ID(pns->nodes[0])) {
            printf("SyntaxError?: lhs of keyword argument must be an id\n");
            return;
        }
        EMIT(load_const_id, PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]));
        compile_node(comp, pns2->nodes[0]);
        comp->n_arg_keyword += 1;
    } else if (PY_PARSE_NODE_STRUCT_KIND(pns2) == PN_comp_for) {
        compile_comprehension(comp, pns, SCOPE_GEN_EXPR);
    } else {
        // shouldn't happen
        assert(0);
    }
}

void compile_yield_expr(compiler_t *comp, py_parse_node_struct_t *pns) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION) {
        printf("SyntaxError: 'yield' outside function\n");
        return;
    }
    if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        EMIT(load_const_tok, PY_TOKEN_KW_NONE);
        EMIT(yield_value);
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_yield_arg_from)) {
        pns = (py_parse_node_struct_t*)pns->nodes[0];
        compile_node(comp, pns->nodes[0]);
        EMIT(get_iter);
        EMIT(load_const_tok, PY_TOKEN_KW_NONE);
        EMIT(yield_from);
    } else {
        compile_node(comp, pns->nodes[0]);
        EMIT(yield_value);
    }
}

typedef void (*compile_function_t)(compiler_t*, py_parse_node_struct_t*);
static compile_function_t compile_function[] = {
    NULL,
#define nc NULL
#define c(f) compile_##f
#define DEF_RULE(rule, comp, kind, arg...) comp,
#include "grammar.h"
#undef nc
#undef c
#undef DEF_RULE
};

void compile_node(compiler_t *comp, py_parse_node_t pn) {
    if (PY_PARSE_NODE_IS_NULL(pn)) {
        // pass
    } else if (PY_PARSE_NODE_IS_LEAF(pn)) {
        int arg = PY_PARSE_NODE_LEAF_ARG(pn);
        switch (PY_PARSE_NODE_LEAF_KIND(pn)) {
            case PY_PARSE_NODE_ID: EMIT_COMMON(load_id, comp->qstr___class__, comp->emit, arg); break;
            case PY_PARSE_NODE_SMALL_INT: EMIT(load_const_small_int, arg); break;
            case PY_PARSE_NODE_INTEGER: EMIT(load_const_int, arg); break;
            case PY_PARSE_NODE_DECIMAL: EMIT(load_const_dec, arg); break;
            case PY_PARSE_NODE_STRING: EMIT(load_const_str, arg, false); break;
            case PY_PARSE_NODE_BYTES: EMIT(load_const_str, arg, true); break;
            case PY_PARSE_NODE_TOKEN: EMIT(load_const_tok, arg); break;
            default: assert(0);
        }
    } else {
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
        compile_function_t f = compile_function[PY_PARSE_NODE_STRUCT_KIND(pns)];
        if (f == NULL) {
            printf("node %u cannot be compiled\n", (uint)PY_PARSE_NODE_STRUCT_KIND(pns));
            parse_node_show(pn, 0);
            assert(0);
        } else {
            f(comp, pns);
        }
    }
}

void compile_scope_func_lambda_param(compiler_t *comp, py_parse_node_t pn, pn_kind_t pn_name, pn_kind_t pn_star, pn_kind_t pn_dbl_star, bool allow_annotations) {
    // TODO verify that *k and **k are last etc
    assert(PY_PARSE_NODE_IS_STRUCT(pn));
    py_parse_node_struct_t *pns = (py_parse_node_struct_t*)pn;
    qstr param_name = 0;
    py_parse_node_t pn_annotation = PY_PARSE_NODE_NULL;
    if (PY_PARSE_NODE_STRUCT_KIND(pns) == pn_name) {
        param_name = PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
        //int node_index = 1; unused
        if (allow_annotations) {
            if (!PY_PARSE_NODE_IS_NULL(pns->nodes[1])) {
                // this parameter has an annotation
                pn_annotation = pns->nodes[1];
            }
            //node_index = 2; unused
        }
        /* this is obsolete now that num dict/default params are calculated in compile_funcdef_param
        if (!PY_PARSE_NODE_IS_NULL(pns->nodes[node_index])) {
            // this parameter has a default value
            if (comp->have_bare_star) {
                comp->scope_cur->num_dict_params += 1;
            } else {
                comp->scope_cur->num_default_params += 1;
            }
        }
        */
        if (comp->have_bare_star) {
            // comes after a bare star, so doesn't count as a parameter
        } else {
            comp->scope_cur->num_params += 1;
        }
    } else if (PY_PARSE_NODE_STRUCT_KIND(pns) == pn_star) {
        if (PY_PARSE_NODE_IS_NULL(pns->nodes[0])) {
            // bare star
            // TODO see http://www.python.org/dev/peps/pep-3102/
            comp->have_bare_star = true;
            //assert(comp->scope_cur->num_dict_params == 0);
        } else if (PY_PARSE_NODE_IS_ID(pns->nodes[0])) {
            // named star
            comp->scope_cur->flags |= SCOPE_FLAG_VARARGS;
            param_name = PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
        } else if (allow_annotations && PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_tfpdef)) {
            // named star with annotation
            comp->scope_cur->flags |= SCOPE_FLAG_VARARGS;
            pns = (py_parse_node_struct_t*)pns->nodes[0];
            param_name = PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            pn_annotation = pns->nodes[1];
        } else {
            // shouldn't happen
            assert(0);
        }
    } else if (PY_PARSE_NODE_STRUCT_KIND(pns) == pn_dbl_star) {
        param_name = PY_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
        if (allow_annotations && !PY_PARSE_NODE_IS_NULL(pns->nodes[1])) {
            // this parameter has an annotation
            pn_annotation = pns->nodes[1];
        }
        comp->scope_cur->flags |= SCOPE_FLAG_VARKEYWORDS;
    } else {
        // TODO anything to implement?
        assert(0);
    }

    if (param_name != 0) {
        if (!PY_PARSE_NODE_IS_NULL(pn_annotation)) {
            // TODO this parameter has an annotation
        }
        bool added;
        id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, param_name, &added);
        if (!added) {
            printf("SyntaxError?: same name used for parameter; %s\n", qstr_str(param_name));
            return;
        }
        id_info->param = true;
        id_info->kind = ID_INFO_KIND_LOCAL;
    }
}

void compile_scope_func_param(compiler_t *comp, py_parse_node_t pn) {
    compile_scope_func_lambda_param(comp, pn, PN_typedargslist_name, PN_typedargslist_star, PN_typedargslist_dbl_star, true);
}

void compile_scope_lambda_param(compiler_t *comp, py_parse_node_t pn) {
    compile_scope_func_lambda_param(comp, pn, PN_varargslist_name, PN_varargslist_star, PN_varargslist_dbl_star, false);
}

void compile_scope_comp_iter(compiler_t *comp, py_parse_node_t pn_iter, py_parse_node_t pn_inner_expr, int l_top, int for_depth) {
    tail_recursion:
    if (PY_PARSE_NODE_IS_NULL(pn_iter)) {
        // no more nested if/for; compile inner expression
        compile_node(comp, pn_inner_expr);
        if (comp->scope_cur->kind == SCOPE_LIST_COMP) {
            EMIT(list_append, for_depth + 2);
        } else if (comp->scope_cur->kind == SCOPE_DICT_COMP) {
            EMIT(map_add, for_depth + 2);
        } else if (comp->scope_cur->kind == SCOPE_SET_COMP) {
            EMIT(set_add, for_depth + 2);
        } else {
            EMIT(yield_value);
            EMIT(pop_top);
        }
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pn_iter, PN_comp_if)) {
        // if condition
        py_parse_node_struct_t *pns_comp_if = (py_parse_node_struct_t*)pn_iter;
        c_if_cond(comp, pns_comp_if->nodes[0], false, l_top);
        pn_iter = pns_comp_if->nodes[1];
        goto tail_recursion;
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pn_iter, PN_comp_for)) {
        // for loop
        py_parse_node_struct_t *pns_comp_for2 = (py_parse_node_struct_t*)pn_iter;
        compile_node(comp, pns_comp_for2->nodes[1]);
        int l_end2 = EMIT(label_new);
        int l_top2 = EMIT(label_new);
        EMIT(get_iter);
        EMIT(label_assign, l_top2);
        EMIT(for_iter, l_end2);
        c_assign(comp, pns_comp_for2->nodes[0], ASSIGN_STORE);
        compile_scope_comp_iter(comp, pns_comp_for2->nodes[2], pn_inner_expr, l_top2, for_depth + 1);
        EMIT(jump, l_top2);
        EMIT(label_assign, l_end2);
        EMIT(for_iter_end);
    } else {
        // shouldn't happen
        assert(0);
    }
}

void check_for_doc_string(compiler_t *comp, py_parse_node_t pn) {
    // see http://www.python.org/dev/peps/pep-0257/

    // look for the first statement
    if (PY_PARSE_NODE_IS_STRUCT_KIND(pn, PN_expr_stmt)) {
        // fall through
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pn, PN_file_input_2)) {
        pn = ((py_parse_node_struct_t*)pn)->nodes[0];
    } else if (PY_PARSE_NODE_IS_STRUCT_KIND(pn, PN_suite_block_stmts)) {
        pn = ((py_parse_node_struct_t*)pn)->nodes[0];
    } else {
        return;
    }

    // check the first statement for a doc string
    if (PY_PARSE_NODE_IS_STRUCT_KIND(pn, PN_expr_stmt)) {
        py_parse_node_struct_t* pns = (py_parse_node_struct_t*)pn;
        if (PY_PARSE_NODE_IS_LEAF(pns->nodes[0])) {
            int kind = PY_PARSE_NODE_LEAF_KIND(pns->nodes[0]);
            if (kind == PY_PARSE_NODE_STRING) {
                compile_node(comp, pns->nodes[0]); // a doc string
                // store doc string
                EMIT_COMMON(store_id, comp->emit, comp->qstr___doc__);
            }
        }
    }
}

void compile_scope(compiler_t *comp, scope_t *scope, pass_kind_t pass) {
    comp->pass = pass;
    comp->scope_cur = scope;
    emit_start_pass(comp->emit, pass, scope);

    if (comp->pass == PASS_1) {
        scope->stack_size = 0;
    }

    if (comp->pass == PASS_3) {
        //printf("----\n");
        scope_print_info(scope);
    }

    // compile
    if (scope->kind == SCOPE_MODULE) {
        check_for_doc_string(comp, scope->pn);
        compile_node(comp, scope->pn);
        EMIT(load_const_tok, PY_TOKEN_KW_NONE);
        EMIT(return_value);
    } else if (scope->kind == SCOPE_FUNCTION) {
        assert(PY_PARSE_NODE_IS_STRUCT(scope->pn));
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)scope->pn;
        assert(PY_PARSE_NODE_STRUCT_KIND(pns) == PN_funcdef);

        // work out number of parameters, keywords and default parameters, and add them to the id_info array
        if (comp->pass == PASS_1) {
            comp->have_bare_star = false;
            apply_to_single_or_list(comp, pns->nodes[1], PN_typedargslist, compile_scope_func_param);
        }

        assert(pns->nodes[2] == 0); // 2 is something...

        compile_node(comp, pns->nodes[3]); // 3 is function body
        // emit return if it wasn't the last opcode
        if (!emit_last_emit_was_return_value(comp->emit)) {
            EMIT(load_const_tok, PY_TOKEN_KW_NONE);
            EMIT(return_value);
        }
    } else if (scope->kind == SCOPE_LAMBDA) {
        assert(PY_PARSE_NODE_IS_STRUCT(scope->pn));
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)scope->pn;
        assert(PY_PARSE_NODE_STRUCT_NUM_NODES(pns) == 3);

        // work out number of parameters, keywords and default parameters, and add them to the id_info array
        if (comp->pass == PASS_1) {
            comp->have_bare_star = false;
            apply_to_single_or_list(comp, pns->nodes[0], PN_varargslist, compile_scope_lambda_param);
        }

        compile_node(comp, pns->nodes[1]); // 1 is lambda body
        EMIT(return_value);
    } else if (scope->kind == SCOPE_LIST_COMP || scope->kind == SCOPE_DICT_COMP || scope->kind == SCOPE_SET_COMP || scope->kind == SCOPE_GEN_EXPR) {
        // a bit of a hack at the moment

        assert(PY_PARSE_NODE_IS_STRUCT(scope->pn));
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)scope->pn;
        assert(PY_PARSE_NODE_STRUCT_NUM_NODES(pns) == 2);
        assert(PY_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_comp_for));
        py_parse_node_struct_t *pns_comp_for = (py_parse_node_struct_t*)pns->nodes[1];

        qstr qstr_arg = qstr_from_strn_copy(".0", 2);
        if (comp->pass == PASS_1) {
            bool added;
            id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, qstr_arg, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
            scope->num_params = 1;
        }

        if (scope->kind == SCOPE_LIST_COMP) {
            EMIT(build_list, 0);
        } else if (scope->kind == SCOPE_DICT_COMP) {
            EMIT(build_map, 0);
        } else if (scope->kind == SCOPE_SET_COMP) {
            EMIT(build_set, 0);
        }

        int l_end = EMIT(label_new);
        int l_top = EMIT(label_new);
        EMIT_COMMON(load_id, comp->qstr___class__, comp->emit, qstr_arg);
        EMIT(label_assign, l_top);
        EMIT(for_iter, l_end);
        c_assign(comp, pns_comp_for->nodes[0], ASSIGN_STORE);
        compile_scope_comp_iter(comp, pns_comp_for->nodes[2], pns->nodes[0], l_top, 0);
        EMIT(jump, l_top);
        EMIT(label_assign, l_end);
        EMIT(for_iter_end);

        if (scope->kind == SCOPE_GEN_EXPR) {
            EMIT(load_const_tok, PY_TOKEN_KW_NONE);
        }
        EMIT(return_value);
    } else {
        assert(scope->kind == SCOPE_CLASS);
        assert(PY_PARSE_NODE_IS_STRUCT(scope->pn));
        py_parse_node_struct_t *pns = (py_parse_node_struct_t*)scope->pn;
        assert(PY_PARSE_NODE_STRUCT_KIND(pns) == PN_classdef);

        if (comp->pass == PASS_1) {
            bool added;
            id_info_t *id_info = scope_find_or_add_id(scope, comp->qstr___class__, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
            id_info = scope_find_or_add_id(scope, comp->qstr___locals__, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
            id_info->param = true;
            scope->num_params = 1; // __locals__ is the parameter
        }

        EMIT_COMMON(load_id, comp->qstr___class__, comp->emit, comp->qstr___locals__);
        EMIT(store_locals);
        EMIT_COMMON(load_id, comp->qstr___class__, comp->emit, comp->qstr___name__);
        EMIT_COMMON(store_id, comp->emit, comp->qstr___module__);
        EMIT(load_const_id, PY_PARSE_NODE_LEAF_ARG(pns->nodes[0])); // 0 is class name
        EMIT_COMMON(store_id, comp->emit, comp->qstr___qualname__);

        check_for_doc_string(comp, pns->nodes[2]);
        compile_node(comp, pns->nodes[2]); // 2 is class body

        id_info_t *id = scope_find(scope, comp->qstr___class__);
        assert(id != NULL);
        if (id->kind == ID_INFO_KIND_LOCAL) {
            EMIT(load_const_tok, PY_TOKEN_KW_NONE);
        } else {
            EMIT(load_closure, comp->qstr___class__);
        }
        EMIT(return_value);
    }

    emit_end_pass(comp->emit);
}

void compile_scope_compute_things(compiler_t *comp, scope_t *scope) {
    // in functions, turn implicit globals into explicit globals
    // compute num_locals, and the index of each local
    scope->num_locals = 0;
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
        if (scope->kind == SCOPE_CLASS && id->qstr == comp->qstr___class__) {
            // __class__ is not counted as a local; if it's used then it becomes a ID_INFO_KIND_CELL
            continue;
        }
        if (scope->kind >= SCOPE_FUNCTION && scope->kind <= SCOPE_GEN_EXPR && id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
            id->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
        }
        if (id->param || id->kind == ID_INFO_KIND_LOCAL) {
            id->local_num = scope->num_locals;
            scope->num_locals += 1;
        }
    }

    // compute flags
    //scope->flags = 0; since we set some things in parameters
    if (scope->kind != SCOPE_MODULE) {
        scope->flags |= SCOPE_FLAG_NEWLOCALS;
    }
    if (scope->kind == SCOPE_FUNCTION || scope->kind == SCOPE_LAMBDA || scope->kind == SCOPE_LIST_COMP || scope->kind == SCOPE_DICT_COMP || scope->kind == SCOPE_SET_COMP || scope->kind == SCOPE_GEN_EXPR) {
        assert(scope->parent != NULL);
        scope->flags |= SCOPE_FLAG_OPTIMISED;

        // TODO possibly other ways it can be nested
        if (scope->parent->kind == SCOPE_FUNCTION || (scope->parent->kind == SCOPE_CLASS && scope->parent->parent->kind == SCOPE_FUNCTION)) {
            scope->flags |= SCOPE_FLAG_NESTED;
        }
    }
    int num_free = 0;
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
        if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
            num_free += 1;
        }
    }
    if (num_free == 0) {
        scope->flags |= SCOPE_FLAG_NOFREE;
    }
}

void py_compile(py_parse_node_t pn) {
    compiler_t *comp = m_new(compiler_t, 1);

    comp->qstr___class__ = qstr_from_strn_copy("__class__", 9);
    comp->qstr___locals__ = qstr_from_strn_copy("__locals__", 10);
    comp->qstr___name__ = qstr_from_strn_copy("__name__", 8);
    comp->qstr___module__ = qstr_from_strn_copy("__module__", 10);
    comp->qstr___qualname__ = qstr_from_strn_copy("__qualname__", 12);
    comp->qstr___doc__ = qstr_from_strn_copy("__doc__", 7);
    comp->qstr_assertion_error = qstr_from_strn_copy("AssertionError", 14);

    comp->break_label = 0;
    comp->continue_label = 0;
    comp->except_nest_level = 0;
    comp->scope_head = NULL;
    comp->scope_cur = NULL;

    comp->emit = emit_new(comp->qstr___class__);

    pn = fold_constants(pn);
    scope_new_and_link(comp, SCOPE_MODULE, pn);

    for (scope_t *s = comp->scope_head; s != NULL; s = s->next) {
        compile_scope(comp, s, PASS_1);
    }

    for (scope_t *s = comp->scope_head; s != NULL; s = s->next) {
        compile_scope_compute_things(comp, s);
    }

    for (scope_t *s = comp->scope_head; s != NULL; s = s->next) {
        compile_scope(comp, s, PASS_2);
        compile_scope(comp, s, PASS_3);
    }

    m_free(comp);
}
