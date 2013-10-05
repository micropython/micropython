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

void emit_common_load_id(pass_kind_t pass, scope_t *scope, emit_t *emit, const emit_method_table_t *emit_method_table, qstr qstr___class__, qstr qstr) {
    id_info_t *id_info = NULL;
    if (pass == PASS_1) {
        // name adding/lookup
        bool added;
        id_info = scope_find_or_add_id(scope, qstr, &added);
        if (added) {
            if (strcmp(qstr_str(qstr), "AssertionError") == 0) {
                id_info->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
                // TODO how much of a hack is this?
            } else if (strcmp(qstr_str(qstr), "super") == 0 && scope->kind == SCOPE_FUNCTION) {
                // special case, super is a global, and also counts as use of __class__
                id_info->kind = ID_INFO_KIND_GLOBAL_EXPLICIT;
                id_info_t *id_info2 = scope_find_local_in_parent(scope, qstr___class__);
                if (id_info2 != NULL) {
                    id_info2 = scope_find_or_add_id(scope, qstr___class__, &added);
                    if (added) {
                        id_info2->kind = ID_INFO_KIND_FREE;
                        scope_close_over_in_parents(scope, qstr___class__);
                    }
                }
            } else {
                id_info_t *id_info2 = scope_find_local_in_parent(scope, qstr);
                if (id_info2 != NULL && (id_info2->kind == ID_INFO_KIND_LOCAL || id_info2->kind == ID_INFO_KIND_CELL || id_info2->kind == ID_INFO_KIND_FREE)) {
                    id_info->kind = ID_INFO_KIND_FREE;
                    scope_close_over_in_parents(scope, qstr);
                } else {
                    id_info->kind = ID_INFO_KIND_GLOBAL_IMPLICIT;
                }
            }
        }
    } else {
        id_info = scope_find(scope, qstr);
    }

    assert(id_info != NULL); // TODO can this ever fail?

    // call the emit backend with the correct code
    if (id_info == NULL || id_info->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
        EMIT(load_name, qstr);
    } else if (id_info->kind == ID_INFO_KIND_GLOBAL_EXPLICIT) {
        EMIT(load_global, qstr);
    } else if (id_info->kind == ID_INFO_KIND_LOCAL) {
        EMIT(load_fast, qstr, id_info->local_num);
    } else if (id_info->kind == ID_INFO_KIND_CELL || id_info->kind == ID_INFO_KIND_FREE) {
        EMIT(load_deref, qstr);
    } else {
        assert(0);
    }
}

static id_info_t *get_id_for_modification(pass_kind_t pass, scope_t *scope, qstr qstr) {
    id_info_t *id_info = NULL;
    if (pass == PASS_1) {
        // name adding/lookup
        bool added;
        id_info = scope_find_or_add_id(scope, qstr, &added);
        if (added) {
            if (scope->kind == SCOPE_MODULE || scope->kind == SCOPE_CLASS) {
                id_info->kind = ID_INFO_KIND_GLOBAL_IMPLICIT;
            } else {
                id_info->kind = ID_INFO_KIND_LOCAL;
            }
        } else if (scope->kind >= SCOPE_FUNCTION && scope->kind <= SCOPE_GEN_EXPR && id_info->kind == ID_INFO_KIND_GLOBAL_IMPLICIT) {
            // rebind as a local variable
            id_info->kind = ID_INFO_KIND_LOCAL;
        }
    } else {
        id_info = scope_find(scope, qstr);
    }

    assert(id_info != NULL); // TODO can this ever fail?

    return id_info;
}

void emit_common_store_id(pass_kind_t pass, scope_t *scope, emit_t *emit, const emit_method_table_t *emit_method_table, qstr qstr) {
    // create/get the id info
    id_info_t *id = get_id_for_modification(pass, scope, qstr);

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

void emit_common_delete_id(pass_kind_t pass, scope_t *scope, emit_t *emit, const emit_method_table_t *emit_method_table, qstr qstr) {
    // create/get the id info
    id_info_t *id = get_id_for_modification(pass, scope, qstr);

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
