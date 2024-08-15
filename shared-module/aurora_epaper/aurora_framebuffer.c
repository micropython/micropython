/*
 * Hardware driver for Pervasive Displays' e-paper panels
 *
 * Copyright (c) Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/pervasive-displays-epaper-panel-hardware-driver
 *
 * Copyright 2023 Cisco Systems, Inc. and its affiliates
 * Author: Wyrdsec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */
#include "py/gc.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "py/mphal.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/aurora_epaper/aurora_framebuffer.h"
#include "shared-module/aurora_epaper/aurora_framebuffer.h"
#include "supervisor/port.h"

#include <stdio.h>

#define AURORA_EPAPER_NONE_PIXEL (0x00)
#define AURORA_EPAPER_WHITE_PIXEL (0x02)
#define AURORA_EPAPER_BLACK_PIXEL (0x03)

void common_hal_aurora_epaper_framebuffer_construct(
    aurora_epaper_framebuffer_obj_t *self,
    busio_spi_obj_t *spi,
    const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *reset,
    const mcu_pin_obj_t *busy,
    const mcu_pin_obj_t *discharge,
    const mcu_pin_obj_t *power,
    int width,
    int height,
    bool free_bus) {

    // Pervasive Displays only makes Aurora CoG drivers of specific size
    if (width == 128 && height == 96) {
        self->type = SMALL_1_44;
    } else if (width == 200 && height == 96) {
        self->type = MEDIUM_2;
    } else if (width == 264 && height == 176) {
        self->type = LARGE_2_7;
    } else if (width == 144 && height == 128) {
        self->type = SMALL_1_9;
    } else if (width == 232 && height == 128) {
        self->type = LARGE_2_6;
    } else {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q and %q"), MP_QSTR_width, MP_QSTR_height);
    }

    // CS
    common_hal_digitalio_digitalinout_construct(&self->chip_select, chip_select);
    common_hal_digitalio_digitalinout_switch_to_output(&self->chip_select, true, DRIVE_MODE_PUSH_PULL);
    common_hal_never_reset_pin(chip_select);

    // RST
    common_hal_digitalio_digitalinout_construct(&self->reset, reset);
    common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);
    common_hal_never_reset_pin(reset);

    // BUSY
    common_hal_digitalio_digitalinout_construct(&self->busy, busy);
    common_hal_digitalio_digitalinout_switch_to_input(&self->busy, PULL_NONE);
    common_hal_never_reset_pin(busy);

    // DC
    common_hal_digitalio_digitalinout_construct(&self->discharge, discharge);
    common_hal_digitalio_digitalinout_switch_to_output(&self->discharge, true, DRIVE_MODE_PUSH_PULL);
    common_hal_never_reset_pin(discharge);

    // Power pin (if set)
    if (power != NULL) {
        common_hal_digitalio_digitalinout_construct(&self->power, power);
        common_hal_digitalio_digitalinout_switch_to_output(&self->power, true, DRIVE_MODE_PUSH_PULL);
        common_hal_never_reset_pin(power);
    } else {
        self->power.pin = NULL;
    }

    self->bus = spi;
    common_hal_busio_spi_never_reset(self->bus);

    self->width = width;
    self->height = height;

    // Set default temperature
    common_hal_aurora_epaper_framebuffer_set_temperature(self, 20);

    // Malloc bufinfo and pframe
    common_hal_aurora_epaper_framebuffer_get_bufinfo(self, NULL);

    // By default free bus when deinit is called
    common_hal_aurora_epaper_framebuffer_set_free_bus(self, free_bus);
}

int common_hal_aurora_epaper_framebuffer_get_width(aurora_epaper_framebuffer_obj_t *self) {
    return self->width;
}

int common_hal_aurora_epaper_framebuffer_get_height(aurora_epaper_framebuffer_obj_t *self) {
    return self->height;
}

int common_hal_aurora_epaper_framebuffer_get_row_stride(aurora_epaper_framebuffer_obj_t *self) {
    return self->width / 8;
}

bool common_hal_aurora_epaper_framebuffer_get_reverse_pixels_in_byte(aurora_epaper_framebuffer_obj_t *self) {
    return false;
}

bool common_hal_aurora_epaper_framebuffer_get_pixels_in_byte_share_row(aurora_epaper_framebuffer_obj_t *self) {
    return true;
}

void common_hal_aurora_epaper_framebuffer_set_temperature(aurora_epaper_framebuffer_obj_t *self, double celsius) {
    uint32_t d = 480;
    if (self->type == LARGE_2_6 || self->type == LARGE_2_7) {
        d = 630;
    }

    if (celsius <= -10) {
        self->frametime = d * 17;
    } else if (celsius <= -5) {
        self->frametime = d * 12;
    } else if (celsius <= 5) {
        self->frametime = d * 8;
    } else if (celsius <= 10) {
        self->frametime = d * 4;
    } else if (celsius <= 15) {
        self->frametime = d * 3;
    } else if (celsius <= 20) {
        self->frametime = d * 2;
    } else if (celsius <= 40) {
        self->frametime = d * 1;
    } else {
        self->frametime = d * 0.7;
    }
}

void common_hal_aurora_epaper_framebuffer_set_free_bus(aurora_epaper_framebuffer_obj_t *self, bool val) {
    self->free_bus = val;
}

uint8_t common_hal_aurora_epaper_framebuffer_spi_raw_pair(aurora_epaper_framebuffer_obj_t *self, uint8_t b0, uint8_t b1) {
    uint8_t ret = 0;

    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    common_hal_busio_spi_write(self->bus, &b0, 1);
    common_hal_busio_spi_transfer(self->bus, &b1, &ret, 1);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);

    return ret;
}

uint8_t common_hal_aurora_epaper_framebuffer_spi_get_id(aurora_epaper_framebuffer_obj_t *self) {
    return common_hal_aurora_epaper_framebuffer_spi_raw_pair(self, 0x71, 0x00);
}

uint8_t common_hal_aurora_epaper_framebuffer_spi_read(aurora_epaper_framebuffer_obj_t *self, uint8_t index) {
    common_hal_aurora_epaper_framebuffer_spi_raw_pair(self, 0x70, index);
    return common_hal_aurora_epaper_framebuffer_spi_raw_pair(self, 0x73, 0x00);
}

void common_hal_aurora_epaper_framebuffer_spi_write_command(aurora_epaper_framebuffer_obj_t *self, uint8_t index, uint8_t data) {
    common_hal_aurora_epaper_framebuffer_spi_raw_pair(self, 0x70, index);
    common_hal_aurora_epaper_framebuffer_spi_raw_pair(self, 0x72, data);
}

void common_hal_aurora_epaper_framebuffer_get_bufinfo(aurora_epaper_framebuffer_obj_t *self, mp_buffer_info_t *bufinfo) {
    if (!self->bufinfo.buf) {
        int row_stride = common_hal_aurora_epaper_framebuffer_get_row_stride(self);
        int height = common_hal_aurora_epaper_framebuffer_get_height(self);
        self->bufinfo.len = row_stride * height;
        self->bufinfo.buf = port_malloc(self->bufinfo.len, false);
        if (self->bufinfo.buf == NULL) {
            m_malloc_fail(self->bufinfo.len);
        }
        memset(self->bufinfo.buf, 0, self->bufinfo.len);

        self->pframe.len = self->bufinfo.len;
        self->pframe.buf = port_malloc(self->bufinfo.len, false);
        if (self->pframe.buf == NULL) {
            m_malloc_fail(self->pframe.len);
        }
        memset(self->pframe.buf, 0, self->pframe.len);
    }
    if (bufinfo) {
        *bufinfo = self->bufinfo;
    }
}


bool common_hal_aurora_epaper_framebuffer_power_on(aurora_epaper_framebuffer_obj_t *self) {

    // Power on display
    if (self->power.pin != NULL) {
        common_hal_digitalio_digitalinout_set_value(&self->power, true);
    }

    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    common_hal_digitalio_digitalinout_set_value(&self->discharge, false);

    mp_hal_delay_ms(5);
    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    mp_hal_delay_ms(5);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    mp_hal_delay_ms(5);

    // Initialize Display
    while (common_hal_digitalio_digitalinout_get_value(&self->busy)) {
        mp_hal_delay_ms(1);
    }

    uint8_t cog_id = common_hal_aurora_epaper_framebuffer_spi_get_id(self);

    if (cog_id != 0x12) {
        common_hal_busio_spi_unlock(self->bus);
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_id);
        return false;
    }

    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x02, 0x40); // Disable OE
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x0B, 0x02); // Power saving mode

    uint8_t channel[8] = {0};
    switch (self->type) {
        case SMALL_1_44:
            memcpy(channel, (uint8_t []) {0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0x00}, 8);
            break;
        case SMALL_1_9:
            memcpy(channel, (uint8_t []) {0x00, 0x00, 0x00, 0x03, 0xFC, 0x00, 0x00, 0xFF}, 8);
            break;
        case MEDIUM_2:
            memcpy(channel, (uint8_t []) {0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xE0, 0x00}, 8);
            break;
        case LARGE_2_6:
            memcpy(channel, (uint8_t []) {0x00, 0x00, 0x1F, 0xE0, 0x00, 0x00, 0x00, 0xFF}, 8);
            break;
        case LARGE_2_7:
            memcpy(channel, (uint8_t []) {0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFE, 0x00, 0x00}, 8);
            break;
        default:
            common_hal_busio_spi_unlock(self->bus);
            return false;
    }

    // Send channel data manually
    common_hal_aurora_epaper_framebuffer_spi_raw_pair(self, 0x70, 0x01);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    uint8_t data_start = 0x72;
    common_hal_busio_spi_write(self->bus, &data_start, 1);
    common_hal_busio_spi_write(self->bus, channel, 8);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);

    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x07, 0xD1);  // High power mode osc setting
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x08, 0x02);  // Power setting
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x09, 0xC2);  // Set Vcom level
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x04, 0x03);  // Power setting
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x03, 0x01);  // Driver latch on
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x03, 0x00);  // Driver latch off

    mp_hal_delay_ms(5);

    for (int i = 0; i < 4; i++) {
        common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x05, 0x01); // Start charge pump positive voltage, VGH & VDH on
        mp_hal_delay_ms(150);
        common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x05, 0x03); // Start charge pump negative voltage, VGL & VDL on
        mp_hal_delay_ms(90);
        common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x05, 0x0F); // Set charge pump Vcom on
        mp_hal_delay_ms(40);

        if ((common_hal_aurora_epaper_framebuffer_spi_read(self, 0x0F) & 0x40) != 0) { // Check DC/DC
            common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x02, 0x06); // Output enable to disable
            return true; // Success
        }
    }

    return false; // Fail
}

void common_hal_aurora_epaper_framebuffer_power_finish(aurora_epaper_framebuffer_obj_t *self) {
    uint8_t border = 0x00;
    if (self->type == SMALL_1_9 || self->type == LARGE_2_6 || self->type == LARGE_2_7) {
        border = 0xAA;
    }

    for (int i = 0; i < self->height; i++) { // Nothing frame
        common_hal_aurora_epaper_framebuffer_draw_line(self, (uint8_t *)self->bufinfo.buf, i, AURORA_EPAPER_NONE_PIXEL, AURORA_EPAPER_NONE_PIXEL, border);
    }

    // Send special no-scan line
    border = 0xAA;
    common_hal_aurora_epaper_framebuffer_draw_line(self, (uint8_t *)self->bufinfo.buf, -4, AURORA_EPAPER_NONE_PIXEL, AURORA_EPAPER_NONE_PIXEL, border);

    common_hal_aurora_epaper_framebuffer_power_off(self);
}

void common_hal_aurora_epaper_framebuffer_power_off(aurora_epaper_framebuffer_obj_t *self) {
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x0B, 0x00); // Undocumented
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x03, 0x01); // Latch reset
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x05, 0x03); // Power off chargepump
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x05, 0x01); // Power off chargepump negative
    mp_hal_delay_ms(300);

    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x04, 0x80); // Discharge internal
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x05, 0x00); // Power off chargepump positive
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x07, 0x01); // OSC off
    mp_hal_delay_ms(50);

    if (self->power.pin != NULL) {
        common_hal_digitalio_digitalinout_set_value(&self->power, false);
    }
    mp_hal_delay_ms(10);
    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);


    common_hal_digitalio_digitalinout_set_value(&self->discharge, true);
    mp_hal_delay_ms(150);
    common_hal_digitalio_digitalinout_set_value(&self->discharge, false);
}

void common_hal_aurora_epaper_framebuffer_swapbuffers(aurora_epaper_framebuffer_obj_t *self, uint8_t *dirty_row_bitmask) {

    // claim SPI bus
    if (!common_hal_busio_spi_try_lock(self->bus)) {
        return;
    }

    // Spec states baud rate can be 20kHz, but my testing showed this is optimistic
    common_hal_busio_spi_configure(self->bus, 15000000, 0, 0, 8);

    if (!common_hal_aurora_epaper_framebuffer_power_on(self)) {
        common_hal_aurora_epaper_framebuffer_power_off(self);
        common_hal_busio_spi_unlock(self->bus);
        return;
    }

    int iters = 0;
    uint32_t start = (uint32_t)mp_hal_ticks_ms();
    uint8_t border = 0x00;
    if (self->type == LARGE_2_7) {
        border = 0xFF;
    }

    // Find out how many frames need to be sent to match frametime
    do {
        // Stage 1: Compensate
        common_hal_aurora_epaper_framebuffer_draw_frame(self, (uint8_t *)self->pframe.buf, AURORA_EPAPER_BLACK_PIXEL, AURORA_EPAPER_WHITE_PIXEL, border, 1);
        iters++;
    } while ((uint32_t)mp_hal_ticks_ms() - start < self->frametime);

    border = 0x00;

    // Stage 2: White
    common_hal_aurora_epaper_framebuffer_draw_frame(self, (uint8_t *)self->pframe.buf, AURORA_EPAPER_WHITE_PIXEL, AURORA_EPAPER_NONE_PIXEL, border, iters);

    // Stage 3: Inverse
    common_hal_aurora_epaper_framebuffer_draw_frame(self, (uint8_t *)self->bufinfo.buf, AURORA_EPAPER_BLACK_PIXEL, AURORA_EPAPER_WHITE_PIXEL, border, iters);

    if (self->type == SMALL_1_9 || self->type == LARGE_2_6 || self->type == LARGE_2_7) {
        border = 0xAA;
    }

    // Stage 4: Normal
    common_hal_aurora_epaper_framebuffer_draw_frame(self, (uint8_t *)self->bufinfo.buf, AURORA_EPAPER_WHITE_PIXEL, AURORA_EPAPER_BLACK_PIXEL, border, iters);

    memcpy(self->pframe.buf, self->bufinfo.buf, self->bufinfo.len);

    common_hal_aurora_epaper_framebuffer_power_finish(self);
    common_hal_busio_spi_unlock(self->bus);
}

void common_hal_aurora_epaper_framebuffer_draw_frame(aurora_epaper_framebuffer_obj_t *self, uint8_t *buf, uint32_t whiteMap, uint32_t blackMap, uint8_t border, uint16_t iters) {
    int stride = common_hal_aurora_epaper_framebuffer_get_row_stride(self);
    for (int i = 0; i < iters; i++) {
        for (int row = 0; row < self->height; row++) {
            common_hal_aurora_epaper_framebuffer_draw_line(self, buf + (row * stride), row, whiteMap, blackMap, border);
        }
    }
}

void common_hal_aurora_epaper_framebuffer_draw_line(aurora_epaper_framebuffer_obj_t *self, uint8_t *buf, int row, uint32_t whiteMap, uint32_t blackMap, uint8_t border) {
    // Write to buffer cmd
    common_hal_aurora_epaper_framebuffer_spi_raw_pair(self, 0x70, 0x0A);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);

    uint8_t startData = 0x72;
    common_hal_busio_spi_write(self->bus, &startData, 1);

    if (self->type == MEDIUM_2 || self->type == LARGE_2_7) {
        common_hal_busio_spi_write(self->bus, &border, 1);
    }

    if (self->type == SMALL_1_44 || self->type == MEDIUM_2 || self->type == LARGE_2_7) {

        // 3 bit to 4 bit lookup table
        #define DO_MAP(mapping, input) \
    (((mapping) >> (((input) & 5) << 2)) & 0xF)

        uint32_t evenMap =
            (whiteMap << 2 | whiteMap) << 0 |
                    (whiteMap << 2 | blackMap) << 4 |
                    (blackMap << 2 | whiteMap) << 16 |
                    (blackMap << 2 | blackMap) << 20;

        int stride = common_hal_aurora_epaper_framebuffer_get_row_stride(self);

        // Even bytes
        for (int x = stride - 1; x >= 0; x--) {
            uint8_t p = buf[x];
            uint8_t b = (uint8_t)(
                (DO_MAP(evenMap, p >> 4) << 4) |
                (DO_MAP(evenMap, p >> 0) << 0));

            common_hal_busio_spi_write(self->bus, &b, 1);
        }

        // Scan bytes
        for (int y = (self->height / 4) - 1; y >= 0; y--) {
            uint8_t b = 0x00;
            if (y == row / 4) {
                b = 3 << (row % 4 * 2);
            }

            common_hal_busio_spi_write(self->bus, &b, 1);
        }

        uint32_t oddMap =
            (whiteMap << 2 | whiteMap) << 0 |
                    (whiteMap << 2 | blackMap) << 16 |
                    (blackMap << 2 | whiteMap) << 4 |
                    (blackMap << 2 | blackMap) << 20;

        // Odd bytes
        for (int x = 0; x < stride; x++) {
            uint8_t p = buf[x];
            uint8_t b = (uint8_t)(
                (DO_MAP(oddMap, p >> 5) << 0) |
                (DO_MAP(oddMap, p >> 1) << 4));

            common_hal_busio_spi_write(self->bus, &b, 1);
        }

        if (self->type == SMALL_1_44) {
            common_hal_busio_spi_write(self->bus, &border, 1);
        }

#undef DO_MAP
    }
    // This code is untested
    else if (self->type == SMALL_1_9 || self->type == LARGE_2_6) {

        // 2 bit to 4 bit lookup table
        #define DO_MAP(mapping, input) \
    (((mapping) >> (((input) & 0x3) << 2)) & 0xF)

        uint32_t pixelMap =
            (whiteMap << 2 | whiteMap) << 0 |
                    (whiteMap << 2 | blackMap) << 4 |
                    (blackMap << 2 | whiteMap) << 8 |
                    (blackMap << 2 | blackMap) << 12;

        int stride = common_hal_aurora_epaper_framebuffer_get_row_stride(self);

        // Odd scan bytes
        for (int y = 0; y < (self->height / 8); y++) {
            uint8_t b = 0x00;
            if (row % 2 != 0 && row / 8 == y) {
                b = 3 << ((row - (y * 8)) ^ 7) / 2;
            }

            common_hal_busio_spi_write(self->bus, &b, 1);
        }

        // Send all pixels
        for (int x = stride - 1; x >= 0; x--) {
            uint8_t p = buf[x];
            uint8_t b = (uint8_t)(
                (DO_MAP(pixelMap, p >> 6) << 4) |
                (DO_MAP(pixelMap, p >> 4)) << 0);

            common_hal_busio_spi_write(self->bus, &b, 1);

            b = (uint8_t)(
                (DO_MAP(pixelMap, p >> 2) << 4) |
                (DO_MAP(pixelMap, p >> 0)) << 0);

            common_hal_busio_spi_write(self->bus, &b, 1);
        }

        // Send Even scan bytes
        for (int y = 0; y < (self->height / 8); y++) {
            uint8_t b = 0x00;
            if (row % 2 == 0 && row / 8 == y) {
                b = 3 << ((row - (y * 8)) ^ 7) / 2;
            }

            common_hal_busio_spi_write(self->bus, &b, 1);
        }

        common_hal_busio_spi_write(self->bus, &border, 1);

#undef DO_MAP
    }

    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
    common_hal_aurora_epaper_framebuffer_spi_write_command(self, 0x02, 0x07); // Turn on OE: output data from COG driver to panel
}

void common_hal_aurora_epaper_framebuffer_deinit(aurora_epaper_framebuffer_obj_t *self) {

    // Only deinit bus if it is static / or being used by other display
    if (self->free_bus) {
        common_hal_busio_spi_deinit(self->bus);
    }

    common_hal_reset_pin(self->chip_select.pin);
    common_hal_reset_pin(self->reset.pin);
    common_hal_reset_pin(self->busy.pin);
    common_hal_reset_pin(self->discharge.pin);
    if (self->power.pin != NULL) {
        common_hal_reset_pin(self->power.pin);
    }

    port_free(self->bufinfo.buf);
    port_free(self->pframe.buf);
}

void common_hal_aurora_epaper_framebuffer_collect_ptrs(aurora_epaper_framebuffer_obj_t *self) {
    gc_collect_ptr(self->bus);
}

/* Protocol functions */

static void aurora_epaper_framebuffer_deinit(mp_obj_t self_in) {
    aurora_epaper_framebuffer_obj_t *self = self_in;
    common_hal_aurora_epaper_framebuffer_deinit(self);
}

static void aurora_epaper_framebuffer_get_bufinfo(mp_obj_t self_in, mp_buffer_info_t *bufinfo) {
    aurora_epaper_framebuffer_obj_t *self = self_in;
    common_hal_aurora_epaper_framebuffer_get_bufinfo(self, bufinfo);
}

static int aurora_epaper_frambuffer_get_color_depth(mp_obj_t self_in) {
    return 1;
}

static bool aurora_epaper_framebuffer_get_grayscale(mp_obj_t self_in) {
    return true;
}

static int aurora_epaper_framebuffer_get_height(mp_obj_t self_in) {
    aurora_epaper_framebuffer_obj_t *self = self_in;
    return common_hal_aurora_epaper_framebuffer_get_height(self);
}

static int aurora_epaper_framebuffer_get_width(mp_obj_t self_in) {
    aurora_epaper_framebuffer_obj_t *self = self_in;
    return common_hal_aurora_epaper_framebuffer_get_width(self);
}

static void aurora_epaper_framebuffer_swapbuffers(mp_obj_t self_in, uint8_t *dirty_row_bitmask) {
    aurora_epaper_framebuffer_obj_t *self = self_in;
    common_hal_aurora_epaper_framebuffer_swapbuffers(self, dirty_row_bitmask);
}

static int aurora_epaper_framebuffer_get_first_pixel_offset(mp_obj_t self_in) {
    return 0;
}

static bool aurora_epaper_framebuffer_get_pixels_in_byte_share_row(mp_obj_t self_in) {
    aurora_epaper_framebuffer_obj_t *self = self_in;
    return common_hal_aurora_epaper_framebuffer_get_pixels_in_byte_share_row(self);
}

static bool aurora_epaper_framebuffer_get_reverse_pixels_in_byte(mp_obj_t self_in) {
    aurora_epaper_framebuffer_obj_t *self = self_in;
    return common_hal_aurora_epaper_framebuffer_get_reverse_pixels_in_byte(self);
}

static int aurora_epaper_framebuffer_get_row_stride(mp_obj_t self_in) {
    aurora_epaper_framebuffer_obj_t *self = self_in;
    return common_hal_aurora_epaper_framebuffer_get_row_stride(self);
}

const framebuffer_p_t aurora_epaper_framebuffer_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_framebuffer)
    .deinit = aurora_epaper_framebuffer_deinit,
    .get_bufinfo = aurora_epaper_framebuffer_get_bufinfo,
    .get_color_depth = aurora_epaper_frambuffer_get_color_depth,
    .get_grayscale = aurora_epaper_framebuffer_get_grayscale,
    .get_height = aurora_epaper_framebuffer_get_height,
    .get_width = aurora_epaper_framebuffer_get_width,
    .swapbuffers = aurora_epaper_framebuffer_swapbuffers,

    .get_first_pixel_offset = aurora_epaper_framebuffer_get_first_pixel_offset,
    .get_pixels_in_byte_share_row = aurora_epaper_framebuffer_get_pixels_in_byte_share_row,
    .get_reverse_pixels_in_byte = aurora_epaper_framebuffer_get_reverse_pixels_in_byte,
    .get_row_stride = aurora_epaper_framebuffer_get_row_stride,
};
