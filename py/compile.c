#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "parse.h"
#include "scope.h"
#include "runtime0.h"
#include "emit.h"
#include "obj.h"
#include "compile.h"
#include "runtime.h"

// TODO need to mangle __attr names

#define MICROPY_EMIT_NATIVE (MICROPY_EMIT_X64 || MICROPY_EMIT_THUMB)

typedef enum {
    PN_none = 0,
#define DEF_RULE(rule, comp, kind, ...) PN_##rule,
#include "grammar.h"
#undef DEF_RULE
    PN_maximum_number_of,
} pn_kind_t;

#define EMIT(fun) (comp->emit_method_table->fun(comp->emit))
#define EMIT_ARG(fun, ...) (comp->emit_method_table->fun(comp->emit, __VA_ARGS__))
#define EMIT_INLINE_ASM(fun) (comp->emit_inline_asm_method_table->fun(comp->emit_inline_asm))
#define EMIT_INLINE_ASM_ARG(fun, ...) (comp->emit_inline_asm_method_table->fun(comp->emit_inline_asm, __VA_ARGS__))

#define EMIT_OPT_NONE           (0)
#define EMIT_OPT_BYTE_CODE      (1)
#define EMIT_OPT_NATIVE_PYTHON  (2)
#define EMIT_OPT_VIPER          (3)
#define EMIT_OPT_ASM_THUMB      (4)

typedef struct _compiler_t {
    qstr source_file;
    bool is_repl;
    pass_kind_t pass;
    bool had_error; // try to keep compiler clean from nlr

    int next_label;

    int break_label;
    int continue_label;
    int break_continue_except_level;
    int cur_except_level; // increased for SETUP_EXCEPT, SETUP_FINALLY; decreased for POP_BLOCK, POP_EXCEPT

    int n_arg_keyword;
    bool have_star_arg;
    bool have_dbl_star_arg;
    bool have_bare_star;
    int param_pass;
    int param_pass_num_dict_params;
    int param_pass_num_default_params;

    bool func_arg_is_super; // used to compile special case of super() function call

    scope_t *scope_head;
    scope_t *scope_cur;

    emit_t *emit;                                   // current emitter
    const emit_method_table_t *emit_method_table;   // current emit method table

    emit_inline_asm_t *emit_inline_asm;                                   // current emitter for inline asm
    const emit_inline_asm_method_table_t *emit_inline_asm_method_table;   // current emit method table for inline asm
} compiler_t;

mp_parse_node_t fold_constants(mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);

        // fold arguments first
        for (int i = 0; i < n; i++) {
            pns->nodes[i] = fold_constants(pns->nodes[i]);
        }

        switch (MP_PARSE_NODE_STRUCT_KIND(pns)) {
            case PN_shift_expr:
                if (n == 3 && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[2])) {
                    int arg0 = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                    int arg1 = MP_PARSE_NODE_LEAF_ARG(pns->nodes[2]);
                    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_DBL_LESS)) {
#if MICROPY_EMIT_CPYTHON
                        // can overflow; enabled only to compare with CPython
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0 << arg1);
#endif
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_DBL_MORE)) {
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0 >> arg1);
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

            case PN_arith_expr:
                // overflow checking here relies on SMALL_INT being strictly smaller than machine_int_t
                if (n == 3 && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[2])) {
                    machine_int_t arg0 = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                    machine_int_t arg1 = MP_PARSE_NODE_LEAF_ARG(pns->nodes[2]);
                    machine_int_t res;
                    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_PLUS)) {
                        res = arg0 + arg1;
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_MINUS)) {
                        res = arg0 - arg1;
                    } else {
                        // shouldn't happen
                        assert(0);
                        res = 0;
                    }
                    if (MP_FIT_SMALL_INT(res)) {
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, res);
                    }
                }
                break;

            case PN_term:
                if (n == 3 && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[2])) {
                    int arg0 = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                    int arg1 = MP_PARSE_NODE_LEAF_ARG(pns->nodes[2]);
                    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_STAR)) {
#if MICROPY_EMIT_CPYTHON
                        // can overflow; enabled only to compare with CPython
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0 * arg1);
#endif
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_SLASH)) {
                        ; // pass
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_PERCENT)) {
                        // XXX implement this properly as Python's % operator acts differently to C's
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0 % arg1);
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_DBL_SLASH)) {
                        // XXX implement this properly as Python's // operator acts differently to C's
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0 / arg1);
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

            case PN_factor_2:
                if (MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[1])) {
                    machine_int_t arg = MP_PARSE_NODE_LEAF_ARG(pns->nodes[1]);
                    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_PLUS)) {
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg);
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_MINUS)) {
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, -arg);
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_TILDE)) {
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, ~arg);
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

#if MICROPY_EMIT_CPYTHON
            case PN_power:
                // can overflow; enabled only to compare with CPython
                if (MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_NULL(pns->nodes[1]) && !MP_PARSE_NODE_IS_NULL(pns->nodes[2])) {
                    mp_parse_node_struct_t* pns2 = (mp_parse_node_struct_t*)pns->nodes[2];
                    if (MP_PARSE_NODE_IS_SMALL_INT(pns2->nodes[0])) {
                        int power = MP_PARSE_NODE_LEAF_ARG(pns2->nodes[0]);
                        if (power >= 0) {
                            int ans = 1;
                            int base = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                            for (; power > 0; power--) {
                                ans *= base;
                            }
                            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, ans);
                        }
                    }
                }
                break;
#endif
        }
    }

    return pn;
}

STATIC void compile_trailer_paren_helper(compiler_t *comp, mp_parse_node_t pn_arglist, bool is_method_call, int n_positional_extra);
void compile_node(compiler_t *comp, mp_parse_node_t pn);

STATIC int comp_next_label(compiler_t *comp) {
    return comp->next_label++;
}

STATIC scope_t *scope_new_and_link(compiler_t *comp, scope_kind_t kind, mp_parse_node_t pn, uint emit_options) {
    scope_t *scope = scope_new(kind, pn, comp->source_file, rt_get_unique_code_id(), emit_options);
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

STATIC int list_len(mp_parse_node_t pn, int pn_kind) {
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        return 0;
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        return 1;
    } else {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) != pn_kind) {
            return 1;
        } else {
            return MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        }
    }
}

STATIC void apply_to_single_or_list(compiler_t *comp, mp_parse_node_t pn, int pn_list_kind, void (*f)(compiler_t*, mp_parse_node_t)) {
    if (MP_PARSE_NODE_IS_STRUCT(pn) && MP_PARSE_NODE_STRUCT_KIND((mp_parse_node_struct_t*)pn) == pn_list_kind) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        for (int i = 0; i < num_nodes; i++) {
            f(comp, pns->nodes[i]);
        }
    } else if (!MP_PARSE_NODE_IS_NULL(pn)) {
        f(comp, pn);
    }
}

STATIC int list_get(mp_parse_node_t *pn, int pn_kind, mp_parse_node_t **nodes) {
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

void compile_do_nothing(compiler_t *comp, mp_parse_node_struct_t *pns) {
}

void compile_generic_all_nodes(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < num_nodes; i++) {
        compile_node(comp, pns->nodes[i]);
    }
}

#if MICROPY_EMIT_CPYTHON
STATIC bool cpython_c_tuple_is_const(mp_parse_node_t pn) {
    if (!MP_PARSE_NODE_IS_LEAF(pn)) {
        return false;
    }
    if (MP_PARSE_NODE_IS_ID(pn)) {
        return false;
    }
    return true;
}

STATIC void cpython_c_print_quoted_str(vstr_t *vstr, qstr qstr, bool bytes) {
    uint len;
    const byte *str = qstr_data(qstr, &len);
    bool has_single_quote = false;
    bool has_double_quote = false;
    for (int i = 0; i < len; i++) {
        if (str[i] == '\'') {
            has_single_quote = true;
        } else if (str[i] == '"') {
            has_double_quote = true;
        }
    }
    if (bytes) {
        vstr_printf(vstr, "b");
    }
    bool quote_single = false;
    if (has_single_quote && !has_double_quote) {
        vstr_printf(vstr, "\"");
    } else {
        quote_single = true;
        vstr_printf(vstr, "'");
    }
    for (int i = 0; i < len; i++) {
        if (str[i] == '\n') {
            vstr_printf(vstr, "\\n");
        } else if (str[i] == '\\') {
            vstr_printf(vstr, "\\\\");
        } else if (str[i] == '\'' && quote_single) {
            vstr_printf(vstr, "\\'");
        } else {
            vstr_printf(vstr, "%c", str[i]);
        }
    }
    if (has_single_quote && !has_double_quote) {
        vstr_printf(vstr, "\"");
    } else {
        vstr_printf(vstr, "'");
    }
}

STATIC void cpython_c_tuple_emit_const(compiler_t *comp, mp_parse_node_t pn, vstr_t *vstr) {
    assert(MP_PARSE_NODE_IS_LEAF(pn));
    int arg = MP_PARSE_NODE_LEAF_ARG(pn);
    switch (MP_PARSE_NODE_LEAF_KIND(pn)) {
        case MP_PARSE_NODE_ID: assert(0);
        case MP_PARSE_NODE_SMALL_INT: vstr_printf(vstr, "%d", arg); break;
        case MP_PARSE_NODE_INTEGER: vstr_printf(vstr, "%s", qstr_str(arg)); break;
        case MP_PARSE_NODE_DECIMAL: vstr_printf(vstr, "%s", qstr_str(arg)); break;
        case MP_PARSE_NODE_STRING: cpython_c_print_quoted_str(vstr, arg, false); break;
        case MP_PARSE_NODE_BYTES: cpython_c_print_quoted_str(vstr, arg, true); break;
        case MP_PARSE_NODE_TOKEN:
            switch (arg) {
                case MP_TOKEN_KW_FALSE: vstr_printf(vstr, "False"); break;
                case MP_TOKEN_KW_NONE: vstr_printf(vstr, "None"); break;
                case MP_TOKEN_KW_TRUE: vstr_printf(vstr, "True"); break;
                default: assert(0);
            }
            break;
        default: assert(0);
    }
}

STATIC void cpython_c_tuple(compiler_t *comp, mp_parse_node_t pn, mp_parse_node_struct_t *pns_list) {
    int n = 0;
    if (pns_list != NULL) {
        n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns_list);
    }
    int total = n;
    bool is_const = true;
    if (!MP_PARSE_NODE_IS_NULL(pn)) {
        total += 1;
        if (!cpython_c_tuple_is_const(pn)) {
            is_const = false;
        }
    }
    for (int i = 0; i < n; i++) {
        if (!cpython_c_tuple_is_const(pns_list->nodes[i])) {
            is_const = false;
            break;
        }
    }
    if (total > 0 && is_const) {
        bool need_comma = false;
        vstr_t *vstr = vstr_new();
        vstr_printf(vstr, "(");
        if (!MP_PARSE_NODE_IS_NULL(pn)) {
            cpython_c_tuple_emit_const(comp, pn, vstr);
            need_comma = true;
        }
        for (int i = 0; i < n; i++) {
            if (need_comma) {
                vstr_printf(vstr, ", ");
            }
            cpython_c_tuple_emit_const(comp, pns_list->nodes[i], vstr);
            need_comma = true;
        }
        if (total == 1) {
            vstr_printf(vstr, ",)");
        } else {
            vstr_printf(vstr, ")");
        }
        EMIT_ARG(load_const_verbatim_str, vstr_str(vstr));
        vstr_free(vstr);
    } else {
        if (!MP_PARSE_NODE_IS_NULL(pn)) {
            compile_node(comp, pn);
        }
        for (int i = 0; i < n; i++) {
            compile_node(comp, pns_list->nodes[i]);
        }
        EMIT_ARG(build_tuple, total);
    }
}
#endif

// funnelling all tuple creations through this function is purely so we can optionally agree with CPython
void c_tuple(compiler_t *comp, mp_parse_node_t pn, mp_parse_node_struct_t *pns_list) {
#if MICROPY_EMIT_CPYTHON
    cpython_c_tuple(comp, pn, pns_list);
#else
    int total = 0;
    if (!MP_PARSE_NODE_IS_NULL(pn)) {
        compile_node(comp, pn);
        total += 1;
    }
    if (pns_list != NULL) {
        int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns_list);
        for (int i = 0; i < n; i++) {
            compile_node(comp, pns_list->nodes[i]);
        }
        total += n;
    }
    EMIT_ARG(build_tuple, total);
#endif
}

void compile_generic_tuple(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // a simple tuple expression
    c_tuple(comp, MP_PARSE_NODE_NULL, pns);
}

STATIC bool node_is_const_false(mp_parse_node_t pn) {
    return MP_PARSE_NODE_IS_TOKEN_KIND(pn, MP_TOKEN_KW_FALSE);
    // untested: || (MP_PARSE_NODE_IS_SMALL_INT(pn) && MP_PARSE_NODE_LEAF_ARG(pn) == 1);
}

STATIC bool node_is_const_true(mp_parse_node_t pn) {
    return MP_PARSE_NODE_IS_TOKEN_KIND(pn, MP_TOKEN_KW_TRUE) || (MP_PARSE_NODE_IS_SMALL_INT(pn) && MP_PARSE_NODE_LEAF_ARG(pn) == 1);
}

#if MICROPY_EMIT_CPYTHON
// the is_nested variable is purely to match with CPython, which doesn't fully optimise not's
STATIC void cpython_c_if_cond(compiler_t *comp, mp_parse_node_t pn, bool jump_if, int label, bool is_nested) {
    if (node_is_const_false(pn)) {
        if (jump_if == false) {
            EMIT_ARG(jump, label);
        }
        return;
    } else if (node_is_const_true(pn)) {
        if (jump_if == true) {
            EMIT_ARG(jump, label);
        }
        return;
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_or_test) {
            if (jump_if == false) {
                int label2 = comp_next_label(comp);
                for (int i = 0; i < n - 1; i++) {
                    cpython_c_if_cond(comp, pns->nodes[i], true, label2, true);
                }
                cpython_c_if_cond(comp, pns->nodes[n - 1], false, label, true);
                EMIT_ARG(label_assign, label2);
            } else {
                for (int i = 0; i < n; i++) {
                    cpython_c_if_cond(comp, pns->nodes[i], true, label, true);
                }
            }
            return;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_and_test) {
            if (jump_if == false) {
                for (int i = 0; i < n; i++) {
                    cpython_c_if_cond(comp, pns->nodes[i], false, label, true);
                }
            } else {
                int label2 = comp_next_label(comp);
                for (int i = 0; i < n - 1; i++) {
                    cpython_c_if_cond(comp, pns->nodes[i], false, label2, true);
                }
                cpython_c_if_cond(comp, pns->nodes[n - 1], true, label, true);
                EMIT_ARG(label_assign, label2);
            }
            return;
        } else if (!is_nested && MP_PARSE_NODE_STRUCT_KIND(pns) == PN_not_test_2) {
            cpython_c_if_cond(comp, pns->nodes[0], !jump_if, label, true);
            return;
        }
    }

    // nothing special, fall back to default compiling for node and jump
    compile_node(comp, pn);
    if (jump_if == false) {
        EMIT_ARG(pop_jump_if_false, label);
    } else {
        EMIT_ARG(pop_jump_if_true, label);
    }
}
#endif

STATIC void c_if_cond(compiler_t *comp, mp_parse_node_t pn, bool jump_if, int label) {
#if MICROPY_EMIT_CPYTHON
    cpython_c_if_cond(comp, pn, jump_if, label, false);
#else
    if (node_is_const_false(pn)) {
        if (jump_if == false) {
            EMIT_ARG(jump, label);
        }
        return;
    } else if (node_is_const_true(pn)) {
        if (jump_if == true) {
            EMIT_ARG(jump, label);
        }
        return;
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_or_test) {
            if (jump_if == false) {
                int label2 = comp_next_label(comp);
                for (int i = 0; i < n - 1; i++) {
                    c_if_cond(comp, pns->nodes[i], true, label2);
                }
                c_if_cond(comp, pns->nodes[n - 1], false, label);
                EMIT_ARG(label_assign, label2);
            } else {
                for (int i = 0; i < n; i++) {
                    c_if_cond(comp, pns->nodes[i], true, label);
                }
            }
            return;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_and_test) {
            if (jump_if == false) {
                for (int i = 0; i < n; i++) {
                    c_if_cond(comp, pns->nodes[i], false, label);
                }
            } else {
                int label2 = comp_next_label(comp);
                for (int i = 0; i < n - 1; i++) {
                    c_if_cond(comp, pns->nodes[i], false, label2);
                }
                c_if_cond(comp, pns->nodes[n - 1], true, label);
                EMIT_ARG(label_assign, label2);
            }
            return;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_not_test_2) {
            c_if_cond(comp, pns->nodes[0], !jump_if, label);
            return;
        }
    }

    // nothing special, fall back to default compiling for node and jump
    compile_node(comp, pn);
    if (jump_if == false) {
        EMIT_ARG(pop_jump_if_false, label);
    } else {
        EMIT_ARG(pop_jump_if_true, label);
    }
#endif
}

typedef enum { ASSIGN_STORE, ASSIGN_AUG_LOAD, ASSIGN_AUG_STORE } assign_kind_t;
void c_assign(compiler_t *comp, mp_parse_node_t pn, assign_kind_t kind);

void c_assign_power(compiler_t *comp, mp_parse_node_struct_t *pns, assign_kind_t assign_kind) {
    if (assign_kind != ASSIGN_AUG_STORE) {
        compile_node(comp, pns->nodes[0]);
    }

    if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
        mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
        if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_power_trailers) {
            int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns1);
            if (assign_kind != ASSIGN_AUG_STORE) {
                for (int i = 0; i < n - 1; i++) {
                    compile_node(comp, pns1->nodes[i]);
                }
            }
            assert(MP_PARSE_NODE_IS_STRUCT(pns1->nodes[n - 1]));
            pns1 = (mp_parse_node_struct_t*)pns1->nodes[n - 1];
        }
        if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_paren) {
            printf("SyntaxError: can't assign to function call\n");
            return;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_bracket) {
            if (assign_kind == ASSIGN_AUG_STORE) {
                EMIT(rot_three);
                EMIT(store_subscr);
            } else {
                compile_node(comp, pns1->nodes[0]);
                if (assign_kind == ASSIGN_AUG_LOAD) {
                    EMIT(dup_top_two);
                    EMIT_ARG(binary_op, RT_BINARY_OP_SUBSCR);
                } else {
                    EMIT(store_subscr);
                }
            }
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_period) {
            assert(MP_PARSE_NODE_IS_ID(pns1->nodes[0]));
            if (assign_kind == ASSIGN_AUG_LOAD) {
                EMIT(dup_top);
                EMIT_ARG(load_attr, MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]));
            } else {
                if (assign_kind == ASSIGN_AUG_STORE) {
                    EMIT(rot_two);
                }
                EMIT_ARG(store_attr, MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]));
            }
        } else {
            // shouldn't happen
            assert(0);
        }
    } else {
        // shouldn't happen
        assert(0);
    }

    if (!MP_PARSE_NODE_IS_NULL(pns->nodes[2])) {
        // SyntaxError, cannot assign
        assert(0);
    }
}

void c_assign_tuple(compiler_t *comp, int n, mp_parse_node_t *nodes) {
    assert(n >= 0);
    int have_star_index = -1;
    for (int i = 0; i < n; i++) {
        if (MP_PARSE_NODE_IS_STRUCT_KIND(nodes[i], PN_star_expr)) {
            if (have_star_index < 0) {
                EMIT_ARG(unpack_ex, i, n - i - 1);
                have_star_index = i;
            } else {
                printf("SyntaxError: two starred expressions in assignment\n");
                return;
            }
        }
    }
    if (have_star_index < 0) {
        EMIT_ARG(unpack_sequence, n);
    }
    for (int i = 0; i < n; i++) {
        if (i == have_star_index) {
            c_assign(comp, ((mp_parse_node_struct_t*)nodes[i])->nodes[0], ASSIGN_STORE);
        } else {
            c_assign(comp, nodes[i], ASSIGN_STORE);
        }
    }
}

// assigns top of stack to pn
void c_assign(compiler_t *comp, mp_parse_node_t pn, assign_kind_t assign_kind) {
    tail_recursion:
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        assert(0);
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        if (MP_PARSE_NODE_IS_ID(pn)) {
            int arg = MP_PARSE_NODE_LEAF_ARG(pn);
            switch (assign_kind) {
                case ASSIGN_STORE:
                case ASSIGN_AUG_STORE:
                    EMIT_ARG(store_id, arg);
                    break;
                case ASSIGN_AUG_LOAD:
                    EMIT_ARG(load_id, arg);
                    break;
            }
        } else {
            printf("SyntaxError: can't assign to literal\n");
            return;
        }
    } else {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        switch (MP_PARSE_NODE_STRUCT_KIND(pns)) {
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
                c_assign_tuple(comp, MP_PARSE_NODE_STRUCT_NUM_NODES(pns), pns->nodes);
                break;

            case PN_atom_paren:
                // lhs is something in parenthesis
                if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                    // empty tuple
                    printf("SyntaxError: can't assign to ()\n");
                    return;
                } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
                    pns = (mp_parse_node_struct_t*)pns->nodes[0];
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
                if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                    // empty list, assignment allowed
                    c_assign_tuple(comp, 0, NULL);
                } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
                    pns = (mp_parse_node_struct_t*)pns->nodes[0];
                    goto testlist_comp;
                } else {
                    // brackets around 1 item
                    c_assign_tuple(comp, 1, &pns->nodes[0]);
                }
                break;

            default:
                printf("unknown assign, %u\n", (uint)MP_PARSE_NODE_STRUCT_KIND(pns));
                assert(0);
        }
        return;

        testlist_comp:
        // lhs is a sequence
        if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
            mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3b) {
                // sequence of one item, with trailing comma
                assert(MP_PARSE_NODE_IS_NULL(pns2->nodes[0]));
                c_assign_tuple(comp, 1, &pns->nodes[0]);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3c) {
                // sequence of many items
                // TODO call c_assign_tuple instead
                int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns2);
                EMIT_ARG(unpack_sequence, 1 + n);
                c_assign(comp, pns->nodes[0], ASSIGN_STORE);
                for (int i = 0; i < n; i++) {
                    c_assign(comp, pns2->nodes[i], ASSIGN_STORE);
                }
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_comp_for) {
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
    // ensure they are closed over in the order defined in the outer scope (mainly to agree with CPython)
    int nfree = 0;
    if (comp->scope_cur->kind != SCOPE_MODULE) {
        for (int i = 0; i < comp->scope_cur->id_info_len; i++) {
            id_info_t *id = &comp->scope_cur->id_info[i];
            if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
                for (int j = 0; j < this_scope->id_info_len; j++) {
                    id_info_t *id2 = &this_scope->id_info[j];
                    if (id2->kind == ID_INFO_KIND_FREE && id->qstr == id2->qstr) {
#if MICROPY_EMIT_CPYTHON
                        EMIT_ARG(load_closure, id->qstr, id->local_num);
#else
                        // in Micro Python we load closures using LOAD_FAST
                        EMIT_ARG(load_fast, id->qstr, id->local_num);
#endif
                        nfree += 1;
                    }
                }
            }
        }
    }
    if (nfree > 0) {
        EMIT_ARG(build_tuple, nfree);
    }

    // make the function/closure
    if (nfree == 0) {
        EMIT_ARG(make_function, this_scope, n_dict_params, n_default_params);
    } else {
        EMIT_ARG(make_closure, this_scope, n_dict_params, n_default_params);
    }
}

void compile_funcdef_param(compiler_t *comp, mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_typedargslist_name)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (!MP_PARSE_NODE_IS_NULL(pns->nodes[2])) {
            // this parameter has a default value
            // in CPython, None (and True, False?) as default parameters are loaded with LOAD_NAME; don't understandy why
            if (comp->have_bare_star) {
                comp->param_pass_num_dict_params += 1;
                if (comp->param_pass == 1) {
                    EMIT_ARG(load_const_id, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]));
                    compile_node(comp, pns->nodes[2]);
                }
            } else {
                comp->param_pass_num_default_params += 1;
                if (comp->param_pass == 2) {
                    compile_node(comp, pns->nodes[2]);
                }
            }
        }
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_typedargslist_star)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
            // bare star
            comp->have_bare_star = true;
        }
    }
}

// leaves function object on stack
// returns function name
qstr compile_funcdef_helper(compiler_t *comp, mp_parse_node_struct_t *pns, uint emit_options) {
    if (comp->pass == PASS_1) {
        // create a new scope for this function
        scope_t *s = scope_new_and_link(comp, SCOPE_FUNCTION, (mp_parse_node_t)pns, emit_options);
        // store the function scope so the compiling function can use it at each pass
        pns->nodes[4] = (mp_parse_node_t)s;
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
qstr compile_classdef_helper(compiler_t *comp, mp_parse_node_struct_t *pns, uint emit_options) {
    if (comp->pass == PASS_1) {
        // create a new scope for this class
        scope_t *s = scope_new_and_link(comp, SCOPE_CLASS, (mp_parse_node_t)pns, emit_options);
        // store the class scope so the compiling function can use it at each pass
        pns->nodes[3] = (mp_parse_node_t)s;
    }

    EMIT(load_build_class);

    // scope for this class
    scope_t *cscope = (scope_t*)pns->nodes[3];

    // compile the class
    close_over_variables_etc(comp, cscope, 0, 0);

    // get its name
    EMIT_ARG(load_const_id, cscope->simple_name);

    // nodes[1] has parent classes, if any
    comp->func_arg_is_super = false;
    compile_trailer_paren_helper(comp, pns->nodes[1], false, 2);

    // return its name (the 'C' in class C(...):")
    return cscope->simple_name;
}

// returns true if it was a built-in decorator (even if the built-in had an error)
STATIC bool compile_built_in_decorator(compiler_t *comp, int name_len, mp_parse_node_t *name_nodes, uint *emit_options) {
    if (MP_PARSE_NODE_LEAF_ARG(name_nodes[0]) != MP_QSTR_micropython) {
        return false;
    }

    if (name_len != 2) {
        printf("SyntaxError: invalid micropython decorator\n");
        return true;
    }

    qstr attr = MP_PARSE_NODE_LEAF_ARG(name_nodes[1]);
    if (attr == MP_QSTR_byte_code) {
        *emit_options = EMIT_OPT_BYTE_CODE;
#if MICROPY_EMIT_NATIVE
    } else if (attr == MP_QSTR_native) {
        *emit_options = EMIT_OPT_NATIVE_PYTHON;
    } else if (attr == MP_QSTR_viper) {
        *emit_options = EMIT_OPT_VIPER;
#endif
#if MICROPY_EMIT_INLINE_THUMB
    } else if (attr == MP_QSTR_asm_thumb) {
        *emit_options = EMIT_OPT_ASM_THUMB;
#endif
    } else {
        printf("SyntaxError: invalid micropython decorator '%s'\n", qstr_str(attr));
    }

    return true;
}

void compile_decorated(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // get the list of decorators
    mp_parse_node_t *nodes;
    int n = list_get(&pns->nodes[0], PN_decorators, &nodes);

    // inherit emit options for this function/class definition
    uint emit_options = comp->scope_cur->emit_options;

    // compile each decorator
    int num_built_in_decorators = 0;
    for (int i = 0; i < n; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(nodes[i], PN_decorator)); // should be
        mp_parse_node_struct_t *pns_decorator = (mp_parse_node_struct_t*)nodes[i];

        // nodes[0] contains the decorator function, which is a dotted name
        mp_parse_node_t *name_nodes;
        int name_len = list_get(&pns_decorator->nodes[0], PN_dotted_name, &name_nodes);

        // check for built-in decorators
        if (compile_built_in_decorator(comp, name_len, name_nodes, &emit_options)) {
            // this was a built-in
            num_built_in_decorators += 1;

        } else {
            // not a built-in, compile normally

            // compile the decorator function
            compile_node(comp, name_nodes[0]);
            for (int i = 1; i < name_len; i++) {
                assert(MP_PARSE_NODE_IS_ID(name_nodes[i])); // should be
                EMIT_ARG(load_attr, MP_PARSE_NODE_LEAF_ARG(name_nodes[i]));
            }

            // nodes[1] contains arguments to the decorator function, if any
            if (!MP_PARSE_NODE_IS_NULL(pns_decorator->nodes[1])) {
                // call the decorator function with the arguments in nodes[1]
                comp->func_arg_is_super = false;
                compile_node(comp, pns_decorator->nodes[1]);
            }
        }
    }

    // compile the body (funcdef or classdef) and get its name
    mp_parse_node_struct_t *pns_body = (mp_parse_node_struct_t*)pns->nodes[1];
    qstr body_name = 0;
    if (MP_PARSE_NODE_STRUCT_KIND(pns_body) == PN_funcdef) {
        body_name = compile_funcdef_helper(comp, pns_body, emit_options);
    } else if (MP_PARSE_NODE_STRUCT_KIND(pns_body) == PN_classdef) {
        body_name = compile_classdef_helper(comp, pns_body, emit_options);
    } else {
        // shouldn't happen
        assert(0);
    }

    // call each decorator
    for (int i = 0; i < n - num_built_in_decorators; i++) {
        EMIT_ARG(call_function, 1, 0, false, false);
    }

    // store func/class object into name
    EMIT_ARG(store_id, body_name);
}

void compile_funcdef(compiler_t *comp, mp_parse_node_struct_t *pns) {
    qstr fname = compile_funcdef_helper(comp, pns, comp->scope_cur->emit_options);
    // store function object into function name
    EMIT_ARG(store_id, fname);
}

void c_del_stmt(compiler_t *comp, mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_ID(pn)) {
        EMIT_ARG(delete_id, MP_PARSE_NODE_LEAF_ARG(pn));
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_power)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;

        compile_node(comp, pns->nodes[0]); // base of the power node

        if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
            mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_power_trailers) {
                int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns1);
                for (int i = 0; i < n - 1; i++) {
                    compile_node(comp, pns1->nodes[i]);
                }
                assert(MP_PARSE_NODE_IS_STRUCT(pns1->nodes[n - 1]));
                pns1 = (mp_parse_node_struct_t*)pns1->nodes[n - 1];
            }
            if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_paren) {
                // SyntaxError: can't delete a function call
                assert(0);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_bracket) {
                compile_node(comp, pns1->nodes[0]);
                EMIT(delete_subscr);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_period) {
                assert(MP_PARSE_NODE_IS_ID(pns1->nodes[0]));
                EMIT_ARG(delete_attr, MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]));
            } else {
                // shouldn't happen
                assert(0);
            }
        } else {
            // shouldn't happen
            assert(0);
        }

        if (!MP_PARSE_NODE_IS_NULL(pns->nodes[2])) {
            // SyntaxError, cannot delete
            assert(0);
        }
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_atom_paren)) {
        pn = ((mp_parse_node_struct_t*)pn)->nodes[0];
        if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_testlist_comp)) {
            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
            // TODO perhaps factorise testlist_comp code with other uses of PN_testlist_comp

            if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
                mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
                if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_testlist_comp_3b) {
                    // sequence of one item, with trailing comma
                    assert(MP_PARSE_NODE_IS_NULL(pns1->nodes[0]));
                    c_del_stmt(comp, pns->nodes[0]);
                } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_testlist_comp_3c) {
                    // sequence of many items
                    int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns1);
                    c_del_stmt(comp, pns->nodes[0]);
                    for (int i = 0; i < n; i++) {
                        c_del_stmt(comp, pns1->nodes[i]);
                    }
                } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_comp_for) {
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

void compile_del_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    apply_to_single_or_list(comp, pns->nodes[0], PN_exprlist, c_del_stmt);
}

void compile_break_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->break_label == 0) {
        printf("ERROR: cannot break from here\n");
    }
    EMIT_ARG(break_loop, comp->break_label, comp->cur_except_level - comp->break_continue_except_level);
}

void compile_continue_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->continue_label == 0) {
        printf("ERROR: cannot continue from here\n");
    }
    EMIT_ARG(continue_loop, comp->continue_label, comp->cur_except_level - comp->break_continue_except_level);
}

void compile_return_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION) {
        printf("SyntaxError: 'return' outside function\n");
        comp->had_error = true;
        return;
    }
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // no argument to 'return', so return None
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_test_if_expr)) {
        // special case when returning an if-expression; to match CPython optimisation
        mp_parse_node_struct_t *pns_test_if_expr = (mp_parse_node_struct_t*)pns->nodes[0];
        mp_parse_node_struct_t *pns_test_if_else = (mp_parse_node_struct_t*)pns_test_if_expr->nodes[1];

        int l_fail = comp_next_label(comp);
        c_if_cond(comp, pns_test_if_else->nodes[0], false, l_fail); // condition
        compile_node(comp, pns_test_if_expr->nodes[0]); // success value
        EMIT(return_value);
        EMIT_ARG(label_assign, l_fail);
        compile_node(comp, pns_test_if_else->nodes[1]); // failure value
    } else {
        compile_node(comp, pns->nodes[0]);
    }
    EMIT(return_value);
}

void compile_yield_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]);
    EMIT(pop_top);
}

void compile_raise_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // raise
        EMIT_ARG(raise_varargs, 0);
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_raise_stmt_arg)) {
        // raise x from y
        pns = (mp_parse_node_struct_t*)pns->nodes[0];
        compile_node(comp, pns->nodes[0]);
        compile_node(comp, pns->nodes[1]);
        EMIT_ARG(raise_varargs, 2);
    } else {
        // raise x
        compile_node(comp, pns->nodes[0]);
        EMIT_ARG(raise_varargs, 1);
    }
}

// q1 holds the base, q2 the full name
// eg   a -> q1=q2=a
//      a.b.c -> q1=a, q2=a.b.c
void do_import_name(compiler_t *comp, mp_parse_node_t pn, qstr *q1, qstr *q2) {
    bool is_as = false;
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_dotted_as_name)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        // a name of the form x as y; unwrap it
        *q1 = MP_PARSE_NODE_LEAF_ARG(pns->nodes[1]);
        pn = pns->nodes[0];
        is_as = true;
    }
    if (MP_PARSE_NODE_IS_ID(pn)) {
        // just a simple name
        *q2 = MP_PARSE_NODE_LEAF_ARG(pn);
        if (!is_as) {
            *q1 = *q2;
        }
        EMIT_ARG(import_name, *q2);
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_dotted_name) {
            // a name of the form a.b.c
            if (!is_as) {
                *q1 = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            }
            int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
            int len = n - 1;
            for (int i = 0; i < n; i++) {
                len += qstr_len(MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
            }
            byte *q_ptr;
            byte *str_dest = qstr_build_start(len, &q_ptr);
            for (int i = 0; i < n; i++) {
                if (i > 0) {
                    *str_dest++ = '.';
                }
                uint str_src_len;
                const byte *str_src = qstr_data(MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]), &str_src_len);
                memcpy(str_dest, str_src, str_src_len);
                str_dest += str_src_len;
            }
            *q2 = qstr_build_end(q_ptr);
            EMIT_ARG(import_name, *q2);
            if (is_as) {
                for (int i = 1; i < n; i++) {
                    EMIT_ARG(load_attr, MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
                }
            }
        } else {
            // TODO not implemented
            // This covers relative imports starting with dot(s) like "from .foo import"
            assert(0);
        }
    } else {
        // TODO not implemented
        // This covers relative imports with dots only like "from .. import"
        assert(0);
    }
}

void compile_dotted_as_name(compiler_t *comp, mp_parse_node_t pn) {
    EMIT_ARG(load_const_small_int, 0); // ??
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    qstr q1, q2;
    do_import_name(comp, pn, &q1, &q2);
    EMIT_ARG(store_id, q1);
}

void compile_import_name(compiler_t *comp, mp_parse_node_struct_t *pns) {
    apply_to_single_or_list(comp, pns->nodes[0], PN_dotted_as_names, compile_dotted_as_name);
}

void compile_import_from(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_STAR)) {
        EMIT_ARG(load_const_small_int, 0); // level 0 for __import__

        // build the "fromlist" tuple
#if MICROPY_EMIT_CPYTHON
        EMIT_ARG(load_const_verbatim_str, "('*',)");
#else
        EMIT_ARG(load_const_str, QSTR_FROM_STR_STATIC("*"), false);
        EMIT_ARG(build_tuple, 1);
#endif

        // do the import
        qstr dummy_q, id1;
        do_import_name(comp, pns->nodes[0], &dummy_q, &id1);
        EMIT(import_star);

    } else {
        EMIT_ARG(load_const_small_int, 0); // level 0 for __import__

        // build the "fromlist" tuple
        mp_parse_node_t *pn_nodes;
        int n = list_get(&pns->nodes[1], PN_import_as_names, &pn_nodes);
#if MICROPY_EMIT_CPYTHON
        {
            vstr_t *vstr = vstr_new();
            vstr_printf(vstr, "(");
            for (int i = 0; i < n; i++) {
                assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_nodes[i], PN_import_as_name));
                mp_parse_node_struct_t *pns3 = (mp_parse_node_struct_t*)pn_nodes[i];
                qstr id2 = MP_PARSE_NODE_LEAF_ARG(pns3->nodes[0]); // should be id
                if (i > 0) {
                    vstr_printf(vstr, ", ");
                }
                vstr_printf(vstr, "'");
                uint len;
                const byte *str = qstr_data(id2, &len);
                vstr_add_strn(vstr, (const char*)str, len);
                vstr_printf(vstr, "'");
            }
            if (n == 1) {
                vstr_printf(vstr, ",");
            }
            vstr_printf(vstr, ")");
            EMIT_ARG(load_const_verbatim_str, vstr_str(vstr));
            vstr_free(vstr);
        }
#else
        for (int i = 0; i < n; i++) {
            assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_nodes[i], PN_import_as_name));
            mp_parse_node_struct_t *pns3 = (mp_parse_node_struct_t*)pn_nodes[i];
            qstr id2 = MP_PARSE_NODE_LEAF_ARG(pns3->nodes[0]); // should be id
            EMIT_ARG(load_const_str, id2, false);
        }
        EMIT_ARG(build_tuple, n);
#endif

        // do the import
        qstr dummy_q, id1;
        do_import_name(comp, pns->nodes[0], &dummy_q, &id1);
        for (int i = 0; i < n; i++) {
            assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_nodes[i], PN_import_as_name));
            mp_parse_node_struct_t *pns3 = (mp_parse_node_struct_t*)pn_nodes[i];
            qstr id2 = MP_PARSE_NODE_LEAF_ARG(pns3->nodes[0]); // should be id
            EMIT_ARG(import_from, id2);
            if (MP_PARSE_NODE_IS_NULL(pns3->nodes[1])) {
                EMIT_ARG(store_id, id2);
            } else {
                EMIT_ARG(store_id, MP_PARSE_NODE_LEAF_ARG(pns3->nodes[1]));
            }
        }
        EMIT(pop_top);
    }
}

void compile_global_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->pass == PASS_1) {
        if (MP_PARSE_NODE_IS_LEAF(pns->nodes[0])) {
            scope_declare_global(comp->scope_cur, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]));
        } else {
            pns = (mp_parse_node_struct_t*)pns->nodes[0];
            int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
            for (int i = 0; i < num_nodes; i++) {
                scope_declare_global(comp->scope_cur, MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
            }
        }
    }
}

void compile_nonlocal_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->pass == PASS_1) {
        if (MP_PARSE_NODE_IS_LEAF(pns->nodes[0])) {
            scope_declare_nonlocal(comp->scope_cur, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]));
        } else {
            pns = (mp_parse_node_struct_t*)pns->nodes[0];
            int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
            for (int i = 0; i < num_nodes; i++) {
                scope_declare_nonlocal(comp->scope_cur, MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
            }
        }
    }
}

void compile_assert_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int l_end = comp_next_label(comp);
    c_if_cond(comp, pns->nodes[0], true, l_end);
    EMIT_ARG(load_global, MP_QSTR_AssertionError); // we load_global instead of load_id, to be consistent with CPython
    if (!MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        // assertion message
        compile_node(comp, pns->nodes[1]);
        EMIT_ARG(call_function, 1, 0, false, false);
    }
    EMIT_ARG(raise_varargs, 1);
    EMIT_ARG(label_assign, l_end);
}

void compile_if_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // TODO proper and/or short circuiting

    int l_end = comp_next_label(comp);

    int l_fail = comp_next_label(comp);
    c_if_cond(comp, pns->nodes[0], false, l_fail); // if condition

    compile_node(comp, pns->nodes[1]); // if block
    //if (!(MP_PARSE_NODE_IS_NULL(pns->nodes[2]) && MP_PARSE_NODE_IS_NULL(pns->nodes[3]))) { // optimisation; doesn't align with CPython
        // jump over elif/else blocks if they exist
        if (!EMIT(last_emit_was_return_value)) { // simple optimisation to align with CPython
            EMIT_ARG(jump, l_end);
        }
    //}
    EMIT_ARG(label_assign, l_fail);

    if (!MP_PARSE_NODE_IS_NULL(pns->nodes[2])) {
        // compile elif blocks

        mp_parse_node_struct_t *pns_elif = (mp_parse_node_struct_t*)pns->nodes[2];

        if (MP_PARSE_NODE_STRUCT_KIND(pns_elif) == PN_if_stmt_elif_list) {
            // multiple elif blocks

            int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns_elif);
            for (int i = 0; i < n; i++) {
                mp_parse_node_struct_t *pns_elif2 = (mp_parse_node_struct_t*)pns_elif->nodes[i];
                l_fail = comp_next_label(comp);
                c_if_cond(comp, pns_elif2->nodes[0], false, l_fail); // elif condition

                compile_node(comp, pns_elif2->nodes[1]); // elif block
                if (!EMIT(last_emit_was_return_value)) { // simple optimisation to align with CPython
                    EMIT_ARG(jump, l_end);
                }
                EMIT_ARG(label_assign, l_fail);
            }

        } else {
            // a single elif block

            l_fail = comp_next_label(comp);
            c_if_cond(comp, pns_elif->nodes[0], false, l_fail); // elif condition

            compile_node(comp, pns_elif->nodes[1]); // elif block
            if (!EMIT(last_emit_was_return_value)) { // simple optimisation to align with CPython
                EMIT_ARG(jump, l_end);
            }
            EMIT_ARG(label_assign, l_fail);
        }
    }

    // compile else block
    compile_node(comp, pns->nodes[3]); // can be null

    EMIT_ARG(label_assign, l_end);
}

#define START_BREAK_CONTINUE_BLOCK \
    int old_break_label = comp->break_label; \
    int old_continue_label = comp->continue_label; \
    int break_label = comp_next_label(comp); \
    int continue_label = comp_next_label(comp); \
    comp->break_label = break_label; \
    comp->continue_label = continue_label; \
    comp->break_continue_except_level = comp->cur_except_level;

#define END_BREAK_CONTINUE_BLOCK \
    comp->break_label = old_break_label; \
    comp->continue_label = old_continue_label; \
    comp->break_continue_except_level = comp->cur_except_level;

void compile_while_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    START_BREAK_CONTINUE_BLOCK

    // compared to CPython, we have an optimised version of while loops
#if MICROPY_EMIT_CPYTHON
    int done_label = comp_next_label(comp);
    EMIT_ARG(setup_loop, break_label);
    EMIT_ARG(label_assign, continue_label);
    c_if_cond(comp, pns->nodes[0], false, done_label); // condition
    compile_node(comp, pns->nodes[1]); // body
    if (!EMIT(last_emit_was_return_value)) {
        EMIT_ARG(jump, continue_label);
    }
    EMIT_ARG(label_assign, done_label);
    // CPython does not emit POP_BLOCK if the condition was a constant; don't undertand why
    // this is a small hack to agree with CPython
    if (!node_is_const_true(pns->nodes[0])) {
        EMIT(pop_block);
    }
#else
    int top_label = comp_next_label(comp);
    EMIT_ARG(jump, continue_label);
    EMIT_ARG(label_assign, top_label);
    compile_node(comp, pns->nodes[1]); // body
    EMIT_ARG(label_assign, continue_label);
    c_if_cond(comp, pns->nodes[0], true, top_label); // condition
#endif

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    compile_node(comp, pns->nodes[2]); // else

    EMIT_ARG(label_assign, break_label);
}

// TODO preload end and step onto stack if they are not constants
// TODO check if step is negative and do opposite test
void compile_for_stmt_optimised_range(compiler_t *comp, mp_parse_node_t pn_var, mp_parse_node_t pn_start, mp_parse_node_t pn_end, mp_parse_node_t pn_step, mp_parse_node_t pn_body, mp_parse_node_t pn_else) {
    START_BREAK_CONTINUE_BLOCK

    int top_label = comp_next_label(comp);
    int entry_label = comp_next_label(comp);

    // compile: var = start
    compile_node(comp, pn_start);
    c_assign(comp, pn_var, ASSIGN_STORE);

    EMIT_ARG(jump, entry_label);
    EMIT_ARG(label_assign, top_label);

    // compile body
    compile_node(comp, pn_body);

    EMIT_ARG(label_assign, continue_label);

    // compile: var += step
    c_assign(comp, pn_var, ASSIGN_AUG_LOAD);
    compile_node(comp, pn_step);
    EMIT_ARG(binary_op, RT_BINARY_OP_INPLACE_ADD);
    c_assign(comp, pn_var, ASSIGN_AUG_STORE);

    EMIT_ARG(label_assign, entry_label);

    // compile: if var <cond> end: goto top
    compile_node(comp, pn_var);
    compile_node(comp, pn_end);
    if (MP_PARSE_NODE_LEAF_ARG(pn_step) >= 0) {
        EMIT_ARG(binary_op, RT_BINARY_OP_LESS);
    } else {
        EMIT_ARG(binary_op, RT_BINARY_OP_MORE);
    }
    EMIT_ARG(pop_jump_if_true, top_label);

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    compile_node(comp, pn_else);

    EMIT_ARG(label_assign, break_label);
}

void compile_for_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
#if !MICROPY_EMIT_CPYTHON
    // this bit optimises: for <x> in range(...), turning it into an explicitly incremented variable
    // this is actually slower, but uses no heap memory
    // for viper it will be much, much faster
    if (/*comp->scope_cur->emit_options == EMIT_OPT_VIPER &&*/ MP_PARSE_NODE_IS_ID(pns->nodes[0]) && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_power)) {
        mp_parse_node_struct_t *pns_it = (mp_parse_node_struct_t*)pns->nodes[1];
        if (MP_PARSE_NODE_IS_ID(pns_it->nodes[0]) 
            && MP_PARSE_NODE_LEAF_ARG(pns_it->nodes[0]) == MP_QSTR_range
            && MP_PARSE_NODE_IS_STRUCT_KIND(pns_it->nodes[1], PN_trailer_paren)
            && MP_PARSE_NODE_IS_NULL(pns_it->nodes[2])) {
            mp_parse_node_t pn_range_args = ((mp_parse_node_struct_t*)pns_it->nodes[1])->nodes[0];
            mp_parse_node_t *args;
            int n_args = list_get(&pn_range_args, PN_arglist, &args);
            mp_parse_node_t pn_range_start;
            mp_parse_node_t pn_range_end;
            mp_parse_node_t pn_range_step;
            bool optimize = false;
            if (1 <= n_args && n_args <= 3) {
                optimize = true;
                if (n_args == 1) {
                    pn_range_start = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, 0);
                    pn_range_end = args[0];
                    pn_range_step = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, 1);
                } else if (n_args == 2) {
                    pn_range_start = args[0];
                    pn_range_end = args[1];
                    pn_range_step = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, 1);
                } else {
                    pn_range_start = args[0];
                    pn_range_end = args[1];
                    pn_range_step = args[2];
                    // We need to know sign of step. This is possible only if it's constant
                    if (!MP_PARSE_NODE_IS_SMALL_INT(pn_range_step)) {
                        optimize = false;
                    }
                }
            }
            if (optimize) {
                compile_for_stmt_optimised_range(comp, pns->nodes[0], pn_range_start, pn_range_end, pn_range_step, pns->nodes[2], pns->nodes[3]);
                return;
            }
        }
    }
#endif

    START_BREAK_CONTINUE_BLOCK

    int pop_label = comp_next_label(comp);
    int end_label = comp_next_label(comp);

    // I don't think our implementation needs SETUP_LOOP/POP_BLOCK for for-statements
#if MICROPY_EMIT_CPYTHON
    EMIT_ARG(setup_loop, end_label);
#endif

    compile_node(comp, pns->nodes[1]); // iterator
    EMIT(get_iter);
    EMIT_ARG(label_assign, continue_label);
    EMIT_ARG(for_iter, pop_label);
    c_assign(comp, pns->nodes[0], ASSIGN_STORE); // variable
    compile_node(comp, pns->nodes[2]); // body
    if (!EMIT(last_emit_was_return_value)) {
        EMIT_ARG(jump, continue_label);
    }
    EMIT_ARG(label_assign, pop_label);
    EMIT(for_iter_end);

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

#if MICROPY_EMIT_CPYTHON
    EMIT(pop_block);
#endif

    compile_node(comp, pns->nodes[3]); // else (not tested)

    EMIT_ARG(label_assign, break_label);
    EMIT_ARG(label_assign, end_label);
}

void compile_try_except(compiler_t *comp, mp_parse_node_t pn_body, int n_except, mp_parse_node_t *pn_excepts, mp_parse_node_t pn_else) {
    // this function is a bit of a hack at the moment
    // don't understand how the stack works with exceptions, so we force it to return to the correct value

    // setup code
    int stack_size = EMIT(get_stack_size);
    int l1 = comp_next_label(comp);
    int success_label = comp_next_label(comp);

    EMIT_ARG(setup_except, l1);
    comp->cur_except_level += 1;

    compile_node(comp, pn_body); // body
    EMIT(pop_block);
    EMIT_ARG(jump, success_label);
    EMIT_ARG(label_assign, l1);
    int l2 = comp_next_label(comp);

    for (int i = 0; i < n_except; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_excepts[i], PN_try_stmt_except)); // should be
        mp_parse_node_struct_t *pns_except = (mp_parse_node_struct_t*)pn_excepts[i];

        qstr qstr_exception_local = 0;
        int end_finally_label = comp_next_label(comp);

        if (MP_PARSE_NODE_IS_NULL(pns_except->nodes[0])) {
            // this is a catch all exception handler
            if (i + 1 != n_except) {
                printf("SyntaxError: default 'except:' must be last\n");
                return;
            }
        } else {
            // this exception handler requires a match to a certain type of exception
            mp_parse_node_t pns_exception_expr = pns_except->nodes[0];
            if (MP_PARSE_NODE_IS_STRUCT(pns_exception_expr)) {
                mp_parse_node_struct_t *pns3 = (mp_parse_node_struct_t*)pns_exception_expr;
                if (MP_PARSE_NODE_STRUCT_KIND(pns3) == PN_try_stmt_as_name) {
                    // handler binds the exception to a local
                    pns_exception_expr = pns3->nodes[0];
                    qstr_exception_local = MP_PARSE_NODE_LEAF_ARG(pns3->nodes[1]);
                }
            }
            EMIT(dup_top);
            compile_node(comp, pns_exception_expr);
            EMIT_ARG(binary_op, RT_BINARY_OP_EXCEPTION_MATCH);
            EMIT_ARG(pop_jump_if_false, end_finally_label);
        }

        EMIT(pop_top);

        if (qstr_exception_local == 0) {
            EMIT(pop_top);
        } else {
            EMIT_ARG(store_id, qstr_exception_local);
        }

        EMIT(pop_top);

        int l3 = 0;
        if (qstr_exception_local != 0) {
            l3 = comp_next_label(comp);
            EMIT_ARG(setup_finally, l3);
            comp->cur_except_level += 1;
        }
        compile_node(comp, pns_except->nodes[1]);
        if (qstr_exception_local != 0) {
            EMIT(pop_block);
        }
        EMIT(pop_except);
        if (qstr_exception_local != 0) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
            EMIT_ARG(label_assign, l3);
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
            EMIT_ARG(store_id, qstr_exception_local);
            EMIT_ARG(delete_id, qstr_exception_local);

            comp->cur_except_level -= 1;
            EMIT(end_finally);
        }
        EMIT_ARG(jump, l2);
        EMIT_ARG(label_assign, end_finally_label);
    }

    comp->cur_except_level -= 1;
    EMIT(end_finally);

    EMIT_ARG(label_assign, success_label);
    compile_node(comp, pn_else); // else block, can be null
    EMIT_ARG(label_assign, l2);
    EMIT_ARG(set_stack_size, stack_size);
}

void compile_try_finally(compiler_t *comp, mp_parse_node_t pn_body, int n_except, mp_parse_node_t *pn_except, mp_parse_node_t pn_else, mp_parse_node_t pn_finally) {
    // don't understand how the stack works with exceptions, so we force it to return to the correct value
    int stack_size = EMIT(get_stack_size);
    int l_finally_block = comp_next_label(comp);

    EMIT_ARG(setup_finally, l_finally_block);
    comp->cur_except_level += 1;

    if (n_except == 0) {
        assert(MP_PARSE_NODE_IS_NULL(pn_else));
        compile_node(comp, pn_body);
    } else {
        compile_try_except(comp, pn_body, n_except, pn_except, pn_else);
    }
    EMIT(pop_block);
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    EMIT_ARG(label_assign, l_finally_block);
    compile_node(comp, pn_finally);

    comp->cur_except_level -= 1;
    EMIT(end_finally);

    EMIT_ARG(set_stack_size, stack_size);
}

void compile_try_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
        mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[1];
        if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_try_stmt_finally) {
            // just try-finally
            compile_try_finally(comp, pns->nodes[0], 0, NULL, MP_PARSE_NODE_NULL, pns2->nodes[0]);
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_try_stmt_except_and_more) {
            // try-except and possibly else and/or finally
            mp_parse_node_t *pn_excepts;
            int n_except = list_get(&pns2->nodes[0], PN_try_stmt_except_list, &pn_excepts);
            if (MP_PARSE_NODE_IS_NULL(pns2->nodes[2])) {
                // no finally
                compile_try_except(comp, pns->nodes[0], n_except, pn_excepts, pns2->nodes[1]);
            } else {
                // have finally
                compile_try_finally(comp, pns->nodes[0], n_except, pn_excepts, pns2->nodes[1], ((mp_parse_node_struct_t*)pns2->nodes[2])->nodes[0]);
            }
        } else {
            // just try-except
            mp_parse_node_t *pn_excepts;
            int n_except = list_get(&pns->nodes[1], PN_try_stmt_except_list, &pn_excepts);
            compile_try_except(comp, pns->nodes[0], n_except, pn_excepts, MP_PARSE_NODE_NULL);
        }
    } else {
        // shouldn't happen
        assert(0);
    }
}

void compile_with_stmt_helper(compiler_t *comp, int n, mp_parse_node_t *nodes, mp_parse_node_t body) {
    if (n == 0) {
        // no more pre-bits, compile the body of the with
        compile_node(comp, body);
    } else {
        int l_end = comp_next_label(comp);
        if (MP_PARSE_NODE_IS_STRUCT_KIND(nodes[0], PN_with_item)) {
            // this pre-bit is of the form "a as b"
            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)nodes[0];
            compile_node(comp, pns->nodes[0]);
            EMIT_ARG(setup_with, l_end);
            c_assign(comp, pns->nodes[1], ASSIGN_STORE);
        } else {
            // this pre-bit is just an expression
            compile_node(comp, nodes[0]);
            EMIT_ARG(setup_with, l_end);
            EMIT(pop_top);
        }
        // compile additional pre-bits and the body
        compile_with_stmt_helper(comp, n - 1, nodes + 1, body);
        // finish this with block
        EMIT(pop_block);
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT_ARG(label_assign, l_end);
        EMIT(with_cleanup);
        EMIT(end_finally);
    }
}

void compile_with_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // get the nodes for the pre-bit of the with (the a as b, c as d, ... bit)
    mp_parse_node_t *nodes;
    int n = list_get(&pns->nodes[0], PN_with_stmt_list, &nodes);
    assert(n > 0);

    // compile in a nested fashion
    compile_with_stmt_helper(comp, n, nodes, pns->nodes[1]);
}

void compile_expr_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        if (comp->is_repl && comp->scope_cur->kind == SCOPE_MODULE) {
            // for REPL, evaluate then print the expression
            EMIT_ARG(load_id, MP_QSTR___repl_print__);
            compile_node(comp, pns->nodes[0]);
            EMIT_ARG(call_function, 1, 0, false, false);
            EMIT(pop_top);

        } else {
            // for non-REPL, evaluate then discard the expression
            if (MP_PARSE_NODE_IS_LEAF(pns->nodes[0]) && !MP_PARSE_NODE_IS_ID(pns->nodes[0])) {
                // do nothing with a lonely constant
            } else {
                compile_node(comp, pns->nodes[0]); // just an expression
                EMIT(pop_top); // discard last result since this is a statement and leaves nothing on the stack
            }
        }
    } else {
        mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
        int kind = MP_PARSE_NODE_STRUCT_KIND(pns1);
        if (kind == PN_expr_stmt_augassign) {
            c_assign(comp, pns->nodes[0], ASSIGN_AUG_LOAD); // lhs load for aug assign
            compile_node(comp, pns1->nodes[1]); // rhs
            assert(MP_PARSE_NODE_IS_TOKEN(pns1->nodes[0]));
            rt_binary_op_t op;
            switch (MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0])) {
                case MP_TOKEN_DEL_PIPE_EQUAL: op = RT_BINARY_OP_INPLACE_OR; break;
                case MP_TOKEN_DEL_CARET_EQUAL: op = RT_BINARY_OP_INPLACE_XOR; break;
                case MP_TOKEN_DEL_AMPERSAND_EQUAL: op = RT_BINARY_OP_INPLACE_AND; break;
                case MP_TOKEN_DEL_DBL_LESS_EQUAL: op = RT_BINARY_OP_INPLACE_LSHIFT; break;
                case MP_TOKEN_DEL_DBL_MORE_EQUAL: op = RT_BINARY_OP_INPLACE_RSHIFT; break;
                case MP_TOKEN_DEL_PLUS_EQUAL: op = RT_BINARY_OP_INPLACE_ADD; break;
                case MP_TOKEN_DEL_MINUS_EQUAL: op = RT_BINARY_OP_INPLACE_SUBTRACT; break;
                case MP_TOKEN_DEL_STAR_EQUAL: op = RT_BINARY_OP_INPLACE_MULTIPLY; break;
                case MP_TOKEN_DEL_DBL_SLASH_EQUAL: op = RT_BINARY_OP_INPLACE_FLOOR_DIVIDE; break;
                case MP_TOKEN_DEL_SLASH_EQUAL: op = RT_BINARY_OP_INPLACE_TRUE_DIVIDE; break;
                case MP_TOKEN_DEL_PERCENT_EQUAL: op = RT_BINARY_OP_INPLACE_MODULO; break;
                case MP_TOKEN_DEL_DBL_STAR_EQUAL: op = RT_BINARY_OP_INPLACE_POWER; break;
                default: assert(0); op = RT_BINARY_OP_INPLACE_OR; // shouldn't happen
            }
            EMIT_ARG(binary_op, op);
            c_assign(comp, pns->nodes[0], ASSIGN_AUG_STORE); // lhs store for aug assign
        } else if (kind == PN_expr_stmt_assign_list) {
            int rhs = MP_PARSE_NODE_STRUCT_NUM_NODES(pns1) - 1;
            compile_node(comp, ((mp_parse_node_struct_t*)pns1->nodes[rhs])->nodes[0]); // rhs
            // following CPython, we store left-most first
            if (rhs > 0) {
                EMIT(dup_top);
            }
            c_assign(comp, pns->nodes[0], ASSIGN_STORE); // lhs store
            for (int i = 0; i < rhs; i++) {
                if (i + 1 < rhs) {
                    EMIT(dup_top);
                }
                c_assign(comp, ((mp_parse_node_struct_t*)pns1->nodes[i])->nodes[0], ASSIGN_STORE); // middle store
            }
        } else if (kind == PN_expr_stmt_assign) {
            if (MP_PARSE_NODE_IS_STRUCT_KIND(pns1->nodes[0], PN_testlist_star_expr)
                && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_star_expr)
                && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns1->nodes[0]) == 2
                && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns->nodes[0]) == 2) {
                // optimisation for a, b = c, d; to match CPython's optimisation
                mp_parse_node_struct_t* pns10 = (mp_parse_node_struct_t*)pns1->nodes[0];
                mp_parse_node_struct_t* pns0 = (mp_parse_node_struct_t*)pns->nodes[0];
                compile_node(comp, pns10->nodes[0]); // rhs
                compile_node(comp, pns10->nodes[1]); // rhs
                EMIT(rot_two);
                c_assign(comp, pns0->nodes[0], ASSIGN_STORE); // lhs store
                c_assign(comp, pns0->nodes[1], ASSIGN_STORE); // lhs store
            } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns1->nodes[0], PN_testlist_star_expr)
                && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_star_expr)
                && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns1->nodes[0]) == 3
                && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns->nodes[0]) == 3) {
                // optimisation for a, b, c = d, e, f; to match CPython's optimisation
                mp_parse_node_struct_t* pns10 = (mp_parse_node_struct_t*)pns1->nodes[0];
                mp_parse_node_struct_t* pns0 = (mp_parse_node_struct_t*)pns->nodes[0];
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

void c_binary_op(compiler_t *comp, mp_parse_node_struct_t *pns, rt_binary_op_t binary_op) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i < num_nodes; i += 1) {
        compile_node(comp, pns->nodes[i]);
        EMIT_ARG(binary_op, binary_op);
    }
}

void compile_test_if_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_test_if_else));
    mp_parse_node_struct_t *pns_test_if_else = (mp_parse_node_struct_t*)pns->nodes[1];

    int stack_size = EMIT(get_stack_size);
    int l_fail = comp_next_label(comp);
    int l_end = comp_next_label(comp);
    c_if_cond(comp, pns_test_if_else->nodes[0], false, l_fail); // condition
    compile_node(comp, pns->nodes[0]); // success value
    EMIT_ARG(jump, l_end);
    EMIT_ARG(label_assign, l_fail);
    EMIT_ARG(set_stack_size, stack_size); // force stack size reset
    compile_node(comp, pns_test_if_else->nodes[1]); // failure value
    EMIT_ARG(label_assign, l_end);
}

void compile_lambdef(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // TODO default params etc for lambda; possibly just use funcdef code
    //mp_parse_node_t pn_params = pns->nodes[0];
    //mp_parse_node_t pn_body = pns->nodes[1];

    if (comp->pass == PASS_1) {
        // create a new scope for this lambda
        scope_t *s = scope_new_and_link(comp, SCOPE_LAMBDA, (mp_parse_node_t)pns, comp->scope_cur->emit_options);
        // store the lambda scope so the compiling function (this one) can use it at each pass
        pns->nodes[2] = (mp_parse_node_t)s;
    }

    // get the scope for this lambda
    scope_t *this_scope = (scope_t*)pns->nodes[2];

    // make the lambda
    close_over_variables_etc(comp, this_scope, 0, 0);
}

void compile_or_test(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int l_end = comp_next_label(comp);
    int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < n; i += 1) {
        compile_node(comp, pns->nodes[i]);
        if (i + 1 < n) {
            EMIT_ARG(jump_if_true_or_pop, l_end);
        }
    }
    EMIT_ARG(label_assign, l_end);
}

void compile_and_test(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int l_end = comp_next_label(comp);
    int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < n; i += 1) {
        compile_node(comp, pns->nodes[i]);
        if (i + 1 < n) {
            EMIT_ARG(jump_if_false_or_pop, l_end);
        }
    }
    EMIT_ARG(label_assign, l_end);
}

void compile_not_test_2(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]);
    EMIT_ARG(unary_op, RT_UNARY_OP_NOT);
}

void compile_comparison(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int stack_size = EMIT(get_stack_size);
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    bool multi = (num_nodes > 3);
    int l_fail = 0;
    if (multi) {
        l_fail = comp_next_label(comp);
    }
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (i + 2 < num_nodes) {
            EMIT(dup_top);
            EMIT(rot_three);
        }
        if (MP_PARSE_NODE_IS_TOKEN(pns->nodes[i])) {
            rt_binary_op_t op;
            switch (MP_PARSE_NODE_LEAF_ARG(pns->nodes[i])) {
                case MP_TOKEN_OP_LESS: op = RT_BINARY_OP_LESS; break;
                case MP_TOKEN_OP_MORE: op = RT_BINARY_OP_MORE; break;
                case MP_TOKEN_OP_DBL_EQUAL: op = RT_BINARY_OP_EQUAL; break;
                case MP_TOKEN_OP_LESS_EQUAL: op = RT_BINARY_OP_LESS_EQUAL; break;
                case MP_TOKEN_OP_MORE_EQUAL: op = RT_BINARY_OP_MORE_EQUAL; break;
                case MP_TOKEN_OP_NOT_EQUAL: op = RT_BINARY_OP_NOT_EQUAL; break;
                case MP_TOKEN_KW_IN: op = RT_BINARY_OP_IN; break;
                default: assert(0); op = RT_BINARY_OP_LESS; // shouldn't happen
            }
            EMIT_ARG(binary_op, op);
        } else if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[i])) {
            mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[i];
            int kind = MP_PARSE_NODE_STRUCT_KIND(pns2);
            if (kind == PN_comp_op_not_in) {
                EMIT_ARG(binary_op, RT_BINARY_OP_NOT_IN);
            } else if (kind == PN_comp_op_is) {
                if (MP_PARSE_NODE_IS_NULL(pns2->nodes[0])) {
                    EMIT_ARG(binary_op, RT_BINARY_OP_IS);
                } else {
                    EMIT_ARG(binary_op, RT_BINARY_OP_IS_NOT);
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
            EMIT_ARG(jump_if_false_or_pop, l_fail);
        }
    }
    if (multi) {
        int l_end = comp_next_label(comp);
        EMIT_ARG(jump, l_end);
        EMIT_ARG(label_assign, l_fail);
        EMIT(rot_two);
        EMIT(pop_top);
        EMIT_ARG(label_assign, l_end);
        EMIT_ARG(set_stack_size, stack_size + 1); // force stack size
    }
}

void compile_star_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // TODO
    assert(0);
    compile_node(comp, pns->nodes[0]);
    //EMIT_ARG(unary_op, "UNARY_STAR");
}

void compile_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    c_binary_op(comp, pns, RT_BINARY_OP_OR);
}

void compile_xor_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    c_binary_op(comp, pns, RT_BINARY_OP_XOR);
}

void compile_and_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    c_binary_op(comp, pns, RT_BINARY_OP_AND);
}

void compile_shift_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_DBL_LESS)) {
            EMIT_ARG(binary_op, RT_BINARY_OP_LSHIFT);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_DBL_MORE)) {
            EMIT_ARG(binary_op, RT_BINARY_OP_RSHIFT);
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

void compile_arith_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_PLUS)) {
            EMIT_ARG(binary_op, RT_BINARY_OP_ADD);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_MINUS)) {
            EMIT_ARG(binary_op, RT_BINARY_OP_SUBTRACT);
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

void compile_term(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_STAR)) {
            EMIT_ARG(binary_op, RT_BINARY_OP_MULTIPLY);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_DBL_SLASH)) {
            EMIT_ARG(binary_op, RT_BINARY_OP_FLOOR_DIVIDE);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_SLASH)) {
            EMIT_ARG(binary_op, RT_BINARY_OP_TRUE_DIVIDE);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_PERCENT)) {
            EMIT_ARG(binary_op, RT_BINARY_OP_MODULO);
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

void compile_factor_2(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[1]);
    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_PLUS)) {
        EMIT_ARG(unary_op, RT_UNARY_OP_POSITIVE);
    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_MINUS)) {
        EMIT_ARG(unary_op, RT_UNARY_OP_NEGATIVE);
    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_TILDE)) {
        EMIT_ARG(unary_op, RT_UNARY_OP_INVERT);
    } else {
        // shouldn't happen
        assert(0);
    }
}

void compile_power(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // this is to handle special super() call
    comp->func_arg_is_super = MP_PARSE_NODE_IS_ID(pns->nodes[0]) && MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]) == MP_QSTR_super;

    compile_generic_all_nodes(comp, pns);
}

STATIC void compile_trailer_paren_helper(compiler_t *comp, mp_parse_node_t pn_arglist, bool is_method_call, int n_positional_extra) {
    // function to call is on top of stack

#if !MICROPY_EMIT_CPYTHON
    // this is to handle special super() call
    if (MP_PARSE_NODE_IS_NULL(pn_arglist) && comp->func_arg_is_super && comp->scope_cur->kind == SCOPE_FUNCTION) {
        EMIT_ARG(load_id, MP_QSTR___class__);
        // get first argument to function
        bool found = false;
        for (int i = 0; i < comp->scope_cur->id_info_len; i++) {
            if (comp->scope_cur->id_info[i].param) {
                EMIT_ARG(load_fast, MP_QSTR_, comp->scope_cur->id_info[i].local_num);
                found = true;
                break;
            }
        }
        if (!found) {
            printf("TypeError: super() call cannot find self\n");
            return;
        }
        EMIT_ARG(call_function, 2, 0, false, false);
        return;
    }
#endif

    int old_n_arg_keyword = comp->n_arg_keyword;
    bool old_have_star_arg = comp->have_star_arg;
    bool old_have_dbl_star_arg = comp->have_dbl_star_arg;
    comp->n_arg_keyword = 0;
    comp->have_star_arg = false;
    comp->have_dbl_star_arg = false;

    compile_node(comp, pn_arglist); // arguments to function call; can be null

    // compute number of positional arguments
    int n_positional = n_positional_extra + list_len(pn_arglist, PN_arglist) - comp->n_arg_keyword;
    if (comp->have_star_arg) {
        n_positional -= 1;
    }
    if (comp->have_dbl_star_arg) {
        n_positional -= 1;
    }

    if (is_method_call) {
        EMIT_ARG(call_method, n_positional, comp->n_arg_keyword, comp->have_star_arg, comp->have_dbl_star_arg);
    } else {
        EMIT_ARG(call_function, n_positional, comp->n_arg_keyword, comp->have_star_arg, comp->have_dbl_star_arg);
    }

    comp->n_arg_keyword = old_n_arg_keyword;
    comp->have_star_arg = old_have_star_arg;
    comp->have_dbl_star_arg = old_have_dbl_star_arg;
}

void compile_power_trailers(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < num_nodes; i++) {
        if (i + 1 < num_nodes && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[i], PN_trailer_period) && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[i + 1], PN_trailer_paren)) {
            // optimisation for method calls a.f(...), following PyPy
            mp_parse_node_struct_t *pns_period = (mp_parse_node_struct_t*)pns->nodes[i];
            mp_parse_node_struct_t *pns_paren = (mp_parse_node_struct_t*)pns->nodes[i + 1];
            EMIT_ARG(load_method, MP_PARSE_NODE_LEAF_ARG(pns_period->nodes[0])); // get the method
            compile_trailer_paren_helper(comp, pns_paren->nodes[0], true, 0);
            i += 1;
        } else {
            compile_node(comp, pns->nodes[i]);
        }
        comp->func_arg_is_super = false;
    }
}

void compile_power_dbl_star(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]);
    EMIT_ARG(binary_op, RT_BINARY_OP_POWER);
}

void compile_atom_string(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // a list of strings

    // check type of list (string or bytes) and count total number of bytes
    int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    int n_bytes = 0;
    int string_kind = MP_PARSE_NODE_NULL;
    for (int i = 0; i < n; i++) {
        assert(MP_PARSE_NODE_IS_LEAF(pns->nodes[i]));
        int pn_kind = MP_PARSE_NODE_LEAF_KIND(pns->nodes[i]);
        assert(pn_kind == MP_PARSE_NODE_STRING || pn_kind == MP_PARSE_NODE_BYTES);
        if (i == 0) {
            string_kind = pn_kind;
        } else if (pn_kind != string_kind) {
            printf("SyntaxError: cannot mix bytes and nonbytes literals\n");
            return;
        }
        n_bytes += qstr_len(MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
    }

    // concatenate string/bytes
    byte *q_ptr;
    byte *s_dest = qstr_build_start(n_bytes, &q_ptr);
    for (int i = 0; i < n; i++) {
        uint s_len;
        const byte *s = qstr_data(MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]), &s_len);
        memcpy(s_dest, s, s_len);
        s_dest += s_len;
    }
    qstr q = qstr_build_end(q_ptr);

    EMIT_ARG(load_const_str, q, string_kind == MP_PARSE_NODE_BYTES);
}

// pns needs to have 2 nodes, first is lhs of comprehension, second is PN_comp_for node
void compile_comprehension(compiler_t *comp, mp_parse_node_struct_t *pns, scope_kind_t kind) {
    assert(MP_PARSE_NODE_STRUCT_NUM_NODES(pns) == 2);
    assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_comp_for));
    mp_parse_node_struct_t *pns_comp_for = (mp_parse_node_struct_t*)pns->nodes[1];

    if (comp->pass == PASS_1) {
        // create a new scope for this comprehension
        scope_t *s = scope_new_and_link(comp, kind, (mp_parse_node_t)pns, comp->scope_cur->emit_options);
        // store the comprehension scope so the compiling function (this one) can use it at each pass
        pns_comp_for->nodes[3] = (mp_parse_node_t)s;
    }

    // get the scope for this comprehension
    scope_t *this_scope = (scope_t*)pns_comp_for->nodes[3];

    // compile the comprehension
    close_over_variables_etc(comp, this_scope, 0, 0);

    compile_node(comp, pns_comp_for->nodes[1]); // source of the iterator
    EMIT(get_iter);
    EMIT_ARG(call_function, 1, 0, false, false);
}

void compile_atom_paren(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // an empty tuple
        c_tuple(comp, MP_PARSE_NODE_NULL, NULL);
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
        pns = (mp_parse_node_struct_t*)pns->nodes[0];
        assert(!MP_PARSE_NODE_IS_NULL(pns->nodes[1]));
        if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
            mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3b) {
                // tuple of one item, with trailing comma
                assert(MP_PARSE_NODE_IS_NULL(pns2->nodes[0]));
                c_tuple(comp, pns->nodes[0], NULL);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3c) {
                // tuple of many items
                c_tuple(comp, pns->nodes[0], pns2);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_comp_for) {
                // generator expression
                compile_comprehension(comp, pns, SCOPE_GEN_EXPR);
            } else {
                // tuple with 2 items
                goto tuple_with_2_items;
            }
        } else {
            // tuple with 2 items
            tuple_with_2_items:
            c_tuple(comp, MP_PARSE_NODE_NULL, pns);
        }
    } else {
        // parenthesis around a single item, is just that item
        compile_node(comp, pns->nodes[0]);
    }
}

void compile_atom_bracket(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // empty list
        EMIT_ARG(build_list, 0);
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
        mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[0];
        if (MP_PARSE_NODE_IS_STRUCT(pns2->nodes[1])) {
            mp_parse_node_struct_t *pns3 = (mp_parse_node_struct_t*)pns2->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns3) == PN_testlist_comp_3b) {
                // list of one item, with trailing comma
                assert(MP_PARSE_NODE_IS_NULL(pns3->nodes[0]));
                compile_node(comp, pns2->nodes[0]);
                EMIT_ARG(build_list, 1);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns3) == PN_testlist_comp_3c) {
                // list of many items
                compile_node(comp, pns2->nodes[0]);
                compile_generic_all_nodes(comp, pns3);
                EMIT_ARG(build_list, 1 + MP_PARSE_NODE_STRUCT_NUM_NODES(pns3));
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns3) == PN_comp_for) {
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
            EMIT_ARG(build_list, 2);
        }
    } else {
        // list with 1 item
        compile_node(comp, pns->nodes[0]);
        EMIT_ARG(build_list, 1);
    }
}

void compile_atom_brace(compiler_t *comp, mp_parse_node_struct_t *pns) {
    mp_parse_node_t pn = pns->nodes[0];
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        // empty dict
        EMIT_ARG(build_map, 0);
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_dictorsetmaker_item) {
            // dict with one element
            EMIT_ARG(build_map, 1);
            compile_node(comp, pn);
            EMIT(store_map);
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_dictorsetmaker) {
            assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should succeed
            mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_dictorsetmaker_list) {
                // dict/set with multiple elements

                // get tail elements (2nd, 3rd, ...)
                mp_parse_node_t *nodes;
                int n = list_get(&pns1->nodes[0], PN_dictorsetmaker_list2, &nodes);

                // first element sets whether it's a dict or set
                bool is_dict;
                if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_dictorsetmaker_item)) {
                    // a dictionary
                    EMIT_ARG(build_map, 1 + n);
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
                    mp_parse_node_t pn = nodes[i];
                    bool is_key_value = MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_dictorsetmaker_item);
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
                    EMIT_ARG(build_set, 1 + n);
                }
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_comp_for) {
                // dict/set comprehension
                if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_dictorsetmaker_item)) {
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
        EMIT_ARG(build_set, 1);
    }
}

void compile_trailer_paren(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_trailer_paren_helper(comp, pns->nodes[0], false, 0);
}

void compile_trailer_bracket(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // object who's index we want is on top of stack
    compile_node(comp, pns->nodes[0]); // the index
    EMIT_ARG(binary_op, RT_BINARY_OP_SUBSCR);
}

void compile_trailer_period(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // object who's attribute we want is on top of stack
    EMIT_ARG(load_attr, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0])); // attribute to get
}

void compile_subscript_3_helper(compiler_t *comp, mp_parse_node_struct_t *pns) {
    assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_3); // should always be
    mp_parse_node_t pn = pns->nodes[0];
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        // [?:]
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT_ARG(build_slice, 2);
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_3c) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
            pn = pns->nodes[0];
            if (MP_PARSE_NODE_IS_NULL(pn)) {
                // [?::]
                EMIT_ARG(build_slice, 2);
            } else {
                // [?::x]
                compile_node(comp, pn);
                EMIT_ARG(build_slice, 3);
            }
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_3d) {
            compile_node(comp, pns->nodes[0]);
            assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should always be
            pns = (mp_parse_node_struct_t*)pns->nodes[1];
            assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_sliceop); // should always be
            if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                // [?:x:]
                EMIT_ARG(build_slice, 2);
            } else {
                // [?:x:x]
                compile_node(comp, pns->nodes[0]);
                EMIT_ARG(build_slice, 3);
            }
        } else {
            // [?:x]
            compile_node(comp, pn);
            EMIT_ARG(build_slice, 2);
        }
    } else {
        // [?:x]
        compile_node(comp, pn);
        EMIT_ARG(build_slice, 2);
    }
}

void compile_subscript_2(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]); // start of slice
    assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should always be
    compile_subscript_3_helper(comp, (mp_parse_node_struct_t*)pns->nodes[1]);
}

void compile_subscript_3(compiler_t *comp, mp_parse_node_struct_t *pns) {
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    compile_subscript_3_helper(comp, pns);
}

void compile_dictorsetmaker_item(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // if this is called then we are compiling a dict key:value pair
    compile_node(comp, pns->nodes[1]); // value
    compile_node(comp, pns->nodes[0]); // key
}

void compile_classdef(compiler_t *comp, mp_parse_node_struct_t *pns) {
    qstr cname = compile_classdef_helper(comp, pns, comp->scope_cur->emit_options);
    // store class object into class name
    EMIT_ARG(store_id, cname);
}

void compile_arglist_star(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->have_star_arg) {
        printf("SyntaxError?: can't have multiple *x\n");
        return;
    }
    comp->have_star_arg = true;
    compile_node(comp, pns->nodes[0]);
}

void compile_arglist_dbl_star(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->have_dbl_star_arg) {
        printf("SyntaxError?: can't have multiple **x\n");
        return;
    }
    comp->have_dbl_star_arg = true;
    compile_node(comp, pns->nodes[0]);
}

void compile_argument(compiler_t *comp, mp_parse_node_struct_t *pns) {
    assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should always be
    mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[1];
    if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_argument_3) {
        if (!MP_PARSE_NODE_IS_ID(pns->nodes[0])) {
            printf("SyntaxError?: lhs of keyword argument must be an id\n");
            return;
        }
        EMIT_ARG(load_const_id, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]));
        compile_node(comp, pns2->nodes[0]);
        comp->n_arg_keyword += 1;
    } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_comp_for) {
        compile_comprehension(comp, pns, SCOPE_GEN_EXPR);
    } else {
        // shouldn't happen
        assert(0);
    }
}

void compile_yield_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION) {
        printf("SyntaxError: 'yield' outside function\n");
        return;
    }
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT(yield_value);
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_yield_arg_from)) {
        pns = (mp_parse_node_struct_t*)pns->nodes[0];
        compile_node(comp, pns->nodes[0]);
        EMIT(get_iter);
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT(yield_from);
    } else {
        compile_node(comp, pns->nodes[0]);
        EMIT(yield_value);
    }
}

typedef void (*compile_function_t)(compiler_t*, mp_parse_node_struct_t*);
STATIC compile_function_t compile_function[] = {
    NULL,
#define nc NULL
#define c(f) compile_##f
#define DEF_RULE(rule, comp, kind, arg...) comp,
#include "grammar.h"
#undef nc
#undef c
#undef DEF_RULE
};

void compile_node(compiler_t *comp, mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        // pass
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        machine_int_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
        switch (MP_PARSE_NODE_LEAF_KIND(pn)) {
            case MP_PARSE_NODE_ID: EMIT_ARG(load_id, arg); break;
            case MP_PARSE_NODE_SMALL_INT: EMIT_ARG(load_const_small_int, arg); break;
            case MP_PARSE_NODE_INTEGER: EMIT_ARG(load_const_int, arg); break;
            case MP_PARSE_NODE_DECIMAL: EMIT_ARG(load_const_dec, arg); break;
            case MP_PARSE_NODE_STRING: EMIT_ARG(load_const_str, arg, false); break;
            case MP_PARSE_NODE_BYTES: EMIT_ARG(load_const_str, arg, true); break;
            case MP_PARSE_NODE_TOKEN:
                if (arg == MP_TOKEN_NEWLINE) {
                    // this can occur when file_input lets through a NEWLINE (eg if file starts with a newline)
                    // or when single_input lets through a NEWLINE (user enters a blank line)
                    // do nothing
                } else {
                  EMIT_ARG(load_const_tok, arg);
                }
                break;
            default: assert(0);
        }
    } else {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        EMIT_ARG(set_line_number, pns->source_line);
        compile_function_t f = compile_function[MP_PARSE_NODE_STRUCT_KIND(pns)];
        if (f == NULL) {
            printf("node %u cannot be compiled\n", (uint)MP_PARSE_NODE_STRUCT_KIND(pns));
#if MICROPY_DEBUG_PRINTERS
            mp_parse_node_print(pn, 0);
#endif
            assert(0);
        } else {
            f(comp, pns);
        }
    }
}

void compile_scope_func_lambda_param(compiler_t *comp, mp_parse_node_t pn, pn_kind_t pn_name, pn_kind_t pn_star, pn_kind_t pn_dbl_star, bool allow_annotations) {
    // TODO verify that *k and **k are last etc
    qstr param_name = 0;
    mp_parse_node_t pn_annotation = MP_PARSE_NODE_NULL;
    if (MP_PARSE_NODE_IS_ID(pn)) {
        param_name = MP_PARSE_NODE_LEAF_ARG(pn);
        if (comp->have_bare_star) {
            // comes after a bare star, so doesn't count as a parameter
        } else {
            comp->scope_cur->num_params += 1;
        }
    } else {
        assert(MP_PARSE_NODE_IS_STRUCT(pn));
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == pn_name) {
            param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            //int node_index = 1; unused
            if (allow_annotations) {
                if (!MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
                    // this parameter has an annotation
                    pn_annotation = pns->nodes[1];
                }
                //node_index = 2; unused
            }
            /* this is obsolete now that num dict/default params are calculated in compile_funcdef_param
            if (!MP_PARSE_NODE_IS_NULL(pns->nodes[node_index])) {
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
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == pn_star) {
            if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                // bare star
                // TODO see http://www.python.org/dev/peps/pep-3102/
                comp->have_bare_star = true;
                //assert(comp->scope_cur->num_dict_params == 0);
            } else if (MP_PARSE_NODE_IS_ID(pns->nodes[0])) {
                // named star
                comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARARGS;
                param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            } else if (allow_annotations && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_tfpdef)) {
                // named star with annotation
                comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARARGS;
                pns = (mp_parse_node_struct_t*)pns->nodes[0];
                param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                pn_annotation = pns->nodes[1];
            } else {
                // shouldn't happen
                assert(0);
            }
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == pn_dbl_star) {
            param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            if (allow_annotations && !MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
                // this parameter has an annotation
                pn_annotation = pns->nodes[1];
            }
            comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARKEYWORDS;
        } else {
            // TODO anything to implement?
            assert(0);
        }
    }

    if (param_name != 0) {
        if (!MP_PARSE_NODE_IS_NULL(pn_annotation)) {
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

void compile_scope_func_param(compiler_t *comp, mp_parse_node_t pn) {
    compile_scope_func_lambda_param(comp, pn, PN_typedargslist_name, PN_typedargslist_star, PN_typedargslist_dbl_star, true);
}

void compile_scope_lambda_param(compiler_t *comp, mp_parse_node_t pn) {
    compile_scope_func_lambda_param(comp, pn, PN_varargslist_name, PN_varargslist_star, PN_varargslist_dbl_star, false);
}

void compile_scope_comp_iter(compiler_t *comp, mp_parse_node_t pn_iter, mp_parse_node_t pn_inner_expr, int l_top, int for_depth) {
    tail_recursion:
    if (MP_PARSE_NODE_IS_NULL(pn_iter)) {
        // no more nested if/for; compile inner expression
        compile_node(comp, pn_inner_expr);
        if (comp->scope_cur->kind == SCOPE_LIST_COMP) {
            EMIT_ARG(list_append, for_depth + 2);
        } else if (comp->scope_cur->kind == SCOPE_DICT_COMP) {
            EMIT_ARG(map_add, for_depth + 2);
        } else if (comp->scope_cur->kind == SCOPE_SET_COMP) {
            EMIT_ARG(set_add, for_depth + 2);
        } else {
            EMIT(yield_value);
            EMIT(pop_top);
        }
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn_iter, PN_comp_if)) {
        // if condition
        mp_parse_node_struct_t *pns_comp_if = (mp_parse_node_struct_t*)pn_iter;
        c_if_cond(comp, pns_comp_if->nodes[0], false, l_top);
        pn_iter = pns_comp_if->nodes[1];
        goto tail_recursion;
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn_iter, PN_comp_for)) {
        // for loop
        mp_parse_node_struct_t *pns_comp_for2 = (mp_parse_node_struct_t*)pn_iter;
        compile_node(comp, pns_comp_for2->nodes[1]);
        int l_end2 = comp_next_label(comp);
        int l_top2 = comp_next_label(comp);
        EMIT(get_iter);
        EMIT_ARG(label_assign, l_top2);
        EMIT_ARG(for_iter, l_end2);
        c_assign(comp, pns_comp_for2->nodes[0], ASSIGN_STORE);
        compile_scope_comp_iter(comp, pns_comp_for2->nodes[2], pn_inner_expr, l_top2, for_depth + 1);
        EMIT_ARG(jump, l_top2);
        EMIT_ARG(label_assign, l_end2);
        EMIT(for_iter_end);
    } else {
        // shouldn't happen
        assert(0);
    }
}

void check_for_doc_string(compiler_t *comp, mp_parse_node_t pn) {
    // see http://www.python.org/dev/peps/pep-0257/

    // look for the first statement
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_expr_stmt)) {
        // a statement; fall through
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_file_input_2)) {
        // file input; find the first non-newline node
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        for (int i = 0; i < num_nodes; i++) {
            pn = pns->nodes[i];
            if (!(MP_PARSE_NODE_IS_LEAF(pn) && MP_PARSE_NODE_LEAF_KIND(pn) == MP_PARSE_NODE_TOKEN && MP_PARSE_NODE_LEAF_ARG(pn) == MP_TOKEN_NEWLINE)) {
                // not a newline, so this is the first statement; finish search
                break;
            }
        }
        // if we didn't find a non-newline then it's okay to fall through; pn will be a newline and so doc-string test below will fail gracefully
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_suite_block_stmts)) {
        // a list of statements; get the first one
        pn = ((mp_parse_node_struct_t*)pn)->nodes[0];
    } else {
        return;
    }

    // check the first statement for a doc string
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_expr_stmt)) {
        mp_parse_node_struct_t* pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_IS_LEAF(pns->nodes[0])) {
            int kind = MP_PARSE_NODE_LEAF_KIND(pns->nodes[0]);
            if (kind == MP_PARSE_NODE_STRING) {
                compile_node(comp, pns->nodes[0]); // a doc string
                // store doc string
                EMIT_ARG(store_id, MP_QSTR___doc__);
            }
        }
    }
}

void compile_scope(compiler_t *comp, scope_t *scope, pass_kind_t pass) {
    comp->pass = pass;
    comp->scope_cur = scope;
    comp->next_label = 1;
    EMIT_ARG(start_pass, pass, scope);

    if (comp->pass == PASS_1) {
        scope->stack_size = 0;
    }

#if MICROPY_EMIT_CPYTHON
    if (comp->pass == PASS_3) {
        scope_print_info(scope);
    }
#endif

    // compile
    if (MP_PARSE_NODE_IS_STRUCT_KIND(scope->pn, PN_eval_input)) {
        assert(scope->kind == SCOPE_MODULE);
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
        compile_node(comp, pns->nodes[0]); // compile the expression
        EMIT(return_value);
    } else if (scope->kind == SCOPE_MODULE) {
        if (!comp->is_repl) {
            check_for_doc_string(comp, scope->pn);
        }
        compile_node(comp, scope->pn);
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT(return_value);
    } else if (scope->kind == SCOPE_FUNCTION) {
        assert(MP_PARSE_NODE_IS_STRUCT(scope->pn));
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
        assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_funcdef);

        // work out number of parameters, keywords and default parameters, and add them to the id_info array
        // must be done before compiling the body so that arguments are numbered first (for LOAD_FAST etc)
        if (comp->pass == PASS_1) {
            comp->have_bare_star = false;
            apply_to_single_or_list(comp, pns->nodes[1], PN_typedargslist, compile_scope_func_param);
        }

        // pns->nodes[2] is return/whole function annotation

        compile_node(comp, pns->nodes[3]); // 3 is function body
        // emit return if it wasn't the last opcode
        if (!EMIT(last_emit_was_return_value)) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
            EMIT(return_value);
        }
    } else if (scope->kind == SCOPE_LAMBDA) {
        assert(MP_PARSE_NODE_IS_STRUCT(scope->pn));
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
        assert(MP_PARSE_NODE_STRUCT_NUM_NODES(pns) == 3);

        // work out number of parameters, keywords and default parameters, and add them to the id_info array
        // must be done before compiling the body so that arguments are numbered first (for LOAD_FAST etc)
        if (comp->pass == PASS_1) {
            comp->have_bare_star = false;
            apply_to_single_or_list(comp, pns->nodes[0], PN_varargslist, compile_scope_lambda_param);
        }

        compile_node(comp, pns->nodes[1]); // 1 is lambda body
        EMIT(return_value);
    } else if (scope->kind == SCOPE_LIST_COMP || scope->kind == SCOPE_DICT_COMP || scope->kind == SCOPE_SET_COMP || scope->kind == SCOPE_GEN_EXPR) {
        // a bit of a hack at the moment

        assert(MP_PARSE_NODE_IS_STRUCT(scope->pn));
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
        assert(MP_PARSE_NODE_STRUCT_NUM_NODES(pns) == 2);
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_comp_for));
        mp_parse_node_struct_t *pns_comp_for = (mp_parse_node_struct_t*)pns->nodes[1];

        qstr qstr_arg = QSTR_FROM_STR_STATIC(".0");
        if (comp->pass == PASS_1) {
            bool added;
            id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, qstr_arg, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
            scope->num_params = 1;
        }

        if (scope->kind == SCOPE_LIST_COMP) {
            EMIT_ARG(build_list, 0);
        } else if (scope->kind == SCOPE_DICT_COMP) {
            EMIT_ARG(build_map, 0);
        } else if (scope->kind == SCOPE_SET_COMP) {
            EMIT_ARG(build_set, 0);
        }

        int l_end = comp_next_label(comp);
        int l_top = comp_next_label(comp);
        EMIT_ARG(load_id, qstr_arg);
        EMIT_ARG(label_assign, l_top);
        EMIT_ARG(for_iter, l_end);
        c_assign(comp, pns_comp_for->nodes[0], ASSIGN_STORE);
        compile_scope_comp_iter(comp, pns_comp_for->nodes[2], pns->nodes[0], l_top, 0);
        EMIT_ARG(jump, l_top);
        EMIT_ARG(label_assign, l_end);
        EMIT(for_iter_end);

        if (scope->kind == SCOPE_GEN_EXPR) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        }
        EMIT(return_value);
    } else {
        assert(scope->kind == SCOPE_CLASS);
        assert(MP_PARSE_NODE_IS_STRUCT(scope->pn));
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
        assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_classdef);

        if (comp->pass == PASS_1) {
            bool added;
            id_info_t *id_info = scope_find_or_add_id(scope, MP_QSTR___class__, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
            id_info = scope_find_or_add_id(scope, MP_QSTR___locals__, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
            id_info->param = true;
            scope->num_params = 1; // __locals__ is the parameter
        }

        EMIT_ARG(load_id, MP_QSTR___locals__);
        EMIT(store_locals);
        EMIT_ARG(load_id, MP_QSTR___name__);
        EMIT_ARG(store_id, MP_QSTR___module__);
        EMIT_ARG(load_const_id, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0])); // 0 is class name
        EMIT_ARG(store_id, MP_QSTR___qualname__);

        check_for_doc_string(comp, pns->nodes[2]);
        compile_node(comp, pns->nodes[2]); // 2 is class body

        id_info_t *id = scope_find(scope, MP_QSTR___class__);
        assert(id != NULL);
        if (id->kind == ID_INFO_KIND_LOCAL) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        } else {
#if MICROPY_EMIT_CPYTHON
            EMIT_ARG(load_closure, MP_QSTR___class__, 0); // XXX check this is the correct local num
#else
            EMIT_ARG(load_fast, MP_QSTR___class__, id->local_num);
#endif
        }
        EMIT(return_value);
    }

    EMIT(end_pass);
}

void compile_scope_inline_asm(compiler_t *comp, scope_t *scope, pass_kind_t pass) {
    comp->pass = pass;
    comp->scope_cur = scope;
    comp->next_label = 1;

    if (scope->kind != SCOPE_FUNCTION) {
        printf("Error: inline assembler must be a function\n");
        return;
    }

    if (comp->pass > PASS_1) {
        EMIT_INLINE_ASM_ARG(start_pass, comp->pass, comp->scope_cur);
    }

    // get the function definition parse node
    assert(MP_PARSE_NODE_IS_STRUCT(scope->pn));
    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
    assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_funcdef);

    //qstr f_id = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]); // function name

    // parameters are in pns->nodes[1]
    if (comp->pass == PASS_2) {
        mp_parse_node_t *pn_params;
        int n_params = list_get(&pns->nodes[1], PN_typedargslist, &pn_params);
        scope->num_params = EMIT_INLINE_ASM_ARG(count_params, n_params, pn_params);
    }

    assert(MP_PARSE_NODE_IS_NULL(pns->nodes[2])); // type

    mp_parse_node_t pn_body = pns->nodes[3]; // body
    mp_parse_node_t *nodes;
    int num = list_get(&pn_body, PN_suite_block_stmts, &nodes);

    /*
    if (comp->pass == PASS_3) {
        //printf("----\n");
        scope_print_info(scope);
    }
    */

    for (int i = 0; i < num; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT(nodes[i]));
        mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)nodes[i];
        assert(MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_expr_stmt);
        assert(MP_PARSE_NODE_IS_STRUCT(pns2->nodes[0]));
        assert(MP_PARSE_NODE_IS_NULL(pns2->nodes[1]));
        pns2 = (mp_parse_node_struct_t*)pns2->nodes[0];
        assert(MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_power);
        assert(MP_PARSE_NODE_IS_ID(pns2->nodes[0]));
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns2->nodes[1], PN_trailer_paren));
        assert(MP_PARSE_NODE_IS_NULL(pns2->nodes[2]));
        qstr op = MP_PARSE_NODE_LEAF_ARG(pns2->nodes[0]);
        pns2 = (mp_parse_node_struct_t*)pns2->nodes[1]; // PN_trailer_paren
        mp_parse_node_t *pn_arg;
        int n_args = list_get(&pns2->nodes[0], PN_arglist, &pn_arg);

        // emit instructions
        if (strcmp(qstr_str(op), "label") == 0) {
            if (!(n_args == 1 && MP_PARSE_NODE_IS_ID(pn_arg[0]))) {
                printf("SyntaxError: inline assembler 'label' requires 1 argument\n");
                return;
            }
            int lab = comp_next_label(comp);
            if (pass > PASS_1) {
                EMIT_INLINE_ASM_ARG(label, lab, MP_PARSE_NODE_LEAF_ARG(pn_arg[0]));
            }
        } else {
            if (pass > PASS_1) {
                EMIT_INLINE_ASM_ARG(op, op, n_args, pn_arg);
            }
        }
    }

    if (comp->pass > PASS_1) {
        EMIT_INLINE_ASM(end_pass);
    }
}

void compile_scope_compute_things(compiler_t *comp, scope_t *scope) {
    // in functions, turn implicit globals into explicit globals
    // compute the index of each local
    scope->num_locals = 0;
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
        if (scope->kind == SCOPE_CLASS && id->qstr == MP_QSTR___class__) {
            // __class__ is not counted as a local; if it's used then it becomes a ID_INFO_KIND_CELL
            continue;
        }
        if (scope->kind >= SCOPE_FUNCTION && scope->kind <= SCOPE_GEN_EXPR && id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
            id->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
        }
        // note: params always count for 1 local, even if they are a cell
        if (id->param || id->kind == ID_INFO_KIND_LOCAL) {
            id->local_num = scope->num_locals;
            scope->num_locals += 1;
        }
    }

    // compute the index of cell vars (freevars[idx] in CPython)
#if MICROPY_EMIT_CPYTHON
    int num_cell = 0;
#endif
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
#if MICROPY_EMIT_CPYTHON
        // in CPython the cells are numbered starting from 0
        if (id->kind == ID_INFO_KIND_CELL) {
            id->local_num = num_cell;
            num_cell += 1;
        }
#else
        // in Micro Python the cells come right after the fast locals
        // parameters are not counted here, since they remain at the start
        // of the locals, even if they are cell vars
        if (!id->param && id->kind == ID_INFO_KIND_CELL) {
            id->local_num = scope->num_locals;
            scope->num_locals += 1;
        }
#endif
    }

    // compute the index of free vars (freevars[idx] in CPython)
    // make sure they are in the order of the parent scope
    if (scope->parent != NULL) {
        int num_free = 0;
        for (int i = 0; i < scope->parent->id_info_len; i++) {
            id_info_t *id = &scope->parent->id_info[i];
            if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
                for (int j = 0; j < scope->id_info_len; j++) {
                    id_info_t *id2 = &scope->id_info[j];
                    if (id2->kind == ID_INFO_KIND_FREE && id->qstr == id2->qstr) {
                        assert(!id2->param); // free vars should not be params
#if MICROPY_EMIT_CPYTHON
                        // in CPython the frees are numbered after the cells
                        id2->local_num = num_cell + num_free;
#else
                        // in Micro Python the frees come first, before the params
                        id2->local_num = num_free;
#endif
                        num_free += 1;
                    }
                }
            }
        }
#if !MICROPY_EMIT_CPYTHON
        // in Micro Python shift all other locals after the free locals
        if (num_free > 0) {
            for (int i = 0; i < scope->id_info_len; i++) {
                id_info_t *id = &scope->id_info[i];
                if (id->param || id->kind != ID_INFO_KIND_FREE) {
                    id->local_num += num_free;
                }
            }
            scope->num_params += num_free; // free vars are counted as params for passing them into the function
            scope->num_locals += num_free;
        }
#endif
    }

    // compute scope_flags
    //scope->scope_flags = 0; since we set some things in parameters
    if (scope->kind != SCOPE_MODULE) {
        scope->scope_flags |= MP_SCOPE_FLAG_NEWLOCALS;
    }
    if (scope->kind == SCOPE_FUNCTION || scope->kind == SCOPE_LAMBDA || scope->kind == SCOPE_LIST_COMP || scope->kind == SCOPE_DICT_COMP || scope->kind == SCOPE_SET_COMP || scope->kind == SCOPE_GEN_EXPR) {
        assert(scope->parent != NULL);
        scope->scope_flags |= MP_SCOPE_FLAG_OPTIMISED;

        // TODO possibly other ways it can be nested
        // Note that we don't actually use this information at the moment (for CPython compat only)
        if ((SCOPE_FUNCTION <= scope->parent->kind && scope->parent->kind <= SCOPE_SET_COMP) || (scope->parent->kind == SCOPE_CLASS && scope->parent->parent->kind == SCOPE_FUNCTION)) {
            scope->scope_flags |= MP_SCOPE_FLAG_NESTED;
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
        scope->scope_flags |= MP_SCOPE_FLAG_NOFREE;
    }
}

mp_obj_t mp_compile(mp_parse_node_t pn, qstr source_file, bool is_repl) {
    compiler_t *comp = m_new(compiler_t, 1);

    comp->source_file = source_file;
    comp->is_repl = is_repl;
    comp->had_error = false;

    comp->break_label = 0;
    comp->continue_label = 0;
    comp->break_continue_except_level = 0;
    comp->cur_except_level = 0;

    comp->func_arg_is_super = false;

    comp->scope_head = NULL;
    comp->scope_cur = NULL;

    // optimise constants
    pn = fold_constants(pn);

    // set the outer scope
    scope_t *module_scope = scope_new_and_link(comp, SCOPE_MODULE, pn, EMIT_OPT_NONE);

    // compile pass 1
    comp->emit = emit_pass1_new();
    comp->emit_method_table = &emit_pass1_method_table;
    comp->emit_inline_asm = NULL;
    comp->emit_inline_asm_method_table = NULL;
    uint max_num_labels = 0;
    for (scope_t *s = comp->scope_head; s != NULL && !comp->had_error; s = s->next) {
        if (false) {
#if MICROPY_EMIT_INLINE_THUMB
        } else if (s->emit_options == EMIT_OPT_ASM_THUMB) {
            compile_scope_inline_asm(comp, s, PASS_1);
#endif
        } else {
            compile_scope(comp, s, PASS_1);
        }

        // update maximim number of labels needed
        if (comp->next_label > max_num_labels) {
            max_num_labels = comp->next_label;
        }
    }

    // compute some things related to scope and identifiers
    for (scope_t *s = comp->scope_head; s != NULL && !comp->had_error; s = s->next) {
        compile_scope_compute_things(comp, s);
    }

    // finish with pass 1
    emit_pass1_free(comp->emit);

    // compile pass 2 and 3
#if !MICROPY_EMIT_CPYTHON
    emit_t *emit_bc = NULL;
#if MICROPY_EMIT_NATIVE
    emit_t *emit_native = NULL;
#endif
#if MICROPY_EMIT_INLINE_THUMB
    emit_inline_asm_t *emit_inline_thumb = NULL;
#endif
#endif // !MICROPY_EMIT_CPYTHON
    for (scope_t *s = comp->scope_head; s != NULL && !comp->had_error; s = s->next) {
        if (false) {
            // dummy

#if MICROPY_EMIT_INLINE_THUMB
        } else if (s->emit_options == EMIT_OPT_ASM_THUMB) {
            // inline assembly for thumb
            if (emit_inline_thumb == NULL) {
                emit_inline_thumb = emit_inline_thumb_new(max_num_labels);
            }
            comp->emit = NULL;
            comp->emit_method_table = NULL;
            comp->emit_inline_asm = emit_inline_thumb;
            comp->emit_inline_asm_method_table = &emit_inline_thumb_method_table;
            compile_scope_inline_asm(comp, s, PASS_2);
            compile_scope_inline_asm(comp, s, PASS_3);
#endif

        } else {

            // choose the emit type

#if MICROPY_EMIT_CPYTHON
            comp->emit = emit_cpython_new(max_num_labels);
            comp->emit_method_table = &emit_cpython_method_table;
#else
            switch (s->emit_options) {

#if MICROPY_EMIT_NATIVE
                case EMIT_OPT_NATIVE_PYTHON:
                case EMIT_OPT_VIPER:
#if MICROPY_EMIT_X64
                    if (emit_native == NULL) {
                        emit_native = emit_native_x64_new(max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_x64_method_table;
#elif MICROPY_EMIT_THUMB
                    if (emit_native == NULL) {
                        emit_native = emit_native_thumb_new(max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_thumb_method_table;
#endif
                    comp->emit = emit_native;
                    comp->emit_method_table->set_native_types(comp->emit, s->emit_options == EMIT_OPT_VIPER);
                    break;
#endif // MICROPY_EMIT_NATIVE

                default:
                    if (emit_bc == NULL) {
                        emit_bc = emit_bc_new(max_num_labels);
                    }
                    comp->emit = emit_bc;
                    comp->emit_method_table = &emit_bc_method_table;
                    break;
            }
#endif // !MICROPY_EMIT_CPYTHON

            // compile pass 2 and pass 3
            compile_scope(comp, s, PASS_2);
            compile_scope(comp, s, PASS_3);
        }
    }

    // free the emitters
#if !MICROPY_EMIT_CPYTHON
    if (emit_bc != NULL) {
        emit_bc_free(emit_bc);
    }
#if MICROPY_EMIT_NATIVE
    if (emit_native != NULL) {
#if MICROPY_EMIT_X64
        emit_native_x64_free(emit_native);
#elif MICROPY_EMIT_THUMB
        emit_native_thumb_free(emit_native);
#endif
    }
#endif
#if MICROPY_EMIT_INLINE_THUMB
    if (emit_inline_thumb != NULL) {
        emit_inline_thumb_free(emit_inline_thumb);
    }
#endif
#endif // !MICROPY_EMIT_CPYTHON

    // free the scopes
    uint unique_code_id = module_scope->unique_code_id;
    for (scope_t *s = module_scope; s;) {
        scope_t *next = s->next;
        scope_free(s);
        s = next;
    }

    // free the compiler
    bool had_error = comp->had_error;
    m_del_obj(compiler_t, comp);

    if (had_error) {
        // TODO return a proper error message
        return mp_const_none;
    } else {
#if MICROPY_EMIT_CPYTHON
        // can't create code, so just return true
        (void)unique_code_id; // to suppress warning that unique_code_id is unused
        return mp_const_true;
#else
        // return function that executes the outer module
        return rt_make_function_from_id(unique_code_id, MP_OBJ_NULL);
#endif
    }
}
