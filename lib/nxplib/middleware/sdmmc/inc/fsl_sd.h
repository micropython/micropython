/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SD_H_
#define _FSL_SD_H_

#include "fsl_sdmmc_common.h"
/*!
 * @addtogroup SDCARD
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief SD card flags */
enum _sd_card_flag
{
    kSD_SupportHighCapacityFlag = (1U << 1U),     /*!< Support high capacity */
    kSD_Support4BitWidthFlag = (1U << 2U),        /*!< Support 4-bit data width */
    kSD_SupportSdhcFlag = (1U << 3U),             /*!< Card is SDHC */
    kSD_SupportSdxcFlag = (1U << 4U),             /*!< Card is SDXC */
    kSD_SupportVoltage180v = (1U << 5U),          /*!< card support 1.8v voltage*/
    kSD_SupportSetBlockCountCmd = (1U << 6U),     /*!< card support cmd23 flag*/
    kSD_SupportSpeedClassControlCmd = (1U << 7U), /*!< card support speed class control flag */
};

/*!
 * @brief SD card state
 *
 * Define the card structure including the necessary fields to identify and describe the card.
 */
typedef struct _sd_card
{
    SDMMCHOST_CONFIG host; /*!< Host information */

    sdcard_usr_param_t usrParam;    /*!< user parameter */
    bool isHostReady;               /*!< use this flag to indicate if need host re-init or not*/
    bool noInteralAlign;            /*!< use this flag to disable sdmmc align. If disable, sdmmc will not make sure the
                                    data buffer address is word align, otherwise all the transfer are align to low level driver */
    uint32_t busClock_Hz;           /*!< SD bus clock frequency united in Hz */
    uint32_t relativeAddress;       /*!< Relative address of the card */
    uint32_t version;               /*!< Card version */
    uint32_t flags;                 /*!< Flags in _sd_card_flag */
    uint32_t rawCid[4U];            /*!< Raw CID content */
    uint32_t rawCsd[4U];            /*!< Raw CSD content */
    uint32_t rawScr[2U];            /*!< Raw CSD content */
    uint32_t ocr;                   /*!< Raw OCR content */
    sd_cid_t cid;                   /*!< CID */
    sd_csd_t csd;                   /*!< CSD */
    sd_scr_t scr;                   /*!< SCR */
    sd_status_t stat;               /*!< sd 512 bit status */
    uint32_t blockCount;            /*!< Card total block number */
    uint32_t blockSize;             /*!< Card block size */
    sd_timing_mode_t currentTiming; /*!< current timing mode */
    sd_driver_strength_t driverStrength;        /*!< driver strength */
    sd_max_current_t maxCurrent;                /*!< card current limit */
    sdmmc_operation_voltage_t operationVoltage; /*!< card operation voltage */
} sd_card_t;

/*************************************************************************************************
 * API
 ************************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name SDCARD Function
 * @{
 */

/*!
 * @brief Initializes the card on a specific host controller.
 * @deprecated Do not use this function.  It has been superceded by @ref SD_HostInit,SD_CardInit.

 * This function initializes the card on a specific host controller, it is consist of
 * host init, card detect, card init function, however user can ignore this high level function,
 * instead of use the low level function, such as SD_CardInit, SD_HostInit, SD_CardDetect.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_HostNotReady host is not ready.
 * @retval kStatus_SDMMC_GoIdleFailed Go idle failed.
 * @retval kStatus_SDMMC_NotSupportYet Card not support.
 * @retval kStatus_SDMMC_SendOperationConditionFailed Send operation condition failed.
 * @retval kStatus_SDMMC_AllSendCidFailed Send CID failed.
 * @retval kStatus_SDMMC_SendRelativeAddressFailed Send relative address failed.
 * @retval kStatus_SDMMC_SendCsdFailed Send CSD failed.
 * @retval kStatus_SDMMC_SelectCardFailed Send SELECT_CARD command failed.
 * @retval kStatus_SDMMC_SendScrFailed Send SCR failed.
 * @retval kStatus_SDMMC_SetBusWidthFailed Set bus width failed.
 * @retval kStatus_SDMMC_SwitchHighSpeedFailed Switch high speed failed.
 * @retval kStatus_SDMMC_SetCardBlockSizeFailed Set card block size failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SD_Init(sd_card_t *card);

/*!
 * @brief Deinitializes the card.
 * @deprecated Do not use this function.  It has been superceded by @ref SD_HostDeinit,SD_CardDeinit.
 * This function deinitializes the specific card and host.
 *
 * @param card Card descriptor.
 */
void SD_Deinit(sd_card_t *card);

/*!
 * @brief Initializes the card.
 *
 * This function initializes the card only, make sure the host is ready when call this function,
 * otherwise it will return kStatus_SDMMC_HostNotReady.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_HostNotReady host is not ready.
 * @retval kStatus_SDMMC_GoIdleFailed Go idle failed.
 * @retval kStatus_SDMMC_NotSupportYet Card not support.
 * @retval kStatus_SDMMC_SendOperationConditionFailed Send operation condition failed.
 * @retval kStatus_SDMMC_AllSendCidFailed Send CID failed.
 * @retval kStatus_SDMMC_SendRelativeAddressFailed Send relative address failed.
 * @retval kStatus_SDMMC_SendCsdFailed Send CSD failed.
 * @retval kStatus_SDMMC_SelectCardFailed Send SELECT_CARD command failed.
 * @retval kStatus_SDMMC_SendScrFailed Send SCR failed.
 * @retval kStatus_SDMMC_SetBusWidthFailed Set bus width failed.
 * @retval kStatus_SDMMC_SwitchHighSpeedFailed Switch high speed failed.
 * @retval kStatus_SDMMC_SetCardBlockSizeFailed Set card block size failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SD_CardInit(sd_card_t *card);

/*!
 * @brief Deinitializes the card.
 *
 * This function deinitializes the specific card.
 *
 * @param card Card descriptor.
 */
void SD_CardDeinit(sd_card_t *card);

/*!
 * @brief initialize the host.
 *
 * This function deinitializes the specific host.
 *
 * @param card Card descriptor.
 */
status_t SD_HostInit(sd_card_t *card);

/*!
 * @brief Deinitializes the host.
 *
 * This function deinitializes the host.
 *
 * @param card Card descriptor.
 */
void SD_HostDeinit(sd_card_t *card);

/*!
 * @brief reset the host.
 *
 * This function reset the specific host.
 *
 * @param host host descriptor.
 */
void SD_HostReset(SDMMCHOST_CONFIG *host);

/*!
 * @brief power on card.
 *
 * The power on operation depend on host or the user define power on function.
 * @param base host base address.
 * @param pwr user define power control configuration
 */
void SD_PowerOnCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr);

/*!
 * @brief power off card.
 *
 * The power off operation depend on host or the user define power on function.
 * @param base host base address.
 * @param pwr user define power control configuration
 */
void SD_PowerOffCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr);

/*!
 * @brief sd wait card detect function.
 *
 * Detect card through GPIO, CD, DATA3.
 *
 * @param card card descriptor.
 * @param card detect configuration
 * @param waitCardStatus wait card detect status
 */
status_t SD_WaitCardDetectStatus(SDMMCHOST_TYPE *hostBase, const sdmmchost_detect_card_t *cd, bool waitCardStatus);

/*!
 * @brief sd card present check function.
 *
 * @param card card descriptor.
 */
bool SD_IsCardPresent(sd_card_t *card);

/*!
 * @brief Checks whether the card is write-protected.
 *
 * This function checks if the card is write-protected via the CSD register.
 *
 * @param card The specific card.
 * @retval true Card is read only.
 * @retval false Card isn't read only.
 */
bool SD_CheckReadOnly(sd_card_t *card);

/*!
 * @brief Send SELECT_CARD command to set the card to be transfer state or not.
 *
 * @param card Card descriptor.
 * @param isSelected True to set the card into transfer state, false to disselect.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SD_SelectCard(sd_card_t *card, bool isSelected);

/*!
 * @brief Send ACMD13 to get the card current status.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_SendApplicationCommandFailed send application command failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SD_ReadStatus(sd_card_t *card);

/*!
 * @brief Reads blocks from the specific card.
 *
 * This function reads blocks from the specific card with default block size defined by the
 * SDHC_CARD_DEFAULT_BLOCK_SIZE.
 *
 * @param card Card descriptor.
 * @param buffer The buffer to save the data read from card.
 * @param startBlock The start block index.
 * @param blockCount The number of blocks to read.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_SDMMC_CardNotSupport Card not support.
 * @retval kStatus_SDMMC_NotSupportYet Not support now.
 * @retval kStatus_SDMMC_WaitWriteCompleteFailed Send status failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_StopTransmissionFailed Stop transmission failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SD_ReadBlocks(sd_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);

/*!
 * @brief Writes blocks of data to the specific card.
 *
 * This function writes blocks to the specific card with default block size 512 bytes.
 *
 * @param card Card descriptor.
 * @param buffer The buffer holding the data to be written to the card.
 * @param startBlock The start block index.
 * @param blockCount The number of blocks to write.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_SDMMC_NotSupportYet Not support now.
 * @retval kStatus_SDMMC_CardNotSupport Card not support.
 * @retval kStatus_SDMMC_WaitWriteCompleteFailed Send status failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_StopTransmissionFailed Stop transmission failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SD_WriteBlocks(sd_card_t *card, const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);

/*!
 * @brief Erases blocks of the specific card.
 *
 * This function erases blocks of the specific card with default block size 512 bytes.
 *
 * @param card Card descriptor.
 * @param startBlock The start block index.
 * @param blockCount The number of blocks to erase.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_SDMMC_WaitWriteCompleteFailed Send status failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_WaitWriteCompleteFailed Send status failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SD_EraseBlocks(sd_card_t *card, uint32_t startBlock, uint32_t blockCount);

/*!
 * @brief select card driver strength
 * select card driver strength
 * @param card Card descriptor.
 * @param driverStrength Driver strength
 */
status_t SD_SetDriverStrength(sd_card_t *card, sd_driver_strength_t driverStrength);

/*!
 * @brief select max current
 * select max operation current
 * @param card Card descriptor.
 * @param maxCurrent Max current
 */
status_t SD_SetMaxCurrent(sd_card_t *card, sd_max_current_t maxCurrent);

/* @} */

#if defined(__cplusplus)
}
#endif
/*! @} */
#endif /* _FSL_SD_H_*/
