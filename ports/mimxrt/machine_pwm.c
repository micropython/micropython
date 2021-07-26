/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "pin.h"
#include "fsl_clock.h"
#include "fsl_iomuxc.h"
#include "fsl_pwm.h"
#include "fsl_qtmr.h"
#include "hal/pwm_backport.h"

#define PWM_MIDDLE       (0)
#define PWM_BEGIN        (1)
#define PWM_END          (2)

#define PWM_CHANNEL1     (1)
#define PWM_CHANNEL2     (2)

/******************************************************************************/
// MicroPython bindings for machine.PWM

const mp_obj_type_t machine_pwm_type;

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    PWM_Type *instance;
    bool is_flexpwm;
    uint8_t complementary;
    uint8_t module;
    uint8_t submodule;
    uint8_t channel1;
    uint8_t channel2;
    uint8_t invert;
    bool sync;
    uint32_t freq;
    uint16_t duty;
    uint16_t center;
    uint32_t deadtime;
    bool start;
    uint8_t xor;
} machine_pwm_obj_t;

static char channel_char[] = {'B', 'A', 'X' };
static char *ERRMSG_FREQ = "PWM frequency too low";
static char *ERRMSG_INIT = "PWM set-up failed";

STATIC void configure_duty_cycle(machine_pwm_obj_t *self);

STATIC void machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->is_flexpwm) {
        mp_printf(print, "<FLEXPWM module=%u submodule=%u ", self->module, self->submodule);
        if (self->complementary) {
            mp_printf(print, "channel=%c/%c", channel_char[self->channel1], channel_char[self->channel2]);
        } else {
            mp_printf(print, "channel=%c", channel_char[self->channel1]);
        }
        mp_printf(print, " freq=%u duty=%u, center=%u, deadtime=%u, sync=%u>",
            self->freq, self->duty, self->center, self->deadtime, self->sync);
    } else {
        mp_printf(print, "<QTMR_PWM module=%u channel=%u freq1=%u duty=%u>",
            self->module, self->channel1, self->freq, self->duty);
    }
}

// Utililty functions for decoding and convertings
//
STATIC uint32_t duty_ns_to_duty_u16(uint32_t freq, uint32_t duty_ns) {
    uint64_t duty = (uint64_t)duty_ns * freq * 65538ULL / 1000000000ULL;
    if (duty > 65535) {
        mp_raise_ValueError(MP_ERROR_TEXT("duty larger than period"));
    }
    return (uint32_t)duty;
}

STATIC uint8_t module_decode(char channel) {
    switch (channel) {
        case '0':
            return kPWM_Module_0;
        case '1':
            return kPWM_Module_1;
        case '2':
            return kPWM_Module_2;
        case '3':
            return kPWM_Module_3;
        default:
            return kPWM_Module_1;
    }
}

STATIC uint8_t channel_decode(char channel) {
    switch (channel) {
        case 'A':
            return kPWM_PwmA;
        case 'B':
            return kPWM_PwmB;
        case 'X':
            return kPWM_PwmX;
        default:
            return kPWM_PwmA;
    }
}

STATIC uint8_t qtmr_decode(char channel) {
    switch (channel) {
        case '0':
            return kQTMR_Channel_0;
        case '1':
            return kQTMR_Channel_1;
        case '2':
            return kQTMR_Channel_2;
        case '3':
            return kQTMR_Channel_3;
        default:
            return kPWM_Module_1;
    }
}

// decode the AF objects module and Port numer. Returns NULL if it is not a FLEXPWM object
STATIC const machine_pin_af_obj_t *af_name_decode_flexpwm(const machine_pin_af_obj_t *af_obj,
    uint8_t *module, uint8_t *submodule, uint8_t *channel) {
    const char *str;
    size_t len;
    str = (char *)qstr_data(af_obj->name, &len);
    // test for the name starting with FLEXPWM
    if (len < 15 || strncmp(str, "FLEXPWM", 7) != 0) {
        return NULL;
    }
    // Get module, submodule and channel from the name, e.g. FLEXPWM1_PWM0_A
    *module = str[7] - '0';
    *submodule = module_decode(str[12]);
    *channel = channel_decode(str[14]);

    return af_obj;
}

// decode the AF objects module and Port numer. Returns NULL if it is not a QTMR object
STATIC const machine_pin_af_obj_t *af_name_decode_qtmr(const machine_pin_af_obj_t *af_obj, uint8_t *module, uint8_t *channel) {
    const char *str;
    size_t len;
    str = (char *)qstr_data(af_obj->name, &len);
    // test for the name starting with TMR
    if (len < 11 || strncmp(str, "TMR", 3) != 0) {
        return NULL;
    }
    // Get module, submodule and channel from the name, e.g. FLEXPWM1_PWM0_A
    *module = str[3] - '0';
    *channel = qtmr_decode(str[10]);

    return af_obj;
}
//
// Functions for configuring the PWM Device
//
STATIC int calc_prescaler(uint32_t clock, uint32_t freq) {
    int prescale = 0;
    double temp = clock / (65538.0 * freq);
    for (int i = 1; i < 256; i *= 2, prescale++) {
        if (i > temp) {
            return prescale;
        }
    }
    // Frequency too low, cannot scale down.
    return -1;
}

STATIC void configure_flexpwm(machine_pwm_obj_t *self) {
    pwm_signal_param_u16_t pwmSignal;

    // Initialize PWM module.
    uint32_t pwmSourceClockInHz = CLOCK_GetFreq(kCLOCK_IpgClk);

    pwm_config_t pwmConfig;
    PWM_GetDefaultConfig(&pwmConfig);
    pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle;
    if (self->complementary) {
        pwmConfig.pairOperation = self->channel1 == kPWM_PwmA ? kPWM_ComplementaryPwmA : kPWM_ComplementaryPwmB;
    } else {
        pwmConfig.pairOperation = kPWM_Independent;
    }
    pwmConfig.clockSource = kPWM_BusClock;
    pwmConfig.enableWait = false;
    pwmConfig.initializationControl = self->sync ? kPWM_Initialize_MasterSync : kPWM_Initialize_LocalSync;
    int prescale = calc_prescaler(pwmSourceClockInHz, self->freq);
    if (prescale >= 0) {
        pwmConfig.prescale = prescale;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_FREQ));
    }

    if (PWM_Init(self->instance, self->submodule, &pwmConfig) == kStatus_Fail) {
        mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_INIT));
    }

    // disable the fault detect function to avoid using the xbara
    PWM_SetupFaultDisableMap(self->instance, self->submodule, self->channel1, kPWM_faultchannel_0, 0);
    PWM_SetupFaultDisableMap(self->instance, self->submodule, self->channel1, kPWM_faultchannel_1, 0);
    if (self->complementary) {
        PWM_SetupFaultDisableMap(self->instance, self->submodule, self->channel2, kPWM_faultchannel_0, 0);
        PWM_SetupFaultDisableMap(self->instance, self->submodule, self->channel2, kPWM_faultchannel_1, 0);
    }

    if (self->channel1 != kPWM_PwmX) {  // Only for A/B channels
        // Initialize the channel parameters
        pwmSignal.pwmChannel = self->channel1;
        pwmSignal.level = (self->invert & PWM_CHANNEL1) ? kPWM_LowTrue : kPWM_HighTrue;
        pwmSignal.dutyCycle_u16 = self->duty;
        pwmSignal.Center_u16 = self->center;
        pwmSignal.deadtimeValue = ((uint64_t)pwmSourceClockInHz * self->deadtime) / 1000000000ULL;
        PWM_SetupPwm_u16(self->instance, self->submodule, &pwmSignal, self->freq, pwmSourceClockInHz);

        if (self->complementary) {
            // Initialize the second channel of the pair.
            pwmSignal.pwmChannel = self->channel2;
            pwmSignal.level = (self->invert & PWM_CHANNEL2) ? kPWM_LowTrue : kPWM_HighTrue;
            PWM_SetupPwm_u16(self->instance, self->submodule, &pwmSignal, self->freq, pwmSourceClockInHz);
        }
        if (self->xor) {
            // Set the DBLEN and SPLIT bits for A, B = A ^ B
            self->instance->SM[self->submodule].CTRL |=
                self->xor & 2 ? (PWM_CTRL_SPLIT_MASK | PWM_CTRL_DBLEN_MASK) : PWM_CTRL_DBLEN_MASK;
        } else {
            self->instance->SM[self->submodule].CTRL &= ~(PWM_CTRL_DBLEN_MASK | PWM_CTRL_SPLIT_MASK);
        }
    } else {
        PWM_SetupPwmx_u16(self->instance, self->submodule, self->freq, self->duty,
            self->invert, pwmSourceClockInHz);
        if (self->xor) {
            // Set the DBLX bit for X = A ^ B
            self->instance->SM[self->submodule].CTRL |= PWM_CTRL_DBLX_MASK;
        } else {
            self->instance->SM[self->submodule].CTRL &= ~PWM_CTRL_DBLX_MASK;
        }
    }
    // Set the load okay bit for the submodules
    PWM_SetPwmLdok(self->instance, 1 << self->submodule, true);

    // Start the PWM generation from the Submodules
    if (self->start) {
        PWM_StartTimer(self->instance, 1 << self->submodule);
    }
}

STATIC void configure_qtmr(machine_pwm_obj_t *self) {
    qtmr_config_t qtmrConfig;
    int prescale;

    TMR_Type *instance = (TMR_Type *)self->instance;

    QTMR_GetDefaultConfig(&qtmrConfig);
    prescale = calc_prescaler(CLOCK_GetFreq(kCLOCK_IpgClk), self->freq);
    if (prescale < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_FREQ));
    }
    qtmrConfig.primarySource = prescale + kQTMR_ClockDivide_1;
    QTMR_Init(instance, self->channel1, &qtmrConfig);
    // Set up the PWM channel
    if (QTMR_SetupPwm_u16(instance, self->channel1, self->freq, self->duty,
        self->invert, CLOCK_GetFreq(kCLOCK_IpgClk) / (1 << prescale)) == kStatus_Fail) {
        mp_raise_ValueError(MP_ERROR_TEXT(ERRMSG_INIT));
    }
    // Start the output
    if (self->start) {
        QTMR_StartTimer(instance, self->channel1, kQTMR_PriSrcRiseEdge);
    }
}

STATIC void configure_pwm(machine_pwm_obj_t *self) {
    // Set the clock frequencies
    // With AhbDiv scaling, freq range is 5Hz to ~1 MHz
    // Without the AhbDiv scaling, freq range is 15Hz to ~ 3 MHz.
    static bool set_frequency = true;
    // set the frequency only once
    if (set_frequency) {
        CLOCK_SetDiv(kCLOCK_AhbDiv, 0x2); // Set AHB PODF to 2, divide by 3
        CLOCK_SetDiv(kCLOCK_IpgDiv, 0x3); // Set IPG PODF to 3, divide by 4
        set_frequency = false;
    }

    if (self->is_flexpwm) {
        configure_flexpwm(self);
    } else {
        configure_qtmr(self);
    }
}

STATIC void configure_duty_cycle(machine_pwm_obj_t *self) {
    if (self->is_flexpwm) {
        PWM_UpdatePwmDutycycle_u16(self->instance, self->submodule,
            self->channel1, self->duty, self->center);
        // Set the load okay bit for the submodules
        PWM_SetPwmLdok(self->instance, 1 << self->submodule, true);
    } else {
        configure_qtmr(self);
    }
}

// Micropython API functions
//
STATIC void machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_center, ARG_align,
           ARG_invert, ARG_sync, ARG_start, ARG_xor, ARG_deadtime };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_duty_u16, MP_ARG_INT, {.u_int = 0x10000} },
        { MP_QSTR_duty_ns, MP_ARG_INT, {.u_int = 0x10000} },
        { MP_QSTR_center, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0x10000} },
        { MP_QSTR_align, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PWM_MIDDLE}},
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
        { MP_QSTR_sync, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = false}},
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = true}},
        { MP_QSTR_xor, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
        { MP_QSTR_deadtime, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Maybe change PWM timer
    self->freq = args[ARG_freq].u_int;

    // Set duty cycle?
    uint32_t duty = args[ARG_duty_u16].u_int;
    if (duty != 0x10000) {
        self->duty = duty;
    }
    // Set duty_ns value?
    duty = args[ARG_duty_ns].u_int;
    if (duty != 0x10000) {
        self->duty = duty_ns_to_duty_u16(self->freq, duty);
    }
    // Set center value?
    uint32_t center = args[ARG_center].u_int;
    if (center != 0x10000) {
        self->center = args[ARG_center].u_int;
    } else {  // Use alignment setting shortcut
        uint8_t align = args[ARG_align].u_int & 3; // limit to 0..3
        if (align == PWM_BEGIN) {
            self->center = self->duty / 2;
        } else if (align == PWM_END) {
            self->center = 65537 - self->duty / 2;
        } else {
            self->center = 32768; // Default value: mid.
        }
    }

    self->invert = args[ARG_invert].u_int & (PWM_CHANNEL1 | PWM_CHANNEL2);
    self->sync = args[ARG_sync].u_int != false && self->submodule != 0;
    self->start = !!args[ARG_start].u_int;
    self->xor = args[ARG_xor].u_int & 0x03;
    self->deadtime = args[ARG_deadtime].u_int;

    configure_pwm(self);
}

// PWM(pin | pin-tuple, freq, [args])
STATIC mp_obj_t machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_obj_t *pins;
    const machine_pin_obj_t *pin1;
    const machine_pin_obj_t *pin2;

    // Get referred Pin object(s)
    if (mp_obj_is_type(args[0], &mp_type_tuple)) {
        mp_obj_get_array_fixed_n(args[0], 2, &pins);
        pin1 = pin_find(pins[0]);
        pin2 = pin_find(pins[1]);
    } else {
        pin1 = pin_find(args[0]);
        pin2 = NULL;
    }

    // Check whether it supports PWM and decode submodule & channel
    const machine_pin_af_obj_t *af_obj1 = NULL;
    uint8_t submodule1;
    uint8_t channel1;
    const machine_pin_af_obj_t *af_obj2 = NULL;
    uint8_t submodule2;
    uint8_t channel2;
    uint8_t module;
    bool is_flexpwm = false;

    for (int i = 0; i < pin1->af_list_len; ++i) {
        af_obj1 = af_name_decode_flexpwm(&(pin1->af_list[i]), &module, &submodule1, &channel1);
        if (af_obj1 != NULL) {
            break;
        }
    }
    if (pin2 != NULL) {
        for (int i = 0; i < pin1->af_list_len; ++i) {
            af_obj2 = af_name_decode_flexpwm(&(pin2->af_list[i]), &module, &submodule2, &channel2);
            if (af_obj2 != NULL) {
                break;
            }
        }
    }
    if (af_obj1 == NULL || (pin2 != NULL && af_obj2 == NULL)) {
        submodule1 = 0;
        // Check for QTimer support
        for (int i = 0; i < pin1->af_list_len; ++i) {
            af_obj1 = af_name_decode_qtmr(&(pin1->af_list[i]), &module, &channel1);
            if (af_obj1 != NULL) {
                break;
            }
        }
        if (af_obj1 == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("the requested Pin(s) does not support PWM"));
        }
    } else {
        // is flexpwm, check for instance match
        is_flexpwm = true;
        if (pin2 != NULL && af_obj1->instance != af_obj2->instance && submodule1 != submodule2) {
            mp_raise_ValueError(MP_ERROR_TEXT("the pins must be a A/B pair of a submodule"));
        }
    }

    // Create and populate the PWM object.
    machine_pwm_obj_t *self = m_new_obj(machine_pwm_obj_t);
    self->base.type = &machine_pwm_type;
    self->is_flexpwm = is_flexpwm;
    self->instance = af_obj1->instance;
    self->module = module;
    self->submodule = submodule1;
    self->channel1 = channel1;
    self->invert = 0;
    self->duty = 32768;
    self->center = 32768;
    // initialize the Pin(s)
    CLOCK_EnableClock(kCLOCK_Iomuxc); // just in case it was not set yet
    IOMUXC_SetPinMux(pin1->muxRegister, af_obj1->af_mode, af_obj1->input_register, af_obj1->input_daisy,
        pin1->configRegister, 0U);
    IOMUXC_SetPinConfig(pin1->muxRegister, af_obj1->af_mode, af_obj1->input_register, af_obj1->input_daisy,
        pin1->configRegister, 0x10B0U);

    if (pin2 != NULL && pin2 != pin1) {  // Settings for the second pin
        self->complementary = 1;
        self->channel2 = channel2;
        // initialize the Pin(s)
        IOMUXC_SetPinMux(pin2->muxRegister, af_obj2->af_mode, af_obj2->input_register, af_obj2->input_daisy,
            pin2->configRegister, 0U);
        IOMUXC_SetPinConfig(pin2->muxRegister, af_obj2->af_mode, af_obj2->input_register, af_obj2->input_daisy,
            pin2->configRegister, 0x10B0U);
    } else {
        self->complementary = 0;
    }
    // Process the remaining parameters
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

// PWM.init([args])
STATIC mp_obj_t machine_pwm_init(size_t n_args,
    const mp_obj_t *args, mp_map_t *kw_args) {
    machine_pwm_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pwm_init_obj, 1, machine_pwm_init);

// Disable all PWM devices. Called on soft reset
void machine_pwm_deinit() {
    static PWM_Type *const pwm_bases[] = PWM_BASE_PTRS;
    static TMR_Type *const tmr_bases[] = TMR_BASE_PTRS;

    for (int i = 1; i < ARRAY_SIZE(pwm_bases); i++) {
        PWM_StopTimer(pwm_bases[i], 0x0f); // Stop all submodules
        pwm_bases[i]->OUTEN = 0; // Disable ouput on all submodules, all channels
    }
    for (int i = 1; i < ARRAY_SIZE(tmr_bases); i++) {
        for (int j = 0; j < 4; j++) {
            QTMR_StopTimer(tmr_bases[i], j); // Stop all timers
        }
    }
}

// PWM.stop([submodules]) or PWM.deinit([submnodules])
STATIC mp_obj_t machine_pwm_stop(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (self->is_flexpwm) {
        if (n_args > 1) {
            PWM_StopTimer(self->instance, mp_obj_get_int(args[1]) & 0x0f);
        } else {
            PWM_StopTimer(self->instance, 1 << self->submodule);
        }
    } else {
        QTMR_StopTimer((TMR_Type *)self->instance, self->channel1);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_stop_obj, 1, 2, machine_pwm_stop);

// PWM.start([submodule])
STATIC mp_obj_t machine_pwm_start(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (self->is_flexpwm) {
        if (n_args > 1) {
            PWM_StartTimer(self->instance, mp_obj_get_int(args[1]) & 0x0f);
        } else {
            PWM_StartTimer(self->instance, 1 << self->submodule);
        }
    } else {
        QTMR_StartTimer((TMR_Type *)self->instance, self->channel1, kQTMR_PriSrcRiseEdge);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_start_obj, 1, 2, machine_pwm_start);

// PWM.freq([value])
STATIC mp_obj_t machine_pwm_freq(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return MP_OBJ_NEW_SMALL_INT(self->freq);
    } else {
        self->freq = mp_obj_get_int(args[1]);
        configure_pwm(self);
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_freq_obj, 1, 2, machine_pwm_freq);

// PWM.duty_u16([value])
STATIC mp_obj_t machine_pwm_duty_u16(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // Get duty cycle.
        return MP_OBJ_NEW_SMALL_INT(self->duty);
    } else {
        // Set duty cycle.
        self->duty = mp_obj_get_int(args[1]);
        configure_duty_cycle(self);
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_duty_u16_obj, 1, 2, machine_pwm_duty_u16);

// PWM.duty_ns([value])
STATIC mp_obj_t machine_pwm_duty_ns(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // Get duty cycle.
        return MP_OBJ_NEW_SMALL_INT(1000000000ULL / self->freq * self->duty / 65536);
    } else {
        // Set duty cycle.
        self->duty = duty_ns_to_duty_u16(self->freq, mp_obj_get_int(args[1]));
        configure_duty_cycle(self);
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_duty_ns_obj, 1, 2, machine_pwm_duty_ns);

// PWM.center([value])
STATIC mp_obj_t machine_pwm_center(size_t n_args, const mp_obj_t *args) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // Get Center value.
        return MP_OBJ_NEW_SMALL_INT(self->center);
    } else {
        // Set duty cycle.
        self->center = mp_obj_get_int(args[1]);
        configure_duty_cycle(self);
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pwm_center_obj, 1, 2, machine_pwm_center);

STATIC const mp_rom_map_elem_t machine_pwm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_pwm_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_pwm_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq), MP_ROM_PTR(&machine_pwm_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty_u16), MP_ROM_PTR(&machine_pwm_duty_u16_obj) },
    { MP_ROM_QSTR(MP_QSTR_duty_ns), MP_ROM_PTR(&machine_pwm_duty_ns_obj) },
    { MP_ROM_QSTR(MP_QSTR_center), MP_ROM_PTR(&machine_pwm_center_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&machine_pwm_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&machine_pwm_stop_obj) },
    // class constants
    { MP_ROM_QSTR(MP_QSTR_MIDDLE), MP_ROM_INT(PWM_MIDDLE) },
    { MP_ROM_QSTR(MP_QSTR_BEGIN), MP_ROM_INT(PWM_BEGIN) },
    { MP_ROM_QSTR(MP_QSTR_END), MP_ROM_INT(PWM_END) },
};
STATIC MP_DEFINE_CONST_DICT(machine_pwm_locals_dict, machine_pwm_locals_dict_table);

const mp_obj_type_t machine_pwm_type = {
    { &mp_type_type },
    .name = MP_QSTR_PWM,
    .print = machine_pwm_print,
    .make_new = machine_pwm_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_pwm_locals_dict,
};
