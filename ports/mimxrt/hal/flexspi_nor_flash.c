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
#include "flexspi_flash_config.h"

bool flash_busy_status_pol = 0;
bool flash_busy_status_offset = 0;

uint32_t LUT_pageprogram_quad[4] = {
    // 10 Page Program - quad mode
    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x32, RADDR_SDR, FLEXSPI_1PAD, 24),
    FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_4PAD, 0x04, STOP, FLEXSPI_1PAD, 0),
    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
};

uint32_t LUT_write_status[4] = {
    // 4 Write status word for Quad mode
    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, MICROPY_HW_FLASH_QE_CMD, WRITE_SDR, FLEXSPI_1PAD, 0x01),
    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
};

#if defined(MIMXRT117x_SERIES)
static uint8_t div_table_mhz[] = {
    17, // Entry 0 is out of range
    17, // 30 -> 31 MHz
    10, // 50 -> 52.8 MHz
    9, // 60 -> 58.7 MHz
    7, // 75 -> 75.4 MHz
    7, // 80 -> 75.4 MHz
    5, // 100 -> 105.6 Mhz
    4, // 133 -> 132 MHz
    3  // 166 -> 176 MHz
};

#else
typedef struct _ps_div_t {
    uint8_t pfd480_div;
    uint8_t podf_div;
} ps_div_t;

static ps_div_t div_table_mhz[] = {
    { 35, 8 }, // Entry 0 is out of range
    { 35, 8 }, // 30 -> 30.85 MHz
    { 29, 6 }, // 50 -> 49.65 MHz
    { 18, 8 }, // 60 -> 60 MHz
    { 23, 5 }, // 75 -> 75.13 MHz
    { 18, 6 }, // 80 -> 80 MHz
    { 17, 5 }, // 100 -> 101 Mhz
    { 13, 5 }, // 133 -> 132.92 MHz
    { 13, 4 }  // 166  -> 166.15 MHz
};
#endif

__attribute__((section(".ram_functions"))) void flexspi_nor_update_lut_clk(uint32_t freq_index) {
    // Create a local copy of the LookupTable. Modify the entry for WRITESTATUSREG
    // Add an entry for PAGEPROGRAM_QUAD.
    uint32_t lookuptable_copy[64];
    memcpy(lookuptable_copy, (const uint32_t *)&qspiflash_config.memConfig.lookupTable, 64 * sizeof(uint32_t));
    // write local WRITESTATUSREG code to index 4
    memcpy(&lookuptable_copy[NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG * 4],
        LUT_write_status, 4 * sizeof(uint32_t));
    // write local PAGEPROGRAM_QUAD code to index 10
    memcpy(&lookuptable_copy[NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD * 4],
        LUT_pageprogram_quad, 4 * sizeof(uint32_t));
    // Update the LookupTable.
    FLEXSPI_UpdateLUT(BOARD_FLEX_SPI, 0, lookuptable_copy, 64);

    __DSB();
    __ISB();
    __disable_irq();
    SCB_DisableDCache();

    #if defined(MIMXRT117x_SERIES)
    volatile uint8_t pll2_div = div_table_mhz[freq_index] - 1;

    while (!FLEXSPI_GetBusIdleStatus(BOARD_FLEX_SPI)) {
    }
    FLEXSPI_Enable(BOARD_FLEX_SPI, false);

    // Disable FlexSPI clock
    // Flexspi is clocked by PLL2. Only the divider can be changed.
    CCM->LPCG[kCLOCK_Flexspi1].DIRECT = ((uint32_t)kCLOCK_Off & CCM_LPCG_DIRECT_ON_MASK);
    // Change the PLL divider
    CCM->CLOCK_ROOT[kCLOCK_Root_Flexspi1].CONTROL = (CCM->CLOCK_ROOT[kCLOCK_Root_Flexspi1].CONTROL & ~CCM_CLOCK_ROOT_CONTROL_DIV_MASK) |
        CCM_CLOCK_ROOT_CONTROL_DIV(pll2_div);
    // Re-enable FlexSPI clock
    CCM->LPCG[kCLOCK_Flexspi1].DIRECT = ((uint32_t)kCLOCK_On & CCM_LPCG_DIRECT_ON_MASK);

    #else

    volatile uint8_t pfd480_div = div_table_mhz[freq_index].pfd480_div;
    volatile uint8_t podf_div = div_table_mhz[freq_index].podf_div - 1;

    while (!FLEXSPI_GetBusIdleStatus(BOARD_FLEX_SPI)) {
    }
    FLEXSPI_Enable(BOARD_FLEX_SPI, false);

    // Disable FlexSPI clock
    CCM->CCGR6 &= ~CCM_CCGR6_CG5_MASK;
    // Changing the clock is OK now.
    // Change the PFD
    CCM_ANALOG->PFD_480 = (CCM_ANALOG->PFD_480 & ~CCM_ANALOG_PFD_480_TOG_PFD0_FRAC_MASK) | CCM_ANALOG_PFD_480_TOG_PFD0_FRAC(pfd480_div);
    // Change the flexspi divider
    CCM->CSCMR1 = (CCM->CSCMR1 & ~CCM_CSCMR1_FLEXSPI_PODF_MASK) | CCM_CSCMR1_FLEXSPI_PODF(podf_div);
    // Re-enable FlexSPI
    CCM->CCGR6 |= CCM_CCGR6_CG5_MASK;
    #endif

    FLEXSPI_Enable(BOARD_FLEX_SPI, true);
    FLEXSPI_SoftwareReset(BOARD_FLEX_SPI);
    while (!FLEXSPI_GetBusIdleStatus(BOARD_FLEX_SPI)) {
    }

    SCB_EnableDCache();
    __enable_irq();
}

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
    uint32_t writeValue = MICROPY_HW_FLASH_QE_ARG;

    /* Write enable */
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

status_t flexspi_nor_flash_erase_block(FLEXSPI_Type *base, uint32_t address) __attribute__((section(".ram_functions")));
status_t flexspi_nor_flash_erase_block(FLEXSPI_Type *base, uint32_t address) {
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
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK;
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
