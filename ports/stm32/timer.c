/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include <string.h>

#include "py/runtime.h"
#include "py/gc.h"
#include "timer.h"
#include "servo.h"
#include "pin.h"
#include "irq.h"

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
///     tim = pyb.Timer(4, prescaler=0, period=99)
///     tim.counter()                   # get counter (can also set)
///     tim.prescaler(2)                # set prescaler (can also get)
///     tim.period(199)                 # set period (can also get)
///     tim.callback(lambda t: ...)     # set callback for update interrupt (t=tim instance)
///     tim.callback(None)              # clear callback
///
/// *Note:* Timer 3 is used for fading the blue LED.  Timer 5 controls
/// the servo driver, and Timer 6 is used for timed ADC/DAC reading/writing.
/// It is recommended to use the other timers in your programs.

// The timers can be used by multiple drivers, and need a common point for
// the interrupts to be dispatched, so they are all collected here.
//
// TIM3:
//  - LED 4, PWM to set the LED intensity
//
// TIM5:
//  - servo controller, PWM
//
// TIM6:
//  - ADC, DAC for read_timed and write_timed

typedef enum {
    CHANNEL_MODE_PWM_NORMAL,
    CHANNEL_MODE_PWM_INVERTED,
    CHANNEL_MODE_OC_TIMING,
    CHANNEL_MODE_OC_ACTIVE,
    CHANNEL_MODE_OC_INACTIVE,
    CHANNEL_MODE_OC_TOGGLE,
    CHANNEL_MODE_OC_FORCED_ACTIVE,
    CHANNEL_MODE_OC_FORCED_INACTIVE,
    CHANNEL_MODE_IC,
    CHANNEL_MODE_ENC_A,
    CHANNEL_MODE_ENC_B,
    CHANNEL_MODE_ENC_AB,
} pyb_channel_mode;

STATIC const struct {
    qstr name;
    uint32_t oc_mode;
} channel_mode_info[] = {
    { MP_QSTR_PWM,                TIM_OCMODE_PWM1 },
    { MP_QSTR_PWM_INVERTED,       TIM_OCMODE_PWM2 },
    { MP_QSTR_OC_TIMING,          TIM_OCMODE_TIMING },
    { MP_QSTR_OC_ACTIVE,          TIM_OCMODE_ACTIVE },
    { MP_QSTR_OC_INACTIVE,        TIM_OCMODE_INACTIVE },
    { MP_QSTR_OC_TOGGLE,          TIM_OCMODE_TOGGLE },
    { MP_QSTR_OC_FORCED_ACTIVE,   TIM_OCMODE_FORCED_ACTIVE },
    { MP_QSTR_OC_FORCED_INACTIVE, TIM_OCMODE_FORCED_INACTIVE },
    { MP_QSTR_IC,                 0 },
    { MP_QSTR_ENC_A,              TIM_ENCODERMODE_TI1 },
    { MP_QSTR_ENC_B,              TIM_ENCODERMODE_TI2 },
    { MP_QSTR_ENC_AB,             TIM_ENCODERMODE_TI12 },
};

enum {
    BRK_OFF,
    BRK_LOW,
    BRK_HIGH,
};

typedef struct _pyb_timer_channel_obj_t {
    mp_obj_base_t base;
    struct _pyb_timer_obj_t *timer;
    uint8_t channel;
    uint8_t mode;
    mp_obj_t callback;
    struct _pyb_timer_channel_obj_t *next;
} pyb_timer_channel_obj_t;

typedef struct _pyb_timer_obj_t {
    mp_obj_base_t base;
    uint8_t tim_id;
    uint8_t is_32bit;
    mp_obj_t callback;
    TIM_HandleTypeDef tim;
    IRQn_Type irqn;
    pyb_timer_channel_obj_t *channel;
} pyb_timer_obj_t;

// The following yields TIM_IT_UPDATE when channel is zero and
// TIM_IT_CC1..TIM_IT_CC4 when channel is 1..4
#define TIMER_IRQ_MASK(channel) (1 << (channel))
#define TIMER_CNT_MASK(self)    ((self)->is_32bit ? 0xffffffff : 0xffff)
#define TIMER_CHANNEL(self)     ((((self)->channel) - 1) << 2)

TIM_HandleTypeDef TIM5_Handle;
TIM_HandleTypeDef TIM6_Handle;

#define PYB_TIMER_OBJ_ALL_NUM MP_ARRAY_SIZE(MP_STATE_PORT(pyb_timer_obj_all))

STATIC mp_obj_t pyb_timer_deinit(mp_obj_t self_in);
STATIC mp_obj_t pyb_timer_callback(mp_obj_t self_in, mp_obj_t callback);
STATIC mp_obj_t pyb_timer_channel_callback(mp_obj_t self_in, mp_obj_t callback);

void timer_init0(void) {
    for (uint i = 0; i < PYB_TIMER_OBJ_ALL_NUM; i++) {
        MP_STATE_PORT(pyb_timer_obj_all)[i] = NULL;
    }
}

// unregister all interrupt sources
void timer_deinit(void) {
    for (uint i = 0; i < PYB_TIMER_OBJ_ALL_NUM; i++) {
        pyb_timer_obj_t *tim = MP_STATE_PORT(pyb_timer_obj_all)[i];
        if (tim != NULL) {
            pyb_timer_deinit(MP_OBJ_FROM_PTR(tim));
        }
    }
}

#if defined(TIM5)
// TIM5 is set-up for the servo controller
// This function inits but does not start the timer
void timer_tim5_init(void) {
    // TIM5 clock enable
    __HAL_RCC_TIM5_CLK_ENABLE();

    // set up and enable interrupt
    NVIC_SetPriority(TIM5_IRQn, IRQ_PRI_TIM5);
    HAL_NVIC_EnableIRQ(TIM5_IRQn);

    // PWM clock configuration
    TIM5_Handle.Instance = TIM5;
    TIM5_Handle.Init.Period = 2000 - 1; // timer cycles at 50Hz
    TIM5_Handle.Init.Prescaler = (timer_get_source_freq(5) / 100000) - 1; // timer runs at 100kHz
    TIM5_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TIM5_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;

    HAL_TIM_PWM_Init(&TIM5_Handle);
}
#endif

#if defined(TIM6)
// Init TIM6 with a counter-overflow at the given frequency (given in Hz)
// TIM6 is used by the DAC and ADC for auto sampling at a given frequency
// This function inits but does not start the timer
TIM_HandleTypeDef *timer_tim6_init(uint freq) {
    // TIM6 clock enable
    __HAL_RCC_TIM6_CLK_ENABLE();

    // Timer runs at SystemCoreClock / 2
    // Compute the prescaler value so TIM6 triggers at freq-Hz
    uint32_t period = MAX(1, timer_get_source_freq(6) / freq);
    uint32_t prescaler = 1;
    while (period > 0xffff) {
        period >>= 1;
        prescaler <<= 1;
    }

    // Time base clock configuration
    TIM6_Handle.Instance = TIM6;
    TIM6_Handle.Init.Period = period - 1;
    TIM6_Handle.Init.Prescaler = prescaler - 1;
    TIM6_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // unused for TIM6
    TIM6_Handle.Init.CounterMode = TIM_COUNTERMODE_UP; // unused for TIM6
    HAL_TIM_Base_Init(&TIM6_Handle);

    return &TIM6_Handle;
}
#endif

// Interrupt dispatch
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    #if MICROPY_HW_ENABLE_SERVO
    if (htim == &TIM5_Handle) {
        servo_timer_irq_callback();
    }
    #endif
}

// Get the frequency (in Hz) of the source clock for the given timer.
// On STM32F405/407/415/417 there are 2 cases for how the clock freq is set.
// If the APB prescaler is 1, then the timer clock is equal to its respective
// APB clock.  Otherwise (APB prescaler > 1) the timer clock is twice its
// respective APB clock.  See DM00031020 Rev 4, page 115.
uint32_t timer_get_source_freq(uint32_t tim_id) {
    uint32_t source, clk_div;
    if (tim_id == 1 || (8 <= tim_id && tim_id <= 11)) {
        // TIM{1,8,9,10,11} are on APB2
        #if defined(STM32F0)
        source = HAL_RCC_GetPCLK1Freq();
        clk_div = RCC->CFGR & RCC_CFGR_PPRE;
        #elif defined(STM32H7)
        source = HAL_RCC_GetPCLK2Freq();
        clk_div = RCC->D2CFGR & RCC_D2CFGR_D2PPRE2;
        #else
        source = HAL_RCC_GetPCLK2Freq();
        clk_div = RCC->CFGR & RCC_CFGR_PPRE2;
        #endif
    } else {
        // TIM{2,3,4,5,6,7,12,13,14} are on APB1
        source = HAL_RCC_GetPCLK1Freq();
        #if defined(STM32F0)
        clk_div = RCC->CFGR & RCC_CFGR_PPRE;
        #elif defined(STM32H7)
        clk_div = RCC->D2CFGR & RCC_D2CFGR_D2PPRE1;
        #else
        clk_div = RCC->CFGR & RCC_CFGR_PPRE1;
        #endif
    }
    if (clk_div != 0) {
        // APB prescaler for this timer is > 1
        source *= 2;
    }
    return source;
}

/******************************************************************************/
/* MicroPython bindings                                                       */

STATIC const mp_obj_type_t pyb_timer_channel_type;

// This is the largest value that we can multiply by 100 and have the result
// fit in a uint32_t.
#define MAX_PERIOD_DIV_100  42949672

// computes prescaler and period so TIM triggers at freq-Hz
STATIC uint32_t compute_prescaler_period_from_freq(pyb_timer_obj_t *self, mp_obj_t freq_in, uint32_t *period_out) {
    uint32_t source_freq = timer_get_source_freq(self->tim_id);
    uint32_t prescaler = 1;
    uint32_t period;
    if (0) {
    #if MICROPY_PY_BUILTINS_FLOAT
    } else if (mp_obj_is_type(freq_in, &mp_type_float)) {
        float freq = mp_obj_get_float_to_f(freq_in);
        if (freq <= 0) {
            goto bad_freq;
        }
        while (freq < 1 && prescaler < 6553) {
            prescaler *= 10;
            freq *= 10.0f;
        }
        period = (uint32_t)((float)source_freq / freq);
    #endif
    } else {
        mp_int_t freq = mp_obj_get_int(freq_in);
        if (freq <= 0) {
            goto bad_freq;
        bad_freq:
            mp_raise_ValueError(MP_ERROR_TEXT("must have positive freq"));
        }
        period = source_freq / freq;
    }
    period = MAX(1, period);
    while (period > TIMER_CNT_MASK(self)) {
        // if we can divide exactly, do that first
        if (period % 5 == 0) {
            prescaler *= 5;
            period /= 5;
        } else if (period % 3 == 0) {
            prescaler *= 3;
            period /= 3;
        } else {
            // may not divide exactly, but loses minimal precision
            prescaler <<= 1;
            period >>= 1;
        }
    }
    *period_out = (period - 1) & TIMER_CNT_MASK(self);
    return (prescaler - 1) & 0xffff;
}

// computes prescaler and period so TIM triggers with a period of t_num/t_den seconds
STATIC uint32_t compute_prescaler_period_from_t(pyb_timer_obj_t *self, int32_t t_num, int32_t t_den, uint32_t *period_out) {
    uint32_t source_freq = timer_get_source_freq(self->tim_id);
    if (t_num <= 0 || t_den <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("must have positive freq"));
    }
    uint64_t period = (uint64_t)source_freq * (uint64_t)t_num / (uint64_t)t_den;
    uint32_t prescaler = 1;
    while (period > TIMER_CNT_MASK(self)) {
        // if we can divide exactly, and without prescaler overflow, do that first
        if (prescaler <= 13107 && period % 5 == 0) {
            prescaler *= 5;
            period /= 5;
        } else if (prescaler <= 21845 && period % 3 == 0) {
            prescaler *= 3;
            period /= 3;
        } else {
            // may not divide exactly, but loses minimal precision
            uint32_t period_lsb = period & 1;
            prescaler <<= 1;
            period >>= 1;
            if (period < prescaler) {
                // round division up
                prescaler |= period_lsb;
            }
            if (prescaler > 0x10000) {
                mp_raise_ValueError(MP_ERROR_TEXT("period too large"));
            }
        }
    }
    *period_out = (period - 1) & TIMER_CNT_MASK(self);
    return (prescaler - 1) & 0xffff;
}

// Helper function for determining the period used for calculating percent
STATIC uint32_t compute_period(pyb_timer_obj_t *self) {
    // In center mode,  compare == period corresponds to 100%
    // In edge mode, compare == (period + 1) corresponds to 100%
    uint32_t period = (__HAL_TIM_GET_AUTORELOAD(&self->tim) & TIMER_CNT_MASK(self));
    if (period != 0xffffffff) {
        if (self->tim.Init.CounterMode == TIM_COUNTERMODE_UP ||
            self->tim.Init.CounterMode == TIM_COUNTERMODE_DOWN) {
            // Edge mode
            period++;
        }
    }
    return period;
}

// Helper function to compute PWM value from timer period and percent value.
// 'percent_in' can be an int or a float between 0 and 100 (out of range
// values are clamped).
STATIC uint32_t compute_pwm_value_from_percent(uint32_t period, mp_obj_t percent_in) {
    uint32_t cmp;
    if (0) {
    #if MICROPY_PY_BUILTINS_FLOAT
    } else if (mp_obj_is_type(percent_in, &mp_type_float)) {
        mp_float_t percent = mp_obj_get_float(percent_in);
        if (percent <= 0.0) {
            cmp = 0;
        } else if (percent >= 100.0) {
            cmp = period;
        } else {
            cmp = (uint32_t)(percent / MICROPY_FLOAT_CONST(100.0) * ((mp_float_t)period));
        }
    #endif
    } else {
        // For integer arithmetic, if period is large and 100*period will
        // overflow, then divide period before multiplying by cmp.  Otherwise
        // do it the other way round to retain precision.
        mp_int_t percent = mp_obj_get_int(percent_in);
        if (percent <= 0) {
            cmp = 0;
        } else if (percent >= 100) {
            cmp = period;
        } else if (period > MAX_PERIOD_DIV_100) {
            cmp = (uint32_t)percent * (period / 100);
        } else {
            cmp = ((uint32_t)percent * period) / 100;
        }
    }
    return cmp;
}

// Helper function to compute percentage from timer perion and PWM value.
STATIC mp_obj_t compute_percent_from_pwm_value(uint32_t period, uint32_t cmp) {
    #if MICROPY_PY_BUILTINS_FLOAT
    mp_float_t percent;
    if (cmp >= period) {
        percent = 100.0;
    } else {
        percent = (mp_float_t)cmp * 100.0 / ((mp_float_t)period);
    }
    return mp_obj_new_float(percent);
    #else
    mp_int_t percent;
    if (cmp >= period) {
        percent = 100;
    } else if (cmp > MAX_PERIOD_DIV_100) {
        percent = cmp / (period / 100);
    } else {
        percent = cmp * 100 / period;
    }
    return mp_obj_new_int(percent);
    #endif
}

#if !defined(STM32L0)

// Computes the 8-bit value for the DTG field in the BDTR register.
//
// 1 tick = 1 count of the timer's clock (source_freq) divided by div.
// 0-128 ticks in inrements of 1
// 128-256 ticks in increments of 2
// 256-512 ticks in increments of 8
// 512-1008 ticks in increments of 16
STATIC uint32_t compute_dtg_from_ticks(mp_int_t ticks) {
    if (ticks <= 0) {
        return 0;
    }
    if (ticks < 128) {
        return ticks;
    }
    if (ticks < 256) {
        return 0x80 | ((ticks - 128) / 2);
    }
    if (ticks < 512) {
        return 0xC0 | ((ticks - 256) / 8);
    }
    if (ticks < 1008) {
        return 0xE0 | ((ticks - 512) / 16);
    }
    return 0xFF;
}

// Given the 8-bit value stored in the DTG field of the BDTR register, compute
// the number of ticks.
STATIC mp_int_t compute_ticks_from_dtg(uint32_t dtg) {
    if ((dtg & 0x80) == 0) {
        return dtg & 0x7F;
    }
    if ((dtg & 0xC0) == 0x80) {
        return 128 + ((dtg & 0x3F) * 2);
    }
    if ((dtg & 0xE0) == 0xC0) {
        return 256 + ((dtg & 0x1F) * 8);
    }
    return 512 + ((dtg & 0x1F) * 16);
}

STATIC void config_deadtime(pyb_timer_obj_t *self, mp_int_t ticks, mp_int_t brk) {
    TIM_BreakDeadTimeConfigTypeDef deadTimeConfig;
    deadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    deadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    deadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    deadTimeConfig.DeadTime = compute_dtg_from_ticks(ticks);
    deadTimeConfig.BreakState = brk == BRK_OFF ? TIM_BREAK_DISABLE : TIM_BREAK_ENABLE;
    deadTimeConfig.BreakPolarity = brk == BRK_LOW ? TIM_BREAKPOLARITY_LOW : TIM_BREAKPOLARITY_HIGH;
    #if defined(STM32F7) || defined(STM32H7) | defined(STM32L4)
    deadTimeConfig.BreakFilter = 0;
    deadTimeConfig.Break2State = TIM_BREAK_DISABLE;
    deadTimeConfig.Break2Polarity = TIM_BREAKPOLARITY_LOW;
    deadTimeConfig.Break2Filter = 0;
    #endif
    deadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&self->tim, &deadTimeConfig);
}

#endif

TIM_HandleTypeDef *pyb_timer_get_handle(mp_obj_t timer) {
    if (mp_obj_get_type(timer) != &pyb_timer_type) {
        mp_raise_ValueError(MP_ERROR_TEXT("need a Timer object"));
    }
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(timer);
    return &self->tim;
}

STATIC void pyb_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->tim.State == HAL_TIM_STATE_RESET) {
        mp_printf(print, "Timer(%u)", self->tim_id);
    } else {
        uint32_t prescaler = self->tim.Instance->PSC & 0xffff;
        uint32_t period = __HAL_TIM_GET_AUTORELOAD(&self->tim) & TIMER_CNT_MASK(self);
        // for efficiency, we compute and print freq as an int (not a float)
        uint32_t freq = timer_get_source_freq(self->tim_id) / ((prescaler + 1) * (period + 1));
        mp_printf(print, "Timer(%u, freq=%u, prescaler=%u, period=%u, mode=%s, div=%u",
            self->tim_id,
            freq,
            prescaler,
            period,
            self->tim.Init.CounterMode == TIM_COUNTERMODE_UP     ? "UP" :
            self->tim.Init.CounterMode == TIM_COUNTERMODE_DOWN   ? "DOWN" : "CENTER",
            self->tim.Init.ClockDivision == TIM_CLOCKDIVISION_DIV4 ? 4 :
            self->tim.Init.ClockDivision == TIM_CLOCKDIVISION_DIV2 ? 2 : 1);

        #if !defined(STM32L0)
        #if defined(IS_TIM_ADVANCED_INSTANCE)
        if (IS_TIM_ADVANCED_INSTANCE(self->tim.Instance))
        #elif defined(IS_TIM_BREAK_INSTANCE)
        if (IS_TIM_BREAK_INSTANCE(self->tim.Instance))
        #else
        if (0)
        #endif
        {
            mp_printf(print, ", deadtime=%u",
                compute_ticks_from_dtg(self->tim.Instance->BDTR & TIM_BDTR_DTG));
            if ((self->tim.Instance->BDTR & TIM_BDTR_BKE) == TIM_BDTR_BKE) {
                mp_printf(print, ", brk=%s",
                    ((self->tim.Instance->BDTR & TIM_BDTR_BKP) == TIM_BDTR_BKP) ? "BRK_HIGH" : "BRK_LOW");
            } else {
                mp_printf(print, ", brk=BRK_OFF");
            }
        }
        #endif
        mp_print_str(print, ")");
    }
}

/// \method init(*, freq, prescaler, period)
/// Initialise the timer.  Initialisation must be either by frequency (in Hz)
/// or by prescaler and period:
///
///     tim.init(freq=100)                  # set the timer to trigger at 100Hz
///     tim.init(prescaler=83, period=999)  # set the prescaler and period directly
///
/// Keyword arguments:
///
///   - `freq` - specifies the periodic frequency of the timer. You migh also
///              view this as the frequency with which the timer goes through
///              one complete cycle.
///
///   - `prescaler` [0-0xffff] - specifies the value to be loaded into the
///                 timer's Prescaler Register (PSC). The timer clock source is divided by
///     (`prescaler + 1`) to arrive at the timer clock. Timers 2-7 and 12-14
///     have a clock source of 84 MHz (pyb.freq()[2] * 2), and Timers 1, and 8-11
///     have a clock source of 168 MHz (pyb.freq()[3] * 2).
///
///   - `period` [0-0xffff] for timers 1, 3, 4, and 6-15. [0-0x3fffffff] for timers 2 & 5.
///              Specifies the value to be loaded into the timer's AutoReload
///     Register (ARR). This determines the period of the timer (i.e. when the
///     counter cycles). The timer counter will roll-over after `period + 1`
///     timer clock cycles.
///
///   - `mode` can be one of:
///     - `Timer.UP` - configures the timer to count from 0 to ARR (default)
///     - `Timer.DOWN` - configures the timer to count from ARR down to 0.
///     - `Timer.CENTER` - confgures the timer to count from 0 to ARR and
///       then back down to 0.
///
///   - `div` can be one of 1, 2, or 4. Divides the timer clock to determine
///       the sampling clock used by the digital filters.
///
///   - `callback` - as per Timer.callback()
///
///   - `deadtime` - specifies the amount of "dead" or inactive time between
///       transitions on complimentary channels (both channels will be inactive)
///       for this time). `deadtime` may be an integer between 0 and 1008, with
///       the following restrictions: 0-128 in steps of 1. 128-256 in steps of
///       2, 256-512 in steps of 8, and 512-1008 in steps of 16. `deadime`
///       measures ticks of `source_freq` divided by `div` clock ticks.
///       `deadtime` is only available on timers 1 and 8.
///
///   - `brk` - specifies if the break mode is used to kill the output of
///       the PWM when the BRK_IN input is asserted. The polarity set how the
///       BRK_IN input is triggered. It can be set to `BRK_OFF`, `BRK_LOW`
///       and `BRK_HIGH`.
///
///
///  You must either specify freq or both of period and prescaler.
STATIC mp_obj_t pyb_timer_init_helper(pyb_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_prescaler, ARG_period, ARG_tick_hz, ARG_mode, ARG_div, ARG_callback, ARG_deadtime, ARG_brk };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_prescaler,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_tick_hz,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIM_COUNTERMODE_UP} },
        { MP_QSTR_div,          MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_deadtime,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_brk,          MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = BRK_OFF} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // set the TIM configuration values
    TIM_Base_InitTypeDef *init = &self->tim.Init;

    if (args[ARG_freq].u_obj != mp_const_none) {
        // set prescaler and period from desired frequency
        init->Prescaler = compute_prescaler_period_from_freq(self, args[ARG_freq].u_obj, &init->Period);
    } else if (args[ARG_prescaler].u_int != 0xffffffff && args[ARG_period].u_int != 0xffffffff) {
        // set prescaler and period directly
        init->Prescaler = args[ARG_prescaler].u_int;
        init->Period = args[ARG_period].u_int;
    } else if (args[ARG_period].u_int != 0xffffffff) {
        // set prescaler and period from desired period and tick_hz scale
        init->Prescaler = compute_prescaler_period_from_t(self, args[ARG_period].u_int, args[ARG_tick_hz].u_int, &init->Period);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("must specify either freq, period, or prescaler and period"));
    }

    init->CounterMode = args[ARG_mode].u_int;
    if (!IS_TIM_COUNTER_MODE(init->CounterMode)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid mode (%d)"), init->CounterMode);
    }

    init->ClockDivision = args[ARG_div].u_int == 2 ? TIM_CLOCKDIVISION_DIV2 :
        args[ARG_div].u_int == 4 ? TIM_CLOCKDIVISION_DIV4 :
        TIM_CLOCKDIVISION_DIV1;

    #if !defined(STM32L0)
    init->RepetitionCounter = 0;
    #endif

    // enable TIM clock
    switch (self->tim_id) {
        #if defined(TIM1)
        case 1:
            __HAL_RCC_TIM1_CLK_ENABLE();
            break;
        #endif
        case 2:
            __HAL_RCC_TIM2_CLK_ENABLE();
            break;
        #if defined(TIM3)
        case 3:
            __HAL_RCC_TIM3_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM4)
        case 4:
            __HAL_RCC_TIM4_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM5)
        case 5:
            __HAL_RCC_TIM5_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM6)
        case 6:
            __HAL_RCC_TIM6_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM7)
        case 7:
            __HAL_RCC_TIM7_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM8)
        case 8:
            __HAL_RCC_TIM8_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM9)
        case 9:
            __HAL_RCC_TIM9_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM10)
        case 10:
            __HAL_RCC_TIM10_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM11)
        case 11:
            __HAL_RCC_TIM11_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM12)
        case 12:
            __HAL_RCC_TIM12_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM13)
        case 13:
            __HAL_RCC_TIM13_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM14)
        case 14:
            __HAL_RCC_TIM14_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM15)
        case 15:
            __HAL_RCC_TIM15_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM16)
        case 16:
            __HAL_RCC_TIM16_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM17)
        case 17:
            __HAL_RCC_TIM17_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM18)
        case 18:
            __HAL_RCC_TIM18_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM19)
        case 19:
            __HAL_RCC_TIM19_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM20)
        case 20:
            __HAL_RCC_TIM20_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM21)
        case 21:
            __HAL_RCC_TIM21_CLK_ENABLE();
            break;
        #endif
        #if defined(TIM22)
        case 22:
            __HAL_RCC_TIM22_CLK_ENABLE();
            break;
        #endif
    }

    // set IRQ priority (if not a special timer)
    if (self->tim_id != 5) {
        NVIC_SetPriority(IRQn_NONNEG(self->irqn), IRQ_PRI_TIMX);
        if (self->tim_id == 1) {
            #if defined(TIM1)
            NVIC_SetPriority(TIM1_CC_IRQn, IRQ_PRI_TIMX);
            #endif
        } else if (self->tim_id == 8) {
            #if defined(TIM8)
            NVIC_SetPriority(TIM8_CC_IRQn, IRQ_PRI_TIMX);
            #endif
        }
    }

    // init TIM
    HAL_TIM_Base_Init(&self->tim);
    #if !defined(STM32L0)
    #if defined(IS_TIM_ADVANCED_INSTANCE)
    if (IS_TIM_ADVANCED_INSTANCE(self->tim.Instance)) {
    #elif defined(IS_TIM_BREAK_INSTANCE)
    if (IS_TIM_BREAK_INSTANCE(self->tim.Instance)) {
    #else
    if (0) {
        #endif
        config_deadtime(self, args[ARG_deadtime].u_int, args[ARG_brk].u_int);

    }
    #endif

    // Enable ARPE so that the auto-reload register is buffered.
    // This allows to smoothly change the frequency of the timer.
    self->tim.Instance->CR1 |= TIM_CR1_ARPE;

    // Start the timer running
    if (args[ARG_callback].u_obj == mp_const_none) {
        HAL_TIM_Base_Start(&self->tim);
    } else {
        pyb_timer_callback(MP_OBJ_FROM_PTR(self), args[ARG_callback].u_obj);
    }

    return mp_const_none;
}

// This table encodes the timer instance and irq number (for the update irq).
// It assumes that timer instance pointer has the lower 8 bits cleared.
#define TIM_ENTRY(id, irq) [id - 1] = (uint32_t)TIM##id | irq
STATIC const uint32_t tim_instance_table[MICROPY_HW_MAX_TIMER] = {
    #if defined(TIM1)
    #if defined(STM32F0)
    TIM_ENTRY(1, TIM1_BRK_UP_TRG_COM_IRQn),
    #elif defined(STM32F4) || defined(STM32F7)
    TIM_ENTRY(1, TIM1_UP_TIM10_IRQn),
    #elif defined(STM32H7)
    TIM_ENTRY(1, TIM1_UP_IRQn),
    #elif defined(STM32L4)
    TIM_ENTRY(1, TIM1_UP_TIM16_IRQn),
    #endif
    #endif
    TIM_ENTRY(2, TIM2_IRQn),
    #if defined(TIM3)
    TIM_ENTRY(3, TIM3_IRQn),
    #endif
    #if defined(TIM4)
    TIM_ENTRY(4, TIM4_IRQn),
    #endif
    #if defined(TIM5)
    TIM_ENTRY(5, TIM5_IRQn),
    #endif
    #if defined(TIM6)
    #if defined(STM32F412Zx)
    TIM_ENTRY(6, TIM6_IRQn),
    #else
    TIM_ENTRY(6, TIM6_DAC_IRQn),
    #endif
    #endif
    #if defined(TIM7)
    TIM_ENTRY(7, TIM7_IRQn),
    #endif
    #if defined(TIM8)
    #if defined(STM32F4) || defined(STM32F7) || defined(STM32H7)
    TIM_ENTRY(8, TIM8_UP_TIM13_IRQn),
    #elif defined(STM32L4)
    TIM_ENTRY(8, TIM8_UP_IRQn),
    #endif
    #endif
    #if defined(TIM9)
    TIM_ENTRY(9, TIM1_BRK_TIM9_IRQn),
    #endif
    #if defined(TIM10)
    TIM_ENTRY(10, TIM1_UP_TIM10_IRQn),
    #endif
    #if defined(TIM11)
    TIM_ENTRY(11, TIM1_TRG_COM_TIM11_IRQn),
    #endif
    #if defined(TIM12)
    TIM_ENTRY(12, TIM8_BRK_TIM12_IRQn),
    #endif
    #if defined(TIM13)
    TIM_ENTRY(13, TIM8_UP_TIM13_IRQn),
    #endif
    #if defined(STM32F0)
    TIM_ENTRY(14, TIM14_IRQn),
    #elif defined(TIM14)
    TIM_ENTRY(14, TIM8_TRG_COM_TIM14_IRQn),
    #endif
    #if defined(TIM15)
    #if defined(STM32F0) || defined(STM32H7)
    TIM_ENTRY(15, TIM15_IRQn),
    #else
    TIM_ENTRY(15, TIM1_BRK_TIM15_IRQn),
    #endif
    #endif
    #if defined(TIM16)
    #if defined(STM32F0) || defined(STM32H7)
    TIM_ENTRY(16, TIM16_IRQn),
    #else
    TIM_ENTRY(16, TIM1_UP_TIM16_IRQn),
    #endif
    #endif
    #if defined(TIM17)
    #if defined(STM32F0) || defined(STM32H7)
    TIM_ENTRY(17, TIM17_IRQn),
    #else
    TIM_ENTRY(17, TIM1_TRG_COM_TIM17_IRQn),
    #endif
    #endif
};
#undef TIM_ENTRY

/// \classmethod \constructor(id, ...)
/// Construct a new timer object of the given id.  If additional
/// arguments are given, then the timer is initialised by `init(...)`.
/// `id` can be 1 to 14, excluding 3.
STATIC mp_obj_t pyb_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get the timer id
    mp_int_t tim_id = mp_obj_get_int(args[0]);

    // check if the timer exists
    if (tim_id <= 0 || tim_id > MICROPY_HW_MAX_TIMER || tim_instance_table[tim_id - 1] == 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Timer(%d) doesn't exist"), tim_id);
    }

    pyb_timer_obj_t *tim;
    if (MP_STATE_PORT(pyb_timer_obj_all)[tim_id - 1] == NULL) {
        // create new Timer object
        tim = m_new_obj(pyb_timer_obj_t);
        memset(tim, 0, sizeof(*tim));
        tim->base.type = &pyb_timer_type;
        tim->tim_id = tim_id;
        tim->is_32bit = tim_id == 2 || tim_id == 5;
        tim->callback = mp_const_none;
        uint32_t ti = tim_instance_table[tim_id - 1];
        tim->tim.Instance = (TIM_TypeDef *)(ti & 0xffffff00);
        tim->irqn = ti & 0xff;
        MP_STATE_PORT(pyb_timer_obj_all)[tim_id - 1] = tim;
    } else {
        // reference existing Timer object
        tim = MP_STATE_PORT(pyb_timer_obj_all)[tim_id - 1];
    }

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_timer_init_helper(tim, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(tim);
}

STATIC mp_obj_t pyb_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_timer_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_timer_init_obj, 1, pyb_timer_init);

// timer.deinit()
STATIC mp_obj_t pyb_timer_deinit(mp_obj_t self_in) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Disable the base interrupt
    pyb_timer_callback(self_in, mp_const_none);

    pyb_timer_channel_obj_t *chan = self->channel;
    self->channel = NULL;

    // Disable the channel interrupts
    while (chan != NULL) {
        pyb_timer_channel_callback(MP_OBJ_FROM_PTR(chan), mp_const_none);
        pyb_timer_channel_obj_t *prev_chan = chan;
        chan = chan->next;
        prev_chan->next = NULL;
    }

    self->tim.State = HAL_TIM_STATE_RESET;
    self->tim.Instance->CCER = 0x0000; // disable all capture/compare outputs
    self->tim.Instance->CR1 = 0x0000; // disable the timer and reset its state

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_timer_deinit_obj, pyb_timer_deinit);

/// \method channel(channel, mode, ...)
///
/// If only a channel number is passed, then a previously initialized channel
/// object is returned (or `None` if there is no previous channel).
///
/// Othwerwise, a TimerChannel object is initialized and returned.
///
/// Each channel can be configured to perform pwm, output compare, or
/// input capture. All channels share the same underlying timer, which means
/// that they share the same timer clock.
///
/// Keyword arguments:
///
///   - `mode` can be one of:
///     - `Timer.PWM` - configure the timer in PWM mode (active high).
///     - `Timer.PWM_INVERTED` - configure the timer in PWM mode (active low).
///     - `Timer.OC_TIMING` - indicates that no pin is driven.
///     - `Timer.OC_ACTIVE` - the pin will be made active when a compare
///        match occurs (active is determined by polarity)
///     - `Timer.OC_INACTIVE` - the pin will be made inactive when a compare
///        match occurs.
///     - `Timer.OC_TOGGLE` - the pin will be toggled when an compare match occurs.
///     - `Timer.OC_FORCED_ACTIVE` - the pin is forced active (compare match is ignored).
///     - `Timer.OC_FORCED_INACTIVE` - the pin is forced inactive (compare match is ignored).
///     - `Timer.IC` - configure the timer in Input Capture mode.
///     - `Timer.ENC_A` --- configure the timer in Encoder mode. The counter only changes when CH1 changes.
///     - `Timer.ENC_B` --- configure the timer in Encoder mode. The counter only changes when CH2 changes.
///     - `Timer.ENC_AB` --- configure the timer in Encoder mode. The counter changes when CH1 or CH2 changes.
///
///   - `callback` - as per TimerChannel.callback()
///
///   - `pin` None (the default) or a Pin object. If specified (and not None)
///           this will cause the alternate function of the the indicated pin
///      to be configured for this timer channel. An error will be raised if
///      the pin doesn't support any alternate functions for this timer channel.
///
/// Keyword arguments for Timer.PWM modes:
///
///   - `pulse_width` - determines the initial pulse width value to use.
///   - `pulse_width_percent` - determines the initial pulse width percentage to use.
///
/// Keyword arguments for Timer.OC modes:
///
///   - `compare` - determines the initial value of the compare register.
///
///   - `polarity` can be one of:
///     - `Timer.HIGH` - output is active high
///     - `Timer.LOW` - output is acive low
///
/// Optional keyword arguments for Timer.IC modes:
///
///   - `polarity` can be one of:
///     - `Timer.RISING` - captures on rising edge.
///     - `Timer.FALLING` - captures on falling edge.
///     - `Timer.BOTH` - captures on both edges.
///
///   Note that capture only works on the primary channel, and not on the
///   complimentary channels.
///
/// Notes for Timer.ENC modes:
///
///   - Requires 2 pins, so one or both pins will need to be configured to use
///     the appropriate timer AF using the Pin API.
///   - Read the encoder value using the timer.counter() method.
///   - Only works on CH1 and CH2 (and not on CH1N or CH2N)
///   - The channel number is ignored when setting the encoder mode.
///
/// PWM Example:
///
///     timer = pyb.Timer(2, freq=1000)
///     ch2 = timer.channel(2, pyb.Timer.PWM, pin=pyb.Pin.board.X2, pulse_width=210000)
///     ch3 = timer.channel(3, pyb.Timer.PWM, pin=pyb.Pin.board.X3, pulse_width=420000)
STATIC mp_obj_t pyb_timer_channel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,                MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_callback,            MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_pin,                 MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_pulse_width,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_pulse_width_percent, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_compare,             MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_polarity,            MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
    };

    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_int_t channel = mp_obj_get_int(pos_args[1]);

    if (channel < 1 || channel > 4) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid channel (%d)"), channel);
    }

    pyb_timer_channel_obj_t *chan = self->channel;
    pyb_timer_channel_obj_t *prev_chan = NULL;

    while (chan != NULL) {
        if (chan->channel == channel) {
            break;
        }
        prev_chan = chan;
        chan = chan->next;
    }

    // If only the channel number is given return the previously allocated
    // channel (or None if no previous channel).
    if (n_args == 2 && kw_args->used == 0) {
        if (chan) {
            return MP_OBJ_FROM_PTR(chan);
        }
        return mp_const_none;
    }

    // If there was already a channel, then remove it from the list. Note that
    // the order we do things here is important so as to appear atomic to
    // the IRQ handler.
    if (chan) {
        // Turn off any IRQ associated with the channel.
        pyb_timer_channel_callback(MP_OBJ_FROM_PTR(chan), mp_const_none);

        // Unlink the channel from the list.
        if (prev_chan) {
            prev_chan->next = chan->next;
        }
        self->channel = chan->next;
        chan->next = NULL;
    }

    // Allocate and initialize a new channel
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    chan = m_new_obj(pyb_timer_channel_obj_t);
    memset(chan, 0, sizeof(*chan));
    chan->base.type = &pyb_timer_channel_type;
    chan->timer = self;
    chan->channel = channel;
    chan->mode = args[0].u_int;
    chan->callback = args[1].u_obj;

    mp_obj_t pin_obj = args[2].u_obj;
    if (pin_obj != mp_const_none) {
        if (!mp_obj_is_type(pin_obj, &pin_type)) {
            mp_raise_ValueError(MP_ERROR_TEXT("pin argument needs to be be a Pin type"));
        }
        const pin_obj_t *pin = MP_OBJ_TO_PTR(pin_obj);
        const pin_af_obj_t *af = pin_find_af(pin, AF_FN_TIM, self->tim_id);
        if (af == NULL) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%q) doesn't have an af for Timer(%d)"), pin->name, self->tim_id);
        }
        // pin.init(mode=AF_PP, af=idx)
        const mp_obj_t args2[6] = {
            MP_OBJ_FROM_PTR(&pin_init_obj),
            pin_obj,
            MP_OBJ_NEW_QSTR(MP_QSTR_mode),  MP_OBJ_NEW_SMALL_INT(GPIO_MODE_AF_PP),
            MP_OBJ_NEW_QSTR(MP_QSTR_af),    MP_OBJ_NEW_SMALL_INT(af->idx)
        };
        mp_call_method_n_kw(0, 2, args2);
    }

    // Link the channel to the timer before we turn the channel on.
    // Note that this needs to appear atomic to the IRQ handler (the write
    // to self->channel is atomic, so we're good, but I thought I'd mention
    // in case this was ever changed in the future).
    chan->next = self->channel;
    self->channel = chan;

    switch (chan->mode) {

        case CHANNEL_MODE_PWM_NORMAL:
        case CHANNEL_MODE_PWM_INVERTED: {
            TIM_OC_InitTypeDef oc_config;
            oc_config.OCMode = channel_mode_info[chan->mode].oc_mode;
            if (args[4].u_obj != mp_const_none) {
                // pulse width percent given
                uint32_t period = compute_period(self);
                oc_config.Pulse = compute_pwm_value_from_percent(period, args[4].u_obj);
            } else {
                // use absolute pulse width value (defaults to 0 if nothing given)
                oc_config.Pulse = args[3].u_int;
            }
            oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
            oc_config.OCFastMode = TIM_OCFAST_DISABLE;
            #if !defined(STM32L0)
            oc_config.OCNPolarity = TIM_OCNPOLARITY_HIGH;
            oc_config.OCIdleState = TIM_OCIDLESTATE_SET;
            oc_config.OCNIdleState = TIM_OCNIDLESTATE_SET;
            #endif

            HAL_TIM_PWM_ConfigChannel(&self->tim, &oc_config, TIMER_CHANNEL(chan));
            if (chan->callback == mp_const_none) {
                HAL_TIM_PWM_Start(&self->tim, TIMER_CHANNEL(chan));
            } else {
                pyb_timer_channel_callback(MP_OBJ_FROM_PTR(chan), chan->callback);
            }
            #if !defined(STM32L0)
            // Start the complimentary channel too (if its supported)
            if (IS_TIM_CCXN_INSTANCE(self->tim.Instance, TIMER_CHANNEL(chan))) {
                HAL_TIMEx_PWMN_Start(&self->tim, TIMER_CHANNEL(chan));
            }
            #endif
            break;
        }

        case CHANNEL_MODE_OC_TIMING:
        case CHANNEL_MODE_OC_ACTIVE:
        case CHANNEL_MODE_OC_INACTIVE:
        case CHANNEL_MODE_OC_TOGGLE:
        case CHANNEL_MODE_OC_FORCED_ACTIVE:
        case CHANNEL_MODE_OC_FORCED_INACTIVE: {
            TIM_OC_InitTypeDef oc_config;
            oc_config.OCMode = channel_mode_info[chan->mode].oc_mode;
            oc_config.Pulse = args[5].u_int;
            oc_config.OCPolarity = args[6].u_int;
            if (oc_config.OCPolarity == 0xffffffff) {
                oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
            }
            oc_config.OCFastMode = TIM_OCFAST_DISABLE;
            #if !defined(STM32L0)
            if (oc_config.OCPolarity == TIM_OCPOLARITY_HIGH) {
                oc_config.OCNPolarity = TIM_OCNPOLARITY_HIGH;
            } else {
                oc_config.OCNPolarity = TIM_OCNPOLARITY_LOW;
            }
            oc_config.OCIdleState = TIM_OCIDLESTATE_SET;
            oc_config.OCNIdleState = TIM_OCNIDLESTATE_SET;
            #endif

            if (!IS_TIM_OC_POLARITY(oc_config.OCPolarity)) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid polarity (%d)"), oc_config.OCPolarity);
            }
            HAL_TIM_OC_ConfigChannel(&self->tim, &oc_config, TIMER_CHANNEL(chan));
            if (chan->callback == mp_const_none) {
                HAL_TIM_OC_Start(&self->tim, TIMER_CHANNEL(chan));
            } else {
                pyb_timer_channel_callback(MP_OBJ_FROM_PTR(chan), chan->callback);
            }
            #if !defined(STM32L0)
            // Start the complimentary channel too (if its supported)
            if (IS_TIM_CCXN_INSTANCE(self->tim.Instance, TIMER_CHANNEL(chan))) {
                HAL_TIMEx_OCN_Start(&self->tim, TIMER_CHANNEL(chan));
            }
            #endif
            break;
        }

        case CHANNEL_MODE_IC: {
            TIM_IC_InitTypeDef ic_config;

            ic_config.ICPolarity = args[6].u_int;
            if (ic_config.ICPolarity == 0xffffffff) {
                ic_config.ICPolarity = TIM_ICPOLARITY_RISING;
            }
            ic_config.ICSelection = TIM_ICSELECTION_DIRECTTI;
            ic_config.ICPrescaler = TIM_ICPSC_DIV1;
            ic_config.ICFilter = 0;

            if (!IS_TIM_IC_POLARITY(ic_config.ICPolarity)) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid polarity (%d)"), ic_config.ICPolarity);
            }
            HAL_TIM_IC_ConfigChannel(&self->tim, &ic_config, TIMER_CHANNEL(chan));
            if (chan->callback == mp_const_none) {
                HAL_TIM_IC_Start(&self->tim, TIMER_CHANNEL(chan));
            } else {
                pyb_timer_channel_callback(MP_OBJ_FROM_PTR(chan), chan->callback);
            }
            break;
        }

        case CHANNEL_MODE_ENC_A:
        case CHANNEL_MODE_ENC_B:
        case CHANNEL_MODE_ENC_AB: {
            TIM_Encoder_InitTypeDef enc_config;

            enc_config.EncoderMode = channel_mode_info[chan->mode].oc_mode;
            enc_config.IC1Polarity = args[6].u_int;
            if (enc_config.IC1Polarity == 0xffffffff) {
                enc_config.IC1Polarity = TIM_ICPOLARITY_RISING;
            }
            enc_config.IC2Polarity = enc_config.IC1Polarity;
            enc_config.IC1Selection = TIM_ICSELECTION_DIRECTTI;
            enc_config.IC2Selection = TIM_ICSELECTION_DIRECTTI;
            enc_config.IC1Prescaler = TIM_ICPSC_DIV1;
            enc_config.IC2Prescaler = TIM_ICPSC_DIV1;
            enc_config.IC1Filter = 0;
            enc_config.IC2Filter = 0;

            if (!IS_TIM_IC_POLARITY(enc_config.IC1Polarity)) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid polarity (%d)"), enc_config.IC1Polarity);
            }
            // Only Timers 1, 2, 3, 4, 5, and 8 support encoder mode
            if (
                #if defined(TIM1)
                self->tim.Instance != TIM1
                &&
                #endif
                self->tim.Instance != TIM2
                #if defined(TIM3)
                && self->tim.Instance != TIM3
                #endif
                #if defined(TIM4)
                && self->tim.Instance != TIM4
                #endif
                #if defined(TIM5)
                && self->tim.Instance != TIM5
                #endif
                #if defined(TIM8)
                && self->tim.Instance != TIM8
                #endif
                ) {
                mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("encoder not supported on timer %d"), self->tim_id);
            }

            // Disable & clear the timer interrupt so that we don't trigger
            // an interrupt by initializing the timer.
            __HAL_TIM_DISABLE_IT(&self->tim, TIM_IT_UPDATE);
            HAL_TIM_Encoder_Init(&self->tim, &enc_config);
            __HAL_TIM_SET_COUNTER(&self->tim, 0);
            if (self->callback != mp_const_none) {
                __HAL_TIM_CLEAR_FLAG(&self->tim, TIM_IT_UPDATE);
                __HAL_TIM_ENABLE_IT(&self->tim, TIM_IT_UPDATE);
            }
            HAL_TIM_Encoder_Start(&self->tim, TIM_CHANNEL_ALL);
            break;
        }

        default:
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid mode (%d)"), chan->mode);
    }

    return MP_OBJ_FROM_PTR(chan);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_timer_channel_obj, 2, pyb_timer_channel);

/// \method counter([value])
/// Get or set the timer counter.
STATIC mp_obj_t pyb_timer_counter(size_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->tim.Instance->CNT);
    } else {
        // set
        __HAL_TIM_SET_COUNTER(&self->tim, mp_obj_get_int(args[1]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_counter_obj, 1, 2, pyb_timer_counter);

/// \method source_freq()
/// Get the frequency of the source of the timer.
STATIC mp_obj_t pyb_timer_source_freq(mp_obj_t self_in) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t source_freq = timer_get_source_freq(self->tim_id);
    return mp_obj_new_int(source_freq);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_timer_source_freq_obj, pyb_timer_source_freq);

/// \method freq([value])
/// Get or set the frequency for the timer (changes prescaler and period if set).
STATIC mp_obj_t pyb_timer_freq(size_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        uint32_t prescaler = self->tim.Instance->PSC & 0xffff;
        uint32_t period = __HAL_TIM_GET_AUTORELOAD(&self->tim) & TIMER_CNT_MASK(self);
        uint32_t source_freq = timer_get_source_freq(self->tim_id);
        uint32_t divide_a = prescaler + 1;
        uint32_t divide_b = period + 1;
        #if MICROPY_PY_BUILTINS_FLOAT
        if (source_freq % divide_a != 0) {
            return mp_obj_new_float((mp_float_t)source_freq / (mp_float_t)divide_a / (mp_float_t)divide_b);
        }
        source_freq /= divide_a;
        if (source_freq % divide_b != 0) {
            return mp_obj_new_float((mp_float_t)source_freq / (mp_float_t)divide_b);
        } else {
            return mp_obj_new_int(source_freq / divide_b);
        }
        #else
        return mp_obj_new_int(source_freq / divide_a / divide_b);
        #endif
    } else {
        // set
        uint32_t period;
        uint32_t prescaler = compute_prescaler_period_from_freq(self, args[1], &period);
        self->tim.Instance->PSC = prescaler;
        __HAL_TIM_SET_AUTORELOAD(&self->tim, period);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_freq_obj, 1, 2, pyb_timer_freq);

/// \method prescaler([value])
/// Get or set the prescaler for the timer.
STATIC mp_obj_t pyb_timer_prescaler(size_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->tim.Instance->PSC & 0xffff);
    } else {
        // set
        self->tim.Instance->PSC = mp_obj_get_int(args[1]) & 0xffff;
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_prescaler_obj, 1, 2, pyb_timer_prescaler);

/// \method period([value])
/// Get or set the period of the timer.
STATIC mp_obj_t pyb_timer_period(size_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        return mp_obj_new_int(__HAL_TIM_GET_AUTORELOAD(&self->tim) & TIMER_CNT_MASK(self));
    } else {
        // set
        __HAL_TIM_SET_AUTORELOAD(&self->tim, mp_obj_get_int(args[1]) & TIMER_CNT_MASK(self));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_period_obj, 1, 2, pyb_timer_period);

/// \method callback(fun)
/// Set the function to be called when the timer triggers.
/// `fun` is passed 1 argument, the timer object.
/// If `fun` is `None` then the callback will be disabled.
STATIC mp_obj_t pyb_timer_callback(mp_obj_t self_in, mp_obj_t callback) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (callback == mp_const_none) {
        // stop interrupt (but not timer)
        __HAL_TIM_DISABLE_IT(&self->tim, TIM_IT_UPDATE);
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        __HAL_TIM_DISABLE_IT(&self->tim, TIM_IT_UPDATE);
        self->callback = callback;
        // start timer, so that it interrupts on overflow, but clear any
        // pending interrupts which may have been set by initializing it.
        __HAL_TIM_CLEAR_FLAG(&self->tim, TIM_IT_UPDATE);
        HAL_TIM_Base_Start_IT(&self->tim); // This will re-enable the IRQ
        HAL_NVIC_EnableIRQ(self->irqn);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("callback must be None or a callable object"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_timer_callback_obj, pyb_timer_callback);

STATIC const mp_rom_map_elem_t pyb_timer_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pyb_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&pyb_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_channel), MP_ROM_PTR(&pyb_timer_channel_obj) },
    { MP_ROM_QSTR(MP_QSTR_counter), MP_ROM_PTR(&pyb_timer_counter_obj) },
    { MP_ROM_QSTR(MP_QSTR_source_freq), MP_ROM_PTR(&pyb_timer_source_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&pyb_timer_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_prescaler), MP_ROM_PTR(&pyb_timer_prescaler_obj) },
    { MP_ROM_QSTR(MP_QSTR_period), MP_ROM_PTR(&pyb_timer_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&pyb_timer_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_UP), MP_ROM_INT(TIM_COUNTERMODE_UP) },
    { MP_ROM_QSTR(MP_QSTR_DOWN), MP_ROM_INT(TIM_COUNTERMODE_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_CENTER), MP_ROM_INT(TIM_COUNTERMODE_CENTERALIGNED1) },
    { MP_ROM_QSTR(MP_QSTR_PWM), MP_ROM_INT(CHANNEL_MODE_PWM_NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_PWM_INVERTED), MP_ROM_INT(CHANNEL_MODE_PWM_INVERTED) },
    { MP_ROM_QSTR(MP_QSTR_OC_TIMING), MP_ROM_INT(CHANNEL_MODE_OC_TIMING) },
    { MP_ROM_QSTR(MP_QSTR_OC_ACTIVE), MP_ROM_INT(CHANNEL_MODE_OC_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_OC_INACTIVE), MP_ROM_INT(CHANNEL_MODE_OC_INACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_OC_TOGGLE), MP_ROM_INT(CHANNEL_MODE_OC_TOGGLE) },
    { MP_ROM_QSTR(MP_QSTR_OC_FORCED_ACTIVE), MP_ROM_INT(CHANNEL_MODE_OC_FORCED_ACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_OC_FORCED_INACTIVE), MP_ROM_INT(CHANNEL_MODE_OC_FORCED_INACTIVE) },
    { MP_ROM_QSTR(MP_QSTR_IC), MP_ROM_INT(CHANNEL_MODE_IC) },
    { MP_ROM_QSTR(MP_QSTR_ENC_A), MP_ROM_INT(CHANNEL_MODE_ENC_A) },
    { MP_ROM_QSTR(MP_QSTR_ENC_B), MP_ROM_INT(CHANNEL_MODE_ENC_B) },
    { MP_ROM_QSTR(MP_QSTR_ENC_AB), MP_ROM_INT(CHANNEL_MODE_ENC_AB) },
    { MP_ROM_QSTR(MP_QSTR_HIGH), MP_ROM_INT(TIM_OCPOLARITY_HIGH) },
    { MP_ROM_QSTR(MP_QSTR_LOW), MP_ROM_INT(TIM_OCPOLARITY_LOW) },
    { MP_ROM_QSTR(MP_QSTR_RISING), MP_ROM_INT(TIM_ICPOLARITY_RISING) },
    { MP_ROM_QSTR(MP_QSTR_FALLING), MP_ROM_INT(TIM_ICPOLARITY_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_BOTH), MP_ROM_INT(TIM_ICPOLARITY_BOTHEDGE) },
    { MP_ROM_QSTR(MP_QSTR_BRK_OFF), MP_ROM_INT(BRK_OFF) },
    { MP_ROM_QSTR(MP_QSTR_BRK_LOW), MP_ROM_INT(BRK_LOW) },
    { MP_ROM_QSTR(MP_QSTR_BRK_HIGH), MP_ROM_INT(BRK_HIGH) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_timer_locals_dict, pyb_timer_locals_dict_table);

const mp_obj_type_t pyb_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = pyb_timer_print,
    .make_new = pyb_timer_make_new,
    .locals_dict = (mp_obj_dict_t *)&pyb_timer_locals_dict,
};

/// \moduleref pyb
/// \class TimerChannel - setup a channel for a timer.
///
/// Timer channels are used to generate/capture a signal using a timer.
///
/// TimerChannel objects are created using the Timer.channel() method.
STATIC void pyb_timer_channel_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_channel_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "TimerChannel(timer=%u, channel=%u, mode=%s)",
        self->timer->tim_id,
        self->channel,
        qstr_str(channel_mode_info[self->mode].name));
}

/// \method capture([value])
/// Get or set the capture value associated with a channel.
/// capture, compare, and pulse_width are all aliases for the same function.
/// capture is the logical name to use when the channel is in input capture mode.

/// \method compare([value])
/// Get or set the compare value associated with a channel.
/// capture, compare, and pulse_width are all aliases for the same function.
/// compare is the logical name to use when the channel is in output compare mode.

/// \method pulse_width([value])
/// Get or set the pulse width value associated with a channel.
/// capture, compare, and pulse_width are all aliases for the same function.
/// pulse_width is the logical name to use when the channel is in PWM mode.
///
/// In edge aligned mode, a pulse_width of `period + 1` corresponds to a duty cycle of 100%
/// In center aligned mode, a pulse width of `period` corresponds to a duty cycle of 100%
STATIC mp_obj_t pyb_timer_channel_capture_compare(size_t n_args, const mp_obj_t *args) {
    pyb_timer_channel_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        return mp_obj_new_int(__HAL_TIM_GET_COMPARE(&self->timer->tim, TIMER_CHANNEL(self)) & TIMER_CNT_MASK(self->timer));
    } else {
        // set
        __HAL_TIM_SET_COMPARE(&self->timer->tim, TIMER_CHANNEL(self), mp_obj_get_int(args[1]) & TIMER_CNT_MASK(self->timer));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_channel_capture_compare_obj, 1, 2, pyb_timer_channel_capture_compare);

/// \method pulse_width_percent([value])
/// Get or set the pulse width percentage associated with a channel.  The value
/// is a number between 0 and 100 and sets the percentage of the timer period
/// for which the pulse is active.  The value can be an integer or
/// floating-point number for more accuracy.  For example, a value of 25 gives
/// a duty cycle of 25%.
STATIC mp_obj_t pyb_timer_channel_pulse_width_percent(size_t n_args, const mp_obj_t *args) {
    pyb_timer_channel_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t period = compute_period(self->timer);
    if (n_args == 1) {
        // get
        uint32_t cmp = __HAL_TIM_GET_COMPARE(&self->timer->tim, TIMER_CHANNEL(self)) & TIMER_CNT_MASK(self->timer);
        return compute_percent_from_pwm_value(period, cmp);
    } else {
        // set
        uint32_t cmp = compute_pwm_value_from_percent(period, args[1]);
        __HAL_TIM_SET_COMPARE(&self->timer->tim, TIMER_CHANNEL(self), cmp & TIMER_CNT_MASK(self->timer));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_channel_pulse_width_percent_obj, 1, 2, pyb_timer_channel_pulse_width_percent);

/// \method callback(fun)
/// Set the function to be called when the timer channel triggers.
/// `fun` is passed 1 argument, the timer object.
/// If `fun` is `None` then the callback will be disabled.
STATIC mp_obj_t pyb_timer_channel_callback(mp_obj_t self_in, mp_obj_t callback) {
    pyb_timer_channel_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (callback == mp_const_none) {
        // stop interrupt (but not timer)
        __HAL_TIM_DISABLE_IT(&self->timer->tim, TIMER_IRQ_MASK(self->channel));
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
        __HAL_TIM_CLEAR_IT(&self->timer->tim, TIMER_IRQ_MASK(self->channel));
        #if defined(TIM1)
        if (self->timer->tim_id == 1) {
            HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
        } else
        #endif
        #if defined(TIM8) // STM32F401 doesn't have a TIM8
        if (self->timer->tim_id == 8) {
            HAL_NVIC_EnableIRQ(TIM8_CC_IRQn);
        } else
        #endif
        {
            HAL_NVIC_EnableIRQ(self->timer->irqn);
        }
        // start timer, so that it interrupts on overflow
        switch (self->mode) {
            case CHANNEL_MODE_PWM_NORMAL:
            case CHANNEL_MODE_PWM_INVERTED:
                HAL_TIM_PWM_Start_IT(&self->timer->tim, TIMER_CHANNEL(self));
                break;
            case CHANNEL_MODE_OC_TIMING:
            case CHANNEL_MODE_OC_ACTIVE:
            case CHANNEL_MODE_OC_INACTIVE:
            case CHANNEL_MODE_OC_TOGGLE:
            case CHANNEL_MODE_OC_FORCED_ACTIVE:
            case CHANNEL_MODE_OC_FORCED_INACTIVE:
                HAL_TIM_OC_Start_IT(&self->timer->tim, TIMER_CHANNEL(self));
                break;
            case CHANNEL_MODE_IC:
                HAL_TIM_IC_Start_IT(&self->timer->tim, TIMER_CHANNEL(self));
                break;
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("callback must be None or a callable object"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_timer_channel_callback_obj, pyb_timer_channel_callback);

STATIC const mp_rom_map_elem_t pyb_timer_channel_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&pyb_timer_channel_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_pulse_width), MP_ROM_PTR(&pyb_timer_channel_capture_compare_obj) },
    { MP_ROM_QSTR(MP_QSTR_pulse_width_percent), MP_ROM_PTR(&pyb_timer_channel_pulse_width_percent_obj) },
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&pyb_timer_channel_capture_compare_obj) },
    { MP_ROM_QSTR(MP_QSTR_compare), MP_ROM_PTR(&pyb_timer_channel_capture_compare_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_timer_channel_locals_dict, pyb_timer_channel_locals_dict_table);

STATIC const mp_obj_type_t pyb_timer_channel_type = {
    { &mp_type_type },
    .name = MP_QSTR_TimerChannel,
    .print = pyb_timer_channel_print,
    .locals_dict = (mp_obj_dict_t *)&pyb_timer_channel_locals_dict,
};

STATIC void timer_handle_irq_channel(pyb_timer_obj_t *tim, uint8_t channel, mp_obj_t callback) {
    uint32_t irq_mask = TIMER_IRQ_MASK(channel);

    if (__HAL_TIM_GET_FLAG(&tim->tim, irq_mask) != RESET) {
        if (__HAL_TIM_GET_IT_SOURCE(&tim->tim, irq_mask) != RESET) {
            // clear the interrupt
            __HAL_TIM_CLEAR_IT(&tim->tim, irq_mask);

            // execute callback if it's set
            if (callback != mp_const_none) {
                mp_sched_lock();
                // When executing code within a handler we must lock the GC to prevent
                // any memory allocations.  We must also catch any exceptions.
                gc_lock();
                nlr_buf_t nlr;
                if (nlr_push(&nlr) == 0) {
                    mp_call_function_1(callback, MP_OBJ_FROM_PTR(tim));
                    nlr_pop();
                } else {
                    // Uncaught exception; disable the callback so it doesn't run again.
                    tim->callback = mp_const_none;
                    __HAL_TIM_DISABLE_IT(&tim->tim, irq_mask);
                    if (channel == 0) {
                        mp_printf(MICROPY_ERROR_PRINTER, "uncaught exception in Timer(%u) interrupt handler\n", tim->tim_id);
                    } else {
                        mp_printf(MICROPY_ERROR_PRINTER, "uncaught exception in Timer(%u) channel %u interrupt handler\n", tim->tim_id, channel);
                    }
                    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
                }
                gc_unlock();
                mp_sched_unlock();
            }
        }
    }
}

void timer_irq_handler(uint tim_id) {
    if (tim_id - 1 < PYB_TIMER_OBJ_ALL_NUM) {
        // get the timer object
        pyb_timer_obj_t *tim = MP_STATE_PORT(pyb_timer_obj_all)[tim_id - 1];

        if (tim == NULL) {
            // Timer object has not been set, so we can't do anything.
            // This can happen under normal circumstances for timers like
            // 1 & 10 which use the same IRQ.
            return;
        }

        // Check for timer (versus timer channel) interrupt.
        timer_handle_irq_channel(tim, 0, tim->callback);
        uint32_t handled = TIMER_IRQ_MASK(0);

        // Check to see if a timer channel interrupt was pending
        pyb_timer_channel_obj_t *chan = tim->channel;
        while (chan != NULL) {
            timer_handle_irq_channel(tim, chan->channel, chan->callback);
            handled |= TIMER_IRQ_MASK(chan->channel);
            chan = chan->next;
        }

        // Finally, clear any remaining interrupt sources. Otherwise we'll
        // just get called continuously.
        uint32_t unhandled = tim->tim.Instance->DIER & 0xff & ~handled;
        if (unhandled != 0) {
            __HAL_TIM_DISABLE_IT(&tim->tim, unhandled);
            __HAL_TIM_CLEAR_IT(&tim->tim, unhandled);
            mp_printf(MICROPY_ERROR_PRINTER, "unhandled interrupt SR=0x%02x (now disabled)\n", (unsigned int)unhandled);
        }
    }
}
