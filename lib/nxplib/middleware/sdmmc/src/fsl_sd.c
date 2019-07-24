/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_sd.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Wait write process complete.
 *
 * @param card Card descriptor.
 * @retval kStatus_Timeout Send command timeout.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_WaitWriteComplete(sd_card_t *card);

/*!
 * @brief send write success blocks.
 *
 * @param card Card descriptor.
 * @param blocks blocks number wirte successed
 * @retval kStatus_SDMMC_TransferFailed Send command failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SendWriteSuccessBlocks(sd_card_t *card, uint32_t *blocks);

/*!
 * @brief Send SEND_APPLICATION_COMMAND command.
 *
 * @param card Card descriptor.
 * @param relativeaddress
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline SD_SendApplicationCmd(sd_card_t *card, uint32_t relativeAddress);

/*!
 * @brief Send GO_IDLE command to set the card to be idle state.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline SD_GoIdle(sd_card_t *card);

/*!
 * @brief Send STOP_TRANSMISSION command after multiple blocks read/write.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_StopTransmission(sd_card_t *card);

/*!
 * @brief Send SET_BLOCK_SIZE command.
 *
 * @param card Card descriptor.
 * @param blockSize Block size.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline SD_SetBlockSize(sd_card_t *card, uint32_t blockSize);

/*!
 * @brief Send GET_RCA command to get card relative address.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SendRca(sd_card_t *card);

/*!
 * @brief Send SWITCH_FUNCTION command to switch the card function group.
 *
 * @param card Card descriptor.
 * @param mode 0 to check function group. 1 to switch function group
 * @param group Function group
 * @param number Function number in the function group.
 * @param status Switch function status.
 * @retval kStatus_SDMMC_SetCardBlockSizeFailed Set card block size failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SwitchFunction(sd_card_t *card, uint32_t mode, uint32_t group, uint32_t number, uint32_t *status);

/*!
 * @brief Decode raw SCR register content in the data blocks.
 *
 * @param card Card descriptor.
 * @param rawScr Raw SCR register content.
 */
static void SD_DecodeScr(sd_card_t *card, uint32_t *rawScr);

/*!
 * @brief Send GET_SCR command.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_SendApplicationCommandFailed Send application command failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_NotSupportYet Not support yet.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SendScr(sd_card_t *card);

/*!
 * @brief Switch the card to be high speed mode.
 *
 * @param card Card descriptor.
 * @param group Group number.
 * @param functio Function number.
 * @retval kStatus_SDMMC_CardNotSupport Card not support.
 * @retval kStatus_SDMMC_SwitchFailed Switch failed.
 * @retval kStatus_SDMMC_NotSupportYet Not support yet.
 * @retval kStatus_Fail Switch failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SelectFunction(sd_card_t *card, uint32_t group, uint32_t function);

/*!
 * @brief Send SET_DATA_WIDTH command to set SD bus width.
 *
 * @param card Card descriptor.
 * @param width Data bus width.
 * @retval kStatus_SDMMC_SendApplicationCommandFailed Send application command failed.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SetDataBusWidth(sd_card_t *card, sd_data_bus_width_t width);

/*!
 * @brief Decode raw CSD register content in the data blocks.
 *
 * @param card Card descriptor.
 * @param rawCsd Raw CSD register content.
 */
static void SD_DecodeCsd(sd_card_t *card, uint32_t *rawCsd);

/*!
 * @brief Send SEND_CSD command to get CSD register content from Card.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SendCsd(sd_card_t *card);

/*!
 * @brief Decode raw CID register content in the data blocks.
 *
 * @param rawCid raw CID register content.
 * @param card Card descriptor.
 */
static void SD_DecodeCid(sd_card_t *card, uint32_t *rawCid);

/*!
 * @brief Send GET_CID command to get CID from card.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_AllSendCid(sd_card_t *card);

/*!
 * @brief Send SEND_OPERATION_CONDITION command.
 *
 * This function sends host capacity support information and asks the accessed card to send its operating condition
 * register content.
 *
 * @param card Card descriptor.
 * @param argument The argument of the send operation condition ncomamnd.
 * @retval kStatus_SDMMC_SendApplicationCommandFailed Send application command failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Timeout Timeout.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_ApplicationSendOperationCondition(sd_card_t *card, uint32_t argument);

/*!
 * @brief Send GET_INTERFACE_CONDITION command to get card interface condition.
 *
 * This function checks card interface condition, which includes host supply voltage information and asks the card
 * whether card supports the specified host voltage.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_SendInterfaceCondition(sd_card_t *card);

/*!
 * @brief Send switch voltage command
 * switch card voltage to 1.8v
 *
 * @param card Card descriptor.
 */
static status_t SD_SwitchVoltage(sd_card_t *card);

/*!
 * @brief select bus timing
 * select card timing
 * @param card Card descriptor.
 */
static status_t SD_SelectBusTiming(sd_card_t *card);

/*!
 * @brief Decode sd 512 bit status
 * @param card Card descriptor.
 * @param 512 bits satus raw data.
 */
static void SD_DecodeStatus(sd_card_t *card, uint32_t *src);

/*!
 * @brief Read data from specific SD card.
 *
 * @param card Card descriptor.
 * @param buffer Buffer to save data blocks read.
 * @param startBlock Card start block number to be read.
 * @param blockSize Block size.
 * @param blockCount Block count.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_SDMMC_WaitWriteCompleteFailed Wait write complete failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_StopTransmissionFailed Stop transmission failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_Read(sd_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount);

/*!
 * @brief Write data to specific card
 *
 * @param card Card descriptor.
 * @param buffer Buffer to be sent.
 * @param startBlock Card start block number to be written.
 * @param blockSize Block size.
 * @param blockCount Block count.
 * @param blockWritten successfully write blocks
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_StopTransmissionFailed Stop transmission failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_Write(sd_card_t *card,
                         const uint8_t *buffer,
                         uint32_t startBlock,
                         uint32_t blockSize,
                         uint32_t blockCount,
                         uint32_t *blockWritten);

/*!
 * @brief Erase data for the given block range.
 *
 * @param card Card descriptor.
 * @param startBlock Card start block number to be erased.
 * @param blockCount The block count to be erased.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t SD_Erase(sd_card_t *card, uint32_t startBlock, uint32_t blockCount);

/*!
 * @brief card transfer function.
 *
 * @param card Card descriptor.
 * @param content Transfer content.
 * @param retry Retry times
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 * @retval kStatus_SDMMC_TuningFail tuning fail
 */
static status_t SD_Transfer(sd_card_t *card, SDMMCHOST_TRANSFER *content, uint32_t retry);

/*!
 * @brief card execute tuning function.
 *
 * @param card Card descriptor.
 * @retval kStatus_Success Operate successfully.
 * @retval kStatus_SDMMC_TuningFail tuning fail.
 * @retval kStatus_SDMMC_TransferFailed transfer fail
 */
static status_t inline SD_ExecuteTuning(sd_card_t *card);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* g_sdmmc statement */
extern uint32_t g_sdmmc[SDK_SIZEALIGN(SDMMC_GLOBAL_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)];
static uint32_t s_sdAuSizeMap[] = {0,
                                   16 * 1024,
                                   32 * 1024,
                                   64 * 1024,
                                   128 * 1024,
                                   256 * 1024,
                                   512 * 1024,
                                   1024 * 1024,
                                   2 * 1024 * 1024,
                                   4 * 1024 * 1024,
                                   8 * 1024 * 1024,
                                   12 * 1024 * 1024,
                                   16 * 1024 * 1024,
                                   24 * 1024 * 1024,
                                   32 * 1024 * 1024,
                                   64 * 1024 * 1024};
/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t inline SD_SendApplicationCmd(sd_card_t *card, uint32_t relativeAddress)
{
    assert(card);

    return SDMMC_SendApplicationCommand(card->host.base, card->host.transfer, relativeAddress);
}

static status_t inline SD_GoIdle(sd_card_t *card)
{
    assert(card);

    return SDMMC_GoIdle(card->host.base, card->host.transfer);
}

static status_t inline SD_SetBlockSize(sd_card_t *card, uint32_t blockSize)
{
    assert(card);

    return SDMMC_SetBlockSize(card->host.base, card->host.transfer, blockSize);
}

static status_t inline SD_ExecuteTuning(sd_card_t *card)
{
    assert(card);

    return SDMMC_ExecuteTuning(card->host.base, card->host.transfer, kSD_SendTuningBlock, 64U);
}

static status_t SD_SwitchVoltage(sd_card_t *card)
{
    assert(card);

    if ((card->usrParam.cardVoltage != NULL) && (card->usrParam.cardVoltage->cardSignalLine1V8 != NULL))
    {
        return SDMMC_SwitchToVoltage(card->host.base, card->host.transfer,
                                     card->usrParam.cardVoltage->cardSignalLine1V8);
    }

    return SDMMC_SwitchToVoltage(card->host.base, card->host.transfer, NULL);
}

static status_t SD_StopTransmission(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    status_t error = kStatus_Success;

    command.index = kSDMMC_StopTransmission;
    command.argument = 0U;
    command.type = kCARD_CommandTypeAbort;
    command.responseType = kCARD_ResponseTypeR1b;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    content.command = &command;
    content.data = 0U;
    error = card->host.transfer(card->host.base, &content);
    if (kStatus_Success != error)
    {
        SDMMC_LOG("\r\nError: send CMD12 failed with host error %d, reponse %x\r\n", error, command.response[0U]);
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

static status_t SD_Transfer(sd_card_t *card, SDMMCHOST_TRANSFER *content, uint32_t retry)
{
    assert(card->host.transfer);
    assert(content);
    status_t error;

    do
    {
        error = card->host.transfer(card->host.base, content);
#if SDMMC_ENABLE_SOFTWARE_TUNING
        if (((error == SDMMCHOST_RETUNING_REQUEST) || (error == SDMMCHOST_TUNING_ERROR)) &&
            (card->currentTiming == kSD_TimingSDR104Mode))
        {
            /* tuning error need reset tuning circuit */
            if (error == SDMMCHOST_TUNING_ERROR)
            {
                SDMMCHOST_RESET_TUNING(card->host.base, 100U);
            }

            /* execute re-tuning */
            if (SD_ExecuteTuning(card) != kStatus_Success)
            {
                error = kStatus_SDMMC_TuningFail;
                break;
            }
            else
            {
                continue;
            }
        }
        else
#endif
            if (error != kStatus_Success)
        {
            /* if transfer data failed, send cmd12 to abort current transfer */
            if (content->data)
            {
                SD_StopTransmission(card);
            }
        }

        if (retry != 0U)
        {
            retry--;
        }
        else
        {
            break;
        }

    } while (error != kStatus_Success);

    return error;
}

static status_t SD_WaitWriteComplete(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    status_t error = kStatus_Success;

    command.index = kSDMMC_SendStatus;
    command.argument = card->relativeAddress << 16U;
    command.responseType = kCARD_ResponseTypeR1;

    do
    {
        content.command = &command;
        content.data = 0U;
        error = SD_Transfer(card, &content, 2U);
        if (kStatus_Success != error)
        {
            SDMMC_LOG("\r\nError: send CMD13 failed with host error %d, response %x", error, command.response[0U]);
            break;
        }

        if ((command.response[0U] & SDMMC_MASK(kSDMMC_R1ReadyForDataFlag)) &&
            (SDMMC_R1_CURRENT_STATE(command.response[0U]) != kSDMMC_R1StateProgram))
        {
            break;
        }
    } while (true);

    return error;
}

static status_t SD_SendWriteSuccessBlocks(sd_card_t *card, uint32_t *blocks)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};
    status_t error = kStatus_Success;

    memset(g_sdmmc, 0U, sizeof(g_sdmmc));

    /* Wait for the card write process complete because of that card read process and write process use one buffer. */
    if (kStatus_Success != SD_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    if (kStatus_Success != SD_SendApplicationCmd(card, card->relativeAddress))
    {
        return kStatus_SDMMC_SendApplicationCommandFailed;
    }

    command.index = kSD_ApplicationSendNumberWriteBlocks;
    command.responseType = kCARD_ResponseTypeR1;

    data.blockSize = 4U;
    data.blockCount = 1U;
    data.rxData = &g_sdmmc[0];

    content.command = &command;
    content.data = &data;
    error = card->host.transfer(card->host.base, &content);
    if ((kStatus_Success != error) || ((command.response[0U]) & SDMMC_R1_ALL_ERROR_FLAG))
    {
        SDMMC_LOG("\r\nError: send ACMD13 failed with host error %d, response %x", error, command.response[0U]);
    }
    else
    {
        *blocks = SWAP_WORD_BYTE_SEQUENCE(g_sdmmc[0]);
    }

    return error;
}

static status_t SD_SendRca(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    status_t error = kStatus_Success;

    command.index = kSD_SendRelativeAddress;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR6;

    content.command = &command;
    content.data = NULL;

    error = card->host.transfer(card->host.base, &content);
    if (kStatus_Success == error)
    {
        card->relativeAddress = (command.response[0U] >> 16U);
    }
    else
    {
        SDMMC_LOG("\r\nError: send CMD3 failed with host error %d, response %x", error, command.response[0U]);
    }

    return error;
}

static status_t SD_SwitchFunction(sd_card_t *card, uint32_t mode, uint32_t group, uint32_t number, uint32_t *status)
{
    assert(card);
    assert(status);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};
    status_t error = kStatus_Success;

    command.index = kSD_Switch;
    command.argument = (mode << 31U | 0x00FFFFFFU);
    command.argument &= ~((uint32_t)(0xFU) << (group * 4U));
    command.argument |= (number << (group * 4U));
    command.responseType = kCARD_ResponseTypeR1;

    data.blockSize = 64U;
    data.blockCount = 1U;
    data.rxData = status;

    content.command = &command;
    content.data = &data;
    error = card->host.transfer(card->host.base, &content);
    if ((kStatus_Success != error) || ((command.response[0U]) & SDMMC_R1_ALL_ERROR_FLAG))
    {
        SDMMC_LOG("\r\n\r\nError: send CMD6 failed with host error %d, response %x", error, command.response[0U]);
    }

    return error;
}

static void SD_DecodeScr(sd_card_t *card, uint32_t *rawScr)
{
    assert(card);
    assert(rawScr);

    sd_scr_t *scr;

    scr = &(card->scr);
    scr->scrStructure = (uint8_t)((rawScr[0U] & 0xF0000000U) >> 28U);
    scr->sdSpecification = (uint8_t)((rawScr[0U] & 0xF000000U) >> 24U);
    if ((uint8_t)((rawScr[0U] & 0x800000U) >> 23U))
    {
        scr->flags |= kSD_ScrDataStatusAfterErase;
    }
    scr->sdSecurity = (uint8_t)((rawScr[0U] & 0x700000U) >> 20U);
    scr->sdBusWidths = (uint8_t)((rawScr[0U] & 0xF0000U) >> 16U);
    if ((uint8_t)((rawScr[0U] & 0x8000U) >> 15U))
    {
        scr->flags |= kSD_ScrSdSpecification3;
    }
    scr->extendedSecurity = (uint8_t)((rawScr[0U] & 0x7800U) >> 10U);
    scr->commandSupport = (uint8_t)(rawScr[0U] & 0x3U);
    scr->reservedForManufacturer = rawScr[1U];
    /* Get specification version. */
    switch (scr->sdSpecification)
    {
        case 0U:
            card->version = kSD_SpecificationVersion1_0;
            break;
        case 1U:
            card->version = kSD_SpecificationVersion1_1;
            break;
        case 2U:
            card->version = kSD_SpecificationVersion2_0;
            if (card->scr.flags & kSD_ScrSdSpecification3)
            {
                card->version = kSD_SpecificationVersion3_0;
            }
            break;
        default:
            break;
    }
    if (card->scr.sdBusWidths & 0x4U)
    {
        card->flags |= kSD_Support4BitWidthFlag;
    }
    /* speed class control cmd */
    if (card->scr.commandSupport & 0x01U)
    {
        card->flags |= kSD_SupportSpeedClassControlCmd;
    }
    /* set block count cmd */
    if (card->scr.commandSupport & 0x02U)
    {
        card->flags |= kSD_SupportSetBlockCountCmd;
    }
}

static status_t SD_SendScr(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};
    uint32_t *rawScr = g_sdmmc;
    status_t error = kStatus_Success;

    /* memset the global buffer */
    memset(g_sdmmc, 0U, sizeof(g_sdmmc));

    if (kStatus_Success != SD_SendApplicationCmd(card, card->relativeAddress))
    {
        return kStatus_SDMMC_SendApplicationCommandFailed;
    }

    command.index = kSD_ApplicationSendScr;
    command.responseType = kCARD_ResponseTypeR1;
    command.argument = 0U;

    data.blockSize = 8U;
    data.blockCount = 1U;
    data.rxData = rawScr;

    content.data = &data;
    content.command = &command;
    error = card->host.transfer(card->host.base, &content);
    if ((kStatus_Success != error) || ((command.response[0U]) & SDMMC_R1_ALL_ERROR_FLAG))
    {
        SDMMC_LOG("\r\nError: send ACMD51 failed with host error %d, response %x", error, command.response[0U]);
    }
    else
    {
        /* SCR register data byte sequence from card is big endian(MSB first). */
        switch (card->host.config.endianMode)
        {
            case kSDMMCHOST_EndianModeLittle:
                /* In little endian mode, SD bus byte transferred first is the byte stored in lowest byte position in a
                word which will cause 4 byte's sequence in a word is not consistent with their original sequence from
                card. So the sequence of 4 bytes received in a word should be converted. */
                rawScr[0U] = SWAP_WORD_BYTE_SEQUENCE(rawScr[0U]);
                rawScr[1U] = SWAP_WORD_BYTE_SEQUENCE(rawScr[1U]);
                break;
            case kSDMMCHOST_EndianModeBig:
                break; /* Doesn't need to switch byte sequence when decodes bytes as big endian sequence. */
            case kSDMMCHOST_EndianModeHalfWordBig:
                rawScr[0U] = SWAP_HALF_WROD_BYTE_SEQUENCE(rawScr[0U]);
                rawScr[1U] = SWAP_HALF_WROD_BYTE_SEQUENCE(rawScr[1U]);
                break;
            default:
                return kStatus_SDMMC_NotSupportYet;
        }
        memcpy(card->rawScr, rawScr, sizeof(card->rawScr));
        /* decode scr */
        SD_DecodeScr(card, rawScr);
    }

    return error;
}

static status_t SD_SelectFunction(sd_card_t *card, uint32_t group, uint32_t function)
{
    assert(card);

    uint32_t *functionStatus = g_sdmmc;
    uint16_t functionGroupInfo[6U] = {0};
    uint32_t currentFunctionStatus = 0U;

    /* memset the global buffer */
    memset(g_sdmmc, 0, sizeof(g_sdmmc));

    /* check if card support CMD6 */
    if ((card->version <= kSD_SpecificationVersion1_0) || (!(card->csd.cardCommandClass & kSDMMC_CommandClassSwitch)))
    {
        SDMMC_LOG("\r\nError: current card not support CMD6");
        return kStatus_SDMMC_NotSupportYet;
    }

    /* Check if card support high speed mode. */
    if (kStatus_Success != SD_SwitchFunction(card, kSD_SwitchCheck, group, function, functionStatus))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Switch function status byte sequence from card is big endian(MSB first). */
    switch (card->host.config.endianMode)
    {
        case kSDMMCHOST_EndianModeLittle:
            /* In little endian mode, SD bus byte transferred first is the byte stored in lowest byte position in
            a word which will cause 4 byte's sequence in a word is not consistent with their original sequence from
            card. So the sequence of 4 bytes received in a word should be converted. */
            functionStatus[0U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[0U]);
            functionStatus[1U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[1U]);
            functionStatus[2U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[2U]);
            functionStatus[3U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[3U]);
            functionStatus[4U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[4U]);
            break;
        case kSDMMCHOST_EndianModeBig:
            break; /* Doesn't need to switch byte sequence when decodes bytes as big endian sequence. */
        case kSDMMCHOST_EndianModeHalfWordBig:
            functionStatus[0U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[0U]);
            functionStatus[1U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[1U]);
            functionStatus[2U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[2U]);
            functionStatus[3U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[3U]);
            functionStatus[4U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[4U]);
            break;
        default:
            return kStatus_SDMMC_NotSupportYet;
    }
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
        SDMMC_LOG("\r\nError: current card not support function %d", function);
        return kStatus_SDMMC_NotSupportYet;
    }

    /* Switch to high speed mode. */
    if (kStatus_Success != SD_SwitchFunction(card, kSD_SwitchSet, group, function, functionStatus))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Switch function status byte sequence from card is big endian(MSB first). */
    switch (card->host.config.endianMode)
    {
        case kSDMMCHOST_EndianModeLittle:
            /* In little endian mode is little endian, SD bus byte transferred first is the byte stored in lowest byte
            position in a word which will cause 4 byte's sequence in a word is not consistent with their original
            sequence from card. So the sequence of 4 bytes received in a word should be converted. */
            functionStatus[3U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[3U]);
            functionStatus[4U] = SWAP_WORD_BYTE_SEQUENCE(functionStatus[4U]);
            break;
        case kSDMMCHOST_EndianModeBig:
            break; /* Doesn't need to switch byte sequence when decodes bytes as big endian sequence. */
        case kSDMMCHOST_EndianModeHalfWordBig:
            functionStatus[3U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[3U]);
            functionStatus[4U] = SWAP_HALF_WROD_BYTE_SEQUENCE(functionStatus[4U]);
            break;
        default:
            return kStatus_SDMMC_NotSupportYet;
    }
    /* According to the "switch function status[bits 511~0]" return by switch command in mode "set function":
       -check if group 1 is successfully changed to function 1 by checking if bits 379~376 equal value 1;
     */
    currentFunctionStatus = ((functionStatus[3U] & 0xFFU) << 8U) | (functionStatus[4U] >> 24U);

    if (((currentFunctionStatus >> (group * 4U)) & 0xFU) != function)
    {
        SDMMC_LOG("\r\nError: switch to function %d failed", function);
        return kStatus_SDMMC_SwitchFailed;
    }

    return kStatus_Success;
}

static status_t SD_SetDataBusWidth(sd_card_t *card, sd_data_bus_width_t width)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    status_t error = kStatus_Success;

    if (kStatus_Success != SD_SendApplicationCmd(card, card->relativeAddress))
    {
        return kStatus_SDMMC_SendApplicationCommandFailed;
    }

    command.index = kSD_ApplicationSetBusWdith;
    command.responseType = kCARD_ResponseTypeR1;
    switch (width)
    {
        case kSD_DataBusWidth1Bit:
            command.argument = 0U;
            break;
        case kSD_DataBusWidth4Bit:
            command.argument = 2U;
            break;
        default:
            return kStatus_InvalidArgument;
    }

    content.command = &command;
    content.data = NULL;
    error = card->host.transfer(card->host.base, &content);
    if ((kStatus_Success != error) || ((command.response[0U]) & SDMMC_R1_ALL_ERROR_FLAG))
    {
        SDMMC_LOG("\r\nError: send ACMD6 failed with host error %d, response %x", error, command.response[0U]);
    }

    return error;
}

static void SD_DecodeCsd(sd_card_t *card, uint32_t *rawCsd)
{
    assert(card);
    assert(rawCsd);

    sd_csd_t *csd;

    csd = &(card->csd);
    csd->csdStructure = (uint8_t)((rawCsd[3U] & 0xC0000000U) >> 30U);
    csd->dataReadAccessTime1 = (uint8_t)((rawCsd[3U] & 0xFF0000U) >> 16U);
    csd->dataReadAccessTime2 = (uint8_t)((rawCsd[3U] & 0xFF00U) >> 8U);
    csd->transferSpeed = (uint8_t)(rawCsd[3U] & 0xFFU);
    csd->cardCommandClass = (uint16_t)((rawCsd[2U] & 0xFFF00000U) >> 20U);
    csd->readBlockLength = (uint8_t)((rawCsd[2U] & 0xF0000U) >> 16U);
    if (rawCsd[2U] & 0x8000U)
    {
        csd->flags |= kSD_CsdReadBlockPartialFlag;
    }
    if (rawCsd[2U] & 0x4000U)
    {
        csd->flags |= kSD_CsdReadBlockPartialFlag;
    }
    if (rawCsd[2U] & 0x2000U)
    {
        csd->flags |= kSD_CsdReadBlockMisalignFlag;
    }
    if (rawCsd[2U] & 0x1000U)
    {
        csd->flags |= kSD_CsdDsrImplementedFlag;
    }
    switch (csd->csdStructure)
    {
        case 0:
            csd->deviceSize = (uint32_t)((rawCsd[2U] & 0x3FFU) << 2U);
            csd->deviceSize |= (uint32_t)((rawCsd[1U] & 0xC0000000U) >> 30U);
            csd->readCurrentVddMin = (uint8_t)((rawCsd[1U] & 0x38000000U) >> 27U);
            csd->readCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x7000000U) >> 24U);
            csd->writeCurrentVddMin = (uint8_t)((rawCsd[1U] & 0xE00000U) >> 20U);
            csd->writeCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x1C0000U) >> 18U);
            csd->deviceSizeMultiplier = (uint8_t)((rawCsd[1U] & 0x38000U) >> 15U);

            /* Get card total block count and block size. */
            card->blockCount = ((csd->deviceSize + 1U) << (csd->deviceSizeMultiplier + 2U));
            card->blockSize = (1U << (csd->readBlockLength));
            if (card->blockSize != FSL_SDMMC_DEFAULT_BLOCK_SIZE)
            {
                card->blockCount = (card->blockCount * card->blockSize);
                card->blockSize = FSL_SDMMC_DEFAULT_BLOCK_SIZE;
                card->blockCount = (card->blockCount / card->blockSize);
            }
            break;
        case 1:
            card->blockSize = FSL_SDMMC_DEFAULT_BLOCK_SIZE;

            csd->deviceSize = (uint32_t)((rawCsd[2U] & 0x3FU) << 16U);
            csd->deviceSize |= (uint32_t)((rawCsd[1U] & 0xFFFF0000U) >> 16U);
            if (csd->deviceSize >= 0xFFFFU)
            {
                card->flags |= kSD_SupportSdxcFlag;
            }

            card->blockCount = ((csd->deviceSize + 1U) * 1024U);
            break;
        default:
            break;
    }
    if ((uint8_t)((rawCsd[1U] & 0x4000U) >> 14U))
    {
        csd->flags |= kSD_CsdEraseBlockEnabledFlag;
    }
    csd->eraseSectorSize = (uint8_t)((rawCsd[1U] & 0x3F80U) >> 7U);
    csd->writeProtectGroupSize = (uint8_t)(rawCsd[1U] & 0x7FU);
    if ((uint8_t)(rawCsd[0U] & 0x80000000U))
    {
        csd->flags |= kSD_CsdWriteProtectGroupEnabledFlag;
    }
    csd->writeSpeedFactor = (uint8_t)((rawCsd[0U] & 0x1C000000U) >> 26U);
    csd->writeBlockLength = (uint8_t)((rawCsd[0U] & 0x3C00000U) >> 22U);
    if ((uint8_t)((rawCsd[0U] & 0x200000U) >> 21U))
    {
        csd->flags |= kSD_CsdWriteBlockPartialFlag;
    }
    if ((uint8_t)((rawCsd[0U] & 0x8000U) >> 15U))
    {
        csd->flags |= kSD_CsdFileFormatGroupFlag;
    }
    if ((uint8_t)((rawCsd[0U] & 0x4000U) >> 14U))
    {
        csd->flags |= kSD_CsdCopyFlag;
    }
    if ((uint8_t)((rawCsd[0U] & 0x2000U) >> 13U))
    {
        csd->flags |= kSD_CsdPermanentWriteProtectFlag;
    }
    if ((uint8_t)((rawCsd[0U] & 0x1000U) >> 12U))
    {
        csd->flags |= kSD_CsdTemporaryWriteProtectFlag;
    }
    csd->fileFormat = (uint8_t)((rawCsd[0U] & 0xC00U) >> 10U);
}

static status_t SD_SendCsd(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    status_t error = kStatus_Success;

    command.index = kSDMMC_SendCsd;
    command.argument = (card->relativeAddress << 16U);
    command.responseType = kCARD_ResponseTypeR2;

    content.command = &command;
    content.data = NULL;
    error = card->host.transfer(card->host.base, &content);
    if (kStatus_Success == error)
    {
        memcpy(card->rawCsd, command.response, sizeof(card->rawCsd));
        /* The response is from bit 127:8 in R2, corrisponding to command.response[3U]:command.response[0U][31U:8]. */
        SD_DecodeCsd(card, command.response);
    }
    else
    {
        error = kStatus_SDMMC_TransferFailed;
        SDMMC_LOG("\r\nError: send CMD9(get csd) failed with host error %d, response %x", error, command.response[0U]);
    }

    return error;
}

static void SD_DecodeCid(sd_card_t *card, uint32_t *rawCid)
{
    assert(card);
    assert(rawCid);

    sd_cid_t *cid;

    cid = &(card->cid);
    cid->manufacturerID = (uint8_t)((rawCid[3U] & 0xFF000000U) >> 24U);
    cid->applicationID = (uint16_t)((rawCid[3U] & 0xFFFF00U) >> 8U);

    cid->productName[0U] = (uint8_t)((rawCid[3U] & 0xFFU));
    cid->productName[1U] = (uint8_t)((rawCid[2U] & 0xFF000000U) >> 24U);
    cid->productName[2U] = (uint8_t)((rawCid[2U] & 0xFF0000U) >> 16U);
    cid->productName[3U] = (uint8_t)((rawCid[2U] & 0xFF00U) >> 8U);
    cid->productName[4U] = (uint8_t)((rawCid[2U] & 0xFFU));

    cid->productVersion = (uint8_t)((rawCid[1U] & 0xFF000000U) >> 24U);

    cid->productSerialNumber = (uint32_t)((rawCid[1U] & 0xFFFFFFU) << 8U);
    cid->productSerialNumber |= (uint32_t)((rawCid[0U] & 0xFF000000U) >> 24U);

    cid->manufacturerData = (uint16_t)((rawCid[0U] & 0xFFF00U) >> 8U);
}

static status_t SD_AllSendCid(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};

    command.index = kSDMMC_AllSendCid;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR2;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success == card->host.transfer(card->host.base, &content))
    {
        memcpy(card->rawCid, command.response, sizeof(card->rawCid));
        SD_DecodeCid(card, command.response);

        return kStatus_Success;
    }

    return kStatus_SDMMC_TransferFailed;
}

static status_t SD_ApplicationSendOperationCondition(sd_card_t *card, uint32_t argument)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    status_t error = kStatus_Fail;
    uint32_t i = FSL_SDMMC_MAX_VOLTAGE_RETRIES;

    command.index = kSD_ApplicationSendOperationCondition;
    command.argument = argument;
    command.responseType = kCARD_ResponseTypeR3;

    while (i--)
    {
        if (kStatus_Success != SD_SendApplicationCmd(card, 0U))
        {
            continue;
        }

        content.command = &command;
        content.data = NULL;
        error = card->host.transfer(card->host.base, &content);
        if (kStatus_Success != error)
        {
            SDMMC_LOG("\r\nError: send ACMD41 failed with host error %d, response %x", error, command.response[0U]);
            return kStatus_SDMMC_TransferFailed;
        }

        /* Wait until card exit busy state. */
        if (command.response[0U] & SDMMC_MASK(kSD_OcrPowerUpBusyFlag))
        {
            /* high capacity check */
            if (command.response[0U] & SDMMC_MASK(kSD_OcrCardCapacitySupportFlag))
            {
                card->flags |= kSD_SupportHighCapacityFlag;
            }
            /* 1.8V support */
            if (command.response[0U] & SDMMC_MASK(kSD_OcrSwitch18AcceptFlag))
            {
                card->flags |= kSD_SupportVoltage180v;
            }
            card->ocr = command.response[0U];

            return kStatus_Success;
        }
    }

    SDMMC_LOG("\r\nError: send ACMD41 timeout");

    return error;
}

static status_t SD_SendInterfaceCondition(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    uint32_t i = FSL_SDMMC_MAX_CMD_RETRIES;
    status_t error = kStatus_Success;

    command.index = kSD_SendInterfaceCondition;
    command.argument = 0x1AAU;
    command.responseType = kCARD_ResponseTypeR7;

    content.command = &command;
    content.data = NULL;
    do
    {
        error = card->host.transfer(card->host.base, &content);
        if (kStatus_Success != error)
        {
            SDMMC_LOG("\r\nError: send CMD8 failed with host error %d, response %x", error, command.response[0U]);
        }
        else
        {
            if ((command.response[0U] & 0xFFU) != 0xAAU)
            {
                error = kStatus_SDMMC_CardNotSupport;
                SDMMC_LOG("\r\nError: card not support CMD8");
            }
            else
            {
                error = kStatus_Success;
            }
        }
    } while (--i && (error != kStatus_Success));

    return error;
}

static status_t SD_SelectBusTiming(sd_card_t *card)
{
    assert(card);

    status_t error = kStatus_SDMMC_SwitchBusTimingFailed;

    if (card->operationVoltage != kCARD_OperationVoltage180V)
    {
        /* Switch the card to high speed mode */
        if (card->host.capability.flags & kSDMMCHOST_SupportHighSpeed)
        {
            /* group 1, function 1 ->high speed mode*/
            error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionSDR25HighSpeed);
            /* If the result isn't "switching to high speed mode(50MHZ) successfully or card doesn't support high speed
             * mode". Return failed status. */
            if (error == kStatus_Success)
            {
                card->currentTiming = kSD_TimingSDR25HighSpeedMode;
                card->busClock_Hz =
                    SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_50MHZ);
            }
            else if (error == kStatus_SDMMC_NotSupportYet)
            {
                /* if not support high speed, keep the card work at default mode */
                SDMMC_LOG("\r\nNote: High speed mode is not supported by card");
                return kStatus_Success;
            }
        }
        else
        {
            /* if not support high speed, keep the card work at default mode */
            return kStatus_Success;
        }
    }
    /* card is in UHS_I mode */
    else if ((kSDMMCHOST_SupportSDR104 != SDMMCHOST_NOT_SUPPORT) ||
             (kSDMMCHOST_SupportSDR50 != SDMMCHOST_NOT_SUPPORT) || (kSDMMCHOST_SupportDDR50 != SDMMCHOST_NOT_SUPPORT))
    {
        switch (card->currentTiming)
        {
            /* if not select timing mode, sdmmc will handle it automatically*/
            case kSD_TimingSDR12DefaultMode:
            case kSD_TimingSDR104Mode:
                error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionSDR104);
                if (error == kStatus_Success)
                {
                    card->currentTiming = kSD_TimingSDR104Mode;
                    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz,
                                                                 SDMMCHOST_SUPPORT_SDR104_FREQ);
                    break;
                }
                SDMMC_LOG("\r\nNote: SDR104 mode is not supported by card");

            case kSD_TimingDDR50Mode:
                error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionDDR50);
                if (error == kStatus_Success)
                {
                    card->currentTiming = kSD_TimingDDR50Mode;
                    card->busClock_Hz =
                        SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_50MHZ);
                    SDMMCHOST_ENABLE_DDR_MODE(card->host.base, true, 0U);
                    break;
                }
                SDMMC_LOG("\r\nNote: DDR50 mode is not supported by card");

            case kSD_TimingSDR50Mode:
                error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionSDR50);
                if (error == kStatus_Success)
                {
                    card->currentTiming = kSD_TimingSDR50Mode;
                    card->busClock_Hz =
                        SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_100MHZ);
                    break;
                }
                SDMMC_LOG("\r\nNote: SDR50 mode is not supported by card");

            case kSD_TimingSDR25HighSpeedMode:
                error = SD_SelectFunction(card, kSD_GroupTimingMode, kSD_FunctionSDR25HighSpeed);
                if (error == kStatus_Success)
                {
                    card->currentTiming = kSD_TimingSDR25HighSpeedMode;
                    card->busClock_Hz =
                        SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_50MHZ);
                }
                break;

            default:
                SDMMC_LOG("\r\nWarning: unknown timing mode");
                break;
        }
    }
    else
    {
    }

    if (error == kStatus_Success)
    {
        /* SDR50 and SDR104 mode need tuning */
        if ((card->currentTiming == kSD_TimingSDR50Mode) || (card->currentTiming == kSD_TimingSDR104Mode))
        {
            /* config IO strength in IOMUX*/
            if (card->currentTiming == kSD_TimingSDR50Mode)
            {
                SDMMCHOST_CONFIG_SD_IO(CARD_BUS_FREQ_100MHZ1, CARD_BUS_STRENGTH_7);
            }
            else
            {
                SDMMCHOST_CONFIG_SD_IO(CARD_BUS_FREQ_200MHZ, CARD_BUS_STRENGTH_7);
            }
            /* execute tuning */
            if (SD_ExecuteTuning(card) != kStatus_Success)
            {
                SDMMC_LOG("\r\nError: tuning failed for mode %d", card->currentTiming);
                return kStatus_SDMMC_TuningFail;
            }
        }
        else
        {
            /* set default IO strength to 4 to cover card adapter driver strength difference */
            SDMMCHOST_CONFIG_SD_IO(CARD_BUS_FREQ_100MHZ1, CARD_BUS_STRENGTH_4);
        }
    }

    return error;
}

static void SD_DecodeStatus(sd_card_t *card, uint32_t *src)
{
    assert(card);
    assert(src);

    card->stat.busWidth = (uint8_t)((src[0U] & 0xC0000000U) >> 30U);                                       /* 511-510 */
    card->stat.secureMode = (uint8_t)((src[0U] & 0x20000000U) >> 29U);                                     /* 509 */
    card->stat.cardType = (uint16_t)((src[0U] & 0x0000FFFFU));                                             /* 495-480 */
    card->stat.protectedSize = src[1U];                                                                    /* 479-448 */
    card->stat.speedClass = (uint8_t)((src[2U] & 0xFF000000U) >> 24U);                                     /* 447-440 */
    card->stat.performanceMove = (uint8_t)((src[2U] & 0x00FF0000U) >> 16U);                                /* 439-432 */
    card->stat.auSize = (uint8_t)((src[2U] & 0x0000F000U) >> 12U);                                         /* 431-428 */
    card->stat.eraseSize = (uint16_t)(((src[2U] & 0x000000FFU) << 8U) | ((src[3U] & 0xFF000000U) >> 24U)); /* 423-408 */
    card->stat.eraseTimeout = (((uint8_t)((src[3U] & 0x00FF0000U) >> 16U)) & 0xFCU) >> 2U;                 /* 407-402 */
    card->stat.eraseOffset = ((uint8_t)((src[3U] & 0x00FF0000U) >> 16U)) & 0x3U;                           /* 401-400 */
    card->stat.uhsSpeedGrade = (((uint8_t)((src[3U] & 0x0000FF00U) >> 8U)) & 0xF0U) >> 4U;                 /* 399-396 */
    card->stat.uhsAuSize = ((uint8_t)((src[3U] & 0x0000FF00U) >> 8U)) & 0xFU;                              /* 395-392 */
}

status_t SD_ReadStatus(sd_card_t *card)
{
    assert(card);

    uint32_t i = 0U;
    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};
    status_t error = kStatus_Success;

    memset(g_sdmmc, 0U, sizeof(g_sdmmc));

    /* wait card status ready. */
    if (kStatus_Success != SD_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    if (kStatus_Success != SD_SendApplicationCmd(card, card->relativeAddress))
    {
        return kStatus_SDMMC_SendApplicationCommandFailed;
    }

    command.index = kSDMMC_SendStatus;
    command.responseType = kCARD_ResponseTypeR1;

    data.blockSize = 64U;
    data.blockCount = 1U;
    data.rxData = &g_sdmmc[0];

    content.command = &command;
    content.data = &data;
    error = card->host.transfer(card->host.base, &content);
    if ((kStatus_Success != error) || ((command.response[0U]) & SDMMC_R1_ALL_ERROR_FLAG))
    {
        SDMMC_LOG("\r\nError: send ACMD13 failed with host error %d, response %x", error, command.response[0U]);

        return kStatus_SDMMC_TransferFailed;
    }

    switch (card->host.config.endianMode)
    {
        case kSDMMCHOST_EndianModeLittle:
            /* In little endian mode, SD bus byte transferred first is the byte stored in lowest byte position in
            a word which will cause 4 byte's sequence in a word is not consistent with their original sequence from
            card. So the sequence of 4 bytes received in a word should be converted. */
            for (i = 0U; i < 16; i++)
            {
                g_sdmmc[i] = SWAP_WORD_BYTE_SEQUENCE(g_sdmmc[i]);
            }
            break;
        case kSDMMCHOST_EndianModeBig:
            break; /* Doesn't need to switch byte sequence when decodes bytes as big endian sequence. */
        case kSDMMCHOST_EndianModeHalfWordBig:
            for (i = 0U; i < 16; i++)
            {
                g_sdmmc[i] = SWAP_HALF_WROD_BYTE_SEQUENCE(g_sdmmc[i]);
            }
            break;
        default:
            return kStatus_SDMMC_NotSupportYet;
    }

    SD_DecodeStatus(card, g_sdmmc);

    return kStatus_Success;
}

status_t SD_SelectCard(sd_card_t *card, bool isSelected)
{
    assert(card);

    return SDMMC_SelectCard(card->host.base, card->host.transfer, card->relativeAddress, isSelected);
}

status_t SD_SetDriverStrength(sd_card_t *card, sd_driver_strength_t driverStrength)
{
    assert(card);

    status_t error;
    uint32_t strength = driverStrength;

    error = SD_SelectFunction(card, kSD_GroupDriverStrength, strength);

    return error;
}

status_t SD_SetMaxCurrent(sd_card_t *card, sd_max_current_t maxCurrent)
{
    assert(card);

    status_t error;
    uint32_t current = maxCurrent;

    error = SD_SelectFunction(card, kSD_GroupCurrentLimit, current);

    return error;
}

static status_t SD_Read(sd_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount)
{
    assert(card);
    assert(buffer);
    assert(blockCount);
    assert(blockSize == FSL_SDMMC_DEFAULT_BLOCK_SIZE);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};

    if (((card->flags & kSD_SupportHighCapacityFlag) && (blockSize != 512U)) || (blockSize > card->blockSize) ||
        (blockSize > card->host.capability.maxBlockLength) || (blockSize % 4))
    {
        SDMMC_LOG("\r\nError: read with parameter, block size %d is not support", blockSize);
        return kStatus_SDMMC_CardNotSupport;
    }

    /* Wait for the card write process complete because of that card read process and write process use one buffer. */
    if (kStatus_Success != SD_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.rxData = (uint32_t *)buffer;
    data.enableAutoCommand12 = true;

    command.index = (blockCount == 1U) ? kSDMMC_ReadSingleBlock : kSDMMC_ReadMultipleBlock;
    command.argument = startBlock;
    if (!(card->flags & kSD_SupportHighCapacityFlag))
    {
        command.argument *= data.blockSize;
    }
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    content.command = &command;
    content.data = &data;

    return SD_Transfer(card, &content, 1U);
}

static status_t SD_Write(sd_card_t *card,
                         const uint8_t *buffer,
                         uint32_t startBlock,
                         uint32_t blockSize,
                         uint32_t blockCount,
                         uint32_t *writtenBlocks)
{
    assert(card);
    assert(buffer);
    assert(blockCount);
    assert(blockSize == FSL_SDMMC_DEFAULT_BLOCK_SIZE);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};
    status_t error;

    if (((card->flags & kSD_SupportHighCapacityFlag) && (blockSize != 512U)) || (blockSize > card->blockSize) ||
        (blockSize > card->host.capability.maxBlockLength) || (blockSize % 4U))
    {
        SDMMC_LOG("\r\nError: write with parameter, block size %d is not support", blockSize);
        return kStatus_SDMMC_CardNotSupport;
    }

    /* Wait for the card write process complete because of that card read process and write process use one buffer.*/
    if (kStatus_Success != SD_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    /* Wait for the card's buffer to be not full to write to improve the write performance. */
    while ((GET_SDMMCHOST_STATUS(card->host.base) & CARD_DATA0_STATUS_MASK) != CARD_DATA0_NOT_BUSY)
    {
    }

    data.enableAutoCommand12 = true;
    data.blockSize = blockSize;
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    command.index = (blockCount == 1U) ? kSDMMC_WriteSingleBlock : kSDMMC_WriteMultipleBlock;
    command.argument = startBlock;
    if (!(card->flags & kSD_SupportHighCapacityFlag))
    {
        command.argument *= data.blockSize;
    }

    *writtenBlocks = blockCount;
    data.blockCount = blockCount;
    data.txData = (const uint32_t *)(buffer);

    content.command = &command;
    content.data = &data;

    error = SD_Transfer(card, &content, 0U);
    if (error != kStatus_Success)
    {
        /* check the successfully written block */
        if ((SD_SendWriteSuccessBlocks(card, writtenBlocks) == kStatus_Success))
        {
            if (*writtenBlocks)
            {
                /* written success, but not all the blocks are written */
                error = kStatus_Success;
            }
        }
        SDMMC_LOG("\r\nWarning: write failed with block count %d, successed %d", blockCount, *writtenBlocks);
    }

    return error;
}

static status_t SD_Erase(sd_card_t *card, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(blockCount);

    uint32_t eraseBlockStart;
    uint32_t eraseBlockEnd;
    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    status_t error = kStatus_Success;

    /* Wait for the card write process complete because of that card read process and write process use one buffer.*/
    if (kStatus_Success != SD_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }
    /* Wait for the card's buffer to be not full to write to improve the write performance. */
    while ((GET_SDMMCHOST_STATUS(card->host.base) & CARD_DATA0_STATUS_MASK) != CARD_DATA0_NOT_BUSY)
    {
    }

    eraseBlockStart = startBlock;
    eraseBlockEnd = eraseBlockStart + blockCount - 1U;
    if (!(card->flags & kSD_SupportHighCapacityFlag))
    {
        eraseBlockStart = eraseBlockStart * FSL_SDMMC_DEFAULT_BLOCK_SIZE;
        eraseBlockEnd = eraseBlockEnd * FSL_SDMMC_DEFAULT_BLOCK_SIZE;
    }

    /* Send ERASE_WRITE_BLOCK_START command to set the start block number to erase. */
    command.index = kSD_EraseWriteBlockStart;
    command.argument = eraseBlockStart;
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    content.command = &command;
    content.data = NULL;
    error = SD_Transfer(card, &content, 1U);
    if (kStatus_Success != error)
    {
        SDMMC_LOG("\r\nError: send CMD32(erase start) failed with host error %d, response %x", error,
                  command.response[0U]);
        return kStatus_SDMMC_TransferFailed;
    }

    /* Send ERASE_WRITE_BLOCK_END command to set the end block number to erase. */
    command.index = kSD_EraseWriteBlockEnd;
    command.argument = eraseBlockEnd;

    content.command = &command;
    content.data = NULL;
    error = SD_Transfer(card, &content, 0U);
    if (kStatus_Success != error)
    {
        SDMMC_LOG("\r\nError: send CMD33(erase end) failed with host error %d, response %x", error,
                  command.response[0U]);
        return kStatus_SDMMC_TransferFailed;
    }

    /* Send ERASE command to start erase process. */
    command.index = kSDMMC_Erase;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR1b;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    content.command = &command;
    content.data = NULL;
    error = SD_Transfer(card, &content, 0U);
    if (kStatus_Success != error)
    {
        SDMMC_LOG("\r\nError: send CMD38(erase) failed with host error %d, response %x", error, command.response[0U]);
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

bool SD_CheckReadOnly(sd_card_t *card)
{
    assert(card);

    return ((card->csd.flags & kSD_CsdPermanentWriteProtectFlag) ||
            (card->csd.flags & kSD_CsdTemporaryWriteProtectFlag));
}

status_t SD_ReadBlocks(sd_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(buffer);
    assert(blockCount);
    assert((blockCount + startBlock) <= card->blockCount);

    uint32_t blockCountOneTime;
    uint32_t blockLeft;
    uint32_t blockDone = 0U;
    uint8_t *nextBuffer = buffer;
    bool dataAddrAlign = true;

    blockLeft = blockCount;

    while (blockLeft)
    {
        nextBuffer = (buffer + blockDone * FSL_SDMMC_DEFAULT_BLOCK_SIZE);
        if (!card->noInteralAlign && (!dataAddrAlign || (((uint32_t)nextBuffer) & (sizeof(uint32_t) - 1U))))
        {
            blockLeft--;
            blockCountOneTime = 1U;
            memset(g_sdmmc, 0U, FSL_SDMMC_DEFAULT_BLOCK_SIZE);
            dataAddrAlign = false;
        }
        else
        {
            if (blockLeft > card->host.capability.maxBlockCount)
            {
                blockLeft = (blockLeft - card->host.capability.maxBlockCount);
                blockCountOneTime = card->host.capability.maxBlockCount;
            }
            else
            {
                blockCountOneTime = blockLeft;
                blockLeft = 0U;
            }
        }

        if (kStatus_Success != SD_Read(card, dataAddrAlign ? nextBuffer : (uint8_t *)g_sdmmc, (startBlock + blockDone),
                                       FSL_SDMMC_DEFAULT_BLOCK_SIZE, blockCountOneTime))
        {
            return kStatus_SDMMC_TransferFailed;
        }

        blockDone += blockCountOneTime;

        if (!card->noInteralAlign && (!dataAddrAlign))
        {
            memcpy(nextBuffer, (uint8_t *)&g_sdmmc, FSL_SDMMC_DEFAULT_BLOCK_SIZE);
        }
    }

    return kStatus_Success;
}

status_t SD_WriteBlocks(sd_card_t *card, const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(buffer);
    assert(blockCount);
    assert((blockCount + startBlock) <= card->blockCount);

    uint32_t blockCountOneTime = 0U; /* The block count can be wrote in one time sending WRITE_BLOCKS command. */
    uint32_t blockWrittenOneTime = 0U;
    uint32_t blockLeft = 0U; /* Left block count to be wrote. */
    const uint8_t *nextBuffer;
    bool dataAddrAlign = true;

    blockLeft = blockCount;
    while (blockLeft)
    {
        nextBuffer = (buffer + (blockCount - blockLeft) * FSL_SDMMC_DEFAULT_BLOCK_SIZE);
        if (!card->noInteralAlign && (!dataAddrAlign || (((uint32_t)nextBuffer) & (sizeof(uint32_t) - 1U))))
        {
            blockCountOneTime = 1U;
            memcpy((uint8_t *)&g_sdmmc, nextBuffer, FSL_SDMMC_DEFAULT_BLOCK_SIZE);
            dataAddrAlign = false;
        }
        else
        {
            if (blockLeft > card->host.capability.maxBlockCount)
            {
                blockCountOneTime = card->host.capability.maxBlockCount;
            }
            else
            {
                blockCountOneTime = blockLeft;
            }
        }

        if (kStatus_Success != SD_Write(card, dataAddrAlign ? nextBuffer : (uint8_t *)g_sdmmc,
                                        (startBlock + blockCount - blockLeft), FSL_SDMMC_DEFAULT_BLOCK_SIZE,
                                        blockCountOneTime, &blockWrittenOneTime))
        {
            return kStatus_SDMMC_TransferFailed;
        }

        blockLeft -= blockWrittenOneTime;

        if ((!card->noInteralAlign) && !dataAddrAlign)
        {
            memset(g_sdmmc, 0U, FSL_SDMMC_DEFAULT_BLOCK_SIZE);
        }
    }

    return kStatus_Success;
}

status_t SD_EraseBlocks(sd_card_t *card, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(blockCount);
    assert((blockCount + startBlock) <= card->blockCount);

    uint32_t blockCountOneTime; /* The block count can be erased in one time sending ERASE_BLOCKS command. */
    uint32_t blockDone = 0U;    /* The block count has been erased. */
    uint32_t blockLeft;         /* Left block count to be erase. */
    status_t error;
    uint32_t onetimeMaxEraseBlocks = 0U;

    /* sdsc card erasable sector is determined by CSD register */
    if (card->csd.csdStructure == 0U)
    {
        onetimeMaxEraseBlocks = card->csd.eraseSectorSize + 1U;
    }
    else
    {
        /* limit one time maximum erase size to 1 AU */
        if (card->stat.auSize >= SD_AU_START_VALUE)
        {
            onetimeMaxEraseBlocks = s_sdAuSizeMap[card->stat.auSize] / FSL_SDMMC_DEFAULT_BLOCK_SIZE;
        }
    }

    if (onetimeMaxEraseBlocks == 0U)
    {
        SDMMC_LOG(
            "Warning: AU size in sd descriptor is not set properly, please check if SD_ReadStatus is called before\
          SD_EraseBlocks");
        return kStatus_SDMMC_AuSizeNotSetProperly;
    }

    blockLeft = blockCount;
    while (blockLeft)
    {
        if (blockLeft > onetimeMaxEraseBlocks)
        {
            blockCountOneTime = onetimeMaxEraseBlocks;
            blockLeft = blockLeft - blockCountOneTime;
        }
        else
        {
            blockCountOneTime = blockLeft;
            blockLeft = 0U;
        }

        error = SD_Erase(card, (startBlock + blockDone), blockCountOneTime);
        if (error != kStatus_Success)
        {
            return error;
        }

        blockDone += blockCountOneTime;
    }

    return kStatus_Success;
}

status_t SD_ProbeBusVoltage(sd_card_t *card)
{
    assert(card);

    uint32_t applicationCommand41Argument = 0U;
    status_t error = kStatus_Success;

    /* 3.3V voltage should be supported as default */
    applicationCommand41Argument |=
        SDMMC_MASK(kSD_OcrVdd29_30Flag) | SDMMC_MASK(kSD_OcrVdd32_33Flag) | SDMMC_MASK(kSD_OcrVdd33_34Flag);
    /* make sure card signal line voltage is 3.3v before initalization */
    if ((card->usrParam.cardVoltage != NULL) && (card->usrParam.cardVoltage->cardSignalLine3V3 != NULL))
    {
        card->usrParam.cardVoltage->cardSignalLine3V3();
    }
    else
    {
        SDMMCHOST_SWITCH_VOLTAGE180V(card->host.base, false);
    }
    card->operationVoltage = kCARD_OperationVoltage330V;

    /* allow user select the work voltage, if not select, sdmmc will handle it automatically */
    if (kSDMMCHOST_SupportV180 != SDMMCHOST_NOT_SUPPORT)
    {
        applicationCommand41Argument |= SDMMC_MASK(kSD_OcrSwitch18RequestFlag);
    }

    do
    {
        /* card go idle */
        if (kStatus_Success != SD_GoIdle(card))
        {
            error = kStatus_SDMMC_GoIdleFailed;
            break;
        }

        /* Check card's supported interface condition. */
        if (kStatus_Success == SD_SendInterfaceCondition(card))
        {
            /* SDHC or SDXC card */
            applicationCommand41Argument |= SDMMC_MASK(kSD_OcrHostCapacitySupportFlag);
            card->flags |= kSD_SupportSdhcFlag;
        }
        else
        {
            /* SDSC card */
            if (kStatus_Success != SD_GoIdle(card))
            {
                error = kStatus_SDMMC_GoIdleFailed;
                break;
            }
        }
        /* Set card interface condition according to SDHC capability and card's supported interface condition. */
        if (kStatus_Success != SD_ApplicationSendOperationCondition(card, applicationCommand41Argument))
        {
            error = kStatus_SDMMC_HandShakeOperationConditionFailed;
            break;
        }

        /* check if card support 1.8V */
        if ((card->flags & kSD_SupportVoltage180v))
        {
            error = SD_SwitchVoltage(card);
            if (kStatus_SDMMC_SwitchVoltageFail == error)
            {
                break;
            }

            if (error == kStatus_SDMMC_SwitchVoltage18VFail33VSuccess)
            {
                applicationCommand41Argument &= ~SDMMC_MASK(kSD_OcrSwitch18RequestFlag);
                card->flags &= ~kSD_SupportVoltage180v;
                continue;
            }
            else
            {
                card->operationVoltage = kCARD_OperationVoltage180V;
                break;
            }
        }

        break;
    } while (1U);

    return error;
}

status_t SD_CardInit(sd_card_t *card)
{
    assert(card);
    assert(card->isHostReady == true);

    /* reset variables */
    card->flags = 0U;
    /* set DATA bus width */
    SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH1BIT);
    /*set card freq to 400KHZ*/
    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SDMMC_CLOCK_400KHZ);
    /* send card active */
    SDMMCHOST_SEND_CARD_ACTIVE(card->host.base, 100U);
    /* Get host capability. */
    GET_SDMMCHOST_CAPABILITY(card->host.base, &(card->host.capability));

    /* probe bus voltage*/
    if (SD_ProbeBusVoltage(card) == kStatus_SDMMC_SwitchVoltageFail)
    {
        return kStatus_SDMMC_SwitchVoltageFail;
    }

    /* Initialize card if the card is SD card. */
    if (kStatus_Success != SD_AllSendCid(card))
    {
        return kStatus_SDMMC_AllSendCidFailed;
    }
    if (kStatus_Success != SD_SendRca(card))
    {
        return kStatus_SDMMC_SendRelativeAddressFailed;
    }
    if (kStatus_Success != SD_SendCsd(card))
    {
        return kStatus_SDMMC_SendCsdFailed;
    }
    if (kStatus_Success != SD_SelectCard(card, true))
    {
        return kStatus_SDMMC_SelectCardFailed;
    }

    /* Set to max frequency in non-high speed mode. */
    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SD_CLOCK_25MHZ);

    if (kStatus_Success != SD_SendScr(card))
    {
        return kStatus_SDMMC_SendScrFailed;
    }
    /* Set to 4-bit data bus mode. */
    if (((card->host.capability.flags) & kSDMMCHOST_Support4BitBusWidth) && (card->flags & kSD_Support4BitWidthFlag))
    {
        if (kStatus_Success != SD_SetDataBusWidth(card, kSD_DataBusWidth4Bit))
        {
            return kStatus_SDMMC_SetDataBusWidthFailed;
        }
        SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH4BIT);
    }

    /* set block size */
    if (SD_SetBlockSize(card, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
    {
        return kStatus_SDMMC_SetCardBlockSizeFailed;
    }

    /* select bus timing */
    if (kStatus_Success != SD_SelectBusTiming(card))
    {
        return kStatus_SDMMC_SwitchBusTimingFailed;
    }

    /* try to get card current status */
    SD_ReadStatus(card);

    return kStatus_Success;
}

void SD_CardDeinit(sd_card_t *card)
{
    assert(card);

    SD_SelectCard(card, false);
}

status_t SD_HostInit(sd_card_t *card)
{
    assert(card);

    if ((!card->isHostReady) && SDMMCHOST_Init(&(card->host), (void *)(&(card->usrParam))) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    /* set the host status flag, after the card re-plug in, don't need init host again */
    card->isHostReady = true;

    return kStatus_Success;
}

void SD_HostDeinit(sd_card_t *card)
{
    assert(card);

    SDMMCHOST_Deinit(&(card->host));
    /* should re-init host */
    card->isHostReady = false;
}

void SD_HostReset(SDMMCHOST_CONFIG *host)
{
    SDMMCHOST_Reset(host->base);
}

void SD_PowerOnCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    SDMMCHOST_PowerOnCard(base, pwr);
}

void SD_PowerOffCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    SDMMCHOST_PowerOffCard(base, pwr);
}

status_t SD_WaitCardDetectStatus(SDMMCHOST_TYPE *hostBase, const sdmmchost_detect_card_t *cd, bool waitCardStatus)
{
    return SDMMCHOST_WaitCardDetectStatus(hostBase, cd, waitCardStatus);
}

bool SD_IsCardPresent(sd_card_t *card)
{
    return SDMMCHOST_IsCardPresent();
}

status_t SD_Init(sd_card_t *card)
{
    assert(card);

    if (!card->isHostReady)
    {
        if (SD_HostInit(card) != kStatus_Success)
        {
            return kStatus_SDMMC_HostNotReady;
        }
    }
    else
    {
        SD_HostReset(&(card->host));
    }
    SD_PowerOffCard(card->host.base, card->usrParam.pwr);

    if (SD_WaitCardDetectStatus(card->host.base, card->usrParam.cd, true) != kStatus_Success)
    {
        return kStatus_SDMMC_CardDetectFailed;
    }
    SD_PowerOnCard(card->host.base, card->usrParam.pwr);

    return SD_CardInit(card);
}

void SD_Deinit(sd_card_t *card)
{
    /* card deinitialize */
    SD_CardDeinit(card);
    /* host deinitialize */
    SD_HostDeinit(card);
}
