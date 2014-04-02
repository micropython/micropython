#include <stdlib.h>
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
}

STATIC void emit_pass1_start_pass(emit_t *emit, pass_kind_t pass, scope_t *scope) {
    assert(pass == PASS_1);
    emit->scope = scope;
}

STATIC void emit_pass1_end_pass(emit_t *emit) {
}

STATIC void emit_pass1_load_id(emit_t *emit, qstr qstr) {
    // name adding/lookup
    bool added;
    id_info_t *id = scope_find_or_add_id(emit->scope, qstr, &added);
    if (added) {
#if MICROPY_EMIT_CPYTHON
        if (qstr == MP_QSTR_super && emit->scope->kind == SCOPE_FUNCTION) {
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
            id_info_t *id2 = scope_find_local_in_parent(emit->scope, qstr);
            if (id2 != NULL && (id2->kind == ID_INFO_KIND_LOCAL || id2->kind == ID_INFO_KIND_CELL || id2->kind == ID_INFO_KIND_FREE)) {
                id->kind = ID_INFO_KIND_FREE;
                scope_close_over_in_parents(emit->scope, qstr);
            } else {
                id->kind = ID_INFO_KIND_GLOBAL_IMPLICIT;
            }
        }
    }
}

STATIC id_info_t *get_id_for_modification(scope_t *scope, qstr qstr) {
    // name adding/lookup
    bool added;
    id_info_t *id = scope_find_or_add_id(scope, qstr, &added);
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

STATIC void emit_pass1_store_id(emit_t *emit, qstr qstr) {
    get_id_for_modification(emit->scope, qstr);
}

STATIC void emit_pass1_delete_id(emit_t *emit, qstr qstr) {
    get_id_for_modification(emit->scope, qstr);
}

const emit_method_table_t emit_pass1_method_table = {
    (void*)emit_pass1_dummy,
    emit_pass1_start_pass,
    emit_pass1_end_pass,
    (void*)emit_pass1_dummy,
    (void*)emit_pass1_dummy,
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
};
