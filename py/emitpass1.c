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

struct _emit_t {
    scope_t *scope;
};

emit_t *emit_pass1_new(void) {
    emit_t *emit = m_new(emit_t, 1);
    return emit;
}

void emit_pass1_free(emit_t *emit) {
    m_del_obj(emit_t, emit);
}

STATIC void emit_pass1_dummy(emit_t *emit) {
    (void)emit;
}

STATIC void emit_pass1_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    assert(pass == MP_PASS_SCOPE);
    emit->scope = scope;
}

STATIC void emit_pass1_end_pass(emit_t *emit) {
    (void)emit;
}

STATIC bool emit_pass1_last_emit_was_return_value(emit_t *emit) {
    (void)emit;
    return false;
}

STATIC void emit_pass1_load_id(emit_t *emit, qstr qst) {
    // name adding/lookup
    bool added;
    id_info_t *id = scope_find_or_add_id(emit->scope, qst, &added);
    if (added) {
#if MICROPY_EMIT_CPYTHON
        if (qst == MP_QSTR_super && emit->scope->kind == SCOPE_FUNCTION) {
            // special case, super is a global, and also counts as use of __class__
            id->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
            id_info_t *id2 = scope_find_local_in_parent(emit->scope, MP_QSTR___class__);
            if (id2 != NULL) {
                id2 = scope_find_or_add_id(emit->scope, MP_QSTR___class__, &added);
                if (added) {
                    id2->kind = ID_INFO_KIND_FREE;
                    scope_close_over_in_parents(emit->scope, MP_QSTR___class__);
                }
            }
        } else
#endif
        {
            id_info_t *id2 = scope_find_local_in_parent(emit->scope, qst);
            if (id2 != NULL && (id2->kind == ID_INFO_KIND_LOCAL || id2->kind == ID_INFO_KIND_CELL || id2->kind == ID_INFO_KIND_FREE)) {
                id->kind = ID_INFO_KIND_FREE;
                scope_close_over_in_parents(emit->scope, qst);
            } else {
                id->kind = ID_INFO_KIND_GLOBAL_IMPLICIT;
            }
        }
    }
}

STATIC id_info_t *get_id_for_modification(scope_t *scope, qstr qst) {
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

    assert(id != NULL); // TODO can this ever fail?

    return id;
}

STATIC void emit_pass1_store_id(emit_t *emit, qstr qst) {
    get_id_for_modification(emit->scope, qst);
}

STATIC void emit_pass1_delete_id(emit_t *emit, qstr qst) {
    id_info_t *id = get_id_for_modification(emit->scope, qst);
    // this flag is unused
    //id->flags |= ID_FLAG_IS_DELETED;
    (void)id; // suppress compiler warning
}

const emit_method_table_t emit_pass1_method_table = {
    (void*)emit_pass1_dummy,
    emit_pass1_start_pass,
    emit_pass1_end_pass,
    emit_pass1_last_emit_was_return_value,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,

    emit_pass1_load_id,
    emit_pass1_store_id,
    emit_pass1_delete_id,

    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    #if MICROPY_PY_BUILTINS_SET
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    #endif
    #if MICROPY_PY_BUILTINS_SLICE
    (void*)emit_pass1_dummy,
    #endif
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,

    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,

#if MICROPY_EMIT_CPYTHON
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
#endif
};
