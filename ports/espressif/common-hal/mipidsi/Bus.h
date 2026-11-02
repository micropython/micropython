// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include <esp_lcd_mipi_dsi.h>
#include <esp_ldo_regulator.h>

typedef struct {
    mp_obj_base_t base;
    mp_uint_t frequency;
    esp_lcd_dsi_bus_handle_t bus_handle;
    esp_ldo_channel_handle_t phy_ldo;
    uint8_t num_data_lanes;
    uint8_t use_count; // Up to 4 displays
} mipidsi_bus_obj_t;

void mipidsi_bus_increment_use_count(mipidsi_bus_obj_t *self);
void mipidsi_bus_decrement_use_count(mipidsi_bus_obj_t *self);

// Acquire the shared MIPI PHY LDO rail (LDO channel 3, 2.5 V). DSI and CSI
// users share the rail; a shared reference count keeps it powered until the
// last user releases it, so the CSI receiver can stay active while a DSI bus
// comes and goes (and vice versa).
esp_ldo_channel_handle_t mipidsi_phy_ldo_acquire(void);
void mipidsi_phy_ldo_release(void);
