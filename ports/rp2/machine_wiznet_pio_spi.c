/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "wiznet_pio_spi.h"

#define DEFAULT_WIZNET_PIO_SPI_BAUDRATE    (1000000)
#define DEFAULT_WIZNET_PIO_SPI_BITS        (8)

wiznet_pio_spi_config_t wiznet_pio_spi_config;
wiznet_pio_spi_handle_t wiznet_pio_spi_handle = NULL;

static void machine_wiznet_pio_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_wiznet_pio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "WIZNET_PIO_SPI(baudrate=%u, sck=" MP_HAL_PIN_FMT ", mosi=" MP_HAL_PIN_FMT ", miso=" MP_HAL_PIN_FMT ")",
        self->spi.delay_half, mp_hal_pin_name(self->spi.sck), mp_hal_pin_name(self->spi.mosi), mp_hal_pin_name(self->spi.miso));
}

static mp_obj_t machine_wiznet_pio_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_baudrate, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = DEFAULT_WIZNET_PIO_SPI_BAUDRATE} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // create new object
    machine_wiznet_pio_spi_obj_t *self = mp_obj_malloc(machine_wiznet_pio_spi_obj_t, &machine_wiznet_pio_spi_type);

    // set parameters
    if (args[ARG_sck].u_obj == MP_OBJ_NULL
        || args[ARG_mosi].u_obj == MP_OBJ_NULL
        || args[ARG_miso].u_obj == MP_OBJ_NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("must specify all of sck/mosi/miso"));
    }
    self->spi.sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    self->spi.mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);
    self->spi.miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);

    uint32_t clock = clock_get_hz(clk_sys);
    uint32_t clock_div = clock / args[ARG_baudrate].u_int;

    if (clock_div > clock / 4) {
        clock_div = clock / 4;
    }

    wiznet_pio_spi_config.clock_div_major = clock_div;
    wiznet_pio_spi_config.clock_div_minor = 0;
    wiznet_pio_spi_config.data_in_pin = self->spi.miso;
    wiznet_pio_spi_config.data_out_pin = self->spi.mosi;
    wiznet_pio_spi_config.clock_pin = self->spi.sck;

    if (wiznet_pio_spi_handle != NULL) {
        wiznet_pio_spi_close(wiznet_pio_spi_handle);
    }
    wiznet_pio_spi_handle = wiznet_pio_spi_open(&wiznet_pio_spi_config);
    (*wiznet_pio_spi_handle)->set_active(wiznet_pio_spi_handle);

    return MP_OBJ_FROM_PTR(self);
}

static void machine_wiznet_pio_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_wiznet_pio_spi_obj_t *self = (machine_wiznet_pio_spi_obj_t *)self_in;

    enum { ARG_baudrate, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_sck].u_obj != MP_OBJ_NULL) {
        self->spi.sck = mp_hal_get_pin_obj(args[ARG_sck].u_obj);
    }
    if (args[ARG_mosi].u_obj != MP_OBJ_NULL) {
        self->spi.mosi = mp_hal_get_pin_obj(args[ARG_mosi].u_obj);
    }
    if (args[ARG_miso].u_obj != MP_OBJ_NULL) {
        self->spi.miso = mp_hal_get_pin_obj(args[ARG_miso].u_obj);
    }
    uint32_t clock = clock_get_hz(clk_sys);
    uint32_t clock_div = clock / args[ARG_baudrate].u_int;

    if (clock_div > clock / 4) {
        clock_div = clock / 4;
    }

    wiznet_pio_spi_config.clock_div_major = clock_div;
    wiznet_pio_spi_config.clock_div_minor = 0;
    wiznet_pio_spi_config.data_in_pin = self->spi.miso;
    wiznet_pio_spi_config.data_out_pin = self->spi.mosi;
    wiznet_pio_spi_config.clock_pin = self->spi.sck;

    if (wiznet_pio_spi_handle != NULL) {
        wiznet_pio_spi_close(wiznet_pio_spi_handle);
    }
    wiznet_pio_spi_handle = wiznet_pio_spi_open(&wiznet_pio_spi_config);
    (*wiznet_pio_spi_handle)->set_active(wiznet_pio_spi_handle);
}

static void machine_wiznet_pio_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    if (dest == NULL) {
        wiznet_pio_spi_write_buffer(src, len);
    } else {
        wiznet_pio_spi_read_buffer(dest, len);
    }
}

const mp_machine_spi_p_t machine_wiznet_pio_spi_p = {
    .init = machine_wiznet_pio_spi_init,
    .deinit = NULL,
    .transfer = machine_wiznet_pio_spi_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_wiznet_pio_spi_type,
    MP_QSTR_WIZNET_PIO_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_wiznet_pio_spi_make_new,
    print, machine_wiznet_pio_spi_print,
    protocol, &machine_wiznet_pio_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );
