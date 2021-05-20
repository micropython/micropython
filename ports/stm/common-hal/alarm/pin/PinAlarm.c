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

#include "peripherals/exti.h"

STATIC bool woke_up;

STATIC uint16_t alarm_pin_triggered;
STATIC bool deep_wkup_enabled;
STATIC bool reserved_alarms[STM32_GPIO_PORT_SIZE];

STATIC void pin_alarm_callback(uint8_t num) {
    alarm_pin_triggered |= (1 << num);
    woke_up = true;
    HAL_GPIO_EXTI_IRQHandler(pin_mask(num));
}

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, const mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
    if (!edge) {
        mp_raise_NotImplementedError(translate("Only edge detection is available on this hardware"));
    }
    if (!stm_peripherals_exti_is_free(pin->number)) {
        mp_raise_RuntimeError(translate("Pin interrupt already in use"));
    }
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(pin->number);
    GPIO_InitStruct.Mode = value ? GPIO_MODE_IT_RISING : GPIO_MODE_IT_FALLING;
    // Pull is automatically set to oppose value.
    // TODO: match digitalIO API instead?
    if (value) {
        GPIO_InitStruct.Pull = pull ? GPIO_PULLDOWN : GPIO_NOPULL;
    } else {
        GPIO_InitStruct.Pull = pull ? GPIO_PULLUP : GPIO_NOPULL;
    }
    HAL_GPIO_Init(pin_port(pin->port), &GPIO_InitStruct);

    // EXTI is set up and enabled in set_alarm
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
        if (alarm_pin_triggered & (1 << alarm->pin->number)) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_pin_pinalarm_create_wakeup_alarm(void) {
    alarm_pin_pinalarm_obj_t *alarm = m_new_obj(alarm_pin_pinalarm_obj_t);
    alarm->base.type = &alarm_pin_pinalarm_type;
    // TODO: replace this if/when other WKUP pins are supported
    alarm->pin = &pin_PA00;
    return alarm;
}

void alarm_pin_pinalarm_reset(void) {
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    alarm_pin_triggered = 0;
    woke_up = false;
    deep_wkup_enabled = false;
    for (uint8_t i = 0; i < STM32_GPIO_PORT_SIZE; i++) {
        if (reserved_alarms[i]) {
            stm_peripherals_exti_reset_exti(i);
            reserved_alarms[i] = false;
        }
    }
}

// Deep sleep alarms don't actually make use of EXTI, but we pretend they're the same.
void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);
            if (deep_sleep) {
                // Deep sleep only wakes on a rising edge from one pin, WKUP (PA00)
                // All pin settings are handled automatically.
                if (alarm->pin != &pin_PA00) {
                    mp_raise_ValueError(translate("Pin cannot wake from Deep Sleep"));
                }
                if (alarm->value == false || alarm->pull == false) {
                    // Enabling WakeUp automatically sets this, but warn anyway to set expectations
                    mp_raise_ValueError(translate("Deep sleep pins must use a rising edge with pulldown"));
                }
                // We can't actually turn WakeUp on here, since enabling it disables EXTI,
                // so we put it off until right before sleeping.
                deep_wkup_enabled = true;
                // EXTI needs to persist past the VM cleanup for fake deep sleep
                stm_peripherals_exti_never_reset(alarm->pin->number);
            }
            if (!stm_peripherals_exti_reserve(alarm->pin->number)) {
                mp_raise_RuntimeError(translate("Pin interrupt already in use"));
            }
            stm_peripherals_exti_set_callback(pin_alarm_callback,alarm->pin->number);
            stm_peripherals_exti_enable(alarm->pin->number);
            reserved_alarms[alarm->pin->number] = true;
        }
    }
}

// If we don't have WKUP enabled, ensure it's disabled
// TODO; is this really required?
void alarm_pin_pinalarm_prepare_for_deep_sleep(void) {
    if (deep_wkup_enabled) {
        HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
        HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    }
}
