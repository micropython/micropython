/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_XSPI_H
#define MICROPY_INCLUDED_STM32_XSPI_H

#include "drivers/bus/qspi.h"

typedef struct _xspi_flash_t xspi_flash_t;

extern const mp_qspi_proto_t xspi_proto;
extern const xspi_flash_t xspi_flash1;
extern const xspi_flash_t xspi_flash2;

void xspi_init(void);
uint32_t xspi_get_xip_base(const xspi_flash_t *self);
bool xspi_is_valid_addr(const xspi_flash_t *self, uint32_t addr);
void xspi_switch_to_spi(void);
void xspi_switch_to_dtr(void);

#endif // MICROPY_INCLUDED_STM32_XSPI_H
