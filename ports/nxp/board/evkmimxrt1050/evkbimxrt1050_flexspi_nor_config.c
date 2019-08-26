/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "evkbimxrt1050_flexspi_nor_config.h"

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

const flexspi_nor_config_t hyperflash_config = {
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
                    // Read LUTs
                    FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xA0, RADDR_DDR, FLEXSPI_8PAD, 0x18),
                    FLEXSPI_LUT_SEQ(CADDR_DDR, FLEXSPI_8PAD, 0x10, DUMMY_DDR, FLEXSPI_8PAD, 0x06),
                    FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP, FLEXSPI_1PAD, 0x0),
                },
        },
    .pageSize           = 512u,
    .sectorSize         = 256u * 1024u,
    .blockSize          = 256u * 1024u,
    .isUniformBlockSize = true,
};
#endif /* XIP_BOOT_HEADER_ENABLE */
