/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"
#include "flexspi_hyper_flash.h"

// Copy of a few (pseudo-)functions from fsl_clock.h, which were nor reliably
// inlined when they should be. That caused DEBUG mode to fail.
// It does not increase the code size, since they were supposed to be inline.

__attribute__((always_inline)) static inline void clock_set_div(clock_div_t divider, uint32_t value) {
    uint32_t busyShift;

    busyShift                   = CCM_TUPLE_BUSY_SHIFT(divider);
    CCM_TUPLE_REG(CCM, divider) = (CCM_TUPLE_REG(CCM, divider) & (~CCM_TUPLE_MASK(divider))) |
                                  (((uint32_t)((value) << CCM_TUPLE_SHIFT(divider))) & CCM_TUPLE_MASK(divider));

    /* Clock switch need Handshake? */
    if (CCM_NO_BUSY_WAIT != busyShift) {
        /* Wait until CCM internal handshake finish. */
        while (CCM->CDHIPR & (1U << busyShift)) {}
    }
}

__attribute__((always_inline)) static inline void clock_control_gate(clock_ip_name_t name, clock_gate_value_t value) {
    uint32_t index = ((uint32_t)name) >> 8U;
    uint32_t shift = ((uint32_t)name) & 0x1FU;
    volatile uint32_t *reg;

    reg  = ((volatile uint32_t *)&CCM->CCGR0) + index;
    *reg = ((*reg) & ~(3U << shift)) | (((uint32_t)value) << shift);
}

__attribute__((always_inline)) static inline void clock_enable_clock(clock_ip_name_t name) {
    clock_control_gate(name, kCLOCK_ClockNeededRunWait);
}

__attribute__((always_inline)) static inline void clock_disable_clock(clock_ip_name_t name) {
    clock_control_gate(name, kCLOCK_ClockNotNeeded);
}

#define DIV_PAGE_PGM	4
#define DIV_ERASE_PGM	4
#define DIV_READ		0

static void SetFlexSPIDiv(uint32_t div) __attribute__((section(".ram_functions")));
static void SetFlexSPIDiv(uint32_t div) {
    FLEXSPI_Enable(FLEXSPI, false);
    clock_disable_clock(kCLOCK_FlexSpi);
    clock_set_div(kCLOCK_FlexspiDiv, div); /* flexspi clock 332M, DDR mode, internal clock 166M. */
    clock_enable_clock(kCLOCK_FlexSpi);
    FLEXSPI_Enable(FLEXSPI, true);	
}

status_t flexspi_nor_hyperbus_read(FLEXSPI_Type *base, uint32_t addr, uint32_t *buffer, uint32_t bytes) __attribute__((section(".ram_functions")));
status_t flexspi_nor_hyperbus_read(FLEXSPI_Type *base, uint32_t addr, uint32_t *buffer, uint32_t bytes) {
    flexspi_transfer_t flashXfer;
    status_t status;

    flashXfer.deviceAddress = addr * 2;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = HYPERFLASH_CMD_LUT_SEQ_IDX_READDATA;
    flashXfer.data          = buffer;
    flashXfer.dataSize      = bytes;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

status_t flexspi_nor_hyperbus_write(FLEXSPI_Type *base, uint32_t addr, uint32_t *buffer, uint32_t bytes) __attribute__((section(".ram_functions")));
status_t flexspi_nor_hyperbus_write(FLEXSPI_Type *base, uint32_t addr, uint32_t *buffer, uint32_t bytes) {
    flexspi_transfer_t flashXfer;
    status_t status;

    flashXfer.deviceAddress = addr * 2;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 1;
    flashXfer.seqIndex      = HYPERFLASH_CMD_LUT_SEQ_IDX_WRITEDATA;
    flashXfer.data          = buffer;
    flashXfer.dataSize      = bytes;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

status_t flexspi_nor_write_enable(FLEXSPI_Type *base, uint32_t baseAddr) __attribute__((section(".ram_functions")));
status_t flexspi_nor_write_enable(FLEXSPI_Type *base, uint32_t baseAddr) {
    flexspi_transfer_t flashXfer;
    status_t status;

    /* Write enable */
    flashXfer.deviceAddress = baseAddr;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 2;
    flashXfer.seqIndex      = HYPERFLASH_CMD_LUT_SEQ_IDX_WRITEENABLE;

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
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Read;
    flashXfer.SeqNumber     = 2;
    flashXfer.seqIndex      = HYPERFLASH_CMD_LUT_SEQ_IDX_READSTATUS;
    flashXfer.data          = &readValue;
    flashXfer.dataSize      = 2;

    do {
        status = FLEXSPI_TransferBlocking(base, &flashXfer);

        if (status != kStatus_Success) {
            return status;
        }
        if (readValue & 0x8000) {
            isBusy = false;
        } else {
            isBusy = true;
        }

        if (readValue & 0x3200) {
            status = kStatus_Fail;
            break;
        }

    } while (isBusy);

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

    flashXfer.deviceAddress = address;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Command;
    flashXfer.SeqNumber     = 4;
    flashXfer.seqIndex      = HYPERFLASH_CMD_LUT_SEQ_IDX_ERASESECTOR;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success) {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t address, const uint32_t *src,  uint32_t size ) __attribute__((section(".ram_functions")));
status_t flexspi_nor_flash_page_program(FLEXSPI_Type *base, uint32_t address, const uint32_t *src,  uint32_t size) {
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Speed down flexspi clock */
	SetFlexSPIDiv(DIV_PAGE_PGM);

    /* Write enable */
    status = flexspi_nor_write_enable(base, address);

    if (status != kStatus_Success) {
        return status;
    }

    /* Prepare page program command */
    flashXfer.deviceAddress = address;
    flashXfer.port          = kFLEXSPI_PortA1;
    flashXfer.cmdType       = kFLEXSPI_Write;
    flashXfer.SeqNumber     = 2;
    flashXfer.seqIndex      = HYPERFLASH_CMD_LUT_SEQ_IDX_PAGEPROGRAM;
    flashXfer.data          = (uint32_t *)src;
    flashXfer.dataSize      = size;
    status                  = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success) {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

	SetFlexSPIDiv(DIV_READ);

    return status;
}

status_t flexspi_nor_hyperflash_cfi(FLEXSPI_Type *base) __attribute__((section(".ram_functions")));
status_t flexspi_nor_hyperflash_cfi(FLEXSPI_Type *base) {
    /*
     * Read ID-CFI Parameters
     */
    // CFI Entry
    status_t status;
    uint32_t buffer[2];
    uint8_t data[4] = {0x00, 0x98};
    status          = flexspi_nor_hyperbus_write(base, 0x555, (uint32_t *)data, 2);
    if (status != kStatus_Success) {
        return status;
    }

    // ID-CFI Read
    // Read Query Unique ASCII String
    status = flexspi_nor_hyperbus_read(base, 0x10, &buffer[0], sizeof(buffer));
    if (status != kStatus_Success) {
        return status;
    }
    buffer[1] &= 0xFFFF;
    // Check that the data read out is  unicode "QRY" in big-endian order
    if ((buffer[0] != 0x52005100) || (buffer[1] != 0x5900)) {
        status = kStatus_Fail;
        return status;
    }
    // ASO Exit 0xF000
    data[1] = 0xF0;
    status  = flexspi_nor_hyperbus_write(base, 0x0, (uint32_t *)data, 2);
    if (status != kStatus_Success) {
        return status;
    }

    return status;
}
