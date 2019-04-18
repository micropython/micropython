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
#include "shared-bindings/displayio/ParallelBus.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/display.h"

#include <stdint.h>

#include "tick.h"

#define DELAY 0x80

void common_hal_displayio_display_construct(displayio_display_obj_t* self,
        mp_obj_t bus, uint16_t width, uint16_t height, int16_t colstart, int16_t rowstart, uint16_t rotation,
        uint16_t color_depth, uint8_t set_column_command, uint8_t set_row_command,
        uint8_t write_ram_command, uint8_t set_vertical_scroll, uint8_t* init_sequence, uint16_t init_sequence_len,
        const mcu_pin_obj_t* backlight_pin, mp_float_t brightness, bool auto_brightness,
        bool single_byte_bounds, bool data_as_commands) {
    self->color_depth = color_depth;
    self->set_column_command = set_column_command;
    self->set_row_command = set_row_command;
    self->write_ram_command = write_ram_command;
    self->refresh = false;
    self->current_group = NULL;
    self->colstart = colstart;
    self->rowstart = rowstart;
    self->auto_brightness = auto_brightness;
    self->data_as_commands = data_as_commands;
    self->single_byte_bounds = single_byte_bounds;

    if (MP_OBJ_IS_TYPE(bus, &displayio_parallelbus_type)) {
        self->begin_transaction = common_hal_displayio_parallelbus_begin_transaction;
        self->send = common_hal_displayio_parallelbus_send;
        self->end_transaction = common_hal_displayio_parallelbus_end_transaction;
    } else if (MP_OBJ_IS_TYPE(bus, &displayio_fourwire_type)) {
        self->begin_transaction = common_hal_displayio_fourwire_begin_transaction;
        self->send = common_hal_displayio_fourwire_send;
        self->end_transaction = common_hal_displayio_fourwire_end_transaction;
    } else {
        mp_raise_ValueError(translate("Unsupported display bus type"));
    }
    self->bus = bus;

    uint32_t i = 0;
    while (!self->begin_transaction(self->bus)) {
#ifdef MICROPY_VM_HOOK_LOOP
        MICROPY_VM_HOOK_LOOP ;
#endif
    }
    while (i < init_sequence_len) {
        uint8_t *cmd = init_sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        data_size &= ~DELAY;
        uint8_t *data = cmd + 2;
        self->send(self->bus, true, cmd, 1);
        if (self->data_as_commands) {
            // Loop through each parameter to force a CS toggle
            for (uint32_t j=0; j < data_size; j++) {
                self->send(self->bus, true, data + j, 1);
            }
        } else {
            self->send(self->bus, false, data, data_size);
        }
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
    self->end_transaction(self->bus);

    supervisor_start_terminal(width, height);

    // Set the group after initialization otherwise we may send pixels while we delay in
    // initialization.
    self->refresh = true;
    self->current_group = &circuitpython_splash;

    self->width = width;
    self->height = height;
    rotation = rotation % 360;
    self->mirror_x = false;
    self->mirror_y = false;
    self->transpose_xy = false;
    if (rotation == 0 || rotation == 180) {
        if (rotation == 180) {
            self->mirror_x = true;
            self->mirror_y = true;
        }
    } else {
        self->transpose_xy = true;
        if (rotation == 90) {
            self->mirror_y = true;
        } else {
            self->mirror_x = true;
        }
    }

    // Always set the backlight type in case we're reusing memory.
    self->backlight_inout.base.type = &mp_type_NoneType;
    if (backlight_pin != NULL && common_hal_mcu_pin_is_free(backlight_pin)) {
        pwmout_result_t result = common_hal_pulseio_pwmout_construct(&self->backlight_pwm, backlight_pin, 0, 50000, false);
        if (result != PWMOUT_OK) {
            self->backlight_inout.base.type = &digitalio_digitalinout_type;
            common_hal_digitalio_digitalinout_construct(&self->backlight_inout, backlight_pin);
            never_reset_pin_number(backlight_pin->number);
        } else {
            self->backlight_pwm.base.type = &pulseio_pwmout_type;
            common_hal_pulseio_pwmout_never_reset(&self->backlight_pwm);
            if (!self->auto_brightness) {
                common_hal_displayio_display_set_brightness(self, brightness);
            }
        }
    }
}

void common_hal_displayio_display_show(displayio_display_obj_t* self, displayio_group_t* root_group) {
    if (root_group == NULL) {
        root_group = &circuitpython_splash;
    }
    self->current_group = root_group;
    common_hal_displayio_display_refresh_soon(self);
}

void common_hal_displayio_display_refresh_soon(displayio_display_obj_t* self) {
    self->refresh = true;
}

int32_t common_hal_displayio_display_wait_for_frame(displayio_display_obj_t* self) {
    uint64_t last_refresh = self->last_refresh;
    // Don't try to refresh if we got an exception.
    while (last_refresh == self->last_refresh && MP_STATE_VM(mp_pending_exception) == NULL) {
        MICROPY_VM_HOOK_LOOP
    }
    return 0;
}

bool common_hal_displayio_display_get_auto_brightness(displayio_display_obj_t* self) {
    return self->auto_brightness;
}

uint16_t common_hal_displayio_display_get_width(displayio_display_obj_t* self){
    return self->width;
}

uint16_t common_hal_displayio_display_get_height(displayio_display_obj_t* self){
    return self->height;
}

void common_hal_displayio_display_set_auto_brightness(displayio_display_obj_t* self, bool auto_brightness) {
    self->auto_brightness = auto_brightness;
}

mp_float_t common_hal_displayio_display_get_brightness(displayio_display_obj_t* self) {
    if (self->backlight_pwm.base.type == &pulseio_pwmout_type) {
        uint16_t duty_cycle = common_hal_pulseio_pwmout_get_duty_cycle(&self->backlight_pwm);
        return duty_cycle / ((mp_float_t) 0xffff);
    } else if (self->backlight_inout.base.type == &digitalio_digitalinout_type) {
        if (common_hal_digitalio_digitalinout_get_value(&self->backlight_inout)) {
            return 1.0;
        } else {
            return 0.0;
        }
    }
    return -1.0;
}

bool common_hal_displayio_display_set_brightness(displayio_display_obj_t* self, mp_float_t brightness) {
    self->updating_backlight = true;
    bool ok = false;
    if (self->backlight_pwm.base.type == &pulseio_pwmout_type) {
        common_hal_pulseio_pwmout_set_duty_cycle(&self->backlight_pwm, (uint16_t) (0xffff * brightness));
        ok = true;
    } else if (self->backlight_inout.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_set_value(&self->backlight_inout, brightness > 0.99);
        ok = true;
    }
    self->updating_backlight = false;
    return ok;
}

bool displayio_display_begin_transaction(displayio_display_obj_t* self) {
    return self->begin_transaction(self->bus);
}

void displayio_display_end_transaction(displayio_display_obj_t* self) {
    self->end_transaction(self->bus);
}

void displayio_display_set_region_to_update(displayio_display_obj_t* self, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

    self->send(self->bus, true, &self->set_column_command, 1);
    bool isCommand = self->data_as_commands;
    if (self->single_byte_bounds) {
        uint8_t data[2];
        data[0] = x0 + self->colstart;
        data[1] = x1 - 1 + self->colstart;
        self->send(self->bus, isCommand, (uint8_t*) data, 2);
    } else {
        uint16_t data[2];
        data[0] = __builtin_bswap16(x0 + self->colstart);
        data[1] = __builtin_bswap16(x1 - 1 + self->colstart);
        self->send(self->bus, isCommand, (uint8_t*) data, 4);
    }
    self->send(self->bus, true, &self->set_row_command, 1);
    if (self->single_byte_bounds) {
        uint8_t data[2];
        data[0] = y0 + self->rowstart;
        data[1] = y1 - 1 + self->rowstart;
        self->send(self->bus, isCommand, (uint8_t*) data, 2);
    } else {
        uint16_t data[2];
        data[0] = __builtin_bswap16(y0  + self->rowstart);
        data[1] = __builtin_bswap16(y1 - 1 + self->rowstart);
        self->send(self->bus, isCommand, (uint8_t*) data, 4);
    }
    if (!self->data_as_commands) {
        self->send(self->bus, true, &self->write_ram_command, 1);
    }
}

bool displayio_display_frame_queued(displayio_display_obj_t* self) {
    // Refresh at ~30 fps.
    return (ticks_ms - self->last_refresh) > 32;
}

bool displayio_display_refresh_queued(displayio_display_obj_t* self) {
    return self->refresh || (self->current_group != NULL && displayio_group_needs_refresh(self->current_group));
}

void displayio_display_finish_refresh(displayio_display_obj_t* self) {
    if (self->current_group != NULL) {
        displayio_group_finish_refresh(self->current_group);
    }
    self->refresh = false;
    self->last_refresh = ticks_ms;
}

void displayio_display_send_pixels(displayio_display_obj_t* self, uint32_t* pixels, uint32_t length) {
    self->send(self->bus, false, (uint8_t*) pixels, length * 4);
}

void displayio_display_update_backlight(displayio_display_obj_t* self) {
    if (!self->auto_brightness || self->updating_backlight) {
        return;
    }
    if (ticks_ms - self->last_backlight_refresh < 100) {
        return;
    }
    // TODO(tannewt): Fade the backlight based on it's existing value and a target value. The target
    // should account for ambient light when possible.
    common_hal_displayio_display_set_brightness(self, 1.0);

    self->last_backlight_refresh = ticks_ms;
}

void release_display(displayio_display_obj_t* self) {
    if (self->backlight_pwm.base.type == &pulseio_pwmout_type) {
        common_hal_pulseio_pwmout_reset_ok(&self->backlight_pwm);
        common_hal_pulseio_pwmout_deinit(&self->backlight_pwm);
    } else if (self->backlight_inout.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->backlight_inout);
    }
}
