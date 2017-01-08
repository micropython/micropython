/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2016 Glenn Ruben Bakke
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
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/machine_spi.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"
#include "hal_spi.h"

#if MICROPY_PY_MACHINE_SPI

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
///     spi = SPI(1, SPI.MASTER, baudrate=600000, polarity=1, phase=0, crc=0x7)
///
/// Only required parameter is mode, SPI.MASTER or SPI.SLAVE.  Polarity can be
/// 0 or 1, and is the level the idle clock line sits at.  Phase can be 0 or 1
/// to sample data on the first or second clock edge respectively.  Crc can be
/// None for no CRC, or a polynomial specifier.
///
/// Additional method for SPI:
///
///     data = spi.send_recv(b'1234')        # send 4 bytes and receive 4 bytes
///     buf = bytearray(4)
///     spi.send_recv(b'1234', buf)          # send 4 bytes and receive 4 into buf
///     spi.send_recv(buf, buf)              # send/recv 4 bytes from/to buf

#if defined(MICROPY_HW_SPI0_SCK)
SPI_HandleTypeDef SPIHandle0 = {.instance = NULL};
#endif

STATIC const pyb_spi_obj_t machine_spi_obj[] = {
    #if defined(MICROPY_HW_SPI0_SCK)
    {{&machine_hard_spi_type}, &SPIHandle0},
    #else
    {{&machine_hard_spi_type}, NULL},
    #endif
};

void spi_init0(void) {
    // reset the SPI handles
    #if defined(MICROPY_HW_SPI0_SCK)
    memset(&SPIHandle0, 0, sizeof(SPI_HandleTypeDef));
    SPIHandle0.instance = SPI0;
    #endif
}

STATIC int spi_find(mp_obj_t id) {
    if (MP_OBJ_IS_STR(id)) {
        // given a string id
        const char *port = mp_obj_str_get_str(id);
        if (0) {
        #ifdef MICROPY_HW_SPI0_NAME
        } else if (strcmp(port, MICROPY_HW_SPI0_NAME) == 0) {
            return 1;
        #endif
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "SPI(%s) does not exist", port));
    } else {
        // given an integer id
        int spi_id = mp_obj_get_int(id);
        if (spi_id >= 0 && spi_id <= MP_ARRAY_SIZE(machine_spi_obj)
            && machine_spi_obj[spi_id].spi != NULL) {
            return spi_id;
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "SPI(%d) does not exist", spi_id));
    }
}

void spi_init(SPI_HandleTypeDef *spi, bool enable_nss_pin) {
}

void spi_deinit(SPI_HandleTypeDef *spi) {
}

STATIC void spi_transfer(const pyb_spi_obj_t * self, size_t len, const uint8_t * src, uint8_t * dest, uint32_t timeout) {
	hal_spi_master_tx_rx(self->spi->instance, len, src, dest);
}

STATIC void spi_print(const mp_print_t *print, SPI_HandleTypeDef *spi, bool legacy) {
    uint spi_num = 0; // default to SPI1
    mp_printf(print, "SPI(%u)", spi_num);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

// for make_new
enum {
    ARG_NEW_id,
    ARG_NEW_baudrate,
    ARG_NEW_polarity,
    ARG_NEW_phase,
    ARG_NEW_bits,
    ARG_NEW_firstbit,
    ARG_NEW_sck,
    ARG_NEW_mosi,
    ARG_NEW_miso
};

// for init
enum {
    ARG_INIT_baudrate,
    ARG_INIT_polarity,
    ARG_INIT_phase,
    ARG_INIT_bits,
    ARG_INIT_firstbit
};

STATIC mp_obj_t machine_hard_spi_make_new(mp_arg_val_t *args);
STATIC void machine_hard_spi_init(mp_obj_t self, mp_arg_val_t *args);
STATIC void machine_hard_spi_deinit(mp_obj_t self);

/* common code for both soft and hard implementations *************************/

STATIC mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 500000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0 /* SPI_FIRSTBIT_MSB */} },
        { MP_QSTR_sck,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mosi,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_miso,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_NEW_id].u_obj == MP_OBJ_NEW_SMALL_INT(-1)) {
        // TODO: implement soft SPI
        // return machine_soft_spi_make_new(args);
        return mp_const_none;
    } else {
        // hardware peripheral id given
        return machine_hard_spi_make_new(args);
    }
}

STATIC mp_obj_t machine_spi_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // parse args
    mp_obj_t self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // dispatch to specific implementation
    if (mp_obj_get_type(self) == &machine_hard_spi_type) {
        machine_hard_spi_init(self, args);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_init_obj, 1, machine_spi_init);

STATIC mp_obj_t machine_spi_deinit(mp_obj_t self) {
    // dispatch to specific implementation
    if (mp_obj_get_type(self) == &machine_hard_spi_type) {
        machine_hard_spi_deinit(self);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_spi_deinit_obj, machine_spi_deinit);

STATIC const mp_rom_map_elem_t machine_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_spi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_spi_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_machine_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_machine_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_machine_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&mp_machine_spi_write_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_MSB), MP_ROM_INT(0) }, // SPI_FIRSTBIT_MSB
    { MP_ROM_QSTR(MP_QSTR_LSB), MP_ROM_INT(1) }, // SPI_FIRSTBIT_LSB
};

STATIC MP_DEFINE_CONST_DICT(machine_spi_locals_dict, machine_spi_locals_dict_table);

/* code for hard implementation ***********************************************/

STATIC const machine_hard_spi_obj_t machine_hard_spi_obj[] = {
    {{&machine_hard_spi_type}, &machine_spi_obj[0]},
};

STATIC void machine_hard_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_spi_obj_t *self = self_in;
    spi_print(print, self->pyb->spi, false);
}

STATIC mp_obj_t machine_hard_spi_make_new(mp_arg_val_t *args) {
    // get static peripheral object
    int spi_id = spi_find(args[ARG_NEW_id].u_obj);
    const machine_hard_spi_obj_t *self = &machine_hard_spi_obj[spi_id];

    // here we would check the sck/mosi/miso pins and configure them
    if (args[ARG_NEW_sck].u_obj != MP_OBJ_NULL
        && args[ARG_NEW_mosi].u_obj != MP_OBJ_NULL
        && args[ARG_NEW_miso].u_obj != MP_OBJ_NULL) {

        self->pyb->spi->init.clk_pin = mp_obj_get_int(args[ARG_NEW_sck].u_obj);
        self->pyb->spi->init.mosi_pin = mp_obj_get_int(args[ARG_NEW_mosi].u_obj);
        self->pyb->spi->init.miso_pin = mp_obj_get_int(args[ARG_NEW_miso].u_obj);
    } else {
        self->pyb->spi->init.clk_pin = MICROPY_HW_SPI0_SCK;
        self->pyb->spi->init.mosi_pin = MICROPY_HW_SPI0_MOSI;
        self->pyb->spi->init.miso_pin = MICROPY_HW_SPI0_MISO;
        self->pyb->spi->init.clk_pin_port = MICROPY_HW_SPI0_SCK_PORT;
        self->pyb->spi->init.mosi_pin_port = MICROPY_HW_SPI0_MOSI_PORT;
        self->pyb->spi->init.miso_pin_port = MICROPY_HW_SPI0_MISO_PORT;
    }

    int baudrate = args[ARG_NEW_baudrate].u_int;

    if (baudrate <= 125000) {
        self->pyb->spi->init.freq = HAL_FREQ_125_Kbps;
    } else if (baudrate <= 250000) {
        self->pyb->spi->init.freq = HAL_FREQ_250_Kbps;
    } else if (baudrate <= 500000) {
        self->pyb->spi->init.freq = HAL_FREQ_500_Kbps;
    } else if (baudrate <= 1000000) {
        self->pyb->spi->init.freq = HAL_FREQ_1_Mbps;
    } else if (baudrate <= 2000000) {
        self->pyb->spi->init.freq = HAL_FREQ_2_Mbps;
    } else if (baudrate <= 4000000) {
        self->pyb->spi->init.freq = HAL_FREQ_4_Mbps;
    } else {
        self->pyb->spi->init.freq = HAL_FREQ_8_Mbps;
    }

    self->pyb->spi->init.irq_priority = 4;
    self->pyb->spi->init.mode = HAL_SPI_MODE_CPOL0_CPHA0;
    self->pyb->spi->init.lsb_first = false;
    hal_spi_master_init(self->pyb->spi->instance, &self->pyb->spi->init);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_hard_spi_init(mp_obj_t self_in, mp_arg_val_t *args) {
}

STATIC void machine_hard_spi_deinit(mp_obj_t self_in) {
    machine_hard_spi_obj_t *self = self_in;
    spi_deinit(self->pyb->spi);
}

STATIC void machine_hard_spi_transfer(mp_obj_base_t *self_in, size_t len, const uint8_t *src, uint8_t *dest) {
    machine_hard_spi_obj_t *self = (machine_hard_spi_obj_t*)self_in;
    spi_transfer(self->pyb, len, src, dest, 100);
}

STATIC const mp_machine_spi_p_t machine_hard_spi_p = {
    .transfer = machine_hard_spi_transfer,
};

const mp_obj_type_t machine_hard_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .print = machine_hard_spi_print,
    .make_new = machine_spi_make_new,
    .protocol = &machine_hard_spi_p,
    .locals_dict = (mp_obj_t)&machine_spi_locals_dict,
};

#endif // MICROPY_PY_MACHINE_SPI
