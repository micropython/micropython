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
#include "usbd_cdc_msc_hid.h"
#include "usbd_cdc_interface.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "gc.h"
#include "obj.h"
#include "runtime.h"
#include "timer.h"
#include "servo.h"

/// \moduleref pyb
/// \class Timer - periodically call a function
///
/// Timers can be used for a great variety of tasks.  At the moment, only
/// the simplest case is implemented: that of calling a function periodically.
///
/// Each timer consists of a counter that counts up at a certain rate.  The rate
/// at which it counts is the peripheral clock frequency (in Hz) divided by the
/// timer prescaler.  When the counter reaches the timer period it triggers an
/// event, and the counter resets back to zero.  By using the callback method,
/// the timer event can call a Python function.
///
/// Example usage to toggle an LED at a fixed frequency:
///
///     tim = pyb.Timer(4)              # create a timer object using timer 4
///     tim.init(freq=2)                # trigger at 2Hz
///     tim.callback(lambda t:pyb.LED(1).toggle())
///
/// Further examples:
///
///     tim = pyb.Timer(4, freq=100)    # freq in Hz
///     tim = pyb.Timer(4, prescaler=1, period=100)
///     tim.counter()                   # get counter (can also set)
///     tim.prescaler(2)                # set prescaler (can also get)
///     tim.period(200)                 # set period (can also get)
///     tim.callback(lambda t: ...)     # set callback for update interrupt (t=tim instance)
///     tim.callback(None)              # clear callback
///
/// *Note:* Timer 3 is reserved for internal use.  Timer 5 controls
/// the servo driver, and Timer 6 is used for timed ADC/DAC reading/writing.
/// It is recommended to use the other timers in your programs.

// The timers can be used by multiple drivers, and need a common point for
// the interrupts to be dispatched, so they are all collected here.
//
// TIM3:
//  - flash storage controller, to flush the cache
//  - USB CDC interface, interval, to check for new data
//  - LED 4, PWM to set the LED intensity
//
// TIM5:
//  - servo controller, PWM
//
// TIM6:
//  - ADC, DAC for read_timed and write_timed

typedef struct _pyb_timer_obj_t {
    mp_obj_base_t base;
    mp_uint_t tim_id;
    mp_obj_t callback;
    TIM_HandleTypeDef tim;
    IRQn_Type irqn;
} pyb_timer_obj_t;

TIM_HandleTypeDef TIM3_Handle;
TIM_HandleTypeDef TIM5_Handle;
TIM_HandleTypeDef TIM6_Handle;

// Used to divide down TIM3 and periodically call the flash storage IRQ
static uint32_t tim3_counter = 0;

// Used to do callbacks to Python code on interrupt
STATIC pyb_timer_obj_t *pyb_timer_obj_all[14];
#define PYB_TIMER_OBJ_ALL_NUM MP_ARRAY_SIZE(pyb_timer_obj_all)

STATIC mp_obj_t pyb_timer_deinit(mp_obj_t self_in);
STATIC mp_obj_t pyb_timer_callback(mp_obj_t self_in, mp_obj_t callback);

void timer_init0(void) {
    tim3_counter = 0;
    for (uint i = 0; i < PYB_TIMER_OBJ_ALL_NUM; i++) {
        pyb_timer_obj_all[i] = NULL;
    }
}

// unregister all interrupt sources
void timer_deinit(void) {
    for (uint i = 0; i < PYB_TIMER_OBJ_ALL_NUM; i++) {
        pyb_timer_obj_t *tim = pyb_timer_obj_all[i];
        if (tim != NULL) {
            pyb_timer_deinit(tim);
        }
    }
}

// TIM3 is set-up for the USB CDC interface
void timer_tim3_init(void) {
    // set up the timer for USBD CDC
    __TIM3_CLK_ENABLE();

    TIM3_Handle.Instance = TIM3;
    TIM3_Handle.Init.Period = (USBD_CDC_POLLING_INTERVAL*1000) - 1; // TIM3 fires every USBD_CDC_POLLING_INTERVAL ms
    TIM3_Handle.Init.Prescaler = 84-1; // for System clock at 168MHz, TIM3 runs at 1MHz
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
// This function inits but does not start the timer
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

// Init TIM6 with a counter-overflow at the given frequency (given in Hz)
// TIM6 is used by the DAC and ADC for auto sampling at a given frequency
// This function inits but does not start the timer
void timer_tim6_init(uint freq) {
    // TIM6 clock enable
    __TIM6_CLK_ENABLE();

    // Timer runs at SystemCoreClock / 2
    // Compute the prescaler value so TIM6 triggers at freq-Hz
    uint32_t period = MAX(1, (SystemCoreClock / 2) / freq);
    uint32_t prescaler = 1;
    while (period > 0xffff) {
        period >>= 1;
        prescaler <<= 1;
    }

    // Time base clock configuration
    TIM6_Handle.Instance = TIM6;
    TIM6_Handle.Init.Period = period - 1;
    TIM6_Handle.Init.Prescaler = prescaler - 1;
    TIM6_Handle.Init.ClockDivision = 0; // unused for TIM6
    TIM6_Handle.Init.CounterMode = TIM_COUNTERMODE_UP; // unused for TIM6
    HAL_TIM_Base_Init(&TIM6_Handle);
}

// Interrupt dispatch
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &TIM3_Handle) {
        USBD_CDC_HAL_TIM_PeriodElapsedCallback();

        // Periodically raise a flash IRQ for the flash storage controller
        if (tim3_counter++ >= 500 / USBD_CDC_POLLING_INTERVAL) {
            tim3_counter = 0;
            NVIC->STIR = FLASH_IRQn;
        }

    } else if (htim == &TIM5_Handle) {
        servo_timer_irq_callback();
    }
}

/******************************************************************************/
/* Micro Python bindings                                                      */

STATIC void pyb_timer_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_obj_t *self = self_in;

    if (self->tim.State == HAL_TIM_STATE_RESET) {
        print(env, "Timer(%u)", self->tim_id);
    } else {
        print(env, "Timer(%u, prescaler=%u, period=%u, mode=%u, div=%u)",
            self->tim_id,
            self->tim.Init.Prescaler,
            self->tim.Init.Period,
            self->tim.Init.CounterMode,
            self->tim.Init.ClockDivision
        );
    }
}

/// \method init(*, freq, prescaler, period)
/// Initialise the timer.  Initialisation must be either by frequency (in Hz)
/// or by prescaler and period:
///
///     tim.init(freq=100)                  # set the timer to trigger at 100Hz
///     tim.init(prescaler=100, period=300) # set the prescaler and period directly
STATIC const mp_arg_t pyb_timer_init_args[] = {
    { MP_QSTR_freq,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
    { MP_QSTR_prescaler, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
    { MP_QSTR_period,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
    { MP_QSTR_mode,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_COUNTERMODE_UP} },
    { MP_QSTR_div,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_CLOCKDIVISION_DIV1} },
};
#define PYB_TIMER_INIT_NUM_ARGS MP_ARRAY_SIZE(pyb_timer_init_args)

STATIC mp_obj_t pyb_timer_init_helper(pyb_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_TIMER_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_TIMER_INIT_NUM_ARGS, pyb_timer_init_args, vals);

    // set the TIM configuration values
    TIM_Base_InitTypeDef *init = &self->tim.Init;

    if (vals[0].u_int != 0xffffffff) {
        // set prescaler and period from frequency

        if (vals[0].u_int == 0) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "can't have 0 frequency"));
        }

        // work out TIM's clock source
        uint tim_clock;
        if (self->tim_id == 1 || (8 <= self->tim_id && self->tim_id <= 11)) {
            // TIM{1,8,9,10,11} are on APB2
            tim_clock = HAL_RCC_GetPCLK2Freq();
        } else {
            // TIM{2,3,4,5,6,7,12,13,14} are on APB1
            tim_clock = HAL_RCC_GetPCLK1Freq();
        }

        // Compute the prescaler value so TIM triggers at freq-Hz
        // On STM32F405/407/415/417 there are 2 cases for how the clock freq is set.
        // If the APB prescaler is 1, then the timer clock is equal to its respective
        // APB clock.  Otherwise (APB prescaler > 1) the timer clock is twice its
        // respective APB clock.  See DM00031020 Rev 4, page 115.
        uint32_t period = MAX(1, 2 * tim_clock / vals[0].u_int);
        uint32_t prescaler = 1;
        while (period > 0xffff) {
            period >>= 1;
            prescaler <<= 1;
        }
        init->Prescaler = prescaler - 1;
        init->Period = period - 1;
    } else if (vals[1].u_int != 0xffffffff && vals[2].u_int != 0xffffffff) {
        // set prescaler and period directly
        init->Prescaler = vals[1].u_int;
        init->Period = vals[2].u_int;
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "must specify either freq, or prescaler and period"));
    }

    init->CounterMode = vals[3].u_int;
    init->ClockDivision = vals[4].u_int;
    init->RepetitionCounter = 0;

    // init the TIM peripheral
    switch (self->tim_id) {
        case 1: __TIM1_CLK_ENABLE(); break;
        case 2: __TIM2_CLK_ENABLE(); break;
        case 3: __TIM3_CLK_ENABLE(); break;
        case 4: __TIM4_CLK_ENABLE(); break;
        case 5: __TIM5_CLK_ENABLE(); break;
        case 6: __TIM6_CLK_ENABLE(); break;
        case 7: __TIM7_CLK_ENABLE(); break;
        case 8: __TIM8_CLK_ENABLE(); break;
        case 9: __TIM9_CLK_ENABLE(); break;
        case 10: __TIM10_CLK_ENABLE(); break;
        case 11: __TIM11_CLK_ENABLE(); break;
        case 12: __TIM12_CLK_ENABLE(); break;
        case 13: __TIM13_CLK_ENABLE(); break;
        case 14: __TIM14_CLK_ENABLE(); break;
    }
    HAL_TIM_Base_Init(&self->tim);
    HAL_TIM_Base_Start(&self->tim);

    // set the priority (if not a special timer)
    if (self->tim_id != 3 && self->tim_id != 5) {
        HAL_NVIC_SetPriority(self->irqn, 0xe, 0xe); // next-to lowest priority
    }

    return mp_const_none;
}

/// \classmethod \constructor(id, ...)
/// Construct a new timer object of the given id.  If additional
/// arguments are given, then the timer is initialised by `init(...)`.
/// `id` can be 1 to 14, excluding 3.
STATIC mp_obj_t pyb_timer_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // create new Timer object
    pyb_timer_obj_t *tim = m_new_obj(pyb_timer_obj_t);
    tim->base.type = &pyb_timer_type;
    tim->callback = mp_const_none;
    memset(&tim->tim, 0, sizeof(tim->tim));

    // get TIM number
    tim->tim_id = mp_obj_get_int(args[0]);

    switch (tim->tim_id) {
        case 1: tim->tim.Instance = TIM1; tim->irqn = TIM1_UP_TIM10_IRQn; break;
        case 2: tim->tim.Instance = TIM2; tim->irqn = TIM2_IRQn; break;
        case 3: nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Timer 3 is for internal use only")); // TIM3 used for low-level stuff; go via regs if necessary
        case 4: tim->tim.Instance = TIM4; tim->irqn = TIM4_IRQn; break;
        case 5: tim->tim.Instance = TIM5; tim->irqn = TIM5_IRQn; break;
        case 6: tim->tim.Instance = TIM6; tim->irqn = TIM6_DAC_IRQn; break;
        case 7: tim->tim.Instance = TIM7; tim->irqn = TIM7_IRQn; break;
        case 8: tim->tim.Instance = TIM8; tim->irqn = TIM8_UP_TIM13_IRQn; break;
        case 9: tim->tim.Instance = TIM9; tim->irqn = TIM1_BRK_TIM9_IRQn; break;
        case 10: tim->tim.Instance = TIM10; tim->irqn = TIM1_UP_TIM10_IRQn; break;
        case 11: tim->tim.Instance = TIM11; tim->irqn = TIM1_TRG_COM_TIM11_IRQn; break;
        case 12: tim->tim.Instance = TIM12; tim->irqn = TIM8_BRK_TIM12_IRQn; break;
        case 13: tim->tim.Instance = TIM13; tim->irqn = TIM8_UP_TIM13_IRQn; break;
        case 14: tim->tim.Instance = TIM14; tim->irqn = TIM8_TRG_COM_TIM14_IRQn; break;
        default: nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Timer %d does not exist", tim->tim_id));
    }

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_timer_init_helper(tim, n_args - 1, args + 1, &kw_args);
    }

    // set the global variable for interrupt callbacks
    if (tim->tim_id - 1 < PYB_TIMER_OBJ_ALL_NUM) {
        pyb_timer_obj_all[tim->tim_id - 1] = tim;
    }

    return (mp_obj_t)tim;
}

STATIC mp_obj_t pyb_timer_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_timer_init_obj, 1, pyb_timer_init);

/// \method deinit()
/// Deinitialises the timer.
///
/// Disables the callback (and the associated irq).
/// Stops the timer, and disables the timer peripheral.
STATIC mp_obj_t pyb_timer_deinit(mp_obj_t self_in) {
    pyb_timer_obj_t *self = self_in;

    // Disable the interrupt
    pyb_timer_callback(self_in, mp_const_none);

    HAL_TIM_Base_DeInit(&self->tim);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_timer_deinit_obj, pyb_timer_deinit);

/// \method counter([value])
/// Get or set the timer counter.
mp_obj_t pyb_timer_counter(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->tim.Instance->CNT);
    } else {
        // set
        __HAL_TIM_SetCounter(&self->tim, mp_obj_get_int(args[1]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_counter_obj, 1, 2, pyb_timer_counter);

/// \method prescaler([value])
/// Get or set the prescaler for the timer.
mp_obj_t pyb_timer_prescaler(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->tim.Instance->PSC & 0xffff);
    } else {
        // set
        self->tim.Init.Prescaler = self->tim.Instance->PSC = mp_obj_get_int(args[1]) & 0xffff;
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_prescaler_obj, 1, 2, pyb_timer_prescaler);

/// \method period([value])
/// Get or set the period of the timer.
mp_obj_t pyb_timer_period(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->tim.Instance->ARR & 0xffff);
    } else {
        // set
        __HAL_TIM_SetAutoreload(&self->tim, mp_obj_get_int(args[1]) & 0xffff);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_period_obj, 1, 2, pyb_timer_period);

/// \method callback(fun)
/// Set the function to be called when the timer triggers.
/// `fun` is passed 1 argument, the timer object.
/// If `fun` is `None` then the callback will be disabled.
STATIC mp_obj_t pyb_timer_callback(mp_obj_t self_in, mp_obj_t callback) {
    pyb_timer_obj_t *self = self_in;
    if (callback == mp_const_none) {
        // stop interrupt (but not timer)
        __HAL_TIM_DISABLE_IT(&self->tim, TIM_IT_UPDATE);
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
        HAL_NVIC_EnableIRQ(self->irqn);
        // start timer, so that it interrupts on overflow
        HAL_TIM_Base_Start_IT(&self->tim);
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "callback must be None or a callable object"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_timer_callback_obj, pyb_timer_callback);

STATIC const mp_map_elem_t pyb_timer_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_timer_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_timer_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_counter), (mp_obj_t)&pyb_timer_counter_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_prescaler), (mp_obj_t)&pyb_timer_prescaler_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_period), (mp_obj_t)&pyb_timer_period_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&pyb_timer_callback_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_timer_locals_dict, pyb_timer_locals_dict_table);

const mp_obj_type_t pyb_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = pyb_timer_print,
    .make_new = pyb_timer_make_new,
    .locals_dict = (mp_obj_t)&pyb_timer_locals_dict,
};

void timer_irq_handler(uint tim_id) {
    if (tim_id - 1 < PYB_TIMER_OBJ_ALL_NUM) {
        // get the timer object
        pyb_timer_obj_t *tim = pyb_timer_obj_all[tim_id - 1];

        if (tim == NULL) {
            // timer object has not been set, so we can't do anything
            return;
        }

        // see if it was a TIM update event (the only event we currently interrupt on)
        if (__HAL_TIM_GET_FLAG(&tim->tim, TIM_FLAG_UPDATE) != RESET) {
            if (__HAL_TIM_GET_ITSTATUS(&tim->tim, TIM_IT_UPDATE) != RESET) {
                // clear the interrupt
                __HAL_TIM_CLEAR_IT(&tim->tim, TIM_IT_UPDATE);

                // execute callback if it's set
                if (tim->callback != mp_const_none) {
                    // When executing code within a handler we must lock the GC to prevent
                    // any memory allocations.  We must also catch any exceptions.
                    gc_lock();
                    nlr_buf_t nlr;
                    if (nlr_push(&nlr) == 0) {
                        mp_call_function_1(tim->callback, tim);
                        nlr_pop();
                    } else {
                        // Uncaught exception; disable the callback so it doesn't run again.
                        tim->callback = mp_const_none;
                        __HAL_TIM_DISABLE_IT(&tim->tim, TIM_IT_UPDATE);
                        printf("Uncaught exception in Timer(" UINT_FMT ") interrupt handler\n", tim->tim_id);
                        mp_obj_print_exception((mp_obj_t)nlr.ret_val);
                    }
                    gc_unlock();
                }
            }
        }
    }
}
