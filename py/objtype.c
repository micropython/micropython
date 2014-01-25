#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime0.h"
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

// will return MP_OBJ_NULL if not found
static mp_obj_t mp_obj_class_lookup(const mp_obj_type_t *type, qstr attr) {
    for (;;) {
        if (type->locals_dict != NULL) {
            // search locals_dict (the dynamically created set of methods/attributes)

            assert(MP_OBJ_IS_TYPE(type->locals_dict, &dict_type)); // Micro Python restriction, for now
            mp_map_t *locals_map = ((void*)type->locals_dict + sizeof(mp_obj_base_t)); // XXX hack to get map object from dict object
            mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
            if (elem != NULL) {
                return elem->value;
            }
        } else if (type->methods != NULL) {
            // search methods (the const set of methods)

            for (const mp_method_t *meth = type->methods; meth->name != NULL; meth++) {
                if (strcmp(meth->name, qstr_str(attr)) == 0) {
                    return (mp_obj_t)meth->fun;
                }
            }
        }

        // attribute not found, keep searching base classes

        // for a const struct, this entry might be NULL
        if (type->bases_tuple == MP_OBJ_NULL) {
            return NULL;
        }

        uint len;
        mp_obj_t *items;
        mp_obj_tuple_get(type->bases_tuple, &len, &items);
        if (len == 0) {
            return NULL;
        }
        for (uint i = 0; i < len - 1; i++) {
            assert(MP_OBJ_IS_TYPE(items[i], &mp_const_type));
            mp_obj_t obj = mp_obj_class_lookup((mp_obj_type_t*)items[i], attr);
            if (obj != MP_OBJ_NULL) {
                return obj;
            }
        }

        // search last base (simple tail recursion elimination)
        assert(MP_OBJ_IS_TYPE(items[len - 1], &mp_const_type));
        type = (mp_obj_type_t*)items[len - 1];
    }
}

static void class_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    print(env, "<%s object at %p>", mp_obj_get_type_str(self_in), self_in);
}

static mp_obj_t class_make_new(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_const_type));
    mp_obj_type_t *self = self_in;

    mp_obj_t o = mp_obj_new_class(self_in);

    // look for __init__ function
    mp_obj_t init_fn = mp_obj_class_lookup(self, MP_QSTR___init__);

    if (init_fn != MP_OBJ_NULL) {
        // call __init__ function
        mp_obj_t init_ret;
        if (n_args == 0 && n_kw == 0) {
            init_ret = rt_call_function_n_kw(init_fn, 1, 0, (mp_obj_t*)&o);
        } else {
            mp_obj_t *args2 = m_new(mp_obj_t, 1 + n_args + 2 * n_kw);
            args2[0] = o;
            memcpy(args2 + 1, args, (n_args + 2 * n_kw) * sizeof(mp_obj_t));
            init_ret = rt_call_function_n_kw(init_fn, n_args + 1, n_kw, args2);
            m_del(mp_obj_t, args2, 1 + n_args + 2 * n_kw);
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

// TODO somehow replace const char * with a qstr
static const qstr binary_op_method_name[] = {
    [RT_BINARY_OP_SUBSCR] = MP_QSTR___getitem__,
    /*
    RT_BINARY_OP_OR,
    RT_BINARY_OP_XOR,
    RT_BINARY_OP_AND,
    RT_BINARY_OP_LSHIFT,
    RT_BINARY_OP_RSHIFT,
    */
    [RT_BINARY_OP_ADD] = MP_QSTR___add__,
    [RT_BINARY_OP_SUBTRACT] = MP_QSTR___sub__,
    /*
    RT_BINARY_OP_MULTIPLY,
    RT_BINARY_OP_FLOOR_DIVIDE,
    RT_BINARY_OP_TRUE_DIVIDE,
    RT_BINARY_OP_MODULO,
    RT_BINARY_OP_POWER,
    RT_BINARY_OP_INPLACE_OR,
    RT_BINARY_OP_INPLACE_XOR,
    RT_BINARY_OP_INPLACE_AND,
    RT_BINARY_OP_INPLACE_LSHIFT,
    RT_BINARY_OP_INPLACE_RSHIFT,
    RT_BINARY_OP_INPLACE_ADD,
    RT_BINARY_OP_INPLACE_SUBTRACT,
    RT_BINARY_OP_INPLACE_MULTIPLY,
    RT_BINARY_OP_INPLACE_FLOOR_DIVIDE,
    RT_BINARY_OP_INPLACE_TRUE_DIVIDE,
    RT_BINARY_OP_INPLACE_MODULO,
    RT_BINARY_OP_INPLACE_POWER,
    RT_COMPARE_OP_LESS,
    RT_COMPARE_OP_MORE,
    RT_COMPARE_OP_EQUAL,
    RT_COMPARE_OP_LESS_EQUAL,
    RT_COMPARE_OP_MORE_EQUAL,
    RT_COMPARE_OP_NOT_EQUAL,
    RT_COMPARE_OP_IN,
    RT_COMPARE_OP_NOT_IN,
    RT_COMPARE_OP_IS,
    RT_COMPARE_OP_IS_NOT,
    */
    [RT_COMPARE_OP_EXCEPTION_MATCH] = MP_QSTR_, // not implemented, used to make sure array has full size
};

static mp_obj_t class_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    mp_obj_class_t *lhs = lhs_in;
    qstr op_name = binary_op_method_name[op];
    if (op_name == 0) {
        return MP_OBJ_NULL;
    }
    mp_obj_t member = mp_obj_class_lookup(lhs->base.type, op_name);
    if (member != MP_OBJ_NULL) {
        return rt_call_function_2(member, lhs_in, rhs_in);
    } else {
        return MP_OBJ_NULL;
    }
}

static void class_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // logic: look in obj members then class locals (TODO check this against CPython)
    mp_obj_class_t *self = self_in;
    mp_map_elem_t *elem = mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
    if (elem != NULL) {
        // object member, always treated as a value
        dest[0] = elem->value;
        return;
    }
    mp_obj_t member = mp_obj_class_lookup(self->base.type, attr);
    if (member != MP_OBJ_NULL) {
        if (mp_obj_is_callable(member)) {
            // class member is callable so build a bound method
            // check if the methods are functions, static or class methods
            // see http://docs.python.org/3.3/howto/descriptor.html
            // TODO check that this is the correct place to have this logic
            if (MP_OBJ_IS_TYPE(member, &mp_type_staticmethod)) {
                // return just the function
                dest[0] = ((mp_obj_staticmethod_t*)member)->fun;
            } else if (MP_OBJ_IS_TYPE(member, &mp_type_classmethod)) {
                // return a bound method, with self being the type of this object
                dest[0] = ((mp_obj_classmethod_t*)member)->fun;
                dest[1] = mp_obj_get_type(self_in);
            } else {
                // return a bound method, with self being this object
                dest[0] = member;
                dest[1] = self_in;
            }
            return;
        } else {
            // class member is a value, so just return that value
            dest[0] = member;
            return;
        }
    }
}

static bool class_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    mp_obj_class_t *self = self_in;
    mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
    return true;
}

bool class_store_item(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_class_t *self = self_in;
    mp_obj_t member = mp_obj_class_lookup(self->base.type, QSTR_FROM_STR_STATIC("__setitem__"));
    if (member != MP_OBJ_NULL) {
        mp_obj_t args[3] = {self_in, index, value};
        rt_call_function_n_kw(member, 3, 0, args);
        return true;
    } else {
        return false;
    }
}

/******************************************************************************/
// type object
//  - the struct is mp_obj_type_t and is defined in obj.h so const types can be made
//  - there is a constant mp_obj_type_t (called mp_const_type) for the 'type' object
//  - creating a new class (a new type) creates a new mp_obj_type_t

static void type_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_type_t *self = self_in;
    print(env, "<class '%s'>", self->name);
}

static mp_obj_t type_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    switch (n_args) {
        case 1:
            return mp_obj_get_type(args[0]);

        case 3:
            // args[0] = name
            // args[1] = bases tuple
            // args[2] = locals dict
            return mp_obj_new_type(mp_obj_str_get_str(args[0]), args[1], args[2]);

        default:
            nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "type takes at 1 or 3 arguments"));
    }
}

static mp_obj_t type_call(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // instantiate an instance of a class

    mp_obj_type_t *self = self_in;

    if (self->make_new == NULL) {
        nlr_jump(mp_obj_new_exception_msg_1_arg(MP_QSTR_TypeError, "cannot create '%s' instances", self->name));
    }

    // make new instance
    mp_obj_t o = self->make_new(self, n_args, n_kw, args);

    // return new instance
    return o;
}

// for fail, do nothing; for attr, dest[0] = value; for method, dest[0] = method, dest[1] = self
static void type_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_const_type));
    mp_obj_type_t *self = self_in;
    mp_obj_t member = mp_obj_class_lookup(self, attr);
    if (member != MP_OBJ_NULL) {
        // check if the methods are functions, static or class methods
        // see http://docs.python.org/3.3/howto/descriptor.html
        if (MP_OBJ_IS_TYPE(member, &mp_type_staticmethod)) {
            // return just the function
            dest[0] = ((mp_obj_staticmethod_t*)member)->fun;
        } else if (MP_OBJ_IS_TYPE(member, &mp_type_classmethod)) {
            // return a bound method, with self being this class
            dest[0] = ((mp_obj_classmethod_t*)member)->fun;
            dest[1] = self_in;
        } else {
            // return just the function
            // TODO need to wrap in a type check for the first argument; eg list.append(1,1) needs to throw an exception
            dest[0] = (mp_obj_t)member;
        }
    }
}

static bool type_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_const_type));
    mp_obj_type_t *self = self_in;

    // TODO CPython allows STORE_ATTR to a class, but is this the correct implementation?

    if (self->locals_dict != NULL) {
        assert(MP_OBJ_IS_TYPE(self->locals_dict, &dict_type)); // Micro Python restriction, for now
        mp_map_t *locals_map = ((void*)self->locals_dict + sizeof(mp_obj_base_t)); // XXX hack to get map object from dict object
        mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
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
    .call = type_call,
    .load_attr = type_load_attr,
    .store_attr = type_store_attr,
};

mp_obj_t mp_obj_new_type(const char *name, mp_obj_t bases_tuple, mp_obj_t locals_dict) {
    assert(MP_OBJ_IS_TYPE(bases_tuple, &tuple_type)); // Micro Python restriction, for now
    assert(MP_OBJ_IS_TYPE(locals_dict, &dict_type)); // Micro Python restriction, for now
    mp_obj_type_t *o = m_new0(mp_obj_type_t, 1);
    o->base.type = &mp_const_type;
    o->name = name;
    o->print = class_print;
    o->make_new = class_make_new;
    o->binary_op = class_binary_op;
    o->load_attr = class_load_attr;
    o->store_attr = class_store_attr;
    o->store_item = class_store_item;
    o->bases_tuple = bases_tuple;
    o->locals_dict = locals_dict;
    return o;
}

/******************************************************************************/
// built-ins specific to types

static mp_obj_t mp_builtin_issubclass(mp_obj_t object, mp_obj_t classinfo) {
    if (!MP_OBJ_IS_TYPE(object, &mp_const_type)) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "issubclass() arg 1 must be a class"));
    }

    // TODO support a tuple of classes for second argument
    if (!MP_OBJ_IS_TYPE(classinfo, &mp_const_type)) {
        nlr_jump(mp_obj_new_exception_msg(MP_QSTR_TypeError, "issubclass() arg 2 must be a class"));
    }

    for (;;) {
        if (object == classinfo) {
            return mp_const_true;
        }

        // not equivalent classes, keep searching base classes

        assert(MP_OBJ_IS_TYPE(object, &mp_const_type));
        mp_obj_type_t *self = object;

        // for a const struct, this entry might be NULL
        if (self->bases_tuple == MP_OBJ_NULL) {
            return mp_const_false;
        }

        uint len;
        mp_obj_t *items;
        mp_obj_tuple_get(self->bases_tuple, &len, &items);
        if (len == 0) {
            return mp_const_false;
        }
        for (uint i = 0; i < len - 1; i++) {
            if (mp_builtin_issubclass(items[i], classinfo) == mp_const_true) {
                return mp_const_true;
            }
        }

        // search last base (simple tail recursion elimination)
        object = items[len - 1];
    }
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_issubclass_obj, mp_builtin_issubclass);

static mp_obj_t mp_builtin_isinstance(mp_obj_t object, mp_obj_t classinfo) {
    return mp_builtin_issubclass(mp_obj_get_type(object), classinfo);
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_isinstance_obj, mp_builtin_isinstance);

/******************************************************************************/
// staticmethod and classmethod types (probably should go in a different file)

const mp_obj_type_t mp_type_staticmethod = {
    { &mp_const_type },
    "staticmethod",
};

const mp_obj_type_t mp_type_classmethod = {
    { &mp_const_type },
    "classmethod",
};
