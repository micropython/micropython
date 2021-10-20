/*
 * Based largely on examples provided by NXP:
 *
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Port-specific extensions and adaptions:
 *
 * The MIT License (MIT)
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Copyright (c) 2021 Damien P. George
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
#include <assert.h>
#include "fsl_common.h"
#include "flexspi_nor_flash.h"

void flexspi_nor_reset(FLEXSPI_Type *base) __attribute__((section(".ram_functions")));
void flexspi_nor_reset(FLEXSPI_Type *base) {
    // Using content of FLEXSPI_SoftwareReset directly to prevent issues when compiler does not inline function
    base->MCR0 |= FLEXSPI_MCR0_SWRESET_MASK;
    while (base->MCR0 & FLEXSPI_MCR0_SWRESET_MASK) {
    }
}

status_t flexspi_nor_write_enable(FLEXSPI_Type *base, uint32_t baseAddr) __attribute__((section(".ram_functions")));
status_t flexspi_nor_write_enable(FLEXSPI_Type *base, uint32_t baseAddr) {
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write neable */
    flashXfer.deviceAddress = baseAddr;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

status_t flexspi_nor_wait_bus_busy(FLEXSPI_Type *base) __attribute__((section(".ram_functions")));
status_t flexspi_nor_wait_bus_busy(FLEXSPI_Type *base) {
    /* Wait status ready. */
    bool isBusy;
    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READSTATUSREG;
    flashXfer.data = &readValue;
    flashXfer.dataSize = 1;

    do {
        status = FLEXSPI_TransferBlocking(base, &flashXfer);

        if (status != kStatus_Success) {
            return status;
        }
        if (FLASH_BUSY_STATUS_POL) {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET)) {
                isBusy = false;
            } else {
                isBusy = true;
            }
        } else {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET)) {
                isBusy = true;
            } else {
                isBusy = false;
            }
        }
    } while (isBusy);

    return status;
}

status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base) __attribute__((section(".ram_functions")));
status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base) {
    flexspi_transfer_t flashXfer;
    status_t status;
    uint32_t writeValue = 0x40;

    /* Write neable */
    status = flexspi_nor_write_enable(base, 0);

    if (status != kStatus_Success) {
        return status;
    }

    /* Enable quad mode. */
    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Write;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG;
    flashXfer.data = &writeValue;
    flashXfer.dataSize = 1;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);
    if (status != kStatus_Success) {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address) __attribute__((section(".ram_functions")));
status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address) {
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = flexspi_nor_write_enable(base, address);

    if (status != kStatus_Success) {
        return status;
    }

    /* Erase sector */
    flashXfer.deviceAddress = address;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR;
    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success) {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    flexspi_nor_reset(base);

    return status;
}

status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t dstAddr, const uint32_t *src, uint32_t size) __attribute__((section(".ram_functions")));
status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t dstAddr, const uint32_t *src, uint32_t size) {
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = flexspi_nor_write_enable(base, dstAddr);

    if (status != kStatus_Success) {
        return status;
    }

    /* Prepare page program command */
    flashXfer.deviceAddress = dstAddr;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Write;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD;
    flashXfer.data = (uint32_t *)src;
    flashXfer.dataSize = size;
    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success) {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    flexspi_nor_reset(BOARD_FLEX_SPI);

    return status;
}

status_t flexspi_nor_get_vendor_id(FLEXSPI_Type *base, uint8_t *vendorId) __attribute__((section(".ram_functions")));
status_t flexspi_nor_get_vendor_id(FLEXSPI_Type *base, uint8_t *vendorId) {
    uint32_t temp;
    flexspi_transfer_t flashXfer;
    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READID;
    flashXfer.data = &temp;
    flashXfer.dataSize = 2;

    status_t status = FLEXSPI_TransferBlocking(base, &flashXfer);

    *vendorId = temp;

    return status;
}
