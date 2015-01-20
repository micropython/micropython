/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "py/nlr.h"
#include "py/objtype.h"
#include "py/runtime0.h"
#include "py/runtime.h"

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

STATIC mp_obj_t static_class_method_make_new(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args);

/******************************************************************************/
// instance object

#define is_instance_type(type) ((type)->make_new == instance_make_new)
#define is_native_type(type) ((type)->make_new != instance_make_new)
mp_obj_t instance_make_new(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args);
STATIC void instance_convert_return_attr(mp_obj_t self, const mp_obj_type_t *type, mp_obj_t member, mp_obj_t *dest);

STATIC mp_obj_t mp_obj_new_instance(mp_obj_t class, uint subobjs) {
    mp_obj_instance_t *o = m_new_obj_var(mp_obj_instance_t, mp_obj_t, subobjs);
    o->base.type = class;
    mp_map_init(&o->members, 0);
    mp_seq_clear(o->subobj, 0, subobjs, sizeof(*o->subobj));
    return o;
}

STATIC int instance_count_native_bases(const mp_obj_type_t *type, const mp_obj_type_t **last_native_base) {
    mp_uint_t len;
    mp_obj_t *items;
    mp_obj_tuple_get(type->bases_tuple, &len, &items);

    int count = 0;
    for (uint i = 0; i < len; i++) {
        assert(MP_OBJ_IS_TYPE(items[i], &mp_type_type));
        const mp_obj_type_t *bt = (const mp_obj_type_t *)items[i];
        if (bt == &mp_type_object) {
            // Not a "real" type
            continue;
        }
        if (is_native_type(bt)) {
            *last_native_base = items[i];
            count++;
        } else {
            count += instance_count_native_bases(items[i], last_native_base);
        }
    }

    return count;
}

// TODO
// This implements depth-first left-to-right MRO, which is not compliant with Python3 MRO
// http://python-history.blogspot.com/2010/06/method-resolution-order.html
// https://www.python.org/download/releases/2.3/mro/
//
// will return MP_OBJ_NULL if not found
// will return MP_OBJ_SENTINEL if special method was found in a native type base
// via slot id (meth_offset). As there can be only one native base, it's known that it
// applies to instance->subobj[0]. In most cases, we also don't need to know which type
// it was - because instance->subobj[0] is of that type. The only exception is when
// object is not yet constructed, then we need to know base native type to construct
// instance->subobj[0]. This case is handled via instance_count_native_bases() though.
struct class_lookup_data {
    mp_obj_instance_t *obj;
    qstr attr;
    mp_uint_t meth_offset;
    mp_obj_t *dest;
    bool is_type;
};

STATIC void mp_obj_class_lookup(struct class_lookup_data  *lookup, const mp_obj_type_t *type) {
    assert(lookup->dest[0] == NULL);
    assert(lookup->dest[1] == NULL);
    for (;;) {
        // Optimize special method lookup for native types
        // This avoids extra method_name => slot lookup. On the other hand,
        // this should not be applied to class types, as will result in extra
        // lookup either.
        if (lookup->meth_offset != 0 && is_native_type(type)) {
            if (*(void**)((char*)type + lookup->meth_offset) != NULL) {
                DEBUG_printf("mp_obj_class_lookup: matched special meth slot for %s\n", qstr_str(lookup->attr));
                lookup->dest[0] = MP_OBJ_SENTINEL;
                return;
            }
        }

        if (type->locals_dict != NULL) {
            // search locals_dict (the set of methods/attributes)
            assert(MP_OBJ_IS_TYPE(type->locals_dict, &mp_type_dict)); // Micro Python restriction, for now
            mp_map_t *locals_map = mp_obj_dict_get_map(type->locals_dict);
            mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(lookup->attr), MP_MAP_LOOKUP);
            if (elem != NULL) {
                lookup->dest[0] = elem->value;
                if (lookup->is_type) {
                    // If we look up class method, we need to pass original type there,
                    // not type where we found a class method.
                    const mp_obj_type_t *org_type = (const mp_obj_type_t*)lookup->obj;
                    instance_convert_return_attr(NULL, org_type, elem->value, lookup->dest);
                } else if (lookup->obj != MP_OBJ_NULL && !lookup->is_type && is_native_type(type)) {
                    instance_convert_return_attr(lookup->obj->subobj[0], type, elem->value, lookup->dest);
                } else {
                    instance_convert_return_attr(lookup->obj, type, elem->value, lookup->dest);
                }
#if DEBUG_PRINT
                printf("mp_obj_class_lookup: Returning: ");
                mp_obj_print(lookup->dest[0], PRINT_REPR); printf(" ");
                mp_obj_print(lookup->dest[1], PRINT_REPR); printf("\n");
#endif
                return;
            }
        }

        // Try this for completeness, but all native methods should be statically defined
        // in locals_dict, and would be handled by above.
        if (lookup->obj != MP_OBJ_NULL && !lookup->is_type && is_native_type(type)) {
            mp_load_method_maybe(lookup->obj->subobj[0], lookup->attr, lookup->dest);
            if (lookup->dest[0] != MP_OBJ_NULL) {
                return;
            }
        }

        // attribute not found, keep searching base classes

        // for a const struct, this entry might be NULL
        if (type->bases_tuple == MP_OBJ_NULL) {
            return;
        }

        mp_uint_t len;
        mp_obj_t *items;
        mp_obj_tuple_get(type->bases_tuple, &len, &items);
        if (len == 0) {
            return;
        }
        for (uint i = 0; i < len - 1; i++) {
            assert(MP_OBJ_IS_TYPE(items[i], &mp_type_type));
            mp_obj_type_t *bt = (mp_obj_type_t*)items[i];
            if (bt == &mp_type_object) {
                // Not a "real" type
                continue;
            }
            mp_obj_class_lookup(lookup, bt);
            if (lookup->dest[0] != MP_OBJ_NULL) {
                return;
            }
        }

        // search last base (simple tail recursion elimination)
        assert(MP_OBJ_IS_TYPE(items[len - 1], &mp_type_type));
        type = (mp_obj_type_t*)items[len - 1];
        if (type == &mp_type_object) {
            // Not a "real" type
            return;
        }
    }
}

STATIC void instance_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_instance_t *self = self_in;
    qstr meth = (kind == PRINT_STR) ? MP_QSTR___str__ : MP_QSTR___repr__;
    mp_obj_t member[2] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = self,
        .attr = meth,
        .meth_offset = offsetof(mp_obj_type_t, print),
        .dest = member,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);
    if (member[0] == MP_OBJ_NULL && kind == PRINT_STR) {
        // If there's no __str__, fall back to __repr__
        lookup.attr = MP_QSTR___repr__;
        lookup.meth_offset = 0;
        mp_obj_class_lookup(&lookup, self->base.type);
    }

    if (member[0] == MP_OBJ_SENTINEL) {
        // Handle Exception subclasses specially
        if (mp_obj_is_native_exception_instance(self->subobj[0])) {
            if (kind != PRINT_STR) {
                print(env, "%s", qstr_str(self->base.type->name));
            }
            mp_obj_print_helper(print, env, self->subobj[0], kind | PRINT_EXC_SUBCLASS);
        } else {
            mp_obj_print_helper(print, env, self->subobj[0], kind);
        }
        return;
    }

    if (member[0] != MP_OBJ_NULL) {
        mp_obj_t r = mp_call_function_1(member[0], self_in);
        mp_obj_print_helper(print, env, r, PRINT_STR);
        return;
    }

    // TODO: CPython prints fully-qualified type name
    print(env, "<%s object at %p>", mp_obj_get_type_str(self_in), self_in);
}

mp_obj_t instance_make_new(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_type));
    mp_obj_type_t *self = self_in;

    const mp_obj_type_t *native_base;
    uint num_native_bases = instance_count_native_bases(self, &native_base);
    assert(num_native_bases < 2);

    mp_obj_instance_t *o = mp_obj_new_instance(self_in, num_native_bases);

    // This executes only "__new__" part of obejection creation.
    // TODO: This won't work will for classes with native bases.
    // TODO: This is hack, should be resolved along the lines of
    // https://github.com/micropython/micropython/issues/606#issuecomment-43685883
    if (n_args == 1 && *args == MP_OBJ_SENTINEL) {
        return o;
    }

    // look for __new__ function
    mp_obj_t init_fn[2] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = NULL,
        .attr = MP_QSTR___new__,
        .meth_offset = offsetof(mp_obj_type_t, make_new),
        .dest = init_fn,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self);

    mp_obj_t new_ret = o;
    if (init_fn[0] == MP_OBJ_SENTINEL) {
        // Native type's constructor is what wins - it gets all our arguments,
        // and none Python classes are initialized at all.
        o->subobj[0] = native_base->make_new((mp_obj_type_t*)native_base, n_args, n_kw, args);
    } else if (init_fn[0] != MP_OBJ_NULL) {
        // now call Python class __new__ function with all args
        if (n_args == 0 && n_kw == 0) {
            new_ret = mp_call_function_n_kw(init_fn[0], 1, 0, (mp_obj_t*)(void*)&self_in);
        } else {
            mp_obj_t *args2 = m_new(mp_obj_t, 1 + n_args + 2 * n_kw);
            args2[0] = self_in;
            memcpy(args2 + 1, args, (n_args + 2 * n_kw) * sizeof(mp_obj_t));
            new_ret = mp_call_function_n_kw(init_fn[0], n_args + 1, n_kw, args2);
            m_del(mp_obj_t, args2, 1 + n_args + 2 * n_kw);
        }

    }

    // https://docs.python.org/3.4/reference/datamodel.html#object.__new__
    // "If __new__() does not return an instance of cls, then the new instance’s __init__() method will not be invoked."
    if (mp_obj_get_type(new_ret) != self_in) {
        return new_ret;
    }

    o = new_ret;

    // now call Python class __init__ function with all args
    init_fn[0] = init_fn[1] = MP_OBJ_NULL;
    lookup.obj = o;
    lookup.attr = MP_QSTR___init__;
    lookup.meth_offset = 0;
    mp_obj_class_lookup(&lookup, self);
    if (init_fn[0] != MP_OBJ_NULL) {
        mp_obj_t init_ret;
        if (n_args == 0 && n_kw == 0) {
            init_ret = mp_call_method_n_kw(0, 0, init_fn);
        } else {
            mp_obj_t *args2 = m_new(mp_obj_t, 2 + n_args + 2 * n_kw);
            args2[0] = init_fn[0];
            args2[1] = init_fn[1];
            memcpy(args2 + 2, args, (n_args + 2 * n_kw) * sizeof(mp_obj_t));
            init_ret = mp_call_method_n_kw(n_args, n_kw, args2);
            m_del(mp_obj_t, args2, 2 + n_args + 2 * n_kw);
        }
        if (init_ret != mp_const_none) {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError,
                    "__init__() should return None"));
            } else {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                    "__init__() should return None, not '%s'", mp_obj_get_type_str(init_ret)));
            }
        }

    }

    return o;
}

STATIC const qstr unary_op_method_name[] = {
    [MP_UNARY_OP_BOOL] = MP_QSTR___bool__,
    [MP_UNARY_OP_LEN] = MP_QSTR___len__,
    //[MP_UNARY_OP_POSITIVE,
    //[MP_UNARY_OP_NEGATIVE,
    //[MP_UNARY_OP_INVERT,
    [MP_UNARY_OP_NOT] = MP_QSTR_, // don't need to implement this, used to make sure array has full size
};

STATIC mp_obj_t instance_unary_op(mp_uint_t op, mp_obj_t self_in) {
    mp_obj_instance_t *self = self_in;
    qstr op_name = unary_op_method_name[op];
    /* Still try to lookup native slot
    if (op_name == 0) {
        return MP_OBJ_NULL;
    }
    */
    mp_obj_t member[2] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = self,
        .attr = op_name,
        .meth_offset = offsetof(mp_obj_type_t, unary_op),
        .dest = member,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);
    if (member[0] == MP_OBJ_SENTINEL) {
        return mp_unary_op(op, self->subobj[0]);
    } else if (member[0] != MP_OBJ_NULL) {
        return mp_call_function_1(member[0], self_in);
    } else {
        return MP_OBJ_NULL; // op not supported
    }
}

const qstr mp_binary_op_method_name[] = {
    /*
    MP_BINARY_OP_OR,
    MP_BINARY_OP_XOR,
    MP_BINARY_OP_AND,
    MP_BINARY_OP_LSHIFT,
    MP_BINARY_OP_RSHIFT,
    */
    [MP_BINARY_OP_ADD] = MP_QSTR___add__,
    [MP_BINARY_OP_SUBTRACT] = MP_QSTR___sub__,
    /*
    MP_BINARY_OP_MULTIPLY,
    MP_BINARY_OP_FLOOR_DIVIDE,
    MP_BINARY_OP_TRUE_DIVIDE,
    MP_BINARY_OP_MODULO,
    MP_BINARY_OP_POWER,
    MP_BINARY_OP_INPLACE_OR,
    MP_BINARY_OP_INPLACE_XOR,
    MP_BINARY_OP_INPLACE_AND,
    MP_BINARY_OP_INPLACE_LSHIFT,
    MP_BINARY_OP_INPLACE_RSHIFT,
    MP_BINARY_OP_INPLACE_ADD,
    MP_BINARY_OP_INPLACE_SUBTRACT,
    MP_BINARY_OP_INPLACE_MULTIPLY,
    MP_BINARY_OP_INPLACE_FLOOR_DIVIDE,
    MP_BINARY_OP_INPLACE_TRUE_DIVIDE,
    MP_BINARY_OP_INPLACE_MODULO,
    MP_BINARY_OP_INPLACE_POWER,*/
    [MP_BINARY_OP_LESS] = MP_QSTR___lt__,
    [MP_BINARY_OP_MORE] = MP_QSTR___gt__,
    [MP_BINARY_OP_EQUAL] = MP_QSTR___eq__,
    [MP_BINARY_OP_LESS_EQUAL] = MP_QSTR___le__,
    [MP_BINARY_OP_MORE_EQUAL] = MP_QSTR___ge__,
    /*
    MP_BINARY_OP_NOT_EQUAL, // a != b calls a == b and inverts result
    */
    [MP_BINARY_OP_IN] = MP_QSTR___contains__,
    /*
    MP_BINARY_OP_IS,
    */
    [MP_BINARY_OP_EXCEPTION_MATCH] = MP_QSTR_, // not implemented, used to make sure array has full size
};

// Given a member that was extracted from an instance, convert it correctly
// and put the result in the dest[] array for a possible method call.
// Conversion means dealing with static/class methods, callables, and values.
// see http://docs.python.org/3/howto/descriptor.html
STATIC void instance_convert_return_attr(mp_obj_t self, const mp_obj_type_t *type, mp_obj_t member, mp_obj_t *dest) {
    assert(dest[1] == NULL);
    if (MP_OBJ_IS_TYPE(member, &mp_type_staticmethod)) {
        // return just the function
        dest[0] = ((mp_obj_static_class_method_t*)member)->fun;
    } else if (MP_OBJ_IS_TYPE(member, &mp_type_classmethod)) {
        // return a bound method, with self being the type of this object
        dest[0] = ((mp_obj_static_class_method_t*)member)->fun;
        dest[1] = (mp_obj_t)type;
    } else if (MP_OBJ_IS_TYPE(member, &mp_type_type)) {
        // Don't try to bind types
        dest[0] = member;
    } else if (mp_obj_is_callable(member)) {
        // return a bound method, with self being this object
        dest[0] = member;
        dest[1] = self;
    } else {
        // class member is a value, so just return that value
        dest[0] = member;
    }
}

STATIC mp_obj_t instance_binary_op(mp_uint_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    // Note: For ducktyping, CPython does not look in the instance members or use
    // __getattr__ or __getattribute__.  It only looks in the class dictionary.
    mp_obj_instance_t *lhs = lhs_in;
    qstr op_name = mp_binary_op_method_name[op];
    /* Still try to lookup native slot
    if (op_name == 0) {
        return MP_OBJ_NULL;
    }
    */
    mp_obj_t dest[3] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = lhs,
        .attr = op_name,
        .meth_offset = offsetof(mp_obj_type_t, binary_op),
        .dest = dest,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, lhs->base.type);
    if (dest[0] == MP_OBJ_SENTINEL) {
        return mp_binary_op(op, lhs->subobj[0], rhs_in);
    } else if (dest[0] != MP_OBJ_NULL) {
        dest[2] = rhs_in;
        return mp_call_method_n_kw(1, 0, dest);
    } else {
        return MP_OBJ_NULL; // op not supported
    }
}

void mp_obj_instance_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // logic: look in instance members then class locals
    assert(is_instance_type(mp_obj_get_type(self_in)));
    mp_obj_instance_t *self = self_in;

    mp_map_elem_t *elem = mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
    if (elem != NULL) {
        // object member, always treated as a value
        // TODO should we check for properties?
        dest[0] = elem->value;
        return;
    }

    struct class_lookup_data lookup = {
        .obj = self,
        .attr = attr,
        .meth_offset = 0,
        .dest = dest,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);
    mp_obj_t member = dest[0];
    if (member != MP_OBJ_NULL) {
#if MICROPY_PY_BUILTINS_PROPERTY
        if (MP_OBJ_IS_TYPE(member, &mp_type_property)) {
            // object member is a property
            // delegate the store to the property
            // TODO should this be part of instance_convert_return_attr?
            const mp_obj_t *proxy = mp_obj_property_get(member);
            if (proxy[0] == mp_const_none) {
                // TODO
            } else {
                dest[0] = mp_call_function_n_kw(proxy[0], 1, 0, &self_in);
                // TODO should we convert the returned value using instance_convert_return_attr?
            }
        }
#endif
        return;
    }

    // try __getattr__
    if (attr != MP_QSTR___getattr__) {
        mp_obj_t dest2[3];
        mp_load_method_maybe(self_in, MP_QSTR___getattr__, dest2);
        if (dest2[0] != MP_OBJ_NULL) {
            // __getattr__ exists, call it and return its result
            // XXX if this fails to load the requested attr, should we catch the attribute error and return silently?
            dest2[2] = MP_OBJ_NEW_QSTR(attr);
            dest[0] = mp_call_method_n_kw(1, 0, dest2);
            return;
        }
    }
}

bool mp_obj_instance_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    mp_obj_instance_t *self = self_in;

#if MICROPY_PY_BUILTINS_PROPERTY
    // for property, we need to do a lookup first in the class dict
    // this makes all stores slow... how to fix?
    mp_obj_t member[2] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = self,
        .attr = attr,
        .meth_offset = 0,
        .dest = member,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);
    if (member[0] != MP_OBJ_NULL && MP_OBJ_IS_TYPE(member[0], &mp_type_property)) {
        // attribute already exists and is a property
        // delegate the store to the property
        const mp_obj_t *proxy = mp_obj_property_get(member[0]);
        if (proxy[1] == mp_const_none) {
            // TODO better error message
            return false;
        } else {
            mp_obj_t dest[2] = {self_in, value};
            mp_call_function_n_kw(proxy[1], 2, 0, dest);
            return true;
        }
    }
#endif

    if (value == MP_OBJ_NULL) {
        // delete attribute
        mp_map_elem_t *elem = mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
        return elem != NULL;
    } else {
        // store attribute
        mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
        return true;
    }
}

STATIC mp_obj_t instance_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_instance_t *self = self_in;
    mp_obj_t member[2] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = self,
        .meth_offset = offsetof(mp_obj_type_t, subscr),
        .dest = member,
        .is_type = false,
    };
    uint meth_args;
    if (value == MP_OBJ_NULL) {
        // delete item
        lookup.attr = MP_QSTR___delitem__;
        mp_obj_class_lookup(&lookup, self->base.type);
        meth_args = 2;
    } else if (value == MP_OBJ_SENTINEL) {
        // load item
        lookup.attr = MP_QSTR___getitem__;
        mp_obj_class_lookup(&lookup, self->base.type);
        meth_args = 2;
    } else {
        // store item
        lookup.attr = MP_QSTR___setitem__;
        mp_obj_class_lookup(&lookup, self->base.type);
        meth_args = 3;
    }
    if (member[0] == MP_OBJ_SENTINEL) {
        return mp_obj_subscr(self->subobj[0], index, value);
    } else if (member[0] != MP_OBJ_NULL) {
        mp_obj_t args[3] = {self_in, index, value};
        // TODO probably need to call instance_convert_return_attr, and use mp_call_method_n_kw
        mp_obj_t ret = mp_call_function_n_kw(member[0], meth_args, 0, args);
        if (value == MP_OBJ_SENTINEL) {
            return ret;
        } else {
            return mp_const_none;
        }
    } else {
        return MP_OBJ_NULL; // op not supported
    }
}

bool mp_obj_instance_is_callable(mp_obj_t self_in) {
    mp_obj_instance_t *self = self_in;
    mp_obj_t member[2] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = self,
        .attr = MP_QSTR___call__,
        .meth_offset = offsetof(mp_obj_type_t, call),
        .dest = member,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);
    return member[0] != MP_OBJ_NULL;
}

mp_obj_t mp_obj_instance_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_obj_instance_t *self = self_in;
    mp_obj_t member[2] = {MP_OBJ_NULL, MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = self,
        .attr = MP_QSTR___call__,
        .meth_offset = offsetof(mp_obj_type_t, call),
        .dest = member,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);
    if (member[0] == MP_OBJ_NULL) {
        if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError,
                "object not callable"));
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                "'%s' object is not callable", mp_obj_get_type_str(self_in)));
        }
    }
    if (member[0] == MP_OBJ_SENTINEL) {
        return mp_call_function_n_kw(self->subobj[0], n_args, n_kw, args);
    }
    mp_obj_t meth = mp_obj_new_bound_meth(member[0], self);
    return mp_call_function_n_kw(meth, n_args, n_kw, args);
}

STATIC mp_obj_t instance_getiter(mp_obj_t self_in) {
    mp_obj_instance_t *self = self_in;
    mp_obj_t member[2] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = self,
        .attr = MP_QSTR___iter__,
        .meth_offset = offsetof(mp_obj_type_t, getiter),
        .dest = member,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);
    if (member[0] == MP_OBJ_NULL) {
        // This kinda duplicates code in mp_getiter()
        lookup.attr = MP_QSTR___getitem__;
        lookup.meth_offset = 0; // TODO
        mp_obj_class_lookup(&lookup, self->base.type);
        if (member[0] != MP_OBJ_NULL) {
            // __getitem__ exists, create an iterator
            return mp_obj_new_getitem_iter(member);
        }
        return MP_OBJ_NULL;
    }
    if (member[0] == MP_OBJ_SENTINEL) {
        mp_obj_type_t *type = mp_obj_get_type(self->subobj[0]);
        return type->getiter(self->subobj[0]);
    }
    mp_obj_t meth = mp_obj_new_bound_meth(member[0], self);
    return mp_call_function_n_kw(meth, 0, 0, NULL);
}

/******************************************************************************/
// type object
//  - the struct is mp_obj_type_t and is defined in obj.h so const types can be made
//  - there is a constant mp_obj_type_t (called mp_type_type) for the 'type' object
//  - creating a new class (a new type) creates a new mp_obj_type_t

STATIC void type_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_type_t *self = self_in;
    print(env, "<class '%s'>", qstr_str(self->name));
}

STATIC mp_obj_t type_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    (void)type_in;

    mp_arg_check_num(n_args, n_kw, 1, 3, false);

    switch (n_args) {
        case 1:
            return mp_obj_get_type(args[0]);

        case 3:
            // args[0] = name
            // args[1] = bases tuple
            // args[2] = locals dict
            return mp_obj_new_type(mp_obj_str_get_qstr(args[0]), args[1], args[2]);

        default:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "type takes 1 or 3 arguments"));
    }
}

STATIC mp_obj_t type_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // instantiate an instance of a class

    mp_obj_type_t *self = self_in;

    if (self->make_new == NULL) {
        if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "cannot create instance"));
        } else {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                "cannot create '%s' instances", qstr_str(self->name)));
        }
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
#if MICROPY_CPYTHON_COMPAT
    if (attr == MP_QSTR___name__) {
        dest[0] = MP_OBJ_NEW_QSTR(self->name);
        return;
    }
#endif
    struct class_lookup_data lookup = {
        .obj = self_in,
        .attr = attr,
        .meth_offset = 0,
        .dest = dest,
        .is_type = true,
    };
    mp_obj_class_lookup(&lookup, self);
}

STATIC bool type_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_type));
    mp_obj_type_t *self = self_in;

    // TODO CPython allows STORE_ATTR to a class, but is this the correct implementation?

    if (self->locals_dict != NULL) {
        assert(MP_OBJ_IS_TYPE(self->locals_dict, &mp_type_dict)); // Micro Python restriction, for now
        mp_map_t *locals_map = mp_obj_dict_get_map(self->locals_dict);
        if (value == MP_OBJ_NULL) {
            // delete attribute
            mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
            // note that locals_map may be in ROM, so remove will fail in that case
            return elem != NULL;
        } else {
            // store attribute
            mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
            // note that locals_map may be in ROM, so add will fail in that case
            if (elem != NULL) {
                elem->value = value;
                return true;
            }
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
    assert(MP_OBJ_IS_TYPE(bases_tuple, &mp_type_tuple)); // Micro Python restriction, for now
    assert(MP_OBJ_IS_TYPE(locals_dict, &mp_type_dict)); // Micro Python restriction, for now

    // TODO might need to make a copy of locals_dict; at least that's how CPython does it

    // Basic validation of base classes
    mp_uint_t len;
    mp_obj_t *items;
    mp_obj_tuple_get(bases_tuple, &len, &items);
    for (uint i = 0; i < len; i++) {
        assert(MP_OBJ_IS_TYPE(items[i], &mp_type_type));
        mp_obj_type_t *t = items[i];
        // TODO: Verify with CPy, tested on function type
        if (t->make_new == NULL) {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError,
                    "type is not an acceptable base type"));
            } else {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError,
                    "type '%s' is not an acceptable base type", qstr_str(t->name)));
            }
        }
    }

    mp_obj_type_t *o = m_new0(mp_obj_type_t, 1);
    o->base.type = &mp_type_type;
    o->name = name;
    o->print = instance_print;
    o->make_new = instance_make_new;
    o->unary_op = instance_unary_op;
    o->binary_op = instance_binary_op;
    o->load_attr = mp_obj_instance_load_attr;
    o->store_attr = mp_obj_instance_store_attr;
    o->subscr = instance_subscr;
    o->call = mp_obj_instance_call;
    o->getiter = instance_getiter;
    o->bases_tuple = bases_tuple;
    o->locals_dict = locals_dict;

    const mp_obj_type_t *native_base;
    uint num_native_bases = instance_count_native_bases(o, &native_base);
    if (num_native_bases > 1) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "multiple bases have instance lay-out conflict"));
    }

    mp_map_t *locals_map = mp_obj_dict_get_map(o->locals_dict);
    mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(MP_QSTR___new__), MP_MAP_LOOKUP);
    if (elem != NULL) {
        // __new__ slot exists; check if it is a function
        if (MP_OBJ_IS_FUN(elem->value)) {
            // __new__ is a function, wrap it in a staticmethod decorator
            elem->value = static_class_method_make_new((mp_obj_t)&mp_type_staticmethod, 1, 0, &elem->value);
        }
    }

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
    (void)kind;
    mp_obj_super_t *self = self_in;
    print(env, "<super: ");
    mp_obj_print_helper(print, env, self->type, PRINT_STR);
    print(env, ", ");
    mp_obj_print_helper(print, env, self->obj, PRINT_STR);
    print(env, ">");
}

STATIC mp_obj_t super_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    // 0 arguments are turned into 2 in the compiler
    // 1 argument is not yet implemented
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    return mp_obj_new_super(args[0], args[1]);
}

// for fail, do nothing; for attr, dest[0] = value; for method, dest[0] = method, dest[1] = self
STATIC void super_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_super));
    mp_obj_super_t *self = self_in;

    assert(MP_OBJ_IS_TYPE(self->type, &mp_type_type));

    mp_obj_type_t *type = self->type;

    // for a const struct, this entry might be NULL
    if (type->bases_tuple == MP_OBJ_NULL) {
        return;
    }

    mp_uint_t len;
    mp_obj_t *items;
    mp_obj_tuple_get(type->bases_tuple, &len, &items);
    struct class_lookup_data lookup = {
        .obj = self->obj,
        .attr = attr,
        .meth_offset = 0,
        .dest = dest,
        .is_type = false,
    };
    for (uint i = 0; i < len; i++) {
        assert(MP_OBJ_IS_TYPE(items[i], &mp_type_type));
        mp_obj_class_lookup(&lookup, (mp_obj_type_t*)items[i]);
        if (dest[0] != MP_OBJ_NULL) {
            return;
        }
    }
    mp_obj_class_lookup(&lookup, &mp_type_object);
}

const mp_obj_type_t mp_type_super = {
    { &mp_type_type },
    .name = MP_QSTR_super,
    .print = super_print,
    .make_new = super_make_new,
    .load_attr = super_load_attr,
};

mp_obj_t mp_obj_new_super(mp_obj_t type, mp_obj_t obj) {
    mp_obj_super_t *o = m_new_obj(mp_obj_super_t);
    *o = (mp_obj_super_t){{&mp_type_super}, type, obj};
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
        mp_uint_t len;
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
    mp_uint_t len;
    mp_obj_t *items;
    if (MP_OBJ_IS_TYPE(classinfo, &mp_type_type)) {
        len = 1;
        items = &classinfo;
    } else if (MP_OBJ_IS_TYPE(classinfo, &mp_type_tuple)) {
        mp_obj_tuple_get(classinfo, &len, &items);
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "issubclass() arg 2 must be a class or a tuple of classes"));
    }

    for (uint i = 0; i < len; i++) {
        // We explicitly check for 'object' here since no-one explicitly derives from it
        if (items[i] == &mp_type_object || mp_obj_is_subclass_fast(object, items[i])) {
            return mp_const_true;
        }
    }
    return mp_const_false;
}

STATIC mp_obj_t mp_builtin_issubclass(mp_obj_t object, mp_obj_t classinfo) {
    if (!MP_OBJ_IS_TYPE(object, &mp_type_type)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "issubclass() arg 1 must be a class"));
    }
    return mp_obj_is_subclass(object, classinfo);
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_issubclass_obj, mp_builtin_issubclass);

STATIC mp_obj_t mp_builtin_isinstance(mp_obj_t object, mp_obj_t classinfo) {
    return mp_obj_is_subclass(mp_obj_get_type(object), classinfo);
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_isinstance_obj, mp_builtin_isinstance);

mp_obj_t mp_instance_cast_to_native_base(mp_const_obj_t self_in, mp_const_obj_t native_type) {
    mp_obj_type_t *self_type = mp_obj_get_type(self_in);
    if (!mp_obj_is_subclass_fast(self_type, native_type)) {
        return MP_OBJ_NULL;
    }
    mp_obj_instance_t *self = (mp_obj_instance_t*)self_in;
    return self->subobj[0];
}

/******************************************************************************/
// staticmethod and classmethod types (probably should go in a different file)

STATIC mp_obj_t static_class_method_make_new(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    assert(self_in == &mp_type_staticmethod || self_in == &mp_type_classmethod);

    mp_arg_check_num(n_args, n_kw, 1, 1, false);

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
