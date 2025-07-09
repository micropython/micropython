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
#include <stdlib.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"


#if MICROPY_QPY_MACHINE_TIMER

#include "helios_timer.h"
#include "helios_debug.h"
#include "callbackdeal.h"


typedef unsigned long int UINT32;
#define HELIOS_TIMER_LOG(msg, ...)      custom_log("machine_timer", msg, ##__VA_ARGS__)


typedef enum
{
	Timer0 = 0,
	Timer1 = 1,
	Timer2 = 2,
	Timer3 = 3
}Timern;
    
typedef enum   /* The meaning of the API flag*/         
{
	TIMER_PERIODIC = 0x1,		/* periodic execution */
	TIMER_AUTO_DELETE = 0x2	    /* one execution */
}TIMER_FLAG;

typedef enum
{
	MP_TIMER_CREATED,
	MP_TIMER_RUNNING,
	MP_TIMER_STOP
}MP_TIMER_STATUS;

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
	//ONE_SHOT OR PERIODIC 
    mp_uint_t mode;
    mp_uint_t period;
    c_callback_t callback;
	mp_int_t timer_id;
	mp_int_t timer_id_real;
	MP_TIMER_STATUS timer_status;
	//for traverse all the timers, such as soft reset while input CTRL+B
    struct _machine_timer_obj_t *next;
} machine_timer_obj_t;

static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)(print);
    (void)(kind);
    machine_timer_obj_t *self = self_in;
    HELIOS_TIMER_LOG("period=%d\r\n", self->period);
    HELIOS_TIMER_LOG("timer_id=%d\r\n", self->timer_id);
}

#if defined(BOARD_BC32RA) || defined(BOARD_BC92RB) || defined(PLAT_SONY_ALT1350)//forrest.liu@20231228 add for FAE-102027 dump issue
#define HELIOS_TIMER_MAX_NUM (4)
static st_CallBack_Timer Timer_t[HELIOS_TIMER_MAX_NUM] = {{{0,0,0,0,0}},{{0,0,0,0,0}},{{0,0,0,0,0}},{{0,0,0,0,0}}};
#endif

static void machine_timer_isr(UINT32 self_in) {

    machine_timer_obj_t *self = (void*)self_in;
	if(self != NULL)
	{
    #if defined(BOARD_BC32RA) || defined(BOARD_BC92RB) || defined(PLAT_SONY_ALT1350)//forrest.liu@20231228 modify for FAE-102027 dump issue
	    Timer_t[self->timer_id].callback = self->callback;
	    qpy_send_msg_to_callback_deal_thread(CALLBACK_TYPE_ID_TIMER, &Timer_t[self->timer_id]); 
	#else
		mp_sched_schedule_ex(&self->callback, self);
	#endif
	}

}


static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_mode,
        ARG_callback,
        ARG_period,
        ARG_tick_hz,
        ARG_freq,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_PERIODIC} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_tick_hz,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        #if MICROPY_PY_BUILTINS_FLOAT
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        #else
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        #endif
    };

    int ret;

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if MICROPY_PY_BUILTINS_FLOAT
    if (args[ARG_freq].u_obj != mp_const_none) {
        self->period = (mp_uint_t)(1000 / mp_obj_get_float(args[ARG_freq].u_obj));
    }
    #else
    if (args[ARG_freq].u_int != 0xffffffff) {
        self->period = 1000 / ((mp_uint_t)args[ARG_freq].u_int);
    }
    #endif
    else {
        self->period = (((uint64_t)args[ARG_period].u_int) * 1000) / args[ARG_tick_hz].u_int;
    }

    self->mode = args[ARG_mode].u_int;

    mp_sched_schedule_callback_register(&self->callback, args[ARG_callback].u_obj);

    HELIOS_TIMER_LOG("mode: %d, period: %d,callback:%#X\r\n", self->mode, self->period,self->callback.cb);

// Check whether the timer is already running, if so return 0
    if (MP_TIMER_RUNNING == self->timer_status) {
        return mp_obj_new_int(0);
    }

    int timer = 0;
	if(0 == self->timer_id_real)
	{
        
        if(!self)
        {
            HELIOS_TIMER_LOG("timer test 147!\n");
        }
        HELIOS_TIMER_LOG("self = %x\n",self);
	    timer = Helios_Timer_init( (void* )machine_timer_isr, self);
	}
	else
	{
        HELIOS_TIMER_LOG("timer test 153!\n");
        timer = self->timer_id_real;
	}
	
    if(!timer) {
        return mp_const_false;
	}

	uint8_t cyclicalEn = 0;
	if(self->mode == TIMER_PERIODIC) {
		cyclicalEn = 1;
	}
    HELIOS_TIMER_LOG("timer test 165!\n");
	ret = Helios_Timer_Start(timer, (uint32_t) self->period, cyclicalEn);
    HELIOS_TIMER_LOG("timer test 167!\n");
    
	if(!ret)
	{
        //get time_id_real
		HELIOS_TIMER_LOG("timer_id: %#x\r\n", timer);
		self->timer_id_real = timer;
		self->timer_status = MP_TIMER_RUNNING;
	}
	else
	{
        HELIOS_TIMER_LOG("timer create failed.ret=%d\r\n",ret);
	}
    return mp_obj_new_int(0);
}



static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)(type);
	mp_int_t timer_id=0;
	
	if (n_args > 0) {
			timer_id = mp_obj_get_int(args[0]);
			--n_args;
			++args;
	}

	if ((timer_id < 0) || (timer_id > 3))
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid value, Timern should be in (Timer0~Timer3)."));
	}
	
	machine_timer_obj_t *self = m_new_obj(machine_timer_obj_t);
    self->base.type = &machine_timer_type;
	self->timer_id = timer_id;
	self->timer_id_real = 0;
	self->timer_status = MP_TIMER_CREATED;

    if (n_args > 0 || n_kw > 0) {
        // Start the timer
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
    }
	
	return MP_OBJ_FROM_PTR(self);
}


static mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    return machine_timer_init_helper(self, n_args - 1, args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

static mp_obj_t machine_timer_stop(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);

	if(self->timer_id_real)
	{
		HELIOS_TIMER_LOG("machine_timer_stop (%#X) \r\n",self->timer_id_real);		
		Helios_Timer_Stop(self->timer_id_real);
		self->timer_status = MP_TIMER_STOP;
		return mp_obj_new_int(0);
	}
    return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_stop_obj, machine_timer_stop);


static mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);

	if(self->timer_id_real)
	{
		HELIOS_TIMER_LOG("machine_timer_deinit (%#X) \r\n",self->timer_id_real);
		Helios_Timer_Stop(self->timer_id_real);
		Helios_Timer_Deinit(self->timer_id_real);
		self->timer_id_real = 0;
		self->timer_status = MP_TIMER_STOP;
		return mp_obj_new_int(0);
	}
    return mp_obj_new_int(-1);
}

static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);


static mp_obj_t machine_timer_initialize() {
    static int initialized = 0;
    if (!initialized) {
        
        HELIOS_TIMER_LOG("machine_timer_initialize\r\n");
        initialized = 1;
    }
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0(machine_timer_initialize_obj, machine_timer_initialize);

static const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_deinit_obj) },
	{ MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&machine_timer_initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&machine_timer_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_AUTO_DELETE) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_PERIODIC) },
    { MP_ROM_QSTR(MP_QSTR_Timer0), MP_ROM_INT(Timer0) },
    { MP_ROM_QSTR(MP_QSTR_Timer1), MP_ROM_INT(Timer1) },
    { MP_ROM_QSTR(MP_QSTR_Timer2), MP_ROM_INT(Timer2) },
    { MP_ROM_QSTR(MP_QSTR_Timer3), MP_ROM_INT(Timer3) },
};
static MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
	machine_timer_type,
	MP_QSTR_Timer,
	MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
	print, machine_timer_print,
    locals_dict, &machine_timer_locals_dict
	);

#endif
