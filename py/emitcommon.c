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

#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "py/emit.h"

#define EMIT(fun, ...) (emit_method_table->fun(emit, __VA_ARGS__))

void emit_common_load_id(emit_t *emit, const emit_method_table_t *emit_method_table, scope_t *scope, qstr qst) {
    // assumes pass is greater than 1, ie that all identifiers are defined in the scope

    id_info_t *id = scope_find(scope, qst);
    assert(id != NULL); // TODO can this ever fail?

    // call the emit backend with the correct code
    if (id == NULL || id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        EMIT(load_name, qst);
    } else if (id->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
        EMIT(load_global, qst);
    } else if (id->kind == ID_INFO_KIND_LOCAL) {
        EMIT(load_fast, qst, id->local_num);
    } else if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
        EMIT(load_deref, qst, id->local_num);
    } else {
        assert(0);
    }
}

void emit_common_store_id(emit_t *emit, const emit_method_table_t *emit_method_table, scope_t *scope, qstr qst) {
    // assumes pass is greater than 1, ie that all identifiers are defined in the scope

    id_info_t *id = scope_find(scope, qst);
    assert(id != NULL); // TODO can this ever fail?

    // call the emit backend with the correct code
    if (id == NULL || id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        EMIT(store_name, qst);
    } else if (id->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
        EMIT(store_global, qst);
    } else if (id->kind == ID_INFO_KIND_LOCAL) {
        EMIT(store_fast, qst, id->local_num);
    } else if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
        EMIT(store_deref, qst, id->local_num);
    } else {
        assert(0);
    }
}

void emit_common_delete_id(emit_t *emit, const emit_method_table_t *emit_method_table, scope_t *scope, qstr qst) {
    // assumes pass is greater than 1, ie that all identifiers are defined in the scope

    id_info_t *id = scope_find(scope, qst);
    assert(id != NULL); // TODO can this ever fail?

    // call the emit backend with the correct code
    if (id == NULL || id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        EMIT(delete_name, qst);
    } else if (id->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
        EMIT(delete_global, qst);
    } else if (id->kind == ID_INFO_KIND_LOCAL) {
        EMIT(delete_fast, qst, id->local_num);
    } else if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
        EMIT(delete_deref, qst, id->local_num);
    } else {
        assert(0);
    }
}
