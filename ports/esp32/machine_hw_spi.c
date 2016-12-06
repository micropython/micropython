/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
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
#include "modmachine.h"

#include "driver/spi_master.h"

typedef struct _machine_hw_spi_obj_t {
    mp_obj_base_t base;
    spi_host_device_t host;
    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
    int8_t sck;
    int8_t mosi;
    int8_t miso;
    spi_device_handle_t spi;
    enum {
        MACHINE_HW_SPI_STATE_NONE,
        MACHINE_HW_SPI_STATE_INIT,
        MACHINE_HW_SPI_STATE_DEINIT
    } state;
} machine_hw_spi_obj_t;

STATIC void machine_hw_spi_deinit_internal(machine_hw_spi_obj_t *self) {
    switch (spi_bus_remove_device(self->spi)) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, "invalid configuration");
            return;

        case ESP_ERR_INVALID_STATE:
            mp_raise_msg(&mp_type_OSError, "SPI device already freed");
            return;
    }

    switch (spi_bus_free(self->host)) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, "invalid configuration");
            return;

        case ESP_ERR_INVALID_STATE:
            mp_raise_msg(&mp_type_OSError, "SPI bus already freed");
            return;
    }

    int8_t pins[3] = {self->miso, self->mosi, self->sck};

    for (int i = 0; i < 3; i++) {
        if (pins[i] != -1) {
            gpio_pad_select_gpio(pins[i]);
            gpio_matrix_out(pins[i], SIG_GPIO_OUT_IDX, false, false);
            gpio_set_direction(pins[i], GPIO_MODE_INPUT);
        }
    }
}

STATIC void machine_hw_spi_init_internal(
    machine_hw_spi_obj_t    *self,
    int8_t                  host,
    int32_t                 baudrate,
    int8_t                  polarity,
    int8_t                  phase,
    int8_t                  bits,
    int8_t                  firstbit,
    int8_t                  sck,
    int8_t                  mosi,
    int8_t                  miso) {

    // if we're not initialized, then we're
    // implicitly 'changed', since this is the init routine
    bool changed = self->state != MACHINE_HW_SPI_STATE_INIT;

    esp_err_t ret;

    machine_hw_spi_obj_t old_self = *self;

    if (host != -1 && host != self->host) {
        self->host = host;
        changed = true;
    }

    if (baudrate != -1 && baudrate != self->baudrate) {
        self->baudrate = baudrate;
        changed = true;
    }

    if (polarity != -1 && polarity != self->polarity) {
        self->polarity = polarity;
        changed = true;
    }

    if (phase != -1 && phase != self->phase) {
        self->phase =  phase;
        changed = true;
    }

    if (bits != -1 && bits != self->bits) {
        self->bits = bits;
        changed = true;
    }

    if (firstbit != -1 && firstbit != self->firstbit) {
        self->firstbit = firstbit;
        changed = true;
    }

    if (sck != -2 && sck != self->sck) {
        self->sck = sck;
        changed = true;
    }

    if (mosi != -2 && mosi != self->mosi) {
        self->mosi = mosi;
        changed = true;
    }

    if (miso != -2 && miso != self->miso) {
        self->miso = miso;
        changed = true;
    }

    if (self->host != HSPI_HOST && self->host != VSPI_HOST) {
        mp_raise_ValueError("SPI ID must be either HSPI(1) or VSPI(2)");
    }

    if (changed) {
        if (self->state == MACHINE_HW_SPI_STATE_INIT) {
            self->state = MACHINE_HW_SPI_STATE_DEINIT;
            machine_hw_spi_deinit_internal(&old_self);
        }
    } else {
        return; // no changes
    }

    spi_bus_config_t buscfg = {
        .miso_io_num = self->miso,
        .mosi_io_num = self->mosi,
        .sclk_io_num = self->sck,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = self->baudrate,
        .mode = self->phase | (self->polarity << 1),
        .spics_io_num = -1, // No CS pin
        .queue_size = 1,
        .flags = self->firstbit == MICROPY_PY_MACHINE_SPI_LSB ? SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST : 0,
        .pre_cb = NULL
    };

    //Initialize the SPI bus
    // FIXME: Does the DMA matter? There are two

    ret = spi_bus_initialize(self->host, &buscfg, 1);
    switch (ret) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, "invalid configuration");
            return;

        case ESP_ERR_INVALID_STATE:
            mp_raise_msg(&mp_type_OSError, "SPI device already in use");
            return;
    }

    ret = spi_bus_add_device(self->host, &devcfg, &self->spi);
    switch (ret) {
        case ESP_ERR_INVALID_ARG:
            mp_raise_msg(&mp_type_OSError, "invalid configuration");
            spi_bus_free(self->host);
            return;

        case ESP_ERR_NO_MEM:
            mp_raise_msg(&mp_type_OSError, "out of memory");
            spi_bus_free(self->host);
            return;

        case ESP_ERR_NOT_FOUND:
            mp_raise_msg(&mp_type_OSError, "no free slots");
            spi_bus_free(self->host);
            return;
    }
    self->state = MACHINE_HW_SPI_STATE_INIT;
}

STATIC void machine_hw_spi_deinit(mp_obj_base_t *self_in) {
    machine_hw_spi_obj_t *self = (machine_hw_spi_obj_t *) self_in;
    if (self->state == MACHINE_HW_SPI_STATE_INIT) {
        self->state = MACHINE_HW_SPI_STATE_DEINIT;
        machine_hw_spi_deinit_internal(self);
    }
}

STATIC void machine_hw_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int bits_to_send = len * self->bits;
    bool shortMsg = len <= 4;

    if (self->state == MACHINE_HW_SPI_STATE_DEINIT) {
        mp_raise_msg(&mp_type_OSError, "transfer on deinitialized SPI");
        return;
    }

    struct spi_transaction_t transaction = {
        .flags = 0,
        .length = bits_to_send,
        .tx_buffer = NULL,
        .rx_buffer = NULL,
    };

    if (shortMsg) {
        if (src != NULL) {
            memcpy(&transaction.tx_data, src, len);
        }
        transaction.flags |= (SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA);
    } else {
        transaction.tx_buffer = src;
        transaction.rx_buffer = dest;
    }

    spi_device_transmit(self->spi, &transaction);

    if (shortMsg && dest != NULL) {
        memcpy(dest, &transaction.rx_data, len);
    }
}

/******************************************************************************/
// MicroPython bindings for hw_spi

STATIC void machine_hw_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hw_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(id=%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%u, sck=%d, mosi=%d, miso=%d)",
              self->host, self->baudrate, self->polarity,
              self->phase, self->bits, self->firstbit,
              self->sck, self->mosi, self->miso);
}

STATIC void machine_hw_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_hw_spi_obj_t *self = (machine_hw_spi_obj_t *) self_in;

    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_INT , {.u_int = -1} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);
    int8_t sck, mosi, miso;

    if (args[ARG_sck].u_obj == MP_OBJ_NULL) {
        sck = -2;
    } else if (args[ARG_sck].u_obj == mp_const_none) {
        sck = -1;
    } else {
        sck = machine_pin_get_id(args[ARG_sck].u_obj);
    }

    if (args[ARG_miso].u_obj == MP_OBJ_NULL) {
        miso = -2;
    } else if (args[ARG_miso].u_obj == mp_const_none) {
        miso = -1;
    } else {
        miso = machine_pin_get_id(args[ARG_miso].u_obj);
    }

    if (args[ARG_mosi].u_obj == MP_OBJ_NULL) {
        mosi = -2;
    } else if (args[ARG_mosi].u_obj == mp_const_none) {
        mosi = -1;
    } else {
        mosi = machine_pin_get_id(args[ARG_mosi].u_obj);
    }

    machine_hw_spi_init_internal(self, args[ARG_id].u_int, args[ARG_baudrate].u_int,
                                 args[ARG_polarity].u_int, args[ARG_phase].u_int, args[ARG_bits].u_int,
                                 args[ARG_firstbit].u_int, sck, mosi, miso);
}

mp_obj_t machine_hw_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT , {.u_int = -1} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 500000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_hw_spi_obj_t *self = m_new_obj(machine_hw_spi_obj_t);
    self->base.type = &machine_hw_spi_type;

    machine_hw_spi_init_internal(
        self,
        args[ARG_id].u_int,
        args[ARG_baudrate].u_int,
        args[ARG_polarity].u_int,
        args[ARG_phase].u_int,
        args[ARG_bits].u_int,
        args[ARG_firstbit].u_int,
        args[ARG_sck].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_sck].u_obj),
        args[ARG_mosi].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_mosi].u_obj),
        args[ARG_miso].u_obj == MP_OBJ_NULL ? -1 : machine_pin_get_id(args[ARG_miso].u_obj));

    return MP_OBJ_FROM_PTR(self);
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
