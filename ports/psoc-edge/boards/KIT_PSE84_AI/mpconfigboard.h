/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2025 Infineon Technologies AG
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
// Move to 256KB sector region to avoid 128KB limitation
#define EXT_FLASH_BASE              (0x00920000)

// Usable filesystem space: ~54.5MB in 256KB sector region
#define EXT_FLASH_SIZE              (218 * 0x40000)

// Logical block size: 16KB (RAM efficient, hardware erases 256KB behind the scenes)
#define EXT_FLASH_SECTOR_SIZE        (0x40000)       /** 256KB*/

// Block device block size: Must match erase sector size for proper filesystem operation
#define EXT_FLASH_BLOCK_SIZE_BYTES  (EXT_FLASH_SECTOR_SIZE)

// Program page size: Fixed by flash chip hardware (minimum writable unit). Matches LittleFS write_size.
#define EXT_FLASH_PAGE_SIZE         (0x200) /** 512 Bytes */

// I2C Configuration
#define MICROPY_HW_I2C0_SCB                     (SCB5)
#define MICROPY_HW_I2C0_SCL_PORT                GPIO_PRT17
#define MICROPY_HW_I2C0_SCL_PIN                 P17_0_NUM
#define MICROPY_HW_I2C0_SCL_HSIOM               P17_0_SCB5_I2C_SCL
#define MICROPY_HW_I2C0_SDA_PORT                GPIO_PRT17
#define MICROPY_HW_I2C0_SDA_PIN                 P17_1_NUM
#define MICROPY_HW_I2C0_SDA_HSIOM               P17_1_SCB5_I2C_SDA
#define MICROPY_HW_I2C0_SCL                     (P17_0_NUM)
#define MICROPY_HW_I2C0_SDA                     (P17_1_NUM)
#define MAX_I2C                                 1
#define MICROPY_HW_I2C_INTR_PRIORITY            (7UL)
#define MICROPY_HW_I2C_PCLK                     PCLK_SCB5_CLOCK_SCB_EN
#define MICROPY_HW_I2C_IRQn                     scb_5_interrupt_IRQn
