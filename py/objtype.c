/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
 * Copyright (c) 2014-2018 Paul Sokolovsky
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

#include "py/objtype.h"
#include "py/runtime.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

#define ENABLE_SPECIAL_ACCESSORS \
    (MICROPY_PY_DESCRIPTORS  || MICROPY_PY_DELATTR_SETATTR || MICROPY_PY_BUILTINS_PROPERTY)

STATIC mp_obj_t static_class_method_make_new(const mp_obj_type_t *self_in, size_t n_args, size_t n_kw, const mp_obj_t *args);

/******************************************************************************/
// instance object

STATIC int instance_count_native_bases(const mp_obj_type_t *type, const mp_obj_type_t **last_native_base) {
    int count = 0;
    for (;;) {
        if (type == &mp_type_object) {
            // Not a "real" type, end search here.
            return count;
        } else if (mp_obj_is_native_type(type)) {
            // Native types don't have parents (at least not from our perspective) so end.
            *last_native_base = type;
            return count + 1;
        } else if (type->parent == NULL) {
            // No parents so end search here.
            return count;
        #if MICROPY_MULTIPLE_INHERITANCE
        } else if (((mp_obj_base_t*)type->parent)->type == &mp_type_tuple) {
            // Multiple parents, search through them all recursively.
            const mp_obj_tuple_t *parent_tuple = type->parent;
            const mp_obj_t *item = parent_tuple->items;
            const mp_obj_t *top = item + parent_tuple->len;
            for (; item < top; ++item) {
                assert(mp_obj_is_type(*item, &mp_type_type));
                const mp_obj_type_t *bt = (const mp_obj_type_t *)MP_OBJ_TO_PTR(*item);
                count += instance_count_native_bases(bt, last_native_base);
            }
            return count;
        #endif
        } else {
            // A single parent, use iteration to continue the search.
            type = type->parent;
        }
    }
}

// This wrapper function is allows a subclass of a native type to call the
// __init__() method (corresponding to type->make_new) of the native type.
STATIC mp_obj_t native_base_init_wrapper(size_t n_args, const mp_obj_t *args) {
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(args[0]);
    const mp_obj_type_t *native_base = NULL;
    instance_count_native_bases(self->base.type, &native_base);
    self->subobj[0] = native_base->make_new(native_base, n_args - 1, 0, args + 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(native_base_init_wrapper_obj, 1, MP_OBJ_FUN_ARGS_MAX, native_base_init_wrapper);

#if !MICROPY_CPYTHON_COMPAT
STATIC
#endif
mp_obj_instance_t *mp_obj_new_instance(const mp_obj_type_t *class, const mp_obj_type_t **native_base) {
    size_t num_native_bases = instance_count_native_bases(class, native_base);
    assert(num_native_bases < 2);
    mp_obj_instance_t *o = m_new_obj_var(mp_obj_instance_t, mp_obj_t, num_native_bases);
    o->base.type = class;
    mp_map_init(&o->members, 0);
    // Initialise the native base-class slot (should be 1 at most) with a valid
    // object.  It doesn't matter which object, so long as it can be uniquely
    // distinguished from a native class that is initialised.
    if (num_native_bases != 0) {
        o->subobj[0] = MP_OBJ_FROM_PTR(&native_base_init_wrapper_obj);
    }
    return o;
}

// TODO
// This implements depth-first left-to-right MRO, which is not compliant with Python3 MRO
// http://python-history.blogspot.com/2010/06/method-resolution-order.html
// https://www.python.org/download/releases/2.3/mro/
//
// will keep lookup->dest[0]'s value (should be MP_OBJ_NULL on invocation) if attribute
// is not found
// will set lookup->dest[0] to MP_OBJ_SENTINEL if special method was found in a native
// type base via slot id (as specified by lookup->meth_offset). As there can be only one
// native base, it's known that it applies to instance->subobj[0]. In most cases, we also
// don't need to know which type it was - because instance->subobj[0] is of that type.
// The only exception is when object is not yet constructed, then we need to know base
// native type to construct its instance->subobj[0] from. But this case is handled via
// instance_count_native_bases(), which returns a native base which it saw.
struct class_lookup_data {
    mp_obj_instance_t *obj;
    qstr attr;
    size_t meth_offset;
    mp_obj_t *dest;
    bool is_type;
};

STATIC void mp_obj_class_lookup(struct class_lookup_data  *lookup, const mp_obj_type_t *type) {
    assert(lookup->dest[0] == MP_OBJ_NULL);
    assert(lookup->dest[1] == MP_OBJ_NULL);
    for (;;) {
        DEBUG_printf("mp_obj_class_lookup: Looking up %s in %s\n", qstr_str(lookup->attr), qstr_str(type->name));
        // Optimize special method lookup for native types
        // This avoids extra method_name => slot lookup. On the other hand,
        // this should not be applied to class types, as will result in extra
        // lookup either.
        if (lookup->meth_offset != 0 && mp_obj_is_native_type(type)) {
            if (*(void**)((char*)type + lookup->meth_offset) != NULL) {
                DEBUG_printf("mp_obj_class_lookup: Matched special meth slot (off=%d) for %s\n",
                    lookup->meth_offset, qstr_str(lookup->attr));
                lookup->dest[0] = MP_OBJ_SENTINEL;
                return;
            }
        }

        if (type->locals_dict != NULL) {
            // search locals_dict (the set of methods/attributes)
            assert(type->locals_dict->base.type == &mp_type_dict); // MicroPython restriction, for now
            mp_map_t *locals_map = &type->locals_dict->map;
            mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(lookup->attr), MP_MAP_LOOKUP);
            if (elem != NULL) {
                if (lookup->is_type) {
                    // If we look up a class method, we need to return original type for which we
                    // do a lookup, not a (base) type in which we found the class method.
                    const mp_obj_type_t *org_type = (const mp_obj_type_t*)lookup->obj;
                    mp_convert_member_lookup(MP_OBJ_NULL, org_type, elem->value, lookup->dest);
                } else {
                    mp_obj_instance_t *obj = lookup->obj;
                    mp_obj_t obj_obj;
                    if (obj != NULL && mp_obj_is_native_type(type) && type != &mp_type_object /* object is not a real type */) {
                        // If we're dealing with native base class, then it applies to native sub-object
                        obj_obj = obj->subobj[0];
                    } else {
                        obj_obj = MP_OBJ_FROM_PTR(obj);
                    }
                    mp_convert_member_lookup(obj_obj, type, elem->value, lookup->dest);
                }
#if DEBUG_PRINT
                DEBUG_printf("mp_obj_class_lookup: Returning: ");
                mp_obj_print_helper(MICROPY_DEBUG_PRINTER, lookup->dest[0], PRINT_REPR);
                if (lookup->dest[1] != MP_OBJ_NULL) {
                    // Don't try to repr() lookup->dest[1], as we can be called recursively
                    DEBUG_printf(" <%s @%p>", mp_obj_get_type_str(lookup->dest[1]), MP_OBJ_TO_PTR(lookup->dest[1]));
                }
                DEBUG_printf("\n");
#endif
                return;
            }
        }

        // Previous code block takes care about attributes defined in .locals_dict,
        // but some attributes of native types may be handled using .load_attr method,
        // so make sure we try to lookup those too.
        if (lookup->obj != NULL && !lookup->is_type && mp_obj_is_native_type(type) && type != &mp_type_object /* object is not a real type */) {
            mp_load_method_maybe(lookup->obj->subobj[0], lookup->attr, lookup->dest);
            if (lookup->dest[0] != MP_OBJ_NULL) {
                return;
            }
        }

        // attribute not found, keep searching base classes

        if (type->parent == NULL) {
            DEBUG_printf("mp_obj_class_lookup: No more parents\n");
            return;
        #if MICROPY_MULTIPLE_INHERITANCE
        } else if (((mp_obj_base_t*)type->parent)->type == &mp_type_tuple) {
            const mp_obj_tuple_t *parent_tuple = type->parent;
            const mp_obj_t *item = parent_tuple->items;
            const mp_obj_t *top = item + parent_tuple->len - 1;
            for (; item < top; ++item) {
                assert(mp_obj_is_type(*item, &mp_type_type));
                mp_obj_type_t *bt = (mp_obj_type_t*)MP_OBJ_TO_PTR(*item);
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
            assert(mp_obj_is_type(*item, &mp_type_type));
            type = (mp_obj_type_t*)MP_OBJ_TO_PTR(*item);
        #endif
        } else {
            type = type->parent;
        }
        if (type == &mp_type_object) {
            // Not a "real" type
            return;
        }
    }
}

STATIC void instance_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);
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
                mp_print_str(print, qstr_str(self->base.type->name));
            }
            mp_obj_print_helper(print, self->subobj[0], kind | PRINT_EXC_SUBCLASS);
        } else {
            mp_obj_print_helper(print, self->subobj[0], kind);
        }
        return;
    }

    if (member[0] != MP_OBJ_NULL) {
        mp_obj_t r = mp_call_function_1(member[0], self_in);
        mp_obj_print_helper(print, r, PRINT_STR);
        return;
    }

    // TODO: CPython prints fully-qualified type name
    mp_printf(print, "<%s object at %p>", mp_obj_get_type_str(self_in), self);
}

mp_obj_t mp_obj_instance_make_new(const mp_obj_type_t *self, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(mp_obj_is_instance_type(self));

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

    const mp_obj_type_t *native_base = NULL;
    mp_obj_instance_t *o;
    if (init_fn[0] == MP_OBJ_NULL || init_fn[0] == MP_OBJ_SENTINEL) {
        // Either there is no __new__() method defined or there is a native
        // constructor.  In both cases create a blank instance.
        o = mp_obj_new_instance(self, &native_base);

        // Since type->make_new() implements both __new__() and __init__() in
        // one go, of which the latter may be overridden by the Python subclass,
        // we defer (see the end of this function) the call of the native
        // constructor to give a chance for the Python __init__() method to call
        // said native constructor.

    } else {
        // Call Python class __new__ function with all args to create an instance
        mp_obj_t new_ret;
        if (n_args == 0 && n_kw == 0) {
            mp_obj_t args2[1] = {MP_OBJ_FROM_PTR(self)};
            new_ret = mp_call_function_n_kw(init_fn[0], 1, 0, args2);
        } else {
            mp_obj_t *args2 = m_new(mp_obj_t, 1 + n_args + 2 * n_kw);
            args2[0] = MP_OBJ_FROM_PTR(self);
            memcpy(args2 + 1, args, (n_args + 2 * n_kw) * sizeof(mp_obj_t));
            new_ret = mp_call_function_n_kw(init_fn[0], n_args + 1, n_kw, args2);
            m_del(mp_obj_t, args2, 1 + n_args + 2 * n_kw);
        }

        // https://docs.python.org/3.4/reference/datamodel.html#object.__new__
        // "If __new__() does not return an instance of cls, then the new
        // instance's __init__() method will not be invoked."
        if (mp_obj_get_type(new_ret) != self) {
            return new_ret;
        }

        // The instance returned by __new__() becomes the new object
        o = MP_OBJ_TO_PTR(new_ret);
    }

    // now call Python class __init__ function with all args
    // This method has a chance to call super().__init__() to construct a
    // possible native base class.
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
                mp_raise_TypeError("__init__() should return None");
            } else {
                mp_raise_msg_varg(&mp_type_TypeError,
                    "__init__() should return None, not '%s'", mp_obj_get_type_str(init_ret));
            }
        }

    }

    // If the type had a native base that was not explicitly initialised
    // (constructed) by the Python __init__() method then construct it now.
    if (native_base != NULL && o->subobj[0] == MP_OBJ_FROM_PTR(&native_base_init_wrapper_obj)) {
        o->subobj[0] = native_base->make_new(native_base, n_args, n_kw, args);
    }

    return MP_OBJ_FROM_PTR(o);
}

// Qstrs for special methods are guaranteed to have a small value, so we use byte
// type to represent them.
const byte mp_unary_op_method_name[MP_UNARY_OP_NUM_RUNTIME] = {
    [MP_UNARY_OP_BOOL] = MP_QSTR___bool__,
    [MP_UNARY_OP_LEN] = MP_QSTR___len__,
    [MP_UNARY_OP_HASH] = MP_QSTR___hash__,
    [MP_UNARY_OP_INT] = MP_QSTR___int__,
    #if MICROPY_PY_ALL_SPECIAL_METHODS
    [MP_UNARY_OP_POSITIVE] = MP_QSTR___pos__,
    [MP_UNARY_OP_NEGATIVE] = MP_QSTR___neg__,
    [MP_UNARY_OP_INVERT] = MP_QSTR___invert__,
    [MP_UNARY_OP_ABS] = MP_QSTR___abs__,
    #endif
    #if MICROPY_PY_SYS_GETSIZEOF
    [MP_UNARY_OP_SIZEOF] = MP_QSTR___sizeof__,
    #endif
};

STATIC mp_obj_t instance_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);

    #if MICROPY_PY_SYS_GETSIZEOF
    if (MP_UNLIKELY(op == MP_UNARY_OP_SIZEOF)) {
        // TODO: This doesn't count inherited objects (self->subobj)
        const mp_obj_type_t *native_base;
        size_t num_native_bases = instance_count_native_bases(mp_obj_get_type(self_in), &native_base);

        size_t sz = sizeof(*self) + sizeof(*self->subobj) * num_native_bases
            + sizeof(*self->members.table) * self->members.alloc;
        return MP_OBJ_NEW_SMALL_INT(sz);
    }
    #endif

    qstr op_name = mp_unary_op_method_name[op];
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
        mp_obj_t val = mp_call_function_1(member[0], self_in);

        switch (op) {
            case MP_UNARY_OP_HASH:
                // __hash__ must return a small int
                val = MP_OBJ_NEW_SMALL_INT(mp_obj_get_int_truncated(val));
                break;
            case MP_UNARY_OP_INT:
                // Must return int
                if (!mp_obj_is_int(val)) {
                    mp_raise_TypeError(NULL);
                }
                break;
            default:
                // No need to do anything
                ;
        }
        return val;
    } else {
        if (op == MP_UNARY_OP_HASH) {
            lookup.attr = MP_QSTR___eq__;
            mp_obj_class_lookup(&lookup, self->base.type);
            if (member[0] == MP_OBJ_NULL) {
                // https://docs.python.org/3/reference/datamodel.html#object.__hash__
                // "User-defined classes have __eq__() and __hash__() methods by default;
                // with them, all objects compare unequal (except with themselves) and
                // x.__hash__() returns an appropriate value such that x == y implies
                // both that x is y and hash(x) == hash(y)."
                return MP_OBJ_NEW_SMALL_INT((mp_uint_t)self_in);
            }
            // "A class that overrides __eq__() and does not define __hash__() will have its __hash__() implicitly set to None.
            // When the __hash__() method of a class is None, instances of the class will raise an appropriate TypeError"
        }

        return MP_OBJ_NULL; // op not supported
    }
}

// Binary-op enum values not listed here will have the default value of 0 in the
// table, corresponding to MP_QSTRnull, and are therefore unsupported (a lookup will
// fail).  They can be added at the expense of code size for the qstr.
// Qstrs for special methods are guaranteed to have a small value, so we use byte
// type to represent them.
const byte mp_binary_op_method_name[MP_BINARY_OP_NUM_RUNTIME] = {
    [MP_BINARY_OP_LESS] = MP_QSTR___lt__,
    [MP_BINARY_OP_MORE] = MP_QSTR___gt__,
    [MP_BINARY_OP_EQUAL] = MP_QSTR___eq__,
    [MP_BINARY_OP_LESS_EQUAL] = MP_QSTR___le__,
    [MP_BINARY_OP_MORE_EQUAL] = MP_QSTR___ge__,
    [MP_BINARY_OP_NOT_EQUAL] = MP_QSTR___ne__,
    [MP_BINARY_OP_CONTAINS] = MP_QSTR___contains__,

    // If an inplace method is not found a normal method will be used as a fallback
    [MP_BINARY_OP_INPLACE_ADD] = MP_QSTR___iadd__,
    [MP_BINARY_OP_INPLACE_SUBTRACT] = MP_QSTR___isub__,
    #if MICROPY_PY_ALL_INPLACE_SPECIAL_METHODS
    [MP_BINARY_OP_INPLACE_MULTIPLY] = MP_QSTR___imul__,
    [MP_BINARY_OP_INPLACE_MAT_MULTIPLY] = MP_QSTR___imatmul__,
    [MP_BINARY_OP_INPLACE_FLOOR_DIVIDE] = MP_QSTR___ifloordiv__,
    [MP_BINARY_OP_INPLACE_TRUE_DIVIDE] = MP_QSTR___itruediv__,
    [MP_BINARY_OP_INPLACE_MODULO] = MP_QSTR___imod__,
    [MP_BINARY_OP_INPLACE_POWER] = MP_QSTR___ipow__,
    [MP_BINARY_OP_INPLACE_OR] = MP_QSTR___ior__,
    [MP_BINARY_OP_INPLACE_XOR] = MP_QSTR___ixor__,
    [MP_BINARY_OP_INPLACE_AND] = MP_QSTR___iand__,
    [MP_BINARY_OP_INPLACE_LSHIFT] = MP_QSTR___ilshift__,
    [MP_BINARY_OP_INPLACE_RSHIFT] = MP_QSTR___irshift__,
    #endif

    [MP_BINARY_OP_ADD] = MP_QSTR___add__,
    [MP_BINARY_OP_SUBTRACT] = MP_QSTR___sub__,
    #if MICROPY_PY_ALL_SPECIAL_METHODS
    [MP_BINARY_OP_MULTIPLY] = MP_QSTR___mul__,
    [MP_BINARY_OP_MAT_MULTIPLY] = MP_QSTR___matmul__,
    [MP_BINARY_OP_FLOOR_DIVIDE] = MP_QSTR___floordiv__,
    [MP_BINARY_OP_TRUE_DIVIDE] = MP_QSTR___truediv__,
    [MP_BINARY_OP_MODULO] = MP_QSTR___mod__,
    [MP_BINARY_OP_DIVMOD] = MP_QSTR___divmod__,
    [MP_BINARY_OP_POWER] = MP_QSTR___pow__,
    [MP_BINARY_OP_OR] = MP_QSTR___or__,
    [MP_BINARY_OP_XOR] = MP_QSTR___xor__,
    [MP_BINARY_OP_AND] = MP_QSTR___and__,
    [MP_BINARY_OP_LSHIFT] = MP_QSTR___lshift__,
    [MP_BINARY_OP_RSHIFT] = MP_QSTR___rshift__,
    #endif

    #if MICROPY_PY_REVERSE_SPECIAL_METHODS
    [MP_BINARY_OP_REVERSE_ADD] = MP_QSTR___radd__,
    [MP_BINARY_OP_REVERSE_SUBTRACT] = MP_QSTR___rsub__,
    #if MICROPY_PY_ALL_SPECIAL_METHODS
    [MP_BINARY_OP_REVERSE_MULTIPLY] = MP_QSTR___rmul__,
    [MP_BINARY_OP_REVERSE_MAT_MULTIPLY] = MP_QSTR___rmatmul__,
    [MP_BINARY_OP_REVERSE_FLOOR_DIVIDE] = MP_QSTR___rfloordiv__,
    [MP_BINARY_OP_REVERSE_TRUE_DIVIDE] = MP_QSTR___rtruediv__,
    [MP_BINARY_OP_REVERSE_MODULO] = MP_QSTR___rmod__,
    [MP_BINARY_OP_REVERSE_POWER] = MP_QSTR___rpow__,
    [MP_BINARY_OP_REVERSE_OR] = MP_QSTR___ror__,
    [MP_BINARY_OP_REVERSE_XOR] = MP_QSTR___rxor__,
    [MP_BINARY_OP_REVERSE_AND] = MP_QSTR___rand__,
    [MP_BINARY_OP_REVERSE_LSHIFT] = MP_QSTR___rlshift__,
    [MP_BINARY_OP_REVERSE_RSHIFT] = MP_QSTR___rrshift__,
    #endif
    #endif
};

STATIC mp_obj_t instance_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    // Note: For ducktyping, CPython does not look in the instance members or use
    // __getattr__ or __getattribute__.  It only looks in the class dictionary.
    mp_obj_instance_t *lhs = MP_OBJ_TO_PTR(lhs_in);
retry:;
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

    mp_obj_t res;
    if (dest[0] == MP_OBJ_SENTINEL) {
        res = mp_binary_op(op, lhs->subobj[0], rhs_in);
    } else if (dest[0] != MP_OBJ_NULL) {
        dest[2] = rhs_in;
        res = mp_call_method_n_kw(1, 0, dest);
    } else {
        // If this was an inplace method, fallback to normal method
        // https://docs.python.org/3/reference/datamodel.html#object.__iadd__ :
        // "If a specific method is not defined, the augmented assignment
        // falls back to the normal methods."
        if (op >= MP_BINARY_OP_INPLACE_OR && op <= MP_BINARY_OP_INPLACE_POWER) {
            op -= MP_BINARY_OP_INPLACE_OR - MP_BINARY_OP_OR;
            goto retry;
        }
        return MP_OBJ_NULL; // op not supported
    }

    #if MICROPY_PY_BUILTINS_NOTIMPLEMENTED
    // NotImplemented means "try other fallbacks (like calling __rop__
    // instead of __op__) and if nothing works, raise TypeError". As
    // MicroPython doesn't implement any fallbacks, signal to raise
    // TypeError right away.
    if (res == mp_const_notimplemented) {
        return MP_OBJ_NULL; // op not supported
    }
    #endif

    return res;
}

STATIC void mp_obj_instance_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // logic: look in instance members then class locals
    assert(mp_obj_is_instance_type(mp_obj_get_type(self_in)));
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);

    mp_map_elem_t *elem = mp_map_lookup(&self->members, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
    if (elem != NULL) {
        // object member, always treated as a value
        dest[0] = elem->value;
        return;
    }
#if MICROPY_CPYTHON_COMPAT
    if (attr == MP_QSTR___dict__) {
        // Create a new dict with a copy of the instance's map items.
        // This creates, unlike CPython, a 'read-only' __dict__: modifying
        // it will not result in modifications to the actual instance members.
        mp_map_t *map = &self->members;
        mp_obj_t attr_dict = mp_obj_new_dict(map->used);
        for (size_t i = 0; i < map->alloc; ++i) {
            if (mp_map_slot_is_filled(map, i)) {
                mp_obj_dict_store(attr_dict, map->table[i].key, map->table[i].value);
            }
        }
        dest[0] = attr_dict;
        return;
    }
#endif
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
        if (!(self->base.type->flags & MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS)) {
            // Class doesn't have any special accessors to check so return straightaway
            return;
        }

        #if MICROPY_PY_BUILTINS_PROPERTY
        if (mp_obj_is_type(member, &mp_type_property)) {
            // object member is a property; delegate the load to the property
            // Note: This is an optimisation for code size and execution time.
            // The proper way to do it is have the functionality just below
            // in a __get__ method of the property object, and then it would
            // be called by the descriptor code down below.  But that way
            // requires overhead for the nested mp_call's and overhead for
            // the code.
            const mp_obj_t *proxy = mp_obj_property_get(member);
            if (proxy[0] == mp_const_none) {
                mp_raise_msg(&mp_type_AttributeError, "unreadable attribute");
            } else {
                dest[0] = mp_call_function_n_kw(proxy[0], 1, 0, &self_in);
            }
            return;
        }
        #endif

        #if MICROPY_PY_DESCRIPTORS
        // found a class attribute; if it has a __get__ method then call it with the
        // class instance and class as arguments and return the result
        // Note that this is functionally correct but very slow: each load_attr
        // requires an extra mp_load_method_maybe to check for the __get__.
        mp_obj_t attr_get_method[4];
        mp_load_method_maybe(member, MP_QSTR___get__, attr_get_method);
        if (attr_get_method[0] != MP_OBJ_NULL) {
            attr_get_method[2] = self_in;
            attr_get_method[3] = MP_OBJ_FROM_PTR(mp_obj_get_type(self_in));
            dest[0] = mp_call_method_n_kw(2, 0, attr_get_method);
        }
        #endif
        return;
    }

    // try __getattr__
    if (attr != MP_QSTR___getattr__) {
        #if MICROPY_PY_DELATTR_SETATTR
        // If the requested attr is __setattr__/__delattr__ then don't delegate the lookup
        // to __getattr__.  If we followed CPython's behaviour then __setattr__/__delattr__
        // would have already been found in the "object" base class.
        if (attr == MP_QSTR___setattr__ || attr == MP_QSTR___delattr__) {
            return;
        }
        #endif

        mp_obj_t dest2[3];
        mp_load_method_maybe(self_in, MP_QSTR___getattr__, dest2);
        if (dest2[0] != MP_OBJ_NULL) {
            // __getattr__ exists, call it and return its result
            dest2[2] = MP_OBJ_NEW_QSTR(attr);
            dest[0] = mp_call_method_n_kw(1, 0, dest2);
            return;
        }
    }
}

STATIC bool mp_obj_instance_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);

    if (!(self->base.type->flags & MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS)) {
        // Class doesn't have any special accessors so skip their checks
        goto skip_special_accessors;
    }

    #if MICROPY_PY_BUILTINS_PROPERTY || MICROPY_PY_DESCRIPTORS
    // With property and/or descriptors enabled we need to do a lookup
    // first in the class dict for the attribute to see if the store should
    // be delegated.
    mp_obj_t member[2] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = self,
        .attr = attr,
        .meth_offset = 0,
        .dest = member,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);

    if (member[0] != MP_OBJ_NULL) {
        #if MICROPY_PY_BUILTINS_PROPERTY
        if (mp_obj_is_type(member[0], &mp_type_property)) {
            // attribute exists and is a property; delegate the store/delete
            // Note: This is an optimisation for code size and execution time.
            // The proper way to do it is have the functionality just below in
            // a __set__/__delete__ method of the property object, and then it
            // would be called by the descriptor code down below.  But that way
            // requires overhead for the nested mp_call's and overhead for
            // the code.
            const mp_obj_t *proxy = mp_obj_property_get(member[0]);
            mp_obj_t dest[2] = {self_in, value};
            if (value == MP_OBJ_NULL) {
                // delete attribute
                if (proxy[2] == mp_const_none) {
                    // TODO better error message?
                    return false;
                } else {
                    mp_call_function_n_kw(proxy[2], 1, 0, dest);
                    return true;
                }
            } else {
                // store attribute
                if (proxy[1] == mp_const_none) {
                    // TODO better error message?
                    return false;
                } else {
                    mp_call_function_n_kw(proxy[1], 2, 0, dest);
                    return true;
                }
            }
        }
        #endif

        #if MICROPY_PY_DESCRIPTORS
        // found a class attribute; if it has a __set__/__delete__ method then
        // call it with the class instance (and value) as arguments
        if (value == MP_OBJ_NULL) {
            // delete attribute
            mp_obj_t attr_delete_method[3];
            mp_load_method_maybe(member[0], MP_QSTR___delete__, attr_delete_method);
            if (attr_delete_method[0] != MP_OBJ_NULL) {
                attr_delete_method[2] = self_in;
                mp_call_method_n_kw(1, 0, attr_delete_method);
                return true;
            }
        } else {
            // store attribute
            mp_obj_t attr_set_method[4];
            mp_load_method_maybe(member[0], MP_QSTR___set__, attr_set_method);
            if (attr_set_method[0] != MP_OBJ_NULL) {
                attr_set_method[2] = self_in;
                attr_set_method[3] = value;
                mp_call_method_n_kw(2, 0, attr_set_method);
                return true;
            }
        }
        #endif
    }
    #endif

    #if MICROPY_PY_DELATTR_SETATTR
    if (value == MP_OBJ_NULL) {
        // delete attribute
        // try __delattr__ first
        mp_obj_t attr_delattr_method[3];
        mp_load_method_maybe(self_in, MP_QSTR___delattr__, attr_delattr_method);
        if (attr_delattr_method[0] != MP_OBJ_NULL) {
            // __delattr__ exists, so call it
            attr_delattr_method[2] = MP_OBJ_NEW_QSTR(attr);
            mp_call_method_n_kw(1, 0, attr_delattr_method);
            return true;
        }
    } else {
        // store attribute
        // try __setattr__ first
        mp_obj_t attr_setattr_method[4];
        mp_load_method_maybe(self_in, MP_QSTR___setattr__, attr_setattr_method);
        if (attr_setattr_method[0] != MP_OBJ_NULL) {
            // __setattr__ exists, so call it
            attr_setattr_method[2] = MP_OBJ_NEW_QSTR(attr);
            attr_setattr_method[3] = value;
            mp_call_method_n_kw(2, 0, attr_setattr_method);
            return true;
        }
    }
    #endif

skip_special_accessors:

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

STATIC void mp_obj_instance_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] == MP_OBJ_NULL) {
        mp_obj_instance_load_attr(self_in, attr, dest);
    } else {
        if (mp_obj_instance_store_attr(self_in, attr, dest[1])) {
            dest[0] = MP_OBJ_NULL; // indicate success
        }
    }
}

STATIC mp_obj_t instance_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t member[4] = {MP_OBJ_NULL, MP_OBJ_NULL, index, value};
    struct class_lookup_data lookup = {
        .obj = self,
        .meth_offset = offsetof(mp_obj_type_t, subscr),
        .dest = member,
        .is_type = false,
    };
    if (value == MP_OBJ_NULL) {
        // delete item
        lookup.attr = MP_QSTR___delitem__;
    } else if (value == MP_OBJ_SENTINEL) {
        // load item
        lookup.attr = MP_QSTR___getitem__;
    } else {
        // store item
        lookup.attr = MP_QSTR___setitem__;
    }
    mp_obj_class_lookup(&lookup, self->base.type);
    if (member[0] == MP_OBJ_SENTINEL) {
        return mp_obj_subscr(self->subobj[0], index, value);
    } else if (member[0] != MP_OBJ_NULL) {
        size_t n_args = value == MP_OBJ_NULL || value == MP_OBJ_SENTINEL ? 1 : 2;
        mp_obj_t ret = mp_call_method_n_kw(n_args, 0, member);
        if (value == MP_OBJ_SENTINEL) {
            return ret;
        } else {
            return mp_const_none;
        }
    } else {
        return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t mp_obj_instance_get_call(mp_obj_t self_in, mp_obj_t *member) {
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);
    struct class_lookup_data lookup = {
        .obj = self,
        .attr = MP_QSTR___call__,
        .meth_offset = offsetof(mp_obj_type_t, call),
        .dest = member,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);
    return member[0];
}

bool mp_obj_instance_is_callable(mp_obj_t self_in) {
    mp_obj_t member[2] = {MP_OBJ_NULL, MP_OBJ_NULL};
    return mp_obj_instance_get_call(self_in, member) != MP_OBJ_NULL;
}

mp_obj_t mp_obj_instance_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_t member[2] = {MP_OBJ_NULL, MP_OBJ_NULL};
    mp_obj_t call = mp_obj_instance_get_call(self_in, member);
    if (call == MP_OBJ_NULL) {
        if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
            mp_raise_TypeError("object not callable");
        } else {
            mp_raise_msg_varg(&mp_type_TypeError,
                "'%s' object isn't callable", mp_obj_get_type_str(self_in));
        }
    }
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);
    if (call == MP_OBJ_SENTINEL) {
        return mp_call_function_n_kw(self->subobj[0], n_args, n_kw, args);
    }

    return mp_call_method_self_n_kw(member[0], member[1], n_args, n_kw, args);
}

// Note that iter_buf may be NULL, and needs to be allocated if needed
mp_obj_t mp_obj_instance_getiter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);
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
        return MP_OBJ_NULL;
    } else if (member[0] == MP_OBJ_SENTINEL) {
        const mp_obj_type_t *type = mp_obj_get_type(self->subobj[0]);
        if (iter_buf == NULL) {
            iter_buf = m_new_obj(mp_obj_iter_buf_t);
        }
        return type->getiter(self->subobj[0], iter_buf);
    } else {
        return mp_call_method_n_kw(0, 0, member);
    }
}

STATIC mp_int_t instance_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mp_obj_instance_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t member[2] = {MP_OBJ_NULL};
    struct class_lookup_data lookup = {
        .obj = self,
        .attr = MP_QSTR_, // don't actually look for a method
        .meth_offset = offsetof(mp_obj_type_t, buffer_p.get_buffer),
        .dest = member,
        .is_type = false,
    };
    mp_obj_class_lookup(&lookup, self->base.type);
    if (member[0] == MP_OBJ_SENTINEL) {
        const mp_obj_type_t *type = mp_obj_get_type(self->subobj[0]);
        return type->buffer_p.get_buffer(self->subobj[0], bufinfo, flags);
    } else {
        return 1; // object does not support buffer protocol
    }
}

/******************************************************************************/
// type object
//  - the struct is mp_obj_type_t and is defined in obj.h so const types can be made
//  - there is a constant mp_obj_type_t (called mp_type_type) for the 'type' object
//  - creating a new class (a new type) creates a new mp_obj_type_t

#if ENABLE_SPECIAL_ACCESSORS
STATIC bool check_for_special_accessors(mp_obj_t key, mp_obj_t value) {
    #if MICROPY_PY_DELATTR_SETATTR
    if (key == MP_OBJ_NEW_QSTR(MP_QSTR___setattr__) || key == MP_OBJ_NEW_QSTR(MP_QSTR___delattr__)) {
        return true;
    }
    #endif
    #if MICROPY_PY_BUILTINS_PROPERTY
    if (mp_obj_is_type(value, &mp_type_property)) {
        return true;
    }
    #endif
    #if MICROPY_PY_DESCRIPTORS
    static const uint8_t to_check[] = {
        MP_QSTR___get__, MP_QSTR___set__, MP_QSTR___delete__,
    };
    for (size_t i = 0; i < MP_ARRAY_SIZE(to_check); ++i) {
        mp_obj_t dest_temp[2];
        mp_load_method_protected(value, to_check[i], dest_temp, true);
        if (dest_temp[0] != MP_OBJ_NULL) {
            return true;
        }
    }
    #endif
    return false;
}
#endif

STATIC void type_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_type_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<class '%q'>", self->name);
}

STATIC mp_obj_t type_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;

    mp_arg_check_num(n_args, n_kw, 1, 3, false);

    switch (n_args) {
        case 1:
            return MP_OBJ_FROM_PTR(mp_obj_get_type(args[0]));

        case 3:
            // args[0] = name
            // args[1] = bases tuple
            // args[2] = locals dict
            return mp_obj_new_type(mp_obj_str_get_qstr(args[0]), args[1], args[2]);

        default:
            mp_raise_TypeError("type takes 1 or 3 arguments");
    }
}

STATIC mp_obj_t type_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // instantiate an instance of a class

    mp_obj_type_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->make_new == NULL) {
        if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
            mp_raise_TypeError("cannot create instance");
        } else {
            mp_raise_msg_varg(&mp_type_TypeError, "cannot create '%q' instances", self->name);
        }
    }

    // make new instance
    mp_obj_t o = self->make_new(self, n_args, n_kw, args);

    // return new instance
    return o;
}

STATIC void type_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    assert(mp_obj_is_type(self_in, &mp_type_type));
    mp_obj_type_t *self = MP_OBJ_TO_PTR(self_in);

    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        #if MICROPY_CPYTHON_COMPAT
        if (attr == MP_QSTR___name__) {
            dest[0] = MP_OBJ_NEW_QSTR(self->name);
            return;
        }
        if (attr == MP_QSTR___bases__) {
            if (self == &mp_type_object) {
                dest[0] = mp_const_empty_tuple;
                return;
            }
            mp_obj_t parent_obj = self->parent ? MP_OBJ_FROM_PTR(self->parent) : MP_OBJ_FROM_PTR(&mp_type_object);
            #if MICROPY_MULTIPLE_INHERITANCE
            if (mp_obj_is_type(parent_obj, &mp_type_tuple)) {
                dest[0] = parent_obj;
                return;
            }
            #endif
            dest[0] = mp_obj_new_tuple(1, &parent_obj);
            return;
        }
        #endif
        struct class_lookup_data lookup = {
            .obj = (mp_obj_instance_t*)self,
            .attr = attr,
            .meth_offset = 0,
            .dest = dest,
            .is_type = true,
        };
        mp_obj_class_lookup(&lookup, self);
    } else {
        // delete/store attribute

        if (self->locals_dict != NULL) {
            assert(self->locals_dict->base.type == &mp_type_dict); // MicroPython restriction, for now
            mp_map_t *locals_map = &self->locals_dict->map;
            if (locals_map->is_fixed) {
                // can't apply delete/store to a fixed map
                return;
            }
            if (dest[1] == MP_OBJ_NULL) {
                // delete attribute
                mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_REMOVE_IF_FOUND);
                if (elem != NULL) {
                    dest[0] = MP_OBJ_NULL; // indicate success
                }
            } else {
                #if ENABLE_SPECIAL_ACCESSORS
                // Check if we add any special accessor methods with this store
                if (!(self->flags & MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS)) {
                    if (check_for_special_accessors(MP_OBJ_NEW_QSTR(attr), dest[1])) {
                        if (self->flags & MP_TYPE_FLAG_IS_SUBCLASSED) {
                            // This class is already subclassed so can't have special accessors added
                            mp_raise_msg(&mp_type_AttributeError, "can't add special method to already-subclassed class");
                        }
                        self->flags |= MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS;
                    }
                }
                #endif

                // store attribute
                mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
                elem->value = dest[1];
                dest[0] = MP_OBJ_NULL; // indicate success
            }
        }
    }
}

const mp_obj_type_t mp_type_type = {
    { &mp_type_type },
    .name = MP_QSTR_type,
    .print = type_print,
    .make_new = type_make_new,
    .call = type_call,
    .unary_op = mp_generic_unary_op,
    .attr = type_attr,
};

mp_obj_t mp_obj_new_type(qstr name, mp_obj_t bases_tuple, mp_obj_t locals_dict) {
    // Verify input objects have expected type
    if (!mp_obj_is_type(bases_tuple, &mp_type_tuple)) {
        mp_raise_TypeError(NULL);
    }
    if (!mp_obj_is_type(locals_dict, &mp_type_dict)) {
        mp_raise_TypeError(NULL);
    }

    // TODO might need to make a copy of locals_dict; at least that's how CPython does it

    // Basic validation of base classes
    uint16_t base_flags = MP_TYPE_FLAG_EQ_NOT_REFLEXIVE
        | MP_TYPE_FLAG_EQ_CHECKS_OTHER_TYPE | MP_TYPE_FLAG_EQ_HAS_NEQ_TEST;
    size_t bases_len;
    mp_obj_t *bases_items;
    mp_obj_tuple_get(bases_tuple, &bases_len, &bases_items);
    for (size_t i = 0; i < bases_len; i++) {
        if (!mp_obj_is_type(bases_items[i], &mp_type_type)) {
            mp_raise_TypeError(NULL);
        }
        mp_obj_type_t *t = MP_OBJ_TO_PTR(bases_items[i]);
        // TODO: Verify with CPy, tested on function type
        if (t->make_new == NULL) {
            if (MICROPY_ERROR_REPORTING == MICROPY_ERROR_REPORTING_TERSE) {
                mp_raise_TypeError("type isn't an acceptable base type");
            } else {
                mp_raise_msg_varg(&mp_type_TypeError,
                    "type '%q' isn't an acceptable base type", t->name);
            }
        }
        #if ENABLE_SPECIAL_ACCESSORS
        if (mp_obj_is_instance_type(t)) {
            t->flags |= MP_TYPE_FLAG_IS_SUBCLASSED;
            base_flags |= t->flags & MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS;
        }
        #endif
    }

    mp_obj_type_t *o = m_new0(mp_obj_type_t, 1);
    o->base.type = &mp_type_type;
    o->flags = base_flags;
    o->name = name;
    o->print = instance_print;
    o->make_new = mp_obj_instance_make_new;
    o->call = mp_obj_instance_call;
    o->unary_op = instance_unary_op;
    o->binary_op = instance_binary_op;
    o->attr = mp_obj_instance_attr;
    o->subscr = instance_subscr;
    o->getiter = mp_obj_instance_getiter;
    //o->iternext = ; not implemented
    o->buffer_p.get_buffer = instance_get_buffer;

    if (bases_len > 0) {
        // Inherit protocol from a base class. This allows to define an
        // abstract base class which would translate C-level protocol to
        // Python method calls, and any subclass inheriting from it will
        // support this feature.
        o->protocol = ((mp_obj_type_t*)MP_OBJ_TO_PTR(bases_items[0]))->protocol;

        if (bases_len >= 2) {
            #if MICROPY_MULTIPLE_INHERITANCE
            o->parent = MP_OBJ_TO_PTR(bases_tuple);
            #else
            mp_raise_NotImplementedError("multiple inheritance not supported");
            #endif
        } else {
            o->parent = MP_OBJ_TO_PTR(bases_items[0]);
        }
    }

    o->locals_dict = MP_OBJ_TO_PTR(locals_dict);

    #if ENABLE_SPECIAL_ACCESSORS
    // Check if the class has any special accessor methods
    if (!(o->flags & MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS)) {
        for (size_t i = 0; i < o->locals_dict->map.alloc; i++) {
            if (mp_map_slot_is_filled(&o->locals_dict->map, i)) {
                const mp_map_elem_t *elem = &o->locals_dict->map.table[i];
                if (check_for_special_accessors(elem->key, elem->value)) {
                    o->flags |= MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS;
                    break;
                }
            }
        }
    }
    #endif

    const mp_obj_type_t *native_base;
    size_t num_native_bases = instance_count_native_bases(o, &native_base);
    if (num_native_bases > 1) {
        mp_raise_TypeError("multiple bases have instance lay-out conflict");
    }

    mp_map_t *locals_map = &o->locals_dict->map;
    mp_map_elem_t *elem = mp_map_lookup(locals_map, MP_OBJ_NEW_QSTR(MP_QSTR___new__), MP_MAP_LOOKUP);
    if (elem != NULL) {
        // __new__ slot exists; check if it is a function
        if (mp_obj_is_fun(elem->value)) {
            // __new__ is a function, wrap it in a staticmethod decorator
            elem->value = static_class_method_make_new(&mp_type_staticmethod, 1, 0, &elem->value);
        }
    }

    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/
// super object

typedef struct _mp_obj_super_t {
    mp_obj_base_t base;
    mp_obj_t type;
    mp_obj_t obj;
} mp_obj_super_t;

STATIC void super_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_super_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "<super: ");
    mp_obj_print_helper(print, self->type, PRINT_STR);
    mp_print_str(print, ", ");
    mp_obj_print_helper(print, self->obj, PRINT_STR);
    mp_print_str(print, ">");
}

STATIC mp_obj_t super_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    // 0 arguments are turned into 2 in the compiler
    // 1 argument is not yet implemented
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    if (!mp_obj_is_type(args[0], &mp_type_type)) {
        mp_raise_TypeError(NULL);
    }
    mp_obj_super_t *o = m_new_obj(mp_obj_super_t);
    *o = (mp_obj_super_t){{type_in}, args[0], args[1]};
    return MP_OBJ_FROM_PTR(o);
}

STATIC void super_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // not load attribute
        return;
    }

    assert(mp_obj_is_type(self_in, &mp_type_super));
    mp_obj_super_t *self = MP_OBJ_TO_PTR(self_in);

    assert(mp_obj_is_type(self->type, &mp_type_type));

    mp_obj_type_t *type = MP_OBJ_TO_PTR(self->type);

    struct class_lookup_data lookup = {
        .obj = MP_OBJ_TO_PTR(self->obj),
        .attr = attr,
        .meth_offset = 0,
        .dest = dest,
        .is_type = false,
    };

    // Allow a call super().__init__() to reach any native base classes
    if (attr == MP_QSTR___init__) {
        lookup.meth_offset = offsetof(mp_obj_type_t, make_new);
    }

    if (type->parent == NULL) {
        // no parents, do nothing
    #if MICROPY_MULTIPLE_INHERITANCE
    } else if (((mp_obj_base_t*)type->parent)->type == &mp_type_tuple) {
        const mp_obj_tuple_t *parent_tuple = type->parent;
        size_t len = parent_tuple->len;
        const mp_obj_t *items = parent_tuple->items;
        for (size_t i = 0; i < len; i++) {
            assert(mp_obj_is_type(items[i], &mp_type_type));
            if (MP_OBJ_TO_PTR(items[i]) == &mp_type_object) {
                // The "object" type will be searched at the end of this function,
                // and we don't want to lookup native methods in object.
                continue;
            }
            mp_obj_class_lookup(&lookup, (mp_obj_type_t*)MP_OBJ_TO_PTR(items[i]));
            if (dest[0] != MP_OBJ_NULL) {
                break;
            }
        }
    #endif
    } else if (type->parent != &mp_type_object) {
        mp_obj_class_lookup(&lookup, type->parent);
    }

    if (dest[0] != MP_OBJ_NULL) {
        if (dest[0] == MP_OBJ_SENTINEL) {
            // Looked up native __init__ so defer to it
            dest[0] = MP_OBJ_FROM_PTR(&native_base_init_wrapper_obj);
            dest[1] = self->obj;
        }
        return;
    }

    // Reset meth_offset so we don't look up any native methods in object,
    // because object never takes up the native base-class slot.
    lookup.meth_offset = 0;

    mp_obj_class_lookup(&lookup, &mp_type_object);
}

const mp_obj_type_t mp_type_super = {
    { &mp_type_type },
    .name = MP_QSTR_super,
    .print = super_print,
    .make_new = super_make_new,
    .attr = super_attr,
};

void mp_load_super_method(qstr attr, mp_obj_t *dest) {
    mp_obj_super_t super = {{&mp_type_super}, dest[1], dest[2]};
    mp_load_method(MP_OBJ_FROM_PTR(&super), attr, dest);
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
        if (!mp_obj_is_type(object, &mp_type_type)) {
            return false;
        }

        const mp_obj_type_t *self = MP_OBJ_TO_PTR(object);

        if (self->parent == NULL) {
            // type has no parents
            return false;
        #if MICROPY_MULTIPLE_INHERITANCE
        } else if (((mp_obj_base_t*)self->parent)->type == &mp_type_tuple) {
            // get the base objects (they should be type objects)
            const mp_obj_tuple_t *parent_tuple = self->parent;
            const mp_obj_t *item = parent_tuple->items;
            const mp_obj_t *top = item + parent_tuple->len - 1;

            // iterate through the base objects
            for (; item < top; ++item) {
                if (mp_obj_is_subclass_fast(*item, classinfo)) {
                    return true;
                }
            }

            // search last base (simple tail recursion elimination)
            object = *item;
        #endif
        } else {
            // type has 1 parent
            object = MP_OBJ_FROM_PTR(self->parent);
        }
    }
}

STATIC mp_obj_t mp_obj_is_subclass(mp_obj_t object, mp_obj_t classinfo) {
    size_t len;
    mp_obj_t *items;
    if (mp_obj_is_type(classinfo, &mp_type_type)) {
        len = 1;
        items = &classinfo;
    } else if (mp_obj_is_type(classinfo, &mp_type_tuple)) {
        mp_obj_tuple_get(classinfo, &len, &items);
    } else {
        mp_raise_TypeError("issubclass() arg 2 must be a class or a tuple of classes");
    }

    for (size_t i = 0; i < len; i++) {
        // We explicitly check for 'object' here since no-one explicitly derives from it
        if (items[i] == MP_OBJ_FROM_PTR(&mp_type_object) || mp_obj_is_subclass_fast(object, items[i])) {
            return mp_const_true;
        }
    }
    return mp_const_false;
}

STATIC mp_obj_t mp_builtin_issubclass(mp_obj_t object, mp_obj_t classinfo) {
    if (!mp_obj_is_type(object, &mp_type_type)) {
        mp_raise_TypeError("issubclass() arg 1 must be a class");
    }
    return mp_obj_is_subclass(object, classinfo);
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_issubclass_obj, mp_builtin_issubclass);

STATIC mp_obj_t mp_builtin_isinstance(mp_obj_t object, mp_obj_t classinfo) {
    return mp_obj_is_subclass(MP_OBJ_FROM_PTR(mp_obj_get_type(object)), classinfo);
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_builtin_isinstance_obj, mp_builtin_isinstance);

mp_obj_t mp_instance_cast_to_native_base(mp_const_obj_t self_in, mp_const_obj_t native_type) {
    const mp_obj_type_t *self_type = mp_obj_get_type(self_in);
    if (!mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(self_type), native_type)) {
        return MP_OBJ_NULL;
    }
    mp_obj_instance_t *self = (mp_obj_instance_t*)MP_OBJ_TO_PTR(self_in);
    return self->subobj[0];
}

/******************************************************************************/
// staticmethod and classmethod types (probably should go in a different file)

STATIC mp_obj_t static_class_method_make_new(const mp_obj_type_t *self, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(self == &mp_type_staticmethod || self == &mp_type_classmethod);

    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_static_class_method_t *o = m_new_obj(mp_obj_static_class_method_t);
    *o = (mp_obj_static_class_method_t){{self}, args[0]};
    return MP_OBJ_FROM_PTR(o);
}

const mp_obj_type_t mp_type_staticmethod = {
    { &mp_type_type },
    .name = MP_QSTR_staticmethod,
    .make_new = static_class_method_make_new,
};

const mp_obj_type_t mp_type_classmethod = {
    { &mp_type_type },
    .name = MP_QSTR_classmethod,
    .make_new = static_class_method_make_new,
};
