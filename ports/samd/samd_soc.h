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

void samd_init(void);
void samd_main(void);

void USB_Handler_wrapper(void);
void USB_0_Handler_wrapper(void);
void USB_1_Handler_wrapper(void);
void USB_2_Handler_wrapper(void);
void USB_3_Handler_wrapper(void);

void sercom_enable(Sercom *spi, int state);
void sercom_register_irq(int sercom_id, void (*sercom_irq_handler));

#define SERCOM_IRQ_TYPE_UART  (0)
#define SERCOM_IRQ_TYPE_SPI   (1)
#define SERCOM_IRQ_TYPE_I2C   (2)

#endif // MICROPY_INCLUDED_SAMD_SAMD_SOC_H
