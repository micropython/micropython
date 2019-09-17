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

#include "wm_include.h"
#include "wm_gpio_afsel.h"
#include "wm_hspi.h"

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"
#include "extmod/machine_spi.h"
#include "modmachine.h"


typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;

    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
    int8_t sck;
    int8_t mosi;
    int8_t miso;
    int8_t cs;

    uint8_t spi_type;/* 0-lspi, 1-hspi */
} machine_spi_obj_t;

STATIC size_t   machine_spi_rx_len = 0;
STATIC uint8_t *machine_spi_rx_buf = NULL;

STATIC s16 machine_spi_rx_cmd_callback(char *buf) {
    memcpy(machine_spi_rx_buf, buf, machine_spi_rx_len);
    return 0;
}

STATIC s16 machine_spi_rx_data_callback(char *buf) {
    memcpy(machine_spi_rx_buf, buf, machine_spi_rx_len);
    return 0;
}

STATIC void w600_spi_set_endian(u8 endian)
{
	u32 reg_val;

	reg_val = tls_reg_read32(HR_SPI_SPICFG_REG);

	if (endian == 0) {
		reg_val &= ~(0x01U << 3);
		reg_val |= SPI_LITTLE_ENDIAN;
	} else if(endian == 1) {
		reg_val &= ~(0x01U << 3);
		reg_val |= SPI_BIG_ENDIAN;
	}

	tls_reg_write32(HR_SPI_SPICFG_REG, reg_val);
}

STATIC u8 w600_spi_write(u8 *data, u32 len)
{
    u32 cnt;
    u32 repeat;
    u32 remain;

    cnt = 0;
    remain = len;

    if(len > SPI_DMA_BUF_MAX_SIZE)
    {
        repeat = len / SPI_DMA_BUF_MAX_SIZE;
        remain = len % SPI_DMA_BUF_MAX_SIZE;

        while(cnt < (repeat * SPI_DMA_BUF_MAX_SIZE))
        {
            tls_spi_write(data + cnt, SPI_DMA_BUF_MAX_SIZE);
            cnt += SPI_DMA_BUF_MAX_SIZE;
        }
    }

    return tls_spi_write(data + cnt, remain);
}

STATIC u8 w600_spi_read(u8 *data, u32 len)
{
    u32 cnt;
    u32 repeat;
    u32 remain;

    cnt = 0;
    remain = len;

    if(len > SPI_DMA_BUF_MAX_SIZE)
    {
        repeat = len / SPI_DMA_BUF_MAX_SIZE;
        remain = len % SPI_DMA_BUF_MAX_SIZE;

        while(cnt < (repeat * SPI_DMA_BUF_MAX_SIZE))
        {
            tls_spi_read(data + cnt, SPI_DMA_BUF_MAX_SIZE);
            cnt += SPI_DMA_BUF_MAX_SIZE;
        }
    }

    return tls_spi_read(data + cnt, remain);
}

STATIC void machine_spi_init_internal(
    machine_spi_obj_t    *self,
    int32_t                 baudrate,
    int8_t                  polarity,
    int8_t                  phase,
    int8_t                  bits,
    int8_t                  firstbit,
    int8_t                  sck,
    int8_t                  mosi,
    int8_t                  miso,
    int8_t                  cs) 
{
    int ret;
    u8 mode;

    self->baudrate = baudrate;
    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
    self->firstbit = firstbit;
    self->sck = sck;
    self->mosi = mosi;
    self->miso = miso;
    self->cs = cs;

    if (0 == self->spi_type) {
        wm_spi_ck_config(sck);
        wm_spi_do_config(mosi);
        wm_spi_di_config(miso);
        wm_spi_cs_config(cs);

        if (-1 == firstbit) {
            w600_spi_set_endian(0);
        } else {
            w600_spi_set_endian(firstbit ? 1 : 0);
        }

        if (-1 == bits) {
            tls_spi_trans_type(SPI_DMA_TRANSFER);
        } else {
            if (8 == bits) {
                tls_spi_trans_type(SPI_BYTE_TRANSFER);
            } else if (32 == bits) {
                tls_spi_trans_type(SPI_WORD_TRANSFER);
            } else {
                tls_spi_trans_type(SPI_DMA_TRANSFER);
            }
        }

        if (0 == polarity || 0 == phase) {
            mode = TLS_SPI_MODE_0;
        } else if (0 == polarity || 1 == phase) {
            mode = TLS_SPI_MODE_2;
        } else if (1 == polarity || 0 == phase) {
            mode = TLS_SPI_MODE_1;
        } else if (1 == polarity || 1 == phase) {
            mode = TLS_SPI_MODE_3;
        }
        ret = tls_spi_setup(mode, TLS_SPI_CS_LOW, baudrate);
    } else {
        if (WM_IO_PB_16 == sck) {
            wm_hspi_gpio_config(0);
        } else if (WM_IO_PB_08 == sck) {
            wm_hspi_gpio_config(1);
        }
        ret = tls_slave_spi_init();
        if (0 == ret) {
            tls_set_high_speed_interface_type(HSPI_INTERFACE_SPI);
            tls_set_hspi_user_mode(1);
            tls_hspi_rx_cmd_callback_register(machine_spi_rx_cmd_callback);
            tls_hspi_rx_data_callback_register(machine_spi_rx_data_callback);
        }
    }
}

STATIC void machine_spi_deinit(mp_obj_base_t *self_in) {
    machine_spi_obj_t *self = (machine_spi_obj_t *) self_in;
}

STATIC void machine_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    int ret = TLS_SPI_STATUS_OK;
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (0 == self->spi_type) {
        if (src && len) {
            ret = w600_spi_write(src, len);
        }
        if (dest && len) {
            ret = w600_spi_read(dest, len);
        }
    } else {
        machine_spi_rx_len = len;
        machine_spi_rx_buf = dest;
        ret = tls_hspi_tx_data((char *)src, len);
        if (len == ret) {
            /* do nothing */
        }
    }
}

/******************************************************************************/
// MicroPython bindings for hw_spi

STATIC void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(id=%u, baudrate=%u, polarity=%u, phase=%u, bits=%u, firstbit=%u, sck=%d, mosi=%d, miso=%d, cs=%d)",
              self->spi_type, self->baudrate, self->polarity,
              self->phase, self->bits, self->firstbit,
              self->sck, self->mosi, self->miso, self->cs);
}

STATIC void machine_spi_init(mp_obj_base_t *self_in, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_spi_obj_t *self = (machine_spi_obj_t *) self_in;

    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso, ARG_cs };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_cs,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);

    if (args[ARG_id].u_int == 1) {
        self->spi_type = 1;
    } else {
        self->spi_type = 0;
    }

    self->base.type = &machine_spi_type;

    int8_t sck, mosi, miso, cs;

    if (args[ARG_sck].u_obj == MP_OBJ_NULL) {
        sck = WM_IO_PB_16;
    } else if (args[ARG_sck].u_obj == mp_const_none) {
        sck = WM_IO_PB_16;
    } else {
        sck = machine_pin_get_id(args[ARG_sck].u_obj);
    }

    if (args[ARG_miso].u_obj == MP_OBJ_NULL) {
        miso = WM_IO_PB_17;
    } else if (args[ARG_miso].u_obj == mp_const_none) {
        miso = WM_IO_PB_17;
    } else {
        miso = machine_pin_get_id(args[ARG_miso].u_obj);
    }

    if (args[ARG_mosi].u_obj == MP_OBJ_NULL) {
        mosi = WM_IO_PB_18;
    } else if (args[ARG_mosi].u_obj == mp_const_none) {
        mosi = WM_IO_PB_18;
    } else {
        mosi = machine_pin_get_id(args[ARG_mosi].u_obj);
    }

    if (args[ARG_cs].u_obj == MP_OBJ_NULL) {
        cs = WM_IO_PB_15;
    } else if (args[ARG_cs].u_obj == mp_const_none) {
        cs = WM_IO_PB_15;
    } else {
        cs = machine_pin_get_id(args[ARG_cs].u_obj);
    }

    machine_spi_init_internal(self, args[ARG_baudrate].u_int,
                              args[ARG_polarity].u_int, args[ARG_phase].u_int, args[ARG_bits].u_int,
                              args[ARG_firstbit].u_int, sck, mosi, miso, cs);
}

mp_obj_t machine_hard_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit, ARG_sck, ARG_mosi, ARG_miso, ARG_cs };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = SPI_DEFAULT_SPEED} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = MICROPY_PY_MACHINE_SPI_MSB} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_cs,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    machine_spi_obj_t *self = m_new_obj(machine_spi_obj_t);
    if (args[ARG_id].u_int == 1) {
        self->spi_type = 1;
    } else {
        self->spi_type = 0;
    }
    self->base.type = &machine_spi_type;

    machine_spi_init_internal(
        self,
        args[ARG_baudrate].u_int,
        args[ARG_polarity].u_int,
        args[ARG_phase].u_int,
        args[ARG_bits].u_int,
        args[ARG_firstbit].u_int,
        args[ARG_sck].u_obj == MP_OBJ_NULL ? WM_IO_PB_16 : machine_pin_get_id(args[ARG_sck].u_obj),
        args[ARG_mosi].u_obj == MP_OBJ_NULL ? WM_IO_PB_18 : machine_pin_get_id(args[ARG_mosi].u_obj),
        args[ARG_miso].u_obj == MP_OBJ_NULL ? WM_IO_PB_17 : machine_pin_get_id(args[ARG_miso].u_obj),
        args[ARG_cs].u_obj == MP_OBJ_NULL ? WM_IO_PB_15 : machine_pin_get_id(args[ARG_cs].u_obj));

    return MP_OBJ_FROM_PTR(self);
}

STATIC const mp_machine_spi_p_t machine_spi_p = {
    .init = machine_spi_init,
    .deinit = machine_spi_deinit,
    .transfer = machine_spi_transfer,
};

const mp_obj_type_t machine_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = machine_spi_print,
    .make_new = machine_hard_spi_make_new,
    .protocol = &machine_spi_p,
    .locals_dict = (mp_obj_dict_t *) &mp_machine_spi_locals_dict,
};
