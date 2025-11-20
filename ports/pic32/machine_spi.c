/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
 */

#include "py/runtime.h"
#include "py/mphal.h"

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    uint8_t spi_id;
    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
} machine_spi_obj_t;

STATIC mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 5, true);

    machine_spi_obj_t *self = mp_obj_malloc(machine_spi_obj_t, type);
    self->spi_id = mp_obj_get_int(args[0]);
    self->baudrate = (n_args >= 2) ? mp_obj_get_int(args[1]) : 1000000;
    self->polarity = 0;
    self->phase = 0;
    self->bits = 8;

    // TODO: Initialize SPI peripheral

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_spi_write(mp_obj_t self_in, mp_obj_t buf_in) {
    // TODO: Implement SPI write
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_write_obj, machine_spi_write);

STATIC const mp_rom_map_elem_t machine_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_spi_write_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_spi_locals_dict, machine_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    locals_dict, &machine_spi_locals_dict
);
