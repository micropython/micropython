/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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
#include "utimer.h"

#define UTIMER ((UTIMER_Type *)UTIMER_BASE)
#define UTIMER_SOURCE_HZ (400000000)

// Each UTIMER channel has two independent output drivers.
enum {
    DRIVER_A,
    DRIVER_B,
};

enum {
    BUFFERING_SINGLE = 0,
    BUFFERING_DOUBLE = 1,
};

enum {
    DRIVER_VALUE_RETAINED = 0,
    DRIVER_VALUE_0 = 1,
    DRIVER_VALUE_1 = 2,
    DRIVER_VALUE_TOGGLE = 3,
};

// deinit (go to idle level)

static void utimer_pwm_channel_init(unsigned int channel) {
    utimer_counter_stop(UTIMER, channel, true);
    utimer_reset(UTIMER, channel);
    utimer_clock_enable(UTIMER, channel);
    UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_CNTR_PTR = 1;
    UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_CNTR_PTR_BUF1 = 1;
    UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_CNTR_CTRL |= CNTR_CTRL_EN;
    UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_BUF_OP_CTRL =
        BUF_OP_CTRL_COMPARE_B_BUF_EVENT
        | BUF_OP_CTRL_COMPARE_A_BUF_EVENT
        | BUF_OP_CTRL_CNTR_BUF_OP_BIT0 // single buffer operation
        | BUF_OP_CTRL_COMPARE_BUF_EN
        | BUF_OP_CTRL_CNTR_BUF_EN;
    utimer_control_enable(UTIMER, channel);
}

static void utimer_pwm_channel_deinit(unsigned int channel) {
    utimer_counter_stop(UTIMER, channel, true);
    utimer_reset(UTIMER, channel);
    utimer_clock_disable(UTIMER, channel);
}

static void utimer_pwm_driver_init(unsigned int channel, unsigned int driver, bool invert) {
    uint32_t ctrl = COMPARE_CTRL_DRV_COMPARE_EN;
    if (invert) {
        ctrl |=
            COMPARE_CTRL_DRV_DISABLE_VAL // high upon disable
            | COMPARE_CTRL_DRV_START_VAL
            | COMPARE_CTRL_DRV_STOP_VAL
            | DRIVER_VALUE_0 << 2 // low at cycle end
                | DRIVER_VALUE_1 << 0 // high at compare match
        ;
    } else {
        ctrl |=
            DRIVER_VALUE_1 << 2 // high at cycle end
                | DRIVER_VALUE_0 << 0 // low at compare match
        ;
    }

    uint32_t chan_oen;
    volatile uint32_t *compare_ctrl;
    if (driver == DRIVER_A) {
        chan_oen = GLB_DRIVER_CHAN_A_OEN;
        compare_ctrl = &UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_COMPARE_CTRL_A;
    } else {
        chan_oen = GLB_DRIVER_CHAN_B_OEN;
        compare_ctrl = &UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_COMPARE_CTRL_B;
    }

    // Enable driver output (the bit is active low).
    UTIMER->UTIMER_GLB_DRIVER_OEN &= ~(chan_oen << (channel << 1));

    // Configure driver logic levels.
    *compare_ctrl = (*compare_ctrl & COMPARE_CTRL_DRV_DRIVER_EN) | ctrl;
}

static void utimer_pwm_driver_deinit(unsigned int channel, unsigned int driver) {
    if (driver == DRIVER_A) {
        UTIMER->UTIMER_GLB_DRIVER_OEN |= GLB_DRIVER_CHAN_A_OEN << (channel << 1);
        UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_COMPARE_CTRL_A &= ~COMPARE_CTRL_DRV_DRIVER_EN;
    } else {
        UTIMER->UTIMER_GLB_DRIVER_OEN |= GLB_DRIVER_CHAN_B_OEN << (channel << 1);
        UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_COMPARE_CTRL_B &= ~COMPARE_CTRL_DRV_DRIVER_EN;
    }
}

static inline bool utimer_pwm_freq_is_valid(unsigned int channel) {
    return UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_CNTR_PTR_BUF1 != 1;
}

static inline uint32_t utimer_pwm_get_max_count(unsigned int channel) {
    return UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_CNTR_PTR_BUF1;
}

static inline void utimer_pwm_set_max_count(unsigned int channel, uint32_t max_count) {
    UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_CNTR_PTR_BUF1 = max_count;
}

static inline uint32_t utimer_pwm_get_duty(unsigned int channel, unsigned int driver) {
    if (driver == DRIVER_A) {
        return utimer_get_count(UTIMER, channel, UTIMER_COMPARE_A_BUF1);
    } else {
        return utimer_get_count(UTIMER, channel, UTIMER_COMPARE_B_BUF1);
    }
}

static inline void utimer_pwm_set_duty(unsigned int channel, unsigned int driver, uint32_t duty) {
    volatile uint32_t *compare_ctrl;
    volatile uint32_t *compare_buf;
    if (driver == DRIVER_A) {
        compare_ctrl = &UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_COMPARE_CTRL_A;
        compare_buf = &UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_COMPARE_A_BUF1;
    } else {
        compare_ctrl = &UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_COMPARE_CTRL_B;
        compare_buf = &UTIMER->UTIMER_CHANNEL_CFG[channel].UTIMER_COMPARE_B_BUF1;
    }
    if (duty == 0) {
        *compare_ctrl &= ~COMPARE_CTRL_DRV_DRIVER_EN;
    } else {
        *compare_ctrl |= COMPARE_CTRL_DRV_DRIVER_EN;
    }
    *compare_buf = duty;
}

static inline void utimer_pwm_start(unsigned int channel) {
    utimer_counter_start(UTIMER, channel);
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
    uint8_t state_index; // indexes machine_pwm_state
    uint8_t channel; // UTIMER channel id
    uint8_t driver; // DRIVER_A or DRIVER_B
} machine_pwm_obj_t;

typedef struct _machine_pwm_state_t {
    bool invert;
    uint8_t duty_type;
    mp_int_t duty;
} machine_pwm_state_t;

static uint32_t utimer_pwm_channel_active = 0;

static const machine_pwm_obj_t machine_pwm_obj[22] = {
    {{&machine_pwm_type}, 0, 0, DRIVER_A},
    {{&machine_pwm_type}, 1, 0, DRIVER_B},
    {{&machine_pwm_type}, 2, 1, DRIVER_A},
    {{&machine_pwm_type}, 3, 1, DRIVER_B},
    {{&machine_pwm_type}, 4, 2, DRIVER_A},
    {{&machine_pwm_type}, 5, 2, DRIVER_B},
    {{&machine_pwm_type}, 6, 3, DRIVER_A},
    {{&machine_pwm_type}, 7, 3, DRIVER_B},
    {{&machine_pwm_type}, 8, 4, DRIVER_A},
    {{&machine_pwm_type}, 9, 4, DRIVER_B},
    {{&machine_pwm_type}, 10, 5, DRIVER_A},
    {{&machine_pwm_type}, 11, 5, DRIVER_B},
    {{&machine_pwm_type}, 12, 6, DRIVER_A},
    {{&machine_pwm_type}, 13, 6, DRIVER_B},
    {{&machine_pwm_type}, 14, 7, DRIVER_A},
    {{&machine_pwm_type}, 15, 7, DRIVER_B},
    {{&machine_pwm_type}, 16, 8, DRIVER_A},
    {{&machine_pwm_type}, 17, 8, DRIVER_B},
    {{&machine_pwm_type}, 18, 9, DRIVER_A},
    {{&machine_pwm_type}, 19, 9, DRIVER_B},
    {{&machine_pwm_type}, 20, 10, DRIVER_A},
    {{&machine_pwm_type}, 21, 10, DRIVER_B},
    #if 0
    // UTIMER 11 is used by system tick.
    {{&machine_pwm_type}, 22, 11, DRIVER_A},
    {{&machine_pwm_type}, 23, 11, DRIVER_B},
    #endif
};

static machine_pwm_state_t machine_pwm_state[22];

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<PWM channel=%u driver=%u>", self->channel, self->driver);
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

    machine_pwm_state_t *state = &machine_pwm_state[self->state_index];

    if (args[ARG_invert].u_int != -1) {
        state->invert = !!args[ARG_invert].u_int;
        utimer_pwm_driver_init(self->channel, self->driver, state->invert);
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

    // Search for the UTIMER alternate function for the given pin.
    int pwm_index = -1;
    unsigned int pin_alt_index;
    for (pin_alt_index = 0; pin_alt_index < MP_ARRAY_SIZE(pin->alt); pin_alt_index++) {
        unsigned int alt_func = MP_HAL_PIN_ALT_GET_FUNC(pin->alt[pin_alt_index]);
        unsigned int alt_unit = MP_HAL_PIN_ALT_GET_UNIT(pin->alt[pin_alt_index]);
        if (alt_func == MP_HAL_PIN_ALT_UT_T0) {
            pwm_index = alt_unit * 2;
            break;
        } else if (alt_func == MP_HAL_PIN_ALT_UT_T1) {
            pwm_index = alt_unit * 2 + 1;
            break;
        }
    }

    // Pin does not have an available UTIMER associated with it.
    if (pwm_index < 0 || pwm_index >= MP_ARRAY_SIZE(machine_pwm_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%q) doesn't have PWM capabilities"), pin->name);
    }

    // Get static peripheral object.
    machine_pwm_obj_t *self = (machine_pwm_obj_t *)&machine_pwm_obj[pwm_index];

    // If inactive, initialise state and UTIMER peripheral.
    if (!(utimer_pwm_channel_active & (1 << self->channel))) {
        utimer_pwm_channel_active |= 1 << self->channel;
        utimer_pwm_channel_init(self->channel);
        for (size_t i = 0; i < 2; ++i) {
            machine_pwm_state_t *state = &machine_pwm_state[(pwm_index & ~1) + i];
            state->invert = false;
            state->duty_type = DUTY_NOT_SET;
        }
    }

    // Initialise the UTIMER output driver.
    machine_pwm_state_t *state = &machine_pwm_state[self->state_index];
    utimer_pwm_driver_init(self->channel, self->driver, state->invert);

    // Process the remaining parameters.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, all_args + 1, &kw_args);

    // Select PWM function for given pin.
    mp_hal_pin_config(pin, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, MP_HAL_PIN_SPEED_HIGH, MP_HAL_PIN_DRIVE_8MA, pin->alt[pin_alt_index], false);

    return MP_OBJ_FROM_PTR(self);
}

void machine_pwm_deinit_all(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(machine_pwm_obj); ++i) {
        const machine_pwm_obj_t *self = &machine_pwm_obj[i];
        utimer_pwm_driver_deinit(self->channel, self->driver);
        if (self->driver == DRIVER_B) {
            utimer_pwm_channel_deinit(self->channel);
        }
    }
    utimer_pwm_channel_active = 0;
}

static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    utimer_pwm_driver_deinit(self->channel, self->driver);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    if (utimer_pwm_freq_is_valid(self->channel)) {
        uint32_t max_count = utimer_pwm_get_max_count(self->channel);
        return MP_OBJ_NEW_SMALL_INT(UTIMER_SOURCE_HZ / (max_count + 1));
    } else {
        return MP_OBJ_NEW_SMALL_INT(0);
    }
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    // Ensure we don't go outside the limits of the UTIMER peripheral.
    if (freq == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too small"));
    } else if (freq > 50000000) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too large"));
    }

    // Configure UTIMER to cycle at the given frequency.
    uint32_t max_count = (UTIMER_SOURCE_HZ + freq / 2) / freq - 1;
    utimer_pwm_set_max_count(self->channel, max_count);

    machine_pwm_state_t *state = &machine_pwm_state[self->state_index];
    if (state->duty_type == DUTY_U16) {
        mp_machine_pwm_duty_set_u16(self, state->duty);
    } else if (state->duty_type == DUTY_NS) {
        mp_machine_pwm_duty_set_ns(self, state->duty);
    }

    machine_pwm_obj_t *other = self->driver == DRIVER_A ? self + 1 : self - 1;
    machine_pwm_state_t *other_state = &machine_pwm_state[other->state_index];
    if (other_state->duty_type == DUTY_U16) {
        mp_machine_pwm_duty_set_u16(other, other_state->duty);
    } else if (other_state->duty_type == DUTY_NS) {
        mp_machine_pwm_duty_set_ns(other, other_state->duty);
    }
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    machine_pwm_state_t *state = &machine_pwm_state[self->state_index];
    if (state->duty_type != DUTY_NOT_SET && utimer_pwm_freq_is_valid(self->channel)) {
        uint32_t max_count = utimer_pwm_get_max_count(self->channel);
        uint32_t comp = utimer_pwm_get_duty(self->channel, self->driver);
        uint32_t duty_u16 = ((uint64_t)comp * 65535ULL + (uint64_t)(max_count + 1) / 2) / (max_count + 1);
        return MP_OBJ_NEW_SMALL_INT(duty_u16);
    } else {
        return MP_OBJ_NEW_SMALL_INT(0);
    }
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    // Limit duty_u16 to 65535.
    if (duty_u16 > 65535) {
        duty_u16 = 65535;
    }

    machine_pwm_state_t *state = &machine_pwm_state[self->state_index];
    state->duty = duty_u16;
    state->duty_type = DUTY_U16;

    if (utimer_pwm_freq_is_valid(self->channel)) {
        uint32_t max_count = utimer_pwm_get_max_count(self->channel);
        uint32_t comp = ((uint64_t)duty_u16 * (uint64_t)(max_count + 1) + 65535ULL / 2ULL) / 65535ULL;
        utimer_pwm_set_duty(self->channel, self->driver, comp);
        utimer_pwm_start(self->channel);
    }
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    machine_pwm_state_t *state = &machine_pwm_state[self->state_index];
    if (state->duty_type != DUTY_NOT_SET && utimer_pwm_freq_is_valid(self->channel)) {
        MP_STATIC_ASSERT(UTIMER_SOURCE_HZ == 400000000);
        uint32_t comp = utimer_pwm_get_duty(self->channel, self->driver);
        uint32_t duty_ns = comp * 5 / 2; // optimised version of: comp * 1e9 / UTIMER_SOURCE_HZ
        return mp_obj_new_int_from_uint(duty_ns);
    } else {
        return MP_OBJ_NEW_SMALL_INT(0);
    }
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    machine_pwm_state_t *state = &machine_pwm_state[self->state_index];
    state->duty = duty_ns;
    state->duty_type = DUTY_NS;

    if (utimer_pwm_freq_is_valid(self->channel)) {
        MP_STATIC_ASSERT(UTIMER_SOURCE_HZ == 400000000);
        uint32_t max_count = utimer_pwm_get_max_count(self->channel);
        uint32_t comp = duty_ns * 2 / 5; // optimised version of: duty_ns * 1e-9 * UTIMER_SOURCE_HZ
        if (comp > (max_count + 1)) {
            comp = max_count + 1;
        }
        utimer_pwm_set_duty(self->channel, self->driver, comp);
        utimer_pwm_start(self->channel);
    }
}
