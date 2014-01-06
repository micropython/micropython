#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "mpqstr.h"
#include "obj.h"
#include "runtime.h"
#include "map.h"

typedef struct _mp_obj_instance_t {
    mp_obj_base_t base;
    mp_obj_base_t *class; // points to a "class" object
    mp_map_t *members;
} mp_obj_instance_t;

/*
type needs to be specified dynamically
            case O_OBJ:
            {
                py_map_elem_t *qn = py_qstr_map_lookup(o->u_obj.class->u_class.locals, qstr_from_str_static("__qualname__"), false); assert(qn != NULL);
                assert(IS_O(qn->value, O_STR));
                return qstr_str(((py_obj_base_t*)qn->value)->u_str);
            }
            */

mp_obj_t mp_obj_instance_load_attr(mp_obj_t self_in, qstr attr) {
    // logic: look in obj members then class locals (TODO check this against CPython)
    mp_obj_instance_t *self = self_in;
    mp_map_elem_t *elem = mp_qstr_map_lookup(self->members, attr, false);
    if (elem != NULL) {
        // object member, always treated as a value
        return elem->value;
    }
    elem = mp_qstr_map_lookup(mp_obj_class_get_locals(self->class), attr, false);
    if (elem != NULL) {
        if (mp_obj_is_callable(elem->value)) {
            // class member is callable so build a bound method
            return mp_obj_new_bound_meth(self_in, elem->value);
        } else {
            // class member is a value, so just return that value
            return elem->value;
        }
    }
    nlr_jump(mp_obj_new_exception_msg_2_args(MP_QSTR_AttributeError, "'%s' object has no attribute '%s'", mp_obj_get_type_str(self_in), qstr_str(attr)));
}

void mp_obj_instance_load_method(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // logic: look in obj members then class locals (TODO check this against CPython)
    mp_obj_instance_t *self = self_in;
    mp_map_elem_t *elem = mp_qstr_map_lookup(self->members, attr, false);
    if (elem != NULL) {
        // object member, always treated as a value
        dest[1] = elem->value;
        dest[0] = NULL;
        return;
    }
    elem = mp_qstr_map_lookup(mp_obj_class_get_locals(self->class), attr, false);
    if (elem != NULL) {
        if (mp_obj_is_callable(elem->value)) {
            // class member is callable so build a bound method
            dest[1] = elem->value;
            dest[0] = self_in;
            return;
        } else {
            // class member is a value, so just return that value
            dest[1] = elem->value;
            dest[0] = NULL;
            return;
        }
    }

    // no such method, so fall back to load attr
    dest[1] = rt_load_attr(self_in, attr);
    dest[0] = NULL;
}

void mp_obj_instance_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    // logic: look in class locals (no add) then obj members (add) (TODO check this against CPython)
    mp_obj_instance_t *self = self_in;
    mp_map_elem_t *elem = mp_qstr_map_lookup(mp_obj_class_get_locals(self->class), attr, false);
    if (elem != NULL) {
        elem->value = value;
    } else {
        mp_qstr_map_lookup(self->members, attr, true)->value = value;
    }
}

const mp_obj_type_t instance_type = {
    { &mp_const_type },
    "instance",
    NULL, // print
    NULL, // make_new
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    NULL, // iternext
    .methods = {{NULL, NULL},},
};

mp_obj_t mp_obj_new_instance(mp_obj_t class) {
    mp_obj_instance_t *o = m_new_obj(mp_obj_instance_t);
    o->base.type = &instance_type;
    o->class = class;
    o->members = mp_map_new(MP_MAP_QSTR, 0);
    return o;
}
