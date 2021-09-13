/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2021 NXP
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

#include <zephyr.h>
#include <drivers/spi.h>

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/machine_spi.h"
#include "modmachine.h"

#if MICROPY_PY_MACHINE_SPI

#define DEFAULT_SPI_BAUDRATE    (50000)
#define DEFAULT_SPI_POLARITY    (0)
#define DEFAULT_SPI_PHASE       (0)
#define DEFAULT_SPI_BITS        (8)
#define DEFAULT_SPI_FIRSTBIT    (SPI_TRANSFER_MSB)
#define SPI_LOOP                (0)    // For testing, enable loop mode by setting SPI_LOOP (1)

typedef struct _machine_hard_spi_obj_t {
    mp_obj_base_t base;
    const struct device *dev;
    struct spi_config config;
} machine_hard_spi_obj_t;

STATIC void machine_hard_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_spi_obj_t *self = self_in;
    mp_printf(print, "SPI(%s, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%s)",
        self->dev->name,
        self->config.frequency,
        (self->config.operation & 0x2) >> 1,
        (self->config.operation & 0x4) >> 2,
        (self->config.operation & ~0x1F) >> 5,
        ((self->config.operation & 0x10) >> 4) == SPI_TRANSFER_MSB ? "MSB" : "LSB");
}

mp_obj_t machine_hard_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso};

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = DEFAULT_SPI_BAUDRATE} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_POLARITY} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_PHASE} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_BITS} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_SPI_FIRSTBIT} },
        { MP_QSTR_sck, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const char *dev_name = mp_obj_str_get_str(args[ARG_id].u_obj);
    const struct device *dev = device_get_binding(dev_name);

    if (dev == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("device not found"));
    }

    if ((args[ARG_sck].u_obj != MP_OBJ_NULL) || (args[ARG_miso].u_obj != MP_OBJ_NULL) || (args[ARG_mosi].u_obj != MP_OBJ_NULL)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("explicit choice of sck/miso/mosi is not implemented"));
    }

    struct spi_config cfg = {
        .frequency = args[ARG_baudrate].u_int,
        .operation = (SPI_OP_MODE_MASTER |
            args[ARG_polarity].u_int << 1 |
                args[ARG_phase].u_int << 2 |
                SPI_LOOP << 3 |
                args[ARG_firstbit].u_int << 4 |
                args[ARG_bits].u_int << 5 |
                SPI_LINES_SINGLE),
        .slave = 0,
        .cs = NULL
    };

    machine_hard_spi_obj_t *self = m_new_obj(machine_hard_spi_obj_t);

    self->base.type = &machine_hard_spi_type;
    self->dev = dev;
    self->config = cfg;

    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_hard_spi_init(mp_obj_base_t *obj, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit};

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)obj;

    uint32_t baudrate;
    uint16_t operation = self->config.operation;

    if (args[ARG_baudrate].u_int != -1) {
        baudrate = args[ARG_baudrate].u_int;
    } else {
        baudrate = self->config.frequency;
    }

    if (args[ARG_polarity].u_int != -1) {
        operation = (operation & ~0x2) | (args[ARG_polarity].u_int << 1);
    }

    if (args[ARG_phase].u_int != -1) {
        operation = (operation & ~0x4) | (args[ARG_phase].u_int << 2);
    }

    if (args[ARG_bits].u_int != -1) {
        operation = (operation & 0x1F) | (args[ARG_bits].u_int << 5);
    }

    if (args[ARG_firstbit].u_int != -1) {
        operation = (operation & ~0x10) | (args[ARG_firstbit].u_int << 4);
    }

    struct spi_config cfg = {
        .frequency = baudrate,
        .operation = operation,
        .slave = 0,
        .cs = NULL
    };

    self->config = cfg;
}

STATIC void machine_hard_spi_transfer(mp_obj_base_t *obj, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t *)obj;

    int ret;

    struct spi_buf tx_bufs[1];
    tx_bufs[0].buf = (uint8_t *)src;
    tx_bufs[0].len = len;
    const struct spi_buf_set tx = {
        .buffers = tx_bufs,
        .count = ARRAY_SIZE(tx_bufs)
    };

    struct spi_buf rx_bufs[1];
    rx_bufs[0].buf = dest;
    rx_bufs[0].len = len;
    const struct spi_buf_set rx = {
        .buffers = rx_bufs,
        .count = ARRAY_SIZE(rx_bufs)
    };

    ret = spi_transceive(self->dev, &self->config, &tx, &rx);

    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
}

STATIC const mp_machine_spi_p_t machine_hard_spi_p = {
    .init = machine_hard_spi_init,
    .transfer = machine_hard_spi_transfer,
};

const mp_obj_type_t machine_hard_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = machine_hard_spi_print,
    .make_new = machine_hard_spi_make_new,
    .protocol = &machine_hard_spi_p,
    .locals_dict = (mp_obj_dict_t *)&mp_machine_spi_locals_dict,
};

#endif // MICROPY_PY_MACHINE_SPI
