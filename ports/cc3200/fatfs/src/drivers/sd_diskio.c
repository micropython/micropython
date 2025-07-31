//*****************************************************************************
// sd_diskio.c
//
// Low level SD Card access hookup for FatFS
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
#include <stdbool.h>

#include "py/mpconfig.h"
#include "py/mphal.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "rom_map.h"
#include "sd_diskio.h"
#include "sdhost.h"
#include "pin.h"
#include "prcm.h"
#include "stdcmd.h"
#include "utils.h"

//*****************************************************************************
// Macros
//*****************************************************************************
#define DISKIO_RETRY_TIMEOUT        0xFFFFFFFF

#define CARD_TYPE_UNKNOWN           0
#define CARD_TYPE_MMC               1
#define CARD_TYPE_SDCARD            2

#define CARD_CAP_CLASS_SDSC         0
#define CARD_CAP_CLASS_SDHC         1

#define CARD_VERSION_1              0
#define CARD_VERSION_2              1

//*****************************************************************************
// Disk Info for attached disk
//*****************************************************************************
DiskInfo_t sd_disk_info =  {CARD_TYPE_UNKNOWN, CARD_VERSION_1, CARD_CAP_CLASS_SDSC, 0, 0, STA_NOINIT, 0};

//*****************************************************************************
//
//! Send Command to card
//!
//! \param ulCmd is the command to be send
//! \paran ulArg is the command argument
//!
//! This function sends command to attached card and check the response status
//! if any.
//!
//! \return Returns 0 on success, 1 otherwise
//
//*****************************************************************************
static unsigned int CardSendCmd (unsigned int ulCmd, unsigned int ulArg) {
    unsigned long ulStatus;

    // Clear the interrupt status
    MAP_SDHostIntClear(SDHOST_BASE,0xFFFFFFFF);

    // Send command
    MAP_SDHostCmdSend(SDHOST_BASE,ulCmd,ulArg);

    // Wait for command complete or error
    do {
        ulStatus = MAP_SDHostIntStatus(SDHOST_BASE);
        ulStatus = (ulStatus & (SDHOST_INT_CC | SDHOST_INT_ERRI));
    } while (!ulStatus);

    // Check error status
    if (ulStatus & SDHOST_INT_ERRI) {
        // Reset the command line
        MAP_SDHostCmdReset(SDHOST_BASE);
        return 1;
    }
    else {
        return 0;
    }
}

//*****************************************************************************
//
//! Get the capacity of specified card
//!
//! \param ulRCA is the Relative Card Address (RCA)
//!
//! This function gets the capacity of card addressed by \e ulRCA paramaeter.
//!
//! \return Returns 0 on success, 1 otherwise.
//
//*****************************************************************************
static unsigned int CardCapacityGet(DiskInfo_t *psDiskInfo) {
    unsigned long ulRet;
    unsigned long ulResp[4];
    unsigned long ulBlockSize;
    unsigned long ulBlockCount;
    unsigned long ulCSizeMult;
    unsigned long ulCSize;

    // Read the CSD register
    ulRet = CardSendCmd(CMD_SEND_CSD, (psDiskInfo->usRCA << 16));

    if(ulRet == 0) {
        // Read the response
        MAP_SDHostRespGet(SDHOST_BASE,ulResp);

        // 136 bit CSD register is read into an array of 4 words.
        // ulResp[0] = CSD[31:0]
        // ulResp[1] = CSD[63:32]
        // ulResp[2] = CSD[95:64]
        // ulResp[3] = CSD[127:96]
        if(ulResp[3] >> 30) {
            ulBlockSize = SD_SECTOR_SIZE * 1024;
            ulBlockCount = (ulResp[1] >> 16 | ((ulResp[2] & 0x3F) << 16)) + 1;
        }
        else {
            ulBlockSize  = 1 << ((ulResp[2] >> 16) & 0xF);
            ulCSizeMult  = ((ulResp[1] >> 15) & 0x7);
            ulCSize      = ((ulResp[1] >> 30) | (ulResp[2] & 0x3FF) << 2);
            ulBlockCount = (ulCSize + 1) * (1 << (ulCSizeMult + 2));
        }

        // Calculate the card capacity in bytes
        psDiskInfo->ulBlockSize = ulBlockSize;
        psDiskInfo->ulNofBlock  = ulBlockCount;
    }

    return ulRet;
}

//*****************************************************************************
//
//! Select a card for reading or writing
//!
//! \param Card is the pointer to card attribute structure.
//!
//! This function selects a card for reading or writing using its RCA from
//! \e Card parameter.
//!
//! \return Returns 0 success, 1 otherwise.
//
//*****************************************************************************
static unsigned int CardSelect (DiskInfo_t *sDiskInfo) {
    unsigned long ulRCA;
    unsigned long ulRet;

    ulRCA = sDiskInfo->usRCA;

    // Send select command with card's RCA.
    ulRet = CardSendCmd(CMD_SELECT_CARD, (ulRCA << 16));

    if (ulRet == 0) {
        while (!(MAP_SDHostIntStatus(SDHOST_BASE) & SDHOST_INT_TC));
    }

    // Delay 250ms for the card to become ready
    mp_hal_delay_ms(250);

    return ulRet;
}

//*****************************************************************************
//
//! Initializes physical drive
//!
//! This function initializes the physical drive
//!
//! \return Returns 0 on succeeded.
//*****************************************************************************
DSTATUS sd_disk_init (void) {
    unsigned long ulRet;
    unsigned long ulResp[4];

    if (sd_disk_info.bStatus != 0) {
        sd_disk_info.bStatus = STA_NODISK;
        // Send std GO IDLE command
        if (CardSendCmd(CMD_GO_IDLE_STATE, 0) == 0) {
            // Get interface operating condition for the card
            ulRet = CardSendCmd(CMD_SEND_IF_COND,0x000001A5);
            MAP_SDHostRespGet(SDHOST_BASE,ulResp);

            // It's a SD ver 2.0 or higher card
            if (ulRet == 0 && ((ulResp[0] & 0xFF) == 0xA5)) {
                // Version 1 card do not respond to this command
                sd_disk_info.ulVersion = CARD_VERSION_2;
                sd_disk_info.ucCardType = CARD_TYPE_SDCARD;

                // Wait for card to become ready.
                do {
                    // Send ACMD41
                    CardSendCmd(CMD_APP_CMD, 0);
                    ulRet = CardSendCmd(CMD_SD_SEND_OP_COND, 0x40E00000);

                    // Response contains 32-bit OCR register
                    MAP_SDHostRespGet(SDHOST_BASE, ulResp);

                } while (((ulResp[0] >> 31) == 0));

                if (ulResp[0] & (1UL<<30)) {
                    sd_disk_info.ulCapClass = CARD_CAP_CLASS_SDHC;
                }
                sd_disk_info.bStatus = 0;
            }
            //It's a MMC or SD 1.x card
            else {
                // Wait for card to become ready.
                do {
                    CardSendCmd(CMD_APP_CMD, 0);
                    ulRet = CardSendCmd(CMD_SD_SEND_OP_COND,0x00E00000);
                    if (ulRet == 0) {
                        // Response contains 32-bit OCR register
                        MAP_SDHostRespGet(SDHOST_BASE, ulResp);
                    }
                } while (((ulRet == 0) && (ulResp[0] >> 31) == 0));

                if (ulRet == 0) {
                    sd_disk_info.ucCardType = CARD_TYPE_SDCARD;
                    sd_disk_info.bStatus = 0;
                }
                else {
                    if (CardSendCmd(CMD_SEND_OP_COND, 0) == 0) {
                        // MMC not supported by the controller
                        sd_disk_info.ucCardType = CARD_TYPE_MMC;
                    }
                }
            }
        }

        // Get the RCA of the attached card
        if (sd_disk_info.bStatus == 0) {
            ulRet = CardSendCmd(CMD_ALL_SEND_CID, 0);
            if (ulRet == 0) {
                CardSendCmd(CMD_SEND_REL_ADDR,0);
                MAP_SDHostRespGet(SDHOST_BASE, ulResp);

                //  Fill in the RCA
                sd_disk_info.usRCA = (ulResp[0] >> 16);

                // Get the card capacity
                CardCapacityGet(&sd_disk_info);
            }

            // Select the card.
            ulRet = CardSelect(&sd_disk_info);
            if (ulRet == 0) {
                sd_disk_info.bStatus = 0;
            }
        }
    }

    return sd_disk_info.bStatus;
}

//*****************************************************************************
//
//! De-initializes the physical drive
//!
//! This function de-initializes the physical drive
//*****************************************************************************
void sd_disk_deinit (void) {
    sd_disk_info.ucCardType = CARD_TYPE_UNKNOWN;
    sd_disk_info.ulVersion = CARD_VERSION_1;
    sd_disk_info.ulCapClass = CARD_CAP_CLASS_SDSC;
    sd_disk_info.ulNofBlock = 0;
    sd_disk_info.ulBlockSize = 0;
    sd_disk_info.bStatus = STA_NOINIT;
    sd_disk_info.usRCA = 0;
}

//*****************************************************************************
//
//! Reads sector(s) from the disk drive.
//!
//!
//! This function reads specified number of sectors from the drive
//!
//! \return Returns RES_OK on success.
//
//*****************************************************************************
DRESULT sd_disk_read (BYTE* pBuffer, DWORD ulSectorNumber, UINT SectorCount) {
    DRESULT Res = RES_ERROR;
    unsigned long ulSize;

    if (SectorCount > 0) {
        // Return if disk not initialized
        if (sd_disk_info.bStatus & STA_NOINIT) {
            return RES_NOTRDY;
        }

        // SDSC uses linear address, SDHC uses block address
        if (sd_disk_info.ulCapClass == CARD_CAP_CLASS_SDSC) {
            ulSectorNumber = ulSectorNumber * SD_SECTOR_SIZE;
        }

        // Set the block count
        MAP_SDHostBlockCountSet(SDHOST_BASE, SectorCount);

        // Compute the number of words
        ulSize = (SD_SECTOR_SIZE * SectorCount) / 4;

        // Check if 1 block or multi block transfer
        if (SectorCount == 1) {
            // Send single block read command
            if (CardSendCmd(CMD_READ_SINGLE_BLK, ulSectorNumber) == 0) {
                // Read the block of data
                while (ulSize--) {
                    MAP_SDHostDataRead(SDHOST_BASE, (unsigned long *)pBuffer);
                    pBuffer += 4;
                }
                Res = RES_OK;
            }
        }
        else {
            // Send multi block read command
            if (CardSendCmd(CMD_READ_MULTI_BLK, ulSectorNumber) == 0) {
                // Read the data
                while (ulSize--) {
                    MAP_SDHostDataRead(SDHOST_BASE, (unsigned long *)pBuffer);
                    pBuffer += 4;
                }
                CardSendCmd(CMD_STOP_TRANS, 0);
                while (!(MAP_SDHostIntStatus(SDHOST_BASE) & SDHOST_INT_TC));
                Res = RES_OK;
            }
        }
    }

    return Res;
}

//*****************************************************************************
//
//! Writes sector(s) to the disk drive.
//!
//!
//! This function writes specified number of sectors to the drive
//!
//! \return Returns RES_OK on success.
//
//*****************************************************************************
DRESULT sd_disk_write (const BYTE* pBuffer, DWORD ulSectorNumber, UINT SectorCount) {
    DRESULT Res = RES_ERROR;
    unsigned long ulSize;

    if (SectorCount > 0) {
        // Return if disk not initialized
        if (sd_disk_info.bStatus & STA_NOINIT) {
            return RES_NOTRDY;
        }

        // SDSC uses linear address, SDHC uses block address
        if (sd_disk_info.ulCapClass == CARD_CAP_CLASS_SDSC) {
            ulSectorNumber = ulSectorNumber * SD_SECTOR_SIZE;
        }

        // Set the block count
        MAP_SDHostBlockCountSet(SDHOST_BASE, SectorCount);

        // Compute the number of words
        ulSize = (SD_SECTOR_SIZE * SectorCount) / 4;

        // Check if 1 block or multi block transfer
        if (SectorCount == 1) {
            // Send single block write command
            if (CardSendCmd(CMD_WRITE_SINGLE_BLK, ulSectorNumber) == 0) {
                // Write the data
                while (ulSize--) {
                    MAP_SDHostDataWrite (SDHOST_BASE, (*(unsigned long *)pBuffer));
                    pBuffer += 4;
                }
                // Wait for data transfer complete
                while (!(MAP_SDHostIntStatus(SDHOST_BASE) & SDHOST_INT_TC));
                Res = RES_OK;
            }
        }
        else {
            // Set the card write block count
            if (sd_disk_info.ucCardType == CARD_TYPE_SDCARD) {
                CardSendCmd(CMD_APP_CMD,sd_disk_info.usRCA << 16);
                CardSendCmd(CMD_SET_BLK_CNT, SectorCount);
            }

            // Send multi block write command
            if (CardSendCmd(CMD_WRITE_MULTI_BLK, ulSectorNumber) == 0) {
                // Write the data buffer
                while (ulSize--) {
                    MAP_SDHostDataWrite(SDHOST_BASE, (*(unsigned long *)pBuffer));
                    pBuffer += 4;
                }
                // Wait for transfer complete
                while (!(MAP_SDHostIntStatus(SDHOST_BASE) & SDHOST_INT_TC));
                CardSendCmd(CMD_STOP_TRANS, 0);
                while (!(MAP_SDHostIntStatus(SDHOST_BASE) & SDHOST_INT_TC));
                Res = RES_OK;
            }
        }
    }

    return Res;
}
