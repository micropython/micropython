// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "shared/runtime/pyexec.h"
#include "supervisor/port.h"

#include "py/mpconfig.h"
#include "rgb_led_colors.h"

#include "supervisor/shared/safe_mode.h"

// Overall, the time module must be implemented.
// To work with a DotStar, one must have MICROPY_HW_APA102_SCK and
// MICROPY_HW_APA102_MOSI defined and bitbangio.SPI or busio.SPI implemented.
// To work with a NeoPixel, one must have MICROPY_HW_NEOPIXEL defined and
// neopixel_write implemented.

#define CIRCUITPY_PWM_RGB_LED (defined(CIRCUITPY_RGB_STATUS_R) || defined(CIRCUITPY_RGB_STATUS_G) || defined(CIRCUITPY_RGB_STATUS_B))
#define CIRCUITPY_STATUS_LED ((CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_STATUS)) || defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || CIRCUITPY_PWM_RGB_LED)

void status_led_init(void);
void status_led_deinit(void);
void new_status_color(uint32_t rgb);

uint32_t color_brightness(uint32_t color, uint8_t brightness);
void set_status_brightness(uint8_t level);
uint8_t get_status_brightness(void);

void init_rxtx_leds(void);
void deinit_rxtx_leds(void);
void toggle_rx_led(void);
void toggle_tx_led(void);
