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

#ifndef EPAPER_SLD00200P_DRIVER_H__
#define EPAPER_SLD00200P_DRIVER_H__

#if NRF52 // TODO: For now only supported by NRF52 targets, as PWM soft-pwm is not present for nrf51 yet.

#include "py/mphal.h"

#include "hal_spi.h"
#include "hal_pwm.h"
#include "framebuffer.h"

typedef enum
{
	EPD_COMP,
	EPD_WHITE,
	EPD_INV,
	EPD_NORM
} epd_stage_t;

void driver_sld00200p_init(NRF_SPI_Type * p_spi_instance,
                           NRF_PWM_Type * p_pwm_instance,
                           pin_obj_t * p_pin_cs,
                           pin_obj_t * p_pin_panel_on,
                           pin_obj_t * p_pin_border,
                           pin_obj_t * p_pin_busy,
                           pin_obj_t * p_pin_reset,
                           pin_obj_t * p_pin_discharge);

void driver_sld00200p_reinit(void);

void driver_sld00200p_deinit(void);

void driver_sld00200p_clear(uint16_t color);

void driver_sld00200p_update_line(uint16_t line, framebuffer_byte_t * p_bytes, framebuffer_byte_t * p_bytes_old, uint16_t len);

#endif // NRF52

#endif // EPAPER_SLD00200P_DRIVER_H__
