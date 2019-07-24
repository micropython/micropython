/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include "fsl_mmc.h"

/*******************************************************************************
 * Definitons
 ******************************************************************************/
/*! @brief The divide value used to avoid float point calculation when calculate max speed in normal mode. */
#define DIVIDER_IN_TRANSFER_SPEED (10U)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Send SELECT_CARD command to set the card enter or exit transfer state.
 *
 * @param card Card descriptor.
 * @param isSelected True to enter transfer state.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline MMC_SelectCard(mmc_card_t *card, bool isSelected);

/*!
 * @brief Wait write process complete.
 *
 * @param card Card descriptor.
 * @retval kStatus_Timeout Operation timeout.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_WaitWriteComplete(mmc_card_t *card);

/*!
 * @brief Send SET_BLOCK_COUNT command.
 *
 * @param card Card descriptor.
 * @param blockCount Block count.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline MMC_SetBlockCount(mmc_card_t *card, uint32_t blockCount);

/*!
 * @brief Send GO_IDLE command to reset all cards to idle state
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline MMC_GoIdle(mmc_card_t *card);

/*!
 * @brief Send STOP_TRANSMISSION command to card to stop ongoing data transferring.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_StopTransmission(mmc_card_t *card);

/*!
 * @brief Send SET_BLOCK_SIZE command to set the block length in bytes for MMC cards.
 *
 * @param card Card descriptor.
 * @param blockSize Block size.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t inline MMC_SetBlockSize(mmc_card_t *card, uint32_t blockSize);

/*!
 * @brief switch voltage.
 *
 * @param card Card descriptor.
 * @param opcode use to send operation condition
 * @retval kStatus_SDMMC_HostNotSupport Host doesn't support the voltage window to access the card.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SwitchVoltage(mmc_card_t *card, uint32_t *opCode);

/*!
 * @brief Send SEND_OPERATION_CONDITION command to validate if the card support host's voltage window
 *
 * @param card Card descriptor.
 * @param arg Command argument.
 * @retval kStatus_SDMMC_TransferFailed Transfers failed.
 * @retval kStatus_Timeout Operation timeout.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SendOperationCondition(mmc_card_t *card, uint32_t arg);

/*!
 * @brief Send SET_RCA command to set the relative address of the card.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SetRelativeAddress(mmc_card_t *card);

/*!
 * @brief Decode CSD register content.
 *
 * @param card Card descriptor.
 * @param rawCsd raw CSD register content.
 */
static void MMC_DecodeCsd(mmc_card_t *card, uint32_t *rawCsd);

/*!
 * @brief Set the card to max transfer speed in non-high speed mode.
 *
 * @param card Card descriptor.
 */
static void MMC_SetMaxFrequency(mmc_card_t *card);

/*!
 * @brief Set erase unit size of the card
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_ConfigureExtendedCsdFailed Configure Extended CSD failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SetMaxEraseUnitSize(mmc_card_t *card);

/*!
 * @brief Send SWITCH command to set the specific byte in Extended CSD.
 *
 * Example:
   @code
   mmc_extended_csd_config_t config;
   config.accessMode = kMMC_ExtendedCsdAccessModeSetBits;
   config.ByteIndex = 1U;
   config.ByteValue = 0x033U;
   config.commandSet = kMMC_CommandSetStandard;
   MMC_SetExtendedCsdConfig(card, &config);
   @endcode
 *
 * @param card Card descriptor.
 * @param config Configuration for Extended CSD.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_WaitWriteCompleteFailed Wait write complete failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SetExtendedCsdConfig(mmc_card_t *card, const mmc_extended_csd_config_t *config);

/*!
 * @brief Decode the Extended CSD register
 *
 * @param card Card descriptor.
 * @param rawExtendedCsd Raw extended CSD register content.
 */
static void MMC_DecodeExtendedCsd(mmc_card_t *card, uint32_t *rawExtendedCsd);

/*!
 * @brief Send SEND_EXTENDED_CSD command to get the content of the Extended CSD register
 * Allow read the special byte index value if targetAddr is not NULL
 * @param card Card descriptor.
 * @param targetAddr Pointer to store the target byte value.
 * @param byteIndex Target byte index.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SendExtendedCsd(mmc_card_t *card, uint8_t *targetAddr, uint32_t byteIndex);

/*!
 * @brief Set the power class of the card at specific bus width and host intended voltage window.
 *
 * @param card Card descriptor.
 * @return The power class switch status.
 */
static status_t MMC_SetPowerClass(mmc_card_t *card);

/*!
 * @brief Send test pattern to get the functional pin in the MMC bus
 *
 * @param card Card descriptor.
 * @param blockSize Test pattern block size.
 * @param pattern Test pattern data buffer.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SendTestPattern(mmc_card_t *card, uint32_t blockSize, uint32_t *pattern);

/*!
 * @brief Receive test pattern reversed by the card.
 *
 * @param card Card descriptor.
 * @param blockSize Test pattern block size.
 * @param pattern Test pattern data buffer.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_ReceiveTestPattern(mmc_card_t *card, uint32_t blockSize, uint32_t *pattern);

/*!
 * @brief Bus test procedure to get the functional data pin in the bus
 *
 * @param card Card descriptor.
 * @param width Data bus width.
 * @retval kStatus_SDMMC_SendTestPatternFailed Send test pattern failed.
 * @retval kStatus_SDMMC_ReceiveTestPatternFailed Receive test pattern failed.
 * @retval kStatus_Fail Test failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_TestDataBusWidth(mmc_card_t *card, mmc_data_bus_width_t width);

/*!
 * @brief Send SET_BUS_WIDTH command to set the bus width.
 *
 * @param card Card descriptor.
 * @param width Data bus width.
 * @retval kStatus_SDMMC_ConfigureExtendedCsdFailed Configure extended CSD failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SetDataBusWidth(mmc_card_t *card, mmc_data_bus_width_t width);

/*!
 * @brief Set max the bus width automatically
 *
 * @param card Card descriptor.
 * @param targetTiming switch target timing
 * @retval kStatus_SDMMC_SetDataBusWidthFailed switch fail.
 * @retval kStatus_Success switch success.
 */
static status_t MMC_SetMaxDataBusWidth(mmc_card_t *card, mmc_high_speed_timing_t targetTiming);

/*!
 * @brief Switch the card to high speed mode
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_ConfigureExtendedCsdFailed Configure extended CSD failed.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support high speed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SelectBusTiming(mmc_card_t *card);

/*!
 * @brief select card HS_TIMING value and card driver strength
 *
 * @param card Card descriptor.
 * @param timing Timing interface value.
 * @param driverStrength driver strength value.
 * @retval kStatus_Success switch success.
 * @retval kStatus_SDMMC_ConfigureExtendedCsdFailed , config extend csd register fail.
 */
static status_t MMC_SwitchHSTiming(mmc_card_t *card, uint8_t timing, uint8_t driverStrength);

/*!
 * @brief switch to HS400 mode.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_ConfigureExtendedCsdFailed Configure extended CSD failed.
 * @retval kStatus_SDMMC_SwitchBusTimingFailed switch bus timing fail.
 * @retval kStatus_SDMMC_SetDataBusWidthFailed switch bus width fail.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SwitchToHS400(mmc_card_t *card);

/*!
 * @brief switch to HS200 mode.
 *
 * @param card Card descriptor.
 * @param freq Target frequency.
 * @retval kStatus_SDMMC_ConfigureExtendedCsdFailed Configure extended CSD failed.
 * @retval kStatus_SDMMC_TuningFail tuning fail.
 * @retval kStatus_SDMMC_SetDataBusWidthFailed switch bus width fail.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SwitchToHS200(mmc_card_t *card, uint32_t freq);

/*!
 * @brief switch to HS400 mode.
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_ConfigureExtendedCsdFailed Configure extended CSD failed.
 * @retval kStatus_SDMMC_SetDataBusWidthFailed switch bus width fail.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SwitchToHighSpeed(mmc_card_t *card);

/*!
 * @brief Decode CID register
 *
 * @param card Card descriptor.
 * @param rawCid Raw CID register content.
 */
static void MMC_DecodeCid(mmc_card_t *card, uint32_t *rawCid);

/*!
 * @brief Send ALL_SEND_CID command
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_AllSendCid(mmc_card_t *card);

/*!
 * @brief Send SEND_CSD command to get CSD from card
 *
 * @param card Card descriptor.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_SendCsd(mmc_card_t *card);

/*!
 * @brief Check if the block range accessed is within current partition.
 *
 * @param card Card descriptor.
 * @param startBlock Start block to access.
 * @param blockCount Block count to access.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_CheckBlockRange(mmc_card_t *card, uint32_t startBlock, uint32_t blockCount);

/*!
 * @brief Check if the erase group range accessed is within current partition.
 *
 * @param card Card descriptor.
 * @param startGroup Start group to access.
 * @param endGroup End group to access.
 * @retval kStatus_InvalidArgument Invalid argument.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_CheckEraseGroupRange(mmc_card_t *card, uint32_t startGroup, uint32_t endGroup);

/*!
 * @brief MMC excute tuning function.
 *
 * @param card Card descriptor.
 * @retval kStatus_Success Operate successfully.
 * @retval kStatus_SDMMC_TuningFail tuning fail.
 * @retval kStatus_SDMMC_TransferFailed transfer fail
 */
static status_t inline MMC_ExecuteTuning(mmc_card_t *card);

/*!
 * @brief Read data from specific MMC card
 *
 * @param card Card descriptor.
 * @param buffer Buffer to save received data.
 * @param startBlock Start block to read.
 * @param blockSize Block size.
 * @param blockCount Block count to read.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_SDMMC_WaitWriteCompleteFailed Wait write complete failed.
 * @retval kStatus_SDMMC_SetBlockCountFailed Set block count failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_StopTransmissionFailed Stop transmission failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_Read(
    mmc_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount);

/*!
 * @brief Write data from specific MMC card
 *
 * @param card Card descriptor.
 * @param buffer Buffer to hold the data to write.
 * @param startBlock Start block to write.
 * @param blockSize Block size.
 * @param blockCount Block count to write.
 * @retval kStatus_SDMMC_CardNotSupport Card doesn't support.
 * @retval kStatus_SDMMC_SetBlockCountFailed Set block count failed.
 * @retval kStatus_SDMMC_TransferFailed Transfer failed.
 * @retval kStatus_SDMMC_StopTransmissionFailed Stop transmission failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t MMC_Write(
    mmc_card_t *card, const uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount);

/*!
 * @brief card transfer function wrapper
 * This function is used to do tuning before transfer if the cmd won't casue re-tuning
 * request, then you can call host transfer function directly
 * @param card Card descriptor.
 * @param content Transfer content.
 * @param retry Retry times.
 * @retval kStatus_SDMMC_TransferFailed transfer fail
 * @retval kStatus_SDMMC_TuningFail tuning fail
 * @retval kStatus_Success transfer success
 */
static status_t MMC_Transfer(mmc_card_t *card, SDMMCHOST_TRANSFER *content, uint32_t retry);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Frequency unit defined in TRANSFER SPEED field in CSD */
static const uint32_t g_transerSpeedFrequencyUnit[] = {100000U, 1000000U, 10000000U, 100000000U};
/* The multiplying value defined in TRANSFER SPEED field in CSD */
static const uint32_t g_transerSpeedMultiplierFactor[] = {0U,  10U, 12U, 13U, 15U, 20U, 26U, 30U,
                                                          35U, 40U, 45U, 52U, 55U, 60U, 70U, 80U};
/* g_sdmmc statement */
extern uint32_t g_sdmmc[SDK_SIZEALIGN(SDMMC_GLOBAL_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)];

/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t inline MMC_SelectCard(mmc_card_t *card, bool isSelected)
{
    assert(card);

    return SDMMC_SelectCard(card->host.base, card->host.transfer, card->relativeAddress, isSelected);
}

static status_t inline MMC_SetBlockCount(mmc_card_t *card, uint32_t blockCount)
{
    assert(card);

    return SDMMC_SetBlockCount(card->host.base, card->host.transfer, blockCount);
}

static status_t inline MMC_GoIdle(mmc_card_t *card)
{
    assert(card);

    return SDMMC_GoIdle(card->host.base, card->host.transfer);
}

static status_t inline MMC_SetBlockSize(mmc_card_t *card, uint32_t blockSize)
{
    assert(card);

    return SDMMC_SetBlockSize(card->host.base, card->host.transfer, blockSize);
}

static status_t MMC_ExecuteTuning(mmc_card_t *card)
{
    assert(card);

    uint32_t blockSize = 0U;

    if (card->busWidth == kMMC_DataBusWidth4bit)
    {
        blockSize = 64U;
    }
    else if (card->busWidth == kMMC_DataBusWidth8bit)
    {
        blockSize = 128U;
    }
    else
    {
        /* do not need tuning in this situation */
        return kStatus_Success;
    }

    return SDMMC_ExecuteTuning(card->host.base, card->host.transfer, kMMC_SendTuningBlock, blockSize);
}

static status_t MMC_Transfer(mmc_card_t *card, SDMMCHOST_TRANSFER *content, uint32_t retry)
{
    assert(card->host.transfer);
    assert(content);
    status_t error;

    do
    {
        error = card->host.transfer(card->host.base, content);
#if SDMMC_ENABLE_SOFTWARE_TUNING
        if (((error == SDMMCHOST_RETUNING_REQUEST) || (error == SDMMCHOST_TUNING_ERROR)) &&
            ((card->busTiming == kMMC_HighSpeed200Timing) || (card->busTiming == kMMC_HighSpeed400Timing)))
        {
            /* tuning error need reset tuning circuit */
            if (error == SDMMCHOST_TUNING_ERROR)
            {
                SDMMCHOST_RESET_TUNING(card->host.base, 100U);
            }
            /* execute re-tuning */
            if (MMC_ExecuteTuning(card) != kStatus_Success)
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
            error = kStatus_SDMMC_TransferFailed;
        }
        else
        {
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

static status_t MMC_WaitWriteComplete(mmc_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};

    command.index = kSDMMC_SendStatus;
    command.argument = card->relativeAddress << 16U;
    command.responseType = kCARD_ResponseTypeR1;

    do
    {
        content.command = &command;
        content.data = 0U;
        if (kStatus_Success != MMC_Transfer(card, &content, 2U))
        {
            return kStatus_SDMMC_TransferFailed;
        }

        /* check the response error */
        if ((command.response[0U] & (SDMMC_R1_ALL_ERROR_FLAG | SDMMC_MASK(kSDMMC_R1SwitchErrorFlag))))
        {
            return kStatus_SDMMC_WaitWriteCompleteFailed;
        }

        if ((command.response[0U] & SDMMC_MASK(kSDMMC_R1ReadyForDataFlag)) &&
            (SDMMC_R1_CURRENT_STATE(command.response[0U]) != kSDMMC_R1StateProgram))
        {
            break;
        }
    } while (true);

    return kStatus_Success;
}

static status_t MMC_StopTransmission(mmc_card_t *card)
{
    assert(card);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};

    command.index = kSDMMC_StopTransmission;
    command.argument = 0U;
    command.type = kCARD_CommandTypeAbort;
    command.responseType = kCARD_ResponseTypeR1b;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    content.command = &command;
    content.data = 0U;
    if (kStatus_Success != MMC_Transfer(card, &content, 2U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

static status_t MMC_SwitchVoltage(mmc_card_t *card, uint32_t *opCode)
{
    mmc_voltage_window_t tempVoltage = kMMC_VoltageWindowNone;
    /* Get host's voltage window. */
    if (((kSDMMCHOST_SupportV330 != SDMMCHOST_NOT_SUPPORT) || (kSDMMCHOST_SupportV300 != SDMMCHOST_NOT_SUPPORT)) &&
        (card->ocr & MMC_OCR_V270TO360_MASK) && ((card->hostVoltageWindowVCC == kMMC_VoltageWindowNone) ||
                                                 (card->hostVoltageWindowVCC == kMMC_VoltageWindows270to360)))
    {
        /* Save host intended voltage range */
        tempVoltage = kMMC_VoltageWindows270to360;
        /* set the opcode */
        *opCode = MMC_OCR_V270TO360_MASK;
        /* power off the card first */
        SDMMCHOST_ENABLE_MMC_POWER(false);
        /* power off time */
        SDMMCHOST_Delay(1U);
        /*switch voltage to 3.3V*/
        SDMMCHOST_SWITCH_VCC_TO_330V();
        /* repower the card */
        SDMMCHOST_ENABLE_MMC_POWER(true);
        /* meet emmc spec, wait 1ms and 74 clocks */
        SDMMCHOST_Delay(2U);
    }

    if ((kSDMMCHOST_SupportV180 != SDMMCHOST_NOT_SUPPORT) && (card->ocr & MMC_OCR_V170TO195_MASK) &&
        ((card->hostVoltageWindowVCC == kMMC_VoltageWindowNone) ||
         (card->hostVoltageWindowVCC == kMMC_VoltageWindow170to195)))
    {
        /* Save host intended voltage range */
        tempVoltage = kMMC_VoltageWindow170to195;
        /* set the opcode */
        *opCode = MMC_OCR_V170TO195_MASK;
        /* power off the card first */
        SDMMCHOST_ENABLE_MMC_POWER(false);
        /* power off time */
        SDMMCHOST_Delay(1U);
        /* switch voltage to 1.8V */
        SDMMCHOST_SWITCH_VCC_TO_180V();
        /* repower the card */
        SDMMCHOST_ENABLE_MMC_POWER(true);
        /* meet emmc spec, wait 1ms and 74 clocks */
        SDMMCHOST_Delay(2U);
    }

    card->hostVoltageWindowVCC = tempVoltage;

    return kStatus_Success;
}

static status_t MMC_SendOperationCondition(mmc_card_t *card, uint32_t arg)
{
    assert(card);
    assert(card->host.transfer);

    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_TRANSFER content = {0};
    status_t error;
    uint32_t i = FSL_SDMMC_MAX_VOLTAGE_RETRIES;

    /* Send CMD1 with the intended voltage range in the argument(either 0x00FF8000 or 0x00000080) */
    command.index = kMMC_SendOperationCondition;
    command.argument = arg;
    command.responseType = kCARD_ResponseTypeR3;

    content.command = &command;
    content.data = NULL;
    do
    {
        if (kStatus_Success != card->host.transfer(card->host.base, &content))
        {
            return kStatus_SDMMC_TransferFailed;
        }

        /* record OCR register */
        card->ocr = command.response[0U];

        if ((arg == 0U) && (command.response[0U] != 0U))
        {
            error = kStatus_Success;
        }
        /* Repeat CMD1 until the busy bit is cleared. */
        else if (!(command.response[0U] & MMC_OCR_BUSY_MASK))
        {
            error = kStatus_Timeout;
        }
        else
        {
            error = kStatus_Success;
            if (((card->ocr & MMC_OCR_ACCESS_MODE_MASK) >> MMC_OCR_ACCESS_MODE_SHIFT) == kMMC_AccessModeSector)
            {
                card->flags |= kMMC_SupportHighCapacityFlag;
            }
        }
    } while ((i--) && (error != kStatus_Success));

    return error;
}

static status_t MMC_SetRelativeAddress(mmc_card_t *card)
{
    assert(card);
    assert(card->host.transfer);

    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_TRANSFER content = {0};

    /* Send CMD3 with a chosen relative address, with value greater than 1 */
    command.index = kMMC_SetRelativeAddress;
    command.argument = (MMC_DEFAULT_RELATIVE_ADDRESS << 16U);
    command.responseType = kCARD_ResponseTypeR1;

    content.command = &command;
    content.data = NULL;
    if ((kStatus_Success == card->host.transfer(card->host.base, &content)) ||
        (!((command.response[0U]) & SDMMC_R1_ALL_ERROR_FLAG)))
    {
        card->relativeAddress = MMC_DEFAULT_RELATIVE_ADDRESS;
        return kStatus_Success;
    }

    return kStatus_SDMMC_TransferFailed;
}

static void MMC_DecodeCsd(mmc_card_t *card, uint32_t *rawCsd)
{
    assert(card);
    assert(rawCsd);

    mmc_csd_t *csd;
    uint32_t multiplier;

    csd = &(card->csd);
    csd->csdStructureVersion = (uint8_t)((rawCsd[3U] & 0xC0000000U) >> 30U);
    csd->systemSpecificationVersion = (uint8_t)((rawCsd[3U] & 0x3C000000U) >> 26U);
    csd->dataReadAccessTime1 = (uint8_t)((rawCsd[3U] & 0xFF0000U) >> 16U);
    csd->dataReadAccessTime2 = (uint8_t)((rawCsd[3U] & 0xFF00U) >> 8U);
    csd->transferSpeed = (uint8_t)(rawCsd[3U] & 0xFFU);
    csd->cardCommandClass = (uint16_t)((rawCsd[2U] & 0xFFF00000U) >> 20U);
    /* Max block length read/write one time */
    csd->readBlockLength = (uint8_t)((rawCsd[2U] & 0xF0000U) >> 16U);
    if (rawCsd[2U] & 0x8000U)
    {
        csd->flags |= kMMC_CsdReadBlockPartialFlag;
    }
    if (rawCsd[2U] & 0x4000U)
    {
        csd->flags |= kMMC_CsdWriteBlockMisalignFlag;
    }
    if (rawCsd[2U] & 0x2000U)
    {
        csd->flags |= kMMC_CsdReadBlockMisalignFlag;
    }
    if (rawCsd[2U] & 0x1000U)
    {
        csd->flags |= kMMC_CsdDsrImplementedFlag;
    }
    csd->deviceSize = (uint16_t)(((rawCsd[2U] & 0x3FFU) << 2U) + ((rawCsd[1U] & 0xC0000000U) >> 30U));
    csd->readCurrentVddMin = (uint8_t)((rawCsd[1U] & 0x38000000U) >> 27U);
    csd->readCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x07000000U) >> 24U);
    csd->writeCurrentVddMin = (uint8_t)((rawCsd[1U] & 0x00E00000U) >> 21U);
    csd->writeCurrentVddMax = (uint8_t)((rawCsd[1U] & 0x001C0000U) >> 18U);
    csd->deviceSizeMultiplier = (uint8_t)((rawCsd[1U] & 0x00038000U) >> 15U);
    csd->eraseGroupSize = (uint8_t)((rawCsd[1U] & 0x00007C00U) >> 10U);
    csd->eraseGroupSizeMultiplier = (uint8_t)((rawCsd[1U] & 0x000003E0U) >> 5U);
    csd->writeProtectGroupSize = (uint8_t)(rawCsd[1U] & 0x0000001FU);
    if (rawCsd[0U] & 0x80000000U)
    {
        csd->flags |= kMMC_CsdWriteProtectGroupEnabledFlag;
    }
    csd->defaultEcc = (uint8_t)((rawCsd[0U] & 0x60000000U) >> 29U);
    csd->writeSpeedFactor = (uint8_t)((rawCsd[0U] & 0x1C000000U) >> 26U);
    csd->maxWriteBlockLength = (uint8_t)((rawCsd[0U] & 0x03C00000U) >> 22U);
    if (rawCsd[0U] & 0x00200000U)
    {
        csd->flags |= kMMC_CsdWriteBlockPartialFlag;
    }
    if (rawCsd[0U] & 0x00010000U)
    {
        csd->flags |= kMMC_ContentProtectApplicationFlag;
    }
    if (rawCsd[0U] & 0x00008000U)
    {
        csd->flags |= kMMC_CsdFileFormatGroupFlag;
    }
    if (rawCsd[0U] & 0x00004000U)
    {
        csd->flags |= kMMC_CsdCopyFlag;
    }
    if (rawCsd[0U] & 0x00002000U)
    {
        csd->flags |= kMMC_CsdPermanentWriteProtectFlag;
    }
    if (rawCsd[0U] & 0x00001000U)
    {
        csd->flags |= kMMC_CsdTemporaryWriteProtectFlag;
    }
    csd->fileFormat = (uint8_t)((rawCsd[0U] & 0x00000C00U) >> 10U);
    csd->eccCode = (uint8_t)((rawCsd[0U] & 0x00000300U) >> 8U);

    /* Calculate the device total block count. */
    /* For the card capacity of witch higher than 2GB, the maximum possible value should be set to this register
    is 0xFFF. */
    if (card->csd.deviceSize != 0xFFFU)
    {
        multiplier = (2U << (card->csd.deviceSizeMultiplier + 2U - 1U));
        card->userPartitionBlocks = (((card->csd.deviceSize + 1U) * multiplier) / FSL_SDMMC_DEFAULT_BLOCK_SIZE);
    }

    card->blockSize = FSL_SDMMC_DEFAULT_BLOCK_SIZE;
}

static void MMC_SetMaxFrequency(mmc_card_t *card)
{
    assert(card);

    uint32_t frequencyUnit;
    uint32_t multiplierFactor;
    uint32_t maxBusClock_Hz;

    /* g_fsdhcCommandUnitInTranSpeed and g_transerSpeedMultiplierFactor are used to calculate the max speed in normal
    mode not high speed mode.
    For cards supporting version 4.0, 4.1, and 4.2 of the specification, the value shall be 20MHz(0x2A).
    For cards supporting version 4.3, the value shall be 26 MHz (0x32H). In High speed mode, the max
    frequency is decided by CARD_TYPE in Extended CSD. */
    frequencyUnit = g_transerSpeedFrequencyUnit[READ_MMC_TRANSFER_SPEED_FREQUENCY_UNIT(card->csd)];
    multiplierFactor = g_transerSpeedMultiplierFactor[READ_MMC_TRANSFER_SPEED_MULTIPLIER(card->csd)];
    maxBusClock_Hz = (frequencyUnit * multiplierFactor) / DIVIDER_IN_TRANSFER_SPEED;
    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, maxBusClock_Hz);
}

static status_t MMC_SetMaxEraseUnitSize(mmc_card_t *card)
{
    assert(card);

    uint32_t erase_group_size;
    uint32_t erase_group_multiplier;
    mmc_extended_csd_config_t extendedCsdconfig;

    if (((!(card->flags & kMMC_SupportHighCapacityFlag)) || (card->extendedCsd.highCapacityEraseUnitSize == 0)) ||
        (card->extendedCsd.highCapacityEraseTimeout == 0))
    {
        erase_group_size = card->csd.eraseGroupSize;
        erase_group_multiplier = card->csd.eraseGroupSizeMultiplier;
        card->eraseGroupBlocks = ((erase_group_size + 1U) * (erase_group_multiplier + 1U));
    }
    else
    {
        /* Erase Unit Size = 512Kbyte * HC_ERASE_GRP_SIZE. Block size is 512 bytes. */
        card->eraseGroupBlocks = (card->extendedCsd.highCapacityEraseUnitSize * 1024U);
        /* Enable high capacity erase unit size. */
        extendedCsdconfig.accessMode = kMMC_ExtendedCsdAccessModeSetBits;
        extendedCsdconfig.ByteIndex = kMMC_ExtendedCsdIndexEraseGroupDefinition;
        extendedCsdconfig.ByteValue = 0x01U; /* The high capacity erase unit size enable bit is bit 0 */
        extendedCsdconfig.commandSet = kMMC_CommandSetStandard;
        if (kStatus_Success != MMC_SetExtendedCsdConfig(card, &extendedCsdconfig))
        {
            return kStatus_SDMMC_ConfigureExtendedCsdFailed;
        }
    }

    return kStatus_Success;
}

static status_t MMC_SetExtendedCsdConfig(mmc_card_t *card, const mmc_extended_csd_config_t *config)
{
    assert(card);
    assert(card->host.transfer);
    assert(config);

    uint32_t parameter = 0U;
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_TRANSFER content = {0};

    parameter |= ((uint32_t)(config->commandSet) << MMC_SWITCH_COMMAND_SET_SHIFT);
    parameter |= ((uint32_t)(config->ByteValue) << MMC_SWITCH_VALUE_SHIFT);
    parameter |= ((uint32_t)(config->ByteIndex) << MMC_SWITCH_BYTE_INDEX_SHIFT);
    parameter |= ((uint32_t)(config->accessMode) << MMC_SWITCH_ACCESS_MODE_SHIFT);

    command.index = kMMC_Switch;
    command.argument = parameter;
    command.responseType = kCARD_ResponseTypeR1b; /* Send switch command to set the pointed byte in Extended CSD. */
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG | SDMMC_MASK(kSDMMC_R1SwitchErrorFlag);

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success != MMC_Transfer(card, &content, 2U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Wait for the card write process complete because of that card read process and write process use one buffer. */
    if (kStatus_Success != MMC_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    return kStatus_Success;
}

static void MMC_DecodeExtendedCsd(mmc_card_t *card, uint32_t *rawExtendedCsd)
{
    assert(card);
    assert(rawExtendedCsd);

    uint8_t *buffer = (uint8_t *)rawExtendedCsd;
    mmc_extended_csd_t *extendedCsd = &(card->extendedCsd);

    /* Extended CSD is transferred as a data block from least byte indexed 0. */
    extendedCsd->bootPartitionWP = buffer[173U];
    extendedCsd->bootWPStatus = buffer[174U];
    extendedCsd->highDensityEraseGroupDefinition = buffer[175U];
    extendedCsd->bootDataBusConditions = buffer[177U];
    extendedCsd->bootConfigProtect = buffer[178U];
    extendedCsd->partitionConfig = buffer[179U];
    extendedCsd->eraseMemoryContent = buffer[181U];
    extendedCsd->dataBusWidth = buffer[183U];
    extendedCsd->highSpeedTiming = buffer[185U];
    extendedCsd->powerClass = buffer[187U];
    extendedCsd->commandSetRevision = buffer[189U];
    extendedCsd->commandSet = buffer[191U];
    extendedCsd->extendecCsdVersion = buffer[192U];
    extendedCsd->csdStructureVersion = buffer[194U];
    extendedCsd->partitionAttribute = buffer[156U];
    extendedCsd->extPartitionSupport = buffer[494U];
    extendedCsd->cardType = buffer[196U];
    /* This field define the type of the card. The only currently valid values for this field are 0x01 and 0x03. */
    card->flags |= extendedCsd->cardType;

    extendedCsd->ioDriverStrength = buffer[197U];

    extendedCsd->powerClass52MHz195V = buffer[200U];
    extendedCsd->powerClass26MHz195V = buffer[201U];
    extendedCsd->powerClass52MHz360V = buffer[202U];
    extendedCsd->powerClass26MHz360V = buffer[203U];
    extendedCsd->powerClass200MHZVCCQ130VVCC360V = buffer[236U];
    extendedCsd->powerClass200MHZVCCQ195VVCC360V = buffer[237U];
    extendedCsd->powerClass52MHZDDR195V = buffer[238U];
    extendedCsd->powerClass52MHZDDR360V = buffer[239U];
    extendedCsd->powerClass200MHZDDR360V = buffer[253U];
    extendedCsd->minimumReadPerformance4Bit26MHz = buffer[205U];
    extendedCsd->minimumWritePerformance4Bit26MHz = buffer[206U];
    extendedCsd->minimumReadPerformance8Bit26MHz4Bit52MHz = buffer[207U];
    extendedCsd->minimumWritePerformance8Bit26MHz4Bit52MHz = buffer[208U];
    extendedCsd->minimumReadPerformance8Bit52MHz = buffer[209U];
    extendedCsd->minimumWritePerformance8Bit52MHz = buffer[210U];
    extendedCsd->minReadPerformance8bitAt52MHZDDR = buffer[234U];
    extendedCsd->minWritePerformance8bitAt52MHZDDR = buffer[235U];
    /* Get user partition size. */
    extendedCsd->sectorCount = ((((uint32_t)buffer[215U]) << 24U) + (((uint32_t)buffer[214U]) << 16U) +
                                (((uint32_t)buffer[213U]) << 8U) + (uint32_t)buffer[212U]);
    if (card->flags & kMMC_SupportHighCapacityFlag)
    {
        card->userPartitionBlocks = card->extendedCsd.sectorCount;
    }

    extendedCsd->sleepAwakeTimeout = buffer[217U];
    extendedCsd->sleepCurrentVCCQ = buffer[219U];
    extendedCsd->sleepCurrentVCC = buffer[220U];
    extendedCsd->highCapacityWriteProtectGroupSize = buffer[221U];
    extendedCsd->reliableWriteSectorCount = buffer[222U];
    extendedCsd->highCapacityEraseTimeout = buffer[223U];
    extendedCsd->highCapacityEraseUnitSize = buffer[224U];
    extendedCsd->accessSize = buffer[225U];

    /* Get boot partition size: 128KB * BOOT_SIZE_MULT*/
    card->bootPartitionBlocks = ((128U * 1024U * buffer[226U]) / FSL_SDMMC_DEFAULT_BLOCK_SIZE);

    /* Check if card support boot mode. */
    if (buffer[228U] & 0x1U)
    {
        card->flags |= kMMC_SupportAlternateBootFlag;
    }
    else if (buffer[228U] & 0x2U)
    {
        card->flags |= kMMC_SupportDDRBootFlag;
    }
    else if (buffer[228U] & 0x4U)
    {
        card->flags |= kMMC_SupportHighSpeedBootFlag;
    }
    else
    {
    }
    /* cache size unit 1kb */
    extendedCsd->cacheSize = (((uint32_t)buffer[252U]) << 24) | (((uint32_t)buffer[251U]) << 16) |
                             (((uint32_t)buffer[250U]) << 8) | (((uint32_t)buffer[249U]));

    extendedCsd->supportedCommandSet = buffer[504U];
}

static status_t MMC_SendExtendedCsd(mmc_card_t *card, uint8_t *targetAddr, uint32_t byteIndex)
{
    assert(card);
    assert(card->host.transfer);

    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_DATA data = {0};
    uint32_t i;

    command.index = kMMC_SendExtendedCsd;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR1;

    data.blockCount = 1U;
    data.blockSize = MMC_EXTENDED_CSD_BYTES;
    data.rxData = card->rawExtendedCsd;

    content.command = &command;
    content.data = &data;
    if ((kStatus_Success == card->host.transfer(card->host.base, &content)) &&
        (!(command.response[0U] & SDMMC_R1_ALL_ERROR_FLAG)))
    {
        /* The response is from bit 127:8 in R2, corresponding to command.response[3][31:0] to
        command.response[0U][31:8] */
        switch (card->host.config.endianMode)
        {
            case kSDMMCHOST_EndianModeLittle:
                /* Doesn't need to switch byte sequence when decode bytes as little endian sequence. */
                break;
            case kSDMMCHOST_EndianModeBig:
                /* In big endian mode, the SD bus byte transferred first is the byte stored in highest position
                in a word which cause 4 byte's sequence in a word is not consistent with their original sequence
                from card. */
                for (i = 0U; i < MMC_EXTENDED_CSD_BYTES / 4U; i++)
                {
                    card->rawExtendedCsd[i] = SWAP_WORD_BYTE_SEQUENCE(card->rawExtendedCsd[i]);
                }
                break;
            case kSDMMCHOST_EndianModeHalfWordBig:
                for (i = 0U; i < MMC_EXTENDED_CSD_BYTES / 4U; i++)
                {
                    card->rawExtendedCsd[i] = SWAP_HALF_WROD_BYTE_SEQUENCE(card->rawExtendedCsd[i]);
                }
                break;
            default:
                return kStatus_SDMMC_NotSupportYet;
        }
        if (targetAddr != NULL)
        {
            *targetAddr = ((uint8_t *)card->rawExtendedCsd)[byteIndex];
        }
        else
        {
            MMC_DecodeExtendedCsd(card, card->rawExtendedCsd);
        }

        return kStatus_Success;
    }

    return kStatus_SDMMC_TransferFailed;
}

static status_t MMC_SetPowerClass(mmc_card_t *card)
{
    assert(card);

    uint8_t mask = 0, shift = 0U;
    uint8_t powerClass = 0;
    mmc_extended_csd_config_t extendedCsdconfig;

    if ((card->busWidth == kMMC_DataBusWidth4bit) || (card->busWidth == kMMC_DataBusWidth4bitDDR))
    {
        mask = MMC_POWER_CLASS_4BIT_MASK; /* The mask of 4 bit bus width's power class */
        shift = 0U;
    }
    else if ((card->busWidth == kMMC_DataBusWidth8bit) || (card->busWidth == kMMC_DataBusWidth8bitDDR))
    {
        mask = MMC_POWER_CLASS_8BIT_MASK; /* The mask of 8 bit bus width's power class */
        shift = 4U;
    }
    else
    {
        return kStatus_Success;
    }

    switch (card->hostVoltageWindowVCC)
    {
        case kMMC_VoltageWindows270to360:

            if (card->busTiming == kMMC_HighSpeed200Timing)
            {
                if (card->hostVoltageWindowVCCQ == kMMC_VoltageWindow170to195)
                {
                    powerClass = ((card->extendedCsd.powerClass200MHZVCCQ195VVCC360V) & mask);
                }
                else if (card->hostVoltageWindowVCCQ == kMMC_VoltageWindow120)
                {
                    powerClass = ((card->extendedCsd.powerClass200MHZVCCQ130VVCC360V) & mask);
                }
            }
            else if (card->busTiming == kMMC_HighSpeed400Timing)
            {
                powerClass = ((card->extendedCsd.powerClass200MHZDDR360V) & mask);
            }
            else if ((card->busTiming == kMMC_HighSpeedTiming) && (card->busWidth > kMMC_DataBusWidth8bit))
            {
                powerClass = ((card->extendedCsd.powerClass52MHZDDR360V) & mask);
            }
            else if ((card->busTiming == kMMC_HighSpeedTiming) && (card->busClock_Hz > MMC_CLOCK_26MHZ))
            {
                powerClass = ((card->extendedCsd.powerClass52MHz360V) & mask);
            }
            else if (card->busTiming == kMMC_HighSpeedTiming)
            {
                powerClass = ((card->extendedCsd.powerClass26MHz360V) & mask);
            }

            break;

        case kMMC_VoltageWindow170to195:

            if ((card->busTiming == kMMC_HighSpeedTiming) && (card->busClock_Hz <= MMC_CLOCK_26MHZ))
            {
                powerClass = ((card->extendedCsd.powerClass26MHz195V) & mask);
            }
            else if ((card->busTiming == kMMC_HighSpeedTiming) && (card->busClock_Hz > MMC_CLOCK_26MHZ))
            {
                powerClass = ((card->extendedCsd.powerClass52MHz195V) & mask);
            }
            else if ((card->busTiming == kMMC_HighSpeedTiming) && (card->busWidth > kMMC_DataBusWidth8bit))
            {
                powerClass = ((card->extendedCsd.powerClass52MHZDDR195V) & mask);
            }

            break;
        default:
            powerClass = 0;
            break;
    }

    /* due to 8bit power class position [7:4] */
    powerClass >>= shift;

    if (powerClass > 0U)
    {
        extendedCsdconfig.accessMode = kMMC_ExtendedCsdAccessModeWriteBits;
        extendedCsdconfig.ByteIndex = kMMC_ExtendedCsdIndexPowerClass;
        extendedCsdconfig.ByteValue = powerClass;
        extendedCsdconfig.commandSet = kMMC_CommandSetStandard;
        if (kStatus_Success != MMC_SetExtendedCsdConfig(card, &extendedCsdconfig))
        {
            return kStatus_SDMMC_ConfigureExtendedCsdFailed;
        }
        /* restore power class */
        card->extendedCsd.powerClass = powerClass;
    }

    return kStatus_Success;
}

static status_t MMC_SendTestPattern(mmc_card_t *card, uint32_t blockSize, uint32_t *pattern)
{
    assert(card);
    assert(card->host.transfer);
    assert(blockSize <= FSL_SDMMC_DEFAULT_BLOCK_SIZE);
    assert(pattern);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};

    command.index = kMMC_SendingBusTest;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR1;

    /* Ignore errors in bus test procedure to improve chances that the test will work. */
    data.enableIgnoreError = true;
    data.blockCount = 1U;
    data.blockSize = blockSize;
    data.txData = pattern;

    content.command = &command;
    content.data = &data;
    if ((kStatus_Success != card->host.transfer(card->host.base, &content)) ||
        (command.response[0U] & SDMMC_R1_ALL_ERROR_FLAG))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

static status_t MMC_ReceiveTestPattern(mmc_card_t *card, uint32_t blockSize, uint32_t *pattern)
{
    assert(card);
    assert(card->host.transfer);
    assert(blockSize <= FSL_SDMMC_DEFAULT_BLOCK_SIZE);
    assert(pattern);

    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};

    command.index = kMMC_BusTestRead;
    command.responseType = kCARD_ResponseTypeR1;

    /* Ignore errors in bus test procedure to improve chances that the test will work. */
    data.enableIgnoreError = true;
    data.blockCount = 1U;
    data.blockSize = blockSize;
    data.rxData = pattern;

    content.command = &command;
    content.data = &data;
    if ((kStatus_Success != card->host.transfer(card->host.base, &content)) ||
        ((command.response[0U]) & SDMMC_R1_ALL_ERROR_FLAG))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

static status_t MMC_TestDataBusWidth(mmc_card_t *card, mmc_data_bus_width_t width)
{
    assert(card);

    uint32_t blockSize = 0U;
    uint32_t tempsendPattern = 0U;
    uint32_t *tempPattern = g_sdmmc;
    uint32_t xorMask = 0U;
    uint32_t xorResult = 0U;

    /* For 8 data lines the data block would be (MSB to LSB): 0x0000_0000_0000_AA55,
    For 4 data lines the data block would be (MSB to LSB): 0x0000_005A,
    For only 1 data line the data block would be: 0x80 */
    switch (width)
    {
        case kMMC_DataBusWidth8bit:
        case kMMC_DataBusWidth8bitDDR:
            blockSize = 8U;
            tempPattern[0U] = 0xAA55U;
            xorMask = 0xFFFFU;
            xorResult = 0xFFFFU;
            break;
        case kMMC_DataBusWidth4bit:
        case kMMC_DataBusWidth4bitDDR:
            blockSize = 4U;
            tempPattern[0U] = 0x5AU;
            xorMask = 0xFFU;
            xorResult = 0xFFU;
            break;
        default:
            blockSize = 4U;
            tempPattern[0U] = 0x80U;
            xorMask = 0xFFU;
            xorResult = 0xC0U;
            break;
    }
    switch (card->host.config.endianMode)
    {
        case kSDMMCHOST_EndianModeLittle:
            /* Doesn't need to switch byte sequence when decodes bytes as little endian sequence. */
            break;
        case kSDMMCHOST_EndianModeBig:
            /* In big endian mode, the byte transferred first is the byte stored in highest byte position in a word
            which will cause the card receive the inverted byte sequence in a word in bus test procedure. So the
            sequence of 4 bytes stored in a word should be converted. */
            tempPattern[0] = SWAP_WORD_BYTE_SEQUENCE(tempPattern[0]);
            xorMask = SWAP_WORD_BYTE_SEQUENCE(xorMask);
            xorResult = SWAP_WORD_BYTE_SEQUENCE(xorResult);
            break;
        case kSDMMCHOST_EndianModeHalfWordBig:
            /* In half word big endian mode, the byte transferred first is the lower byte in the higher half word.
            0xAA55U should be converted to 0xAA550000U to set the 0x55 to be the first byte to transfer. */
            tempPattern[0] = SWAP_HALF_WROD_BYTE_SEQUENCE(tempPattern[0]);
            xorMask = SWAP_HALF_WROD_BYTE_SEQUENCE(xorMask);
            xorResult = SWAP_HALF_WROD_BYTE_SEQUENCE(xorResult);
            tempPattern[0] = SWAP_WORD_BYTE_SEQUENCE(tempPattern[0]);
            xorMask = SWAP_WORD_BYTE_SEQUENCE(xorMask);
            xorResult = SWAP_WORD_BYTE_SEQUENCE(xorResult);
            break;
        default:
            return kStatus_SDMMC_NotSupportYet;
    }

    if (kStatus_Success != MMC_SendTestPattern(card, blockSize, tempPattern))
    {
        return kStatus_SDMMC_SendTestPatternFailed;
    }
    /* restore the send pattern */
    tempsendPattern = tempPattern[0U];
    /* reset the global buffer */
    tempPattern[0U] = 0U;

    if (kStatus_Success != MMC_ReceiveTestPattern(card, blockSize, tempPattern))
    {
        return kStatus_SDMMC_ReceiveTestPatternFailed;
    }

    /* XOR the send pattern and receive pattern */
    if (((tempPattern[0U] ^ tempsendPattern) & xorMask) != xorResult)
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

static status_t MMC_SetDataBusWidth(mmc_card_t *card, mmc_data_bus_width_t width)
{
    assert(card);

    mmc_extended_csd_config_t extendedCsdconfig;

    /* Set data bus width */
    extendedCsdconfig.accessMode = kMMC_ExtendedCsdAccessModeWriteBits;
    extendedCsdconfig.ByteIndex = kMMC_ExtendedCsdIndexBusWidth;
    extendedCsdconfig.ByteValue = width;
    extendedCsdconfig.commandSet = kMMC_CommandSetStandard;
    if (kStatus_Success != MMC_SetExtendedCsdConfig(card, &extendedCsdconfig))
    {
        return kStatus_SDMMC_ConfigureExtendedCsdFailed;
    }
    /* restore data bus width */
    card->extendedCsd.dataBusWidth = width;

    return kStatus_Success;
}

static status_t MMC_SetMaxDataBusWidth(mmc_card_t *card, mmc_high_speed_timing_t targetTiming)
{
    assert(card);

    status_t error = kStatus_Fail;

    switch (card->busWidth)
    {
        case kMMC_DataBusWidth1bit:
        case kMMC_DataBusWidth8bitDDR:
            /* Test and set the data bus width for card. */
            if ((SDMMCHOST_NOT_SUPPORT != kSDMMCHOST_Support8BitBusWidth) &&
                (card->flags & (kMMC_SupportHighSpeedDDR52MHZ180V300VFlag | kMMC_SupportHighSpeedDDR52MHZ120VFlag)) &&
                ((targetTiming == kMMC_HighSpeedTiming) || (targetTiming == kMMC_HighSpeed400Timing)))
            {
                SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH8BIT);
                if ((kStatus_Success == MMC_TestDataBusWidth(card, kMMC_DataBusWidth8bitDDR)) &&
                    (kStatus_Success == MMC_SetDataBusWidth(card, kMMC_DataBusWidth8bitDDR)))
                {
                    error = kStatus_Success;
                    card->busWidth = kMMC_DataBusWidth8bitDDR;
                    break;
                }
                /* HS400 mode only support 8bit data bus */
                else if (card->busTiming == kMMC_HighSpeed400Timing)
                {
                    return kStatus_SDMMC_SetDataBusWidthFailed;
                }
            }
        case kMMC_DataBusWidth4bitDDR:
            if ((SDMMCHOST_NOT_SUPPORT != kSDMMCHOST_Support4BitBusWidth) &&
                (card->flags & (kMMC_SupportHighSpeedDDR52MHZ180V300VFlag | kMMC_SupportHighSpeedDDR52MHZ120VFlag)) &&
                ((targetTiming == kMMC_HighSpeedTiming) || (targetTiming == kMMC_HighSpeed400Timing)))
            {
                SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH4BIT);
                if ((kStatus_Success == MMC_TestDataBusWidth(card, kMMC_DataBusWidth4bitDDR)) &&
                    (kStatus_Success == MMC_SetDataBusWidth(card, kMMC_DataBusWidth4bitDDR)))
                {
                    error = kStatus_Success;
                    card->busWidth = kMMC_DataBusWidth4bitDDR;

                    break;
                }
            }
        case kMMC_DataBusWidth8bit:
            if ((SDMMCHOST_NOT_SUPPORT != kSDMMCHOST_Support8BitBusWidth) &&
                ((targetTiming == kMMC_HighSpeedTiming) || (targetTiming == kMMC_HighSpeed200Timing)))
            {
                SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH8BIT);
                if ((kStatus_Success == MMC_TestDataBusWidth(card, kMMC_DataBusWidth8bit)) &&
                    (kStatus_Success == MMC_SetDataBusWidth(card, kMMC_DataBusWidth8bit)))
                {
                    error = kStatus_Success;
                    card->busWidth = kMMC_DataBusWidth8bit;
                    break;
                }
            }

        case kMMC_DataBusWidth4bit:
            if ((SDMMCHOST_NOT_SUPPORT != kSDMMCHOST_Support4BitBusWidth) &&
                ((targetTiming == kMMC_HighSpeedTiming) || (targetTiming == kMMC_HighSpeed200Timing)))
            {
                SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH4BIT);
                if ((kStatus_Success == MMC_TestDataBusWidth(card, kMMC_DataBusWidth4bit)) &&
                    (kStatus_Success == MMC_SetDataBusWidth(card, kMMC_DataBusWidth4bit)))
                {
                    error = kStatus_Success;
                    card->busWidth = kMMC_DataBusWidth4bit;
                    break;
                }
                /* HS200 mode only support 4bit/8bit data bus */
                else if (targetTiming == kMMC_HighSpeed200Timing)
                {
                    return kStatus_SDMMC_SetDataBusWidthFailed;
                }
            }
        default:
            break;
    }

    if (error == kStatus_Fail)
    {
        /* Card's data bus width will be default 1 bit mode. */
        SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH1BIT);
    }

    return kStatus_Success;
}

static status_t MMC_SwitchHSTiming(mmc_card_t *card, uint8_t timing, uint8_t driverStrength)
{
    assert(card);

    uint8_t hsTiming = 0;

    mmc_extended_csd_config_t extendedCsdconfig;

    /* check the target driver strength support or not */
    if (((card->extendedCsd.ioDriverStrength & (1 << driverStrength)) == 0U) &&
        (card->extendedCsd.extendecCsdVersion >= kMMC_ExtendedCsdRevision17))
    {
        return kStatus_SDMMC_NotSupportYet;
    }
    /* calucate the register value */
    hsTiming = (timing & 0xF) | (uint8_t)(driverStrength << 4U);

    /* Switch to high speed timing. */
    extendedCsdconfig.accessMode = kMMC_ExtendedCsdAccessModeWriteBits;
    extendedCsdconfig.ByteIndex = kMMC_ExtendedCsdIndexHighSpeedTiming;
    extendedCsdconfig.ByteValue = hsTiming;
    extendedCsdconfig.commandSet = kMMC_CommandSetStandard;
    if (kStatus_Success != MMC_SetExtendedCsdConfig(card, &extendedCsdconfig))
    {
        return kStatus_SDMMC_ConfigureExtendedCsdFailed;
    }

    card->extendedCsd.highSpeedTiming = hsTiming;

    return kStatus_Success;
}

static status_t MMC_SwitchToHighSpeed(mmc_card_t *card)
{
    assert(card);

    uint32_t freq = 0U;

    /* check VCCQ voltage supply */
    if (kSDMMCHOST_SupportV180 != SDMMCHOST_NOT_SUPPORT)
    {
        if ((card->hostVoltageWindowVCCQ != kMMC_VoltageWindow170to195) &&
            (card->extendedCsd.extendecCsdVersion > kMMC_ExtendedCsdRevision10))
        {
            SDMMCHOST_SWITCH_VOLTAGE180V(card->host.base, true);
            card->hostVoltageWindowVCCQ = kMMC_VoltageWindow170to195;
        }
    }
    else if (kSDMMCHOST_SupportV120 != SDMMCHOST_NOT_SUPPORT)
    {
        if ((card->hostVoltageWindowVCCQ != kMMC_VoltageWindow120) &&
            (card->extendedCsd.extendecCsdVersion >= kMMC_ExtendedCsdRevision16))
        {
            SDMMCHOST_SWITCH_VOLTAGE120V(card->host.base, true);
            card->hostVoltageWindowVCCQ = kMMC_VoltageWindow120;
        }
    }
    else
    {
        card->hostVoltageWindowVCCQ = kMMC_VoltageWindows270to360;
    }

    if (kStatus_Success != MMC_SwitchHSTiming(card, kMMC_HighSpeedTiming, kMMC_DriverStrength0))
    {
        return kStatus_SDMMC_SwitchBusTimingFailed;
    }

    if ((card->busWidth == kMMC_DataBusWidth4bitDDR) || (card->busWidth == kMMC_DataBusWidth8bitDDR))
    {
        freq = MMC_CLOCK_DDR52;
        SDMMCHOST_ENABLE_DDR_MODE(card->host.base, true, 0U);
    }
    else if (card->flags & kMMC_SupportHighSpeed52MHZFlag)
    {
        freq = MMC_CLOCK_52MHZ;
    }
    else if (card->flags & kMMC_SupportHighSpeed26MHZFlag)
    {
        freq = MMC_CLOCK_26MHZ;
    }

    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, freq);
    /* config io speed and strength */
    SDMMCHOST_CONFIG_MMC_IO(CARD_BUS_FREQ_100MHZ1, CARD_BUS_STRENGTH_7);

    /* Set card data width, it is nessesary to config the the data bus here, to meet emmc5.0 specification,
    * when you are working in DDR mode , HS_TIMING must set before set bus width
    */
    if (MMC_SetMaxDataBusWidth(card, kMMC_HighSpeedTiming) != kStatus_Success)
    {
        return kStatus_SDMMC_SetDataBusWidthFailed;
    }

    card->busTiming = kMMC_HighSpeedTiming;

    return kStatus_Success;
}

static status_t MMC_SwitchToHS200(mmc_card_t *card, uint32_t freq)
{
    assert(card);

    /* check VCCQ voltage supply */
    if (kSDMMCHOST_SupportV180 != SDMMCHOST_NOT_SUPPORT)
    {
        if (card->hostVoltageWindowVCCQ != kMMC_VoltageWindow170to195)
        {
            SDMMCHOST_SWITCH_VOLTAGE180V(card->host.base, true);
            card->hostVoltageWindowVCCQ = kMMC_VoltageWindow170to195;
        }
    }
    else if (kSDMMCHOST_SupportV120 != SDMMCHOST_NOT_SUPPORT)
    {
        if (card->hostVoltageWindowVCCQ != kMMC_VoltageWindow120)
        {
            SDMMCHOST_SWITCH_VOLTAGE120V(card->host.base, true);
            card->hostVoltageWindowVCCQ = kMMC_VoltageWindow120;
        }
    }
    else
    {
        return kStatus_SDMMC_InvalidVoltage;
    }

    /* select bus width before select bus timing for HS200 mode */
    if (MMC_SetMaxDataBusWidth(card, kMMC_HighSpeed200Timing) != kStatus_Success)
    {
        return kStatus_SDMMC_SetDataBusWidthFailed;
    }

    /* switch to HS200 mode */
    if (kStatus_Success != MMC_SwitchHSTiming(card, kMMC_HighSpeed200Timing, kMMC_DriverStrength0))
    {
        return kStatus_SDMMC_SwitchBusTimingFailed;
    }

    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, freq);
    /* config io speed and strength */
    SDMMCHOST_CONFIG_MMC_IO(CARD_BUS_FREQ_200MHZ, CARD_BUS_STRENGTH_7);

    /* excute tuning for HS200 */
    if (MMC_ExecuteTuning(card) != kStatus_Success)
    {
        return kStatus_SDMMC_TuningFail;
    }

    /* Wait for the card status ready. */
    if (kStatus_Success != MMC_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    card->busTiming = kMMC_HighSpeed200Timing;

    return kStatus_Success;
}

static status_t MMC_SwitchToHS400(mmc_card_t *card)
{
    assert(card);

    /* check VCCQ voltage supply */
    if (kSDMMCHOST_SupportV180 != SDMMCHOST_NOT_SUPPORT)
    {
        if (card->hostVoltageWindowVCCQ != kMMC_VoltageWindow170to195)
        {
            SDMMCHOST_SWITCH_VOLTAGE180V(card->host.base, true);
            card->hostVoltageWindowVCCQ = kMMC_VoltageWindow170to195;
        }
    }
    else if (kSDMMCHOST_SupportV120 != SDMMCHOST_NOT_SUPPORT)
    {
        if (card->hostVoltageWindowVCCQ != kMMC_VoltageWindow120)
        {
            SDMMCHOST_SWITCH_VOLTAGE120V(card->host.base, true);
            card->hostVoltageWindowVCCQ = kMMC_VoltageWindow120;
        }
    }
    else
    {
        return kStatus_SDMMC_InvalidVoltage;
    }

    /* check data bus width is 8 bit , otherwise return false */
    if (card->busWidth == kMMC_DataBusWidth8bit)
    {
        return kStatus_SDMMC_SwitchBusTimingFailed;
    }

    /* switch to high speed first */
    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, MMC_CLOCK_52MHZ);
    SDMMCHOST_CONFIG_MMC_IO(CARD_BUS_FREQ_100MHZ1, CARD_BUS_STRENGTH_5);
    /*switch to high speed*/
    if (kStatus_Success != MMC_SwitchHSTiming(card, kMMC_HighSpeedTiming, kMMC_DriverStrength0))
    {
        return kStatus_SDMMC_ConfigureExtendedCsdFailed;
    }
    card->busTiming = kMMC_HighSpeed400Timing;
    /* switch to 8 bit DDR data bus width */
    if (kStatus_Success != MMC_SetDataBusWidth(card, kMMC_DataBusWidth8bitDDR))
    {
        return kStatus_SDMMC_SetDataBusWidthFailed;
    }
    /* switch to HS400 */
    if (kStatus_Success != MMC_SwitchHSTiming(card, kMMC_HighSpeed400Timing, kMMC_DriverStrength0))
    {
        return kStatus_SDMMC_SwitchBusTimingFailed;
    }
    /* config to target freq */
    card->busClock_Hz =
        SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SDMMCHOST_SUPPORT_HS400_FREQ);
    /* config io speed and strength */
    SDMMCHOST_CONFIG_MMC_IO(CARD_BUS_FREQ_200MHZ, CARD_BUS_STRENGTH_7);
    /* enable HS400 mode */
    SDMMCHOST_ENABLE_HS400_MODE(card->host.base, true);
    /* enable DDR mode */
    SDMMCHOST_ENABLE_DDR_MODE(card->host.base, true, 0U);
    /* config strobe DLL */
    SDMMCHOST_CONFIG_STROBE_DLL(card->host.base, SDMMCHOST_STROBE_DLL_DELAY_TARGET,
                                SDMMCHOST_STROBE_DLL_DELAY_UPDATE_INTERVAL);
    /* enable DLL */
    SDMMCHOST_ENABLE_STROBE_DLL(card->host.base, true);

    return kStatus_Success;
}

static status_t MMC_SelectBusTiming(mmc_card_t *card)
{
    assert(card);

    mmc_high_speed_timing_t targetTiming = card->busTiming;

    switch (targetTiming)
    {
        case kMMC_HighSpeedTimingNone:
        case kMMC_HighSpeed400Timing:
            if ((card->flags & (kMMC_SupportHS400DDR200MHZ180VFlag | kMMC_SupportHS400DDR200MHZ120VFlag)) &&
                ((kSDMMCHOST_SupportHS400 != SDMMCHOST_NOT_SUPPORT)))
            {
                /* switch to HS200 perform tuning */
                if (kStatus_Success != MMC_SwitchToHS200(card, SDMMCHOST_SUPPORT_HS400_FREQ / 2U))
                {
                    return kStatus_SDMMC_SwitchBusTimingFailed;
                }
                /* switch to HS400 */
                if (kStatus_Success != MMC_SwitchToHS400(card))
                {
                    return kStatus_SDMMC_SwitchBusTimingFailed;
                }
                break;
            }
        case kMMC_HighSpeed200Timing:
            if ((card->flags & (kMMC_SupportHS200200MHZ180VFlag | kMMC_SupportHS200200MHZ120VFlag)) &&
                ((kSDMMCHOST_SupportHS200 != SDMMCHOST_NOT_SUPPORT)))
            {
                if (kStatus_Success != MMC_SwitchToHS200(card, SDMMCHOST_SUPPORT_HS200_FREQ))
                {
                    return kStatus_SDMMC_SwitchBusTimingFailed;
                }
                break;
            }
        case kMMC_HighSpeedTiming:
            if (kStatus_Success != MMC_SwitchToHighSpeed(card))
            {
                return kStatus_SDMMC_SwitchBusTimingFailed;
            }
            break;

        default:
            card->busTiming = kMMC_HighSpeedTimingNone;
    }

    return kStatus_Success;
}

static void MMC_DecodeCid(mmc_card_t *card, uint32_t *rawCid)
{
    assert(card);
    assert(rawCid);

    mmc_cid_t *cid;

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

static status_t MMC_AllSendCid(mmc_card_t *card)
{
    assert(card);
    assert(card->host.transfer);

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
        MMC_DecodeCid(card, command.response);

        return kStatus_Success;
    }

    return kStatus_SDMMC_TransferFailed;
}

static status_t MMC_SendCsd(mmc_card_t *card)
{
    assert(card);
    assert(card->host.transfer);

    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_TRANSFER content = {0};

    command.index = kSDMMC_SendCsd;
    command.argument = (card->relativeAddress << 16U);
    command.responseType = kCARD_ResponseTypeR2;

    content.command = &command;
    content.data = 0U;
    if (kStatus_Success == card->host.transfer(card->host.base, &content))
    {
        memcpy(card->rawCsd, command.response, sizeof(card->rawCsd));
        /* The response is from bit 127:8 in R2, corresponding to command.response[3][31:0] to
        command.response[0U][31:8]. */
        MMC_DecodeCsd(card, card->rawCsd);

        return kStatus_Success;
    }

    return kStatus_SDMMC_TransferFailed;
}

static status_t MMC_CheckBlockRange(mmc_card_t *card, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(blockCount);

    status_t error = kStatus_Success;
    uint32_t partitionBlocks;

    switch (card->currentPartition)
    {
        case kMMC_AccessPartitionUserAera:
        {
            partitionBlocks = card->userPartitionBlocks;
            break;
        }
        case kMMC_AccessPartitionBoot1:
        case kMMC_AccessPartitionBoot2:
        {
            /* Boot partition 1 and partition 2 have the same partition size. */
            partitionBlocks = card->bootPartitionBlocks;
            break;
        }
        default:
            error = kStatus_InvalidArgument;
            break;
    }
    /* Check if the block range accessed is within current partition's block boundary. */
    if ((error == kStatus_Success) && ((startBlock + blockCount) > partitionBlocks))
    {
        error = kStatus_InvalidArgument;
    }

    return error;
}

static status_t MMC_CheckEraseGroupRange(mmc_card_t *card, uint32_t startGroup, uint32_t endGroup)
{
    assert(card);

    status_t error = kStatus_Success;
    uint32_t partitionBlocks;
    uint32_t eraseGroupBoundary;

    switch (card->currentPartition)
    {
        case kMMC_AccessPartitionUserAera:
        {
            partitionBlocks = card->userPartitionBlocks;
            break;
        }
        case kMMC_AccessPartitionBoot1:
        case kMMC_AccessPartitionBoot2:
        {
            /* Boot partition 1 and partition 2 have the same partition size. */
            partitionBlocks = card->bootPartitionBlocks;
            break;
        }
        default:
            error = kStatus_InvalidArgument;
            break;
    }

    if (error == kStatus_Success)
    {
        /* Check if current partition's total block count is integer multiples of the erase group size. */
        if ((partitionBlocks % card->eraseGroupBlocks) == 0U)
        {
            eraseGroupBoundary = (partitionBlocks / card->eraseGroupBlocks);
        }
        else
        {
            /* Card will ignore the unavailable blocks within the last erase group automatically. */
            eraseGroupBoundary = (partitionBlocks / card->eraseGroupBlocks + 1U);
        }

        /* Check if the group range accessed is within current partition's erase group boundary. */
        if ((startGroup > eraseGroupBoundary) || (endGroup > eraseGroupBoundary))
        {
            error = kStatus_InvalidArgument;
        }
    }

    return error;
}

static status_t MMC_Read(
    mmc_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount)
{
    assert(card);
    assert(card->host.transfer);
    assert(buffer);
    assert(blockCount);
    assert(blockSize);
    assert(blockSize == FSL_SDMMC_DEFAULT_BLOCK_SIZE);

    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};
    SDMMCHOST_TRANSFER content = {0};
    status_t error;

    if (((card->flags & kMMC_SupportHighCapacityFlag) && (blockSize != 512U)) || (blockSize > card->blockSize) ||
        (blockSize > card->host.capability.maxBlockLength) || (blockSize % 4U))
    {
        return kStatus_SDMMC_CardNotSupport;
    }

    /* Wait for the card write process complete because of that card read process and write process use one buffer. */
    if (kStatus_Success != MMC_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.rxData = (uint32_t *)buffer;
    data.enableAutoCommand12 = true;
    command.index = kSDMMC_ReadMultipleBlock;
    if (data.blockCount == 1U)
    {
        command.index = kSDMMC_ReadSingleBlock;
    }
    else
    {
        if ((!(data.enableAutoCommand12)) && (card->enablePreDefinedBlockCount))
        {
            /* If enabled the pre-define count read/write feature of the card, need to set block count firstly. */
            if (kStatus_Success != MMC_SetBlockCount(card, blockCount))
            {
                return kStatus_SDMMC_SetBlockCountFailed;
            }
        }
    }
    command.argument = startBlock;
    if (!(card->flags & kMMC_SupportHighCapacityFlag))
    {
        command.argument *= data.blockSize;
    }
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    content.command = &command;
    content.data = &data;

    /* should check tuning error during every transfer */
    error = MMC_Transfer(card, &content, 1U);
    if (kStatus_Success != error)
    {
        return error;
    }

    /* When host's AUTO_COMMAND12 feature isn't enabled and PRE_DEFINED_COUNT command isn't enabled in multiple
    blocks transmission, sends STOP_TRANSMISSION command. */
    if ((blockCount > 1U) && (!(data.enableAutoCommand12)) && (!card->enablePreDefinedBlockCount))
    {
        if (kStatus_Success != MMC_StopTransmission(card))
        {
            return kStatus_SDMMC_StopTransmissionFailed;
        }
    }

    return kStatus_Success;
}

static status_t MMC_Write(
    mmc_card_t *card, const uint8_t *buffer, uint32_t startBlock, uint32_t blockSize, uint32_t blockCount)
{
    assert(card);
    assert(card->host.transfer);
    assert(buffer);
    assert(blockCount);
    assert(blockSize);
    assert(blockSize == FSL_SDMMC_DEFAULT_BLOCK_SIZE);

    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_DATA data = {0};
    SDMMCHOST_TRANSFER content = {0};
    status_t error;

    /* Check address range */
    if (((card->flags & kMMC_SupportHighCapacityFlag) && (blockSize != 512U)) || (blockSize > card->blockSize) ||
        (blockSize > card->host.capability.maxBlockLength) || (blockSize % 4U))
    {
        return kStatus_SDMMC_CardNotSupport;
    }

    /* Wait for the card's buffer to be not full to write to improve the write performance. */
    while ((GET_SDMMCHOST_STATUS(card->host.base) & CARD_DATA0_STATUS_MASK) != CARD_DATA0_NOT_BUSY)
    {
    }

    /* Wait for the card write process complete */
    if (kStatus_Success != MMC_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    data.blockSize = blockSize;
    data.blockCount = blockCount;
    data.txData = (const uint32_t *)buffer;
    data.enableAutoCommand12 = true;

    command.index = kSDMMC_WriteMultipleBlock;
    if (data.blockCount == 1U)
    {
        command.index = kSDMMC_WriteSingleBlock;
    }
    else
    {
        if ((!(data.enableAutoCommand12)) && (card->enablePreDefinedBlockCount))
        {
            /* If enabled the pre-define count read/write featue of the card, need to set block count firstly */
            if (kStatus_Success != MMC_SetBlockCount(card, blockCount))
            {
                return kStatus_SDMMC_SetBlockCountFailed;
            }
        }
    }
    command.argument = startBlock;
    if (!(card->flags & kMMC_SupportHighCapacityFlag))
    {
        command.argument *= blockSize;
    }
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    content.command = &command;
    content.data = &data;

    /* should check tuning error during every transfer */
    error = MMC_Transfer(card, &content, 1U);
    if (kStatus_Success != error)
    {
        return error;
    }

    /* When host's AUTO_COMMAND12 feature isn't enabled and PRE_DEFINED_COUNT command isn't enabled in multiple
    blocks transmission, sends STOP_TRANSMISSION command. */
    if ((blockCount > 1U) && (!(data.enableAutoCommand12)) && (!card->enablePreDefinedBlockCount))
    {
        if (kStatus_Success != MMC_StopTransmission(card))
        {
            return kStatus_SDMMC_StopTransmissionFailed;
        }
    }

    return kStatus_Success;
}

status_t MMC_CardInit(mmc_card_t *card)
{
    assert(card);
    assert((card->hostVoltageWindowVCC != kMMC_VoltageWindowNone) &&
           (card->hostVoltageWindowVCC != kMMC_VoltageWindow120));

    uint32_t opcode = 0U;

    if (!card->isHostReady)
    {
        return kStatus_SDMMC_HostNotReady;
    }
    /* set DATA bus width */
    SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH1BIT);
    /* Set clock to 400KHz. */
    card->busClock_Hz = SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, SDMMC_CLOCK_400KHZ);
    /* get host capability first */
    GET_SDMMCHOST_CAPABILITY(card->host.base, &(card->host.capability));

    /* Send CMD0 to reset the bus */
    if (kStatus_Success != MMC_GoIdle(card))
    {
        return kStatus_SDMMC_GoIdleFailed;
    }

    /* Hand-shaking with card to validata the voltage range Host first sending its expected
       information.*/
    if (kStatus_Success != MMC_SendOperationCondition(card, 0U))
    {
        return kStatus_SDMMC_HandShakeOperationConditionFailed;
    }

    /* switch the host voltage which the card can support */
    if (kStatus_Success != MMC_SwitchVoltage(card, &opcode))
    {
        return kStatus_SDMMC_HandShakeOperationConditionFailed;
    }

    /* Get host's access mode. */
    if (card->host.capability.maxBlockLength >= FSL_SDMMC_DEFAULT_BLOCK_SIZE)
    {
        opcode |= kMMC_AccessModeSector << MMC_OCR_ACCESS_MODE_SHIFT;
    }
    else
    {
        opcode |= kMMC_AccessModeByte << MMC_OCR_ACCESS_MODE_SHIFT;
    }

    if (kStatus_Success != MMC_SendOperationCondition(card, opcode))
    {
        return kStatus_SDMMC_HandShakeOperationConditionFailed;
    }

    /* Get card CID */
    if (kStatus_Success != MMC_AllSendCid(card))
    {
        return kStatus_SDMMC_AllSendCidFailed;
    }

    /* Set the card relative address */
    if (kStatus_Success != MMC_SetRelativeAddress(card))
    {
        return kStatus_SDMMC_SetRelativeAddressFailed;
    }

    /* Get the CSD register content */
    if (kStatus_Success != MMC_SendCsd(card))
    {
        return kStatus_SDMMC_SendCsdFailed;
    }

    /* Set to maximum speed in normal mode. */
    MMC_SetMaxFrequency(card);

    /* Send CMD7 with the card's relative address to place the card in transfer state. Puts current selected card in
    transfer state. */
    if (kStatus_Success != MMC_SelectCard(card, true))
    {
        return kStatus_SDMMC_SelectCardFailed;
    }

    /* Get Extended CSD register content. */
    if (kStatus_Success != MMC_SendExtendedCsd(card, NULL, 0U))
    {
        return kStatus_SDMMC_SendExtendedCsdFailed;
    }

    /* set block size */
    if (kStatus_Success != MMC_SetBlockSize(card, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
    {
        return kStatus_SDMMC_SetCardBlockSizeFailed;
    }

    /* switch to host support speed mode, then switch MMC data bus width and select power class */
    if (kStatus_Success != MMC_SelectBusTiming(card))
    {
        return kStatus_SDMMC_SwitchBusTimingFailed;
    }

    /* switch power class */
    if (kStatus_Success != MMC_SetPowerClass(card))
    {
        return kStatus_SDMMC_SetPowerClassFail;
    }

    /* Set to max erase unit size */
    if (kStatus_Success != MMC_SetMaxEraseUnitSize(card))
    {
        return kStatus_SDMMC_EnableHighCapacityEraseFailed;
    }

    /* Set card default to access non-boot partition */
    card->currentPartition = kMMC_AccessPartitionUserAera;

    return kStatus_Success;
}

void MMC_CardDeinit(mmc_card_t *card)
{
    assert(card);

    MMC_SelectCard(card, false);
}

status_t MMC_HostInit(mmc_card_t *card)
{
    assert(card);

    if ((!card->isHostReady) && SDMMCHOST_Init(&(card->host), NULL) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    /* set the host status flag, after the card re-plug in, don't need init host again */
    card->isHostReady = true;

    return kStatus_Success;
}

void MMC_HostDeinit(mmc_card_t *card)
{
    assert(card);

    SDMMCHOST_Deinit(&(card->host));
    /* should re-init host */
    card->isHostReady = false;
}

void MMC_HostReset(SDMMCHOST_CONFIG *host)
{
    SDMMCHOST_Reset(host->base);
}

void MMC_PowerOnCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    SDMMCHOST_PowerOnCard(base, pwr);
}

void MMC_PowerOffCard(SDMMCHOST_TYPE *base, const sdmmchost_pwr_card_t *pwr)
{
    SDMMCHOST_PowerOffCard(base, pwr);
}

status_t MMC_Init(mmc_card_t *card)
{
    assert(card);

    if (!card->isHostReady)
    {
        if (MMC_HostInit(card) != kStatus_Success)
        {
            return kStatus_SDMMC_HostNotReady;
        }
    }
    else
    {
        /* reset the host */
        MMC_HostReset(&(card->host));
    }

    /*first power off card*/
    MMC_PowerOffCard(card->host.base, card->usrParam.pwr);

    /*power on card*/
    MMC_PowerOnCard(card->host.base, card->usrParam.pwr);

    return MMC_CardInit(card);
}

void MMC_Deinit(mmc_card_t *card)
{
    assert(card);

    MMC_CardDeinit(card);
    MMC_HostDeinit(card);
}

bool MMC_CheckReadOnly(mmc_card_t *card)
{
    assert(card);

    return ((card->csd.flags & kMMC_CsdPermanentWriteProtectFlag) ||
            (card->csd.flags & kMMC_CsdTemporaryWriteProtectFlag));
}

status_t MMC_SelectPartition(mmc_card_t *card, mmc_access_partition_t partitionNumber)
{
    assert(card);

    uint8_t bootConfig;
    mmc_extended_csd_config_t extendedCsdconfig;

    bootConfig = card->extendedCsd.partitionConfig;
    bootConfig &= ~MMC_PARTITION_CONFIG_PARTITION_ACCESS_MASK;
    bootConfig |= ((uint32_t)partitionNumber << MMC_PARTITION_CONFIG_PARTITION_ACCESS_SHIFT);

    extendedCsdconfig.accessMode = kMMC_ExtendedCsdAccessModeWriteBits;
    extendedCsdconfig.ByteIndex = kMMC_ExtendedCsdIndexPartitionConfig;
    extendedCsdconfig.ByteValue = bootConfig;
    extendedCsdconfig.commandSet = kMMC_CommandSetStandard;
    if (kStatus_Success != MMC_SetExtendedCsdConfig(card, &extendedCsdconfig))
    {
        return kStatus_SDMMC_ConfigureExtendedCsdFailed;
    }

    /* Save current configuration. */
    card->extendedCsd.partitionConfig = bootConfig;
    card->currentPartition = partitionNumber;

    return kStatus_Success;
}

status_t MMC_ReadBlocks(mmc_card_t *card, uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(buffer);
    assert(blockCount);

    uint32_t blockCountOneTime; /* The block count can be erased in one time sending READ_BLOCKS command. */
    uint32_t blockDone;         /* The blocks has been read. */
    uint32_t blockLeft;         /* Left blocks to be read. */
    uint8_t *nextBuffer;
    bool dataAddrAlign = true;

    blockLeft = blockCount;
    blockDone = 0U;
    if (kStatus_Success != MMC_CheckBlockRange(card, startBlock, blockCount))
    {
        return kStatus_InvalidArgument;
    }

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

        if (kStatus_Success != MMC_Read(card, dataAddrAlign ? nextBuffer : (uint8_t *)g_sdmmc, (startBlock + blockDone),
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

status_t MMC_WriteBlocks(mmc_card_t *card, const uint8_t *buffer, uint32_t startBlock, uint32_t blockCount)
{
    assert(card);
    assert(buffer);
    assert(blockCount);

    uint32_t blockCountOneTime;
    uint32_t blockLeft;
    uint32_t blockDone;
    const uint8_t *nextBuffer;
    bool dataAddrAlign = true;

    blockLeft = blockCount;
    blockDone = 0U;

    if (kStatus_Success != MMC_CheckBlockRange(card, startBlock, blockCount))
    {
        return kStatus_InvalidArgument;
    }

    while (blockLeft)
    {
        nextBuffer = (buffer + blockDone * FSL_SDMMC_DEFAULT_BLOCK_SIZE);
        if (!card->noInteralAlign && (!dataAddrAlign || (((uint32_t)nextBuffer) & (sizeof(uint32_t) - 1U))))
        {
            blockLeft--;
            blockCountOneTime = 1U;
            memcpy((uint8_t *)&g_sdmmc, nextBuffer, FSL_SDMMC_DEFAULT_BLOCK_SIZE);
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

        if (kStatus_Success != MMC_Write(card, dataAddrAlign ? nextBuffer : (uint8_t *)g_sdmmc,
                                         (startBlock + blockDone), FSL_SDMMC_DEFAULT_BLOCK_SIZE, blockCountOneTime))
        {
            return kStatus_SDMMC_TransferFailed;
        }

        blockDone += blockCountOneTime;
        if (!card->noInteralAlign)
        {
            memset(g_sdmmc, 0U, FSL_SDMMC_DEFAULT_BLOCK_SIZE);
        }
    }

    return kStatus_Success;
}

status_t MMC_EraseGroups(mmc_card_t *card, uint32_t startGroup, uint32_t endGroup)
{
    assert(card);
    assert(card->host.transfer);

    uint32_t startGroupAddress;
    uint32_t endGroupAddress;
    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_TRANSFER content = {0};

    if (kStatus_Success != MMC_CheckEraseGroupRange(card, startGroup, endGroup))
    {
        return kStatus_InvalidArgument;
    }

    /* Wait for the card's buffer to be not full to write to improve the write performance. */
    while ((GET_SDMMCHOST_STATUS(card->host.base) & CARD_DATA0_STATUS_MASK) != CARD_DATA0_NOT_BUSY)
    {
    }

    if (kStatus_Success != MMC_WaitWriteComplete(card))
    {
        return kStatus_SDMMC_WaitWriteCompleteFailed;
    }

    /* Calculate the start group address and end group address */
    startGroupAddress = startGroup;
    endGroupAddress = endGroup;
    if (card->flags & kMMC_SupportHighCapacityFlag)
    {
        /* The implementation of a higher than 2GB of density of memory will not be backwards compatible with the
        lower densities.First of all the address argument for higher than 2GB of density of memory is changed to
        be sector address (512B sectors) instead of byte address */
        startGroupAddress = (startGroupAddress * (card->eraseGroupBlocks));
        endGroupAddress = (endGroupAddress * (card->eraseGroupBlocks));
    }
    else
    {
        /* The address unit is byte when card capacity is lower than 2GB */
        startGroupAddress = (startGroupAddress * (card->eraseGroupBlocks) * FSL_SDMMC_DEFAULT_BLOCK_SIZE);
        endGroupAddress = (endGroupAddress * (card->eraseGroupBlocks) * FSL_SDMMC_DEFAULT_BLOCK_SIZE);
    }

    /* Set the start erase group address */
    command.index = kMMC_EraseGroupStart;
    command.argument = startGroupAddress;
    command.responseType = kCARD_ResponseTypeR1;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success != MMC_Transfer(card, &content, 0U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Set the end erase group address */
    command.index = kMMC_EraseGroupEnd;
    command.argument = endGroupAddress;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success != MMC_Transfer(card, &content, 0U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    /* Start the erase process */
    command.index = kSDMMC_Erase;
    command.argument = 0U;
    command.responseType = kCARD_ResponseTypeR1b;
    command.responseErrorFlags = SDMMC_R1_ALL_ERROR_FLAG;

    content.command = &command;
    content.data = NULL;
    if (kStatus_Success != MMC_Transfer(card, &content, 0U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t MMC_SetBootConfigWP(mmc_card_t *card, uint8_t wp)
{
    assert(card);

    mmc_extended_csd_config_t extendedCsdconfig;
    extendedCsdconfig.accessMode = kMMC_ExtendedCsdAccessModeWriteBits;
    extendedCsdconfig.ByteIndex = kMMC_ExtendedCsdIndexBootConfigWP;
    extendedCsdconfig.ByteValue = wp;
    extendedCsdconfig.commandSet = kMMC_CommandSetStandard;
    if (kStatus_Success != MMC_SetExtendedCsdConfig(card, &extendedCsdconfig))
    {
        return kStatus_SDMMC_ConfigureExtendedCsdFailed;
    }

    card->extendedCsd.bootConfigProtect = wp;

    return kStatus_Success;
}

status_t MMC_SetBootPartitionWP(mmc_card_t *card, mmc_boot_partition_wp_t bootPartitionWP)
{
    assert(card);

    mmc_extended_csd_config_t extendedCsdconfig;
    extendedCsdconfig.accessMode = kMMC_ExtendedCsdAccessModeWriteBits;
    extendedCsdconfig.ByteIndex = kMMC_ExtendedCsdIndexBootPartitionWP;
    extendedCsdconfig.ByteValue = bootPartitionWP;
    extendedCsdconfig.commandSet = kMMC_CommandSetStandard;
    if (kStatus_Success != MMC_SetExtendedCsdConfig(card, &extendedCsdconfig))
    {
        return kStatus_SDMMC_ConfigureExtendedCsdFailed;
    }

    card->extendedCsd.bootPartitionWP = bootPartitionWP;

    return kStatus_Success;
}

status_t MMC_SetBootConfig(mmc_card_t *card, const mmc_boot_config_t *config)
{
    assert(card);
    assert(config);

    uint8_t bootParameter;
    uint8_t bootBusWidth = config->bootDataBusWidth;
    mmc_extended_csd_config_t extendedCsdconfig;

    if (card->extendedCsd.extendecCsdVersion <=
        kMMC_ExtendedCsdRevision13) /* V4.3 or above version card support boot mode */
    {
        return kStatus_SDMMC_NotSupportYet;
    }

    /* Set the BOOT_CONFIG field of Extended CSD */
    bootParameter = card->extendedCsd.partitionConfig;
    bootParameter &= ~(MMC_PARTITION_CONFIG_BOOT_ACK_MASK | MMC_PARTITION_CONFIG_PARTITION_ENABLE_MASK);
    bootParameter |= ((config->enableBootAck ? 1U : 0U) << MMC_PARTITION_CONFIG_BOOT_ACK_SHIFT);
    bootParameter |= ((uint32_t)(config->bootPartition) << MMC_PARTITION_CONFIG_PARTITION_ENABLE_SHIFT);

    extendedCsdconfig.accessMode = kMMC_ExtendedCsdAccessModeWriteBits;
    extendedCsdconfig.ByteIndex = kMMC_ExtendedCsdIndexPartitionConfig;
    extendedCsdconfig.ByteValue = bootParameter;
    extendedCsdconfig.commandSet = kMMC_CommandSetStandard;
    if (kStatus_Success != MMC_SetExtendedCsdConfig(card, &extendedCsdconfig))
    {
        return kStatus_SDMMC_ConfigureExtendedCsdFailed;
    }

    card->extendedCsd.partitionConfig = bootParameter;

    /* data bus remapping */
    if (bootBusWidth == kMMC_DataBusWidth1bit)
    {
        bootBusWidth = 0U;
    }
    else if ((bootBusWidth == kMMC_DataBusWidth4bit) || (bootBusWidth == kMMC_DataBusWidth4bitDDR))
    {
        bootBusWidth = 1U;
    }
    else
    {
        bootBusWidth = 2U;
    }

    /*Set BOOT_BUS_CONDITIONS in Extended CSD */
    bootParameter = card->extendedCsd.bootDataBusConditions;
    bootParameter &= ~(MMC_BOOT_BUS_CONDITION_RESET_BUS_CONDITION_MASK | MMC_BOOT_BUS_CONDITION_BUS_WIDTH_MASK |
                       MMC_BOOT_BUS_CONDITION_BOOT_MODE_MASK);
    bootParameter |=
        ((config->retainBootbusCondition ? true : false) << MMC_BOOT_BUS_CONDITION_RESET_BUS_CONDITION_SHIFT);
    bootParameter |= bootBusWidth << MMC_BOOT_BUS_CONDITION_BUS_WIDTH_SHIFT;
    bootParameter |= (uint32_t)(config->bootTimingMode);

    extendedCsdconfig.accessMode = kMMC_ExtendedCsdAccessModeWriteBits;
    extendedCsdconfig.ByteIndex = kMMC_ExtendedCsdIndexBootBusConditions;
    extendedCsdconfig.ByteValue = bootParameter;
    if (kStatus_Success != MMC_SetExtendedCsdConfig(card, &extendedCsdconfig))
    {
        return kStatus_SDMMC_ConfigureBootFailed;
    }

    card->extendedCsd.bootDataBusConditions = bootParameter;
    /* check and configure the boot config write protect */
    bootParameter = config->pwrBootConfigProtection | ((uint8_t)(config->premBootConfigProtection) << 4U);
    if (bootParameter != (card->extendedCsd.bootConfigProtect))
    {
        if (kStatus_Success != MMC_SetBootConfigWP(card, bootParameter))
        {
            return kStatus_SDMMC_ConfigureBootFailed;
        }
    }
    /* check and configure the boot partition write protect */
    if (card->extendedCsd.bootPartitionWP != (uint8_t)(config->bootPartitionWP))
    {
        if (kStatus_Success != MMC_SetBootPartitionWP(card, config->bootPartitionWP))
        {
            return kStatus_SDMMC_ConfigureBootFailed;
        }
    }

    return kStatus_Success;
}

status_t MMC_StartBoot(mmc_card_t *card,
                       const mmc_boot_config_t *mmcConfig,
                       uint8_t *buffer,
                       SDMMCHOST_BOOT_CONFIG *hostConfig)
{
    assert(card);
    assert(mmcConfig);
    assert(buffer);

    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_DATA data = {0};
    uint32_t tempClock = 0U;

    if (!card->isHostReady)
    {
        return kStatus_Fail;
    }

    /* send card active */
    SDMMCHOST_SEND_CARD_ACTIVE(card->host.base, 100U);
    /* config the host */
    SDMMCHOST_SETMMCBOOTCONFIG(card->host.base, hostConfig);
    /* enable MMC boot */
    SDMMCHOST_ENABLE_MMC_BOOT(card->host.base, true);

    if (mmcConfig->bootTimingMode == kMMC_BootModeSDRWithDefaultTiming)
    {
        /* Set clock to 400KHz. */
        tempClock = SDMMC_CLOCK_400KHZ;
    }
    else
    {
        /* Set clock to 52MHZ. */
        tempClock = MMC_CLOCK_52MHZ;
    }
    SDMMCHOST_SET_CARD_CLOCK(card->host.base, card->host.sourceClock_Hz, tempClock);

    if (mmcConfig->bootTimingMode == kMMC_BootModeDDRTiming)
    {
        /* enable DDR mode */
        SDMMCHOST_ENABLE_DDR_MODE(card->host.base, true, 0U);
    }

    /* data bus remapping */
    if (mmcConfig->bootDataBusWidth == kMMC_DataBusWidth1bit)
    {
        SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH1BIT);
    }
    else if ((mmcConfig->bootDataBusWidth == kMMC_DataBusWidth4bit) ||
             (mmcConfig->bootDataBusWidth == kMMC_DataBusWidth4bitDDR))
    {
        SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH4BIT);
    }
    else
    {
        SDMMCHOST_SET_CARD_BUS_WIDTH(card->host.base, kSDMMCHOST_DATABUSWIDTH8BIT);
    }

    if (kMMC_BootModeAlternative == (uint32_t)SDMMCHOST_GET_HOST_CONFIG_BOOT_MODE(hostConfig))
    {
        /* alternative boot mode */
        command.argument = 0xFFFFFFFA;
    }

    command.index = kSDMMC_GoIdleState;

    data.blockSize = SDMMCHOST_GET_HOST_CONFIG_BLOCK_SIZE(hostConfig);
    data.blockCount = SDMMCHOST_GET_HOST_CONFIG_BLOCK_COUNT(hostConfig);
    data.rxData = (uint32_t *)buffer;
    SDMMCHOST_ENABLE_BOOT_FLAG(data);

    content.data = &data;
    content.command = &command;

    /* should check tuning error during every transfer*/
    if (kStatus_Success != MMC_Transfer(card, &content, 1U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t MMC_ReadBootData(mmc_card_t *card, uint8_t *buffer, SDMMCHOST_BOOT_CONFIG *hostConfig)
{
    assert(card);
    assert(buffer);

    SDMMCHOST_COMMAND command = {0};
    SDMMCHOST_TRANSFER content = {0};
    SDMMCHOST_DATA data = {0};

    /* enable MMC boot */
    SDMMCHOST_ENABLE_MMC_BOOT(card->host.base, true);
    /* config the host */
    SDMMCHOST_SETMMCBOOTCONFIG(card->host.base, hostConfig);
    data.blockSize = SDMMCHOST_GET_HOST_CONFIG_BLOCK_SIZE(hostConfig);
    data.blockCount = SDMMCHOST_GET_HOST_CONFIG_BLOCK_COUNT(hostConfig);
    data.rxData = (uint32_t *)buffer;
    SDMMCHOST_ENABLE_BOOT_CONTINOUS_FLAG(data);
    /* no command should be send out  */
    SDMMCHOST_EMPTY_CMD_FLAG(command);

    content.data = &data;
    content.command = &command;

    /* should check tuning error during every transfer*/
    if (kStatus_Success != MMC_Transfer(card, &content, 1U))
    {
        return kStatus_SDMMC_TransferFailed;
    }

    return kStatus_Success;
}

status_t MMC_StopBoot(mmc_card_t *card, uint32_t bootMode)
{
    assert(card);
    /* Disable boot mode */
    if (kMMC_BootModeAlternative == bootMode)
    {
        /* Send CMD0 to reset the bus */
        if (kStatus_Success != MMC_GoIdle(card))
        {
            return kStatus_SDMMC_GoIdleFailed;
        }
    }
    /* disable MMC boot */
    SDMMCHOST_ENABLE_MMC_BOOT(card->host.base, false);

    return kStatus_Success;
}
