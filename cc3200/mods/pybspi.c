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
#include "pybpin.h"
#include "pins.h"

/// \moduleref pyb
/// \class SPI - a master-driven serial protocol

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct _pyb_spi_obj_t {
    mp_obj_base_t base;
    uint baudrate;
    uint config;
    byte polarity;
    byte phase;
    byte submode;
    byte wlen;
} pyb_spi_obj_t;

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define PYBSPI_FIRST_BIT_MSB                    0

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC pyb_spi_obj_t pyb_spi_obj = {.baudrate = 0};

STATIC const mp_obj_t pyb_spi_def_pin[3] = {&pin_GP14, &pin_GP16, &pin_GP30};

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
    uint32_t txdata;
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

STATIC void pybspi_transfer (pyb_spi_obj_t *self, const char *txdata, char *rxdata, uint32_t len, uint32_t *txchar) {
    if (!self->baudrate) {
        mp_raise_msg(&mp_type_OSError, mpexception_os_request_not_possible);
    }
    // send and receive the data
    MAP_SPICSEnable(GSPI_BASE);
    for (int i = 0; i < len; i += self->wlen) {
        pybspi_tx(self, txdata ? (const void *)&txdata[i] : txchar);
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
        mp_printf(print, "SPI(0, SPI.MASTER, baudrate=%u, bits=%u, polarity=%u, phase=%u, firstbit=SPI.MSB)",
                  self->baudrate, (self->wlen * 8), self->polarity, self->phase);
    } else {
        mp_print_str(print, "SPI(0)");
    }
}

STATIC mp_obj_t pyb_spi_init_helper(pyb_spi_obj_t *self, const mp_arg_val_t *args) {
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

    uint firstbit = args[5].u_int;
    if (firstbit != PYBSPI_FIRST_BIT_MSB) {
        goto invalid_args;
    }

    // build the configuration
    self->baudrate = args[1].u_int;
    self->wlen = args[2].u_int >> 3;
    self->config = bits | SPI_CS_ACTIVELOW | SPI_SW_CTRL_CS | SPI_4PIN_MODE | SPI_TURBO_OFF;
    self->polarity = polarity;
    self->phase = phase;
    self->submode = (polarity << 1) | phase;

    // assign the pins
    mp_obj_t pins_o = args[6].u_obj;
    if (pins_o != mp_const_none) {
        mp_obj_t *pins;
        if (pins_o == MP_OBJ_NULL) {
            // use the default pins
            pins = (mp_obj_t *)pyb_spi_def_pin;
        } else {
            mp_obj_get_array_fixed_n(pins_o, 3, &pins);
        }
        pin_assign_pins_af (pins, 3, PIN_TYPE_STD_PU, PIN_FN_SPI, 0);
    }

    // init the bus
    pybspi_init((const pyb_spi_obj_t *)self);

    // register it with the sleep module
    pyb_sleep_add((const mp_obj_t)self, (WakeUpCB_t)pybspi_init);

    return mp_const_none;

invalid_args:
    mp_raise_ValueError(mpexception_value_invalid_arguments);
}

static const mp_arg_t pyb_spi_init_args[] = {
    { MP_QSTR_id,                             MP_ARG_INT,  {.u_int = 0} },
    { MP_QSTR_mode,                           MP_ARG_INT,  {.u_int = SPI_MODE_MASTER} },
    { MP_QSTR_baudrate,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 1000000} },    // 1MHz
    { MP_QSTR_bits,         MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 8} },
    { MP_QSTR_polarity,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 0} },
    { MP_QSTR_phase,        MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 0} },
    { MP_QSTR_firstbit,     MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = PYBSPI_FIRST_BIT_MSB} },
    { MP_QSTR_pins,         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = MP_OBJ_NULL} },
};
STATIC mp_obj_t pyb_spi_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_spi_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), pyb_spi_init_args, args);

    // check the peripheral id
    if (args[0].u_int != 0) {
        mp_raise_msg(&mp_type_OSError, mpexception_os_resource_not_avaliable);
    }

    // setup the object
    pyb_spi_obj_t *self = &pyb_spi_obj;
    self->base.type = &pyb_spi_type;

    // start the peripheral
    pyb_spi_init_helper(self, &args[1]);

    return self;
}

STATIC mp_obj_t pyb_spi_init(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_spi_init_args) - 1];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), &pyb_spi_init_args[1], args);
    return pyb_spi_init_helper(pos_args[0], args);
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
    pyb_sleep_remove((const mp_obj_t)self_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_spi_deinit_obj, pyb_spi_deinit);

STATIC mp_obj_t pyb_spi_write (mp_obj_t self_in, mp_obj_t buf) {
    // parse args
    pyb_spi_obj_t *self = self_in;

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(buf, &bufinfo, data);

    // just send
    pybspi_transfer(self, (const char *)bufinfo.buf, NULL, bufinfo.len, NULL);

    // return the number of bytes written
    return mp_obj_new_int(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_spi_write_obj, pyb_spi_write);

STATIC mp_obj_t pyb_spi_read(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_nbytes,    MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_write,     MP_ARG_INT, {.u_int = 0x00} },
    };

    // parse args
    pyb_spi_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    // get the buffer to receive into
    vstr_t vstr;
    pyb_buf_get_for_recv(args[0].u_obj, &vstr);

    // just receive
    uint32_t write = args[1].u_int;
    pybspi_transfer(self, NULL, vstr.buf, vstr.len, &write);

    // return the received data
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_read_obj, 1, pyb_spi_read);

STATIC mp_obj_t pyb_spi_readinto(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf,       MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_write,     MP_ARG_INT, {.u_int = 0x00} },
    };

    // parse args
    pyb_spi_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    // get the buffer to receive into
    vstr_t vstr;
    pyb_buf_get_for_recv(args[0].u_obj, &vstr);

    // just receive
    uint32_t write = args[1].u_int;
    pybspi_transfer(self, NULL, vstr.buf, vstr.len, &write);

    // return the number of bytes received
    return mp_obj_new_int(vstr.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_spi_readinto_obj, 1, pyb_spi_readinto);

STATIC mp_obj_t pyb_spi_write_readinto (mp_obj_t self, mp_obj_t writebuf, mp_obj_t readbuf) {
    // get buffers to write from/read to
    mp_buffer_info_t bufinfo_write;
    uint8_t data_send[1];
    mp_buffer_info_t bufinfo_read;

    if (writebuf == readbuf) {
        // same object for writing and reading, it must be a r/w buffer
        mp_get_buffer_raise(writebuf, &bufinfo_write, MP_BUFFER_RW);
        bufinfo_read = bufinfo_write;
    } else {
        // get the buffer to write from
        pyb_buf_get_for_send(writebuf, &bufinfo_write, data_send);

        // get the read buffer
        mp_get_buffer_raise(readbuf, &bufinfo_read, MP_BUFFER_WRITE);
        if (bufinfo_read.len != bufinfo_write.len) {
            mp_raise_ValueError(mpexception_value_invalid_arguments);
        }
    }

    // send and receive
    pybspi_transfer(self, (const char *)bufinfo_write.buf, bufinfo_read.buf, bufinfo_write.len, NULL);

    // return the number of transferred bytes
    return mp_obj_new_int(bufinfo_write.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_spi_write_readinto_obj, pyb_spi_write_readinto);

STATIC const mp_map_elem_t pyb_spi_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),                (mp_obj_t)&pyb_spi_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit),              (mp_obj_t)&pyb_spi_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),               (mp_obj_t)&pyb_spi_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),                (mp_obj_t)&pyb_spi_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto),            (mp_obj_t)&pyb_spi_readinto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_readinto),      (mp_obj_t)&pyb_spi_write_readinto_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER),              MP_OBJ_NEW_SMALL_INT(SPI_MODE_MASTER) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MSB),                 MP_OBJ_NEW_SMALL_INT(PYBSPI_FIRST_BIT_MSB) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_spi_locals_dict, pyb_spi_locals_dict_table);

const mp_obj_type_t pyb_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = pyb_spi_print,
    .make_new = pyb_spi_make_new,
    .locals_dict = (mp_obj_t)&pyb_spi_locals_dict,
};
