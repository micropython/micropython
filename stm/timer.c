#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stm_misc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "parse.h"
#include "obj.h"
#include "runtime.h"

#include "timer.h"

// TIM6 is used as an internal interrup to schedule something at a specific rate
mp_obj_t timer_py_callback;

mp_obj_t timer_py_set_callback(mp_obj_t f) {
    timer_py_callback = f;
    return mp_const_none;
}

mp_obj_t timer_py_set_period(mp_obj_t period) {
    TIM6->ARR = mp_obj_get_int(period) & 0xffff;
    return mp_const_none;
}

mp_obj_t timer_py_set_prescaler(mp_obj_t prescaler) {
    TIM6->PSC = mp_obj_get_int(prescaler) & 0xffff;
    return mp_const_none;
}

mp_obj_t timer_py_get_value(void) {
    return mp_obj_new_int(TIM6->CNT & 0xfffff);
}

void timer_init(void) {
    timer_py_callback = mp_const_none;

    // TIM6 clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    // Compute the prescaler value so TIM6 runs at 20kHz
    uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 20000) - 1;

    // Time base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 20000; // timer cycles at 1Hz
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; // unused for TIM6
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // unused for TIM6
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    // enable perhipheral preload register
    TIM_ARRPreloadConfig(TIM6, ENABLE);

    // enable interrupt when counter overflows
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    // set up interrupt
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0f; // lowest priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0f; // lowest priority
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // TIM6 enable counter
    TIM_Cmd(TIM6, ENABLE);

    // Python interface
    mp_obj_t m = mp_obj_new_module(QSTR_FROM_STR_STATIC("timer"));
    mp_store_attr(m, QSTR_FROM_STR_STATIC("callback"), mp_make_function_n(1, timer_py_set_callback));
    mp_store_attr(m, QSTR_FROM_STR_STATIC("period"), mp_make_function_n(1, timer_py_set_period));
    mp_store_attr(m, QSTR_FROM_STR_STATIC("prescaler"), mp_make_function_n(1, timer_py_set_prescaler));
    mp_store_attr(m, QSTR_FROM_STR_STATIC("value"), mp_make_function_n(0, timer_py_get_value));
    mp_store_name(QSTR_FROM_STR_STATIC("timer"), m);
}

void timer_interrupt(void) {
    if (timer_py_callback != mp_const_none) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            // XXX what to do if the GC is in the middle of running??
            mp_call_function_0(timer_py_callback);
            nlr_pop();
        } else {
            // uncaught exception
            printf("exception in timer interrupt\n");
            mp_obj_print((mp_obj_t)nlr.ret_val, PRINT_REPR);
            printf("\n");
        }
    }
}
