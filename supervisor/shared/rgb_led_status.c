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
#include "shared-bindings/microcontroller/Pin.h"
#include "rgb_led_status.h"
#include "supervisor/shared/tick.h"

#ifdef MICROPY_HW_NEOPIXEL
uint8_t rgb_status_brightness = 63;
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/neopixel_write/__init__.h"
static uint8_t status_neopixel_color[3];
static digitalio_digitalinout_obj_t status_neopixel;
#endif


#if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
uint8_t rgb_status_brightness = 50;

#define APA102_BUFFER_LENGTH 12
static uint8_t status_apa102_color[APA102_BUFFER_LENGTH] = {0, 0, 0, 0, 0xff, 0, 0, 0, 0xff, 0xff, 0xff, 0xff};

#if CIRCUITPY_BITBANG_APA102
#include "shared-bindings/bitbangio/SPI.h"
#include "shared-module/bitbangio/types.h"
static bitbangio_spi_obj_t status_apa102 = {
    .base = {
        .type = &bitbangio_spi_type,
    },
};
#else
#include "shared-bindings/busio/SPI.h"
busio_spi_obj_t status_apa102 = {
    .base = {
        .type = &busio_spi_type,
    },
};
#endif
#endif

#if defined(CP_RGB_STATUS_R) || defined(CP_RGB_STATUS_G) || defined(CP_RGB_STATUS_B)
#define CP_RGB_STATUS_LED

#include "shared-bindings/pulseio/PWMOut.h"
#include "shared-bindings/microcontroller/Pin.h"

pulseio_pwmout_obj_t rgb_status_r = {
    .base = {
        .type = &pulseio_pwmout_type,
    },
};
pulseio_pwmout_obj_t rgb_status_g = {
    .base = {
        .type = &pulseio_pwmout_type,
    },
};
pulseio_pwmout_obj_t rgb_status_b = {
    .base = {
        .type = &pulseio_pwmout_type,
    },
};

uint8_t rgb_status_brightness = 0xFF;

uint16_t status_rgb_color[3] = {
    0 /* red */, 0 /* green */, 0 /* blue */
};
#endif

#if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || (defined(CP_RGB_STATUS_LED))
static uint32_t current_status_color = 0;
#endif

static bool rgb_led_status_init_in_progress = false;
void rgb_led_status_init() {
    if (rgb_led_status_init_in_progress) {
        // Avoid recursion.
        return;
    }
    rgb_led_status_init_in_progress = true;

    #ifdef MICROPY_HW_NEOPIXEL
        common_hal_digitalio_digitalinout_construct(&status_neopixel, MICROPY_HW_NEOPIXEL);
        // Pretend we aren't using the pins. digitalio.DigitalInOut
        // will mark them as used.
        neopixel_in_use = false;
        common_hal_digitalio_digitalinout_switch_to_output(&status_neopixel, false, DRIVE_MODE_PUSH_PULL);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        #if CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_construct(&status_apa102,
                                              MICROPY_HW_APA102_SCK,
                                              MICROPY_HW_APA102_MOSI,
                                              NULL);
        #else
        if (!common_hal_busio_spi_deinited(&status_apa102)) {
            // This may call us recursively if common_hal_reset_pin() is called,
            // The rgb_led_status_init_in_progress guard will prevent further recursion.
            common_hal_busio_spi_deinit(&status_apa102);
        }
        common_hal_busio_spi_construct(&status_apa102,
                                      MICROPY_HW_APA102_SCK,
                                      MICROPY_HW_APA102_MOSI,
                                      NULL);
        common_hal_busio_spi_never_reset(&status_apa102);
        #endif
        // Pretend we aren't using the pins. bitbangio.SPI will
        // mark them as used.
        apa102_mosi_in_use = false;
        apa102_sck_in_use = false;
        #if CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_try_lock(&status_apa102);
        // Use 1MHz for clock rate. Some APA102's are spec'd 800kHz-1200kHz,
        // though many can run much faster. bitbang will probably run slower.
        shared_module_bitbangio_spi_configure(&status_apa102, 1000000, 0, 0, 8);
        #else
        common_hal_busio_spi_try_lock(&status_apa102);
        common_hal_busio_spi_configure(&status_apa102, 1000000, 0, 0, 8);
        #endif
    #endif


    #if defined(CP_RGB_STATUS_LED)
    if (common_hal_mcu_pin_is_free(CP_RGB_STATUS_R)) {
        pwmout_result_t red_result = common_hal_pulseio_pwmout_construct(&rgb_status_r, CP_RGB_STATUS_R, 0, 50000, false);

        if (PWMOUT_OK == red_result) {
            common_hal_pulseio_pwmout_never_reset(&rgb_status_r);
        }
    }

    if (common_hal_mcu_pin_is_free(CP_RGB_STATUS_G)) {
        pwmout_result_t green_result = common_hal_pulseio_pwmout_construct(&rgb_status_g, CP_RGB_STATUS_G, 0, 50000, false);

        if (PWMOUT_OK == green_result) {
            common_hal_pulseio_pwmout_never_reset(&rgb_status_g);
        }
    }

    if (common_hal_mcu_pin_is_free(CP_RGB_STATUS_B)) {
        pwmout_result_t blue_result = common_hal_pulseio_pwmout_construct(&rgb_status_b, CP_RGB_STATUS_B, 0, 50000, false);

        if (PWMOUT_OK == blue_result) {
            common_hal_pulseio_pwmout_never_reset(&rgb_status_b);
        }
    }
    #endif

    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || (defined(CP_RGB_STATUS_LED))
    // Force a write of the current status color.
    uint32_t rgb = current_status_color;
    current_status_color = 0x1000000; // Not a valid color
    new_status_color(rgb);
    #endif

    rgb_led_status_init_in_progress = false;
}

void reset_status_led() {
    #ifdef MICROPY_HW_NEOPIXEL
        common_hal_reset_pin(MICROPY_HW_NEOPIXEL);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        common_hal_reset_pin(MICROPY_HW_APA102_MOSI);
        common_hal_reset_pin(MICROPY_HW_APA102_SCK);
    #endif
    #if defined(CP_RGB_STATUS_LED)
        // TODO: Support sharing status LED with user.
    #endif
}

void new_status_color(uint32_t rgb) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || (defined(CP_RGB_STATUS_LED))
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

        #if CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_write(&status_apa102, status_apa102_color, APA102_BUFFER_LENGTH);
        #else
        common_hal_busio_spi_write(&status_apa102, status_apa102_color, APA102_BUFFER_LENGTH);
        #endif
    #endif

    #if defined(CP_RGB_STATUS_LED)
        uint8_t red_u8 = (rgb_adjusted >> 16) & 0xFF;
        uint8_t green_u8 = (rgb_adjusted >> 8) & 0xFF;
        uint8_t blue_u8 = rgb_adjusted & 0xFF;

	#if defined(CP_RGB_STATUS_INVERTED_PWM)
        status_rgb_color[0] = (1 << 16) - 1 - ((uint16_t) (red_u8 << 8) + red_u8);
        status_rgb_color[1] = (1 << 16) - 1 - ((uint16_t) (green_u8 << 8) + green_u8);
        status_rgb_color[2] = (1 << 16) - 1 - ((uint16_t) (blue_u8 << 8) + blue_u8);
    #else
        status_rgb_color[0] = (uint16_t) (red_u8 << 8) + red_u8;
        status_rgb_color[1] = (uint16_t) (green_u8 << 8) + green_u8;
        status_rgb_color[2] = (uint16_t) (blue_u8 << 8) + blue_u8;
	#endif

        common_hal_pulseio_pwmout_set_duty_cycle(&rgb_status_r, status_rgb_color[0]);
        common_hal_pulseio_pwmout_set_duty_cycle(&rgb_status_g, status_rgb_color[1]);
        common_hal_pulseio_pwmout_set_duty_cycle(&rgb_status_b, status_rgb_color[2]);
    #endif
}

void temp_status_color(uint32_t rgb) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || (defined(CP_RGB_STATUS_LED))
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
        uint8_t colors[APA102_BUFFER_LENGTH] = {0, 0, 0, 0, 0xff, rgb_adjusted & 0xff, (rgb_adjusted >> 8) & 0xff, (rgb_adjusted >> 16) & 0xff, 0xff, 0xff, 0xff, 0xff};
        #if CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_write(&status_apa102, colors, APA102_BUFFER_LENGTH);
        #else
        common_hal_busio_spi_write(&status_apa102, colors, APA102_BUFFER_LENGTH);
        #endif
    #endif
    #if defined(CP_RGB_STATUS_LED)
        uint8_t red_u8 = (rgb_adjusted >> 16) & 0xFF;
        uint8_t green_u8 = (rgb_adjusted >> 8) & 0xFF;
        uint8_t blue_u8 = rgb_adjusted & 0xFF;

        uint16_t temp_status_color_rgb[3] = {0};

	#if defined(CP_RGB_STATUS_INVERTED_PWM)
        temp_status_color_rgb[0] = (1 << 16) - 1 - ((uint16_t) (red_u8 << 8) + red_u8);
        temp_status_color_rgb[1] = (1 << 16) - 1 - ((uint16_t) (green_u8 << 8) + green_u8);
        temp_status_color_rgb[2] = (1 << 16) - 1 - ((uint16_t) (blue_u8 << 8) + blue_u8);
	#else
        temp_status_color_rgb[0] = (uint16_t) (red_u8 << 8) + red_u8;
        temp_status_color_rgb[1] = (uint16_t) (green_u8 << 8) + green_u8;
        temp_status_color_rgb[2] = (uint16_t) (blue_u8 << 8) + blue_u8;
    #endif

        common_hal_pulseio_pwmout_set_duty_cycle(&rgb_status_r, temp_status_color_rgb[0]);
        common_hal_pulseio_pwmout_set_duty_cycle(&rgb_status_g, temp_status_color_rgb[1]);
        common_hal_pulseio_pwmout_set_duty_cycle(&rgb_status_b, temp_status_color_rgb[2]);
    #endif
}

void clear_temp_status() {
    #ifdef MICROPY_HW_NEOPIXEL
        if (neopixel_in_use) {
            return;
        }
        common_hal_neopixel_write(&status_neopixel, status_neopixel_color, 3);
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
        if (apa102_mosi_in_use || apa102_sck_in_use) {
            return;
        }
        #if CIRCUITPY_BITBANG_APA102
        shared_module_bitbangio_spi_write(&status_apa102, status_apa102_color, APA102_BUFFER_LENGTH);
        #else
        common_hal_busio_spi_write(&status_apa102, status_apa102_color, APA102_BUFFER_LENGTH);
        #endif
    #endif
    #if defined(CP_RGB_STATUS_LED)

	uint16_t red = 0;
	uint16_t green = 0;
	uint16_t blue = 0;

	#if defined(CP_RGB_STATUS_INVERTED_PWM)
		red = (1 << 16) - 1 - status_rgb_color[0];
		green = (1 << 16) - 1 - status_rgb_color[1];
		blue = (1 << 16) - 1 - status_rgb_color[2];
	#else
		red = status_rgb_color[0];
		green = status_rgb_color[1];
		blue = status_rgb_color[2];
	#endif

        common_hal_pulseio_pwmout_set_duty_cycle(&rgb_status_r, red);
        common_hal_pulseio_pwmout_set_duty_cycle(&rgb_status_g, green);
        common_hal_pulseio_pwmout_set_duty_cycle(&rgb_status_b, blue);
    #endif
}

uint32_t color_brightness(uint32_t color, uint8_t brightness) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || (defined(CP_RGB_STATUS_LED))
    uint32_t result = ((color & 0xff0000) * brightness / 255) & 0xff0000;
    result += ((color & 0xff00) * brightness / 255) & 0xff00;
    result += ((color & 0xff) * brightness / 255) & 0xff;
    return result;
    #else
    return color;
    #endif
}

void set_rgb_status_brightness(uint8_t level){
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || (defined(CP_RGB_STATUS_LED))
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
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || (defined(CP_RGB_STATUS_LED))
    new_status_color(ALL_DONE);
    status->pattern_start = supervisor_ticks_ms32();
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
    } else if (mp_obj_is_subclass_fast(result->exception_type, &mp_type_MpyError)) {
        status->exception_color = MPY_ERROR;
    } else {
        status->exception_color = OTHER_ERROR;
    }
    #endif
}

void tick_rgb_status_animation(rgb_status_animation_t* status) {
    #if defined(MICROPY_HW_NEOPIXEL) || (defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)) || (defined(CP_RGB_STATUS_LED))
    uint32_t tick_diff = supervisor_ticks_ms32() - status->pattern_start;
    if (status->ok) {
        // All is good. Ramp ALL_DONE up and down.
        if (tick_diff > ALL_GOOD_CYCLE_MS) {
            status->pattern_start = supervisor_ticks_ms32();
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
            status->pattern_start = supervisor_ticks_ms32();
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
