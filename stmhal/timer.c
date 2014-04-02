#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <stm32f4xx_hal.h>
#include "usbd_cdc_msc_hid.h"
#include "usbd_cdc_interface.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "timer.h"
#include "servo.h"

// The timers can be used by multiple drivers, and need a common point for
// the interrupts to be dispatched, so they are all collected here.
//
// TIM3:
//  - USB CDC interface, interval, to check for new data
//  - LED 4, PWM to set the LED intensity
//
// TIM5:
//  - servo controller, PWM

TIM_HandleTypeDef TIM3_Handle;
TIM_HandleTypeDef TIM5_Handle;

// TIM3 is set-up for the USB CDC interface
void timer_tim3_init(void) {
    // set up the timer for USBD CDC
    __TIM3_CLK_ENABLE();

    TIM3_Handle.Instance = TIM3;
    TIM3_Handle.Init.Period = (USBD_CDC_POLLING_INTERVAL*1000) - 1;
    TIM3_Handle.Init.Prescaler = 84-1;
    TIM3_Handle.Init.ClockDivision = 0;
    TIM3_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&TIM3_Handle);

    HAL_NVIC_SetPriority(TIM3_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    if (HAL_TIM_Base_Start(&TIM3_Handle) != HAL_OK) {
        /* Starting Error */
    }
}

/* unused
void timer_tim3_deinit(void) {
    // reset TIM3 timer
    __TIM3_FORCE_RESET();
    __TIM3_RELEASE_RESET();
}
*/

// TIM5 is set-up for the servo controller
void timer_tim5_init(void) {
    // TIM5 clock enable
    __TIM5_CLK_ENABLE();

    // set up and enable interrupt
    HAL_NVIC_SetPriority(TIM5_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM5_IRQn);

    // PWM clock configuration
    TIM5_Handle.Instance = TIM5;
    TIM5_Handle.Init.Period = 2000; // timer cycles at 50Hz
    TIM5_Handle.Init.Prescaler = ((SystemCoreClock / 2) / 100000) - 1; // timer runs at 100kHz
    TIM5_Handle.Init.ClockDivision = 0;
    TIM5_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&TIM5_Handle);
}

// Interrupt dispatch
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &TIM3_Handle) {
        USBD_CDC_HAL_TIM_PeriodElapsedCallback();
    } else if (htim == &TIM5_Handle) {
        servo_timer_irq_callback();
    }
}

// below is old code from stm/ which has not yet been fully ported to stmhal/
#if 0
typedef struct _pyb_hal_tim_t {
    mp_obj_base_t base;
    TIM_HandleTypeDef htim;
} pyb_hal_tim_t;

pyb_hal_tim_t pyb_hal_tim_6;

    pyb_hal_tim_6 = {
        .base = {&pyb_type_hal_tim};
        .htim = {TIM6

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
    __TIM6_CLK_ENABLE();

    // Compute the prescaler value so TIM6 runs at 20kHz
    uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 20000) - 1;

    // Time base configuration
    tim_handle.Instance = TIM6;
    tim_handle.Init.Prescaler = PrescalerValue;
    tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP; // unused for TIM6
    tim_handle.Init.Period = 20000; // timer cycles at 1Hz
    tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // unused for TIM6
    tim_handle.Init.RepetitionCounter = 0; // unused for TIM6
    HAL_TIM_Base_Init(&tim_handle);

    // enable perhipheral preload register
    //TIM_ARRPreloadConfig(TIM6, ENABLE); ??

    // set up interrupt
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0xf, 0xf); // lowest priority
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    // start timer, so that it interrupts on overflow
    HAL_TIM_Base_Start_IT(&tim_handle);

    // Python interface
    mp_obj_t m = mp_obj_new_module(QSTR_FROM_STR_STATIC("timer"));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("callback"), rt_make_function_n(1, timer_py_set_callback));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("period"), rt_make_function_n(1, timer_py_set_period));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("prescaler"), rt_make_function_n(1, timer_py_set_prescaler));
    rt_store_attr(m, QSTR_FROM_STR_STATIC("value"), rt_make_function_n(0, timer_py_get_value));
    rt_store_name(QSTR_FROM_STR_STATIC("timer"), m);
}

void timer_interrupt(void) {
    if (timer_py_callback != mp_const_none) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            // XXX what to do if the GC is in the middle of running??
            rt_call_function_0(timer_py_callback);
            nlr_pop();
        } else {
            // uncaught exception
            printf("exception in timer interrupt\n");
            mp_obj_print((mp_obj_t)nlr.ret_val, PRINT_REPR);
            printf("\n");
        }
    }
}

mp_obj_t pyb_Timer(mp_obj_t timx_in) {
    TIM_TypeDef *TIMx = (TIM_TypeDef*)mp_obj_get_int(timx_in);
    if (!IS_TIM_INSTANCE(TIMx)) {
        nlr_jump(mp_obj_new_exception_msg(&mp_type_ValueError, "argument 1 is not a TIM instance"));
    }
    pyb_hal_tim_t *tim = m_new_obj(pyb_hal_tim_t);
    tim->htim.Instance = TIMx;
    tim->htim.Instance.Init.Prescaler = x;
    tim->htim.Instance.Init.CounterMode = y;
    tim->htim.Instance.Init.Period = y;
    tim->htim.Instance.Init.ClockDivision = y;
    tim->htim.Instance.Init.RepetitionCounter = y;
    HAL_TIM_Base_Init(&tim->htim);
    return tim;
}
#endif
