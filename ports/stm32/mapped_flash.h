/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Kwabena W. Agyeman
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
#ifndef MICROPY_INCLUDED_STM32_MAPPED_FLASH_H
#define MICROPY_INCLUDED_STM32_MAPPED_FLASH_H

#include "octospi.h"
#include "qspi.h"
#include "xspi.h"

// Whether addr is in memory-mapped external flash, as the flash drivers map it.  Such memory is
// read through a flash controller that other code, like USB MSC reading a filesystem on the same
// flash from its IRQ, can take out of memory-mapped mode at any time: a DMA reading it then fails
// or stalls.  Memory-mapped RAM (eg PSRAM on another XSPI) is not flash and isn't included.
static inline bool mapped_flash_is_valid_addr(uint32_t addr) {
    #if defined(MICROPY_HW_QSPIFLASH_SIZE_BITS_LOG2)
    if (qspi_is_valid_addr(addr)) {
        return true;
    }
    #endif
    #if defined(MICROPY_HW_OSPIFLASH_SIZE_BITS_LOG2)
    if (octospi_is_valid_addr(addr)) {
        return true;
    }
    #endif
    #if defined(MICROPY_HW_XSPIFLASH_SIZE_BITS_LOG2)
    if (xspi_is_valid_addr(&xspi_flash2, addr)) {       // the XSPI that xspi.c drives as flash
        return true;
    }
    #endif
    (void)addr;
    return false;
}

#endif // MICROPY_INCLUDED_STM32_MAPPED_FLASH_H
