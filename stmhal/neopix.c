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


/// \classmethod \constructor(pin)
///
/// Construct an Neopix object. Needs a pin object as input. Pin has to have a TIM15 CH1N output
STATIC mp_obj_t pyb_neopix_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // create object
    pyb_neopix_obj_t *neo = m_new_obj(pyb_neopix_obj_t);
    neo->base.type = &pyb_neopix_type;
	
	GPIO_InitTypeDef GPIO_InitStruct;
/*
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
*/

	mp_obj_t pin_obj = args[0];
	if (!MP_OBJ_IS_TYPE(pin_obj, &pin_type)) {
		nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "pin argument needs to be be a Pin type"));
	}
	const pin_obj_t *pin = pin_obj;
	const pin_af_obj_t *af = pin_find_af(pin, AF_FN_TIM, 15);
	if (af == NULL) {
		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %q doesn't have an af for TIM%d", pin->name, 15));
	}
	const mp_obj_t args2[6] = {
		(mp_obj_t)&pin_init_obj,
		pin_obj,
		MP_OBJ_NEW_QSTR(MP_QSTR_mode),  MP_OBJ_NEW_SMALL_INT(GPIO_MODE_AF_PP),
		MP_OBJ_NEW_QSTR(MP_QSTR_af),    MP_OBJ_NEW_SMALL_INT(af->idx)
	};
	mp_call_method_n_kw(0, 2, args2);
	
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
	oc_config.OCNPolarity  = TIM_OCNPOLARITY_LOW;
	oc_config.OCFastMode   = TIM_OCFAST_DISABLE;
	oc_config.OCIdleState  = TIM_OCIDLESTATE_SET;
	oc_config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	
	HAL_TIM_PWM_ConfigChannel(&tim, &oc_config, 0); //TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&tim, 0);
	HAL_TIMEx_PWMN_Start(&tim, 0);
	
	
	return neo;
}


/// \method display()
///
/// Takes an array of RGB values, or a single one.
/// Uses the 0xRRGGBB format
STATIC mp_obj_t pyb_neopix_display(mp_obj_t self_in, mp_obj_t rgb) {
	pyb_neopix_obj_t *self = self_in;
	
	int len;
	int tx;
	int mask;
	int val;
	
	mp_obj_t *items;
	
	if (MP_OBJ_IS_INT(rgb))
		len = 1;
	else
		mp_obj_get_array(rgb, &len, &items);
	
	//this is obviously a bit crap, needs replacing with DMA
	__disable_irq();
	
	while(len){
		len--;
		
		if (MP_OBJ_IS_INT(rgb))
			val = mp_obj_get_int(rgb);
		else
			val = mp_obj_get_int(items[len]);
		
		mask = (1<<23);
		tx = ((val & 0xFF00) << 8) | ((val & 0xFF0000) >> 8) | (val & 0xFF);
		while (mask){
			
			tim.Instance->SR =  ~TIM_SR_CC1IF;
			while (!(TIM15->SR & TIM_SR_CC1IF));
			
			if (mask & tx)
				tim.Instance->CCR1 = 64;
			else
				tim.Instance->CCR1 = 32;
			
			
				
			mask = mask >> 1;
		}
	}
	
	tim.Instance->SR =  ~TIM_SR_CC1IF;
	while (!(TIM15->SR & TIM_SR_CC1IF));
	
	tim.Instance->CCR1 = 0;
	
	__enable_irq();
	
	
	
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_neopix_display_obj, pyb_neopix_display);

/// \method destroy()
///
/// Stops the timer
STATIC mp_obj_t pyb_neopix_destroy(mp_obj_t self_in) {
	tim.State = HAL_TIM_STATE_RESET;
	tim.Instance->CCER = 0x0000; // disable all capture/compare outputs
	tim.Instance->CR1 = 0x0000; // disable the timer and reset its state
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_neopix_destroy_obj, pyb_neopix_destroy);	

STATIC const mp_map_elem_t pyb_neopix_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&pyb_neopix_destroy_obj},    
    { MP_OBJ_NEW_QSTR(MP_QSTR_display), (mp_obj_t)&pyb_neopix_display_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_destroy), (mp_obj_t)&pyb_neopix_destroy_obj },

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
