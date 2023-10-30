/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Lucian Copeland for Adafruit Industries
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
#include "samd/external_interrupts.h"
#include "eic_handler.h"
#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"
// #include <stdio.h>

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/alarm/__init__.h"

// This variable stores whether a PinAlarm woke in light sleep or fake deep sleep
// It CANNOT detect if the program woke from deep sleep.
STATIC volatile bool woke_up = false;
STATIC alarm_pin_pinalarm_obj_t *trig_alarm = NULL;

// Tamper Pins for deep sleep: IN0:PB00; IN1:PB02; IN2:PA02; IN3:PC00; IN4:PC01;
// wakeup from deep sleep seems to be triggered when these pins go from Hi/Lo to Hi-Z state.
typedef struct {
    int n;
    const mcu_pin_obj_t *pin;
} samd_tamper_pin_t;

static samd_tamper_pin_t TAMPER_PINS[] = {
    #if defined(PIN_PB00) && !defined(IGNORE_PIN_PB00)
    { 0, &pin_PB00 },
    #endif
    #if defined(PIN_PB02) && !defined(IGNORE_PIN_PB02)
    { 1, &pin_PB02 },
    #endif
    #if defined(PIN_PA02) && !defined(IGNORE_PIN_PA02)
    { 2, &pin_PA02 },
    #endif
    #if defined(PIN_PC00) && !defined(IGNORE_PIN_PC00)
    { 3, &pin_PC00 },
    #endif
    #if defined(PIN_PC01) && !defined(IGNORE_PIN_PC01)
    { 4, &pin_PC01 },
    #endif
    { -1, NULL }
};

void pin_alarm_callback(uint8_t num) {
    // called back with EIC/RTC interrupt
    if (!SAMD_ALARM_FLAG_PIN_CHK) {
        return;
    }
    woke_up = true;
    // SAMD_ALARM_FLAG_PIN_CLR;
    if (RTC->MODE0.INTFLAG.reg & RTC_MODE0_INTFLAG_TAMPER) {    // fake deep sleep
        RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_TAMPER;
        trig_alarm = NULL;
    } else {    // light sleep
        // Turn off sensing for the channel, or level detection will
        // be tirggered again.
        // If we clear EIC->INTENCLR flag here, cleaning up in deint
        // may fail because MCLK to EIC is stopped earlier by
        // turn_off_eic_channel.
        configure_eic_channel(num, EIC_CONFIG_SENSE0_NONE_Val);
        trig_alarm = get_eic_channel_data(num);
    }
}

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, const mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
    self->pin = pin;
    self->value = value;
    self->edge = edge;
    self->pull = pull;
}

const mcu_pin_obj_t *common_hal_alarm_pin_pinalarm_get_pin(alarm_pin_pinalarm_obj_t *self) {
    return self->pin;
}

bool common_hal_alarm_pin_pinalarm_get_value(alarm_pin_pinalarm_obj_t *self) {
    return self->value;
}

bool common_hal_alarm_pin_pinalarm_get_edge(alarm_pin_pinalarm_obj_t *self) {
    return self->edge;
}

bool common_hal_alarm_pin_pinalarm_get_pull(alarm_pin_pinalarm_obj_t *self) {
    return self->pull;
}

bool alarm_pin_pinalarm_woke_this_cycle(void) {
    return woke_up;
}

mp_obj_t alarm_pin_pinalarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);
        if (alarm == trig_alarm) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_pin_pinalarm_record_wake_alarm(uint32_t TAMPID) {
    alarm_pin_pinalarm_obj_t *const alarm = &alarm_wake_alarm.pin_alarm;

    for (samd_tamper_pin_t *t = TAMPER_PINS; t->n >= 0; t++) {
        if (TAMPID & (1 << t->n)) {
            alarm->base.type = &alarm_pin_pinalarm_type;
            alarm->pin = t->pin;
            return alarm;
        }
    }
    return mp_const_none;
}

void alarm_pin_pinalarm_reset(void) {
    SAMD_ALARM_FLAG_PIN_CLR;
    woke_up = false;
    // Disable TAMPER interrupt
    RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_TAMPER;
    if (RTC->MODE0.TAMPCTRL.reg != 0) {
        common_hal_mcu_disable_interrupts();
        RTC->MODE0.CTRLA.bit.ENABLE = 0;            // Disable the RTC
        while (RTC->MODE0.SYNCBUSY.bit.ENABLE) {    // Wait for synchronization
        }
        RTC->MODE0.TAMPCTRL.reg = 0;                // reset everything
        RTC->MODE0.CTRLA.bit.ENABLE = 1;            // Enable the RTC
        while (RTC->MODE0.SYNCBUSY.bit.ENABLE) {    // Wait for synchronization
        }
        common_hal_mcu_enable_interrupts();
    }
}

#define PINALARM_NOERR          0
#define PINALARM_ERR_NOT_FREE   -1
#define PINALARM_ERR_NOEXTINT   -2
#define PINALARM_ERR_NOCHANNEL  -3
#define PINALARM_ERR_NOTTAMPER  -4

// Because PinAlarm does not have deinit, pins are furnished just
// before the light sleep here, and deinited after wake-up.
static void pinalarm_set_alarms_light(size_t n_alarms, const mp_obj_t *alarms) {
    int err = PINALARM_NOERR;
    size_t i;
    const mcu_pin_obj_t *pin;

    for (i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);
        pin = alarm->pin;

        if (!pin_number_is_free(pin->number)) {
            err = PINALARM_ERR_NOT_FREE;
            break;
        }
        if (!pin->has_extint) {
            err = PINALARM_ERR_NOEXTINT;
            break;
        }
        if (eic_get_enable()) {
            if (!eic_channel_free(pin->extint_channel)) {
                err = PINALARM_ERR_NOCHANNEL;
                break;
            }
        } else {
            turn_on_external_interrupt_controller();
            // It is automatically turned-off in turn_off_eic_channel.
        }

        SAMD_ALARM_FLAG_PIN_SET;
        gpio_set_pin_function(pin->number, GPIO_PIN_FUNCTION_A);
        uint32_t sense;
        if (alarm->value) {
            sense = alarm->edge ? EIC_CONFIG_SENSE0_RISE_Val : EIC_CONFIG_SENSE0_HIGH_Val;
            if (alarm->pull) {
                // detect rising edge means pull down
                gpio_set_pin_pull_mode(pin->number, GPIO_PULL_DOWN);
            }
        } else {
            sense = alarm->edge ? EIC_CONFIG_SENSE0_FALL_Val : EIC_CONFIG_SENSE0_LOW_Val;
            if (alarm->pull) {
                // detect falling edge means pull up
                gpio_set_pin_pull_mode(pin->number, GPIO_PULL_UP);
            }
        }

        set_eic_channel_data(pin->extint_channel, (void *)alarm);
        claim_pin(pin);
        set_eic_handler(pin->extint_channel, EIC_HANDLER_ALARM);
        turn_on_eic_channel(pin->extint_channel, sense);
    }

    if (err == PINALARM_NOERR) {
        return;
    }

    SAMD_ALARM_FLAG_PIN_CLR;
    alarm_pin_pinalarm_deinit_alarms(i, alarms);

    switch (err) {
        case PINALARM_ERR_NOT_FREE:
            assert_pin_free(pin);
            // raise ValueError here
            MP_FALLTHROUGH
        case PINALARM_ERR_NOEXTINT:
            raise_ValueError_invalid_pin();
        case PINALARM_ERR_NOCHANNEL:
            mp_raise_RuntimeError(MP_ERROR_TEXT("A hardware interrupt channel is already in use"));
        default:
            mp_raise_RuntimeError(MP_ERROR_TEXT("Unknown reason."));
    }
}

static void pinalarm_set_alarms_deep(size_t n_alarms, const mp_obj_t *alarms) {
    // In case of deep sleep, pin_number_is_free is not checked.
    // Errata says that falling edge should be detected for the tamper pins.
    samd_tamper_pin_t *t;
    uint32_t tampctrl = 0;

    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);
        const mcu_pin_obj_t *pin = alarm->pin;

        for (t = TAMPER_PINS; t->n >= 0; t++) {
            if (pin == t->pin) {
                break;
            }
        }
        if (t->n < 0) {
            raise_ValueError_invalid_pin();
        }

        // It is strange, but to my experiment, interrupt during sleep
        // is triggered on transition from Hi/Lo to Hi-Z state, and
        // not on switching from Hi/Lo to Lo/Hi state, regardless of
        // TAMLVL value.  If tested on non-sleep condition, TAMPID
        // reacts either on falling or rising edge, according to
        // TAMLVL value as doumented.
        tampctrl |= (((1UL << t->n) << 24) |             // DEBNCn
            ((alarm->value << t->n) << 16) |             // TAMLVLn
            (1UL << t->n * 2));                          // INnACT

        // Tamper pins under the control of RTC are kept in Hi-Z
        // state, and pull mode is not effective.
        #if 0
        if (alarm->pull) {
            if (alarm->value) {
                gpio_set_pin_pull_mode(pin->number, GPIO_PULL_DOWN);
            } else {
                gpio_set_pin_pull_mode(pin->number, GPIO_PULL_UP);
            }
        }
        #endif
    }
    if (tampctrl == 0) {
        return;
    }

    SAMD_ALARM_FLAG_PIN_SET;
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_TAMPER;
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_TAMPER;
    common_hal_mcu_disable_interrupts();
    RTC->MODE0.CTRLA.bit.ENABLE = 0;                     // Disable the RTC
    while (RTC->MODE0.SYNCBUSY.bit.ENABLE) {             // Wait for synchronization
    }
    RTC->MODE0.TAMPCTRL.reg = tampctrl;
    RTC->MODE0.CTRLA.bit.ENABLE = 1;                     // Enable the RTC
    while (RTC->MODE0.SYNCBUSY.bit.ENABLE) {             // Wait for synchronization
    }
    common_hal_mcu_enable_interrupts();
}

void alarm_pin_pinalarm_deinit_alarms(size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);
        const mcu_pin_obj_t *pin = alarm->pin;

        set_eic_handler(pin->extint_channel, EIC_HANDLER_NO_INTERRUPT);
        // Set sense = 0 or INTFLAG may be set on reset_pin_number.
        configure_eic_channel(pin->extint_channel, EIC_CONFIG_SENSE0_NONE_Val);
        turn_off_eic_channel(pin->extint_channel);
        reset_pin_number(pin->number);
    }
}

void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    trig_alarm = NULL;
    if (deep_sleep) {
        pinalarm_set_alarms_deep(n_alarms, alarms);
    } else {
        pinalarm_set_alarms_light(n_alarms, alarms);
    }
}

void alarm_pin_pinalarm_prepare_for_deep_sleep(void) {
    // TODO: This function is called after the VM reset, right before
    //       the program actually enters deep sleep. If the VM reset
    //       (see port_reset) resets any pins/EVSYS settings/RTC values
    //       needed by the pinalarms, this function is responsible for setting them
    //       back up again.
}
