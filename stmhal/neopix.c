/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>
#include <math.h>

#include STM32_HAL_H

#include "py/nlr.h"
#include "py/runtime.h"

#if MICROPY_HW_HAS_NEOPIX

#include "pin.h"

#include "genhdr/pins.h"
#include "bufhelper.h"
#include "neopix.h"

#define MICROPY_HW_NEOPIX_PORT GPIOA
#define MICROPY_HW_NEOPIX_PIN GPIO_PIN_2

/// \moduleref Neopix
/// UPDATE ME

TIM_HandleTypeDef tim;



typedef struct _pyb_neopix_t {
    mp_obj_base_t base;
} pyb_neopix_obj_t;


/// \classmethod \constructor()
///
/// Construct an Container object. Need to call .show() after creation
STATIC mp_obj_t pyb_neopix_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // create object
    pyb_neopix_obj_t *neo = m_new_obj(pyb_neopix_obj_t);
    neo->base.type = &pyb_neopix_type;
	
	GPIO_InitTypeDef GPIO_InitStruct;
	     
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_TIM15;
	GPIO_InitStruct.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	
	tim.Instance = TIM15;
	tim.Init.Prescaler = 0;
	tim.Init.Period = 99;
	tim.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	tim.Init.RepetitionCounter = 0;
	__TIM15_CLK_ENABLE();

	
	HAL_TIM_Base_Init(&tim);

	
	TIM_OC_InitTypeDef oc_config;
	oc_config.OCMode = TIM_OCMODE_PWM1;
	oc_config.Pulse = 0;
	
	oc_config.OCPolarity   = TIM_OCPOLARITY_HIGH;
	oc_config.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	oc_config.OCFastMode   = TIM_OCFAST_DISABLE;
	oc_config.OCIdleState  = TIM_OCIDLESTATE_SET;
	oc_config.OCNIdleState = TIM_OCNIDLESTATE_SET;
	
	HAL_TIM_PWM_ConfigChannel(&tim, &oc_config, 0); //TIM_CHANNEL_1);
	//HAL_TIM_PWM_Start(&tim, 0);
	HAL_TIMEx_PWMN_Start(&tim, 0);
	
	
	return neo;
}


/// \method display()
///
STATIC mp_obj_t pyb_neopix_display(mp_obj_t self_in, mp_obj_t rgb) {
	pyb_neopix_obj_t *self = self_in;
	int val = mp_obj_get_int(rgb);
	
	int tx = ((val & 0xFF00) << 8) | ((val & 0xFF0000) >> 8) | (val & 0xFF);

	int mask = (1<<23);
	
	int c = 24+1;
	
	while (mask){
		
		if (mask & tx)
			tim.Instance->CCR1 = 64;
		else
			tim.Instance->CCR1 = 32;
		
		tim.Instance->SR =  ~TIM_SR_CC1IF;
		while (!(TIM15->SR & TIM_SR_CC1IF));
			
		mask = mask >> 1;
	}
	
	tim.Instance->CCR1 = 0;
	
	
	
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_neopix_display_obj, pyb_neopix_display);


STATIC const mp_map_elem_t pyb_neopix_locals_dict_table[] = {
    // instance methods
    //{ MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&pyb_container_destroy_obj},
   
    
    { MP_OBJ_NEW_QSTR(MP_QSTR_display), (mp_obj_t)&pyb_neopix_display_obj },

	//class constants
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_RED),        MP_OBJ_NEW_SMALL_INT(Red) },

};

STATIC MP_DEFINE_CONST_DICT(pyb_neopix_locals_dict, pyb_neopix_locals_dict_table);

const mp_obj_type_t pyb_neopix_type = {
    { &mp_type_type },
    .name = MP_QSTR_Neopix,
    .make_new = pyb_neopix_make_new,
    .locals_dict = (mp_obj_t)&pyb_neopix_locals_dict,
};


#endif // MICROPY_HW_HAS_NEOPIX
