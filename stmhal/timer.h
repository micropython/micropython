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

// Periodically, the state of the buffer "UserTxBuffer" is checked.
// The period depends on USBD_CDC_POLLING_INTERVAL
// The value is in ms. The max is 65 and the min is 1.
#define USBD_CDC_POLLING_INTERVAL (10)

typedef struct _pyb_timer_obj_t {
    mp_obj_base_t base;
    mp_uint_t tim_id;
    mp_obj_t callback;
    TIM_HandleTypeDef tim;
    IRQn_Type irqn;
} pyb_timer_obj_t;

void pyb_timer_init_new(pyb_timer_obj_t *tim, mp_uint_t tim_id);

extern TIM_HandleTypeDef TIM3_Handle;
extern TIM_HandleTypeDef TIM5_Handle;
extern TIM_HandleTypeDef TIM6_Handle;

extern const mp_obj_type_t pyb_timer_type;

#define TIMER_BASE_ARGS \
    { MP_QSTR_freq,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} }, \
    { MP_QSTR_prescaler, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} }, \
    { MP_QSTR_period,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} }, \
    { MP_QSTR_mode,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_COUNTERMODE_UP} }, \
    { MP_QSTR_div,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_CLOCKDIVISION_DIV1} },

MP_DECLARE_CONST_FUN_OBJ(pyb_timer_counter_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_timer_prescaler_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_timer_period_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_timer_callback_obj);

mp_obj_t pyb_timer_callback(mp_obj_t self_in, mp_obj_t callback);

void pyb_timer_base_init_helper(pyb_timer_obj_t *self, mp_arg_val_t *vals);

void timer_init0(void);
void timer_tim3_init(void);
void timer_tim5_init(void);
void timer_tim6_init(uint freq);

void timer_deinit(void);

void timer_irq_handler(uint tim_id);
