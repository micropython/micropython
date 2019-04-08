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

#include "wm_include.h"
#include "wm_i2c.h"

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "modmachine.h"

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    uint32_t freq;
    uint32_t device_addr;
    uint32_t reg_addr;
} machine_i2c_obj_t;

/******************************************************************************/
// MicroPython bindings for I2C

STATIC void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(freq=%u, device_addr=%u, reg_addr=%u)",
        self->freq, self->device_addr, self->reg_addr);
}

STATIC void machine_i2c_init_helper(machine_i2c_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_deviceaddr, ARG_regaddr };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_deviceaddr, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_regaddr, MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->device_addr = args[ARG_deviceaddr].u_int;
    self->reg_addr = args[ARG_regaddr].u_int;

    tls_i2c_init(self->freq);
}

STATIC mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 3, 3, true);

    // Defaults
    machine_i2c_obj_t *self = m_new_obj(machine_i2c_obj_t);
    self->base.type = &machine_i2c_type;

    self->freq = mp_obj_get_int(args[0]);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_i2c_init_helper(self, n_args - 1, args + 1, &kw_args);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_i2c_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_i2c_obj_t *self = args[0];
    machine_i2c_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_i2c_init_obj, 1, machine_i2c_init);

STATIC const mp_rom_map_elem_t machine_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_i2c_init_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_i2c_locals_dict, machine_i2c_locals_dict_table);

STATIC u8 w600_i2c_readOneByte(u16 deviceaddr, u16 regaddr)
{				  
	u8 temp=0;		  	    																 
	tls_i2c_write_byte(deviceaddr << 7,1);   
	tls_i2c_wait_ack(); 
    	tls_i2c_write_byte(regaddr,0);   
	tls_i2c_wait_ack();	    

	tls_i2c_write_byte((deviceaddr << 7) | 0x1,1);
	tls_i2c_wait_ack();	 
	temp=tls_i2c_read_byte(0,1);
	return temp;
}

STATIC void w600_i2c_writeOneByte(u16 deviceaddr, u16 regaddr, u8 data)
{				   	  	    																 
	tls_i2c_write_byte(deviceaddr << 7, 1); 
	tls_i2c_wait_ack();	   
	tls_i2c_write_byte(regaddr, 0);
	tls_i2c_wait_ack(); 	 										  		   
	tls_i2c_write_byte(data, 0); 				   
	tls_i2c_wait_ack();  	   
 	tls_i2c_stop();
	tls_os_time_delay(1);
}

STATIC void w600_i2c_read(u16 deviceaddr, u16 regaddr, u8 *buf, u16 len)
{
	while(len)
	{
		*buf++=w600_i2c_readOneByte(deviceaddr, regaddr++);	
		len--;
	}
}

STATIC void w600_i2c_write(u16 deviceaddr, u16 regaddr, u8 *buf, u16 len)
{
	while(len--)
	{
		w600_i2c_writeOneByte(deviceaddr, regaddr,*buf);
		regaddr++;
		buf++;
	}
} 

STATIC mp_uint_t machine_i2c_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    w600_i2c_read(self->device_addr, self->reg_addr, buf_in, size);

    return size;
}

STATIC mp_uint_t machine_i2c_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);

    w600_i2c_write(self->device_addr, self->reg_addr, buf_in, size);

    return size;
}


STATIC const mp_stream_p_t i2c_stream_p = {
    .read = machine_i2c_read,
    .write = machine_i2c_write,
    .is_text = false,
};

const mp_obj_type_t machine_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .print = machine_i2c_print,
    .make_new = machine_i2c_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &i2c_stream_p,
    .locals_dict = (mp_obj_dict_t*)&machine_i2c_locals_dict,
};
