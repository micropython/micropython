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
    uint16_t start_up_time_us;

    // Three response bytes to 0x9f JEDEC ID command.
    uint8_t manufacturer_id;
    uint8_t memory_type;
    uint8_t capacity;

    // Max clock speed for all operations and the fastest read mode.
    uint8_t max_clock_speed_mhz;

    // Bitmask for Quad Enable bit if present. 0x00 otherwise. This is for the highest byte in the
    // status register.
    uint8_t quad_enable_bit_mask;

    bool has_sector_protection : 1;

    // Supports the 0x0b fast read command with 8 dummy cycles.
    bool supports_fast_read : 1;

    // Supports the fast read, quad output command 0x6b with 8 dummy cycles.
    bool supports_qspi : 1;

    // Supports the quad input page program command 0x32. This is known as 1-1-4 because it only
    // uses all four lines for data.
    bool supports_qspi_writes: 1;

    // Requires a separate command 0x31 to write to the second byte of the status register.
    // Otherwise two byte are written via 0x01.
    bool write_status_register_split: 1;

    // True when the status register is a single byte. This implies the Quad Enable bit is in the
    // first byte and the Read Status Register 2 command (0x35) is unsupported.
    bool single_status_byte: 1;

    // Does not support using a ready bit within the status register
    bool no_ready_bit: 1;

    // Does not support the erase command (0x20)
    bool no_erase_cmd: 1;

    // Device does not have a reset command
    bool no_reset_cmd: 1;
} external_flash_device;

// Settings for the Adesto Tech AT25DF081A 1MiB SPI flash. It's on the SAMD21
// Xplained board.
// Datasheet: https://www.adestotech.com/wp-content/uploads/doc8715.pdf
#define AT25DF081A {\
    .total_size = (1 << 20), /* 1 MiB */ \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0x1f, \
    .memory_type = 0x45, \
    .capacity = 0x01, \
    .max_clock_speed_mhz = 85, \
    .quad_enable_bit_mask = 0x00, \
    .has_sector_protection = true, \
    .supports_fast_read = true, \
    .supports_qspi = false, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Adesto Tech AT25SF161-SSHD-T 2MiB SPI flash
// for the StringCar M0 (SAMD21) Express board.
// Source: https://www.digikey.com/product-detail/en/adesto-technologies/AT25SF161-SDHD-T/1265-1230-1-ND/
// Datasheet: https://www.adestotech.com/wpo-content/uploads/jDS-AT25SF161_046.pdf
#define AT25SF161 {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0x1f, \
    .memory_type = 0x86, \
    .capacity = 0x01, \
    .max_clock_speed_mhz = 85, \
    .quad_enable_bit_mask = 0x00, \
    .has_sector_protection = true, \
    .supports_fast_read = true, \
    .supports_qspi = false, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Adesto Tech AT25SF041 1MiB SPI flash. It's on the SparkFun
// SAMD51 Thing Plus board
// Datasheet: https://www.adestotech.com/wp-content/uploads/DS-AT25SF041_044.pdf
#define AT25SF041A {\
    .total_size = (1 << 19), /* 512 KiB */ \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0x1f, \
    .memory_type = 0x84, \
    .capacity = 0x01, \
    .max_clock_speed_mhz = 85, \
    .quad_enable_bit_mask = 0x00, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = false, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Gigadevice GD25Q16C 2MiB SPI flash.
// Datasheet: http://www.gigadevice.com/datasheet/gd25q16c/
#define GD25Q16C {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xc8, \
    .memory_type = 0x40, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 104, /* if we need 120 then we can turn on high performance mode */ \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Gigadevice GD25Q32C 4MiB SPI flash.
// Datasheet: http://www.elm-tech.com/en/products/spi-flash-memory/gd25q32/gd25q32.pdf
#define GD25Q32C {\
    .total_size = (1 << 22), /* 4 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xc8, \
    .memory_type = 0x40, \
    .capacity = 0x16, \
    .max_clock_speed_mhz = 104, /* if we need 120 then we can turn on high performance mode */ \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = true, \
    .single_status_byte = false, \
}

// Settings for the Gigadevice GD25Q64C 8MiB SPI flash.
// Datasheet: http://www.elm-tech.com/en/products/spi-flash-memory/gd25q64/gd25q64.pdf
#define GD25Q64C {\
    .total_size = (1 << 23), /* 8 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xc8, \
    .memory_type = 0x40, \
    .capacity = 0x17, \
    .max_clock_speed_mhz = 104, /* if we need 120 then we can turn on high performance mode */ \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = true, \
    .single_status_byte = false, \
}

// Settings for the Gigadevice GD25S512MD 64MiB SPI flash.
// Datasheet: http://www.gigadevice.com/datasheet/gd25s512md/
#define GD25S512MD {\
    .total_size = (1 << 26), /* 64 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xc8, \
    .memory_type = 0x40, \
    .capacity = 0x19, \
    .max_clock_speed_mhz = 104, /* if we need 120 then we can turn on high performance mode */ \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = true, \
    .single_status_byte = false, \
}

// Settings for the Cypress (was Spansion) S25FL064L 8MiB SPI flash.
// Datasheet: http://www.cypress.com/file/316661/download
#define S25FL064L {\
    .total_size = (1 << 23), /* 8 MiB */ \
    .start_up_time_us = 300, \
    .manufacturer_id = 0x01, \
    .memory_type = 0x60, \
    .capacity = 0x17, \
    .max_clock_speed_mhz = 108, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Cypress (was Spansion) S25FL116K 2MiB SPI flash.
// Datasheet: http://www.cypress.com/file/196886/download
#define S25FL116K {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0x01, \
    .memory_type = 0x40, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 108, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Cypress (was Spansion) S25FL216K 2MiB SPI flash.
// Datasheet: http://www.cypress.com/file/197346/download
#define S25FL216K {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0x01, \
    .memory_type = 0x40, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 65, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = false, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Winbond W25Q16FW 2MiB SPI flash.
// Datasheet: https://www.winbond.com/resource-files/w25q16fw%20revj%2005182017%20sfdp.pdf
#define W25Q16FW {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x60, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Winbond W25Q16JV-IQ 2MiB SPI flash. Note that JV-IM has a different .memory_type (0x70)
// Datasheet: https://www.winbond.com/resource-files/w25q16jv%20spi%20revf%2005092017.pdf
#define W25Q16JV_IQ {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x40, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Winbond W25Q16JV-IM 2MiB SPI flash. Note that JV-IQ has a different .memory_type (0x40)
// Datasheet: https://www.winbond.com/resource-files/w25q16jv%20spi%20revf%2005092017.pdf
#define W25Q16JV_IM {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x70, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
}

// Settings for the Winbond W25Q32BV 4MiB SPI flash.
// Datasheet: https://www.winbond.com/resource-files/w25q32bv_revi_100413_wo_automotive.pdf
#define W25Q32BV {\
    .total_size = (1 << 22), /* 4 MiB */ \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x60, \
    .capacity = 0x16, \
    .max_clock_speed_mhz = 104, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}
// Settings for the Winbond W25Q32JV-IM 4MiB SPI flash.
// Datasheet: https://www.winbond.com/resource-files/w25q32jv%20revg%2003272018%20plus.pdf
#define W25Q32JV_IM {\
    .total_size = (1 << 22), /* 4 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x70, \
    .capacity = 0x16, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
}

// Settings for the Winbond W25Q32JV-IQ 4MiB SPI flash.
// Datasheet: https://www.mouser.com/datasheet/2/949/w25q32jv_revg_03272018_plus-1489806.pdf
#define W25Q32JV_IQ {\
    .total_size = (1 << 22), /* 4 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x40, \
    .capacity = 0x16, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
}

// Settings for the Winbond W25Q64JV-IM 8MiB SPI flash. Note that JV-IQ has a different .memory_type (0x40)
// Datasheet: http://www.winbond.com/resource-files/w25q64jv%20revj%2003272018%20plus.pdf
#define W25Q64JV_IM {\
    .total_size = (1 << 23), /* 8 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x70, \
    .capacity = 0x17, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Winbond W25Q64JV-IQ 8MiB SPI flash. Note that JV-IM has a different .memory_type (0x70)
// Datasheet: http://www.winbond.com/resource-files/w25q64jv%20revj%2003272018%20plus.pdf
#define W25Q64JV_IQ {\
    .total_size = (1 << 23), /* 8 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x40, \
    .capacity = 0x17, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Winbond W25Q80DL 1MiB SPI flash.
// Datasheet: https://www.winbond.com/resource-files/w25q80dv%20dl_revh_10022015.pdf
#define W25Q80DL {\
    .total_size = (1 << 20), /* 1 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x60, \
    .capacity = 0x14, \
    .max_clock_speed_mhz = 104, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Winbond W25Q80DV 1MiB SPI flash.. Note that W25Q80DL has a different memory type (0x60)
// Datasheet: https://www.winbond.com/resource-files/w25q80dv%20dl_revh_10022015.pdf
#define W25Q80DV {\
    .total_size = (1 << 20), /* 1 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x40, \
    .capacity = 0x14, \
    .max_clock_speed_mhz = 104, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Winbond W25Q128JV-SQ 16MiB SPI flash. Note that JV-IM has a different .memory_type (0x70)
// Datasheet: https://www.winbond.com/resource-files/w25q128jv%20revf%2003272018%20plus.pdf
#define W25Q128JV_SQ {\
    .total_size = (1 << 24), /* 16 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x40, \
    .capacity = 0x18, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the Everspin MR20H40 / MR25H40 magnetic non-volatile RAM
// Datasheet: https://www.everspin.com/supportdocs/MR25H40CDFR
#define MR2xH40 {\
    .total_size = (1 << 22), /* 4 MiB */ \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0xef, /*no JDEC*/ \
    .memory_type = 0x40, /*no JDEC*/ \
    .capacity = 0x14, /*no JDEC*/ \
    .max_clock_speed_mhz = 10, \
    .quad_enable_bit_mask = 0x00, \
    .has_sector_protection = false, \
    .supports_fast_read = false, \
    .supports_qspi = false, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = true, \
    .no_ready_bit = true, \
    .no_erase_cmd = true, \
    .no_reset_cmd = true, \
}

// Settings for the Macronix MX25L1606 2MiB SPI flash.
// Datasheet:
#define MX25L1606  {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xc2, \
    .memory_type = 0x20, \
    .capacity = 0x15, \
    .max_clock_speed_mhz = 8, \
    .quad_enable_bit_mask = 0x40, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = true, \
}

// Settings for the Macronix MX25L3233F 4MiB SPI flash.
// Datasheet: http://www.macronix.com/Lists/Datasheet/Attachments/7426/MX25L3233F,%203V,%2032Mb,%20v1.6.pdf
#define MX25L3233F  {\
    .total_size = (1 << 22), /* 4 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xc2, \
    .memory_type = 0x20, \
    .capacity = 0x16, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x40, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = true, \
}

// Settings for the Macronix MX25R6435F 8MiB SPI flash.
// Datasheet: http://www.macronix.com/Lists/Datasheet/Attachments/7428/MX25R6435F,%20Wide%20Range,%2064Mb,%20v1.4.pdf
// By default its in lower power mode which can only do 8mhz. In high power mode it can do 80mhz.
#define MX25R6435F  {\
    .total_size = (1 << 23), /* 8 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xc2, \
    .memory_type = 0x28, \
    .capacity = 0x17, \
    .max_clock_speed_mhz = 8, \
    .quad_enable_bit_mask = 0x40, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = true, \
}

// Settings for the Macronix MX25R1635F 8MiB SPI flash.
// Datasheet: https://www.macronix.com/Lists/Datasheet/Attachments/7595/MX25R1635F,%20Wide%20Range,%2016Mb,%20v1.6.pdf
// In low power mode, quad operations can only run at 8 MHz.
#define MX25R1635F  {\
    .total_size = (1 << 21), /* 2 MiB */ \
    .start_up_time_us = 800, \
    .manufacturer_id = 0xc2, \
    .memory_type = 0x28, \
    .capacity = 0x18, \
    .max_clock_speed_mhz = 33, /* 8 mhz for dual/quad */ \
    .quad_enable_bit_mask = 0x80, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = true, \
}

// Settings for the Macronix MX25L51245G 64MiB SPI flash.
// Datasheet: https://www.macronix.com/Lists/Datasheet/Attachments/7437/MX25L51245G,%203V,%20512Mb,%20v1.6.pdf
#define MX25L51245G  {\
    .total_size = (1 << 26), /* 64 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xc2, \
    .memory_type = 0x20, \
    .capacity = 0x1a, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x40, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = true, \
}

// Settings for the Winbond W25Q128JV-PM 16MiB SPI flash. Note that JV-IM has a different .memory_type (0x70)
// Datasheet: https://www.winbond.com/resource-files/w25q128jv%20revf%2003272018%20plus.pdf
#define W25Q128JV_PM {\
    .total_size = (1 << 24), /* 16 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x70, \
    .capacity = 0x18, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
}

// Settings for the Winbond W25Q32FV 4MiB SPI flash.
// Datasheet:http://www.winbond.com/resource-files/w25q32fv%20revj%2006032016.pdf?__locale=en
#define W25Q32FV {\
    .total_size = (1 << 22), /* 4 MiB */ \
    .start_up_time_us = 5000, \
    .manufacturer_id = 0xef, \
    .memory_type = 0x40, \
    .capacity = 0x16, \
    .max_clock_speed_mhz = 104, \
    .quad_enable_bit_mask = 0x00, \
    .has_sector_protection = false, \
    .supports_fast_read = true, \
    .supports_qspi = false, \
    .supports_qspi_writes = false, \
    .write_status_register_split = false, \
    .single_status_byte = false, \
}

// Settings for the ISSI IS25LP128F 16MiB SPI flash.
// Datasheet: http://www.issi.com/WW/pdf/25LP-WP128F.pdf
#define IS25LP128F {\
    .total_size = (1 << 24), /* 16 MiB */ \
    .start_up_time_us = 10000, \
    .manufacturer_id = 0x9d, \
    .memory_type = 0x60, \
    .capacity = 0x18, \
    .max_clock_speed_mhz = 133, \
    .quad_enable_bit_mask = 0x02, \
    .has_sector_protection = true, \
    .supports_fast_read = true, \
    .supports_qspi = true, \
    .supports_qspi_writes = true, \
    .write_status_register_split = false, \
    .single_status_byte = true, \
}
#endif  // MICROPY_INCLUDED_ATMEL_SAMD_EXTERNAL_FLASH_DEVICES_H
