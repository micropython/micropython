/*
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
#pragma once

#include "py/obj.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

enum AuroraType {
    SMALL_1_44 = 0,
    SMALL_1_9,
    MEDIUM_2,
    LARGE_2_6,
    LARGE_2_7,
};

typedef struct {
    mp_obj_base_t base;
    busio_spi_obj_t *bus;
    digitalio_digitalinout_obj_t chip_select;
    digitalio_digitalinout_obj_t reset;
    digitalio_digitalinout_obj_t busy;
    digitalio_digitalinout_obj_t discharge;
    digitalio_digitalinout_obj_t power;
    mp_buffer_info_t bufinfo;
    mp_buffer_info_t pframe;

    uint32_t frametime;
    uint16_t width, height;
    enum AuroraType type;

    bool free_bus;
} aurora_epaper_framebuffer_obj_t;

extern const framebuffer_p_t aurora_epaper_framebuffer_proto;

void common_hal_aurora_epaper_framebuffer_construct(aurora_epaper_framebuffer_obj_t *self, busio_spi_obj_t *spi, const mcu_pin_obj_t *chip_select, const mcu_pin_obj_t *reset, const mcu_pin_obj_t *busy, const mcu_pin_obj_t *discharge, const mcu_pin_obj_t *power, int width, int height, bool free_bus);
int common_hal_aurora_epaper_framebuffer_get_width(aurora_epaper_framebuffer_obj_t *self);
int common_hal_aurora_epaper_framebuffer_get_height(aurora_epaper_framebuffer_obj_t *self);
int common_hal_aurora_epaper_framebuffer_get_row_stride(aurora_epaper_framebuffer_obj_t *self);
bool common_hal_aurora_epaper_framebuffer_get_reverse_pixels_in_byte(aurora_epaper_framebuffer_obj_t *self);
bool common_hal_aurora_epaper_framebuffer_get_pixels_in_byte_share_row(aurora_epaper_framebuffer_obj_t *self);
void common_hal_aurora_epaper_framebuffer_get_bufinfo(aurora_epaper_framebuffer_obj_t *self, mp_buffer_info_t *bufinfo);
void common_hal_aurora_epaper_framebuffer_swapbuffers(aurora_epaper_framebuffer_obj_t *self, uint8_t *dirty_row_bitmask);
void common_hal_aurora_epaper_framebuffer_deinit(aurora_epaper_framebuffer_obj_t *self);

void common_hal_aurora_epaper_framebuffer_set_temperature(aurora_epaper_framebuffer_obj_t *self, double celsius);
void common_hal_aurora_epaper_framebuffer_set_free_bus(aurora_epaper_framebuffer_obj_t *self, bool val);

uint8_t common_hal_aurora_epaper_framebuffer_spi_raw_pair(aurora_epaper_framebuffer_obj_t *self, uint8_t b0, uint8_t b1);
uint8_t common_hal_aurora_epaper_framebuffer_spi_get_id(aurora_epaper_framebuffer_obj_t *self);
uint8_t common_hal_aurora_epaper_framebuffer_spi_read(aurora_epaper_framebuffer_obj_t *self, uint8_t index);
void common_hal_aurora_epaper_framebuffer_spi_write_command(aurora_epaper_framebuffer_obj_t *self, uint8_t index, uint8_t data);
bool common_hal_aurora_epaper_framebuffer_power_on(aurora_epaper_framebuffer_obj_t *self);
void common_hal_aurora_epaper_framebuffer_power_finish(aurora_epaper_framebuffer_obj_t *self);
void common_hal_aurora_epaper_framebuffer_power_off(aurora_epaper_framebuffer_obj_t *self);
void common_hal_aurora_epaper_framebuffer_draw_frame(aurora_epaper_framebuffer_obj_t *self, uint8_t *buf, uint32_t whiteMap, uint32_t blackMap, uint8_t border, uint16_t iterations);
void common_hal_aurora_epaper_framebuffer_draw_line(aurora_epaper_framebuffer_obj_t *self, uint8_t *buf, int row, uint32_t whiteMap, uint32_t blackMap, uint8_t border);

void common_hal_aurora_epaper_framebuffer_collect_ptrs(aurora_epaper_framebuffer_obj_t *);
