/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_SAMD_SAMD_SOC_H
#define MICROPY_INCLUDED_SAMD_SAMD_SOC_H

#include <stdint.h>
#include "sam.h"
#include "clock_config.h"

typedef struct _samd_unique_id_t {
    uint8_t bytes[16];
} samd_unique_id_t;

extern Sercom *sercom_instance[];

void samd_init(void);
void samd_main(void);

void USB_Handler_wrapper(void);

void sercom_enable(Sercom *spi, int state);
void sercom_register_irq(int sercom_id, void (*sercom_irq_handler));

// Each device has a unique 128-bit serial number. The uniqueness of the serial number is
// guaranteed only when using all 128 bits.
void samd_get_unique_id(samd_unique_id_t *id);

#define SERCOM_IRQ_TYPE_UART  (0)
#define SERCOM_IRQ_TYPE_SPI   (1)
#define SERCOM_IRQ_TYPE_I2C   (2)

#endif // MICROPY_INCLUDED_SAMD_SAMD_SOC_H
