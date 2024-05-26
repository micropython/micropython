// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "esp_err.h"
#include "py/mpconfig.h"
#include "py/obj.h"

#include "common-hal/espidf/__init__.h"

extern const mp_obj_type_t mp_type_espidf_IDFError;
extern const mp_obj_type_t mp_type_espidf_MemoryError;

NORETURN void mp_raise_espidf_MemoryError(void);

void raise_esp_error(esp_err_t err) NORETURN;
#define CHECK_ESP_RESULT(x) do { int res = (x); if (res != ESP_OK) raise_esp_error(res); } while (0)

size_t common_hal_espidf_get_total_psram(void);
intptr_t common_hal_espidf_get_psram_start(void);
intptr_t common_hal_espidf_get_psram_end(void);
