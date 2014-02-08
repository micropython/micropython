#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime0.h"
#include "runtime.h"

// Use special typecode to differentiate repr() of bytearray vs array.array('B')
// (underlyingly they're same).
#define BYTEARRAY_TYPECODE 0

typedef struct _mp_obj_array_t {
    mp_obj_base_t base;
    struct {
        machine_uint_t typecode : 8;
        // free is number of unused elements after len used elements
        // alloc size = len + free
        machine_uint_t free : (8 * sizeof(machine_uint_t) - 8);
    };
    machine_uint_t len; // in elements
    void *items;
} mp_obj_array_t;

static mp_obj_t array_iterator_new(mp_obj_t array_in);
static mp_obj_array_t *array_new(char typecode, uint n);
static mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg);

/******************************************************************************/
/* array                                                                       */

static machine_int_t array_get_el_size(char typecode) {
    // This assumes that unsigned and signed types are of the same type,
    // which is invariant for [u]intN_t.
    switch (typecode) {
        case BYTEARRAY_TYPECODE:
        case 'b':
        case 'B':
            return sizeof(int8_t);
        case 'h':
        case 'H':
            return sizeof(int16_t);
        case 'i':
        case 'I':
            return sizeof(int32_t);
        case 'l':
        case 'L':
            return sizeof(int32_t);
    }
    return -1;
}

static machine_int_t array_get_el(mp_obj_array_t *o, int index) {
    machine_int_t val = 0;
    switch (o->typecode) {
        case 'b':
            val = ((int8_t*)o->items)[index];
            break;
        case BYTEARRAY_TYPECODE:
        case 'B':
            val = ((uint8_t*)o->items)[index];
            break;
        case 'h':
            val = ((int16_t*)o->items)[index];
            break;
        case 'H':
            val = ((uint16_t*)o->items)[index];
            break;
        case 'i':
            val = ((int32_t*)o->items)[index];
            break;
        case 'I':
            val = ((uint32_t*)o->items)[index];
            break;
        case 'l':
            val = ((int32_t*)o->items)[index];
            break;
        case 'L':
            val = ((uint32_t*)o->items)[index];
            break;
    }
    return val;
}

static void array_set_el(mp_obj_array_t *o, int index, mp_obj_t val_in) {
    machine_int_t val = mp_obj_int_get(val_in);
    switch (o->typecode) {
        case 'b':
            ((int8_t*)o->items)[index] = val;
            break;
        case BYTEARRAY_TYPECODE:
        case 'B':
            ((uint8_t*)o->items)[index] = val;
            break;
        case 'h':
            ((int16_t*)o->items)[index] = val;
            break;
        case 'H':
            ((uint16_t*)o->items)[index] = val;
            break;
        case 'i':
            ((int32_t*)o->items)[index] = val;
            break;
        case 'I':
            ((uint32_t*)o->items)[index] = val;
            break;
        case 'l':
            ((int32_t*)o->items)[index] = val;
            break;
        case 'L':
            ((uint32_t*)o->items)[index] = val;
            break;
    }
}


static void array_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in, mp_print_kind_t kind) {
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
                print(env, "%d", array_get_el(o, i));
            }
            print(env, "]");
        }
    }
    print(env, ")");
}

static mp_obj_t array_construct(char typecode, mp_obj_t initializer) {
    uint len;
    // Try to create array of exact len if initializer len is known
    mp_obj_t len_in = mp_obj_len_maybe(initializer);
    if (len_in == MP_OBJ_NULL) {
        len = 0;
    } else {
        len = MP_OBJ_SMALL_INT_VALUE(len_in);
    }

    mp_obj_array_t *array = array_new(typecode, len);

    mp_obj_t iterable = rt_getiter(initializer);
    mp_obj_t item;
    int i = 0;
    while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
        if (len == 0) {
            array_append(array, item);
        } else {
            array_set_el(array, i++, item);
        }
    }

    return array;
}

static mp_obj_t array_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    if (n_args < 1 || n_args > 2) {
        nlr_jump(mp_obj_new_exception_msg_varg(MP_QSTR_TypeError, "unexpected # of arguments, %d given", n_args));
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
static mp_obj_t mp_builtin_bytearray(mp_obj_t arg) {
    return array_construct(BYTEARRAY_TYPECODE, arg);
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_builtin_bytearray_obj, mp_builtin_bytearray);

static mp_obj_t array_unary_op(int op, mp_obj_t o_in) {
    mp_obj_array_t *o = o_in;
    switch (op) {
        case RT_UNARY_OP_BOOL: return MP_BOOL(o->len != 0);
        case RT_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(o->len);
        default: return MP_OBJ_NULL; // op not supported
    }
}

static mp_obj_t array_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_array_t *o = lhs;
    switch (op) {
        case RT_BINARY_OP_SUBSCR:
        {
            uint index = mp_get_index(o->base.type, o->len, rhs);
            machine_int_t val = array_get_el(o, index);
            return mp_obj_new_int(val);
        }

        default:
            // op not supported
            return MP_OBJ_NULL;
    }
}

static mp_obj_t array_append(mp_obj_t self_in, mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(self_in, &array_type));
    mp_obj_array_t *self = self_in;
    if (self->free == 0) {
        int item_sz = array_get_el_size(self->typecode);
        // TODO: alloc policy
        self->free = 8;
        self->items = m_realloc(self->items,  item_sz * self->len, item_sz * (self->len + self->free));
    }
    array_set_el(self, self->len++, arg);
    self->free--;
    return mp_const_none; // return None, as per CPython
}
static MP_DEFINE_CONST_FUN_OBJ_2(array_append_obj, array_append);

static bool array_store_item(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    mp_obj_array_t *o = self_in;
    uint index = mp_get_index(o->base.type, o->len, index_in);
    array_set_el(o, index, value);
    return true;
}

static machine_int_t array_get_buffer(mp_obj_t o_in, buffer_info_t *bufinfo, int flags) {
    mp_obj_array_t *o = o_in;
    bufinfo->buf = o->items;
    bufinfo->len = o->len * array_get_el_size(o->typecode);
    return 0;
}

static const mp_method_t array_type_methods[] = {
    { "append", &array_append_obj },
    { NULL, NULL },
};

const mp_obj_type_t array_type = {
    { &mp_const_type },
    "array",
    .print = array_print,
    .make_new = array_make_new,
    .getiter = array_iterator_new,
    .unary_op = array_unary_op,
    .binary_op = array_binary_op,
    .store_item = array_store_item,
    .methods = array_type_methods,
    .buffer_p = { .get_buffer = array_get_buffer },
};

static mp_obj_array_t *array_new(char typecode, uint n) {
    mp_obj_array_t *o = m_new_obj(mp_obj_array_t);
    o->base.type = &array_type;
    o->typecode = typecode;
    o->free = 0;
    o->len = n;
    o->items = m_malloc(array_get_el_size(typecode) * o->len);
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
    o->base.type = &array_type;
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

mp_obj_t array_it_iternext(mp_obj_t self_in) {
    mp_obj_array_it_t *self = self_in;
    if (self->cur < self->array->len) {
        machine_int_t val = array_get_el(self->array, self->cur++);
        return mp_obj_new_int(val);
    } else {
        return mp_const_stop_iteration;
    }
}

static const mp_obj_type_t array_it_type = {
    { &mp_const_type },
    "array_iterator",
    .iternext = array_it_iternext,
};

mp_obj_t array_iterator_new(mp_obj_t array_in) {
    mp_obj_array_t *array = array_in;
    mp_obj_array_it_t *o = m_new_obj(mp_obj_array_it_t);
    o->base.type = &array_it_type;
    o->array = array;
    o->cur = 0;
    return o;
}
