/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
 * Copyright (c) 2016 Chester Tseng
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
#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "exception.h"

#include "bufhelper.h"
#include "obji2c.h"

#define I2C_MASTER                          (0)
#define I2C_SLAVE                           (1)
#define I2C_MIN_BAUD_RATE_HZ                (50000)
#define I2C_MAX_BAUD_RATE_HZ                (400000)

#define I2C_SDA                             PD_7
#define I2C_SCL                             PD_6

// Declare RTK i2c struct data in heap. If you declare it in the stack, it will hang !

// channe 0 (I2C 0) is mapped to (SDA, SCL) = (PD_4, PD_5) 
// channe 1 (I2C 1) is mapped to (SDA, SCL) = (PD_7, PD_6), (PC_4, PC_5)
// channe 2 (I2C 2) did not mapped to evm
// channe 3 (I2C 3) is mapped to (SDA, SCL) = (PB_3, PB_2) 
i2c_t i2c_channel0;
i2c_t i2c_channel1;
i2c_t i2c_channel3;

STATIC mp_obj_t mp_i2c_reset(mp_obj_t self_in) {
    i2c_obj_t *self = (i2c_obj_t *)&self_in;
    i2c_reset((i2c_t *)self->obj);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(i2c_reset_obj, mp_i2c_reset);

STATIC bool pyb_i2c_write(i2c_t *i2c, byte addr, byte *data, uint len, bool stop) {
    int8_t retval = true;

    retval = i2c_write(i2c, addr, data, len, stop);

    if (retval <= 0) {
        return false;
    }
ret:
    return true;
}

STATIC bool pyb_i2c_read(i2c_t *i2c, byte addr, byte **data, uint len) {
    int8_t retval = true;

    retval = i2c_read(i2c, addr, *data, len, true);
    
    if (retval != len) {
        return false;
    }
ret:
    return true;
}

STATIC mp_obj_t i2c_readfrom(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t i2c_readfrom_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT, },
        { MP_QSTR_nbytes,  MP_ARG_REQUIRED | MP_ARG_OBJ, },
    };

    i2c_obj_t *self = pos_args[0];
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(i2c_readfrom_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), i2c_readfrom_args, args);

    vstr_t vstr;

    // get the buffer to receive into
    pyb_buf_get_for_recv(args[1].u_obj, &vstr);

    // receive the data
    if (!pyb_i2c_read((i2c_t *)self->obj, args[0].u_int, (byte *)&(vstr.buf), vstr.len)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }
    // return the received data
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(i2c_readfrom_obj, 3, i2c_readfrom);

STATIC mp_obj_t i2c_writeto(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t pyb_i2c_writeto_args[] = {
        { MP_QSTR_addr,    MP_ARG_REQUIRED | MP_ARG_INT,  },
        { MP_QSTR_buf,     MP_ARG_REQUIRED | MP_ARG_OBJ,  },
        { MP_QSTR_stop,    MP_ARG_KW_ONLY  | MP_ARG_BOOL, {.u_bool = true} },
    };

    i2c_obj_t *self = pos_args[0];
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(pyb_i2c_writeto_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), pyb_i2c_writeto_args, args);

    // get the buffer to send from
    mp_buffer_info_t bufinfo;
    uint8_t data[1];
    pyb_buf_get_for_send(args[1].u_obj, &bufinfo, data);

    // send the data
    if (!pyb_i2c_write((i2c_t *)self->obj, args[0].u_int, bufinfo.buf, bufinfo.len, args[2].u_bool)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_operation_failed));
    }

    // return the number of bytes written
    return mp_obj_new_int(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(i2c_writeto_obj, 1, i2c_writeto);

STATIC void i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    i2c_obj_t *self = self_in;
    qstr mode;
    if (self->mode == I2C_MASTER) {
        mode = MP_QSTR_MASTER;
    }
    else {
        mode = MP_QSTR_SLAVE;
    }
    mp_printf(print, "I2C(%d, I2C.%q, baudrate=%u)", self->id, mode, self->baudrate);
}

STATIC const mp_arg_t i2c_init_args[] = {
    { MP_QSTR_id,                          MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_mode,      MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = I2C_MASTER} },
    { MP_QSTR_baudrate,  MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 100000} },
    { MP_QSTR_pins,      MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};

STATIC mp_obj_t i2c_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(i2c_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), i2c_init_args, args);

    if (args[0].u_int < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_resource_not_avaliable));
    }   

    i2c_obj_t *self = m_new_obj(i2c_obj_t);
    self->base.type = &i2c_type;
    self->id        = args[0].u_int;
    self->mode      = args[1].u_int;
    self->baudrate  = MIN(MAX(args[2].u_int, I2C_MIN_BAUD_RATE_HZ), I2C_MAX_BAUD_RATE_HZ);

    self->obj = (void *)&i2c_channel1;

    // TODO: should check alternative pins 
    i2c_init((i2c_t *)self->obj, I2C_SDA, I2C_SCL);

    i2c_frequency((i2c_t *)self->obj, self->baudrate);

    return (mp_obj_t)self;
}


STATIC const mp_map_elem_t i2c_locals_dict_table[] = {
    // instance methods
#if 0 // TODO: Need to figure out how to scan ...
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan),                (mp_obj_t)&i2c_scan_obj },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset),               (mp_obj_t)&i2c_reset_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readfrom),            (mp_obj_t)&i2c_readfrom_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeto),             (mp_obj_t)&i2c_writeto_obj },
#if 0
    { MP_OBJ_NEW_QSTR(MP_QSTR_readfrom_into),       (mp_obj_t)&pyb_i2c_readfrom_into_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readfrom_mem),        (mp_obj_t)&pyb_i2c_readfrom_mem_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readfrom_mem_into),   (mp_obj_t)&pyb_i2c_readfrom_mem_into_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeto_mem),         (mp_obj_t)&pyb_i2c_writeto_mem_obj },

#endif
    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_MASTER),              MP_OBJ_NEW_SMALL_INT(I2C_MASTER) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLAVE),               MP_OBJ_NEW_SMALL_INT(I2C_SLAVE) },
};
STATIC MP_DEFINE_CONST_DICT(i2c_locals_dict, i2c_locals_dict_table);

const mp_obj_type_t i2c_type = {
    { &mp_type_type },
    .name        = MP_QSTR_I2C,
    .print       = i2c_print,
    .make_new    = i2c_make_new,
    .locals_dict = (mp_obj_t)&i2c_locals_dict,
};
