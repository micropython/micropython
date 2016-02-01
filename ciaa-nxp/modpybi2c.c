/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#include "py/stream.h"
#include "board.h"
#include "modpyb.h"
#include "ciaanxp_mphal.h"
#include "stmhal/bufhelper.h"

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    mp_int_t slaveAddr;
} pyb_i2c_obj_t;

STATIC pyb_i2c_obj_t pyb_i2c_obj[] = {
    {{&pyb_i2c_type}}
};

#define I2C_ID(obj) ((obj) - &pyb_i2c_obj[0])

void pyb_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_i2c_obj_t *self = self_in;
    mp_printf(print, "I2C (%u)", I2C_ID(self) );
}

STATIC mp_obj_t pyb_i2c_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_int_t bitrate = mp_obj_get_int(args[0]);

    mp_hal_setSpeedI2C(bitrate);    

    return (mp_obj_t)&pyb_i2c_obj[0];
}



// functions used by streams
STATIC mp_uint_t pyb_i2c_write_m(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {

    uint8_t* buf = (uint8_t*)buf_in;
	   
    // write the data    
    mp_hal_masterWrite(pyb_i2c_obj[0].slaveAddr, buf, size);
    return size+1;
}
STATIC mp_uint_t pyb_i2c_read_m(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    uint8_t* buf = (uint8_t*)buf_in;

	return mp_hal_masterReadI2C(pyb_i2c_obj[0].slaveAddr, buf, size);    
}


STATIC mp_obj_t pyb_i2c_readinto(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buf,       MP_ARG_REQUIRED | MP_ARG_OBJ, }
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), allowed_args, args);

    // get the buffer to receive into
    vstr_t vstr;
    pyb_buf_get_for_recv(args[0].u_obj, &vstr);

    // just receive
    mp_hal_masterReadI2C(pyb_i2c_obj[0].slaveAddr, (uint8_t*)vstr.buf, vstr.len);

    // return the number of bytes received
    return mp_obj_new_int(vstr.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readinto_obj, 1, pyb_i2c_readinto);


/// \method slave_addr([value])
STATIC mp_obj_t i2c_slave_addr(mp_uint_t n_args, const mp_obj_t *args) {

    if (n_args == 1) {
        // get 
        return MP_OBJ_NEW_SMALL_INT(pyb_i2c_obj[0].slaveAddr);
    } else {
        // set 
	    pyb_i2c_obj[0].slaveAddr = mp_obj_get_int(args[1]);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_i2c_slave_addr_obj, 1, 2, i2c_slave_addr);



STATIC const mp_map_elem_t pyb_i2c_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto),            (mp_obj_t)&pyb_i2c_readinto_obj },    
    { MP_OBJ_NEW_QSTR(MP_QSTR_slave_addr),            (mp_obj_t)&pyb_i2c_slave_addr_obj },    
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_table);

STATIC const mp_stream_p_t i2c_stream_p = {
    .read = pyb_i2c_read_m,
    .write = pyb_i2c_write_m,
    .is_text = false,
};

const mp_obj_type_t pyb_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = pyb_i2c_print,
    .make_new = pyb_i2c_make_new,
    .stream_p = &i2c_stream_p,
    .locals_dict = (mp_obj_t)&pyb_i2c_locals_dict,
};
