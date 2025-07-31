/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Glenn Ruben Bakke
 * Copyright (c) 2023 Robert Hammelrath
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

#include <string.h>
#include "py/mphal.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "pwm.h"

#if defined(NRF52_SERIES)
// Use PWM hardware.
#include "nrfx_pwm.h"
#endif

#define PWM_MAX_BASE_FREQ   (16000000)
#define PWM_MIN_BASE_FREQ   (125000)
#define PWM_MAX_PERIOD      (32768)

typedef enum {
    MODE_HIGH_LOW = 0,
    MODE_LOW_HIGH
} pwm_mode_t;

typedef enum {
    DUTY_NOT_SET = 0,
    DUTY_PERCENT,
    DUTY_U16,
    DUTY_NS
} pwm_duty_t;

typedef enum {
    FREE = 0,
    STOPPED,
    RUNNING
} pwm_run_t;

typedef struct {
    uint8_t pwm_pin[NRF_PWM_CHANNEL_COUNT];
    pwm_mode_t mode[NRF_PWM_CHANNEL_COUNT];
    pwm_duty_t duty_mode[NRF_PWM_CHANNEL_COUNT];
    uint32_t duty[NRF_PWM_CHANNEL_COUNT];
    uint16_t pwm_seq[4];
    pwm_run_t active;
    bool defer_start;
    int8_t freq_div;
    uint32_t freq;
} machine_pwm_config_t;

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    const nrfx_pwm_t *p_pwm;
    machine_pwm_config_t *p_config;
    uint8_t id;
    uint8_t channel;
} machine_pwm_obj_t;

static const nrfx_pwm_t machine_hard_pwm_instances[] = {
    #if defined(NRF52_SERIES)
    NRFX_PWM_INSTANCE(0),
    NRFX_PWM_INSTANCE(1),
    NRFX_PWM_INSTANCE(2),
    #if NRF52840
    NRFX_PWM_INSTANCE(3),
    #endif
    #endif
};

static machine_pwm_config_t hard_configs[MP_ARRAY_SIZE(machine_hard_pwm_instances)];

static const machine_pwm_obj_t machine_hard_pwm_obj[] = {
    #if defined(NRF52_SERIES)
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[0], .p_config = &hard_configs[0], 0, 0},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[0], .p_config = &hard_configs[0], 0, 1},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[0], .p_config = &hard_configs[0], 0, 2},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[0], .p_config = &hard_configs[0], 0, 3},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[1], .p_config = &hard_configs[1], 1, 0},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[1], .p_config = &hard_configs[1], 1, 1},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[1], .p_config = &hard_configs[1], 1, 2},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[1], .p_config = &hard_configs[1], 1, 3},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[2], .p_config = &hard_configs[2], 2, 0},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[2], .p_config = &hard_configs[2], 2, 1},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[2], .p_config = &hard_configs[2], 2, 2},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[2], .p_config = &hard_configs[2], 2, 3},
    #if NRF52840
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[3], .p_config = &hard_configs[3], 3, 0},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[3], .p_config = &hard_configs[3], 3, 1},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[3], .p_config = &hard_configs[3], 3, 2},
    {{&machine_pwm_type}, .p_pwm = &machine_hard_pwm_instances[3], .p_config = &hard_configs[3], 3, 3},
    #endif
    #endif
};

void pwm_init0(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(hard_configs); i++) {
        hard_configs[i].active = FREE;
        hard_configs[i].freq_div = -1;
        hard_configs[i].freq = 0;
        memset(hard_configs[i].duty_mode, DUTY_NOT_SET, NRF_PWM_CHANNEL_COUNT);
    }
}

// Find a free PWM object
static int hard_pwm_find() {
    // look for a free module.
    for (int j = 0; j < MP_ARRAY_SIZE(hard_configs); j++) {
        if (hard_configs[j].active == FREE) {
            return j * NRF_PWM_CHANNEL_COUNT;
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("all PWM devices in use"));
}

static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = self_in;
    static char *duty_suffix[] = { "", "", "_u16", "_ns" };
    mp_printf(print, "<PWM: Pin=%u freq=%dHz duty%s=%d invert=%d id=%u channel=%u>",
        self->p_config->pwm_pin[self->channel], self->p_config->freq,
        duty_suffix[self->p_config->duty_mode[self->channel]], self->p_config->duty[self->channel],
        self->p_config->mode[self->channel], self->id, self->channel);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

static void machine_hard_pwm_start(const machine_pwm_obj_t *self);

static const mp_arg_t allowed_args[] = {
    { MP_QSTR_pin,      MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_duty,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_device,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    { MP_QSTR_channel,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
};

static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_pin, ARG_freq, ARG_duty, ARG_duty_u16, ARG_duty_ns, ARG_invert, ARG_device, ARG_channel };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->p_config->defer_start = true;
    if (args[ARG_freq].u_int != -1) {
        mp_machine_pwm_freq_set(self, args[ARG_freq].u_int);
    }
    if (args[ARG_duty].u_int != -1) {
        mp_machine_pwm_duty_set(self, args[ARG_duty].u_int);
    }
    if (args[ARG_duty_u16].u_int != -1) {
        mp_machine_pwm_duty_set_u16(self, args[ARG_duty_u16].u_int);
    }
    if (args[ARG_duty_ns].u_int != -1) {
        mp_machine_pwm_duty_set_ns(self, args[ARG_duty_ns].u_int);
    }
    if (args[ARG_invert].u_int != -1) {
        self->p_config->mode[self->channel] = args[ARG_invert].u_int ? MODE_LOW_HIGH : MODE_HIGH_LOW;
    }
    self->p_config->defer_start = false;

    machine_hard_pwm_start(self);
}


static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_freq, ARG_duty, ARG_duty_u16, ARG_duty_ns, ARG_invert, ARG_device, ARG_channel };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // check if the PWM pin is given.
    int pwm_pin;
    if (args[ARG_pin].u_obj != MP_OBJ_NULL) {
        pwm_pin = mp_hal_get_pin_obj(args[ARG_pin].u_obj)->pin;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Pin missing"));
    }

    // Get the PWM object number
    // If just the ID is given, use channel 0
    // If none is given, attempt to find an unused object.
    int pwm_id = -1;
    if (args[ARG_device].u_int != -1) {
        if (args[ARG_device].u_int >= 0 && args[ARG_device].u_int < MP_ARRAY_SIZE(machine_hard_pwm_instances)) {
            pwm_id = args[ARG_device].u_int * NRF_PWM_CHANNEL_COUNT;
            if (args[ARG_channel].u_int != -1) {
                if (args[ARG_channel].u_int >= 0 && args[ARG_channel].u_int < NRF_PWM_CHANNEL_COUNT) {
                    pwm_id += args[ARG_channel].u_int;
                }
            }
        }
    } else {
        // no ID given, search for a free ID.
        pwm_id = hard_pwm_find();
    }
    if (pwm_id < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid PWM id"));
    }
    const machine_pwm_obj_t *self = &machine_hard_pwm_obj[pwm_id];
    int pwm_channel = pwm_id % NRF_PWM_CHANNEL_COUNT;
    self->p_config->pwm_pin[pwm_channel] = pwm_pin;
    self->p_config->duty_mode[pwm_channel] = DUTY_NOT_SET;
    self->p_config->duty[pwm_channel] = 0;
    self->p_config->mode[pwm_channel] = MODE_HIGH_LOW;
    self->p_config->defer_start = false;

    // Allocate the device if it was not used before.
    if (hard_configs[pwm_id].active == FREE) {
        hard_configs[pwm_id].active = STOPPED;
    }

    // start the PWM running for this channel
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_machine_pwm_init_helper((machine_pwm_obj_t *)self, n_args, all_args, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

// Stop all PWM modules and release them
void pwm_deinit_all(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(machine_hard_pwm_instances); i++) {
        mp_machine_pwm_deinit((machine_pwm_obj_t *)&machine_hard_pwm_obj[i * NRF_PWM_CHANNEL_COUNT]);
    }
    pwm_init0();
}

// Stop the PWM module, but do not release it.
static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    self->p_config->active = STOPPED;
    nrfx_pwm_stop(self->p_pwm, true);
    nrfx_pwm_uninit(self->p_pwm);
}

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->p_config->freq);
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {

    uint8_t div = 0;
    if (freq > (PWM_MAX_BASE_FREQ / 3) || freq <= (PWM_MIN_BASE_FREQ / PWM_MAX_PERIOD)) {
        mp_raise_ValueError(MP_ERROR_TEXT("frequency out of range"));
    }
    for (div = 0; div < 8; div++) {
        if (PWM_MAX_BASE_FREQ / (1 << div) / freq < PWM_MAX_PERIOD) {
            break;
        }
    }
    self->p_config->freq_div = div;
    self->p_config->freq = freq;
    machine_hard_pwm_start(self);
}

static mp_obj_t mp_machine_pwm_duty_get(machine_pwm_obj_t *self) {
    if (self->p_config->duty_mode[self->channel] == DUTY_PERCENT) {
        return MP_OBJ_NEW_SMALL_INT(self->p_config->duty[self->channel]);
    } else if (self->p_config->duty_mode[self->channel] == DUTY_U16) {
        return MP_OBJ_NEW_SMALL_INT(self->p_config->duty[self->channel] * 100 / 65535);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}

static void mp_machine_pwm_duty_set(machine_pwm_obj_t *self, mp_int_t duty) {
    self->p_config->duty[self->channel] = duty;
    self->p_config->duty_mode[self->channel] = DUTY_PERCENT;
    machine_hard_pwm_start(self);
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    if (self->p_config->duty_mode[self->channel] == DUTY_U16) {
        return MP_OBJ_NEW_SMALL_INT(self->p_config->duty[self->channel]);
    } else if (self->p_config->duty_mode[self->channel] == DUTY_PERCENT) {
        return MP_OBJ_NEW_SMALL_INT(self->p_config->duty[self->channel] * 65535 / 100);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty) {
    self->p_config->duty[self->channel] = duty;
    self->p_config->duty_mode[self->channel] = DUTY_U16;
    machine_hard_pwm_start(self);
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    if (self->p_config->duty_mode[self->channel] == DUTY_NS) {
        return MP_OBJ_NEW_SMALL_INT(self->p_config->duty[self->channel]);
    } else {
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty) {
    self->p_config->duty[self->channel] = duty;
    self->p_config->duty_mode[self->channel] = DUTY_NS;
    machine_hard_pwm_start(self);
}

/* code for hard implementation ***********************************************/

static void machine_hard_pwm_start(const machine_pwm_obj_t *self) {

    nrfx_pwm_config_t config;

    // check if ready to go
    if (self->p_config->defer_start == true || self->p_config->freq_div < 0 || self->p_config->duty_mode[self->channel] == DUTY_NOT_SET) {
        return; // Not ready yet.
    }

    self->p_config->active = RUNNING;

    config.output_pins[0] = self->p_config->duty_mode[0] != DUTY_NOT_SET ? self->p_config->pwm_pin[0] : NRFX_PWM_PIN_NOT_USED;
    config.output_pins[1] = self->p_config->duty_mode[1] != DUTY_NOT_SET ? self->p_config->pwm_pin[1] : NRFX_PWM_PIN_NOT_USED;
    config.output_pins[2] = self->p_config->duty_mode[2] != DUTY_NOT_SET ? self->p_config->pwm_pin[2] : NRFX_PWM_PIN_NOT_USED;
    config.output_pins[3] = self->p_config->duty_mode[3] != DUTY_NOT_SET ? self->p_config->pwm_pin[3] : NRFX_PWM_PIN_NOT_USED;

    uint32_t tick_freq = PWM_MAX_BASE_FREQ / (1 << self->p_config->freq_div);
    uint32_t period = tick_freq / self->p_config->freq;

    config.irq_priority = 6;
    config.base_clock = self->p_config->freq_div;
    config.count_mode = NRF_PWM_MODE_UP;
    config.top_value = period;
    config.load_mode = NRF_PWM_LOAD_INDIVIDUAL;
    config.step_mode = NRF_PWM_STEP_AUTO;

    nrfx_pwm_stop(self->p_pwm, true);
    nrfx_pwm_uninit(self->p_pwm);

    nrfx_pwm_init(self->p_pwm, &config, NULL, NULL);

    for (int i = 0; i < NRF_PWM_CHANNEL_COUNT; i++) {
        uint16_t pulse_width = 0;
        if (self->p_config->duty_mode[i] == DUTY_PERCENT) {
            pulse_width = ((period * self->p_config->duty[i]) / 100);
        } else if (self->p_config->duty_mode[i] == DUTY_U16) {
            pulse_width = ((period * self->p_config->duty[i]) / 65535);
        } else if (self->p_config->duty_mode[i] == DUTY_NS) {
            pulse_width = (uint64_t)self->p_config->duty[i] * tick_freq / 1000000000ULL;
        }

        if (self->p_config->mode[i] == MODE_HIGH_LOW) {
            self->p_config->pwm_seq[i] = 0x8000 | pulse_width;
        } else {
            self->p_config->pwm_seq[i] = pulse_width;
        }
    }

    const nrf_pwm_sequence_t pwm_sequence = {
        .values.p_raw = (const uint16_t *)&self->p_config->pwm_seq,
        .length = 4,
        .repeats = 0,
        .end_delay = 0
    };

    nrfx_pwm_simple_playback(self->p_pwm,
        &pwm_sequence,
        0, // Loop disabled.
        0);
}
