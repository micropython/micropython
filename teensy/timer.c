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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "pin.h"
#include "reg.h"
#include "timer.h"

typedef enum {
    CHANNEL_MODE_PWM_NORMAL,
    CHANNEL_MODE_PWM_INVERTED,
    CHANNEL_MODE_OC_TIMING,
    CHANNEL_MODE_OC_ACTIVE,
    CHANNEL_MODE_OC_INACTIVE,
    CHANNEL_MODE_OC_TOGGLE,
//    CHANNEL_MODE_OC_FORCED_ACTIVE,
//    CHANNEL_MODE_OC_FORCED_INACTIVE,
    CHANNEL_MODE_IC,
} pyb_channel_mode;

STATIC const struct {
    qstr        name;
    uint32_t    oc_mode;
} channel_mode_info[] = {
    { MP_QSTR_PWM,                FTM_OCMODE_PWM1 },
    { MP_QSTR_PWM_INVERTED,       FTM_OCMODE_PWM2 },
    { MP_QSTR_OC_TIMING,          FTM_OCMODE_TIMING },
    { MP_QSTR_OC_ACTIVE,          FTM_OCMODE_ACTIVE },
    { MP_QSTR_OC_INACTIVE,        FTM_OCMODE_INACTIVE },
    { MP_QSTR_OC_TOGGLE,          FTM_OCMODE_TOGGLE },
//    { MP_QSTR_OC_FORCED_ACTIVE,   FTM_OCMODE_FORCED_ACTIVE },
//    { MP_QSTR_OC_FORCED_INACTIVE, FTM_OCMODE_FORCED_INACTIVE },
    { MP_QSTR_IC,                 0 },
};

struct _pyb_timer_obj_t;

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
    uint8_t irqn;
    mp_obj_t callback;
    FTM_HandleTypeDef ftm;
    pyb_timer_channel_obj_t *channel;
} pyb_timer_obj_t;

// Used to do callbacks to Python code on interrupt
STATIC pyb_timer_obj_t *pyb_timer_obj_all[3];
#define PYB_TIMER_OBJ_ALL_NUM MP_ARRAY_SIZE(pyb_timer_obj_all)

STATIC mp_obj_t pyb_timer_deinit(mp_obj_t self_in);
STATIC mp_obj_t pyb_timer_callback(mp_obj_t self_in, mp_obj_t callback);
STATIC mp_obj_t pyb_timer_channel_callback(mp_obj_t self_in, mp_obj_t callback);

void timer_init0(void) {
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

mp_uint_t get_prescaler_shift(mp_int_t prescaler) {
    mp_uint_t prescaler_shift;
    for (prescaler_shift = 0; prescaler_shift < 8; prescaler_shift++) {
        if (prescaler == (1 << prescaler_shift)) {
            return prescaler_shift;
        }
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "prescaler must be a power of 2 between 1 and 128, not %d", prescaler));
}

/******************************************************************************/
/* MicroPython bindings                                                       */

STATIC const mp_obj_type_t pyb_timer_channel_type;

// Helper function for determining the period used for calculating percent
STATIC uint32_t compute_period(pyb_timer_obj_t *self) {
    // In center mode,  compare == period corresponds to 100%
    // In edge mode, compare == (period + 1) corresponds to 100%
    FTM_TypeDef *FTMx = self->ftm.Instance;
    uint32_t period = (FTMx->MOD & 0xffff);
    if ((FTMx->SC & FTM_SC_CPWMS) == 0) {
        // Edge mode
        period++;
    }
    return period;
}

// Helper function to compute PWM value from timer period and percent value.
// 'val' can be an int or a float between 0 and 100 (out of range values are
// clamped).
STATIC uint32_t compute_pwm_value_from_percent(uint32_t period, mp_obj_t percent_in) {
    uint32_t cmp;
    if (0) {
    #if MICROPY_PY_BUILTINS_FLOAT
    } else if (MP_OBJ_IS_TYPE(percent_in, &mp_type_float)) {
        float percent = mp_obj_get_float(percent_in);
        if (percent <= 0.0) {
            cmp = 0;
        } else if (percent >= 100.0) {
            cmp = period;
        } else {
            cmp = percent / 100.0 * ((float)period);
        }
    #endif
    } else {
        mp_int_t percent = mp_obj_get_int(percent_in);
        if (percent <= 0) {
            cmp = 0;
        } else if (percent >= 100) {
            cmp = period;
        } else {
            cmp = ((uint32_t)percent * period) / 100;
        }
    }
    return cmp;
}

// Helper function to compute percentage from timer perion and PWM value.
STATIC mp_obj_t compute_percent_from_pwm_value(uint32_t period, uint32_t cmp) {
    #if MICROPY_PY_BUILTINS_FLOAT
    float percent = (float)cmp * 100.0 / (float)period;
    if (cmp >= period) {
        percent = 100.0;
    } else {
        percent = (float)cmp * 100.0 / (float)period;
    }
    return mp_obj_new_float(percent);
    #else
    mp_int_t percent;
    if (cmp >= period) {
        percent = 100;
    } else {
        percent = cmp * 100 / period;
    }
    return mp_obj_new_int(percent);
    #endif
}

STATIC void pyb_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_obj_t *self = self_in;

    if (self->ftm.State == HAL_FTM_STATE_RESET) {
        mp_printf(print, "Timer(%u)", self->tim_id);
    } else {
        mp_printf(print, "Timer(%u, prescaler=%u, period=%u, mode=%s)",
            self->tim_id,
            1 << (self->ftm.Instance->SC & 7),
            self->ftm.Instance->MOD & 0xffff,
            self->ftm.Init.CounterMode == FTM_COUNTERMODE_UP ? "UP" : "CENTER");
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
///   - `prescaler` 1, 2, 4, 8 16 32, 64 or 128 - specifies the value to be loaded into the
///                 timer's prescaler. The timer clock source is divided by
///     (`prescaler`) to arrive at the timer clock.
///
///   - `period` [0-0xffff] - Specifies the value to be loaded into the timer's
///     Modulo Register (MOD). This determines the period of the timer (i.e.
///     when the counter cycles). The timer counter will roll-over after
///     `period` timer clock cycles. In center mode, a compare register > 0x7fff
///     doesn't seem to work properly, so keep this in mind.
///
///   - `mode` can be one of:
///     - `Timer.UP` - configures the timer to count from 0 to MOD (default)
///     - `Timer.CENTER` - confgures the timer to count from 0 to MOD and
///       then back down to 0.
///
///   - `callback` - as per Timer.callback()
///
///  You must either specify freq or both of period and prescaler.
STATIC const mp_arg_t pyb_timer_init_args[] = {
    { MP_QSTR_freq,             MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
    { MP_QSTR_prescaler,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
    { MP_QSTR_period,           MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
    { MP_QSTR_mode,             MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = FTM_COUNTERMODE_UP} },
    { MP_QSTR_callback,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
};
#define PYB_TIMER_INIT_NUM_ARGS MP_ARRAY_SIZE(pyb_timer_init_args)

STATIC mp_obj_t pyb_timer_init_helper(pyb_timer_obj_t *self, uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_TIMER_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_TIMER_INIT_NUM_ARGS, pyb_timer_init_args, vals);

    FTM_HandleTypeDef *ftm = &self->ftm;

    // set the TIM configuration values
    FTM_Base_InitTypeDef *init = &ftm->Init;

    if (vals[0].u_int != 0xffffffff) {
        // set prescaler and period from frequency

        if (vals[0].u_int == 0) {
            mp_raise_ValueError("can't have 0 frequency");
        }

        uint32_t period = MAX(1, F_BUS / vals[0].u_int);
        uint32_t prescaler_shift = 0;
        while (period > 0xffff && prescaler_shift < 7) {
            period >>= 1;
            prescaler_shift++;
        }
        if (period > 0xffff) {
            period = 0xffff;
        }
        init->PrescalerShift = prescaler_shift;
        init->Period = period;
    } else if (vals[1].u_int != 0xffffffff && vals[2].u_int != 0xffffffff) {
        // set prescaler and period directly
        init->PrescalerShift = get_prescaler_shift(vals[1].u_int);
        init->Period = vals[2].u_int;
        if (!IS_FTM_PERIOD(init->Period)) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "period must be between 0 and 65535, not %d", init->Period));
        }
    } else {
        mp_raise_TypeError("must specify either freq, or prescaler and period");
    }

    init->CounterMode = vals[3].u_int;
    if (!IS_FTM_COUNTERMODE(init->CounterMode)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_TypeError, "invalid counter mode: %d", init->CounterMode));
    }

    // Currently core/mk20dx128.c sets SIM_SCGC6_FTM0, SIM_SCGC6_FTM1, SIM_SCGC3_FTM2
    // so we don't need to do it here.

    NVIC_SET_PRIORITY(self->irqn, 0xe); // next-to lowest priority

    HAL_FTM_Base_Init(ftm);
    if (vals[4].u_obj == mp_const_none) {
        HAL_FTM_Base_Start(ftm);
    } else {
        pyb_timer_callback(self, vals[4].u_obj);
    }

    return mp_const_none;
}

/// \classmethod \constructor(id, ...)
/// Construct a new timer object of the given id.  If additional
/// arguments are given, then the timer is initialised by `init(...)`.
/// `id` can be 1 to 14, excluding 3.
STATIC mp_obj_t pyb_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // create new Timer object
    pyb_timer_obj_t *tim = m_new_obj(pyb_timer_obj_t);
    memset(tim, 0, sizeof(*tim));

    tim->base.type = &pyb_timer_type;
    tim->callback = mp_const_none;
    tim->channel = NULL;

    // get FTM number
    tim->tim_id = mp_obj_get_int(args[0]);

    switch (tim->tim_id) {
        case 0: tim->ftm.Instance = FTM0; tim->irqn = IRQ_FTM0; break;
        case 1: tim->ftm.Instance = FTM1; tim->irqn = IRQ_FTM1; break;
        case 2: tim->ftm.Instance = FTM2; tim->irqn = IRQ_FTM2; break;
        default: nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Timer %d does not exist", tim->tim_id));
    }

    if (n_args > 1 || n_kw > 0) {
        // start the peripheral
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_timer_init_helper(tim, n_args - 1, args + 1, &kw_args);
    }

    // set the global variable for interrupt callbacks
    if (tim->tim_id < PYB_TIMER_OBJ_ALL_NUM) {
        pyb_timer_obj_all[tim->tim_id] = tim;
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
/// Disables any channel callbacks (and the associated irq).
/// Stops the timer, and disables the timer peripheral.
STATIC mp_obj_t pyb_timer_deinit(mp_obj_t self_in) {
    pyb_timer_obj_t *self = self_in;

    // Disable the base interrupt
    pyb_timer_callback(self_in, mp_const_none);

    pyb_timer_channel_obj_t *chan = self->channel;
    self->channel = NULL;

    // Disable the channel interrupts
    while (chan != NULL) {
        pyb_timer_channel_callback(chan, mp_const_none);
        pyb_timer_channel_obj_t *prev_chan = chan;
        chan = chan->next;
        prev_chan->next = NULL;
    }

    HAL_FTM_Base_DeInit(&self->ftm);
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
///     - `Timer.IC` - configure the timer in Input Capture mode.
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
/// PWM Example:
///
///     timer = pyb.Timer(0, prescaler=128, period=37500, counter_mode=pyb.Timer.COUNTER_MODE_CENTER)
///     ch0 = t0.channel(0, pyb.Timer.PWM, pin=pyb.Pin.board.D22, pulse_width=(t0.period() + 1) // 4)
///     ch1 = t0.channel(1, pyb.Timer.PWM, pin=pyb.Pin.board.D23, pulse_width=(t0.period() + 1) // 2)
STATIC const mp_arg_t pyb_timer_channel_args[] = {
    { MP_QSTR_callback,            MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_pin,                 MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_pulse_width,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_pulse_width_percent, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_compare,             MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_polarity,            MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
};
#define PYB_TIMER_CHANNEL_NUM_ARGS MP_ARRAY_SIZE(pyb_timer_channel_args)

STATIC mp_obj_t pyb_timer_channel(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_timer_obj_t *self = args[0];
    mp_int_t channel = mp_obj_get_int(args[1]);

    if (channel < 0 || channel > 7) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid channel (%d)", channel));
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
    if (n_args == 2) {
        if (chan) {
            return chan;
        }
        return mp_const_none;
    }

    // If there was already a channel, then remove it from the list. Note that
    // the order we do things here is important so as to appear atomic to
    // the IRQ handler.
    if (chan) {
        // Turn off any IRQ associated with the channel.
        pyb_timer_channel_callback(chan, mp_const_none);

        // Unlink the channel from the list.
        if (prev_chan) {
            prev_chan->next = chan->next;
        }
        self->channel = chan->next;
        chan->next = NULL;
    }

    // Allocate and initialize a new channel
    mp_arg_val_t vals[PYB_TIMER_CHANNEL_NUM_ARGS];
    mp_arg_parse_all(n_args - 3, args + 3, kw_args, PYB_TIMER_CHANNEL_NUM_ARGS, pyb_timer_channel_args, vals);

    chan = m_new_obj(pyb_timer_channel_obj_t);
    memset(chan, 0, sizeof(*chan));
    chan->base.type = &pyb_timer_channel_type;
    chan->timer = self;
    chan->channel = channel;
    chan->mode = mp_obj_get_int(args[2]);
    chan->callback = vals[0].u_obj;

    mp_obj_t pin_obj = vals[1].u_obj;
    if (pin_obj != mp_const_none) {
        if (!MP_OBJ_IS_TYPE(pin_obj, &pin_type)) {
            mp_raise_ValueError("pin argument needs to be be a Pin type");
        }
        const pin_obj_t *pin = pin_obj;
        const pin_af_obj_t *af = pin_find_af(pin, AF_FN_FTM, self->tim_id);
        if (af == NULL) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %s doesn't have an af for TIM%d", qstr_str(pin->name), self->tim_id));
        }
        // pin.init(mode=AF_PP, af=idx)
        const mp_obj_t args[6] = {
            (mp_obj_t)&pin_init_obj,
            pin_obj,
            MP_OBJ_NEW_QSTR(MP_QSTR_mode),  MP_OBJ_NEW_SMALL_INT(GPIO_MODE_AF_PP),
            MP_OBJ_NEW_QSTR(MP_QSTR_af),    MP_OBJ_NEW_SMALL_INT(af->idx)
        };
        mp_call_method_n_kw(0, 2, args);
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
            FTM_OC_InitTypeDef oc_config;
            oc_config.OCMode = channel_mode_info[chan->mode].oc_mode;
            if (vals[3].u_obj != mp_const_none) {
                // pulse width ratio given
                uint32_t period = compute_period(self);
                oc_config.Pulse = compute_pwm_value_from_percent(period, vals[3].u_obj);
            } else {
                // use absolute pulse width value (defaults to 0 if nothing given)
                oc_config.Pulse = vals[2].u_int;
            }
            oc_config.OCPolarity = FTM_OCPOLARITY_HIGH;

            HAL_FTM_PWM_ConfigChannel(&self->ftm, &oc_config, channel);
            if (chan->callback == mp_const_none) {
                HAL_FTM_PWM_Start(&self->ftm, channel);
            } else {
                HAL_FTM_PWM_Start_IT(&self->ftm, channel);
            }
            break;
        }

        case CHANNEL_MODE_OC_TIMING:
        case CHANNEL_MODE_OC_ACTIVE:
        case CHANNEL_MODE_OC_INACTIVE:
        case CHANNEL_MODE_OC_TOGGLE: {
            FTM_OC_InitTypeDef oc_config;
            oc_config.OCMode       = channel_mode_info[chan->mode].oc_mode;
            oc_config.Pulse        = vals[4].u_int;
            oc_config.OCPolarity   = vals[5].u_int;
            if (oc_config.OCPolarity == 0xffffffff) {
                oc_config.OCPolarity = FTM_OCPOLARITY_HIGH;
            }

            if (!IS_FTM_OC_POLARITY(oc_config.OCPolarity)) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid polarity (%d)", oc_config.OCPolarity));
            }
            HAL_FTM_OC_ConfigChannel(&self->ftm, &oc_config, channel);
            if (chan->callback == mp_const_none) {
                HAL_FTM_OC_Start(&self->ftm, channel);
            } else {
                HAL_FTM_OC_Start_IT(&self->ftm, channel);
            }
            break;
        }

        case CHANNEL_MODE_IC: {
            FTM_IC_InitTypeDef ic_config;

            ic_config.ICPolarity  = vals[5].u_int;
            if (ic_config.ICPolarity == 0xffffffff) {
                ic_config.ICPolarity = FTM_ICPOLARITY_RISING;
            }

            if (!IS_FTM_IC_POLARITY(ic_config.ICPolarity)) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid polarity (%d)", ic_config.ICPolarity));
            }
            HAL_FTM_IC_ConfigChannel(&self->ftm, &ic_config, chan->channel);
            if (chan->callback == mp_const_none) {
                HAL_FTM_IC_Start(&self->ftm, channel);
            } else {
                HAL_FTM_IC_Start_IT(&self->ftm, channel);
            }
            break;
        }

        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid mode (%d)", chan->mode));
    }

    return chan;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_timer_channel_obj, 2, pyb_timer_channel);

/// \method counter([value])
/// Get or set the timer counter.
STATIC mp_obj_t pyb_timer_counter(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->ftm.Instance->CNT);
    }
    // set - In order to write to CNT we need to set CNTIN
    self->ftm.Instance->CNTIN = mp_obj_get_int(args[1]);
    self->ftm.Instance->CNT = 0;    // write any value to load CNTIN into CNT
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_counter_obj, 1, 2, pyb_timer_counter);

/// \method prescaler([value])
/// Get or set the prescaler for the timer.
STATIC mp_obj_t pyb_timer_prescaler(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get
        return mp_obj_new_int(1 << (self->ftm.Instance->SC & 7));
    }

    // set
    mp_uint_t prescaler_shift = get_prescaler_shift(mp_obj_get_int(args[1]));

    mp_uint_t sc = self->ftm.Instance->SC;
    sc &= ~7;
    sc |= FTM_SC_PS(prescaler_shift);
    self->ftm.Instance->SC = sc;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_prescaler_obj, 1, 2, pyb_timer_prescaler);

/// \method period([value])
/// Get or set the period of the timer.
STATIC mp_obj_t pyb_timer_period(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    if (n_args == 1) {
        // get
        return mp_obj_new_int(self->ftm.Instance->MOD & 0xffff);
    }

    // set
    mp_int_t period = mp_obj_get_int(args[1]) & 0xffff;
    self->ftm.Instance->CNT = 0;
    self->ftm.Instance->MOD = period;
    return mp_const_none;
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
        __HAL_FTM_DISABLE_TOF_IT(&self->ftm);
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
        HAL_NVIC_EnableIRQ(self->irqn);
        // start timer, so that it interrupts on overflow
        HAL_FTM_Base_Start_IT(&self->ftm);
    } else {
        mp_raise_ValueError("callback must be None or a callable object");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_timer_callback_obj, pyb_timer_callback);

#if MICROPY_TIMER_REG
reg_t timer_reg[] = {
    REG_ENTRY(FTM_TypeDef, SC),
    REG_ENTRY(FTM_TypeDef, CNT),
    REG_ENTRY(FTM_TypeDef, MOD),
    REG_ENTRY(FTM_TypeDef, CNTIN),
    REG_ENTRY(FTM_TypeDef, STATUS),
    REG_ENTRY(FTM_TypeDef, MODE),
    REG_ENTRY(FTM_TypeDef, SYNC),
    REG_ENTRY(FTM_TypeDef, OUTINIT),
    REG_ENTRY(FTM_TypeDef, OUTMASK),
    REG_ENTRY(FTM_TypeDef, COMBINE),
    REG_ENTRY(FTM_TypeDef, DEADTIME),
    REG_ENTRY(FTM_TypeDef, EXTTRIG),
    REG_ENTRY(FTM_TypeDef, POL),
    REG_ENTRY(FTM_TypeDef, FMS),
    REG_ENTRY(FTM_TypeDef, FILTER),
    REG_ENTRY(FTM_TypeDef, FLTCTRL),
    REG_ENTRY(FTM_TypeDef, QDCTRL),
    REG_ENTRY(FTM_TypeDef, CONF),
    REG_ENTRY(FTM_TypeDef, FLTPOL),
    REG_ENTRY(FTM_TypeDef, SYNCONF),
    REG_ENTRY(FTM_TypeDef, INVCTRL),
    REG_ENTRY(FTM_TypeDef, SWOCTRL),
    REG_ENTRY(FTM_TypeDef, PWMLOAD),
};

mp_obj_t pyb_timer_reg(uint n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = args[0];
    return reg_cmd(self->ftm.Instance, timer_reg, MP_ARRAY_SIZE(timer_reg), n_args - 1, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_reg_obj, 1, 3, pyb_timer_reg);
#endif // MICROPY_TIMER_REG

STATIC const mp_map_elem_t pyb_timer_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), (mp_obj_t)&pyb_timer_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), (mp_obj_t)&pyb_timer_deinit_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_channel), (mp_obj_t)&pyb_timer_channel_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_counter), (mp_obj_t)&pyb_timer_counter_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_prescaler), (mp_obj_t)&pyb_timer_prescaler_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_period), (mp_obj_t)&pyb_timer_period_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&pyb_timer_callback_obj },
#if MICROPY_TIMER_REG
    { MP_OBJ_NEW_QSTR(MP_QSTR_reg), (mp_obj_t)&pyb_timer_reg_obj },
#endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_UP),              MP_OBJ_NEW_SMALL_INT(FTM_COUNTERMODE_UP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CENTER),          MP_OBJ_NEW_SMALL_INT(FTM_COUNTERMODE_CENTER) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PWM),             MP_OBJ_NEW_SMALL_INT(CHANNEL_MODE_PWM_NORMAL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PWM_INVERTED),    MP_OBJ_NEW_SMALL_INT(CHANNEL_MODE_PWM_INVERTED) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OC_TIMING),       MP_OBJ_NEW_SMALL_INT(CHANNEL_MODE_OC_TIMING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OC_ACTIVE),       MP_OBJ_NEW_SMALL_INT(CHANNEL_MODE_OC_ACTIVE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OC_INACTIVE),     MP_OBJ_NEW_SMALL_INT(CHANNEL_MODE_OC_INACTIVE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OC_TOGGLE),       MP_OBJ_NEW_SMALL_INT(CHANNEL_MODE_OC_TOGGLE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IC),              MP_OBJ_NEW_SMALL_INT(CHANNEL_MODE_IC) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_HIGH),            MP_OBJ_NEW_SMALL_INT(FTM_OCPOLARITY_HIGH) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_LOW),             MP_OBJ_NEW_SMALL_INT(FTM_OCPOLARITY_LOW) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_RISING),          MP_OBJ_NEW_SMALL_INT(FTM_ICPOLARITY_RISING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_FALLING),         MP_OBJ_NEW_SMALL_INT(FTM_ICPOLARITY_FALLING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BOTH),            MP_OBJ_NEW_SMALL_INT(FTM_ICPOLARITY_BOTH) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_timer_locals_dict, pyb_timer_locals_dict_table);

const mp_obj_type_t pyb_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = pyb_timer_print,
    .make_new = pyb_timer_make_new,
    .locals_dict = (mp_obj_t)&pyb_timer_locals_dict,
};

/// \moduleref pyb
/// \class TimerChannel - setup a channel for a timer.
///
/// Timer channels are used to generate/capture a signal using a timer.
///
/// TimerChannel objects are created using the Timer.channel() method.
STATIC void pyb_timer_channel_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_channel_obj_t *self = self_in;

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
STATIC mp_obj_t pyb_timer_channel_capture_compare(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_channel_obj_t *self = args[0];
    FTM_TypeDef *FTMx = self->timer->ftm.Instance;
    if (n_args == 1) {
        // get
        return mp_obj_new_int(FTMx->channel[self->channel].CV & 0xffff);
    }

    mp_int_t pw = mp_obj_get_int(args[1]);

    // set
    FTMx->channel[self->channel].CV = pw & 0xffff;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_channel_capture_compare_obj, 1, 2, pyb_timer_channel_capture_compare);

/// \method pulse_width_percent([value])
/// Get or set the pulse width percentage associated with a channel.  The value
/// is a number between 0 and 100 and sets the percentage of the timer period
/// for which the pulse is active.  The value can be an integer or
/// floating-point number for more accuracy.  For example, a value of 25 gives
/// a duty cycle of 25%.
STATIC mp_obj_t pyb_timer_channel_pulse_width_percent(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_timer_channel_obj_t *self = args[0];
    FTM_TypeDef *FTMx = self->timer->ftm.Instance;
    uint32_t period = compute_period(self->timer);
    if (n_args == 1) {
        // get
        uint32_t cmp = FTMx->channel[self->channel].CV & 0xffff;
        return compute_percent_from_pwm_value(period, cmp);
    } else {
        // set
        uint32_t cmp = compute_pwm_value_from_percent(period, args[1]);
        FTMx->channel[self->channel].CV = cmp & 0xffff;
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_channel_pulse_width_percent_obj, 1, 2, pyb_timer_channel_pulse_width_percent);

/// \method callback(fun)
/// Set the function to be called when the timer channel triggers.
/// `fun` is passed 1 argument, the timer object.
/// If `fun` is `None` then the callback will be disabled.
STATIC mp_obj_t pyb_timer_channel_callback(mp_obj_t self_in, mp_obj_t callback) {
    pyb_timer_channel_obj_t *self = self_in;
    if (callback == mp_const_none) {
        // stop interrupt (but not timer)
        __HAL_FTM_DISABLE_CH_IT(&self->timer->ftm, self->channel);
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
        HAL_NVIC_EnableIRQ(self->timer->irqn);
        // start timer, so that it interrupts on overflow
        switch (self->mode) {
            case CHANNEL_MODE_PWM_NORMAL:
            case CHANNEL_MODE_PWM_INVERTED:
                HAL_FTM_PWM_Start_IT(&self->timer->ftm, self->channel);
                break;
            case CHANNEL_MODE_OC_TIMING:
            case CHANNEL_MODE_OC_ACTIVE:
            case CHANNEL_MODE_OC_INACTIVE:
            case CHANNEL_MODE_OC_TOGGLE:
                HAL_FTM_OC_Start_IT(&self->timer->ftm, self->channel);
                break;
            case CHANNEL_MODE_IC:
                HAL_FTM_IC_Start_IT(&self->timer->ftm, self->channel);
                break;
        }
    } else {
        mp_raise_ValueError("callback must be None or a callable object");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_timer_channel_callback_obj, pyb_timer_channel_callback);

#if MICROPY_TIMER_REG
reg_t timer_channel_reg[] = {
    REG_ENTRY(FTM_ChannelTypeDef, CSC),
    REG_ENTRY(FTM_ChannelTypeDef, CV),
};

mp_obj_t pyb_timer_channel_reg(uint n_args, const mp_obj_t *args) {
    pyb_timer_channel_obj_t *self = args[0];
    return reg_cmd(&self->timer->ftm.Instance->channel[self->channel],
                   timer_channel_reg, MP_ARRAY_SIZE(timer_channel_reg),
                   n_args - 1, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_channel_reg_obj, 1, 3, pyb_timer_channel_reg);
#endif

STATIC const mp_map_elem_t pyb_timer_channel_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&pyb_timer_channel_callback_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pulse_width), (mp_obj_t)&pyb_timer_channel_capture_compare_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pulse_width_percent), (mp_obj_t)&pyb_timer_channel_pulse_width_percent_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_capture), (mp_obj_t)&pyb_timer_channel_capture_compare_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_compare), (mp_obj_t)&pyb_timer_channel_capture_compare_obj },
#if MICROPY_TIMER_REG
    { MP_OBJ_NEW_QSTR(MP_QSTR_reg), (mp_obj_t)&pyb_timer_channel_reg_obj },
#endif
};
STATIC MP_DEFINE_CONST_DICT(pyb_timer_channel_locals_dict, pyb_timer_channel_locals_dict_table);

STATIC const mp_obj_type_t pyb_timer_channel_type = {
    { &mp_type_type },
    .name = MP_QSTR_TimerChannel,
    .print = pyb_timer_channel_print,
    .locals_dict = (mp_obj_t)&pyb_timer_channel_locals_dict,
};

STATIC bool ftm_handle_irq_callback(pyb_timer_obj_t *self, mp_uint_t channel, mp_obj_t callback) {
    // execute callback if it's set
    if (callback == mp_const_none) {
        return false;
    }
    bool handled = false;

    // When executing code within a handler we must lock the GC to prevent
    // any memory allocations.  We must also catch any exceptions.
    gc_lock();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_call_function_1(callback, self);
        nlr_pop();
        handled = true;
    } else {
        // Uncaught exception; disable the callback so it doesn't run again.
        self->callback = mp_const_none;
        if (channel == 0xffffffff) {
            printf("Uncaught exception in Timer(" UINT_FMT
                   ") interrupt handler\n", self->tim_id);
        } else {
            printf("Uncaught exception in Timer(" UINT_FMT ") channel "
                   UINT_FMT " interrupt handler\n", self->tim_id, channel);
        }
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
    gc_unlock();
    return handled;
}

STATIC void ftm_irq_handler(uint tim_id) {
    if (tim_id >= PYB_TIMER_OBJ_ALL_NUM) {
        return;
    }
    // get the timer object
    pyb_timer_obj_t *self = pyb_timer_obj_all[tim_id];
    if (self == NULL) {
        // timer object has not been set, so we can't do anything
        printf("No timer object for id=%d\n", tim_id);
        return;
    }
    FTM_HandleTypeDef *hftm = &self->ftm;

    bool handled = false;

    // Check for timer (versus timer channel) interrupt.
    if (__HAL_FTM_GET_TOF_IT(hftm) && __HAL_FTM_GET_TOF_FLAG(hftm)) {
        __HAL_FTM_CLEAR_TOF_FLAG(hftm);
        if (ftm_handle_irq_callback(self, 0xffffffff, self->callback)) {
            handled = true;
        } else {
            __HAL_FTM_DISABLE_TOF_IT(&self->ftm);
            printf("No callback for Timer %d TOF (now disabled)\n", tim_id);
        }
    }

    uint32_t processed = 0;

    // Check to see if a timer channel interrupt is pending
    pyb_timer_channel_obj_t *chan = self->channel;
    while (chan != NULL) {
        processed |= (1 << chan->channel);
        if (__HAL_FTM_GET_CH_IT(&self->ftm, chan->channel) && __HAL_FTM_GET_CH_FLAG(&self->ftm, chan->channel)) {
            __HAL_FTM_CLEAR_CH_FLAG(&self->ftm, chan->channel);
            if (ftm_handle_irq_callback(self, chan->channel, chan->callback)) {
                handled = true;
            } else {
                __HAL_FTM_DISABLE_CH_IT(&self->ftm, chan->channel);
                printf("No callback for Timer %d channel %u (now disabled)\n",
                       self->tim_id, chan->channel);
            }
        }
        chan = chan->next;
    }

    if (!handled) {
        // An interrupt occurred for a channel we didn't process. Find it and
        // turn it off.
        for (mp_uint_t channel = 0; channel < 8; channel++) {
            if ((processed & (1 << channel)) == 0) {
                if (__HAL_FTM_GET_CH_FLAG(&self->ftm, channel) != 0) {
                    __HAL_FTM_CLEAR_CH_FLAG(&self->ftm, channel);
                    __HAL_FTM_DISABLE_CH_IT(&self->ftm, channel);
                    printf("Unhandled interrupt Timer %d channel %u (now disabled)\n",
                           tim_id, channel);
                }
            }
        }
    }
}

void ftm0_isr(void) {
    ftm_irq_handler(0);
}

void ftm1_isr(void) {
    ftm_irq_handler(1);
}

void ftm2_isr(void) {
    ftm_irq_handler(2);
}
