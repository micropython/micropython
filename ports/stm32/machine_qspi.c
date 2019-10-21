/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
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
#include "extmod/machine_qspi.h"
#include "qspi.h"

#if MICROPY_PY_MACHINE_QSPI

/******************************************************************************/
// Implementation of hard SPI for machine module

STATIC const mp_machine_qspi_obj_t qspi_obj = {{&machine_hard_qspi_type}, &qspi_proto};

STATIC void machine_hard_qspi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)self_in;
    mp_printf(print, "QSPI()");
}

mp_obj_t machine_hard_qspi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    (void)type;
    (void)n_args;
    (void)n_kw;
    (void)all_args;

    // get static peripheral object
    const mp_machine_qspi_obj_t *self = &qspi_obj;

    // configure bus
    qspi_proto.ioctl(NULL, MP_QSPI_IOCTL_INIT);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void mp_machine_qspi_write_cmd_data(mp_obj_base_t *self_in, uint8_t cmd, size_t len, uint32_t data) {
    (void)self_in;
    qspi_proto.write_cmd_data(NULL, cmd, len, data);
}

STATIC void mp_machine_qspi_write_cmd_addr_data(mp_obj_base_t *self_in, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
    (void)self_in;
    qspi_proto.write_cmd_addr_data(NULL, cmd, addr, len, src);
}

STATIC uint32_t mp_machine_qspi_read_cmd(mp_obj_base_t *self_in, uint8_t cmd, size_t len) {
    (void)self_in;
    return qspi_proto.read_cmd(NULL, cmd, len);
}

STATIC void mp_machine_qspi_read_cmd_qaddr_qdata(mp_obj_base_t *self_in, uint8_t cmd, uint32_t addr, size_t len, uint8_t *dest) {
    (void)self_in;
    qspi_proto.read_cmd_qaddr_qdata(NULL, cmd, addr, len, dest);
}

STATIC const mp_machine_qspi_p_t machine_hard_qspi_p = {
    .init = NULL,
    .deinit = NULL,
    .write_cmd_data = mp_machine_qspi_write_cmd_data,
    .write_cmd_addr_data = mp_machine_qspi_write_cmd_addr_data,
    .read_cmd = mp_machine_qspi_read_cmd,
    .read_cmd_qaddr_qdata = mp_machine_qspi_read_cmd_qaddr_qdata,
};

const mp_obj_type_t machine_hard_qspi_type = {
    { &mp_type_type },
    .name = MP_QSTR_QSPI,
    .print = machine_hard_qspi_print,
    .make_new = mp_machine_qspi_make_new, // delegate to master constructor
    .protocol = &machine_hard_qspi_p,
    .locals_dict = (mp_obj_dict_t*)&mp_machine_qspi_locals_dict,
    .parent = &mp_machine_qspi_type,
};

#endif // MICROPY_PY_MACHINE_QSPI
