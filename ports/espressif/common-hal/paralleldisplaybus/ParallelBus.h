// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/digitalio/DigitalInOut.h"

#include "esp-idf/components/esp_lcd/include/esp_lcd_panel_io.h"

typedef struct {
    mp_obj_base_t base;
    gpio_num_t cs_pin_number;
    gpio_num_t read_pin_number;
    gpio_num_t reset_pin_number;
    esp_lcd_i80_bus_config_t config;
    esp_lcd_i80_bus_handle_t bus_handle;
    esp_lcd_panel_io_handle_t panel_io_handle;
    bool transfer_done;
} paralleldisplaybus_parallelbus_obj_t;
