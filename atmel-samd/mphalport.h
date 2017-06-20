/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#ifndef __MICROPY_INCLUDED_ATMEL_SAMD_MPHALPORT_H__
#define __MICROPY_INCLUDED_ATMEL_SAMD_MPHALPORT_H__

#include "py/obj.h"

#include "lib/oofatfs/ff.h"

#define USB_RX_BUF_SIZE 128

// Global millisecond tick count (driven by SysTick interrupt).
extern volatile uint64_t ticks_ms;

static inline mp_uint_t mp_hal_ticks_ms(void) {
  return ticks_ms;
}
// Number of bytes in receive buffer
volatile uint8_t usb_rx_count;
volatile bool mp_cdc_enabled;

FIL* boot_output_file;

int receive_usb(void);

void mp_hal_set_interrupt_char(int c);

void mp_hal_disable_all_interrupts(void);

void mp_hal_enable_all_interrupts(void);

#endif // __MICROPY_INCLUDED_ATMEL_SAMD_MPHALPORT_H__
