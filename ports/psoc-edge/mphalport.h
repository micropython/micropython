/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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


#define MP_HAL_PIN_FMT   "%u"
#define mp_hal_pin_obj_t uint

#define mp_hal_delay_us_fast mp_hal_delay_us

void mp_hal_delay_us(mp_uint_t us);
void mp_hal_delay_us_fast(mp_uint_t us);
void mp_hal_delay_ms(mp_uint_t ms);

uint64_t mp_hal_time_ns(void);

mp_uint_t mp_hal_ticks_us(void);
mp_uint_t mp_hal_ticks_ms(void);
mp_uint_t mp_hal_ticks_cpu(void);


void mp_hal_pin_od_low(mp_hal_pin_obj_t pin);
void mp_hal_pin_od_high(mp_hal_pin_obj_t pin);
void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin);
void mp_hal_set_interrupt_char(int c); // -1 to disable

int mp_hal_pin_read(mp_hal_pin_obj_t pin);
uint8_t mp_hal_pin_name(mp_hal_pin_obj_t pin);
mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj);

void mp_hal_pin_write(mp_hal_pin_obj_t pin, uint8_t polarity);
void mp_hal_pin_output(mp_hal_pin_obj_t pin);
void mp_hal_pin_input(mp_hal_pin_obj_t pin);

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags);

int mp_hal_stdin_rx_chr(void);

static inline mp_uint_t mp_hal_get_cpu_freq(void) {
    return 1000000; // 1 MHz, this is a placeholder value
}

static inline void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t alt) {
    printf("mp_hal_pin_config %d   mode : %ld   pull : %ld   alt : %ld\n", pin, mode, pull, alt);

}
