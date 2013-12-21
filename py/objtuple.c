#include <stdlib.h>
#include <stdint.h>
//#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime0.h"

typedef struct _mp_obj_tuple_t {
    mp_obj_base_t base;
    machine_uint_t len;
    mp_obj_t items[];
} mp_obj_tuple_t;

static mp_obj_t mp_obj_new_tuple_iterator(mp_obj_tuple_t *tuple, int cur);

/******************************************************************************/
/* tuple                                                                      */

void tuple_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in) {
    mp_obj_tuple_t *o = o_in;
    print(env, "(");
    for (int i = 0; i < o->len; i++) {
        if (i > 0) {
            print(env, ", ");
        }
        mp_obj_print_helper(print, env, o->items[i]);
    }
    if (o->len == 1) {
        print(env, ",");
    }
    print(env, ")");
}

mp_obj_t tuple_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_tuple_t *o = lhs;
    switch (op) {
        case RT_BINARY_OP_SUBSCR:
        {
            // tuple load
            uint index = mp_get_index(o->base.type, o->len, rhs);
            return o->items[index];
        }
        default:
            // op not supported
            return NULL;
    }
}

mp_obj_t tuple_getiter(mp_obj_t o_in) {
    return mp_obj_new_tuple_iterator(o_in, 0);
}

void mp_obj_tuple_get(mp_obj_t self_in, uint *len, mp_obj_t **items) {
    mp_obj_tuple_t *self = self_in;
    *len = self->len;
    *items = &self->items[0];
}

const mp_obj_type_t tuple_type = {
    { &mp_const_type },
    "tuple",
    tuple_print, // print
    NULL, // call_n
    NULL, // unary_op
    tuple_binary_op, // binary_op
    tuple_getiter, // getiter
    NULL, // iternext
    {{NULL, NULL},}, // method list
};

mp_obj_t mp_obj_new_tuple(uint n, mp_obj_t *items) {
    mp_obj_tuple_t *o = m_new_obj_var(mp_obj_tuple_t, mp_obj_t, n);
    o->base.type = &tuple_type;
    o->len = n;
    for (int i = 0; i < n; i++) {
        o->items[i] = items[i];
    }
    return o;
}

mp_obj_t mp_obj_new_tuple_reverse(uint n, mp_obj_t *items) {
    mp_obj_tuple_t *o = m_new_obj_var(mp_obj_tuple_t, mp_obj_t, n);
    o->base.type = &tuple_type;
    o->len = n;
    for (int i = 0; i < n; i++) {
        o->items[i] = items[n - i - 1];
    }
    return o;
}

/******************************************************************************/
/* tuple iterator                                                             */

typedef struct _mp_obj_tuple_it_t {
    mp_obj_base_t base;
    mp_obj_tuple_t *tuple;
    machine_uint_t cur;
} mp_obj_tuple_it_t;

mp_obj_t tuple_it_iternext(mp_obj_t self_in) {
    mp_obj_tuple_it_t *self = self_in;
    if (self->cur < self->tuple->len) {
        mp_obj_t o_out = self->tuple->items[self->cur];
        self->cur += 1;
        return o_out;
    } else {
        return mp_const_stop_iteration;
    }
}

static const mp_obj_type_t tuple_it_type = {
    { &mp_const_type },
    "tuple_iterator",
    NULL, // print
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    tuple_it_iternext,
    {{NULL, NULL},}, // method list
};

static mp_obj_t mp_obj_new_tuple_iterator(mp_obj_tuple_t *tuple, int cur) {
    mp_obj_tuple_it_t *o = m_new_obj(mp_obj_tuple_it_t);
    o->base.type = &tuple_it_type;
    o->tuple = tuple;
    o->cur = cur;
    return o;
}
