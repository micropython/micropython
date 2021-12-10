/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_SUPERVISOR_STATUS_LEDS_H
#define MICROPY_INCLUDED_SUPERVISOR_STATUS_LEDS_H

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

#define CIRCUITPY_PWM_RGB_LED defined(CIRCUITPY_RGB_STATUS_R) || defined(CIRCUITPY_RGB_STATUS_G) || defined(CIRCUITPY_RGB_STATUS_B)
#define CIRCUITPY_STATUS_LED (CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_STATUS)) || defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || CIRCUITPY_PWM_RGB_LED

void status_led_init(void);
void status_led_deinit(void);
void new_status_color(uint32_t rgb);

uint32_t color_brightness(uint32_t color, uint8_t brightness);
void set_status_brightness(uint8_t level);

void init_rxtx_leds(void);
void deinit_rxtx_leds(void);
void toggle_rx_led(void);
void toggle_tx_led(void);

#endif  // MICROPY_INCLUDED_SUPERVISOR_STATUS_LEDS_H
