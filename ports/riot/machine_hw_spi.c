/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Koen Zandberg
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


#include <string.h>

#include "periph/spi.h"
#include "periph/gpio.h"
#include "board.h"

#include "py/runtime.h"
#include "modmachine.h"
#include "extmod/machine_spi.h"

#if MICROPY_PY_MACHINE_SPI

typedef struct _machine_hw_spi_obj_t {
    mp_obj_base_t base;
    spi_t bus;
    uint32_t baudrate;
    spi_mode_t mode;
} machine_hw_spi_obj_t;

STATIC machine_hw_spi_obj_t machine_hw_spi_obj[SPI_NUMOF];

// Convert a MP polarity and phase to a RIOT enum value
STATIC spi_mode_t _to_riot_mode(bool polarity, bool phase)
{
    if (polarity) {
        if (phase) {
            return SPI_MODE_3;
        }
        else {
            return SPI_MODE_2;
        }
    }
    else {
        if (phase) {
            return SPI_MODE_1;
        }
        else {
            return SPI_MODE_0;
        }
    }
}

STATIC uint8_t _phase(spi_mode_t mode)
{
    return (mode == SPI_MODE_1 || mode == SPI_MODE_3) ? 1 : 0;
}

STATIC bool _polarity(spi_mode_t mode)
{
    return (mode == SPI_MODE_2 || mode == SPI_MODE_3) ? 1 : 0;
}

STATIC void machine_hw_spi_print(const mp_print_t *print, mp_obj_t self_in,
                                 mp_print_kind_t kind) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(id=%u, baudrate=%u, polarity=%u, phase=%u, sck=?, mosi=?, miso=?)",
              self->bus, self->baudrate,
              _polarity(self->mode), _phase(self->mode));
}

STATIC void machine_hw_spi_init(mp_obj_base_t *self_in, size_t n_args,
                                const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_hw_spi_obj_t *self = (machine_hw_spi_obj_t *) self_in;

    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);

    self->mode = _to_riot_mode(args[ARG_polarity].u_int, args[ARG_phase].u_int);
    self->baudrate = args[ARG_baudrate].u_int;
}

mp_obj_t machine_hw_spi_make_new(const mp_obj_type_t *type, size_t n_args,
                                 size_t n_kw, const mp_obj_t *all_args) {

    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits,
           ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args,
                              MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // here we would check the sck/mosi/miso pins and configure them,
    // but it's not supported with RIOT
    if (args[ARG_sck].u_obj != MP_OBJ_NULL
        || args[ARG_mosi].u_obj != MP_OBJ_NULL
        || args[ARG_miso].u_obj != MP_OBJ_NULL) {
        mp_raise_ValueError("explicit choice of sck/mosi/miso is not allowed by port");
    }

    // get static peripheral object
    int spi_id = args[ARG_id].u_int;
    if (spi_id < 0 || spi_id >= (int)SPI_NUMOF) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "SPI(%d) doesn't exist (max id=%u)", spi_id, SPI_NUMOF - 1));
    }
    machine_hw_spi_obj_t *self = &machine_hw_spi_obj[spi_id];
    self->base.type = &machine_hw_spi_type;

    self->bus = SPI_DEV(spi_id);
    self->mode = _to_riot_mode(args[ARG_polarity].u_int, args[ARG_phase].u_int);
    self->baudrate = args[ARG_baudrate].u_int;
    return MP_OBJ_FROM_PTR(self);
}

// Nothing to deinit here
STATIC void machine_hw_spi_deinit(mp_obj_base_t *self_in) {
    (void)self_in;
}

STATIC void machine_hw_spi_transfer(mp_obj_base_t *self_in, size_t len,
                                    const uint8_t *src, uint8_t *dest) {
    machine_hw_spi_obj_t *self = (machine_hw_spi_obj_t*)self_in;
    spi_acquire(self->bus, SPI_CS_UNDEF, self->mode, self->baudrate);
    spi_transfer_bytes(self->bus, SPI_CS_UNDEF, true, src, dest, len);
    spi_release(self->bus);
}

STATIC const mp_machine_spi_p_t machine_hw_spi_p = {
    .init = machine_hw_spi_init,
    .deinit = machine_hw_spi_deinit,
    .transfer = machine_hw_spi_transfer,
};

const mp_obj_type_t machine_hw_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = machine_hw_spi_print,
    .make_new = machine_hw_spi_make_new,
    .protocol = &machine_hw_spi_p,
    .locals_dict = (mp_obj_dict_t *) &mp_machine_spi_locals_dict,
};
#endif
