/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "shared-bindings/alarm/touch/TouchAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "peripherals/touch.h"

#include "esp_sleep.h"

void common_hal_alarm_touch_touchalarm_construct(alarm_touch_touchalarm_obj_t *self, const mcu_pin_obj_t *pin) {
    if (pin->touch_channel == TOUCH_PAD_MAX) {
        mp_raise_ValueError(translate("Invalid pin"));
    }
    self->pin = pin;
}

mp_obj_t alarm_touch_touchalarm_get_wakeup_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    // First, check to see if we match any given alarms.
    for (size_t i = 0; i < n_alarms; i++) {
        if (MP_OBJ_IS_TYPE(alarms[i], &alarm_touch_touchalarm_type)) {
            return alarms[i];
        }
    }

    gpio_num_t pin_number = esp_sleep_get_touchpad_wakeup_status();

    alarm_touch_touchalarm_obj_t *alarm = m_new_obj(alarm_touch_touchalarm_obj_t);
    alarm->base.type = &alarm_touch_touchalarm_type;
    alarm->pin = NULL;

    // Map the pin number back to a pin object.
    for (size_t i = 0; i < mcu_pin_globals.map.used; i++) {
        const mcu_pin_obj_t* pin_obj = MP_OBJ_TO_PTR(mcu_pin_globals.map.table[i].value);
        if (pin_obj->number == pin_number) {
            alarm->pin = mcu_pin_globals.map.table[i].value;
            break;
        }
    }

    return alarm;
}

static uint16_t sleep_touch_pin;

void alarm_touch_touchalarm_set_alarm(alarm_touch_touchalarm_obj_t *self) {
    sleep_touch_pin |= 1 << self->pin->number;
    esp_sleep_enable_touchpad_wakeup();
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
}

static void configure_sleep_touch_pin(touch_pad_t touch_channel) {
    // intialize touchpad
    peripherals_touch_init(touch_channel);

    // configure touchpad for sleep
    touch_pad_sleep_channel_enable(touch_channel, true);
    touch_pad_sleep_channel_enable_proximity(touch_channel, false);

    // wait for touch data to reset
    mp_hal_delay_ms(10);

    uint32_t touch_value;
    touch_pad_sleep_channel_read_smooth(touch_channel, &touch_value);
    touch_pad_sleep_set_threshold(touch_channel, touch_value * 0.1); //10%
}

void alarm_touch_touchalarm_prepare_for_deep_sleep(void) {
    for (uint8_t i = 1; i <= 14; i++) {
        if ((sleep_touch_pin & 1 << i) != 0) {
            configure_sleep_touch_pin((touch_pad_t)i);
        }
    }
}

bool alarm_touch_touchalarm_woke_us_up(void) {
    return false;
}

void alarm_touch_touchalarm_reset(void) {
    sleep_touch_pin = 0;
}
