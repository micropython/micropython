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
#include "shared-bindings/memorymonitor/AllocationAlarm.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class AllocationAlarm:
//|
//|     def __init__(self, *, minimum_block_count: int = 1) -> None:
//|         """Throw an exception when an allocation of ``minimum_block_count`` or more blocks
//|            occurs while active.
//|
//|         Track allocations::
//|
//|           import memorymonitor
//|
//|           aa = memorymonitor.AllocationAlarm(minimum_block_count=2)
//|           x = 2
//|           # Should not allocate any blocks.
//|           with aa:
//|               x = 5
//|
//|           # Should throw an exception when allocating storage for the 20 bytes.
//|           with aa:
//|               x = bytearray(20)
//|
//|         """
//|         ...
//|
STATIC mp_obj_t memorymonitor_allocationalarm_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_minimum_block_count };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_minimum_block_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_int_t minimum_block_count = args[ARG_minimum_block_count].u_int;
    if (minimum_block_count < 1) {
        mp_raise_ValueError_varg(translate("%q must be >= 1"), MP_QSTR_minimum_block_count);
    }

    memorymonitor_allocationalarm_obj_t *self = m_new_obj(memorymonitor_allocationalarm_obj_t);
    self->base.type = &memorymonitor_allocationalarm_type;

    common_hal_memorymonitor_allocationalarm_construct(self, minimum_block_count);

    return MP_OBJ_FROM_PTR(self);
}

//|     def ignore(self, count: int) -> AllocationAlarm:
//|         """Sets the number of applicable allocations to ignore before raising the exception.
//|            Automatically set back to zero at context exit.
//|
//|            Use it within a ``with`` block::
//|
//|              # Will not alarm because the bytearray allocation will be ignored.
//|              with aa.ignore(2):
//|                  x = bytearray(20)
//|            """
//|         ...
//|
STATIC mp_obj_t memorymonitor_allocationalarm_obj_ignore(mp_obj_t self_in, mp_obj_t count_obj) {
    mp_int_t count = mp_obj_get_int(count_obj);
    if (count < 0) {
        mp_raise_ValueError_varg(translate("%q must be >= 0"), MP_QSTR_count);
    }
    common_hal_memorymonitor_allocationalarm_set_ignore(self_in, count);
    return self_in;
}
MP_DEFINE_CONST_FUN_OBJ_2(memorymonitor_allocationalarm_ignore_obj, memorymonitor_allocationalarm_obj_ignore);

//|     def __enter__(self) -> AllocationAlarm:
//|         """Enables the alarm."""
//|         ...
//|
STATIC mp_obj_t memorymonitor_allocationalarm_obj___enter__(mp_obj_t self_in) {
    common_hal_memorymonitor_allocationalarm_resume(self_in);
    return self_in;
}
MP_DEFINE_CONST_FUN_OBJ_1(memorymonitor_allocationalarm___enter___obj, memorymonitor_allocationalarm_obj___enter__);

//|     def __exit__(self) -> None:
//|         """Automatically disables the allocation alarm when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t memorymonitor_allocationalarm_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_memorymonitor_allocationalarm_set_ignore(args[0], 0);
    common_hal_memorymonitor_allocationalarm_pause(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(memorymonitor_allocationalarm___exit___obj, 4, 4, memorymonitor_allocationalarm_obj___exit__);

STATIC const mp_rom_map_elem_t memorymonitor_allocationalarm_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_ignore), MP_ROM_PTR(&memorymonitor_allocationalarm_ignore_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&memorymonitor_allocationalarm___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&memorymonitor_allocationalarm___exit___obj) },
};
STATIC MP_DEFINE_CONST_DICT(memorymonitor_allocationalarm_locals_dict, memorymonitor_allocationalarm_locals_dict_table);

const mp_obj_type_t memorymonitor_allocationalarm_type = {
    { &mp_type_type },
    .name = MP_QSTR_AllocationAlarm,
    .make_new = memorymonitor_allocationalarm_make_new,
    .locals_dict = (mp_obj_dict_t *)&memorymonitor_allocationalarm_locals_dict,
};
