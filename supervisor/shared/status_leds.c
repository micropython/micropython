/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2021 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/shared/status_leds.h"

#include <string.h>

#include "mphalport.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/tick.h"
#include "py/obj.h"


#ifdef CIRCUITPY_STATUS_LED_POWER
#include "shared-bindings/digitalio/DigitalInOut.h"
static digitalio_digitalinout_obj_t _status_power;
#endif

#ifdef MICROPY_HW_NEOPIXEL
uint8_t rgb_status_brightness = 63;
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/neopixel_write/__init__.h"

#ifndef MICROPY_HW_NEOPIXEL_COUNT
#define MICROPY_HW_NEOPIXEL_COUNT (1)
#endif

static uint8_t status_neopixel_color[3 * MICROPY_HW_NEOPIXEL_COUNT];
static digitalio_digitalinout_obj_t status_neopixel;

#elif defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
uint8_t rgb_status_brightness = 50;

#ifndef MICROPY_HW_APA102_COUNT
#define MICROPY_HW_APA102_COUNT (1)
#endif

    #define APA102_BUFFER_LENGTH (4 + 4 * MICROPY_HW_APA102_COUNT + 4)
static uint8_t status_apa102_color[APA102_BUFFER_LENGTH];

    #if CIRCUITPY_BITBANG_APA102
    #include "shared-bindings/bitbangio/SPI.h"
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

#elif CIRCUITPY_PWM_RGB_LED
    #include "shared-bindings/pwmio/PWMOut.h"
    #include "shared-bindings/microcontroller/Pin.h"

pwmio_pwmout_obj_t rgb_status_r = {
    .base = {
        .type = &pwmio_pwmout_type,
    },
};
pwmio_pwmout_obj_t rgb_status_g = {
    .base = {
        .type = &pwmio_pwmout_type,
    },
};
pwmio_pwmout_obj_t rgb_status_b = {
    .base = {
        .type = &pwmio_pwmout_type,
    },
};

uint8_t rgb_status_brightness = 0xFF;

uint16_t status_rgb_color[3] = {
    0 /* red */, 0 /* green */, 0     /* blue */
};
#elif CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_STATUS)
#include "shared-bindings/digitalio/DigitalInOut.h"
digitalio_digitalinout_obj_t single_color_led;

uint8_t rgb_status_brightness = 0xff;

#ifndef MICROPY_HW_LED_STATUS_INVERTED
#define MICROPY_HW_LED_STATUS_INVERTED (0)
#endif

#endif

#if CIRCUITPY_DIGITALIO && (defined(MICROPY_HW_LED_RX) || defined(MICROPY_HW_LED_TX))
#include "shared-bindings/digitalio/DigitalInOut.h"

#ifdef MICROPY_HW_LED_RX
digitalio_digitalinout_obj_t rx_led;
#endif

#ifdef MICROPY_HW_LED_TX
digitalio_digitalinout_obj_t tx_led;
#endif
#endif

#if CIRCUITPY_STATUS_LED
static uint32_t current_status_color = 0;
#endif

static bool status_led_init_in_progress = false;
void status_led_init() {
    if (status_led_init_in_progress) {
        // Avoid recursion.
        return;
    }
    status_led_init_in_progress = true;

    #ifdef CIRCUITPY_STATUS_LED_POWER
    common_hal_digitalio_digitalinout_construct(&_status_power, CIRCUITPY_STATUS_LED_POWER);
    common_hal_digitalio_digitalinout_switch_to_output(&_status_power,
        CIRCUITPY_STATUS_LED_POWER_INVERTED == 0, DRIVE_MODE_PUSH_PULL);
    #endif

    #ifdef MICROPY_HW_NEOPIXEL
    common_hal_digitalio_digitalinout_construct(&status_neopixel, MICROPY_HW_NEOPIXEL);
    common_hal_digitalio_digitalinout_switch_to_output(&status_neopixel, false, DRIVE_MODE_PUSH_PULL);
    #elif defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
    // Set every byte to 0xff except the start 4 bytes that make up the header.
    memset(status_apa102_color + 4, 0xff, APA102_BUFFER_LENGTH - 4);
    #if CIRCUITPY_BITBANG_APA102
    shared_module_bitbangio_spi_construct(&status_apa102,
        MICROPY_HW_APA102_SCK,
        MICROPY_HW_APA102_MOSI,
        NULL);
    #else
    if (!common_hal_busio_spi_deinited(&status_apa102)) {
        common_hal_busio_spi_deinit(&status_apa102);
    }
    common_hal_busio_spi_construct(&status_apa102,
        MICROPY_HW_APA102_SCK,
        MICROPY_HW_APA102_MOSI,
        NULL);
    #endif
    #if CIRCUITPY_BITBANG_APA102
    shared_module_bitbangio_spi_try_lock(&status_apa102);
    // Use 1MHz for clock rate. Some APA102's are spec'd 800kHz-1200kHz,
    // though many can run much faster. bitbang will probably run slower.
    shared_module_bitbangio_spi_configure(&status_apa102, 1000000, 0, 0, 8);
    #else
    common_hal_busio_spi_try_lock(&status_apa102);
    common_hal_busio_spi_configure(&status_apa102, 1000000, 0, 0, 8);
    #endif


    #elif CIRCUITPY_PWM_RGB_LED
    if (common_hal_mcu_pin_is_free(CIRCUITPY_RGB_STATUS_R)) {
        pwmout_result_t red_result = common_hal_pwmio_pwmout_construct(&rgb_status_r, CIRCUITPY_RGB_STATUS_R, 0, 50000, false);

        if (PWMOUT_OK == red_result) {
            common_hal_pwmio_pwmout_never_reset(&rgb_status_r);
        }
    }

    if (common_hal_mcu_pin_is_free(CIRCUITPY_RGB_STATUS_G)) {
        pwmout_result_t green_result = common_hal_pwmio_pwmout_construct(&rgb_status_g, CIRCUITPY_RGB_STATUS_G, 0, 50000, false);

        if (PWMOUT_OK == green_result) {
            common_hal_pwmio_pwmout_never_reset(&rgb_status_g);
        }
    }

    if (common_hal_mcu_pin_is_free(CIRCUITPY_RGB_STATUS_B)) {
        pwmout_result_t blue_result = common_hal_pwmio_pwmout_construct(&rgb_status_b, CIRCUITPY_RGB_STATUS_B, 0, 50000, false);

        if (PWMOUT_OK == blue_result) {
            common_hal_pwmio_pwmout_never_reset(&rgb_status_b);
        }
    }

    #elif defined(MICROPY_HW_LED_STATUS)
    common_hal_digitalio_digitalinout_construct(&single_color_led, MICROPY_HW_LED_STATUS);
    common_hal_digitalio_digitalinout_switch_to_output(
        &single_color_led, MICROPY_HW_LED_STATUS_INVERTED == 0, DRIVE_MODE_PUSH_PULL);
    #endif

    #if CIRCUITPY_DIGITALIO && CIRCUITPY_STATUS_LED
    // Force a write of the current status color.
    uint32_t rgb = current_status_color;
    current_status_color = 0x1000000; // Not a valid color
    new_status_color(rgb);
    #endif

    status_led_init_in_progress = false;
}

void status_led_deinit() {
    #ifdef MICROPY_HW_NEOPIXEL
    common_hal_reset_pin(MICROPY_HW_NEOPIXEL);

    #elif defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
    #if CIRCUITPY_BITBANG_APA102
    shared_module_bitbangio_spi_deinit(&status_apa102);
    #else
    common_hal_busio_spi_deinit(&status_apa102);
    #endif

    #elif CIRCUITPY_PWM_RGB_LED
    if (!common_hal_mcu_pin_is_free(CIRCUITPY_RGB_STATUS_R)) {
        common_hal_pwmio_pwmout_deinit(&rgb_status_r);
    }

    if (!common_hal_mcu_pin_is_free(CIRCUITPY_RGB_STATUS_G)) {
        common_hal_pwmio_pwmout_deinit(&rgb_status_g);
    }

    if (!common_hal_mcu_pin_is_free(CIRCUITPY_RGB_STATUS_B)) {
        common_hal_pwmio_pwmout_deinit(&rgb_status_b);
    }

    #elif defined(MICROPY_HW_LED_STATUS)
    common_hal_digitalio_digitalinout_deinit(&single_color_led);
    #endif

    #ifdef CIRCUITPY_STATUS_LED_POWER
    common_hal_digitalio_digitalinout_deinit(&_status_power);
    #endif
}

void new_status_color(uint32_t rgb) {
    #if CIRCUITPY_STATUS_LED
    if (current_status_color == rgb) {
        return;
    }
    uint32_t rgb_adjusted = color_brightness(rgb, rgb_status_brightness);
    current_status_color = rgb;
    #endif

    #ifdef MICROPY_HW_NEOPIXEL
    for (size_t i = 0; i < MICROPY_HW_NEOPIXEL_COUNT; i++) {
        status_neopixel_color[3 * i + 0] = (rgb_adjusted >> 8) & 0xff;
        status_neopixel_color[3 * i + 1] = (rgb_adjusted >> 16) & 0xff;
        status_neopixel_color[3 * i + 2] = rgb_adjusted & 0xff;
    }
    common_hal_neopixel_write(&status_neopixel, status_neopixel_color, 3 * MICROPY_HW_NEOPIXEL_COUNT);

    #elif defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
    for (size_t i = 0; i < MICROPY_HW_APA102_COUNT; i++) {
        // Skip 4 + offset to skip the header bytes too.
        status_apa102_color[4 * i + 4 + 1] = rgb_adjusted & 0xff;
        status_apa102_color[4 * i + 4 + 2] = (rgb_adjusted >> 8) & 0xff;
        status_apa102_color[4 * i + 4 + 3] = (rgb_adjusted >> 16) & 0xff;
    }

    #if CIRCUITPY_BITBANG_APA102
    shared_module_bitbangio_spi_write(&status_apa102, status_apa102_color, APA102_BUFFER_LENGTH);
    #else
    common_hal_busio_spi_write(&status_apa102, status_apa102_color, APA102_BUFFER_LENGTH);
    #endif

    #elif CIRCUITPY_PWM_RGB_LED
    uint8_t red_u8 = (rgb_adjusted >> 16) & 0xFF;
    uint8_t green_u8 = (rgb_adjusted >> 8) & 0xFF;
    uint8_t blue_u8 = rgb_adjusted & 0xFF;

    #ifdef CIRCUITPY_RGB_STATUS_INVERTED_PWM
    status_rgb_color[0] = (1 << 16) - 1 - ((uint16_t)(red_u8 << 8) + red_u8);
    status_rgb_color[1] = (1 << 16) - 1 - ((uint16_t)(green_u8 << 8) + green_u8);
    status_rgb_color[2] = (1 << 16) - 1 - ((uint16_t)(blue_u8 << 8) + blue_u8);
    #else
    status_rgb_color[0] = (uint16_t)(red_u8 << 8) + red_u8;
    status_rgb_color[1] = (uint16_t)(green_u8 << 8) + green_u8;
    status_rgb_color[2] = (uint16_t)(blue_u8 << 8) + blue_u8;
    #endif

    common_hal_pwmio_pwmout_set_duty_cycle(&rgb_status_r, status_rgb_color[0]);
    common_hal_pwmio_pwmout_set_duty_cycle(&rgb_status_g, status_rgb_color[1]);
    common_hal_pwmio_pwmout_set_duty_cycle(&rgb_status_b, status_rgb_color[2]);
    #elif CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_STATUS)
    common_hal_digitalio_digitalinout_set_value(
        &single_color_led, (rgb_adjusted > 0) ^ MICROPY_HW_LED_STATUS_INVERTED);
    #endif
}

uint32_t color_brightness(uint32_t color, uint8_t brightness) {
    #if CIRCUITPY_STATUS_LED
    uint32_t result = ((color & 0xff0000) * brightness / 255) & 0xff0000;
    result += ((color & 0xff00) * brightness / 255) & 0xff00;
    result += ((color & 0xff) * brightness / 255) & 0xff;
    return result;
    #else
    return color;
    #endif
}

void set_status_brightness(uint8_t level) {
    #if CIRCUITPY_STATUS_LED
    rgb_status_brightness = level;
    uint32_t current_color = current_status_color;
    // Temporarily change the current color global to force the new_status_color call to update the
    // LED. Usually duplicate calls of the same color are ignored without regard to brightness
    // changes.
    current_status_color = 0;
    new_status_color(current_color);
    #endif
}

void init_rxtx_leds(void) {
    #if CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_RX)
    common_hal_digitalio_digitalinout_construct(&rx_led, MICROPY_HW_LED_RX);
    common_hal_digitalio_digitalinout_switch_to_output(&rx_led, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_never_reset(&rx_led);
    #endif
    #if CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_TX)
    common_hal_digitalio_digitalinout_construct(&tx_led, MICROPY_HW_LED_TX);
    common_hal_digitalio_digitalinout_switch_to_output(&tx_led, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_never_reset(&tx_led);
    #endif
}

void deinit_rxtx_leds(void) {
    #if CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_RX)
    common_hal_digitalio_digitalinout_deinit(&rx_led);
    #endif
    #if CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_TX)
    common_hal_digitalio_digitalinout_deinit(&tx_led);
    #endif
}

void toggle_rx_led(void) {
    #if CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_RX)
    common_hal_digitalio_digitalinout_set_value(&rx_led, !common_hal_digitalio_digitalinout_get_value(&rx_led));
    #endif
}


void toggle_tx_led(void) {
    #if CIRCUITPY_DIGITALIO && defined(MICROPY_HW_LED_TX)
    common_hal_digitalio_digitalinout_set_value(&tx_led, !common_hal_digitalio_digitalinout_get_value(&tx_led));
    #endif
}
