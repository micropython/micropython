/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 microDev
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

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/coproc/Coproc.h"
#include "shared-bindings/coproc/CoprocMemory.h"

STATIC coproc_memory_obj_t *get_coproc_memory_obj(mp_obj_t *self_in) {
    if (!mp_obj_is_type(*self_in, &coproc_memory_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), MP_QSTR_CoprocMemory);
    }
    return MP_OBJ_TO_PTR(*self_in);
}

//| class Coproc:
//|     def __init__(self, buffer: ReadableBuffer, memory: CoprocMemory) -> None:
//|         """Loads the program binary into memory.
//|
//|         :param buffer: The program binary to run on the core/co-processor
//|         :param memory: The `CoprocMemory` object used to access shared memory"""
STATIC mp_obj_t coproc_coproc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_buffer, ARG_memory };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_memory, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    coproc_coproc_obj_t *self = m_new_obj(coproc_coproc_obj_t);
    self->base.type = &coproc_coproc_type;

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    coproc_memory_obj_t *coproc_memory = (args[ARG_memory].u_obj == mp_const_none) ? NULL : get_coproc_memory_obj(&args[ARG_memory].u_obj);

    common_hal_coproc_coproc_construct(self, bufinfo.buf, bufinfo.len, coproc_memory);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Releases control of the underlying hardware so other classes can use it."""
//|         ...
STATIC mp_obj_t coproc_coproc_obj_deinit(mp_obj_t self_in) {
    coproc_coproc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_coproc_coproc_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(coproc_coproc_deinit_obj, coproc_coproc_obj_deinit);

//|     def __enter__(self) -> Coproc:
//|         """No-op used in Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Close the request."""
//|         ...
//|
STATIC mp_obj_t coproc_coproc_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_check_self(mp_obj_is_type(args[0], &coproc_coproc_type));
    return coproc_coproc_obj_deinit(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(coproc_coproc___exit___obj, 4, 4, coproc_coproc_obj___exit__);

STATIC const mp_rom_map_elem_t coproc_coproc_locals_dict_table[] = {
    // context managers
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&coproc_coproc___exit___obj) },

    // functions
    { MP_ROM_QSTR(MP_QSTR_deinit),  MP_ROM_PTR(&coproc_coproc_deinit_obj) },
};
STATIC MP_DEFINE_CONST_DICT(coproc_coproc_locals_dict, coproc_coproc_locals_dict_table);

const mp_obj_type_t coproc_coproc_type = {
    { &mp_type_type },
    .name = MP_QSTR_Coproc,
    .make_new = coproc_coproc_make_new,
    .locals_dict = (mp_obj_dict_t *)&coproc_coproc_locals_dict,
};
