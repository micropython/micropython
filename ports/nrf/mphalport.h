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

#include "py/mpconfig.h"
#include "py/ringbuf.h"
#include <nrfx.h>
#include "pin.h"
#include "nrf_gpio.h"
#include "nrfx_config.h"
#include "shared/runtime/interrupt_char.h"

typedef enum
{
    HAL_OK       = 0x00,
    HAL_ERROR    = 0x01,
    HAL_BUSY     = 0x02,
    HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

extern const unsigned char mp_hal_status_to_errno_table[4];

extern ringbuf_t stdin_ringbuf;

NORETURN void mp_hal_raise(HAL_StatusTypeDef status);
void mp_hal_set_interrupt_char(int c); // -1 to disable

int mp_hal_stdin_rx_chr(void);
void mp_hal_stdout_tx_str(const char *str);

static inline void mp_hal_wake_main_task_from_isr(void) {
    // Defined for tinyusb support, nothing needs to be done here.
}

void mp_hal_delay_ms(mp_uint_t ms);
void mp_hal_delay_us(mp_uint_t us);

const char *nrfx_error_code_lookup(uint32_t err_code);

void mp_nrf_start_lfclk(void);

#define MP_HAL_PIN_FMT           "%q"
#define mp_hal_pin_obj_t const pin_obj_t *
#define mp_hal_get_pin_obj(o)    pin_find(o)
#define mp_hal_pin_name(p)       ((p)->name)
#define mp_hal_pin_high(p)       nrf_gpio_pin_set(p->pin)
#define mp_hal_pin_low(p)        nrf_gpio_pin_clear(p->pin)
#define mp_hal_pin_read(p)       (nrf_gpio_pin_dir_get(p->pin) == NRF_GPIO_PIN_DIR_OUTPUT) ? nrf_gpio_pin_out_read(p->pin) : nrf_gpio_pin_read(p->pin)
#define mp_hal_pin_write(p, v)   ((v) ? mp_hal_pin_high(p) : mp_hal_pin_low(p))
#define mp_hal_pin_od_low(p)     mp_hal_pin_low(p)
#define mp_hal_pin_od_high(p)    mp_hal_pin_high(p)
#define mp_hal_pin_open_drain(p) nrf_gpio_cfg_input(p->pin, NRF_GPIO_PIN_NOPULL)

#if MICROPY_PY_TIME_TICKS
void rtc1_init_time_ticks();
#else
mp_uint_t mp_hal_ticks_ms(void);
#define mp_hal_ticks_us() (0)
#endif

// TODO: empty implementation for now. Used by machine_spi.c:69
#define mp_hal_delay_us_fast(p)
#define mp_hal_ticks_cpu() (0)

#endif
