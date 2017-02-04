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

#ifndef LCD_LS0XXB7DXXX_DRIVER_H__
#define LCD_LS0XXB7DXXX_DRIVER_H__

#include "py/mphal.h"

#include "hal_spi.h"
#include "framebuffer.h"

void driver_ls0xxb7dxxx_init(NRF_SPI_Type * p_instance,
                             pin_obj_t    * p_cs_pin,
                             pin_obj_t    * p_disp_pin,
                             pin_obj_t    * p_ext_com_in_pin,
                             pin_obj_t    * p_ext_mode_pin,
                             pin_obj_t    * p_power_control_pin,
                             pin_obj_t    * p_power_charge_pin);

void driver_ls0xxb7dxxx_clear(uint16_t color);

void driver_ls0xxb7dxxx_update_line(uint16_t line, framebuffer_byte_t * p_bytes, uint16_t len);

#endif // LCD_LS0XXB7DXXX_DRIVER_H__
