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
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "runtime.h"
#include "gc.h"
#include "mphal.h"
#include "gccollect.h"

#if MICROPY_QPY_MACHINE_EXTINT
#include "helios_pin.h"
#include "helios_extint.h"
#include "helios_debug.h"
#include "callbackdeal.h"
#include "helios_os.h"

//#if defined(PLAT_EIGEN)
static Helios_Thread_t thread_id = 0;
static Helios_MsgQ_t extint_queue = 0;
typedef struct 
{
    uint8_t msg_type;
    uint8_t pin;
	uint8_t edge;
	void* cb;
}extint_Msg;

//#endif
#if 1
#define EXTINT_LOG(msg, ...)      custom_log("extint", msg, ##__VA_ARGS__)
#else
#define EXTINT_LOG(msg, ...)
#endif
/* sample time (ms) */
#define EXTINT_SAMPLE_TIME		(10)

extern int Helios_GPIO_GetLevel(Helios_GPIONum gpio_num);
const mp_obj_type_t machine_extint_type;

#if defined(BOARD_EG810MCN_GA_ALIPAY) || defined(BOARD_EG810MCN_GA_VOLTE)
static int __fastIntGpio = 0xffff;
#endif

typedef struct {
    mp_obj_base_t base;
    mp_int_t line;
	mp_int_t mode;
	mp_int_t pull;
	mp_int_t fliter_time;
	c_callback_t callback;
} extint_obj_t;


typedef struct {
	mp_int_t rising_count;
	mp_int_t falling_count;
} extint_count_t;

#if defined(PLAT_aic8800m40)
typedef void(*eint_handler_t)(int EDGE);
#else
typedef void(*eint_handler_t)(void);

#endif

static extint_obj_t *extint_obj[HELIOS_GPIOMAX];
static extint_count_t extint_count[HELIOS_GPIOMAX] = {0};
static Helios_OSTimer_t extint_fliter_timer[HELIOS_GPIOMAX] = {0};
static uint8_t extint_fliter_timer_is_start[HELIOS_GPIOMAX] = {0};

enum {
	HELIOS_EXTINT_RISING,
	HELIOS_EXTINT_FALLING,
};

#define EINT_HANDLER_DEF(n) handler##n
#define PLAT_EINT_HANDLER_DEF(n) BOOST_PP_REPEAT_1(n,EINT_HANDLER_DEF)

/*
** Jayceon-20200908:
** Replace function mp_call_function_1_protected() with mp_sched_schedule_ex to slove the dump problem.
*/
#if MICROPY_ENABLE_CALLBACK_DEAL
#define EXTINT_CALLBACK_OP(X, edge)     do{                                                                                     \
                                            st_CallBack_Extint *extint = malloc(sizeof(st_CallBack_Extint));                    \
                                    	    if(NULL != extint) {                                                                \
                                        	    extint->pin_no = X;                                                             \
                                        	    extint->edge = edge;                                                            \
                                        	    extint->callback = extint_obj[X]->callback;                                     \
                                        	    if(qpy_send_msg_to_callback_deal_thread(CALLBACK_TYPE_ID_EXTINT, extint))          \
                                        	    {																				\
													free(extint);																\
                                        	    }																				\
                                        	}                                                                                   \
                                        }while(0)
#else
#define EXTINT_CALLBACK_OP(X, edge)     do{                                                                                     \
                                            GC_STACKTOP_SET();                                                                  \
                                            mp_obj_t extint_list[2] = {                                                         \
                                                mp_obj_new_int(X),                                                              \
                                                mp_obj_new_int(edge),                                                           \
                                            };                                                                                  \
                                            mp_sched_schedule(&extint_obj[X]->callback,  mp_obj_new_list(2, extint_list));   \
                                            GC_STACKTOP_CLEAR();                                                                \
                                        }while(0)

#endif


#if defined(PLAT_aic8800m40)
#define HANDLER_FUN(X) 															            \
static void fliter_timer_handler##X(void *args) {											\
	int edge = (int)args;																	\
	static uint32_t extint_fliter_count = 0;												\
	uint32_t extint_fliter_count_max = extint_obj[X]->fliter_time / EXTINT_SAMPLE_TIME;		\
	int level = Helios_GPIO_GetLevel(X);													\
	switch (edge) {																			\
		case HELIOS_EXTINT_RISING:															\
			if (level != 1) {																\
				extint_fliter_count = 0;													\
			}																				\
			break;																			\
		case HELIOS_EXTINT_FALLING:															\
			if (level != 0) {																\
				extint_fliter_count = 0;													\
			}																				\
			break;																			\
	}																						\
	if (++extint_fliter_count > extint_fliter_count_max) {									\
		Helios_OSTimer_Stop(extint_fliter_timer[X]);										\
		extint_fliter_count = 0;															\
		extint_fliter_timer_is_start[X] = false;											\
		EXTINT_CALLBACK_OP(X, edge);														\
	}																						\
}																							\
static void handler##X(int EDGE)	       													\
{	                                                                                        \
	int edge = EDGE;                                                        				\
	if (extint_obj[X]->callback.cb != mp_const_none &&                                      \
	            ((mp_sched_num_pending() < MICROPY_SCHEDULER_DEPTH)))                       \
	{			                                                                            \
		if (extint_obj[X]->fliter_time > 0 && !extint_fliter_timer_is_start[X]) {			\
			if (!extint_fliter_timer[X]) {													\
				extint_fliter_timer[X] = Helios_OSTimer_Create();							\
				if (!extint_fliter_timer[X]) {												\
					EXTINT_LOG("error: extint fliter timer create failed!");				\
					EXTINT_CALLBACK_OP(X, edge);											\
				}																			\
			} else {																		\
				Helios_OSTimerAttr attr = {													\
					.ms = EXTINT_SAMPLE_TIME,												\
					.cycle_enable = 1,														\
					.cb = fliter_timer_handler##X,											\
					.argv = (void*)edge														\
				};																			\
				Helios_OSTimer_Start(extint_fliter_timer[X], &attr);						\
				extint_fliter_timer_is_start[X] = true;										\
			}																				\
		} else if(extint_obj[X]->fliter_time == 0){											\
			EXTINT_CALLBACK_OP(X, edge);                                                  	\
		}																					\
	}																			            \
	Helios_ExtInt_Enable(extint_obj[X]->line);                                              \
}                                                                                    		
#else
#define HANDLER_FUN(X) 															            \
static void fliter_timer_handler##X(void *args) {											\
	int edge = (int)args;																	\
	static uint32_t extint_fliter_count = 0;												\
	uint32_t extint_fliter_count_max = extint_obj[X]->fliter_time / EXTINT_SAMPLE_TIME;		\
	int level = Helios_GPIO_GetLevel(X);													\
	switch (edge) {																			\
		case HELIOS_EXTINT_RISING:															\
			if (level != 1) {																\
				Helios_OSTimer_Stop(extint_fliter_timer[X]);										\
				extint_fliter_count = 0;															\
				extint_fliter_timer_is_start[X] = false;\
			}																				\
			break;																			\
		case HELIOS_EXTINT_FALLING:															\
			if (level != 0) {		\
				Helios_OSTimer_Stop(extint_fliter_timer[X]);										\
				extint_fliter_count = 0;															\
				extint_fliter_timer_is_start[X] = false;\
			}																				\
			break;																			\
	}																						\
	if (++extint_fliter_count > extint_fliter_count_max) {	\
		Helios_OSTimer_Stop(extint_fliter_timer[X]);										\
		extint_fliter_count = 0;															\
		extint_fliter_timer_is_start[X] = false;											\
		EXTINT_CALLBACK_OP(X, edge);                                                        \
		if(edge == HELIOS_EXTINT_RISING)                                                    \
		{                                                                                   \
			extint_count[X].rising_count++;                                                 \
		}                                                                                   \
		else                                                                                \
		{                                                                                   \
			extint_count[X].falling_count++;                                                \
		}																					\
	}																						\
	Helios_ExtInt_Enable(extint_obj[X]->line);                                              \
}																							\
static void handler##X(void)	                                                            \
{	                                                                                        \
	int edge = HELIOS_EXTINT_RISING;\
	if(extint_obj[X]->mode == HELIOS_EDGE_RISING) 						                    \
	{                                                                                       \
		if(extint_obj[X]->fliter_time == 0) 												\
		{																					\
			extint_count[X].rising_count++;                                                 \
		}																					\
	}		                                                                                \
	else if(extint_obj[X]->mode == HELIOS_EDGE_FALLING)                                     \
	{                                                                                       \
		edge = HELIOS_EXTINT_FALLING;                                                       \
		if(extint_obj[X]->fliter_time == 0)                                                 \
		{                                                                                   \
			extint_count[X].falling_count++;                                                \
		}                                                                                   \
	}                                                                                       \
	else                                                                                    \
	{                                                                                       \
		if(Helios_GPIO_GetLevel((Helios_GPIONum)X) == 0)                                    \
		{                                                                                   \
			edge = HELIOS_EXTINT_FALLING;                                                   \
			if(extint_obj[X]->fliter_time == 0)                                             \
			{                                                                               \
			 	extint_count[X].falling_count++;                                            \
			}                                                                               \
		}                                                                                   \
		else                                                                                \
		{                                                                                   \
			if(extint_obj[X]->fliter_time == 0)                                             \
			{                                                                               \
             	extint_count[X].rising_count++;                                                \
            }                                                                               \
		}                                                                                   \
	}	                                                                                    \
	if (extint_obj[X]->callback.cb != mp_const_none &&                                      \
	            ((mp_sched_num_pending() < MICROPY_SCHEDULER_DEPTH)))                       \
	{			                                                                            \
		if (extint_obj[X]->fliter_time > 0 && !extint_fliter_timer_is_start[X]) {			\
			extint_fliter_timer_is_start[X] = true;\
			if(extint_queue && thread_id)                                                   \
			{                                                                               \
				extint_Msg msg; \
				msg.msg_type = 1;\
				msg.pin = X;                                                                \
				msg.edge = edge;  \
				msg.cb = fliter_timer_handler##X;\
				Helios_MsgQ_Put(extint_queue, (void *)&msg, sizeof(extint_Msg), HELIOS_NO_WAIT);\
			}                                                                               \
			else\
			{\
				Helios_OSTimerAttr attr = \
				{						\
					.ms = EXTINT_SAMPLE_TIME,\
					.cycle_enable = 1,				\
					.cb = fliter_timer_handler##X,	\
					.argv = (void*)edge		\
				};							\
				if(extint_fliter_timer[X])\
				{\
					Helios_OSTimer_Start(extint_fliter_timer[X], &attr);	\
				}\
			}\
		} else if(extint_obj[X]->fliter_time == 0){\
			if(extint_queue && thread_id)                                                   \
			{                                                                               \
				extint_Msg msg;                                                             \
				msg.msg_type = 0;\
				msg.pin = X;                                                                \
				msg.edge = edge;                                                            \
				Helios_MsgQ_Put(extint_queue, (void *)&msg, sizeof(extint_Msg), HELIOS_NO_WAIT);\
			}                                                                               \
			else                                                                            \
			{                                                                               \
				EXTINT_CALLBACK_OP(X, edge);                                                \
			}                                                                               \
		}																					\
	}																			            \
	if((extint_obj[X]->fliter_time == 0) && !(extint_queue && thread_id))                   \
	{																						\
		Helios_ExtInt_Enable(extint_obj[X]->line);                                          \
	}																	                    \
} 																							
#endif																						


HANDLER_FUN_0_N(PLAT_GPIO_NUM)




eint_handler_t eint_handler[HELIOS_GPIOMAX] = {
	handler0, PLAT_EINT_HANDLER_DEF(PLAT_GPIO_NUM)
};



static void extint_count_reset(int offset) {
	extint_count[offset].falling_count = 0;
	extint_count[offset].rising_count = 0;
}

/// \method line()
/// Return the line number that the pin is mapped to.
static mp_obj_t extint_obj_line(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->line);
}
static MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_line_obj, extint_obj_line);

/// \method enable()
/// Enable a disabled interrupt.
static mp_obj_t extint_obj_enable(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = Helios_ExtInt_Enable((Helios_GPIONum) self->line);
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_enable_obj, extint_obj_enable);

/// \method disable()
/// Disable the interrupt associated with the ExtInt object.
/// This could be useful for debouncing.
static mp_obj_t extint_obj_disable(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = Helios_ExtInt_Disable((Helios_GPIONum) self->line);
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_disable_obj, extint_obj_disable);

#if defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718) || defined(PLAT_SONY_ALT1350)
static void extint_entry(void * argv)
{
	extint_Msg msg; 
	int edge;
	while(1)
	{
		Helios_MsgQ_Get(extint_queue, (void *)&msg, sizeof(extint_Msg), HELIOS_WAIT_FOREVER);
		edge = msg.edge;
		if(msg.msg_type == 1)//start timer
		{
			if(extint_fliter_timer[msg.pin])
			{
				Helios_OSTimerAttr attr = 
				{													
					.ms = EXTINT_SAMPLE_TIME,												
					.cycle_enable = 1,														
					.cb = (void(*)(void *))msg.cb,											
					.argv = (void*)edge														
				};																			
				if(Helios_OSTimer_Start(extint_fliter_timer[msg.pin], &attr))//eigen 618 plat,we cant call Helios_OSTimer_Start in IRQ				
				{
				}
				else
				{
				}	
			}														
		}
		else
		{
			EXTINT_CALLBACK_OP(msg.pin,edge);  
			Helios_ExtInt_Enable(msg.pin); 
		}
	}
}
#endif
/// \classmethod \constructor(pin, mode, pull, callback)
/// Create an ExtInt object:
///
///   - `pin` is the pin on which to enable the interrupt (can be a pin object or any valid pin name).
///   - `mode` can be one of:
///     - `ExtInt.IRQ_RISING` - trigger on a rising edge;
///     - `ExtInt.IRQ_FALLING` - trigger on a falling edge;
///     - `ExtInt.IRQ_RISING_FALLING` - trigger on a rising or falling edge.
///   - `pull` can be one of:
///     - `pyb.Pin.PULL_NONE` - no pull up or down resistors;
///     - `pyb.Pin.PULL_UP` - enable the pull-up resistor;
///     - `pyb.Pin.PULL_DOWN` - enable the pull-down resistor.
///   - `callback` is the function to call when the interrupt triggers.  The
///   callback function must accept exactly 1 argument, which is the line that
///   triggered the interrupt.
static const mp_arg_t pyb_extint_make_new_args[] = {
    { MP_QSTR_pin,      MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = 0} },
    { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_pull,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_callback, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
#if defined(BOARD_EG810MCN_GA_ALIPAY) || defined(BOARD_EG810MCN_GA_VOLTE)
    { MP_QSTR_fast, 	MP_ARG_INT, {.u_int = 0} },
#endif
	{ MP_QSTR_filter_time, MP_ARG_INT, {.u_int = 0} },
};
#define PYB_EXTINT_MAKE_NEW_NUM_ARGS MP_ARRAY_SIZE(pyb_extint_make_new_args)

static mp_obj_t extint_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // type_in == extint_obj_type
    // parse args
    mp_arg_val_t vals[PYB_EXTINT_MAKE_NEW_NUM_ARGS];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, PYB_EXTINT_MAKE_NEW_NUM_ARGS, pyb_extint_make_new_args, vals);

#if !defined(PLAT_ECR6600)
	if (vals[0].u_int < HELIOS_GPIO0 || vals[0].u_int > HELIOS_GPIOMAX) {
		mp_raise_ValueError(MP_ERROR_TEXT("invalid pin value"));
	}
#endif

	if (extint_obj[vals[0].u_int] == NULL)
	{
		extint_obj[vals[0].u_int] =  mp_obj_malloc_with_finaliser(extint_obj_t, &machine_extint_type);
	}
	extint_obj_t *self = extint_obj[vals[0].u_int];
	self->base.type = type;
	self->line = vals[0].u_int;
	
	self->mode = vals[1].u_int;
	if (self->mode < HELIOS_EDGE_RISING || self->mode > HELIOS_EDGE_BOTH) {
		mp_raise_ValueError(MP_ERROR_TEXT("invalid mode value"));
	}
	
	self->pull = vals[2].u_int;
	if (self->pull < HELIOS_PULL_NONE || self->pull > HELIOS_PULL_DOWN) {
		mp_raise_ValueError(MP_ERROR_TEXT("invalid pull value"));
	}
	
	static c_callback_t cb = {0};
    memset(&cb, 0, sizeof(c_callback_t));
	cb.arg = mp_obj_new_tuple(2, NULL);
	self->callback = cb;
    mp_sched_schedule_callback_register(&self->callback, vals[3].u_obj);
	// EXTINT_LOG("ds cb 457!\n");
	
	Helios_ExtIntStruct extint_struct = {0};
	extint_struct.gpio_trigger = HELIOS_EDGE_TRIGGER;
	extint_struct.gpio_edge = self->mode;
	extint_struct.gpio_debounce = HELIOS_DEBOUNCE_EN;
	extint_struct.gpio_pull = self->pull;
	extint_struct.eint_cb = NULL;
	extint_struct.wakeup_eint_cb = eint_handler[self->line];

	// EXTINT_LOG("%x\n", extint_struct.wakeup_eint_cb);

	Helios_ExtInt_Deinit((Helios_GPIONum) self->line);

	if(0 != Helios_ExtInt_Init((Helios_GPIONum) self->line,  &extint_struct)) {
		mp_raise_ValueError(MP_ERROR_TEXT("Interrupt initialization failed"));
	}
	
#if defined(BOARD_EG810MCN_GA_ALIPAY) || defined(BOARD_EG810MCN_GA_VOLTE)
	if(vals[4].u_int == 1) {
		__fastIntGpio = self->line;
		Helios_ExtInt_FastSetGpio(__fastIntGpio);
	} else {
		if(__fastIntGpio == self->line) {
			__fastIntGpio = 0xffff;
			Helios_ExtInt_FastSetGpio(__fastIntGpio);
		}
	}
	self->fliter_time = vals[5].u_int;
#else
	self->fliter_time = vals[4].u_int;
#endif

	if (self->fliter_time < 0) 
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid fliter time value"));
	}

	if(self->fliter_time > 0)
	{
		if(!extint_fliter_timer[self->line]) 
		{													
			extint_fliter_timer[self->line] = Helios_OSTimer_Create();							
			if (!extint_fliter_timer[self->line]) 
			{												
				mp_raise_ValueError(MP_ERROR_TEXT("error: extint fliter timer create failed!"));															
			}																			
		}
	}
#if defined(PLAT_EIGEN) || defined(PLAT_EIGEN_718) || defined(PLAT_SONY_ALT1350)

	if(extint_queue == 0)
	{
		extint_queue = Helios_MsgQ_Create(256, sizeof(extint_Msg));
		if(extint_queue == 0)
		{
			mp_raise_ValueError(MP_ERROR_TEXT("initialization failed"));
		}
	}
	
	if(thread_id == 0)
	{
		Helios_ThreadAttr ThreadAttr = 
		{
	        .name = "machine_extint",
	        .stack_size = 2048,
	        .priority = 92,
	        .entry = extint_entry,
	        .argv = NULL
	    };
		thread_id = Helios_Thread_Create(&ThreadAttr);
		if(thread_id == 0)
		{
			mp_raise_ValueError(MP_ERROR_TEXT("initialization failed"));
		}
	}

#endif
	extint_count_reset(self->line);
    return MP_OBJ_FROM_PTR(self);
}

static void extint_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ExtInt line=%u>", self->line);
}

static mp_obj_t extint_obj_read_count(mp_obj_t self_in, mp_obj_t is_reset) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
	int reset_flag = mp_obj_get_int(is_reset);

	if(reset_flag != 0 && reset_flag != 1) {
		mp_raise_ValueError(MP_ERROR_TEXT("invalid is_reset value, must in [0,1]"));
	} 

#if defined(BOARD_EG810MCN_GA_ALIPAY) || defined(BOARD_EG810MCN_GA_VOLTE)
	int count = 0;
	if(__fastIntGpio == self->line) {
		count += Helios_ExtInt_FastGetCnt();
		if(extint_obj[self->line]->mode == HELIOS_EDGE_RISING){
			extint_count[self->line].rising_count += count;
		} else if(extint_obj[self->line]->mode == HELIOS_EDGE_FALLING) {
			extint_count[self->line].falling_count += count;
		} else {
			if(count % 2 == 0) {
				extint_count[self->line].rising_count += count / 2;
				extint_count[self->line].falling_count += count / 2;
			} else {
				if(Helios_GPIO_GetLevel(self->line) == 0) {
					extint_count[self->line].rising_count++;
				} else {
					extint_count[self->line].falling_count++;
				}
			}
		}
	}
#endif
	
	mp_obj_t extint_list[2] = { 
			mp_obj_new_int(extint_count[self->line].rising_count),
			mp_obj_new_int(extint_count[self->line].falling_count),
	};
			
	if(1 == reset_flag) {
		extint_count_reset(self->line);
	}
    return mp_obj_new_list(2, extint_list);
}
static MP_DEFINE_CONST_FUN_OBJ_2(extint_obj_read_count_obj, extint_obj_read_count);

static mp_obj_t extint_obj_count_reset(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
	extint_count_reset(self->line);

	return mp_obj_new_int(0);
	
}
static MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_count_reset_obj, extint_obj_count_reset);

static mp_obj_t extint_obj_Deinit(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
	int ret = -1;
	//EXTINT_LOG("extint deinit");

	extint_obj[self->line] = NULL;
	extint_count_reset(self->line);

	if((0 == Helios_ExtInt_Disable((Helios_GPIONum) self->line)) && (0 == Helios_ExtInt_Deinit((Helios_GPIONum) self->line))) {
		ret = 0;
	}
	
	return mp_obj_new_int(ret);
	
}
static MP_DEFINE_CONST_FUN_OBJ_1(extint__del__obj, extint_obj_Deinit);

static mp_obj_t extint_obj_read_level(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
	int ret = -1;
	
	ret = Helios_GPIO_GetLevel((Helios_GPIONum) self->line);
	
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_read_level_obj, extint_obj_read_level);


static const mp_rom_map_elem_t extint_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), 	MP_ROM_PTR(&extint__del__obj) },
    { MP_ROM_QSTR(MP_QSTR_line),    MP_ROM_PTR(&extint_obj_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),  MP_ROM_PTR(&extint_obj_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&extint_obj_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_count), MP_ROM_PTR(&extint_obj_read_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_count_reset), MP_ROM_PTR(&extint_obj_count_reset_obj) },
	{ MP_ROM_QSTR(MP_QSTR_read_level), MP_ROM_PTR(&extint_obj_read_level_obj)},

    // class constants
    /// \constant IRQ_RISING - interrupt on a rising edge
    /// \constant IRQ_FALLING - interrupt on a falling edge
    /// \constant IRQ_RISING_FALLING - interrupt on a rising or falling edge
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING),         MP_ROM_INT(HELIOS_EDGE_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING),        MP_ROM_INT(HELIOS_EDGE_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING_FALLING), MP_ROM_INT(HELIOS_EDGE_BOTH) },
	{ MP_ROM_QSTR(MP_QSTR_PULL_DISABLE), MP_ROM_INT(HELIOS_PULL_NONE) },
	{ MP_ROM_QSTR(MP_QSTR_PULL_PU),   MP_ROM_INT(HELIOS_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_PD), MP_ROM_INT(HELIOS_PULL_DOWN) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO0), MP_ROM_INT(HELIOS_GPIO0) },
#if defined(PLAT_Qualcomm)
#if defined(BOARD_BG95M1) || defined(BOARD_BG95M2) || defined(BOARD_BG95M3) \
 || defined(BOARD_BG95M8) || defined(BOARD_BG95M9) || defined(BOARD_BG95M3_LX) \
 || defined(BOARD_BG95M6) || defined(BOARD_BG95M8_SANX)
	{ MP_ROM_QSTR(MP_QSTR_GPIO2), MP_ROM_INT(HELIOS_GPIO2) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO3), MP_ROM_INT(HELIOS_GPIO3) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO6), MP_ROM_INT(HELIOS_GPIO6) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO7), MP_ROM_INT(HELIOS_GPIO7) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO8), MP_ROM_INT(HELIOS_GPIO8) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO9), MP_ROM_INT(HELIOS_GPIO9) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO11), MP_ROM_INT(HELIOS_GPIO11) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO12), MP_ROM_INT(HELIOS_GPIO12) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO14), MP_ROM_INT(HELIOS_GPIO14) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO16), MP_ROM_INT(HELIOS_GPIO16) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO17), MP_ROM_INT(HELIOS_GPIO17) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO18), MP_ROM_INT(HELIOS_GPIO18) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO19), MP_ROM_INT(HELIOS_GPIO19) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO20), MP_ROM_INT(HELIOS_GPIO20) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO21), MP_ROM_INT(HELIOS_GPIO21) },

	{ MP_ROM_QSTR(MP_QSTR_GPIO22), MP_ROM_INT(HELIOS_GPIO22) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO23), MP_ROM_INT(HELIOS_GPIO23) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO24), MP_ROM_INT(HELIOS_GPIO24) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO25), MP_ROM_INT(HELIOS_GPIO25) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO26), MP_ROM_INT(HELIOS_GPIO26) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO29), MP_ROM_INT(HELIOS_GPIO29) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO30), MP_ROM_INT(HELIOS_GPIO30) },
#elif defined(BOARD_BG77)
    { MP_ROM_QSTR(MP_QSTR_GPIO1), MP_ROM_INT(HELIOS_GPIO1) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO2), MP_ROM_INT(HELIOS_GPIO2) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO4), MP_ROM_INT(HELIOS_GPIO4) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO5), MP_ROM_INT(HELIOS_GPIO5) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO8), MP_ROM_INT(HELIOS_GPIO8) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO10), MP_ROM_INT(HELIOS_GPIO10) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO13), MP_ROM_INT(HELIOS_GPIO13) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO14), MP_ROM_INT(HELIOS_GPIO14) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO17), MP_ROM_INT(HELIOS_GPIO17) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO20), MP_ROM_INT(HELIOS_GPIO20) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO21), MP_ROM_INT(HELIOS_GPIO21) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO22), MP_ROM_INT(HELIOS_GPIO22) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO25), MP_ROM_INT(HELIOS_GPIO25) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO28), MP_ROM_INT(HELIOS_GPIO28) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO31), MP_ROM_INT(HELIOS_GPIO31) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO33), MP_ROM_INT(HELIOS_GPIO33) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO34), MP_ROM_INT(HELIOS_GPIO34) },

	{ MP_ROM_QSTR(MP_QSTR_GPIO35), MP_ROM_INT(HELIOS_GPIO35) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO36), MP_ROM_INT(HELIOS_GPIO36) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO39), MP_ROM_INT(HELIOS_GPIO39) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO40), MP_ROM_INT(HELIOS_GPIO40) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO42), MP_ROM_INT(HELIOS_GPIO42) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO43), MP_ROM_INT(HELIOS_GPIO43) },
#elif defined(BOARD_BG600LM3)
    { MP_ROM_QSTR(MP_QSTR_GPIO1), MP_ROM_INT(HELIOS_GPIO1) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO2), MP_ROM_INT(HELIOS_GPIO2) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO3), MP_ROM_INT(HELIOS_GPIO3) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO6), MP_ROM_INT(HELIOS_GPIO6) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO7), MP_ROM_INT(HELIOS_GPIO7) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO8), MP_ROM_INT(HELIOS_GPIO8) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO10), MP_ROM_INT(HELIOS_GPIO10) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO11), MP_ROM_INT(HELIOS_GPIO11) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO15), MP_ROM_INT(HELIOS_GPIO15) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO16), MP_ROM_INT(HELIOS_GPIO16) },

	{ MP_ROM_QSTR(MP_QSTR_GPIO17), MP_ROM_INT(HELIOS_GPIO17) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO18), MP_ROM_INT(HELIOS_GPIO18) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO19), MP_ROM_INT(HELIOS_GPIO19) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO20), MP_ROM_INT(HELIOS_GPIO20) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO23), MP_ROM_INT(HELIOS_GPIO23) },
	{ MP_ROM_QSTR(MP_QSTR_GPIO25), MP_ROM_INT(HELIOS_GPIO25) },
#endif
#else
#if !defined(PLAT_ECR6600)
	PLAT_GPIO_DEF(PLAT_GPIO_NUM),
#endif
#endif
};

static MP_DEFINE_CONST_DICT(extint_locals_dict, extint_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_extint_type,
    MP_QSTR_ExtInt,
    MP_TYPE_FLAG_NONE,
    make_new, extint_make_new,
    print, extint_obj_print,
    locals_dict, &extint_locals_dict
    );
#endif
