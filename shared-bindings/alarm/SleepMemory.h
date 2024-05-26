// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/alarm/SleepMemory.h"

extern const mp_obj_type_t alarm_sleep_memory_type;

uint32_t common_hal_alarm_sleep_memory_get_length(alarm_sleep_memory_obj_t *self);

bool common_hal_alarm_sleep_memory_set_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, const uint8_t *values, uint32_t len);
void common_hal_alarm_sleep_memory_get_bytes(alarm_sleep_memory_obj_t *self, uint32_t start_index, uint8_t *values, uint32_t len);
