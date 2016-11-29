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
// bitbangio.I2C class.

#include "shared-bindings/bitbangio/I2C.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "py/runtime.h"
//| .. currentmodule:: bitbangio
//|
//| :class:`I2C` --- Two wire serial protocol
//| ------------------------------------------
//|
//| .. class:: I2C(scl, sda, \*, freq=400000)
//|
//|   I2C is a two-wire protocol for communicating between devices.  At the
//|   physical level it consists of 2 wires: SCL and SDA, the clock and data
//|   lines respectively.
//|
//|   :param ~microcontroller.Pin scl: The clock pin
//|   :param ~microcontroller.Pin sda: The data pin
//|   :param int freq: The clock frequency
//|
STATIC mp_obj_t bitbangio_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    bitbangio_i2c_obj_t *self = m_new_obj(bitbangio_i2c_obj_t);
    self->base.type = &bitbangio_i2c_type;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_scl, ARG_sda, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    assert_pin(args[ARG_scl].u_obj, false);
    assert_pin(args[ARG_sda].u_obj, false);
    const mcu_pin_obj_t* scl = MP_OBJ_TO_PTR(args[ARG_scl].u_obj);
    const mcu_pin_obj_t* sda = MP_OBJ_TO_PTR(args[ARG_sda].u_obj);
    shared_module_bitbangio_i2c_construct(self, scl, sda, args[ARG_freq].u_int);
    return (mp_obj_t)self;
}

//|   .. method:: I2C.deinit()
//|
//|     Releases control of the underlying hardware so other classes can use it.
//|
STATIC mp_obj_t bitbangio_i2c_obj_deinit(mp_obj_t self_in) {
    bitbangio_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    shared_module_bitbangio_i2c_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_i2c_deinit_obj, bitbangio_i2c_obj_deinit);

//|   .. method:: I2C.__enter__()
//|
//|     No-op used in Context Managers.
//|
STATIC mp_obj_t bitbangio_i2c_obj___enter__(mp_obj_t self_in) {
    return self_in;
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_i2c___enter___obj, bitbangio_i2c_obj___enter__);

//|   .. method:: I2C.__exit__()
//|
//|     Automatically deinitializes the hardware on context exit.
//|
STATIC mp_obj_t bitbangio_i2c_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    shared_module_bitbangio_i2c_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitbangio_i2c_obj___exit___obj, 4, 4, bitbangio_i2c_obj___exit__);

//|   .. method:: I2C.scan()
//|
//|      Scan all I2C addresses between 0x08 and 0x77 inclusive and return a list of
//|      those that respond.  A device responds if it pulls the SDA line low after
//|      its address (including a read bit) is sent on the bus.
//|
STATIC mp_obj_t bitbangio_i2c_scan(mp_obj_t self_in) {
    bitbangio_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t list = mp_obj_new_list(0, NULL);
    // 7-bit addresses 0b0000xxx and 0b1111xxx are reserved
    for (int addr = 0x08; addr < 0x78; ++addr) {
        bool success = shared_module_bitbangio_i2c_probe(self, addr);
        if (success) {
           mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
        }
    }
    return list;
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_i2c_scan_obj, bitbangio_i2c_scan);

//|   .. method:: I2C.writeto(address, buffer, stop=True)
//|
//|      Write the bytes from ``buffer`` to the slave specified by ``address``.
//|      Transmits a stop bit if ``stop`` is set.
//|
STATIC mp_obj_t bitbangio_i2c_writeto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_address, ARG_buffer, ARG_stop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop,       MP_ARG_BOOL, {.u_bool = true} },
    };
    bitbangio_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to write the data from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    // do the transfer
    bool ok = shared_module_bitbangio_i2c_write(self, args[ARG_address].u_int,
        bufinfo.buf, bufinfo.len, args[ARG_stop].u_bool);
    if (!ok) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(bitbangio_i2c_writeto_obj, 1, bitbangio_i2c_writeto);

//|   .. method:: I2C.readfrom_into(address, buffer)
//|
//|      Read into ``buffer`` from the slave specified by ``address``.
//|      The number of bytes read will be the length of `buf`.
//|
STATIC mp_obj_t bitbangio_i2c_readfrom_into(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t buf_in) {
    bitbangio_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    shared_module_bitbangio_i2c_read(self, mp_obj_get_int(addr_in), (uint8_t*)bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(bitbangio_i2c_readfrom_into_obj, bitbangio_i2c_readfrom_into);

STATIC const mp_rom_map_elem_t bitbangio_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&bitbangio_i2c_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&bitbangio_i2c___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&bitbangio_i2c_obj___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&bitbangio_i2c_scan_obj) },

    // standard bus operations
    { MP_ROM_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&bitbangio_i2c_writeto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_into), MP_ROM_PTR(&bitbangio_i2c_readfrom_into_obj) },
};

STATIC MP_DEFINE_CONST_DICT(bitbangio_i2c_locals_dict, bitbangio_i2c_locals_dict_table);

const mp_obj_type_t bitbangio_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .make_new = bitbangio_i2c_make_new,
    .locals_dict = (mp_obj_dict_t*)&bitbangio_i2c_locals_dict,
};
