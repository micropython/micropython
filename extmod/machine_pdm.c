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
// Supports three operation modes:
//
// Mode 1: Blocking
// - readinto() blocks until the supplied buffer is filled
// - this is the default mode of operation
//
// Mode 2: Non-Blocking
// - readinto() returns immediately
// - buffer filling happens asynchronously
// - a callback function is called when the supplied buffer has been filled
// - non-blocking mode is enabled when a callback is set with the irq() method
//
// Mode 3: Asyncio
// - implements the stream protocol
// - asyncio mode is enabled when the ioctl() function is called
// - the state of the internal ring buffer is used to detect that PDM samples can be read

// The port must provide implementations of these low-level PDM functions.
static void mp_machine_pdm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
static mp_obj_t mp_machine_pdm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static void mp_machine_pdm_init_helper(machine_pdm_obj_t *self, size_t n_pos_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static void mp_machine_pdm_deinit(machine_pdm_obj_t *self);
static mp_uint_t mp_machine_pdm_stream_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode);
static mp_uint_t mp_machine_pdm_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode);
static void mp_machine_pdm_irq_update(machine_pdm_obj_t *self);

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

// PDM.irq(handler)
static mp_obj_t machine_pdm_irq(mp_obj_t self_in, mp_obj_t handler) {
    machine_pdm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (handler != mp_const_none && !mp_obj_is_callable(handler)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid callback"));
    }

    self->callback_for_non_blocking = handler;
    mp_machine_pdm_irq_update(self);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_pdm_irq_obj, machine_pdm_irq);

// PDM.read(nbytes)
// Wrapper around stream protocol for asyncio compatibility
static mp_obj_t machine_pdm_read(size_t n_args, const mp_obj_t *args) {
    machine_pdm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t n = -1;
    if (n_args > 1) {
        n = mp_obj_get_int(args[1]);
    }

    if (n == -1) {
        // Read until EOF - not supported for PDM
        mp_raise_ValueError(MP_ERROR_TEXT("unbounded read not supported"));
    }

    // Allocate buffer and read into it
    vstr_t vstr;
    vstr_init_len(&vstr, n);

    int errcode;
    mp_uint_t ret = mp_machine_pdm_stream_read(MP_OBJ_FROM_PTR(self), vstr.buf, n, &errcode);

    if (ret == MP_STREAM_ERROR) {
        vstr_clear(&vstr);
        mp_raise_OSError(errcode);
    }

    if (ret == 0) {
        // No data available - return None for asyncio compatibility
        vstr_clear(&vstr);
        return mp_const_none;
    }

    vstr.len = ret;
    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pdm_read_obj, 1, 2, machine_pdm_read);

static const mp_rom_map_elem_t machine_pdm_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_init),            MP_ROM_PTR(&machine_pdm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),            MP_ROM_PTR(&machine_pdm_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),        MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),          MP_ROM_PTR(&machine_pdm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_irq),             MP_ROM_PTR(&machine_pdm_irq_obj) },

    // Constants - will be defined by the port-specific implementation
    MICROPY_PY_MACHINE_PDM_CLASS_CONSTANTS
};
static MP_DEFINE_CONST_DICT(machine_pdm_locals_dict, machine_pdm_locals_dict_table);

static const mp_stream_p_t pdm_stream_p = {
    .read = mp_machine_pdm_stream_read,
    .ioctl = mp_machine_pdm_ioctl,
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
