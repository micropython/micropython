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

#include "lcd_ls0xxb7dxxx_driver.h"
#include "hal_spi.h"
#include "hal_time.h"

#include "framebuffer.h"

#if MICROPY_PY_DISPLAY_LCD_LS0XXB7DXXX

static pin_obj_t    * mp_cs_pin;
static pin_obj_t    * mp_disp_pin;
static pin_obj_t    * mp_extcomin_pin;
static pin_obj_t    * mp_extmode_pin;
static pin_obj_t    * mp_power_control_pin;
static pin_obj_t    * mp_power_charge_pin;
static NRF_SPI_Type * mp_instance;


static void raw_write(uint8_t value)
{
    hal_spi_master_tx_rx(mp_instance, 1, &value, NULL);
}

void driver_ls0xxb7dxxx_init(NRF_SPI_Type * p_instance,
                             pin_obj_t    * p_cs_pin,
                             pin_obj_t    * p_disp_pin,
                             pin_obj_t    * p_ext_com_in_pin,
                             pin_obj_t    * p_ext_mode_pin,
                             pin_obj_t    * p_power_control_pin,
                             pin_obj_t    * p_power_charge_pin) {
    mp_instance          = p_instance;
    mp_cs_pin            = p_cs_pin;
    mp_disp_pin          = p_disp_pin;
    mp_extcomin_pin      = p_ext_com_in_pin;
    mp_extmode_pin       = p_ext_mode_pin;
    mp_power_control_pin = p_power_control_pin;
    mp_power_charge_pin  = p_power_charge_pin;

    mp_hal_pin_high(mp_extcomin_pin);
    mp_hal_pin_low(mp_disp_pin);
    mp_hal_pin_low(mp_cs_pin);
    mp_hal_pin_low(mp_extmode_pin);
    mp_hal_pin_low(mp_power_charge_pin);
    mp_hal_pin_low(mp_power_control_pin);

    // power on display
    mp_hal_pin_high(mp_power_charge_pin);
    mp_hal_pin_high(mp_power_control_pin);

    // display on
    mp_hal_pin_high(mp_disp_pin);

    mp_hal_pin_low(mp_extcomin_pin);

}

void driver_ls0xxb7dxxx_clear(uint16_t color)
{
    mp_hal_pin_high(mp_cs_pin);
    raw_write(0x04); // clear command
    raw_write(0x00);
    mp_hal_pin_low(mp_cs_pin);
}

void driver_ls0xxb7dxxx_update_line(uint16_t line, framebuffer_byte_t * p_bytes, uint16_t len) {
    // update single line - 0x01 <line_num> <50bytes data> 0x00 0x00
    // update multi line  - 0x01 <line_num> <50bytes data> 0x00 [<line_num> <50bytes data> 0x00] 0x00

    mp_hal_pin_high(mp_cs_pin);

    mp_hal_delay_us(3);

    raw_write(0x01);

    raw_write(line);
    for (uint8_t i = 0; i < 50; i++)
    {
        uint8_t byte = (uint8_t)((uint8_t *)p_bytes)[i];
        raw_write(~byte);
    }
    raw_write(0x00);

    raw_write(0x00);

    mp_hal_delay_us(1);

    mp_hal_pin_low(mp_cs_pin);
}

#endif
