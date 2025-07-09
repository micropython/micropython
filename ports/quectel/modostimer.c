/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include <stdlib.h>
#include <string.h>
#include "py/compile.h"
#include "py/nlr.h"
#include "py/objlist.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "py/obj.h"

#if MICROPY_QPY_MODULE_OSTIMER

#include "helios_os.h"
#include "helios_debug.h"

#define HELIOS_OSTIMER_LOG(msg, ...)      custom_log("osTimer", msg, ##__VA_ARGS__)


typedef struct _mod_ostimer_obj_t 
{
	mp_obj_base_t base;
	Helios_OSTimer_t handle;     	/* OS supplied timer reference             			*/
	unsigned int initialTime;   	/* initial expiration time in ms           			*/
	bool cyclicalEn;				/* wether to enable the cyclical mode or not		*/
	c_callback_t callback;  		/* timer call-back routine     						*/
	bool deleteFlagh;
} mod_ostimer_obj_t;

const mp_obj_type_t mp_ostimer_type;

STATIC void mod_ostimer_isr(void *cb) {
    c_callback_t *callback = (c_callback_t *)cb;
	if(NULL != callback){
    	mp_sched_schedule_ex(callback, mp_const_none);
	}
}


STATIC mp_obj_t mod_ostimer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
	mod_ostimer_obj_t *timer = mp_obj_malloc_with_finaliser(mod_ostimer_obj_t);

	timer->base.type = &mp_ostimer_type;
	timer->handle = Helios_OSTimer_Create();
	timer->deleteFlagh = 0;

	return MP_OBJ_FROM_PTR(timer);
}


STATIC mp_obj_t mod_ostimer_start(uint n_args, const mp_obj_t *args)
{
	int ret = 0;
	
	mod_ostimer_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (!(self->deleteFlagh))
    {
    	self->initialTime = mp_obj_get_int(args[1]);

    	self->cyclicalEn = !!mp_obj_get_int(args[2]);

    	//self->callback = args[3];
    	mp_sched_schedule_callback_register(&self->callback, args[3]);

        Helios_OSTimerAttr OSTimerAttr = {
            .ms = (uint32_t)self->initialTime,
            .cycle_enable = self->cyclicalEn,
            .cb = mod_ostimer_isr,
            .argv = (void *)&self->callback
        };
        ret = Helios_OSTimer_Start(self->handle, &OSTimerAttr);
    }
    else
    {
        ret = -1;
    }
	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_ostimer_start_obj, 3, 5, mod_ostimer_start);

STATIC mp_obj_t mod_ostimer_stop(mp_obj_t arg0)
{
	int ret = 0;
	
	mod_ostimer_obj_t *self = MP_OBJ_TO_PTR(arg0);
    if (!(self->deleteFlagh))
    {
	    ret = Helios_OSTimer_Stop(self->handle);
	}
	else
	{
        ret = -1;
	}
	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_ostimer_stop_obj, mod_ostimer_stop);



STATIC mp_obj_t mod_ostimer_delete(mp_obj_t arg0)
{
	int ret = 0;
	
	mod_ostimer_obj_t *self = MP_OBJ_TO_PTR(arg0);

	if (!(self->deleteFlagh))
	{
		self->deleteFlagh = 1;
        Helios_OSTimer_Delete(self->handle);
        HELIOS_OSTIMER_LOG("[osTimer] ostimer delete\r\n");
	}

	return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_ostimer_delete_obj, mod_ostimer_delete);


STATIC const mp_rom_map_elem_t mod_ostimer_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_osTimer) },
	{ MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mod_ostimer_delete_obj) },
	{ MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&mod_ostimer_start_obj) },
	{ MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&mod_ostimer_stop_obj) },
	{ MP_ROM_QSTR(MP_QSTR_delete_timer), MP_ROM_PTR(&mod_ostimer_delete_obj) },
};
STATIC MP_DEFINE_CONST_DICT(mod_ostimer_locals_dict, mod_ostimer_locals_dict_table);


const mp_obj_type_t mp_ostimer_type = {
    { &mp_type_type },
    .name = MP_QSTR_osTimer,
	.make_new = mod_ostimer_make_new,
	.locals_dict = (mp_obj_dict_t *)&mod_ostimer_locals_dict,
};


#endif /* MICROPY_QPY_MODULE_OSTIMER */
