/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 * Copyright (c) 2016-2017 Paul Sokolovsky
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

#include <string.h>

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/smallint.h"

#if MICROPY_PY_UTIMEQ

#define MODULO MICROPY_PY_UTIME_TICKS_PERIOD

#define DEBUG 0

// the algorithm here is modelled on CPython's heapq.py

struct qentry {
    mp_uint_t time;
    mp_uint_t id;
    mp_obj_t callback;
    mp_obj_t args;
};

typedef struct _mp_obj_utimeq_t {
    mp_obj_base_t base;
    mp_uint_t alloc;
    mp_uint_t len;
    struct qentry items[];
} mp_obj_utimeq_t;

STATIC mp_uint_t utimeq_id;

STATIC mp_obj_utimeq_t *get_heap(mp_obj_t heap_in) {
    return MP_OBJ_TO_PTR(heap_in);
}

STATIC bool time_less_than(struct qentry *item, struct qentry *parent) {
    mp_uint_t item_tm = item->time;
    mp_uint_t parent_tm = parent->time;
    mp_uint_t res = parent_tm - item_tm;
    if (res == 0) {
        // TODO: This actually should use the same "ring" logic
        // as for time, to avoid artifacts when id's overflow.
        return item->id < parent->id;
    }
    if ((mp_int_t)res < 0) {
        res += MODULO;
    }
    return res && res < (MODULO / 2);
}

STATIC mp_obj_t utimeq_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_uint_t alloc = mp_obj_get_int(args[0]);
    mp_obj_utimeq_t *o = m_new_obj_var(mp_obj_utimeq_t, struct qentry, alloc);
    o->base.type = type;
    memset(o->items, 0, sizeof(*o->items) * alloc);
    o->alloc = alloc;
    o->len = 0;
    return MP_OBJ_FROM_PTR(o);
}

STATIC void heap_siftdown(mp_obj_utimeq_t *heap, mp_uint_t start_pos, mp_uint_t pos) {
    struct qentry item = heap->items[pos];
    while (pos > start_pos) {
        mp_uint_t parent_pos = (pos - 1) >> 1;
        struct qentry *parent = &heap->items[parent_pos];
        bool lessthan = time_less_than(&item, parent);
        if (lessthan) {
            heap->items[pos] = *parent;
            pos = parent_pos;
        } else {
            break;
        }
    }
    heap->items[pos] = item;
}

STATIC void heap_siftup(mp_obj_utimeq_t *heap, mp_uint_t pos) {
    mp_uint_t start_pos = pos;
    mp_uint_t end_pos = heap->len;
    struct qentry item = heap->items[pos];
    for (mp_uint_t child_pos = 2 * pos + 1; child_pos < end_pos; child_pos = 2 * pos + 1) {
        // choose right child if it's <= left child
        if (child_pos + 1 < end_pos) {
            bool lessthan = time_less_than(&heap->items[child_pos], &heap->items[child_pos + 1]);
            if (!lessthan) {
                child_pos += 1;
            }
        }
        // bubble up the smaller child
        heap->items[pos] = heap->items[child_pos];
        pos = child_pos;
    }
    heap->items[pos] = item;
    heap_siftdown(heap, start_pos, pos);
}

STATIC mp_obj_t mod_utimeq_heappush(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_obj_t heap_in = args[0];
    mp_obj_utimeq_t *heap = get_heap(heap_in);
    if (heap->len == heap->alloc) {
        mp_raise_msg(&mp_type_IndexError, "queue overflow");
    }
    mp_uint_t l = heap->len;
    heap->items[l].time = MP_OBJ_SMALL_INT_VALUE(args[1]);
    heap->items[l].id = utimeq_id++;
    heap->items[l].callback = args[2];
    heap->items[l].args = args[3];
    heap_siftdown(heap, 0, heap->len);
    heap->len++;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_utimeq_heappush_obj, 4, 4, mod_utimeq_heappush);

STATIC mp_obj_t mod_utimeq_heappop(mp_obj_t heap_in, mp_obj_t list_ref) {
    mp_obj_utimeq_t *heap = get_heap(heap_in);
    if (heap->len == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_IndexError, "empty heap"));
    }
    mp_obj_list_t *ret = MP_OBJ_TO_PTR(list_ref);
    if (!MP_OBJ_IS_TYPE(list_ref, &mp_type_list) || ret->len < 3) {
        mp_raise_TypeError("");
    }

    struct qentry *item = &heap->items[0];
    ret->items[0] = MP_OBJ_NEW_SMALL_INT(item->time);
    ret->items[1] = item->callback;
    ret->items[2] = item->args;
    heap->len -= 1;
    heap->items[0] = heap->items[heap->len];
    heap->items[heap->len].callback = MP_OBJ_NULL; // so we don't retain a pointer
    heap->items[heap->len].args = MP_OBJ_NULL;
    if (heap->len) {
        heap_siftup(heap, 0);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_utimeq_heappop_obj, mod_utimeq_heappop);

STATIC mp_obj_t mod_utimeq_peektime(mp_obj_t heap_in) {
    mp_obj_utimeq_t *heap = get_heap(heap_in);
    if (heap->len == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_IndexError, "empty heap"));
    }

    struct qentry *item = &heap->items[0];
    return MP_OBJ_NEW_SMALL_INT(item->time);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_utimeq_peektime_obj, mod_utimeq_peektime);

#if DEBUG
STATIC mp_obj_t mod_utimeq_dump(mp_obj_t heap_in) {
    mp_obj_utimeq_t *heap = get_heap(heap_in);
    for (int i = 0; i < heap->len; i++) {
        printf(UINT_FMT "\t%p\t%p\n", heap->items[i].time,
            MP_OBJ_TO_PTR(heap->items[i].callback), MP_OBJ_TO_PTR(heap->items[i].args));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_utimeq_dump_obj, mod_utimeq_dump);
#endif

STATIC mp_obj_t utimeq_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    mp_obj_utimeq_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL: return mp_obj_new_bool(self->len != 0);
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->len);
        default: return MP_OBJ_NULL; // op not supported
    }
}

STATIC const mp_rom_map_elem_t utimeq_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_push), MP_ROM_PTR(&mod_utimeq_heappush_obj) },
    { MP_ROM_QSTR(MP_QSTR_pop), MP_ROM_PTR(&mod_utimeq_heappop_obj) },
    { MP_ROM_QSTR(MP_QSTR_peektime), MP_ROM_PTR(&mod_utimeq_peektime_obj) },
    #if DEBUG
    { MP_ROM_QSTR(MP_QSTR_dump), MP_ROM_PTR(&mod_utimeq_dump_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(utimeq_locals_dict, utimeq_locals_dict_table);

STATIC const mp_obj_type_t utimeq_type = {
    { &mp_type_type },
    .name = MP_QSTR_utimeq,
    .make_new = utimeq_make_new,
    .unary_op = utimeq_unary_op,
    .locals_dict = (void*)&utimeq_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module_utimeq_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_utimeq) },
    { MP_ROM_QSTR(MP_QSTR_utimeq), MP_ROM_PTR(&utimeq_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_utimeq_globals, mp_module_utimeq_globals_table);

const mp_obj_module_t mp_module_utimeq = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_utimeq_globals,
};

#endif //MICROPY_PY_UTIMEQ
