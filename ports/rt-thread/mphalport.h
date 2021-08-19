/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Armink (armink.ztl@gmail.com)
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

#include <rtthread.h>
#include <drivers/pin.h>

#define MP_HAL_PIN_FMT                 "%s"

extern void mp_hal_set_interrupt_char(int c);
extern void mp_pin_od_write(void *machine_pin, int stat);
extern void mp_hal_pin_open_set(void *machine_pin, int mode);
extern void mp_hal_stdout_tx_strn_stream(const char *str, size_t len);

#define mp_hal_quiet_timing_enter()         MICROPY_BEGIN_ATOMIC_SECTION()
#define mp_hal_quiet_timing_exit(irq_state) MICROPY_END_ATOMIC_SECTION(irq_state)

// needed for machine.I2C
#define mp_hal_delay_us_fast(us) mp_hal_delay_us(us)
#define mp_hal_pin_od_low(pin)   mp_pin_od_write(pin, PIN_LOW)
#define mp_hal_pin_od_high(pin)  mp_pin_od_write(pin, PIN_HIGH)
#define mp_hal_pin_open_drain(p) mp_hal_pin_open_set(p, PIN_MODE_OUTPUT_OD)

// needed for soft machine.SPI
#define mp_hal_pin_output(p)     mp_hal_pin_open_set(p, PIN_MODE_OUTPUT)
#define mp_hal_pin_input(p)      mp_hal_pin_open_set(p, PIN_MODE_INPUT)
#define mp_hal_pin_name(p)       mp_hal_pin_get_name(p)
#define mp_hal_pin_high(p)       mp_hal_pin_write(p, 1)

