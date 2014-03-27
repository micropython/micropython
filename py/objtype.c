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

STATIC mp_obj_t mp_obj_new_class(mp_obj_t class) {
    mp_obj_class_t *o = m_new_obj(mp_obj_class_t);
    o->base.type = class;
    mp_map_init(&o->members, 0);
    return o;
}

// will return MP_OBJ_NULL if not found
STATIC mp_obj_t mp_obj_class_lookup(const mp_obj_type_t *type, qstr attr) {
    for (;;) {
        if (type->locals_dict != NULL) {
            // search locals_dict (the set of methods/attributes)
            assert(MP_OBJ_IS_TYPE(type->locals_dict, &dict_type)); // Micro Python restriction, for now
            mp_map_t *locals_map = mp_obj_dict_get_map(type->locals_dict);
            mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
            if (elem != NULL) {
                return elem->value;
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
            assert(MP_OBJ_IS_TYPE(items[i], &mp_type_type));
            mp_obj_t obj = mp_obj_class_lookup((mp_obj_type_t*)items[i], attr);
            if (obj != MP_OBJ_NULL) {
                return obj;
            }
        }

        // search last base (simple tail recursion elimination)
        assert(MP_OBJ_IS_TYPE(items[len - 1], &mp_type_type));
        type = (mp_obj_type_t*)items[len - 1];
    }
}

STATIC void class_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_class_t *self = self_in;
    qstr meth = (kind == PRINT_STR) ? MP_QSTR___str__ : MP_QSTR___repr__;
    mp_obj_t member = mp_obj_class_lookup(self->base.type, meth);
    if (member == MP_OBJ_NULL && kind == PRINT_STR) {
        // If there's no __str__, fall back to __repr__
        member = mp_obj_class_lookup(self->base.type, MP_QSTR___repr__);
    }

    if (member != MP_OBJ_NULL) {
        mp_obj_t r = rt_call_function_1(member, self_in);
        mp_obj_print_helper(print, env, r, PRINT_STR);
        return;
    }

    // TODO: CPython prints fully-qualified type name
    print(env, "<%s object at %p>", mp_obj_get_type_str(self_in), self_in);
}

STATIC mp_obj_t class_make_new(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_type));
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
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "__init__() should return None, not '%s'", mp_obj_get_type_str(init_ret)));
        }

    } else {
        // TODO
        if (n_args != 0) {
            nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "function takes 0 positional arguments but %d were given", n_args));
        }
    }

    return o;
}

STATIC const qstr unary_op_method_name[] = {
    [RT_UNARY_OP_BOOL] = MP_QSTR___bool__,
    [RT_UNARY_OP_LEN] = MP_QSTR___len__,
    //[RT_UNARY_OP_POSITIVE,
    //[RT_UNARY_OP_NEGATIVE,
    //[RT_UNARY_OP_INVERT,
    [RT_UNARY_OP_NOT] = MP_QSTR_, // don't need to implement this, used to make sure array has full size
};

STATIC mp_obj_t class_unary_op(int op, mp_obj_t self_in) {
    mp_obj_class_t *self = self_in;
    qstr op_name = unary_op_method_name[op];
    if (op_name == 0) {
        return MP_OBJ_NULL;
    }
    mp_obj_t member = mp_obj_class_lookup(self->base.type, op_name);
    if (member != MP_OBJ_NULL) {
        return rt_call_function_1(member, self_in);
    } else {
        return MP_OBJ_NULL;
    }
}

STATIC const qstr binary_op_method_name[] = {
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
    RT_BINARY_OP_LESS,
    RT_BINARY_OP_MORE,
    RT_BINARY_OP_EQUAL,
    RT_BINARY_OP_LESS_EQUAL,
    RT_BINARY_OP_MORE_EQUAL,
    RT_BINARY_OP_NOT_EQUAL,
    RT_BINARY_OP_IN,
    RT_BINARY_OP_IS,
    */
    [RT_BINARY_OP_EXCEPTION_MATCH] = MP_QSTR_, // not implemented, used to make sure array has full size
};

STATIC mp_obj_t class_binary_op(int op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
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

STATIC void class_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
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
        // check if the methods are functions, static or class methods
        // see http://docs.python.org/3.3/howto/descriptor.html
        // TODO check that this is the correct place to have this logic
        if (MP_OBJ_IS_TYPE(member, &mp_type_staticmethod)) {
            // return just the function
            dest[0] = ((mp_obj_static_class_method_t*)member)->fun;
        } else if (MP_OBJ_IS_TYPE(member, &mp_type_classmethod)) {
            // return a bound method, with self being the type of this object
            dest[0] = ((mp_obj_static_class_method_t*)member)->fun;
            dest[1] = mp_obj_get_type(self_in);
        } else if (mp_obj_is_callable(member)) {
            // return a bound method, with self being this object
            dest[0] = member;
            dest[1] = self_in;
        } else {
            // class member is a value, so just return that value
            dest[0] = member;
            return;
        }
    }
}

STATIC bool class_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    mp_obj_class_t *self = self_in;
    mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
    return true;
}

bool class_store_item(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_class_t *self = self_in;
    mp_obj_t member = mp_obj_class_lookup(self->base.type, MP_QSTR___setitem__);
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
//  - there is a constant mp_obj_type_t (called mp_type_type) for the 'type' object
//  - creating a new class (a new type) creates a new mp_obj_type_t

STATIC void type_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_type_t *self = self_in;
    print(env, "<class '%s'>", qstr_str(self->name));
}

STATIC mp_obj_t type_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // TODO check n_kw == 0

    switch (n_args) {
        case 1:
            return mp_obj_get_type(args[0]);

        case 3:
            // args[0] = name
            // args[1] = bases tuple
            // args[2] = locals dict
            return mp_obj_new_type(mp_obj_str_get_qstr(args[0]), args[1], args[2]);

        default:
            nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "type takes 1 or 3 arguments"));
    }
}

STATIC mp_obj_t type_call(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // instantiate an instance of a class

    mp_obj_type_t *self = self_in;

    if (self->make_new == NULL) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "cannot create '%s' instances", qstr_str(self->name)));
    }

    // make new instance
    mp_obj_t o = self->make_new(self, n_args, n_kw, args);

    // return new instance
    return o;
}

// for fail, do nothing; for attr, dest[0] = value; for method, dest[0] = method, dest[1] = self
STATIC void type_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_type));
    mp_obj_type_t *self = self_in;
    mp_obj_t member = mp_obj_class_lookup(self, attr);
    if (member != MP_OBJ_NULL) {
        // check if the methods are functions, static or class methods
        // see http://docs.python.org/3.3/howto/descriptor.html
        if (MP_OBJ_IS_TYPE(member, &mp_type_staticmethod)) {
            // return just the function
            dest[0] = ((mp_obj_static_class_method_t*)member)->fun;
        } else if (MP_OBJ_IS_TYPE(member, &mp_type_classmethod)) {
            // return a bound method, with self being this class
            dest[0] = ((mp_obj_static_class_method_t*)member)->fun;
            dest[1] = self_in;
        } else {
            // return just the function
            // TODO need to wrap in a type check for the first argument; eg list.append(1,1) needs to throw an exception
            dest[0] = (mp_obj_t)member;
        }
    }
}

STATIC bool type_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_type));
    mp_obj_type_t *self = self_in;

    // TODO CPython allows STORE_ATTR to a class, but is this the correct implementation?

    if (self->locals_dict != NULL) {
        assert(MP_OBJ_IS_TYPE(self->locals_dict, &dict_type)); // Micro Python restriction, for now
        mp_map_t *locals_map = mp_obj_dict_get_map(self->locals_dict);
        mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
        // note that locals_map may be in ROM, so add will fail in that case
        if (elem != NULL) {
            elem->value = value;
            return true;
        }
    }

    return false;
}

const mp_obj_type_t mp_type_type = {
    { &mp_type_type },
    .name = MP_QSTR_type,
    .print = type_print,
    .make_new = type_make_new,
    .call = type_call,
    .load_attr = type_load_attr,
    .store_attr = type_store_attr,
};

mp_obj_t mp_obj_new_type(qstr name, mp_obj_t bases_tuple, mp_obj_t locals_dict) {
    assert(MP_OBJ_IS_TYPE(bases_tuple, &tuple_type)); // Micro Python restriction, for now
    assert(MP_OBJ_IS_TYPE(locals_dict, &dict_type)); // Micro Python restriction, for now
    mp_obj_type_t *o = m_new0(mp_obj_type_t, 1);
    o->base.type = &mp_type_type;
    o->name = name;
    o->print = class_print;
    o->make_new = class_make_new;
    o->unary_op = class_unary_op;
    o->binary_op = class_binary_op;
    o->load_attr = class_load_attr;
    o->store_attr = class_store_attr;
    o->store_item = class_store_item;
    o->bases_tuple = bases_tuple;
    o->locals_dict = locals_dict;
    return o;
}

/******************************************************************************/
// super object

typedef struct _mp_obj_super_t {
    mp_obj_base_t base;
    mp_obj_t type;
    mp_obj_t obj;
} mp_obj_super_t;

STATIC void super_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_super_t *self = self_in;
    print(env, "<super: ");
    mp_obj_print_helper(print, env, self->type, PRINT_STR);
    print(env, ", ");
    mp_obj_print_helper(print, env, self->obj, PRINT_STR);
    print(env, ">");
}

STATIC mp_obj_t super_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    if (n_args != 2 || n_kw != 0) {
        // 0 arguments are turned into 2 in the compiler
        // 1 argument is not yet implemented
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "super() requires 2 arguments"));
    }
    return mp_obj_new_super(args[0], args[1]);
}

// for fail, do nothing; for attr, dest[0] = value; for method, dest[0] = method, dest[1] = self
STATIC void super_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    assert(MP_OBJ_IS_TYPE(self_in, &super_type));
    mp_obj_super_t *self = self_in;

    assert(MP_OBJ_IS_TYPE(self->type, &mp_type_type));

    mp_obj_type_t *type = self->type;

    // for a const struct, this entry might be NULL
    if (type->bases_tuple == MP_OBJ_NULL) {
        return;
    }

    uint len;
    mp_obj_t *items;
    mp_obj_tuple_get(type->bases_tuple, &len, &items);
    for (uint i = 0; i < len; i++) {
        assert(MP_OBJ_IS_TYPE(items[i], &mp_type_type));
        mp_obj_t member = mp_obj_class_lookup((mp_obj_type_t*)items[i], attr);
        if (member != MP_OBJ_NULL) {
            // XXX this and the code in class_load_attr need to be factored out
            // check if the methods are functions, static or class methods
            // see http://docs.python.org/3.3/howto/descriptor.html
            // TODO check that this is the correct place to have this logic
            if (MP_OBJ_IS_TYPE(member, &mp_type_staticmethod)) {
                // return just the function
                dest[0] = ((mp_obj_static_class_method_t*)member)->fun;
            } else if (MP_OBJ_IS_TYPE(member, &mp_type_classmethod)) {
                // return a bound method, with self being the type of this object
                dest[0] = ((mp_obj_static_class_method_t*)member)->fun;
                dest[1] = mp_obj_get_type(self->obj);
            } if (mp_obj_is_callable(member)) {
                // return a bound method, with self being this object
                dest[0] = member;
                dest[1] = self->obj;
            } else {
                // class member is a value, so just return that value
                dest[0] = member;
            }
            return;
        }
    }
}

const mp_obj_type_t super_type = {
    { &mp_type_type },
    .name = MP_QSTR_super,
    .print = super_print,
    .make_new = super_make_new,
    .load_attr = super_load_attr,
};

mp_obj_t mp_obj_new_super(mp_obj_t type, mp_obj_t obj) {
    mp_obj_super_t *o = m_new_obj(mp_obj_super_t);
    *o = (mp_obj_super_t){{&super_type}, type, obj};
    return o;
}

/******************************************************************************/
// subclassing and built-ins specific to types

// object and classinfo should be type objects
// (but the function will fail gracefully if they are not)
bool mp_obj_is_subclass_fast(mp_const_obj_t object, mp_const_obj_t classinfo) {
    for (;;) {
        if (object == classinfo) {
            return true;
        }

        // not equivalent classes, keep searching base classes

        // object should always be a type object, but just return false if it's not
        if (!MP_OBJ_IS_TYPE(object, &mp_type_type)) {
            return false;
        }

        const mp_obj_type_t *self = object;

        // for a const struct, this entry might be NULL
        if (self->bases_tuple == MP_OBJ_NULL) {
            return false;
        }

        // get the base objects (they should be type objects)
        uint len;
        mp_obj_t *items;
        mp_obj_tuple_get(self->bases_tuple, &len, &items);
        if (len == 0) {
            return false;
        }

        // iterate through the base objects
        for (uint i = 0; i < len - 1; i++) {
            if (mp_obj_is_subclass_fast(items[i], classinfo)) {
                return true;
            }
        }

        // search last base (simple tail recursion elimination)
        object = items[len - 1];
    }
}

STATIC mp_obj_t mp_obj_is_subclass(mp_obj_t object, mp_obj_t classinfo) {
    uint len;
    mp_obj_t *items;
    if (MP_OBJ_IS_TYPE(classinfo, &mp_type_type)) {
        len = 1;
        items = &classinfo;
    } else if (MP_OBJ_IS_TYPE(classinfo, &tuple_type)) {
        mp_obj_tuple_get(classinfo, &len, &items);
    } else {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "issubclass() arg 2 must be a class or a tuple of classes"));
    }

    for (uint i = 0; i < len; i++) {
        if (mp_obj_is_subclass_fast(object, items[i])) {
            return mp_const_true;
        }
    }
    return mp_const_false;
}

STATIC mp_obj_t mp_builtin_issubclass(mp_obj_t object, mp_obj_t classinfo) {
    if (!MP_OBJ_IS_TYPE(object, &mp_type_type)) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_TypeError, "issubclass() arg 1 must be a class"));
    }
    return mp_obj_is_subclass(object, classinfo);
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_issubclass_obj, mp_builtin_issubclass);

STATIC mp_obj_t mp_builtin_isinstance(mp_obj_t object, mp_obj_t classinfo) {
    return mp_obj_is_subclass(mp_obj_get_type(object), classinfo);
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_isinstance_obj, mp_builtin_isinstance);

/******************************************************************************/
// staticmethod and classmethod types (probably should go in a different file)

STATIC mp_obj_t static_class_method_make_new(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    assert(self_in == &mp_type_staticmethod || self_in == &mp_type_classmethod);

    if (n_args != 1 || n_kw != 0) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "function takes 1 positional argument but %d were given", n_args));
    }

    mp_obj_static_class_method_t *o = m_new_obj(mp_obj_static_class_method_t);
    *o = (mp_obj_static_class_method_t){{(mp_obj_type_t*)self_in}, args[0]};
    return o;
}

const mp_obj_type_t mp_type_staticmethod = {
    { &mp_type_type },
    .name = MP_QSTR_staticmethod,
    .make_new = static_class_method_make_new
};

const mp_obj_type_t mp_type_classmethod = {
    { &mp_type_type },
    .name = MP_QSTR_classmethod,
    .make_new = static_class_method_make_new
};
