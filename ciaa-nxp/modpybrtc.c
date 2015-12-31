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


typedef struct _pyb_rtc_obj_t {
    mp_obj_base_t base;
} pyb_rtc_obj_t;

STATIC pyb_rtc_obj_t pyb_rtc_obj[] = {
    {{&pyb_rtc_type}}
};

#define RTC_ID(obj) ((obj) - &pyb_rtc_obj[0]) + 1

void pyb_rtc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_rtc_obj_t *self = self_in;
    mp_printf(print, "RTC (%u)", RTC_ID(self) );
}

STATIC mp_obj_t pyb_rtc_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    mp_hal_initRTC();

    return (mp_obj_t)&pyb_rtc_obj[0];
}

/// \method datetime([datetimetuple])
STATIC mp_obj_t rtc_datetime(mp_uint_t n_args, const mp_obj_t *args) {
    //pyb_rtc_obj_t *self = args[0];

    if (n_args == 1) {
        // get
        uint32_t yr;
		uint32_t mon;
		uint32_t day;
		uint32_t wd;
		uint32_t hr;
		uint32_t min;
		uint32_t sec; 
		
		mp_obj_t tuple[7];
		mp_hal_getTimeRTC(&hr, &min, &sec, &day, &mon, &yr, &wd);
		
		tuple[0] = MP_OBJ_NEW_SMALL_INT(yr);
		tuple[1] = MP_OBJ_NEW_SMALL_INT(mon);
		tuple[2] = MP_OBJ_NEW_SMALL_INT(day);
		tuple[3] = MP_OBJ_NEW_SMALL_INT(wd);
		tuple[4] = MP_OBJ_NEW_SMALL_INT(hr);
		tuple[5] = MP_OBJ_NEW_SMALL_INT(min);
		tuple[6] = MP_OBJ_NEW_SMALL_INT(sec);
					
		return mp_obj_new_tuple(7,tuple);
		
    } else  {
        // set 
        //(year, month, day, weekday, hours, minutes, seconds)
		mp_buffer_info_t bufinfo;
		mp_obj_t *items;
		mp_obj_get_array(args[1], &bufinfo.len, &items);
		
		if(bufinfo.len==7)
		{
			uint32_t yr = mp_obj_get_int(items[0]);
			uint32_t mon = mp_obj_get_int(items[1]);
			uint32_t day = mp_obj_get_int(items[2]);
			uint32_t wd = mp_obj_get_int(items[3]);
			uint32_t hr = mp_obj_get_int(items[4]);
			uint32_t min = mp_obj_get_int(items[5]);
			uint32_t sec = mp_obj_get_int(items[6]);					
			mp_hal_setTimeRTC(hr,min,sec,day,mon,yr,wd);
		}
		else{
			nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "7 arguments are required"));
		}
		
        return mp_const_none;	
    }
    
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rtc_datetime_obj, 1, 2, rtc_datetime);



/// \method read_bkp_reg(addr)
STATIC mp_obj_t rtc_read_bkp_reg(mp_obj_t self_in, mp_obj_t addrObj) {
   
	uint8_t addr = (uint8_t)mp_obj_get_int(addrObj);
	
	uint32_t val = mp_hal_readBkpRegisterRTC(addr);
	
	return MP_OBJ_NEW_SMALL_INT(val);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(rtc_read_bkp_reg_obj, rtc_read_bkp_reg);

/// \method write_bkp_write(addr,value)
STATIC mp_obj_t rtc_write_bkp_reg(mp_obj_t self_in, mp_obj_t addrObj, mp_obj_t valObj) {
   
	uint8_t addr = (uint8_t)mp_obj_get_int(addrObj);
	uint32_t val = (uint32_t)mp_obj_get_int(valObj);
	
	mp_hal_writeBkpRegisterRTC(addr,val);
	
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(rtc_write_bkp_reg_obj, rtc_write_bkp_reg);


/// \method calibration(value)
STATIC mp_obj_t rtc_calibration(mp_obj_t self_in, mp_obj_t valObj) {
   
	uint32_t val = (uint32_t)mp_obj_get_int(valObj);
	
	mp_hal_setCalibRTC(val);
	
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(rtc_calibration_obj, rtc_calibration);


STATIC const mp_map_elem_t pyb_rtc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_datetime), (mp_obj_t)&rtc_datetime_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_bkp_reg), (mp_obj_t)&rtc_read_bkp_reg_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_bkp_reg), (mp_obj_t)&rtc_write_bkp_reg_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_calibration), (mp_obj_t)&rtc_calibration_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_rtc_locals_dict, pyb_rtc_locals_dict_table);



const mp_obj_type_t pyb_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .print = pyb_rtc_print,
    .make_new = pyb_rtc_make_new,
    .locals_dict = (mp_obj_t)&pyb_rtc_locals_dict,
};
