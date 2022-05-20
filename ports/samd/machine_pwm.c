/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022 Robert Hammelrath
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

#include "sam.h"
#include "pin_cap.h"

/******************************************************************************/
// MicroPython bindings for machine.PWM

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    Tcc *instance;
    uint8_t pin_id;
    uint8_t device;
    uint8_t channel1;
    uint8_t channel2;
    uint16_t prescaler;
    uint32_t period;  // full period count ticks
    uint32_t duty;
} machine_pwm_obj_t;

#define PWM_NOT_INIT    (0)
#define PWM_INIT_CLK    (1)
#define PWM_ENABLE_TCC  (2)
#define PWM_MASTER_CLK  (48000000)
#define PWM_FULL_SCALE  (65536)

static Tcc *tcc_instance[] = TCC_INSTS;

#if defined(MCU_SAMD21)
static const int tcc_gclk_id[] = {
    GCLK_CLKCTRL_ID_TCC0_TCC1, GCLK_CLKCTRL_ID_TCC0_TCC1, GCLK_CLKCTRL_ID_TCC2_TC3
};
const uint8_t tcc_channel_count[] = {4, 2, 2};
#elif defined(MCU_SAMD51)
static const int tcc_gclk_id[] = {
    TCC0_GCLK_ID, TCC1_GCLK_ID, TCC2_GCLK_ID,
    #if defined(TCC4_GCLK_ID)
    TCC3_GCLK_ID, TCC4_GCLK_ID
    #endif
};
const uint8_t tcc_channel_count[] = {6, 4, 3, 2, 2};
#endif

static uint8_t duty_type_flags[TCC_INST_NUM];
static uint8_t device_status[TCC_INST_NUM];

STATIC void pwm_stop_device(int device);
STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq);
STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16);
STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns);

STATIC void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM P%c%02u device=%u channel=%u", "ABCD"[self->pin_id / 32], self->pin_id % 32, self->device, self->channel1);
}

// PWM(pin)
STATIC mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_device, ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,      MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_device,   MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_freq,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get GPIO and optional device to connect to PWM.
    uint32_t pin_id = mp_hal_get_pin_obj(args[ARG_pin].u_obj);
    int32_t wanted_dev = args[ARG_device].u_int; // -1 = any

    // Get the peripheral object and populate it

    pwm_config_t config = is_pwm(pin_id, wanted_dev, device_status);
    uint8_t device = config.device_channel >> 4;
    if (device >= TCC_INST_NUM) {
        mp_raise_ValueError(MP_ERROR_TEXT("wrong device"));
    }

    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->instance = tcc_instance[device];
    self->device = device;
    self->pin_id = pin_id;
    self->channel1 = config.device_channel & 0x0f;
    self->prescaler = 1;
    self->period = 1; // Use an invalid but safe value
    self->duty = 0;

    // Select PWM function for given GPIO.
    mp_hal_set_pin_mux(pin_id, config.alt_fct);
    Tcc *tcc = self->instance;

    if (device_status[device] == PWM_NOT_INIT) {
        // Enable the device clock at first use.
        #if defined(MCU_SAMD21)
        // Enable synchronous clock. The bits are nicely arranged
        PM->APBCMASK.reg |= 0x100 << device;
        // Select multiplexer generic clock source and enable.
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | tcc_gclk_id[device];
        // Wait while it updates synchronously.
        while (GCLK->STATUS.bit.SYNCBUSY) {
        }
        #elif defined(MCU_SAMD51)
        // GenClk3 to the tcc
        GCLK->PCHCTRL[tcc_gclk_id[device]].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(2);
        while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL_GCLK2) {
        }
        // Enable MCLK
        switch (device) {
            case 0:
                MCLK->APBBMASK.reg |= MCLK_APBBMASK_TCC0;
                break;
            case 1:
                MCLK->APBBMASK.reg |= MCLK_APBBMASK_TCC1;
                break;
            case 2:
                MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC2;
                break;
            #if defined(TCC4_GCLK_ID)
            case 3:
                MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC3;
                break;
            case 4:
                MCLK->APBDMASK.reg |= MCLK_APBDMASK_TCC4;
                break;
            #endif
        }
        #endif
        // Reset the device
        tcc->CTRLA.reg = TCC_CTRLA_SWRST;
        while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_SWRST) {
        }
        tcc->CTRLA.reg = TCC_CTRLA_PRESCALER_DIV1;
        tcc->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
        // Flag the clock as initialized, but not the device as enabled.
        device_status[device] = PWM_INIT_CLK;
    }

    if (args[ARG_invert].u_int != -1) {
        bool invert = !!args[ARG_invert].u_int;
        if (device_status[device] != PWM_INIT_CLK) {
            pwm_stop_device(device);
        }
        uint32_t mask = 1 << (self->channel1 + TCC_DRVCTRL_INVEN0_Pos);
        if (invert) {
            tcc->DRVCTRL.reg |= mask;
        } else {
            tcc->DRVCTRL.reg &= ~(mask);
        }
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
    return MP_OBJ_FROM_PTR(self);
}

STATIC void pwm_stop_device(int device) {
    Tcc *tcc = tcc_instance[device];
    tcc->CTRLA.bit.ENABLE = 0;
    while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_ENABLE) {
    }
    device_status[device] = PWM_INIT_CLK;
}

// Stop all TTC devices
void pwm_deinit_all(void) {
    for (int i = 0; i < TCC_INST_NUM; i++) {
        Tcc *tcc = tcc_instance[i];
        tcc->CTRLA.reg = TCC_CTRLA_SWRST;
        while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_SWRST) {
        }
        device_status[i] = PWM_NOT_INIT;
        duty_type_flags[i] = 0;
    }
}

// Switch off the device related to a channel.
// That stops all channels, but keeps the configuration
// Calling pwm.freq(n) will start them again
STATIC void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    pwm_stop_device(self->device);
}

STATIC mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(PWM_MASTER_CLK / self->prescaler / self->period);
}

STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    // Set the frequency. The period counter is 24 bit or 16 bit, allowing a range
    // from 24 MHz down to ~3 mHz (24 bit channels) or ~1 Hz (16 bit channels).
    static const uint32_t max_period[5] = {1 << 24, 1 << 24, 1 << 16, 1 << 16, 1 << 16};
    static const uint16_t prescaler_table[] = {1, 2, 4, 8, 16, 64, 256, 1024};

    Tcc *tcc = self->instance;

    // Get the actual settings of prescaler & period from the unit
    // To be able for cope for changes.
    uint32_t prev_period = tcc->PER.reg + 1;
    uint16_t prev_prescaler = prescaler_table[tcc->CTRLA.bit.PRESCALER];

    // Check for the right prescaler
    uint8_t index;
    for (index = 0; index < 8; index++) {
        uint32_t temp = PWM_MASTER_CLK / prescaler_table[index] / freq;
        if (temp < max_period[self->device]) {
            break;
        }
    }
    self->prescaler = prescaler_table[index];

    uint32_t period = PWM_MASTER_CLK / self->prescaler / freq;
    if (period >= max_period[self->device]) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too small"));
    } else if (period < 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too large"));
    }
    // check, if the prescaler has to be changed
    if (index != tcc->CTRLA.bit.PRESCALER) {
        // stop the device
        pwm_stop_device(self->device);
        // update the prescaler
        tcc->CTRLA.bit.PRESCALER = index;
    }
    tcc->PER.reg = period - 1;
    self->period = period;

    // Check if the Duty rate has to be aligned again when freq or prescaler were changed.
    if (period != prev_period) {
        for (uint16_t ch = 0, mask = 0x100; ch < tcc_channel_count[self->device]; ch++, mask <<= 1) {
            if ((duty_type_flags[self->device] & (1 << ch)) != 0) {  // duty_u16 type?
                tcc->CC[ch].reg = (uint64_t)self->duty * period / PWM_FULL_SCALE;
            }
        }
    }
    if (self->prescaler != prev_prescaler) {
        for (uint16_t ch = 0, mask = 0x100; ch < tcc_channel_count[self->device]; ch++, mask <<= 1) {
            if ((duty_type_flags[self->device] & (1 << ch)) == 0) {  // duty_ns type?
                tcc->CC[ch].reg = (uint64_t)self->duty * PWM_MASTER_CLK / self->prescaler / 1000000000ULL;
            }
        }
    }
    // Reset the count register to avoid a lng dropout on freq increase.
    tcc->COUNT.reg = 0;
    // (Re-)Start the module
    if ((device_status[self->device] & PWM_ENABLE_TCC) == 0) {
        tcc->CTRLA.reg |= TCC_CTRLA_ENABLE;
        while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_ENABLE) {
        }
        device_status[self->device] |= PWM_ENABLE_TCC;
    }
}

STATIC mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->instance->CC[self->channel1].reg * PWM_FULL_SCALE / self->period);
}

STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    self->duty = duty_u16;
    self->instance->CC[self->channel1].reg = (uint64_t)duty_u16 * self->period / PWM_FULL_SCALE;
    duty_type_flags[self->device] |= 1 << self->channel1;
}

STATIC mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(1000000000ULL * self->instance->CC[self->channel1].reg * self->prescaler / PWM_MASTER_CLK);
}

STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    self->duty = duty_ns;
    self->instance->CC[self->channel1].reg = (uint64_t)duty_ns * PWM_MASTER_CLK / self->prescaler / 1000000000ULL;
    duty_type_flags[self->device] &= ~(1 << self->channel1);
}
