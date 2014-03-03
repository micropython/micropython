#include <string.h>

#include "misc.h"
#include "mpconfig.h"
#include "nlr.h"
#include "qstr.h"
#include "obj.h"
#include "objtuple.h"

typedef struct _mp_obj_namedtuple_type_t {
    mp_obj_type_t base;
    const char *fields;
} mp_obj_namedtuple_type_t;

typedef struct _mp_obj_namedtuple_t {
    mp_obj_tuple_t tuple;
} mp_obj_namedtuple_t;

static inline bool is_end_tok(char c) {
    return c == ' ' || c == ',';
}

static inline const char *skip_to_next(const char *p) {
    while (!is_end_tok(*p)) {
        if (*p == 0) {
            return NULL;
        }
        p++;
    }
    while (is_end_tok(*p)) {
        if (*p == 0) {
            return NULL;
        }
        p++;
    }
    return p;
}

STATIC uint namedtuple_count_fields(const char *namedef) {
    uint cnt = 0;
    while (*namedef != 0) {
        cnt++;
        while (!is_end_tok(*namedef) && *namedef != 0) {
            namedef++;
        }
        while (is_end_tok(*namedef) && *namedef != 0) {
            namedef++;
        }
    }
    return cnt;
}

STATIC int namedtuple_find_field(const char *name, const char *namedef) {
    int id = 0;
    int len = strlen(name);
    while (namedef) {
        if (memcmp(name, namedef, len) == 0) {
            namedef += len;
            if (*namedef == 0 || is_end_tok(*namedef)) {
                return id;
            }
        }
        namedef = skip_to_next(namedef);
        id++;
    }

    return -1;
}

STATIC void namedtuple_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_namedtuple_t *o = o_in;
    print(env, "%s(", qstr_str(o->tuple.base.type->name));
    const char *fields = ((mp_obj_namedtuple_type_t*)o->tuple.base.type)->fields;

    for (int i = 0; i < o->tuple.len; i++) {
        if (i > 0) {
                print(env, ", ");
        }
        const char *next = fields;

        while (!is_end_tok(*next) && *next != 0) {
            next++;
        }
        print(env, "%.*s=", next - fields, fields);
        mp_obj_print_helper(print, env, o->tuple.items[i], PRINT_REPR);
        while (is_end_tok(*next) && *next != 0) {
            next++;
        }
        fields = next;
    }
    print(env, ")");
}

STATIC void namedtuple_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_namedtuple_t *self = self_in;
    const char *fields = ((mp_obj_namedtuple_type_t*)self->tuple.base.type)->fields;
    int id = namedtuple_find_field(qstr_str(attr), fields);
    if (id < 0) {
        return;
    }
    dest[0] = self->tuple.items[id];
}

STATIC bool namedtuple_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    nlr_jump(mp_obj_new_exception_msg(&mp_type_AttributeError, "can't set attribute"));
}

STATIC mp_obj_t namedtuple_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_namedtuple_type_t *type = type_in;
    uint num_fields = namedtuple_count_fields(type->fields);
    if (n_args != num_fields) {
        // Counts include implicit "self"
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                                               "__new__() takes %d positional arguments but %d were given",
                                               num_fields + 1, n_args + 1));
    }
    mp_obj_tuple_t *tuple = mp_obj_new_tuple(n_args, args);
    tuple->base.type = type_in;
    return tuple;
}

STATIC const mp_obj_tuple_t namedtuple_base_tuple = {{&tuple_type}, 1, {(mp_obj_t)&tuple_type}};

mp_obj_t mp_obj_new_namedtuple_type(qstr name, const char *fields) {
    mp_obj_namedtuple_type_t *o = m_new0(mp_obj_namedtuple_type_t, 1);
    o->base.base.type = &mp_type_type;
    o->base.name = name;
    o->base.print = namedtuple_print;
    o->base.make_new = namedtuple_make_new;
    o->base.unary_op = tuple_unary_op;
    o->base.binary_op = tuple_binary_op;
    o->base.load_attr = namedtuple_load_attr;
    o->base.store_attr = namedtuple_store_attr;
    o->base.bases_tuple = (mp_obj_t)&namedtuple_base_tuple;
    o->fields = fields;
    return o;
}

STATIC mp_obj_t new_namedtuple_type(mp_obj_t name_in, mp_obj_t fields_in) {
    qstr name = MP_OBJ_QSTR_VALUE(name_in);
    const char *fields = mp_obj_str_get_str(fields_in);
    return mp_obj_new_namedtuple_type(name, fields);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_namedtuple_obj, new_namedtuple_type);
