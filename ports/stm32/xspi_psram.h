/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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
#ifndef MICROPY_INCLUDED_STM32_XSPI_PSRAM_H
#define MICROPY_INCLUDED_STM32_XSPI_PSRAM_H

// The XSPI1 memory-mapped window, where the PSRAM is mapped.
#define XSPI_PSRAM_BASE (0x90000000)
#define XSPI_PSRAM_END (0xa0000000)

// Supported PSRAM types, for MICROPY_HW_XSPI_PSRAM_TYPE.
#define XSPI_PSRAM_TYPE_APMEM_16BITS (1) // AP Memory APS256XX-style

// Initialise the PSRAM and map it at XSPI_PSRAM_BASE.  Returns its size in bytes, or 0 if
// it couldn't be initialised or identified.
size_t xspi_psram_init(void);

#endif // MICROPY_INCLUDED_STM32_XSPI_PSRAM_H
