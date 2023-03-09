/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
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
#include "extmod/machine_spi.h"
#include "bufhelper.h"
#include "spi.h"

/******************************************************************************/
// MicroPython bindings for legacy pyb API

// class pyb.SPI - a controller-driven serial protocol
//
// SPI is a serial protocol that is driven by a controller.  At the physical level
// there are 3 lines: SCK, MOSI, MISO.
//
// See usage model of I2C; SPI is very similar.  Main difference is
// parameters to init the SPI bus:
//
//     from pyb import SPI
//     spi = SPI(1, SPI.CONTROLLER, baudrate=600000, polarity=1, phase=0, crc=0x7)
//
// Only required parameter is mode, SPI.CONTROLLER or SPI.PERIPHERAL.  Polarity can be
// 0 or 1, and is the level the idle clock line sits at.  Phase can be 0 or 1
// to sample data on the first or second clock edge respectively.  Crc can be
// None for no CRC, or a polynomial specifier.
//
// Additional method for SPI:
//
//     data = spi.send_recv(b'1234')        # send 4 bytes and receive 4 bytes
//     buf = bytearray(4)
//     spi.send_recv(b'1234', buf)          # send 4 bytes and receive 4 into buf
//     spi.send_recv(buf, buf)              # send/recv 4 bytes from/to buf

STATIC const pyb_spi_obj_t pyb_spi_obj[] = {
    {{&pyb_spi_type}, &spi_obj[0]},
    {{&pyb_spi_type}, &spi_obj[1]},
    {{&pyb_spi_type}, &spi_obj[2]},
    {{&pyb_spi_type}, &spi_obj[3]},
    {{&pyb_spi_type}, &spi_obj[4]},
    {{&pyb_spi_type}, &spi_obj[5]},
};

STATIC void pyb_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    spi_print(print, self->spi, true);
}

// init(mode, baudrate=328125, *, polarity=1, phase=0, bits=8, firstbit=SPI.MSB, ti=False, crc=None)
//
// Initialise the SPI bus with the given parameters:
//   - `mode` must be either `SPI.CONTROLLER` or `SPI.PERIPHERAL`.
//   - `baudrate` is the SCK clock rate (only sensible for a controller).
STATIC mp_obj_t pyb_spi_init_helper(const pyb_spi_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 328125} },
        { MP_QSTR_prescaler, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 0} },
        { MP_QSTR_dir,      MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = SPI_DIRECTION_2LINES} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 8} },
        { MP_QSTR_nss,      MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = SPI_NSS_SOFT} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = SPI_FIRSTBIT_MSB} },
        { MP_QSTR_ti,       MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_crc,      MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_rom_obj = MP_ROM_NONE} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // set the SPI configuration values
    SPI_InitTypeDef *init = &self->spi->spi->Init;
    init->Mode = args[0].u_int;

    spi_set_params(self->spi, args[2].u_int, args[1].u_int, args[3].u_int, args[4].u_int,
        args[6].u_int, args[8].u_int);

    init->Direction = args[5].u_int;
    init->NSS = args[7].u_int;
    init->TIMode = args[9].u_bool ? SPI_TIMODE_ENABLE : SPI_TIMODE_DISABLE;
    if (args[10].u_obj == mp_const_none) {
        init->CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        init->CRCPolynomial = 0;
    } else {
        init->CRCCalculation = SPI_CRCCALCULATION_ENABLE;
        init->CRCPolynomial = mp_obj_get_int(args[10].u_obj);
    }

    // init the SPI bus
    int ret = spi_init(self->spi, init->NSS != SPI_NSS_SOFT);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }

    return mp_const_none;
}

// constructor(bus, ...)
//
// Construct an SPI object on the given bus.  `bus` can be 1 or 2.
// With no additional parameters, the SPI object is created but not
// initialised (it has the settings from the last initialisation of
// the bus, if any).  If extra arguments are given, the bus is initialised.
// See `init` for parameters of initialisation.
//
// The physical pins of the SPI buses are:
//   - `SPI(1)` is on the X position: `(NSS, SCK, MISO, MOSI) = (X5, X6, X7, X8) = (PA4, PA5, PA6, PA7)`
//   - `SPI(2)` is on the Y position: `(NSS, SCK, MISO, MOSI) = (Y5, Y6, Y7, Y8) = (PB12, PB13, PB14, PB15)`
//
// At the moment, the NSS pin is not used by the SPI driver and is free
// for other use.
STATIC mp_obj_t pyb_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // work out SPI bus
    int spi_id = spi_find_index(args[0]);

    // get SPI object
    const pyb_spi_obj_t *spi_obj = &pyb_spi_obj[spi_id - 1];

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_spi_init_helper(spi_obj, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(spi_obj);
}

STATIC mp_obj_t pyb_spi_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_spi_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_init_obj, 1, pyb_spi_init);

// deinit()
// Turn off the SPI bus.
STATIC mp_obj_t pyb_spi_deinit(mp_obj_t self_in) {
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    spi_deinit(self->spi);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_spi_deinit_obj, pyb_spi_deinit);

// send(send, *, timeout=5000)
// Send data on the bus:
//   - `send` is the data to send (an integer to send, or a buffer object).
//   - `timeout` is the timeout in milliseconds to wait for the send.
//
// Return value: `None`.
STATIC mp_obj_t pyb_spi_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[0].u_obj, &bufinfo, data);

    // send the data
    spi_transfer(self->spi, bufinfo.len, bufinfo.buf, NULL, args[1].u_int);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_send_obj, 1, pyb_spi_send);

// recv(recv, *, timeout=5000)
//
// Receive data on the bus:
//   - `recv` can be an integer, which is the number of bytes to receive,
//     or a mutable buffer, which will be filled with received bytes.
//   - `timeout` is the timeout in milliseconds to wait for the receive.
//
// Return value: if `recv` is an integer then a new buffer of the bytes received,
// otherwise the same buffer that was passed in to `recv`.
STATIC mp_obj_t pyb_spi_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_recv,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to receive into
    vstr_t vstr;
    mp_obj_t o_ret = pyb_buf_get_for_recv(args[0].u_obj, &vstr);

    // receive the data
    spi_transfer(self->spi, vstr.len, NULL, (uint8_t *)vstr.buf, args[1].u_int);

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return o_ret;
    } else {
        return mp_obj_new_bytes_from_vstr(&vstr);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_recv_obj, 1, pyb_spi_recv);

// send_recv(send, recv=None, *, timeout=5000)
//
// Send and receive data on the bus at the same time:
//   - `send` is the data to send (an integer to send, or a buffer object).
//   - `recv` is a mutable buffer which will be filled with received bytes.
//   It can be the same as `send`, or omitted.  If omitted, a new buffer will
//   be created.
//   - `timeout` is the timeout in milliseconds to wait for the receive.
//
// Return value: the buffer with the received bytes.
STATIC mp_obj_t pyb_spi_send_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // TODO assumes transmission size is 8-bits wide

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_send,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_recv,    MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5000} },
    };

    // parse args
    pyb_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get buffers to send from/receive to
    mp_buffer_info_t bufinfo_send;
    uint8_t data_send[1];
    mp_buffer_info_t bufinfo_recv;
    vstr_t vstr_recv;
    mp_obj_t o_ret;

    if (args[0].u_obj == args[1].u_obj) {
        // same object for send and receive, it must be a r/w buffer
        mp_get_buffer_raise(args[0].u_obj, &bufinfo_send, MP_BUFFER_RW);
        bufinfo_recv = bufinfo_send;
        o_ret = args[0].u_obj;
    } else {
        // get the buffer to send from
        pyb_buf_get_for_send(args[0].u_obj, &bufinfo_send, data_send);

        // get the buffer to receive into
        if (args[1].u_obj == MP_OBJ_NULL) {
            // only send argument given, so create a fresh buffer of the send length
            vstr_init_len(&vstr_recv, bufinfo_send.len);
            bufinfo_recv.len = vstr_recv.len;
            bufinfo_recv.buf = vstr_recv.buf;
            o_ret = MP_OBJ_NULL;
        } else {
            // recv argument given
            mp_get_buffer_raise(args[1].u_obj, &bufinfo_recv, MP_BUFFER_WRITE);
            if (bufinfo_recv.len != bufinfo_send.len) {
                mp_raise_ValueError(MP_ERROR_TEXT("recv must be same length as send"));
            }
            o_ret = args[1].u_obj;
        }
    }

    // do the transfer
    spi_transfer(self->spi, bufinfo_send.len, bufinfo_send.buf, bufinfo_recv.buf, args[2].u_int);

    // return the received data
    if (o_ret != MP_OBJ_NULL) {
        return o_ret;
    } else {
        return mp_obj_new_bytes_from_vstr(&vstr_recv);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_send_recv_obj, 1, pyb_spi_send_recv);

STATIC const mp_rom_map_elem_t pyb_spi_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_spi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&pyb_spi_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_machine_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_machine_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_machine_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&mp_machine_spi_write_readinto_obj) },

    // legacy methods
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&pyb_spi_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&pyb_spi_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_recv), MP_ROM_PTR(&pyb_spi_send_recv_obj) },

    // class constants
    /// \constant CONTROLLER - for initialising the bus to controller mode
    /// \constant PERIPHERAL - for initialising the bus to peripheral mode
    /// \constant MSB - set the first bit to MSB
    /// \constant LSB - set the first bit to LSB
    { MP_ROM_QSTR(MP_QSTR_CONTROLLER), MP_ROM_INT(SPI_MODE_MASTER) },
    { MP_ROM_QSTR(MP_QSTR_PERIPHERAL),  MP_ROM_INT(SPI_MODE_SLAVE) },
    // TODO - remove MASTER/SLAVE when CONTROLLER/PERIPHERAL gain wide adoption
    { MP_ROM_QSTR(MP_QSTR_MASTER), MP_ROM_INT(SPI_MODE_MASTER) },
    { MP_ROM_QSTR(MP_QSTR_SLAVE),  MP_ROM_INT(SPI_MODE_SLAVE) },
    { MP_ROM_QSTR(MP_QSTR_MSB),    MP_ROM_INT(SPI_FIRSTBIT_MSB) },
    { MP_ROM_QSTR(MP_QSTR_LSB),    MP_ROM_INT(SPI_FIRSTBIT_LSB) },
    /* TODO
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_2LINES             ((uint32_t)0x00000000)
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_2LINES_RXONLY      SPI_CR1_RXONLY
    { MP_ROM_QSTR(MP_QSTR_DIRECTION_1LINE              SPI_CR1_BIDIMODE
    { MP_ROM_QSTR(MP_QSTR_NSS_SOFT                    SPI_CR1_SSM
    { MP_ROM_QSTR(MP_QSTR_NSS_HARD_INPUT              ((uint32_t)0x00000000)
    { MP_ROM_QSTR(MP_QSTR_NSS_HARD_OUTPUT             ((uint32_t)0x00040000)
    */
};
STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);

STATIC void spi_transfer_machine(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    pyb_spi_obj_t *self = (pyb_spi_obj_t *)self_in;
    spi_transfer(self->spi, len, src, dest, SPI_TRANSFER_TIMEOUT(len));
}

STATIC const mp_machine_spi_p_t pyb_spi_p = {
    .transfer = spi_transfer_machine,
};

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_spi_make_new,
    print, pyb_spi_print,
    protocol, &pyb_spi_p,
    locals_dict, &pyb_spi_locals_dict
    );
