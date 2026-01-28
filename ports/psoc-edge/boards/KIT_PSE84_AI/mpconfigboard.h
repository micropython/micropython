/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// Board and hardware specific configuration
#define MICROPY_HW_MCU_NAME                     "PSOCE84"
#define MICROPY_HW_BOARD_NAME                   "KIT_PSE84_AI"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "KIT_PSE84_AI"

// External QSPI Flash Configuration
// These sizes are determined by the physical flash chip specifications
// and memory map layout for the KIT_PSE84_AI board
#define MICROPY_PY_EXT_FLASH (1)

// Flash memory map: Total 64MB (0x04000000) QSPI flash
// Base address starts at 9MB offset (0x00900000) - space reserved for firmware/bootloader
#define EXT_FLASH_BASE              (0x00900000)

// Usable filesystem space: 64MB - 9MB = 55MB (0x03700000 bytes)
#define EXT_FLASH_SIZE              (0x04000000 - EXT_FLASH_BASE)

// Erase sector size: Fixed by flash chip hardware (minimum erasable unit)
#define EXT_FLASH_SECTOR_SIZE       (0x00001000) /** 4KB */

// Block device block size: Must match erase sector size for proper filesystem operation
#define EXT_FLASH_BLOCK_SIZE_BYTES  (EXT_FLASH_SECTOR_SIZE)

// Program page size: Fixed by flash chip hardware (minimum writable unit)
#define EXT_FLASH_PAGE_SIZE         (0x00000100) /** 256 Bytes */
