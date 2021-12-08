/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Dan Halbert for Adafruit Industries
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/esp_port.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "esp_sleep.h"
#include "hal/gpio_ll.h"
#include "esp_debug_helpers.h"

#include "components/driver/include/driver/rtc_io.h"
#include "components/freertos/include/freertos/FreeRTOS.h"

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, const mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
    if (edge) {
        mp_raise_ValueError(translate("Cannot wake on pin edge. Only level."));
    }

    if (pull && !GPIO_IS_VALID_OUTPUT_GPIO(pin->number)) {
        mp_raise_ValueError(translate("Cannot pull on input-only pin."));
    }
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
    return false;
}

bool common_hal_alarm_pin_pinalarm_get_pull(alarm_pin_pinalarm_obj_t *self) {
    return self->pull;
}

gpio_isr_handle_t gpio_interrupt_handle;
// Low and high are relative to pin number. 32+ is high. <32 is low.
static volatile uint32_t pin_31_0_status = 0;
static volatile uint32_t pin_63_32_status = 0;
STATIC void gpio_interrupt(void *arg) {
    (void)arg;

    gpio_ll_get_intr_status(&GPIO, xPortGetCoreID(), (uint32_t *)&pin_31_0_status);
    gpio_ll_clear_intr_status(&GPIO, pin_31_0_status);
    gpio_ll_get_intr_status_high(&GPIO, xPortGetCoreID(), (uint32_t *)&pin_63_32_status);
    gpio_ll_clear_intr_status_high(&GPIO, pin_63_32_status);

    // disable the interrupts that fired, maybe all of them
    for (size_t i = 0; i < 32; i++) {
        uint32_t mask = 1 << i;
        if ((pin_31_0_status & mask) != 0) {
            gpio_ll_intr_disable(&GPIO, i);
        }
        if ((pin_63_32_status & mask) != 0) {
            gpio_ll_intr_disable(&GPIO, 32 + i);
        }
    }
    BaseType_t high_task_wakeup;
    vTaskNotifyGiveFromISR(circuitpython_task, &high_task_wakeup);
    if (high_task_wakeup) {
        portYIELD_FROM_ISR();
    }
}

bool alarm_pin_pinalarm_woke_this_cycle(void) {
    return pin_31_0_status != 0 || pin_63_32_status != 0;
}

mp_obj_t alarm_pin_pinalarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    uint64_t pin_status = ((uint64_t)pin_63_32_status) << 32 | pin_31_0_status;
    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);
        if ((pin_status & (1ull << alarm->pin->number)) != 0) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_pin_pinalarm_create_wakeup_alarm(void) {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    // Pin status will persist into a fake deep sleep
    uint64_t pin_status = ((uint64_t)pin_63_32_status) << 32 | pin_31_0_status;
    size_t pin_number = 64;

    if (cause == ESP_SLEEP_WAKEUP_EXT0) {
        pin_number = REG_GET_FIELD(RTC_IO_EXT_WAKEUP0_REG, RTC_IO_EXT_WAKEUP0_SEL);
    } else {
        if (cause == ESP_SLEEP_WAKEUP_EXT1) {
            pin_status = esp_sleep_get_ext1_wakeup_status();
        }
        // If the cause is GPIO, we've already snagged pin_status in the interrupt.
        // We'll only get here if we pretended to deep sleep. Light sleep will
        // pass in existing objects.
        for (size_t i = 0; i < 64; i++) {
            if ((pin_status & (1ull << i)) != 0) {
                pin_number = i;
                break;
            }
        }
    }

    alarm_pin_pinalarm_obj_t *alarm = m_new_obj(alarm_pin_pinalarm_obj_t);
    alarm->base.type = &alarm_pin_pinalarm_type;
    alarm->pin = NULL;
    // Map the pin number back to a pin object.
    for (size_t i = 0; i < mcu_pin_globals.map.used; i++) {
        const mcu_pin_obj_t *pin_obj = MP_OBJ_TO_PTR(mcu_pin_globals.map.table[i].value);
        if ((size_t)pin_obj->number == pin_number) {
            alarm->pin = mcu_pin_globals.map.table[i].value;
            break;
        }
    }
    return alarm;
}

// These must be static because we need to configure pulls later, right before
// deep sleep.
static uint64_t high_alarms = 0;
static uint64_t low_alarms = 0;
static uint64_t pull_pins = 0;

void alarm_pin_pinalarm_reset(void) {
    if (gpio_interrupt_handle != NULL) {
        esp_intr_free(gpio_interrupt_handle);
        gpio_interrupt_handle = NULL;
    }
    for (size_t i = 0; i < 64; i++) {
        uint64_t mask = 1ull << i;
        bool high = (high_alarms & mask) != 0;
        bool low = (low_alarms & mask) != 0;
        if (!(high || low)) {
            continue;
        }
        reset_pin_number(i);
    }
    high_alarms = 0;
    low_alarms = 0;
    pull_pins = 0;
    pin_63_32_status = 0;
    pin_31_0_status = 0;
}

void alarm_pin_pinalarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    // Bitmask of wake up settings.
    size_t high_count = 0;
    size_t low_count = 0;

    for (size_t i = 0; i < n_alarms; i++) {
        // TODO: Check for ULP or touch alarms because they can't coexist with GPIO alarms.
        if (!mp_obj_is_type(alarms[i], &alarm_pin_pinalarm_type)) {
            continue;
        }
        alarm_pin_pinalarm_obj_t *alarm = MP_OBJ_TO_PTR(alarms[i]);

        gpio_num_t pin_number = alarm->pin->number;
        if (alarm->value) {
            high_alarms |= 1ull << pin_number;
            high_count++;
        } else {
            low_alarms |= 1ull << pin_number;
            low_count++;
        }
        if (alarm->pull) {
            pull_pins |= 1ull << pin_number;
        }
    }
    if (high_count == 0 && low_count == 0) {
        return;
    }
    if (deep_sleep && low_count > 2 && high_count == 0) {
        mp_raise_ValueError(translate("Can only alarm on two low pins from deep sleep."));
    }
    if (deep_sleep && low_count > 1 && high_count > 0) {
        mp_raise_ValueError(translate("Can only alarm on one low pin while others alarm high from deep sleep."));
    }
    // Only use ext0 and ext1 during deep sleep.
    if (deep_sleep) {
        if (high_count > 0) {
            if (esp_sleep_enable_ext1_wakeup(high_alarms, ESP_EXT1_WAKEUP_ANY_HIGH) != ESP_OK) {
                mp_raise_ValueError(translate("Can only alarm on RTC IO from deep sleep."));
            }
        }
        size_t low_pins[2];
        size_t j = 0;
        for (size_t i = 0; i < 64; i++) {
            uint64_t mask = 1ull << i;
            if ((low_alarms & mask) != 0) {
                low_pins[j++] = i;
            }
            if (j == 2) {
                break;
            }
        }
        if (low_count > 1) {
            if (esp_sleep_enable_ext1_wakeup(1ull << low_pins[1], ESP_EXT1_WAKEUP_ALL_LOW) != ESP_OK) {
                mp_raise_ValueError(translate("Can only alarm on RTC IO from deep sleep."));
            }
        }
        if (low_count > 0) {
            if (esp_sleep_enable_ext0_wakeup(low_pins[0], 0) != ESP_OK) {
                mp_raise_ValueError(translate("Can only alarm on RTC IO from deep sleep."));
            }
        }
    } else {
        // Enable GPIO wake up if we're sleeping.
        esp_sleep_enable_gpio_wakeup();
    }
    // Set GPIO interrupts so they wake us from light sleep or from idle via the
    // interrupt handler above.
    pin_31_0_status = 0;
    pin_63_32_status = 0;
    if (gpio_isr_register(gpio_interrupt, NULL, 0, &gpio_interrupt_handle) != ESP_OK) {
        mp_raise_ValueError(translate("Can only alarm on RTC IO from deep sleep."));
    }
    for (size_t i = 0; i < 64; i++) {
        uint64_t mask = 1ull << i;
        bool high = (high_alarms & mask) != 0;
        bool low = (low_alarms & mask) != 0;
        bool pull = (pull_pins & mask) != 0;
        if (!(high || low)) {
            continue;
        }
        if (rtc_gpio_is_valid_gpio(i)) {
            rtc_gpio_deinit(i);
        }
        gpio_int_type_t interrupt_mode = GPIO_INTR_DISABLE;
        gpio_pull_mode_t pull_mode = GPIO_FLOATING;
        if (high) {
            interrupt_mode = GPIO_INTR_HIGH_LEVEL;
            pull_mode = GPIO_PULLDOWN_ONLY;
        }
        if (low) {
            interrupt_mode = GPIO_INTR_LOW_LEVEL;
            pull_mode = GPIO_PULLUP_ONLY;
        }
        gpio_set_direction(i, GPIO_MODE_DEF_INPUT);
        PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[i], PIN_FUNC_GPIO);
        if (pull) {
            gpio_set_pull_mode(i, pull_mode);
            size_t j = 0;
            while (gpio_get_level(i) == false) {
                j++;
            }
        }
        never_reset_pin_number(i);
        // Sets interrupt type and wakeup bits.
        gpio_wakeup_enable(i, interrupt_mode);
        gpio_intr_enable(i);
    }
}


void alarm_pin_pinalarm_prepare_for_deep_sleep(void) {
    if (pull_pins == 0) {
        return;
    }
    for (size_t i = 0; i < 64; i++) {
        uint64_t mask = 1ull << i;
        bool pull = (pull_pins & mask) != 0;
        if (!pull) {
            continue;
        }
        bool high = (high_alarms & mask) != 0;
        bool low = (low_alarms & mask) != 0;
        // The pull direction is opposite from alarm value.
        if (high) {
            rtc_gpio_pullup_dis(i);
            rtc_gpio_pulldown_en(i);
        }
        if (low) {
            rtc_gpio_pullup_en(i);
            rtc_gpio_pulldown_dis(i);
        }
    }
}
