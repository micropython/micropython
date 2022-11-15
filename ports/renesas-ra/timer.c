/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021 Renesas Electronics Corporation
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
#include "pin.h"
#include "irq.h"

#define TIMER_SIZE  2

void timer_irq_handler(void *param);
STATIC mp_obj_t pyb_timer_freq(size_t n_args, const mp_obj_t *args);

#if defined(TIMER_CHANNEL)
typedef struct _pyb_timer_channel_obj_t {
    mp_obj_base_t base;
    struct _pyb_timer_obj_t *timer;
    uint8_t channel;
    mp_obj_t callback;
    struct _pyb_timer_channel_obj_t *next;
} pyb_timer_channel_obj_t;
#endif

typedef struct _pyb_timer_obj_t {
    mp_obj_base_t base;
    uint8_t tim_id;
    mp_obj_t callback;
    #if defined(TIMER_CHANNEL)
    pyb_timer_channel_obj_t *channel;
    #endif
} pyb_timer_obj_t;
#define PYB_TIMER_OBJ_ALL_NUM MP_ARRAY_SIZE(MP_STATE_PORT(pyb_timer_obj_all))

STATIC mp_obj_t pyb_timer_deinit(mp_obj_t self_in);
STATIC mp_obj_t pyb_timer_callback(mp_obj_t self_in, mp_obj_t callback);
#if defined(TIMER_CHANNEL)
STATIC mp_obj_t pyb_timer_channel_callback(mp_obj_t self_in, mp_obj_t callback);
#endif
static const int ra_agt_timer_ch[TIMER_SIZE] = {1, 2};

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

/*
 * Timer Class
 */

STATIC void pyb_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "Timer(%u)", self->tim_id);
}

/// \method init(*, freq, prescaler, period)
/// Initialise the timer.  Initialisation must be either by frequency (in Hz)
/// or by prescaler and period:
///
///     tim.init(freq=100)                  # set the timer to trigger at 100Hz
///
/// Keyword arguments:
///
///   - `freq` - specifies the periodic frequency of the timer. You migh also
///              view this as the frequency with which the timer goes through
///              one complete cycle.
//////
///   - `callback` - as per Timer.callback()
//////
///  You must either specify freq.
STATIC mp_obj_t pyb_timer_init_helper(pyb_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // enum { ARG_freq, ARG_prescaler, ARG_period, ARG_tick_hz, ARG_mode, ARG_div, ARG_callback, ARG_deadtime };
    enum { ARG_freq, ARG_callback };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // init TIM
    for (int i = 1; i <= TIMER_SIZE; i++) {
        ra_agt_timer_set_callback(i - 1, (AGT_TIMER_CB)timer_irq_handler, (void *)&ra_agt_timer_ch[i - 1]);
    }
    ra_agt_timer_init(self->tim_id - 1, 1.0f);
    if (args[ARG_freq].u_obj != mp_const_none) {
        mp_obj_t freq_args[2];
        freq_args[0] = self;
        freq_args[1] = args[ARG_freq].u_obj;
        pyb_timer_freq(2, (const mp_obj_t *)&freq_args);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("must specify either freq, period, or prescaler and period"));
    }
    // Enable ARPE so that the auto-reload register is buffered.
    // This allows to smoothly change the frequency of the timer.
    // Start the timer running
    if (args[ARG_callback].u_obj == mp_const_none) {
        // do nothing
    } else {
        pyb_timer_callback(MP_OBJ_FROM_PTR(self), args[ARG_callback].u_obj);
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

    // get the timer id
    mp_int_t tim_id = mp_obj_get_int(args[0]);
    // create new Timer object
    pyb_timer_obj_t *tim;
    if (MP_STATE_PORT(pyb_timer_obj_all)[tim_id - 1] == NULL) {
        // create new Timer object
        tim = m_new_obj(pyb_timer_obj_t);
        memset(tim, 0, sizeof(*tim));
        tim->base.type = &pyb_timer_type;
        tim->tim_id = tim_id;
        tim->callback = mp_const_none;
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

/// \method deinit()
/// Deinitialises the timer.
///
/// Disables the callback (and the associated irq).
/// Disables any channel callbacks (and the associated irq).
/// Stops the timer, and disables the timer peripheral.
STATIC mp_obj_t pyb_timer_deinit(mp_obj_t self_in) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Disable the base interrupt
    pyb_timer_callback(self_in, mp_const_none);

    #if defined(TIMER_CHANNEL)
    pyb_timer_channel_obj_t *chan = self->channel;
    self->channel = NULL;

    // Disable the channel interrupts
    while (chan != NULL) {
        pyb_timer_channel_callback(MP_OBJ_FROM_PTR(chan), mp_const_none);
        pyb_timer_channel_obj_t *prev_chan = chan;
        chan = chan->next;
        prev_chan->next = NULL;
    }
    #endif

    ra_agt_timer_deinit(self->tim_id - 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_timer_deinit_obj, pyb_timer_deinit);

#if defined(TIMER_CHANNEL)
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
STATIC mp_obj_t pyb_timer_channel(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_callback,            MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_compare,             MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
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
    chan->callback = args[1].u_obj;

    mp_obj_t pin_obj = args[2].u_obj;
    if (pin_obj != mp_const_none) {
        // ToDo
    }
    // Link the channel to the timer before we turn the channel on.
    // Note that this needs to appear atomic to the IRQ handler (the write
    // to self->channel is atomic, so we're good, but I thought I'd mention
    // in case this was ever changed in the future).
    chan->next = self->channel;
    self->channel = chan;

    // ToDo
    return MP_OBJ_FROM_PTR(chan);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_timer_channel_obj, 2, pyb_timer_channel);
#endif

#if TIMER_COUNTER
// Not implemented
/// \method counter([value])
/// Get or set the timer counter.
STATIC mp_obj_t pyb_timer_counter(size_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        return mp_obj_new_int((int)ra_agt_timer_get_counter(self->tim_id));
    } else {
        // set
        ra_agt_timer_set_counter((unsigned int)self->tim_id, (unsigned long)mp_obj_get_int(args[1]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_counter_obj, 1, 2, pyb_timer_counter);
#endif

/// \method freq([value])
/// Get or set the frequency for the timer (changes prescaler and period if set).
STATIC mp_obj_t pyb_timer_freq(size_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int ch = self->tim_id - 1;
    if (n_args == 1) {
        // get
        #if MICROPY_PY_BUILTINS_FLOAT
        float freq = ra_agt_timer_get_freq(ch);
        return mp_obj_new_float(freq);
        #else
        uint32_t freq = (uint32_t)ra_agt_timer_get_freq(ch);
        return mp_obj_new_int(freq);
        #endif
    } else {
        // set
        uint32_t freq;
        if (0) {
        #if MICROPY_PY_BUILTINS_FLOAT
        } else if (mp_obj_is_type(args[1], &mp_type_float)) {
            freq = (int)mp_obj_get_float(args[1]);
        #endif
        } else {
            freq = mp_obj_get_int(args[1]);
        }
        if (freq == 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("freq must not be 0"));
        }
        ra_agt_timer_stop(ch);
        ra_agt_timer_set_freq(ch, (float)freq);
        ra_agt_timer_start(ch);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_freq_obj, 1, 2, pyb_timer_freq);

#if TIMER_PERIOD
// Not implemented
/// \method period([value])
/// Get or set the period of the timer.
STATIC mp_obj_t pyb_timer_period(size_t n_args, const mp_obj_t *args) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // get
        return mp_obj_new_int((int)ra_agt_timer_get_period(self->tim_id));
        return mp_const_none;
    } else {
        // set
        ra_agt_timer_set_period((uint32_t)self->tim_id, (uint16_t)mp_obj_get_int(args[1]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_period_obj, 1, 2, pyb_timer_period);
#endif

/// \method callback(fun)
/// Set the function to be called when the timer triggers.
/// `fun` is passed 1 argument, the timer object.
/// If `fun` is `None` then the callback will be disabled.
STATIC mp_obj_t pyb_timer_callback(mp_obj_t self_in, mp_obj_t callback) {
    pyb_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (callback == mp_const_none) {
        // stop interrupt (but not timer)
        // __HAL_TIM_DISABLE_IT(&self->tim, TIM_IT_UPDATE);
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        // __HAL_TIM_DISABLE_IT(&self->tim, TIM_IT_UPDATE);
        self->callback = callback;
        // start timer, so that it interrupts on overflow, but clear any
        // pending interrupts which may have been set by initializing it.
        // __HAL_TIM_CLEAR_FLAG(&self->tim, TIM_IT_UPDATE);
        // HAL_TIM_Base_Start_IT(&self->tim); // This will re-enable the IRQ
        // HAL_NVIC_EnableIRQ(self->irqn);
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
    #if TIMER_COUNTER
    { MP_ROM_QSTR(MP_QSTR_counter), MP_ROM_PTR(&pyb_timer_counter_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&pyb_timer_freq_obj) },
    #if TIMER_PERIOD
    { MP_ROM_QSTR(MP_QSTR_period), MP_ROM_PTR(&pyb_timer_period_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&pyb_timer_callback_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_timer_locals_dict, pyb_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_timer_make_new,
    locals_dict, &pyb_timer_locals_dict,
    print, pyb_timer_print
    );

#if defined(TIMER_CHANNEL)
/*
 * Timer Channel
 */

/// \moduleref pyb
/// \class TimerChannel - setup a channel for a timer.
///
/// Timer channels are used to generate/capture a signal using a timer.
///
/// TimerChannel objects are created using the Timer.channel() method.
STATIC void pyb_timer_channel_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_timer_channel_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "TimerChannel(timer=%u, channel=%u",
        self->timer->tim_id,
        self->channel);
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
        return mp_const_none;
    } else {
        // set
        // __HAL_TIM_SET_COMPARE(&self->timer->tim, TIMER_CHANNEL(self), mp_obj_get_int(args[1]) & TIMER_CNT_MASK(self->timer));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_timer_channel_capture_compare_obj, 1, 2, pyb_timer_channel_capture_compare);

/// \method callback(fun)
/// Set the function to be called when the timer channel triggers.
/// `fun` is passed 1 argument, the timer object.
/// If `fun` is `None` then the callback will be disabled.
STATIC mp_obj_t pyb_timer_channel_callback(mp_obj_t self_in, mp_obj_t callback) {
    pyb_timer_channel_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (callback == mp_const_none) {
        // stop interrupt (but not timer)
        // _HAL_TIM_DISABLE_IT(&self->timer->tim, TIMER_IRQ_MASK(self->channel));
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
        uint8_t tim_id = self->timer->tim_id;
        // __HAL_TIM_CLEAR_IT(&self->timer->tim, TIMER_IRQ_MASK(self->channel));
        if (tim_id == 1) {
            // HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
        } else {
            // HAL_NVIC_EnableIRQ(self->timer->irqn);
        }
        // start timer, so that it interrupts on overflow
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("callback must be None or a callable object"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_timer_channel_callback_obj, pyb_timer_channel_callback);

STATIC const mp_rom_map_elem_t pyb_timer_channel_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&pyb_timer_channel_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_capture), MP_ROM_PTR(&pyb_timer_channel_capture_compare_obj) },
    { MP_ROM_QSTR(MP_QSTR_compare), MP_ROM_PTR(&pyb_timer_channel_capture_compare_obj) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_timer_channel_locals_dict, pyb_timer_channel_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    pyb_timer_channel_type,
    MP_QSTR_TimerChannel,
    MP_TYPE_FLAG_NONE,
    locals_dict, &pyb_timer_channel_locals_dict,
    print, pyb_timer_channel_print
    );
#endif

STATIC void timer_handle_irq_channel(pyb_timer_obj_t *tim, uint8_t channel, mp_obj_t callback) {

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
            // __HAL_TIM_DISABLE_IT(&tim->tim, irq_mask);
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

void timer_irq_handler(void *param) {
    uint tim_id = *(uint *)param;
    if ((tim_id != 0) && (tim_id - 1 < PYB_TIMER_OBJ_ALL_NUM)) {
        // get the timer object
        pyb_timer_obj_t *tim = MP_STATE_PORT(pyb_timer_obj_all)[tim_id - 1];

        if (tim == NULL) {
            // do nohting
            return;
        }
        timer_handle_irq_channel(tim, 0, tim->callback);
        // Check to see if a timer channel interrupt was pending
        #if defined(TIMER_CHANNEL)
        pyb_timer_channel_obj_t *chan = tim->channel;
        while (chan != NULL) {
            timer_handle_irq_channel(tim, chan->channel, chan->callback);
            // handled |= TIMER_IRQ_MASK(chan->channel);
            chan = chan->next;
        }
        #endif
        // ToDo
        // Finally, clear any remaining interrupt sources. Otherwise we'll
        // just get called continuously.
        // uint32_t unhandled = 0;
        // if (unhandled != 0) {
        //    printf("Unhandled interrupt SR=0x%02x (now disabled)\n", (unsigned int)unhandled);
        // }
    }
}

MP_REGISTER_ROOT_POINTER(struct _pyb_timer_obj_t *pyb_timer_obj_all[MICROPY_HW_MAX_TIMER]);
