/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include "flash.h"
#include "mimxrt_flash.h"


/*******************************************************************************
* Definitions
******************************************************************************/

#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL 0
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG 1
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD 2
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE 3
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS_XPI 4
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR 5
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG 6
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD 7
#define NOR_CMD_LUT_SEQ_IDX_READID 8
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM 9
#define NOR_CMD_LUT_SEQ_IDX_ENTERQPI 10
#define NOR_CMD_LUT_SEQ_IDX_CHIPERASE 11
#define NOR_CMD_LUT_SEQ_IDX_EXITQPI 12

#define FLASH_BUSY_STATUS_POL 1
#define FLASH_BUSY_STATUS_OFFSET 0

/*******************************************************************************
* Prototypes
******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

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
                isBusy = true;
            } else {
                isBusy = false;
            }
        } else {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET)) {
                isBusy = false;
            } else {
                isBusy = true;
            }
        }
        // size_t busyBit = readValue & (1U << qspiflash_config.memConfig.busyOffset);
        
        // isBusy = (qspiflash_config.memConfig.busyBitPolarity == 0 && busyBit != 0) ||
        //     (qspiflash_config.memConfig.busyBitPolarity == 1 && busyBit == 0);
    } while (isBusy);

    return status;
}

status_t flexspi_nor_enable_quad_mode(FLEXSPI_Type *base) {
    flexspi_transfer_t flashXfer;
    status_t status;
    uint32_t writeValue = 0x40;

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

status_t flexspi_nor_flash_erase_sector(FLEXSPI_Type *base, uint32_t address) {
    status_t status;
    flexspi_transfer_t flashXfer;

    /* Write enable */
    status = flexspi_nor_write_enable(base, address);

    if (status != kStatus_Success) {
        return status;
    }

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

    return status;
}

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
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM;
    flashXfer.data = (uint32_t *)src;
    flashXfer.dataSize = size;
    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success) {
        return status;
    }

    status = flexspi_nor_wait_bus_busy(base);

    return status;
}

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

// flexspi_device_config_t deviceconfig = {
//     .flexspiRootClk = 120000000,
//     .flashSize = BOARD_FLASH_SIZE,
//     .CSIntervalUnit = kFLEXSPI_CsIntervalUnit1SckCycle,
//     .CSInterval = 2,
//     .CSHoldTime = 3,
//     .CSSetupTime = 3,
//     .dataValidTime = 0,
//     .columnspace = 0,
//     .enableWordAddress = 0,
//     .AWRSeqIndex = 0,
//     .AWRSeqNumber = 0,
//     .ARDSeqIndex = NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD,
//     .ARDSeqNumber = 1,
//     .AHBWriteWaitUnit = kFLEXSPI_AhbWriteWaitUnit2AhbCycle,
//     .AHBWriteWaitInterval = 0,
// };


// extern void BOARD_InitFlexspiPins();
// status_t flexspi_nor_init(void) {
//     uint32_t primask = __get_PRIMASK();
//     __set_PRIMASK(1);
//     status_t status;
//     flexspi_config_t config;
//     const clock_usb_pll_config_t g_ccmConfigUsbPll = {.loopDivider = 0U};


//     // BOARD_InitFlexspiPins();

//     CLOCK_InitUsb1Pll(&g_ccmConfigUsbPll);
//     CLOCK_InitUsb1Pfd(kCLOCK_Pfd0, 24);   /* Set PLL3 PFD0 clock 360MHZ. */
//     CLOCK_SetMux(kCLOCK_FlexspiMux, 0x3); /* Choose PLL3 PFD0 clock as flexspi source clock. */
//     CLOCK_SetDiv(kCLOCK_FlexspiDiv, 2);   /* flexspi clock 120M. */

//     /*Get FLEXSPI default settings and configure the flexspi. */
//     FLEXSPI_GetDefaultConfig(&config);
//     /*Set AHB buffer size for reading data through AHB bus. */
//     config.ahbConfig.enableAHBPrefetch = true;
//     config.rxSampleClock = kFLEXSPI_ReadSampleClkLoopbackFromDqsPad;
//     FLEXSPI_Init(FLEXSPI, &config);

//     /* Configure flash settings according to serial flash feature. */
//     FLEXSPI_SetFlashConfig(FLEXSPI, &deviceconfig, kFLEXSPI_PortA1);

//     /* Update LUT table. */
//     // FLEXSPI_UpdateLUT(FLEXSPI, 0, customLUT, CUSTOM_LUT_LENGTH);
//     /* Enter quad mode. */
//     status = flexspi_nor_enable_quad_mode(FLEXSPI);

//     __set_PRIMASK(primask);
//     return status;
// }

void flexspi_nor_reset(void) {
    // 2020-0509-1448_rocky: in case this "inline" function is failed to inline and put to flash
    FLEXSPI_SoftwareReset(FLEXSPI);
}
