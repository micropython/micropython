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
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "extmod/machine_spi.h"

/******************************************************************************/
// MicroPython bindings for SPI

STATIC uint32_t baudrate_from_delay_half(uint32_t delay_half) {
    return 500000 / delay_half;
}

STATIC uint32_t baudrate_to_delay_half(uint32_t baudrate) {
    uint32_t delay_half = 500000 / baudrate;
    // round delay_half up so that: actual_baudrate <= requested_baudrate
    if (500000 % baudrate != 0) {
        delay_half += 1;
    }
    return delay_half;
}

STATIC void pyb_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_machine_soft_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(baudrate=%u, polarity=%u, phase=%u, sck=%u, mosi=%u, miso=%u)",
        baudrate_from_delay_half(self->delay_half),
        self->polarity, self->phase, self->sck, self->mosi, self->miso);
}

STATIC void pyb_spi_init_helper(mp_machine_soft_spi_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_baudrate].u_int != -1) {
        self->delay_half = baudrate_to_delay_half(args[ARG_baudrate].u_int);
    }
    if (args[ARG_polarity].u_int != -1) {
        self->polarity = args[ARG_polarity].u_int;
    }
    if (args[ARG_phase].u_int != -1) {
        self->phase = args[ARG_phase].u_int;
    }
    if (args[ARG_sck].u_obj != MP_OBJ_NULL) {
        self->sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    }
    if (args[ARG_mosi].u_obj != MP_OBJ_NULL) {
        self->mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);
    }
    if (args[ARG_miso].u_obj != MP_OBJ_NULL) {
        self->miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);
    }

    // configure pins
    mp_hal_pin_write(self->sck, self->polarity);
    mp_hal_pin_output(self->sck);
    mp_hal_pin_output(self->mosi);
    mp_hal_pin_input(self->miso);
}

mp_obj_t pyb_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    mp_machine_soft_spi_obj_t *self = m_new_obj(mp_machine_soft_spi_obj_t);
    self->base.type = &pyb_spi_type;
    // set defaults
    self->delay_half = baudrate_to_delay_half(500000);
    self->polarity = 0;
    self->phase = 0;
    self->sck = 14;
    self->mosi = 13;
    self->miso = 12;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    pyb_spi_init_helper(self, n_args, args, &kw_args);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_spi_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_spi_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_init_obj, 1, pyb_spi_init);

STATIC const mp_rom_map_elem_t pyb_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_spi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_machine_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_machine_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_machine_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&mp_machine_spi_write_readinto_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);

STATIC const mp_machine_spi_p_t pyb_spi_p = {
    .transfer = mp_machine_soft_spi_transfer,
};

const mp_obj_type_t pyb_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SoftSPI,
    .print = pyb_spi_print,
    .make_new = pyb_spi_make_new,
    .protocol = &pyb_spi_p,
    .locals_dict = (mp_obj_dict_t*)&pyb_spi_locals_dict,
};
