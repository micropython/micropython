/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexspi_nor_boot.h"
#include "fsl_flexspi_nor_config.h"


__attribute__((section(".boot_hdr.ivt")))
/*************************************
 *  IVT Data
 *************************************/
const ivt image_vector_table = {
  IVT_HEADER,                         /* IVT Header */
  IMAGE_ENTRY_ADDRESS,                /* Image Entry Function */
  IVT_RSVD,                           /* Reserved = 0 */
  (uint32_t)DCD_ADDRESS,              /* Address where DCD information is stored */
  (uint32_t)BOOT_DATA_ADDRESS,        /* Address where BOOT Data Structure is stored */
  (uint32_t)&image_vector_table,      /* Pointer to IVT Self (absolute address */
  (uint32_t)CSF_ADDRESS,              /* Address where CSF file is stored */
  IVT_RSVD                            /* Reserved = 0 */
};

__attribute__((section(".boot_hdr.boot_data")))
/*************************************
 *  Boot Data
 *************************************/
const BOOT_DATA_T boot_data = {
  FLASH_BASE,                 /* boot start location */
  FLASH_SIZE,                 /* size */
  PLUGIN_FLAG,                /* Plugin flag*/
  0xFFFFFFFF                  /* empty - extra data word */
};

__attribute__((section(".boot_hdr.conf")))
// Values copied from https://github.com/PaulStoffregen/cores/blob/ddb23fa5d97dac763bc06e11b9b41f026bd51f0a/teensy4/bootdata.c#L39
const flexspi_nor_config_t qspiflash_config = {
    .memConfig =
        {
            .tag              = FLEXSPI_CFG_BLK_TAG,
            .version          = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc = kFlexSPIReadSampleClk_LoopbackFromDqsPad,
            .csHoldTime       = 1u,
            .csSetupTime      = 2u,
            // Enable DDR mode, Wordaddressable, Safe configuration, Differential clock
            .deviceType    = kFlexSpiDeviceType_SerialNOR,
            .sflashPadType = kSerialFlash_4Pads,
            .serialClkFreq = kFlexSpiSerialClk_60MHz, // 03
            .sflashA1Size  = FLASH_SIZE,
            .lookupTable =
                {
                    // FLEXSPI_LUT_SEQ(cmd0, pad0, op0, cmd1, pad1, op1)
                    // (FLEXSPI_LUT_OPERAND0(op0) | FLEXSPI_LUT_NUM_PADS0(pad0) | FLEXSPI_LUT_OPCODE0(cmd0) | FLEXSPI_LUT_OPERAND1(op1) |
                    //  FLEXSPI_LUT_NUM_PADS1(pad1) | FLEXSPI_LUT_OPCODE1(cmd1))
                    // Read LUTs
                    FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0xEB, RADDR_SDR, FLEXSPI_4PAD, 0x18),
                    FLEXSPI_LUT_SEQ(DUMMY_SDR, FLEXSPI_4PAD, 0x06, READ_SDR, FLEXSPI_4PAD, 0x04),
                    0,
                    0,

                    0x24040405,
                    0,
                    0,
                    0,

                    0,
                    0,
                    0,
                    0,

                    0x00000406,
                    0,
                    0,
                    0,

                    0,
                    0,
                    0,
                    0,

                    0x08180420,
                    0,
                    0,
                    0,

                    0,
                    0,
                    0,
                    0,

                    0,
                    0,
                    0,
                    0,

                    0x081804D8,
                    0,
                    0,
                    0,

                    0x08180402,
                    0x00002004,
                    0,
                    0,

                    0,
                    0,
                    0,
                    0,

                    0x00000460,
                },
        },
    .pageSize           = 256u,
    .sectorSize         = 4u * 1024u,
    .ipcmdSerialClkFreq = kFlexSpiSerialClk_30MHz,
    .blockSize          = 0x00010000,
    .isUniformBlockSize = false,
};
