/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "evkmimxrt1050_flexspi_nor_config.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.xip_board"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
#if defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf")))
#elif defined(__ICCARM__)
#pragma location = ".boot_hdr.conf"
#endif

const flexspi_nor_config_t qspiflash_config = {
    .memConfig =
        {
            .tag                = FLEXSPI_CFG_BLK_TAG,
            .version            = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc   = kFlexSPIReadSampleClk_ExternalInputFromDqsPad,
            .csHoldTime         = 3u,
            .csSetupTime        = 3u,
            .columnAddressWidth = 3u,
            // Enable DDR mode, Wordaddassable, Safe configuration, Differential clock
            .controllerMiscOption =
                (1u << kFlexSpiMiscOffset_DdrModeEnable) | (1u << kFlexSpiMiscOffset_WordAddressableEnable) |
                (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable) | (1u << kFlexSpiMiscOffset_DiffClkEnable),
            .sflashPadType = kSerialFlash_8Pads,
            .serialClkFreq = kFlexSpiSerialClk_133MHz,
            .sflashA1Size  = 64u * 1024u * 1024u,
            .dataValidTime = {16u, 16u},
            .lookupTable =
                {
                    // 0 Read LUTs 0 -> 0
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 0x18),
                    FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 0x06, READ_SDR, FLEXSPI_4PAD, 0x04),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 1 Read status register -> 1
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05, READ_SDR, FLEXSPI_1PAD, 0x01),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 2 Fast read quad mode - SDR
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x6B, RADDR_SDR, FLEXSPI_1PAD, 0x18),
                    FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 0x08, READ_SDR, FLEXSPI_4PAD, 0x04),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 3 Write Enable -> 3
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP, FLEXSPI_1PAD, 0),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 4 Read extend parameters
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x81, READ_SDR, FLEXSPI_1PAD, 0x04),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 5 Erase Sector -> 5
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x20, RADDR_SDR, FLEXSPI_1PAD, 24),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 6 Write Status Reg
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x01, WRITE_SDR, FLEXSPI_1PAD, 0x04),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 7 Page Program - quad mode (-> 9)
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x32, RADDR_SDR, FLEXSPI_1PAD, 0x18),
                    FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_4PAD, 0x04, STOP, FLEXSPI_1PAD, 0),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 8 Read ID
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x90, DUMMY_SDR, FLEXSPI_1PAD, 24),
                    FLEXSPI_LUT_SEQ(READ_SDR, FLEXSPI_1PAD, 0x00, 0, 0, 0),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 9 Page Program - single mode -> 9
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x02, RADDR_SDR, FLEXSPI_1PAD, 24),
                    FLEXSPI_LUT_SEQ(WRITE_SDR, FLEXSPI_1PAD, 0, 0, 0, 0),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 10 Enter QPI mode
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x35, STOP, FLEXSPI_1PAD, 0),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 11 Erase Chip
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x60, STOP, FLEXSPI_1PAD, 0),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler

                    // 12 Exit QPI mode
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_4PAD, 0xF5, STOP, FLEXSPI_1PAD, 0),
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                    FLEXSPI_LUT_SEQ(0, 0, 0, 0, 0, 0), // Filler
                },
        },
    .pageSize           = 512u,
    .sectorSize         = 256u * 1024u,
    .blockSize          = 256u * 1024u,
    .isUniformBlockSize = true,
};

#endif /* XIP_BOOT_HEADER_ENABLE */
