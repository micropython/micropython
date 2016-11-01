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

// This file contains all of the Python API definitions for the machine module.
// Machine is the HAL for low-level, hardware accelerated functions. It is not
// meant to simplify APIs, its only meant to unify them so that other modules
// do not require port specific logic.

#include "machine.h"

#include "py/runtime.h"

//| :mod:`machine` --- functions related to the board
//| =================================================
//|
//| .. module:: machine
//|   :synopsis: functions related to the board
//|   :platform: SAMD21
//|
//| The ``machine`` module contains specific functions related to the board.
//|
//| This is soon to be renamed to distinguish it from upstream's `machine`!
//|
//| :class:`I2C` --- Two wire serial protocol
//| ------------------------------------------
//|
//| .. class:: I2C(scl, sda, \*, freq=400000)
//|
//|   I2C is a two-wire protocol for communicating between devices.  At the
//|   physical level it consists of 2 wires: SCL and SDA, the clock and data lines
//|   respectively.
//|
//|   I2C objects are created attached to a specific bus.  They can be initialised
//|   when created, or initialised later on.
//|
//|   :param str scl: The clock pin
//|   :param str sda: The data pin
//|   :param int freq: The clock frequency
//|
STATIC mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
   mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
   machine_i2c_obj_t *self = m_new_obj(machine_i2c_obj_t);
   self->base.type = &machine_i2c_type;
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
   // TODO(tannewt): Replace pin_find with a unified version.
   const pin_obj_t* scl = pin_find(args[ARG_scl].u_obj);
   const pin_obj_t* sda = pin_find(args[ARG_sda].u_obj);
   mp_hal_i2c_construct(self, scl, sda, args[ARG_freq].u_int);
   return (mp_obj_t)self;
}


//|   .. method:: I2C.deinit()
//|
//|     Releases control of the underlying hardware so other classes can use it.
//|
STATIC mp_obj_t machine_i2c_obj_deinit(mp_obj_t self_in) {
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
   mp_hal_i2c_deinit(self);
   return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_deinit_obj, machine_i2c_obj_deinit);

//|   .. method:: I2C.__enter__()
//|
//|     No-op used in Context Managers.
//|
STATIC mp_obj_t machine_i2c_obj___enter__(mp_obj_t self_in) {
   return self_in;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c___enter___obj, machine_i2c_obj___enter__);

//|   .. method:: I2C.__exit__()
//|
//|     Automatically deinitializes the hardware on context exit.
//|
STATIC mp_obj_t machine_i2c_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_hal_i2c_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_obj___exit___obj, 4, 4, machine_i2c_obj___exit__);

//|   .. method:: I2C.scan()
//|
//|      Scan all I2C addresses between 0x08 and 0x77 inclusive and return a list of
//|      those that respond.  A device responds if it pulls the SDA line low after
//|      its address (including a read bit) is sent on the bus.
//|
STATIC mp_obj_t machine_i2c_scan(mp_obj_t self_in) {
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
   mp_obj_t list = mp_obj_new_list(0, NULL);
   // 7-bit addresses 0b0000xxx and 0b1111xxx are reserved
   for (int addr = 0x08; addr < 0x78; ++addr) {
       bool success = mp_hal_i2c_probe(self, addr);
       if (success) {
           mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
       }
   }
   return list;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_scan_obj, machine_i2c_scan);

//|   .. method:: I2C.readfrom(addr, nbytes)
//|
//|      Read `nbytes` from the slave specified by `addr`.
//|
//|     :param int addr: The 7 bit address of the device
//|     :param int nbytes: The number of bytes to read
//|     :return: the data read
//|     :rtype: bytes
//|
STATIC mp_obj_t machine_i2c_readfrom(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t nbytes_in) {
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
   vstr_t vstr;
   vstr_init_len(&vstr, mp_obj_get_int(nbytes_in));
   mp_hal_i2c_read(self, mp_obj_get_int(addr_in), (uint8_t*)vstr.buf, vstr.len);
   return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_3(machine_i2c_readfrom_obj, machine_i2c_readfrom);

//|   .. method:: I2C.readfrom_into(addr, buf)
//|
//|      Read into `buf` from the slave specified by `addr`.
//|      The number of bytes read will be the length of `buf`.
//|
STATIC mp_obj_t machine_i2c_readfrom_into(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t buf_in) {
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
   mp_buffer_info_t bufinfo;
   mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
   mp_hal_i2c_read(self, mp_obj_get_int(addr_in), (uint8_t*)bufinfo.buf, bufinfo.len);
   return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(machine_i2c_readfrom_into_obj, machine_i2c_readfrom_into);

//|   .. method:: I2C.writeto(addr, buf)
//|
//|      Write the bytes from `buf` to the slave specified by `addr`.
//|
STATIC mp_obj_t machine_i2c_writeto(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t buf_in) {
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
   mp_buffer_info_t bufinfo;
   mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
   mp_hal_i2c_write(self, mp_obj_get_int(addr_in), bufinfo.buf, bufinfo.len);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_i2c_writeto_obj, machine_i2c_writeto);

//|   .. method:: I2C.readfrom_mem(addr, memaddr, nbytes, \*, addrsize=8)
//|
//|      Read `nbytes` from the slave specified by `addr` starting from the memory
//|      address specified by `memaddr`.
//|      The argument `addrsize` specifies the address size in bits (on ESP8266
//|      this argument is not recognised and the address size is always 8 bits).
//|      Returns a `bytes` object with the data read.
//|
STATIC mp_obj_t machine_i2c_readfrom_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
   enum { ARG_addr, ARG_memaddr, ARG_n, ARG_addrsize };
   static const mp_arg_t allowed_args[] = {
       { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
       { MP_QSTR_memaddr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
       { MP_QSTR_n,       MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
       //{ MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} }, TODO
   };
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
   mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
   mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

   // create the buffer to store data into
   vstr_t vstr;
   vstr_init_len(&vstr, args[ARG_n].u_int);

   // do the transfer
   mp_hal_i2c_read_mem(self, args[ARG_addr].u_int, args[ARG_memaddr].u_int, (uint8_t*)vstr.buf, vstr.len);
   return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_readfrom_mem_obj, 1, machine_i2c_readfrom_mem);


//|   .. method:: I2C.readfrom_mem_into(addr, memaddr, buf, \*, addrsize=8)
//|
//|      Read into `buf` from the slave specified by `addr` starting from the
//|      memory address specified by `memaddr`.  The number of bytes read is the
//|      length of `buf`.
//|      The argument `addrsize` specifies the address size in bits (on ESP8266
//|      this argument is not recognised and the address size is always 8 bits).
//|
STATIC mp_obj_t machine_i2c_readfrom_mem_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
   enum { ARG_addr, ARG_memaddr, ARG_buf, ARG_addrsize };
   static const mp_arg_t allowed_args[] = {
       { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
       { MP_QSTR_memaddr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
       { MP_QSTR_buf,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
       //{ MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} }, TODO
   };
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
   mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
   mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

   // get the buffer to store data into
   mp_buffer_info_t bufinfo;
   mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_WRITE);

   // do the transfer
   mp_hal_i2c_read_mem(self, args[ARG_addr].u_int, args[ARG_memaddr].u_int, bufinfo.buf, bufinfo.len);
   return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_readfrom_mem_into_obj, 1, machine_i2c_readfrom_mem_into);

//|   .. method:: I2C.writeto_mem(addr, memaddr, buf, \*, addrsize=8)
//|
//|      Write `buf` to the slave specified by `addr` starting from the
//|      memory address specified by `memaddr`.
//|      The argument `addrsize` specifies the address size in bits (on ESP8266
//|      this argument is not recognised and the address size is always 8 bits).
//|
STATIC mp_obj_t machine_i2c_writeto_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
   enum { ARG_addr, ARG_memaddr, ARG_buf, ARG_addrsize };
   static const mp_arg_t allowed_args[] = {
       { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
       { MP_QSTR_memaddr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
       { MP_QSTR_buf,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
       //{ MP_QSTR_addrsize, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} }, TODO
   };
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
   mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
   mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

   // get the buffer to write the data from
   mp_buffer_info_t bufinfo;
   mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_READ);

   // do the transfer
   mp_hal_i2c_write_mem(self, args[ARG_addr].u_int, args[ARG_memaddr].u_int, bufinfo.buf, bufinfo.len);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_writeto_mem_obj, 1, machine_i2c_writeto_mem);

STATIC const mp_rom_map_elem_t machine_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_i2c_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&machine_i2c___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&machine_i2c_obj___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&machine_i2c_scan_obj) },

    // standard bus operations
    { MP_ROM_QSTR(MP_QSTR_readfrom), MP_ROM_PTR(&machine_i2c_readfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_into), MP_ROM_PTR(&machine_i2c_readfrom_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&machine_i2c_writeto_obj) },

    // memory operations
    // TODO(tannewt): Move these into a separate loadable Python module.
    { MP_ROM_QSTR(MP_QSTR_readfrom_mem), MP_ROM_PTR(&machine_i2c_readfrom_mem_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_mem_into), MP_ROM_PTR(&machine_i2c_readfrom_mem_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto_mem), MP_ROM_PTR(&machine_i2c_writeto_mem_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_i2c_locals_dict, machine_i2c_locals_dict_table);

const mp_obj_type_t machine_i2c_type = {
   { &mp_type_type },
   .name = MP_QSTR_I2C,
   .make_new = machine_i2c_make_new,
   .locals_dict = (mp_obj_dict_t*)&machine_i2c_locals_dict,
};

//| :class:`SPI` -- a 3-4 wire serial protocol
//| -----------------------------------------------
//|
//| SPI is a serial protocol that has exlusive pins for data in and out of the
//| master.  It is typically faster than `I2C` because a separate pin is used to
//| control the active slave rather than a transitted address. This class only
//| manages three of the four SPI lines: `clock`, `MOSI`, `MISO`. Its up to the
//| client to manage the appropriate slave select line. (This is common because
//| multiple slaves can share the `clock`, `MOSI` and `MISO` lines and therefore
//| the hardware.)
//|
//| .. class:: SPI(clock, MOSI, MISO, baudrate=1000000)
//|
//|    Construct an SPI object on the given bus.  ``id`` can be only 0.
//|    With no additional parameters, the SPI object is created but not
//|    initialised (it has the settings from the last initialisation of
//|    the bus, if any).  If extra arguments are given, the bus is initialised.
//|    See ``init`` for parameters of initialisation.
//|
//|  - ``clock`` is the pin to use for the clock.
//|  - ``MOSI`` is the  Master Out Slave In pin.
//|  - ``MISO`` is the Master In Slave Out pin.
//|  - ``baudrate`` is the SCK clock rate.
//|

// TODO(tannewt): Support LSB SPI.
// TODO(tannewt): Support phase, polarity and bit order.
STATIC mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
   mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
   machine_spi_obj_t *self = m_new_obj(machine_spi_obj_t);
   self->base.type = &machine_spi_type;
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
   // TODO(tannewt): Replace pin_find with a unified version.
   const pin_obj_t* clock = pin_find(args[ARG_clock].u_obj);
   const pin_obj_t* mosi = pin_find(args[ARG_MOSI].u_obj);
   const pin_obj_t* miso = pin_find(args[ARG_MISO].u_obj);
   mp_hal_spi_construct(self, clock, mosi, miso, args[ARG_baudrate].u_int);
   return (mp_obj_t)self;
}

//|   .. method:: SPI.deinit()
//|
//|      Turn off the SPI bus.
//|
STATIC mp_obj_t machine_spi_obj_deinit(mp_obj_t self_in) {
   machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
   mp_hal_spi_deinit(self);
   return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_spi_deinit_obj, machine_spi_obj_deinit);

//|   .. method:: SPI.__enter__()
//|
//|      No-op used by Context Managers.
//|
STATIC mp_obj_t machine_spi_obj___enter__(mp_obj_t self_in) {
   return self_in;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_spi___enter___obj, machine_spi_obj___enter__);

//|   .. method:: SPI.__enter__()
//|
//|      Automatically deinitializes the hardware when exiting a context.
//|
STATIC mp_obj_t machine_spi_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    mp_hal_spi_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_spi_obj___exit___obj, 4, 4, machine_spi_obj___exit__);

//|   .. method:: SPI.write_readinto(write_buf, read_buf)
//|
//|       Write from ``write_buf`` and read into ``read_buf``. Both buffers must have the
//|       same length. This is the same as a SPI transfer function on other platforms.
//|       Returns the number of bytes written
//|
STATIC mp_obj_t mp_machine_spi_write_readinto(mp_obj_t self_in, mp_obj_t wr_buf, mp_obj_t rd_buf) {
    mp_buffer_info_t src;
    mp_get_buffer_raise(wr_buf, &src, MP_BUFFER_READ);
    mp_buffer_info_t dest;
    mp_get_buffer_raise(rd_buf, &dest, MP_BUFFER_WRITE);
    if (src.len != dest.len) {
        mp_raise_ValueError("buffers must be the same length");
    }
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_spi_transfer(self, src.len, (uint8_t *) src.buf, (uint8_t *) dest.buf);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(mp_machine_spi_write_readinto_obj, mp_machine_spi_write_readinto);

//|   .. method:: SPI.write(buf)
//|
//|       Write the data contained in ``buf``.
//|       Returns the number of bytes written.
//|
STATIC mp_obj_t mp_machine_spi_write(mp_obj_t self_in, mp_obj_t wr_buf) {
    mp_buffer_info_t src;
    mp_get_buffer_raise(wr_buf, &src, MP_BUFFER_READ);
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_spi_transfer(self, src.len, (uint8_t *) src.buf, NULL);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_machine_spi_write_obj, mp_machine_spi_write);

//|   .. method:: SPI.read(nbytes, *, write=0x00)
//|
//|       Read the ``nbytes`` while writing the data specified by ``write``.
//|       Return the number of bytes read.
//|
STATIC mp_obj_t mp_machine_spi_read(size_t n_args, const mp_obj_t *args) {
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(args[1]));
    memset(vstr.buf, n_args == 3 ? mp_obj_get_int(args[2]) : 0, vstr.len);
    mp_hal_spi_transfer(args[0], vstr.len, (uint8_t *) vstr.buf, (uint8_t *) vstr.buf);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_spi_read_obj, 2, 3, mp_machine_spi_read);

//|   .. method:: SPI.readinto(buf, *, write=0x00)
//|
//|       Read into the buffer specified by ``buf`` while writing the data
//|       specified by ``write``.
//|       Return the number of bytes read.
//|
STATIC mp_obj_t mp_machine_spi_readinto(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, n_args == 3 ? mp_obj_get_int(args[2]) : 0, bufinfo.len);
    mp_hal_spi_transfer(args[0], bufinfo.len, (uint8_t *) bufinfo.buf, (uint8_t *) bufinfo.buf);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_spi_readinto_obj, 2, 3, mp_machine_spi_readinto);

STATIC const mp_rom_map_elem_t machine_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&machine_spi___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&machine_spi_obj___exit___obj) },

    // Standard simultaneous read/write transfer.
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&mp_machine_spi_write_readinto_obj) },

    // Helper methods.
    // TODO(tannewt): Move these into a helper Python class.
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_machine_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_machine_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_machine_spi_write_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_spi_locals_dict, machine_spi_locals_dict_table);

const mp_obj_type_t machine_spi_type = {
   { &mp_type_type },
   .name = MP_QSTR_SPI,
   .make_new = machine_spi_make_new,
   .locals_dict = (mp_obj_dict_t*)&machine_spi_locals_dict,
};
