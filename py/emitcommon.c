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

#include <assert.h>

#include "py/emit.h"

#if MICROPY_ENABLE_COMPILER

void mp_emit_common_get_id_for_load(scope_t *scope, qstr qst) {
    // name adding/lookup
    bool added;
    id_info_t *id = scope_find_or_add_id(scope, qst, &added);
    if (added) {
        id_info_t *id2 = scope_find_local_in_parent(scope, qst);
        if (id2 != NULL && (id2->kind == ID_INFO_KIND_LOCAL || id2->kind == ID_INFO_KIND_CELL || id2->kind == ID_INFO_KIND_FREE)) {
            id->kind = ID_INFO_KIND_FREE;
            scope_close_over_in_parents(scope, qst);
        } else {
            id->kind = ID_INFO_KIND_GLOBAL_IMPLICIT;
        }
    }
}

void mp_emit_common_get_id_for_modification(scope_t *scope, qstr qst) {
    // name adding/lookup
    bool added;
    id_info_t *id = scope_find_or_add_id(scope, qst, &added);
    if (added) {
        if (scope->kind == SCOPE_MODULE || scope->kind == SCOPE_CLASS) {
            id->kind = ID_INFO_KIND_GLOBAL_IMPLICIT;
        } else {
            id->kind = ID_INFO_KIND_LOCAL;
        }
    } else if (scope->kind >= SCOPE_FUNCTION && scope->kind <= SCOPE_GEN_EXPR && id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        // rebind as a local variable
        id->kind = ID_INFO_KIND_LOCAL;
    }
}

void mp_emit_common_id_op(emit_t *emit, const mp_emit_method_table_id_ops_t *emit_method_table, scope_t *scope, qstr qst) {
    // assumes pass is greater than 1, ie that all identifiers are defined in the scope

    id_info_t *id = scope_find(scope, qst);
    assert(id != NULL);

    // call the emit backend with the correct code
    if (id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        emit_method_table->name(emit, qst);
    } else if (id->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
        emit_method_table->global(emit, qst);
    } else if (id->kind == ID_INFO_KIND_LOCAL) {
        emit_method_table->fast(emit, qst, id->local_num);
    } else {
        assert(id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE);
        emit_method_table->deref(emit, qst, id->local_num);
    }
}

#endif // MICROPY_ENABLE_COMPILER
