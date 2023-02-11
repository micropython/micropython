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

#include <string.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"
#include "clock_config.h"

#include "sam.h"
#include "pin_af.h"

/******************************************************************************/
// MicroPython bindings for machine.PWM

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    Tcc *instance;
    uint8_t pin_id;
    uint8_t alt_fct;
    uint8_t device;
    uint8_t channel;
    uint8_t output;
    uint16_t prescaler;
    uint32_t period;  // full period count ticks
    uint32_t duty_ns; // just for reporting
    uint16_t duty_u16; // just for reporting
} machine_pwm_obj_t;

#define PWM_NOT_INIT    (0)
#define PWM_CLK_READY   (1)
#define PWM_TCC_ENABLED (2)
#define PWM_MASTER_CLK  (get_peripheral_freq())
#define PWM_FULL_SCALE  (65536)
#define PWM_UPDATE_TIMEOUT (2000)

static Tcc *tcc_instance[] = TCC_INSTS;

#if defined(MCU_SAMD21)

static const int tcc_gclk_id[] = {
    GCLK_CLKCTRL_ID_TCC0_TCC1, GCLK_CLKCTRL_ID_TCC0_TCC1, GCLK_CLKCTRL_ID_TCC2_TC3
};
const uint8_t tcc_channel_count[] = {4, 2, 2};
const static uint8_t tcc_channel_offset[] = {0, 4, 6};
static uint32_t pwm_duty_values[8];

#define PERBUF      PERB
#define CCBUF       CCB

#elif defined(MCU_SAMD51)

static const int tcc_gclk_id[] = {
    TCC0_GCLK_ID, TCC1_GCLK_ID, TCC2_GCLK_ID,
    #if TCC_INST_NUM > 3
    TCC3_GCLK_ID, TCC4_GCLK_ID
    #endif
};

#if TCC_INST_NUM > 3
const uint8_t tcc_channel_count[] = {6, 4, 3, 2, 2};
const static uint8_t tcc_channel_offset[] = {0, 6, 10, 13, 15};
static uint32_t pwm_duty_values[17];
#else
const uint8_t tcc_channel_count[] = {6, 4, 3};
const static uint8_t tcc_channel_offset[] = {0, 6, 10};
static uint32_t pwm_duty_values[13];
#endif // TCC_INST_NUM > 3

#endif // defined(MCU_SAMD51)

#define put_duty_value(device, channel, duty) \
    pwm_duty_values[tcc_channel_offset[device] + channel] = duty;

#define get_duty_value(device, channel) \
    pwm_duty_values[tcc_channel_offset[device] + channel]

static uint8_t duty_type_flags[TCC_INST_NUM];
static uint8_t device_status[TCC_INST_NUM];
static uint8_t output_active[TCC_INST_NUM];
const uint16_t prescaler_table[] = {1, 2, 4, 8, 16, 64, 256, 1024};

STATIC void pwm_stop_device(int device);
STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq);
STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16);
STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns);

STATIC void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(%s, device=%u, channel=%u, output=%u)",
        pin_name(self->pin_id), self->device, self->channel, self->output);
}

// PWM(pin)
STATIC mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pin, ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert, ARG_device };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin,      MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq,     MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_device,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get GPIO and optional device to connect to PWM.
    uint32_t pin_id = mp_hal_get_pin_obj(args[ARG_pin].u_obj);
    int32_t wanted_dev = args[ARG_device].u_int; // -1 = any

    // Get the peripheral object and populate it

    pwm_config_t config = get_pwm_config(pin_id, wanted_dev, device_status);
    uint8_t device = config.device_channel >> 4;
    if (device >= TCC_INST_NUM) {
        mp_raise_ValueError(MP_ERROR_TEXT("wrong device"));
    }

    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->instance = tcc_instance[device];
    self->device = device;
    self->pin_id = pin_id;
    self->alt_fct = config.alt_fct;
    self->channel = (config.device_channel & 0x0f) % tcc_channel_count[device];
    self->output = config.device_channel & 0x0f;
    self->prescaler = 1;
    self->period = 1; // Use an invalid but safe value
    self->duty_u16 = self->duty_ns = 0;
    put_duty_value(self->device, self->channel, 0);

    Tcc *tcc = self->instance;

    if (device_status[device] == PWM_NOT_INIT) {
        // Enable the device clock at first use.
        #if defined(MCU_SAMD21)
        // Enable synchronous clock. The bits are nicely arranged
        PM->APBCMASK.reg |= PM_APBCMASK_TCC0 << device;
        // Select multiplexer generic clock source and enable.
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | tcc_gclk_id[device];
        // Wait while it updates synchronously.
        while (GCLK->STATUS.bit.SYNCBUSY) {
        }
        #elif defined(MCU_SAMD51)
        // GenClk2 to the tcc
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
            #if TCC_INST_NUM > 3
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
        device_status[device] = PWM_CLK_READY;
    }

    if (args[ARG_invert].u_int != -1) {
        bool invert = !!args[ARG_invert].u_int;
        if (device_status[device] != PWM_CLK_READY) {
            pwm_stop_device(device);
        }
        uint32_t mask = 1 << (self->output + TCC_DRVCTRL_INVEN0_Pos);
        if (invert) {
            tcc->DRVCTRL.reg |= mask;
        } else {
            tcc->DRVCTRL.reg &= ~(mask);
        }
    }
    if (args[ARG_duty_u16].u_int != -1) {
        mp_machine_pwm_duty_set_u16(self, args[ARG_duty_u16].u_int);
    }
    if (args[ARG_duty_ns].u_int != -1) {
        mp_machine_pwm_duty_set_ns(self, args[ARG_duty_ns].u_int);
    }
    if (args[ARG_freq].u_int != -1) {
        mp_machine_pwm_freq_set(self, args[ARG_freq].u_int);
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC void pwm_stop_device(int device) {
    Tcc *tcc = tcc_instance[device];
    tcc->CTRLA.bit.ENABLE = 0;
    while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_ENABLE) {
    }
    device_status[device] = PWM_CLK_READY;
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
        output_active[i] = 0;
        memset(pwm_duty_values, 0, sizeof(pwm_duty_values));
    }
}

// Switch off an output. If all outputs of a device are off,
// switch off that device.
// This stops all channels, but keeps the configuration
// Calling pwm.freq(n) will start an instance again.
STATIC void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    mp_hal_clr_pin_mux(self->pin_id); // Switch the output off
    output_active[self->device] &= ~(1 << self->output);  // clear output flasg
    // Stop the device, if no output is active.
    if (output_active[self->device] == 0) {
        pwm_stop_device(self->device);
    }
}

STATIC void wait_for_register_update(Tcc *tcc) {
    // Wait for a period's end (may be long) to have the change settled
    // Each loop cycle takes at least 1 ms, giving an implicit timeout.
    for (int i = 0; i < PWM_UPDATE_TIMEOUT; i++) {
        if (tcc->INTFLAG.reg & TCC_INTFLAG_OVF) {
            break;
        }
        MICROPY_EVENT_POLL_HOOK
    }
    // Clear the flag, telling that a cycle has been handled.
    tcc->INTFLAG.reg = TCC_INTFLAG_OVF;
}

STATIC mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    if (self->instance->CTRLA.reg & TCC_CTRLA_ENABLE) {
        return MP_OBJ_NEW_SMALL_INT(PWM_MASTER_CLK / self->prescaler / self->period);
    } else {
        return MP_OBJ_NEW_SMALL_INT(0);
    }
}

STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    // Set the frequency. The period counter is 24 bit or 16 bit with a pre-scaling
    // of up to 1024, allowing a range from 24 MHz down to 1 Hz.
    static const uint32_t max_period[5] = {1 << 24, 1 << 24, 1 << 16, 1 << 16, 1 << 16};

    Tcc *tcc = self->instance;
    if (freq < 1) {
        pwm_stop_device(self->device);
        return;
    }

    // Get the actual settings of prescaler & period from the unit
    // To be able for cope for changes.
    uint32_t prev_period = tcc->PER.reg + 1;

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
    if (period < 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too large"));
    }
    // If the PWM is running, ensure that a cycle has passed since the
    // previous setting before setting a new frequency/duty value
    if (tcc->CTRLA.reg & TCC_CTRLA_ENABLE) {
        wait_for_register_update(tcc);
    }
    // Check, if the prescaler has to be changed and stop the device if so.
    if (index != tcc->CTRLA.bit.PRESCALER) {
        // stop the device
        pwm_stop_device(self->device);
        // update the prescaler
        tcc->CTRLA.bit.PRESCALER = index;
    }
    // Lock the update to get a glitch-free change of period and duty cycle
    tcc->CTRLBSET.reg = TCC_CTRLBSET_LUPD;
    tcc->PERBUF.reg = period - 1;
    self->period = period;

    // Check if the Duty rate has to be aligned again when freq or prescaler were changed.
    // This condition is as well true on first call after instantiation. So (re-)configure
    // all channels with a duty_u16 setting.
    if (period != prev_period) {
        for (uint16_t ch = 0; ch < tcc_channel_count[self->device]; ch++) {
            if ((duty_type_flags[self->device] & (1 << ch)) != 0) {  // duty_u16 type?
                tcc->CCBUF[ch].reg = (uint64_t)get_duty_value(self->device, ch) * period /
                    PWM_FULL_SCALE;
            }
        }
    }
    // If the prescaler was changed, the device is disabled. So this condition is true
    // after the instantiation and after a prescaler change.
    // (re-)configure all channels with a duty_ns setting.
    if (!(tcc->CTRLA.reg & TCC_CTRLA_ENABLE)) {
        for (uint16_t ch = 0; ch < tcc_channel_count[self->device]; ch++) {
            if ((duty_type_flags[self->device] & (1 << ch)) == 0) {  // duty_ns type?
                tcc->CCBUF[ch].reg = (uint64_t)get_duty_value(self->device, ch) * PWM_MASTER_CLK /
                    self->prescaler / 1000000000ULL;
            }
        }
    }
    // Remember the output as active.
    output_active[self->device] |= 1 << self->output; // set output flag
    // (Re-)Select PWM function for given GPIO.
    mp_hal_set_pin_mux(self->pin_id, self->alt_fct);
    // Enable the device, if required.
    if ((device_status[self->device] & PWM_TCC_ENABLED) == 0) {
        tcc->CTRLBSET.reg = TCC_CTRLBSET_CMD_UPDATE;
        tcc->CTRLA.reg |= TCC_CTRLA_ENABLE;
        while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_ENABLE) {
        }
        device_status[self->device] |= PWM_TCC_ENABLED;
    }
    // Unlock the register update, now that the settings are complete
    tcc->CTRLBCLR.reg = TCC_CTRLBCLR_LUPD;
}

STATIC mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->duty_u16);
}

STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    // Remember the values for update & reporting
    put_duty_value(self->device, self->channel, duty_u16);
    self->duty_u16 = duty_u16;
    self->duty_ns = 0;
    // If the device is enabled, than the period is set and we get a reasonable value for
    // the duty cycle, set to the CCBUF register. Otherwise, PWM does not start.
    if (self->instance->CTRLA.reg & TCC_CTRLA_ENABLE) {
        // Ensure that a cycle has passed updating the registers
        // since the previous setting before setting a new duty value
        wait_for_register_update(self->instance);
        self->instance->CCBUF[self->channel].reg = (uint64_t)duty_u16 * (self->period) / PWM_FULL_SCALE;
    }
    duty_type_flags[self->device] |= 1 << self->channel;
}

STATIC mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->duty_ns);
}

STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    // Remember the values for update & reporting
    put_duty_value(self->device, self->channel, duty_ns);
    self->duty_ns = duty_ns;
    self->duty_u16 = 0;
    // Ensure that a cycle has passed updating the registers
    // since the previous setting before setting a new duty value
    wait_for_register_update(self->instance);
    self->instance->CCBUF[self->channel].reg = (uint64_t)duty_ns * PWM_MASTER_CLK / self->prescaler / 1000000000ULL;
    duty_type_flags[self->device] &= ~(1 << self->channel);
}
