/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 NED KONZ <NED@METAMAGIX.TECH>
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

#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_PY_MACHINE_PDM

#include "extmod/modmachine.h"

// The PDM class provides access to PDM (Pulse Density Modulation) digital microphones.
// Currently supports blocking mode only:
// - readinto() blocks until the supplied buffer is filled
// - this is the default and only mode of operation
//
// Non-blocking and asyncio modes are planned for future implementation.

// The port must provide implementations of these low-level PDM functions.
static void mp_machine_pdm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
static mp_obj_t mp_machine_pdm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static void mp_machine_pdm_init_helper(machine_pdm_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static void mp_machine_pdm_deinit(machine_pdm_obj_t *self);
static mp_uint_t mp_machine_pdm_stream_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode);

// The port provides implementations of the above in this file.
#include MICROPY_PY_MACHINE_PDM_INCLUDEFILE

// PDM.init(...)
static mp_obj_t machine_pdm_init(size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_pdm_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_machine_pdm_deinit(self);
    mp_machine_pdm_init_helper(self, n_pos_args - 1, pos_args + 1, kw_args);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_pdm_init_obj, 1, machine_pdm_init);

// PDM.deinit()
static mp_obj_t machine_pdm_deinit(mp_obj_t self_in) {
    machine_pdm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_pdm_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pdm_deinit_obj, machine_pdm_deinit);

static const mp_rom_map_elem_t machine_pdm_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_pdm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_pdm_deinit_obj) },

    // Constants - will be defined by the port-specific implementation
    MICROPY_PY_MACHINE_PDM_CLASS_CONSTANTS
};
static MP_DEFINE_CONST_DICT(machine_pdm_locals_dict, machine_pdm_locals_dict_table);

static const mp_stream_p_t pdm_stream_p = {
    .read = mp_machine_pdm_stream_read,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pdm_type,
    MP_QSTR_PDM,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, mp_machine_pdm_make_new,
    print, mp_machine_pdm_print,
    protocol, &pdm_stream_p,
    locals_dict, &machine_pdm_locals_dict
    );

#endif // MICROPY_PY_MACHINE_PDM
