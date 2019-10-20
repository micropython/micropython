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
#ifndef MICROPY_INCLUDED_EXTMOD_MACHINE_QSPI_H
#define MICROPY_INCLUDED_EXTMOD_MACHINE_QSPI_H

#include "py/obj.h"
#include "py/mphal.h"
#include "drivers/bus/qspi.h"

// QSPI protocol
typedef struct _mp_machine_qspi_p_t {
    void (*init)(mp_obj_base_t *obj, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
    void (*deinit)(mp_obj_base_t *obj); // can be NULL
    void (*write_cmd_data)(mp_obj_base_t *obj, uint8_t cmd, size_t len, uint32_t data);
    void (*write_cmd_addr_data)(mp_obj_base_t *obj, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src);
    uint32_t (*read_cmd)(mp_obj_base_t *obj, uint8_t cmd, size_t len);
    void (*read_cmd_qaddr_qdata)(mp_obj_base_t *obj, uint8_t cmd, uint32_t addr, size_t len, uint8_t *dest);

} mp_machine_qspi_p_t;

typedef struct _mp_machine_qspi_obj_t {
    mp_obj_base_t base;
    const mp_qspi_proto_t *proto;
} mp_machine_qspi_obj_t;

typedef struct _mp_machine_soft_qspi_obj_t {
    mp_machine_qspi_obj_t qspi;
    mp_soft_qspi_obj_t config;
} mp_machine_soft_qspi_obj_t;

extern const mp_obj_type_t mp_machine_qspi_type;
extern const mp_obj_type_t mp_machine_soft_qspi_type;
extern const mp_machine_qspi_p_t mp_machine_soft_qspi_p;
extern const mp_obj_dict_t mp_machine_qspi_locals_dict;

mp_obj_t mp_machine_qspi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_qspi_read_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_qspi_readinto_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_machine_qspi_write_obj);
MP_DECLARE_CONST_FUN_OBJ_3(mp_machine_qspi_write_readinto_obj);

#endif // MICROPY_INCLUDED_EXTMOD_MACHINE_QSPI_H
