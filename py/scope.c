/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include <assert.h>

#include "py/scope.h"

#if MICROPY_ENABLE_COMPILER

// These low numbered qstrs should fit in 8 bits.  See assertions below.
STATIC const uint8_t scope_simple_name_table[] = {
    [SCOPE_MODULE] = MP_QSTR__lt_module_gt_,
    [SCOPE_LAMBDA] = MP_QSTR__lt_lambda_gt_,
    [SCOPE_LIST_COMP] = MP_QSTR__lt_listcomp_gt_,
    [SCOPE_DICT_COMP] = MP_QSTR__lt_dictcomp_gt_,
    [SCOPE_SET_COMP] = MP_QSTR__lt_setcomp_gt_,
    [SCOPE_GEN_EXPR] = MP_QSTR__lt_genexpr_gt_,
};

scope_t *scope_new(scope_kind_t kind, mp_parse_node_t pn, qstr source_file, mp_uint_t emit_options) {
    // Make sure those qstrs indeed fit in an uint8_t.
    MP_STATIC_ASSERT(MP_QSTR__lt_module_gt_ <= UINT8_MAX);
    MP_STATIC_ASSERT(MP_QSTR__lt_lambda_gt_ <= UINT8_MAX);
    MP_STATIC_ASSERT(MP_QSTR__lt_listcomp_gt_ <= UINT8_MAX);
    MP_STATIC_ASSERT(MP_QSTR__lt_dictcomp_gt_ <= UINT8_MAX);
    MP_STATIC_ASSERT(MP_QSTR__lt_setcomp_gt_ <= UINT8_MAX);
    MP_STATIC_ASSERT(MP_QSTR__lt_genexpr_gt_ <= UINT8_MAX);

    scope_t *scope = m_new0(scope_t, 1);
    scope->kind = kind;
    scope->pn = pn;
    scope->source_file = source_file;
    if (kind == SCOPE_FUNCTION || kind == SCOPE_CLASS) {
        assert(MP_PARSE_NODE_IS_STRUCT(pn));
        scope->simple_name = MP_PARSE_NODE_LEAF_ARG(((mp_parse_node_struct_t *)pn)->nodes[0]);
    } else {
        scope->simple_name = scope_simple_name_table[kind];
    }
    scope->raw_code = mp_emit_glue_new_raw_code();
    scope->emit_options = emit_options;
    scope->id_info_alloc = MICROPY_ALLOC_SCOPE_ID_INIT;
    scope->id_info = m_new(id_info_t, scope->id_info_alloc);

    return scope;
}

void scope_free(scope_t *scope) {
    m_del(id_info_t, scope->id_info, scope->id_info_alloc);
    m_del(scope_t, scope, 1);
}

id_info_t *scope_find_or_add_id(scope_t *scope, qstr qst, id_info_kind_t kind) {
    id_info_t *id_info = scope_find(scope, qst);
    if (id_info != NULL) {
        return id_info;
    }

    // make sure we have enough memory
    if (scope->id_info_len >= scope->id_info_alloc) {
        scope->id_info = m_renew(id_info_t, scope->id_info, scope->id_info_alloc, scope->id_info_alloc + MICROPY_ALLOC_SCOPE_ID_INC);
        scope->id_info_alloc += MICROPY_ALLOC_SCOPE_ID_INC;
    }

    // add new id to end of array of all ids; this seems to match CPython
    // important thing is that function arguments are first, but that is
    // handled by the compiler because it adds arguments before compiling the body
    id_info = &scope->id_info[scope->id_info_len++];

    id_info->kind = kind;
    id_info->flags = 0;
    id_info->local_num = 0;
    id_info->qst = qst;
    return id_info;
}

id_info_t *scope_find(scope_t *scope, qstr qst) {
    for (mp_uint_t i = 0; i < scope->id_info_len; i++) {
        if (scope->id_info[i].qst == qst) {
            return &scope->id_info[i];
        }
    }
    return NULL;
}

id_info_t *scope_find_global(scope_t *scope, qstr qst) {
    while (scope->parent != NULL) {
        scope = scope->parent;
    }
    return scope_find(scope, qst);
}

STATIC void scope_close_over_in_parents(scope_t *scope, qstr qst) {
    assert(scope->parent != NULL); // we should have at least 1 parent
    for (scope_t *s = scope->parent;; s = s->parent) {
        assert(s->parent != NULL); // we should not get to the outer scope
        id_info_t *id = scope_find_or_add_id(s, qst, ID_INFO_KIND_UNDECIDED);
        if (id->kind == ID_INFO_KIND_UNDECIDED) {
            // variable not previously declared in this scope, so declare it as free and keep searching parents
            id->kind = ID_INFO_KIND_FREE;
        } else {
            // variable is declared in this scope, so finish
            if (id->kind == ID_INFO_KIND_LOCAL) {
                // variable local to this scope, close it over
                id->kind = ID_INFO_KIND_CELL;
            } else {
                // ID_INFO_KIND_FREE: variable already closed over in a parent scope
                // ID_INFO_KIND_CELL: variable already closed over in this scope
                assert(id->kind == ID_INFO_KIND_FREE || id->kind == ID_INFO_KIND_CELL);
            }
            return;
        }
    }
}

void scope_check_to_close_over(scope_t *scope, id_info_t *id) {
    if (scope->parent != NULL) {
        for (scope_t *s = scope->parent; s->parent != NULL; s = s->parent) {
            id_info_t *id2 = scope_find(s, id->qst);
            if (id2 != NULL) {
                if (id2->kind == ID_INFO_KIND_LOCAL || id2->kind == ID_INFO_KIND_CELL || id2->kind == ID_INFO_KIND_FREE) {
                    id->kind = ID_INFO_KIND_FREE;
                    scope_close_over_in_parents(scope, id->qst);
                }
                break;
            }
        }
    }
}

#endif // MICROPY_ENABLE_COMPILER
