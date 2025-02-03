/*
 * Copyright 2019 NXP.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Based on tinyusb/hw/bsp/teensy_40/evkmimxrt1010_flexspi_nor_config.c

#include BOARD_FLASH_CONFIG_HEADER_H

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.xip_board"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
#if defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf")))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.conf"
#endif

#ifndef MICROPY_HW_FLASH_DQS
#define MICROPY_HW_FLASH_DQS kFlexSPIReadSampleClk_LoopbackFromDqsPad
#endif

#ifndef MICROPY_HW_FLASH_CLK
#define MICROPY_HW_FLASH_CLK kFlexSpiSerialClk_100MHz
#endif

const flexspi_nor_config_t qspiflash_config = {
    .memConfig =
    {
        .tag = FLEXSPI_CFG_BLK_TAG,
        .version = FLEXSPI_CFG_BLK_VERSION,
        .readSampleClkSrc = MICROPY_HW_FLASH_DQS,
        .csHoldTime = 3u,
        .csSetupTime = 3u,
        .busyOffset = FLASH_BUSY_STATUS_OFFSET,         // Status bit 0 indicates busy.
        .busyBitPolarity = FLASH_BUSY_STATUS_POL,       // Busy when the bit is 1.
        .deviceModeCfgEnable = 1u,
        .deviceModeType = kDeviceConfigCmdType_QuadEnable,
        .deviceModeSeq = {
            .seqId = 4u,
            .seqNum = 1u,
        },
        .deviceModeArg = MICROPY_HW_FLASH_QE_ARG,
        .deviceType = kFlexSpiDeviceType_SerialNOR,
        .sflashPadType = kSerialFlash_4Pads,
        .serialClkFreq = MICROPY_HW_FLASH_CLK,
        .sflashA1Size = MICROPY_HW_FLASH_SIZE,
        .lookupTable =
        {
            // 0 Read LUTs 0
            FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 24),
            FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 0x06, READ_SDR, FLEXSPI_4PAD, 0x04),
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler

            // 1 Read status register
            FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05, READ_SDR, FLEXSPI_1PAD, 0x01),
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler

            // 2 Read extend parameters
            EMPTY_LUT_SEQ

            // 3 Write Enable
            FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP, FLEXSPI_1PAD, 0),
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler

            // 4 Write Status Reg
            FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, MICROPY_HW_FLASH_QE_CMD, WRITE_SDR, FLEXSPI_1PAD, 0x01),
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler

            // 5 Erase Sector
            FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x20, RADDR_SDR, FLEXSPI_1PAD, 24),
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler

            // 6 Fast read quad mode - SDR
            EMPTY_LUT_SEQ

            // 7 Read ID
            EMPTY_LUT_SEQ

            // 8 Erase Block (32k)
            FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x52, RADDR_SDR, FLEXSPI_1PAD, 24),
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler

            // 9 Page Program - single mode
            FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x02, RADDR_SDR, FLEXSPI_1PAD, 24),
            FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_1PAD, 0, 0, 0, 0),
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler

            // 10 Page Program - quad mode
            FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x32, RADDR_SDR, FLEXSPI_1PAD, 24),
            FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_4PAD, 0x04, STOP, FLEXSPI_1PAD, 0),
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler

            // 11 Erase Chip
            FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x60, STOP, FLEXSPI_1PAD, 0),
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler
            FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0),         // Filler

            // 12 Not used
            EMPTY_LUT_SEQ

            // 13 READ SDFP
            EMPTY_LUT_SEQ

            // 14 Not used
            EMPTY_LUT_SEQ

            // 15 Not used
            EMPTY_LUT_SEQ
        },
    },
    .pageSize = 256u,
    .sectorSize = 4u * 1024u,
    .blockSize = 32u * 1024u,
    .isUniformBlockSize = false,
    // .ipcmdSerialClkFreq = kFlexSpiSerialClk_30MHz,
};
#endif /* XIP_BOOT_HEADER_ENABLE */
