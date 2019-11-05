/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/objlist.h"
#include "py/runtime.h"

#if MICROPY_PY_UHEAPQ

// the algorithm here is modelled on CPython's heapq.py

STATIC mp_obj_list_t *uheapq_get_heap(mp_obj_t heap_in) {
    if (!mp_obj_is_type(heap_in, &mp_type_list)) {
        mp_raise_TypeError("heap must be a list");
    }
    return MP_OBJ_TO_PTR(heap_in);
}

STATIC void uheapq_heap_siftdown(mp_obj_list_t *heap, mp_uint_t start_pos, mp_uint_t pos) {
    mp_obj_t item = heap->items[pos];
    while (pos > start_pos) {
        mp_uint_t parent_pos = (pos - 1) >> 1;
        mp_obj_t parent = heap->items[parent_pos];
        if (mp_binary_op(MP_BINARY_OP_LESS, item, parent) == mp_const_true) {
            heap->items[pos] = parent;
            pos = parent_pos;
        } else {
            break;
        }
    }
    heap->items[pos] = item;
}

STATIC void uheapq_heap_siftup(mp_obj_list_t *heap, mp_uint_t pos) {
    mp_uint_t start_pos = pos;
    mp_uint_t end_pos = heap->len;
    mp_obj_t item = heap->items[pos];
    for (mp_uint_t child_pos = 2 * pos + 1; child_pos < end_pos; child_pos = 2 * pos + 1) {
        // choose right child if it's <= left child
        if (child_pos + 1 < end_pos && mp_binary_op(MP_BINARY_OP_LESS, heap->items[child_pos], heap->items[child_pos + 1]) == mp_const_false) {
            child_pos += 1;
        }
        // bubble up the smaller child
        heap->items[pos] = heap->items[child_pos];
        pos = child_pos;
    }
    heap->items[pos] = item;
    uheapq_heap_siftdown(heap, start_pos, pos);
}

STATIC mp_obj_t mod_uheapq_heappush(mp_obj_t heap_in, mp_obj_t item) {
    mp_obj_list_t *heap = uheapq_get_heap(heap_in);
    mp_obj_list_append(heap_in, item);
    uheapq_heap_siftdown(heap, 0, heap->len - 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_uheapq_heappush_obj, mod_uheapq_heappush);

STATIC mp_obj_t mod_uheapq_heappop(mp_obj_t heap_in) {
    mp_obj_list_t *heap = uheapq_get_heap(heap_in);
    if (heap->len == 0) {
        mp_raise_msg(&mp_type_IndexError, "empty heap");
    }
    mp_obj_t item = heap->items[0];
    heap->len -= 1;
    heap->items[0] = heap->items[heap->len];
    heap->items[heap->len] = MP_OBJ_NULL; // so we don't retain a pointer
    if (heap->len) {
        uheapq_heap_siftup(heap, 0);
    }
    return item;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_uheapq_heappop_obj, mod_uheapq_heappop);

STATIC mp_obj_t mod_uheapq_heapify(mp_obj_t heap_in) {
    mp_obj_list_t *heap = uheapq_get_heap(heap_in);
    for (mp_uint_t i = heap->len / 2; i > 0;) {
        uheapq_heap_siftup(heap, --i);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_uheapq_heapify_obj, mod_uheapq_heapify);

#if !MICROPY_ENABLE_DYNRUNTIME
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
#endif

#endif //MICROPY_PY_UHEAPQ
