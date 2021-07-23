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

#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

// This variable stores whether a PinAlarm woke in light sleep or fake deep sleep
// It CANNOT detect if the program woke from deep sleep.
STATIC bool woke_up;

// TODO: Create tables here reserving IRQ instances, and for the IRQ
//       callback to store what pin triggered the interrupt
// STATIC bool reserved_alarms[SOME_VAL];
// STATIC uint16_t triggered_pins[SOME_VAL];

STATIC void pin_alarm_callback(uint8_t num) { // parameters can be changed
    // TODO: This is responsible for resetting the IRQ (so it doesn't
    //       go off constantly, and recording what pin was responsible for
    //       the trigger. This will only work for light sleep/fake deep
    //       sleep, in conjunction with the find_triggered_alarm function
}

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, const mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
    mp_raise_NotImplementedError(translate("PinAlarms not available"));
    // TODO: Catch edge or level mode if not supported
    // if (!edge) {
    //     mp_raise_NotImplementedError(translate("Only edge detection is available on this hardware"));
    // }

    // TODO: determine if pin has an interrupt channel available
    // TODO: set pin pull settings, input/output, etc
    // QUESTION: can PORT/EVSYS interrupts (lightsleep) coexist with RTC->TAMPER (deepsleep) settings?
    // Actual initialization of the interrupt should be delayed until set_alarm

    self->pin = pin;
    self->value = value;
    self->pull = pull;
}

const mcu_pin_obj_t *common_hal_alarm_pin_pinalarm_get_pin(alarm_pin_pinalarm_obj_t *self) {
    return self->pin;
}

bool common_hal_alarm_pin_pinalarm_get_value(alarm_pin_pinalarm_obj_t *self) {
    return self->value;
}

bool common_hal_alarm_pin_pinalarm_get_edge(alarm_pin_pinalarm_obj_t *self) {
    // TODO: is SAMD edge or level only?
    return true;
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

        // TODO: Determine whether any pins have been marked as
        //       triggering the alarm (using the static vars at
        //       start of file) and if so return that alarm.
    }
    // Return nothing if no matching alarms are found.
    return mp_const_none;
}

mp_obj_t alarm_pin_pinalarm_create_wakeup_alarm(void) {
    alarm_pin_pinalarm_obj_t *alarm = m_new_obj(alarm_pin_pinalarm_obj_t);
    alarm->base.type = &alarm_pin_pinalarm_type;
    // TODO: Extract information about what pin woke the device.
    //       Because this interrupt occurs in deep sleep, the callback
    //       cannot be used to store this information. It must be extracted
    //       from the registers, if possible. It may be impossible to
    //       obtain, in which case return a dummy value.
    return alarm;
}

void alarm_pin_pinalarm_reset(void) {
    // TODO: this is responsible for resetting ALL pinalarms. Make
    //       sure to clear any reserved tables, deinit both PORT and TAMPER
    //       settings, etc. If flags are set to indicate this module is in
    //       use, reset them.
}

void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    // The outer layer of this loop simply checks if there are any pin
    // alarms in the parameter array
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);
            if (deep_sleep) {
                // TODO: Set up deep sleep alarms.
                //       For deep sleep alarms, first check if the
                //       alarm pin value is valid for RTC->TAMPER. Ensure
                //       that the PULL and VALUE values are possible to
                //       implement with TAMPER, and throw value errors if
                //       not.
                //       A VM reset will occur before deep sleep
                //       starts, so either init these settings now and
                //       protect them with a `never_reset` function, or
                //       store them all in static variables and only
                //       actually implement the settings in
                //       `alarm_pin_pinalarm_prepare_for_deep_sleep`
                //       below.
            } // use else-if here if RTC-TAMPER can wake from IDLE

            // TODO: Set up light sleep / fake deep sleep alarms.
            //       PORT/EVSYS should have more valid pin combinations
            //       than the TAMPER system. Check if there is a valid
            //       PORT/EVSYS combination, set it up, and reserve it with
            //       the tables at the start of this file so it can't be
            //       reused. Also set up IRQ callbacks and any other
            //       busywork.

            // TODO: Might want to reserve or otherwise interact with
            //       peripherals/sam_d5x/external_interrupts.c or events.c here

            // TODO: Even if an alarm is being set for deep sleep, it
            //       still needs to be able to wake from fake deep sleep,
            //       which is actually just like a light sleep. If the
            //       RTC Tamper IRQs are capable of waking from IDLE mode,
            //       this isn't a big deal, and there can be a strict
            //       if-else statement here. Otherwise, it will need to
            //       either set PORT and TAMPER IRQs simultaniously, or if that
            //       isn't possible, make a new function that can shunt fake deep
            //       sleep setup to common_hal_alarm_pretending_deep_sleep
        }
    }
}

void alarm_pin_pinalarm_prepare_for_deep_sleep(void) {
    // TODO: This function is called after the VM reset, right before
    //       the program actually enters deep sleep. If the VM reset
    //       (see port_reset) resets any pins/EVSYS settings/RTC values
    //       needed by the pinalarms, this function is responsible for setting them
    //       back up again.
}
