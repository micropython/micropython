// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "supervisor/port.h"

#include "common-hal/alarm/__init__.h"
#include "shared-bindings/alarm/pin/PinAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "esp_sleep.h"
#include "hal/gpio_ll.h"
#include "esp_debug_helpers.h"

#ifdef SOC_PM_SUPPORT_EXT0_WAKEUP
#include "soc/rtc_cntl_reg.h"
#endif

#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"

void common_hal_alarm_pin_pinalarm_construct(alarm_pin_pinalarm_obj_t *self, const mcu_pin_obj_t *pin, bool value, bool edge, bool pull) {
    if (edge) {
        mp_raise_ValueError(MP_ERROR_TEXT("Cannot wake on pin edge. Only level."));
    }

    if (pull && !GPIO_IS_VALID_OUTPUT_GPIO(pin->number)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Cannot pull on input-only pin."));
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
static void gpio_interrupt(void *arg) {
    (void)arg;

    gpio_ll_get_intr_status(&GPIO, xPortGetCoreID(), (uint32_t *)&pin_31_0_status);
    gpio_ll_clear_intr_status(&GPIO, pin_31_0_status);
    #if SOC_GPIO_PIN_COUNT > 32
    gpio_ll_get_intr_status_high(&GPIO, xPortGetCoreID(), (uint32_t *)&pin_63_32_status);
    gpio_ll_clear_intr_status_high(&GPIO, pin_63_32_status);
    #endif

    // disable the interrupts that fired, maybe all of them
    for (size_t i = 0; i < 32; i++) {
        uint32_t mask = 1 << i;
        if ((pin_31_0_status & mask) != 0) {
            gpio_ll_intr_disable(&GPIO, i);
        }
        #if SOC_GPIO_PIN_COUNT > 32
        if ((pin_63_32_status & mask) != 0) {
            gpio_ll_intr_disable(&GPIO, 32 + i);
        }
        #endif
    }
    port_wake_main_task_from_isr();
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

mp_obj_t alarm_pin_pinalarm_record_wake_alarm(void) {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    // Pin status will persist into a fake deep sleep
    uint64_t pin_status = ((uint64_t)pin_63_32_status) << 32 | pin_31_0_status;
    size_t pin_number = 64;

    #ifdef SOC_PM_SUPPORT_EXT0_WAKEUP
    if (cause == ESP_SLEEP_WAKEUP_EXT0) {
        pin_number = REG_GET_FIELD(RTC_IO_EXT_WAKEUP0_REG, RTC_IO_EXT_WAKEUP0_SEL);
    } else {
    #endif
    #ifdef SOC_PM_SUPPORT_EXT1_WAKEUP
    if (cause == ESP_SLEEP_WAKEUP_EXT1) {
        pin_status = esp_sleep_get_ext1_wakeup_status();
    }
    #endif
    #ifdef SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
    if (cause == ESP_SLEEP_WAKEUP_GPIO) {
        pin_status = esp_sleep_get_gpio_wakeup_status();
    }
    #endif
    // If the cause is GPIO, we've already snagged pin_status in the interrupt.
    // We'll only get here if we pretended to deep sleep. Light sleep will
    // pass in existing objects.
    for (size_t i = 0; i < 64; i++) {
        if ((pin_status & (1ull << i)) != 0) {
            pin_number = i;
            break;
        }
    }
    #ifdef SOC_PM_SUPPORT_EXT0_WAKEUP
}
    #endif

    alarm_pin_pinalarm_obj_t *const alarm = &alarm_wake_alarm.pin_alarm;

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

#if defined(SOC_PM_SUPPORT_EXT1_WAKEUP) && !defined(SOC_PM_SUPPORT_EXT0_WAKEUP)
static esp_err_t _setup_ext1(size_t low_count, size_t high_count) {
    esp_err_t result;
    if (low_count > 0) {
        result = esp_sleep_enable_ext1_wakeup_io(low_alarms, ESP_EXT1_WAKEUP_ANY_LOW);
        if (result != ESP_OK) {
            return result;
        }
    }
    if (high_count > 0) {
        result = esp_sleep_enable_ext1_wakeup_io(high_alarms, ESP_EXT1_WAKEUP_ANY_HIGH);
        if (result != ESP_OK) {
            return result;
        }
    }

    #ifdef SOC_PM_SUPPORT_RTC_PERIPH_PD
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    #endif
    return ESP_OK;
}
#endif

// How to wake from deep sleep by a pin varies a lot across the ESP line and isn't hidden behind
// the IDF API. So we change our _setup_deep_sleep() implementation based on what the ESP SoC can
// do.
#ifdef CONFIG_IDF_TARGET_ESP32
static esp_err_t _setup_deep_sleep(size_t low_count, size_t high_count) {
    if (low_count > 2 && high_count == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Can only alarm on two low pins from deep sleep."));
    }
    if (low_count > 1 && high_count > 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Can only alarm on one low pin while others alarm high from deep sleep."));
    }
    esp_err_t result;
    if (high_count > 0) {
        result = esp_sleep_enable_ext1_wakeup_io(high_alarms, ESP_EXT1_WAKEUP_ANY_HIGH);
        if (result != ESP_OK) {
            return result;
        }
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
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
        if (esp_sleep_enable_ext1_wakeup_io(1ull << low_pins[1], ESP_EXT1_WAKEUP_ALL_LOW) != ESP_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("Can only alarm on RTC IO from deep sleep."));
        }
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    }
    if (low_count > 0) {
        #ifdef SOC_PM_SUPPORT_EXT0_WAKEUP
        if (esp_sleep_enable_ext0_wakeup(low_pins[0], 0) != ESP_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("Can only alarm on RTC IO from deep sleep."));
        }
        #endif
    }
    return ESP_OK;
}
#elif defined(SOC_PM_SUPPORT_EXT0_WAKEUP) // S2 and S3
static esp_err_t _setup_deep_sleep(size_t low_count, size_t high_count) {
    if (low_count > 1 && high_count > 1) {
        mp_raise_ValueError(MP_ERROR_TEXT("Can only alarm on one low pin while others alarm high from deep sleep."));
    }
    uint64_t ext1_pin_mask;
    esp_sleep_ext1_wakeup_mode_t ext1_mode;

    esp_err_t result;
    // Only use EXT0 if we need to trigger both directions.
    if (low_count > 0 && high_count > 0) {
        size_t ext0_pin_number;
        size_t level;
        if (low_count == 1) {
            ext0_pin_number = __builtin_ctzll(low_alarms);
            level = 0;
            ext1_pin_mask = high_alarms;
            ext1_mode = ESP_EXT1_WAKEUP_ANY_HIGH;
        } else {
            ext0_pin_number = __builtin_ctzll(high_alarms);
            level = 1;
            ext1_pin_mask = low_alarms;
            ext1_mode = ESP_EXT1_WAKEUP_ANY_LOW;
        }
        result = esp_sleep_enable_ext0_wakeup(ext0_pin_number, level);
        if (result != ESP_OK) {
            return result;
        }
    } else if (low_count > 0) {
        ext1_pin_mask = low_alarms;
        ext1_mode = ESP_EXT1_WAKEUP_ANY_LOW;
    } else {
        ext1_pin_mask = high_alarms;
        ext1_mode = ESP_EXT1_WAKEUP_ANY_HIGH;
    }

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    return esp_sleep_enable_ext1_wakeup_io(ext1_pin_mask, ext1_mode);
}
#elif defined(SOC_PM_SUPPORT_EXT1_WAKEUP) && defined(SOC_PM_SUPPORT_EXT1_WAKEUP_MODE_PER_PIN)
static esp_err_t _setup_deep_sleep(size_t low_count, size_t high_count) {
    return _setup_ext1(low_count, high_count);
}
#elif defined(SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP)
static esp_err_t _setup_deep_sleep(size_t low_count, size_t high_count) {
    #ifdef SOC_PM_SUPPORT_EXT1_WAKEUP
    // Don't turn on RTC GPIO if we can use EXT1.
    if (low_count == 0 || high_count == 0) {
        return _setup_ext1(low_count, high_count);
    }
    #endif
    esp_err_t result = esp_deep_sleep_enable_gpio_wakeup(low_alarms, ESP_GPIO_WAKEUP_GPIO_LOW);
    if (result != ESP_OK) {
        return result;
    }
    result = esp_deep_sleep_enable_gpio_wakeup(high_alarms, ESP_GPIO_WAKEUP_GPIO_HIGH);
    return result;
}
#else
#error "Unsupported deep sleep capabilities."
#endif
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
    // Only use ext0 and ext1 during deep sleep.
    if (deep_sleep) {
        if (_setup_deep_sleep(low_count, high_count) != ESP_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("Can only alarm on RTC IO from deep sleep."));
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
        mp_raise_ValueError(MP_ERROR_TEXT("Can only alarm on RTC IO from deep sleep."));
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
            #ifdef SOC_PM_SUPPORT_RTC_PERIPH_PD
            rtc_gpio_deinit(i);
            #endif
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
        }
        never_reset_pin_number(i);
        // Sets interrupt type and wakeup bits.
        gpio_wakeup_enable(i, interrupt_mode);
        gpio_intr_enable(i);
    }
    // Wait for any pulls to settle.
    mp_hal_delay_ms(50);
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
            #ifdef SOC_PM_SUPPORT_RTC_PERIPH_PD
            rtc_gpio_pullup_dis(i);
            rtc_gpio_pulldown_en(i);
            #else
            gpio_pullup_dis(i);
            gpio_pulldown_en(i);
            #endif
        }
        if (low) {
            #ifdef SOC_PM_SUPPORT_RTC_PERIPH_PD
            rtc_gpio_pullup_en(i);
            rtc_gpio_pulldown_dis(i);
            #else
            gpio_pullup_en(i);
            gpio_pulldown_dis(i);
            #endif
        }
    }
}
