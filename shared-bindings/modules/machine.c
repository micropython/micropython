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

//| .. currentmodule:: machine
//|
//| class I2C -- a two-wire serial protocol
//| =======================================
//|
//| I2C is a two-wire protocol for communicating between devices.  At the
//| physical level it consists of 2 wires: SCL and SDA, the clock and data lines
//| respectively.
//|
//| I2C objects are created attached to a specific bus.  They can be initialised
//| when created, or initialised later on.

STATIC void machine_i2c_obj_init_helper(machine_i2c_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
   enum { ARG_scl, ARG_sda, ARG_freq };
   static const mp_arg_t allowed_args[] = {
       { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_OBJ },
       { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_OBJ },
       { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
   };
   mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
   mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
   // TODO(tannewt): Replace pin_find with a unified version.
   const pin_obj_t* scl = pin_find(args[ARG_scl].u_obj);
   const pin_obj_t* sda = pin_find(args[ARG_sda].u_obj);
   mp_hal_i2c_init(self, scl, sda, args[ARG_freq].u_int);
}

//| Constructors
//| ------------
//|     .. class:: I2C(scl, sda, \*, freq=400000)
//|
//|        Construct and return a new I2C object.
//|        See the init method below for a description of the arguments.
STATIC mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
   mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
   machine_i2c_obj_t *self = m_new_obj(machine_i2c_obj_t);
   self->base.type = &machine_i2c_type;
   mp_map_t kw_args;
   mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
   machine_i2c_obj_init_helper(self, n_args, args, &kw_args);
   return (mp_obj_t)self;
}

//| General Methods
//| ---------------
//|   .. method:: I2C.init(scl, sda, \*, freq=400000)
//|
//|     Initialise the I2C bus with the given arguments:
//|
//|        - `scl` is a pin object for the SCL line
//|        - `sda` is a pin object for the SDA line
//|        - `freq` is the SCL clock rate
STATIC mp_obj_t machine_i2c_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
   machine_i2c_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
   return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_init_obj, 1, machine_i2c_obj_init);

//| .. method:: I2C.scan()
//|
//|    Scan all I2C addresses between 0x08 and 0x77 inclusive and return a list of
//|    those that respond.  A device responds if it pulls the SDA line low after
//|    its address (including a read bit) is sent on the bus.
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

//| Standard bus operations
//| -----------------------
//|
//| The following methods implement the standard I2C master read and write
//| operations that target a given slave device.
//|
//| .. method:: I2C.readfrom(addr, nbytes)
//|
//|    Read `nbytes` from the slave specified by `addr`.
//|    Returns a `bytes` object with the data read.
STATIC mp_obj_t machine_i2c_readfrom(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t nbytes_in) {
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
   vstr_t vstr;
   vstr_init_len(&vstr, mp_obj_get_int(nbytes_in));
   mp_hal_i2c_read(self, mp_obj_get_int(addr_in), (uint8_t*)vstr.buf, vstr.len);
   return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_3(machine_i2c_readfrom_obj, machine_i2c_readfrom);

//| .. method:: I2C.readfrom_into(addr, buf)
//|
//|    Read into `buf` from the slave specified by `addr`.
//|    The number of bytes read will be the length of `buf`.
//|
//|    On WiPy the return value is the number of bytes read.  Otherwise the
//|    return value is `None`.
STATIC mp_obj_t machine_i2c_readfrom_into(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t buf_in) {
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
   mp_buffer_info_t bufinfo;
   mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
   mp_hal_i2c_read(self, mp_obj_get_int(addr_in), (uint8_t*)bufinfo.buf, bufinfo.len);
   return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(machine_i2c_readfrom_into_obj, machine_i2c_readfrom_into);

//| .. method:: I2C.writeto(addr, buf)
//|
//|    Write the bytes from `buf` to the slave specified by `addr`.
STATIC mp_obj_t machine_i2c_writeto(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t buf_in) {
   machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
   mp_buffer_info_t bufinfo;
   mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
   mp_hal_i2c_write(self, mp_obj_get_int(addr_in), bufinfo.buf, bufinfo.len);
   return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_i2c_writeto_obj, machine_i2c_writeto);

//| Memory operations
//| -----------------
//|
//| Some I2C devices act as a memory device (or set of registers) that can be
//| read from and written to.  In this case there are two addresses associated
//| with an I2C transaction: the slave address and the memory address.  The following
//| following methods are convenience functions to communicate with such
//| devices.
//|
//| .. method:: I2C.readfrom_mem(addr, memaddr, nbytes, \*, addrsize=8)
//|
//|    Read `nbytes` from the slave specified by `addr` starting from the memory
//|    address specified by `memaddr`.
//|    The argument `addrsize` specifies the address size in bits (on ESP8266
//|    this argument is not recognised and the address size is always 8 bits).
//|    Returns a `bytes` object with the data read.
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


//| .. method:: I2C.readfrom_mem_into(addr, memaddr, buf, \*, addrsize=8)
//|
//|    Read into `buf` from the slave specified by `addr` starting from the
//|    memory address specified by `memaddr`.  The number of bytes read is the
//|    length of `buf`.
//|    The argument `addrsize` specifies the address size in bits (on ESP8266
//|    this argument is not recognised and the address size is always 8 bits).
//|
//|    On WiPy the return value is the number of bytes read.  Otherwise the
//|    return value is `None`.
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

//| .. method:: I2C.writeto_mem(addr, memaddr, buf, \*, addrsize=8)
//|
//|    Write `buf` to the slave specified by `addr` starting from the
//|    memory address specified by `memaddr`.
//|    The argument `addrsize` specifies the address size in bits (on ESP8266
//|    this argument is not recognised and the address size is always 8 bits).
//|
//|    On WiPy the return value is the number of bytes written.  Otherwise the
//|    return value is `None`.
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
   { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_i2c_init_obj) },
   { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&machine_i2c_scan_obj) },

   // standard bus operations
   { MP_ROM_QSTR(MP_QSTR_readfrom), MP_ROM_PTR(&machine_i2c_readfrom_obj) },
   { MP_ROM_QSTR(MP_QSTR_readfrom_into), MP_ROM_PTR(&machine_i2c_readfrom_into_obj) },
   { MP_ROM_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&machine_i2c_writeto_obj) },

   // memory operations
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
