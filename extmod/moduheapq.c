/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime0.h"
#include "py/runtime.h"
#include "py/smallint.h"

#if MICROPY_PY_UHEAPQ

#define MODULO MICROPY_PY_UTIME_TICKS_PERIOD

// the algorithm here is modelled on CPython's heapq.py

STATIC mp_obj_list_t *get_heap(mp_obj_t heap_in) {
    if (!MP_OBJ_IS_TYPE(heap_in, &mp_type_list)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "heap must be a list"));
    }
    return MP_OBJ_TO_PTR(heap_in);
}

STATIC bool time_less_than(mp_obj_t item, mp_obj_t parent) {
    if (!MP_OBJ_IS_TYPE(item, &mp_type_tuple) || !MP_OBJ_IS_TYPE(parent, &mp_type_tuple)) {
        mp_raise_TypeError("");
    }
    mp_obj_tuple_t *item_p = MP_OBJ_TO_PTR(item);
    mp_obj_tuple_t *parent_p = MP_OBJ_TO_PTR(parent);
    mp_uint_t item_tm = MP_OBJ_SMALL_INT_VALUE(item_p->items[0]);
    mp_uint_t parent_tm = MP_OBJ_SMALL_INT_VALUE(parent_p->items[0]);
    mp_uint_t res = parent_tm - item_tm;
    if ((mp_int_t)res < 0) {
        res += MODULO;
    }
    return res < (MODULO / 2);
}

STATIC void heap_siftdown(mp_obj_list_t *heap, mp_uint_t start_pos, mp_uint_t pos, bool timecmp) {
    mp_obj_t item = heap->items[pos];
    while (pos > start_pos) {
        mp_uint_t parent_pos = (pos - 1) >> 1;
        mp_obj_t parent = heap->items[parent_pos];
        bool lessthan;
        if (MP_UNLIKELY(timecmp)) {
            lessthan = time_less_than(item, parent);
        } else {
            lessthan = (mp_binary_op(MP_BINARY_OP_LESS, item, parent) == mp_const_true);
        }
        if (lessthan) {
            heap->items[pos] = parent;
            pos = parent_pos;
        } else {
            break;
        }
    }
    heap->items[pos] = item;
}

STATIC void heap_siftup(mp_obj_list_t *heap, mp_uint_t pos, bool timecmp) {
    mp_uint_t start_pos = pos;
    mp_uint_t end_pos = heap->len;
    mp_obj_t item = heap->items[pos];
    for (mp_uint_t child_pos = 2 * pos + 1; child_pos < end_pos; child_pos = 2 * pos + 1) {
        // choose right child if it's <= left child
        if (child_pos + 1 < end_pos) {
            bool lessthan;
            if (MP_UNLIKELY(timecmp)) {
                lessthan = time_less_than(heap->items[child_pos], heap->items[child_pos + 1]);
            } else {
                lessthan = (mp_binary_op(MP_BINARY_OP_LESS, heap->items[child_pos], heap->items[child_pos + 1]) == mp_const_true);
            }
            if (!lessthan) {
                child_pos += 1;
            }
        }
        // bubble up the smaller child
        heap->items[pos] = heap->items[child_pos];
        pos = child_pos;
    }
    heap->items[pos] = item;
    heap_siftdown(heap, start_pos, pos, timecmp);
}

STATIC mp_obj_t mod_uheapq_heappush(size_t n_args, const mp_obj_t *args) {
    mp_obj_t heap_in = args[0];
    mp_obj_list_t *heap = get_heap(heap_in);
    mp_obj_list_append(heap_in, args[1]);
    bool is_timeq = (n_args > 2 && args[2] == mp_const_true);
    heap_siftdown(heap, 0, heap->len - 1, is_timeq);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_uheapq_heappush_obj, 2, 3, mod_uheapq_heappush);

STATIC mp_obj_t mod_uheapq_heappop(size_t n_args, const mp_obj_t *args) {
    mp_obj_t heap_in = args[0];
    mp_obj_list_t *heap = get_heap(heap_in);
    if (heap->len == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_IndexError, "empty heap"));
    }
    mp_obj_t item = heap->items[0];
    heap->len -= 1;
    heap->items[0] = heap->items[heap->len];
    heap->items[heap->len] = MP_OBJ_NULL; // so we don't retain a pointer
    if (heap->len) {
        bool is_timeq = (n_args > 1 && args[1] == mp_const_true);
        heap_siftup(heap, 0, is_timeq);
    }
    return item;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_uheapq_heappop_obj, 1, 2, mod_uheapq_heappop);

STATIC mp_obj_t mod_uheapq_heapify(mp_obj_t heap_in) {
    mp_obj_list_t *heap = get_heap(heap_in);
    for (mp_uint_t i = heap->len / 2; i > 0;) {
        heap_siftup(heap, --i, false);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_uheapq_heapify_obj, mod_uheapq_heapify);

STATIC const mp_rom_map_elem_t mp_module_uheapq_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uheapq) },
    { MP_ROM_QSTR(MP_QSTR_heappush), MP_ROM_PTR(&mod_uheapq_heappush_obj) },
    { MP_ROM_QSTR(MP_QSTR_heappop), MP_ROM_PTR(&mod_uheapq_heappop_obj) },
    { MP_ROM_QSTR(MP_QSTR_heapify), MP_ROM_PTR(&mod_uheapq_heapify_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_uheapq_globals, mp_module_uheapq_globals_table);

const mp_obj_module_t mp_module_uheapq = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_uheapq_globals,
};

#endif //MICROPY_PY_UHEAPQ
