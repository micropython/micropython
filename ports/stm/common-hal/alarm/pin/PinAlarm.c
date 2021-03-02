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

STATIC void pin_alarm_callback(uint8_t num) {
    alarm_pin_triggered |= (1 << num);
    woke_up = true;
    HAL_GPIO_EXTI_IRQHandler(pin_mask(num));
}

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
    if (!edge) {
        mp_raise_NotImplementedError(translate("Only edge detection is available on this hardware"));
    }
    if (!stm_peripherals_exti_is_free(pin->number)) {
        mp_raise_RuntimeError(translate("Pin interrupt already in use"));
    }
    stm_peripherals_exti_reserve(pin->number);
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

    stm_peripherals_exti_set_callback(pin_alarm_callback,pin->number);

    self->pin = pin;
    self->value = value;
    self->pull = pull;
}

mcu_pin_obj_t *common_hal_alarm_pin_pinalarm_get_pin(alarm_pin_pinalarm_obj_t *self) {
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

bool alarm_pin_pinalarm_woke_us_up(void) {
    return woke_up;
}

mp_obj_t alarm_pin_pinalarm_get_wakeup_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (!MP_OBJ_IS_TYPE(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm  = MP_OBJ_TO_PTR(alarms[i]);
        if (alarm_pin_triggered & (1 << alarm->pin->number)) {
            return alarms[i];
        }
    }

    // Placeholder for deep sleep
    alarm_pin_pinalarm_obj_t *alarm = m_new_obj(alarm_pin_pinalarm_obj_t);
    alarm->base.type = &alarm_pin_pinalarm_type;
    alarm->pin = NULL;
    return alarm;
}

void alarm_pin_pinalarm_reset(void) {
    alarm_pin_triggered = 0;
    woke_up = false;
}

void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (MP_OBJ_IS_TYPE(alarms[i], &alarm_pin_pinalarm_type)) {
            alarm_pin_pinalarm_obj_t *alarm  = MP_OBJ_TO_PTR(alarms[i]);
            stm_peripherals_exti_enable(alarm->pin->number);
        }
    }
}

void alarm_pin_pinalarm_reset_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    alarm_pin_triggered = 0;
    for (size_t i = 0; i < n_alarms; i++) {
        if (MP_OBJ_IS_TYPE(alarms[i], &alarm_pin_pinalarm_type)) {
            alarm_pin_pinalarm_obj_t *alarm  = MP_OBJ_TO_PTR(alarms[i]);
            stm_peripherals_exti_disable(alarm->pin->number);
        }
    }
}

void alarm_pin_pinalarm_prepare_for_deep_sleep(void) {

}
