/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Infineon Technologies AG
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

// std includes
#include <stdio.h>
#include <unistd.h>


// micropython includes
#include "py/mpconfig.h"
#include "py/runtime.h"
#include "machine_pin.h"

#define mp_hal_delay_us_fast mp_hal_delay_us

void mp_hal_delay_us(mp_uint_t us);
void mp_hal_delay_us_fast(mp_uint_t us);
void mp_hal_delay_ms(mp_uint_t ms);

uint64_t mp_hal_time_ns(void);

mp_uint_t mp_hal_ticks_us(void);
mp_uint_t mp_hal_ticks_ms(void);
mp_uint_t mp_hal_ticks_cpu(void);

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags);
int mp_hal_stdin_rx_chr(void);
void mp_hal_set_interrupt_char(int c); // -1 to disable

static inline mp_uint_t mp_hal_get_cpu_freq(void) {
    return 1000000; // 1 MHz, this is a placeholder value
}

#define MP_HAL_PIN_FMT          "%q"
#define mp_hal_pin_obj_t        const machine_pin_obj_t *
#define mp_hal_get_pin_obj(o)   machine_pin_get_pin_obj(o)
#define mp_hal_pin_name(p)      ((p)->name)
#define mp_hal_pin_input(p)     mp_hal_pin_config((p), GPIO_MODE_IN, GPIO_PULL_NONE, 0)
#define mp_hal_pin_output(p)    mp_hal_pin_config((p), GPIO_MODE_OUT, GPIO_PULL_NONE, 0)
#define mp_hal_pin_open_drain(p) mp_hal_pin_config((p), GPIO_MODE_OPEN_DRAIN, GPIO_PULL_NONE, 0)
#define mp_hal_pin_high(p)      mp_hal_pin_write((p), 1)
#define mp_hal_pin_low(p)       mp_hal_pin_write((p), 0)
#define mp_hal_pin_od_low(p)    mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p)   mp_hal_pin_high(p)

void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull);
void mp_hal_pin_write(mp_hal_pin_obj_t pin, uint8_t polarity);
uint32_t mp_hal_pin_read(mp_hal_pin_obj_t pin);

uint32_t mp_hal_pin_get_drive(mp_hal_pin_obj_t pin);
void mp_hal_pin_set_drive(mp_hal_pin_obj_t pin, uint32_t drive);
