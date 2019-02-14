/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2018 Glenn Ruben Bakke
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
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"

#if MICROPY_PY_MACHINE_HW_PWM

#include "pin.h"
#include "genhdr/pins.h"
#include "pwm.h"

#if defined(NRF52_SERIES)
// Use PWM hardware.
#include "nrfx_pwm.h"
#endif

typedef enum {
    MODE_LOW_HIGH,
    MODE_HIGH_LOW
} pwm_mode_t;

typedef struct {
    uint8_t        pwm_pin;
    uint8_t        duty;
    uint16_t       pulse_width;
    uint16_t       period;
    nrf_pwm_clk_t  freq;
    pwm_mode_t     mode;
} machine_pwm_config_t;

typedef struct _machine_hard_pwm_obj_t {
    mp_obj_base_t          base;
    const nrfx_pwm_t *     p_pwm;
    machine_pwm_config_t * p_config;
} machine_hard_pwm_obj_t;

STATIC const nrfx_pwm_t machine_hard_pwm_instances[] = {
#if defined(NRF52_SERIES)
    NRFX_PWM_INSTANCE(0),
    NRFX_PWM_INSTANCE(1),
    NRFX_PWM_INSTANCE(2),
#if NRF52840
    NRFX_PWM_INSTANCE(3),
#endif
#else
    NULL
#endif
};

STATIC machine_pwm_config_t hard_configs[MP_ARRAY_SIZE(machine_hard_pwm_instances)];

STATIC const machine_hard_pwm_obj_t machine_hard_pwm_obj[] = {
#if defined(NRF52_SERIES)
    {{&machine_hard_pwm_type}, .p_pwm = &machine_hard_pwm_instances[0], .p_config = &hard_configs[0]},
    {{&machine_hard_pwm_type}, .p_pwm = &machine_hard_pwm_instances[1], .p_config = &hard_configs[1]},
    {{&machine_hard_pwm_type}, .p_pwm = &machine_hard_pwm_instances[2], .p_config = &hard_configs[2]},
#if NRF52840
    {{&machine_hard_pwm_type}, .p_pwm = &machine_hard_pwm_instances[3], .p_config = &hard_configs[3]},
#endif
#endif
};

void pwm_init0(void) {
}


STATIC int hard_pwm_find(mp_obj_t id) {
    if (mp_obj_is_int(id)) {
        // given an integer id
        int pwm_id = mp_obj_get_int(id);
        if (pwm_id >= 0 && pwm_id < MP_ARRAY_SIZE(machine_hard_pwm_obj)) {
            return pwm_id;
        }
        mp_raise_ValueError("PWM doesn't exist");
    }
    return -1;
}

STATIC void machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_pwm_obj_t *self = self_in;
    mp_printf(print, "PWM(%u)", self->p_pwm->drv_inst_idx);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

STATIC mp_obj_t machine_hard_pwm_make_new(mp_arg_val_t *args);
STATIC void machine_hard_pwm_init(mp_obj_t self, mp_arg_val_t *args);
STATIC void machine_hard_pwm_deinit(mp_obj_t self);
STATIC mp_obj_t machine_hard_pwm_freq(mp_obj_t self, mp_arg_val_t *args);

/* common code for both soft and hard implementations *************************/

STATIC mp_obj_t machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_pin, ARG_freq, ARG_period, ARG_duty, ARG_pulse_width, ARG_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
        { MP_QSTR_pin,      MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_period,   MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_duty,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_pulse_width, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_id].u_obj == MP_OBJ_NEW_SMALL_INT(-1)) {
        // TODO: implement soft PWM
        // return machine_soft_pwm_make_new(args);
        return mp_const_none;
    } else {
        // hardware peripheral id given
        return machine_hard_pwm_make_new(args);
    }
}

STATIC mp_obj_t machine_pwm_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_INIT_pin };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} }
    };

    // parse args
    mp_obj_t self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // dispatch to specific implementation
    if (mp_obj_get_type(self) == &machine_hard_pwm_type) {
        machine_hard_pwm_init(self, args);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_pwm_init_obj, 1, machine_pwm_init);

STATIC mp_obj_t machine_pwm_deinit(mp_obj_t self) {
    // dispatch to specific implementation
    if (mp_obj_get_type(self) == &machine_hard_pwm_type) {
        machine_hard_pwm_deinit(self);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pwm_deinit_obj, machine_pwm_deinit);

STATIC mp_obj_t machine_pwm_freq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_FREQ_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = -1} },
    };

    mp_obj_t self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (mp_obj_get_type(self) == &machine_hard_pwm_type) {
        machine_hard_pwm_freq(self, args);
    } else {
        // soft pwm
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_machine_pwm_freq_obj, 1, machine_pwm_freq);

STATIC mp_obj_t machine_pwm_period(size_t n_args, const mp_obj_t *args) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_pwm_period_obj, 1, 2, machine_pwm_period);

STATIC mp_obj_t machine_pwm_duty(size_t n_args, const mp_obj_t *args) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_machine_pwm_duty_obj, 1, 2, machine_pwm_duty);

STATIC const mp_rom_map_elem_t machine_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),   MP_ROM_PTR(&machine_pwm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_pwm_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_freq),   MP_ROM_PTR(&mp_machine_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_period), MP_ROM_PTR(&mp_machine_pwm_period_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty),   MP_ROM_PTR(&mp_machine_pwm_duty_obj) },

    { MP_ROM_QSTR(MP_QSTR_FREQ_16MHZ),    MP_ROM_INT(NRF_PWM_CLK_16MHz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_8MHZ),     MP_ROM_INT(NRF_PWM_CLK_8MHz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_4MHZ),     MP_ROM_INT(NRF_PWM_CLK_4MHz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_2MHZ),     MP_ROM_INT(NRF_PWM_CLK_2MHz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_1MHZ),     MP_ROM_INT(NRF_PWM_CLK_1MHz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_500KHZ),   MP_ROM_INT(NRF_PWM_CLK_500kHz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_250KHZ),   MP_ROM_INT(NRF_PWM_CLK_250kHz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_125KHZ),   MP_ROM_INT(NRF_PWM_CLK_125kHz) },

    { MP_ROM_QSTR(MP_QSTR_MODE_LOW_HIGH), MP_ROM_INT(MODE_LOW_HIGH) },
    { MP_ROM_QSTR(MP_QSTR_MODE_HIGH_LOW), MP_ROM_INT(MODE_HIGH_LOW) },
};

STATIC MP_DEFINE_CONST_DICT(machine_pwm_locals_dict, machine_pwm_locals_dict_table);

/* code for hard implementation ***********************************************/

STATIC mp_obj_t machine_hard_pwm_make_new(mp_arg_val_t *args) {
    enum { ARG_id, ARG_pin, ARG_freq, ARG_period, ARG_duty, ARG_pulse_width, ARG_mode };
    // get static peripheral object
    int pwm_id = hard_pwm_find(args[ARG_id].u_obj);
    const machine_hard_pwm_obj_t *self = &machine_hard_pwm_obj[pwm_id];

    // check if PWM pin is set
    if (args[ARG_pin].u_obj != MP_OBJ_NULL) {
        self->p_config->pwm_pin = mp_hal_get_pin_obj(args[ARG_pin].u_obj)->pin;
    } else {
        // TODO: raise exception.
    }

    if (args[ARG_freq].u_obj != MP_OBJ_NULL) {
        self->p_config->freq = mp_obj_get_int(args[ARG_freq].u_obj);
    } else {
        self->p_config->freq = 50; // 50 Hz by default.
    }

    if (args[ARG_period].u_obj != MP_OBJ_NULL) {
        self->p_config->period = mp_obj_get_int(args[ARG_period].u_obj);
    } else {
        mp_raise_ValueError("PWM period must be within 16000 cycles");
    }

    if (args[ARG_duty].u_obj != MP_OBJ_NULL) {
        self->p_config->duty = mp_obj_get_int(args[ARG_duty].u_obj);
    } else {
        self->p_config->duty = 50; // 50% by default.
    }

    if (args[ARG_pulse_width].u_obj != MP_OBJ_NULL) {
        self->p_config->pulse_width = mp_obj_get_int(args[ARG_pulse_width].u_obj);
    } else {
        self->p_config->pulse_width = 0;
    }

    if (args[ARG_mode].u_obj != MP_OBJ_NULL) {
        self->p_config->mode = mp_obj_get_int(args[ARG_mode].u_obj);
    } else {
        self->p_config->mode = MODE_HIGH_LOW;
    }
 
    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_hard_pwm_init(mp_obj_t self_in, mp_arg_val_t *args) {
    machine_hard_pwm_obj_t *self = self_in;

    nrfx_pwm_config_t config;

    config.output_pins[0] = self->p_config->pwm_pin;
    config.output_pins[1] = NRFX_PWM_PIN_NOT_USED;
    config.output_pins[2] = NRFX_PWM_PIN_NOT_USED;
    config.output_pins[3] = NRFX_PWM_PIN_NOT_USED;

    config.irq_priority   = 6;
    config.base_clock     = self->p_config->freq;
    config.count_mode     = NRF_PWM_MODE_UP;
    config.top_value      = self->p_config->period;
    config.load_mode      = NRF_PWM_LOAD_INDIVIDUAL;
    config.step_mode      = NRF_PWM_STEP_AUTO;

    nrfx_pwm_init(self->p_pwm, &config, NULL);

    uint16_t pulse_width = ((self->p_config->period * self->p_config->duty) / 100);

    // If manual period has been set, override duty-cycle.
    if (self->p_config->pulse_width > 0) {
        pulse_width = self->p_config->pulse_width;
    }

    // TODO: Move DMA buffer to global memory.
    volatile static uint16_t pwm_seq[4];

    if (self->p_config->mode == MODE_HIGH_LOW) {
        pwm_seq[0] = self->p_config->period - pulse_width;
        pwm_seq[1] = self->p_config->period - pulse_width;
    } else {
        pwm_seq[0] = self->p_config->period - pulse_width;
        pwm_seq[1] = self->p_config->period - pulse_width;
    }

    pwm_seq[2] = self->p_config->period - pulse_width;
    pwm_seq[3] = self->p_config->period - pulse_width;

    const nrf_pwm_sequence_t pwm_sequence = {
        .values.p_raw = (const uint16_t *)&pwm_seq,
        .length = 4,
        .repeats = 0,
        .end_delay = 0
    };

    nrfx_pwm_simple_playback(self->p_pwm,
                             &pwm_sequence,
                             0, // Loop disabled.
                             0);
}

STATIC void machine_hard_pwm_deinit(mp_obj_t self_in) {
    machine_hard_pwm_obj_t *self = self_in;
    (void)self;
    nrfx_pwm_stop(self->p_pwm, true);
    nrfx_pwm_uninit(self->p_pwm);
}

STATIC mp_obj_t machine_hard_pwm_freq(mp_obj_t self_in, mp_arg_val_t *args) {
    machine_hard_pwm_obj_t *self = self_in;
    (void)self;
    return mp_const_none;
}

const mp_obj_type_t machine_hard_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = machine_pwm_print,
    .make_new = machine_pwm_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_pwm_locals_dict,
};

#endif // MICROPY_PY_MACHINE_HW_PWM
