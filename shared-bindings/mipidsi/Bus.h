// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/mipidsi/Bus.h"

extern const mp_obj_type_t mipidsi_bus_type;

void common_hal_mipidsi_bus_construct(mipidsi_bus_obj_t *self, mp_uint_t minimum_frequency, uint8_t num_lanes);
void common_hal_mipidsi_bus_deinit(mipidsi_bus_obj_t *self);
bool common_hal_mipidsi_bus_deinited(mipidsi_bus_obj_t *self);
mp_uint_t common_hal_mipidsi_bus_get_frequency(mipidsi_bus_obj_t *self);
uint8_t common_hal_mipidsi_bus_get_num_lanes(mipidsi_bus_obj_t *self);
