// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mipidsi/Display.h"
#include "shared-bindings/mipidsi/Bus.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "bindings/espidf/__init__.h"
#include <esp_lcd_panel_ops.h>
#include <esp_heap_caps.h>
#include "py/runtime.h"

// Cache write-back function (should be from rom/cache.h but it's not always available)
extern int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);

void common_hal_mipidsi_display_construct(mipidsi_display_obj_t *self,
    mipidsi_bus_obj_t *bus,
    const uint8_t *init_sequence,
    size_t init_sequence_len,
    mp_uint_t virtual_channel,
    mp_uint_t width,
    mp_uint_t height,
    mp_int_t rotation,
    mp_uint_t color_depth,
    const mcu_pin_obj_t *backlight_pin,
    mp_float_t brightness,
    mp_uint_t native_frames_per_second,
    bool backlight_on_high,
    mp_uint_t hsync_pulse_width,
    mp_uint_t hsync_back_porch,
    mp_uint_t hsync_front_porch,
    mp_uint_t vsync_pulse_width,
    mp_uint_t vsync_back_porch,
    mp_uint_t vsync_front_porch,
    mp_uint_t pixel_clock_frequency) {
    self->bus = bus;
    self->virtual_channel = virtual_channel;
    self->width = width;
    self->height = height;
    self->rotation = rotation;
    self->color_depth = color_depth;
    self->native_frames_per_second = native_frames_per_second;
    self->backlight_on_high = backlight_on_high;
    self->framebuffer = NULL;
    self->dbi_io_handle = NULL;
    self->dpi_panel_handle = NULL;
    self->backlight_inout.base.type = &mp_type_NoneType;

    // Create the DBI interface for sending commands
    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = virtual_channel,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };

    CHECK_ESP_RESULT(esp_lcd_new_panel_io_dbi(bus->bus_handle, &dbi_config, &self->dbi_io_handle));

    // Determine the pixel format based on color depth
    lcd_color_format_t color_format;
    if (color_depth == 16) {
        color_format = LCD_COLOR_FMT_RGB565;
    } else if (color_depth == 24) {
        color_format = LCD_COLOR_FMT_RGB888;
    } else {
        common_hal_mipidsi_display_deinit(self);
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_color_depth);
    }

    // Create the DPI panel for sending pixel data
    esp_lcd_dpi_panel_config_t dpi_config = {
        .virtual_channel = virtual_channel,
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        // .dpi_clock_freq_mhz is a float; preserve fractional clock frequency
        .dpi_clock_freq_mhz = pixel_clock_frequency / 1000000.0f,
        .in_color_format = color_format,
        .out_color_format = color_format,
        .num_fbs = 1,
        .video_timing = {
            .h_size = width,
            .v_size = height,
            .hsync_pulse_width = hsync_pulse_width,
            .hsync_back_porch = hsync_back_porch,
            .hsync_front_porch = hsync_front_porch,
            .vsync_pulse_width = vsync_pulse_width,
            .vsync_back_porch = vsync_back_porch,
            .vsync_front_porch = vsync_front_porch,
        },
        .flags = {
            .disable_lp = false,
        },
    };

    esp_err_t ret = esp_lcd_new_panel_dpi(bus->bus_handle, &dpi_config, &self->dpi_panel_handle);
    if (ret != ESP_OK) {
        common_hal_mipidsi_display_deinit(self);
        CHECK_ESP_RESULT(ret);
    }

    // Get the framebuffer allocated by the driver
    void *fb = NULL;
    ret = esp_lcd_dpi_panel_get_frame_buffer(self->dpi_panel_handle, 1, &fb);
    if (ret != ESP_OK || fb == NULL) {
        common_hal_mipidsi_display_deinit(self);
        CHECK_ESP_RESULT(ret);
    }

    self->framebuffer = (uint8_t *)fb;
    self->framebuffer_size = width * height * (color_depth / 8);

    // Send initialization sequence (format matches busdisplay)
    #define DELAY 0x80
    uint32_t i = 0;
    while (i < init_sequence_len) {
        const uint8_t *cmd = init_sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        data_size &= ~DELAY;
        const uint8_t *data = cmd + 2;
        esp_lcd_panel_io_tx_param(self->dbi_io_handle, cmd[0], data, data_size);

        uint16_t delay_length_ms = 0;
        if (delay) {
            data_size++;
            delay_length_ms = *(cmd + 1 + data_size);
            if (delay_length_ms == 255) {
                delay_length_ms = 500;
            }
        }
        common_hal_time_delay_ms(delay_length_ms);
        i += 2 + data_size;
    }

    // Initialize the panel after sending init commands
    ret = esp_lcd_panel_init(self->dpi_panel_handle);
    if (ret != ESP_OK) {
        common_hal_mipidsi_display_deinit(self);
        CHECK_ESP_RESULT(ret);
    }

    // Send display-ON (0x29) after the DPI/video stream is running, matching
    // the reference BSP flow order.
    esp_lcd_panel_io_tx_param(self->dbi_io_handle, 0x29, NULL, 0);
    common_hal_time_delay_ms(20);

    // Setup backlight PWM if pin is provided
    if (backlight_pin != NULL && common_hal_mcu_pin_is_free(backlight_pin)) {
        #if (CIRCUITPY_PWMIO)
        pwmout_result_t result = common_hal_pwmio_pwmout_construct(&self->backlight_pwm, backlight_pin, 0, 50000, false);
        if (result != PWMOUT_OK) {
            self->backlight_inout.base.type = &digitalio_digitalinout_type;
            common_hal_digitalio_digitalinout_construct(&self->backlight_inout, backlight_pin);
            common_hal_never_reset_pin(backlight_pin);
        } else {
            self->backlight_pwm.base.type = &pwmio_pwmout_type;
            common_hal_pwmio_pwmout_never_reset(&self->backlight_pwm);
        }
        #else
        self->backlight_inout.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->backlight_inout, backlight_pin);
        common_hal_never_reset_pin(backlight_pin);
        #endif

        // Set initial brightness
        #if (CIRCUITPY_PWMIO)
        if (self->backlight_pwm.base.type == &pwmio_pwmout_type) {
            common_hal_pwmio_pwmout_set_duty_cycle(&self->backlight_pwm, (uint16_t)(brightness * 0xFFFF));
        } else
        #endif
        if (self->backlight_inout.base.type == &digitalio_digitalinout_type) {
            bool on = brightness > 0;
            if (!backlight_on_high) {
                on = !on;
            }
            common_hal_digitalio_digitalinout_set_value(&self->backlight_inout, on);
        }
    }
    mipidsi_bus_increment_use_count(self->bus);
}

void common_hal_mipidsi_display_deinit(mipidsi_display_obj_t *self) {
    if (common_hal_mipidsi_display_deinited(self)) {
        return;
    }

    // Cleanup backlight
    #if (CIRCUITPY_PWMIO)
    if (self->backlight_pwm.base.type == &pwmio_pwmout_type) {
        common_hal_pwmio_pwmout_deinit(&self->backlight_pwm);
    } else
    #endif
    if (self->backlight_inout.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->backlight_inout);
    }

    // Delete the DPI panel
    if (self->dpi_panel_handle != NULL) {
        esp_lcd_panel_del(self->dpi_panel_handle);
        self->dpi_panel_handle = NULL;
    }

    // Delete the DBI interface
    if (self->dbi_io_handle != NULL) {
        esp_lcd_panel_io_del(self->dbi_io_handle);
        self->dbi_io_handle = NULL;
    }

    mipidsi_bus_decrement_use_count(self->bus);
    self->bus = NULL;
    self->framebuffer = NULL;
}

bool common_hal_mipidsi_display_deinited(mipidsi_display_obj_t *self) {
    return self->dpi_panel_handle == NULL;
}

void common_hal_mipidsi_display_refresh(mipidsi_display_obj_t *self) {
    // Drawing the framebuffer we got from the IDF will flush the cache(s) so
    // DMA can see our changes. It won't cause an extra copy.
    esp_lcd_panel_draw_bitmap(self->dpi_panel_handle, 0, 0, self->width, self->height, self->framebuffer);

    // The DPI panel will automatically refresh from the framebuffer
    // No explicit refresh call is needed as the DSI hardware continuously
    // sends data from the framebuffer to the display
}

mp_float_t common_hal_mipidsi_display_get_brightness(mipidsi_display_obj_t *self) {
    return self->current_brightness;
}

bool common_hal_mipidsi_display_set_brightness(mipidsi_display_obj_t *self, mp_float_t brightness) {
    if (!self->backlight_on_high) {
        brightness = 1.0 - brightness;
    }
    bool ok = false;

    // Avoid PWM types and functions when the module isn't enabled
    #if (CIRCUITPY_PWMIO)
    bool ispwm = (self->backlight_pwm.base.type == &pwmio_pwmout_type) ? true : false;
    #else
    bool ispwm = false;
    #endif

    if (ispwm) {
        #if (CIRCUITPY_PWMIO)
        common_hal_pwmio_pwmout_set_duty_cycle(&self->backlight_pwm, (uint16_t)(0xffff * brightness));
        ok = true;
        #else
        ok = false;
        #endif
    } else if (self->backlight_inout.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_set_value(&self->backlight_inout, brightness > 0.99);
        ok = true;
    }
    if (ok) {
        self->current_brightness = brightness;
    }
    return ok;
}

int common_hal_mipidsi_display_get_width(mipidsi_display_obj_t *self) {
    return self->width;
}

int common_hal_mipidsi_display_get_height(mipidsi_display_obj_t *self) {
    return self->height;
}

int common_hal_mipidsi_display_get_row_stride(mipidsi_display_obj_t *self) {
    return self->width * (self->color_depth / 8);
}

int common_hal_mipidsi_display_get_color_depth(mipidsi_display_obj_t *self) {
    return self->color_depth;
}

int common_hal_mipidsi_display_get_native_frames_per_second(mipidsi_display_obj_t *self) {
    return self->native_frames_per_second;
}

bool common_hal_mipidsi_display_get_grayscale(mipidsi_display_obj_t *self) {
    return false;
}

mp_int_t common_hal_mipidsi_display_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    mipidsi_display_obj_t *self = (mipidsi_display_obj_t *)self_in;

    bufinfo->buf = self->framebuffer;
    bufinfo->len = self->framebuffer_size;
    bufinfo->typecode = 'B';

    return 0;
}
