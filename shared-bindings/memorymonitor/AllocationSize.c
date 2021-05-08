/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/runtime0.h"
#include "shared-bindings/memorymonitor/AllocationSize.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class AllocationSize:
//|
//|     def __init__(self) -> None:
//|         """Tracks the number of allocations in power of two buckets.
//|
//|         It will have 16 16-bit buckets to track allocation counts. It is total allocations
//|         meaning frees are ignored. Reallocated memory is counted twice, at allocation and when
//|         reallocated with the larger size.
//|
//|         The buckets are measured in terms of blocks which is the finest granularity of the heap.
//|         This means bucket 0 will count all allocations less than or equal to the number of bytes
//|         per block, typically 16. Bucket 2 will be less than or equal to 4 blocks. See
//|         `bytes_per_block` to convert blocks to bytes.
//|
//|         Multiple AllocationSizes can be used to track different code boundaries.
//|
//|         Track allocations::
//|
//|           import memorymonitor
//|
//|           mm = memorymonitor.AllocationSize()
//|           with mm:
//|             print("hello world" * 3)
//|
//|           for bucket, count in enumerate(mm):
//|               print("<", 2 ** bucket, count)
//|
//|         """
//|         ...
//|
STATIC mp_obj_t memorymonitor_allocationsize_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    memorymonitor_allocationsize_obj_t *self = m_new_obj(memorymonitor_allocationsize_obj_t);
    self->base.type = &memorymonitor_allocationsize_type;

    common_hal_memorymonitor_allocationsize_construct(self);

    return MP_OBJ_FROM_PTR(self);
}

//|     def __enter__(self) -> AllocationSize:
//|         """Clears counts and resumes tracking."""
//|         ...
//|
STATIC mp_obj_t memorymonitor_allocationsize_obj___enter__(mp_obj_t self_in) {
    common_hal_memorymonitor_allocationsize_clear(self_in);
    common_hal_memorymonitor_allocationsize_resume(self_in);
    return self_in;
}
MP_DEFINE_CONST_FUN_OBJ_1(memorymonitor_allocationsize___enter___obj, memorymonitor_allocationsize_obj___enter__);

//|     def __exit__(self) -> None:
//|         """Automatically pauses allocation tracking when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t memorymonitor_allocationsize_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_memorymonitor_allocationsize_pause(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(memorymonitor_allocationsize___exit___obj, 4, 4, memorymonitor_allocationsize_obj___exit__);

//|     bytes_per_block: int
//|     """Number of bytes per block"""
//|
STATIC mp_obj_t memorymonitor_allocationsize_obj_get_bytes_per_block(mp_obj_t self_in) {
    memorymonitor_allocationsize_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return MP_OBJ_NEW_SMALL_INT(common_hal_memorymonitor_allocationsize_get_bytes_per_block(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(memorymonitor_allocationsize_get_bytes_per_block_obj, memorymonitor_allocationsize_obj_get_bytes_per_block);

const mp_obj_property_t memorymonitor_allocationsize_bytes_per_block_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&memorymonitor_allocationsize_get_bytes_per_block_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     def __len__(self) -> int:
//|         """Returns the number of allocation buckets.
//|
//|         This allows you to::
//|
//|           mm = memorymonitor.AllocationSize()
//|           print(len(mm))"""
//|         ...
//|
STATIC mp_obj_t memorymonitor_allocationsize_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    memorymonitor_allocationsize_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint16_t len = common_hal_memorymonitor_allocationsize_get_len(self);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(len != 0);
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(len);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

//|     def __getitem__(self, index: int) -> Optional[int]:
//|         """Returns the allocation count for the given bucket.
//|
//|         This allows you to::
//|
//|           mm = memorymonitor.AllocationSize()
//|           print(mm[0])"""
//|         ...
//|
STATIC mp_obj_t memorymonitor_allocationsize_subscr(mp_obj_t self_in, mp_obj_t index_obj, mp_obj_t value) {
    if (value == mp_const_none) {
        // delete item
        mp_raise_AttributeError(translate("Cannot delete values"));
    } else {
        memorymonitor_allocationsize_obj_t *self = MP_OBJ_TO_PTR(self_in);

        if (mp_obj_is_type(index_obj, &mp_type_slice)) {
            mp_raise_NotImplementedError(translate("Slices not supported"));
        } else {
            size_t index = mp_get_index(&memorymonitor_allocationsize_type, common_hal_memorymonitor_allocationsize_get_len(self), index_obj, false);
            if (value == MP_OBJ_SENTINEL) {
                // load
                return MP_OBJ_NEW_SMALL_INT(common_hal_memorymonitor_allocationsize_get_item(self, index));
            } else {
                mp_raise_AttributeError(translate("Read-only"));
            }
        }
    }
    return mp_const_none;
}

STATIC const mp_rom_map_elem_t memorymonitor_allocationsize_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&memorymonitor_allocationsize___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&memorymonitor_allocationsize___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_bytes_per_block), MP_ROM_PTR(&memorymonitor_allocationsize_bytes_per_block_obj) },
};
STATIC MP_DEFINE_CONST_DICT(memorymonitor_allocationsize_locals_dict, memorymonitor_allocationsize_locals_dict_table);

const mp_obj_type_t memorymonitor_allocationsize_type = {
    { &mp_type_type },
    .name = MP_QSTR_AllocationSize,
    .make_new = memorymonitor_allocationsize_make_new,
    .subscr = memorymonitor_allocationsize_subscr,
    .unary_op = memorymonitor_allocationsize_unary_op,
    .getiter = mp_obj_new_generic_iterator,
    .locals_dict = (mp_obj_dict_t *)&memorymonitor_allocationsize_locals_dict,
};
