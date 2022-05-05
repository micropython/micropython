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

#include "py/emit.h"

#if MICROPY_ENABLE_COMPILER

#if MICROPY_EMIT_BYTECODE_USES_QSTR_TABLE
qstr_short_t mp_emit_common_use_qstr(mp_emit_common_t *emit, qstr qst) {
    mp_map_elem_t *elem = mp_map_lookup(&emit->qstr_map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    if (elem->value == MP_OBJ_NULL) {
        assert(emit->pass == MP_PASS_SCOPE);
        elem->value = MP_OBJ_NEW_SMALL_INT(emit->qstr_map.used - 1);
    }
    return MP_OBJ_SMALL_INT_VALUE(elem->value);
}
#endif

void mp_emit_common_get_id_for_modification(scope_t *scope, qstr qst) {
    // name adding/lookup
    id_info_t *id = scope_find_or_add_id(scope, qst, ID_INFO_KIND_GLOBAL_IMPLICIT);
    if (id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        if (SCOPE_IS_FUNC_LIKE(scope->kind)) {
            // rebind as a local variable
            id->kind = ID_INFO_KIND_LOCAL;
        } else {
            // mark this as assigned, to prevent it from being closed over
            id->kind = ID_INFO_KIND_GLOBAL_IMPLICIT_ASSIGNED;
        }
    }
}

void mp_emit_common_id_op(emit_t *emit, const mp_emit_method_table_id_ops_t *emit_method_table, scope_t *scope, qstr qst) {
    // assumes pass is greater than 1, ie that all identifiers are defined in the scope

    id_info_t *id = scope_find(scope, qst);
    assert(id != NULL);

    // call the emit backend with the correct code
    if (id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT || id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT_ASSIGNED) {
        emit_method_table->global(emit, qst, MP_EMIT_IDOP_GLOBAL_NAME);
    } else if (id->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
        emit_method_table->global(emit, qst, MP_EMIT_IDOP_GLOBAL_GLOBAL);
    } else if (id->kind == ID_INFO_KIND_LOCAL) {
        emit_method_table->local(emit, qst, id->local_num, MP_EMIT_IDOP_LOCAL_FAST);
    } else {
        assert(id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE);
        emit_method_table->local(emit, qst, id->local_num, MP_EMIT_IDOP_LOCAL_DEREF);
    }
}

#endif // MICROPY_ENABLE_COMPILER
