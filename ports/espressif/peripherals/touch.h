// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "driver/touch_sens.h"

// Add a touch channel, creating the touch controller if needed, and (re)start
// continuous scanning. Does nothing if the channel is already initialized.
//
// Returns ESP_OK on success, or the first ESP-IDF error. On failure the channel
// is not initialized. Scanning of any existing channels is restarted if possible.
// Does not raise.
extern esp_err_t peripherals_touch_init(const int channel_id);
extern uint16_t peripherals_touch_read(int channel_id);
extern void peripherals_touch_reset(void);
extern void peripherals_touch_never_reset(const bool enable);
extern touch_sensor_handle_t peripherals_touch_get_controller(void);
extern touch_channel_handle_t peripherals_touch_get_handle(int channel_id);
