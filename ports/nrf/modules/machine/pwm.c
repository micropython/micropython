/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#if NRF52
// Use PWM hardware.
#include "hal_pwm.h"
#endif

#ifdef MICROPY_HW_PWM0_NAME
PWM_HandleTypeDef PWMHandle0 = {.instance = NULL};
#endif

STATIC const pyb_pwm_obj_t machine_pwm_obj[] = {
    #ifdef MICROPY_HW_PWM0_NAME
    {{&machine_hard_pwm_type}, &PWMHandle0},
    #else
    {{&machine_hard_pwm_type}, NULL},
    #endif
};

void pwm_init0(void) {
    // reset the PWM handles
    #ifdef MICROPY_HW_PWM0_NAME
    memset(&PWMHandle0, 0, sizeof(PWM_HandleTypeDef));
    PWMHandle0.instance = PWM0;
    #endif
}

STATIC int pwm_find(mp_obj_t id) {
    if (MP_OBJ_IS_STR(id)) {
        // given a string id
        const char *port = mp_obj_str_get_str(id);
        if (0) {
        #ifdef MICROPY_HW_PWM0_NAME
        } else if (strcmp(port, MICROPY_HW_PWM0_NAME) == 0) {
            return 1;
        #endif
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "PWM(%s) does not exist", port));
    } else {
        // given an integer id
        int pwm_id = mp_obj_get_int(id);
        if (pwm_id >= 0 && pwm_id <= MP_ARRAY_SIZE(machine_pwm_obj)
            && machine_pwm_obj[pwm_id].pwm != NULL) {
            return pwm_id;
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "PWM(%d) does not exist", pwm_id));
    }
}

void pwm_init(PWM_HandleTypeDef *pwm) {
	// start pwm
	hal_pwm_start(pwm->instance);
}

void pwm_deinit(PWM_HandleTypeDef *pwm) {
	// stop pwm
	hal_pwm_stop(pwm->instance);
}

STATIC void pwm_print(const mp_print_t *print, PWM_HandleTypeDef *pwm, bool legacy) {
    uint pwm_num = 0; // default to PWM0
    mp_printf(print, "PWM(%u)", pwm_num);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

// for make_new
enum {
    ARG_NEW_id,
    ARG_NEW_pin,
    ARG_NEW_freq,
    ARG_NEW_period,
    ARG_NEW_duty,
    ARG_NEW_pulse_width,
    ARG_NEW_mode
};

// for init
enum {
    ARG_INIT_pin
};

// for freq
enum {
    ARG_FREQ_freq
};

STATIC mp_obj_t machine_hard_pwm_make_new(mp_arg_val_t *args);
STATIC void machine_hard_pwm_init(mp_obj_t self, mp_arg_val_t *args);
STATIC void machine_hard_pwm_deinit(mp_obj_t self);
STATIC mp_obj_t machine_hard_pwm_freq(mp_obj_t self, mp_arg_val_t *args);

/* common code for both soft and hard implementations *************************/

STATIC mp_obj_t machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
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

    if (args[ARG_NEW_id].u_obj == MP_OBJ_NEW_SMALL_INT(-1)) {
        // TODO: implement soft PWM
        // return machine_soft_pwm_make_new(args);
        return mp_const_none;
    } else {
        // hardware peripheral id given
        return machine_hard_pwm_make_new(args);
    }
}

STATIC mp_obj_t machine_pwm_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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

    { MP_ROM_QSTR(MP_QSTR_FREQ_16MHZ),    MP_ROM_INT(HAL_PWM_FREQ_16Mhz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_8MHZ),     MP_ROM_INT(HAL_PWM_FREQ_8Mhz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_4MHZ),     MP_ROM_INT(HAL_PWM_FREQ_4Mhz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_2MHZ),     MP_ROM_INT(HAL_PWM_FREQ_2Mhz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_1MHZ),     MP_ROM_INT(HAL_PWM_FREQ_1Mhz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_500KHZ),   MP_ROM_INT(HAL_PWM_FREQ_500khz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_250KHZ),   MP_ROM_INT(HAL_PWM_FREQ_250khz) },
    { MP_ROM_QSTR(MP_QSTR_FREQ_125KHZ),   MP_ROM_INT(HAL_PWM_FREQ_125khz) },

    { MP_ROM_QSTR(MP_QSTR_MODE_LOW_HIGH), MP_ROM_INT(HAL_PWM_MODE_LOW_HIGH) },
    { MP_ROM_QSTR(MP_QSTR_MODE_HIGH_LOW), MP_ROM_INT(HAL_PWM_MODE_HIGH_LOW) },
};

STATIC MP_DEFINE_CONST_DICT(machine_pwm_locals_dict, machine_pwm_locals_dict_table);

/* code for hard implementation ***********************************************/

STATIC const machine_hard_pwm_obj_t machine_hard_pwm_obj[] = {
    {{&machine_hard_pwm_type}, &machine_pwm_obj[0]},
};

STATIC void machine_hard_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_hard_pwm_obj_t *self = self_in;
    pwm_print(print, self->pyb->pwm, false);
}

STATIC mp_obj_t machine_hard_pwm_make_new(mp_arg_val_t *args) {
    // get static peripheral object
    int pwm_id = pwm_find(args[ARG_NEW_id].u_obj);
    const machine_hard_pwm_obj_t *self = &machine_hard_pwm_obj[pwm_id];

    // check if PWM pin is set
    if (args[ARG_NEW_pin].u_obj != MP_OBJ_NULL) {
        pin_obj_t *pin_obj = args[ARG_NEW_pin].u_obj;
        self->pyb->pwm->init.pwm_pin = pin_obj->pin;
    } else {
        // TODO: raise exception.
    }

    if (args[ARG_NEW_freq].u_obj != MP_OBJ_NULL) {
        self->pyb->pwm->init.freq = mp_obj_get_int(args[ARG_NEW_freq].u_obj);
    } else {
        self->pyb->pwm->init.freq = 50; // 50 Hz by default.
    }

    if (args[ARG_NEW_period].u_obj != MP_OBJ_NULL) {
        self->pyb->pwm->init.period = mp_obj_get_int(args[ARG_NEW_period].u_obj);
    } else {
	    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                  "PWM period has to be within 16000 frequence cycles", self->pyb->pwm->init.period));
    }

    if (args[ARG_NEW_duty].u_obj != MP_OBJ_NULL) {
        self->pyb->pwm->init.duty = mp_obj_get_int(args[ARG_NEW_duty].u_obj);
    } else {
        self->pyb->pwm->init.duty = 50; // 50% by default.
    }

    if (args[ARG_NEW_pulse_width].u_obj != MP_OBJ_NULL) {
        self->pyb->pwm->init.pulse_width = mp_obj_get_int(args[ARG_NEW_pulse_width].u_obj);
    } else {
        self->pyb->pwm->init.pulse_width = 0;
    }

    if (args[ARG_NEW_mode].u_obj != MP_OBJ_NULL) {
        self->pyb->pwm->init.mode = mp_obj_get_int(args[ARG_NEW_mode].u_obj);
    } else {
        self->pyb->pwm->init.mode = HAL_PWM_MODE_HIGH_LOW;
    }

    hal_pwm_init(self->pyb->pwm->instance, &self->pyb->pwm->init);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void machine_hard_pwm_init(mp_obj_t self_in, mp_arg_val_t *args) {
	machine_hard_pwm_obj_t *self = self_in;
	pwm_init(self->pyb->pwm);
}

STATIC void machine_hard_pwm_deinit(mp_obj_t self_in) {
    machine_hard_pwm_obj_t *self = self_in;
    pwm_deinit(self->pyb->pwm);
}

STATIC mp_obj_t machine_hard_pwm_freq(mp_obj_t self_in, mp_arg_val_t *args) {
    machine_hard_pwm_obj_t *self = self_in;

    if (args[ARG_FREQ_freq].u_int != -1) {
	    self->pyb->pwm->init.freq = args[ARG_FREQ_freq].u_int;
        hal_pwm_init(self->pyb->pwm->instance, &self->pyb->pwm->init);
	} else {
        return MP_OBJ_NEW_SMALL_INT(self->pyb->pwm->init.freq);
    }

    return mp_const_none;
}


const mp_obj_type_t machine_hard_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = machine_hard_pwm_print,
    .make_new = machine_pwm_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_pwm_locals_dict,
};

#endif // MICROPY_PY_MACHINE_HW_PWM
