// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "peripherals/touch.h"

static touch_sensor_handle_t touch_controller = NULL;
static touch_channel_handle_t touch_channels[TOUCH_TOTAL_CHAN_NUM] = {NULL};
static bool touch_never_reset_flag = false;
static bool touch_enabled = false;
static bool touch_scanning = false;

static int chan_index(int channel_id) {
    return channel_id - TOUCH_MIN_CHAN_ID;
}

touch_sensor_handle_t peripherals_touch_get_controller(void) {
    return touch_controller;
}

touch_channel_handle_t peripherals_touch_get_handle(int channel_id) {
    return touch_channels[chan_index(channel_id)];
}

void peripherals_touch_reset(void) {
    if (touch_controller != NULL && !touch_never_reset_flag) {
        if (touch_scanning) {
            touch_sensor_stop_continuous_scanning(touch_controller);
            touch_scanning = false;
        }
        if (touch_enabled) {
            touch_sensor_disable(touch_controller);
            touch_enabled = false;
        }
        for (unsigned int i = 0; i < TOUCH_TOTAL_CHAN_NUM; i++) {
            if (touch_channels[i] != NULL) {
                touch_sensor_del_channel(touch_channels[i]);
                touch_channels[i] = NULL;
            }
        }
        touch_sensor_del_controller(touch_controller);
        touch_controller = NULL;
    }
}

void peripherals_touch_never_reset(const bool enable) {
    touch_never_reset_flag = enable;
}

static esp_err_t first_error(esp_err_t first, esp_err_t second) {
    return first != ESP_OK ? first : second;
}

esp_err_t peripherals_touch_init(const int channel_id) {
    int idx = chan_index(channel_id);

    // Already initialized this channel
    if (touch_channels[idx] != NULL) {
        return ESP_OK;
    }

    esp_err_t err;

    // Stop scanning and disable before modifying channels
    if (touch_scanning) {
        err = touch_sensor_stop_continuous_scanning(touch_controller);
        if (err != ESP_OK) {
            return err;
        }
        touch_scanning = false;
    }
    if (touch_enabled) {
        err = touch_sensor_disable(touch_controller);
        if (err != ESP_OK) {
            return err;
        }
        touch_enabled = false;
    }

    if (touch_controller == NULL) {
        #if SOC_TOUCH_SENSOR_VERSION == 1
        touch_sensor_sample_config_t sample_cfg = TOUCH_SENSOR_V1_DEFAULT_SAMPLE_CONFIG(5.0, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_1V7);
        #elif SOC_TOUCH_SENSOR_VERSION == 2
        touch_sensor_sample_config_t sample_cfg = TOUCH_SENSOR_V2_DEFAULT_SAMPLE_CONFIG(500, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_2V2);
        #elif SOC_TOUCH_SENSOR_VERSION == 3
        touch_sensor_sample_config_t sample_cfg = TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG2(3, 29, 8, 3);
        #else
        #error bad SOC_TOUCH_SENSOR_VERSION
        #endif
        touch_sensor_config_t sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(1, &sample_cfg);
        err = touch_sensor_new_controller(&sens_cfg, &touch_controller);
        if (err != ESP_OK) {
            return err;
        }
    }

    #if SOC_TOUCH_SENSOR_VERSION == 1
    touch_channel_config_t chan_cfg = {
        .abs_active_thresh = {1000},
        .charge_speed = TOUCH_CHARGE_SPEED_7,
        .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
        .group = TOUCH_CHAN_TRIG_GROUP_BOTH,
    };
    #elif SOC_TOUCH_SENSOR_VERSION == 2
    touch_channel_config_t chan_cfg = {
        .active_thresh = {2000},
        .charge_speed = TOUCH_CHARGE_SPEED_7,
        .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
    };
    #elif SOC_TOUCH_SENSOR_VERSION == 3
    // Values are similar to an ESP-IDF example: 1000, 2500, 5000.
    touch_channel_config_t chan_cfg = {
        .active_thresh = {1000, 2000, 5000},
    };
    #else
    #error bad SOC_TOUCH_SENSOR_VERSION
    #endif

    // On failure, touch_channels[idx] is left NULL.
    esp_err_t result = touch_sensor_new_channel(touch_controller, channel_id, &chan_cfg, &touch_channels[idx]);

    // Enable and start continuous scanning, even if the new channel failed,
    // so existing channels keep working. Return the first error.
    err = touch_sensor_enable(touch_controller);
    if (err != ESP_OK) {
        return first_error(result, err);
    }
    touch_enabled = true;

    #if SOC_TOUCH_SENSOR_VERSION == 3
    // The smooth data of a new channel is invalid (reads >= 65535) until it has been
    // measured. Scan a few times first, as the ESP-IDF touch_sens_basic example does.
    for (int i = 0; i < 3; i++) {
        err = touch_sensor_trigger_oneshot_scanning(touch_controller, 100);
        if (err != ESP_OK) {
            result = first_error(result, err);
            break;
        }
    }
    #endif

    err = touch_sensor_start_continuous_scanning(touch_controller);
    if (err != ESP_OK) {
        return first_error(result, err);
    }
    touch_scanning = true;

    return result;
}

uint16_t peripherals_touch_read(int channel_id) {
    int idx = chan_index(channel_id);
    if (touch_channels[idx] == NULL) {
        return 0;
    }

    uint32_t touch_value = 0;

    #if SOC_TOUCH_SENSOR_VERSION == 1
    // ESP32 touch reads a lower value when touched.
    // Flip the values to be consistent with TouchIn assumptions.
    touch_channel_read_data(touch_channels[idx], TOUCH_CHAN_DATA_TYPE_RAW, &touch_value);
    if (touch_value > UINT16_MAX) {
        return 0;
    }
    return UINT16_MAX - (uint16_t)touch_value;

    #elif SOC_TOUCH_SENSOR_VERSION == 2
    touch_channel_read_data(touch_channels[idx], TOUCH_CHAN_DATA_TYPE_RAW, &touch_value);
    if (touch_value > UINT16_MAX) {
        return UINT16_MAX;
    }
    return (uint16_t)touch_value;

    #elif SOC_TOUCH_SENSOR_VERSION == 3
    touch_channel_read_data(touch_channels[idx], TOUCH_CHAN_DATA_TYPE_SMOOTH, &touch_value);
    if (touch_value > UINT16_MAX) {
        return UINT16_MAX;
    }
    return (uint16_t)touch_value;

    #else
    #error bad SOC_TOUCH_SENSOR_VERSION
    #endif
}
