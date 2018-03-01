/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries LLC
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
#ifndef MICROPY_INCLUDED_ATMEL_SAMD_EXTERNAL_FLASH_DEVICES_H
#define MICROPY_INCLUDED_ATMEL_SAMD_EXTERNAL_FLASH_DEVICES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t total_size;
    uint16_t erase_size;
    uint16_t page_size;
    uint16_t start_up_time_us;
    uint8_t manufacturer_id;
    uint8_t memory_type;
    uint8_t capacity;
    uint8_t max_clock_speed_mhz;
    bool has_sector_protection;
    bool supports_qspi;
    bool supports_qspi_writes;
} external_flash_device;

// Settings for the Adesto Tech AT25DF081A 1MiB SPI flash. Its on the SAMD21
// Xplained board.
// Datasheet: https://www.adestotech.com/wp-content/uploads/doc8715.pdf
#define AT25DF081A {\
    .total_size = (1 << 20), /* 1 MiB */ \
    .erase_size = (1 << 12), /* 4 KiB */ \
    .page_size = 256,        /*256 bytes */  \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0x1f, \
    .memory_type = 0x45, \
    .capacity = 0x01, \
    .max_clock_speed_mhz = 104, \
    .has_sector_protection = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
}

// Settings for the Gigadevice GD25Q16C 2MiB SPI flash.
// Datasheet: http://www.gigadevice.com/wp-content/uploads/2017/12/DS-00086-GD25Q16C-Rev2.6.pdf

#define GD25Q16C {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .erase_size = (1 << 12), /* 4 KiB */ \
    .page_size = 256,        /*256 bytes */  \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xc8, \
    .memory_type = 0x40, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 104, \
    .has_sector_protection = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
}

// Settings for the Cypress (was Spansion) S25FL064L 8MiB SPI flash.
// Datasheet: http://www.cypress.com/file/316661/download
#define S25FL064L {\
    .total_size = (1 << 23), /* 8 MiB */ \
    .erase_size = (1 << 12), /* 4 KiB */ \
    .page_size = 256,        /*256 bytes */  \
    .start_up_time_us = 300, \
    .manufacturer_id = 0x01, \
    .memory_type = 0x60, \
    .capacity = 0x17, \
    .max_clock_speed_mhz = 104, \
    .has_sector_protection = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
}

// Settings for the Cypress (was Spansion) S25FL116K 2MiB SPI flash.
// Datasheet: http://www.cypress.com/file/196886/download
#define S25FL116K {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .erase_size = (1 << 12), /* 4 KiB */ \
    .page_size = 256,        /*256 bytes */  \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0x01, \
    .memory_type = 0x40, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 104, \
    .has_sector_protection = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
}


// Settings for the Cypress (was Spansion) S25FL216K 2MiB SPI flash.
// Datasheet: http://www.cypress.com/file/197346/download

#define S25FL216K {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .erase_size = (1 << 12), /* 4 KiB */ \
    .page_size = 256,        /*256 bytes */  \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0x01, \
    .memory_type = 0x40, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 104, \
    .has_sector_protection = false, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
}

// Settings for the Winbond W25Q16FW 2MiB SPI flash.
// Datasheet: https://www.winbond.com/resource-files/w25q16fw%20revj%2005182017%20sfdp.pdf

#define W25Q16FW {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .erase_size = (1 << 12), /* 4 KiB */ \
    .page_size = 256,        /*256 bytes */  \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x60, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 104, \
    .has_sector_protection = false, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
}

// Settings for the Winbond W25Q32BV 2MiB SPI flash.
// Datasheet: https://www.winbond.com/resource-files/w25q32bv_revi_100413_wo_automotive.pdf

#define W25Q32BV {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .erase_size = (1 << 12), /* 4 KiB */ \
    .page_size = 256,        /*256 bytes */  \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x60, \
    .capacity = 0x16, \
    .max_clock_speed_mhz = 104, \
    .has_sector_protection = false, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
}

// Settings for the Winbond W25Q80DL 1MiB SPI flash.
// Datasheet: https://www.winbond.com/resource-files/w25q80dv%20dl_revh_10022015.pdf
#define W25Q80DL {\
    .total_size = (1 << 20), /* 1 MiB */ \
    .erase_size = (1 << 12), /* 4 KiB */ \
    .page_size = 256,        /*256 bytes */  \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x60, \
    .capacity = 0x14, \
    .max_clock_speed_mhz = 104, \
    .has_sector_protection = false, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
}

#endif  // MICROPY_INCLUDED_ATMEL_SAMD_EXTERNAL_FLASH_DEVICES_H
