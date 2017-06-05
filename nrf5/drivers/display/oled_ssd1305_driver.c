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

#include "oled_ssd1305_driver.h"
#include "hal_spi.h"
#include "hal_time.h"

#include "framebuffer.h"

#if MICROPY_PY_DISPLAY_OLED_SSD1305

static pin_obj_t    * mp_cs_pin;
static pin_obj_t    * mp_dc_pin;
static pin_obj_t    * mp_reset_pin;
static NRF_SPI_Type * mp_instance;


static void raw_write(uint8_t value)
{
    hal_spi_master_tx_rx(mp_instance, 1, &value, NULL);

}

static void cmd_write(uint8_t value)
{
    mp_hal_pin_low(mp_dc_pin);
    mp_hal_pin_low(mp_cs_pin);

    hal_spi_master_tx_rx(mp_instance, 1, &value, NULL);

    mp_hal_pin_high(mp_cs_pin);
}

#define SET_CONTRAST        (0x81)
#define SET_ENTIRE_ON       (0xa4)
#define SET_NORM_INV        (0xa6)
#define SET_DISP            (0xae)
#define SET_MEM_ADDR        (0x20)
#define SET_COL_ADDR        (0x21)
#define SET_PAGE_ADDR       (0x22)
#define SET_DISP_START_LINE (0x40)
#define SET_SEG_REMAP       (0xa0)
#define SET_MUX_RATIO       (0xa8)
#define SET_COM_OUT_DIR     (0xc0)
#define SET_DISP_OFFSET     (0xd3)
#define SET_COM_PIN_CFG     (0xda)
#define SET_DISP_CLK_DIV    (0xd5)
#define SET_PRECHARGE       (0xd9)
#define SET_VCOM_DESEL      (0xdb)
#define SET_CHARGE_PUMP     (0x8d)

void driver_ssd1305_init(NRF_SPI_Type * p_instance, pin_obj_t * p_cs_pin, pin_obj_t * p_dc_pin, pin_obj_t * p_reset_pin)
{
    mp_instance  = p_instance;
    mp_cs_pin    = p_cs_pin;
    mp_dc_pin    = p_dc_pin;
    mp_reset_pin = p_reset_pin;

    mp_hal_pin_high(mp_cs_pin);
    mp_hal_pin_high(mp_dc_pin);
    mp_hal_pin_high(mp_reset_pin);

    // power on display
    mp_hal_pin_high(mp_reset_pin);
    mp_hal_delay_ms(1);
    mp_hal_pin_low(mp_reset_pin);
    mp_hal_delay_ms(10);
    mp_hal_pin_high(mp_reset_pin);

    // Turn off
    cmd_write(SET_DISP | 0x00); // off

    // address setting
    cmd_write(SET_MEM_ADDR);
    cmd_write(0x00); // horizontal

    // resolution and layout
    cmd_write(SET_DISP_START_LINE | 0x00);
    cmd_write(SET_SEG_REMAP | 0x00); // column addr 127 mapped to SEG0
    cmd_write(SET_MUX_RATIO);

    uint16_t height = 64; // TODO: configurable
    cmd_write(height - 1); // height - 1
    cmd_write(SET_COM_OUT_DIR | 0x08); // scan from COM[N] to COM0
    cmd_write(SET_DISP_OFFSET);
    cmd_write(0x00);
    cmd_write(SET_COM_PIN_CFG);
    if (height == 32) {
        cmd_write(0x02);
    } else {
    	cmd_write(0x12);
    }
    // timing and driving scheme
    cmd_write(SET_DISP_CLK_DIV);
    cmd_write(0x80);
    cmd_write(SET_PRECHARGE);
    bool external_vcc = false;
    if (external_vcc == true) {
        cmd_write(0x22);
    } else {
    	cmd_write(0xf1);
    }
    cmd_write(SET_VCOM_DESEL);
    cmd_write(0x30); // 0.83*Vcc
    // display
    cmd_write(SET_CONTRAST);
    cmd_write(0xff); // maximum
    cmd_write(SET_ENTIRE_ON); // output follows RAM contents
    cmd_write(SET_NORM_INV); // not inverted
    // charge pump
    cmd_write(SET_CHARGE_PUMP);
    if (external_vcc == true) {
        cmd_write(0x10);
    } else {
    	cmd_write(0x14);
    }
    // on
    cmd_write(SET_DISP | 0x01);
}

static void set_col(uint16_t start_col, uint16_t end_col)
{
    cmd_write(SET_COL_ADDR); // column command address
    cmd_write(start_col & 0xFF );
    cmd_write(end_col & 0xFF);
}

static void set_page(uint16_t start_page, uint16_t end_page)
{
    cmd_write(SET_PAGE_ADDR); // page command address
    cmd_write(start_page & 0xFF);
    cmd_write(end_page & 0xFF);
}

void driver_ssd1305_clear(uint16_t color)
{
    uint16_t width = 128;
    uint16_t height = 64;

    uint16_t x0 = 0;
    uint16_t x1 = width - 1;
    uint16_t y0 = 0;
    uint16_t y1 = height -1;

    if (width == 64) {
        // displays with width of 64 pixels are shifted by 32
        x0 += 32;
        x1 += 32;
    }

    uint16_t num_of_pages = height / 8;
    set_col(x0, x1);
    set_page(y0, y1);

    mp_hal_pin_high(mp_dc_pin);
    mp_hal_pin_low(mp_cs_pin);

    for (uint16_t i = 0; i < (width * num_of_pages); i++) {
        raw_write(color);
    }

    mp_hal_pin_high(mp_cs_pin);
}

void driver_ssd1305_update_line(uint16_t line, framebuffer_byte_t * p_bytes, uint16_t len) {
    set_col(line, line);
    set_page(0, 63);

    mp_hal_pin_high(mp_dc_pin);
    mp_hal_pin_low(mp_cs_pin);

    for (uint8_t i = 0; i < len; i++) {
        uint8_t byte = (uint8_t)((uint8_t *)p_bytes)[i];
        raw_write(byte);
    }

    mp_hal_pin_high(mp_cs_pin);
}

#endif
