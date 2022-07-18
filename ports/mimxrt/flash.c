/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

// --------------------------------------------------------------------+
// Global Function Definitions
// --------------------------------------------------------------------+
void flash_init(void) {
    // Upload the custom flash configuration
    // This should be performed by the boot ROM but for some reason it is not.
    FLEXSPI_UpdateLUT(FLEXSPI, 0,
        qspiflash_config.memConfig.lookupTable,
        ARRAY_SIZE(qspiflash_config.memConfig.lookupTable));

    // Configure FLEXSPI IP FIFO access.
    FLEXSPI->MCR0 &= ~(FLEXSPI_MCR0_ARDFEN_MASK);
    FLEXSPI->MCR0 &= ~(FLEXSPI_MCR0_ATDFEN_MASK);
    FLEXSPI->MCR0 |= FLEXSPI_MCR0_ARDFEN(0);
    FLEXSPI->MCR0 |= FLEXSPI_MCR0_ATDFEN(0);

    FLEXSPI_EnableIPParallelMode(FLEXSPI, true);
}

// flash_erase_block(erase_addr)
// erases the block starting at addr. Block size according to the flash properties.
status_t flash_erase_block(uint32_t erase_addr) {
    status_t status = kStatus_Fail;

    SCB_CleanInvalidateDCache();
    SCB_DisableDCache();
    __disable_irq();

    status = flexspi_nor_flash_erase_block(FLEXSPI, erase_addr);

    __enable_irq();
    SCB_EnableDCache();

    return status;
}

// flash_erase_sector(erase_addr_bytes)
// erases the sector starting at addr. Sector size according to the flash properties.
status_t flash_erase_sector(uint32_t erase_addr) {
    status_t status = kStatus_Fail;

    SCB_CleanInvalidateDCache();
    SCB_DisableDCache();
    __disable_irq();

    status = flexspi_nor_flash_erase_sector(FLEXSPI, erase_addr);

    __enable_irq();
    SCB_EnableDCache();

    return status;
}

// flash_write_block(flash_dest_addr_bytes, data_source, length_bytes)
// writes length_byte data to the destination address
// the vfs driver takes care for erasing the sector if required
status_t flash_write_block(uint32_t dest_addr, const uint8_t *src, uint32_t length) {
    #if FLASH_DISABLE_OP == 1
    return kStatus_Success;
    #else
    status_t status = kStatus_Fail;
    uint32_t size;
    uint32_t next_addr;

    if (length == 0) {
        status = kStatus_Success;  // Nothing to do
    } else {

        SCB_CleanInvalidateDCache();
        SCB_DisableDCache();

        // write data in chunks not crossing a page boundary
        while (length > 0) {
            next_addr = dest_addr - (dest_addr % PAGE_SIZE_BYTES) + PAGE_SIZE_BYTES; // next page boundary
            size = next_addr - dest_addr;  // maximal chunk length
            if (size > length) { // compare against remaining data size
                size = length;
            }

            __disable_irq();

            status = flexspi_nor_flash_page_program(FLEXSPI, dest_addr, (uint32_t *)src, size);

            __enable_irq();

            if (status != kStatus_Success) {
                break;
            }
            length -= size;
            src += size;
            dest_addr += size;
        }

        SCB_EnableDCache();

    }
    return status;
    #endif
}
