#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "lexer.h"
#include "machine.h"
#include "parse.h"
#include "scope.h"
#include "runtime.h"
#include "emit.h"

#define EMIT(fun, arg...) (emit_method_table->fun(emit, ##arg))

void emit_common_load_id(emit_t *emit, const emit_method_table_t *emit_method_table, scope_t *scope, qstr qstr) {
    // assumes pass is greater than 1, ie that all identifiers are defined in the scope

    id_info_t *id = scope_find(scope, qstr);
    assert(id != NULL); // TODO can this ever fail?

    // call the emit backend with the correct code
    if (id == NULL || id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        EMIT(load_name, qstr);
    } else if (id->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
        EMIT(load_global, qstr);
    } else if (id->kind == ID_INFO_KIND_LOCAL) {
        EMIT(load_fast, qstr, id->local_num);
    } else if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
        EMIT(load_deref, qstr);
    } else {
        assert(0);
    }
}

void emit_common_store_id(emit_t *emit, const emit_method_table_t *emit_method_table, scope_t *scope, qstr qstr) {
    // assumes pass is greater than 1, ie that all identifiers are defined in the scope

    id_info_t *id = scope_find(scope, qstr);
    assert(id != NULL); // TODO can this ever fail?

    // call the emit backend with the correct code
    if (id == NULL || id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        EMIT(store_name, qstr);
    } else if (id->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
        EMIT(store_global, qstr);
    } else if (id->kind == ID_INFO_KIND_LOCAL) {
        EMIT(store_fast, qstr, id->local_num);
    } else if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
        EMIT(store_deref, qstr);
    } else {
        assert(0);
    }
}

void emit_common_delete_id(emit_t *emit, const emit_method_table_t *emit_method_table, scope_t *scope, qstr qstr) {
    // assumes pass is greater than 1, ie that all identifiers are defined in the scope

    id_info_t *id = scope_find(scope, qstr);
    assert(id != NULL); // TODO can this ever fail?

    // call the emit backend with the correct code
    if (id == NULL || id->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        EMIT(delete_name, qstr);
    } else if (id->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
        EMIT(delete_global, qstr);
    } else if (id->kind == ID_INFO_KIND_LOCAL) {
        EMIT(delete_fast, qstr, id->local_num);
    } else if (id->kind == ID_INFO_KIND_CELL || id->kind == ID_INFO_KIND_FREE) {
        EMIT(delete_deref, qstr);
    } else {
        assert(0);
    }
}
