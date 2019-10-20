/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "extmod/machine_qspi.h"

#if MICROPY_PY_MACHINE_QSPI

/******************************************************************************/
// MicroPython bindings for generic machine.QSPI

STATIC mp_obj_t mp_machine_soft_qspi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);

mp_obj_t mp_machine_qspi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    if (n_args == 0) {
        #if MICROPY_PY_MACHINE_QSPI && defined(MICROPY_PY_MACHINE_QSPI_MAKE_NEW)
        // dispatch to port-specific constructor
        extern mp_obj_t MICROPY_PY_MACHINE_QSPI_MAKE_NEW(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args);
        return MICROPY_PY_MACHINE_QSPI_MAKE_NEW(type, n_args, n_kw, args);
        #else
        mp_raise_ValueError("No hardware QSPI peripheral available");
        #endif
    }

    // software QSPI
    return mp_machine_soft_qspi_make_new(type, n_args, n_kw, args);
}

STATIC mp_obj_t machine_qspi_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_obj_base_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_machine_qspi_p_t *qspi_p = (mp_machine_qspi_p_t*)self->type->protocol;
    qspi_p->init(self, n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_qspi_init_obj, 1, machine_qspi_init);

STATIC mp_obj_t machine_qspi_deinit(mp_obj_t self_in) {
    mp_obj_base_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_qspi_p_t *qspi_p = (mp_machine_qspi_p_t*)self->type->protocol;
    if (qspi_p->deinit != NULL) {
        qspi_p->deinit(self);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_qspi_deinit_obj, machine_qspi_deinit);

STATIC const mp_rom_map_elem_t machine_qspi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_qspi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_qspi_deinit_obj) },
};

MP_DEFINE_CONST_DICT(mp_machine_qspi_locals_dict, machine_qspi_locals_dict_table);

/******************************************************************************/
// MicroPython wrapper for soft QSPI

STATIC void mp_machine_soft_qspi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_machine_soft_qspi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SoftQSPI(cs=" MP_HAL_PIN_FMT ", clk=" MP_HAL_PIN_FMT ", "
        "io0=" MP_HAL_PIN_FMT ", io1=" MP_HAL_PIN_FMT ", io2=" MP_HAL_PIN_FMT ", io3=" MP_HAL_PIN_FMT ")",
        mp_hal_pin_name(self->config.cs), mp_hal_pin_name(self->config.cs), mp_hal_pin_name(self->config.io0), 
        mp_hal_pin_name(self->config.io1), mp_hal_pin_name(self->config.io2), mp_hal_pin_name(self->config.io3));
}

enum { ARG_cs, ARG_clk, ARG_io0, ARG_io1, ARG_io2, ARG_io3, };
static const mp_arg_t allowed_args[] = {
    { MP_QSTR_cs, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_clk, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_io0, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_io1, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_io2, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_io3, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};


STATIC mp_obj_t mp_machine_soft_qspi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // create new object
    mp_machine_soft_qspi_obj_t *self = m_new_obj(mp_machine_soft_qspi_obj_t);
    self->qspi.base.type = &mp_machine_soft_qspi_type;
    self->qspi.proto = &mp_soft_qspi_proto;

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->config.cs = mp_hal_get_pin_obj(args[ARG_cs].u_obj);
    self->config.clk = mp_hal_get_pin_obj(args[ARG_clk].u_obj);
    self->config.io0 = mp_hal_get_pin_obj(args[ARG_io0].u_obj);
    self->config.io1 = mp_hal_get_pin_obj(args[ARG_io1].u_obj);
    self->config.io2 = mp_hal_get_pin_obj(args[ARG_io2].u_obj);
    self->config.io3 = mp_hal_get_pin_obj(args[ARG_io3].u_obj);

    // configure bus
    mp_soft_qspi_proto.ioctl(&self->config, MP_QSPI_IOCTL_INIT);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void mp_machine_soft_qspi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_machine_soft_qspi_obj_t *self = (mp_machine_soft_qspi_obj_t*)self_in;

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->config.cs = mp_hal_get_pin_obj(args[ARG_cs].u_obj);
    self->config.clk = mp_hal_get_pin_obj(args[ARG_clk].u_obj);
    self->config.io0 = mp_hal_get_pin_obj(args[ARG_io0].u_obj);
    self->config.io1 = mp_hal_get_pin_obj(args[ARG_io1].u_obj);
    self->config.io2 = mp_hal_get_pin_obj(args[ARG_io2].u_obj);
    self->config.io3 = mp_hal_get_pin_obj(args[ARG_io3].u_obj);

    // configure bus
    mp_soft_qspi_proto.ioctl(&self->config, MP_QSPI_IOCTL_INIT);
}

STATIC void mp_machine_soft_qspi_write_cmd_data(mp_obj_base_t *self_in, uint8_t cmd, size_t len, uint32_t data) {
    mp_machine_soft_qspi_obj_t *self = (mp_machine_soft_qspi_obj_t*)self_in;
    mp_soft_qspi_proto.write_cmd_data(&self->config, cmd, len, data);
}

STATIC void mp_machine_soft_qspi_write_cmd_addr_data(mp_obj_base_t *self_in, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
    mp_machine_soft_qspi_obj_t *self = (mp_machine_soft_qspi_obj_t*)self_in;
    mp_soft_qspi_proto.write_cmd_addr_data(&self->config, cmd, addr, len, src);
}

STATIC uint32_t mp_machine_soft_qspi_read_cmd(mp_obj_base_t *self_in, uint8_t cmd, size_t len) {
    mp_machine_soft_qspi_obj_t *self = (mp_machine_soft_qspi_obj_t*)self_in;
    return mp_soft_qspi_proto.read_cmd(&self->config, cmd, len);
}

STATIC void mp_machine_soft_qspi_read_cmd_qaddr_qdata(mp_obj_base_t *self_in, uint8_t cmd, uint32_t addr, size_t len, uint8_t *dest) {
    mp_machine_soft_qspi_obj_t *self = (mp_machine_soft_qspi_obj_t*)self_in;
    mp_soft_qspi_proto.read_cmd_qaddr_qdata(&self->config, cmd, addr, len, dest);
}

const mp_machine_qspi_p_t mp_machine_soft_qspi_p = {
    .init = mp_machine_soft_qspi_init,
    .deinit = NULL,
    .write_cmd_data = mp_machine_soft_qspi_write_cmd_data,
    .write_cmd_addr_data = mp_machine_soft_qspi_write_cmd_addr_data,
    .read_cmd = mp_machine_soft_qspi_read_cmd,
    .read_cmd_qaddr_qdata = mp_machine_soft_qspi_read_cmd_qaddr_qdata,
};

// Base type for all QSPI drivers
const mp_obj_type_t mp_machine_qspi_type = {
    { &mp_type_type },
    .name = MP_QSTR_QSPI,
    .make_new = mp_machine_qspi_make_new, // delegate to master constructor
    .locals_dict = (mp_obj_dict_t*)&mp_machine_qspi_locals_dict,
};

const mp_obj_type_t mp_machine_soft_qspi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SoftQSPI,
    .print = mp_machine_soft_qspi_print,
    .protocol = &mp_machine_soft_qspi_p,
    .parent = &mp_machine_qspi_type,
};

#endif // MICROPY_PY_MACHINE_QSPI
