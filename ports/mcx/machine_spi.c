/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2024 NXP
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

#include "drv_spi.h"

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;

    uint8_t id;
    uint32_t baudrate;

    drv_spi_t drv;
} machine_spi_obj_t;

static void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "SPI(%d, ", self->id);
    mp_printf(print, "baudrate=%d", self->baudrate);
    mp_printf(print, ")");
}

static void machine_spi_init_helper(machine_spi_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_sck].u_obj == mp_const_none) {
        /* TODO: Prepare a default pin for SCK */
    } else {
        mp_hal_pin_obj_t pin = args[ARG_sck].u_obj;

        /* TODO: Non LP-Flexcomm pinmap for future A series */
        const machine_pin_af_t *af = mp_hal_pin_find_af(pin, MACHINE_PIN_AF_TYPE_LPFC, self->id, MACHINE_PIN_AF_ATTR_LPFC_P1);
        if (af == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported SCK af"));
        }

        mp_hal_pin_af(pin, af->af_id);
    }

    if (args[ARG_mosi].u_obj == mp_const_none) {
        /* TODO: Prepare a default pin for MOSI */
    } else {
        mp_hal_pin_obj_t pin = args[ARG_mosi].u_obj;

        /* TODO: Non LP-Flexcomm pinmap for future A series */
        const machine_pin_af_t *af = mp_hal_pin_find_af(pin, MACHINE_PIN_AF_TYPE_LPFC, self->id, MACHINE_PIN_AF_ATTR_LPFC_P0);
        if (af == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported MOSI af"));
        }

        mp_hal_pin_af(pin, af->af_id);
    }

    if (args[ARG_miso].u_obj == mp_const_none) {
        /* TODO: Prepare a default pin for MISO */
    } else {
        mp_hal_pin_obj_t pin = args[ARG_miso].u_obj;

        /* TODO: Non LP-Flexcomm pinmap for future A series */
        const machine_pin_af_t *af = mp_hal_pin_find_af(pin, MACHINE_PIN_AF_TYPE_LPFC, self->id, MACHINE_PIN_AF_ATTR_LPFC_P2);
        if (af == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported MISO af"));
        }

        mp_hal_pin_af(pin, af->af_id);
    }

    /* TODO: Initialize SPI peripheral */
    drv_spi_config_t spi_cfg;

    self->baudrate = args[ARG_baudrate].u_int;

    spi_cfg.frequency = self->baudrate;
    spi_cfg.cpol = args[ARG_polarity].u_int;
    spi_cfg.cpha = args[ARG_phase].u_int;
    spi_cfg.frame_size = args[ARG_bits].u_int;

    if (args[ARG_firstbit].u_int == MICROPY_PY_MACHINE_SPI_MSB) {
        spi_cfg.lsbfirst = false;
    } else {
        spi_cfg.lsbfirst = true;
    }

    int ret = drv_spi_init(&self->drv, self->id, &spi_cfg);
    if (ret < 0) {
        if (ret == -ENOENT) {
            mp_raise_ValueError(MP_ERROR_TEXT("SPI not found."));
        } else {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to init SPI instance"));
        }
    }
}

mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    machine_spi_obj_t *self = mp_obj_malloc(machine_spi_obj_t, &machine_spi_type);

    self->id = mp_obj_get_int(args[0]);

    /* Initialize SPI instance only if kw arguments are given. */
    if (n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_spi_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return self;
}

static void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;

    /* Takes same parameters as constructor. */
    machine_spi_init_helper(self, n_args, pos_args, kw_args);
}

static void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_spi_obj_t *self = (machine_spi_obj_t *)self_in;

    drv_spi_transfer_t xfer = {
        .tx_data = src,
        .rx_data = dest,
        .len = len,
    };

    drv_spi_transfer(&self->drv, &xfer);
}

static const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .transfer = machine_spi_transfer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    print, machine_spi_print,
    protocol, &machine_spi_p,
    locals_dict, &mp_machine_spi_locals_dict
    );
