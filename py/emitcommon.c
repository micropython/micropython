#include <unistd.h>
#include <stdint.h>
#include <assert.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "lexer.h"
#include "parse.h"
#include "scope.h"
#include "runtime0.h"
#include "emit.h"

#define EMIT(fun, ...) (emit_method_table->fun(emit, __VA_ARGS__))

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
        EMIT(load_deref, qstr, id->local_num);
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
        EMIT(store_deref, qstr, id->local_num);
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
        EMIT(delete_deref, qstr, id->local_num);
    } else {
        assert(0);
    }
}
