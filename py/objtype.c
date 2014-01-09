#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "map.h"
#include "runtime.h"

/******************************************************************************/
// class object
// creating an instance of a class makes one of these objects

typedef struct _mp_obj_class_t {
    mp_obj_base_t base;
    mp_map_t members;
} mp_obj_class_t;

static mp_obj_t mp_obj_new_class(mp_obj_t class) {
    mp_obj_class_t *o = m_new_obj(mp_obj_class_t);
    o->base.type = class;
    mp_map_init(&o->members, 0);
    return o;
}

static void class_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    print(env, "<%s object at %p>", mp_obj_get_type_str(self_in), self_in);
}

// args are reverse in the array
static mp_obj_t class_make_new(mp_obj_t self_in, int n_args, const mp_obj_t *args) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_const_type));

    mp_obj_t o = mp_obj_new_class(self_in);

    // look for __init__ function
    mp_map_elem_t *init_fn = mp_obj_class_lookup(self_in, MP_QSTR___init__, MP_MAP_LOOKUP);

    if (init_fn != NULL) {
        // call __init__ function
        mp_obj_t init_ret;
        if (n_args == 0) {
            init_ret = rt_call_function_n(init_fn->value, 1, (mp_obj_t*)&o);
        } else {
            mp_obj_t *args2 = m_new(mp_obj_t, n_args + 1);
            memcpy(args2, args, n_args * sizeof(mp_obj_t));
            args2[n_args] = o;
            init_ret = rt_call_function_n(init_fn->value, n_args + 1, args2);
            m_del(mp_obj_t, args2, n_args + 1);
        }
        if (init_ret != mp_const_none) {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "__init__() should return None, not '%s'", mp_obj_get_type_str(init_ret)));
        }

    } else {
        // TODO
        if (n_args != 0) {
            nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "function takes 0 positional arguments but %d were given", (void*)(machine_int_t)n_args));
        }
    }

    return o;
}

static void class_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // logic: look in obj members then class locals (TODO check this against CPython)
    mp_obj_class_t *self = self_in;
    mp_map_elem_t *elem = mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
    if (elem != NULL) {
        // object member, always treated as a value
        dest[1] = elem->value;
        return;
    }
    elem = mp_obj_class_lookup((mp_obj_t)self->base.type, attr, MP_MAP_LOOKUP);
    if (elem != NULL) {
        if (mp_obj_is_callable(elem->value)) {
            // class member is callable so build a bound method
            dest[1] = elem->value;
            dest[0] = self_in;
            return;
        } else {
            // class member is a value, so just return that value
            dest[1] = elem->value;
            return;
        }
    }
}

static bool class_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    // logic: look in class locals (no add) then obj members (add) (TODO check this against CPython)
    mp_obj_class_t *self = self_in;
    mp_map_elem_t *elem = mp_obj_class_lookup((mp_obj_t)self->base.type, attr, MP_MAP_LOOKUP);
    if (elem != NULL) {
        elem->value = value;
    } else {
        mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
    }
    return true;
}

mp_map_elem_t *mp_obj_class_lookup(mp_obj_t self_in, qstr attr, mp_map_lookup_kind_t lookup_kind) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_const_type));
    mp_obj_type_t *self = self_in;
    if (self->locals == NULL) {
        return NULL;
    }
    assert(MP_OBJ_IS_TYPE(self->locals, &dict_type)); // Micro Python restriction, for now
    mp_map_t *locals_map = ((void*)self->locals + sizeof(mp_obj_base_t)); // XXX hack to get map object from dict object
    return mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), lookup_kind);
}

/******************************************************************************/
// type object
//  - the struct is mp_obj_type_t and is defined in obj.h so const types can be made
//  - there is a constant mp_obj_type_t (called mp_const_type) for the 'type' object
//  - creating a new class (a new type) creates a new mp_obj_type_t

static void type_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in) {
    mp_obj_type_t *self = self_in;
    print(env, "<class '%s'>", self->name);
}

// args are reverse in the array
static mp_obj_t type_make_new(mp_obj_t type_in, int n_args, const mp_obj_t *args) {
    switch (n_args) {
        case 1:
            return mp_obj_get_type(args[0]);

        case 3:
        {
            // args[2] = name
            // args[1] = bases tuple
            // args[0] = locals dict

            return mp_obj_new_type(mp_obj_get_qstr(args[2]), args[0]);
        }

        default:
            nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "type takes at 1 or 3 arguments"));
    }
}

// args are in reverse order in the array
static mp_obj_t type_call_n(mp_obj_t self_in, int n_args, const mp_obj_t *args) {
    // instantiate an instance of a class

    mp_obj_type_t *self = self_in;

    if (self->make_new == NULL) {
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "cannot create '%s' instances", self->name));
    }

    // make new instance
    mp_obj_t o = self->make_new(self, n_args, args);

    // return new instance
    return o;
}

// for fail, do nothing; for attr, dest[1] = value; for method, dest[0] = self, dest[1] = method
static void type_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_map_elem_t *elem = mp_obj_class_lookup(self_in, attr, MP_MAP_LOOKUP);
    if (elem != NULL) {
        dest[1] = elem->value;
        return;
    }
}

static bool type_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    // TODO CPython allows STORE_ATTR to a class, but is this the correct implementation?

    mp_map_elem_t *elem = mp_obj_class_lookup(self_in, attr, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    if (elem != NULL) {
        elem->value = value;
        return true;
    } else {
        return false;
    }
}

const mp_obj_type_t mp_const_type = {
    { &mp_const_type },
    "type",
    .print = type_print,
    .make_new = type_make_new,
    .call_n = type_call_n,
    .load_attr = type_load_attr,
    .store_attr = type_store_attr,
};

mp_obj_t mp_obj_new_type(qstr name, mp_obj_t local_dict) {
    mp_obj_type_t *o = m_new0(mp_obj_type_t, 1);
    o->base.type = &mp_const_type;
    o->name = qstr_str(name);
    o->print = class_print;
    o->make_new = class_make_new;
    o->load_attr = class_load_attr;
    o->store_attr = class_store_attr;
    o->locals = local_dict;
    assert(MP_OBJ_IS_TYPE(o->locals, &dict_type)); // Micro Python restriction, for now
    return o;
}
