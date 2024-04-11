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

// This file is never compiled standalone, it's included directly from
// extmod/machine_pwm.c via MICROPY_PY_MACHINE_PWM_INCLUDEFILE.

#include <string.h>
#include "py/mphal.h"
#include "clock_config.h"
#include "sam.h"
#include "pin_af.h"

/******************************************************************************/
// MicroPython bindings for machine.PWM

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    Tcc *instance;
    bool defer_start;
    uint8_t pin_id;
    uint8_t alt_fct;
    int8_t device;
    uint8_t channel;
    uint8_t output;
    uint16_t prescaler;
    int32_t freq;    // for re-init.
} machine_pwm_obj_t;

#define PWM_NOT_INIT    (0)
#define PWM_CLK_READY   (1)
#define PWM_TCC_ENABLED (2)
#define PWM_MASTER_CLK  (get_peripheral_freq())
#define PWM_FULL_SCALE  (65536)
#define PWM_UPDATE_TIMEOUT (2000)

#define VALUE_NOT_SET    (-1)

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

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq);
static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16);
static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns);
static void mp_machine_pwm_start(machine_pwm_obj_t *self);
static void mp_machine_pwm_stop(machine_pwm_obj_t *self);


static void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PWM(%q, device=%u, channel=%u, output=%u)",
        pin_find_by_id(self->pin_id)->name, self->device, self->channel, self->output);
}

// called by the constructor and init()
static void mp_machine_pwm_init_helper(machine_pwm_obj_t *self,
    size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_freq, ARG_duty_u16, ARG_duty_ns, ARG_invert, ARG_device };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_freq,     MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_duty_u16, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_duty_ns,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_invert,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
        { MP_QSTR_device,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = VALUE_NOT_SET} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int8_t device = self->device;
    if (device == VALUE_NOT_SET) { // Device not set, just get & set
        int32_t wanted_dev = args[ARG_device].u_int; // -1 = any
        pwm_config_t config = get_pwm_config(self->pin_id, wanted_dev, device_status);
        device = config.device_channel >> 4;
        self->instance = tcc_instance[device];
        self->device = device;
        self->alt_fct = config.alt_fct;
        self->channel = (config.device_channel & 0x0f) % tcc_channel_count[device];
        self->output = config.device_channel & 0x0f;
        put_duty_value(device, self->channel, 0);
    }

    Tcc *tcc = self->instance;

    // Initialize the hardware if needed
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

    self->defer_start = true;
    if (args[ARG_invert].u_int != VALUE_NOT_SET) {
        bool invert = !!args[ARG_invert].u_int;
        if (device_status[device] != PWM_CLK_READY) {
            mp_machine_pwm_stop(self);
        }
        uint32_t mask = 1 << (self->output + TCC_DRVCTRL_INVEN0_Pos);
        if (invert) {
            tcc->DRVCTRL.reg |= mask;
        } else {
            tcc->DRVCTRL.reg &= ~(mask);
        }
    }
    if (args[ARG_freq].u_int != VALUE_NOT_SET) {
        mp_machine_pwm_freq_set(self, args[ARG_freq].u_int);
    }
    if (args[ARG_duty_u16].u_int != VALUE_NOT_SET) {
        mp_machine_pwm_duty_set_u16(self, args[ARG_duty_u16].u_int);
    }
    if (args[ARG_duty_ns].u_int != VALUE_NOT_SET) {
        mp_machine_pwm_duty_set_ns(self, args[ARG_duty_ns].u_int);
    }
    self->defer_start = false;
    // Start the PWM if properly set.
    mp_machine_pwm_start(self);
}

// PWM(pin)
static mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {

    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Get the peripheral object and populate it
    machine_pwm_obj_t *self = mp_obj_malloc(machine_pwm_obj_t, &machine_pwm_type);
    self->pin_id = mp_hal_get_pin_obj(args[0]);
    self->device = VALUE_NOT_SET;
    self->prescaler = 1;
    self->freq = VALUE_NOT_SET;

    // Process the remaining parameters.
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_pwm_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_pwm_stop(machine_pwm_obj_t *self) {
    Tcc *tcc = tcc_instance[self->device];
    tcc->CTRLA.bit.ENABLE = 0;
    while (tcc->SYNCBUSY.reg & TCC_SYNCBUSY_ENABLE) {
    }
    device_status[self->device] = PWM_CLK_READY;
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
// Calling pwm.freq(n), pwm.duty_x() or pwm.init() will start it again.
static void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    mp_hal_clr_pin_mux(self->pin_id); // Switch the output off
    output_active[self->device] &= ~(1 << self->output);  // clear output flasg
    // Stop the device, if no output is active.
    if (output_active[self->device] == 0) {
        mp_machine_pwm_stop(self);
    }
}

static void wait_for_register_update(Tcc *tcc) {
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

static void mp_machine_pwm_start(machine_pwm_obj_t *self) {
    // Start the PWM. The period counter is 24 bit or 16 bit with a pre-scaling
    // of up to 1024, allowing a range from 24 MHz down to 1 Hz.
    static const uint32_t max_period[5] = {1 << 24, 1 << 24, 1 << 16, 1 << 16, 1 << 16};

    if (self->freq < 1 || self->defer_start == true) {
        return;
    }
    Tcc *tcc = self->instance;

    // Check for the right prescaler
    uint8_t index;
    for (index = 0; index < 8; index++) {
        uint32_t temp = PWM_MASTER_CLK / prescaler_table[index] / self->freq;
        if (temp < max_period[self->device]) {
            break;
        }
    }
    self->prescaler = prescaler_table[index];

    uint32_t period = PWM_MASTER_CLK / self->prescaler / self->freq;
    if (period < 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too large"));
    }
    // If the PWM is running, ensure that a cycle has passed since the
    // previous setting before setting frequency and duty.
    if (tcc->CTRLA.reg & TCC_CTRLA_ENABLE) {
        wait_for_register_update(tcc);
    }
    // Check, if the prescaler has to be changed and stop the device if so.
    if (index != tcc->CTRLA.bit.PRESCALER) {
        mp_machine_pwm_stop(self);
        tcc->CTRLA.bit.PRESCALER = index;
    }
    // Lock the update to get a glitch-free change of period and duty cycle
    tcc->CTRLBSET.reg = TCC_CTRLBSET_LUPD;
    tcc->PERBUF.reg = period - 1;

    // (re-) configure the duty type settings.
    for (uint16_t ch = 0; ch < tcc_channel_count[self->device]; ch++) {
        if ((duty_type_flags[self->device] & (1 << ch)) != 0) {  // duty_u16 type?
            tcc->CCBUF[ch].reg = (uint64_t)get_duty_value(self->device, ch) * period /
                PWM_FULL_SCALE;
        } else { // duty_ns type
            tcc->CCBUF[ch].reg = (uint64_t)get_duty_value(self->device, ch) * PWM_MASTER_CLK /
                self->prescaler / 1000000000ULL;
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

static mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    return MP_OBJ_NEW_SMALL_INT(self->freq);
}

static void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    self->freq = freq;
    mp_machine_pwm_start(self);
}

static mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    if (duty_type_flags[self->device] & (1 << self->channel)) {
        return MP_OBJ_NEW_SMALL_INT(get_duty_value(self->device, self->channel));
    } else {
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}

static void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    put_duty_value(self->device, self->channel, duty_u16);
    duty_type_flags[self->device] |= 1 << self->channel;
    mp_machine_pwm_start(self);
}

static mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    if (!(duty_type_flags[self->device] & (1 << self->channel))) {
        return MP_OBJ_NEW_SMALL_INT(get_duty_value(self->device, self->channel));
    } else {
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
}

static void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    put_duty_value(self->device, self->channel, duty_ns);
    duty_type_flags[self->device] &= ~(1 << self->channel);
    mp_machine_pwm_start(self);
}
