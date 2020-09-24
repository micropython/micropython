/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include <sys/time.h>

#include "common-hal/alarm/__init__.h"
#include "shared-bindings/alarm/__init__.h"

#include "esp_sleep.h"
#include "soc/rtc_periph.h"
#include "driver/rtc_io.h"

static RTC_DATA_ATTR struct timeval sleep_enter_time;
static RTC_DATA_ATTR struct timeval sleep_exit_time;
static RTC_DATA_ATTR uint8_t wake_io;

int common_hal_alarm_get_sleep_time(void) {
    return (sleep_exit_time.tv_sec - sleep_enter_time.tv_sec) * 1000 + (sleep_exit_time.tv_usec - sleep_enter_time.tv_usec) / 1000;
}

void RTC_IRAM_ATTR esp_wake_deep_sleep(void) {
    esp_default_wake_deep_sleep();
    wake_io = rtc_gpio_get_level(6);
    gettimeofday(&sleep_exit_time, NULL);
}

mp_obj_t common_hal_alarm_get_wake_alarm(void) {
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_TIMER: ;
            //Wake up from timer.
            alarm_time_obj_t *timer = m_new_obj(alarm_time_obj_t);
            timer->base.type = &alarm_time_type;           
            return timer;
        case ESP_SLEEP_WAKEUP_EXT0: ;
            //Wake up from GPIO
            /*alarm_io_obj_t *ext0 = m_new_obj(alarm_io_obj_t);
            ext0->base.type = &alarm_io_type;  
            return ext0;*/
            return mp_obj_new_int(wake_io);
        case ESP_SLEEP_WAKEUP_EXT1: 
            //Wake up from GPIO, returns -> esp_sleep_get_ext1_wakeup_status()  
            /*uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
            if (wakeup_pin_mask != 0) {
                int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
                printf("Wake up from GPIO %d\n", pin);
            } else {
                printf("Wake up from GPIO\n");
            }*/
            break;       
        case ESP_SLEEP_WAKEUP_TOUCHPAD: 
            //TODO: implement TouchIO
            //Wake up from touch on pad, returns -> esp_sleep_get_touchpad_wakeup_status()
            break;        
        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            //Not a deep sleep reset
            break;
    }
    return mp_const_none;
}
