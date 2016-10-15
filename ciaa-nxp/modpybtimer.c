/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ernesto Gigliotti <ernestogigliotti@gmail.com>
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
#include "py/nlr.h"
#include "py/gc.h"

#include "board.h"
#include "modpyb.h"
#include "ciaanxp_mphal.h"

typedef struct _pyb_timer_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
    mp_uint_t freq;
} pyb_timer_obj_t;

STATIC pyb_timer_obj_t pyb_timer_obj[] = {
    {{&pyb_timer_type}},
    {{&pyb_timer_type}},
    {{&pyb_timer_type}},
    {{&pyb_timer_type}}
};

#define NUM_TIMER MP_ARRAY_SIZE(pyb_timer_obj)
#define TIMER_ID(obj) ((obj) - &pyb_timer_obj[0])

void pyb_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_obj_t *self = self_in;
    mp_printf(print, "TIMER(%u)", TIMER_ID(self));
}

/// Constructor
STATIC mp_obj_t pyb_timer_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t timer_id = mp_obj_get_int(args[0]);

    if (timer_id >= NUM_TIMER) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "TIMER %d does not exist", timer_id));
    }

    if(n_args>2) {
    	//mp_hal_enableTimerAsTimer();
	//pyb_timer_init_helper(&pyb_timer_obj[timer_id], n_args-1, args+1, mp_map_t *kw_args);
    }
    return (mp_obj_t)&pyb_timer_obj[timer_id];
}

STATIC void pyb_timer_init_helper(pyb_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int =-1} },
        { MP_QSTR_prescaler,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_period, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint32_t freq = args[0].u_int;
    uint32_t presc = args[1].u_int;
    uint32_t period = args[2].u_int;

    if(freq>0 && freq<=1000000) // 1hz - 1Mhz range
    {
		uint32_t f = mp_hal_getTimerClockFrequency();
		period = f/freq;
		presc = 0;
		self->freq = freq;
    }
    else{
        if(freq!=-1 && presc==0 && period==0)
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Invalid Frecuency range"));

		self->freq = 0;
	}

	mp_hal_enableTimerAsTimer(TIMER_ID(self), presc,period,0);
}

void pyb_timer_exec_callback(pyb_timer_obj_t* self)
{
	// execute callback if it's set
	if (self->callback != mp_const_none) {
		// When executing code within a handler we must lock the GC to prevent
		// any memory allocations.  We must also catch any exceptions.
		gc_lock();
		nlr_buf_t nlr;
		if (nlr_push(&nlr) == 0) {
			mp_call_function_1(self->callback, self);
			nlr_pop();
		} else {
			// Uncaught exception; disable the callback so it doesn't run again.
			self->callback = mp_const_none;
			printf("uncaught exception in timer(%u) interrupt handler\n",TIMER_ID(self));
			mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
		}
		gc_unlock();
	}
}

/// \method init(freq)
/// \method init(prescaler, period)
STATIC mp_obj_t pyb_timer_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return  mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_timer_init_obj, 1, pyb_timer_init);

/// \method callback(fun)
/// Set the function to be called in timer interrupt.
/// `fun` is passed 1 argument, the timer object.
/// If `fun` is `None` then the callback will be disabled.
STATIC mp_obj_t pyb_timer_callback(mp_obj_t self_in, mp_obj_t callback) {
    pyb_timer_obj_t *self = self_in;

    if (callback == mp_const_none) {
        // stop interrupt
        self->callback = mp_const_none;
		mp_hal_setTimerCallback(TIMER_ID(self),NULL,NULL);	
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
		mp_hal_setTimerCallback(TIMER_ID(self),( void(*)(void*)  )pyb_timer_exec_callback,self);
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "callback must be None or a callable object"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_timer_callback_obj, pyb_timer_callback);

/// \method interval(period,fun)
STATIC mp_obj_t pyb_timer_interval(mp_obj_t self_in, mp_obj_t periodMs, mp_obj_t callback) {
    pyb_timer_obj_t *self = self_in;

	uint32_t pms = (uint32_t)mp_obj_get_int(periodMs);
	
	mp_hal_enableTimerAsTimer(TIMER_ID(self), pms,mp_hal_getTimerClockFrequency()/1000,0);
	
	pyb_timer_callback(self,callback);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_timer_interval_obj, pyb_timer_interval);

/// \method timeout(period,fun)
STATIC mp_obj_t pyb_timer_timeout(mp_obj_t self_in, mp_obj_t periodMs, mp_obj_t callback) {
    pyb_timer_obj_t *self = self_in;

	uint32_t pms = (uint32_t)mp_obj_get_int(periodMs);
	
	mp_hal_enableTimerAsTimer(TIMER_ID(self), pms,mp_hal_getTimerClockFrequency()/1000,1);
	
	pyb_timer_callback(self,callback);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_timer_timeout_obj, pyb_timer_timeout);


/// \method counter([value])
/// Get or set the timer counter:
///
///   - With no argument, return current timer counter.
///   - With `value` given, set the timer counter
STATIC mp_obj_t pyb_timer_counter(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get counter
        return MP_OBJ_NEW_SMALL_INT(mp_hal_getTimerCounter(TIMER_ID(self)));
    } else {
        // set counter
        mp_hal_setTimerCounter(TIMER_ID(self),mp_obj_get_int(args[1]));		
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_counter_obj, 1, 2, pyb_timer_counter);

/// \method deinit()
STATIC mp_obj_t pyb_timer_deinit(mp_obj_t self_in) {
    pyb_timer_obj_t *self = self_in;

    mp_hal_disableTimer(TIMER_ID(self));
    self->callback = mp_const_none;
	mp_hal_setTimerCallback(TIMER_ID(self),NULL,NULL);	
		
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_timer_deinit_obj, pyb_timer_deinit);

/// \method freq([value])
/// Get or set the timer frequency:
///
///   - With no argument, return current timer frequency.
///   - With `value` given, set the timer frequency
STATIC mp_obj_t pyb_timer_freq(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get freq
        return MP_OBJ_NEW_SMALL_INT(self->freq);
    } else {
        // set freq
        uint32_t freq = mp_obj_get_int(args[1]);
        if(freq>0 && freq<=1000000) // 1hz - 1Mhz range
		{
			uint32_t f = mp_hal_getTimerClockFrequency();
			uint32_t period = f/freq;
			uint32_t presc = 0;
			self->freq = freq;
			mp_hal_enableTimerAsTimer(TIMER_ID(self), presc,period,0);
		}
		else
			nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Frequency out of range"));
		
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_freq_obj, 1, 2, pyb_timer_freq);


/// \method period([value])
/// Get or set the timer period:
///
///   - With no argument, return current timer period.
///   - With `value` given, set the timer period
STATIC mp_obj_t pyb_timer_period(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get period
        return MP_OBJ_NEW_SMALL_INT(mp_hal_getTimerMatch(TIMER_ID(self)));
    } else {
        // set period
        uint32_t p = mp_obj_get_int(args[1]);
        mp_hal_setTimerMatch(TIMER_ID(self),p);		
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_period_obj, 1, 2, pyb_timer_period);


/// \method prescaler([value])
/// Get or set the timer prescaler:
///
///   - With no argument, return current timer prescaler.
///   - With `value` given, set the timer prescaler
STATIC mp_obj_t pyb_timer_prescaler(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get prescaler
        return MP_OBJ_NEW_SMALL_INT(mp_hal_getTimerPrescaler(TIMER_ID(self)));
    } else {
        // set prescaler
        uint32_t p = mp_obj_get_int(args[1]);
        mp_hal_setTimerPrescaler(TIMER_ID(self),p);		
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_prescaler_obj, 1, 2, pyb_timer_prescaler);


/// \method source_freq()
STATIC mp_obj_t pyb_timer_source_freq(mp_obj_t self_in, mp_obj_t callback) {
  		
    return MP_OBJ_NEW_SMALL_INT(mp_hal_getTimerClockFrequency());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_timer_source_freq_obj, pyb_timer_source_freq);


STATIC const mp_map_elem_t pyb_timer_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_timer_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&pyb_timer_callback_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_interval), (mp_obj_t)&pyb_timer_interval_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_timeout), (mp_obj_t)&pyb_timer_timeout_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_counter), (mp_obj_t)&pyb_timer_counter_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_timer_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_freq), (mp_obj_t)&pyb_timer_freq_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_period), (mp_obj_t)&pyb_timer_period_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_prescaler), (mp_obj_t)&pyb_timer_prescaler_obj },    
    { MP_OBJ_NEW_QSTR(MP_QSTR_source_freq), (mp_obj_t)&pyb_timer_source_freq_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_timer_locals_dict, pyb_timer_locals_dict_table);

const mp_obj_type_t pyb_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = pyb_timer_print,
    .make_new = pyb_timer_make_new,
    .locals_dict = (mp_obj_t)&pyb_timer_locals_dict,
};
