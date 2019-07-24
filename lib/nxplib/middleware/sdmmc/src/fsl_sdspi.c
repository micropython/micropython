/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <string.h>
#include "fsl_sdspi.h"

/*******************************************************************************
 * Definitons
 ******************************************************************************/
/*! @brief Card command maximum retry times value */
#define SDSPI_TRANSFER_RETRY_TIMES (20000U)
/*! @brief define SDSPI command code length */
#define SDSPI_COMMAND_CODE_BYTE_LEN (6U)
#define SDSPI_COMMAND_FORMAT_GET_INDEX(command) ((command >> 8U) & 0xFFU)
#define SDSPI_COMMAND_FORMAT_GET_RESPONSE_TYPE(command) (command & 0xFFU)
#define SDSPI_COMMAND_IDLE_CRC (0x95U)
#define SDSPI_COMMAND_SEND_INTERFACE_CRC (0x87U)
/*! @brief Reverse byte sequence in uint32_t */
#define SWAP_WORD_BYTE_SEQUENCE(x) (__REV(x))
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Wait card to be ready state.
 *
 * @param host Host state.
 * @retval kStatus_SDSPI_ExchangeFailed Exchange data over SPI failed.
 * @retval kStatus_SDSPI_ResponseError Response is error.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_WaitReady(sdspi_host_t *host);

#if SDSPI_CARD_CRC_PROTECTION_ENABLE
/*!
 * @brief Calculate CRC7
 *
 * @param buffer Data buffer.
 * @param length Data length.
 * @param crc The orginal crc value.
 * @return Generated CRC7.
 */
static uint32_t SDSPI_GenerateCRC7(uint8_t *buffer, uint32_t length, uint32_t crc);
#endif

/*!
 * @brief Send command.
 *
 * @param host Host state.
 * @param command The command to be wrote.
 * @param arg command argument
 * @param response response buffer
 * @retval kStatus_SDSPI_WaitReadyFailed Wait ready failed.
 * @retval kStatus_SDSPI_ExchangeFailed Exchange data over SPI failed.
 * @retval kStatus_SDSPI_ResponseError Response is error.
 * @retval kStatus_Fail Send command failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_SendCommand(sdspi_host_t *host, uint32_t command, uint32_t arg, uint8_t *response);

/*!
 * @brief Send GO_IDLE command.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDSPI_ExchangeFailed Send timing byte failed.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_SDSPI_ResponseError Response is error.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_GoIdle(sdspi_card_t *card);

/*!
 * @brief Send GET_INTERFACE_CONDITION command.
 *
 * This function checks card interface condition, which includes host supply voltage information and asks the card
 * whether it supports voltage.
 *
 * @param card Card descriptor.
 * @param flags The Host Capacity Support flag
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_SendInterfaceCondition(sdspi_card_t *card, uint32_t *flags);

/*!
 * @brief Send SEND_APPLICATION_COMMAND command.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_SDSPI_ResponseError Response is error.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_SendApplicationCmd(sdspi_card_t *card);

/*!
 * @brief Send GET_OPERATION_CONDITION command.
 *
 * @param card Card descriptor.
 * @param argument Operation condition.
 * @retval kStatus_Timeout Timeout.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_ApplicationSendOperationCondition(sdspi_card_t *card, uint32_t argument);

/*!
 * @brief Send READ_OCR command to get OCR register content.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_SDSPI_ResponseError Response is error.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_ReadOcr(sdspi_card_t *card);

/*!
 * @brief Send SET_BLOCK_SIZE command.
 *
 * This function sets the block length in bytes for SDSC cards. For SDHC cards, it does not affect memory
 * read or write commands, always 512 bytes fixed block length is used.
 * @param card Card descriptor.
 * @param blockSize Block size.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_SetBlockSize(sdspi_card_t *card, uint32_t blockSize);

/*!
 * @brief Read data from card
 *
 * @param host Host state.
 * @param buffer Buffer to save data.
 * @param size The data size to read.
 * @retval kStatus_SDSPI_ResponseError Response is error.
 * @retval kStatus_SDSPI_ExchangeFailed Exchange data over SPI failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_Read(sdspi_host_t *host, uint8_t *buffer, uint32_t size);

/*!
 * @brief Decode CSD register
 *
 * @param card Card descriptor.
 * @param rawCsd Raw CSD register content.
 */
static void SDSPI_DecodeCsd(sdspi_card_t *card, uint8_t *rawCsd);

/*!
 * @brief Send GET-CSD command.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_SDSPI_ReadFailed Read data blocks failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_SendCsd(sdspi_card_t *card);

/*!
 * @brief Decode raw CID register.
 * In our sdspi init function, this function is removed for better code size, if id information
 * is needed, you can call it after the init function directly.
 * @param card Card descriptor.
 * @param rawCid Raw CID register content.
 */
static void SDSPI_DecodeCid(sdspi_card_t *card, uint8_t *rawCid);

/*!
 * @brief Decode SCR register.
 *
 * @param card Card descriptor.
 * @param rawScr Raw SCR register content.
 */
static void SDSPI_DecodeScr(sdspi_card_t *card, uint8_t *rawScr);

/*!
 * @brief Send SEND_SCR command.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_SDSPI_ReadFailed Read data blocks failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_SendScr(sdspi_card_t *card);

/*!
 * @brief Send STOP_TRANSMISSION command to card to stop ongoing data transferring.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDSPI_SendCommandFailed Send command failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_StopTransmission(sdspi_card_t *card);

/*!
 * @brief Write data to card
 *
 * @param host Host state.
 * @param buffer Data to send.
 * @param size Data size.
 * @param token The data token.
 * @retval kStatus_SDSPI_WaitReadyFailed Card is busy error.
 * @retval kStatus_SDSPI_ExchangeFailed Exchange data over SPI failed.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_SDSPI_ResponseError Response is error.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SDSPI_Write(sdspi_host_t *host, uint8_t *buffer, uint32_t size, uint8_t token);

/*!
 * @brief select function.
 *
 * @param card card descriptor.
 * @param group function group.
 * @param function function name.
 */
static status_t SDSPI_SelectFunction(sdspi_card_t *card, uint32_t group, uint32_t function);

/*!
 * @brief Send SWITCH_FUNCTION command to switch the card function group.
 *
 * @param card card descriptor.
 * @param mode 0 to check function group, 1 to switch function group.
 * @param group function group.
 * @param number function name.
 * @status buffer to recieve function status.
 */
static status_t SDSPI_SwitchFunction(
    sdspi_card_t *card, uint32_t mode, uint32_t group, uint32_t number, uint32_t *status);

/*!
 * @brief Erase data for the given block range..
 *
 * @param card card descriptor.
 * @param startBlock start block address.
 * @param blockCount the number of block to be erase.
 */
static status_t SDSPI_Erase(sdspi_card_t *card, uint32_t startBlock, uint32_t blockCount);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t SDSPI_WaitReady(sdspi_host_t *host)
{
    uint8_t response;
    uint8_t timingByte = 0xFFU; /* The byte need to be sent as read/write data block timing requirement */
    uint32_t retryCount = SDSPI_TRANSFER_RETRY_TIMES;

    do
    {
        if (kStatus_Success != host->exchange(&timingByte, &response, 1U))
        {
            return kStatus_SDSPI_ExchangeFailed;
        }

    } while ((response != 0xFFU) && (--retryCount));

    /* Response 0xFF means card is still busy. */
    if (response != 0xFFU)
    {
        return kStatus_SDSPI_ResponseError;
    }

    return kStatus_Success;
}

#if SDSPI_CARD_CRC_PROTECTION_ENABLE
static uint32_t SDSPI_GenerateCRC7(uint8_t *buffer, uint32_t length, uint32_t crc)
{
    uint32_t index;

    static const uint8_t crcTable[] = {0x00U, 0x09U, 0x12U, 0x1BU, 0x24U, 0x2DU, 0x36U, 0x3FU,
                                       0x48U, 0x41U, 0x5AU, 0x53U, 0x6CU, 0x65U, 0x7EU, 0x77U};

    while (length)
    {
        index = (((crc >> 3U) & 0x0FU) ^ ((*buffer) >> 4U));
        crc = ((crc << 4U) ^ crcTable[index]);

        index = (((crc >> 3U) & 0x0FU) ^ ((*buffer) & 0x0FU));
        crc = ((crc << 4U) ^ crcTable[index]);

        buffer++;
        length--;
    }

    return (crc & 0x7FU);
}

static uint16_t SDSPI_GenerateCRC16(uint8_t *buffer, uint32_t length, uint16_t crc)
{
    while (length)
    {
        crc = (uint8_t)(crc >> 8U) | (crc << 8U);
        crc ^= *buffer++;
        crc ^= (uint8_t)(crc & 0xffU) >> 4U;
        crc ^= (crc << 8U) << 4U;
        crc ^= ((crc & 0xffU) << 4U) << 1U;
        length--;
    }

    return (crc >> 8U) | (crc << 8U);
}
#endif

static status_t SDSPI_SendCommand(sdspi_host_t *host, uint32_t command, uint32_t arg, uint8_t *response)
{
    assert(host);
    assert(response);

    uint32_t i;
    uint8_t timingByte = 0xFFU; /* The byte need to be sent as read/write data block timing requirement */
    uint8_t buffer[SDSPI_COMMAND_CODE_BYTE_LEN] = {0U};
    uint32_t responseType = SDSPI_COMMAND_FORMAT_GET_RESPONSE_TYPE(command);
    uint8_t index = SDSPI_COMMAND_FORMAT_GET_INDEX(command);

    if ((kStatus_Success != SDSPI_WaitReady(host)) && (index != kSDMMC_GoIdleState))
    {
        return kStatus_SDSPI_WaitReadyFailed;
    }

    /* Send command. */
    buffer[0U] = (index | 0x40U);
    buffer[1U] = ((arg >> 24U) & 0xFFU);
    buffer[2U] = ((arg >> 16U) & 0xFFU);
    buffer[3U] = ((arg >> 8U) & 0xFFU);
    buffer[4U] = (arg & 0xFFU);
#if SDSPI_CARD_CRC_PROTECTION_ENABLE
    buffer[5U] = ((SDSPI_GenerateCRC7(buffer, 5U, 0U) << 1U) | 1U);
#else
    if (index == kSDMMC_GoIdleState)
    {
        buffer[5U] = SDSPI_COMMAND_IDLE_CRC;
    }
    else if (index == kSD_SendInterfaceCondition)
    {
        buffer[5U] = SDSPI_COMMAND_SEND_INTERFACE_CRC;
    }
#endif

    if (host->exchange(buffer, NULL, SDSPI_COMMAND_CODE_BYTE_LEN))
    {
        return kStatus_SDSPI_ExchangeFailed;
    }

    /* Wait for the response coming, the left most bit which is transfered first in first response byte is 0 */
    for (i = 0U; i < 9U; i++)
    {
        if (kStatus_Success != host->exchange(&timingByte, &response[0U], 1U))
        {
            return kStatus_SDSPI_ExchangeFailed;
        }

        /* Check if response 0 coming. */
        if (!(response[0U] & 0x80U))
        {
            break;
        }
    }

    if (response[0U] & 0x80U) /* Max index byte is high means response comming. */
    {
        return kStatus_SDSPI_ResponseError;
    }

    if (responseType != kSDSPI_ResponseTypeR1)
    {
        if (responseType == kSDSPI_ResponseTypeR1b)
        {
            if (kStatus_Success != SDSPI_WaitReady(host))
            {
                return kStatus_SDSPI_WaitReadyFailed;
            }
        }
        else if (responseType == kSDSPI_ResponseTypeR2)
        {
            if (kStatus_Success != host->exchange(&timingByte, &(response[1U]), 1U))
            {
                return kStatus_SDSPI_ExchangeFailed;
            }
        }
        else if ((responseType == kSDSPI_ResponseTypeR3) || (responseType == kSDSPI_ResponseTypeR7))
        {
            /* Left 4 bytes in response type R3 and R7(total 5 bytes in SPI mode) */
            if (kStatus_Success != host->exchange(&timingByte, &(response[1U]), 4U))
            {
                return kStatus_SDSPI_ExchangeFailed;
            }
        }
    }

    return kStatus_Success;
}

#if SDSPI_CARD_CRC_PROTECTION_ENABLE
status_t SDSPI_CommandCrc(sdspi_card_t *card, bool enable)
{
    uint8_t response = 0U;

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdCrc, enable, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    return kStatus_Success;
}
#endif

static status_t SDSPI_GoIdle(sdspi_card_t *card)
{
    assert(card);
    assert(card->host);

    uint8_t response = 0U;
    uint32_t retryCount = SDSPI_TRANSFER_RETRY_TIMES;

    /* SD card will enter SPI mode if the CS is asserted (negative) during the reception of the reset command (CMD0)
    and the card will be IDLE state. */
    do
    {
        if ((kStatus_Success == SDSPI_SendCommand(card->host, kSDSPI_CmdGoIdle, 0U, &response)) &&
            (response == kSDSPI_R1InIdleStateFlag))
        {
            break;
        }
    } while (--retryCount);

    return kStatus_Success;
}

static status_t SDSPI_SendInterfaceCondition(sdspi_card_t *card, uint32_t *flags)
{
    assert(card);
    assert(card->host);

    uint8_t response[5U] = {0U};
    uint32_t i = SDSPI_TRANSFER_RETRY_TIMES;

    do
    {
        /* CMD8 is used to check if the card accept the current supply voltage and check if
        the card support CMD8 */
        if (kStatus_Success == SDSPI_SendCommand(card->host, kSDSPI_CmdSendInterfaceCondition, 0x1AAU, response))
        {
            /* not support CMD8, clear hcs flag */
            if (response[0U] & kSDSPI_R1IllegalCommandFlag)
            {
                return kStatus_Success;
            }
            /* if VCA is set and pattern is match, then break */
            if ((response[3U] == 0x1U) && (response[4U] == 0xAAU))
            {
                *flags |= SDMMC_MASK(kSD_OcrHostCapacitySupportFlag);
                return kStatus_Success;
            }
            /* if VCA in the reponse not set, then the card not support current voltage, return fail */
            else if (response[3U] == 0U)
            {
                return kStatus_SDSPI_InvalidVoltage;
            }
        }
        else
        {
            return kStatus_SDSPI_SendCommandFailed;
        }
    } while (--i);

    return kStatus_Fail;
}

static status_t SDSPI_SendApplicationCmd(sdspi_card_t *card)
{
    assert(card);
    assert(card->host);

    uint8_t response = 0U;

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdApplicationCmd, 0U, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    if (response && (!(response & kSDSPI_R1InIdleStateFlag)))
    {
        return kStatus_SDSPI_ResponseError;
    }

    return kStatus_Success;
}

static status_t SDSPI_ApplicationSendOperationCondition(sdspi_card_t *card, uint32_t argument)
{
    assert(card);
    assert(card->host);

    uint8_t response = 0U;
    uint32_t i = SDSPI_TRANSFER_RETRY_TIMES;

    do
    {
        if (kStatus_Success == SDSPI_SendApplicationCmd(card))
        {
            if (kStatus_Success ==
                SDSPI_SendCommand(card->host, kSDSPI_CmdAppSendOperationCondition, argument, &response))
            {
                if ((response & kSDSPI_R1InIdleStateFlag) == 0U)
                {
                    return kStatus_Success;
                }
            }
        }

    } while (--i);

    return kStatus_Fail;
}

static status_t SDSPI_ReadOcr(sdspi_card_t *card)
{
    assert(card);
    assert(card->host);

    uint32_t i = 0U;
    uint8_t response[5U] = {0U};

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdReadOcr, 0U, response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    if (response[0U])
    {
        return kStatus_SDSPI_ResponseError;
    }

    /* Switch the bytes sequence. All register's content is transferred from highest byte to lowest byte. */
    card->ocr = 0U;
    for (i = 4U; i > 0U; i--)
    {
        card->ocr |= (uint32_t)response[i] << ((4U - i) * 8U);
    }

    if (card->ocr & SDMMC_MASK(kSD_OcrCardCapacitySupportFlag))
    {
        card->flags |= kSDSPI_SupportHighCapacityFlag;
    }

    return kStatus_Success;
}

static status_t SDSPI_SetBlockSize(sdspi_card_t *card, uint32_t blockSize)
{
    assert(card);
    assert(card->host);

    uint8_t response = 0U;

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdSetBlockLength, blockSize, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    return kStatus_Success;
}

static void SDSPI_DecodeCsd(sdspi_card_t *card, uint8_t *rawCsd)
{
    assert(rawCsd);
    assert(card);

    sd_csd_t *csd = &(card->csd);

    csd->csdStructure = (rawCsd[0U] >> 6U);
    csd->dataReadAccessTime1 = rawCsd[1U];
    csd->dataReadAccessTime2 = rawCsd[2U];
    csd->transferSpeed = rawCsd[3U];
    csd->cardCommandClass = (((uint32_t)rawCsd[4U] << 4U) | ((uint32_t)rawCsd[5U] >> 4U));
    csd->readBlockLength = ((rawCsd)[5U] & 0xFU);
    if (rawCsd[6U] & 0x80U)
    {
        csd->flags |= kSD_CsdReadBlockPartialFlag;
    }
    if (rawCsd[6U] & 0x40U)
    {
        csd->flags |= kSD_CsdWriteBlockMisalignFlag;
    }
    if (rawCsd[6U] & 0x20U)
    {
        csd->flags |= kSD_CsdReadBlockMisalignFlag;
    }
    if (rawCsd[6U] & 0x10U)
    {
        csd->flags |= kSD_CsdDsrImplementedFlag;
    }

    /* Some fileds is different when csdStructure is different. */
    if (csd->csdStructure == 0U) /* Decode the bits when CSD structure is version 1.0 */
    {
        csd->deviceSize =
            ((((uint32_t)rawCsd[6] & 0x3U) << 10U) | ((uint32_t)rawCsd[7U] << 2U) | ((uint32_t)rawCsd[8U] >> 6U));
        csd->readCurrentVddMin = ((rawCsd[8U] >> 3U) & 7U);
        csd->readCurrentVddMax = (rawCsd[8U] >> 7U);
        csd->writeCurrentVddMin = ((rawCsd[9U] >> 5U) & 7U);
        csd->writeCurrentVddMax = (rawCsd[9U] >> 2U);
        csd->deviceSizeMultiplier = (((rawCsd[9U] & 3U) << 1U) | (rawCsd[10U] >> 7U));

        card->blockCount = (csd->deviceSize + 1U) << (csd->deviceSizeMultiplier + 2U);
        card->blockSize = (1U << (csd->readBlockLength));

        if (card->blockSize != FSL_SDSPI_DEFAULT_BLOCK_SIZE)
        {
            card->blockCount = (card->blockCount * card->blockSize);
            card->blockSize = FSL_SDSPI_DEFAULT_BLOCK_SIZE;
            card->blockCount = (card->blockCount / card->blockSize);
        }
        /* CSD V1.0 support SDSC card only */
        card->flags |= kSDSPI_SupportSdscFlag;
    }
    else if (csd->csdStructure == 1U) /* Decode the bits when CSD structure is version 2.0 */
    {
        card->blockSize = FSL_SDSPI_DEFAULT_BLOCK_SIZE;
        csd->deviceSize =
            ((((uint32_t)rawCsd[7U] & 0x3FU) << 16U) | ((uint32_t)rawCsd[8U] << 8U) | ((uint32_t)rawCsd[9U]));
        if (csd->deviceSize >= 0xFFFFU)
        {
            card->flags |= kSDSPI_SupportSdxcFlag;
        }
        else
        {
            card->flags |= kSDSPI_SupportSdhcFlag;
        }
        card->blockCount = ((csd->deviceSize + 1U) * 1024U);
    }
    else
    {
    }

    if ((rawCsd[10U] >> 6U) & 1U)
    {
        csd->flags |= kSD_CsdEraseBlockEnabledFlag;
    }
    csd->eraseSectorSize = (((rawCsd[10U] & 0x3FU) << 1U) | (rawCsd[11U] >> 7U));
    csd->writeProtectGroupSize = (rawCsd[11U] & 0x7FU);
    if (rawCsd[12U] >> 7U)
    {
        csd->flags |= kSD_CsdWriteProtectGroupEnabledFlag;
    }
    csd->writeSpeedFactor = ((rawCsd[12U] >> 2U) & 7U);
    csd->writeBlockLength = (((rawCsd[12U] & 3U) << 2U) | (rawCsd[13U] >> 6U));
    if ((rawCsd[13U] >> 5U) & 1U)
    {
        csd->flags |= kSD_CsdWriteBlockPartialFlag;
    }
    if (rawCsd[14U] >> 7U)
    {
        csd->flags |= kSD_CsdFileFormatGroupFlag;
    }
    if ((rawCsd[14U] >> 6U) & 1U)
    {
        csd->flags |= kSD_CsdCopyFlag;
    }
    if ((rawCsd[14U] >> 5U) & 1U)
    {
        csd->flags |= kSD_CsdPermanentWriteProtectFlag;
    }
    if ((rawCsd[14U] >> 4U) & 1U)
    {
        csd->flags |= kSD_CsdTemporaryWriteProtectFlag;
    }
    csd->fileFormat = ((rawCsd[14U] >> 2U) & 3U);
}

static status_t SDSPI_SendCsd(sdspi_card_t *card)
{
    assert(card);
    assert(card->host);

    uint8_t response = 0U;

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdSendCsd, 0U, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    if (kStatus_Success != SDSPI_Read(card->host, card->rawCsd, sizeof(card->rawCsd)))
    {
        return kStatus_SDSPI_ReadFailed;
    }

    SDSPI_DecodeCsd(card, card->rawCsd);

    return kStatus_Success;
}

static void SDSPI_DecodeCid(sdspi_card_t *card, uint8_t *rawCid)
{
    assert(card);
    assert(rawCid);

    sd_cid_t *cid = &(card->cid);
    cid->manufacturerID = rawCid[0U];
    cid->applicationID = (((uint32_t)rawCid[1U] << 8U) | (uint32_t)(rawCid[2U]));
    memcpy(cid->productName, &rawCid[3U], SD_PRODUCT_NAME_BYTES);
    cid->productVersion = rawCid[8U];
    cid->productSerialNumber = (((uint32_t)rawCid[9U] << 24U) | ((uint32_t)rawCid[10U] << 16U) |
                                ((uint32_t)rawCid[11U] << 8U) | ((uint32_t)rawCid[12U]));
    cid->manufacturerData = ((((uint32_t)rawCid[13U] & 0x0FU) << 8U) | ((uint32_t)rawCid[14U]));
}

status_t SDSPI_SendCid(sdspi_card_t *card)
{
    assert(card);
    assert(card->host);

    uint8_t response = 0U;

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdSendCid, 0U, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    if (kStatus_Success != (SDSPI_Read(card->host, card->rawCid, sizeof(card->rawCid))))
    {
        return kStatus_SDSPI_ReadFailed;
    }

    SDSPI_DecodeCid(card, card->rawCid);

    return kStatus_Success;
}

static void SDSPI_DecodeScr(sdspi_card_t *card, uint8_t *rawScr)
{
    assert(card);
    assert(rawScr);

    sd_scr_t *scr = &(card->scr);
    scr->scrStructure = ((rawScr[0U] & 0xF0U) >> 4U);
    scr->sdSpecification = (rawScr[0U] & 0x0FU);
    if (rawScr[1U] & 0x80U)
    {
        scr->flags |= kSD_ScrDataStatusAfterErase;
    }
    scr->sdSecurity = ((rawScr[1U] & 0x70U) >> 4U);
    scr->sdBusWidths = (rawScr[1U] & 0x0FU);
    if (rawScr[2U] & 0x80U)
    {
        scr->flags |= kSD_ScrSdSpecification3;
    }
    scr->extendedSecurity = ((rawScr[2U] & 0x78U) >> 3U);
    scr->commandSupport = (rawScr[3U] & 0x03U);
}

static status_t SDSPI_SendScr(sdspi_card_t *card)
{
    assert(card);
    assert(card->host);

    uint8_t response = 0U;

    if (kStatus_Success != SDSPI_SendApplicationCmd(card))
    {
        return kStatus_SDSPI_SendApplicationCommandFailed;
    }

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdSendScr, 0U, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    if (kStatus_Success != (SDSPI_Read(card->host, card->rawScr, sizeof(card->rawScr))))
    {
        return kStatus_SDSPI_ReadFailed;
    }

    SDSPI_DecodeScr(card, card->rawScr);

    return kStatus_Success;
}

static status_t SDSPI_StopTransmission(sdspi_card_t *card)
{
    uint8_t response = 0U;

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdStopTransfer, 0U, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    return kStatus_Success;
}

static status_t SDSPI_Write(sdspi_host_t *host, uint8_t *buffer, uint32_t size, uint8_t token)
{
    assert(host);
    assert(host->exchange);

    uint8_t response;
    uint16_t timingByte = 0xFFFFU; /* The byte need to be sent as read/write data block timing requirement */

    if (kStatus_Success != SDSPI_WaitReady(host))
    {
        return kStatus_SDSPI_WaitReadyFailed;
    }

    /* Write data token. */
    if (host->exchange(&token, NULL, 1U))
    {
        return kStatus_SDSPI_ExchangeFailed;
    }
    if (token == kSDSPI_DataTokenStopTransfer)
    {
        return kStatus_Success;
    }

    if ((!size) || (!buffer))
    {
        return kStatus_InvalidArgument;
    }

    /* Write data. */
    if (kStatus_Success != host->exchange(buffer, NULL, size))
    {
        return kStatus_SDSPI_ExchangeFailed;
    }

#if SDSPI_CARD_CRC_PROTECTION_ENABLE
    timingByte = SDSPI_GenerateCRC16(buffer, size, 0U);
#endif

    /* Get the last two bytes CRC */
    if (host->exchange((uint8_t *)&timingByte, NULL, 2U))
    {
        return kStatus_SDSPI_ExchangeFailed;
    }
    /* Get the response token. */
    if (host->exchange((uint8_t *)&timingByte, &response, 1U))
    {
        return kStatus_SDSPI_ExchangeFailed;
    }
    if ((response & SDSPI_DATA_RESPONSE_TOKEN_MASK) != kSDSPI_DataResponseTokenAccepted)
    {
        return kStatus_SDSPI_ResponseError;
    }

    return kStatus_Success;
}

static status_t SDSPI_Read(sdspi_host_t *host, uint8_t *buffer, uint32_t size)
{
    assert(host);
    assert(host->exchange);
    assert(buffer);
    assert(size);

    uint8_t response;
    uint32_t i = SDSPI_TRANSFER_RETRY_TIMES;
    uint16_t timingByte = 0xFFFFU; /* The byte need to be sent as read/write data block timing requirement */
    uint16_t crc = 0U;

    memset(buffer, 0xFFU, size);

    /* Wait data token comming */
    do
    {
        if (kStatus_Success != host->exchange((uint8_t *)&timingByte, &response, 1U))
        {
            return kStatus_SDSPI_ExchangeFailed;
        }

    } while ((response == 0xFFU) && (--i));

    /* Check data token and exchange data. */
    if (response != kSDSPI_DataTokenBlockRead)
    {
        return kStatus_SDSPI_ResponseError;
    }
    if (host->exchange(buffer, buffer, size))
    {
        return kStatus_SDSPI_ExchangeFailed;
    }

    /* Get 16 bit CRC */
    if (kStatus_Success != host->exchange((uint8_t *)&timingByte, (uint8_t *)&crc, 2U))
    {
        return kStatus_SDSPI_ExchangeFailed;
    }

    return kStatus_Success;
}

static status_t SDSPI_Erase(sdspi_card_t *card, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(blockCount);

    uint8_t response = 0U;
    uint32_t eraseBlockStart;
    uint32_t eraseBlockEnd;

    if (kStatus_Success != SDSPI_WaitReady(card->host))
    {
        return kStatus_SDSPI_WaitReadyFailed;
    }

    eraseBlockStart =
        (card->flags & kSDSPI_SupportHighCapacityFlag) == 0U ? (startBlock * card->blockSize) : startBlock;
    eraseBlockEnd = eraseBlockStart +
                    ((card->flags & kSDSPI_SupportHighCapacityFlag) == 0U ? card->blockSize : 1U) * (blockCount - 1U);

    /* set erase start address */
    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdWrBlkEraseStart, eraseBlockStart, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }
    /* set erase end address */
    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdWrBlkEraseEnd, eraseBlockEnd, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }
    /* start erase */
    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdWrBlkErase, 0U, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    return kStatus_Success;
}

static status_t SDSPI_SwitchFunction(
    sdspi_card_t *card, uint32_t mode, uint32_t group, uint32_t number, uint32_t *status)
{
    assert(card);
    assert(status);

    uint8_t response = 0u;
    uint32_t argument = 0U;

    argument = (mode << 31U | 0x00FFFFFFU);
    argument &= ~((uint32_t)(0xFU) << (group * 4U));
    argument |= (number << (group * 4U));

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdSwitch, argument, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    if (kStatus_Success != (SDSPI_Read(card->host, (uint8_t *)status, 64U)))
    {
        return kStatus_SDSPI_ReadFailed;
    }

    return kStatus_Success;
}

static status_t SDSPI_SelectFunction(sdspi_card_t *card, uint32_t group, uint32_t function)
{
    assert(card);

    uint32_t functionStatus[16U] = {0U};
    uint16_t functionGroupInfo[6U] = {0};
    uint32_t currentFunctionStatus = 0U;

    /* check if card support CMD6 */
    if (!(card->csd.cardCommandClass & kSDMMC_CommandClassSwitch))
    {
        return kStatus_SDSPI_NotSupportYet;
    }

    /* Check if card support high speed mode. */
    if (kStatus_Success != SDSPI_SwitchFunction(card, kSD_SwitchCheck, group, function, functionStatus))
    {
        return kStatus_SDSPI_ExchangeFailed;
    }
    /* In little endian mode, SD bus byte transferred first is the byte stored in lowest byte position in
    a word which will cause 4 byte's sequence in a word is not consistent with their original sequence from
    card. So the sequence of 4 bytes received in a word should be converted. */
    functionStatus[0U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[0U]);
    functionStatus[1U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[1U]);
    functionStatus[2U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[2U]);
    functionStatus[3U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[3U]);
    functionStatus[4U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[4U]);

    /* -functionStatus[0U]---bit511~bit480;
       -functionStatus[1U]---bit479~bit448;
       -functionStatus[2U]---bit447~bit416;
       -functionStatus[3U]---bit415~bit384;
       -functionStatus[4U]---bit383~bit352;
       According to the "switch function status[bits 511~0]" return by switch command in mode "check function":
       -Check if function 1(high speed) in function group 1 is supported by checking if bit 401 is set;
       -check if function 1 is ready and can be switched by checking if bits 379~376 equal value 1;
     */
    functionGroupInfo[5U] = (uint16_t)functionStatus[0U];
    functionGroupInfo[4U] = (uint16_t)(functionStatus[1U] >> 16U);
    functionGroupInfo[3U] = (uint16_t)(functionStatus[1U]);
    functionGroupInfo[2U] = (uint16_t)(functionStatus[2U] >> 16U);
    functionGroupInfo[1U] = (uint16_t)(functionStatus[2U]);
    functionGroupInfo[0U] = (uint16_t)(functionStatus[3U] >> 16U);
    currentFunctionStatus = ((functionStatus[3U] & 0xFFU) << 8U) | (functionStatus[4U] >> 24U);

    /* check if function is support */
    if (((functionGroupInfo[group] & (1 << function)) == 0U) ||
        ((currentFunctionStatus >> (group * 4U)) & 0xFU) != function)
    {
        return kStatus_SDSPI_NotSupportYet;
    }

    /* Switch to high speed mode. */
    if (kStatus_Success != SDSPI_SwitchFunction(card, kSD_SwitchSet, group, function, functionStatus))
    {
        return kStatus_SDSPI_ExchangeFailed;
    }
    /* In little endian mode is little endian, SD bus byte transferred first is the byte stored in lowest byte
    position in a word which will cause 4 byte's sequence in a word is not consistent with their original
    sequence from card. So the sequence of 4 bytes received in a word should be converted. */
    functionStatus[3U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[3U]);
    functionStatus[4U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[4U]);

    /* According to the "switch function status[bits 511~0]" return by switch command in mode "set function":
       -check if group 1 is successfully changed to function 1 by checking if bits 379~376 equal value 1;
     */
    currentFunctionStatus = ((functionStatus[3U] & 0xFFU) << 8U) | (functionStatus[4U] >> 24U);

    if (((currentFunctionStatus >> (group * 4U)) & 0xFU) != function)
    {
        return kStatus_SDSPI_SwitchCmdFail;
    }

    return kStatus_Success;
}

status_t SDSPI_Init(sdspi_card_t *card)
{
    assert(card);
    assert(card->host);
    assert(card->host->setFrequency);
    assert(card->host->exchange);

    uint32_t applicationCommand41Argument = 0U;

    /* Card must be initialized in 400KHZ. */
    if (card->host->setFrequency(SDMMC_CLOCK_400KHZ))
    {
        return kStatus_SDSPI_SetFrequencyFailed;
    }

    /* Reset the card by CMD0. */
    if (kStatus_Success != SDSPI_GoIdle(card))
    {
        return kStatus_SDSPI_GoIdleFailed;
    }

    /* Check the card's supported interface condition. */
    if (kStatus_Success != SDSPI_SendInterfaceCondition(card, &applicationCommand41Argument))
    {
        return kStatus_SDSPI_SendInterfaceConditionFailed;
    }

#if SDSPI_CARD_CRC_PROTECTION_ENABLE
    /* enable command crc protection. */
    if (kStatus_Success != SDSPI_CommandCrc(card, true))
    {
        return kStatus_SDSPI_SwitchCmdFail;
    }
#endif

    /* Set card's interface condition according to host's capability and card's supported interface condition */
    if (kStatus_Success != SDSPI_ApplicationSendOperationCondition(card, applicationCommand41Argument))
    {
        return kStatus_SDSPI_SendOperationConditionFailed;
    }

    if (kStatus_Success != SDSPI_ReadOcr(card))
    {
        return kStatus_SDSPI_ReadOcrFailed;
    }

    /* Force to use 512-byte length block, no matter which version.  */
    if (kStatus_Success != SDSPI_SetBlockSize(card, 512U))
    {
        return kStatus_SDSPI_SetBlockSizeFailed;
    }
    if (kStatus_Success != SDSPI_SendCsd(card))
    {
        return kStatus_SDSPI_SendCsdFailed;
    }

    /* Set to max frequency according to the max frequency information in CSD register. */
    if (kStatus_Success !=
        card->host->setFrequency(SD_CLOCK_25MHZ > card->host->busBaudRate ? card->host->busBaudRate : SD_CLOCK_25MHZ))
    {
        return kStatus_SDSPI_SetFrequencyFailed;
    }

    if (kStatus_Success != SDSPI_SendScr(card))
    {
        return kStatus_SDSPI_SendCsdFailed;
    }

    return kStatus_Success;
}

void SDSPI_Deinit(sdspi_card_t *card)
{
    assert(card);

    memset(card, 0, sizeof(sdspi_card_t));
}

bool SDSPI_CheckReadOnly(sdspi_card_t *card)
{
    assert(card);

    if ((card->csd.flags & kSD_CsdPermanentWriteProtectFlag) || (card->csd.flags & kSD_CsdTemporaryWriteProtectFlag))
    {
        return true;
    }

    return false;
}

status_t SDSPI_ReadBlocks(sdspi_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(card->host);
    assert(buffer);
    assert(blockCount);

    uint32_t i;
    uint8_t response = 0U;

    /* send command */
    if (kStatus_Success !=
        SDSPI_SendCommand(
            card->host, blockCount == 1U ? kSDSPI_CmdReadSigleBlock : kSDSPI_CmdReadMultiBlock,
            ((card->flags & kSDSPI_SupportHighCapacityFlag) == 0U ? (startBlock * card->blockSize) : startBlock),
            &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }
    /* read data */
    for (i = 0U; i < blockCount; i++)
    {
        if (kStatus_Success != SDSPI_Read(card->host, buffer, card->blockSize))
        {
            return kStatus_SDSPI_ReadFailed;
        }
        buffer += card->blockSize;
    }

    /* Write stop transmission command after the last data block. */
    if (blockCount > 1U)
    {
        if (kStatus_Success != SDSPI_StopTransmission(card))
        {
            return kStatus_SDSPI_StopTransmissionFailed;
        }
    }

    return kStatus_Success;
}

status_t SDSPI_WriteBlocks(sdspi_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(card->host);
    assert(buffer);
    assert(blockCount);

    uint32_t i;
    uint8_t response = 0U;

    if (SDSPI_CheckReadOnly(card))
    {
        return kStatus_SDSPI_WriteProtected;
    }

    /* send command */
    if (kStatus_Success !=
        SDSPI_SendCommand(
            card->host, blockCount == 1U ? kSDSPI_CmdWriteSigleBlock : kSDSPI_CmdWriteMultiBlock,
            ((card->flags & kSDSPI_SupportHighCapacityFlag) == 0U ? (startBlock * card->blockSize) : startBlock),
            &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }
    /* check response */
    if (response)
    {
        return kStatus_SDSPI_ResponseError;
    }
    /* write data */
    for (i = 0U; i < blockCount; i++)
    {
        if (kStatus_Success !=
            SDSPI_Write(card->host, buffer, card->blockSize,
                        blockCount == 1U ? kSDSPI_DataTokenSingleBlockWrite : kSDSPI_DataTokenMultipleBlockWrite))
        {
            return kStatus_SDSPI_WriteFailed;
        }
        buffer += card->blockSize;
    }
    /* stop transfer */
    if (blockCount > 1U)
    {
        if (kStatus_Success != SDSPI_Write(card->host, 0U, 0U, kSDSPI_DataTokenStopTransfer))
        {
            return kStatus_SDSPI_WriteFailed;
        }

        /* Wait the card programming end. */
        if (kStatus_Success != SDSPI_WaitReady(card->host))
        {
            return kStatus_SDSPI_WaitReadyFailed;
        }
    }

    return kStatus_Success;
}

status_t SDSPI_SendPreErase(sdspi_card_t *card, uint32_t blockCount)
{
    assert(blockCount > 1U);

    uint8_t response = 0U;

    /* Pre-erase before writing data */
    if (kStatus_Success != SDSPI_SendApplicationCmd(card))
    {
        return kStatus_SDSPI_SendApplicationCommandFailed;
    }

    if (kStatus_Success != SDSPI_SendCommand(card->host, kSDSPI_CmdWrBlkEraseCount, blockCount, &response))
    {
        return kStatus_SDSPI_SendCommandFailed;
    }

    if (response)
    {
        return kStatus_SDSPI_ResponseError;
    }

    return kStatus_Success;
}

status_t SDSPI_EraseBlocks(sdspi_card_t *card, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(blockCount);

    uint32_t blockCountOneTime; /* The block count can be erased in one time sending ERASE_BLOCKS command. */
    uint32_t blockDone = 0U;    /* The block count has been erased. */
    uint32_t blockLeft;         /* Left block count to be erase. */

    blockLeft = blockCount;
    while (blockLeft)
    {
        if (blockLeft > (card->csd.eraseSectorSize + 1U))
        {
            blockCountOneTime = card->csd.eraseSectorSize + 1U;
            blockLeft = blockLeft - blockCountOneTime;
        }
        else
        {
            blockCountOneTime = blockLeft;
            blockLeft = 0U;
        }

        if (SDSPI_Erase(card, (startBlock + blockDone), blockCountOneTime) != kStatus_Success)
        {
            return kStatus_Fail;
        }

        blockDone += blockCountOneTime;
    }

    return kStatus_Success;
}

status_t SDSPI_SwitchToHighSpeed(sdspi_card_t *card)
{
    assert(card);

    if (SDSPI_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionSDR25HighSpeed) == kStatus_Success)
    {
        card->host->setFrequency(SD_CLOCK_50MHZ > card->host->busBaudRate ? card->host->busBaudRate : SD_CLOCK_50MHZ);

        return kStatus_Success;
    }

    return kStatus_Fail;
}
