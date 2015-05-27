/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include <stdint.h>
#include <string.h>

#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "py/runtime.h"
#include "bufhelper.h"
#include "inc/hw_types.h"
#include "inc/hw_mcspi.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "prcm.h"
#include "spi.h"
#include "pybspi.h"
#include "mpexception.h"
#include "pybsleep.h"

/// \moduleref pyb
/// \class SPI - a master-driven serial protocol
///
/// SPI is a serial protocol that is driven by a master.  At the physical level
/// there are 3 lines: SCK, MOSI, MISO.
///
/// See usage model of I2C; SPI is very similar.  Main difference is
/// parameters to init the SPI bus:
///
///     from pyb import SPI
///     spi = SPI(1, SPI.MASTER, baudrate=2000000, bits=8, polarity=0, phase=0, nss=SPI.ACTIVE_LOW)
///
/// Only required parameter is the baudrate, in Hz. polarity and phase may be 0 or 1.
/// Bit accepts 8, 16, 32. Chip select values are ACTIVE_LOW and ACTIVE_HIGH
///
/// Additional method for SPI:
///
///     data = spi.send_recv(b'1234')        # send 4 bytes and receive 4 bytes
///     buf = bytearray(4)
///     spi.send_recv(b'1234', buf)          # send 4 bytes and receive 4 into buf
///     spi.send_recv(buf, buf)              # send/recv 4 bytes from/to buf

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct _pyb_spi_obj_t {
    mp_obj_base_t base;
    uint baudrate;
    uint config;
    vstr_t tx_vstr;
    vstr_t rx_vstr;
    uint tx_index;
    uint rx_index;
    byte polarity;
    byte phase;
    byte submode;
    byte wlen;
} pyb_spi_obj_t;

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define PYBSPI_DEF_BAUDRATE                     1000000     // 1MHz

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_spi_obj_t pyb_spi_obj = {.baudrate = 0};

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/
// only master mode is available for the moment
STATIC void pybspi_init (const pyb_spi_obj_t *self) {
    // enable the peripheral clock
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_GSPI);
    MAP_SPIReset(GSPI_BASE);

    // configure the interface (only master mode supported)
    MAP_SPIConfigSetExpClk (GSPI_BASE, MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                            self->baudrate, SPI_MODE_MASTER, self->submode, self->config);

    // enable the interface
    MAP_SPIEnable(GSPI_BASE);
}

STATIC void pybspi_tx (pyb_spi_obj_t *self, const void *data) {
    uint32_t txdata = 0xFFFFFFFF;
    if (data) {
        switch (self->wlen) {
        case 1:
            txdata = (uint8_t)(*(char *)data);
            break;
        case 2:
            txdata = (uint16_t)(*(uint16_t *)data);
            break;
        case 4:
            txdata = (uint32_t)(*(uint32_t *)data);
            break;
        default:
            return;
        }
    }
    MAP_SPIDataPut (GSPI_BASE, txdata);
}

STATIC void pybspi_rx (pyb_spi_obj_t *self, void *data) {
    uint32_t rxdata;
    MAP_SPIDataGet (GSPI_BASE, &rxdata);
    if (data) {
        switch (self->wlen) {
        case 1:
            *(char *)data = rxdata;
            break;
        case 2:
            *(uint16_t *)data = rxdata;
            break;
        case 4:
            *(uint32_t *)data = rxdata;
            break;
        default:
            return;
        }
    }
}

STATIC void pybspi_transfer (pyb_spi_obj_t *self, const char *txdata, char *rxdata, uint32_t len) {
    // send and receive the data
    MAP_SPICSEnable(GSPI_BASE);
    for (int i = 0; i < len / self->wlen; i += self->wlen) {
        pybspi_tx(self, txdata ? (const void *)&txdata[i] : NULL);
        pybspi_rx(self, rxdata ? (void *)&rxdata[i] : NULL);
    }
    MAP_SPICSDisable(GSPI_BASE);
}

/******************************************************************************/
/* Micro Python bindings                                                      */
/******************************************************************************/
STATIC void pyb_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_spi_obj_t *self = self_in;

    if (self->baudrate > 0) {
        mp_printf(print, "<SPI1, SPI.MASTER, baudrate=%u, bits=%u, polarity=%u, phase=%u, nss=%q>",
                  self->baudrate, (self->wlen * 8), self->polarity, self->phase,
                  (self->config & SPI_CS_ACTIVELOW) ? MP_QSTR_ACTIVE_LOW : MP_QSTR_ACTIVE_HIGH);
    }
    else {
        mp_print_str(print, "<SPI1>");
    }
}

/// \method init(mode, *, baudrate=1000000, bits=8, polarity=0, phase=0, nss=SPI.ACTIVELOW)
///
/// Initialise the SPI bus with the given parameters:
///
///   - `mode` must be MASTER.
///   - `baudrate` is the SCK clock rate.
///   - `bits` is the transfer width size (8, 16, 32).
///   - `polarity` (0, 1).
///   - `phase` (0, 1).
///   - `nss` can be ACTIVE_LOW or ACTIVE_HIGH.
static const mp_arg_t pybspi_init_args[] = {
    { MP_QSTR_mode,         MP_ARG_REQUIRED | MP_ARG_INT,  },
    { MP_QSTR_baudrate,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = PYBSPI_DEF_BAUDRATE} },
    { MP_QSTR_bits,         MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 8} },
    { MP_QSTR_polarity,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 0} },
    { MP_QSTR_phase,        MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 0} },
    { MP_QSTR_nss,          MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = SPI_CS_ACTIVELOW} },
};

STATIC mp_obj_t pyb_spi_init_helper(pyb_spi_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pybspi_init_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(pybspi_init_args), pybspi_init_args, args);

    // verify that mode is master
    if (args[0].u_int != SPI_MODE_MASTER) {
        goto invalid_args;
    }

    uint bits;
    switch (args[2].u_int) {
    case 8:
        bits = SPI_WL_8;
        break;
    case 16:
        bits = SPI_WL_16;
        break;
    case 32:
        bits = SPI_WL_32;
        break;
    default:
        goto invalid_args;
        break;
    }

    uint polarity = args[3].u_int;
    uint phase = args[4].u_int;
    if (polarity > 1 || phase > 1) {
        goto invalid_args;
    }

    uint nss = args[5].u_int;
    if (nss != SPI_CS_ACTIVELOW && nss != SPI_CS_ACTIVEHIGH) {
        goto invalid_args;
    }

    // build the configuration
    self->baudrate = args[1].u_int;
    self->wlen = args[2].u_int >> 3;
    self->config = bits | nss | SPI_SW_CTRL_CS | SPI_4PIN_MODE | SPI_TURBO_OFF;
    self->polarity = polarity;
    self->phase = phase;
    self->submode = (polarity << 1) | phase;

    // init the bus
    pybspi_init((const pyb_spi_obj_t *)self);

    // register it with the sleep module
    pybsleep_add((const mp_obj_t)self, (WakeUpCB_t)pybspi_init);

    return mp_const_none;

invalid_args:
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
}

/// \classmethod \constructor(bus, ...)
///
/// Construct an SPI object with the given baudrate. Bus can only be 1.
/// With no extra parameters, the SPI object is created but not
/// initialised (it has the settings from the last initialisation of
/// the bus, if any).  If extra arguments are given, the bus is initialised.
/// See `init` for parameters of initialisation.
///
STATIC mp_obj_t pyb_spi_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    pyb_spi_obj_t *self = &pyb_spi_obj;
    self->base.type = &pyb_spi_type;

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_spi_init_helper(self, n_args - 1, args + 1, &kw_args);
    }

    return self;
}

STATIC mp_obj_t pyb_spi_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_spi_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_init_obj, 1, pyb_spi_init);

/// \method deinit()
/// Turn off the spi bus.
STATIC mp_obj_t pyb_spi_deinit(mp_obj_t self_in) {
    // disable the peripheral
    MAP_SPIDisable(GSPI_BASE);
    MAP_PRCMPeripheralClkDisable(PRCM_GSPI, PRCM_RUN_MODE_CLK | PRCM_SLP_MODE_CLK);
    // invalidate the baudrate
    pyb_spi_obj.baudrate = 0;
    // unregister it with the sleep module
    pybsleep_remove((const mp_obj_t)self_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_spi_deinit_obj, pyb_spi_deinit);

/// \method send(send)
/// Send data on the bus:
///
/// - `send` is the data to send (a byte to send, or a buffer object).
///
STATIC mp_obj_t pyb_spi_send (mp_obj_t self_in, mp_obj_t send_o) {
    pyb_spi_obj_t *self = self_in;
    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(send_o, &bufinfo, data);

    // just send
    pybspi_transfer(self, (const char *)bufinfo.buf, NULL, bufinfo.len);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_spi_send_obj, pyb_spi_send);

/// \method recv(recv)
///
/// Receive data on the bus:
///
///   - `recv` can be an integer, which is the number of bytes to receive,
///      or a mutable buffer, which will be filled with received bytes.
///
/// Return: if `recv` is an integer then a new buffer of the bytes received,
/// otherwise the same buffer that was passed in to `recv`.
STATIC mp_obj_t pyb_spi_recv(mp_obj_t self_in, mp_obj_t recv_o) {
    pyb_spi_obj_t *self = self_in;
    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(recv_o, &vstr);

    // just receive
    pybspi_transfer(self, NULL, vstr.buf, vstr.len);

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return o_ret;
    } else {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_spi_recv_obj, pyb_spi_recv);

/// \method send_recv(send, recv)
///
/// Send and receive data on the bus at the same time:
///
///   - `send` is the data to send (an integer to send, or a buffer object).
///   - `recv` is a mutable buffer which will be filled with received bytes.
///      It can be the same as `send`, or omitted.  If omitted, a new buffer will
///      be created.
///
/// Return: the buffer with the received bytes.
STATIC mp_obj_t pyb_spi_send_recv (mp_uint_t n_args, const mp_obj_t *args) {
    pyb_spi_obj_t *self = args[0];

    // get buffers to send from/receive to
    mp_buffer_info_t bufinfo_send;
    uint8_t data_send[1];
    mp_buffer_info_t bufinfo_recv;
    vstr_t vstr_recv;
    mp_obj_t o_ret;

    if (args[1] == args[2]) {
        // same object for sending and receiving, it must be a r/w buffer
        mp_get_buffer_raise(args[1], &bufinfo_send, MP_BUFFER_RW);
        bufinfo_recv = bufinfo_send;
        o_ret = args[1];
    } else {
        // get the buffer to send from
        pyb_buf_get_for_send(args[1], &bufinfo_send, data_send);

        // get the buffer to receive into
        if (n_args == 2) {
            // only the send was argument given, so create a fresh buffer of the send length
            vstr_init_len(&vstr_recv, bufinfo_send.len);
            bufinfo_recv.len = vstr_recv.len;
            bufinfo_recv.buf = vstr_recv.buf;
            o_ret = MP_OBJ_NULL;
        }
        else {
            // recv argument given
            mp_get_buffer_raise(args[2], &bufinfo_recv, MP_BUFFER_WRITE);
            if (bufinfo_recv.len != bufinfo_send.len) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
            }
            o_ret = args[2];
        }
    }

    // send and receive
    pybspi_transfer(self, (const char *)bufinfo_send.buf, vstr_recv.buf, bufinfo_send.len);

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return o_ret;
    } else {
        return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr_recv);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_spi_send_recv_obj, 2, 3, pyb_spi_send_recv);

STATIC const mp_map_elem_t pyb_spi_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),                (mp_obj_t)&pyb_spi_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),              (mp_obj_t)&pyb_spi_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send),                (mp_obj_t)&pyb_spi_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv),                (mp_obj_t)&pyb_spi_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send_recv),           (mp_obj_t)&pyb_spi_send_recv_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER),              MP_OBJ_NEW_SMALL_INT(SPI_MODE_MASTER) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ACTIVE_LOW),          MP_OBJ_NEW_SMALL_INT(SPI_CS_ACTIVELOW) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ACTIVE_HIGH),         MP_OBJ_NEW_SMALL_INT(SPI_CS_ACTIVEHIGH) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);

const mp_obj_type_t pyb_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = pyb_spi_print,
    .make_new = pyb_spi_make_new,
    .locals_dict = (mp_obj_t)&pyb_spi_locals_dict,
};
