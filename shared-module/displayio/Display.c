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

#include "shared-bindings/displayio/Display.h"

#include "py/runtime.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-bindings/displayio/I2CDisplay.h"
#include "shared-bindings/displayio/ParallelBus.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/display_core.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/tick.h"
#include "supervisor/usb.h"

#include <stdint.h>
#include <string.h>

void common_hal_displayio_display_construct(displayio_display_obj_t *self,
    mp_obj_t bus, uint16_t width, uint16_t height, int16_t colstart, int16_t rowstart,
    uint16_t rotation, uint16_t color_depth, bool grayscale, bool pixels_in_byte_share_row,
    uint8_t bytes_per_cell, bool reverse_pixels_in_byte, bool reverse_bytes_in_word, uint8_t set_column_command,
    uint8_t set_row_command, uint8_t write_ram_command, uint8_t set_vertical_scroll,
    uint8_t *init_sequence, uint16_t init_sequence_len, const mcu_pin_obj_t *backlight_pin,
    uint16_t brightness_command, mp_float_t brightness, bool auto_brightness,
    bool single_byte_bounds, bool data_as_commands, bool auto_refresh, uint16_t native_frames_per_second,
    bool backlight_on_high, bool SH1107_addressing) {

    // Turn off auto-refresh as we init.
    self->auto_refresh = false;
    uint16_t ram_width = 0x100;
    uint16_t ram_height = 0x100;
    if (single_byte_bounds) {
        ram_width = 0xff;
        ram_height = 0xff;
    }
    displayio_display_core_construct(&self->core, bus, width, height, ram_width, ram_height, colstart, rowstart, rotation,
        color_depth, grayscale, pixels_in_byte_share_row, bytes_per_cell, reverse_pixels_in_byte, reverse_bytes_in_word);

    self->set_column_command = set_column_command;
    self->set_row_command = set_row_command;
    self->write_ram_command = write_ram_command;
    self->brightness_command = brightness_command;
    self->auto_brightness = auto_brightness;
    self->first_manual_refresh = !auto_refresh;
    self->data_as_commands = data_as_commands;
    self->backlight_on_high = backlight_on_high;
    self->SH1107_addressing = SH1107_addressing;

    self->native_frames_per_second = native_frames_per_second;
    self->native_ms_per_frame = 1000 / native_frames_per_second;

    uint32_t i = 0;
    while (i < init_sequence_len) {
        uint8_t *cmd = init_sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        data_size &= ~DELAY;
        uint8_t *data = cmd + 2;
        while (!displayio_display_core_begin_transaction(&self->core)) {
            RUN_BACKGROUND_TASKS;
        }
        if (self->data_as_commands) {
            uint8_t full_command[data_size + 1];
            full_command[0] = cmd[0];
            memcpy(full_command + 1, data, data_size);
            self->core.send(self->core.bus, DISPLAY_COMMAND, CHIP_SELECT_TOGGLE_EVERY_BYTE, full_command, data_size + 1);
        } else {
            self->core.send(self->core.bus, DISPLAY_COMMAND, CHIP_SELECT_TOGGLE_EVERY_BYTE, cmd, 1);
            self->core.send(self->core.bus, DISPLAY_DATA, CHIP_SELECT_UNTOUCHED, data, data_size);
        }
        displayio_display_core_end_transaction(&self->core);
        uint16_t delay_length_ms = 10;
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

    // Always set the backlight type in case we're reusing memory.
    self->backlight_inout.base.type = &mp_type_NoneType;
    if (backlight_pin != NULL && common_hal_mcu_pin_is_free(backlight_pin)) {
        // Avoid PWM types and functions when the module isn't enabled
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
        // Otherwise default to digital
        self->backlight_inout.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->backlight_inout, backlight_pin);
        common_hal_never_reset_pin(backlight_pin);
        #endif
    }
    if (!self->auto_brightness && (self->backlight_inout.base.type != &mp_type_NoneType ||
                                   brightness_command != NO_BRIGHTNESS_COMMAND)) {
        common_hal_displayio_display_set_brightness(self, brightness);
    } else {
        self->current_brightness = -1.0;
    }

    // Set the group after initialization otherwise we may send pixels while we delay in
    // initialization.
    common_hal_displayio_display_show(self, &circuitpython_splash);
    common_hal_displayio_display_set_auto_refresh(self, auto_refresh);
}

bool common_hal_displayio_display_show(displayio_display_obj_t *self, displayio_group_t *root_group) {
    return displayio_display_core_show(&self->core, root_group);
}

uint16_t common_hal_displayio_display_get_width(displayio_display_obj_t *self) {
    return displayio_display_core_get_width(&self->core);
}

uint16_t common_hal_displayio_display_get_height(displayio_display_obj_t *self) {
    return displayio_display_core_get_height(&self->core);
}

bool common_hal_displayio_display_get_auto_brightness(displayio_display_obj_t *self) {
    return self->auto_brightness;
}

void common_hal_displayio_display_set_auto_brightness(displayio_display_obj_t *self, bool auto_brightness) {
    self->auto_brightness = auto_brightness;
}

mp_float_t common_hal_displayio_display_get_brightness(displayio_display_obj_t *self) {
    return self->current_brightness;
}

bool common_hal_displayio_display_set_brightness(displayio_display_obj_t *self, mp_float_t brightness) {
    self->updating_backlight = true;
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
    } else if (self->brightness_command != NO_BRIGHTNESS_COMMAND) {
        ok = displayio_display_core_begin_transaction(&self->core);
        if (ok) {
            if (self->data_as_commands) {
                uint8_t set_brightness[2] = {self->brightness_command, (uint8_t)(0xff * brightness)};
                self->core.send(self->core.bus, DISPLAY_COMMAND, CHIP_SELECT_TOGGLE_EVERY_BYTE, set_brightness, 2);
            } else {
                uint8_t command = self->brightness_command;
                uint8_t hex_brightness = 0xff * brightness;
                self->core.send(self->core.bus, DISPLAY_COMMAND, CHIP_SELECT_TOGGLE_EVERY_BYTE, &command, 1);
                self->core.send(self->core.bus, DISPLAY_DATA, CHIP_SELECT_UNTOUCHED, &hex_brightness, 1);
            }
            displayio_display_core_end_transaction(&self->core);
        }

    }
    self->updating_backlight = false;
    if (ok) {
        self->current_brightness = brightness;
    }
    return ok;
}

mp_obj_t common_hal_displayio_display_get_bus(displayio_display_obj_t *self) {
    return self->core.bus;
}

STATIC const displayio_area_t *_get_refresh_areas(displayio_display_obj_t *self) {
    if (self->core.full_refresh) {
        self->core.area.next = NULL;
        return &self->core.area;
    } else if (self->core.current_group != NULL) {
        return displayio_group_get_refresh_areas(self->core.current_group, NULL);
    }
    return NULL;
}

STATIC void _send_pixels(displayio_display_obj_t *self, uint8_t *pixels, uint32_t length) {
    if (!self->data_as_commands) {
        self->core.send(self->core.bus, DISPLAY_COMMAND, CHIP_SELECT_TOGGLE_EVERY_BYTE, &self->write_ram_command, 1);
    }
    self->core.send(self->core.bus, DISPLAY_DATA, CHIP_SELECT_UNTOUCHED, pixels, length);
}

STATIC bool _refresh_area(displayio_display_obj_t *self, const displayio_area_t *area) {
    uint16_t buffer_size = 128; // In uint32_ts

    displayio_area_t clipped;
    // Clip the area to the display by overlapping the areas. If there is no overlap then we're done.
    if (!displayio_display_core_clip_area(&self->core, area, &clipped)) {
        return true;
    }
    uint16_t rows_per_buffer = displayio_area_height(&clipped);
    uint8_t pixels_per_word = (sizeof(uint32_t) * 8) / self->core.colorspace.depth;
    uint16_t pixels_per_buffer = displayio_area_size(&clipped);

    uint16_t subrectangles = 1;
    // for SH1107 and other boundary constrained controllers
    //      write one single row at a time
    if (self->SH1107_addressing) {
        subrectangles = rows_per_buffer / 8;  // page addressing mode writes 8 rows at a time
        rows_per_buffer = 8;
    } else if (displayio_area_size(&clipped) > buffer_size * pixels_per_word) {
        rows_per_buffer = buffer_size * pixels_per_word / displayio_area_width(&clipped);
        if (rows_per_buffer == 0) {
            rows_per_buffer = 1;
        }
        // If pixels are packed by column then ensure rows_per_buffer is on a byte boundary.
        if (self->core.colorspace.depth < 8 && !self->core.colorspace.pixels_in_byte_share_row) {
            uint8_t pixels_per_byte = 8 / self->core.colorspace.depth;
            if (rows_per_buffer % pixels_per_byte != 0) {
                rows_per_buffer -= rows_per_buffer % pixels_per_byte;
            }
        }
        subrectangles = displayio_area_height(&clipped) / rows_per_buffer;
        if (displayio_area_height(&clipped) % rows_per_buffer != 0) {
            subrectangles++;
        }
        pixels_per_buffer = rows_per_buffer * displayio_area_width(&clipped);
        buffer_size = pixels_per_buffer / pixels_per_word;
        if (pixels_per_buffer % pixels_per_word) {
            buffer_size += 1;
        }
    }

    // Allocated and shared as a uint32_t array so the compiler knows the
    // alignment everywhere.
    uint32_t buffer[buffer_size];
    uint32_t mask_length = (pixels_per_buffer / 32) + 1;
    uint32_t mask[mask_length];
    uint16_t remaining_rows = displayio_area_height(&clipped);

    for (uint16_t j = 0; j < subrectangles; j++) {
        displayio_area_t subrectangle = {
            .x1 = clipped.x1,
            .y1 = clipped.y1 + rows_per_buffer * j,
            .x2 = clipped.x2,
            .y2 = clipped.y1 + rows_per_buffer * (j + 1)
        };
        if (remaining_rows < rows_per_buffer) {
            subrectangle.y2 = subrectangle.y1 + remaining_rows;
        }
        remaining_rows -= rows_per_buffer;

        displayio_display_core_set_region_to_update(&self->core, self->set_column_command,
            self->set_row_command, NO_COMMAND, NO_COMMAND, self->data_as_commands, false,
            &subrectangle, self->SH1107_addressing);

        uint16_t subrectangle_size_bytes;
        if (self->core.colorspace.depth >= 8) {
            subrectangle_size_bytes = displayio_area_size(&subrectangle) * (self->core.colorspace.depth / 8);
        } else {
            subrectangle_size_bytes = displayio_area_size(&subrectangle) / (8 / self->core.colorspace.depth);
        }

        memset(mask, 0, mask_length * sizeof(mask[0]));
        memset(buffer, 0, buffer_size * sizeof(buffer[0]));

        displayio_display_core_fill_area(&self->core, &subrectangle, mask, buffer);

        // Can't acquire display bus; skip the rest of the data.
        if (!displayio_display_core_bus_free(&self->core)) {
            return false;
        }

        displayio_display_core_begin_transaction(&self->core);
        _send_pixels(self, (uint8_t *)buffer, subrectangle_size_bytes);
        displayio_display_core_end_transaction(&self->core);

        // TODO(tannewt): Make refresh displays faster so we don't starve other
        // background tasks.
        usb_background();
    }
    return true;
}

STATIC void _refresh_display(displayio_display_obj_t *self) {
    if (!displayio_display_core_start_refresh(&self->core)) {
        // A refresh on this bus is already in progress.  Try next display.
        return;
    }
    const displayio_area_t *current_area = _get_refresh_areas(self);
    while (current_area != NULL) {
        _refresh_area(self, current_area);
        current_area = current_area->next;
    }
    displayio_display_core_finish_refresh(&self->core);
}

void common_hal_displayio_display_set_rotation(displayio_display_obj_t *self, int rotation) {
    bool transposed = (self->core.rotation == 90 || self->core.rotation == 270);
    bool will_transposed = (rotation == 90 || rotation == 270);
    if (transposed != will_transposed) {
        int tmp = self->core.width;
        self->core.width = self->core.height;
        self->core.height = tmp;
    }
    displayio_display_core_set_rotation(&self->core, rotation);
    if (self == &displays[0].display) {
        supervisor_stop_terminal();
        supervisor_start_terminal(self->core.width, self->core.height);
    }
    if (self->core.current_group != NULL) {
        displayio_group_update_transform(self->core.current_group, &self->core.transform);
    }
}

uint16_t common_hal_displayio_display_get_rotation(displayio_display_obj_t *self) {
    return self->core.rotation;
}


bool common_hal_displayio_display_refresh(displayio_display_obj_t *self, uint32_t target_ms_per_frame, uint32_t maximum_ms_per_real_frame) {
    if (!self->auto_refresh && !self->first_manual_refresh && (target_ms_per_frame != 0xffffffff)) {
        uint64_t current_time = supervisor_ticks_ms64();
        uint32_t current_ms_since_real_refresh = current_time - self->core.last_refresh;
        // Test to see if the real frame time is below our minimum.
        if (current_ms_since_real_refresh > maximum_ms_per_real_frame) {
            mp_raise_RuntimeError(translate("Below minimum frame rate"));
        }
        uint32_t current_ms_since_last_call = current_time - self->last_refresh_call;
        self->last_refresh_call = current_time;
        // Skip the actual refresh to help catch up.
        if (current_ms_since_last_call > target_ms_per_frame) {
            return false;
        }
        uint32_t remaining_time = target_ms_per_frame - (current_ms_since_real_refresh % target_ms_per_frame);
        // We're ahead of the game so wait until we align with the frame rate.
        while (supervisor_ticks_ms64() - self->last_refresh_call < remaining_time) {
            RUN_BACKGROUND_TASKS;
        }
    }
    self->first_manual_refresh = false;
    _refresh_display(self);
    return true;
}

bool common_hal_displayio_display_get_auto_refresh(displayio_display_obj_t *self) {
    return self->auto_refresh;
}

void common_hal_displayio_display_set_auto_refresh(displayio_display_obj_t *self,
    bool auto_refresh) {
    self->first_manual_refresh = !auto_refresh;
    if (auto_refresh != self->auto_refresh) {
        if (auto_refresh) {
            supervisor_enable_tick();
        } else {
            supervisor_disable_tick();
        }
    }
    self->auto_refresh = auto_refresh;
}

STATIC void _update_backlight(displayio_display_obj_t *self) {
    if (!self->auto_brightness || self->updating_backlight) {
        return;
    }
    if (supervisor_ticks_ms64() - self->last_backlight_refresh < 100) {
        return;
    }
    // TODO(tannewt): Fade the backlight based on its existing value and a target value. The target
    // should account for ambient light when possible.
    common_hal_displayio_display_set_brightness(self, 1.0);

    self->last_backlight_refresh = supervisor_ticks_ms64();
}

void displayio_display_background(displayio_display_obj_t *self) {
    _update_backlight(self);

    if (self->auto_refresh && (supervisor_ticks_ms64() - self->core.last_refresh) > self->native_ms_per_frame) {
        _refresh_display(self);
    }
}

void release_display(displayio_display_obj_t *self) {
    common_hal_displayio_display_set_auto_refresh(self, false);
    release_display_core(&self->core);
    #if (CIRCUITPY_PWMIO)
    if (self->backlight_pwm.base.type == &pwmio_pwmout_type) {
        common_hal_pwmio_pwmout_reset_ok(&self->backlight_pwm);
        common_hal_pwmio_pwmout_deinit(&self->backlight_pwm);
    } else if (self->backlight_inout.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->backlight_inout);
    }
    #else
    common_hal_digitalio_digitalinout_deinit(&self->backlight_inout);
    #endif
}

void reset_display(displayio_display_obj_t *self) {
    common_hal_displayio_display_set_auto_refresh(self, true);
    self->auto_brightness = true;
    common_hal_displayio_display_show(self, NULL);
}

void displayio_display_collect_ptrs(displayio_display_obj_t *self) {
    displayio_display_core_collect_ptrs(&self->core);
}
