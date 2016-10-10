/**
 * @file module_drivers/class_spi.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

/**
 * Print the spi object.
 */
static void class_spi_print(const mp_print_t *print_p,
                            mp_obj_t self_in,
                            mp_print_kind_t kind)
{
    struct class_spi_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_printf(print_p, "<0x%p>", self_p);
}

/**
 * Create a new Spi object associated with the id. If additional
 * arguments are given, they are used to initialise the spi. See
 * `init`.
 */
static mp_obj_t class_spi_make_new(const mp_obj_type_t *type_p,
                                   mp_uint_t n_args,
                                   mp_uint_t n_kw,
                                   const mp_obj_t *args_p)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_slave_select, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_mode, MP_ARG_INT, { .u_int = SPI_MODE_MASTER } },
        { MP_QSTR_speed, MP_ARG_INT, { .u_int = SPI_SPEED_250KBPS } },
        { MP_QSTR_polarity, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_phase, MP_ARG_INT, { .u_int = 0 } }
    };
    struct class_spi_t *self_p;
    mp_map_t kwargs;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    int device;
    int slave_select;
    int mode;
    int speed;
    int polarity;
    int phase;

    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    /* Parse args. */
    mp_map_init(&kwargs, 0);
    mp_arg_parse_all(n_args,
                     args_p,
                     &kwargs,
                     MP_ARRAY_SIZE(allowed_args),
                     allowed_args,
                     args);

    /* Create a new spi object. */
    self_p = m_new_obj(struct class_spi_t);
    self_p->base.type = &module_drivers_class_spi;

    /* Device argument. */
    device = args[0].u_int;

    if ((device < 0) || (device > SPI_DEVICE_MAX)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "bad device"));
    }

    /* Slave select argument. */
    slave_select = args[1].u_int;

    if ((slave_select < 0) || (slave_select > PIN_DEVICE_MAX)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "bad slave select"));
    }

    /* Mode argument. */
    mode = args[2].u_int;

    if ((mode != SPI_MODE_MASTER) && (mode != SPI_MODE_SLAVE)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "bad mode"));
    }

    /* Speed argument. */
    speed = args[3].u_int;

    /* Polarity argument. */
    polarity = args[4].u_int;

    if ((polarity != 0) && (polarity != 1)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "bad polarity"));
    }

    /* Phase argument. */
    phase = args[5].u_int;

    if ((phase != 0) && (phase != 1)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "bad phase"));
    }

    if (spi_init(&self_p->drv,
                 &spi_device[device],
                 &pin_device[slave_select],
                 mode,
                 speed,
                 polarity,
                 phase) != 0) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (self_p);
}

/**
 * def start(self)
 */
static mp_obj_t class_spi_start(mp_obj_t self_in)
{
    struct class_spi_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    spi_start(&self_p->drv);

    return (mp_const_none);
}

/**
 * def stop(self, samples)
 */
static mp_obj_t class_spi_stop(mp_obj_t self_in)
{
    struct class_spi_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    spi_stop(&self_p->drv);

    return (mp_const_none);
}

/**
 * def take_bus(self)
 */
static mp_obj_t class_spi_take_bus(mp_obj_t self_in)
{
    struct class_spi_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    spi_take_bus(&self_p->drv);

    return (mp_const_none);
}

/**
 * def give_bus(self)
 */
static mp_obj_t class_spi_give_bus(mp_obj_t self_in)
{
    struct class_spi_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    spi_give_bus(&self_p->drv);

    return (mp_const_none);
}

/**
 * def select(self)
 */
static mp_obj_t class_spi_select(mp_obj_t self_in)
{
    struct class_spi_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    spi_select(&self_p->drv);

    return (mp_const_none);
}

/**
 * def deselect(self)
 */
static mp_obj_t class_spi_deselect(mp_obj_t self_in)
{
    struct class_spi_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    spi_deselect(&self_p->drv);

    return (mp_const_none);
}

/**
 * def transfer(self, write_buffer[, size])
 */
static mp_obj_t class_spi_transfer(mp_uint_t n_args, const mp_obj_t *args_p)
{
    struct class_spi_t *self_p;
    vstr_t vstr;
    mp_buffer_info_t buffer_info;
    size_t size;

    self_p = MP_OBJ_TO_PTR(args_p[0]);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(args_p[1]),
                        &buffer_info,
                        MP_BUFFER_READ);

    /* Get the size. */
    if (n_args == 3) {
        size = mp_obj_get_int(args_p[2]);

        if (buffer_info.len < size) {
            nlr_raise(mp_obj_new_exception(&mp_type_OSError));
        }
    } else {
        size = buffer_info.len;
    }

    vstr_init_len(&vstr, size);

    if (spi_transfer(&self_p->drv, vstr.buf, buffer_info.buf, size) != size) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr));
}

/**
 * def transfer(self, read_buffer, write_buffer[, size])
 */
static mp_obj_t class_spi_transfer_into(mp_uint_t n_args, const mp_obj_t *args_p)
{
    struct class_spi_t *self_p;
    mp_buffer_info_t read_buffer_info;
    mp_buffer_info_t write_buffer_info;
    size_t size;

    self_p = MP_OBJ_TO_PTR(args_p[0]);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(args_p[1]),
                        &read_buffer_info,
                        MP_BUFFER_WRITE);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(args_p[2]),
                        &write_buffer_info,
                        MP_BUFFER_READ);

    /* Get the size. */
    if (n_args == 4) {
        size = mp_obj_get_int(args_p[3]);

        if ((read_buffer_info.len < size) || (write_buffer_info.len < size)) {
            nlr_raise(mp_obj_new_exception(&mp_type_OSError));
        }
    } else {
        size = read_buffer_info.len;

        if (read_buffer_info.len != write_buffer_info.len) {
            nlr_raise(mp_obj_new_exception(&mp_type_OSError));
        }
    }

    if (spi_transfer(&self_p->drv,
                     read_buffer_info.buf,
                     write_buffer_info.buf,
                     size) != size) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (MP_OBJ_NEW_SMALL_INT(size));
}

/**
 * def read(self, size)
 */
static mp_obj_t class_spi_read(mp_obj_t self_in, mp_obj_t size_in)
{
    struct class_spi_t *self_p;
    vstr_t vstr;
    size_t size;

    self_p = MP_OBJ_TO_PTR(self_in);
    size = mp_obj_get_int(size_in);

    vstr_init_len(&vstr, size);

    if (spi_read(&self_p->drv, vstr.buf, size) != size) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr));
}

/**
 * def read_into(self, buffer[, size])
 */
static mp_obj_t class_spi_read_into(mp_uint_t n_args, const mp_obj_t *args_p)
{
    struct class_spi_t *self_p;
    mp_buffer_info_t buffer_info;
    size_t size;

    self_p = MP_OBJ_TO_PTR(args_p[0]);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(args_p[1]),
                        &buffer_info,
                        MP_BUFFER_WRITE);

    /* Get the size. */
    if (n_args == 3) {
        size = mp_obj_get_int(args_p[2]);

        if (buffer_info.len < size) {
            nlr_raise(mp_obj_new_exception(&mp_type_OSError));
        }
    } else {
        size = buffer_info.len;
    }

    if (spi_read(&self_p->drv, buffer_info.buf, size) != size) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (MP_OBJ_NEW_SMALL_INT(size));
}

/**
 * def write(self, buffer[, size])
 */
static mp_obj_t class_spi_write(mp_uint_t n_args, const mp_obj_t *args_p)
{
    struct class_spi_t *self_p;
    mp_buffer_info_t buffer_info;
    size_t size;

    self_p = MP_OBJ_TO_PTR(args_p[0]);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(args_p[1]),
                        &buffer_info,
                        MP_BUFFER_READ);

    /* Get the size. */
    if (n_args == 3) {
        size = mp_obj_get_int(args_p[2]);

        if (buffer_info.len < size) {
            nlr_raise(mp_obj_new_exception(&mp_type_OSError));
        }
    } else {
        size = buffer_info.len;
    }

    if (spi_write(&self_p->drv, buffer_info.buf, size) != size) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (MP_OBJ_NEW_SMALL_INT(size));
}

static MP_DEFINE_CONST_FUN_OBJ_1(class_spi_start_obj, class_spi_start);
static MP_DEFINE_CONST_FUN_OBJ_1(class_spi_stop_obj, class_spi_stop);
static MP_DEFINE_CONST_FUN_OBJ_1(class_spi_take_bus_obj, class_spi_take_bus);
static MP_DEFINE_CONST_FUN_OBJ_1(class_spi_give_bus_obj, class_spi_give_bus);
static MP_DEFINE_CONST_FUN_OBJ_1(class_spi_select_obj, class_spi_select);
static MP_DEFINE_CONST_FUN_OBJ_1(class_spi_deselect_obj, class_spi_deselect);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(class_spi_transfer_obj, 2, 3, class_spi_transfer);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(class_spi_transfer_into_obj, 3, 4, class_spi_transfer_into);
static MP_DEFINE_CONST_FUN_OBJ_2(class_spi_read_obj, class_spi_read);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(class_spi_read_into_obj, 2, 3, class_spi_read_into);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(class_spi_write_obj, 2, 3, class_spi_write);

static const mp_rom_map_elem_t class_spi_locals_dict_table[] = {
    /* Instance methods. */
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&class_spi_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&class_spi_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_take_bus), MP_ROM_PTR(&class_spi_take_bus_obj) },
    { MP_ROM_QSTR(MP_QSTR_give_bus), MP_ROM_PTR(&class_spi_give_bus_obj) },
    { MP_ROM_QSTR(MP_QSTR_select), MP_ROM_PTR(&class_spi_select_obj) },
    { MP_ROM_QSTR(MP_QSTR_deselect), MP_ROM_PTR(&class_spi_deselect_obj) },
    { MP_ROM_QSTR(MP_QSTR_transfer), MP_ROM_PTR(&class_spi_transfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_transfer_into), MP_ROM_PTR(&class_spi_transfer_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&class_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_into), MP_ROM_PTR(&class_spi_read_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&class_spi_write_obj) },

    /* Class constants. */
    { MP_ROM_QSTR(MP_QSTR_MODE_MASTER), MP_ROM_INT(SPI_MODE_MASTER) },
    { MP_ROM_QSTR(MP_QSTR_MODE_SLAVE), MP_ROM_INT(SPI_MODE_SLAVE) },

    { MP_ROM_QSTR(MP_QSTR_SPEED_8MBPS), MP_ROM_INT(SPI_SPEED_8MBPS) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_4MBPS), MP_ROM_INT(SPI_SPEED_4MBPS) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_2MBPS), MP_ROM_INT(SPI_SPEED_2MBPS) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_1MBPS), MP_ROM_INT(SPI_SPEED_1MBPS) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_500KBPS), MP_ROM_INT(SPI_SPEED_500KBPS) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_250KBPS), MP_ROM_INT(SPI_SPEED_250KBPS) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_125KBPS), MP_ROM_INT(SPI_SPEED_125KBPS) }
};

static MP_DEFINE_CONST_DICT(class_spi_locals_dict, class_spi_locals_dict_table);

/**
 * Spi class type.
 */
const mp_obj_type_t module_drivers_class_spi = {
    { &mp_type_type },
    .name = MP_QSTR_Spi,
    .print = class_spi_print,
    .make_new = class_spi_make_new,
    .locals_dict = (mp_obj_t)&class_spi_locals_dict,
};
