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

TIM_HandleTypeDef htim15;

static void neo_timer_init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 100;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 110;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim15);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim15);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig);

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 20;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1);
HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
TIM15->CR1 |= TIM_CR1_CEN;
}




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
	 
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_TIM15;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	neo_timer_init();

	return neo;
}


/// \method display()
///
STATIC mp_obj_t pyb_neopix_display(mp_obj_t self_in, mp_obj_t rgb) {
	pyb_neopix_obj_t *self = self_in;
	int val = mp_obj_get_int(rgb);
	


	TIM15->CCR1 = 30;
	HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
	
	while (!(TIM15->SR & TIM_SR_CC1IF));
	TIM15->CCR1 = 60;
	
	
	while (!(TIM15->SR & TIM_SR_CC1IF));
	TIM15->CCR1 = 90;
	
	while (!(TIM15->SR & TIM_SR_CC1IF));
	TIM15->CCR1 = 45;
	
	while (!(TIM15->SR & TIM_SR_CC1IF));
	HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_1);
	
	
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
