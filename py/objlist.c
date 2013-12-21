#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "obj.h"
#include "runtime0.h"
#include "runtime.h"

typedef struct _mp_obj_list_t {
    mp_obj_base_t base;
    machine_uint_t alloc;
    machine_uint_t len;
    mp_obj_t *items;
} mp_obj_list_t;

static mp_obj_t mp_obj_new_list_iterator(mp_obj_list_t *list, int cur);

/******************************************************************************/
/* list                                                                       */

static void list_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t o_in) {
    mp_obj_list_t *o = o_in;
    print(env, "[");
    for (int i = 0; i < o->len; i++) {
        if (i > 0) {
            print(env, ", ");
        }
        mp_obj_print_helper(print, env, o->items[i]);
    }
    print(env, "]");
}

static mp_obj_t list_binary_op(int op, mp_obj_t lhs, mp_obj_t rhs) {
    mp_obj_list_t *o = lhs;
    switch (op) {
        case RT_BINARY_OP_SUBSCR:
        {
            // list load
            uint index = mp_get_index(o->base.type, o->len, rhs);
            return o->items[index];
        }
        default:
            // op not supported
            return NULL;
    }
}

static mp_obj_t list_getiter(mp_obj_t o_in) {
    return mp_obj_new_list_iterator(o_in, 0);
}

mp_obj_t mp_obj_list_append(mp_obj_t self_in, mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_list_t *self = self_in;
    if (self->len >= self->alloc) {
        self->alloc *= 2;
        self->items = m_renew(mp_obj_t, self->items, self->alloc);
    }
    self->items[self->len++] = arg;
    return mp_const_none; // return None, as per CPython
}

static mp_obj_t list_pop(mp_obj_t self_in, mp_obj_t arg) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_list_t *self = self_in;
    uint index = mp_get_index(self->base.type, self->len, arg);
    mp_obj_t ret = self->items[index];
    self->len -= 1;
    memcpy(self->items + index, self->items + index + 1, (self->len - index) * sizeof(mp_obj_t));
    return ret;
}

// TODO make this conform to CPython's definition of sort
static void mp_quicksort(mp_obj_t *head, mp_obj_t *tail, mp_obj_t key_fn) {
    while (head < tail) {
        mp_obj_t *h = head - 1;
        mp_obj_t *t = tail;
        mp_obj_t v = rt_call_function_1(key_fn, tail[0]); // get pivot using key_fn
        for (;;) {
            do ++h; while (rt_compare_op(RT_COMPARE_OP_LESS, rt_call_function_1(key_fn, h[0]), v) == mp_const_true);
            do --t; while (h < t && rt_compare_op(RT_COMPARE_OP_LESS, v, rt_call_function_1(key_fn, t[0])) == mp_const_true);
            if (h >= t) break;
            mp_obj_t x = h[0];
            h[0] = t[0];
            t[0] = x;
        }
        mp_obj_t x = h[0];
        h[0] = tail[0];
        tail[0] = x;
        mp_quicksort(head, t, key_fn);
        head = h + 1;
    }
}

static mp_obj_t list_sort(mp_obj_t self_in, mp_obj_t key_fn) {
    assert(MP_OBJ_IS_TYPE(self_in, &list_type));
    mp_obj_list_t *self = self_in;
    if (self->len > 1) {
        mp_quicksort(self->items, self->items + self->len - 1, key_fn);
    }
    return mp_const_none; // return None, as per CPython
}

static MP_DEFINE_CONST_FUN_OBJ_2(list_append_obj, mp_obj_list_append);
static MP_DEFINE_CONST_FUN_OBJ_2(list_pop_obj, list_pop);
static MP_DEFINE_CONST_FUN_OBJ_2(list_sort_obj, list_sort);

const mp_obj_type_t list_type = {
    { &mp_const_type },
    "list",
    list_print, // print
    NULL, // call_n
    NULL, // unary_op
    list_binary_op, // binary_op
    list_getiter, // getiter
    NULL, // iternext
    { // method list
        { "append", &list_append_obj },
        { "pop", &list_pop_obj },
        { "sort", &list_sort_obj },
        { NULL, NULL }, // end-of-list sentinel
    },
};

static mp_obj_list_t *list_new(uint n) {
    mp_obj_list_t *o = m_new_obj(mp_obj_list_t);
    o->base.type = &list_type;
    o->alloc = n < 4 ? 4 : n;
    o->len = n;
    o->items = m_new(mp_obj_t, o->alloc);
    return o;
}

mp_obj_t mp_obj_new_list(uint n, mp_obj_t *items) {
    mp_obj_list_t *o = list_new(n);
    for (int i = 0; i < n; i++) {
        o->items[i] = items[i];
    }
    return o;
}

mp_obj_t mp_obj_new_list_reverse(uint n, mp_obj_t *items) {
    mp_obj_list_t *o = list_new(n);
    for (int i = 0; i < n; i++) {
        o->items[i] = items[n - i - 1];
    }
    return o;
}

void mp_obj_list_get(mp_obj_t self_in, uint *len, mp_obj_t **items) {
    mp_obj_list_t *self = self_in;
    *len = self->len;
    *items = self->items;
}

void mp_obj_list_store(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_list_t *self = self_in;
    uint i = mp_get_index(self->base.type, self->len, index);
    self->items[i] = value;
}

/******************************************************************************/
/* list iterator                                                              */

typedef struct _mp_obj_list_it_t {
    mp_obj_base_t base;
    mp_obj_list_t *list;
    machine_uint_t cur;
} mp_obj_list_it_t;

mp_obj_t list_it_iternext(mp_obj_t self_in) {
    mp_obj_list_it_t *self = self_in;
    if (self->cur < self->list->len) {
        mp_obj_t o_out = self->list->items[self->cur];
        self->cur += 1;
        return o_out;
    } else {
        return mp_const_stop_iteration;
    }
}

static const mp_obj_type_t list_it_type = {
    { &mp_const_type },
    "list_iterator",
    NULL, // print
    NULL, // call_n
    NULL, // unary_op
    NULL, // binary_op
    NULL, // getiter
    list_it_iternext, // iternext
    { { NULL, NULL }, }, // method list
};

mp_obj_t mp_obj_new_list_iterator(mp_obj_list_t *list, int cur) {
    mp_obj_list_it_t *o = m_new_obj(mp_obj_list_it_t);
    o->base.type = &list_it_type;
    o->list = list;
    o->cur = cur;
    return o;
}
