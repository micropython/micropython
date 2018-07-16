/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
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

#ifndef __NRF52_HAL
#define __NRF52_HAL

#include <stdbool.h>
#include <stdint.h>

#include "lib/utils/interrupt_char.h"
#include "nrfx_uart.h"
#include "py/mpconfig.h"

extern nrfx_uart_t serial_instance;

extern volatile uint64_t ticks_ms;

static inline mp_uint_t mp_hal_ticks_ms(void) {
    return ticks_ms;
}

int mp_hal_stdin_rx_chr(void);
void mp_hal_stdout_tx_str(const char *str);
bool mp_hal_stdin_any(void);
void mp_hal_delay_ms(mp_uint_t ms);
#define mp_hal_delay_us(us)     NRFX_DELAY_US((uint32_t) (us))

#endif
