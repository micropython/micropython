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
#include "runtime.h"

#if MICROPY_QPY_MISC_PWM

#include "helios_pwm.h"
#include "helios_debug.h"


#define	PWM_LOG(msg, ...)      custom_log("pwm", msg, ##__VA_ARGS__)



// Forward dec'l
const mp_obj_type_t misc_pwm_type;
const mp_obj_type_t misc_pwm_v2_type;

typedef enum
{
    PWM_V1=0,
    PWM_V2
}pwm_type_t;

typedef struct _misc_pwm_obj_t {
    mp_obj_base_t base;
    unsigned int pin;
	unsigned int cycle_range;
    unsigned short high_time;
    unsigned short cycle_time;
    unsigned char duty;
    float frequency;
    pwm_type_t type_pwm; 
#if defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)
    unsigned char pwm_en_sleep;
#endif
} misc_pwm_obj_t;

typedef enum HELIOS_PWM_CYCLE_RANGE_ENUM
{
    HELIOS_PWM_CYCLE_ABOVE_1US,
    HELIOS_PWM_CYCLE_ABOVE_1MS,
    HELIOS_PWM_CYCLE_ABOVE_10US,
    HELIOS_PWM_CYCLE_ABOVE_BELOW_US,
} HELIOS_PWM_CYCLE_RANGE_E;

static misc_pwm_obj_t *misc_pwm_obj[PWMMAX] = {NULL};

static void misc_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    misc_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(pin:%u high time:%u cycle_time:%u)", self->pin, self->high_time, self->cycle_time);
}


typedef struct {
	uint16_t Left_coordinate;
	uint16_t Right_coordinata;
}Helios_PWM_range;


//helios_pwm[0]:1US
//helios_pwm[1]:1ms
//helios_pwm[2]:10us
//helios_pwm[3]:ns
#if defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609)  || defined(PLAT_ASR_1602)
const Helios_PWM_range helios_pwm[4] = {{0, 157}, {0, 1023}, {1, 1575}, {0, 1024}};

#elif defined(PLAT_Unisoc) || defined(PLAT_Unisoc_8850) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_ECR6600) || defined(PLAT_Unisoc_8850_R02) || defined(PLAT_Unisoc_8910_R06)
const Helios_PWM_range helios_pwm[4] = {{0, 10000}, {0, 10}, {1, 10000}, {99, 65535}};
#elif defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)
#elif defined(PLAT_EIGEN)
#elif defined(PLAT_Qualcomm)
#elif defined(PLAT_SONY_ALT1350)

#else
#error "Please check whether the platform supports PWM"
#endif


#if !defined(PLAT_ASR_1803s) && !defined(PLAT_ASR_1803sc) && !defined(PLAT_EIGEN) && !defined(PLAT_Qualcomm) && !defined(PLAT_SONY_ALT1350)
static int check_pwm_para(uint16_t cycle_range, uint16_t cycle_time, uint16_t hight_time) {

	if(cycle_range > HELIOS_PWM_CYCLE_ABOVE_BELOW_US ||
		hight_time <= helios_pwm[cycle_range].Left_coordinate || hight_time > helios_pwm[cycle_range].Right_coordinata || 
		cycle_time <= helios_pwm[cycle_range].Left_coordinate || cycle_time > helios_pwm[cycle_range].Right_coordinata || 
		hight_time > cycle_time) {
		return -1;
	}
	return 0;	
}
#endif
static void misc_pwm_init_helper(misc_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_cycle_range, ARG_high_time, ARG_cycle_time };
    static const mp_arg_t allowed_args[] = {
    	{ MP_QSTR_cycle_range, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_high_time,   MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_cycle_time,  MP_ARG_INT, {.u_int = 0} },
#if defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)
        { MP_QSTR_pwm_en_sleep,  MP_ARG_INT, {.u_int = 0} },
#endif
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	//PWM_LOG("cycle_range=%d, high_time=%d, cycle_time=%d\n",args[ARG_cycle_range].u_int, args[ARG_high_time].u_int, args[ARG_cycle_time].u_int);
#if !(defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) \
    || defined(PLAT_EIGEN) || defined(PLAT_Qualcomm) || defined(PLAT_ECR6600) || defined(PLAT_ASR_1602) || defined(PLAT_SONY_ALT1350))
	if(-1 == check_pwm_para(args[ARG_cycle_range].u_int, args[ARG_cycle_time].u_int, args[ARG_high_time].u_int)) {
		mp_raise_ValueError(MP_ERROR_TEXT("PWM parameter error, please refer to wiki instructions to correct the parameters"));
	}
#endif

	self->cycle_range = args[ARG_cycle_range].u_int;
	self->high_time = args[ARG_high_time].u_int;
	self->cycle_time = args[ARG_cycle_time].u_int;
    self->type_pwm = PWM_V1;
	PWM_LOG("cycle_range=%d, high_time=%d, cycle_time=%d\n",self->cycle_range, self->high_time, self->cycle_time);

	if(Helios_PWM_Init(self->pin) != 0) {
		mp_raise_ValueError(MP_ERROR_TEXT("fail"));
	}

#if defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)
	Helios_PWM_En_Sleep(self->pin,args[3].u_int);
    PWM_LOG("Helios_PWM_En_Sleep->%d\n",args[3].u_int);
#endif
}

static mp_obj_t misc_pwm_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 4, 5, true);
    int pin_id = mp_obj_get_int(args[0]);

	PWM_LOG("n_args = %d\n",n_args);

	if ((pin_id < 0) || (pin_id > PWMMAX-1))
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid PWMn value, must be in {0~3}."));
	}
	
    // create PWM object from the given pin
    if (misc_pwm_obj[pin_id] == NULL)
    {
    	misc_pwm_obj[pin_id] = mp_obj_malloc_with_finaliser(misc_pwm_obj_t, &misc_pwm_type);
    }
    misc_pwm_obj_t *self = misc_pwm_obj[pin_id];
    self->base.type = &misc_pwm_type;
    self->pin = pin_id;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
	misc_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t misc_pwm_init(size_t n_args,
    const mp_obj_t *args, mp_map_t *kw_args) {
    misc_pwm_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(misc_pwm_init_obj, 1, misc_pwm_init);
    
//pwm v2 init by pass duty and frequency 20220624 @jimmy
static void misc_pwm_v2_init_helper(misc_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) 
{
    enum {ARG_frequency,ARG_duty};
    static const mp_arg_t allowed_args[] = 
    {
        { MP_QSTR_frequency, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_duty, MP_ARG_INT, {.u_int = 0} },
#if defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)
        { MP_QSTR_pwm_en_sleep,  MP_ARG_INT, {.u_int = 0} },
#endif
    };
        
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
    MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(mp_obj_is_float(args[ARG_frequency].u_obj) == 0)
    {
        mp_raise_TypeError(MP_ERROR_TEXT("frequency type isn't float"));
    }
     if(args[ARG_duty].u_int > 100)
     {
         mp_raise_ValueError(MP_ERROR_TEXT("PWM parameter error,range duty should be 0~100"));
     }
	self->duty = args[ARG_duty].u_int;
	self->frequency = mp_obj_float_get(args[ARG_frequency].u_obj);
    self->type_pwm = PWM_V2;

	if(Helios_PWM_Init(self->pin) != 0) {
		mp_raise_ValueError(MP_ERROR_TEXT("fail"));
	}
#if defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc)
	Helios_PWM_En_Sleep(self->pin,args[2].u_int);
    PWM_LOG("Helios_PWM_En_Sleep->%d\n",args[2].u_int);
#endif

}    
static mp_obj_t misc_pwm_v2_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) 
{
    mp_arg_check_num(n_args, n_kw, 3, 4, true);
    int pin_id = mp_obj_get_int(args[0]);
    
	if ((pin_id < 0) || (pin_id > PWMMAX-1))
	{
		mp_raise_ValueError(MP_ERROR_TEXT("invalid PWMn value, must be in {0~3}."));
	}
	
    // create PWM object from the given pin
    if (misc_pwm_obj[pin_id] == NULL)
    {
    	misc_pwm_obj[pin_id] = mp_obj_malloc_with_finaliser(misc_pwm_obj_t, &misc_pwm_type);
    }
    misc_pwm_obj_t *self = misc_pwm_obj[pin_id];
    self->base.type = &misc_pwm_v2_type;
    self->pin = pin_id;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
	misc_pwm_v2_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t misc_pwm_enable(size_t n_args, const mp_obj_t *args) 
{
    misc_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
	if(self->type_pwm == PWM_V1)
	{   
    	PWM_LOG("cycle_range=%d, high_time=%d, cycle_time=%d\n",self->cycle_range, self->high_time, self->cycle_time);
    	double frequency = 0;
    	float duty = 0;
        if(n_args == 4)
        {
#if !(defined(PLAT_ASR) || defined(PLAT_ASR_1606) || defined(PLAT_ASR_1609) || defined(PLAT_ASR_1803s) || defined(PLAT_ASR_1803sc) \
    || defined(PLAT_EIGEN) || defined(PLAT_Qualcomm) || defined(PLAT_ECR6600) || defined(PLAT_ASR_1602) || defined(PLAT_SONY_ALT1350))
            if(-1 == check_pwm_para(mp_obj_get_int(args[1]), mp_obj_get_int(args[3]), mp_obj_get_int(args[2])))
            {
		        mp_raise_ValueError(MP_ERROR_TEXT("PWM parameter error, please refer to wiki instructions to correct the parameters"));
	        }
#endif
            self->cycle_range = mp_obj_get_int(args[1]);
            self->cycle_time = mp_obj_get_int(args[3]);
            self->high_time = mp_obj_get_int(args[2]);
        }
        
    	if(self->cycle_range == HELIOS_PWM_CYCLE_ABOVE_1US || self->cycle_range == HELIOS_PWM_CYCLE_ABOVE_10US) 
        {
    		frequency = 1000000 /(double)self->cycle_time;
    	} 
        else if (self->cycle_range == HELIOS_PWM_CYCLE_ABOVE_1MS) 
        {
    		frequency = 1000 /(double)self->cycle_time;
    	} 
        else if(self->cycle_range == HELIOS_PWM_CYCLE_ABOVE_BELOW_US) 
        {
    		frequency = 1000000000 /(double)self->cycle_time;
    	}
    	duty = (float)self->high_time / (float)self->cycle_time;
            
    	PWM_LOG("misc_pwm_enable = %lf %f\n",frequency, duty);
    	
    	int ret = Helios_PWM_Start(self->pin, frequency, duty);

        return mp_obj_new_int(ret);
	}
    else
    {
        if(n_args == 3)
        {
             if((mp_obj_get_int(args[2]) < 0) || (mp_obj_get_int(args[2]) > 100))
             {
                 mp_raise_ValueError(MP_ERROR_TEXT("PWM parameter error,range duty should be 0~100"));
             }
             if(mp_obj_is_float(args[1]) == 0)
             {
                 mp_raise_TypeError(MP_ERROR_TEXT("frequency type isn't float"));
             }
             self->frequency = mp_obj_float_get(args[1]);
             self->duty = mp_obj_get_int(args[2]);
        }
        PWM_LOG("duty=%d, frequency=%f\n",self->duty, self->frequency);
	
	    int ret = Helios_PWM_Start(self->pin, self->frequency, (self->duty*1.0)/100);

        return mp_obj_new_int(ret);
    }
}

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(misc_pwm_enable_obj, 1, 4, misc_pwm_enable);

static mp_obj_t misc_pwm_disable(mp_obj_t self_in) 
{
    misc_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);

	int ret = Helios_PWM_Stop(self->pin);
	if (ret != 0)
	{
		ret = -1;
	}

    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(misc_pwm_disable_obj, misc_pwm_disable);

static mp_obj_t misc_pwm_deinit(mp_obj_t self_in) 
{
    misc_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
	
	misc_pwm_obj[self->pin] = NULL;
	
	int ret = Helios_PWM_Deinit((Helios_PwnNum)self->pin);
	if (ret != 0)
	{
		PWM_LOG("pwm%d deinit failed.\r\n", self->pin);
	}

	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(misc_pwm_deinit_obj, misc_pwm_deinit);

static const mp_rom_map_elem_t misc_pwm_locals_dict_table[] = {
    // { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&misc_pwm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&misc_pwm_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&misc_pwm_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&misc_pwm_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_ABOVE_1US), MP_ROM_INT(HELIOS_PWM_CYCLE_ABOVE_1US) },
    { MP_ROM_QSTR(MP_QSTR_ABOVE_MS), MP_ROM_INT(HELIOS_PWM_CYCLE_ABOVE_1MS) },
    { MP_ROM_QSTR(MP_QSTR_ABOVE_10US), MP_ROM_INT(HELIOS_PWM_CYCLE_ABOVE_10US) },
    { MP_ROM_QSTR(MP_QSTR_ABOVE_BELOW_US), MP_ROM_INT(HELIOS_PWM_CYCLE_ABOVE_BELOW_US) },
    { MP_ROM_QSTR(MP_QSTR_PWM0), MP_ROM_INT(PWM0) },
    #if !defined(PLAT_Unisoc) || defined(PLAT_Unisoc_8910_R05) || defined(PLAT_Unisoc_8910_R06) || defined(PLAT_SONY_ALT1350)
    PLAT_PWM_DEF(PLAT_PWM_NUMMAX),
    #endif
};

static MP_DEFINE_CONST_DICT(misc_pwm_locals_dict,
    misc_pwm_locals_dict_table);


MP_DEFINE_CONST_OBJ_TYPE(
    misc_pwm_type,
    MP_QSTR_PWM,
    MP_TYPE_FLAG_NONE,
    make_new, misc_pwm_make_new,
    locals_dict, &misc_pwm_locals_dict
    );

MP_DEFINE_CONST_OBJ_TYPE(
    misc_pwm_v2_type,
    MP_QSTR_PWM_V2,
    MP_TYPE_FLAG_NONE,
    make_new, misc_pwm_v2_make_new,
    locals_dict, &misc_pwm_locals_dict
    );

#endif /* MICROPY_QPY_MISC_PWM */

