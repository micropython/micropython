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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <stm32f4xx_hal.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "gc.h"
#include "obj.h"
#include "objtuple.h"
#include "runtime.h"
#include "timer.h"
#include "timer_pwm.h"

// mode = PWM       HAL_TIM_PWM_ConfigChannel
// 
// channel          1..4
// pwm_mode         PWM1, PWM2
// pulse            0x0000 .. 0xffff
// oc_polarity      high, low
// oc_npolarity     high, low
// oc_fastmode      disable, enable
// oc_idlestate     set, reset
// oc_nidlestate    set, reset

STATIC void pyb_pwm_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_obj_t *self = self_in;

    if (self->tim.State == HAL_TIM_STATE_RESET) {
        print(env, "PWM(%u)", self->tim_id);
    } else {
        print(env, "PWM(%u, prescaler=%u, period=%u, mode=%u, div=%u)",
            self->tim_id,
            self->tim.Init.Prescaler,
            self->tim.Init.Period,
            self->tim.Init.CounterMode,
            self->tim.Init.ClockDivision
        );
    }
}

STATIC const mp_arg_t pyb_pwm_init_args[] = {
    TIMER_BASE_ARGS
    { MP_QSTR_channel,          MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_oc_mode,          MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_OCMODE_PWM1} },
    { MP_QSTR_pulse,            MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_oc_polarity,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_OCPOLARITY_HIGH} },
    { MP_QSTR_ocn_polarity,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_OCNPOLARITY_HIGH} },
    { MP_QSTR_oc_fast_mode,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_OCFAST_DISABLE} },
    { MP_QSTR_oc_idle_state,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_OCIDLESTATE_RESET} },
    { MP_QSTR_ocn_idle_state,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_OCNIDLESTATE_RESET} },
};
#define PYB_PWM_INIT_NUM_ARGS MP_ARRAY_SIZE(pyb_pwm_init_args)

STATIC void pyb_pwm_init_helper(pyb_pwm_obj_t *self, uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_PWM_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_PWM_INIT_NUM_ARGS, pyb_pwm_init_args, vals);

    pyb_timer_base_init_helper(&self->timer, vals);

    self->channel = vals[5].u_int;
    
    self->pwm_config.OCMode = vals[6].u_int; 
    if (!IS_TIM_PWM_MODE(self->pwm_config.OCMode)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid pwm_mode: %d", self->pwm_config.OCMode));
    }

    self->pwm_config.Pulse = vals[7].u_int;
    self->pwm_config.OCPolarity = vals[8].u_int;
    if (!IS_TIM_OC_POLARITY(self->pwm_config.OCPolarity)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid oc_polarity: %d", self->pwm_config.OCPolarity));
    }

    self->pwm_config.OCNPolarity = vals[9].u_int;
    if (!IS_TIM_OCN_POLARITY(self->pwm_config.OCNPolarity)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid ocn_polarity: %d", self->pwm_config.OCNPolarity));
    }

    self->pwm_config.OCFastMode = vals[10].u_int;
    if (!IS_TIM_FAST_STATE(self->pwm_config.OCFastMode)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid oc_fast_mode: %d", self->pwm_config.OCMode));
    }
    self->pwm_config.OCIdleState = vals[11].u_int;
    if (!IS_TIM_OCIDLE_STATE(self->pwm_config.OCIdleState)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid oc_idle_state: %d", self->pwm_config.OCIdleState));
    }

    self->pwm_config.OCNIdleState = vals[12].u_int;
    if (!IS_TIM_OCNIDLE_STATE(self->pwm_config.OCNIdleState)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid ocn_idle_state: %d", self->pwm_config.OCNIdleState));
    }

    HAL_TIM_PWM_Init(&self->timer.tim);
    HAL_TIM_PWM_ConfigChannel(&self->timer.tim, &self->pwm_config, self->channel);
    HAL_TIM_PWM_Start(&self->timer.tim, self->channel);
}

STATIC mp_obj_t pyb_pwm_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    printf("pyb_pwm_make_new called\n");
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // create new Timer object
    pyb_pwm_obj_t *pwm = m_new_obj(pyb_pwm_obj_t);
    pwm->timer.base.type = &pyb_pwm_type;

    pyb_timer_init_new(&pwm->timer, mp_obj_get_int(args[0]));

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_pwm_init_helper(pwm, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pwm;
}

STATIC mp_obj_t pyb_pwm_init(uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    printf("pyb_pwm_init called\n");
    pyb_pwm_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_pwm_init_obj, 1, pyb_pwm_init);

STATIC mp_obj_t pyb_pwm_deinit(mp_obj_t self_in) {
    printf("pyb_pwm_deinit called\n");
    pyb_pwm_obj_t *self = self_in;

    // Disable the interrupt
    pyb_timer_callback(self_in, mp_const_none);

    HAL_TIM_PWM_DeInit(&self->timer.tim);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_pwm_deinit_obj, 1, pyb_pwm_deinit);

STATIC mp_obj_t pyb_pwm_pulse(uint n_args, const mp_obj_t *args) {
    printf("pyb_pwm_pulse called\n");
    pyb_pwm_obj_t *self = args[0];
    if (n_args == 1) {
        // get
        return mp_obj_new_int(__HAL_TIM_GetCompare(&self->timer.tim, self->channel));
    }
    // set
    __HAL_TIM_SetCompare(&self->timer.tim, self->channel, mp_obj_get_int(args[1]));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pwm_pulse_obj, 1, 2, pyb_pwm_pulse);


STATIC const mp_obj_tuple_t pyb_pwm_base_tuple = {
  {&mp_type_tuple}, 2, {(mp_obj_t)&pyb_timer_type, (mp_obj_t)&mp_type_type} 
};

STATIC const mp_map_elem_t pyb_pwm_locals_dict_table[] = {
  { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_pwm_init_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_pulse), (mp_obj_t)&pyb_pwm_pulse_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_pwm_deinit_obj },

  { MP_OBJ_NEW_QSTR(MP_QSTR_counter), (mp_obj_t)&pyb_timer_counter_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_prescaler), (mp_obj_t)&pyb_timer_prescaler_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_period), (mp_obj_t)&pyb_timer_period_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&pyb_timer_callback_obj },
};
STATIC MP_DEFINE_CONST_DICT(pyb_pwm_locals_dict, pyb_pwm_locals_dict_table);

const mp_obj_type_t pyb_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = pyb_pwm_print,
    .make_new = pyb_pwm_make_new,
    .bases_tuple = (mp_obj_t)&pyb_pwm_base_tuple,
    .locals_dict = (mp_obj_t)&pyb_pwm_locals_dict,
};
