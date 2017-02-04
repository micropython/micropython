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

#include "py/mphal.h"

#include "lcd_ili9341_driver.h"
#include "hal_spi.h"
#include "hal_time.h"

#if MICROPY_PY_DISPLAY_LCD_ILI9341

static pin_obj_t    * mp_cs_pin;
static pin_obj_t    * mp_dc_pin;
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

static void data_write(uint8_t value)
{
    mp_hal_pin_high(mp_dc_pin);
    mp_hal_pin_low(mp_cs_pin);

    hal_spi_master_tx_rx(mp_instance, 1, &value, NULL);

    mp_hal_pin_high(mp_cs_pin);
}

void driver_ili9341_init(NRF_SPI_Type * p_instance, pin_obj_t * p_cs_pin, pin_obj_t * p_dc_pin)
{
    mp_instance = p_instance;
    mp_cs_pin   = p_cs_pin;
    mp_dc_pin   = p_dc_pin;
#if 0
    mp_hal_pin_high(enable_pin);
    mp_hal_pin_high(backlight_pin);
#endif

    mp_hal_pin_high(mp_cs_pin);
    mp_hal_pin_high(mp_dc_pin);

    // Read driver id

    mp_hal_delay_ms(1000);

    cmd_write(0x01);

    mp_hal_delay_ms(500);

    cmd_write(0xCF);
    data_write(0x00);
    data_write(0x8B);
    data_write(0X30);

    cmd_write(0xED);
    data_write(0x67);
    data_write(0x03);
    data_write(0X12);
    data_write(0X81);

    cmd_write(0xE8);
    data_write(0x85);
    data_write(0x10);
    data_write(0x7A);

    cmd_write(0xCB);
    data_write(0x39);
    data_write(0x2C);
    data_write(0x00);
    data_write(0x34);
    data_write(0x02);

    cmd_write(0xF7);
    data_write(0x20);

    cmd_write(0xEA);
    data_write(0x00);
    data_write(0x00);

    cmd_write(0xC0); /* Power control */
    data_write(0x1B); /* VRH[5:0] */

    cmd_write(0xC1); /* Power control */
    data_write(0x10); /* SAP[2:0];BT[3:0] */

    cmd_write(0xC5); /* VCM control */
    data_write(0x3F);
    data_write(0x3C);

    cmd_write(0xC7); /* VCM control2 */
    data_write(0XB7);

    cmd_write(0x36); /* Memory Access Control */
    data_write(0x08);

    cmd_write(0x3A);
    data_write(0x55);

    cmd_write(0xB1);
    data_write(0x00);
    data_write(0x1B);

    cmd_write(0xB6); /* Display Function Control */
    data_write(0x0A);
    data_write(0xA2);

    cmd_write(0xF2); /* 3Gamma Function Disable */
    data_write(0x00);

    cmd_write(0x26); /* Gamma curve selected */
    data_write(0x01);

    cmd_write(0xE0); /* Set Gamma */
    data_write(0x0F);
    data_write(0x2A);
    data_write(0x28);
    data_write(0x08);
    data_write(0x0E);
    data_write(0x08);
    data_write(0x54);
    data_write(0XA9);
    data_write(0x43);
    data_write(0x0A);
    data_write(0x0F);
    data_write(0x00);
    data_write(0x00);
    data_write(0x00);
    data_write(0x00);

    cmd_write(0XE1); /* Set Gamma */
    data_write(0x00);
    data_write(0x15);
    data_write(0x17);
    data_write(0x07);
    data_write(0x11);
    data_write(0x06);
    data_write(0x2B);
    data_write(0x56);
    data_write(0x3C);
    data_write(0x05);
    data_write(0x10);
    data_write(0x0F);
    data_write(0x3F);
    data_write(0x3F);
    data_write(0x0F);

    cmd_write(0x11); /* Exit Sleep */

    mp_hal_delay_ms(120);

    cmd_write(0x29); /* Display on */
}

static void set_col(uint16_t start_col, uint16_t end_col)
{
    cmd_write(0x2A); /* Column Command address */
    data_write(start_col >> 8);
    data_write(start_col & 0xFF );
    data_write(end_col >> 8);
    data_write(end_col & 0xFF);
}

static void set_page(uint16_t start_page, uint16_t end_page)
{
    cmd_write(0x2B); /* Column Command address */
    data_write(start_page >> 8);
    data_write(start_page & 0xFF);
    data_write(end_page >> 8);
    data_write(end_page & 0xFF);
}

void driver_ili9341_clear(uint16_t color)
{
    set_col(0, 239);
    set_page(0, 319);

    cmd_write(0x2c); // start writing to the display ram

    mp_hal_pin_high(mp_dc_pin);
    mp_hal_pin_low(mp_cs_pin);

    for(uint16_t i = 0; i < 38400; i++)
    {
        raw_write(color >> 8);
        raw_write(color & 0xFF);
        raw_write(color >> 8);
        raw_write(color & 0xFF);
    }

    mp_hal_pin_high(mp_cs_pin);
}

void driver_ili9341_update_line(uint16_t line, framebuffer_byte_t * p_bytes, uint16_t len) {
    set_col(0, 239);
    set_page(line, line);

    cmd_write(0x2c);

    mp_hal_pin_high(mp_dc_pin);
    mp_hal_pin_low(mp_cs_pin);

    for (uint16_t i = 0; i < len; i++) {
        uint8_t byte = (uint8_t)((uint8_t *)p_bytes)[i];
        for (uint8_t pixel_pos = 0; pixel_pos < 8; pixel_pos++) {
            if (((byte >> pixel_pos) & 0x1) == 0x0) {
                data_write(0x00);
                data_write(0x00);
            } else {
                data_write(0xFF);
                data_write(0xFF);
            }
        }
    }

    mp_hal_pin_high(mp_cs_pin);
}

#endif
