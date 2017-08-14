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
#include <string.h>
#include <assert.h>

#include "py/scope.h"
#include "py/emit.h"
#include "py/compile.h"
#include "py/runtime.h"

#if MICROPY_ENABLE_COMPILER

#if MICROPY_PY_ASYNC_AWAIT
#error "async/await syntax not implemented with this parser/compiler"
#endif

// TODO need to mangle __attr names

typedef enum {
#define DEF_RULE(rule, comp, kind, ...) PN_##rule,
#include "py/grammar.h"
#undef DEF_RULE
    PN_maximum_number_of,
} pn_kind_t;

#define NEED_METHOD_TABLE MICROPY_EMIT_NATIVE

#if NEED_METHOD_TABLE

// we need a method table to do the lookup for the emitter functions
#define EMIT(fun) (comp->emit_method_table->fun(comp->emit))
#define EMIT_ARG(fun, ...) (comp->emit_method_table->fun(comp->emit, __VA_ARGS__))
#define EMIT_LOAD_FAST(qst, local_num) (comp->emit_method_table->load_id.fast(comp->emit, qst, local_num))
#define EMIT_LOAD_GLOBAL(qst) (comp->emit_method_table->load_id.global(comp->emit, qst))

#else

// if we only have the bytecode emitter enabled then we can do a direct call to the functions
#define EMIT(fun) (mp_emit_bc_##fun(comp->emit))
#define EMIT_ARG(fun, ...) (mp_emit_bc_##fun(comp->emit, __VA_ARGS__))
#define EMIT_LOAD_FAST(qst, local_num) (mp_emit_bc_load_fast(comp->emit, qst, local_num))
#define EMIT_LOAD_GLOBAL(qst) (mp_emit_bc_load_global(comp->emit, qst))

#endif

#define EMIT_INLINE_ASM(fun) (comp->emit_inline_asm_method_table->fun(comp->emit_inline_asm))
#define EMIT_INLINE_ASM_ARG(fun, ...) (comp->emit_inline_asm_method_table->fun(comp->emit_inline_asm, __VA_ARGS__))

// elements in this struct are ordered to make it compact
typedef struct _compiler_t {
    qstr source_file;

    uint8_t is_repl;
    uint8_t pass; // holds enum type pass_kind_t
    uint8_t func_arg_is_super; // used to compile special case of super() function call
    uint8_t have_star;

    // try to keep compiler clean from nlr
    mp_obj_t compile_error; // set to an exception object if there's an error
    size_t compile_error_line; // set to best guess of line of error

    uint next_label;

    uint16_t num_dict_params;
    uint16_t num_default_params;

    uint16_t break_label; // highest bit set indicates we are breaking out of a for loop
    uint16_t continue_label;
    uint16_t cur_except_level; // increased for SETUP_EXCEPT, SETUP_FINALLY; decreased for POP_BLOCK, POP_EXCEPT
    uint16_t break_continue_except_level;

    mp_uint_t *co_data;

    size_t num_scopes;
    scope_t **scopes;
    scope_t *scope_cur;

    emit_t *emit;                                   // current emitter
    #if NEED_METHOD_TABLE
    const emit_method_table_t *emit_method_table;   // current emit method table
    #endif

    #if MICROPY_EMIT_INLINE_THUMB
    emit_inline_asm_t *emit_inline_asm;                                   // current emitter for inline asm
    const emit_inline_asm_method_table_t *emit_inline_asm_method_table;   // current emit method table for inline asm
    #endif
} compiler_t;

STATIC void compile_error_set_line(compiler_t *comp, const byte *p) {
    // if the line of the error is unknown then try to update it from the parse data
    if (comp->compile_error_line == 0 && p != NULL && pt_is_any_rule(p)) {
        size_t rule_id, src_line;
        const byte *ptop;
        pt_rule_extract(p, &rule_id, &src_line, &ptop);
        comp->compile_error_line = src_line;
    }
}

STATIC void compile_syntax_error(compiler_t *comp, const byte *p, const char *msg) {
    // only register the error if there has been no other error
    if (comp->compile_error == MP_OBJ_NULL) {
        comp->compile_error = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
        compile_error_set_line(comp, p);
    }
}

STATIC void compile_trailer_paren_helper(compiler_t *comp, const byte *p_arglist, bool is_method_call, int n_positional_extra);
STATIC void compile_comprehension(compiler_t *comp, const byte *p, scope_kind_t kind);
STATIC const byte *compile_node(compiler_t *comp, const byte *p);

STATIC uint comp_next_label(compiler_t *comp) {
    return comp->next_label++;
}

STATIC void compile_increase_except_level(compiler_t *comp) {
    comp->cur_except_level += 1;
    if (comp->cur_except_level > comp->scope_cur->exc_stack_size) {
        comp->scope_cur->exc_stack_size = comp->cur_except_level;
    }
}

STATIC void compile_decrease_except_level(compiler_t *comp) {
    assert(comp->cur_except_level > 0);
    comp->cur_except_level -= 1;
}

STATIC void scope_new_and_link(compiler_t *comp, size_t scope_idx, scope_kind_t kind, const byte *p, uint emit_options) {
    scope_t *scope = scope_new(kind, p, comp->source_file, emit_options);
    scope->parent = comp->scope_cur;
    comp->scopes[scope_idx] = scope;
}

typedef void (*apply_list_fun_t)(compiler_t *comp, const byte *p);

STATIC void apply_to_single_or_list(compiler_t *comp, const byte *p, pn_kind_t pn_list_kind, apply_list_fun_t f) {
    if (pt_is_rule(p, pn_list_kind)) {
        const byte *ptop;
        p = pt_rule_extract_top(p, &ptop);
        while (p != ptop) {
            f(comp, p);
            p = pt_next(p);
        }
    } else if (!pt_is_null(p)) {
        f(comp, p);
    }
}

STATIC void compile_generic_all_nodes(compiler_t *comp, const byte *p, const byte *ptop) {
    while (p != ptop) {
        //printf("NODE: %02x %02x %02x %02x\n", p[0], p[1], p[2], p[3]);
        p = compile_node(comp, p);
    }
}

STATIC void compile_load_id(compiler_t *comp, qstr qst) {
    if (comp->pass == MP_PASS_SCOPE) {
        mp_emit_common_get_id_for_load(comp->scope_cur, qst);
    } else {
        #if NEED_METHOD_TABLE
        mp_emit_common_id_op(comp->emit, &comp->emit_method_table->load_id, comp->scope_cur, qst);
        #else
        mp_emit_common_id_op(comp->emit, &mp_emit_bc_method_table_load_id_ops, comp->scope_cur, qst);
        #endif
    }
}

STATIC void compile_store_id(compiler_t *comp, qstr qst) {
    if (comp->pass == MP_PASS_SCOPE) {
        mp_emit_common_get_id_for_modification(comp->scope_cur, qst);
    } else {
        #if NEED_METHOD_TABLE
        mp_emit_common_id_op(comp->emit, &comp->emit_method_table->store_id, comp->scope_cur, qst);
        #else
        mp_emit_common_id_op(comp->emit, &mp_emit_bc_method_table_store_id_ops, comp->scope_cur, qst);
        #endif
    }
}

STATIC void compile_delete_id(compiler_t *comp, qstr qst) {
    if (comp->pass == MP_PASS_SCOPE) {
        mp_emit_common_get_id_for_modification(comp->scope_cur, qst);
    } else {
        #if NEED_METHOD_TABLE
        mp_emit_common_id_op(comp->emit, &comp->emit_method_table->delete_id, comp->scope_cur, qst);
        #else
        mp_emit_common_id_op(comp->emit, &mp_emit_bc_method_table_delete_id_ops, comp->scope_cur, qst);
        #endif
    }
}

STATIC void c_tuple(compiler_t *comp, const byte *p, const byte *p_list, const byte *p_list_top) {
    int total = 0;
    if (p != NULL) {
        compile_node(comp, p);
        total += 1;
    }
    while (p_list != p_list_top) {
        p_list = compile_node(comp, p_list);
        total += 1;
    }
    EMIT_ARG(build_tuple, total);
}

STATIC void compile_generic_tuple(compiler_t *comp, const byte *p, const byte *ptop) {
    // a simple tuple expression
    c_tuple(comp, NULL, p, ptop);
}

STATIC bool node_is_const_false(const byte *p) {
    return pt_is_tok(p, MP_TOKEN_KW_FALSE)
        || (pt_is_small_int(p) && pt_small_int_value(p) == 0);
}

STATIC bool node_is_const_true(const byte *p) {
    return pt_is_tok(p, MP_TOKEN_KW_TRUE)
        || (pt_is_small_int(p) && pt_small_int_value(p) != 0);
}

STATIC const byte *c_if_cond(compiler_t *comp, const byte *p, bool jump_if, int label) {
    if (node_is_const_false(p)) {
        if (jump_if == false) {
            EMIT_ARG(jump, label);
        }
        return pt_next(p);
    } else if (node_is_const_true(p)) {
        if (jump_if == true) {
            EMIT_ARG(jump, label);
        }
        return pt_next(p);
    } else if (pt_is_any_rule(p)) {
        const byte *ptop;
        const byte *p2 = pt_rule_extract_top(p, &ptop);
        if (pt_is_rule(p, PN_or_test)) {
            if (jump_if == false) {
            and_or_logic1:;
                uint label2 = comp_next_label(comp);
                while (pt_next(p2) != ptop) {
                    p2 = c_if_cond(comp, p2, !jump_if, label2);
                }
                p2 = c_if_cond(comp, p2, jump_if, label);
                EMIT_ARG(label_assign, label2);
            } else {
            and_or_logic2:
                while (p2 != ptop) {
                    p2 = c_if_cond(comp, p2, jump_if, label);
                }
            }
            return p2;
        } else if (pt_is_rule(p, PN_and_test)) {
            if (jump_if == false) {
                goto and_or_logic2;
            } else {
                goto and_or_logic1;
            }
        } else if (pt_is_rule(p, PN_not_test_2)) {
            return c_if_cond(comp, p2, !jump_if, label);
        } else if (pt_is_rule(p, PN_atom_paren)) {
            // cond is something in parenthesis
            if (pt_is_rule_empty(p)) {
                // empty tuple, acts as false for the condition
                if (jump_if == false) {
                    EMIT_ARG(jump, label);
                }
            } else {
                assert(pt_is_rule(pt_rule_first(p), PN_testlist_comp));
                // non-empty tuple, acts as true for the condition
                if (jump_if == true) {
                    EMIT_ARG(jump, label);
                }
            }
            return pt_next(p);
        }
    }

    // nothing special, fall back to default compiling for node and jump
    p = compile_node(comp, p);
    EMIT_ARG(pop_jump_if, jump_if, label);
    return p;
}

typedef enum { ASSIGN_STORE, ASSIGN_AUG_LOAD, ASSIGN_AUG_STORE } assign_kind_t;
STATIC void c_assign(compiler_t *comp, const byte *p, assign_kind_t kind);

STATIC void c_assign_atom_expr(compiler_t *comp, const byte *p_orig, assign_kind_t assign_kind) {
    const byte *ptop;
    const byte *p0 = pt_rule_extract_top(p_orig, &ptop);

    if (assign_kind != ASSIGN_AUG_STORE) {
        compile_node(comp, p0);
    }

    const byte *p1 = pt_next(p0);

    if (pt_is_null_with_top(p1, ptop)) {
    cannot_assign:
        compile_syntax_error(comp, p_orig, "can't assign to expression");
        return;
    }

    if (pt_is_rule(p1, PN_atom_expr_trailers)) {
        const byte *p1top;
        p1 = pt_rule_extract_top(p1, &p1top);
        for (;;) {
            const byte *p1next = pt_next(p1);
            if (p1next >= p1top) {
                break;
            }
            if (assign_kind != ASSIGN_AUG_STORE) {
                compile_node(comp, p1);
            }
            p1 = p1next;
        }
        // p1 now points to final trailer for store
    }

    if (pt_is_rule(p1, PN_trailer_bracket)) {
        if (assign_kind == ASSIGN_AUG_STORE) {
            EMIT(rot_three);
            EMIT(store_subscr);
        } else {
            compile_node(comp, pt_rule_first(p1));
            if (assign_kind == ASSIGN_AUG_LOAD) {
                EMIT(dup_top_two);
                EMIT(load_subscr);
            } else {
                EMIT(store_subscr);
            }
        }
    } else if (pt_is_rule(p1, PN_trailer_period)) {
        qstr attr;
        pt_extract_id(pt_rule_first(p1), &attr);
        if (assign_kind == ASSIGN_AUG_LOAD) {
            EMIT(dup_top);
            EMIT_ARG(load_attr, attr);
        } else {
            if (assign_kind == ASSIGN_AUG_STORE) {
                EMIT(rot_two);
            }
            EMIT_ARG(store_attr, attr);
        }
    } else {
        goto cannot_assign;
    }

    if (!pt_is_null_with_top(pt_next(p1), ptop)) {
        goto cannot_assign;
    }
}

// we need to allow for a caller passing in 1 initial node followed by an array of nodes
STATIC void c_assign_tuple(compiler_t *comp, const byte *p_head, const byte *p_tail, const byte *p_tail_top) {
    uint num_head = (p_head == NULL) ? 0 : 1;
    uint num_tail = pt_num_nodes(p_tail, p_tail_top);

    // look for star expression
    const byte *p_star = NULL;
    if (num_head != 0 && pt_is_rule(p_head, PN_star_expr)) {
        EMIT_ARG(unpack_ex, 0, num_tail);
        p_star = p_head;
    }
    uint i = 0;
    for (const byte *p = p_tail; p != p_tail_top; p = pt_next(p), ++i) {
        if (pt_is_rule(p, PN_star_expr)) {
            if (p_star == NULL) {
                EMIT_ARG(unpack_ex, num_head + i, num_tail - i - 1);
                p_star = p;
            } else {
                compile_syntax_error(comp, p, "multiple *x in assignment");
                return;
            }
        }
    }
    if (p_star == NULL) {
        EMIT_ARG(unpack_sequence, num_head + num_tail);
    }
    if (num_head != 0) {
        if (p_head == p_star) {
            c_assign(comp, pt_rule_first(p_head), ASSIGN_STORE);
        } else {
            c_assign(comp, p_head, ASSIGN_STORE);
        }
    }
    for (const byte *p = p_tail; p != p_tail_top; p = pt_next(p)) {
        if (p == p_star) {
            c_assign(comp, pt_rule_first(p), ASSIGN_STORE);
        } else {
            c_assign(comp, p, ASSIGN_STORE);
        }
    }
}

// assigns top of stack to pn
STATIC void c_assign(compiler_t *comp, const byte *p, assign_kind_t assign_kind) {
    assert(!pt_is_null(p));
    if (pt_is_any_id(p)) {
        qstr arg;
        p = pt_extract_id(p, &arg);
        switch (assign_kind) {
            case ASSIGN_STORE:
            case ASSIGN_AUG_STORE:
                compile_store_id(comp, arg);
                break;
            case ASSIGN_AUG_LOAD:
            default:
                compile_load_id(comp, arg);
                break;
        }
    } else if (!pt_is_any_rule(p)) {
        compile_syntax_error(comp, p, "can't assign to literal");
    } else {
        switch (pt_rule_extract_rule_id(p)) {
            case PN_atom_expr_normal:
                // lhs is an index or attribute
                c_assign_atom_expr(comp, p, assign_kind);
                break;

            case PN_testlist_star_expr:
            case PN_exprlist: {
                // lhs is a tuple
                if (assign_kind != ASSIGN_STORE) {
                    goto bad_aug;
                }
                const byte *ptop;
                const byte *p0 = pt_rule_extract_top(p, &ptop);
                c_assign_tuple(comp, NULL, p0, ptop);
                break;
            }

            case PN_atom_paren: {
                // lhs is something in parenthesis
                const byte *ptop;
                const byte *p0 = pt_rule_extract_top(p, &ptop);
                if (pt_is_null_with_top(p0, ptop)) {
                    // empty tuple
                    goto cannot_assign;
                } else {
                    assert(pt_is_rule(p0, PN_testlist_comp));
                    if (assign_kind != ASSIGN_STORE) {
                        goto bad_aug;
                    }
                    p = p0;
                    goto testlist_comp;
                }
                break;
            }

            case PN_atom_bracket: {
                // lhs is something in brackets
                if (assign_kind != ASSIGN_STORE) {
                    goto bad_aug;
                }
                const byte *ptop;
                const byte *p0 = pt_rule_extract_top(p, &ptop); // skip rule header
                if (pt_is_null_with_top(p0, ptop)) {
                    // empty list, assignment allowed
                    c_assign_tuple(comp, NULL, NULL, NULL);
                } else if (pt_is_rule(p0, PN_testlist_comp)) {
                    p = p0;
                    goto testlist_comp;
                } else {
                    // brackets around 1 item
                    c_assign_tuple(comp, p0, NULL, NULL);
                }
                break;
            }

            default:
                goto cannot_assign;
        }
        return;

        testlist_comp:;
        // lhs is a sequence
        const byte *ptop;
        const byte *p0 = pt_rule_extract_top(p, &ptop);
        const byte *p1 = pt_next(p0);
        if (pt_is_rule(p1, PN_testlist_comp_3b)) {
            // sequence of one item, with trailing comma
            assert(pt_is_rule_empty(p1));
            c_assign_tuple(comp, p0, NULL, NULL);
        } else if (pt_is_rule(p1, PN_testlist_comp_3c)) {
            // sequence of many items
            p1 = pt_rule_extract_top(p1, &ptop);
            c_assign_tuple(comp, p0, p1, ptop);
        } else if (pt_is_rule(p1, PN_comp_for)) {
            // TODO can we ever get here? can it be compiled?
            goto cannot_assign;
        } else {
            // sequence with 2 items
            c_assign_tuple(comp, NULL, p0, pt_next(p1));
        }
    }
    return;

    cannot_assign:
    compile_syntax_error(comp, p, "can't assign to expression");
    return;

    bad_aug:
    compile_syntax_error(comp, p, "illegal expression for augmented assignment");
}

// stuff for lambda and comprehensions and generators:
//  if n_pos_defaults > 0 then there is a tuple on the stack with the positional defaults
//  if n_kw_defaults > 0 then there is a dictionary on the stack with the keyword defaults
//  if both exist, the tuple is above the dictionary (ie the first pop gets the tuple)
STATIC void close_over_variables_etc(compiler_t *comp, scope_t *this_scope, int n_pos_defaults, int n_kw_defaults) {
    assert(n_pos_defaults >= 0);
    assert(n_kw_defaults >= 0);

    // set flags
    if (n_kw_defaults > 0) {
        this_scope->scope_flags |= MP_SCOPE_FLAG_DEFKWARGS;
    }
    this_scope->num_def_pos_args = n_pos_defaults;

    // make closed over variables, if any
    // ensure they are closed over in the order defined in the outer scope (mainly to agree with CPython)
    int nfree = 0;
    if (comp->scope_cur->kind != SCOPE_MODULE) {
        for (int i = 0; i < comp->scope_cur->id_info_len; i++) {
            id_info_t *id = &comp->scope_cur->id_info[i];
            if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
                for (int j = 0; j < this_scope->id_info_len; j++) {
                    id_info_t *id2 = &this_scope->id_info[j];
                    if (id2->kind == ID_INFO_KIND_FREE && id->qst == id2->qst) {
                        // in Micro Python we load closures using LOAD_FAST
                        EMIT_LOAD_FAST(id->qst, id->local_num);
                        nfree += 1;
                    }
                }
            }
        }
    }

    // make the function/closure
    if (nfree == 0) {
        EMIT_ARG(make_function, this_scope, n_pos_defaults, n_kw_defaults);
    } else {
        EMIT_ARG(make_closure, this_scope, nfree, n_pos_defaults, n_kw_defaults);
    }
}

STATIC void compile_funcdef_lambdef_param(compiler_t *comp, const byte *p) {
    const byte *p_orig = p;

    if (pt_is_rule(p, PN_typedargslist_star)
        || pt_is_rule(p, PN_varargslist_star)) {
        comp->have_star = true;
        /* don't need to distinguish bare from named star
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
            // bare star
        } else {
            // named star
        }
        */

    } else if (pt_is_rule(p, PN_typedargslist_dbl_star)
        || pt_is_rule(p, PN_varargslist_dbl_star)) {
        // named double star
        // TODO do we need to do anything with this?

    } else {
        const byte *p_id;
        const byte *p_colon = NULL;
        const byte *p_equal = NULL;
        if (pt_is_any_id(p)) {
            // this parameter is just an id

            p_id = p;

        } else if (pt_is_rule(p, PN_typedargslist_name)) {
            // this parameter has a colon and/or equal specifier

            const byte *ptop;
            p = pt_rule_extract_top(p, &ptop);

            p_id = p;
            p = pt_next(p);
            if (p != ptop) {
                p_colon = p;
                p = pt_next(p);
                if (p != ptop) {
                    p_equal = p;
                }
            }

        } else {
            assert(pt_is_rule(p, PN_varargslist_name)); // should be
            // this parameter has an equal specifier

            p_id = pt_rule_first(p);
            p_equal = pt_next(p_id);
        }

        qstr q_id;
        pt_extract_id(p_id, &q_id);

        if (p_equal == NULL || pt_is_null(p_equal)) {
            // this parameter does not have a default value

            // check for non-default parameters given after default parameters (allowed by parser, but not syntactically valid)
            if (!comp->have_star && comp->num_default_params != 0) {
                compile_syntax_error(comp, p_orig, "non-default argument follows default argument");
                return;
            }

        } else {
            // this parameter has a default value
            // in CPython, None (and True, False?) as default parameters are loaded with LOAD_NAME; don't understandy why

            if (comp->have_star) {
                comp->num_dict_params += 1;
                // in Micro Python we put the default dict parameters into a dictionary using the bytecode
                if (comp->num_dict_params == 1) {
                    // in Micro Python we put the default positional parameters into a tuple using the bytecode
                    // we need to do this here before we start building the map for the default keywords
                    if (comp->num_default_params > 0) {
                        EMIT_ARG(build_tuple, comp->num_default_params);
                    } else {
                        EMIT(load_null); // sentinel indicating empty default positional args
                    }
                    // first default dict param, so make the map
                    EMIT_ARG(build_map, 0);
                }

                // compile value then key, then store it to the dict
                compile_node(comp, p_equal);
                EMIT_ARG(load_const_str, q_id);
                EMIT(store_map);
            } else {
                comp->num_default_params += 1;
                compile_node(comp, p_equal);
            }
        }

        // TODO p_colon not implemented
        (void)p_colon;
    }
}

STATIC void compile_funcdef_lambdef(compiler_t *comp, scope_t *scope, const byte *p, pn_kind_t pn_list_kind) {
    // When we call compile_funcdef_lambdef_param below it can compile an arbitrary
    // expression for default arguments, which may contain a lambda.  The lambda will
    // call here in a nested way, so we must save and restore the relevant state.
    bool orig_have_star = comp->have_star;
    uint16_t orig_num_dict_params = comp->num_dict_params;
    uint16_t orig_num_default_params = comp->num_default_params;

    // compile default parameters
    comp->have_star = false;
    comp->num_dict_params = 0;
    comp->num_default_params = 0;
    apply_to_single_or_list(comp, p, pn_list_kind, compile_funcdef_lambdef_param);

    if (comp->compile_error != MP_OBJ_NULL) {
        return;
    }

    // in Micro Python we put the default positional parameters into a tuple using the bytecode
    // the default keywords args may have already made the tuple; if not, do it now
    if (comp->num_default_params > 0 && comp->num_dict_params == 0) {
        EMIT_ARG(build_tuple, comp->num_default_params);
        EMIT(load_null); // sentinel indicating empty default keyword args
    }

    // make the function
    close_over_variables_etc(comp, scope, comp->num_default_params, comp->num_dict_params);

    // restore state
    comp->have_star = orig_have_star;
    comp->num_dict_params = orig_num_dict_params;
    comp->num_default_params = orig_num_default_params;
}

// leaves function object on stack
// returns function name
STATIC qstr compile_funcdef_helper(compiler_t *comp, const byte *p, uint emit_options) {
    mp_int_t scope_idx;
    p = pt_get_small_int(p, &scope_idx);

    if (comp->pass == MP_PASS_SCOPE) {
        // create a new scope for this function
        scope_new_and_link(comp, scope_idx, SCOPE_FUNCTION, p, emit_options);
    }

    p = pt_next(p); // skip function name

    // get the scope for this function
    scope_t *fscope = comp->scopes[scope_idx];

    // compile the function definition
    compile_funcdef_lambdef(comp, fscope, p, PN_typedargslist);

    // return its name (the 'f' in "def f(...):")
    return fscope->simple_name;
}

// leaves class object on stack
// returns class name
STATIC qstr compile_classdef_helper(compiler_t *comp, const byte *p, uint emit_options) {
    mp_int_t scope_idx;
    p = pt_get_small_int(p, &scope_idx);

    if (comp->pass == MP_PASS_SCOPE) {
        // create a new scope for this class
        scope_new_and_link(comp, scope_idx, SCOPE_CLASS, p, emit_options);
    }

    EMIT(load_build_class);

    // scope for this class
    scope_t *cscope = comp->scopes[scope_idx];

    // compile the class
    close_over_variables_etc(comp, cscope, 0, 0);

    // get its name
    EMIT_ARG(load_const_str, cscope->simple_name);

    // second node has parent classes, if any
    // empty parenthesis (eg class C():) gets here as an empty PN_classdef_2 and needs special handling
    const byte *p_parents = pt_next(p);
    if (pt_is_rule(p_parents, PN_classdef_2)) {
        p_parents = NULL;
    }
    comp->func_arg_is_super = false;
    compile_trailer_paren_helper(comp, p_parents, false, 2);

    // return its name (the 'C' in class C(...):")
    return cscope->simple_name;
}

// returns true if it was a built-in decorator (even if the built-in had an error)
STATIC bool compile_built_in_decorator(compiler_t *comp, const byte *p, const byte *ptop, uint *emit_options) {
    qstr qst;
    p = pt_extract_id(p, &qst);
    if (qst != MP_QSTR_micropython) {
        return false;
    }

    if (p >= ptop || pt_next(p) != ptop) {
        compile_syntax_error(comp, NULL, "invalid micropython decorator");
        return true;
    }

    qstr attr;
    p = pt_extract_id(p, &attr);
    if (attr == MP_QSTR_bytecode) {
        *emit_options = MP_EMIT_OPT_BYTECODE;
#if MICROPY_EMIT_NATIVE
    } else if (attr == MP_QSTR_native) {
        *emit_options = MP_EMIT_OPT_NATIVE_PYTHON;
    } else if (attr == MP_QSTR_viper) {
        *emit_options = MP_EMIT_OPT_VIPER;
#endif
#if MICROPY_EMIT_INLINE_THUMB
    } else if (attr == MP_QSTR_asm_thumb) {
        *emit_options = MP_EMIT_OPT_ASM_THUMB;
#endif
    } else {
        compile_syntax_error(comp, NULL, "invalid micropython decorator");
    }

    return true;
}

STATIC void compile_decorated(compiler_t *comp, const byte *p, const byte *ptop) {
    // get the list of decorators
    ptop = mp_parse_node_extract_list(&p, PN_decorators);

    // inherit emit options for this function/class definition
    uint emit_options = comp->scope_cur->emit_options;

    // compile each decorator
    int num_non_built_in_decorators = 0;
    while (p != ptop) {
        assert(pt_is_rule(p, PN_decorator)); // should be

        const byte *ptop_decorator;
        p = pt_rule_extract_top(p, &ptop_decorator);

        // first node contains the decorator function, which is a dotted name
        const byte *ptop_dotted_name = mp_parse_node_extract_list(&p, PN_dotted_name);

        // check for built-in decorators
        if (compile_built_in_decorator(comp, p, ptop_dotted_name, &emit_options)) {
            // this was a built-in

        } else {
            // not a built-in, compile normally

            num_non_built_in_decorators += 1;

            // compile the decorator function
            p = compile_node(comp, p);
            while (p != ptop_dotted_name) {
                assert(pt_is_any_id(p)); // should be
                qstr qst;
                p = pt_extract_id(p, &qst);
                EMIT_ARG(load_attr, qst);
            }

            // nodes[1] contains arguments to the decorator function, if any
            if (!pt_is_null_with_top(p, ptop_decorator)) {
                // call the decorator function with the arguments in nodes[1]
                comp->func_arg_is_super = false;
                compile_node(comp, p);
            }
        }

        p = ptop_decorator;
    }

    // compile the body (funcdef or classdef) and get its name
    qstr body_name = 0;
    p = pt_rule_first(ptop); // skip the rule header
    if (pt_is_rule(ptop, PN_funcdef)) {
        body_name = compile_funcdef_helper(comp, p, emit_options);
    } else {
        assert(pt_is_rule(ptop, PN_classdef)); // should be
        body_name = compile_classdef_helper(comp, p, emit_options);
    }

    // call each decorator
    while (num_non_built_in_decorators-- > 0) {
        EMIT_ARG(call_function, 1, 0, 0);
    }

    // store func/class object into name
    compile_store_id(comp, body_name);
}

STATIC void compile_funcdef(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    qstr fname = compile_funcdef_helper(comp, p, comp->scope_cur->emit_options);
    // store function object into function name
    compile_store_id(comp, fname);
}

STATIC void c_del_stmt(compiler_t *comp, const byte *p) {
    if (pt_is_any_id(p)) {
        qstr id;
        pt_extract_id(p, &id);
        compile_delete_id(comp, id);
    } else if (pt_is_rule(p, PN_atom_expr_normal)) {
        const byte *ptop;
        const byte *p0 = pt_rule_extract_top(p, &ptop);

        const byte *p1 = compile_node(comp, p0); // base of the power node

        if (pt_is_rule(p1, PN_atom_expr_trailers)) {
            const byte *p1top;
            p1 = pt_rule_extract_top(p1, &p1top);
            for (;;) {
                const byte *p1next = pt_next(p1);
                if (p1next == p1top) {
                    break;
                }
                compile_node(comp, p1);
                p1 = p1next;
            }
            // p1 now points to final trailer for delete
        }

        const byte *p2;
        if (pt_is_rule(p1, PN_trailer_bracket)) {
            p2 = compile_node(comp, pt_rule_first(p1));
            EMIT(delete_subscr);
        } else if (pt_is_rule(p1, PN_trailer_period)) {
            qstr id;
            p2 = pt_extract_id(pt_rule_first(p1), &id);
            EMIT_ARG(delete_attr, id);
        } else {
            goto cannot_delete;
        }

        if (!pt_is_null_with_top(p2, ptop)) {
            goto cannot_delete;
        }
    } else if (pt_is_rule(p, PN_atom_paren)) {
        if (pt_is_rule_empty(p)) {
            goto cannot_delete;
        } else {
            p = pt_rule_first(p);
            assert(pt_is_rule(p, PN_testlist_comp));
            // TODO perhaps factorise testlist_comp code with other uses of PN_testlist_comp
            // or, simplify the logic here my making the parser simplify everything to a list
            const byte *p0 = pt_rule_first(p);
            c_del_stmt(comp, p0);

            const byte *p1 = pt_next(p0);
            if (pt_is_rule(p1, PN_testlist_comp_3b)) {
                // sequence of one item, with trailing comma
                assert(pt_is_rule_empty(p1));
            } else if (pt_is_rule(p1, PN_testlist_comp_3c)) {
                // sequence of many items
                const byte *ptop;
                p1 = pt_rule_extract_top(p1, &ptop);
                while (p1 != ptop) {
                    c_del_stmt(comp, p1);
                    p1 = pt_next(p1);
                }
            } else if (pt_is_rule(p1, PN_comp_for)) {
                // TODO not implemented; can't del comprehension? can we get here?
                goto cannot_delete;
            } else {
                // sequence with 2 items
                c_del_stmt(comp, p1);
            }
        }
    } else {
        // some arbitrary statment that we can't delete (eg del 1)
        goto cannot_delete;
    }

    return;

cannot_delete:
    compile_syntax_error(comp, p, "can't delete expression");
}

STATIC void compile_del_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    apply_to_single_or_list(comp, p, PN_exprlist, c_del_stmt);
}

STATIC void compile_break_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    if (comp->break_label == 0) {
        compile_syntax_error(comp, p, "'break' outside loop");
    }
    assert(comp->cur_except_level >= comp->break_continue_except_level);
    EMIT_ARG(break_loop, comp->break_label, comp->cur_except_level - comp->break_continue_except_level);
}

STATIC void compile_continue_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    if (comp->continue_label == 0) {
        compile_syntax_error(comp, p, "'continue' outside loop");
    }
    assert(comp->cur_except_level >= comp->break_continue_except_level);
    EMIT_ARG(continue_loop, comp->continue_label, comp->cur_except_level - comp->break_continue_except_level);
}

STATIC void compile_return_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION) {
        compile_syntax_error(comp, NULL, "'return' outside function");
        return;
    }
    if (pt_is_null_with_top(p, ptop)) {
        // no argument to 'return', so return None
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    #if 0
    // TODO do we need this optimisation?  i guess it's hardly used
    } else if (pt_is_rule(p, PN_test_if_expr)) {
        // special case when returning an if-expression; to match CPython optimisation
        mp_parse_node_struct_t *pns_test_if_expr = (mp_parse_node_struct_t*)pns->nodes[0];
        mp_parse_node_struct_t *pns_test_if_else = (mp_parse_node_struct_t*)pns_test_if_expr->nodes[1];

        uint l_fail = comp_next_label(comp);
        c_if_cond(comp, pns_test_if_else->nodes[0], false, l_fail); // condition
        compile_node(comp, pns_test_if_expr->nodes[0]); // success value
        EMIT(return_value);
        EMIT_ARG(label_assign, l_fail);
        compile_node(comp, pns_test_if_else->nodes[1]); // failure value
    #endif
    } else {
        compile_node(comp, p);
    }
    EMIT(return_value);
}

STATIC void compile_yield_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    compile_node(comp, p);
    EMIT(pop_top);
}

STATIC void compile_raise_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    if (pt_is_null_with_top(p, ptop)) {
        // raise
        EMIT_ARG(raise_varargs, 0);
    } else if (pt_is_rule(p, PN_raise_stmt_arg)) {
        // raise x from y
        p = pt_rule_first(p);
        p = compile_node(comp, p);
        compile_node(comp, p);
        EMIT_ARG(raise_varargs, 2);
    } else {
        // raise x
        compile_node(comp, p);
        EMIT_ARG(raise_varargs, 1);
    }
}

// q_base holds the base of the name
// eg   a -> q_base=a
//      a.b.c -> q_base=a
STATIC void do_import_name(compiler_t *comp, const byte *p, qstr *q_base) {
    bool is_as = false;
    if (p != NULL && pt_is_rule(p, PN_dotted_as_name)) {
        // a name of the form x as y; unwrap it
        p = pt_rule_first(p); // point to 'x'
        pt_extract_id(pt_next(p), q_base); // extract 'y'
        is_as = true;
    }
    if (p == NULL || pt_is_null(p)) {
        // empty name (eg, from . import x)
        *q_base = MP_QSTR_;
        EMIT_ARG(import_name, MP_QSTR_); // import the empty string
    } else if (pt_is_any_id(p)) {
        // just a simple name
        qstr q_full;
        pt_extract_id(p, &q_full);
        if (!is_as) {
            *q_base = q_full;
        }
        EMIT_ARG(import_name, q_full);
    } else {
        // a name of the form a.b.c
        assert(pt_is_rule(p, PN_dotted_name)); // should be
        const byte *ptop;
        p = pt_rule_extract_top(p, &ptop);

        if (!is_as) {
            pt_extract_id(p, q_base);
        }

        // work out string length
        int len = -1;
        for (const byte *p2 = p; p2 != ptop;) {
            qstr qst;
            p2 = pt_extract_id(p2, &qst);
            len += 1 + qstr_len(qst);
        }

        // build string
        byte *q_ptr;
        byte *str_dest = qstr_build_start(len, &q_ptr);
        for (const byte *p2 = p; p2 != ptop;) {
            if (p2 > p) {
                *str_dest++ = '.';
            }
            qstr qst;
            p2 = pt_extract_id(p2, &qst);
            size_t str_src_len;
            const byte *str_src = qstr_data(qst, &str_src_len);
            memcpy(str_dest, str_src, str_src_len);
            str_dest += str_src_len;
        }
        qstr q_full = qstr_build_end(q_ptr);
        EMIT_ARG(import_name, q_full);
        if (is_as) {
            for (const byte *p2 = pt_next(p); p2 != ptop;) {
                qstr qst;
                p2 = pt_extract_id(p2, &qst);
                EMIT_ARG(load_attr, qst);
            }
        }
    }
}

STATIC void compile_dotted_as_name(compiler_t *comp, const byte *p) {
    EMIT_ARG(load_const_small_int, 0); // level 0 import
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE); // not importing from anything
    qstr q_base;
    do_import_name(comp, p, &q_base);
    compile_store_id(comp, q_base);
}

STATIC void compile_import_name(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    apply_to_single_or_list(comp, p, PN_dotted_as_names, compile_dotted_as_name);
}

STATIC void compile_import_from(compiler_t *comp, const byte *p, const byte *ptop) {
    const byte *p_import_source = p;

    // extract the preceeding .'s (if any) for a relative import, to compute the import level
    uint import_level = 0;
    do {
        const byte *p_rel;
        if (pt_is_any_tok(p_import_source) || pt_is_rule(p_import_source, PN_one_or_more_period_or_ellipsis)) {
            // This covers relative imports with dots only like "from .. import"
            p_rel = p_import_source;
            p_import_source = NULL;
        } else if (pt_is_rule(p_import_source, PN_import_from_2b)) {
            // This covers relative imports starting with dot(s) like "from .foo import"
            p_rel = pt_rule_first(p_import_source);
            p_import_source = pt_next(p_rel);
        } else {
            // Not a relative import
            break;
        }

        // get the list of . and/or ...'s
        const byte *p_rel_top = mp_parse_node_extract_list(&p_rel, PN_one_or_more_period_or_ellipsis);

        // count the total number of .'s
        while (p_rel != p_rel_top) {
            if (pt_is_tok(p_rel, MP_TOKEN_DEL_PERIOD)) {
                import_level++;
            } else {
                // should be an MP_TOKEN_ELLIPSIS
                import_level += 3;
            }
            p_rel = pt_next(p_rel);
        }
    } while (0);

    p = pt_next(p);

    if (pt_is_tok(p, MP_TOKEN_OP_STAR)) {
        EMIT_ARG(load_const_small_int, import_level);

        // build the "fromlist" tuple
        EMIT_ARG(load_const_str, MP_QSTR__star_);
        EMIT_ARG(build_tuple, 1);

        // do the import
        qstr dummy_q;
        do_import_name(comp, p_import_source, &dummy_q);
        EMIT(import_star);

    } else {
        EMIT_ARG(load_const_small_int, import_level);

        // build the "fromlist" tuple
        ptop = mp_parse_node_extract_list(&p, PN_import_as_names);
        uint n = 0;
        for (const byte *p_list = p; p_list < ptop; p_list = pt_next(p_list), ++n) {
            assert(pt_is_rule(p_list, PN_import_as_name));
            qstr id2;
            pt_extract_id(pt_rule_first(p_list), &id2);
            EMIT_ARG(load_const_str, id2);
        }
        EMIT_ARG(build_tuple, n);

        // do the import
        qstr dummy_q;
        do_import_name(comp, p_import_source, &dummy_q);
        for (const byte *p_list = p; p_list < ptop;) {
            assert(pt_is_rule(p_list, PN_import_as_name));
            const byte *p_list_top;
            p_list = pt_rule_extract_top(p_list, &p_list_top);
            qstr id2;
            p_list = pt_extract_id(p_list, &id2);
            EMIT_ARG(import_from, id2);
            if (p_list == p_list_top) {
                compile_store_id(comp, id2);
            } else {
                qstr id3;
                p_list = pt_extract_id(p_list, &id3);
                compile_store_id(comp, id3);
            }
        }
        EMIT(pop_top);
    }
}

STATIC void compile_declare_global(compiler_t *comp, const byte *p_for_err, qstr qst) {
    bool added;
    id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, qst, &added);
    if (!added && id_info->kind != ID_INFO_KIND_GLOBAL_EXPLICIT) {
        compile_syntax_error(comp, p_for_err, "identifier redefined as global");
        return;
    }
    id_info->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;

    // if the id exists in the global scope, set its kind to EXPLICIT_GLOBAL
    id_info = scope_find_global(comp->scope_cur, qst);
    if (id_info != NULL) {
        id_info->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
    }
}

STATIC void compile_global_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    if (comp->pass == MP_PASS_SCOPE) {
        const byte *p_orig = p;
        ptop = mp_parse_node_extract_list(&p, PN_name_list);
        while (p != ptop) {
            qstr qst;
            p = pt_extract_id(p, &qst);
            compile_declare_global(comp, p_orig, qst);
        }
    }
}

STATIC void compile_declare_nonlocal(compiler_t *comp, const byte *p_for_err, qstr qst) {
    bool added;
    id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, qst, &added);
    if (!added && id_info->kind != ID_INFO_KIND_FREE) {
        compile_syntax_error(comp, p_for_err, "identifier redefined as nonlocal");
        return;
    }
    id_info_t *id_info2 = scope_find_local_in_parent(comp->scope_cur, qst);
    if (id_info2 == NULL || !(id_info2->kind == ID_INFO_KIND_LOCAL
        || id_info2->kind == ID_INFO_KIND_CELL || id_info2->kind == ID_INFO_KIND_FREE)) {
        compile_syntax_error(comp, p_for_err, "no binding for nonlocal found");
        return;
    }
    id_info->kind = ID_INFO_KIND_FREE;
    scope_close_over_in_parents(comp->scope_cur, qst);
}

STATIC void compile_nonlocal_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    if (comp->pass == MP_PASS_SCOPE) {
        if (comp->scope_cur->kind == SCOPE_MODULE) {
            compile_syntax_error(comp, p, "can't declare nonlocal in outer code");
            return;
        }
        const byte *p_orig = p;
        ptop = mp_parse_node_extract_list(&p, PN_name_list);
        while (p != ptop) {
            qstr qst;
            p = pt_extract_id(p, &qst);
            compile_declare_nonlocal(comp, p_orig, qst);
        }
    }
}

STATIC void compile_assert_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    // with optimisations enabled we don't compile assertions
    if (MP_STATE_VM(mp_optimise_value) != 0) {
        return;
    }

    uint l_end = comp_next_label(comp);
    p = c_if_cond(comp, p, true, l_end);
    EMIT_LOAD_GLOBAL(MP_QSTR_AssertionError); // we load_global instead of load_id, to be consistent with CPython
    if (!pt_is_null_with_top(p, ptop)) {
        // assertion message
        compile_node(comp, p);
        EMIT_ARG(call_function, 1, 0, 0);
    }
    EMIT_ARG(raise_varargs, 1);
    EMIT_ARG(label_assign, l_end);
}

STATIC void compile_if_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    // TODO proper and/or short circuiting

    uint l_end = comp_next_label(comp);

    // optimisation: don't emit anything when "if False"
    if (node_is_const_false(p)) {
        p = pt_next(p); // skip if condition
        p = pt_next(p); // skip if block
    } else {
        uint l_fail = comp_next_label(comp);
        bool if_true = node_is_const_true(p);
        p = c_if_cond(comp, p, false, l_fail); // if condition

        p = compile_node(comp, p); // if block

        // optimisation: skip everything else when "if True"
        if (if_true) {
            goto done;
        }

        if (
            // optimisation: don't jump over non-existent elif/else blocks
            !(pt_is_null_with_top(p, ptop) && pt_is_null_with_top(pt_next(p), ptop))
            // optimisation: don't jump if last instruction was return
            && !EMIT(last_emit_was_return_value)
            ) {
            // jump over elif/else blocks
            EMIT_ARG(jump, l_end);
        }

        EMIT_ARG(label_assign, l_fail);
    }

    // at this point p points to elif node (which may not exist)

    // compile elif blocks (if any)
    if (p != ptop) {
        const byte *p_else_top = mp_parse_node_extract_list(&p, PN_if_stmt_elif_list);
        while (p != p_else_top) {
            assert(pt_is_rule(p, PN_if_stmt_elif)); // should be
            p = pt_rule_first(p);

            // optimisation: don't emit anything when "if False"
            if (node_is_const_false(p)) {
                p = pt_next(p); // skip elif condition
                p = pt_next(p); // skip elif block
            } else {
                uint l_fail = comp_next_label(comp);
                bool elif_true = node_is_const_true(p);
                p = c_if_cond(comp, p, false, l_fail); // elif condition

                p = compile_node(comp, p); // elif block

                // optimisation: skip everything else when "elif True"
                if (elif_true) {
                    goto done;
                }

                // optimisation: don't jump if last instruction was return
                if (!EMIT(last_emit_was_return_value)) {
                    EMIT_ARG(jump, l_end);
                }
                EMIT_ARG(label_assign, l_fail);
            }
        }

        // compile else block (if any)
        if (p != ptop) {
            compile_node(comp, p);
        }
    }

done:
    EMIT_ARG(label_assign, l_end);
}

#define START_BREAK_CONTINUE_BLOCK \
    uint16_t old_break_label = comp->break_label; \
    uint16_t old_continue_label = comp->continue_label; \
    uint16_t old_break_continue_except_level = comp->break_continue_except_level; \
    uint break_label = comp_next_label(comp); \
    uint continue_label = comp_next_label(comp); \
    comp->break_label = break_label; \
    comp->continue_label = continue_label; \
    comp->break_continue_except_level = comp->cur_except_level;

#define END_BREAK_CONTINUE_BLOCK \
    comp->break_label = old_break_label; \
    comp->continue_label = old_continue_label; \
    comp->break_continue_except_level = old_break_continue_except_level;

STATIC void compile_while_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    START_BREAK_CONTINUE_BLOCK

    const byte *p_body = pt_next(p);
    const byte *p_else = pt_next(p_body);

    if (!node_is_const_false(p)) { // optimisation: don't emit anything for "while False"
        uint top_label = comp_next_label(comp);
        if (!node_is_const_true(p)) { // optimisation: don't jump to cond for "while True"
            EMIT_ARG(jump, continue_label);
        }
        EMIT_ARG(label_assign, top_label);
        compile_node(comp, p_body); // body
        EMIT_ARG(label_assign, continue_label);
        c_if_cond(comp, p, true, top_label); // condition
    }

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    if (p_else != ptop) {
        compile_node(comp, p_else); // else
    }

    EMIT_ARG(label_assign, break_label);
}

// This function compiles an optimised for-loop of the form:
//      for <var> in range(<start>, <end>, <step>):
//          <body>
//      else:
//          <else>
// <var> must be an identifier and <step> must be a small-int.
//
// Semantics of for-loop require:
//  - final failing value should not be stored in the loop variable
//  - if the loop never runs, the loop variable should never be assigned
//  - assignments to <var>, <end> or <step> in the body do not alter the loop
//    (<step> is a constant for us, so no need to worry about it changing)
//
// If <end> is a small-int, then the stack during the for-loop contains just
// the current value of <var>.  Otherwise, the stack contains <end> then the
// current value of <var>.
STATIC void compile_for_stmt_optimised_range(compiler_t *comp, const byte *pn_var,
    const byte *pn_start, const byte *pn_end, mp_int_t step,
    const byte *pn_body, const byte *pn_else) {

    START_BREAK_CONTINUE_BLOCK

    uint top_label = comp_next_label(comp);
    uint entry_label = comp_next_label(comp);

    // put the end value on the stack if it's not a small-int constant
    bool end_on_stack = !pt_is_small_int(pn_end);
    if (end_on_stack) {
        compile_node(comp, pn_end);
    }

    // compile: start
    compile_node(comp, pn_start);

    EMIT_ARG(jump, entry_label);
    EMIT_ARG(label_assign, top_label);

    // duplicate next value and store it to var
    EMIT(dup_top);
    c_assign(comp, pn_var, ASSIGN_STORE);

    // compile body
    compile_node(comp, pn_body);

    EMIT_ARG(label_assign, continue_label);

    // compile: var + step
    EMIT_ARG(load_const_small_int, step);
    EMIT_ARG(binary_op, MP_BINARY_OP_INPLACE_ADD);

    EMIT_ARG(label_assign, entry_label);

    // compile: if var <cond> end: goto top
    if (end_on_stack) {
        EMIT(dup_top_two);
        EMIT(rot_two);
    } else {
        EMIT(dup_top);
        compile_node(comp, pn_end);
    }
    if (step >= 0) {
        EMIT_ARG(binary_op, MP_BINARY_OP_LESS);
    } else {
        EMIT_ARG(binary_op, MP_BINARY_OP_MORE);
    }
    EMIT_ARG(pop_jump_if, true, top_label);

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    if (pn_else != NULL) {
        compile_node(comp, pn_else);
    }

    EMIT_ARG(label_assign, break_label);

    // discard final value of var that failed the loop condition
    EMIT(pop_top);

    // discard <end> value if it's on the stack
    if (end_on_stack) {
        EMIT(pop_top);
    }
}

STATIC void compile_for_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    // this bit optimises: for <x> in range(...), turning it into an explicitly incremented variable
    // this is actually slower, but uses no heap memory
    // for viper it will be much, much faster
    if (/*comp->scope_cur->emit_options == MP_EMIT_OPT_VIPER &&*/ pt_is_any_id(p)
        && pt_is_rule(pt_next(p), PN_atom_expr_normal)) {
        const byte *p_it_top;
        const byte *p_it0 = pt_rule_extract_top(pt_next(p), &p_it_top);
        if (!pt_is_id(p_it0, MP_QSTR_range)) {
            goto optimise_fail;
        }
        const byte *p_it1 = pt_next(p_it0);
        if (pt_is_rule(p_it1, PN_trailer_paren)
            && !pt_is_rule_empty(p_it1)
            && pt_next(p_it1) == p_it_top) {
            // iterator is of the form range(...) with at least 1 arg
            const byte *p_range_args = pt_rule_first(p_it1);
            const byte *p_range_args_top = mp_parse_node_extract_list(&p_range_args, PN_arglist);
            const byte *p_start = pt_const_int0;
            const byte *p_end = p_range_args;
            mp_int_t step = 1;
            p_range_args = pt_next(p_range_args);
            if (p_range_args != p_range_args_top) {
                // range has at least 2 args
                p_start = p_end;
                p_end = p_range_args;
                p_range_args = pt_next(p_range_args);
                if (p_range_args != p_range_args_top) {
                    // range has at least 3 args
                    // We need to know sign of step. This is possible only if it's constant
                    if (!pt_is_small_int(p_range_args)) {
                        goto optimise_fail;
                    }
                    p_range_args = pt_get_small_int(p_range_args, &step);
                    if (p_range_args != p_range_args_top) {
                        // range has at least 4 args, so don't know how to optimise it
                        goto optimise_fail;
                    }
                }
            }
            // arguments must be able to be compiled as standard expressions
            if (pt_is_any_rule(p_start)) {
                int k = pt_rule_extract_rule_id(p_start);
                if (k == PN_arglist_star || k == PN_arglist_dbl_star || k == PN_argument) {
                    goto optimise_fail;
                }
            }
            if (pt_is_any_rule(p_end)) {
                int k = pt_rule_extract_rule_id(p_end);
                if (k == PN_arglist_star || k == PN_arglist_dbl_star || k == PN_argument) {
                    goto optimise_fail;
                }
            }
            // can optimise
            const byte *p_body = p_it_top;
            const byte *p_else = pt_next(p_body);
            if (p_else == ptop) {
                p_else = NULL;
            }
            compile_for_stmt_optimised_range(comp, p, p_start, p_end, step, p_body, p_else);
            return;
        }
    }
optimise_fail:;

    START_BREAK_CONTINUE_BLOCK
    comp->break_label |= MP_EMIT_BREAK_FROM_FOR;

    uint pop_label = comp_next_label(comp);

    const byte *p_it = pt_next(p);
    const byte *p_body = compile_node(comp, p_it); // iterator
    EMIT(get_iter);
    EMIT_ARG(label_assign, continue_label);
    EMIT_ARG(for_iter, pop_label);
    c_assign(comp, p, ASSIGN_STORE); // variable
    const byte *p_else = compile_node(comp, p_body); // body
    if (!EMIT(last_emit_was_return_value)) {
        EMIT_ARG(jump, continue_label);
    }
    EMIT_ARG(label_assign, pop_label);
    EMIT(for_iter_end);

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    if (p_else != ptop) {
        compile_node(comp, p_else); // else
    }

    EMIT_ARG(label_assign, break_label);
}

STATIC void compile_try_except(compiler_t *comp, const byte *p_body, const byte *p_except, const byte *p_except_top, const byte *p_else) {
    // setup code
    uint l1 = comp_next_label(comp);
    uint success_label = comp_next_label(comp);

    EMIT_ARG(setup_except, l1);
    compile_increase_except_level(comp);

    compile_node(comp, p_body); // body
    EMIT(pop_block);
    EMIT_ARG(jump, success_label); // jump over exception handler

    EMIT_ARG(label_assign, l1); // start of exception handler
    EMIT(start_except_handler);

    uint l2 = comp_next_label(comp);

    while (p_except != p_except_top) {
        assert(pt_is_rule(p_except, PN_try_stmt_except)); // should be
        p_except = pt_rule_first(p_except);

        qstr qstr_exception_local = 0;
        uint end_finally_label = comp_next_label(comp);

        if (pt_is_null(p_except)) {
            // this is a catch all exception handler
            if (pt_next(pt_next(p_except)) != p_except_top) {
                compile_syntax_error(comp, p_except, "default 'except:' must be last");
                compile_decrease_except_level(comp);
                return;
            }
        } else {
            // this exception handler requires a match to a certain type of exception
            const byte *p_exception_expr = p_except;
            if (pt_is_rule(p_exception_expr, PN_try_stmt_as_name)) {
                // handler binds the exception to a local
                p_exception_expr = pt_rule_first(p_exception_expr);
                pt_extract_id(pt_next(p_exception_expr), &qstr_exception_local);
            }
            EMIT(dup_top);
            compile_node(comp, p_exception_expr);
            EMIT_ARG(binary_op, MP_BINARY_OP_EXCEPTION_MATCH);
            EMIT_ARG(pop_jump_if, false, end_finally_label);
        }

        p_except = pt_next(p_except);

        EMIT(pop_top);

        if (qstr_exception_local == 0) {
            EMIT(pop_top);
        } else {
            compile_store_id(comp, qstr_exception_local);
        }

        EMIT(pop_top);

        uint l3 = 0;
        if (qstr_exception_local != 0) {
            l3 = comp_next_label(comp);
            EMIT_ARG(setup_finally, l3);
            compile_increase_except_level(comp);
        }
        p_except = compile_node(comp, p_except);
        if (qstr_exception_local != 0) {
            EMIT(pop_block);
        }
        EMIT(pop_except);
        if (qstr_exception_local != 0) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
            EMIT_ARG(label_assign, l3);
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
            compile_store_id(comp, qstr_exception_local);
            compile_delete_id(comp, qstr_exception_local);

            compile_decrease_except_level(comp);
            EMIT(end_finally);
        }
        EMIT_ARG(jump, l2);
        EMIT_ARG(label_assign, end_finally_label);
        EMIT_ARG(adjust_stack_size, 3); // stack adjust for the 3 exception items
    }

    compile_decrease_except_level(comp);
    EMIT(end_finally);
    EMIT(end_except_handler);

    EMIT_ARG(label_assign, success_label);
    if (p_else != NULL) {
        compile_node(comp, p_else); // else block
    }
    EMIT_ARG(label_assign, l2);
}

STATIC void compile_try_finally(compiler_t *comp, const byte *p_body, const byte *p_except, const byte *p_except_top, const byte *p_else, const byte *p_finally) {
    assert(pt_is_rule(p_finally, PN_try_stmt_finally));

    uint l_finally_block = comp_next_label(comp);

    EMIT_ARG(setup_finally, l_finally_block);
    compile_increase_except_level(comp);

    if (p_except == NULL) {
        assert(p_else == NULL);
        EMIT_ARG(adjust_stack_size, 3); // stack adjust for possible UNWIND_JUMP state
        compile_node(comp, p_body);
        EMIT_ARG(adjust_stack_size, -3);
    } else {
        compile_try_except(comp, p_body, p_except, p_except_top, p_else);
    }
    EMIT(pop_block);
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    EMIT_ARG(label_assign, l_finally_block);
    compile_node(comp, pt_rule_first(p_finally));

    compile_decrease_except_level(comp);
    EMIT(end_finally);
}

STATIC void compile_try_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    const byte* p1 = pt_next(p);
    if (pt_is_rule(p1, PN_try_stmt_except) || pt_is_rule(p1, PN_try_stmt_except_list)) {
        // just try-except
        const byte *p1_top = mp_parse_node_extract_list(&p1, PN_try_stmt_except_list);
        compile_try_except(comp, p, p1, p1_top, NULL);
    } else if (pt_is_rule(p1, PN_try_stmt_except_and_more)) {
        // try-except and possibly else and/or finally
        const byte *p1_top;
        const byte *p1_p0 = pt_rule_extract_top(p1, &p1_top);
        const byte *p1_p1 = mp_parse_node_extract_list(&p1_p0, PN_try_stmt_except_list);
        if (pt_next(p1_p1) == p1_top) {
            // no finally, but have else
            compile_try_except(comp, p, p1_p0, p1_p1, p1_p1);
        } else {
            // have finally, may or may not have else
            compile_try_finally(comp, p, p1_p0, p1_p1, p1_p1, pt_next(p1_p1));
        }
    } else {
        // just try-finally
        compile_try_finally(comp, p, NULL, NULL, NULL, p1);
    }
}

STATIC void compile_with_stmt_helper(compiler_t *comp, const byte *n_pre, const byte *p_body) {
    if (n_pre >= p_body) {
        // no more pre-bits, compile the body of the with
        compile_node(comp, p_body);
    } else {
        uint l_end = comp_next_label(comp);
        if (MICROPY_EMIT_NATIVE && comp->scope_cur->emit_options != MP_EMIT_OPT_BYTECODE) {
            // we need to allocate an extra label for the native emitter
            // it will use l_end+1 as an auxiliary label
            comp_next_label(comp);
        }
        if (pt_is_rule(n_pre, PN_with_item)) {
            // this pre-bit is of the form "a as b"
            const byte *p = pt_rule_first(n_pre);
            p = compile_node(comp, p);
            EMIT_ARG(setup_with, l_end);
            c_assign(comp, p, ASSIGN_STORE);
            n_pre = pt_next(n_pre);
        } else {
            // this pre-bit is just an expression
            n_pre = compile_node(comp, n_pre);
            EMIT_ARG(setup_with, l_end);
            EMIT(pop_top);
        }
        compile_increase_except_level(comp);
        // compile additional pre-bits and the body
        compile_with_stmt_helper(comp, n_pre, p_body);
        // finish this with block
        EMIT_ARG(with_cleanup, l_end);
        compile_decrease_except_level(comp);
        EMIT(end_finally);
    }
}

STATIC void compile_with_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    // get the nodes for the pre-bit of the with (the a as b, c as d, ... bit)
    ptop = mp_parse_node_extract_list(&p, PN_with_stmt_list);

    // compile in a nested fashion
    compile_with_stmt_helper(comp, p, ptop);
}

STATIC void compile_expr_stmt(compiler_t *comp, const byte *p, const byte *ptop) {
    const byte *p_n1 = pt_next(p);

    if (pt_is_null_with_top(p_n1, ptop)) {
        if (comp->is_repl && comp->scope_cur->kind == SCOPE_MODULE) {
            // for REPL, evaluate then print the expression
            compile_load_id(comp, MP_QSTR___repl_print__);
            compile_node(comp, p);
            EMIT_ARG(call_function, 1, 0, 0);
            EMIT(pop_top);

        } else {
            #if 0
            // for non-REPL, evaluate then discard the expression
            if ((MP_PARSE_NODE_IS_LEAF(pns->nodes[0]) && !MP_PARSE_NODE_IS_ID(pns->nodes[0]))
                || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_string)
                || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_bytes)
                || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_const_object)) {
                // do nothing with a lonely constant
            } else
            #endif
            {
                compile_node(comp, p); // just an expression
                EMIT(pop_top); // discard last result since this is a statement and leaves nothing on the stack
            }
        }
    } else if (pt_is_rule(p_n1, PN_expr_stmt_augassign)) {
        c_assign(comp, p, ASSIGN_AUG_LOAD); // lhs load for aug assign
        p_n1 = pt_rule_first(p_n1);
        assert(pt_is_any_tok(p_n1));
        byte tok;
        p_n1 = pt_tok_extract(p_n1, &tok);
        mp_binary_op_t op;
        switch (tok) {
            case MP_TOKEN_DEL_PIPE_EQUAL: op = MP_BINARY_OP_INPLACE_OR; break;
            case MP_TOKEN_DEL_CARET_EQUAL: op = MP_BINARY_OP_INPLACE_XOR; break;
            case MP_TOKEN_DEL_AMPERSAND_EQUAL: op = MP_BINARY_OP_INPLACE_AND; break;
            case MP_TOKEN_DEL_DBL_LESS_EQUAL: op = MP_BINARY_OP_INPLACE_LSHIFT; break;
            case MP_TOKEN_DEL_DBL_MORE_EQUAL: op = MP_BINARY_OP_INPLACE_RSHIFT; break;
            case MP_TOKEN_DEL_PLUS_EQUAL: op = MP_BINARY_OP_INPLACE_ADD; break;
            case MP_TOKEN_DEL_MINUS_EQUAL: op = MP_BINARY_OP_INPLACE_SUBTRACT; break;
            case MP_TOKEN_DEL_STAR_EQUAL: op = MP_BINARY_OP_INPLACE_MULTIPLY; break;
            case MP_TOKEN_DEL_DBL_SLASH_EQUAL: op = MP_BINARY_OP_INPLACE_FLOOR_DIVIDE; break;
            case MP_TOKEN_DEL_SLASH_EQUAL: op = MP_BINARY_OP_INPLACE_TRUE_DIVIDE; break;
            case MP_TOKEN_DEL_PERCENT_EQUAL: op = MP_BINARY_OP_INPLACE_MODULO; break;
            case MP_TOKEN_DEL_DBL_STAR_EQUAL: default: op = MP_BINARY_OP_INPLACE_POWER; break;
        }
        compile_node(comp, p_n1); // rhs
        EMIT_ARG(binary_op, op);
        c_assign(comp, p, ASSIGN_AUG_STORE); // lhs store for aug assign
    } else if (pt_is_rule(p_n1, PN_expr_stmt_assign_list)) {
        const byte *p_n1_top;
        p_n1 = pt_rule_extract_top(p_n1, &p_n1_top);
        const byte *p_rhs = NULL;
        for (const byte *pp = p_n1; pp != p_n1_top; pp = pt_next(pp)) {
            p_rhs = pp;
        }
        compile_node(comp, p_rhs); // rhs
        // following CPython, we store left-most first
        //if (num rhs > 1) { always true?
            EMIT(dup_top);
        //}
        c_assign(comp, p, ASSIGN_STORE); // lhs store
        for (const byte *pp = p_n1; pp != p_rhs;) {
            const byte *pp_next = pt_next(pp);
            if (pp_next != p_rhs) {
                EMIT(dup_top);
            }
            c_assign(comp, pp, ASSIGN_STORE); // middle store
            pp = pp_next;
        }
    } else {
        // single assignment
        #if 0
        if (MICROPY_COMP_DOUBLE_TUPLE_ASSIGN
            && MP_PARSE_NODE_IS_STRUCT_KIND(pns1->nodes[0], PN_testlist_star_expr)
            && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_star_expr)
            && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns1->nodes[0]) == 2
            && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns->nodes[0]) == 2) {
            // optimisation for a, b = c, d
            mp_parse_node_struct_t *pns10 = (mp_parse_node_struct_t*)pns1->nodes[0];
            mp_parse_node_struct_t *pns0 = (mp_parse_node_struct_t*)pns->nodes[0];
            if (MP_PARSE_NODE_IS_STRUCT_KIND(pns0->nodes[0], PN_star_expr)
                || MP_PARSE_NODE_IS_STRUCT_KIND(pns0->nodes[1], PN_star_expr)) {
                // can't optimise when it's a star expression on the lhs
                goto no_optimisation;
            }
            compile_node(comp, pns10->nodes[0]); // rhs
            compile_node(comp, pns10->nodes[1]); // rhs
            EMIT(rot_two);
            c_assign(comp, pns0->nodes[0], ASSIGN_STORE); // lhs store
            c_assign(comp, pns0->nodes[1], ASSIGN_STORE); // lhs store
        } else if (MICROPY_COMP_TRIPLE_TUPLE_ASSIGN
            && MP_PARSE_NODE_IS_STRUCT_KIND(pns1->nodes[0], PN_testlist_star_expr)
            && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_star_expr)
            && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns1->nodes[0]) == 3
            && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns->nodes[0]) == 3) {
            // optimisation for a, b, c = d, e, f
            mp_parse_node_struct_t *pns10 = (mp_parse_node_struct_t*)pns1->nodes[0];
            mp_parse_node_struct_t *pns0 = (mp_parse_node_struct_t*)pns->nodes[0];
            if (MP_PARSE_NODE_IS_STRUCT_KIND(pns0->nodes[0], PN_star_expr)
                || MP_PARSE_NODE_IS_STRUCT_KIND(pns0->nodes[1], PN_star_expr)
                || MP_PARSE_NODE_IS_STRUCT_KIND(pns0->nodes[2], PN_star_expr)) {
                // can't optimise when it's a star expression on the lhs
                goto no_optimisation;
            }
            compile_node(comp, pns10->nodes[0]); // rhs
            compile_node(comp, pns10->nodes[1]); // rhs
            compile_node(comp, pns10->nodes[2]); // rhs
            EMIT(rot_three);
            EMIT(rot_two);
            c_assign(comp, pns0->nodes[0], ASSIGN_STORE); // lhs store
            c_assign(comp, pns0->nodes[1], ASSIGN_STORE); // lhs store
            c_assign(comp, pns0->nodes[2], ASSIGN_STORE); // lhs store
        } else
        #endif
        {
            //no_optimisation:
            compile_node(comp, p_n1); // rhs
            c_assign(comp, p, ASSIGN_STORE); // lhs store
        }
    }
}

STATIC void compile_test_if_expr(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    const byte *p_test_if_else = pt_next(p);
    assert(p_test_if_else != ptop && pt_is_rule(p_test_if_else, PN_test_if_else));
    p_test_if_else = pt_rule_first(p_test_if_else);

    uint l_fail = comp_next_label(comp);
    uint l_end = comp_next_label(comp);
    p_test_if_else = c_if_cond(comp, p_test_if_else, false, l_fail); // condition
    compile_node(comp, p); // success value
    EMIT_ARG(jump, l_end);
    EMIT_ARG(label_assign, l_fail);
    EMIT_ARG(adjust_stack_size, -1); // adjust stack size
    compile_node(comp, p_test_if_else); // failure value
    EMIT_ARG(label_assign, l_end);
}

STATIC void compile_lambdef(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    mp_int_t scope_idx;
    p = pt_get_small_int(p, &scope_idx);

    if (comp->pass == MP_PASS_SCOPE) {
        // create a new scope for this lambda
        scope_new_and_link(comp, scope_idx, SCOPE_LAMBDA, p, comp->scope_cur->emit_options);
    }

    // get the scope for this lambda
    scope_t *this_scope = comp->scopes[scope_idx];

    // compile the lambda definition
    compile_funcdef_lambdef(comp, this_scope, p, PN_varargslist);
}

STATIC void compile_or_and_test(compiler_t *comp, const byte *p, const byte *ptop, bool cond) {
    uint l_end = comp_next_label(comp);
    while (p != ptop) {
        p = compile_node(comp, p);
        if (p != ptop) {
            EMIT_ARG(jump_if_or_pop, cond, l_end);
        }
    }
    EMIT_ARG(label_assign, l_end);
}

STATIC void compile_or_test(compiler_t *comp, const byte *p, const byte *ptop) {
    compile_or_and_test(comp, p, ptop, true);
}

STATIC void compile_and_test(compiler_t *comp, const byte *p, const byte *ptop) {
    compile_or_and_test(comp, p, ptop, false);
}

STATIC void compile_not_test_2(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    compile_node(comp, p);
    EMIT_ARG(unary_op, MP_UNARY_OP_NOT);
}

STATIC void compile_comparison(compiler_t *comp, const byte *p, const byte *ptop) {
    int num_nodes = pt_num_nodes(p, ptop);
    p = compile_node(comp, p);
    bool multi = (num_nodes > 3);
    uint l_fail = 0;
    if (multi) {
        l_fail = comp_next_label(comp);
    }
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        mp_binary_op_t op;
        if (pt_is_any_tok(p)) {
            byte tok;
            p = pt_tok_extract(p, &tok);
            switch (tok) {
                case MP_TOKEN_OP_LESS: op = MP_BINARY_OP_LESS; break;
                case MP_TOKEN_OP_MORE: op = MP_BINARY_OP_MORE; break;
                case MP_TOKEN_OP_DBL_EQUAL: op = MP_BINARY_OP_EQUAL; break;
                case MP_TOKEN_OP_LESS_EQUAL: op = MP_BINARY_OP_LESS_EQUAL; break;
                case MP_TOKEN_OP_MORE_EQUAL: op = MP_BINARY_OP_MORE_EQUAL; break;
                case MP_TOKEN_OP_NOT_EQUAL: op = MP_BINARY_OP_NOT_EQUAL; break;
                case MP_TOKEN_KW_IN: default: op = MP_BINARY_OP_IN; break;
            }
        } else {
            if (pt_is_rule(p, PN_comp_op_not_in)) {
                op = MP_BINARY_OP_NOT_IN;
            } else {
                assert(pt_is_rule(p, PN_comp_op_is)); // should be
                if (pt_is_rule_empty(p)) {
                    op = MP_BINARY_OP_IS;
                } else {
                    op = MP_BINARY_OP_IS_NOT;
                }
            }
            p = pt_next(p);
        }

        p = compile_node(comp, p);

        if (i + 2 < num_nodes) {
            EMIT(dup_top);
            EMIT(rot_three);
        }

        EMIT_ARG(binary_op, op);

        if (i + 2 < num_nodes) {
            EMIT_ARG(jump_if_or_pop, false, l_fail);
        }
    }
    if (multi) {
        uint l_end = comp_next_label(comp);
        EMIT_ARG(jump, l_end);
        EMIT_ARG(label_assign, l_fail);
        EMIT_ARG(adjust_stack_size, 1);
        EMIT(rot_two);
        EMIT(pop_top);
        EMIT_ARG(label_assign, l_end);
    }
}

STATIC void compile_star_expr(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    compile_syntax_error(comp, p, "*x must be assignment target");
}

STATIC void c_binary_op(compiler_t *comp, const byte *p, const byte *ptop, mp_binary_op_t binary_op) {
    p = compile_node(comp, p);
    while (p != ptop) {
        p = compile_node(comp, p);
        EMIT_ARG(binary_op, binary_op);
    }
}

STATIC void compile_expr(compiler_t *comp, const byte *p, const byte *ptop) {
    c_binary_op(comp, p, ptop, MP_BINARY_OP_OR);
}

STATIC void compile_xor_expr(compiler_t *comp, const byte *p, const byte *ptop) {
    c_binary_op(comp, p, ptop, MP_BINARY_OP_XOR);
}

STATIC void compile_and_expr(compiler_t *comp, const byte *p, const byte *ptop) {
    c_binary_op(comp, p, ptop, MP_BINARY_OP_AND);
}

STATIC void compile_shift_expr(compiler_t *comp, const byte *p, const byte *ptop) {
    p = compile_node(comp, p);
    while (p != ptop) {
        byte tok;
        p = pt_tok_extract(p, &tok);
        p = compile_node(comp, p);
        if (tok == MP_TOKEN_OP_DBL_LESS) {
            EMIT_ARG(binary_op, MP_BINARY_OP_LSHIFT);
        } else {
            assert(tok == MP_TOKEN_OP_DBL_MORE); // should be
            EMIT_ARG(binary_op, MP_BINARY_OP_RSHIFT);
        }
    }
}

STATIC void compile_arith_expr(compiler_t *comp, const byte *p, const byte *ptop) {
    p = compile_node(comp, p);
    while (p != ptop) {
        byte tok;
        p = pt_tok_extract(p, &tok);
        p = compile_node(comp, p);
        if (tok == MP_TOKEN_OP_PLUS) {
            EMIT_ARG(binary_op, MP_BINARY_OP_ADD);
        } else {
            assert(tok == MP_TOKEN_OP_MINUS); // should be
            EMIT_ARG(binary_op, MP_BINARY_OP_SUBTRACT);
        }
    }
}

STATIC void compile_term(compiler_t *comp, const byte *p, const byte *ptop) {
    p = compile_node(comp, p);
    while (p != ptop) {
        byte tok;
        p = pt_tok_extract(p, &tok);
        p = compile_node(comp, p);
        if (tok == MP_TOKEN_OP_STAR) {
            EMIT_ARG(binary_op, MP_BINARY_OP_MULTIPLY);
        } else if (tok == MP_TOKEN_OP_DBL_SLASH) {
            EMIT_ARG(binary_op, MP_BINARY_OP_FLOOR_DIVIDE);
        } else if (tok == MP_TOKEN_OP_SLASH) {
            EMIT_ARG(binary_op, MP_BINARY_OP_TRUE_DIVIDE);
        } else {
            assert(tok == MP_TOKEN_OP_PERCENT); // should be
            EMIT_ARG(binary_op, MP_BINARY_OP_MODULO);
        }
    }
}

STATIC void compile_factor_2(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    byte tok;
    p = pt_tok_extract(p, &tok);
    compile_node(comp, p);
    if (tok == MP_TOKEN_OP_PLUS) {
        EMIT_ARG(unary_op, MP_UNARY_OP_POSITIVE);
    } else if (tok == MP_TOKEN_OP_MINUS) {
        EMIT_ARG(unary_op, MP_UNARY_OP_NEGATIVE);
    } else {
        assert(tok == MP_TOKEN_OP_TILDE); // should be
        EMIT_ARG(unary_op, MP_UNARY_OP_INVERT);
    }
}

STATIC void compile_atom_expr_normal(compiler_t *comp, const byte *p, const byte *ptop) {
    // this is to handle special super() call
    comp->func_arg_is_super = pt_is_id(p, MP_QSTR_super);

    compile_generic_all_nodes(comp, p, ptop);
}

STATIC void compile_power(compiler_t *comp, const byte *p, const byte *ptop) {
    compile_generic_all_nodes(comp, p, ptop); // 2 nodes, arguments of power
    EMIT_ARG(binary_op, MP_BINARY_OP_POWER);
}

// if p_arglist==NULL then there are no arguments
STATIC void compile_trailer_paren_helper(compiler_t *comp, const byte *p_arglist, bool is_method_call, int n_positional_extra) {
    // function to call is on top of stack

    // this is to handle special super() call
    if (p_arglist == NULL && comp->func_arg_is_super && comp->scope_cur->kind == SCOPE_FUNCTION) {
        compile_load_id(comp, MP_QSTR___class__);
        // look for first argument to function (assumes it's "self")
        for (int i = 0; i < comp->scope_cur->id_info_len; i++) {
            if (comp->scope_cur->id_info[i].flags & ID_FLAG_IS_PARAM) {
                // first argument found; load it and call super
                EMIT_LOAD_FAST(MP_QSTR_, comp->scope_cur->id_info[i].local_num);
                EMIT_ARG(call_function, 2, 0, 0);
                return;
            }
        }
        compile_syntax_error(comp, NULL, "super() call cannot find self"); // really a TypeError
        return;
    }

    // get the list of arguments
    const byte *ptop;
    if (p_arglist == NULL) {
        ptop = NULL;
    } else {
        ptop = mp_parse_node_extract_list(&p_arglist, PN_arglist);
    }

    // compile the arguments
    // Rather than calling compile_node on the list, we go through the list of args
    // explicitly here so that we can count the number of arguments and give sensible
    // error messages.
    int n_positional = n_positional_extra;
    uint n_keyword = 0;
    uint star_flags = 0;
    const byte *p_star_args = NULL, *p_dblstar_args = NULL;
    for (const byte *p = p_arglist; p != ptop;) {
        if (pt_is_rule(p, PN_arglist_star)) {
            if (star_flags & MP_EMIT_STAR_FLAG_SINGLE) {
                compile_syntax_error(comp, p, "can't have multiple *x");
                return;
            }
            star_flags |= MP_EMIT_STAR_FLAG_SINGLE;
            p_star_args = pt_rule_first(p);
            p = pt_next(p);
        } else if (pt_is_rule(p, PN_arglist_dbl_star)) {
            if (star_flags & MP_EMIT_STAR_FLAG_DOUBLE) {
                compile_syntax_error(comp, p, "can't have multiple **x");
                return;
            }
            star_flags |= MP_EMIT_STAR_FLAG_DOUBLE;
            p_dblstar_args = pt_rule_first(p);
            p = pt_next(p);
        } else if (pt_is_rule(p, PN_argument)) {
            p = pt_rule_first(p); // skip rule header
            const byte *p2 = pt_next(p); // get second node
            if (pt_is_rule(p2, PN_comp_for)) {
                // list comprehension argument
                compile_comprehension(comp, p, SCOPE_GEN_EXPR);
                n_positional++;
                p = pt_next(pt_next(p));
            } else {
                // keyword argument
                if (!pt_is_any_id(p)) {
                    compile_syntax_error(comp, p, "LHS of keyword arg must be an id");
                    return;
                }
                qstr kw;
                p = pt_extract_id(p, &kw);
                EMIT_ARG(load_const_str, kw);
                p = compile_node(comp, p);
                n_keyword += 1;
            }
        } else {
            if (n_keyword > 0) {
                compile_syntax_error(comp, p, "non-keyword arg after keyword arg");
                return;
            }
            p = compile_node(comp, p);
            n_positional++;
        }
    }

    // compile the star/double-star arguments if we had them
    // if we had one but not the other then we load "null" as a place holder
    if (star_flags != 0) {
        if (p_star_args == NULL) {
            EMIT(load_null);
        } else {
            compile_node(comp, p_star_args);
        }
        if (p_dblstar_args == NULL) {
            EMIT(load_null);
        } else {
            compile_node(comp, p_dblstar_args);
        }
    }

    // emit the function/method call
    if (is_method_call) {
        EMIT_ARG(call_method, n_positional, n_keyword, star_flags);
    } else {
        EMIT_ARG(call_function, n_positional, n_keyword, star_flags);
    }
}

STATIC void compile_atom_expr_trailers(compiler_t *comp, const byte *p, const byte *ptop) {
    while (p != ptop) {
        const byte *p_next = pt_next(p);
        if (p_next != ptop && pt_is_rule(p, PN_trailer_period) && pt_is_rule(p_next, PN_trailer_paren)) {
            // optimisation for method calls a.f(...), following PyPy
            const byte *p_period = pt_rule_first(p);
            const byte *p_paren;
            if (pt_is_rule_empty(p_next)) {
                p_paren = NULL;
            } else {
                p_paren = pt_rule_first(p_next);
            }
            qstr method_name;
            pt_extract_id(p_period, &method_name);
            EMIT_ARG(load_method, method_name);
            compile_trailer_paren_helper(comp, p_paren, true, 0);
            p = pt_next(p_next);
        } else {
            p = compile_node(comp, p);
        }
        comp->func_arg_is_super = false;
    }
}

// p needs to point to 2 successive nodes, first is lhs of comprehension, second is PN_comp_for node
STATIC void compile_comprehension(compiler_t *comp, const byte *p, scope_kind_t kind) {
    const byte *p_comp_for = pt_next(p);
    assert(pt_is_rule(p_comp_for, PN_comp_for));
    p_comp_for = pt_rule_first(p_comp_for);

    mp_int_t scope_idx;
    p_comp_for = pt_get_small_int(p_comp_for, &scope_idx);

    if (comp->pass == MP_PASS_SCOPE) {
        // create a new scope for this comprehension
        scope_new_and_link(comp, scope_idx, kind, p, comp->scope_cur->emit_options);
    }

    // get the scope for this comprehension
    scope_t *this_scope = comp->scopes[scope_idx];

    // compile the comprehension
    close_over_variables_etc(comp, this_scope, 0, 0);

    compile_node(comp, pt_next(p_comp_for)); // source of the iterator
    EMIT(get_iter);
    EMIT_ARG(call_function, 1, 0, 0);
}

STATIC void compile_atom_paren(compiler_t *comp, const byte *p, const byte *ptop) {
    if (pt_is_null_with_top(p, ptop)) {
        // an empty tuple
        c_tuple(comp, NULL, NULL, NULL);
    } else {
        assert(pt_is_rule(p, PN_testlist_comp));
        p = pt_rule_first(p);
        const byte *p1 = pt_next(p);
        if (pt_is_rule(p1, PN_testlist_comp_3b) || pt_is_rule(p1, PN_testlist_comp_3c)) {
            // tuple of one item with trailing comma (3b); or tuple of many items (3c)
            c_tuple(comp, p, pt_rule_first(p1), ptop);
        } else if (pt_is_rule(p1, PN_comp_for)) {
            // generator expression
            compile_comprehension(comp, p, SCOPE_GEN_EXPR);
        } else {
            // tuple with 2 items
            c_tuple(comp, NULL, p, ptop);
        }
    }
}

STATIC void compile_atom_bracket(compiler_t *comp, const byte *p, const byte *ptop) {
    if (pt_is_null_with_top(p, ptop)) {
        // empty list
        EMIT_ARG(build_list, 0);
    } else if (pt_is_rule(p, PN_testlist_comp)) {
        p = pt_rule_first(p);
        const byte *p3 = pt_next(p);
        if (pt_is_rule(p3, PN_testlist_comp_3b) || pt_is_rule(p3, PN_testlist_comp_3c)) {
            // list of one item with trailing comma (3b); or list of many items (3c)
            p3 = pt_rule_first(p3);
            compile_node(comp, p);
            compile_generic_all_nodes(comp, p3, ptop);
            EMIT_ARG(build_list, 1 + pt_num_nodes(p3, ptop));
        } else if (pt_is_rule(p3, PN_comp_for)) {
            // list comprehension
            compile_comprehension(comp, p, SCOPE_LIST_COMP);
        } else {
            // list with 2 items
            p = compile_node(comp, p);
            compile_node(comp, p);
            EMIT_ARG(build_list, 2);
        }
    } else {
        // list with 1 item
        compile_node(comp, p);
        EMIT_ARG(build_list, 1);
    }
}

STATIC void compile_atom_brace(compiler_t *comp, const byte *p, const byte *ptop) {
    if (pt_is_null_with_top(p, ptop)) {
        // empty dict
        EMIT_ARG(build_map, 0);
    } else if (pt_is_rule(p, PN_dictorsetmaker_item)) {
        // dict with one element
        EMIT_ARG(build_map, 1);
        compile_node(comp, p);
        EMIT(store_map);
    } else if (pt_is_rule(p, PN_dictorsetmaker)) {
        p = pt_rule_first(p);
        const byte *p1 = pt_next(p);
        if (pt_is_rule(p1, PN_dictorsetmaker_list)) {
            // dict/set with multiple elements
            const byte *p1_top;
            p1 = pt_rule_extract_top(p1, &p1_top);

            // get tail elements (2nd, 3rd, ...)
            if (p1 != p1_top) {
                mp_parse_node_extract_list(&p1, PN_dictorsetmaker_list2);
            }

            // first element sets whether it's a dict or set
            bool is_dict;
            if (!MICROPY_PY_BUILTINS_SET || pt_is_rule(p, PN_dictorsetmaker_item)) {
                // a dictionary
                EMIT_ARG(build_map, 1 + pt_num_nodes(p1, p1_top));
                compile_node(comp, p);
                EMIT(store_map);
                is_dict = true;
            } else {
                // a set
                compile_node(comp, p); // 1st value of set
                is_dict = false;
            }

            // process rest of elements
            for (const byte *p_elem = p1; p_elem != p1_top;) {
                bool is_key_value = pt_is_rule(p_elem, PN_dictorsetmaker_item);
                p_elem = compile_node(comp, p_elem);
                if (is_dict) {
                    if (!is_key_value) {
                        // TODO what is the correct p for error node?
                        compile_syntax_error(comp, p, "expecting key:value for dictionary");
                        return;
                    }
                    EMIT(store_map);
                } else {
                    if (is_key_value) {
                        // TODO what is the correct p for error node?
                        compile_syntax_error(comp, p, "expecting just a value for set");
                        return;
                    }
                }
            }

            #if MICROPY_PY_BUILTINS_SET
            // if it's a set, build it
            if (!is_dict) {
                EMIT_ARG(build_set, 1 + pt_num_nodes(p1, p1_top));
            }
            #endif
        } else {
            assert(pt_is_rule(p1, PN_comp_for)); // should be
            // dict/set comprehension
            if (!MICROPY_PY_BUILTINS_SET || pt_is_rule(p, PN_dictorsetmaker_item)) {
                // a dictionary comprehension
                compile_comprehension(comp, p, SCOPE_DICT_COMP);
            } else {
                // a set comprehension
                compile_comprehension(comp, p, SCOPE_SET_COMP);
            }
        }
    } else {
        // set with one element
        #if MICROPY_PY_BUILTINS_SET
        compile_node(comp, p);
        EMIT_ARG(build_set, 1);
        #else
        assert(0);
        #endif
    }
}

STATIC void compile_trailer_paren(compiler_t *comp, const byte *p, const byte *ptop) {
    if (p >= ptop) {
        p = NULL;
    }
    compile_trailer_paren_helper(comp, p, false, 0);
}

STATIC void compile_trailer_bracket(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    // object who's index we want is on top of stack
    compile_node(comp, p); // the index
    EMIT(load_subscr);
}

STATIC void compile_trailer_period(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    // object who's attribute we want is on top of stack
    qstr attr;
    p = pt_extract_id(p, &attr);
    EMIT_ARG(load_attr, attr);
}

#if MICROPY_PY_BUILTINS_SLICE
// p,ptop should be the args of subscript_3
STATIC void compile_subscript_3_helper(compiler_t *comp, const byte *p, const byte *ptop) {
    if (p == ptop) {
        // [?:]
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT_ARG(build_slice, 2);
    } else if (pt_is_rule(p, PN_subscript_3c)) {
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        if (pt_is_rule_empty(p)) {
            // [?::]
            EMIT_ARG(build_slice, 2);
        } else {
            // [?::x]
            compile_node(comp, pt_rule_first(p));
            EMIT_ARG(build_slice, 3);
        }
    } else if (pt_is_rule(p, PN_subscript_3d)) {
        p = pt_rule_first(p);
        p = compile_node(comp, p);
        assert(pt_is_rule(p, PN_sliceop)); // should always be
        p = pt_rule_first(p);
        if (p == ptop) {
            // [?:x:]
            EMIT_ARG(build_slice, 2);
        } else {
            // [?:x:x]
            compile_node(comp, p);
            EMIT_ARG(build_slice, 3);
        }
    } else {
        // [?:x]
        compile_node(comp, p);
        EMIT_ARG(build_slice, 2);
    }
}

STATIC void compile_subscript_2(compiler_t *comp, const byte *p, const byte *ptop) {
    p = compile_node(comp, p); // start of slice
    p = pt_rule_first(p); // skip header of subscript_3
    compile_subscript_3_helper(comp, p, ptop);
}

STATIC void compile_subscript_3(compiler_t *comp, const byte *p, const byte *ptop) {
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    compile_subscript_3_helper(comp, p, ptop);
}
#endif // MICROPY_PY_BUILTINS_SLICE

STATIC void compile_dictorsetmaker_item(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    // if this is called then we are compiling a dict key:value pair
    compile_node(comp, pt_next(p)); // value
    compile_node(comp, p); // key
}

STATIC void compile_classdef(compiler_t *comp, const byte *p, const byte *ptop) {
    (void)ptop;
    qstr cname = compile_classdef_helper(comp, p, comp->scope_cur->emit_options);
    // store class object into class name
    compile_store_id(comp, cname);
}

STATIC void compile_yield_expr(compiler_t *comp, const byte *p, const byte *ptop) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION && comp->scope_cur->kind != SCOPE_LAMBDA) {
        compile_syntax_error(comp, NULL, "'yield' outside function");
        return;
    }
    if (pt_is_null_with_top(p, ptop)) {
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT(yield_value);
    } else if (pt_is_rule(p, PN_yield_arg_from)) {
        p = pt_rule_first(p);
        compile_node(comp, p);
        EMIT(get_iter);
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT(yield_from);
    } else {
        compile_node(comp, p);
        EMIT(yield_value);
    }
}

typedef void (*compile_function_t)(compiler_t*, const byte*, const byte*);
STATIC compile_function_t compile_function[] = {
#define nc NULL
#define c(f) compile_##f
#define DEF_RULE(rule, comp, kind, ...) comp,
#include "py/grammar.h"
#undef nc
#undef c
#undef DEF_RULE
    NULL,
};

STATIC const byte *compile_node(compiler_t *comp, const byte *p) {
    //printf("CN %p %02x %02x %02x\n", p, p[0], p[1], p[2]);
    if (pt_is_null(p)) {
        // pass
        return p + 1;
    } else if (pt_is_small_int(p)) {
        mp_int_t arg;
        p = pt_get_small_int(p, &arg);
        #if MICROPY_DYNAMIC_COMPILER
        mp_uint_t sign_mask = -(1 << (mp_dynamic_compiler.small_int_bits - 1));
        if ((arg & sign_mask) == 0 || (arg & sign_mask) == sign_mask) {
            // integer fits in target runtime's small-int
            EMIT_ARG(load_const_small_int, arg);
        } else {
            // integer doesn't fit, so create a multi-precision int object
            // (but only create the actual object on the last pass)
            if (comp->pass != MP_PASS_EMIT) {
                EMIT_ARG(load_const_obj, mp_const_none);
            } else {
                EMIT_ARG(load_const_obj, mp_obj_new_int_from_ll(arg));
            }
        }
        #else
        EMIT_ARG(load_const_small_int, arg);
        #endif
        return p;
    } else if (pt_is_any_tok(p)) {
        byte tok;
        p = pt_tok_extract(p, &tok);
        if (tok == MP_TOKEN_NEWLINE) {
            // this can occur when file_input lets through a NEWLINE (eg if file starts with a newline)
            // or when single_input lets through a NEWLINE (user enters a blank line)
            // do nothing
        } else {
            EMIT_ARG(load_const_tok, tok);
        }
        return p;
    } else if (*p == MP_PT_STRING) {
        qstr qst = p[1] | (p[2] << 8);
        EMIT_ARG(load_const_str, qst);
        return pt_next(p);
    } else if (*p == MP_PT_BYTES) {
        // only create and load the actual bytes object on the last pass
        if (comp->pass != MP_PASS_EMIT) {
            EMIT_ARG(load_const_obj, mp_const_none);
        } else {
            qstr qst = p[1] | (p[2] << 8);
            size_t len;
            const byte *data = qstr_data(qst, &len);
            EMIT_ARG(load_const_obj, mp_obj_new_bytes(data, len));
        }
        return pt_next(p);
    } else if (pt_is_any_id(p)) {
        qstr qst;
        p = pt_extract_id(p, &qst);
        compile_load_id(comp, qst);
        return p;
    } else if (*p == MP_PT_CONST_OBJECT) {
        size_t idx;
        p = pt_extract_const_obj(p, &idx);
        EMIT_ARG(load_const_obj, (mp_obj_t)comp->co_data[idx]);
        return p;
    } else {
        assert(*p >= MP_PT_RULE_BASE);
        size_t rule_id, src_line;
        const byte *ptop;
        p = pt_rule_extract(p, &rule_id, &src_line, &ptop);
        EMIT_ARG(set_source_line, src_line);
        compile_function_t f = compile_function[rule_id];
        if (f == NULL) {
            #if MICROPY_DEBUG_PRINTERS
            printf("node %u cannot be compiled\n", (uint)rule_id);
            //mp_parse_node_print(pn, 0);
            #endif
            compile_syntax_error(comp, p, "internal compiler error");
            assert(0);
            return ptop;
        } else {
            f(comp, p, ptop);
            if (comp->compile_error != MP_OBJ_NULL && comp->compile_error_line == 0) {
                // add line info for the error in case it didn't have a line number
                comp->compile_error_line = src_line;
            }
            return ptop;
        }
    }
}

STATIC void compile_scope_func_lambda_param(compiler_t *comp, const byte *p, pn_kind_t pn_name, pn_kind_t pn_star, pn_kind_t pn_dbl_star) {
    (void)pn_dbl_star;
    // check that **kw is last
    if ((comp->scope_cur->scope_flags & MP_SCOPE_FLAG_VARKEYWORDS) != 0) {
        compile_syntax_error(comp, p, "invalid syntax");
        return;
    }

    qstr param_name = MP_QSTR_NULL;
    uint param_flag = ID_FLAG_IS_PARAM;
    if (pt_is_any_id(p)) {
        pt_extract_id(p, &param_name);
        if (comp->have_star) {
            // comes after a star, so counts as a keyword-only parameter
            comp->scope_cur->num_kwonly_args += 1;
        } else {
            // comes before a star, so counts as a positional parameter
            comp->scope_cur->num_pos_args += 1;
        }
    } else {
        if (pt_is_rule(p, pn_name)) {
            pt_extract_id(pt_rule_first(p), &param_name);
            if (comp->have_star) {
                // comes after a star, so counts as a keyword-only parameter
                comp->scope_cur->num_kwonly_args += 1;
            } else {
                // comes before a star, so counts as a positional parameter
                comp->scope_cur->num_pos_args += 1;
            }
        } else if (pt_is_rule(p, pn_star)) {
            if (comp->have_star) {
                // more than one star
                compile_syntax_error(comp, p, "invalid syntax");
                return;
            }
            comp->have_star = true;
            param_flag = ID_FLAG_IS_PARAM | ID_FLAG_IS_STAR_PARAM;
            if (pt_is_rule_empty(p)) {
                // bare star
                // TODO see http://www.python.org/dev/peps/pep-3102/
                //assert(comp->scope_cur->num_dict_params == 0);
            } else if (pt_is_any_id(pt_rule_first(p))) {
                // named star
                comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARARGS;
                pt_extract_id(pt_rule_first(p), &param_name);
            } else {
                assert(pt_is_rule(pt_rule_first(p), PN_tfpdef)); // should be
                // named star with possible annotation
                comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARARGS;
                pt_extract_id(pt_rule_first(pt_rule_first(p)), &param_name);
            }
        } else {
            assert(pt_is_rule(p, pn_dbl_star)); // should be
            pt_extract_id(pt_rule_first(p), &param_name);
            param_flag = ID_FLAG_IS_PARAM | ID_FLAG_IS_DBL_STAR_PARAM;
            comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARKEYWORDS;
        }
    }

    if (param_name != MP_QSTR_NULL) {
        bool added;
        id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, param_name, &added);
        if (!added) {
            compile_syntax_error(comp, p, "name reused for argument");
            return;
        }
        id_info->kind = ID_INFO_KIND_LOCAL;
        id_info->flags = param_flag;
    }
}

STATIC void compile_scope_func_param(compiler_t *comp, const byte *p) {
    compile_scope_func_lambda_param(comp, p, PN_typedargslist_name, PN_typedargslist_star, PN_typedargslist_dbl_star);
}

STATIC void compile_scope_lambda_param(compiler_t *comp, const byte *p) {
    compile_scope_func_lambda_param(comp, p, PN_varargslist_name, PN_varargslist_star, PN_varargslist_dbl_star);
}

#if MICROPY_EMIT_NATIVE
STATIC void compile_scope_func_annotations(compiler_t *comp, const byte *p) {
    if (pt_is_rule(p, PN_typedargslist_name)) {
        // named parameter with possible annotation
        // fallthrough
    } else if (pt_is_rule(p, PN_typedargslist_star)) {
        const byte *p0 = pt_rule_first(p);
        if (pt_is_rule(p0, PN_tfpdef)) {
            // named star with possible annotation
            p = p0;
            // fallthrough
        } else {
            // no annotation
            return;
        }
    } else if (pt_is_rule(p, PN_typedargslist_dbl_star)) {
        // double star with possible annotation
        // fallthrough
    } else {
        // no annotation
        return;
    }

    // p should be a rule whose first node is an identifier and second may be the annotation

    const byte *ptop;
    p = pt_rule_extract_top(p, &ptop);

    qstr param_name;
    p = pt_extract_id(p, &param_name);

    if (!pt_is_null_with_top(p, ptop)) {
        id_info_t *id_info = scope_find(comp->scope_cur, param_name);
        assert(id_info != NULL);

        if (pt_is_any_id(p)) {
            qstr arg_type;
            pt_extract_id(p, &arg_type);
            EMIT_ARG(set_native_type, MP_EMIT_NATIVE_TYPE_ARG, id_info->local_num, arg_type);
        } else {
            compile_syntax_error(comp, p, "parameter annotation must be an identifier");
        }
    }
}
#endif // MICROPY_EMIT_NATIVE

STATIC void compile_scope_comp_iter(compiler_t *comp, const byte *p_comp_for, const byte *p_comp_for_top, const byte *p_inner_expr, int for_depth) {
    uint l_top = comp_next_label(comp);
    uint l_end = comp_next_label(comp);
    EMIT_ARG(label_assign, l_top);
    EMIT_ARG(for_iter, l_end);
    c_assign(comp, p_comp_for, ASSIGN_STORE);
    const byte *p_iter = pt_next(pt_next(p_comp_for));

    tail_recursion:
    if (p_iter == p_comp_for_top) {
        // no more nested if/for; compile inner expression
        compile_node(comp, p_inner_expr);
        if (comp->scope_cur->kind == SCOPE_LIST_COMP) {
            EMIT_ARG(list_append, for_depth + 2);
        } else if (comp->scope_cur->kind == SCOPE_DICT_COMP) {
            EMIT_ARG(map_add, for_depth + 2);
        #if MICROPY_PY_BUILTINS_SET
        } else if (comp->scope_cur->kind == SCOPE_SET_COMP) {
            EMIT_ARG(set_add, for_depth + 2);
        #endif
        } else {
            EMIT(yield_value);
            EMIT(pop_top);
        }
    } else if (pt_is_rule(p_iter, PN_comp_if)) {
        // if condition
        const byte *p0 = pt_rule_extract_top(p_iter, &p_comp_for_top);
        p_iter = c_if_cond(comp, p0, false, l_top);
        goto tail_recursion;
    } else {
        assert(pt_is_rule(p_iter, PN_comp_for)); // should be
        // for loop
        const byte *ptop;
        const byte *p0 = pt_rule_extract_top(p_iter, &ptop);
        p0 = pt_next(p0); // skip scope index
        compile_node(comp, pt_next(p0));
        EMIT(get_iter);
        compile_scope_comp_iter(comp, p0, ptop, p_inner_expr, for_depth + 1);
    }

    EMIT_ARG(jump, l_top);
    EMIT_ARG(label_assign, l_end);
    EMIT(for_iter_end);
}

#if 0
STATIC void check_for_doc_string(compiler_t *comp, mp_parse_node_t pn) {
#if MICROPY_ENABLE_DOC_STRING
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
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if ((MP_PARSE_NODE_IS_LEAF(pns->nodes[0])
                && MP_PARSE_NODE_LEAF_KIND(pns->nodes[0]) == MP_PARSE_NODE_STRING)
            || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_string)) {
                // compile the doc string
                compile_node(comp, pns->nodes[0]);
                // store the doc string
                compile_store_id(comp, MP_QSTR___doc__);
        }
    }
#else
    (void)comp;
    (void)pn;
#endif
}
#endif

STATIC void compile_scope(compiler_t *comp, scope_t *scope, pass_kind_t pass) {
    comp->pass = pass;
    comp->scope_cur = scope;
    comp->next_label = 1;
    EMIT_ARG(start_pass, pass, scope);

    if (comp->pass == MP_PASS_SCOPE) {
        // reset maximum stack sizes in scope
        // they will be computed in this first pass
        scope->stack_size = 0;
        scope->exc_stack_size = 0;
    }

    // compile
    if (pt_is_rule(scope->pn, PN_eval_input)) {
        assert(scope->kind == SCOPE_MODULE);
        compile_node(comp, pt_rule_first(scope->pn)); // compile the expression
        EMIT(return_value);
    } else if (scope->kind == SCOPE_MODULE) {
        if (!comp->is_repl) {
            //check_for_doc_string(comp, scope->pn);
        }
        compile_node(comp, scope->pn);
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT(return_value);
    } else if (scope->kind == SCOPE_FUNCTION) {
        const byte *p = scope->pn;

        p = pt_next(p); // skip func name

        // work out number of parameters, keywords and default parameters, and add them to the id_info array
        // must be done before compiling the body so that arguments are numbered first (for LOAD_FAST etc)
        if (comp->pass == MP_PASS_SCOPE) {
            comp->have_star = false;
            apply_to_single_or_list(comp, p, PN_typedargslist, compile_scope_func_param);
        }

        #if MICROPY_EMIT_NATIVE
        else if (scope->emit_options == MP_EMIT_OPT_VIPER) {
            // compile annotations; only needed on latter compiler passes
            // only needed for viper emitter

            // argument annotations
            apply_to_single_or_list(comp, p, PN_typedargslist, compile_scope_func_annotations);

            const byte *p_ret = pt_next(p); // skip arg list

            // next node is return/whole function annotation
            if (pt_is_any_id(p_ret)) {
                qstr ret_type;
                pt_extract_id(p_ret, &ret_type);
                EMIT_ARG(set_native_type, MP_EMIT_NATIVE_TYPE_RETURN, 0, ret_type);
            } else if (!pt_is_null(p_ret)) {
                compile_syntax_error(comp, p_ret, "return annotation must be an identifier");
            }
        }
        #endif // MICROPY_EMIT_NATIVE

        p = pt_next(p); // skip arg list
        p = pt_next(p); // skip return annotation

        compile_node(comp, p); // function body

        // emit return if it wasn't the last opcode
        if (!EMIT(last_emit_was_return_value)) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
            EMIT(return_value);
        }
    } else if (scope->kind == SCOPE_LAMBDA) {
        const byte *p = scope->pn;

        // work out number of parameters, keywords and default parameters, and add them to the id_info array
        // must be done before compiling the body so that arguments are numbered first (for LOAD_FAST etc)
        if (comp->pass == MP_PASS_SCOPE) {
            comp->have_star = false;
            apply_to_single_or_list(comp, p, PN_varargslist, compile_scope_lambda_param);
        }

        p = pt_next(p); // skip arg list

        compile_node(comp, p); // lambda body

        // if the lambda is a generator, then we return None, not the result of the expression of the lambda
        if (scope->scope_flags & MP_SCOPE_FLAG_GENERATOR) {
            EMIT(pop_top);
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        }
        EMIT(return_value);
    } else if (scope->kind == SCOPE_LIST_COMP || scope->kind == SCOPE_DICT_COMP || scope->kind == SCOPE_SET_COMP || scope->kind == SCOPE_GEN_EXPR) {
        const byte *p = scope->pn;
        const byte *p_comp_for = pt_next(p);
        const byte *p_comp_for_top;
        p_comp_for = pt_rule_extract_top(p_comp_for, &p_comp_for_top);
        p_comp_for = pt_next(p_comp_for); // skip scope index

        // We need a unique name for the comprehension argument (the iterator).
        // CPython uses .0, but we should be able to use anything that won't
        // clash with a user defined variable.  Best to use an existing qstr,
        // so we use the blank qstr.
        qstr qstr_arg = MP_QSTR_;
        if (comp->pass == MP_PASS_SCOPE) {
            bool added;
            id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, qstr_arg, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
            scope->num_pos_args = 1;
        }

        if (scope->kind == SCOPE_LIST_COMP) {
            EMIT_ARG(build_list, 0);
        } else if (scope->kind == SCOPE_DICT_COMP) {
            EMIT_ARG(build_map, 0);
        #if MICROPY_PY_BUILTINS_SET
        } else if (scope->kind == SCOPE_SET_COMP) {
            EMIT_ARG(build_set, 0);
        #endif
        }

        compile_load_id(comp, qstr_arg);
        compile_scope_comp_iter(comp, p_comp_for, p_comp_for_top, p, 0);

        if (scope->kind == SCOPE_GEN_EXPR) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        }
        EMIT(return_value);
    } else {
        assert(scope->kind == SCOPE_CLASS);

        if (comp->pass == MP_PASS_SCOPE) {
            bool added;
            id_info_t *id_info = scope_find_or_add_id(scope, MP_QSTR___class__, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
        }

        // just to check, should remove this code
        qstr class_name;
        pt_extract_id(scope->pn, &class_name);
        assert(class_name == scope->simple_name);

        compile_load_id(comp, MP_QSTR___name__);
        compile_store_id(comp, MP_QSTR___module__);
        EMIT_ARG(load_const_str, scope->simple_name);
        compile_store_id(comp, MP_QSTR___qualname__);

        const byte *p = pt_next(pt_next(scope->pn)); // skip name, bases
        //check_for_doc_string(comp, p);
        compile_node(comp, p); // class body

        id_info_t *id = scope_find(scope, MP_QSTR___class__);
        assert(id != NULL);
        if (id->kind == ID_INFO_KIND_LOCAL) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        } else {
            EMIT_LOAD_FAST(MP_QSTR___class__, id->local_num);
        }
        EMIT(return_value);
    }

    EMIT(end_pass);

    // make sure we match all the exception levels
    assert(comp->cur_except_level == 0);
}

#if MICROPY_EMIT_INLINE_THUMB
// requires 3 passes: SCOPE, CODE_SIZE, EMIT
STATIC void compile_scope_inline_asm(compiler_t *comp, scope_t *scope, pass_kind_t pass) {
    comp->pass = pass;
    comp->scope_cur = scope;
    comp->next_label = 1;

    if (scope->kind != SCOPE_FUNCTION) {
        compile_syntax_error(comp, NULL, "inline assembler must be a function");
        return;
    }

    if (comp->pass > MP_PASS_SCOPE) {
        EMIT_INLINE_ASM_ARG(start_pass, comp->pass, comp->scope_cur, &comp->compile_error);
    }

    // get the function definition parse node
    const byte *p = scope->pn;
    assert(pt_is_any_id(p));
    p = pt_next(p); // skip the function name

    // parameters are in next node
    if (comp->pass == MP_PASS_CODE_SIZE) {
        const byte *pp = p;
        const byte *pptop = mp_parse_node_extract_list(&pp, PN_typedargslist);
        scope->num_pos_args = EMIT_INLINE_ASM_ARG(count_params, pp, pptop);
        if (comp->compile_error != MP_OBJ_NULL) {
            goto inline_asm_error;
        }
    }

    p = pt_next(p); // skip the parameter list

    // function return annotation is in the next node
    mp_uint_t type_sig = MP_NATIVE_TYPE_INT;
    if (!pt_is_null(p)) {
        if (pt_is_any_id(p)) {
            qstr ret_type;
            pt_extract_id(p, &ret_type);
            switch (ret_type) {
                case MP_QSTR_object: type_sig = MP_NATIVE_TYPE_OBJ; break;
                case MP_QSTR_bool: type_sig = MP_NATIVE_TYPE_BOOL; break;
                case MP_QSTR_int: type_sig = MP_NATIVE_TYPE_INT; break;
                case MP_QSTR_uint: type_sig = MP_NATIVE_TYPE_UINT; break;
                default: compile_syntax_error(comp, p, "unknown type"); return;
            }
        } else {
            compile_syntax_error(comp, p, "return annotation must be an identifier");
        }
    }
    p = pt_next(p); // move past function return annotation

    // get the list of statements within the body of the function
    const byte *ptop = mp_parse_node_extract_list(&p, PN_suite_block_stmts);

    for (const byte *p_instr = p; p_instr != ptop; p_instr = pt_next(p_instr)) {
        p = p_instr;
        if (pt_is_rule(p, PN_pass_stmt)) {
            // no instructions
            continue;
        } else if (!pt_is_rule(p, PN_expr_stmt)) {
            // not an instruction; error
        not_an_instruction:
            compile_syntax_error(comp, p, "expecting an assembler instruction");
            return;
        }

        // check structure of parse node
        const byte *p_expr_top;
        const byte *p_expr = pt_rule_extract_top(p, &p_expr_top);
        if (!pt_is_rule(p_expr, PN_atom_expr_normal)) {
            goto not_an_instruction;
        }
        if (pt_next(p_expr) != p_expr_top) {
            goto not_an_instruction;
        }
        p_expr = pt_rule_extract_top(p_expr, &p_expr_top);
        if (!pt_is_any_id(p_expr)) {
            goto not_an_instruction;
        }
        const byte *p_expr_paren = pt_next(p_expr);
        if (p_expr_paren == p_expr_top || !pt_is_rule(p_expr_paren, PN_trailer_paren)) {
            goto not_an_instruction;
        }
        if (pt_next(p_expr_paren) != p_expr_top) {
            goto not_an_instruction;
        }

        // parse node looks like an instruction
        // get instruction name and args
        qstr op;
        pt_extract_id(p_expr, &op);

        const byte *p_args = pt_rule_first(p_expr_paren);
        const byte *p_args_top = mp_parse_node_extract_list(&p_args, PN_arglist);
        uint n_args = pt_num_nodes(p_args, p_args_top);

        // emit instructions
        if (op == MP_QSTR_label) {
            if (!(n_args == 1 && pt_is_any_id(p_args))) {
                compile_syntax_error(comp, p, "'label' requires 1 argument");
                return;
            }
            uint lab = comp_next_label(comp);
            if (pass > MP_PASS_SCOPE) {
                qstr id;
                pt_extract_id(p_args, &id);
                if (!EMIT_INLINE_ASM_ARG(label, lab, id)) {
                    compile_syntax_error(comp, p, "label redefined");
                    return;
                }
            }
        } else if (op == MP_QSTR_align) {
            if (!(n_args == 1 && pt_is_small_int(p_args))) {
                compile_syntax_error(comp, p, "'align' requires 1 argument");
                return;
            }
            if (pass > MP_PASS_SCOPE) {
                EMIT_INLINE_ASM_ARG(align, pt_small_int_value(p_args));
            }
        } else if (op == MP_QSTR_data) {
            if (!(n_args >= 2 && pt_is_small_int(p_args))) {
                compile_syntax_error(comp, p, "'data' requires at least 2 arguments");
                return;
            }
            if (pass > MP_PASS_SCOPE) {
                mp_int_t bytesize;
                p_args = pt_get_small_int(p_args, &bytesize);
                for (uint j = 1; j < n_args; j++) {
                    if (!pt_is_small_int(p_args)) {
                        compile_syntax_error(comp, p, "'data' requires integer arguments");
                        return;
                    }
                    mp_int_t val;
                    p_args = pt_get_small_int(p_args, &val);
                    EMIT_INLINE_ASM_ARG(data, bytesize, val);
                }
            }
        } else {
            if (pass > MP_PASS_SCOPE) {
                if (n_args > 3) {
                    goto not_an_instruction;
                }
                const byte *pn_arg[3];
                pn_arg[0] = p_args;
                pn_arg[1] = pt_next(pn_arg[0]);
                pn_arg[2] = pt_next(pn_arg[1]);
                EMIT_INLINE_ASM_ARG(op, op, n_args, pn_arg);
            }
        }

        if (comp->compile_error != MP_OBJ_NULL) {
            goto inline_asm_error;
        }
    }

    if (comp->pass > MP_PASS_SCOPE) {
        EMIT_INLINE_ASM_ARG(end_pass, type_sig);
    }

    if (comp->compile_error != MP_OBJ_NULL) {
        // inline assembler had an error; set line for its exception
    inline_asm_error:
        compile_error_set_line(comp, p);
    }
}
#endif

STATIC void scope_compute_things(scope_t *scope) {
    // in Micro Python we put the *x parameter after all other parameters (except **y)
    if (scope->scope_flags & MP_SCOPE_FLAG_VARARGS) {
        id_info_t *id_param = NULL;
        for (int i = scope->id_info_len - 1; i >= 0; i--) {
            id_info_t *id = &scope->id_info[i];
            if (id->flags & ID_FLAG_IS_STAR_PARAM) {
                if (id_param != NULL) {
                    // swap star param with last param
                    id_info_t temp = *id_param; *id_param = *id; *id = temp;
                }
                break;
            } else if (id_param == NULL && id->flags == ID_FLAG_IS_PARAM) {
                id_param = id;
            }
        }
    }

    // in functions, turn implicit globals into explicit globals
    // compute the index of each local
    scope->num_locals = 0;
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
        if (scope->kind == SCOPE_CLASS && id->qst == MP_QSTR___class__) {
            // __class__ is not counted as a local; if it's used then it becomes a ID_INFO_KIND_CELL
            continue;
        }
        if (scope->kind >= SCOPE_FUNCTION && scope->kind <= SCOPE_GEN_EXPR && id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
            id->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
        }
        // params always count for 1 local, even if they are a cell
        if (id->kind == ID_INFO_KIND_LOCAL || (id->flags & ID_FLAG_IS_PARAM)) {
            id->local_num = scope->num_locals++;
        }
    }

    // compute the index of cell vars
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
        // in Micro Python the cells come right after the fast locals
        // parameters are not counted here, since they remain at the start
        // of the locals, even if they are cell vars
        if (id->kind == ID_INFO_KIND_CELL && !(id->flags & ID_FLAG_IS_PARAM)) {
            id->local_num = scope->num_locals;
            scope->num_locals += 1;
        }
    }

    // compute the index of free vars
    // make sure they are in the order of the parent scope
    if (scope->parent != NULL) {
        int num_free = 0;
        for (int i = 0; i < scope->parent->id_info_len; i++) {
            id_info_t *id = &scope->parent->id_info[i];
            if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
                for (int j = 0; j < scope->id_info_len; j++) {
                    id_info_t *id2 = &scope->id_info[j];
                    if (id2->kind == ID_INFO_KIND_FREE && id->qst == id2->qst) {
                        assert(!(id2->flags & ID_FLAG_IS_PARAM)); // free vars should not be params
                        // in Micro Python the frees come first, before the params
                        id2->local_num = num_free;
                        num_free += 1;
                    }
                }
            }
        }
        // in Micro Python shift all other locals after the free locals
        if (num_free > 0) {
            for (int i = 0; i < scope->id_info_len; i++) {
                id_info_t *id = &scope->id_info[i];
                if (id->kind != ID_INFO_KIND_FREE || (id->flags & ID_FLAG_IS_PARAM)) {
                    id->local_num += num_free;
                }
            }
            scope->num_pos_args += num_free; // free vars are counted as params for passing them into the function
            scope->num_locals += num_free;
        }
    }
}

#if !MICROPY_PERSISTENT_CODE_SAVE
STATIC
#endif
mp_raw_code_t *mp_compile_to_raw_code(mp_parse_tree_t *parse_tree, qstr source_file, uint emit_opt, bool is_repl) {
    // put compiler state on the stack, it's relatively small
    compiler_t comp_state = {0};
    compiler_t *comp = &comp_state;

    comp->source_file = source_file;
    comp->is_repl = is_repl;
    comp->co_data = parse_tree->co_data;

    // create the array of scopes
    comp->num_scopes = pt_small_int_value(pt_next(parse_tree->root));
    comp->scopes = m_new0(scope_t*, comp->num_scopes);

    // create the module scope
    scope_new_and_link(comp, 0, SCOPE_MODULE, parse_tree->root, emit_opt);

    // create standard emitter; it's used at least for MP_PASS_SCOPE
    emit_t *emit_bc = emit_bc_new();

    // compile pass 1
    comp->emit = emit_bc;
    #if MICROPY_EMIT_NATIVE
    comp->emit_method_table = &emit_bc_method_table;
    #endif
    uint max_num_labels = 0;

    // grrr: scope for nested comp_for's are not used, unless they are parenthesised
    // and become individual generators; in this case they are parsed in the wrong
    // direction for allocation of scope id
    bool keep_going = true;
    while (keep_going) {
        keep_going = false;

    for (uint i = 0; i < comp->num_scopes && comp->compile_error == MP_OBJ_NULL; ++i) {
        scope_t *s = comp->scopes[i];
        if (s == NULL) { continue; } // no scope (yet?)
        if (s->raw_code != NULL) { continue; } // scope already did pass 1
        keep_going = true;
        s->raw_code = mp_emit_glue_new_raw_code();
        if (false) {
#if MICROPY_EMIT_INLINE_THUMB
        } else if (s->emit_options == MP_EMIT_OPT_ASM_THUMB) {
            compile_scope_inline_asm(comp, s, MP_PASS_SCOPE);
#endif
        } else {
            compile_scope(comp, s, MP_PASS_SCOPE);
        }

        // update maximim number of labels needed
        if (comp->next_label > max_num_labels) {
            max_num_labels = comp->next_label;
        }
    }
    }

    // compute some things related to scope and identifiers
    for (uint i = 0; i < comp->num_scopes && comp->compile_error == MP_OBJ_NULL; ++i) {
        scope_t *s = comp->scopes[i];
        if (s == NULL) { continue; } // TODO scope for nested comp_for's are not used
        scope_compute_things(s);
    }

    // set max number of labels now that it's calculated
    emit_bc_set_max_num_labels(emit_bc, max_num_labels);

    // compile pass 2 and 3
#if MICROPY_EMIT_NATIVE
    emit_t *emit_native = NULL;
#endif
#if MICROPY_EMIT_INLINE_THUMB
    emit_inline_asm_t *emit_inline_thumb = NULL;
#endif
    for (uint i = 0; i < comp->num_scopes && comp->compile_error == MP_OBJ_NULL; ++i) {
        scope_t *s = comp->scopes[i];
        if (s == NULL) { continue; }
        if (false) {
            // dummy

#if MICROPY_EMIT_INLINE_THUMB
        } else if (s->emit_options == MP_EMIT_OPT_ASM_THUMB) {
            // inline assembly for thumb
            if (emit_inline_thumb == NULL) {
                emit_inline_thumb = emit_inline_thumb_new(max_num_labels);
            }
            comp->emit = NULL;
            comp->emit_inline_asm = emit_inline_thumb;
            comp->emit_inline_asm_method_table = &emit_inline_thumb_method_table;
            compile_scope_inline_asm(comp, s, MP_PASS_CODE_SIZE);
            if (comp->compile_error == MP_OBJ_NULL) {
                compile_scope_inline_asm(comp, s, MP_PASS_EMIT);
            }
#endif

        } else {

            // choose the emit type

            switch (s->emit_options) {

#if MICROPY_EMIT_NATIVE
                case MP_EMIT_OPT_NATIVE_PYTHON:
                case MP_EMIT_OPT_VIPER:
#if MICROPY_EMIT_X64
                    if (emit_native == NULL) {
                        emit_native = emit_native_x64_new(&comp->compile_error, max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_x64_method_table;
#elif MICROPY_EMIT_X86
                    if (emit_native == NULL) {
                        emit_native = emit_native_x86_new(&comp->compile_error, max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_x86_method_table;
#elif MICROPY_EMIT_THUMB
                    if (emit_native == NULL) {
                        emit_native = emit_native_thumb_new(&comp->compile_error, max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_thumb_method_table;
#elif MICROPY_EMIT_ARM
                    if (emit_native == NULL) {
                        emit_native = emit_native_arm_new(&comp->compile_error, max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_arm_method_table;
#endif
                    comp->emit = emit_native;
                    EMIT_ARG(set_native_type, MP_EMIT_NATIVE_TYPE_ENABLE, s->emit_options == MP_EMIT_OPT_VIPER, 0);
                    break;
#endif // MICROPY_EMIT_NATIVE

                default:
                    comp->emit = emit_bc;
                    #if MICROPY_EMIT_NATIVE
                    comp->emit_method_table = &emit_bc_method_table;
                    #endif
                    break;
            }

            // need a pass to compute stack size
            compile_scope(comp, s, MP_PASS_STACK_SIZE);

            // second last pass: compute code size
            if (comp->compile_error == MP_OBJ_NULL) {
                compile_scope(comp, s, MP_PASS_CODE_SIZE);
            }

            // final pass: emit code
            if (comp->compile_error == MP_OBJ_NULL) {
                compile_scope(comp, s, MP_PASS_EMIT);
            }
        }
    }

    if (comp->compile_error != MP_OBJ_NULL) {
        // if there is no line number for the error then use the line
        // number for the start of this scope
        compile_error_set_line(comp, comp->scope_cur->pn);
        // add a traceback to the exception using relevant source info
        mp_obj_exception_add_traceback(comp->compile_error, comp->source_file,
            comp->compile_error_line, comp->scope_cur->simple_name);
    }

    // free the emitters

    emit_bc_free(emit_bc);
#if MICROPY_EMIT_NATIVE
    if (emit_native != NULL) {
#if MICROPY_EMIT_X64
        emit_native_x64_free(emit_native);
#elif MICROPY_EMIT_X86
        emit_native_x86_free(emit_native);
#elif MICROPY_EMIT_THUMB
        emit_native_thumb_free(emit_native);
#elif MICROPY_EMIT_ARM
        emit_native_arm_free(emit_native);
#endif
    }
#endif
#if MICROPY_EMIT_INLINE_THUMB
    if (emit_inline_thumb != NULL) {
        emit_inline_thumb_free(emit_inline_thumb);
    }
#endif

    // free the parse tree
    mp_parse_tree_clear(parse_tree);

    mp_raw_code_t *outer_raw_code = comp->scopes[0]->raw_code;

    // free the scopes
    for (uint i = 0; i < comp->num_scopes; ++i) {
        if (comp->scopes[i] == NULL) { continue; } // TODO scope for nested comp_for's are not used
        scope_free(comp->scopes[i]);
    }
    m_del(scope_t*, comp->scopes, comp->num_scopes);

    if (comp->compile_error != MP_OBJ_NULL) {
        nlr_raise(comp->compile_error);
    } else {
        return outer_raw_code;
    }
}

mp_obj_t mp_compile(mp_parse_tree_t *parse_tree, qstr source_file, uint emit_opt, bool is_repl) {
    mp_raw_code_t *rc = mp_compile_to_raw_code(parse_tree, source_file, emit_opt, is_repl);
    // return function that executes the outer module
    return mp_make_function_from_raw_code(rc, MP_OBJ_NULL, MP_OBJ_NULL);
}

#endif // MICROPY_ENABLE_COMPILER
