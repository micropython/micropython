#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"
#include "binary.h"

typedef struct _mp_obj_array_t {
    mp_obj_base_t base;
    machine_uint_t typecode : 8;
    // free is number of unused elements after len used elements
    // alloc size = len + free
    machine_uint_t free : (8 * sizeof(machine_uint_t) - 8);
    machine_uint_t len; // in elements
    void *items;
} mp_obj_array_t;

STATIC mp_obj_t array_iterator_new(mp_obj_t array_in);
STATIC mp_obj_array_t *array_new(char typecode, uint n);
STATIC mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg);

/******************************************************************************/
/* array                                                                       */

STATIC void array_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_obj_array_t *o = o_in;
    if (o->typecode == BYTEARRAY_TYPECODE) {
        print(env, "bytearray(b", o->typecode);
        mp_str_print_quoted(print, env, o->items, o->len);
    } else {
        print(env, "array('%c'", o->typecode);
        if (o->len > 0) {
            print(env, ", [", o->typecode);
            for (int i = 0; i < o->len; i++) {
                if (i > 0) {
                    print(env, ", ");
                }
                mp_obj_print_helper(print, env, mp_binary_get_val(o->typecode, o->items, i), PRINT_REPR);
            }
            print(env, "]");
        }
    }
    print(env, ")");
}

STATIC mp_obj_t array_construct(char typecode, mp_obj_t initializer) {
    uint len;
    // Try to create array of exact len if initializer len is known
    mp_obj_t len_in = mp_obj_len_maybe(initializer);
    if (len_in == MP_OBJ_NULL) {
        len = 0;
    } else {
        len = MP_OBJ_SMALL_INT_VALUE(len_in);
    }

    mp_obj_array_t *array = array_new(typecode, len);

    mp_obj_t iterable = mp_getiter(initializer);
    mp_obj_t item;
    int i = 0;
    while ((item = mp_iternext(iterable)) != MP_OBJ_NULL) {
        if (len == 0) {
            array_append(array, item);
        } else {
            mp_binary_set_val(typecode, array->items, i++, item);
        }
    }

    return array;
}

STATIC mp_obj_t array_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    if (n_args < 1 || n_args > 2) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "unexpected # of arguments, %d given", n_args));
    }
    // TODO check args
    uint l;
    const char *typecode = mp_obj_str_get_data(args[0], &l);
    if (n_args == 1) {
        return array_new(*typecode, 0);
    }

    return array_construct(*typecode, args[1]);
}

// This is top-level factory function, not virtual method
// TODO: "bytearray" really should be type, not function
STATIC mp_obj_t mp_builtin_bytearray(mp_obj_t arg) {
    return array_construct(BYTEARRAY_TYPECODE, arg);
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_bytearray_obj, mp_builtin_bytearray);

STATIC mp_obj_t array_unary_op(int op, mp_obj_t o_in) {
    mp_obj_array_t *o = o_in;
    switch (op) {
        case MP_UNARY_OP_BOOL: return MP_BOOL(o->len != 0);
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(o->len);
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC mp_obj_t array_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_array_t *o = lhs;
    switch (op) {
        case MP_BINARY_OP_SUBSCR:
        {
            uint index = mp_get_index(o->base.type, o->len, rhs, false);
            return mp_binary_get_val(o->typecode, o->items, index);
        }

        default:
            // op not supported
            return MP_OBJ_NULL;
    }
}

STATIC mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_array));
    mp_obj_array_t *self = self_in;
    if (self->free == 0) {
        int item_sz = mp_binary_get_size(self->typecode);
        // TODO: alloc policy
        self->free = 8;
        self->items = m_realloc(self->items,  item_sz * self->len, item_sz * (self->len + self->free));
    }
    mp_binary_set_val(self->typecode, self->items, self->len++, arg);
    self->free--;
    return mp_const_none; // return None, as per CPython
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(array_append_obj, array_append);

STATIC bool array_store_item(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    mp_obj_array_t *o = self_in;
    uint index = mp_get_index(o->base.type, o->len, index_in, false);
    mp_binary_set_val(o->typecode, o->items, index, value);
    return true;
}

STATIC machine_int_t array_get_buffer(mp_obj_t o_in, buffer_info_t *bufinfo, int flags) {
    mp_obj_array_t *o = o_in;
    bufinfo->buf = o->items;
    bufinfo->len = o->len * mp_binary_get_size(o->typecode);
    return 0;
}

STATIC const mp_map_elem_t array_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_append), (mp_obj_t)&array_append_obj },
};

STATIC MP_DEFINE_CONST_DICT(array_locals_dict, array_locals_dict_table);

const mp_obj_type_t mp_type_array = {
    { &mp_type_type },
    .name = MP_QSTR_array,
    .print = array_print,
    .make_new = array_make_new,
    .getiter = array_iterator_new,
    .unary_op = array_unary_op,
    .binary_op = array_binary_op,
    .store_item = array_store_item,
    .buffer_p = { .get_buffer = array_get_buffer },
    .locals_dict = (mp_obj_t)&array_locals_dict,
};

STATIC mp_obj_array_t *array_new(char typecode, uint n) {
    mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
    o->base.type = &mp_type_array;
    o->typecode = typecode;
    o->free = 0;
    o->len = n;
    o->items = m_malloc(mp_binary_get_size(typecode) * o->len);
    return o;
}

uint mp_obj_array_len(mp_obj_t self_in) {
    return ((mp_obj_array_t *)self_in)->len;
}

mp_obj_t mp_obj_new_bytearray(uint n, void *items) {
    mp_obj_array_t *o = array_new(BYTEARRAY_TYPECODE, n);
    memcpy(o->items, items, n);
    return o;
}

// Create bytearray which references specified memory area
mp_obj_t mp_obj_new_bytearray_by_ref(uint n, void *items) {
    mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
    o->base.type = &mp_type_array;
    o->typecode = BYTEARRAY_TYPECODE;
    o->free = 0;
    o->len = n;
    o->items = items;
    return o;
}

/******************************************************************************/
/* array iterator                                                              */

typedef struct _mp_obj_array_it_t {
    mp_obj_base_t base;
    mp_obj_array_t *array;
    machine_uint_t cur;
} mp_obj_array_it_t;

STATIC mp_obj_t array_it_iternext(mp_obj_t self_in) {
    mp_obj_array_it_t *self = self_in;
    if (self->cur < self->array->len) {
        return mp_binary_get_val(self->array->typecode, self->array->items, self->cur++);
    } else {
        return MP_OBJ_NULL;
    }
}

STATIC const mp_obj_type_t array_it_type = {
    { &mp_type_type },
    .name = MP_QSTR_iterator,
    .getiter = mp_identity,
    .iternext = array_it_iternext,
};

STATIC mp_obj_t array_iterator_new(mp_obj_t array_in) {
    mp_obj_array_t *array = array_in;
    mp_obj_array_it_t *o = m_new_obj(mp_obj_array_it_t);
    o->base.type = &array_it_type;
    o->array = array;
    o->cur = 0;
    return o;
}
