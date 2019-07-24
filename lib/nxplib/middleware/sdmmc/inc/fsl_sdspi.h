/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SDSPI_H_
#define _FSL_SDSPI_H_

#include "fsl_common.h"
#include "fsl_sdmmc_spec.h"

/******************************************************************************
 * Definitions
 *****************************************************************************/
/*! @brief Driver version. */
#define FSL_SDSPI_DRIVER_VERSION (MAKE_VERSION(2U, 1U, 4U)) /*2.1.4*/

/*! @brief Default block size */
#define FSL_SDSPI_DEFAULT_BLOCK_SIZE (512U)

/*! @brief Dummy byte define, OxFF should be defined as the dummy data */
#if defined(FSL_FEATURE_SOC_DSPI_COUNT) && (FSL_FEATURE_SOC_DSPI_COUNT > 0U)
#undef DSPI_DUMMY_DATA
#define DSPI_DUMMY_DATA (0xFFU) /*!< Dummy data used for Tx if there is no txData. */
#elif defined(FSL_FEATURE_SOC_LPSPI_COUNT) && (FSL_FEATURE_SOC_LPSPI_COUNT > 0U)
#undef LPSPI_DUMMY_DATA
#define LPSPI_DUMMY_DATA (0xFFU) /*!< Dummy data used for tx if there is not txData. */
#else
#undef SPI_DUMMYDATA
#define SPI_DUMMYDATA (0xFFU)
#endif

/*! @brief This macro is used to enable or disable the CRC protection for SD card
* command. The SPI interface is intialized in the CRC off mode by default.However, the
* RESET command(cmd0) that is used to switch the card to SPI mode, is recieved by by the card
* while in SD mode and therefore, shall have a valid CRC filed, after the card put into SPI mode
* , CRC check for all command include CMD0 will be done according to CMD59 setting, host can turn
* CRC option on and off using the CMD59, this command should be call before ACMD41.
* CMD8 CRC verification is always enabled. The host shall set correct CRC in the argument of CMD8.
* If CRC check is enabled, then sdspi code size and read/write performance will be lower than CRC off.
* CRC check is off by default.
*/
#ifndef SDSPI_CARD_CRC_PROTECTION_ENABLE
#define SDSPI_CARD_CRC_PROTECTION_ENABLE 0U
#endif
/*!
 * @addtogroup SDSPI
 * @{
 */

/*! @brief SDSPI API status */
enum _sdspi_status
{
    kStatus_SDSPI_SetFrequencyFailed = MAKE_STATUS(kStatusGroup_SDSPI, 0U), /*!< Set frequency failed */
    kStatus_SDSPI_ExchangeFailed = MAKE_STATUS(kStatusGroup_SDSPI, 1U),     /*!< Exchange data on SPI bus failed */
    kStatus_SDSPI_WaitReadyFailed = MAKE_STATUS(kStatusGroup_SDSPI, 2U),    /*!< Wait card ready failed */
    kStatus_SDSPI_ResponseError = MAKE_STATUS(kStatusGroup_SDSPI, 3U),      /*!< Response is error */
    kStatus_SDSPI_WriteProtected = MAKE_STATUS(kStatusGroup_SDSPI, 4U),     /*!< Write protected */
    kStatus_SDSPI_GoIdleFailed = MAKE_STATUS(kStatusGroup_SDSPI, 5U),       /*!< Go idle failed */
    kStatus_SDSPI_SendCommandFailed = MAKE_STATUS(kStatusGroup_SDSPI, 6U),  /*!< Send command failed */
    kStatus_SDSPI_ReadFailed = MAKE_STATUS(kStatusGroup_SDSPI, 7U),         /*!< Read data failed */
    kStatus_SDSPI_WriteFailed = MAKE_STATUS(kStatusGroup_SDSPI, 8U),        /*!< Write data failed */
    kStatus_SDSPI_SendInterfaceConditionFailed =
        MAKE_STATUS(kStatusGroup_SDSPI, 9U), /*!< Send interface condition failed */
    kStatus_SDSPI_SendOperationConditionFailed =
        MAKE_STATUS(kStatusGroup_SDSPI, 10U),                                    /*!< Send operation condition failed */
    kStatus_SDSPI_ReadOcrFailed = MAKE_STATUS(kStatusGroup_SDSPI, 11U),          /*!< Read OCR failed */
    kStatus_SDSPI_SetBlockSizeFailed = MAKE_STATUS(kStatusGroup_SDSPI, 12U),     /*!< Set block size failed */
    kStatus_SDSPI_SendCsdFailed = MAKE_STATUS(kStatusGroup_SDSPI, 13U),          /*!< Send CSD failed */
    kStatus_SDSPI_SendCidFailed = MAKE_STATUS(kStatusGroup_SDSPI, 14U),          /*!< Send CID failed */
    kStatus_SDSPI_StopTransmissionFailed = MAKE_STATUS(kStatusGroup_SDSPI, 15U), /*!< Stop transmission failed */
    kStatus_SDSPI_SendApplicationCommandFailed =
        MAKE_STATUS(kStatusGroup_SDSPI, 16U),                            /*!< Send application command failed */
    kStatus_SDSPI_InvalidVoltage = MAKE_STATUS(kStatusGroup_SDSPI, 17U), /*!< invaild supply voltage */
    kStatus_SDSPI_SwitchCmdFail = MAKE_STATUS(kStatusGroup_SDSPI, 18U),  /*!< switch command crc protection on/off */
    kStatus_SDSPI_NotSupportYet = MAKE_STATUS(kStatusGroup_SDSPI, 19U),  /*!< not support */

};

/*! @brief SDSPI card flag */
enum _sdspi_card_flag
{
    kSDSPI_SupportHighCapacityFlag = (1U << 0U), /*!< Card is high capacity */
    kSDSPI_SupportSdhcFlag = (1U << 1U),         /*!< Card is SDHC */
    kSDSPI_SupportSdxcFlag = (1U << 2U),         /*!< Card is SDXC */
    kSDSPI_SupportSdscFlag = (1U << 3U),         /*!< Card is SDSC */
};

/*! @brief SDSPI response type */
enum _sdspi_response_type
{
    kSDSPI_ResponseTypeR1 = 0U,  /*!< Response 1 */
    kSDSPI_ResponseTypeR1b = 1U, /*!< Response 1 with busy */
    kSDSPI_ResponseTypeR2 = 2U,  /*!< Response 2 */
    kSDSPI_ResponseTypeR3 = 3U,  /*!< Response 3 */
    kSDSPI_ResponseTypeR7 = 4U,  /*!< Response 7 */
};

/*! @brief SDSPI command type */
enum _sdspi_cmd
{
    kSDSPI_CmdGoIdle = kSDMMC_GoIdleState << 8U | kSDSPI_ResponseTypeR1, /*!< command go idle */
    kSDSPI_CmdCrc = kSDSPI_CommandCrc << 8U | kSDSPI_ResponseTypeR1,     /*!< command crc protection */
    kSDSPI_CmdSendInterfaceCondition =
        kSD_SendInterfaceCondition << 8U | kSDSPI_ResponseTypeR7, /*!< command send interface condition */
    kSDSPI_CmdApplicationCmd = kSDMMC_ApplicationCommand << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdAppSendOperationCondition = kSD_ApplicationSendOperationCondition << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdReadOcr = kSDMMC_ReadOcr << 8U | kSDSPI_ResponseTypeR3,
    kSDSPI_CmdSetBlockLength = kSDMMC_SetBlockLength << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdSendCsd = kSDMMC_SendCsd << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdSendCid = kSDMMC_SendCid << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdSendScr = kSD_ApplicationSendScr << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdStopTransfer = kSDMMC_StopTransmission << 8U | kSDSPI_ResponseTypeR1b,
    kSDSPI_CmdWriteSigleBlock = kSDMMC_WriteSingleBlock << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdWriteMultiBlock = kSDMMC_WriteMultipleBlock << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdReadSigleBlock = kSDMMC_ReadSingleBlock << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdReadMultiBlock = kSDMMC_ReadMultipleBlock << 8U | kSDSPI_ResponseTypeR1,

    kSDSPI_CmdWrBlkEraseCount = kSD_ApplicationSetWriteBlockEraseCount << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdWrBlkEraseStart = kSD_EraseWriteBlockStart << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdWrBlkEraseEnd = kSD_EraseWriteBlockEnd << 8U | kSDSPI_ResponseTypeR1,
    kSDSPI_CmdWrBlkErase = kSDMMC_Erase << 8U | kSDSPI_ResponseTypeR1b,

    kSDSPI_CmdSwitch = kSD_Switch << 8U | kSDSPI_ResponseTypeR1,

};

/*! @brief SDSPI host state. */
typedef struct _sdspi_host
{
    uint32_t busBaudRate; /*!< Bus baud rate */

    status_t (*setFrequency)(uint32_t frequency);                   /*!< Set frequency of SPI */
    status_t (*exchange)(uint8_t *in, uint8_t *out, uint32_t size); /*!< Exchange data over SPI */
} sdspi_host_t;

/*!
 * @brief SD Card Structure
 *
 * Define the card structure including the necessary fields to identify and describe the card.
 */
typedef struct _sdspi_card
{
    sdspi_host_t *host;       /*!< Host state information */
    uint32_t relativeAddress; /*!< Relative address of the card */
    uint32_t flags;           /*!< Flags defined in _sdspi_card_flag. */
    uint8_t rawCid[16U];      /*!< Raw CID content */
    uint8_t rawCsd[16U];      /*!< Raw CSD content */
    uint8_t rawScr[8U];       /*!< Raw SCR content */
    uint32_t ocr;             /*!< Raw OCR content */
    sd_cid_t cid;             /*!< CID */
    sd_csd_t csd;             /*!< CSD */
    sd_scr_t scr;             /*!< SCR */
    uint32_t blockCount;      /*!< Card total block number */
    uint32_t blockSize;       /*!< Card block size */
} sdspi_card_t;

/*************************************************************************************************
 * API
 ************************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name SDSPI Function
 * @{
 */

/*!
 * @brief Initializes the card on a specific SPI instance.
 *
 * This function initializes the card on a specific SPI instance.
 *
 * @param card Card descriptor
 * @retval kStatus_SDSPI_SetFrequencyFailed Set frequency failed.
 * @retval kStatus_SDSPI_GoIdleFailed Go idle failed.
 * @retval kStatus_SDSPI_SendInterfaceConditionFailed Send interface condition failed.
 * @retval kStatus_SDSPI_SendOperationConditionFailed Send operation condition failed.
 * @retval kStatus_Timeout Send command timeout.
 * @retval kStatus_SDSPI_NotSupportYet Not support yet.
 * @retval kStatus_SDSPI_ReadOcrFailed Read OCR failed.
 * @retval kStatus_SDSPI_SetBlockSizeFailed Set block size failed.
 * @retval kStatus_SDSPI_SendCsdFailed Send CSD failed.
 * @retval kStatus_SDSPI_SendCidFailed Send CID failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SDSPI_Init(sdspi_card_t *card);

/*!
 * @brief Deinitializes the card.
 *
 * This function deinitializes the specific card.
 *
 * @param card Card descriptor
 */
void SDSPI_Deinit(sdspi_card_t *card);

/*!
 * @brief Checks whether the card is write-protected.
 *
 * This function checks if the card is write-protected via CSD register.
 *
 * @param card Card descriptor.
 * @retval true Card is read only.
 * @retval false Card isn't read only.
 */
bool SDSPI_CheckReadOnly(sdspi_card_t *card);

/*!
 * @brief Reads blocks from the specific card.
 *
 * This function reads blocks from specific card.
 *
 * @param card Card descriptor.
 * @param buffer the buffer to hold the data read from card
 * @param startBlock the start block index
 * @param blockCount the number of blocks to read
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_SDSPI_ReadFailed Read data failed.
 * @retval kStatus_SDSPI_StopTransmissionFailed Stop transmission failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SDSPI_ReadBlocks(sdspi_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);

/*!
 * @brief Writes blocks of data to the specific card.
 *
 * This function writes blocks to specific card
 *
 * @param card Card descriptor.
 * @param buffer the buffer holding the data to be written to the card
 * @param startBlock the start block index
 * @param blockCount the number of blocks to write
 * @retval kStatus_SDSPI_WriteProtected Card is write protected.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_SDSPI_ResponseError Response is error.
 * @retval kStatus_SDSPI_WriteFailed Write data failed.
 * @retval kStatus_SDSPI_ExchangeFailed Exchange data over SPI failed.
 * @retval kStatus_SDSPI_WaitReadyFailed Wait card to be ready status failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SDSPI_WriteBlocks(sdspi_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount);

/*!
 * @brief Send GET-CID command
 * In our sdspi init function, this function is removed for better code size, if id information
 * is needed, you can call it after the init function directly.
 * @param card Card descriptor.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_SDSPI_ReadFailed Read data blocks failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SDSPI_SendCid(sdspi_card_t *card);

#if SDSPI_CARD_CMD_CRC_PROTECTION_ENABLE
/*!
 * @brief Command CRC protection on/off.
 * The SPI interface is intialized in the CRC off mode in default.However, the
 * RESET command(cmd0) that is used to switch the card to SPI mode, is recieved by by the card
 * while in SD mode and therefore, shall have a valid CRC filed, after the card put into SPI mode
 * , CRC check for all command include CMD0 will be done according to CMD59 setting, host can turn
 * CRC option on and off using the CMD59, this command should be call before ACMD41.
 * @param card Card descriptor.
 * @param enable true is enable command crc protection, false is diable command crc protection.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SDSPI_CommandCrc(sdspi_card_t *card, bool enable);
#endif

/*!
 * @brief Multiple blocks write pre-erase function.
 * This function should be called before SDSPI_WriteBlocks, it is used to set the
 * number of the write blocks to be pre-erased before writing.
 * @param card Card descriptor.
 * @param blockCount the block counts to be write.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_SDSPI_SendApplicationCommandFailed
 * @retval kStatus_SDSPI_ResponseError
 * @retval kStatus_Success Operate successfully.
 */
status_t SDSPI_SendPreErase(sdspi_card_t *card, uint32_t blockCount);

/*!
 * @brief Block erase function.
 * @param card Card descriptor.
 * @param startBlock start block address to be erase.
 * @param blockCount the block counts to be erase.
 * @retval kStatus_SDSPI_WaitReadyFailed Wait ready failed.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SDSPI_EraseBlocks(sdspi_card_t *card, uint32_t startBlock, uint32_t blockCount);

/*!
 * @brief Switch to high speed function.
 * This function can be called after SDSPI_Init function if target board's
 * layout support >25MHZ spi baudrate, otherwise this function is useless.Be careful with
 * call this function, code size and stack usage will be enlarge.
 * @param card Card descriptor.
 * @retval kStatus_Fail switch failed.
 * @retval kStatus_Success Operate successfully.
 */
status_t SDSPI_SwitchToHighSpeed(sdspi_card_t *card);

/* @} */
#if defined(__cplusplus)
}
#endif
/*! @} */
#endif /* _FSL_SDSPI_H_ */
