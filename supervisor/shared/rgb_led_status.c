/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "mphalport.h"
#include "common-hal/microcontroller/Pin.h"
#include "rgb_led_status.h"

#ifdef MICROPY_HW_NEOPIXEL
uint8_t rgb_status_brightness = 63;
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/neopixel_write/__init__.h"
static uint8_t status_neopixel_color[3];
static digitalio_digitalinout_obj_t status_neopixel;
#endif


#if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
uint8_t rgb_status_brightness = 255;
static uint8_t status_apa102_color[12] = {0, 0, 0, 0, 0xff, 0, 0, 0};
#ifdef CIRCUITPY_BITBANG_APA102
#include "shared-bindings/bitbangio/SPI.h"
#include "shared-module/bitbangio/types.h"
static bitbangio_spi_obj_t status_apa102;
#else
#include "shared-bindings/busio/SPI.h"
busio_spi_obj_t status_apa102;
#endif
#endif

#if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
static uint32_t current_status_color = 0;
#endif

void rgb_led_status_init() {
    #ifdef MICROPY_HW_NEOPIXEL
        common_hal_digitalio_digitalinout_construct(&status_neopixel, MICROPY_HW_NEOPIXEL);
        // Pretend we aren't using the pins. digitalio.DigitalInOut
        // will mark them as used.
        neopixel_in_use = false;
        common_hal_digitalio_digitalinout_switch_to_output(&status_neopixel, false, DRIVE_MODE_PUSH_PULL);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        #ifdef CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_construct(&status_apa102,
                                              MICROPY_HW_APA102_SCK,
                                              MICROPY_HW_APA102_MOSI,
                                              mp_const_none);
        #else
        if (!common_hal_busio_spi_deinited(&status_apa102)) {
            // Don't use spi_deinit because that leads to infinite
            // recursion because reset_pin_number may call
            // rgb_led_status_init.
            spi_m_sync_disable(&status_apa102.spi_desc);
        }
        common_hal_busio_spi_construct(&status_apa102,
                                      MICROPY_HW_APA102_SCK,
                                      MICROPY_HW_APA102_MOSI,
                                      mp_const_none);
        #endif
        // Pretend we aren't using the pins. bitbangio.SPI will
        // mark them as used.
        apa102_mosi_in_use = false;
        apa102_sck_in_use = false;
        #ifdef CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_try_lock(&status_apa102);
        shared_module_bitbangio_spi_configure(&status_apa102, 100000, 0, 1, 8);
        #else
        common_hal_busio_spi_try_lock(&status_apa102);
        common_hal_busio_spi_configure(&status_apa102, 100000, 0, 1, 8);
        #endif
    #endif

    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
    // Force a write of the current status color.
    uint32_t rgb = current_status_color;
    current_status_color = 0x1000000; // Not a valid color
    new_status_color(rgb);
    #endif
}

void reset_status_led() {
    #ifdef MICROPY_HW_NEOPIXEL
        reset_pin_number(MICROPY_HW_NEOPIXEL->number);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        reset_pin_number(MICROPY_HW_APA102_MOSI->number);
        reset_pin_number(MICROPY_HW_APA102_SCK->number);
    #endif
}

void new_status_color(uint32_t rgb) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
    if (current_status_color == rgb) {
        return;
    }
    uint32_t rgb_adjusted = color_brightness(rgb, rgb_status_brightness);
    current_status_color = rgb;
    #endif

    #ifdef MICROPY_HW_NEOPIXEL
        if (neopixel_in_use) {
            return;
        }
        status_neopixel_color[0] = (rgb_adjusted >> 8) & 0xff;
        status_neopixel_color[1] = (rgb_adjusted >> 16) & 0xff;
        status_neopixel_color[2] = rgb_adjusted & 0xff;
        common_hal_neopixel_write(&status_neopixel, status_neopixel_color, 3);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        if (apa102_mosi_in_use || apa102_sck_in_use) {
            return;
        }
        status_apa102_color[5] = rgb_adjusted & 0xff;
        status_apa102_color[6] = (rgb_adjusted >> 8) & 0xff;
        status_apa102_color[7] = (rgb_adjusted >> 16) & 0xff;

        #ifdef CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_write(&status_apa102, status_apa102_color, 8);
        #else
        common_hal_busio_spi_write(&status_apa102, status_apa102_color, 8);
        #endif
    #endif
}

void temp_status_color(uint32_t rgb) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
      uint32_t rgb_adjusted = rgb;
      rgb_adjusted = color_brightness(rgb, rgb_status_brightness);
    #endif
    #ifdef MICROPY_HW_NEOPIXEL
        if (neopixel_in_use) {
            return;
        }
        uint8_t colors[3] = {(rgb_adjusted >> 8) & 0xff, (rgb_adjusted >> 16) & 0xff, rgb_adjusted & 0xff};
        common_hal_neopixel_write(&status_neopixel, colors, 3);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        if (apa102_mosi_in_use || apa102_sck_in_use) {
            return;
        }
        uint8_t colors[12] = {0, 0, 0, 0, 0xff, rgb_adjusted & 0xff, (rgb_adjusted >> 8) & 0xff, (rgb_adjusted >> 16) & 0xff, 0x0, 0x0, 0x0, 0x0};
        #ifdef CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_write(&status_apa102, colors, 12);
        #else
        common_hal_busio_spi_write(&status_apa102, colors, 12);
        #endif
    #endif
}

void clear_temp_status() {
    #ifdef MICROPY_HW_NEOPIXEL
        common_hal_neopixel_write(&status_neopixel, status_neopixel_color, 3);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        #ifdef CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_write(&status_apa102, status_apa102_color, 8);
        #else
        common_hal_busio_spi_write(&status_apa102, status_apa102_color, 8);
        #endif
    #endif
}

uint32_t color_brightness(uint32_t color, uint8_t brightness) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
    uint32_t result = ((color & 0xff0000) * brightness / 255) & 0xff0000;
    result += ((color & 0xff00) * brightness / 255) & 0xff00;
    result += ((color & 0xff) * brightness / 255) & 0xff;
    return result;
    #else
    return color;
    #endif
}

void set_rgb_status_brightness(uint8_t level){
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
    rgb_status_brightness = level;
    uint32_t current_color = current_status_color;
    // Temporarily change the current color global to force the new_status_color call to update the
    // LED. Usually duplicate calls of the same color are ignored without regard to brightness
    // changes.
    current_status_color = 0;
    new_status_color(current_color);
    #endif
}

void prep_rgb_status_animation(const pyexec_result_t* result,
                               bool found_main,
                               safe_mode_t safe_mode,
                               rgb_status_animation_t* status) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
    new_status_color(ALL_DONE);
    status->pattern_start = ticks_ms;
    status->safe_mode = safe_mode;
    status->found_main = found_main;
    status->total_exception_cycle = 0;
    status->ones = result->exception_line % 10;
    status->ones += status->ones > 0 ? 1 : 0;
    status->tens = (result->exception_line / 10) % 10;
    status->tens += status->tens > 0 ? 1 : 0;
    status->hundreds = (result->exception_line / 100) % 10;
    status->hundreds += status->hundreds > 0 ? 1 : 0;
    status->thousands = (result->exception_line / 1000) % 10;
    status->thousands += status->thousands > 0 ? 1 : 0;
    status->digit_sum = status->ones + status->tens + status->hundreds + status->thousands;
    uint8_t num_places = 0;
    uint16_t line = result->exception_line;
    for (int i = 0; i < 4; i++) {
        if ((line % 10) > 0) {
            num_places++;
        }
        line /= 10;
    }
    status->ok = result->return_code != PYEXEC_EXCEPTION;
    if (!status->ok) {
        status->total_exception_cycle = EXCEPTION_TYPE_LENGTH_MS * 3 + LINE_NUMBER_TOGGLE_LENGTH * status->digit_sum + LINE_NUMBER_TOGGLE_LENGTH * num_places;
    }
    if (mp_obj_is_subclass_fast(result->exception_type, &mp_type_IndentationError)) {
        status->exception_color = INDENTATION_ERROR;
    } else if (mp_obj_is_subclass_fast(result->exception_type, &mp_type_SyntaxError)) {
        status->exception_color = SYNTAX_ERROR;
    } else if (mp_obj_is_subclass_fast(result->exception_type, &mp_type_NameError)) {
        status->exception_color = NAME_ERROR;
    } else if (mp_obj_is_subclass_fast(result->exception_type, &mp_type_OSError)) {
        status->exception_color = OS_ERROR;
    } else if (mp_obj_is_subclass_fast(result->exception_type, &mp_type_ValueError)) {
        status->exception_color = VALUE_ERROR;
    } else {
        status->exception_color = OTHER_ERROR;
    }
    #endif
}

void tick_rgb_status_animation(rgb_status_animation_t* status) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK))
    uint32_t tick_diff = ticks_ms - status->pattern_start;
    if (status->ok) {
        // All is good. Ramp ALL_DONE up and down.
        if (tick_diff > ALL_GOOD_CYCLE_MS) {
            status->pattern_start = ticks_ms;
            tick_diff = 0;
        }

        uint16_t brightness = tick_diff * 255 / (ALL_GOOD_CYCLE_MS / 2);
        if (brightness > 255) {
            brightness = 511 - brightness;
        }
        if (status->safe_mode == NO_SAFE_MODE) {
            new_status_color(color_brightness(ALL_DONE, brightness));
        } else {
            new_status_color(color_brightness(SAFE_MODE, brightness));
        }
    } else {
        if (tick_diff > status->total_exception_cycle) {
            status->pattern_start = ticks_ms;
            tick_diff = 0;
        }
        // First flash the file color.
        if (tick_diff < EXCEPTION_TYPE_LENGTH_MS) {
            if (status->found_main) {
                new_status_color(MAIN_RUNNING);
            } else {
                new_status_color(BOOT_RUNNING);
            }
        // Next flash the exception color.
        } else if (tick_diff < EXCEPTION_TYPE_LENGTH_MS * 2) {
            new_status_color(status->exception_color);
        // Finally flash the line number digits from highest to lowest.
        // Zeroes will not produce a flash but can be read by the absence of
        // a color from the sequence.
        } else if (tick_diff < (EXCEPTION_TYPE_LENGTH_MS * 2 + LINE_NUMBER_TOGGLE_LENGTH * status->digit_sum)) {
            uint32_t digit_diff = tick_diff - EXCEPTION_TYPE_LENGTH_MS * 2;
            if ((digit_diff % LINE_NUMBER_TOGGLE_LENGTH) < (LINE_NUMBER_TOGGLE_LENGTH / 2)) {
                new_status_color(BLACK);
            } else if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * status->thousands) {
                if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH) {
                    new_status_color(BLACK);
                } else {
                    new_status_color(THOUSANDS);
                }
            } else if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (status->thousands + status->hundreds)) {
                if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (status->thousands + 1)) {
                    new_status_color(BLACK);
                } else {
                    new_status_color(HUNDREDS);
                }
            } else if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (status->thousands + status->hundreds + status->tens)) {
                if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (status->thousands + status->hundreds + 1)) {
                    new_status_color(BLACK);
                } else {
                    new_status_color(TENS);
                }
            } else {
                if (digit_diff < LINE_NUMBER_TOGGLE_LENGTH * (status->thousands + status->hundreds + status->tens + 1)) {
                    new_status_color(BLACK);
                } else {
                    new_status_color(ONES);
                }
            }
        } else {
            new_status_color(BLACK);
        }
    }
    #endif
}
