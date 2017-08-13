/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/machine_i2c.h"

#if MICROPY_PY_MACHINE_I2C

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    uint32_t us_delay;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
} machine_i2c_obj_t;

STATIC void mp_hal_i2c_delay(machine_i2c_obj_t *self) {
    // We need to use an accurate delay to get acceptable I2C
    // speeds (eg 1us should be not much more than 1us).
    mp_hal_delay_us_fast(self->us_delay);
}

STATIC void mp_hal_i2c_scl_low(machine_i2c_obj_t *self) {
    mp_hal_pin_od_low(self->scl);
}

STATIC void mp_hal_i2c_scl_release(machine_i2c_obj_t *self) {
    mp_hal_pin_od_high(self->scl);
}

STATIC void mp_hal_i2c_sda_low(machine_i2c_obj_t *self) {
    mp_hal_pin_od_low(self->sda);
}

STATIC void mp_hal_i2c_sda_release(machine_i2c_obj_t *self) {
    mp_hal_pin_od_high(self->sda);
}

STATIC int mp_hal_i2c_sda_read(machine_i2c_obj_t *self) {
    return mp_hal_pin_read(self->sda);
}

STATIC void mp_hal_i2c_start(machine_i2c_obj_t *self) {
    mp_hal_i2c_sda_release(self);
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_release(self);
    mp_hal_i2c_delay(self);
    mp_hal_i2c_sda_low(self);
    mp_hal_i2c_delay(self);
}

STATIC void mp_hal_i2c_stop(machine_i2c_obj_t *self) {
    mp_hal_i2c_delay(self);
    mp_hal_i2c_sda_low(self);
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_release(self);
    mp_hal_i2c_delay(self);
    mp_hal_i2c_sda_release(self);
    mp_hal_i2c_delay(self);
}

STATIC void mp_hal_i2c_init(machine_i2c_obj_t *self, uint32_t freq) {
    self->us_delay = 500000 / freq;
    if (self->us_delay == 0) {
        self->us_delay = 1;
    }
    mp_hal_pin_open_drain(self->scl);
    mp_hal_pin_open_drain(self->sda);
    mp_hal_i2c_stop(self);
}

STATIC int mp_hal_i2c_write_byte(machine_i2c_obj_t *self, uint8_t val) {
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_low(self);

    for (int i = 7; i >= 0; i--) {
        if ((val >> i) & 1) {
            mp_hal_i2c_sda_release(self);
        } else {
            mp_hal_i2c_sda_low(self);
        }
        mp_hal_i2c_delay(self);
        mp_hal_i2c_scl_release(self);
        mp_hal_i2c_delay(self);
        mp_hal_i2c_scl_low(self);
    }

    mp_hal_i2c_sda_release(self);
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_release(self);
    mp_hal_i2c_delay(self);

    int ret = mp_hal_i2c_sda_read(self);
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_low(self);

    return !ret;
}

STATIC void mp_hal_i2c_write(machine_i2c_obj_t *self, uint8_t addr, uint8_t *data, size_t len) {
    mp_hal_i2c_start(self);
    if (!mp_hal_i2c_write_byte(self, addr << 1)) {
        goto er;
    }
    while (len--) {
        if (!mp_hal_i2c_write_byte(self, *data++)) {
            goto er;
        }
    }
    mp_hal_i2c_stop(self);
    return;

er:
    mp_hal_i2c_stop(self);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
}

STATIC int mp_hal_i2c_read_byte(machine_i2c_obj_t *self, uint8_t *val, int nack) {
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_low(self);
    mp_hal_i2c_delay(self);

    uint8_t data = 0;
    for (int i = 7; i >= 0; i--) {
        mp_hal_i2c_scl_release(self);
        mp_hal_i2c_delay(self);
        data = (data << 1) | mp_hal_i2c_sda_read(self);
        mp_hal_i2c_scl_low(self);
        mp_hal_i2c_delay(self);
    }
    *val = data;

    // send ack/nack bit
    if (!nack) {
        mp_hal_i2c_sda_low(self);
    }
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_release(self);
    mp_hal_i2c_delay(self);
    mp_hal_i2c_scl_low(self);
    mp_hal_i2c_sda_release(self);

    return 1; // success
}

STATIC void mp_hal_i2c_read(machine_i2c_obj_t *self, uint8_t addr, uint8_t *data, size_t len) {
    mp_hal_i2c_start(self);
    if (!mp_hal_i2c_write_byte(self, (addr << 1) | 1)) {
        goto er;
    }
    while (len--) {
        if (!mp_hal_i2c_read_byte(self, data++, len == 0)) {
            goto er;
        }
    }
    mp_hal_i2c_stop(self);
    return;

er:
    mp_hal_i2c_stop(self);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
}

STATIC void mp_hal_i2c_write_mem(machine_i2c_obj_t *self, uint8_t addr, uint16_t memaddr, const uint8_t *src, size_t len) {
    // start the I2C transaction
    mp_hal_i2c_start(self);

    // write the slave address and the memory address within the slave
    if (!mp_hal_i2c_write_byte(self, addr << 1)) {
        goto er;
    }
    if (!mp_hal_i2c_write_byte(self, memaddr)) {
        goto er;
    }

    // write the buffer to the I2C memory
    while (len--) {
        if (!mp_hal_i2c_write_byte(self, *src++)) {
            goto er;
        }
    }

    // finish the I2C transaction
    mp_hal_i2c_stop(self);
    return;

er:
    mp_hal_i2c_stop(self);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
}

STATIC void mp_hal_i2c_read_mem(machine_i2c_obj_t *self, uint8_t addr, uint16_t memaddr, uint8_t *dest, size_t len) {
    // start the I2C transaction
    mp_hal_i2c_start(self);

    // write the slave address and the memory address within the slave
    if (!mp_hal_i2c_write_byte(self, addr << 1)) {
        goto er;
    }
    if (!mp_hal_i2c_write_byte(self, memaddr)) {
        goto er;
    }

    // i2c_read will do a repeated start, and then read the I2C memory
    mp_hal_i2c_read(self, addr, dest, len);
    return;

er:
    mp_hal_i2c_stop(self);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
}

/******************************************************************************/
// MicroPython bindings for I2C

STATIC void machine_i2c_obj_init_helper(machine_i2c_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_scl, ARG_sda, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    self->scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
    self->sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
    mp_hal_i2c_init(self, args[ARG_freq].u_int);
}

STATIC mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    machine_i2c_obj_t *self = m_new_obj(machine_i2c_obj_t);
    self->base.type = &machine_i2c_type;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_i2c_obj_init_helper(self, n_args, args, &kw_args);
    return (mp_obj_t)self;
}

STATIC mp_obj_t machine_i2c_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_i2c_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_init_obj, 1, machine_i2c_obj_init);

STATIC mp_obj_t machine_i2c_scan(mp_obj_t self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t list = mp_obj_new_list(0, NULL);
    // 7-bit addresses 0b0000xxx and 0b1111xxx are reserved
    for (int addr = 0x08; addr < 0x78; ++addr) {
        mp_hal_i2c_start(self);
        int ack = mp_hal_i2c_write_byte(self, (addr << 1) | 1);
        if (ack) {
            mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
        }
        mp_hal_i2c_stop(self);
    }
    return list;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_scan_obj, machine_i2c_scan);

STATIC mp_obj_t machine_i2c_start(mp_obj_t self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_i2c_start(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_start_obj, machine_i2c_start);

STATIC mp_obj_t machine_i2c_stop(mp_obj_t self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_i2c_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_stop_obj, machine_i2c_stop);

STATIC mp_obj_t machine_i2c_readinto(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // get the buffer to read into
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);

    // do the read
    uint8_t *dest = bufinfo.buf;
    while (bufinfo.len--) {
        if (!mp_hal_i2c_read_byte(self, dest++, bufinfo.len == 0)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
        }
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(machine_i2c_readinto_obj, machine_i2c_readinto);

STATIC mp_obj_t machine_i2c_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // get the buffer to write from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    // do the write
    uint8_t *src = bufinfo.buf;
    while (bufinfo.len--) {
        if (!mp_hal_i2c_write_byte(self, *src++)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
        }
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(machine_i2c_write_obj, machine_i2c_write);

STATIC mp_obj_t machine_i2c_readfrom(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t nbytes_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    vstr_t vstr;
    vstr_init_len(&vstr, mp_obj_get_int(nbytes_in));
    mp_hal_i2c_read(self, mp_obj_get_int(addr_in), (uint8_t*)vstr.buf, vstr.len);
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_3(machine_i2c_readfrom_obj, machine_i2c_readfrom);

STATIC mp_obj_t machine_i2c_readfrom_into(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t buf_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    mp_hal_i2c_read(self, mp_obj_get_int(addr_in), (uint8_t*)bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(machine_i2c_readfrom_into_obj, machine_i2c_readfrom_into);

STATIC mp_obj_t machine_i2c_writeto(mp_obj_t self_in, mp_obj_t addr_in, mp_obj_t buf_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    mp_hal_i2c_write(self, mp_obj_get_int(addr_in), bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(machine_i2c_writeto_obj, machine_i2c_writeto);

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

    // primitive I2C operations
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&machine_i2c_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&machine_i2c_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&machine_i2c_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_i2c_write_obj) },

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

#endif // MICROPY_PY_MACHINE_I2C
