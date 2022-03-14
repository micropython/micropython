/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
 * Copyright (c) 2021 Robert Hammelrath
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
#ifndef MICROPY_INCLUDED_MIMXRT_MODMACHINE_H
#define MICROPY_INCLUDED_MIMXRT_MODMACHINE_H

#include "py/obj.h"

extern const mp_obj_type_t machine_adc_type;
extern const mp_obj_type_t machine_i2c_type;
extern const mp_obj_type_t machine_i2s_type;
extern const mp_obj_type_t machine_pwm_type;
extern const mp_obj_type_t machine_rtc_type;
extern const mp_obj_type_t machine_sdcard_type;
extern const mp_obj_type_t machine_spi_type;
extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_uart_type;
extern const mp_obj_type_t machine_wdt_type;

void machine_adc_init(void);
void machine_pin_irq_deinit(void);
void machine_pwm_deinit_all(void);
void machine_timer_init_PIT(void);
void machine_sdcard_init0(void);
void mimxrt_sdram_init(void);
void machine_i2s_init0();
void machine_i2s_deinit_all(void);
void machine_rtc_start(void);

#endif // MICROPY_INCLUDED_MIMXRT_MODMACHINE_H
