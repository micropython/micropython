/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Philipp Ebensberger
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

#include "flash.h"

void flash_init(void) {
    // Upload the custom flash configuration
    // And fix the entry for PAGEPROGRAM_QUAD
    // Update the flash CLK
    flexspi_nor_update_lut_clk(MICROPY_HW_FLASH_CLK);

    // Configure FLEXSPI IP FIFO access.
    BOARD_FLEX_SPI->MCR0 &= ~(FLEXSPI_MCR0_ARDFEN_MASK);
    BOARD_FLEX_SPI->MCR0 &= ~(FLEXSPI_MCR0_ATDFEN_MASK);
    BOARD_FLEX_SPI->MCR0 |= FLEXSPI_MCR0_ARDFEN(0);
    BOARD_FLEX_SPI->MCR0 |= FLEXSPI_MCR0_ATDFEN(0);
}

// flash_erase_block(erase_addr)
// erases the block starting at addr. Block size according to the flash properties.
__attribute__((section(".ram_functions"))) status_t flash_erase_block(uint32_t erase_addr) {
    status_t status = kStatus_Fail;

    __disable_irq();
    SCB_DisableDCache();

    status = flexspi_nor_flash_erase_block(BOARD_FLEX_SPI, erase_addr);

    SCB_EnableDCache();
    __enable_irq();

    return status;
}

// flash_erase_sector(erase_addr_bytes)
// erases the sector starting at addr. Sector size according to the flash properties.
__attribute__((section(".ram_functions"))) status_t flash_erase_sector(uint32_t erase_addr) {
    status_t status = kStatus_Fail;

    __disable_irq();
    SCB_DisableDCache();

    status = flexspi_nor_flash_erase_sector(BOARD_FLEX_SPI, erase_addr);

    SCB_EnableDCache();
    __enable_irq();

    return status;
}

// flash_write_block(flash_dest_addr_bytes, data_source, length_bytes)
// writes length_byte data to the destination address
// the vfs driver takes care for erasing the sector if required
__attribute__((section(".ram_functions"))) status_t flash_write_block(uint32_t dest_addr, const uint8_t *src, uint32_t length) {
    status_t status = kStatus_Fail;
    uint32_t write_length;
    uint32_t next_addr;

    if (length == 0) {
        status = kStatus_Success;  // Nothing to do
    } else {
        // write data in chunks not crossing a page boundary
        do {
            next_addr = dest_addr - (dest_addr % PAGE_SIZE_BYTES) + PAGE_SIZE_BYTES; // next page boundary
            write_length = next_addr - dest_addr;  // calculate write length based on destination address and subsequent page boundary.
            if (write_length > length) { // compare possible write_length against remaining data length
                write_length = length;
            }

            __disable_irq();
            SCB_DisableDCache();

            status = flexspi_nor_flash_page_program(BOARD_FLEX_SPI, dest_addr, (uint32_t *)src, write_length);

            SCB_EnableDCache();
            __enable_irq();

            // Update remaining data length
            length -= write_length;

            // Move source and destination pointer
            src += write_length;
            dest_addr += write_length;
        } while ((length > 0) && (status == kStatus_Success));
    }
    return status;
}

// flash_read_block(flash_src_addr_bytes, data_dest, length_bytes)
// read length_byte data to the source address
// It is just a shim to provide the same structure for read_block and write_block.
__attribute__((section(".ram_functions"))) void flash_read_block(uint32_t src_addr, uint8_t *dest, uint32_t length) {
    memcpy(dest, (const uint8_t *)(BOARD_FLEX_SPI_ADDR_BASE + src_addr), length);
}
