/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

// This file contains all of the Python API definitions for the
// bitbangio.SPI class.

#include <string.h>

#include "shared-bindings/bitbangio/SPI.h"

#include "py/runtime.h"

//| .. currentmodule:: bitbangio
//|
//| :class:`SPI` -- a 3-4 wire serial protocol
//| -----------------------------------------------
//|
//| SPI is a serial protocol that has exclusive pins for data in and out of the
//| master.  It is typically faster than :py:class:`~bitbangio.I2C` because a
//| separate pin is used to control the active slave rather than a transmitted
//| address. This class only manages three of the four SPI lines: `!clock`,
//| `!MOSI`, `!MISO`. Its up to the client to manage the appropriate slave
//| select line. (This is common because multiple slaves can share the `!clock`,
//| `!MOSI` and `!MISO` lines and therefore the hardware.)
//|
//| .. class:: SPI(clock, MOSI, MISO, baudrate=1000000)
//|
//|    Construct an SPI object on the given pins.
//|
//|   :param ~microcontroller.Pin clock: the pin to use for the clock.
//|   :param ~microcontroller.Pin MOSI: the Master Out Slave In pin.
//|   :param ~microcontroller.Pin MISO: the Master In Slave Out pin.
//|   :param int baudrate: is the SCK clock rate.
//|

// TODO(tannewt): Support LSB SPI.
// TODO(tannewt): Support phase, polarity and bit order.
STATIC mp_obj_t bitbangio_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    bitbangio_spi_obj_t *self = m_new_obj(bitbangio_spi_obj_t);
    self->base.type = &bitbangio_spi_type;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_clock, ARG_MOSI, ARG_MISO, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit };
    static const mp_arg_t allowed_args[] = {
       { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ },
       { MP_QSTR_MOSI, MP_ARG_REQUIRED | MP_ARG_OBJ },
       { MP_QSTR_MISO, MP_ARG_REQUIRED | MP_ARG_OBJ },
       { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
       { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
       { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
       { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    const mcu_pin_obj_t* clock = MP_OBJ_TO_PTR(args[ARG_clock].u_obj);
     const mcu_pin_obj_t* mosi = MP_OBJ_TO_PTR(args[ARG_MOSI].u_obj);
    const mcu_pin_obj_t* miso = MP_OBJ_TO_PTR(args[ARG_MISO].u_obj);
    shared_module_bitbangio_spi_construct(self, clock, mosi, miso, args[ARG_baudrate].u_int);
    return (mp_obj_t)self;
}

//|   .. method:: SPI.deinit()
//|
//|      Turn off the SPI bus.
//|
STATIC mp_obj_t bitbangio_spi_obj_deinit(mp_obj_t self_in) {
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    shared_module_bitbangio_spi_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_spi_deinit_obj, bitbangio_spi_obj_deinit);

//|   .. method:: SPI.__enter__()
//|
//|      No-op used by Context Managers.
//|
STATIC mp_obj_t bitbangio_spi_obj___enter__(mp_obj_t self_in) {
    return self_in;
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_spi___enter___obj, bitbangio_spi_obj___enter__);

//|   .. method:: SPI.__exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context.
//|
STATIC mp_obj_t bitbangio_spi_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    shared_module_bitbangio_spi_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitbangio_spi_obj___exit___obj, 4, 4, bitbangio_spi_obj___exit__);

//|   .. method:: SPI.transfer(write_buffer=None, read_buffer=None, address=0)
//|
//|     Write out ``write_buffer`` and then read into ``read_buffer``. They do
//|     not need to be the same length. If either buffer is omitted then the
//|     transfer skips the corresponding portion.
//|
//|     ``address`` is taken for I2C compatibility but is ignored.
//|
//|     When writing, data received is dropped. When reading, zeroes are written
//|     out.
//|
STATIC mp_obj_t bitbangio_spi_transfer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_write_buffer, ARG_read_buffer, ARG_address };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_write_buffer,   MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL } },
        { MP_QSTR_read_buffer,    MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL } },
        { MP_QSTR_address,        MP_ARG_INT, {.u_int = 0} },
    };
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to store data into
    mp_buffer_info_t write_bufinfo;
    if (!mp_get_buffer(args[ARG_write_buffer].u_obj, &write_bufinfo, MP_BUFFER_READ)) {
        write_bufinfo.len = 0;
    }

    mp_buffer_info_t read_bufinfo;
    if (!mp_get_buffer(args[ARG_read_buffer].u_obj, &read_bufinfo, MP_BUFFER_WRITE)) {
        read_bufinfo.len = 0;
    }

    if (write_bufinfo.len == 0 && read_bufinfo.len == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "At least one buffer should be provided."));
    }

    // do the transfer
    bool ok = shared_module_bitbangio_spi_transfer(self, write_bufinfo.buf,
        write_bufinfo.len, read_bufinfo.buf, read_bufinfo.len);
    if (!ok) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitbangio_spi_transfer_obj, 2, bitbangio_spi_transfer);

STATIC const mp_rom_map_elem_t bitbangio_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&bitbangio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&bitbangio_spi___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&bitbangio_spi_obj___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_transfer), MP_ROM_PTR(&bitbangio_spi_transfer_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bitbangio_spi_locals_dict, bitbangio_spi_locals_dict_table);

const mp_obj_type_t bitbangio_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .make_new = bitbangio_spi_make_new,
    .locals_dict = (mp_obj_dict_t*)&bitbangio_spi_locals_dict,
};
