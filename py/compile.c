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
#include <string.h>
#include <assert.h>
#include <math.h>

#include "py/scope.h"
#include "py/emit.h"
#include "py/compile.h"
#include "py/smallint.h"
#include "py/runtime.h"
#include "py/builtin.h"

// TODO need to mangle __attr names

typedef enum {
#define DEF_RULE(rule, comp, kind, ...) PN_##rule,
#include "py/grammar.h"
#undef DEF_RULE
    PN_maximum_number_of,
    PN_string, // special node for non-interned string
    PN_bytes, // special node for non-interned bytes
} pn_kind_t;

#define EMIT(fun) (comp->emit_method_table->fun(comp->emit))
#define EMIT_ARG(fun, ...) (comp->emit_method_table->fun(comp->emit, __VA_ARGS__))
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
    // this is set to an exception object if we have a compile error
    mp_obj_t compile_error;

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
    const emit_method_table_t *emit_method_table;   // current emit method table

    emit_inline_asm_t *emit_inline_asm;                                   // current emitter for inline asm
    const emit_inline_asm_method_table_t *emit_inline_asm_method_table;   // current emit method table for inline asm
} compiler_t;

STATIC void compile_syntax_error(compiler_t *comp, mp_parse_node_t pn, const char *msg) {
    mp_obj_t exc = mp_obj_new_exception_msg(&mp_type_SyntaxError, msg);
    // we don't have a 'block' name, so just pass the NULL qstr to indicate this
    if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_obj_exception_add_traceback(exc, comp->source_file, (mp_uint_t)((mp_parse_node_struct_t*)pn)->source_line, MP_QSTR_NULL);
    } else {
        // we don't have a line number, so just pass 0
        mp_obj_exception_add_traceback(exc, comp->source_file, 0, MP_QSTR_NULL);
    }
    comp->compile_error = exc;
}

#if MICROPY_COMP_MODULE_CONST
STATIC const mp_map_elem_t mp_constants_table[] = {
    #if MICROPY_PY_UCTYPES
    { MP_OBJ_NEW_QSTR(MP_QSTR_uctypes), (mp_obj_t)&mp_module_uctypes },
    #endif
    // Extra constants as defined by a port
    MICROPY_PORT_CONSTANTS
};
STATIC MP_DEFINE_CONST_MAP(mp_constants_map, mp_constants_table);
#endif

// this function is essentially a simple preprocessor
STATIC mp_parse_node_t fold_constants(compiler_t *comp, mp_parse_node_t pn, mp_map_t *consts) {
    if (0) {
        // dummy
#if MICROPY_COMP_CONST
    } else if (MP_PARSE_NODE_IS_ID(pn)) {
        // lookup identifier in table of dynamic constants
        qstr qst = MP_PARSE_NODE_LEAF_ARG(pn);
        mp_map_elem_t *elem = mp_map_lookup(consts, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP);
        if (elem != NULL) {
            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, MP_OBJ_SMALL_INT_VALUE(elem->value));
        }
#endif
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;

        // fold some parse nodes before folding their arguments
        switch (MP_PARSE_NODE_STRUCT_KIND(pns)) {
#if MICROPY_COMP_CONST
            case PN_expr_stmt:
                if (!MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
                    mp_parse_node_struct_t *pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
                    if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_expr_stmt_assign) {
                        if (MP_PARSE_NODE_IS_ID(pns->nodes[0])
                            && MP_PARSE_NODE_IS_STRUCT_KIND(pns1->nodes[0], PN_power)
                            && MP_PARSE_NODE_IS_ID(((mp_parse_node_struct_t*)pns1->nodes[0])->nodes[0])
                            && MP_PARSE_NODE_LEAF_ARG(((mp_parse_node_struct_t*)pns1->nodes[0])->nodes[0]) == MP_QSTR_const
                            && MP_PARSE_NODE_IS_STRUCT_KIND(((mp_parse_node_struct_t*)pns1->nodes[0])->nodes[1], PN_trailer_paren)
                            && MP_PARSE_NODE_IS_NULL(((mp_parse_node_struct_t*)pns1->nodes[0])->nodes[2])
                            ) {
                            // code to assign dynamic constants: id = const(value)

                            // get the id
                            qstr id_qstr = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);

                            // get the value
                            mp_parse_node_t pn_value = ((mp_parse_node_struct_t*)((mp_parse_node_struct_t*)pns1->nodes[0])->nodes[1])->nodes[0];
                            pn_value = fold_constants(comp, pn_value, consts);
                            if (!MP_PARSE_NODE_IS_SMALL_INT(pn_value)) {
                                compile_syntax_error(comp, (mp_parse_node_t)pns, "constant must be an integer");
                                break;
                            }
                            mp_int_t value = MP_PARSE_NODE_LEAF_SMALL_INT(pn_value);

                            // store the value in the table of dynamic constants
                            mp_map_elem_t *elem = mp_map_lookup(consts, MP_OBJ_NEW_QSTR(id_qstr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
                            if (elem->value != MP_OBJ_NULL) {
                                compile_syntax_error(comp, (mp_parse_node_t)pns, "constant redefined");
                                break;
                            }
                            elem->value = MP_OBJ_NEW_SMALL_INT(value);

                            // replace const(value) with value
                            pns1->nodes[0] = pn_value;

                            // finished folding this assignment
                            return pn;
                        }
                    }
                }
                break;
#endif
            case PN_string:
            case PN_bytes:
                return pn;
        }

        // fold arguments
        int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
        for (int i = 0; i < n; i++) {
            pns->nodes[i] = fold_constants(comp, pns->nodes[i], consts);
        }

        // try to fold this parse node
        switch (MP_PARSE_NODE_STRUCT_KIND(pns)) {
            case PN_atom_paren:
                if (n == 1 && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0])) {
                    // (int)
                    pn = pns->nodes[0];
                }
                break;

            case PN_expr:
                if (n == 2 && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[1])) {
                    // int | int
                    mp_int_t arg0 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[0]);
                    mp_int_t arg1 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[1]);
                    pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0 | arg1);
                }
                break;

            case PN_and_expr:
                if (n == 2 && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[1])) {
                    // int & int
                    mp_int_t arg0 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[0]);
                    mp_int_t arg1 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[1]);
                    pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0 & arg1);
                }
                break;

            case PN_shift_expr:
                if (n == 3 && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[2])) {
                    mp_int_t arg0 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[0]);
                    mp_int_t arg1 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[2]);
                    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_DBL_LESS)) {
                        // int << int
                        if (!(arg1 >= (mp_int_t)BITS_PER_WORD || arg0 > (MP_SMALL_INT_MAX >> arg1) || arg0 < (MP_SMALL_INT_MIN >> arg1))) {
                            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0 << arg1);
                        }
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_DBL_MORE)) {
                        // int >> int
                        if (arg1 >= (mp_int_t)BITS_PER_WORD) {
                            // Shifting to big amounts is underfined behavior
                            // in C and is CPU-dependent; propagate sign bit.
                            arg1 = BITS_PER_WORD - 1;
                        }
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0 >> arg1);
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

            case PN_arith_expr:
                // overflow checking here relies on SMALL_INT being strictly smaller than mp_int_t
                if (n == 3 && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[2])) {
                    mp_int_t arg0 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[0]);
                    mp_int_t arg1 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[2]);
                    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_PLUS)) {
                        // int + int
                        arg0 += arg1;
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_MINUS)) {
                        // int - int
                        arg0 -= arg1;
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                    if (MP_SMALL_INT_FITS(arg0)) {
                        //printf("%ld + %ld\n", arg0, arg1);
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0);
                    }
                }
                break;

            case PN_term:
                if (n == 3 && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[2])) {
                    mp_int_t arg0 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[0]);
                    mp_int_t arg1 = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[2]);
                    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_STAR)) {
                        // int * int
                        if (!mp_small_int_mul_overflow(arg0, arg1)) {
                            arg0 *= arg1;
                            if (MP_SMALL_INT_FITS(arg0)) {
                                pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg0);
                            }
                        }
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_SLASH)) {
                        // int / int
                        // pass
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_PERCENT)) {
                        // int%int
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, mp_small_int_modulo(arg0, arg1));
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[1], MP_TOKEN_OP_DBL_SLASH)) {
                        if (arg1 != 0) {
                            // int // int
                            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, mp_small_int_floor_divide(arg0, arg1));
                        }
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

            case PN_factor_2:
                if (MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[1])) {
                    mp_int_t arg = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[1]);
                    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_PLUS)) {
                        // +int
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, arg);
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_MINUS)) {
                        // -int
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, -arg);
                    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_TILDE)) {
                        // ~int
                        pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, ~arg);
                    } else {
                        // shouldn't happen
                        assert(0);
                    }
                }
                break;

            case PN_power:
                if (0) {
#if MICROPY_EMIT_CPYTHON
                } else if (MP_PARSE_NODE_IS_SMALL_INT(pns->nodes[0]) && MP_PARSE_NODE_IS_NULL(pns->nodes[1]) && !MP_PARSE_NODE_IS_NULL(pns->nodes[2])) {
                    // int ** x
                    // can overflow; enabled only to compare with CPython
                    mp_parse_node_struct_t* pns2 = (mp_parse_node_struct_t*)pns->nodes[2];
                    if (MP_PARSE_NODE_IS_SMALL_INT(pns2->nodes[0])) {
                        int power = MP_PARSE_NODE_LEAF_SMALL_INT(pns2->nodes[0]);
                        if (power >= 0) {
                            int ans = 1;
                            int base = MP_PARSE_NODE_LEAF_SMALL_INT(pns->nodes[0]);
                            for (; power > 0; power--) {
                                ans *= base;
                            }
                            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, ans);
                        }
                    }
#endif
#if MICROPY_COMP_MODULE_CONST
                } else if (MP_PARSE_NODE_IS_ID(pns->nodes[0]) && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_trailer_period) && MP_PARSE_NODE_IS_NULL(pns->nodes[2])) {
                    // id.id
                    // look it up in constant table, see if it can be replaced with an integer
                    mp_parse_node_struct_t* pns1 = (mp_parse_node_struct_t*)pns->nodes[1];
                    assert(MP_PARSE_NODE_IS_ID(pns1->nodes[0]));
                    qstr q_base = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
                    qstr q_attr = MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]);
                    mp_map_elem_t *elem = mp_map_lookup((mp_map_t*)&mp_constants_map, MP_OBJ_NEW_QSTR(q_base), MP_MAP_LOOKUP);
                    if (elem != NULL) {
                        mp_obj_t dest[2];
                        mp_load_method_maybe(elem->value, q_attr, dest);
                        if (MP_OBJ_IS_SMALL_INT(dest[0]) && dest[1] == NULL) {
                            mp_int_t val = MP_OBJ_SMALL_INT_VALUE(dest[0]);
                            pn = mp_parse_node_new_leaf(MP_PARSE_NODE_SMALL_INT, val);
                        }
                    }
#endif
                }
                break;
        }
    }

    return pn;
}

STATIC void compile_trailer_paren_helper(compiler_t *comp, mp_parse_node_t pn_arglist, bool is_method_call, int n_positional_extra);
STATIC void compile_comprehension(compiler_t *comp, mp_parse_node_struct_t *pns, scope_kind_t kind);
STATIC void compile_node(compiler_t *comp, mp_parse_node_t pn);

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

STATIC void apply_to_single_or_list(compiler_t *comp, mp_parse_node_t pn, pn_kind_t pn_list_kind, void (*f)(compiler_t*, mp_parse_node_t)) {
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

STATIC int list_get(mp_parse_node_t *pn, pn_kind_t pn_kind, mp_parse_node_t **nodes) {
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

STATIC void compile_generic_all_nodes(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < num_nodes; i++) {
        compile_node(comp, pns->nodes[i]);
    }
}

#if MICROPY_EMIT_CPYTHON
STATIC bool cpython_c_tuple_is_const(mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_string)) {
        return true;
    }
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_bytes)) {
        return true;
    }
    if (!MP_PARSE_NODE_IS_LEAF(pn)) {
        return false;
    }
    if (MP_PARSE_NODE_IS_ID(pn)) {
        return false;
    }
    return true;
}

STATIC void cpython_c_print_quoted_str(vstr_t *vstr, const char *str, uint len, bool bytes) {
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
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_string) || MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_bytes)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        cpython_c_print_quoted_str(vstr, (const char*)pns->nodes[0], (mp_uint_t)pns->nodes[1], MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_bytes));
        return;
    }

    assert(MP_PARSE_NODE_IS_LEAF(pn));
    if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        vstr_printf(vstr, INT_FMT, MP_PARSE_NODE_LEAF_SMALL_INT(pn));
        return;
    }

    mp_uint_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
    switch (MP_PARSE_NODE_LEAF_KIND(pn)) {
        case MP_PARSE_NODE_ID: assert(0);
        case MP_PARSE_NODE_INTEGER: vstr_printf(vstr, "%s", qstr_str(arg)); break;
        case MP_PARSE_NODE_DECIMAL: vstr_printf(vstr, "%s", qstr_str(arg)); break;
        case MP_PARSE_NODE_STRING:
        case MP_PARSE_NODE_BYTES: {
            mp_uint_t len;
            const byte *str = qstr_data(arg, &len);
            cpython_c_print_quoted_str(vstr, (const char*)str, len, MP_PARSE_NODE_LEAF_KIND(pn) == MP_PARSE_NODE_BYTES);
            break;
        }
        case MP_PARSE_NODE_TOKEN:
            switch (arg) {
                case MP_TOKEN_KW_FALSE: vstr_printf(vstr, "False"); break;
                case MP_TOKEN_KW_NONE: vstr_printf(vstr, "None"); break;
                case MP_TOKEN_KW_TRUE: vstr_printf(vstr, "True"); break;
                default: assert(0); // shouldn't happen
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
STATIC void c_tuple(compiler_t *comp, mp_parse_node_t pn, mp_parse_node_struct_t *pns_list) {
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

STATIC void compile_generic_tuple(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // a simple tuple expression
    c_tuple(comp, MP_PARSE_NODE_NULL, pns);
}

STATIC bool node_is_const_false(mp_parse_node_t pn) {
    return MP_PARSE_NODE_IS_TOKEN_KIND(pn, MP_TOKEN_KW_FALSE)
        || (MP_PARSE_NODE_IS_SMALL_INT(pn) && MP_PARSE_NODE_LEAF_SMALL_INT(pn) == 0);
}

STATIC bool node_is_const_true(mp_parse_node_t pn) {
    return MP_PARSE_NODE_IS_TOKEN_KIND(pn, MP_TOKEN_KW_TRUE)
        || (MP_PARSE_NODE_IS_SMALL_INT(pn) && MP_PARSE_NODE_LEAF_SMALL_INT(pn) != 0);
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
                uint label2 = comp_next_label(comp);
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
                uint label2 = comp_next_label(comp);
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
                uint label2 = comp_next_label(comp);
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
                uint label2 = comp_next_label(comp);
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
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_atom_paren) {
            // cond is something in parenthesis
            if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                // empty tuple, acts as false for the condition
                if (jump_if == false) {
                    EMIT_ARG(jump, label);
                }
            } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_comp)) {
                // non-empty tuple, acts as true for the condition
                if (jump_if == true) {
                    EMIT_ARG(jump, label);
                }
            } else {
                // parenthesis around 1 item, is just that item
                c_if_cond(comp, pns->nodes[0], jump_if, label);
            }
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
STATIC void c_assign(compiler_t *comp, mp_parse_node_t pn, assign_kind_t kind);

STATIC void c_assign_power(compiler_t *comp, mp_parse_node_struct_t *pns, assign_kind_t assign_kind) {
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
            goto cannot_assign;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_bracket) {
            if (assign_kind == ASSIGN_AUG_STORE) {
                EMIT(rot_three);
                EMIT(store_subscr);
            } else {
                compile_node(comp, pns1->nodes[0]);
                if (assign_kind == ASSIGN_AUG_LOAD) {
                    EMIT(dup_top_two);
                    EMIT(load_subscr);
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
            goto cannot_assign;
        }
    } else {
        goto cannot_assign;
    }

    if (!MP_PARSE_NODE_IS_NULL(pns->nodes[2])) {
        goto cannot_assign;
    }

    return;

cannot_assign:
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
    tail_recursion:
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        assert(0);
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        if (MP_PARSE_NODE_IS_ID(pn)) {
            qstr arg = MP_PARSE_NODE_LEAF_ARG(pn);
            switch (assign_kind) {
                case ASSIGN_STORE:
                case ASSIGN_AUG_STORE:
                    EMIT_ARG(store_id, arg);
                    break;
                case ASSIGN_AUG_LOAD:
                default:
                    EMIT_ARG(load_id, arg);
                    break;
            }
        } else {
            compile_syntax_error(comp, pn, "can't assign to literal");
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
                c_assign_tuple(comp, MP_PARSE_NODE_NULL, MP_PARSE_NODE_STRUCT_NUM_NODES(pns), pns->nodes);
                break;

            case PN_atom_paren:
                // lhs is something in parenthesis
                if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                    // empty tuple
                    goto cannot_assign;
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
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_comp_for) {
                // TODO can we ever get here? can it be compiled?
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
    return;

    bad_aug:
    compile_syntax_error(comp, pn, "illegal expression for augmented assignment");
}

// stuff for lambda and comprehensions and generators
// if we are not in CPython compatibility mode then:
//  if n_pos_defaults > 0 then there is a tuple on the stack with the positional defaults
//  if n_kw_defaults > 0 then there is a dictionary on the stack with the keyword defaults
//  if both exist, the tuple is above the dictionary (ie the first pop gets the tuple)
STATIC void close_over_variables_etc(compiler_t *comp, scope_t *this_scope, int n_pos_defaults, int n_kw_defaults) {
    assert(n_pos_defaults >= 0);
    assert(n_kw_defaults >= 0);

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
#if MICROPY_EMIT_CPYTHON
                        EMIT_ARG(load_closure, id->qst, id->local_num);
#else
                        // in Micro Python we load closures using LOAD_FAST
                        EMIT_ARG(load_fast, id->qst, id->local_num);
#endif
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

STATIC void compile_funcdef_param(compiler_t *comp, mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_typedargslist_star)) {
        comp->have_star = true;
        /* don't need to distinguish bare from named star
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
            // bare star
        } else {
            // named star
        }
        */

    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_typedargslist_dbl_star)) {
        // named double star
        // TODO do we need to do anything with this?

    } else {
        mp_parse_node_t pn_id;
        mp_parse_node_t pn_colon;
        mp_parse_node_t pn_equal;
        if (MP_PARSE_NODE_IS_ID(pn)) {
            // this parameter is just an id

            pn_id = pn;
            pn_colon = MP_PARSE_NODE_NULL;
            pn_equal = MP_PARSE_NODE_NULL;

        } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pn, PN_typedargslist_name)) {
            // this parameter has a colon and/or equal specifier

            mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
            pn_id = pns->nodes[0];
            pn_colon = pns->nodes[1];
            pn_equal = pns->nodes[2];

        } else {
            // XXX what to do here?
            assert(0);
            return;
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
#if MICROPY_EMIT_CPYTHON
                EMIT_ARG(load_const_str, MP_PARSE_NODE_LEAF_ARG(pn_id), false);
                compile_node(comp, pn_equal);
#else
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
                compile_node(comp, pn_equal);
                EMIT_ARG(load_const_str, MP_PARSE_NODE_LEAF_ARG(pn_id), false);
                EMIT(store_map);
#endif
            } else {
                comp->num_default_params += 1;
                compile_node(comp, pn_equal);
            }
        }

        // TODO pn_colon not implemented
        (void)pn_colon;
    }
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

    // save variables (probably don't need to do this, since we can't have nested definitions..?)
    uint old_have_star = comp->have_star;
    uint old_num_dict_params = comp->num_dict_params;
    uint old_num_default_params = comp->num_default_params;

    // compile default parameters
    comp->have_star = false;
    comp->num_dict_params = 0;
    comp->num_default_params = 0;
    apply_to_single_or_list(comp, pns->nodes[1], PN_typedargslist, compile_funcdef_param);

    if (comp->compile_error != MP_OBJ_NULL) {
        return MP_QSTR_NULL;
    }

#if !MICROPY_EMIT_CPYTHON
    // in Micro Python we put the default positional parameters into a tuple using the bytecode
    // the default keywords args may have already made the tuple; if not, do it now
    if (comp->num_default_params > 0 && comp->num_dict_params == 0) {
        EMIT_ARG(build_tuple, comp->num_default_params);
        EMIT(load_null); // sentinel indicating empty default keyword args
    }
#endif

    // get the scope for this function
    scope_t *fscope = (scope_t*)pns->nodes[4];

    // make the function
    close_over_variables_etc(comp, fscope, comp->num_default_params, comp->num_dict_params);

    // restore variables
    comp->have_star = old_have_star;
    comp->num_dict_params = old_num_dict_params;
    comp->num_default_params = old_num_default_params;

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
    EMIT_ARG(load_const_str, cscope->simple_name, false);

    // nodes[1] has parent classes, if any
    // empty parenthesis (eg class C():) gets here as an empty PN_classdef_2 and needs special handling
    mp_parse_node_t parents = pns->nodes[1];
    if (MP_PARSE_NODE_IS_STRUCT_KIND(parents, PN_classdef_2)) {
        parents = MP_PARSE_NODE_NULL;
    }
    comp->func_arg_is_super = false;
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
#if MICROPY_EMIT_INLINE_THUMB
    } else if (attr == MP_QSTR_asm_thumb) {
        *emit_options = MP_EMIT_OPT_ASM_THUMB;
#endif
    } else {
        compile_syntax_error(comp, name_nodes[1], "invalid micropython decorator");
    }

    return true;
}

STATIC void compile_decorated(compiler_t *comp, mp_parse_node_struct_t *pns) {
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
            for (int j = 1; j < name_len; j++) {
                assert(MP_PARSE_NODE_IS_ID(name_nodes[j])); // should be
                EMIT_ARG(load_attr, MP_PARSE_NODE_LEAF_ARG(name_nodes[j]));
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
        EMIT_ARG(call_function, 1, 0, 0);
    }

    // store func/class object into name
    EMIT_ARG(store_id, body_name);
}

STATIC void compile_funcdef(compiler_t *comp, mp_parse_node_struct_t *pns) {
    qstr fname = compile_funcdef_helper(comp, pns, comp->scope_cur->emit_options);
    // store function object into function name
    EMIT_ARG(store_id, fname);
}

STATIC void c_del_stmt(compiler_t *comp, mp_parse_node_t pn) {
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
                // can't delete function calls
                goto cannot_delete;
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_bracket) {
                compile_node(comp, pns1->nodes[0]);
                EMIT(delete_subscr);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_trailer_period) {
                assert(MP_PARSE_NODE_IS_ID(pns1->nodes[0]));
                EMIT_ARG(delete_attr, MP_PARSE_NODE_LEAF_ARG(pns1->nodes[0]));
            } else {
                goto cannot_delete;
            }
        } else {
            goto cannot_delete;
        }

        if (!MP_PARSE_NODE_IS_NULL(pns->nodes[2])) {
            goto cannot_delete;
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
        } else {
            // tuple with 1 element
            c_del_stmt(comp, pn);
        }
    } else {
        // TODO is there anything else to implement?
        goto cannot_delete;
    }

    return;

cannot_delete:
    compile_syntax_error(comp, (mp_parse_node_t)pn, "can't delete expression");
}

STATIC void compile_del_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    apply_to_single_or_list(comp, pns->nodes[0], PN_exprlist, c_del_stmt);
}

STATIC void compile_break_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->break_label == 0) {
        compile_syntax_error(comp, (mp_parse_node_t)pns, "'break' outside loop");
    }
    assert(comp->cur_except_level >= comp->break_continue_except_level);
    EMIT_ARG(break_loop, comp->break_label, comp->cur_except_level - comp->break_continue_except_level);
}

STATIC void compile_continue_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->continue_label == 0) {
        compile_syntax_error(comp, (mp_parse_node_t)pns, "'continue' outside loop");
    }
    assert(comp->cur_except_level >= comp->break_continue_except_level);
    EMIT_ARG(continue_loop, comp->continue_label, comp->cur_except_level - comp->break_continue_except_level);
}

STATIC void compile_return_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION) {
        compile_syntax_error(comp, (mp_parse_node_t)pns, "'return' outside function");
        return;
    }
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
        // no argument to 'return', so return None
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_test_if_expr)) {
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
        EMIT_ARG(import_name, MP_QSTR_); // import the empty string
    } else if (MP_PARSE_NODE_IS_ID(pn)) {
        // just a simple name
        qstr q_full = MP_PARSE_NODE_LEAF_ARG(pn);
        if (!is_as) {
            *q_base = q_full;
        }
        EMIT_ARG(import_name, q_full);
    } else if (MP_PARSE_NODE_IS_STRUCT(pn)) {
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_dotted_name) {
            // a name of the form a.b.c
            if (!is_as) {
                *q_base = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
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
                mp_uint_t str_src_len;
                const byte *str_src = qstr_data(MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]), &str_src_len);
                memcpy(str_dest, str_src, str_src_len);
                str_dest += str_src_len;
            }
            qstr q_full = qstr_build_end(q_ptr);
            EMIT_ARG(import_name, q_full);
            if (is_as) {
                for (int i = 1; i < n; i++) {
                    EMIT_ARG(load_attr, MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
                }
            }
        } else {
            // shouldn't happen
            assert(0);
        }
    } else {
        // shouldn't happen
        assert(0);
    }
}

STATIC void compile_dotted_as_name(compiler_t *comp, mp_parse_node_t pn) {
    EMIT_ARG(load_const_small_int, 0); // level 0 import
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE); // not importing from anything
    qstr q_base;
    do_import_name(comp, pn, &q_base);
    EMIT_ARG(store_id, q_base);
}

STATIC void compile_import_name(compiler_t *comp, mp_parse_node_struct_t *pns) {
    apply_to_single_or_list(comp, pns->nodes[0], PN_dotted_as_names, compile_dotted_as_name);
}

STATIC void compile_import_from(compiler_t *comp, mp_parse_node_struct_t *pns) {
    mp_parse_node_t pn_import_source = pns->nodes[0];

    // extract the preceeding .'s (if any) for a relative import, to compute the import level
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
        int n = list_get(&pn_rel, PN_one_or_more_period_or_ellipsis, &nodes);

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
#if MICROPY_EMIT_CPYTHON
        EMIT_ARG(load_const_verbatim_str, "('*',)");
#else
        EMIT_ARG(load_const_str, MP_QSTR__star_, false);
        EMIT_ARG(build_tuple, 1);
#endif

        // do the import
        qstr dummy_q;
        do_import_name(comp, pn_import_source, &dummy_q);
        EMIT(import_star);

    } else {
        EMIT_ARG(load_const_small_int, import_level);

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
                mp_uint_t len;
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
        qstr dummy_q;
        do_import_name(comp, pn_import_source, &dummy_q);
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

STATIC void compile_declare_global(compiler_t *comp, mp_parse_node_t pn, qstr qst) {
    bool added;
    id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, qst, &added);
    if (!added) {
        compile_syntax_error(comp, pn, "identifier already used");
        return;
    }
    id_info->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;

    // if the id exists in the global scope, set its kind to EXPLICIT_GLOBAL
    id_info = scope_find_global(comp->scope_cur, qst);
    if (id_info != NULL) {
        id_info->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
    }
}

STATIC void compile_global_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->pass == MP_PASS_SCOPE) {
        mp_parse_node_t *nodes;
        int n = list_get(&pns->nodes[0], PN_name_list, &nodes);
        for (int i = 0; i < n; i++) {
            compile_declare_global(comp, (mp_parse_node_t)pns, MP_PARSE_NODE_LEAF_ARG(nodes[i]));
        }
    }
}

STATIC void compile_declare_nonlocal(compiler_t *comp, mp_parse_node_t pn, qstr qst) {
    bool added;
    id_info_t *id_info = scope_find_or_add_id(comp->scope_cur, qst, &added);
    if (!added) {
        compile_syntax_error(comp, pn, "identifier already used");
        return;
    }
    id_info_t *id_info2 = scope_find_local_in_parent(comp->scope_cur, qst);
    if (id_info2 == NULL || !(id_info2->kind == ID_INFO_KIND_LOCAL || id_info2->kind == ID_INFO_KIND_CELL || id_info2->kind == ID_INFO_KIND_FREE)) {
        compile_syntax_error(comp, pn, "no binding for nonlocal found");
        return;
    }
    id_info->kind = ID_INFO_KIND_FREE;
    scope_close_over_in_parents(comp->scope_cur, qst);
}

STATIC void compile_nonlocal_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->pass == MP_PASS_SCOPE) {
        if (comp->scope_cur->kind == SCOPE_MODULE) {
            compile_syntax_error(comp, (mp_parse_node_t)pns, "can't declare nonlocal in outer code");
            return;
        }
        mp_parse_node_t *nodes;
        int n = list_get(&pns->nodes[0], PN_name_list, &nodes);
        for (int i = 0; i < n; i++) {
            compile_declare_nonlocal(comp, (mp_parse_node_t)pns, MP_PARSE_NODE_LEAF_ARG(nodes[i]));
        }
    }
}

STATIC void compile_assert_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    uint l_end = comp_next_label(comp);
    c_if_cond(comp, pns->nodes[0], true, l_end);
    EMIT_ARG(load_global, MP_QSTR_AssertionError); // we load_global instead of load_id, to be consistent with CPython
    if (!MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        // assertion message
        compile_node(comp, pns->nodes[1]);
        EMIT_ARG(call_function, 1, 0, 0);
    }
    EMIT_ARG(raise_varargs, 1);
    EMIT_ARG(label_assign, l_end);
}

STATIC void compile_if_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // TODO proper and/or short circuiting

    uint l_end = comp_next_label(comp);

    // optimisation: don't emit anything when "if False" (not in CPython)
    if (MICROPY_EMIT_CPYTHON || !node_is_const_false(pns->nodes[0])) {
        uint l_fail = comp_next_label(comp);
        c_if_cond(comp, pns->nodes[0], false, l_fail); // if condition

        compile_node(comp, pns->nodes[1]); // if block

        // optimisation: skip everything else when "if True" (not in CPython)
        if (!MICROPY_EMIT_CPYTHON && node_is_const_true(pns->nodes[0])) {
            goto done;
        }

        if (
            // optimisation: don't jump over non-existent elif/else blocks (not in CPython)
            (MICROPY_EMIT_CPYTHON || !(MP_PARSE_NODE_IS_NULL(pns->nodes[2]) && MP_PARSE_NODE_IS_NULL(pns->nodes[3])))
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
    int n_elif = list_get(&pns->nodes[2], PN_if_stmt_elif_list, &pn_elif);
    for (int i = 0; i < n_elif; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_elif[i], PN_if_stmt_elif)); // should be
        mp_parse_node_struct_t *pns_elif = (mp_parse_node_struct_t*)pn_elif[i];

        // optimisation: don't emit anything when "if False" (not in CPython)
        if (MICROPY_EMIT_CPYTHON || !node_is_const_false(pns_elif->nodes[0])) {
            uint l_fail = comp_next_label(comp);
            c_if_cond(comp, pns_elif->nodes[0], false, l_fail); // elif condition

            compile_node(comp, pns_elif->nodes[1]); // elif block

            // optimisation: skip everything else when "elif True" (not in CPython)
            if (!MICROPY_EMIT_CPYTHON && node_is_const_true(pns_elif->nodes[0])) {
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

    // compared to CPython, we have an optimised version of while loops
#if MICROPY_EMIT_CPYTHON
    uint done_label = comp_next_label(comp);
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
    if (!node_is_const_false(pns->nodes[0])) { // optimisation: don't emit anything for "while False"
        uint top_label = comp_next_label(comp);
        if (!node_is_const_true(pns->nodes[0])) { // optimisation: don't jump to cond for "while True"
            EMIT_ARG(jump, continue_label);
        }
        EMIT_ARG(label_assign, top_label);
        compile_node(comp, pns->nodes[1]); // body
        EMIT_ARG(label_assign, continue_label);
        c_if_cond(comp, pns->nodes[0], true, top_label); // condition
    }
#endif

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    compile_node(comp, pns->nodes[2]); // else

    EMIT_ARG(label_assign, break_label);
}

#if !MICROPY_EMIT_CPYTHON
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
    EMIT_ARG(pop_jump_if_true, top_label);

    // break/continue apply to outer loop (if any) in the else block
    END_BREAK_CONTINUE_BLOCK

    compile_node(comp, pn_else);

    EMIT_ARG(label_assign, break_label);

    // discard final value of var that failed the loop condition
    EMIT(pop_top);

    // discard <end> value if it's on the stack
    if (end_on_stack) {
        EMIT(pop_top);
    }
}
#endif

STATIC void compile_for_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
#if !MICROPY_EMIT_CPYTHON
    // this bit optimises: for <x> in range(...), turning it into an explicitly incremented variable
    // this is actually slower, but uses no heap memory
    // for viper it will be much, much faster
    if (/*comp->scope_cur->emit_options == MP_EMIT_OPT_VIPER &&*/ MP_PARSE_NODE_IS_ID(pns->nodes[0]) && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[1], PN_power)) {
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
    comp->break_label |= MP_EMIT_BREAK_FROM_FOR;

    uint pop_label = comp_next_label(comp);
    uint end_label = comp_next_label(comp);

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

STATIC void compile_try_except(compiler_t *comp, mp_parse_node_t pn_body, int n_except, mp_parse_node_t *pn_excepts, mp_parse_node_t pn_else) {
    // setup code
    uint l1 = comp_next_label(comp);
    uint success_label = comp_next_label(comp);

    EMIT_ARG(setup_except, l1);
    compile_increase_except_level(comp);

    compile_node(comp, pn_body); // body
    EMIT(pop_block);
    EMIT_ARG(jump, success_label); // jump over exception handler

    EMIT_ARG(label_assign, l1); // start of exception handler
    EMIT(start_except_handler);

    uint l2 = comp_next_label(comp);

    for (int i = 0; i < n_except; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT_KIND(pn_excepts[i], PN_try_stmt_except)); // should be
        mp_parse_node_struct_t *pns_except = (mp_parse_node_struct_t*)pn_excepts[i];

        qstr qstr_exception_local = 0;
        uint end_finally_label = comp_next_label(comp);

        if (MP_PARSE_NODE_IS_NULL(pns_except->nodes[0])) {
            // this is a catch all exception handler
            if (i + 1 != n_except) {
                compile_syntax_error(comp, pn_excepts[i], "default 'except:' must be last");
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
            EMIT_ARG(pop_jump_if_false, end_finally_label);
        }

        EMIT(pop_top);

        if (qstr_exception_local == 0) {
            EMIT(pop_top);
        } else {
            EMIT_ARG(store_id, qstr_exception_local);
        }

        EMIT(pop_top);

        uint l3 = 0;
        if (qstr_exception_local != 0) {
            l3 = comp_next_label(comp);
            EMIT_ARG(setup_finally, l3);
            compile_increase_except_level(comp);
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
    compile_node(comp, pn_else); // else block, can be null
    EMIT_ARG(label_assign, l2);
}

STATIC void compile_try_finally(compiler_t *comp, mp_parse_node_t pn_body, int n_except, mp_parse_node_t *pn_except, mp_parse_node_t pn_else, mp_parse_node_t pn_finally) {
    uint l_finally_block = comp_next_label(comp);

    EMIT_ARG(setup_finally, l_finally_block);
    compile_increase_except_level(comp);

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
    EMIT(end_finally);
}

STATIC void compile_try_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
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
            EMIT_ARG(setup_with, l_end);
            c_assign(comp, pns->nodes[1], ASSIGN_STORE);
        } else {
            // this pre-bit is just an expression
            compile_node(comp, nodes[0]);
            EMIT_ARG(setup_with, l_end);
            EMIT(pop_top);
        }
        compile_increase_except_level(comp);
        // compile additional pre-bits and the body
        compile_with_stmt_helper(comp, n - 1, nodes + 1, body);
        // finish this with block
        EMIT(pop_block);
        EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
        EMIT_ARG(label_assign, l_end);
        EMIT(with_cleanup);
        compile_decrease_except_level(comp);
        EMIT(end_finally);
    }
}

STATIC void compile_with_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // get the nodes for the pre-bit of the with (the a as b, c as d, ... bit)
    mp_parse_node_t *nodes;
    int n = list_get(&pns->nodes[0], PN_with_stmt_list, &nodes);
    assert(n > 0);

    // compile in a nested fashion
    compile_with_stmt_helper(comp, n, nodes, pns->nodes[1]);
}

STATIC void compile_expr_stmt(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (MP_PARSE_NODE_IS_NULL(pns->nodes[1])) {
        if (comp->is_repl && comp->scope_cur->kind == SCOPE_MODULE) {
            // for REPL, evaluate then print the expression
            EMIT_ARG(load_id, MP_QSTR___repl_print__);
            compile_node(comp, pns->nodes[0]);
            EMIT_ARG(call_function, 1, 0, 0);
            EMIT(pop_top);

        } else {
            // for non-REPL, evaluate then discard the expression
            if ((MP_PARSE_NODE_IS_LEAF(pns->nodes[0]) && !MP_PARSE_NODE_IS_ID(pns->nodes[0]))
                || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_string)
                || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_bytes)) {
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
            } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns1->nodes[0], PN_testlist_star_expr)
                && MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_testlist_star_expr)
                && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns1->nodes[0]) == 3
                && MP_PARSE_NODE_STRUCT_NUM_NODES((mp_parse_node_struct_t*)pns->nodes[0]) == 3) {
                // optimisation for a, b, c = d, e, f; to match CPython's optimisation
                mp_parse_node_struct_t* pns10 = (mp_parse_node_struct_t*)pns1->nodes[0];
                mp_parse_node_struct_t* pns0 = (mp_parse_node_struct_t*)pns->nodes[0];
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
            } else {
                no_optimisation:
                compile_node(comp, pns1->nodes[0]); // rhs
                c_assign(comp, pns->nodes[0], ASSIGN_STORE); // lhs store
            }
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

STATIC void c_binary_op(compiler_t *comp, mp_parse_node_struct_t *pns, mp_binary_op_t binary_op) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i < num_nodes; i += 1) {
        compile_node(comp, pns->nodes[i]);
        EMIT_ARG(binary_op, binary_op);
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
    // TODO default params etc for lambda; possibly just use funcdef code
    //mp_parse_node_t pn_params = pns->nodes[0];
    //mp_parse_node_t pn_body = pns->nodes[1];

    if (comp->pass == MP_PASS_SCOPE) {
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

STATIC void compile_or_test(compiler_t *comp, mp_parse_node_struct_t *pns) {
    uint l_end = comp_next_label(comp);
    int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < n; i += 1) {
        compile_node(comp, pns->nodes[i]);
        if (i + 1 < n) {
            EMIT_ARG(jump_if_true_or_pop, l_end);
        }
    }
    EMIT_ARG(label_assign, l_end);
}

STATIC void compile_and_test(compiler_t *comp, mp_parse_node_struct_t *pns) {
    uint l_end = comp_next_label(comp);
    int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    for (int i = 0; i < n; i += 1) {
        compile_node(comp, pns->nodes[i]);
        if (i + 1 < n) {
            EMIT_ARG(jump_if_false_or_pop, l_end);
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
        } else if (MP_PARSE_NODE_IS_STRUCT(pns->nodes[i])) {
            mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns->nodes[i];
            int kind = MP_PARSE_NODE_STRUCT_KIND(pns2);
            if (kind == PN_comp_op_not_in) {
                EMIT_ARG(binary_op, MP_BINARY_OP_NOT_IN);
            } else if (kind == PN_comp_op_is) {
                if (MP_PARSE_NODE_IS_NULL(pns2->nodes[0])) {
                    EMIT_ARG(binary_op, MP_BINARY_OP_IS);
                } else {
                    EMIT_ARG(binary_op, MP_BINARY_OP_IS_NOT);
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

STATIC void compile_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    c_binary_op(comp, pns, MP_BINARY_OP_OR);
}

STATIC void compile_xor_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    c_binary_op(comp, pns, MP_BINARY_OP_XOR);
}

STATIC void compile_and_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    c_binary_op(comp, pns, MP_BINARY_OP_AND);
}

STATIC void compile_shift_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_DBL_LESS)) {
            EMIT_ARG(binary_op, MP_BINARY_OP_LSHIFT);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_DBL_MORE)) {
            EMIT_ARG(binary_op, MP_BINARY_OP_RSHIFT);
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

STATIC void compile_arith_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_PLUS)) {
            EMIT_ARG(binary_op, MP_BINARY_OP_ADD);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_MINUS)) {
            EMIT_ARG(binary_op, MP_BINARY_OP_SUBTRACT);
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

STATIC void compile_term(compiler_t *comp, mp_parse_node_struct_t *pns) {
    int num_nodes = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    compile_node(comp, pns->nodes[0]);
    for (int i = 1; i + 1 < num_nodes; i += 2) {
        compile_node(comp, pns->nodes[i + 1]);
        if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_STAR)) {
            EMIT_ARG(binary_op, MP_BINARY_OP_MULTIPLY);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_DBL_SLASH)) {
            EMIT_ARG(binary_op, MP_BINARY_OP_FLOOR_DIVIDE);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_SLASH)) {
            EMIT_ARG(binary_op, MP_BINARY_OP_TRUE_DIVIDE);
        } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[i], MP_TOKEN_OP_PERCENT)) {
            EMIT_ARG(binary_op, MP_BINARY_OP_MODULO);
        } else {
            // shouldn't happen
            assert(0);
        }
    }
}

STATIC void compile_factor_2(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[1]);
    if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_PLUS)) {
        EMIT_ARG(unary_op, MP_UNARY_OP_POSITIVE);
    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_MINUS)) {
        EMIT_ARG(unary_op, MP_UNARY_OP_NEGATIVE);
    } else if (MP_PARSE_NODE_IS_TOKEN_KIND(pns->nodes[0], MP_TOKEN_OP_TILDE)) {
        EMIT_ARG(unary_op, MP_UNARY_OP_INVERT);
    } else {
        // shouldn't happen
        assert(0);
    }
}

STATIC void compile_power(compiler_t *comp, mp_parse_node_struct_t *pns) {
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
        // look for first argument to function (assumes it's "self")
        for (int i = 0; i < comp->scope_cur->id_info_len; i++) {
            if (comp->scope_cur->id_info[i].flags & ID_FLAG_IS_PARAM) {
                // first argument found; load it and call super
                EMIT_ARG(load_fast, MP_QSTR_, comp->scope_cur->id_info[i].local_num);
                EMIT_ARG(call_function, 2, 0, 0);
                return;
            }
        }
        compile_syntax_error(comp, MP_PARSE_NODE_NULL, "super() call cannot find self"); // really a TypeError
        return;
    }
#endif

    // get the list of arguments
    mp_parse_node_t *args;
    int n_args = list_get(&pn_arglist, PN_arglist, &args);

    // compile the arguments
    // Rather than calling compile_node on the list, we go through the list of args
    // explicitly here so that we can count the number of arguments and give sensible
    // error messages.
    int n_positional = n_positional_extra;
    uint n_keyword = 0;
    uint star_flags = 0;
    for (int i = 0; i < n_args; i++) {
        if (MP_PARSE_NODE_IS_STRUCT(args[i])) {
            mp_parse_node_struct_t *pns_arg = (mp_parse_node_struct_t*)args[i];
            if (MP_PARSE_NODE_STRUCT_KIND(pns_arg) == PN_arglist_star) {
                if (star_flags & MP_EMIT_STAR_FLAG_SINGLE) {
                    compile_syntax_error(comp, (mp_parse_node_t)pns_arg, "can't have multiple *x");
                    return;
                }
                star_flags |= MP_EMIT_STAR_FLAG_SINGLE;
                compile_node(comp, pns_arg->nodes[0]);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns_arg) == PN_arglist_dbl_star) {
                if (star_flags & MP_EMIT_STAR_FLAG_DOUBLE) {
                    compile_syntax_error(comp, (mp_parse_node_t)pns_arg, "can't have multiple **x");
                    return;
                }
                star_flags |= MP_EMIT_STAR_FLAG_DOUBLE;
                compile_node(comp, pns_arg->nodes[0]);
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns_arg) == PN_argument) {
                assert(MP_PARSE_NODE_IS_STRUCT(pns_arg->nodes[1])); // should always be
                mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)pns_arg->nodes[1];
                if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_argument_3) {
                    if (!MP_PARSE_NODE_IS_ID(pns_arg->nodes[0])) {
                        compile_syntax_error(comp, (mp_parse_node_t)pns_arg, "LHS of keyword arg must be an id");
                        return;
                    }
                    EMIT_ARG(load_const_str, MP_PARSE_NODE_LEAF_ARG(pns_arg->nodes[0]), false);
                    compile_node(comp, pns2->nodes[0]);
                    n_keyword += 1;
                } else {
                    assert(MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_comp_for); // should always be
                    compile_comprehension(comp, pns_arg, SCOPE_GEN_EXPR);
                    n_positional++;
                }
            } else {
                goto normal_argument;
            }
        } else {
            normal_argument:
            if (n_keyword > 0) {
                compile_syntax_error(comp, args[i], "non-keyword arg after keyword arg");
                return;
            }
            compile_node(comp, args[i]);
            n_positional++;
        }
    }

    // emit the function/method call
    if (is_method_call) {
        EMIT_ARG(call_method, n_positional, n_keyword, star_flags);
    } else {
        EMIT_ARG(call_function, n_positional, n_keyword, star_flags);
    }
}

STATIC void compile_power_trailers(compiler_t *comp, mp_parse_node_struct_t *pns) {
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

STATIC void compile_power_dbl_star(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]);
    EMIT_ARG(binary_op, MP_BINARY_OP_POWER);
}

STATIC void compile_atom_string(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // a list of strings

    // check type of list (string or bytes) and count total number of bytes
    int n = MP_PARSE_NODE_STRUCT_NUM_NODES(pns);
    int n_bytes = 0;
    int string_kind = MP_PARSE_NODE_NULL;
    for (int i = 0; i < n; i++) {
        int pn_kind;
        if (MP_PARSE_NODE_IS_LEAF(pns->nodes[i])) {
            pn_kind = MP_PARSE_NODE_LEAF_KIND(pns->nodes[i]);
            assert(pn_kind == MP_PARSE_NODE_STRING || pn_kind == MP_PARSE_NODE_BYTES);
            n_bytes += qstr_len(MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]));
        } else {
            assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[i]));
            mp_parse_node_struct_t *pns_string = (mp_parse_node_struct_t*)pns->nodes[i];
            if (MP_PARSE_NODE_STRUCT_KIND(pns_string) == PN_string) {
                pn_kind = MP_PARSE_NODE_STRING;
            } else {
                assert(MP_PARSE_NODE_STRUCT_KIND(pns_string) == PN_bytes);
                pn_kind = MP_PARSE_NODE_BYTES;
            }
            n_bytes += (mp_uint_t)pns_string->nodes[1];
        }
        if (i == 0) {
            string_kind = pn_kind;
        } else if (pn_kind != string_kind) {
            compile_syntax_error(comp, (mp_parse_node_t)pns, "cannot mix bytes and nonbytes literals");
            return;
        }
    }

    // if we are not in the last pass, just load a dummy object
    if (comp->pass != MP_PASS_EMIT) {
        EMIT_ARG(load_const_obj, mp_const_none);
        return;
    }

    // concatenate string/bytes
    vstr_t vstr;
    vstr_init_len(&vstr, n_bytes);
    byte *s_dest = (byte*)vstr.buf;
    for (int i = 0; i < n; i++) {
        if (MP_PARSE_NODE_IS_LEAF(pns->nodes[i])) {
            mp_uint_t s_len;
            const byte *s = qstr_data(MP_PARSE_NODE_LEAF_ARG(pns->nodes[i]), &s_len);
            memcpy(s_dest, s, s_len);
            s_dest += s_len;
        } else {
            mp_parse_node_struct_t *pns_string = (mp_parse_node_struct_t*)pns->nodes[i];
            memcpy(s_dest, (const char*)pns_string->nodes[0], (mp_uint_t)pns_string->nodes[1]);
            s_dest += (mp_uint_t)pns_string->nodes[1];
        }
    }

    // load the object
    EMIT_ARG(load_const_obj, mp_obj_new_str_from_vstr(string_kind == MP_PARSE_NODE_STRING ? &mp_type_str : &mp_type_bytes, &vstr));
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
    EMIT(get_iter);
    EMIT_ARG(call_function, 1, 0, 0);
}

STATIC void compile_atom_paren(compiler_t *comp, mp_parse_node_struct_t *pns) {
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

STATIC void compile_atom_bracket(compiler_t *comp, mp_parse_node_struct_t *pns) {
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

STATIC void compile_atom_brace(compiler_t *comp, mp_parse_node_struct_t *pns) {
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
                if (!MICROPY_PY_BUILTINS_SET || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_dictorsetmaker_item)) {
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
                    mp_parse_node_t pn_i = nodes[i];
                    bool is_key_value = MP_PARSE_NODE_IS_STRUCT_KIND(pn_i, PN_dictorsetmaker_item);
                    compile_node(comp, pn_i);
                    if (is_dict) {
                        if (!is_key_value) {
                            compile_syntax_error(comp, (mp_parse_node_t)pns, "expecting key:value for dictionary");
                            return;
                        }
                        EMIT(store_map);
                    } else {
                        if (is_key_value) {
                            compile_syntax_error(comp, (mp_parse_node_t)pns, "expecting just a value for set");
                            return;
                        }
                    }
                }

                #if MICROPY_PY_BUILTINS_SET
                // if it's a set, build it
                if (!is_dict) {
                    EMIT_ARG(build_set, 1 + n);
                }
                #endif
            } else if (MP_PARSE_NODE_STRUCT_KIND(pns1) == PN_comp_for) {
                // dict/set comprehension
                if (!MICROPY_PY_BUILTINS_SET || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_dictorsetmaker_item)) {
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
        #if MICROPY_PY_BUILTINS_SET
        compile_node(comp, pn);
        EMIT_ARG(build_set, 1);
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
    EMIT(load_subscr);
}

STATIC void compile_trailer_period(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // object who's attribute we want is on top of stack
    EMIT_ARG(load_attr, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0])); // attribute to get
}

#if MICROPY_PY_BUILTINS_SLICE
STATIC void compile_subscript_3_helper(compiler_t *comp, mp_parse_node_struct_t *pns) {
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

STATIC void compile_subscript_2(compiler_t *comp, mp_parse_node_struct_t *pns) {
    compile_node(comp, pns->nodes[0]); // start of slice
    assert(MP_PARSE_NODE_IS_STRUCT(pns->nodes[1])); // should always be
    compile_subscript_3_helper(comp, (mp_parse_node_struct_t*)pns->nodes[1]);
}

STATIC void compile_subscript_3(compiler_t *comp, mp_parse_node_struct_t *pns) {
    EMIT_ARG(load_const_tok, MP_TOKEN_KW_NONE);
    compile_subscript_3_helper(comp, pns);
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
    EMIT_ARG(store_id, cname);
}

STATIC void compile_yield_expr(compiler_t *comp, mp_parse_node_struct_t *pns) {
    if (comp->scope_cur->kind != SCOPE_FUNCTION && comp->scope_cur->kind != SCOPE_LAMBDA) {
        compile_syntax_error(comp, (mp_parse_node_t)pns, "'yield' outside function");
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

STATIC void compile_string(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // only create and load the actual str object on the last pass
    if (comp->pass != MP_PASS_EMIT) {
        EMIT_ARG(load_const_obj, mp_const_none);
    } else {
        EMIT_ARG(load_const_obj, mp_obj_new_str((const char*)pns->nodes[0], (mp_uint_t)pns->nodes[1], false));
    }
}

STATIC void compile_bytes(compiler_t *comp, mp_parse_node_struct_t *pns) {
    // only create and load the actual bytes object on the last pass
    if (comp->pass != MP_PASS_EMIT) {
        EMIT_ARG(load_const_obj, mp_const_none);
    } else {
        EMIT_ARG(load_const_obj, mp_obj_new_bytes((const byte*)pns->nodes[0], (mp_uint_t)pns->nodes[1]));
    }
}

typedef void (*compile_function_t)(compiler_t*, mp_parse_node_struct_t*);
STATIC compile_function_t compile_function[] = {
#define nc NULL
#define c(f) compile_##f
#define DEF_RULE(rule, comp, kind, ...) comp,
#include "py/grammar.h"
#undef nc
#undef c
#undef DEF_RULE
    NULL,
    compile_string,
    compile_bytes,
};

STATIC void compile_node(compiler_t *comp, mp_parse_node_t pn) {
    if (MP_PARSE_NODE_IS_NULL(pn)) {
        // pass
    } else if (MP_PARSE_NODE_IS_SMALL_INT(pn)) {
        mp_int_t arg = MP_PARSE_NODE_LEAF_SMALL_INT(pn);
        EMIT_ARG(load_const_small_int, arg);
    } else if (MP_PARSE_NODE_IS_LEAF(pn)) {
        mp_uint_t arg = MP_PARSE_NODE_LEAF_ARG(pn);
        switch (MP_PARSE_NODE_LEAF_KIND(pn)) {
            case MP_PARSE_NODE_ID: EMIT_ARG(load_id, arg); break;
            case MP_PARSE_NODE_INTEGER: EMIT_ARG(load_const_int, arg); break;
            case MP_PARSE_NODE_DECIMAL: EMIT_ARG(load_const_dec, arg); break;
            case MP_PARSE_NODE_STRING: EMIT_ARG(load_const_str, arg, false); break;
            case MP_PARSE_NODE_BYTES: EMIT_ARG(load_const_str, arg, true); break;
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
        EMIT_ARG(set_line_number, pns->source_line);
        compile_function_t f = compile_function[MP_PARSE_NODE_STRUCT_KIND(pns)];
        if (f == NULL) {
#if MICROPY_DEBUG_PRINTERS
            printf("node %u cannot be compiled\n", (uint)MP_PARSE_NODE_STRUCT_KIND(pns));
            mp_parse_node_print(pn, 0);
#endif
            compile_syntax_error(comp, pn, "internal compiler error");
        } else {
            f(comp, pns);
        }
    }
}

STATIC void compile_scope_func_lambda_param(compiler_t *comp, mp_parse_node_t pn, pn_kind_t pn_name, pn_kind_t pn_star, pn_kind_t pn_dbl_star) {
    // TODO verify that *k and **k are last etc
    qstr param_name = MP_QSTR_NULL;
    uint param_flag = ID_FLAG_IS_PARAM;
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
        mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
        if (MP_PARSE_NODE_STRUCT_KIND(pns) == pn_name) {
            param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            if (comp->have_star) {
                // comes after a star, so counts as a keyword-only parameter
                comp->scope_cur->num_kwonly_args += 1;
            } else {
                // comes before a star, so counts as a positional parameter
                comp->scope_cur->num_pos_args += 1;
            }
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == pn_star) {
            comp->have_star = true;
            param_flag = ID_FLAG_IS_PARAM | ID_FLAG_IS_STAR_PARAM;
            if (MP_PARSE_NODE_IS_NULL(pns->nodes[0])) {
                // bare star
                // TODO see http://www.python.org/dev/peps/pep-3102/
                //assert(comp->scope_cur->num_dict_params == 0);
            } else if (MP_PARSE_NODE_IS_ID(pns->nodes[0])) {
                // named star
                comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARARGS;
                param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            } else if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_tfpdef)) {
                // named star with possible annotation
                comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARARGS;
                pns = (mp_parse_node_struct_t*)pns->nodes[0];
                param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            } else {
                // shouldn't happen
                assert(0);
            }
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == pn_dbl_star) {
            param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
            param_flag = ID_FLAG_IS_PARAM | ID_FLAG_IS_DBL_STAR_PARAM;
            comp->scope_cur->scope_flags |= MP_SCOPE_FLAG_VARKEYWORDS;
        } else {
            // TODO anything to implement?
            assert(0);
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
    }
}

STATIC void compile_scope_func_param(compiler_t *comp, mp_parse_node_t pn) {
    compile_scope_func_lambda_param(comp, pn, PN_typedargslist_name, PN_typedargslist_star, PN_typedargslist_dbl_star);
}

STATIC void compile_scope_lambda_param(compiler_t *comp, mp_parse_node_t pn) {
    compile_scope_func_lambda_param(comp, pn, PN_varargslist_name, PN_varargslist_star, PN_varargslist_dbl_star);
}

STATIC void compile_scope_func_annotations(compiler_t *comp, mp_parse_node_t pn) {
    if (!MP_PARSE_NODE_IS_STRUCT(pn)) {
        // no annotation
        return;
    }

    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)pn;
    if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_typedargslist_name) {
        // named parameter with possible annotation
        // fallthrough
    } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_typedargslist_star) {
        if (MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_tfpdef)) {
            // named star with possible annotation
            pns = (mp_parse_node_struct_t*)pns->nodes[0];
            // fallthrough
        } else {
            // no annotation
            return;
        }
    } else if (MP_PARSE_NODE_STRUCT_KIND(pns) == PN_typedargslist_dbl_star) {
        // double star with possible annotation
        // fallthrough
    } else {
        // no annotation
        return;
    }

    mp_parse_node_t pn_annotation = pns->nodes[1];

    if (!MP_PARSE_NODE_IS_NULL(pn_annotation)) {
        #if MICROPY_EMIT_NATIVE
        qstr param_name = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]);
        id_info_t *id_info = scope_find(comp->scope_cur, param_name);
        assert(id_info != NULL);

        if (comp->scope_cur->emit_options == MP_EMIT_OPT_VIPER) {
            if (MP_PARSE_NODE_IS_ID(pn_annotation)) {
                qstr arg_type = MP_PARSE_NODE_LEAF_ARG(pn_annotation);
                EMIT_ARG(set_native_type, MP_EMIT_NATIVE_TYPE_ARG, id_info->local_num, arg_type);
            } else {
                compile_syntax_error(comp, pn_annotation, "parameter annotation must be an identifier");
            }
        }
        #endif // MICROPY_EMIT_NATIVE
    }
}

STATIC void compile_scope_comp_iter(compiler_t *comp, mp_parse_node_t pn_iter, mp_parse_node_t pn_inner_expr, int l_top, int for_depth) {
    tail_recursion:
    if (MP_PARSE_NODE_IS_NULL(pn_iter)) {
        // no more nested if/for; compile inner expression
        compile_node(comp, pn_inner_expr);
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
        uint l_end2 = comp_next_label(comp);
        uint l_top2 = comp_next_label(comp);
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

STATIC void check_for_doc_string(compiler_t *comp, mp_parse_node_t pn) {
#if MICROPY_EMIT_CPYTHON || MICROPY_ENABLE_DOC_STRING
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
        if ((MP_PARSE_NODE_IS_LEAF(pns->nodes[0])
                && MP_PARSE_NODE_LEAF_KIND(pns->nodes[0]) == MP_PARSE_NODE_STRING)
            || MP_PARSE_NODE_IS_STRUCT_KIND(pns->nodes[0], PN_string)) {
                // compile the doc string
                compile_node(comp, pns->nodes[0]);
                // store the doc string
                EMIT_ARG(store_id, MP_QSTR___doc__);
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
    comp->next_label = 1;
    EMIT_ARG(start_pass, pass, scope);

    if (comp->pass == MP_PASS_SCOPE) {
        // reset maximum stack sizes in scope
        // they will be computed in this first pass
        scope->stack_size = 0;
        scope->exc_stack_size = 0;
    }

#if MICROPY_EMIT_CPYTHON
    if (comp->pass == MP_PASS_EMIT) {
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
        if (comp->pass == MP_PASS_SCOPE) {
            comp->have_star = false;
            apply_to_single_or_list(comp, pns->nodes[1], PN_typedargslist, compile_scope_func_param);
        } else {
            // compile annotations; only needed on latter compiler passes

            // argument annotations
            apply_to_single_or_list(comp, pns->nodes[1], PN_typedargslist, compile_scope_func_annotations);

            // pns->nodes[2] is return/whole function annotation
            mp_parse_node_t pn_annotation = pns->nodes[2];
            if (!MP_PARSE_NODE_IS_NULL(pn_annotation)) {
                #if MICROPY_EMIT_NATIVE
                if (scope->emit_options == MP_EMIT_OPT_VIPER) {
                    // nodes[2] can be null or a test-expr
                    if (MP_PARSE_NODE_IS_ID(pn_annotation)) {
                        qstr ret_type = MP_PARSE_NODE_LEAF_ARG(pn_annotation);
                        EMIT_ARG(set_native_type, MP_EMIT_NATIVE_TYPE_RETURN, 0, ret_type);
                    } else {
                        compile_syntax_error(comp, pn_annotation, "return annotation must be an identifier");
                    }
                }
                #endif // MICROPY_EMIT_NATIVE
            }
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
#if MICROPY_EMIT_CPYTHON
        qstr qstr_arg = QSTR_FROM_STR_STATIC(".0");
#else
        qstr qstr_arg = MP_QSTR_;
#endif
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

        uint l_end = comp_next_label(comp);
        uint l_top = comp_next_label(comp);
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

        if (comp->pass == MP_PASS_SCOPE) {
            bool added;
            id_info_t *id_info = scope_find_or_add_id(scope, MP_QSTR___class__, &added);
            assert(added);
            id_info->kind = ID_INFO_KIND_LOCAL;
        }

        EMIT_ARG(load_id, MP_QSTR___name__);
        EMIT_ARG(store_id, MP_QSTR___module__);
        EMIT_ARG(load_const_str, MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]), false); // 0 is class name
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
        compile_syntax_error(comp, MP_PARSE_NODE_NULL, "inline assembler must be a function");
        return;
    }

    if (comp->pass > MP_PASS_SCOPE) {
        EMIT_INLINE_ASM_ARG(start_pass, comp->pass, comp->scope_cur);
    }

    // get the function definition parse node
    assert(MP_PARSE_NODE_IS_STRUCT(scope->pn));
    mp_parse_node_struct_t *pns = (mp_parse_node_struct_t*)scope->pn;
    assert(MP_PARSE_NODE_STRUCT_KIND(pns) == PN_funcdef);

    //qstr f_id = MP_PARSE_NODE_LEAF_ARG(pns->nodes[0]); // function name

    // parameters are in pns->nodes[1]
    if (comp->pass == MP_PASS_CODE_SIZE) {
        mp_parse_node_t *pn_params;
        int n_params = list_get(&pns->nodes[1], PN_typedargslist, &pn_params);
        scope->num_pos_args = EMIT_INLINE_ASM_ARG(count_params, n_params, pn_params);
    }

    assert(MP_PARSE_NODE_IS_NULL(pns->nodes[2])); // type

    mp_parse_node_t pn_body = pns->nodes[3]; // body
    mp_parse_node_t *nodes;
    int num = list_get(&pn_body, PN_suite_block_stmts, &nodes);

    /*
    if (comp->pass == MP_PASS_EMIT) {
        //printf("----\n");
        scope_print_info(scope);
    }
    */

    for (int i = 0; i < num; i++) {
        assert(MP_PARSE_NODE_IS_STRUCT(nodes[i]));
        mp_parse_node_struct_t *pns2 = (mp_parse_node_struct_t*)nodes[i];
        if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_pass_stmt) {
            // no instructions
            continue;
        } else if (MP_PARSE_NODE_STRUCT_KIND(pns2) == PN_expr_stmt) {
            // an instruction; fall through
        } else {
            // not an instruction; error
            compile_syntax_error(comp, nodes[i], "inline assembler expecting an instruction");
            return;
        }
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
        if (op == MP_QSTR_label) {
            if (!(n_args == 1 && MP_PARSE_NODE_IS_ID(pn_arg[0]))) {
                compile_syntax_error(comp, nodes[i], "inline assembler 'label' requires 1 argument");
                return;
            }
            uint lab = comp_next_label(comp);
            if (pass > MP_PASS_SCOPE) {
                EMIT_INLINE_ASM_ARG(label, lab, MP_PARSE_NODE_LEAF_ARG(pn_arg[0]));
            }
        } else if (op == MP_QSTR_align) {
            if (!(n_args == 1 && MP_PARSE_NODE_IS_SMALL_INT(pn_arg[0]))) {
                compile_syntax_error(comp, nodes[i], "inline assembler 'align' requires 1 argument");
                return;
            }
            if (pass > MP_PASS_SCOPE) {
                EMIT_INLINE_ASM_ARG(align, MP_PARSE_NODE_LEAF_SMALL_INT(pn_arg[0]));
            }
        } else if (op == MP_QSTR_data) {
            if (!(n_args >= 2 && MP_PARSE_NODE_IS_SMALL_INT(pn_arg[0]))) {
                compile_syntax_error(comp, nodes[i], "inline assembler 'data' requires at least 2 arguments");
                return;
            }
            if (pass > MP_PASS_SCOPE) {
                mp_int_t bytesize = MP_PARSE_NODE_LEAF_SMALL_INT(pn_arg[0]);
                for (uint j = 1; j < n_args; j++) {
                    if (!MP_PARSE_NODE_IS_SMALL_INT(pn_arg[j])) {
                        compile_syntax_error(comp, nodes[i], "inline assembler 'data' requires integer arguments");
                        return;
                    }
                    EMIT_INLINE_ASM_ARG(data, bytesize, MP_PARSE_NODE_LEAF_SMALL_INT(pn_arg[j]));
                }
            }
        } else {
            if (pass > MP_PASS_SCOPE) {
                EMIT_INLINE_ASM_ARG(op, op, n_args, pn_arg);
            }
        }
    }

    if (comp->pass > MP_PASS_SCOPE) {
        bool success = EMIT_INLINE_ASM(end_pass);
        if (!success) {
            // TODO get proper exception from inline assembler
            compile_syntax_error(comp, MP_PARSE_NODE_NULL, "inline assembler error");
        }
    }
}
#endif

STATIC void scope_compute_things(scope_t *scope) {
#if !MICROPY_EMIT_CPYTHON
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
#endif

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
        if (id->kind == ID_INFO_KIND_CELL && !(id->flags & ID_FLAG_IS_PARAM)) {
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
                    if (id2->kind == ID_INFO_KIND_FREE && id->qst == id2->qst) {
                        assert(!(id2->flags & ID_FLAG_IS_PARAM)); // free vars should not be params
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
                if (id->kind != ID_INFO_KIND_FREE || (id->flags & ID_FLAG_IS_PARAM)) {
                    id->local_num += num_free;
                }
            }
            scope->num_pos_args += num_free; // free vars are counted as params for passing them into the function
            scope->num_locals += num_free;
        }
#endif
    }

    // compute scope_flags

#if MICROPY_EMIT_CPYTHON
    // these flags computed here are for CPython compatibility only
    if (scope->kind == SCOPE_FUNCTION || scope->kind == SCOPE_LAMBDA || scope->kind == SCOPE_LIST_COMP || scope->kind == SCOPE_DICT_COMP || scope->kind == SCOPE_SET_COMP || scope->kind == SCOPE_GEN_EXPR) {
        assert(scope->parent != NULL);
        scope->scope_flags |= MP_SCOPE_FLAG_NEWLOCALS;
        scope->scope_flags |= MP_SCOPE_FLAG_OPTIMISED;
        if ((SCOPE_FUNCTION <= scope->parent->kind && scope->parent->kind <= SCOPE_SET_COMP) || (scope->parent->kind == SCOPE_CLASS && scope->parent->parent->kind == SCOPE_FUNCTION)) {
            scope->scope_flags |= MP_SCOPE_FLAG_NESTED;
        }
    }
#endif

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

mp_obj_t mp_compile(mp_parse_node_t pn, qstr source_file, uint emit_opt, bool is_repl) {
    compiler_t *comp = m_new0(compiler_t, 1);
    comp->source_file = source_file;
    comp->is_repl = is_repl;
    comp->compile_error = MP_OBJ_NULL;

    // optimise constants
    mp_map_t consts;
    mp_map_init(&consts, 0);
    pn = fold_constants(comp, pn, &consts);
    mp_map_deinit(&consts);

    // set the outer scope
    scope_t *module_scope = scope_new_and_link(comp, SCOPE_MODULE, pn, emit_opt);

    // compile pass 1
    comp->emit = emit_pass1_new();
    comp->emit_method_table = &emit_pass1_method_table;
    comp->emit_inline_asm = NULL;
    comp->emit_inline_asm_method_table = NULL;
    uint max_num_labels = 0;
    for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
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

    // compute some things related to scope and identifiers
    for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
        scope_compute_things(s);
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
    for (scope_t *s = comp->scope_head; s != NULL && comp->compile_error == MP_OBJ_NULL; s = s->next) {
        if (false) {
            // dummy

#if MICROPY_EMIT_INLINE_THUMB
        } else if (s->emit_options == MP_EMIT_OPT_ASM_THUMB) {
            // inline assembly for thumb
            if (emit_inline_thumb == NULL) {
                emit_inline_thumb = emit_inline_thumb_new(max_num_labels);
            }
            comp->emit = NULL;
            comp->emit_method_table = NULL;
            comp->emit_inline_asm = emit_inline_thumb;
            comp->emit_inline_asm_method_table = &emit_inline_thumb_method_table;
            compile_scope_inline_asm(comp, s, MP_PASS_CODE_SIZE);
            if (comp->compile_error == MP_OBJ_NULL) {
                compile_scope_inline_asm(comp, s, MP_PASS_EMIT);
            }
#endif

        } else {

            // choose the emit type

#if MICROPY_EMIT_CPYTHON
            comp->emit = emit_cpython_new(max_num_labels);
            comp->emit_method_table = &emit_cpython_method_table;
#else
            switch (s->emit_options) {

#if MICROPY_EMIT_NATIVE
                case MP_EMIT_OPT_NATIVE_PYTHON:
                case MP_EMIT_OPT_VIPER:
#if MICROPY_EMIT_X64
                    if (emit_native == NULL) {
                        emit_native = emit_native_x64_new(max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_x64_method_table;
#elif MICROPY_EMIT_X86
                    if (emit_native == NULL) {
                        emit_native = emit_native_x86_new(max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_x86_method_table;
#elif MICROPY_EMIT_THUMB
                    if (emit_native == NULL) {
                        emit_native = emit_native_thumb_new(max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_thumb_method_table;
#elif MICROPY_EMIT_ARM
                    if (emit_native == NULL) {
                        emit_native = emit_native_arm_new(max_num_labels);
                    }
                    comp->emit_method_table = &emit_native_arm_method_table;
#endif
                    comp->emit = emit_native;
                    EMIT_ARG(set_native_type, MP_EMIT_NATIVE_TYPE_ENABLE, s->emit_options == MP_EMIT_OPT_VIPER, 0);
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

    // free the emitters
#if !MICROPY_EMIT_CPYTHON
    if (emit_bc != NULL) {
        emit_bc_free(emit_bc);
    }
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
#endif // !MICROPY_EMIT_CPYTHON

    // free the parse tree
    mp_parse_node_free(pn);

    // free the scopes
    mp_raw_code_t *outer_raw_code = module_scope->raw_code;
    for (scope_t *s = module_scope; s;) {
        scope_t *next = s->next;
        scope_free(s);
        s = next;
    }

    // free the compiler
    mp_obj_t compile_error = comp->compile_error;
    m_del_obj(compiler_t, comp);

    if (compile_error != MP_OBJ_NULL) {
        return compile_error;
    } else {
#if MICROPY_EMIT_CPYTHON
        // can't create code, so just return true
        (void)outer_raw_code; // to suppress warning that outer_raw_code is unused
        return mp_const_true;
#else
        // return function that executes the outer module
        return mp_make_function_from_raw_code(outer_raw_code, MP_OBJ_NULL, MP_OBJ_NULL);
#endif
    }
}
