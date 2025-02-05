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
#define MICROPY_HW_MCU_NAME                     "PSoC62"
#define MICROPY_HW_BOARD_NAME                   "CY8CPROTO-062-4343W"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "CY8C-062-4343W"

#define MICROPY_GC_HEAP_SIZE                    (256 * 1024) // 256 KB

#define MICROPY_PY_HASHLIB                      (1)
#define MICROPY_PY_HASHLIB_MD5                  (1)
#define MICROPY_PY_HASHLIB_SHA1                 (1)
#define MICROPY_PY_HASHLIB_SHA256               (1)

#define MICROPY_PY_MACHINE_SPI_SLAVE            (1)

#define MICROPY_PY_SD_CARD (1)
#if (MICROPY_PY_SD_CARD)
#define MAX_SDHC_SLOT                           (2)
#endif

// Flash type enablement for board
#define MICROPY_PY_EXT_FLASH (1)
#if (MICROPY_PY_EXT_FLASH)
#define EXT_FLASH_BASE                              (0x00)          /** 0x00 */
#define EXT_FLASH_SIZE                              (0x4000000)     /** 64MB */
#define EXT_FLASH_SECTOR_SIZE                       (0x40000)       /** 256KB*/
#define EXT_FLASH_BLOCK_SIZE_BYTES                  (EXT_FLASH_SECTOR_SIZE)
#define EXT_FLASH_PAGE_SIZE                         (0x200)         /** 512 bytes*/
#endif

// Board specific configurations
#define MAX_UART             10
#define MAX_TIMER            32
#define MAX_SPI              7
#define MAX_I2C              9
#define MAX_PWM_OBJS         32
#define MICROPY_HW_MAX_I2S   2
