// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/touch/TouchAlarm.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "esp_sleep.h"
#include "peripherals/touch.h"
#include "supervisor/port.h"

static uint16_t touch_channel_mask;
static volatile bool woke_up = false;

void common_hal_alarm_touch_touchalarm_construct(alarm_touch_touchalarm_obj_t *self, const mcu_pin_obj_t *pin) {
    if (pin->touch_channel == NO_TOUCH_CHANNEL) {
        raise_ValueError_invalid_pin();
    }
    claim_pin(pin);
    self->pin = pin;
}

// Used for light sleep.
mp_obj_t alarm_touch_touchalarm_find_triggered_alarm(const size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_touch_touchalarm_type)) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_touch_touchalarm_record_wake_alarm(void) {
    alarm_touch_touchalarm_obj_t *const alarm = &alarm_wake_alarm.touch_alarm;

    alarm->base.type = &alarm_touch_touchalarm_type;
    alarm->pin = NULL;

    // Map the pin number back to a pin object.
    for (size_t i = 0; i < mcu_pin_globals.map.used; i++) {
        const mcu_pin_obj_t *pin_obj = MP_OBJ_TO_PTR(mcu_pin_globals.map.table[i].value);
        if (pin_obj->touch_channel != NO_TOUCH_CHANNEL) {
            if ((touch_channel_mask & (1 << pin_obj->touch_channel)) != 0) {
                alarm->pin = mcu_pin_globals.map.table[i].value;
                break;
            }
        }
    }

    return alarm;
}

// This callback is used to wake the main CircuitPython task during light sleep.
static bool touch_active_callback(touch_sensor_handle_t sens_handle, const touch_active_event_data_t *event, void *user_ctx) {
    (void)sens_handle;
    (void)event;
    (void)user_ctx;
    woke_up = true;
    port_wake_main_task_from_isr();
    return false;
}

void alarm_touch_touchalarm_set_alarm(const bool deep_sleep, const size_t n_alarms, const mp_obj_t *alarms) {
    bool touch_alarm_set = false;
    alarm_touch_touchalarm_obj_t *touch_alarm = MP_OBJ_NULL;

    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_touch_touchalarm_type)) {
            if (deep_sleep && touch_alarm_set) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("Only one %q can be set in deep sleep."), MP_QSTR_TouchAlarm);
            }
            touch_alarm = MP_OBJ_TO_PTR(alarms[i]);
            touch_channel_mask |= 1 << touch_alarm->pin->touch_channel;
            // Resetting the pin will set a pull-up, which we don't want.
            skip_reset_once_pin_number(touch_alarm->pin->number);
            touch_alarm_set = true;
        }
    }

    if (!touch_alarm_set) {
        return;
    }

    // Reset touch peripheral and keep it from being reset again
    peripherals_touch_reset();
    peripherals_touch_never_reset(true);

    // Initialize all touch channels used for alarms
    for (uint8_t i = TOUCH_MIN_CHAN_ID; i <= TOUCH_MAX_CHAN_ID; i++) {
        if ((touch_channel_mask & (1 << i)) != 0) {
            peripherals_touch_init(i);
        }
    }

    // Wait for touch data to stabilize
    mp_hal_delay_ms(10);

    // Now stop scanning and disable so we can reconfigure thresholds
    touch_sensor_handle_t controller = peripherals_touch_get_controller();
    touch_sensor_stop_continuous_scanning(controller);
    touch_sensor_disable(controller);

    // Configure thresholds based on initial readings
    for (uint8_t i = TOUCH_MIN_CHAN_ID; i <= TOUCH_MAX_CHAN_ID; i++) {
        if ((touch_channel_mask & (1 << i)) == 0) {
            continue;
        }
        touch_channel_handle_t chan = peripherals_touch_get_handle(i);

        uint32_t benchmark = 0;
        #if defined(SOC_TOUCH_SUPPORT_BENCHMARK) && SOC_TOUCH_SUPPORT_BENCHMARK
        touch_channel_read_data(chan, TOUCH_CHAN_DATA_TYPE_BENCHMARK, &benchmark);
        #else
        touch_channel_read_data(chan, TOUCH_CHAN_DATA_TYPE_SMOOTH, &benchmark);
        #endif

        #if SOC_TOUCH_SENSOR_VERSION == 1
        touch_channel_config_t chan_cfg = {
            .abs_active_thresh = {(uint32_t)(benchmark / 2)},
            .charge_speed = TOUCH_CHARGE_SPEED_7,
            .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
            .group = TOUCH_CHAN_TRIG_GROUP_BOTH,
        };
        #elif SOC_TOUCH_SENSOR_VERSION == 2
        touch_channel_config_t chan_cfg = {
            .active_thresh = {(uint32_t)(benchmark / 10)},
            .charge_speed = TOUCH_CHARGE_SPEED_7,
            .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
        };
        #elif SOC_TOUCH_SENSOR_VERSION == 3
        // Values are similar to an ESP-IDF example: 1000, 2500, 5000.
        touch_channel_config_t chan_cfg = {
            .active_thresh = {(uint32_t)(benchmark / 10), (uint32_t)(benchmark / 4), (uint32_t)(benchmark / 2)},
        };
        #else
        #error bad SOC_TOUCH_SENSOR_VERSION
        #endif
        touch_sensor_reconfig_channel(chan, &chan_cfg);
    }

    // Set up filter for proper active/inactive detection
    touch_sensor_filter_config_t filter_cfg = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();
    touch_sensor_config_filter(controller, &filter_cfg);

    // Register callback for light sleep wakeup
    touch_event_callbacks_t callbacks = {
        .on_active = touch_active_callback,
    };
    touch_sensor_register_callbacks(controller, &callbacks, NULL);

    // Re-enable and start scanning
    touch_sensor_enable(controller);
    touch_sensor_start_continuous_scanning(controller);
}

void alarm_touch_touchalarm_prepare_for_deep_sleep(void) {
    if (!touch_channel_mask) {
        return;
    }

    touch_sensor_handle_t controller = peripherals_touch_get_controller();

    // Find the first alarm channel for deep sleep
    int deep_slp_chan_id = -1;
    for (uint8_t i = TOUCH_MIN_CHAN_ID; i <= TOUCH_MAX_CHAN_ID; i++) {
        if ((touch_channel_mask & (1 << i)) != 0) {
            deep_slp_chan_id = i;
            break;
        }
    }

    if (deep_slp_chan_id < 0) {
        return;
    }

    // Stop scanning and disable to reconfigure for deep sleep
    touch_sensor_stop_continuous_scanning(controller);
    touch_sensor_disable(controller);

    #if SOC_TOUCH_SUPPORT_SLEEP_WAKEUP
    touch_sleep_config_t sleep_cfg = {
        .slp_wakeup_lvl = TOUCH_DEEP_SLEEP_WAKEUP,
        #if SOC_TOUCH_SENSOR_VERSION == 1
        .deep_slp_sens_cfg = NULL,
        #else
        .deep_slp_allow_pd = false,
        .deep_slp_chan = peripherals_touch_get_handle(deep_slp_chan_id),
        .deep_slp_sens_cfg = NULL,
        #endif
    };
    touch_sensor_config_sleep_wakeup(controller, &sleep_cfg);
    #endif

    // Re-enable for sleep
    touch_sensor_enable(controller);
    touch_sensor_start_continuous_scanning(controller);

    esp_sleep_enable_touchpad_wakeup();
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
}

bool alarm_touch_touchalarm_woke_this_cycle(void) {
    return woke_up;
}

void alarm_touch_touchalarm_reset(void) {
    woke_up = false;

    // Release claimed pin(s), mirroring alarm_pin_pinalarm_reset(). Without this, the pin
    // stays marked "in use" after light_sleep_until_alarms() returns.
    if (touch_channel_mask) {
        for (size_t i = 0; i < mcu_pin_globals.map.used; i++) {
            const mcu_pin_obj_t *pin_obj = MP_OBJ_TO_PTR(mcu_pin_globals.map.table[i].value);
            if (pin_obj->touch_channel != NO_TOUCH_CHANNEL && (touch_channel_mask & (1 << pin_obj->touch_channel)) != 0) {
                reset_pin_number(pin_obj->number);
            }
        }
    }

    touch_channel_mask = 0;
    peripherals_touch_never_reset(false);
}
