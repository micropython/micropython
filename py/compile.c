/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include <string.h>
#include <assert.h>

#include "py/scope.h"
#include "py/emit.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/asmbase.h"

#if MICROPY_ENABLE_COMPILER

// TODO need to mangle __attr names

#define INVALID_LABEL (0xffff)

typedef enum {
// define rules with a compile function
#define DEF_RULE(rule, comp, kind, ...) PN_##rule,
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
    PN_const_object, // special node for a constant, generic Python object
// define rules without a compile function
#define DEF_RULE(rule, comp, kind, ...)
#define DEF_RULE_NC(rule, kind, ...) PN_##rule,
#include "py/grammar.h"
#undef DEF_RULE
#undef DEF_RULE_NC
} pn_kind_t;

#define NEED_METHOD_TABLE MICROPY_EMIT_NATIVE

#if NEED_METHOD_TABLE

// we need a method table to do the lookup for the emitter functions
#define EMIT(fun) (comp->emit_method_table->fun(comp->emit))
#define EMIT_ARG(fun, ...) (comp->emit_method_table->fun(comp->emit, __VA_ARGS__))
#define EMIT_LOAD_FAST(qst, local_num) (comp->emit_method_table->load_id.local(comp->emit, qst, local_num, MP_EMIT_IDOP_LOCAL_FAST))
#define EMIT_LOAD_GLOBAL(qst) (comp->emit_method_table->load_id.global(comp->emit, qst, MP_EMIT_IDOP_GLOBAL_GLOBAL))

#else

// if we only have the bytecode emitter enabled then we can do a direct call to the functions
#define EMIT(fun) (mp_emit_bc_##fun(comp->emit))
#define EMIT_ARG(fun, ...) (mp_emit_bc_##fun(comp->emit, __VA_ARGS__))
#define EMIT_LOAD_FAST(qst, local_num) (mp_emit_bc_load_local(comp->emit, qst, local_num, MP_EMIT_IDOP_LOCAL_FAST))
#define EMIT_LOAD_GLOBAL(qst) (mp_emit_bc_load_global(comp->emit, qst, MP_EMIT_IDOP_GLOBAL_GLOBAL))

#endif

#if MICROPY_EMIT_NATIVE
// define a macro to access external native emitter
#if MICROPY_EMIT_X64
#define NATIVE_EMITTER(f) emit_native_x64_##f
#elif MICROPY_EMIT_X86
#define NATIVE_EMITTER(f) emit_native_x86_##f
#elif MICROPY_EMIT_THUMB
#define NATIVE_EMITTER(f) emit_native_thumb_##f
#elif MICROPY_EMIT_ARM
#define NATIVE_EMITTER(f) emit_native_arm_##f
#elif MICROPY_EMIT_XTENSA
#define NATIVE_EMITTER(f) emit_native_xtensa_##f
#else
#error "unknown native emitter"
#endif
#endif

#if MICROPY_EMIT_INLINE_ASM
// define macros for inline assembler
#if MICROPY_EMIT_INLINE_THUMB
#define ASM_DECORATOR_QSTR MP_QSTR_asm_thumb
#define ASM_EMITTER(f) emit_inline_thumb_##f
#elif MICROPY_EMIT_INLINE_XTENSA
#define ASM_DECORATOR_QSTR MP_QSTR_asm_xtensa
#define ASM_EMITTER(f) emit_inline_xtensa_##f
#else
#error "unknown asm emitter"
#endif
#endif

#define EMIT_INLINE_ASM(fun) (comp->emit_inline_asm_method_table->fun(comp->emit_inline_asm))
#define EMIT_INLINE_ASM_ARG(fun, ...) (comp->emit_inline_asm_method_table->fun(comp->emit_inline_asm, __VA_ARGS__))

// elements in this struct are ordered to make it compact
typedef struct _compiler_t {
    qstr source_file;

    uint8_t is_repl;
    uint8_t pass; // holds enum type pass_kind_t
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

    scope_t *scope_head;
    scope_t *scope_cur;

    emit_t *emit;                                   // current emitter
    #if NEED_METHOD_TABLE
    const emit_method_table_t *emit_method_table;   // current emit method table
    #endif

    #if MICROPY_EMIT_INLINE_ASM
    emit_inline_asm_t *emit_inline_asm;                                   // current emitter for inline asm
    const emit_inline_asm_method_table_t *emit_inline_asm_method_table;   // current emit method table for inline asm
    #endif
} compiler_t;

STATIC void compile_error_set_line(compiler_t *comp, mp_parse_node_t pn) {
    // if the line of the error is unknown then try to update it from the pn
    if (comp->compile_error_line == 0 && MP_PARSE_NODE_IS_STRUCT(pn)) {
        comp->compile_error_line = ((mp_parse_node_struct_t*)pn)->source_line;
    }
}

STATIC void compile_syntax_error(compiler_t *comp, mp_parse_node_t pn, const char *msg) {
    // only register the error if there has been no other error
    if (comp->compile_error == MP_OBJ_NULL) {
        comp->compile_error = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
        compile_error_set_line(comp, pn);
    }
}

STATIC void compile_trailer_paren_helper(compiler_t *comp, mp_parse_node_t pn_arglist, bool is_method_call, int n_positional_extra);
STATIC void compile_comprehension(compiler_t *comp, mp_parse_node_struct_t *pns, scope_kind_t kind);
STATIC void compile_node(compiler_t *comp, mp_parse_node_t pn);

STATIC uint comp_next_label(compiler_t *comp) {
    return comp->next_label++;
}

#if MICROPY_EMIT_NATIVE
STATIC void reserve_labels_for_native(compiler_t *comp, int n) {
    if (comp->scope_cur->emit_options != MP_EMIT_OPT_BYTECODE) {
        comp->next_label += n;
    }
}
#else
#define reserve_labels_for_native(comp, n)
#endif

STATIC void compile_increase_except_level(compiler_t *comp, uint label, int kind) {
    EMIT_ARG(setup_block, label, kind);
    comp->cur_except_level += 1;
    if (comp->cur_except_level > comp->scope_cur->exc_stack_size) {
        comp->scope_cur->exc_stack_size = comp->cur_except_level;
    }
}

STATIC void compile_decrease_except_level(compiler_t *comp) {
    assert(comp->cur_except_level > 0);
    comp->cur_except_level -= 1;
    EMIT(end_finally);
    reserve_labels_for_native(comp, 1);
}

STATIC scope_t *scope_new_and_link(compiler_t *comp, scope_kind_t kind, mp_parse_node_t pn, uint emit_options) {
    scope_t *scope = scope_new(kind, pn, comp->source_file, emit_options);
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

typedef void (*apply_list_fun_t)(compiler_t *comp, mp_parse_node_t pn);

STATIC void apply_to_single_or_list(compiler_t *comp, mp_parse_node_t pn, pn_kind_t pn_list_kind, apply_list_fun_t f) {
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, pn_list_kind)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        for (int i = 0; i < num_nodes; i++) {
            f(comp, pns->nodes[i]);
        }
    } else if (!MP_PARSE_NODE_IS_NULL(pn)) {
        f(comp, pn);
    }
}

STATIC void compile_generic_all_nodes(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < num_nodes; i++) {
        compile_node(comp, pns->nodes[i]);
        if (comp->compile_error != MP_OBJ_NULL) {
            // add line info for the error in case it didn't have a line number
            compile_error_set_line(comp, pns->nodes[i]);
            return;
        }
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

STATIC void c_tuple(compiler_t *comp, mp_parse_node_t pn, mp_parse_node_struct_t *pns_list) {
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
    EMIT_ARG(build, total, MP_EMIT_BUILD_TUPLE);
}

STATIC void compile_generic_tuple(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // a simple tuple expression
    c_tuple(comp, MP_PARSE_NODE_NULL, pns);
}

STATIC void c_if_cond(compiler_t *comp, mp_parse_node_t pn, bool jump_if, int label) {
    if (mp_parse_node_is_const_false(pn)) {
        if (jump_if == false) {
            EMIT_ARG(jump, label);
        }
        return;
    } else if (mp_parse_node_is_const_true(pn)) {
        if (jump_if == true) {
            EMIT_ARG(jump, label);
        }
        return;
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_or_test) {
            if (jump_if == false) {
            and_or_logic1:;
                uint label2 = comp_next_label(comp);
                for (int i = 0; i < n - 1; i++) {
                    c_if_cond(comp, pns->nodes[i], !jump_if, label2);
                }
                c_if_cond(comp, pns->nodes[n - 1], jump_if, label);
                EMIT_ARG(label_assign, label2);
            } else {
            and_or_logic2:
                for (int i = 0; i < n; i++) {
                    c_if_cond(comp, pns->nodes[i], jump_if, label);
                }
            }
            return;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_and_test) {
            if (jump_if == false) {
                goto and_or_logic2;
            } else {
                goto and_or_logic1;
            }
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_not_test_2) {
            c_if_cond(comp, pns->nodes[0], !jump_if, label);
            return;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_atom_paren) {
            // cond is something in parenthesis
            if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                // empty tuple, acts as false for the condition
                if (jump_if == false) {
                    EMIT_ARG(jump, label);
                }
            } else {
                assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp));
                // non-empty tuple, acts as true for the condition
                if (jump_if == true) {
                    EMIT_ARG(jump, label);
                }
            }
            return;
        }
    }

    // nothing special, fall back to default compiling for node and jump
    compile_node(comp, pn);
    EMIT_ARG(pop_jump_if, jump_if, label);
}

typedef enum { ASSIGN_STORE, ASSIGN_AUG_LOAD, ASSIGN_AUG_STORE } assign_kind_t;
STATIC void c_assign(compiler_t *comp, mp_parse_node_t pn, assign_kind_t kind);

STATIC void c_assign_atom_expr(compiler_t *comp, mp_parse_node_struct_t *pns, assign_kind_t assign_kind) {
    if (assign_kind != ASSIGN_AUG_STORE) {
        compile_node(comp, pns->nodes[0]);
    }

    if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
        mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
        if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_atom_expr_trailers) {
            int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns1);
            if (assign_kind != ASSIGN_AUG_STORE) {
                for (int i = 0; i < n - 1; i++) {
                    compile_node(comp, pns1->nodes[i]);
                }
            }
            assert(MP_PARSE_NODE_IS_STRUCT(pns1->nodes[n - 1]));
            pns1 = (mp_parse_node_struct_t*)pns1->nodes[n - 1];
        }
        if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_bracket) {
            if (assign_kind == ASSIGN_AUG_STORE) {
                EMIT(rot_three);
                EMIT_ARG(subscr, MP_EMIT_SUBSCR_STORE);
            } else {
                compile_node(comp, pns1->nodes[0]);
                if (assign_kind == ASSIGN_AUG_LOAD) {
                    EMIT(dup_top_two);
                    EMIT_ARG(subscr, MP_EMIT_SUBSCR_LOAD);
                } else {
                    EMIT_ARG(subscr, MP_EMIT_SUBSCR_STORE);
                }
            }
            return;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_period) {
            assert(MP_PARSE_NODE_IS_ID(pns1->nodes[0]));
            if (assign_kind == ASSIGN_AUG_LOAD) {
                EMIT(dup_top);
                EMIT_ARG(attr, MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]), MP_EMIT_ATTR_LOAD);
            } else {
                if (assign_kind == ASSIGN_AUG_STORE) {
                    EMIT(rot_two);
                }
                EMIT_ARG(attr, MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]), MP_EMIT_ATTR_STORE);
            }
            return;
        }
    }

    compile_syntax_error(comp, (mp_parse_node_t)pns, "can't assign to expression");
}

// we need to allow for a caller passing in 1 initial node (node_head) followed by an array of nodes (nodes_tail)
STATIC void c_assign_tuple(compiler_t *comp, mp_parse_node_t node_head, uint num_tail, mp_parse_node_t *nodes_tail) {
    uint num_head = (node_head == MP_PARSE_NODE_NULL) ? 0 : 1;

    // look for star expression
    uint have_star_index = -1;
    if (num_head != 0 && MP_PARSE_NODE_IS_STRUCT_KIND(node_head, PN_star_expr)) {
        EMIT_ARG(unpack_ex, 0, num_tail);
        have_star_index = 0;
    }
    for (uint i = 0; i < num_tail; i++) {
        if (MP_PARSE_NODE_IS_STRUCT_KIND(nodes_tail[i], PN_star_expr)) {
            if (have_star_index == (uint)-1) {
                EMIT_ARG(unpack_ex, num_head + i, num_tail - i - 1);
                have_star_index = num_head + i;
            } else {
                compile_syntax_error(comp, nodes_tail[i], "multiple *x in assignment");
                return;
            }
        }
    }
    if (have_star_index == (uint)-1) {
        EMIT_ARG(unpack_sequence, num_head + num_tail);
    }
    if (num_head != 0) {
        if (0 == have_star_index) {
            c_assign(comp, ((mp_parse_node_struct_t*)node_head)->nodes[0], ASSIGN_STORE);
        } else {
            c_assign(comp, node_head, ASSIGN_STORE);
        }
    }
    for (uint i = 0; i < num_tail; i++) {
        if (num_head + i == have_star_index) {
            c_assign(comp, ((mp_parse_node_struct_t*)nodes_tail[i])->nodes[0], ASSIGN_STORE);
        } else {
            c_assign(comp, nodes_tail[i], ASSIGN_STORE);
        }
    }
}

// assigns top of stack to pn
STATIC void c_assign(compiler_t *comp, mp_parse_node_t pn, assign_kind_t assign_kind) {
    assert(!MP_PARSE_NODE_IS_NULL(pn));
    if (MP_PARSE_NODE_IS_LEAF(pn)) {
        if (MP_PARSE_NODE_IS_ID(pn)) {
            qstr arg = MP_PARSE_NODE_LEAF_ARG(pn);
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
        } else {
            goto cannot_assign;
        }
    } else {
        // pn must be a struct
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        switch (MP_PARSE_NODE_STRUCT_KIND(pns)) {
            case PN_atom_expr_normal:
                // lhs is an index or attribute
                c_assign_atom_expr(comp, pns, assign_kind);
                break;

            case PN_testlist_star_expr:
            case PN_exprlist:
                // lhs is a tuple
                if (assign_kind != ASSIGN_STORE) {
                    goto cannot_assign;
                }
                c_assign_tuple(comp, MP_PARSE_NODE_NULL, MP_PARSE_NODE_STRUCT_NUM_NODES(pns), pns->nodes);
                break;

            case PN_atom_paren:
                // lhs is something in parenthesis
                if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                    // empty tuple
                    goto cannot_assign;
                } else {
                    assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp));
                    if (assign_kind != ASSIGN_STORE) {
                        goto cannot_assign;
                    }
                    pns = (mp_parse_node_struct_t*)pns->nodes[0];
                    goto testlist_comp;
                }
                break;

            case PN_atom_bracket:
                // lhs is something in brackets
                if (assign_kind != ASSIGN_STORE) {
                    goto cannot_assign;
                }
                if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                    // empty list, assignment allowed
                    c_assign_tuple(comp, MP_PARSE_NODE_NULL, 0, NULL);
                } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
                    pns = (mp_parse_node_struct_t*)pns->nodes[0];
                    goto testlist_comp;
                } else {
                    // brackets around 1 item
                    c_assign_tuple(comp, pns->nodes[0], 0, NULL);
                }
                break;

            default:
                goto cannot_assign;
        }
        return;

        testlist_comp:
        // lhs is a sequence
        if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
            mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3b) {
                // sequence of one item, with trailing comma
                assert(MP_PARSE_NODE_IS_NULL(pns2->nodes[0]));
                c_assign_tuple(comp, pns->nodes[0], 0, NULL);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_testlist_comp_3c) {
                // sequence of many items
                uint n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns2);
                c_assign_tuple(comp, pns->nodes[0], n, pns2->nodes);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_comp_for) {
                goto cannot_assign;
            } else {
                // sequence with 2 items
                goto sequence_with_2_items;
            }
        } else {
            // sequence with 2 items
            sequence_with_2_items:
            c_assign_tuple(comp, MP_PARSE_NODE_NULL, 2, pns->nodes);
        }
        return;
    }
    return;

    cannot_assign:
    compile_syntax_error(comp, pn, "can't assign to expression");
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

    #if MICROPY_EMIT_NATIVE
    // When creating a function/closure it will take a reference to the current globals
    comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_REFGLOBALS;
    #endif

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
                        // in MicroPython we load closures using LOAD_FAST
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

STATIC void compile_funcdef_lambdef_param(compiler_t *comp, mp_parse_node_t pn) {
    // For efficiency of the code below we extract the parse-node kind here
    int pn_kind;
    if (MP_PARSE_NODE_IS_ID(pn)) {
        pn_kind = -1;
    } else {
        assert(MP_PARSE_NODE_IS_STRUCT(pn));
        pn_kind = MP_PARSE_NODE_STRUCT_KIND((mp_parse_node_struct_t*)pn);
    }

    if (pn_kind == PN_typedargslist_star || pn_kind == PN_varargslist_star) {
        comp->have_star = true;
        /* don't need to distinguish bare from named star
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
            // bare star
        } else {
            // named star
        }
        */

    } else if (pn_kind == PN_typedargslist_dbl_star || pn_kind == PN_varargslist_dbl_star) {
        // named double star
        // TODO do we need to do anything with this?

    } else {
        mp_parse_node_t pn_id;
        mp_parse_node_t pn_equal;
        if (pn_kind == -1) {
            // this parameter is just an id

            pn_id = pn;
            pn_equal = MP_PARSE_NODE_NULL;

        } else if (pn_kind == PN_typedargslist_name) {
            // this parameter has a colon and/or equal specifier

            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
            pn_id = pns->nodes[0];
            //pn_colon = pns->nodes[1]; // unused
            pn_equal = pns->nodes[2];

        } else {
            assert(pn_kind == PN_varargslist_name); // should be
            // this parameter has an equal specifier

            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
            pn_id = pns->nodes[0];
            pn_equal = pns->nodes[1];
        }

        if (MP_PARSE_NODE_IS_NULL(pn_equal)) {
            // this parameter does not have a default value

            // check for non-default parameters given after default parameters (allowed by parser, but not syntactically valid)
            if (!comp->have_star && comp->num_default_params != 0) {
                compile_syntax_error(comp, pn, "non-default argument follows default argument");
                return;
            }

        } else {
            // this parameter has a default value
            // in CPython, None (and True, False?) as default parameters are loaded with LOAD_NAME; don't understandy why

            if (comp->have_star) {
                comp->num_dict_params += 1;
                // in MicroPython we put the default dict parameters into a dictionary using the bytecode
                if (comp->num_dict_params == 1) {
                    // in MicroPython we put the default positional parameters into a tuple using the bytecode
                    // we need to do this here before we start building the map for the default keywords
                    if (comp->num_default_params > 0) {
                        EMIT_ARG(build, comp->num_default_params, MP_EMIT_BUILD_TUPLE);
                    } else {
                        EMIT(load_null); // sentinel indicating empty default positional args
                    }
                    // first default dict param, so make the map
                    EMIT_ARG(build, 0, MP_EMIT_BUILD_MAP);
                }

                // compile value then key, then store it to the dict
                compile_node(comp, pn_equal);
                EMIT_ARG(load_const_str, MP_PARSE_NODE_LEAF_ARG(pn_id));
                EMIT(store_map);
            } else {
                comp->num_default_params += 1;
                compile_node(comp, pn_equal);
            }
        }
    }
}

STATIC void compile_funcdef_lambdef(compiler_t *comp, scope_t *scope, mp_parse_node_t pn_params, pn_kind_t pn_list_kind) {
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
    apply_to_single_or_list(comp, pn_params, pn_list_kind, compile_funcdef_lambdef_param);

    if (comp->compile_error != MP_OBJ_NULL) {
        return;
    }

    // in MicroPython we put the default positional parameters into a tuple using the bytecode
    // the default keywords args may have already made the tuple; if not, do it now
    if (comp->num_default_params > 0 && comp->num_dict_params == 0) {
        EMIT_ARG(build, comp->num_default_params, MP_EMIT_BUILD_TUPLE);
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
STATIC qstr compile_funcdef_helper(compiler_t *comp, mp_parse_node_struct_t *pns, uint emit_options) {
    if (comp->pass == MP_PASS_SCOPE) {
        // create a new scope for this function
        scope_t *s = scope_new_and_link(comp, SCOPE_FUNCTION, (mp_parse_node_t)pns, emit_options);
        // store the function scope so the compiling function can use it at each pass
        pns->nodes[4] = (mp_parse_node_t)s;
    }

    // get the scope for this function
    scope_t *fscope = (scope_t*)pns->nodes[4];

    // compile the function definition
    compile_funcdef_lambdef(comp, fscope, pns->nodes[1], PN_typedargslist);

    // return its name (the 'f' in "def f(...):")
    return fscope->simple_name;
}

// leaves class object on stack
// returns class name
STATIC qstr compile_classdef_helper(compiler_t *comp, mp_parse_node_struct_t *pns, uint emit_options) {
    if (comp->pass == MP_PASS_SCOPE) {
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
    EMIT_ARG(load_const_str, cscope->simple_name);

    // nodes[1] has parent classes, if any
    // empty parenthesis (eg class C():) gets here as an empty PN_classdef_2 and needs special handling
    mp_parse_node_t parents = pns->nodes[1];
    if (MP_PARSE_NODE_IS_STRUCT_KIND(parents, PN_classdef_2)) {
        parents = MP_PARSE_NODE_NULL;
    }
    compile_trailer_paren_helper(comp, parents, false, 2);

    // return its name (the 'C' in class C(...):")
    return cscope->simple_name;
}

// returns true if it was a built-in decorator (even if the built-in had an error)
STATIC bool compile_built_in_decorator(compiler_t *comp, int name_len, mp_parse_node_t *name_nodes, uint *emit_options) {
    if (MP_PARSE_NODE_LEAF_ARG(name_nodes[0]) != MP_QSTR_micropython) {
        return false;
    }

    if (name_len != 2) {
        compile_syntax_error(comp, name_nodes[0], "invalid micropython decorator");
        return true;
    }

    qstr attr = MP_PARSE_NODE_LEAF_ARG(name_nodes[1]);
    if (attr == MP_QSTR_bytecode) {
        *emit_options = MP_EMIT_OPT_BYTECODE;
#if MICROPY_EMIT_NATIVE
    } else if (attr == MP_QSTR_native) {
        *emit_options = MP_EMIT_OPT_NATIVE_PYTHON;
    } else if (attr == MP_QSTR_viper) {
        *emit_options = MP_EMIT_OPT_VIPER;
#endif
    #if MICROPY_EMIT_INLINE_ASM
    } else if (attr == ASM_DECORATOR_QSTR) {
        *emit_options = MP_EMIT_OPT_ASM;
    #endif
    } else {
        compile_syntax_error(comp, name_nodes[1], "invalid micropython decorator");
    }

    return true;
}

STATIC void compile_decorated(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // get the list of decorators
    mp_parse_node_t *nodes;
    int n = mp_parse_node_extract_list(&pns->nodes[0], PN_decorators, &nodes);

    // inherit emit options for this function/class definition
    uint emit_options = comp->scope_cur->emit_options;

    // compile each decorator
    int num_built_in_decorators = 0;
    for (int i = 0; i < n; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(nodes[i], PN_decorator)); // should be
        mp_parse_node_struct_t *pns_decorator = (mp_parse_node_struct_t*)nodes[i];

        // nodes[0] contains the decorator function, which is a dotted name
        mp_parse_node_t *name_nodes;
        int name_len = mp_parse_node_extract_list(&pns_decorator->nodes[0], PN_dotted_name, &name_nodes);

        // check for built-in decorators
        if (compile_built_in_decorator(comp, name_len, name_nodes, &emit_options)) {
            // this was a built-in
            num_built_in_decorators += 1;

        } else {
            // not a built-in, compile normally

            // compile the decorator function
            compile_node(comp, name_nodes[0]);
            for (int j = 1; j < name_len; j++) {
                assert(MP_PARSE_NODE_IS_ID(name_nodes[j])); // should be
                EMIT_ARG(attr, MP_PARSE_NODE_LEAF_ARG(name_nodes[j]), MP_EMIT_ATTR_LOAD);
            }

            // nodes[1] contains arguments to the decorator function, if any
            if (!MP_PARSE_NODE_IS_NULL(pns_decorator->nodes[1])) {
                // call the decorator function with the arguments in nodes[1]
                compile_node(comp, pns_decorator->nodes[1]);
            }
        }
    }

    // compile the body (funcdef, async funcdef or classdef) and get its name
    mp_parse_node_struct_t *pns_body = (mp_parse_node_struct_t*)pns->nodes[1];
    qstr body_name = 0;
    if (MP_PARSE_NODE_STRUCT_KIND(pns_body) == PN_funcdef) {
        body_name = compile_funcdef_helper(comp, pns_body, emit_options);
    #if MICROPY_PY_ASYNC_AWAIT
    } else if (MP_PARSE_NODE_STRUCT_KIND(pns_body) == PN_async_funcdef) {
        assert(MP_PARSE_NODE_IS_STRUCT(pns_body->nodes[0]));
        mp_parse_node_struct_t *pns0 = (mp_parse_node_struct_t*)pns_body->nodes[0];
        body_name = compile_funcdef_helper(comp, pns0, emit_options);
        scope_t *fscope = (scope_t*)pns0->nodes[4];
        fscope->scope_flags |= MP_SCOPE_FLAG_GENERATOR;
    #endif
    } else {
        assert(MP_PARSE_NODE_STRUCT_KIND(pns_body) == PN_classdef); // should be
        body_name = compile_classdef_helper(comp, pns_body, emit_options);
    }

    // call each decorator
    for (int i = 0; i < n - num_built_in_decorators; i++) {
        EMIT_ARG(call_function, 1, 0, 0);
    }

    // store func/class object into name
    compile_store_id(comp, body_name);
}

STATIC void compile_funcdef(compiler_t *comp, mp_parse_node_struct_t *pns) {
    qstr fname = compile_funcdef_helper(comp, pns, comp->scope_cur->emit_options);
    // store function object into function name
    compile_store_id(comp, fname);
}

STATIC void c_del_stmt(compiler_t *comp, mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_ID(pn)) {
        compile_delete_id(comp, MP_PARSE_NODE_LEAF_ARG(pn));
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_atom_expr_normal)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;

        compile_node(comp, pns->nodes[0]); // base of the atom_expr_normal node

        if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
            mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_atom_expr_trailers) {
                int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns1);
                for (int i = 0; i < n - 1; i++) {
                    compile_node(comp, pns1->nodes[i]);
                }
                assert(MP_PARSE_NODE_IS_STRUCT(pns1->nodes[n - 1]));
                pns1 = (mp_parse_node_struct_t*)pns1->nodes[n - 1];
            }
            if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_bracket) {
                compile_node(comp, pns1->nodes[0]);
                EMIT_ARG(subscr, MP_EMIT_SUBSCR_DELETE);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_period) {
                assert(MP_PARSE_NODE_IS_ID(pns1->nodes[0]));
                EMIT_ARG(attr, MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]), MP_EMIT_ATTR_DELETE);
            } else {
                goto cannot_delete;
            }
        } else {
            goto cannot_delete;
        }

    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_atom_paren)) {
        pn = ((mp_parse_node_struct_t*)pn)->nodes[0];
        if (MP_PARSE_NODE_IS_NULL(pn)) {
            goto cannot_delete;
        } else {
            assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_testlist_comp));
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
                } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_comp_for) {
                    goto cannot_delete;
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
        }
    } else {
        // some arbitrary statement that we can't delete (eg del 1)
        goto cannot_delete;
    }

    return;

cannot_delete:
    compile_syntax_error(comp, (mp_parse_node_t)pn, "can't delete expression");
}

STATIC void compile_del_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    apply_to_single_or_list(comp, pns->nodes[0], PN_exprlist, c_del_stmt);
}

STATIC void compile_break_cont_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    uint16_t label;
    const char *error_msg;
    if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_break_stmt) {
        label = comp->break_label;
        error_msg = "'break' outside loop";
    } else {
        label = comp->continue_label;
        error_msg = "'continue' outside loop";
    }
    if (label == INVALID_LABEL) {
        compile_syntax_error(comp, (mp_parse_node_t)pns, error_msg);
    }
    assert(comp->cur_except_level >= comp->break_continue_except_level);
    EMIT_ARG(unwind_jump, label, comp->cur_except_level - comp->break_continue_except_level);
}

STATIC void compile_return_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION) {
        compile_syntax_error(comp, (mp_parse_node_t)pns, "'return' outside function");
        return;
    }
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // no argument to 'return', so return None
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    } else if (MICROPY_COMP_RETURN_IF_EXPR
        && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_test_if_expr)) {
        // special case when returning an if-expression; to match CPython optimisation
        mp_parse_node_struct_t *pns_test_if_expr = (mp_parse_node_struct_t*)pns->nodes[0];
        mp_parse_node_struct_t *pns_test_if_else = (mp_parse_node_struct_t*)pns_test_if_expr->nodes[1];

        uint l_fail = comp_next_label(comp);
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

STATIC void compile_yield_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]);
    EMIT(pop_top);
}

STATIC void compile_raise_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
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

// q_base holds the base of the name
// eg   a -> q_base=a
//      a.b.c -> q_base=a
STATIC void do_import_name(compiler_t *comp, mp_parse_node_t pn, qstr *q_base) {
    bool is_as = false;
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_dotted_as_name)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        // a name of the form x as y; unwrap it
        *q_base = MP_PARSE_NODE_LEAF_ARG(pns->nodes[1]);
        pn = pns->nodes[0];
        is_as = true;
    }
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        // empty name (eg, from . import x)
        *q_base = MP_QSTR_;
        EMIT_ARG(import, MP_QSTR_, MP_EMIT_IMPORT_NAME); // import the empty string
    } else if (MP_PARSE_NODE_IS_ID(pn)) {
        // just a simple name
        qstr q_full = MP_PARSE_NODE_LEAF_ARG(pn);
        if (!is_as) {
            *q_base = q_full;
        }
        EMIT_ARG(import, q_full, MP_EMIT_IMPORT_NAME);
    } else {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_dotted_name)); // should be
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        {
            // a name of the form a.b.c
            if (!is_as) {
                *q_base = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            }
            int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
            int len = n - 1;
            for (int i = 0; i < n; i++) {
                len += qstr_len(MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
            }
            char *q_ptr = mp_local_alloc(len);
            char *str_dest = q_ptr;
            for (int i = 0; i < n; i++) {
                if (i > 0) {
                    *str_dest++ = '.';
                }
                size_t str_src_len;
                const byte *str_src = qstr_data(MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]), &str_src_len);
                memcpy(str_dest, str_src, str_src_len);
                str_dest += str_src_len;
            }
            qstr q_full = qstr_from_strn(q_ptr, len);
            mp_local_free(q_ptr);
            EMIT_ARG(import, q_full, MP_EMIT_IMPORT_NAME);
            if (is_as) {
                for (int i = 1; i < n; i++) {
                    EMIT_ARG(attr, MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]), MP_EMIT_ATTR_LOAD);
                }
            }
        }
    }
}

STATIC void compile_dotted_as_name(compiler_t *comp, mp_parse_node_t pn) {
    EMIT_ARG(load_const_small_int, 0); // level 0 import
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE); // not importing from anything
    qstr q_base;
    do_import_name(comp, pn, &q_base);
    compile_store_id(comp, q_base);
}

STATIC void compile_import_name(compiler_t *comp, mp_parse_node_struct_t *pns) {
    apply_to_single_or_list(comp, pns->nodes[0], PN_dotted_as_names, compile_dotted_as_name);
}

STATIC void compile_import_from(compiler_t *comp, mp_parse_node_struct_t *pns) {
    mp_parse_node_t pn_import_source = pns->nodes[0];

    // extract the preceding .'s (if any) for a relative import, to compute the import level
    uint import_level = 0;
    do {
        mp_parse_node_t pn_rel;
        if (MP_PARSE_NODE_IS_TOKEN(pn_import_source) || MP_PARSE_NODE_IS_STRUCT_KIND(pn_import_source, PN_one_or_more_period_or_ellipsis)) {
            // This covers relative imports with dots only like "from .. import"
            pn_rel = pn_import_source;
            pn_import_source = MP_PARSE_NODE_NULL;
        } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn_import_source, PN_import_from_2b)) {
            // This covers relative imports starting with dot(s) like "from .foo import"
            mp_parse_node_struct_t *pns_2b = (mp_parse_node_struct_t*)pn_import_source;
            pn_rel = pns_2b->nodes[0];
            pn_import_source = pns_2b->nodes[1];
            assert(!MP_PARSE_NODE_IS_NULL(pn_import_source)); // should not be
        } else {
            // Not a relative import
            break;
        }

        // get the list of . and/or ...'s
        mp_parse_node_t *nodes;
        int n = mp_parse_node_extract_list(&pn_rel, PN_one_or_more_period_or_ellipsis, &nodes);

        // count the total number of .'s
        for (int i = 0; i < n; i++) {
            if (MP_PARSE_NODE_IS_TOKEN_KIND(nodes[i], MP_TOKEN_DEL_PERIOD)) {
                import_level++;
            } else {
                // should be an MP_TOKEN_ELLIPSIS
                import_level += 3;
            }
        }
    } while (0);

    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_STAR)) {
        EMIT_ARG(load_const_small_int, import_level);

        // build the "fromlist" tuple
        EMIT_ARG(load_const_str, MP_QSTR__star_);
        EMIT_ARG(build, 1, MP_EMIT_BUILD_TUPLE);

        // do the import
        qstr dummy_q;
        do_import_name(comp, pn_import_source, &dummy_q);
        EMIT_ARG(import, MP_QSTR_NULL, MP_EMIT_IMPORT_STAR);

    } else {
        EMIT_ARG(load_const_small_int, import_level);

        // build the "fromlist" tuple
        mp_parse_node_t *pn_nodes;
        int n = mp_parse_node_extract_list(&pns->nodes[1], PN_import_as_names, &pn_nodes);
        for (int i = 0; i < n; i++) {
            assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_nodes[i], PN_import_as_name));
            mp_parse_node_struct_t *pns3 = (mp_parse_node_struct_t*)pn_nodes[i];
            qstr id2 = MP_PARSE_NODE_LEAF_ARG(pns3->nodes[0]); // should be id
            EMIT_ARG(load_const_str, id2);
        }
        EMIT_ARG(build, n, MP_EMIT_BUILD_TUPLE);

        // do the import
        qstr dummy_q;
        do_import_name(comp, pn_import_source, &dummy_q);
        for (int i = 0; i < n; i++) {
            assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_nodes[i], PN_import_as_name));
            mp_parse_node_struct_t *pns3 = (mp_parse_node_struct_t*)pn_nodes[i];
            qstr id2 = MP_PARSE_NODE_LEAF_ARG(pns3->nodes[0]); // should be id
            EMIT_ARG(import, id2, MP_EMIT_IMPORT_FROM);
            if (MP_PARSE_NODE_IS_NULL(pns3->nodes[1])) {
                compile_store_id(comp, id2);
            } else {
                compile_store_id(comp, MP_PARSE_NODE_LEAF_ARG(pns3->nodes[1]));
            }
        }
        EMIT(pop_top);
    }
}

STATIC void compile_declare_global(compiler_t *comp, mp_parse_node_t pn, qstr qst, bool added, id_info_t *id_info) {
    if (!added && id_info->kind != ID_INFO_KIND_GLOBAL_EXPLICIT) {
        compile_syntax_error(comp, pn, "identifier redefined as global");
        return;
    }
    id_info->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;

    // if the id exists in the global scope, set its kind to EXPLICIT_GLOBAL
    id_info = scope_find_global(comp->scope_cur, qst);
    if (id_info != NULL) {
        id_info->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
    }
}

STATIC void compile_declare_nonlocal(compiler_t *comp, mp_parse_node_t pn, qstr qst, bool added, id_info_t *id_info) {
    if (added) {
        scope_find_local_and_close_over(comp->scope_cur, id_info, qst);
        if (id_info->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
            compile_syntax_error(comp, pn, "no binding for nonlocal found");
        }
    } else if (id_info->kind != ID_INFO_KIND_FREE) {
        compile_syntax_error(comp, pn, "identifier redefined as nonlocal");
    }
}

STATIC void compile_global_nonlocal_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->pass == MP_PASS_SCOPE) {
        bool is_global = MP_PARSE_NODE_STRUCT_KIND(pns) == PN_global_stmt;

        if (!is_global && comp->scope_cur->kind == SCOPE_MODULE) {
            compile_syntax_error(comp, (mp_parse_node_t)pns, "can't declare nonlocal in outer code");
            return;
        }

        mp_parse_node_t *nodes;
        int n = mp_parse_node_extract_list(&pns->nodes[0], PN_name_list, &nodes);
        for (int i = 0; i < n; i++) {
            qstr qst = MP_PARSE_NODE_LEAF_ARG(nodes[i]);
            bool added;
            id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, qst, &added);
            if (is_global) {
                compile_declare_global(comp, (mp_parse_node_t)pns, qst, added, id_info);
            } else {
                compile_declare_nonlocal(comp, (mp_parse_node_t)pns, qst, added, id_info);
            }
        }
    }
}

STATIC void compile_assert_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // with optimisations enabled we don't compile assertions
    if (MP_STATE_VM(mp_optimise_value) != 0) {
        return;
    }

    uint l_end = comp_next_label(comp);
    c_if_cond(comp, pns->nodes[0], true, l_end);
    EMIT_LOAD_GLOBAL(MP_QSTR_AssertionError); // we load_global instead of load_id, to be consistent with CPython
    if (!MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        // assertion message
        compile_node(comp, pns->nodes[1]);
        EMIT_ARG(call_function, 1, 0, 0);
    }
    EMIT_ARG(raise_varargs, 1);
    EMIT_ARG(label_assign, l_end);
}

STATIC void compile_if_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    uint l_end = comp_next_label(comp);

    // optimisation: don't emit anything when "if False"
    if (!mp_parse_node_is_const_false(pns->nodes[0])) {
        uint l_fail = comp_next_label(comp);
        c_if_cond(comp, pns->nodes[0], false, l_fail); // if condition

        compile_node(comp, pns->nodes[1]); // if block

        // optimisation: skip everything else when "if True"
        if (mp_parse_node_is_const_true(pns->nodes[0])) {
            goto done;
        }

        if (
            // optimisation: don't jump over non-existent elif/else blocks
            !(MP_PARSE_NODE_IS_NULL(pns->nodes[2]) && MP_PARSE_NODE_IS_NULL(pns->nodes[3]))
            // optimisation: don't jump if last instruction was return
            && !EMIT(last_emit_was_return_value)
            ) {
            // jump over elif/else blocks
            EMIT_ARG(jump, l_end);
        }

        EMIT_ARG(label_assign, l_fail);
    }

    // compile elif blocks (if any)
    mp_parse_node_t *pn_elif;
    int n_elif = mp_parse_node_extract_list(&pns->nodes[2], PN_if_stmt_elif_list, &pn_elif);
    for (int i = 0; i < n_elif; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_elif[i], PN_if_stmt_elif)); // should be
        mp_parse_node_struct_t *pns_elif = (mp_parse_node_struct_t*)pn_elif[i];

        // optimisation: don't emit anything when "if False"
        if (!mp_parse_node_is_const_false(pns_elif->nodes[0])) {
            uint l_fail = comp_next_label(comp);
            c_if_cond(comp, pns_elif->nodes[0], false, l_fail); // elif condition

            compile_node(comp, pns_elif->nodes[1]); // elif block

            // optimisation: skip everything else when "elif True"
            if (mp_parse_node_is_const_true(pns_elif->nodes[0])) {
                goto done;
            }

            // optimisation: don't jump if last instruction was return
            if (!EMIT(last_emit_was_return_value)) {
                EMIT_ARG(jump, l_end);
            }
            EMIT_ARG(label_assign, l_fail);
        }
    }

    // compile else block
    compile_node(comp, pns->nodes[3]); // can be null

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

STATIC void compile_while_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    START_BREAK_CONTINUE_BLOCK

    if (!mp_parse_node_is_const_false(pns->nodes[0])) { // optimisation: don't emit anything for "while False"
        uint top_label = comp_next_label(comp);
        if (!mp_parse_node_is_const_true(pns->nodes[0])) { // optimisation: don't jump to cond for "while True"
            EMIT_ARG(jump, continue_label);
        }
        EMIT_ARG(label_assign, top_label);
        compile_node(comp, pns->nodes[1]); // body
        EMIT_ARG(label_assign, continue_label);
        c_if_cond(comp, pns->nodes[0], true, top_label); // condition
    }

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    compile_node(comp, pns->nodes[2]); // else

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
STATIC void compile_for_stmt_optimised_range(compiler_t *comp, mp_parse_node_t pn_var, mp_parse_node_t pn_start, mp_parse_node_t pn_end, mp_parse_node_t pn_step, mp_parse_node_t pn_body, mp_parse_node_t pn_else) {
    START_BREAK_CONTINUE_BLOCK

    uint top_label = comp_next_label(comp);
    uint entry_label = comp_next_label(comp);

    // put the end value on the stack if it's not a small-int constant
    bool end_on_stack = !MP_PARSE_NODE_IS_SMALL_INT(pn_end);
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
    compile_node(comp, pn_step);
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
    assert(MP_PARSE_NODE_IS_SMALL_INT(pn_step));
    if (MP_PARSE_NODE_LEAF_SMALL_INT(pn_step) >= 0) {
        EMIT_ARG(binary_op, MP_BINARY_OP_LESS);
    } else {
        EMIT_ARG(binary_op, MP_BINARY_OP_MORE);
    }
    EMIT_ARG(pop_jump_if, true, top_label);

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    // Compile the else block.  We must pop the iterator variables before
    // executing the else code because it may contain break/continue statements.
    uint end_label = 0;
    if (!MP_PARSE_NODE_IS_NULL(pn_else)) {
        // discard final value of "var", and possible "end" value
        EMIT(pop_top);
        if (end_on_stack) {
            EMIT(pop_top);
        }
        compile_node(comp, pn_else);
        end_label = comp_next_label(comp);
        EMIT_ARG(jump, end_label);
        EMIT_ARG(adjust_stack_size, 1 + end_on_stack);
    }

    EMIT_ARG(label_assign, break_label);

    // discard final value of var that failed the loop condition
    EMIT(pop_top);

    // discard <end> value if it's on the stack
    if (end_on_stack) {
        EMIT(pop_top);
    }

    if (!MP_PARSE_NODE_IS_NULL(pn_else)) {
        EMIT_ARG(label_assign, end_label);
    }
}

STATIC void compile_for_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // this bit optimises: for <x> in range(...), turning it into an explicitly incremented variable
    // this is actually slower, but uses no heap memory
    // for viper it will be much, much faster
    if (/*comp->scope_cur->emit_options == MP_EMIT_OPT_VIPER &&*/ MP_PARSE_NODE_IS_ID(pns->nodes[0]) && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_atom_expr_normal)) {
        mp_parse_node_struct_t *pns_it = (mp_parse_node_struct_t*)pns->nodes[1];
        if (MP_PARSE_NODE_IS_ID(pns_it->nodes[0])
            && MP_PARSE_NODE_LEAF_ARG(pns_it->nodes[0]) == MP_QSTR_range
            && MP_PARSE_NODE_STRUCT_KIND((mp_parse_node_struct_t*)pns_it->nodes[1]) == PN_trailer_paren) {
            mp_parse_node_t pn_range_args = ((mp_parse_node_struct_t*)pns_it->nodes[1])->nodes[0];
            mp_parse_node_t *args;
            int n_args = mp_parse_node_extract_list(&pn_range_args, PN_arglist, &args);
            mp_parse_node_t pn_range_start;
            mp_parse_node_t pn_range_end;
            mp_parse_node_t pn_range_step;
            bool optimize = false;
            if (1 <= n_args && n_args <= 3) {
                optimize = true;
                if (n_args == 1) {
                    pn_range_start = mp_parse_node_new_small_int(0);
                    pn_range_end = args[0];
                    pn_range_step = mp_parse_node_new_small_int(1);
                } else if (n_args == 2) {
                    pn_range_start = args[0];
                    pn_range_end = args[1];
                    pn_range_step = mp_parse_node_new_small_int(1);
                } else {
                    pn_range_start = args[0];
                    pn_range_end = args[1];
                    pn_range_step = args[2];
                    // the step must be a non-zero constant integer to do the optimisation
                    if (!MP_PARSE_NODE_IS_SMALL_INT(pn_range_step)
                        || MP_PARSE_NODE_LEAF_SMALL_INT(pn_range_step) == 0) {
                        optimize = false;
                    }
                }
                // arguments must be able to be compiled as standard expressions
                if (optimize && MP_PARSE_NODE_IS_STRUCT(pn_range_start)) {
                    int k = MP_PARSE_NODE_STRUCT_KIND((mp_parse_node_struct_t*)pn_range_start);
                    if (k == PN_arglist_star || k == PN_arglist_dbl_star || k == PN_argument) {
                        optimize = false;
                    }
                }
                if (optimize && MP_PARSE_NODE_IS_STRUCT(pn_range_end)) {
                    int k = MP_PARSE_NODE_STRUCT_KIND((mp_parse_node_struct_t*)pn_range_end);
                    if (k == PN_arglist_star || k == PN_arglist_dbl_star || k == PN_argument) {
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

    START_BREAK_CONTINUE_BLOCK
    comp->break_label |= MP_EMIT_BREAK_FROM_FOR;

    uint pop_label = comp_next_label(comp);

    compile_node(comp, pns->nodes[1]); // iterator
    EMIT_ARG(get_iter, true);
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

    compile_node(comp, pns->nodes[3]); // else (may be empty)

    EMIT_ARG(label_assign, break_label);
}

STATIC void compile_try_except(compiler_t *comp, mp_parse_node_t pn_body, int n_except, mp_parse_node_t *pn_excepts, mp_parse_node_t pn_else) {
    // setup code
    uint l1 = comp_next_label(comp);
    uint success_label = comp_next_label(comp);

    compile_increase_except_level(comp, l1, MP_EMIT_SETUP_BLOCK_EXCEPT);

    compile_node(comp, pn_body); // body
    EMIT(pop_block);
    EMIT_ARG(jump, success_label); // jump over exception handler

    EMIT_ARG(label_assign, l1); // start of exception handler
    EMIT(start_except_handler);

    // at this point the top of the stack contains the exception instance that was raised

    uint l2 = comp_next_label(comp);

    for (int i = 0; i < n_except; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_excepts[i], PN_try_stmt_except)); // should be
        mp_parse_node_struct_t *pns_except = (mp_parse_node_struct_t*)pn_excepts[i];

        qstr qstr_exception_local = 0;
        uint end_finally_label = comp_next_label(comp);

        if (MP_PARSE_NODE_IS_NULL(pns_except->nodes[0])) {
            // this is a catch all exception handler
            if (i + 1 != n_except) {
                compile_syntax_error(comp, pn_excepts[i], "default 'except' must be last");
                compile_decrease_except_level(comp);
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
            EMIT_ARG(binary_op, MP_BINARY_OP_EXCEPTION_MATCH);
            EMIT_ARG(pop_jump_if, false, end_finally_label);
        }

        // either discard or store the exception instance
        if (qstr_exception_local == 0) {
            EMIT(pop_top);
        } else {
            compile_store_id(comp, qstr_exception_local);
        }

        uint l3 = 0;
        if (qstr_exception_local != 0) {
            l3 = comp_next_label(comp);
            compile_increase_except_level(comp, l3, MP_EMIT_SETUP_BLOCK_FINALLY);
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
            compile_store_id(comp, qstr_exception_local);
            compile_delete_id(comp, qstr_exception_local);

            compile_decrease_except_level(comp);
        }
        EMIT_ARG(jump, l2);
        EMIT_ARG(label_assign, end_finally_label);
        EMIT_ARG(adjust_stack_size, 1); // stack adjust for the exception instance
    }

    compile_decrease_except_level(comp);
    EMIT(end_except_handler);

    EMIT_ARG(label_assign, success_label);
    compile_node(comp, pn_else); // else block, can be null
    EMIT_ARG(label_assign, l2);
}

STATIC void compile_try_finally(compiler_t *comp, mp_parse_node_t pn_body, int n_except, mp_parse_node_t *pn_except, mp_parse_node_t pn_else, mp_parse_node_t pn_finally) {
    uint l_finally_block = comp_next_label(comp);

    compile_increase_except_level(comp, l_finally_block, MP_EMIT_SETUP_BLOCK_FINALLY);

    if (n_except == 0) {
        assert(MP_PARSE_NODE_IS_NULL(pn_else));
        EMIT_ARG(adjust_stack_size, 3); // stack adjust for possible UNWIND_JUMP state
        compile_node(comp, pn_body);
        EMIT_ARG(adjust_stack_size, -3);
    } else {
        compile_try_except(comp, pn_body, n_except, pn_except, pn_else);
    }
    EMIT(pop_block);
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    EMIT_ARG(label_assign, l_finally_block);
    compile_node(comp, pn_finally);

    compile_decrease_except_level(comp);
}

STATIC void compile_try_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should be
    {
        mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[1];
        if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_try_stmt_finally) {
            // just try-finally
            compile_try_finally(comp, pns->nodes[0], 0, NULL, MP_PARSE_NODE_NULL, pns2->nodes[0]);
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_try_stmt_except_and_more) {
            // try-except and possibly else and/or finally
            mp_parse_node_t *pn_excepts;
            int n_except = mp_parse_node_extract_list(&pns2->nodes[0], PN_try_stmt_except_list, &pn_excepts);
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
            int n_except = mp_parse_node_extract_list(&pns->nodes[1], PN_try_stmt_except_list, &pn_excepts);
            compile_try_except(comp, pns->nodes[0], n_except, pn_excepts, MP_PARSE_NODE_NULL);
        }
    }
}

STATIC void compile_with_stmt_helper(compiler_t *comp, int n, mp_parse_node_t *nodes, mp_parse_node_t body) {
    if (n == 0) {
        // no more pre-bits, compile the body of the with
        compile_node(comp, body);
    } else {
        uint l_end = comp_next_label(comp);
        if (MP_PARSE_NODE_IS_STRUCT_KIND(nodes[0], PN_with_item)) {
            // this pre-bit is of the form "a as b"
            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)nodes[0];
            compile_node(comp, pns->nodes[0]);
            compile_increase_except_level(comp, l_end, MP_EMIT_SETUP_BLOCK_WITH);
            c_assign(comp, pns->nodes[1], ASSIGN_STORE);
        } else {
            // this pre-bit is just an expression
            compile_node(comp, nodes[0]);
            compile_increase_except_level(comp, l_end, MP_EMIT_SETUP_BLOCK_WITH);
            EMIT(pop_top);
        }
        // compile additional pre-bits and the body
        compile_with_stmt_helper(comp, n - 1, nodes + 1, body);
        // finish this with block
        EMIT_ARG(with_cleanup, l_end);
        reserve_labels_for_native(comp, 3); // used by native's with_cleanup
        compile_decrease_except_level(comp);
    }
}

STATIC void compile_with_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // get the nodes for the pre-bit of the with (the a as b, c as d, ... bit)
    mp_parse_node_t *nodes;
    int n = mp_parse_node_extract_list(&pns->nodes[0], PN_with_stmt_list, &nodes);
    assert(n > 0);

    // compile in a nested fashion
    compile_with_stmt_helper(comp, n, nodes, pns->nodes[1]);
}

STATIC void compile_yield_from(compiler_t *comp) {
    EMIT_ARG(get_iter, false);
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    EMIT_ARG(yield, MP_EMIT_YIELD_FROM);
}

#if MICROPY_PY_ASYNC_AWAIT
STATIC void compile_await_object_method(compiler_t *comp, qstr method) {
    EMIT_ARG(load_method, method, false);
    EMIT_ARG(call_method, 0, 0, 0);
    compile_yield_from(comp);
}

STATIC void compile_async_for_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // comp->break_label |= MP_EMIT_BREAK_FROM_FOR;

    qstr context = MP_PARSE_NODE_LEAF_ARG(pns->nodes[1]);
    uint while_else_label = comp_next_label(comp);
    uint try_exception_label = comp_next_label(comp);
    uint try_else_label = comp_next_label(comp);
    uint try_finally_label = comp_next_label(comp);

    compile_node(comp, pns->nodes[1]); // iterator
    compile_await_object_method(comp, MP_QSTR___aiter__);
    compile_store_id(comp, context);

    START_BREAK_CONTINUE_BLOCK

    EMIT_ARG(label_assign, continue_label);

    compile_increase_except_level(comp, try_exception_label, MP_EMIT_SETUP_BLOCK_EXCEPT);

    compile_load_id(comp, context);
    compile_await_object_method(comp, MP_QSTR___anext__);
    c_assign(comp, pns->nodes[0], ASSIGN_STORE); // variable
    EMIT(pop_block);
    EMIT_ARG(jump, try_else_label);

    EMIT_ARG(label_assign, try_exception_label);
    EMIT(start_except_handler);
    EMIT(dup_top);
    EMIT_LOAD_GLOBAL(MP_QSTR_StopAsyncIteration);
    EMIT_ARG(binary_op, MP_BINARY_OP_EXCEPTION_MATCH);
    EMIT_ARG(pop_jump_if, false, try_finally_label);
    EMIT(pop_top); // pop exception instance
    EMIT(pop_except);
    EMIT_ARG(jump, while_else_label);

    EMIT_ARG(label_assign, try_finally_label);
    EMIT_ARG(adjust_stack_size, 1); // if we jump here, the exc is on the stack
    compile_decrease_except_level(comp);
    EMIT(end_except_handler);

    EMIT_ARG(label_assign, try_else_label);
    compile_node(comp, pns->nodes[2]); // body

    EMIT_ARG(jump, continue_label);
    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    EMIT_ARG(label_assign, while_else_label);
    compile_node(comp, pns->nodes[3]); // else

    EMIT_ARG(label_assign, break_label);
}

STATIC void compile_async_with_stmt_helper(compiler_t *comp, int n, mp_parse_node_t *nodes, mp_parse_node_t body) {
    if (n == 0) {
        // no more pre-bits, compile the body of the with
        compile_node(comp, body);
    } else {
        uint l_finally_block = comp_next_label(comp);
        uint l_aexit_no_exc = comp_next_label(comp);
        uint l_ret_unwind_jump = comp_next_label(comp);
        uint l_end = comp_next_label(comp);

        if (MP_PARSE_NODE_IS_STRUCT_KIND(nodes[0], PN_with_item)) {
            // this pre-bit is of the form "a as b"
            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)nodes[0];
            compile_node(comp, pns->nodes[0]);
            EMIT(dup_top);
            compile_await_object_method(comp, MP_QSTR___aenter__);
            c_assign(comp, pns->nodes[1], ASSIGN_STORE);
        } else {
            // this pre-bit is just an expression
            compile_node(comp, nodes[0]);
            EMIT(dup_top);
            compile_await_object_method(comp, MP_QSTR___aenter__);
            EMIT(pop_top);
        }

        // To keep the Python stack size down, and because we can't access values on
        // this stack further down than 3 elements (via rot_three), we don't preload
        // __aexit__ (as per normal with) but rather wait until we need it below.

        // Start the try-finally statement
        compile_increase_except_level(comp, l_finally_block, MP_EMIT_SETUP_BLOCK_FINALLY);

        // Compile any additional pre-bits of the "async with", and also the body
        EMIT_ARG(adjust_stack_size, 3); // stack adjust for possible UNWIND_JUMP state
        compile_async_with_stmt_helper(comp, n - 1, nodes + 1, body);
        EMIT_ARG(adjust_stack_size, -3);

        // Finish the "try" block
        EMIT(pop_block);

        // At this point, after the with body has executed, we have 3 cases:
        // 1. no exception, we just fall through to this point; stack: (..., ctx_mgr)
        // 2. exception propagating out, we get to the finally block; stack: (..., ctx_mgr, exc)
        // 3. return or unwind jump, we get to the finally block; stack: (..., ctx_mgr, X, INT)

        // Handle case 1: call __aexit__
        // Stack: (..., ctx_mgr)
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE); // to tell end_finally there's no exception
        EMIT(rot_two);
        EMIT_ARG(jump, l_aexit_no_exc); // jump to code below to call __aexit__

        // Start of "finally" block
        // At this point we have case 2 or 3, we detect which one by the TOS being an exception or not
        EMIT_ARG(label_assign, l_finally_block);

        // Detect if TOS an exception or not
        EMIT(dup_top);
        EMIT_LOAD_GLOBAL(MP_QSTR_Exception);
        EMIT_ARG(binary_op, MP_BINARY_OP_EXCEPTION_MATCH);
        EMIT_ARG(pop_jump_if, false, l_ret_unwind_jump); // if not an exception then we have case 3

        // Handle case 2: call __aexit__ and either swallow or re-raise the exception
        // Stack: (..., ctx_mgr, exc)
        EMIT(dup_top);
        EMIT(rot_three);
        EMIT(rot_two);
        EMIT_ARG(load_method, MP_QSTR___aexit__, false);
        EMIT(rot_three);
        EMIT(rot_three);
        EMIT(dup_top);
        #if MICROPY_CPYTHON_COMPAT
        EMIT_ARG(attr, MP_QSTR___class__, MP_EMIT_ATTR_LOAD); // get type(exc)
        #else
        compile_load_id(comp, MP_QSTR_type);
        EMIT(rot_two);
        EMIT_ARG(call_function, 1, 0, 0); // get type(exc)
        #endif
        EMIT(rot_two);
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE); // dummy traceback value
        // Stack: (..., exc, __aexit__, ctx_mgr, type(exc), exc, None)
        EMIT_ARG(call_method, 3, 0, 0);
        compile_yield_from(comp);
        EMIT_ARG(pop_jump_if, false, l_end);
        EMIT(pop_top); // pop exception
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE); // replace with None to swallow exception
        EMIT_ARG(jump, l_end);
        EMIT_ARG(adjust_stack_size, 2);

        // Handle case 3: call __aexit__
        // Stack: (..., ctx_mgr, X, INT)
        EMIT_ARG(label_assign, l_ret_unwind_jump);
        EMIT(rot_three);
        EMIT(rot_three);
        EMIT_ARG(label_assign, l_aexit_no_exc);
        EMIT_ARG(load_method, MP_QSTR___aexit__, false);
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT(dup_top);
        EMIT(dup_top);
        EMIT_ARG(call_method, 3, 0, 0);
        compile_yield_from(comp);
        EMIT(pop_top);
        EMIT_ARG(adjust_stack_size, -1);

        // End of "finally" block
        // Stack can have one of three configurations:
        // a. (..., None) - from either case 1, or case 2 with swallowed exception
        // b. (..., exc) - from case 2 with re-raised exception
        // c. (..., X, INT) - from case 3
        EMIT_ARG(label_assign, l_end);
        compile_decrease_except_level(comp);
    }
}

STATIC void compile_async_with_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // get the nodes for the pre-bit of the with (the a as b, c as d, ... bit)
    mp_parse_node_t *nodes;
    int n = mp_parse_node_extract_list(&pns->nodes[0], PN_with_stmt_list, &nodes);
    assert(n > 0);

    // compile in a nested fashion
    compile_async_with_stmt_helper(comp, n, nodes, pns->nodes[1]);
}

STATIC void compile_async_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[0]));
    mp_parse_node_struct_t *pns0 = (mp_parse_node_struct_t*)pns->nodes[0];
    if (MP_PARSE_NODE_STRUCT_KIND(pns0) == PN_funcdef) {
        // async def
        compile_funcdef(comp, pns0);
        scope_t *fscope = (scope_t*)pns0->nodes[4];
        fscope->scope_flags |= MP_SCOPE_FLAG_GENERATOR;
    } else if (MP_PARSE_NODE_STRUCT_KIND(pns0) == PN_for_stmt) {
        // async for
        compile_async_for_stmt(comp, pns0);
    } else {
        // async with
        assert(MP_PARSE_NODE_STRUCT_KIND(pns0) == PN_with_stmt);
        compile_async_with_stmt(comp, pns0);
    }
}
#endif

STATIC void compile_expr_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        if (comp->is_repl && comp->scope_cur->kind == SCOPE_MODULE) {
            // for REPL, evaluate then print the expression
            compile_load_id(comp, MP_QSTR___repl_print__);
            compile_node(comp, pns->nodes[0]);
            EMIT_ARG(call_function, 1, 0, 0);
            EMIT(pop_top);

        } else {
            // for non-REPL, evaluate then discard the expression
            if ((MP_PARSE_NODE_IS_LEAF(pns->nodes[0]) && !MP_PARSE_NODE_IS_ID(pns->nodes[0]))
                || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_const_object)) {
                // do nothing with a lonely constant
            } else {
                compile_node(comp, pns->nodes[0]); // just an expression
                EMIT(pop_top); // discard last result since this is a statement and leaves nothing on the stack
            }
        }
    } else if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])) {
        mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
        int kind = MP_PARSE_NODE_STRUCT_KIND(pns1);
        if (kind == PN_expr_stmt_augassign) {
            c_assign(comp, pns->nodes[0], ASSIGN_AUG_LOAD); // lhs load for aug assign
            compile_node(comp, pns1->nodes[1]); // rhs
            assert(MP_PARSE_NODE_IS_TOKEN(pns1->nodes[0]));
            mp_binary_op_t op;
            switch (MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0])) {
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
            EMIT_ARG(binary_op, op);
            c_assign(comp, pns->nodes[0], ASSIGN_AUG_STORE); // lhs store for aug assign
        } else if (kind == PN_expr_stmt_assign_list) {
            int rhs = MP_PARSE_NODE_STRUCT_NUM_NODES(pns1) - 1;
            compile_node(comp, pns1->nodes[rhs]); // rhs
            // following CPython, we store left-most first
            if (rhs > 0) {
                EMIT(dup_top);
            }
            c_assign(comp, pns->nodes[0], ASSIGN_STORE); // lhs store
            for (int i = 0; i < rhs; i++) {
                if (i + 1 < rhs) {
                    EMIT(dup_top);
                }
                c_assign(comp, pns1->nodes[i], ASSIGN_STORE); // middle store
            }
        } else {
        plain_assign:
            #if MICROPY_COMP_DOUBLE_TUPLE_ASSIGN
            if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_testlist_star_expr)
                && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_star_expr)) {
                mp_parse_node_struct_t *pns0 = (mp_parse_node_struct_t*)pns->nodes[0];
                pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
                uint32_t n_pns0 = MP_PARSE_NODE_STRUCT_NUM_NODES(pns0);
                // Can only optimise a tuple-to-tuple assignment when all of the following hold:
                //  - equal number of items in LHS and RHS tuples
                //  - 2 or 3 items in the tuples
                //  - there are no star expressions in the LHS tuple
                if (n_pns0 == MP_PARSE_NODE_STRUCT_NUM_NODES(pns1)
                    && (n_pns0 == 2
                        #if MICROPY_COMP_TRIPLE_TUPLE_ASSIGN
                        || n_pns0 == 3
                        #endif
                        )
                    && !MP_PARSE_NODE_IS_STRUCT_KIND(pns0->nodes[0], PN_star_expr)
                    && !MP_PARSE_NODE_IS_STRUCT_KIND(pns0->nodes[1], PN_star_expr)
                    #if MICROPY_COMP_TRIPLE_TUPLE_ASSIGN
                    && (n_pns0 == 2 || !MP_PARSE_NODE_IS_STRUCT_KIND(pns0->nodes[2], PN_star_expr))
                    #endif
                    ) {
                    // Optimisation for a, b = c, d or a, b, c = d, e, f
                    compile_node(comp, pns1->nodes[0]); // rhs
                    compile_node(comp, pns1->nodes[1]); // rhs
                    #if MICROPY_COMP_TRIPLE_TUPLE_ASSIGN
                    if (n_pns0 == 3) {
                        compile_node(comp, pns1->nodes[2]); // rhs
                        EMIT(rot_three);
                    }
                    #endif
                    EMIT(rot_two);
                    c_assign(comp, pns0->nodes[0], ASSIGN_STORE); // lhs store
                    c_assign(comp, pns0->nodes[1], ASSIGN_STORE); // lhs store
                    #if MICROPY_COMP_TRIPLE_TUPLE_ASSIGN
                    if (n_pns0 == 3) {
                        c_assign(comp, pns0->nodes[2], ASSIGN_STORE); // lhs store
                    }
                    #endif
                    return;
                }
            }
            #endif

            compile_node(comp, pns->nodes[1]); // rhs
            c_assign(comp, pns->nodes[0], ASSIGN_STORE); // lhs store
        }
    } else {
        goto plain_assign;
    }
}

STATIC void compile_test_if_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_test_if_else));
    mp_parse_node_struct_t *pns_test_if_else = (mp_parse_node_struct_t*)pns->nodes[1];

    uint l_fail = comp_next_label(comp);
    uint l_end = comp_next_label(comp);
    c_if_cond(comp, pns_test_if_else->nodes[0], false, l_fail); // condition
    compile_node(comp, pns->nodes[0]); // success value
    EMIT_ARG(jump, l_end);
    EMIT_ARG(label_assign, l_fail);
    EMIT_ARG(adjust_stack_size, -1); // adjust stack size
    compile_node(comp, pns_test_if_else->nodes[1]); // failure value
    EMIT_ARG(label_assign, l_end);
}

STATIC void compile_lambdef(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->pass == MP_PASS_SCOPE) {
        // create a new scope for this lambda
        scope_t *s = scope_new_and_link(comp, SCOPE_LAMBDA, (mp_parse_node_t)pns, comp->scope_cur->emit_options);
        // store the lambda scope so the compiling function (this one) can use it at each pass
        pns->nodes[2] = (mp_parse_node_t)s;
    }

    // get the scope for this lambda
    scope_t *this_scope = (scope_t*)pns->nodes[2];

    // compile the lambda definition
    compile_funcdef_lambdef(comp, this_scope, pns->nodes[0], PN_varargslist);
}

STATIC void compile_or_and_test(compiler_t *comp, mp_parse_node_struct_t *pns) {
    bool cond = MP_PARSE_NODE_STRUCT_KIND(pns) == PN_or_test;
    uint l_end = comp_next_label(comp);
    int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < n; i += 1) {
        compile_node(comp, pns->nodes[i]);
        if (i + 1 < n) {
            EMIT_ARG(jump_if_or_pop, cond, l_end);
        }
    }
    EMIT_ARG(label_assign, l_end);
}

STATIC void compile_not_test_2(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]);
    EMIT_ARG(unary_op, MP_UNARY_OP_NOT);
}

STATIC void compile_comparison(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    bool multi = (num_nodes > 3);
    uint l_fail = 0;
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
            mp_binary_op_t op;
            switch (MP_PARSE_NODE_LEAF_ARG(pns->nodes[i])) {
                case MP_TOKEN_OP_LESS: op = MP_BINARY_OP_LESS; break;
                case MP_TOKEN_OP_MORE: op = MP_BINARY_OP_MORE; break;
                case MP_TOKEN_OP_DBL_EQUAL: op = MP_BINARY_OP_EQUAL; break;
                case MP_TOKEN_OP_LESS_EQUAL: op = MP_BINARY_OP_LESS_EQUAL; break;
                case MP_TOKEN_OP_MORE_EQUAL: op = MP_BINARY_OP_MORE_EQUAL; break;
                case MP_TOKEN_OP_NOT_EQUAL: op = MP_BINARY_OP_NOT_EQUAL; break;
                case MP_TOKEN_KW_IN: default: op = MP_BINARY_OP_IN; break;
            }
            EMIT_ARG(binary_op, op);
        } else {
            assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[i])); // should be
            mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[i];
            int kind = MP_PARSE_NODE_STRUCT_KIND(pns2);
            if (kind == PN_comp_op_not_in) {
                EMIT_ARG(binary_op, MP_BINARY_OP_NOT_IN);
            } else {
                assert(kind == PN_comp_op_is); // should be
                if (MP_PARSE_NODE_IS_NULL(pns2->nodes[0])) {
                    EMIT_ARG(binary_op, MP_BINARY_OP_IS);
                } else {
                    EMIT_ARG(binary_op, MP_BINARY_OP_IS_NOT);
                }
            }
        }
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

STATIC void compile_star_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_syntax_error(comp, (mp_parse_node_t)pns, "*x must be assignment target");
}

STATIC void compile_binary_op(compiler_t *comp, mp_parse_node_struct_t *pns) {
    MP_STATIC_ASSERT(MP_BINARY_OP_OR + PN_xor_expr - PN_expr == MP_BINARY_OP_XOR);
    MP_STATIC_ASSERT(MP_BINARY_OP_OR + PN_and_expr - PN_expr == MP_BINARY_OP_AND);
    mp_binary_op_t binary_op = MP_BINARY_OP_OR + MP_PARSE_NODE_STRUCT_KIND(pns) - PN_expr;
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i < num_nodes; ++i) {
        compile_node(comp, pns->nodes[i]);
        EMIT_ARG(binary_op, binary_op);
    }
}

STATIC void compile_term(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        mp_binary_op_t op;
        mp_token_kind_t tok = MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]);
        switch (tok) {
            case MP_TOKEN_OP_PLUS:      op = MP_BINARY_OP_ADD; break;
            case MP_TOKEN_OP_MINUS:     op = MP_BINARY_OP_SUBTRACT; break;
            case MP_TOKEN_OP_STAR:      op = MP_BINARY_OP_MULTIPLY; break;
            case MP_TOKEN_OP_DBL_SLASH: op = MP_BINARY_OP_FLOOR_DIVIDE; break;
            case MP_TOKEN_OP_SLASH:     op = MP_BINARY_OP_TRUE_DIVIDE; break;
            case MP_TOKEN_OP_PERCENT:   op = MP_BINARY_OP_MODULO; break;
            case MP_TOKEN_OP_DBL_LESS:  op = MP_BINARY_OP_LSHIFT; break;
            default:
                assert(tok == MP_TOKEN_OP_DBL_MORE);
                op = MP_BINARY_OP_RSHIFT;
                break;
        }
        EMIT_ARG(binary_op, op);
    }
}

STATIC void compile_factor_2(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[1]);
    mp_unary_op_t op;
    mp_token_kind_t tok = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
    switch (tok) {
        case MP_TOKEN_OP_PLUS:  op = MP_UNARY_OP_POSITIVE; break;
        case MP_TOKEN_OP_MINUS: op = MP_UNARY_OP_NEGATIVE; break;
        default:
            assert(tok == MP_TOKEN_OP_TILDE);
            op = MP_UNARY_OP_INVERT;
            break;
    }
    EMIT_ARG(unary_op, op);
}

STATIC void compile_atom_expr_normal(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // compile the subject of the expression
    compile_node(comp, pns->nodes[0]);

    // compile_atom_expr_await may call us with a NULL node
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        return;
    }

    // get the array of trailers (known to be an array of PARSE_NODE_STRUCT)
    size_t num_trail = 1;
    mp_parse_node_struct_t **pns_trail = (mp_parse_node_struct_t**)&pns->nodes[1];
    if (MP_PARSE_NODE_STRUCT_KIND(pns_trail[0]) == PN_atom_expr_trailers) {
        num_trail = MP_PARSE_NODE_STRUCT_NUM_NODES(pns_trail[0]);
        pns_trail = (mp_parse_node_struct_t**)&pns_trail[0]->nodes[0];
    }

    // the current index into the array of trailers
    size_t i = 0;

    // handle special super() call
    if (comp->scope_cur->kind == SCOPE_FUNCTION
        && MP_PARSE_NODE_IS_ID(pns->nodes[0])
        && MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]) == MP_QSTR_super
        && MP_PARSE_NODE_STRUCT_KIND(pns_trail[0]) == PN_trailer_paren
        && MP_PARSE_NODE_IS_NULL(pns_trail[0]->nodes[0])) {
        // at this point we have matched "super()" within a function

        // load the class for super to search for a parent
        compile_load_id(comp, MP_QSTR___class__);

        // look for first argument to function (assumes it's "self")
        bool found = false;
        id_info_t *id = &comp->scope_cur->id_info[0];
        for (size_t n = comp->scope_cur->id_info_len; n > 0; --n, ++id) {
            if (id->flags & ID_FLAG_IS_PARAM) {
                // first argument found; load it
                compile_load_id(comp, id->qst);
                found = true;
                break;
            }
        }
        if (!found) {
            compile_syntax_error(comp, (mp_parse_node_t)pns_trail[0],
                "super() can't find self"); // really a TypeError
            return;
        }

        if (num_trail >= 3
            && MP_PARSE_NODE_STRUCT_KIND(pns_trail[1]) == PN_trailer_period
            && MP_PARSE_NODE_STRUCT_KIND(pns_trail[2]) == PN_trailer_paren) {
            // optimisation for method calls super().f(...), to eliminate heap allocation
            mp_parse_node_struct_t *pns_period = pns_trail[1];
            mp_parse_node_struct_t *pns_paren = pns_trail[2];
            EMIT_ARG(load_method, MP_PARSE_NODE_LEAF_ARG(pns_period->nodes[0]), true);
            compile_trailer_paren_helper(comp, pns_paren->nodes[0], true, 0);
            i = 3;
        } else {
            // a super() call
            EMIT_ARG(call_function, 2, 0, 0);
            i = 1;
        }
    }

    // compile the remaining trailers
    for (; i < num_trail; i++) {
        if (i + 1 < num_trail
            && MP_PARSE_NODE_STRUCT_KIND(pns_trail[i]) == PN_trailer_period
            && MP_PARSE_NODE_STRUCT_KIND(pns_trail[i + 1]) == PN_trailer_paren) {
            // optimisation for method calls a.f(...), following PyPy
            mp_parse_node_struct_t *pns_period = pns_trail[i];
            mp_parse_node_struct_t *pns_paren = pns_trail[i + 1];
            EMIT_ARG(load_method, MP_PARSE_NODE_LEAF_ARG(pns_period->nodes[0]), false);
            compile_trailer_paren_helper(comp, pns_paren->nodes[0], true, 0);
            i += 1;
        } else {
            // node is one of: trailer_paren, trailer_bracket, trailer_period
            compile_node(comp, (mp_parse_node_t)pns_trail[i]);
        }
    }
}

STATIC void compile_power(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_generic_all_nodes(comp, pns); // 2 nodes, arguments of power
    EMIT_ARG(binary_op, MP_BINARY_OP_POWER);
}

STATIC void compile_trailer_paren_helper(compiler_t *comp, mp_parse_node_t pn_arglist, bool is_method_call, int n_positional_extra) {
    // function to call is on top of stack

    // get the list of arguments
    mp_parse_node_t *args;
    int n_args = mp_parse_node_extract_list(&pn_arglist, PN_arglist, &args);

    // compile the arguments
    // Rather than calling compile_node on the list, we go through the list of args
    // explicitly here so that we can count the number of arguments and give sensible
    // error messages.
    int n_positional = n_positional_extra;
    uint n_keyword = 0;
    uint star_flags = 0;
    mp_parse_node_struct_t *star_args_node = NULL, *dblstar_args_node = NULL;
    for (int i = 0; i < n_args; i++) {
        if (MP_PARSE_NODE_IS_STRUCT(args[i])) {
            mp_parse_node_struct_t *pns_arg = (mp_parse_node_struct_t*)args[i];
            if (MP_PARSE_NODE_STRUCT_KIND(pns_arg) == PN_arglist_star) {
                if (star_flags & MP_EMIT_STAR_FLAG_SINGLE) {
                    compile_syntax_error(comp, (mp_parse_node_t)pns_arg, "can't have multiple *x");
                    return;
                }
                star_flags |= MP_EMIT_STAR_FLAG_SINGLE;
                star_args_node = pns_arg;
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns_arg) == PN_arglist_dbl_star) {
                if (star_flags & MP_EMIT_STAR_FLAG_DOUBLE) {
                    compile_syntax_error(comp, (mp_parse_node_t)pns_arg, "can't have multiple **x");
                    return;
                }
                star_flags |= MP_EMIT_STAR_FLAG_DOUBLE;
                dblstar_args_node = pns_arg;
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns_arg) == PN_argument) {
                if (!MP_PARSE_NODE_IS_STRUCT_KIND(pns_arg->nodes[1], PN_comp_for)) {
                    if (!MP_PARSE_NODE_IS_ID(pns_arg->nodes[0])) {
                        compile_syntax_error(comp, (mp_parse_node_t)pns_arg, "LHS of keyword arg must be an id");
                        return;
                    }
                    EMIT_ARG(load_const_str, MP_PARSE_NODE_LEAF_ARG(pns_arg->nodes[0]));
                    compile_node(comp, pns_arg->nodes[1]);
                    n_keyword += 1;
                } else {
                    compile_comprehension(comp, pns_arg, SCOPE_GEN_EXPR);
                    n_positional++;
                }
            } else {
                goto normal_argument;
            }
        } else {
            normal_argument:
            if (star_flags) {
                compile_syntax_error(comp, args[i], "non-keyword arg after */**");
                return;
            }
            if (n_keyword > 0) {
                compile_syntax_error(comp, args[i], "non-keyword arg after keyword arg");
                return;
            }
            compile_node(comp, args[i]);
            n_positional++;
        }
    }

    // compile the star/double-star arguments if we had them
    // if we had one but not the other then we load "null" as a place holder
    if (star_flags != 0) {
        if (star_args_node == NULL) {
            EMIT(load_null);
        } else {
            compile_node(comp, star_args_node->nodes[0]);
        }
        if (dblstar_args_node == NULL) {
            EMIT(load_null);
        } else {
            compile_node(comp, dblstar_args_node->nodes[0]);
        }
    }

    // emit the function/method call
    if (is_method_call) {
        EMIT_ARG(call_method, n_positional, n_keyword, star_flags);
    } else {
        EMIT_ARG(call_function, n_positional, n_keyword, star_flags);
    }
}

// pns needs to have 2 nodes, first is lhs of comprehension, second is PN_comp_for node
STATIC void compile_comprehension(compiler_t *comp, mp_parse_node_struct_t *pns, scope_kind_t kind) {
    assert(MP_PARSE_NODE_STRUCT_NUM_NODES(pns) == 2);
    assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_comp_for));
    mp_parse_node_struct_t *pns_comp_for = (mp_parse_node_struct_t*)pns->nodes[1];

    if (comp->pass == MP_PASS_SCOPE) {
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
    if (kind == SCOPE_GEN_EXPR) {
        EMIT_ARG(get_iter, false);
    }
    EMIT_ARG(call_function, 1, 0, 0);
}

STATIC void compile_atom_paren(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // an empty tuple
        c_tuple(comp, MP_PARSE_NODE_NULL, NULL);
    } else {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp));
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
    }
}

STATIC void compile_atom_bracket(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // empty list
        EMIT_ARG(build, 0, MP_EMIT_BUILD_LIST);
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
        mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[0];
        if (MP_PARSE_NODE_IS_STRUCT(pns2->nodes[1])) {
            mp_parse_node_struct_t *pns3 = (mp_parse_node_struct_t*)pns2->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns3) == PN_testlist_comp_3b) {
                // list of one item, with trailing comma
                assert(MP_PARSE_NODE_IS_NULL(pns3->nodes[0]));
                compile_node(comp, pns2->nodes[0]);
                EMIT_ARG(build, 1, MP_EMIT_BUILD_LIST);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns3) == PN_testlist_comp_3c) {
                // list of many items
                compile_node(comp, pns2->nodes[0]);
                compile_generic_all_nodes(comp, pns3);
                EMIT_ARG(build, 1 + MP_PARSE_NODE_STRUCT_NUM_NODES(pns3), MP_EMIT_BUILD_LIST);
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
            EMIT_ARG(build, 2, MP_EMIT_BUILD_LIST);
        }
    } else {
        // list with 1 item
        compile_node(comp, pns->nodes[0]);
        EMIT_ARG(build, 1, MP_EMIT_BUILD_LIST);
    }
}

STATIC void compile_atom_brace(compiler_t *comp, mp_parse_node_struct_t *pns) {
    mp_parse_node_t pn = pns->nodes[0];
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        // empty dict
        EMIT_ARG(build, 0, MP_EMIT_BUILD_MAP);
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_dictorsetmaker_item) {
            // dict with one element
            EMIT_ARG(build, 1, MP_EMIT_BUILD_MAP);
            compile_node(comp, pn);
            EMIT(store_map);
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_dictorsetmaker) {
            assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should succeed
            mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
            if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_dictorsetmaker_list) {
                // dict/set with multiple elements

                // get tail elements (2nd, 3rd, ...)
                mp_parse_node_t *nodes;
                int n = mp_parse_node_extract_list(&pns1->nodes[0], PN_dictorsetmaker_list2, &nodes);

                // first element sets whether it's a dict or set
                bool is_dict;
                if (!MICROPY_PY_BUILTINS_SET || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_dictorsetmaker_item)) {
                    // a dictionary
                    EMIT_ARG(build, 1 + n, MP_EMIT_BUILD_MAP);
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
                    mp_parse_node_t pn_i = nodes[i];
                    bool is_key_value = MP_PARSE_NODE_IS_STRUCT_KIND(pn_i, PN_dictorsetmaker_item);
                    compile_node(comp, pn_i);
                    if (is_dict) {
                        if (!is_key_value) {
                            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                                compile_syntax_error(comp, (mp_parse_node_t)pns, "invalid syntax");
                            } else {
                                compile_syntax_error(comp, (mp_parse_node_t)pns, "expecting key:value for dict");
                            }
                            return;
                        }
                        EMIT(store_map);
                    } else {
                        if (is_key_value) {
                            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                                compile_syntax_error(comp, (mp_parse_node_t)pns, "invalid syntax");
                            } else {
                                compile_syntax_error(comp, (mp_parse_node_t)pns, "expecting just a value for set");
                            }
                            return;
                        }
                    }
                }

                #if MICROPY_PY_BUILTINS_SET
                // if it's a set, build it
                if (!is_dict) {
                    EMIT_ARG(build, 1 + n, MP_EMIT_BUILD_SET);
                }
                #endif
            } else {
                assert(MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_comp_for); // should be
                // dict/set comprehension
                if (!MICROPY_PY_BUILTINS_SET || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_dictorsetmaker_item)) {
                    // a dictionary comprehension
                    compile_comprehension(comp, pns, SCOPE_DICT_COMP);
                } else {
                    // a set comprehension
                    compile_comprehension(comp, pns, SCOPE_SET_COMP);
                }
            }
        } else {
            // set with one element
            goto set_with_one_element;
        }
    } else {
        // set with one element
        set_with_one_element:
        #if MICROPY_PY_BUILTINS_SET
        compile_node(comp, pn);
        EMIT_ARG(build, 1, MP_EMIT_BUILD_SET);
        #else
        assert(0);
        #endif
    }
}

STATIC void compile_trailer_paren(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_trailer_paren_helper(comp, pns->nodes[0], false, 0);
}

STATIC void compile_trailer_bracket(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // object who's index we want is on top of stack
    compile_node(comp, pns->nodes[0]); // the index
    EMIT_ARG(subscr, MP_EMIT_SUBSCR_LOAD);
}

STATIC void compile_trailer_period(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // object who's attribute we want is on top of stack
    EMIT_ARG(attr, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]), MP_EMIT_ATTR_LOAD); // attribute to get
}

#if MICROPY_PY_BUILTINS_SLICE
STATIC void compile_subscript(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_2) {
        compile_node(comp, pns->nodes[0]); // start of slice
        assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should always be
        pns = (mp_parse_node_struct_t*)pns->nodes[1];
    } else {
        // pns is a PN_subscript_3, load None for start of slice
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    }

    assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_3); // should always be
    mp_parse_node_t pn = pns->nodes[0];
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        // [?:]
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT_ARG(build, 2, MP_EMIT_BUILD_SLICE);
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_3c) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
            pn = pns->nodes[0];
            if (MP_PARSE_NODE_IS_NULL(pn)) {
                // [?::]
                EMIT_ARG(build, 2, MP_EMIT_BUILD_SLICE);
            } else {
                // [?::x]
                compile_node(comp, pn);
                EMIT_ARG(build, 3, MP_EMIT_BUILD_SLICE);
            }
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_subscript_3d) {
            compile_node(comp, pns->nodes[0]);
            assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should always be
            pns = (mp_parse_node_struct_t*)pns->nodes[1];
            assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_sliceop); // should always be
            if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                // [?:x:]
                EMIT_ARG(build, 2, MP_EMIT_BUILD_SLICE);
            } else {
                // [?:x:x]
                compile_node(comp, pns->nodes[0]);
                EMIT_ARG(build, 3, MP_EMIT_BUILD_SLICE);
            }
        } else {
            // [?:x]
            compile_node(comp, pn);
            EMIT_ARG(build, 2, MP_EMIT_BUILD_SLICE);
        }
    } else {
        // [?:x]
        compile_node(comp, pn);
        EMIT_ARG(build, 2, MP_EMIT_BUILD_SLICE);
    }
}
#endif // MICROPY_PY_BUILTINS_SLICE

STATIC void compile_dictorsetmaker_item(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // if this is called then we are compiling a dict key:value pair
    compile_node(comp, pns->nodes[1]); // value
    compile_node(comp, pns->nodes[0]); // key
}

STATIC void compile_classdef(compiler_t *comp, mp_parse_node_struct_t *pns) {
    qstr cname = compile_classdef_helper(comp, pns, comp->scope_cur->emit_options);
    // store class object into class name
    compile_store_id(comp, cname);
}

STATIC void compile_yield_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION && comp->scope_cur->kind != SCOPE_LAMBDA) {
        compile_syntax_error(comp, (mp_parse_node_t)pns, "'yield' outside function");
        return;
    }
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT_ARG(yield, MP_EMIT_YIELD_VALUE);
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_yield_arg_from)) {
        pns = (mp_parse_node_struct_t*)pns->nodes[0];
        compile_node(comp, pns->nodes[0]);
        compile_yield_from(comp);
    } else {
        compile_node(comp, pns->nodes[0]);
        EMIT_ARG(yield, MP_EMIT_YIELD_VALUE);
    }
}

#if MICROPY_PY_ASYNC_AWAIT
STATIC void compile_atom_expr_await(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION && comp->scope_cur->kind != SCOPE_LAMBDA) {
        compile_syntax_error(comp, (mp_parse_node_t)pns, "'await' outside function");
        return;
    }
    compile_atom_expr_normal(comp, pns);
    compile_yield_from(comp);
}
#endif

STATIC mp_obj_t get_const_object(mp_parse_node_struct_t *pns) {
    #if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
    // nodes are 32-bit pointers, but need to extract 64-bit object
    return (uint64_t)pns->nodes[0] | ((uint64_t)pns->nodes[1] << 32);
    #else
    return (mp_obj_t)pns->nodes[0];
    #endif
}

STATIC void compile_const_object(compiler_t *comp, mp_parse_node_struct_t *pns) {
    EMIT_ARG(load_const_obj, get_const_object(pns));
}

typedef void (*compile_function_t)(compiler_t*, mp_parse_node_struct_t*);
STATIC const compile_function_t compile_function[] = {
// only define rules with a compile function
#define c(f) compile_##f
#define DEF_RULE(rule, comp, kind, ...) comp,
#define DEF_RULE_NC(rule, kind, ...)
#include "py/grammar.h"
#undef c
#undef DEF_RULE
#undef DEF_RULE_NC
    compile_const_object,
};

STATIC void compile_node(compiler_t *comp, mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        // pass
    } else if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        mp_int_t arg = MP_PARSE_NODE_LEAF_SMALL_INT(pn);
        #if MICROPY_DYNAMIC_COMPILER
        mp_uint_t sign_mask = -((mp_uint_t)1 << (mp_dynamic_compiler.small_int_bits - 1));
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
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        uintptr_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
        switch (MP_PARSE_NODE_LEAF_KIND(pn)) {
            case MP_PARSE_NODE_ID: compile_load_id(comp, arg); break;
            case MP_PARSE_NODE_STRING: EMIT_ARG(load_const_str, arg); break;
            case MP_PARSE_NODE_BYTES:
                // only create and load the actual bytes object on the last pass
                if (comp->pass != MP_PASS_EMIT) {
                    EMIT_ARG(load_const_obj, mp_const_none);
                } else {
                    size_t len;
                    const byte *data = qstr_data(arg, &len);
                    EMIT_ARG(load_const_obj, mp_obj_new_bytes(data, len));
                }
                break;
            case MP_PARSE_NODE_TOKEN: default:
                if (arg == MP_TOKEN_NEWLINE) {
                    // this can occur when file_input lets through a NEWLINE (eg if file starts with a newline)
                    // or when single_input lets through a NEWLINE (user enters a blank line)
                    // do nothing
                } else {
                  EMIT_ARG(load_const_tok, arg);
                }
                break;
        }
    } else {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        EMIT_ARG(set_source_line, pns->source_line);
        assert(MP_PARSE_NODE_STRUCT_KIND(pns) <= PN_const_object);
        compile_function_t f = compile_function[MP_PARSE_NODE_STRUCT_KIND(pns)];
        f(comp, pns);
    }
}

#if MICROPY_EMIT_NATIVE
STATIC int compile_viper_type_annotation(compiler_t *comp, mp_parse_node_t pn_annotation) {
    int native_type = MP_NATIVE_TYPE_OBJ;
    if (MP_PARSE_NODE_IS_NULL(pn_annotation)) {
        // No annotation, type defaults to object
    } else if (MP_PARSE_NODE_IS_ID(pn_annotation)) {
        qstr type_name = MP_PARSE_NODE_LEAF_ARG(pn_annotation);
        native_type = mp_native_type_from_qstr(type_name);
        if (native_type < 0) {
            comp->compile_error = mp_obj_new_exception_msg_varg(&mp_type_ViperTypeError, "unknown type '%q'", type_name);
            native_type = 0;
        }
    } else {
        compile_syntax_error(comp, pn_annotation, "annotation must be an identifier");
    }
    return native_type;
}
#endif

STATIC void compile_scope_func_lambda_param(compiler_t *comp, mp_parse_node_t pn, pn_kind_t pn_name, pn_kind_t pn_star, pn_kind_t pn_dbl_star) {
    // check that **kw is last
    if ((comp->scope_cur->scope_flags & MP_SCOPE_FLAG_VARKEYWORDS) != 0) {
        compile_syntax_error(comp, pn, "invalid syntax");
        return;
    }

    qstr param_name = MP_QSTR_NULL;
    uint param_flag = ID_FLAG_IS_PARAM;
    mp_parse_node_struct_t *pns = NULL;
    if (MP_PARSE_NODE_IS_ID(pn)) {
        param_name = MP_PARSE_NODE_LEAF_ARG(pn);
        if (comp->have_star) {
            // comes after a star, so counts as a keyword-only parameter
            comp->scope_cur->num_kwonly_args += 1;
        } else {
            // comes before a star, so counts as a positional parameter
            comp->scope_cur->num_pos_args += 1;
        }
    } else {
        assert(MP_PARSE_NODE_IS_STRUCT(pn));
        pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == pn_name) {
            // named parameter with possible annotation
            param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            if (comp->have_star) {
                // comes after a star, so counts as a keyword-only parameter
                comp->scope_cur->num_kwonly_args += 1;
            } else {
                // comes before a star, so counts as a positional parameter
                comp->scope_cur->num_pos_args += 1;
            }
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == pn_star) {
            if (comp->have_star) {
                // more than one star
                compile_syntax_error(comp, pn, "invalid syntax");
                return;
            }
            comp->have_star = true;
            param_flag = ID_FLAG_IS_PARAM | ID_FLAG_IS_STAR_PARAM;
            if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                // bare star
                // TODO see http://www.python.org/dev/peps/pep-3102/
                //assert(comp->scope_cur->num_dict_params == 0);
                pns = NULL;
            } else if (MP_PARSE_NODE_IS_ID(pns->nodes[0])) {
                // named star
                comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARARGS;
                param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                pns = NULL;
            } else {
                assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_tfpdef)); // should be
                // named star with possible annotation
                comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARARGS;
                pns = (mp_parse_node_struct_t*)pns->nodes[0];
                param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            }
        } else {
            // double star with possible annotation
            assert(MP_PARSE_NODE_STRUCT_KIND(pns) == pn_dbl_star); // should be
            param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            param_flag = ID_FLAG_IS_PARAM | ID_FLAG_IS_DBL_STAR_PARAM;
            comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARKEYWORDS;
        }
    }

    if (param_name != MP_QSTR_NULL) {
        bool added;
        id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, param_name, &added);
        if (!added) {
            compile_syntax_error(comp, pn, "name reused for argument");
            return;
        }
        id_info->kind = ID_INFO_KIND_LOCAL;
        id_info->flags = param_flag;

        #if MICROPY_EMIT_NATIVE
        if (comp->scope_cur->emit_options == MP_EMIT_OPT_VIPER && pn_name == PN_typedargslist_name && pns != NULL) {
            id_info->flags |= compile_viper_type_annotation(comp, pns->nodes[1]) << ID_FLAG_VIPER_TYPE_POS;
        }
        #else
        (void)pns;
        #endif
    }
}

STATIC void compile_scope_func_param(compiler_t *comp, mp_parse_node_t pn) {
    compile_scope_func_lambda_param(comp, pn, PN_typedargslist_name, PN_typedargslist_star, PN_typedargslist_dbl_star);
}

STATIC void compile_scope_lambda_param(compiler_t *comp, mp_parse_node_t pn) {
    compile_scope_func_lambda_param(comp, pn, PN_varargslist_name, PN_varargslist_star, PN_varargslist_dbl_star);
}

STATIC void compile_scope_comp_iter(compiler_t *comp, mp_parse_node_struct_t *pns_comp_for, mp_parse_node_t pn_inner_expr, int for_depth) {
    uint l_top = comp_next_label(comp);
    uint l_end = comp_next_label(comp);
    EMIT_ARG(label_assign, l_top);
    EMIT_ARG(for_iter, l_end);
    c_assign(comp, pns_comp_for->nodes[0], ASSIGN_STORE);
    mp_parse_node_t pn_iter = pns_comp_for->nodes[2];

    tail_recursion:
    if (MP_PARSE_NODE_IS_NULL(pn_iter)) {
        // no more nested if/for; compile inner expression
        compile_node(comp, pn_inner_expr);
        if (comp->scope_cur->kind == SCOPE_GEN_EXPR) {
            EMIT_ARG(yield, MP_EMIT_YIELD_VALUE);
            EMIT(pop_top);
        } else {
            EMIT_ARG(store_comp, comp->scope_cur->kind, 4 * for_depth + 5);
        }
    } else if (MP_PARSE_NODE_STRUCT_KIND((mp_parse_node_struct_t*)pn_iter) == PN_comp_if) {
        // if condition
        mp_parse_node_struct_t *pns_comp_if = (mp_parse_node_struct_t*)pn_iter;
        c_if_cond(comp, pns_comp_if->nodes[0], false, l_top);
        pn_iter = pns_comp_if->nodes[1];
        goto tail_recursion;
    } else {
        assert(MP_PARSE_NODE_STRUCT_KIND((mp_parse_node_struct_t*)pn_iter) == PN_comp_for); // should be
        // for loop
        mp_parse_node_struct_t *pns_comp_for2 = (mp_parse_node_struct_t*)pn_iter;
        compile_node(comp, pns_comp_for2->nodes[1]);
        EMIT_ARG(get_iter, true);
        compile_scope_comp_iter(comp, pns_comp_for2, pn_inner_expr, for_depth + 1);
    }

    EMIT_ARG(jump, l_top);
    EMIT_ARG(label_assign, l_end);
    EMIT(for_iter_end);
}

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
            || (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_const_object)
                && MP_OBJ_IS_STR(get_const_object((mp_parse_node_struct_t*)pns->nodes[0])))) {
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

STATIC void compile_scope(compiler_t *comp, scope_t *scope, pass_kind_t pass) {
    comp->pass = pass;
    comp->scope_cur = scope;
    comp->next_label = 0;
    EMIT_ARG(start_pass, pass, scope);
    reserve_labels_for_native(comp, 6); // used by native's start_pass

    if (comp->pass == MP_PASS_SCOPE) {
        // reset maximum stack sizes in scope
        // they will be computed in this first pass
        scope->stack_size = 0;
        scope->exc_stack_size = 0;
    }

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
        if (comp->pass == MP_PASS_SCOPE) {
            comp->have_star = false;
            apply_to_single_or_list(comp, pns->nodes[1], PN_typedargslist, compile_scope_func_param);

            #if MICROPY_EMIT_NATIVE
            if (scope->emit_options == MP_EMIT_OPT_VIPER) {
                // Compile return type; pns->nodes[2] is return/whole function annotation
                scope->scope_flags |= compile_viper_type_annotation(comp, pns->nodes[2]) << MP_SCOPE_FLAG_VIPERRET_POS;
            }
            #endif // MICROPY_EMIT_NATIVE
        }

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
        if (comp->pass == MP_PASS_SCOPE) {
            comp->have_star = false;
            apply_to_single_or_list(comp, pns->nodes[0], PN_varargslist, compile_scope_lambda_param);
        }

        compile_node(comp, pns->nodes[1]); // 1 is lambda body

        // if the lambda is a generator, then we return None, not the result of the expression of the lambda
        if (scope->scope_flags & MP_SCOPE_FLAG_GENERATOR) {
            EMIT(pop_top);
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        }
        EMIT(return_value);
    } else if (scope->kind == SCOPE_LIST_COMP || scope->kind == SCOPE_DICT_COMP || scope->kind == SCOPE_SET_COMP || scope->kind == SCOPE_GEN_EXPR) {
        // a bit of a hack at the moment

        assert(MP_PARSE_NODE_IS_STRUCT(scope->pn));
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
        assert(MP_PARSE_NODE_STRUCT_NUM_NODES(pns) == 2);
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_comp_for));
        mp_parse_node_struct_t *pns_comp_for = (mp_parse_node_struct_t*)pns->nodes[1];

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
            EMIT_ARG(build, 0, MP_EMIT_BUILD_LIST);
        } else if (scope->kind == SCOPE_DICT_COMP) {
            EMIT_ARG(build, 0, MP_EMIT_BUILD_MAP);
        #if MICROPY_PY_BUILTINS_SET
        } else if (scope->kind == SCOPE_SET_COMP) {
            EMIT_ARG(build, 0, MP_EMIT_BUILD_SET);
        #endif
        }

        // There are 4 slots on the stack for the iterator, and the first one is
        // NULL to indicate that the second one points to the iterator object.
        if (scope->kind == SCOPE_GEN_EXPR) {
            MP_STATIC_ASSERT(MP_OBJ_ITER_BUF_NSLOTS == 4);
            EMIT(load_null);
            compile_load_id(comp, qstr_arg);
            EMIT(load_null);
            EMIT(load_null);
        } else {
            compile_load_id(comp, qstr_arg);
            EMIT_ARG(get_iter, true);
        }

        compile_scope_comp_iter(comp, pns_comp_for, pns->nodes[0], 0);

        if (scope->kind == SCOPE_GEN_EXPR) {
            EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        }
        EMIT(return_value);
    } else {
        assert(scope->kind == SCOPE_CLASS);
        assert(MP_PARSE_NODE_IS_STRUCT(scope->pn));
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
        assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_classdef);

        if (comp->pass == MP_PASS_SCOPE) {
            bool added;
            id_info_t *id_info = scope_find_or_add_id(scope, MP_QSTR___class__, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
        }

        compile_load_id(comp, MP_QSTR___name__);
        compile_store_id(comp, MP_QSTR___module__);
        EMIT_ARG(load_const_str, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0])); // 0 is class name
        compile_store_id(comp, MP_QSTR___qualname__);

        check_for_doc_string(comp, pns->nodes[2]);
        compile_node(comp, pns->nodes[2]); // 2 is class body

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

#if MICROPY_EMIT_INLINE_ASM
// requires 3 passes: SCOPE, CODE_SIZE, EMIT
STATIC void compile_scope_inline_asm(compiler_t *comp, scope_t *scope, pass_kind_t pass) {
    comp->pass = pass;
    comp->scope_cur = scope;
    comp->next_label = 0;

    if (scope->kind != SCOPE_FUNCTION) {
        compile_syntax_error(comp, MP_PARSE_NODE_NULL, "inline assembler must be a function");
        return;
    }

    if (comp->pass > MP_PASS_SCOPE) {
        EMIT_INLINE_ASM_ARG(start_pass, comp->pass, &comp->compile_error);
    }

    // get the function definition parse node
    assert(MP_PARSE_NODE_IS_STRUCT(scope->pn));
    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
    assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_funcdef);

    //qstr f_id = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]); // function name

    // parameters are in pns->nodes[1]
    if (comp->pass == MP_PASS_CODE_SIZE) {
        mp_parse_node_t *pn_params;
        int n_params = mp_parse_node_extract_list(&pns->nodes[1], PN_typedargslist, &pn_params);
        scope->num_pos_args = EMIT_INLINE_ASM_ARG(count_params, n_params, pn_params);
        if (comp->compile_error != MP_OBJ_NULL) {
            goto inline_asm_error;
        }
    }

    // pns->nodes[2] is function return annotation
    mp_uint_t type_sig = MP_NATIVE_TYPE_INT;
    mp_parse_node_t pn_annotation = pns->nodes[2];
    if (!MP_PARSE_NODE_IS_NULL(pn_annotation)) {
        // nodes[2] can be null or a test-expr
        if (MP_PARSE_NODE_IS_ID(pn_annotation)) {
            qstr ret_type = MP_PARSE_NODE_LEAF_ARG(pn_annotation);
            switch (ret_type) {
                case MP_QSTR_object: type_sig = MP_NATIVE_TYPE_OBJ; break;
                case MP_QSTR_bool: type_sig = MP_NATIVE_TYPE_BOOL; break;
                case MP_QSTR_int: type_sig = MP_NATIVE_TYPE_INT; break;
                case MP_QSTR_uint: type_sig = MP_NATIVE_TYPE_UINT; break;
                default: compile_syntax_error(comp, pn_annotation, "unknown type"); return;
            }
        } else {
            compile_syntax_error(comp, pn_annotation, "return annotation must be an identifier");
        }
    }

    mp_parse_node_t pn_body = pns->nodes[3]; // body
    mp_parse_node_t *nodes;
    int num = mp_parse_node_extract_list(&pn_body, PN_suite_block_stmts, &nodes);

    for (int i = 0; i < num; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT(nodes[i]));
        mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)nodes[i];
        if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_pass_stmt) {
            // no instructions
            continue;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) != PN_expr_stmt) {
            // not an instruction; error
        not_an_instruction:
            compile_syntax_error(comp, nodes[i], "expecting an assembler instruction");
            return;
        }

        // check structure of parse node
        assert(MP_PARSE_NODE_IS_STRUCT(pns2->nodes[0]));
        if (!MP_PARSE_NODE_IS_NULL(pns2->nodes[1])) {
            goto not_an_instruction;
        }
        pns2 = (mp_parse_node_struct_t*)pns2->nodes[0];
        if (MP_PARSE_NODE_STRUCT_KIND(pns2) != PN_atom_expr_normal) {
            goto not_an_instruction;
        }
        if (!MP_PARSE_NODE_IS_ID(pns2->nodes[0])) {
            goto not_an_instruction;
        }
        if (!MP_PARSE_NODE_IS_STRUCT_KIND(pns2->nodes[1], PN_trailer_paren)) {
            goto not_an_instruction;
        }

        // parse node looks like an instruction
        // get instruction name and args
        qstr op = MP_PARSE_NODE_LEAF_ARG(pns2->nodes[0]);
        pns2 = (mp_parse_node_struct_t*)pns2->nodes[1]; // PN_trailer_paren
        mp_parse_node_t *pn_arg;
        int n_args = mp_parse_node_extract_list(&pns2->nodes[0], PN_arglist, &pn_arg);

        // emit instructions
        if (op == MP_QSTR_label) {
            if (!(n_args == 1 && MP_PARSE_NODE_IS_ID(pn_arg[0]))) {
                compile_syntax_error(comp, nodes[i], "'label' requires 1 argument");
                return;
            }
            uint lab = comp_next_label(comp);
            if (pass > MP_PASS_SCOPE) {
                if (!EMIT_INLINE_ASM_ARG(label, lab, MP_PARSE_NODE_LEAF_ARG(pn_arg[0]))) {
                    compile_syntax_error(comp, nodes[i], "label redefined");
                    return;
                }
            }
        } else if (op == MP_QSTR_align) {
            if (!(n_args == 1 && MP_PARSE_NODE_IS_SMALL_INT(pn_arg[0]))) {
                compile_syntax_error(comp, nodes[i], "'align' requires 1 argument");
                return;
            }
            if (pass > MP_PASS_SCOPE) {
                mp_asm_base_align((mp_asm_base_t*)comp->emit_inline_asm,
                    MP_PARSE_NODE_LEAF_SMALL_INT(pn_arg[0]));
            }
        } else if (op == MP_QSTR_data) {
            if (!(n_args >= 2 && MP_PARSE_NODE_IS_SMALL_INT(pn_arg[0]))) {
                compile_syntax_error(comp, nodes[i], "'data' requires at least 2 arguments");
                return;
            }
            if (pass > MP_PASS_SCOPE) {
                mp_int_t bytesize = MP_PARSE_NODE_LEAF_SMALL_INT(pn_arg[0]);
                for (uint j = 1; j < n_args; j++) {
                    if (!MP_PARSE_NODE_IS_SMALL_INT(pn_arg[j])) {
                        compile_syntax_error(comp, nodes[i], "'data' requires integer arguments");
                        return;
                    }
                    mp_asm_base_data((mp_asm_base_t*)comp->emit_inline_asm,
                        bytesize, MP_PARSE_NODE_LEAF_SMALL_INT(pn_arg[j]));
                }
            }
        } else {
            if (pass > MP_PASS_SCOPE) {
                EMIT_INLINE_ASM_ARG(op, op, n_args, pn_arg);
            }
        }

        if (comp->compile_error != MP_OBJ_NULL) {
            pns = pns2; // this is the parse node that had the error
            goto inline_asm_error;
        }
    }

    if (comp->pass > MP_PASS_SCOPE) {
        EMIT_INLINE_ASM_ARG(end_pass, type_sig);

        if (comp->pass == MP_PASS_EMIT) {
            void *f = mp_asm_base_get_code((mp_asm_base_t*)comp->emit_inline_asm);
            mp_emit_glue_assign_native(comp->scope_cur->raw_code, MP_CODE_NATIVE_ASM,
                f, mp_asm_base_get_code_size((mp_asm_base_t*)comp->emit_inline_asm),
                NULL, comp->scope_cur->num_pos_args, 0, type_sig);
        }
    }

    if (comp->compile_error != MP_OBJ_NULL) {
        // inline assembler had an error; set line for its exception
    inline_asm_error:
        comp->compile_error_line = pns->source_line;
    }
}
#endif

STATIC void scope_compute_things(scope_t *scope) {
    // in MicroPython we put the *x parameter after all other parameters (except **y)
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
        if (SCOPE_IS_FUNC_LIKE(scope->kind) && id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
            id->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
        }
        #if MICROPY_EMIT_NATIVE
        if (id->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
            // This function makes a reference to a global variable
            if (scope->emit_options == MP_EMIT_OPT_VIPER
                && mp_native_type_from_qstr(id->qst) >= MP_NATIVE_TYPE_INT) {
                // A casting operator in viper mode, not a real global reference
            } else {
                scope->scope_flags |= MP_SCOPE_FLAG_REFGLOBALS;
            }
        }
        #endif
        // params always count for 1 local, even if they are a cell
        if (id->kind == ID_INFO_KIND_LOCAL || (id->flags & ID_FLAG_IS_PARAM)) {
            id->local_num = scope->num_locals++;
        }
    }

    // compute the index of cell vars
    for (int i = 0; i < scope->id_info_len; i++) {
        id_info_t *id = &scope->id_info[i];
        // in MicroPython the cells come right after the fast locals
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
                        // in MicroPython the frees come first, before the params
                        id2->local_num = num_free;
                        num_free += 1;
                    }
                }
            }
        }
        // in MicroPython shift all other locals after the free locals
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
    comp->break_label = INVALID_LABEL;
    comp->continue_label = INVALID_LABEL;

    // create the module scope
    scope_t *module_scope = scope_new_and_link(comp, SCOPE_MODULE, parse_tree->root, emit_opt);

    // create standard emitter; it's used at least for MP_PASS_SCOPE
    emit_t *emit_bc = emit_bc_new();

    // compile pass 1
    comp->emit = emit_bc;
    #if MICROPY_EMIT_NATIVE
    comp->emit_method_table = &emit_bc_method_table;
    #endif
    uint max_num_labels = 0;
    for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
        if (false) {
        #if MICROPY_EMIT_INLINE_ASM
        } else if (s->emit_options == MP_EMIT_OPT_ASM) {
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

    // compute some things related to scope and identifiers
    for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
        scope_compute_things(s);
    }

    // set max number of labels now that it's calculated
    emit_bc_set_max_num_labels(emit_bc, max_num_labels);

    // compile pass 2 and 3
#if MICROPY_EMIT_NATIVE
    emit_t *emit_native = NULL;
#endif
    for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
        if (false) {
            // dummy

        #if MICROPY_EMIT_INLINE_ASM
        } else if (s->emit_options == MP_EMIT_OPT_ASM) {
            // inline assembly
            if (comp->emit_inline_asm == NULL) {
                comp->emit_inline_asm = ASM_EMITTER(new)(max_num_labels);
            }
            comp->emit = NULL;
            comp->emit_inline_asm_method_table = &ASM_EMITTER(method_table);
            compile_scope_inline_asm(comp, s, MP_PASS_CODE_SIZE);
            #if MICROPY_EMIT_INLINE_XTENSA
            // Xtensa requires an extra pass to compute size of l32r const table
            // TODO this can be improved by calculating it during SCOPE pass
            // but that requires some other structural changes to the asm emitters
            compile_scope_inline_asm(comp, s, MP_PASS_CODE_SIZE);
            #endif
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
                    if (emit_native == NULL) {
                        emit_native = NATIVE_EMITTER(new)(&comp->compile_error, &comp->next_label, max_num_labels);
                    }
                    comp->emit_method_table = &NATIVE_EMITTER(method_table);
                    comp->emit = emit_native;
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
        NATIVE_EMITTER(free)(emit_native);
    }
#endif
    #if MICROPY_EMIT_INLINE_ASM
    if (comp->emit_inline_asm != NULL) {
        ASM_EMITTER(free)(comp->emit_inline_asm);
    }
    #endif

    // free the parse tree
    mp_parse_tree_clear(parse_tree);

    // free the scopes
    mp_raw_code_t *outer_raw_code = module_scope->raw_code;
    for (scope_t *s = module_scope; s;) {
        scope_t *next = s->next;
        scope_free(s);
        s = next;
    }

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
