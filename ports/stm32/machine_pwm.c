/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
 * Copyright (c) 2026 Damien P. George
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_pwm.c via MICROPY_PY_MACHINE_PWM_INCLUDEFILE.

#include "py/mphal.h"
#include "timer.h"

// tim is a TIMx instance, ch is one of PWM_CHx
#define CCRx(tim, ch) ((&(tim)->CCR1)[(ch)])

enum {
    #if defined(TIM1)
    TIM1_ENABLED,
    #endif
    #if defined(TIM2)
    TIM2_ENABLED,
    #endif
    #if defined(TIM3)
    TIM3_ENABLED,
    #endif
    #if defined(TIM4)
    TIM4_ENABLED,
    #endif
    #if defined(TIM5)
    TIM5_ENABLED,
    #endif
    #if defined(TIM6)
    TIM6_ENABLED,
    #endif
    #if defined(TIM7)
    TIM7_ENABLED,
    #endif
    #if defined(TIM8)
    TIM8_ENABLED,
    #endif
    #if defined(TIM9)
    TIM9_ENABLED,
    #endif
    #if defined(TIM10)
    TIM10_ENABLED,
    #endif
    #if defined(TIM11)
    TIM11_ENABLED,
    #endif
    #if defined(TIM12)
    TIM12_ENABLED,
    #endif
    #if defined(TIM13)
    TIM13_ENABLED,
    #endif
    #if defined(TIM14)
    TIM14_ENABLED,
    #endif
    #if defined(TIM15)
    TIM15_ENABLED,
    #endif
    #if defined(TIM16)
    TIM16_ENABLED,
    #endif
    #if defined(TIM17)
    TIM17_ENABLED,
    #endif
    #if defined(TIM18)
    TIM18_ENABLED,
    #endif
    #if defined(TIM19)
    TIM19_ENABLED,
    #endif
    #if defined(TIM20)
    TIM20_ENABLED,
    #endif
    #if defined(TIM21)
    TIM21_ENABLED,
    #endif
    #if defined(TIM22)
    TIM22_ENABLED,
    #endif
    NUM_TIMERS,
};

enum {
    PWM_CH1 = 0,
    PWM_CH2 = 1,
    PWM_CH3 = 2,
    PWM_CH4 = 3,
    NUM_CHANNELS_PER_TIMER,
};

enum {
    PWM_POLARITY_NORMAL,
    PWM_POLARITY_INVERTED,
};

typedef struct _pwm_t {
    uint8_t tim_id; // TIMx id
    uint8_t channel; // one of PWM_CH1-PWM_CH4
} pwm_t;

void pwm_init(pwm_t *pwm) {
    // The following code assumes each channel has 8 bits in CCMR1/2.
    MP_STATIC_ASSERT(TIM_CCMR1_CC1S_Pos + 8 == TIM_CCMR1_CC2S_Pos);
    MP_STATIC_ASSERT(TIM_CCMR2_CC3S_Pos + 8 == TIM_CCMR2_CC4S_Pos);

    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);

    // Initialise TIM if it's not already running.
    if (!(tim->CR1 & TIM_CR1_CEN)) {
        timer_clock_enable(pwm->tim_id);

        // Initialise with: clock division 1, up counter mode, auto-reload buffered.
        // ARPE allows to smoothly change the frequency of the timer, and prevents
        // long silent periods when a 32-bit timer is used and CNT goes beyond ARR.
        tim->CR1 = TIM_CR1_ARPE;

        #if defined(IS_TIM_REPETITION_COUNTER_INSTANCE)
        if (IS_TIM_REPETITION_COUNTER_INSTANCE(tim)) {
            tim->RCR = 0;
        }
        #endif
    }

    // Configure PWM mode.
    uint32_t reg = 6 << TIM_CCMR1_OC1M_Pos // PWM1 mode
        | 1 << TIM_CCMR1_OC1PE_Pos // preload enabled
        | 0 << TIM_CCMR1_CC1S_Pos // output mode
    ;
    uint32_t shift = 8 * (pwm->channel & 1);
    if (pwm->channel == PWM_CH1 || pwm->channel == PWM_CH2) {
        tim->CCMR1 = (tim->CCMR1 & ~(0xff << shift)) | reg << shift;
    } else {
        tim->CCMR2 = (tim->CCMR1 & ~(0xff << shift)) | reg << shift;
    }

    #if defined(IS_TIM_BREAK_INSTANCE)
    // Enable master output if needed.
    if (IS_TIM_BREAK_INSTANCE(tim)) {
        tim->BDTR |= TIM_BDTR_MOE;
    }
    #endif
}

unsigned int pwm_get_polarity(pwm_t *pwm) {
    // The following code assumes each channel has 4 bits in CCER.
    MP_STATIC_ASSERT(TIM_CCER_CC1P_Pos + 4 == TIM_CCER_CC2P_Pos);

    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);
    unsigned int shift = 4 * pwm->channel;
    return ((tim->CCER >> shift) >> TIM_CCER_CC1P_Pos) & 1;
}

void pwm_enable_output(pwm_t *pwm, unsigned int polarity) {
    // The following code assumes each channel has 4 bits in CCER.
    MP_STATIC_ASSERT(TIM_CCER_CC1E_Pos + 4 == TIM_CCER_CC2E_Pos);

    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);

    // Enable output on pin, active high for normal channel, active low for inverted channel,
    // so that both normal and inverted channels can be used for PWM in the same way.
    uint32_t reg = TIM_CCER_CC1E;
    if (polarity == PWM_POLARITY_INVERTED) {
        reg |= TIM_CCER_CC1P;
    }
    unsigned int shift = 4 * pwm->channel;
    tim->CCER = (tim->CCER & ~(0xf << shift)) | reg << shift;
}

void pwm_deinit(pwm_t *pwm) {
    // The following code assumes each channel has 4 bits in CCER.
    MP_STATIC_ASSERT(TIM_CCER_CC1E_Pos + 4 == TIM_CCER_CC2E_Pos);

    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);

    // Disable the normal outputs.
    tim->CCER &= ~(TIM_CCER_CC1E << (4 * pwm->channel));

    // Disable the TIM peripheral if all channels are disabled.
    uint32_t ccer_out_mask = TIM_CCER_CCxE_MASK;
    #if defined(TIM_CCER_CCxNE_MASK)
    ccer_out_mask |= TIM_CCER_CCxNE_MASK;
    #endif
    if ((tim->CCER & ccer_out_mask) == 0) {
        #if defined(IS_TIM_BREAK_INSTANCE)
        if (IS_TIM_BREAK_INSTANCE(tim)) {
            tim->BDTR &= ~TIM_BDTR_MOE;
        }
        #endif
        tim->CR1 &= ~TIM_CR1_CEN;
    }
}

bool pwm_freq_is_valid(pwm_t *pwm) {
    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);
    return tim->ARR != 0;
}

// The ARR needs to be configured (eg `pwm_set_freq()`) before this function is called.
void pwm_start(pwm_t *pwm) {
    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);
    if (!(tim->CR1 & TIM_CR1_CEN)) {
        // Reinitialise the counter and update the registers.
        tim->EGR = TIM_EGR_UG;

        // Enable the timer if it's not already running.
        tim->CR1 |= TIM_CR1_CEN;
    }
}

uint32_t pwm_get_freq(pwm_t *pwm) {
    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);
    uint32_t prescaler = tim->PSC;
    uint32_t period = tim->ARR;
    uint32_t freq = timer_get_source_freq(pwm->tim_id) / ((prescaler + 1) * (period + 1));
    return freq;
}

// Input freq_hz must be > 0.
// Returns false if freq_hz is too large.
bool pwm_set_freq(pwm_t *pwm, uint32_t freq_hz) {
    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);

    uint32_t source_freq = timer_get_source_freq(pwm->tim_id);

    if (freq_hz > source_freq) {
        // Requested frequency too high.
        return false;
    }

    // Find optimal prescaler and period values for the given frequency:
    // - For 32-bit TIM, the prescaler is always 1 to get maximum precision.
    // - For 16-bit TIM, find the smallest prescaler with a period that fits 16-bits.
    uint32_t prescaler = 1;
    uint32_t period = (source_freq + freq_hz / 2) / freq_hz;
    if (!IS_TIM_32B_COUNTER_INSTANCE(tim)) {
        while (period > 0xffff) {
            // If we can divide exactly, do that first.
            if (period % 5 == 0) {
                prescaler *= 5;
                period /= 5;
            } else if (period % 3 == 0) {
                prescaler *= 3;
                period /= 3;
            } else {
                // May not divide exactly, but loses minimal precision.
                prescaler <<= 1;
                period >>= 1;
            }
        }
    }

    // Set the prescaler and period registers.
    tim->PSC = prescaler - 1;
    tim->ARR = period - 1;

    return true;
}

uint16_t pwm_get_duty_u16(pwm_t *pwm) {
    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);
    uint32_t top = tim->ARR + 1;
    uint32_t cc = CCRx(tim, pwm->channel);
    return (cc * 65535ULL + top / 2U) / top;
}

void pwm_set_duty_u16(pwm_t *pwm, uint32_t duty_u16) {
    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);
    uint32_t top = tim->ARR + 1;
    uint32_t cc = ((uint64_t)duty_u16 * top + 65535ULL / 2) / 65535U;
    if (cc > top) {
        cc = top;
    }
    CCRx(tim, pwm->channel) = cc;
}

uint32_t pwm_get_duty_ns(pwm_t *pwm) {
    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);
    uint32_t source_freq = timer_get_source_freq(pwm->tim_id);
    uint32_t cc = CCRx(tim, pwm->channel);
    return ((uint64_t)cc * 1000000000ULL + source_freq / 2U) / source_freq;
}

void pwm_set_duty_ns(pwm_t *pwm, uint32_t duty_ns) {
    TIM_TypeDef *tim = timer_id_to_reg(pwm->tim_id);
    uint32_t source_freq = timer_get_source_freq(pwm->tim_id);
    uint32_t top = tim->ARR + 1;
    uint32_t cc = ((uint64_t)duty_ns * source_freq + 1000000000ULL / 2U) / 1000000000ULL;
    if (cc > top) {
        cc = top;
    }
    CCRx(tim, pwm->channel) = cc;
}

/******************************************************************************/
// Pin helper

typedef struct _pwm_pin_config_t {
    uint8_t timer_id;
    uint8_t timer_channel;
    uint8_t alt;
} pwm_pin_config_t;

static bool pin_find_af_for_pwm(mp_hal_pin_obj_t pin, pwm_pin_config_t *cfg) {
    for (size_t i = 0; i < pin->num_af; ++i) {
        if (pin->af[i].fn == AF_FN_TIM && pin->af[i].type >= AF_PIN_TYPE_TIM_CH1 && pin->af[i].type <= AF_PIN_TYPE_TIM_CH4) {
            cfg->timer_id = pin->af[i].unit;
            cfg->timer_channel = pin->af[i].type - AF_PIN_TYPE_TIM_CH1;
            cfg->alt = pin->af[i].idx;
            return true;
        }
    }
    return false;
}

/******************************************************************************/
// MicroPython bindings for machine.PWM

enum {
    DUTY_NOT_SET = 0,
    DUTY_U16,
    DUTY_NS
};

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    pwm_t pwm;
} machine_pwm_obj_t;

typedef struct _machine_pwm_state_t {
    uint8_t duty_type;
    mp_int_t duty;
} machine_pwm_state_t;

static uint32_t timer_pwm_active = 0;

static const machine_pwm_obj_t machine_pwm_obj[NUM_TIMERS * NUM_CHANNELS_PER_TIMER] = {
    #if defined(TIM1)
    {{&machine_pwm_type}, {1, PWM_CH1}},
    {{&machine_pwm_type}, {1, PWM_CH2}},
    {{&machine_pwm_type}, {1, PWM_CH3}},
    {{&machine_pwm_type}, {1, PWM_CH4}},
    #endif
    #if defined(TIM2)
    {{&machine_pwm_type}, {2, PWM_CH1}},
    {{&machine_pwm_type}, {2, PWM_CH2}},
    {{&machine_pwm_type}, {2, PWM_CH3}},
    {{&machine_pwm_type}, {2, PWM_CH4}},
    #endif
    #if defined(TIM3)
    {{&machine_pwm_type}, {3, PWM_CH1}},
    {{&machine_pwm_type}, {3, PWM_CH2}},
    {{&machine_pwm_type}, {3, PWM_CH3}},
    {{&machine_pwm_type}, {3, PWM_CH4}},
    #endif
    #if defined(TIM4)
    {{&machine_pwm_type}, {4, PWM_CH1}},
    {{&machine_pwm_type}, {4, PWM_CH2}},
    {{&machine_pwm_type}, {4, PWM_CH3}},
    {{&machine_pwm_type}, {4, PWM_CH4}},
    #endif
    #if defined(TIM5)
    {{&machine_pwm_type}, {5, PWM_CH1}},
    {{&machine_pwm_type}, {5, PWM_CH2}},
    {{&machine_pwm_type}, {5, PWM_CH3}},
    {{&machine_pwm_type}, {5, PWM_CH4}},
    #endif
    #if defined(TIM6)
    {{&machine_pwm_type}, {6, PWM_CH1}},
    {{&machine_pwm_type}, {6, PWM_CH2}},
    {{&machine_pwm_type}, {6, PWM_CH3}},
    {{&machine_pwm_type}, {6, PWM_CH4}},
    #endif
    #if defined(TIM7)
    {{&machine_pwm_type}, {7, PWM_CH1}},
    {{&machine_pwm_type}, {7, PWM_CH2}},
    {{&machine_pwm_type}, {7, PWM_CH3}},
    {{&machine_pwm_type}, {7, PWM_CH4}},
    #endif
    #if defined(TIM8)
    {{&machine_pwm_type}, {8, PWM_CH1}},
    {{&machine_pwm_type}, {8, PWM_CH2}},
    {{&machine_pwm_type}, {8, PWM_CH3}},
    {{&machine_pwm_type}, {8, PWM_CH4}},
    #endif
    #if defined(TIM9)
    {{&machine_pwm_type}, {9, PWM_CH1}},
    {{&machine_pwm_type}, {9, PWM_CH2}},
    {{&machine_pwm_type}, {9, PWM_CH3}},
    {{&machine_pwm_type}, {9, PWM_CH4}},
    #endif
    #if defined(TIM10)
    {{&machine_pwm_type}, {10, PWM_CH1}},
    {{&machine_pwm_type}, {10, PWM_CH2}},
    {{&machine_pwm_type}, {10, PWM_CH3}},
    {{&machine_pwm_type}, {10, PWM_CH4}},
    #endif
    #if defined(TIM11)
    {{&machine_pwm_type}, {11, PWM_CH1}},
    {{&machine_pwm_type}, {11, PWM_CH2}},
    {{&machine_pwm_type}, {11, PWM_CH3}},
    {{&machine_pwm_type}, {11, PWM_CH4}},
    #endif
    #if defined(TIM12)
    {{&machine_pwm_type}, {12, PWM_CH1}},
    {{&machine_pwm_type}, {12, PWM_CH2}},
    {{&machine_pwm_type}, {12, PWM_CH3}},
    {{&machine_pwm_type}, {12, PWM_CH4}},
    #endif
    #if defined(TIM13)
    {{&machine_pwm_type}, {13, PWM_CH1}},
    {{&machine_pwm_type}, {13, PWM_CH2}},
    {{&machine_pwm_type}, {13, PWM_CH3}},
    {{&machine_pwm_type}, {13, PWM_CH4}},
    #endif
    #if defined(TIM14)
    {{&machine_pwm_type}, {14, PWM_CH1}},
    {{&machine_pwm_type}, {14, PWM_CH2}},
    {{&machine_pwm_type}, {14, PWM_CH3}},
    {{&machine_pwm_type}, {14, PWM_CH4}},
    #endif
    #if defined(TIM15)
    {{&machine_pwm_type}, {15, PWM_CH1}},
    {{&machine_pwm_type}, {15, PWM_CH2}},
    {{&machine_pwm_type}, {15, PWM_CH3}},
    {{&machine_pwm_type}, {15, PWM_CH4}},
    #endif
    #if defined(TIM16)
    {{&machine_pwm_type}, {16, PWM_CH1}},
    {{&machine_pwm_type}, {16, PWM_CH2}},
    {{&machine_pwm_type}, {16, PWM_CH3}},
    {{&machine_pwm_type}, {16, PWM_CH4}},
    #endif
    #if defined(TIM17)
    {{&machine_pwm_type}, {17, PWM_CH1}},
    {{&machine_pwm_type}, {17, PWM_CH2}},
    {{&machine_pwm_type}, {17, PWM_CH3}},
    {{&machine_pwm_type}, {17, PWM_CH4}},
    #endif
    #if defined(TIM18)
    {{&machine_pwm_type}, {18, PWM_CH1}},
    {{&machine_pwm_type}, {18, PWM_CH2}},
    {{&machine_pwm_type}, {18, PWM_CH3}},
    {{&machine_pwm_type}, {18, PWM_CH4}},
    #endif
    #if defined(TIM19)
    {{&machine_pwm_type}, {19, PWM_CH1}},
    {{&machine_pwm_type}, {19, PWM_CH2}},
    {{&machine_pwm_type}, {19, PWM_CH3}},
    {{&machine_pwm_type}, {19, PWM_CH4}},
    #endif
    #if defined(TIM20)
    {{&machine_pwm_type}, {20, PWM_CH1}},
    {{&machine_pwm_type}, {20, PWM_CH2}},
    {{&machine_pwm_type}, {20, PWM_CH3}},
    {{&machine_pwm_type}, {20, PWM_CH4}},
    #endif
    #if defined(TIM21)
    {{&machine_pwm_type}, {21, PWM_CH1}},
    {{&machine_pwm_type}, {21, PWM_CH2}},
    {{&machine_pwm_type}, {21, PWM_CH3}},
    {{&machine_pwm_type}, {21, PWM_CH4}},
    #endif
    #if defined(TIM22)
    {{&machine_pwm_type}, {22, PWM_CH1}},
    {{&machine_pwm_type}, {22, PWM_CH2}},
    {{&machine_pwm_type}, {22, PWM_CH3}},
    {{&machine_pwm_type}, {22, PWM_CH4}},
    #endif
};

static machine_pwm_state_t machine_pwm_state[NUM_TIMERS * NUM_CHANNELS_PER_TIMER];

static inline machine_pwm_state_t *get_state(machine_pwm_obj_t *pwm) {
    size_t pwm_index = pwm - &machine_pwm_obj[0];
    return &machine_pwm_state[pwm_index];
}

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<PWM TIM%u_CH%u invert=%u>",
        self->pwm.tim_id, self->pwm.channel,
        pwm_get_polarity(&self->pwm) == PWM_POLARITY_INVERTED);
}

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,     MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_invert].u_int != -1) {
        pwm_enable_output(&self->pwm, args[ARG_invert].u_int ? PWM_POLARITY_INVERTED : PWM_POLARITY_NORMAL);
    }
    if (args[ARG_freq].u_int != -1) {
        mp_machine_pwm_freq_set(self, args[ARG_freq].u_int);
    }
    if (args[ARG_duty_u16].u_int != -1) {
        mp_machine_pwm_duty_set_u16(self, args[ARG_duty_u16].u_int);
    }
    if (args[ARG_duty_ns].u_int != -1) {
        mp_machine_pwm_duty_set_ns(self, args[ARG_duty_ns].u_int);
    }
}

// PWM(pin [, args])
static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get pin to connect to PWM.
    mp_hal_pin_obj_t pin = mp_hal_get_pin_obj(all_args[0]);

    // Search the given pin's alternate functions for a TIMx_CHy function.
    machine_pwm_obj_t *self = NULL;
    pwm_pin_config_t cfg;
    if (pin_find_af_for_pwm(pin, &cfg)) {
        for (size_t i = 0; i < MP_ARRAY_SIZE(machine_pwm_obj); ++i) {
            if (machine_pwm_obj[i].pwm.tim_id == cfg.timer_id && machine_pwm_obj[i].pwm.channel == cfg.timer_channel) {
                self = (machine_pwm_obj_t *)&machine_pwm_obj[i];
                break;
            }
        }
    }

    if (self == NULL) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%q) doesn't have PWM capabilities"), pin->name);
    }

    // If inactive, clear out the state (may be set from a previous soft reset cycle).
    if (!(timer_pwm_active & (1 << self->pwm.tim_id))) {
        timer_pwm_active |= 1 << self->pwm.tim_id;
        size_t pwm_base_index = (self - &machine_pwm_obj[0]) & ~(NUM_CHANNELS_PER_TIMER - 1);
        for (size_t i = 0; i < 4; ++i) {
            machine_pwm_state_t *state = &machine_pwm_state[pwm_base_index + i];
            state->duty_type = DUTY_NOT_SET;
        }
    }

    // Initialise the TIM and the output driver.
    pwm_init(&self->pwm);
    pwm_enable_output(&self->pwm, pwm_get_polarity(&self->pwm));

    // Process the remaining parameters.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, all_args + 1, &kw_args);

    // Select PWM function for given pin.
    mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, cfg.alt);

    return MP_OBJ_FROM_PTR(self);
}

void machine_pwm_deinit_all(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(machine_pwm_obj); ++i) {
        machine_pwm_state_t *state = &machine_pwm_state[i];
        if (state->duty_type != DUTY_NOT_SET) {
            mp_machine_pwm_deinit((machine_pwm_obj_t *)&machine_pwm_obj[i]);
        }
    }
    timer_pwm_active = 0;
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    machine_pwm_state_t *state = get_state(self);
    state->duty_type = DUTY_NOT_SET;
    pwm_deinit(&self->pwm);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    if (pwm_freq_is_valid(&self->pwm)) {
        return MP_OBJ_NEW_SMALL_INT(pwm_get_freq(&self->pwm));
    } else {
        return MP_OBJ_NEW_SMALL_INT(0);
    }
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    // Check validity and change the frequency of the TIM peripheral.
    if (freq <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too small"));
    }
    if (!pwm_set_freq(&self->pwm, freq)) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too large"));
    }

    // Update the duty cycle of all active channels that use this TIM.
    size_t pwm_base_index = (self - &machine_pwm_obj[0]) & ~(NUM_CHANNELS_PER_TIMER - 1);
    for (size_t i = 0; i < NUM_CHANNELS_PER_TIMER; ++i) {
        machine_pwm_obj_t *obj = (machine_pwm_obj_t *)&machine_pwm_obj[pwm_base_index + i];
        machine_pwm_state_t *state = &machine_pwm_state[pwm_base_index + i];
        if (state->duty_type == DUTY_U16) {
            mp_machine_pwm_duty_set_u16(obj, state->duty);
        } else if (state->duty_type == DUTY_NS) {
            mp_machine_pwm_duty_set_ns(obj, state->duty);
        }
    }
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    machine_pwm_state_t *state = get_state(self);
    if (state->duty_type != DUTY_NOT_SET && pwm_freq_is_valid(&self->pwm)) {
        uint32_t duty_u16 = pwm_get_duty_u16(&self->pwm);
        return MP_OBJ_NEW_SMALL_INT(duty_u16);
    } else {
        return MP_OBJ_NEW_SMALL_INT(0);
    }
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    machine_pwm_state_t *state = get_state(self);
    state->duty = duty_u16;
    state->duty_type = DUTY_U16;

    if (pwm_freq_is_valid(&self->pwm)) {
        pwm_set_duty_u16(&self->pwm, duty_u16);
        pwm_start(&self->pwm);
    }
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    machine_pwm_state_t *state = get_state(self);
    if (state->duty_type != DUTY_NOT_SET && pwm_freq_is_valid(&self->pwm)) {
        return mp_obj_new_int_from_uint(pwm_get_duty_ns(&self->pwm));
    } else {
        return MP_OBJ_NEW_SMALL_INT(0);
    }
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    machine_pwm_state_t *state = get_state(self);
    state->duty = duty_ns;
    state->duty_type = DUTY_NS;

    if (pwm_freq_is_valid(&self->pwm)) {
        pwm_set_duty_ns(&self->pwm, duty_ns);
        pwm_start(&self->pwm);
    }
}
