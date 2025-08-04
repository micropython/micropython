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


// MTB includes
#include "cyhal.h"


// port-specific includes

#define MICROPY_BEGIN_ATOMIC_SECTION()     cyhal_system_critical_section_enter()
#define MICROPY_END_ATOMIC_SECTION(state)  cyhal_system_critical_section_exit(state)

// #define MICROPY_BEGIN_ATOMIC_SECTION()      (0)
// #define MICROPY_END_ATOMIC_SECTION(state)   {(void)state;}

#define MP_HAL_PIN_FMT   "%u"
#define mp_hal_pin_obj_t uint

#define mp_hal_delay_us_fast mp_hal_delay_us


extern __attribute__((weak)) int _write(int fd, const char *ptr, int len);


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


int mp_hal_pin_read(mp_hal_pin_obj_t pin);
uint8_t mp_hal_pin_name(mp_hal_pin_obj_t pin);
mp_hal_pin_obj_t mp_hal_get_pin_obj(mp_obj_t obj);

void mp_hal_pin_write(mp_hal_pin_obj_t pin, uint8_t polarity);
void mp_hal_pin_output(mp_hal_pin_obj_t pin);
void mp_hal_pin_input(mp_hal_pin_obj_t pin);

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags);

int mp_hal_stdin_rx_chr(void);

static inline mp_uint_t mp_hal_get_cpu_freq(void) {
    return Cy_SysClk_ClkFastGetFrequency();
}

static inline void mp_hal_pin_config(mp_hal_pin_obj_t pin, uint32_t mode, uint32_t pull, uint32_t alt) {
    printf("mp_hal_pin_config %d   mode : %ld   pull : %ld   alt : %ld\n", pin, mode, pull, alt);
    cyhal_gpio_configure(pin, mode, pull);

    // assert((mode == MP_HAL_PIN_MODE_INPUT || mode == MP_HAL_PIN_MODE_OUTPUT) && alt == 0);
    // gpio_set_dir(pin, mode);
    // gpio_set_pulls(pin, pull == MP_HAL_PIN_PULL_UP, pull == MP_HAL_PIN_PULL_DOWN);
}
