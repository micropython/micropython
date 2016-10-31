/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ernesto Gigliotti <ernestogigliotti@gmail.com>
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

typedef struct _pyb_eeprom_obj_t {
    mp_obj_base_t base;
    uint32_t currentIndex;
} pyb_eeprom_obj_t;

STATIC pyb_eeprom_obj_t pyb_eeprom_obj[] = {
    {{&pyb_eeprom_type}}
};

#define EEPROM_ID(obj) ((obj) - &pyb_eeprom_obj[0])

void pyb_eeprom_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_eeprom_obj_t *self = self_in;
    mp_printf(print, "EEPROM (%u)", EEPROM_ID(self));
}

STATIC mp_obj_t pyb_eeprom_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    pyb_eeprom_obj[0].currentIndex=0;
    return (mp_obj_t)&pyb_eeprom_obj[0];
}


// read_byte method
mp_obj_t pyb_eeprom_read_byte(mp_obj_t self_in,mp_obj_t mpAddr) {
    uint32_t addr = mp_obj_get_int(mpAddr);
    int32_t r =  mp_hal_readByteEEPROM(addr);
    if(r == -1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    return MP_OBJ_NEW_SMALL_INT((uint8_t)r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_eeprom_read_byte_obj, pyb_eeprom_read_byte);


// write_byte method
mp_obj_t pyb_eeprom_write_byte(mp_obj_t self_in,mp_obj_t mpAddr,mp_obj_t mpValue) {
    uint32_t addr = mp_obj_get_int(mpAddr);
    uint8_t value = (uint8_t)mp_obj_get_int(mpValue);
    int32_t r = mp_hal_writeByteEEPROM(addr,value);
    if(r == -1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    return MP_OBJ_NEW_SMALL_INT(1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_eeprom_write_byte_obj, pyb_eeprom_write_byte);


// read_int method
mp_obj_t pyb_eeprom_read_int(mp_obj_t self_in,mp_obj_t mpAddr) {
    uint32_t addr = mp_obj_get_int(mpAddr);
    uint8_t valArray[4];
    int32_t r;
    r = mp_hal_readByteEEPROM(addr);
    if(r == -1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    valArray[0] = (uint8_t)r;

    r = mp_hal_readByteEEPROM(addr+1);
    if(r == -1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    valArray[1] = (uint8_t)r;

    r = mp_hal_readByteEEPROM(addr+2);
    if(r == -1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    valArray[2] = (uint8_t)r;

    r = mp_hal_readByteEEPROM(addr+3);
    if(r == -1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    valArray[3] = (uint8_t)r;

    return MP_OBJ_NEW_SMALL_INT(valArray[3]<<24 | valArray[2]<<16 | valArray[1]<<8 | valArray[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_eeprom_read_int_obj, pyb_eeprom_read_int);

// write_int method
mp_obj_t pyb_eeprom_write_int(mp_obj_t self_in,mp_obj_t mpAddr,mp_obj_t mpValue) {
    uint32_t addr = mp_obj_get_int(mpAddr);
    uint32_t value = (uint32_t)mp_obj_get_int(mpValue);
    uint8_t valArray[4];
    valArray[0] = value&0x000000FF;
    valArray[1] = (value>>8)&0x000000FF;
    valArray[2] = (value>>16)&0x000000FF;
    valArray[3] = (value>>24)&0x000000FF;

    int32_t r = 0;
    r|=mp_hal_writeByteEEPROM(addr,valArray[0]);
    r|=mp_hal_writeByteEEPROM(addr+1,valArray[1]);
    r|=mp_hal_writeByteEEPROM(addr+2,valArray[2]);
    r|=mp_hal_writeByteEEPROM(addr+3,valArray[3]);

    if(r==-1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));

    return MP_OBJ_NEW_SMALL_INT(4);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_eeprom_write_int_obj, pyb_eeprom_write_int);


// read_float method
mp_obj_t pyb_eeprom_read_float(mp_obj_t self_in,mp_obj_t mpAddr) {
    uint32_t addr = mp_obj_get_int(mpAddr);
    float val;
    uint8_t* pFloat = (uint8_t*)&val;
    int32_t r;

    r = mp_hal_readByteEEPROM(addr);
    if(r==-1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    pFloat[0] = (uint8_t)r;

    r = mp_hal_readByteEEPROM(addr+1);
    if(r==-1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    pFloat[1] = (uint8_t)r;

    r = mp_hal_readByteEEPROM(addr+2);
    if(r==-1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    pFloat[2] = (uint8_t)r;

    r = mp_hal_readByteEEPROM(addr+3);
    if(r==-1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));
    pFloat[3] = (uint8_t)r;

    return mp_obj_new_float(val);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_eeprom_read_float_obj, pyb_eeprom_read_float);


// write_float method
mp_obj_t pyb_eeprom_write_float(mp_obj_t self_in,mp_obj_t mpAddr,mp_obj_t mpValue) {

    uint32_t addr = mp_obj_get_int(mpAddr);

    float value = (float)mp_obj_get_float(mpValue);

    uint8_t* pFloat = (uint8_t*)&value;

    int32_t r=0;
    r|=mp_hal_writeByteEEPROM(addr,pFloat[0]);
    r|=mp_hal_writeByteEEPROM(addr+1,pFloat[1]);
    r|=mp_hal_writeByteEEPROM(addr+2,pFloat[2]);
    r|=mp_hal_writeByteEEPROM(addr+3,pFloat[3]);

    if(r==-1)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid address"));

    return MP_OBJ_NEW_SMALL_INT(4);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_eeprom_write_float_obj, pyb_eeprom_write_float);


// functions used by streams
STATIC mp_uint_t pyb_eeprom_write_m(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {

    const byte *buf = buf_in;

    if((size+1)>=(16*1024)) // 16Kbytes max
	    return 0;

    // write the data
    uint32_t addr;
    for(addr=0; addr<size; addr++)
	    mp_hal_writeByteEEPROM(addr,buf[addr]);

    mp_hal_writeByteEEPROM(addr,0x00);

    return size+1;
}

STATIC mp_uint_t pyb_eeprom_read_m(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    pyb_eeprom_obj_t *self = self_in;
    byte *buf = buf_in;

	if(self->currentIndex==-1)
	{
		self->currentIndex=0;
		return 0; // end of reading
	}
	
	if(mp_hal_readByteEEPROM(self->currentIndex)==0x00)
		return 0; // end of reading
	
    if (size == 0) {
        return 0;
    }

    uint32_t i;
    uint8_t data;
    uint32_t c=0;
    i=self->currentIndex;
    while(1) {
		data = mp_hal_readByteEEPROM(i);
        *buf++ = data;
        size--;
        i++;
        c++;
        if(size==0)
            break;
		if(data==0x00)
			break;	
    }

    if(data!=0x00 && mp_hal_readByteEEPROM(i)!=0x00)
    {
           // reading not finished. save current index for next reading
           self->currentIndex = i;
    }
    else
    {       // packet cleaning
        self->currentIndex=-1;
        if(data==0x00){
			if(c>0)
				c--; // 0x00 does not count
		}
    }

    return c;
}




STATIC const mp_map_elem_t pyb_eeprom_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_byte), (mp_obj_t)&pyb_eeprom_read_byte_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_int), (mp_obj_t)&pyb_eeprom_read_int_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_float), (mp_obj_t)&pyb_eeprom_read_float_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_write_byte), (mp_obj_t)&pyb_eeprom_write_byte_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_int), (mp_obj_t)&pyb_eeprom_write_int_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_float), (mp_obj_t)&pyb_eeprom_write_float_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },

    /// \method readall()
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall), (mp_obj_t)&mp_stream_readall_obj },

};

STATIC MP_DEFINE_CONST_DICT(pyb_eeprom_locals_dict, pyb_eeprom_locals_dict_table);


STATIC const mp_stream_p_t eeprom_stream_p = {
    .read = pyb_eeprom_read_m,
    .write = pyb_eeprom_write_m,
    .is_text = true,
};

const mp_obj_type_t pyb_eeprom_type = {
    { &mp_type_type },
    .name = MP_QSTR_EEPROM,
    .print = pyb_eeprom_print,
    .make_new = pyb_eeprom_make_new,
    .stream_p = &eeprom_stream_p,
    .locals_dict = (mp_obj_t)&pyb_eeprom_locals_dict,
};
