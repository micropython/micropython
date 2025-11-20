/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/modmachine.h"

// PIC32MX I2C base addresses
#define I2C1_BASE  0xBF805000
#define I2C2_BASE  0xBF805100

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    uint8_t i2c_id;
    uint32_t freq;
    uint8_t scl;
    uint8_t sda;
} machine_i2c_obj_t;

STATIC mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 4, true);

    machine_i2c_obj_t *self = mp_obj_malloc(machine_i2c_obj_t, type);
    self->i2c_id = mp_obj_get_int(args[0]);
    self->freq = (n_args >= 2) ? mp_obj_get_int(args[1]) : 100000;

    // Initialize I2C peripheral
    // TODO: Configure I2C controller registers

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_i2c_scan(mp_obj_t self_in) {
    mp_obj_t list = mp_obj_new_list(0, NULL);
    // TODO: Scan I2C bus for devices
    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_scan_obj, machine_i2c_scan);

STATIC const mp_rom_map_elem_t machine_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&machine_i2c_scan_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_i2c_locals_dict, machine_i2c_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    locals_dict, &machine_i2c_locals_dict
);
