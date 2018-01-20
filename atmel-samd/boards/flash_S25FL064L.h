/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2017 Dave Astels
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_BOARD_FLASH_S25FL064L_H
#define MICROPY_INCLUDED_ATMEL_SAMD_BOARD_FLASH_S25FL064L_H

// Settings for the Cypress (was Spansion) S25FL064L 8MiB SPI flash.
// Datasheet: http://www.cypress.com/file/316661/download

// The total flash size in bytes.
#define SPI_FLASH_TOTAL_SIZE  (1 << 23) // 8 MiB

// The size of the smallest erase unit thats erased with command 0x20.
#define SPI_FLASH_ERASE_SIZE  (1 << 12) // 4 KiB

// The size of a page that is programmed with page program command 0x02.
#define SPI_FLASH_PAGE_SIZE   (256)     // 256 bytes

// These are the first three response bytes to the JEDEC ID command 0x9f that is
// used to confirm we're talking to the flash we expect.
#ifndef SPI_FLASH_JEDEC_MANUFACTURER
#define SPI_FLASH_JEDEC_MANUFACTURER 0x01
#define SPI_FLASH_SECTOR_PROTECTION false
#define SPI_FLASH_JEDEC_MEMORY_TYPE  0x60
#else
#define SPI_FLASH_JEDEC_MANUFACTURER_2 0x013
#define SPI_FLASH_SECTOR_PROTECTION_2 false
#define SPI_FLASH_JEDEC_MEMORY_TYPE_2  0x60
#endif
#define SPI_FLASH_JEDEC_CAPACITY     0x17

#endif  // MICROPY_INCLUDED_ATMEL_SAMD_BOARD_FLASH_S25FL216K_H
