/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include <stdbool.h>

#include "py/mphal.h"

#include "lcd_ssd1289_driver.h"
#include "hal_time.h"

#include "framebuffer.h"

#if MICROPY_PY_DISPLAY_LCD_SSD1289

static pin_obj_t * mp_cs_pin;
static pin_obj_t * mp_rs_pin;
static pin_obj_t * mp_wr_pin;
static pin_obj_t * mp_reset_pin;
static pin_obj_t * mp_d0_pin;
static pin_obj_t * mp_d1_pin;
static pin_obj_t * mp_d2_pin;
static pin_obj_t * mp_d3_pin;
static pin_obj_t * mp_d4_pin;
static pin_obj_t * mp_d5_pin;
static pin_obj_t * mp_d6_pin;
static pin_obj_t * mp_d7_pin;

static void data_port_write(uint8_t byte) {
    mp_hal_pin_write(mp_d0_pin, (byte >> 0) & 0x1);
    mp_hal_pin_write(mp_d1_pin, (byte >> 1) & 0x1);
    mp_hal_pin_write(mp_d2_pin, (byte >> 2) & 0x1);
    mp_hal_pin_write(mp_d3_pin, (byte >> 3) & 0x1);
    mp_hal_pin_write(mp_d4_pin, (byte >> 4) & 0x1);
    mp_hal_pin_write(mp_d5_pin, (byte >> 5) & 0x1);
    mp_hal_pin_write(mp_d6_pin, (byte >> 6) & 0x1);
    mp_hal_pin_write(mp_d7_pin, (byte >> 7) & 0x1);
    mp_hal_pin_low(mp_wr_pin);
    mp_hal_delay_us(20);
    mp_hal_pin_high(mp_wr_pin);
}

static void cmd_write(uint8_t cmd) {
    mp_hal_pin_low(mp_rs_pin);
    mp_hal_delay_us(20);
    data_port_write(0x00);
    mp_hal_delay_us(20);
    data_port_write(cmd);
}

static void data_write(uint16_t value) {
    mp_hal_pin_high(mp_rs_pin);
    
    uint8_t high_byte = (uint8_t)(value >> 8);
    uint8_t low_byte  = (uint8_t)(value & 0xFF);
    mp_hal_delay_us(20);
    data_port_write(high_byte);
    mp_hal_delay_us(20);
    data_port_write(low_byte);
}

#define LCD_WRITE(a, b) { \
    cmd_write(a); \
    data_write(b); }

void driver_ssd1289_init(pin_obj_t * p_cs_pin,
                         pin_obj_t * p_rs_pin,
                         pin_obj_t * p_wr_pin,
                         pin_obj_t * p_reset_pin,
                         pin_obj_t * p_d0_pin,
                         pin_obj_t * p_d1_pin,
                         pin_obj_t * p_d2_pin,
                         pin_obj_t * p_d3_pin,
                         pin_obj_t * p_d4_pin,
                         pin_obj_t * p_d5_pin,
                         pin_obj_t * p_d6_pin,
                         pin_obj_t * p_d7_pin) {

    mp_cs_pin    = p_cs_pin;
    mp_rs_pin    = p_rs_pin;
    mp_wr_pin    = p_wr_pin;
    mp_reset_pin = p_reset_pin;
    mp_d0_pin    = p_d0_pin;
    mp_d1_pin    = p_d1_pin;
    mp_d2_pin    = p_d2_pin;
    mp_d3_pin    = p_d3_pin;
    mp_d4_pin    = p_d4_pin;
    mp_d5_pin    = p_d5_pin;
    mp_d6_pin    = p_d6_pin;
    mp_d7_pin    = p_d7_pin;

    mp_hal_pin_low(mp_d0_pin);
    mp_hal_pin_low(mp_d1_pin);
    mp_hal_pin_low(mp_d2_pin);
    mp_hal_pin_low(mp_d3_pin);
    mp_hal_pin_low(mp_d4_pin);
    mp_hal_pin_low(mp_d5_pin);
    mp_hal_pin_low(mp_d6_pin);
    mp_hal_pin_low(mp_d7_pin);

    mp_hal_pin_low(mp_rs_pin);
    mp_hal_pin_low(mp_wr_pin);

    mp_hal_pin_low(mp_cs_pin);

    mp_hal_pin_high(mp_reset_pin);
    mp_hal_delay_ms(20);
    mp_hal_pin_high(mp_reset_pin);

    LCD_WRITE(0x00,0x0001);
    LCD_WRITE(0x03,0xA8A4);
    LCD_WRITE(0x0C,0x0000);
    LCD_WRITE(0x0D,0x080C);
    LCD_WRITE(0x0E,0x2B00);
    LCD_WRITE(0x1E,0x00B7);
    LCD_WRITE(0x01,0x2B3F);
    LCD_WRITE(0x02,0x0600);
    LCD_WRITE(0x10,0x0000);
    LCD_WRITE(0x11,0x6070);
    LCD_WRITE(0x05,0x0000);
    LCD_WRITE(0x06,0x0000);
    LCD_WRITE(0x16,0xEF1C);
    LCD_WRITE(0x17,0x0003);
    LCD_WRITE(0x07,0x0233);
    LCD_WRITE(0x0B,0x0000);
    LCD_WRITE(0x0F,0x0000);
    LCD_WRITE(0x41,0x0000);
    LCD_WRITE(0x42,0x0000);
    LCD_WRITE(0x48,0x0000);
    LCD_WRITE(0x49,0x013F);
    LCD_WRITE(0x4A,0x0000);
    LCD_WRITE(0x4B,0x0000);
    LCD_WRITE(0x44,0xEF00);
    LCD_WRITE(0x45,0x0000);
    LCD_WRITE(0x46,0x013F);
    LCD_WRITE(0x30,0x0707);
    LCD_WRITE(0x31,0x0204);
    LCD_WRITE(0x32,0x0204);
    LCD_WRITE(0x33,0x0502);
    LCD_WRITE(0x34,0x0507);
    LCD_WRITE(0x35,0x0204);
    LCD_WRITE(0x36,0x0204);
    LCD_WRITE(0x37,0x0502);
    LCD_WRITE(0x3A,0x0302);
    LCD_WRITE(0x3B,0x0302);
    LCD_WRITE(0x23,0x0000);
    LCD_WRITE(0x24,0x0000);
    LCD_WRITE(0x25,0x8000);
    LCD_WRITE(0x4f,0x0000);
    LCD_WRITE(0x4e,0x0000);
    cmd_write(0x22);
    
    mp_hal_pin_high(mp_cs_pin);
}

static void set_xy(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    LCD_WRITE(0x44, (y1 << 8) + y0);
    LCD_WRITE(0x45, 319 - x1);
    LCD_WRITE(0x46, 319 - x0);
    LCD_WRITE(0x4e, y0);
    LCD_WRITE(0x4f, 319 - x1);
    cmd_write(0x22);
}

void driver_ssd1289_clear(uint16_t color) {
    uint16_t x;
    uint16_t y;
    uint16_t lcd_x_size = 240;
    uint16_t lcd_y_size = 320;

    mp_hal_pin_low(mp_cs_pin);

    set_xy(0, 0, lcd_y_size - 1, lcd_x_size - 1);

    for (x = 0; x < lcd_x_size; x++) {
        for (y = 0; y < lcd_y_size; y++) {
            data_write(color);
        }
    }

    mp_hal_pin_high(mp_cs_pin);
}

void driver_ssd1289_update_line(uint16_t line, framebuffer_byte_t * p_bytes, uint16_t len) {
    set_xy(0, line, 319, line);

    mp_hal_pin_low(mp_cs_pin);

    for (uint8_t i = 0; i < len; i++) {
        uint8_t byte = (uint8_t)((uint8_t *)p_bytes)[i];
        for (uint8_t pixel_pos = 0; pixel_pos < 8; pixel_pos++) {
            if (((byte >> pixel_pos) & 0x1) == 0x0) {
		        data_write(0x0000);
            } else {
                data_write(0xFFFF);
            }
        }
    }

    mp_hal_pin_high(mp_cs_pin);
}

#endif // MICROPY_PY_DISPLAY_LCD_SSD1289
