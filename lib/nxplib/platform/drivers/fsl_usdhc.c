/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_usdhc.h"
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
#include "fsl_cache.h"
#endif /* FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.usdhc"
#endif

/*! @brief Clock setting */
/* Max SD clock divisor from base clock */
#define USDHC_MAX_DVS ((USDHC_SYS_CTRL_DVS_MASK >> USDHC_SYS_CTRL_DVS_SHIFT) + 1U)
#define USDHC_MAX_CLKFS ((USDHC_SYS_CTRL_SDCLKFS_MASK >> USDHC_SYS_CTRL_SDCLKFS_SHIFT) + 1U)
#define USDHC_PREV_DVS(x) ((x) -= 1U)
#define USDHC_PREV_CLKFS(x, y) ((x) >>= (y))
/*! @brief USDHC ADMA table address align size */
#define USDHC_ADMA_TABLE_ADDRESS_ALIGN (4U)

/* Typedef for interrupt handler. */
typedef void (*usdhc_isr_t)(USDHC_Type *base, usdhc_handle_t *handle);
/*! @brief Dummy data buffer for mmc boot mode  */
AT_NONCACHEABLE_SECTION_ALIGN(uint32_t s_usdhcBootDummy, USDHC_ADMA2_ADDRESS_ALIGN);
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the instance.
 *
 * @param base USDHC peripheral base address.
 * @return Instance number.
 */
static uint32_t USDHC_GetInstance(USDHC_Type *base);

/*!
 * @brief Set transfer interrupt.
 *
 * @param base USDHC peripheral base address.
 * @param usingInterruptSignal True to use IRQ signal.
 */
static void USDHC_SetTransferInterrupt(USDHC_Type *base, bool usingInterruptSignal);

/*!
 * @brief Start transfer according to current transfer state
 *
 * @param base USDHC peripheral base address.
 * @param data Data to be transferred.
 * @param flag data present flag
 * @param enDMA DMA enable flag
 */
static status_t USDHC_SetDataTransferConfig(USDHC_Type *base,
                                            usdhc_data_t *data,
                                            uint32_t *dataPresentFlag,
                                            bool enDMA);

/*!
 * @brief Receive command response
 *
 * @param base USDHC peripheral base address.
 * @param command Command to be sent.
 */
static status_t USDHC_ReceiveCommandResponse(USDHC_Type *base, usdhc_command_t *command);

/*!
 * @brief Read DATAPORT when buffer enable bit is set.
 *
 * @param base USDHC peripheral base address.
 * @param data Data to be read.
 * @param transferredWords The number of data words have been transferred last time transaction.
 * @return The number of total data words have been transferred after this time transaction.
 */
static uint32_t USDHC_ReadDataPort(USDHC_Type *base, usdhc_data_t *data, uint32_t transferredWords);

/*!
 * @brief Read data by using DATAPORT polling way.
 *
 * @param base USDHC peripheral base address.
 * @param data Data to be read.
 * @retval kStatus_Fail Read DATAPORT failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t USDHC_ReadByDataPortBlocking(USDHC_Type *base, usdhc_data_t *data);

/*!
 * @brief Write DATAPORT when buffer enable bit is set.
 *
 * @param base USDHC peripheral base address.
 * @param data Data to be read.
 * @param transferredWords The number of data words have been transferred last time.
 * @return The number of total data words have been transferred after this time transaction.
 */
static uint32_t USDHC_WriteDataPort(USDHC_Type *base, usdhc_data_t *data, uint32_t transferredWords);

/*!
 * @brief Write data by using DATAPORT polling way.
 *
 * @param base USDHC peripheral base address.
 * @param data Data to be transferred.
 * @retval kStatus_Fail Write DATAPORT failed.
 * @retval kStatus_Success Operate successfully.
 */
static status_t USDHC_WriteByDataPortBlocking(USDHC_Type *base, usdhc_data_t *data);

/*!
 * @brief Transfer data by polling way.
 *
 * @param base USDHC peripheral base address.
 * @param data Data to be transferred.
 * @param use DMA flag.
 * @retval kStatus_Fail Transfer data failed.
 * @retval kStatus_InvalidArgument Argument is invalid.
 * @retval kStatus_Success Operate successfully.
 */
static status_t USDHC_TransferDataBlocking(USDHC_Type *base, usdhc_data_t *data, bool enDMA);

/*!
 * @brief Handle card detect interrupt.
 *
 * @param base USDHC peripheral base address.
 * @param handle USDHC handle.
 * @param interruptFlags Card detect related interrupt flags.
 */
static void USDHC_TransferHandleCardDetect(USDHC_Type *base, usdhc_handle_t *handle, uint32_t interruptFlags);

/*!
 * @brief Handle command interrupt.
 *
 * @param base USDHC peripheral base address.
 * @param handle USDHC handle.
 * @param interruptFlags Command related interrupt flags.
 */
static void USDHC_TransferHandleCommand(USDHC_Type *base, usdhc_handle_t *handle, uint32_t interruptFlags);

/*!
 * @brief Handle data interrupt.
 *
 * @param base USDHC peripheral base address.
 * @param handle USDHC handle.
 * @param interruptFlags Data related interrupt flags.
 */
static void USDHC_TransferHandleData(USDHC_Type *base, usdhc_handle_t *handle, uint32_t interruptFlags);

/*!
 * @brief Handle SDIO card interrupt signal.
 *
 * @param base USDHC peripheral base address.
 * @param handle USDHC handle.
 */
static void USDHC_TransferHandleSdioInterrupt(USDHC_Type *base, usdhc_handle_t *handle);

/*!
 * @brief Handle SDIO block gap event.
 *
 * @param base USDHC peripheral base address.
 * @param handle USDHC handle.
 */
static void USDHC_TransferHandleBlockGap(USDHC_Type *base, usdhc_handle_t *handle);

/*!
 * @brief Handle retuning
 *
 * @param base USDHC peripheral base address.
 * @param handle USDHC handle.
 * @param interrupt flags
 */
static void USDHC_TransferHandleReTuning(USDHC_Type *base, usdhc_handle_t *handle, uint32_t interruptFlags);

/*!
 * @brief wait command done
 *
 * @param base USDHC peripheral base address.
 * @param command configuration
 * @param pollingCmdDone polling command done flag
 */
static status_t USDHC_WaitCommandDone(USDHC_Type *base, usdhc_command_t *command, bool pollingCmdDone);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief USDHC base pointer array */
static USDHC_Type *const s_usdhcBase[] = USDHC_BASE_PTRS;

/*! @brief USDHC internal handle pointer array */
static usdhc_handle_t *s_usdhcHandle[ARRAY_SIZE(s_usdhcBase)] = {NULL};

/*! @brief USDHC IRQ name array */
static const IRQn_Type s_usdhcIRQ[] = USDHC_IRQS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief USDHC clock array name */
static const clock_ip_name_t s_usdhcClock[] = USDHC_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/* USDHC ISR for transactional APIs. */
static usdhc_isr_t s_usdhcIsr;

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t USDHC_GetInstance(USDHC_Type *base)
{
    uint8_t instance = 0;

    while ((instance < ARRAY_SIZE(s_usdhcBase)) && (s_usdhcBase[instance] != base))
    {
        instance++;
    }

    assert(instance < ARRAY_SIZE(s_usdhcBase));

    return instance;
}

static void USDHC_SetTransferInterrupt(USDHC_Type *base, bool usingInterruptSignal)
{
    uint32_t interruptEnabled; /* The Interrupt status flags to be enabled */

    /* Disable all interrupts */
    USDHC_DisableInterruptStatus(base, (uint32_t)kUSDHC_AllInterruptFlags);
    USDHC_DisableInterruptSignal(base, (uint32_t)kUSDHC_AllInterruptFlags);
    DisableIRQ(s_usdhcIRQ[USDHC_GetInstance(base)]);

    interruptEnabled = (kUSDHC_CommandFlag | kUSDHC_CardInsertionFlag | kUSDHC_DataFlag | kUSDHC_CardRemovalFlag |
                        kUSDHC_SDR104TuningFlag | kUSDHC_BlockGapEventFlag);

    USDHC_EnableInterruptStatus(base, interruptEnabled);

    if (usingInterruptSignal)
    {
        USDHC_EnableInterruptSignal(base, interruptEnabled);
    }
}

static status_t USDHC_SetDataTransferConfig(USDHC_Type *base, usdhc_data_t *data, uint32_t *dataPresentFlag, bool enDMA)
{
    uint32_t mixCtrl = base->MIX_CTRL;

    if (data != NULL)
    {
        /* if transfer boot continous, only need set the CREQ bit, leave others as it is */
        if (data->dataType == kUSDHC_TransferDataBootcontinous)
        {
            /* clear stop at block gap request */
            base->PROT_CTRL &= ~USDHC_PROT_CTRL_SABGREQ_MASK;
            /* continous transfer data */
            base->PROT_CTRL |= USDHC_PROT_CTRL_CREQ_MASK;
            return kStatus_Success;
        }

        /* check data inhibit flag */
        if (base->PRES_STATE & kUSDHC_DataInhibitFlag)
        {
            return kStatus_USDHC_BusyTransferring;
        }
        /* check transfer block count */
        if ((data->blockCount > USDHC_MAX_BLOCK_COUNT) || ((data->txData == NULL) && (data->rxData == NULL)))
        {
            return kStatus_InvalidArgument;
        }

        /* config mix parameter */
        mixCtrl &= ~(USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK | USDHC_MIX_CTRL_DTDSEL_MASK |
                     USDHC_MIX_CTRL_AC12EN_MASK);

        if (data->rxData)
        {
            mixCtrl |= USDHC_MIX_CTRL_DTDSEL_MASK;
        }
        if (data->blockCount > 1U)
        {
            mixCtrl |= USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK;
            /* auto command 12 */
            if (data->enableAutoCommand12)
            {
                mixCtrl |= USDHC_MIX_CTRL_AC12EN_MASK;
            }
        }

        /* auto command 23, auto send set block count cmd before multiple read/write */
        if ((data->enableAutoCommand23))
        {
            mixCtrl |= USDHC_MIX_CTRL_AC23EN_MASK;
            base->VEND_SPEC2 |= USDHC_VEND_SPEC2_ACMD23_ARGU2_EN_MASK;
            /* config the block count to DS_ADDR */
            base->DS_ADDR = data->blockCount;
        }
        else
        {
            mixCtrl &= ~USDHC_MIX_CTRL_AC23EN_MASK;
            base->VEND_SPEC2 &= ~USDHC_VEND_SPEC2_ACMD23_ARGU2_EN_MASK;
        }

        /* if transfer boot data, leave the block count to USDHC_SetMmcBootConfig function */
        if (data->dataType != kUSDHC_TransferDataBoot)
        {
            /* config data block size/block count */
            base->BLK_ATT = ((base->BLK_ATT & ~(USDHC_BLK_ATT_BLKSIZE_MASK | USDHC_BLK_ATT_BLKCNT_MASK)) |
                             (USDHC_BLK_ATT_BLKSIZE(data->blockSize) | USDHC_BLK_ATT_BLKCNT(data->blockCount)));
        }
        else
        {
            mixCtrl |= USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK;
            base->PROT_CTRL |= USDHC_PROT_CTRL_RD_DONE_NO_8CLK_MASK;
        }

        /* data present flag */
        *dataPresentFlag |= kUSDHC_DataPresentFlag;
        /* Disable useless interrupt */
        if (enDMA)
        {
            base->INT_SIGNAL_EN &= ~(kUSDHC_BufferWriteReadyFlag | kUSDHC_BufferReadReadyFlag | kUSDHC_DmaCompleteFlag);
            base->INT_STATUS_EN &= ~(kUSDHC_BufferWriteReadyFlag | kUSDHC_BufferReadReadyFlag | kUSDHC_DmaCompleteFlag);
        }
        else
        {
            base->INT_SIGNAL_EN |= kUSDHC_BufferWriteReadyFlag | kUSDHC_BufferReadReadyFlag;
            base->INT_STATUS_EN |= kUSDHC_BufferWriteReadyFlag | kUSDHC_BufferReadReadyFlag;
        }
    }
    else
    {
        /* clear data flags */
        mixCtrl &= ~(USDHC_MIX_CTRL_MSBSEL_MASK | USDHC_MIX_CTRL_BCEN_MASK | USDHC_MIX_CTRL_DTDSEL_MASK |
                     USDHC_MIX_CTRL_AC12EN_MASK | USDHC_MIX_CTRL_AC23EN_MASK);

        if (base->PRES_STATE & kUSDHC_CommandInhibitFlag)
        {
            return kStatus_USDHC_BusyTransferring;
        }
    }

    /* config the mix parameter */
    base->MIX_CTRL = mixCtrl;

    return kStatus_Success;
}

static status_t USDHC_ReceiveCommandResponse(USDHC_Type *base, usdhc_command_t *command)
{
    assert(command != NULL);

    if (command->responseType != kCARD_ResponseTypeNone)
    {
        command->response[0U] = base->CMD_RSP0;
        if (command->responseType == kCARD_ResponseTypeR2)
        {
            /* R3-R2-R1-R0(lowest 8 bit is invalid bit) has the same format as R2 format in SD specification document
            after removed internal CRC7 and end bit. */
            command->response[0U] <<= 8U;
            command->response[1U] = (base->CMD_RSP1 << 8U) | ((base->CMD_RSP0 & 0xFF000000U) >> 24U);
            command->response[2U] = (base->CMD_RSP2 << 8U) | ((base->CMD_RSP1 & 0xFF000000U) >> 24U);
            command->response[3U] = (base->CMD_RSP3 << 8U) | ((base->CMD_RSP2 & 0xFF000000U) >> 24U);
        }
    }

    /* check response error flag */
    if ((command->responseErrorFlags != 0U) &&
        ((command->responseType == kCARD_ResponseTypeR1) || (command->responseType == kCARD_ResponseTypeR1b) ||
         (command->responseType == kCARD_ResponseTypeR6) || (command->responseType == kCARD_ResponseTypeR5)))
    {
        if (((command->responseErrorFlags) & (command->response[0U])) != 0U)
        {
            return kStatus_USDHC_SendCommandFailed;
        }
    }

    return kStatus_Success;
}

static uint32_t USDHC_ReadDataPort(USDHC_Type *base, usdhc_data_t *data, uint32_t transferredWords)
{
    uint32_t i;
    uint32_t totalWords;
    uint32_t wordsCanBeRead; /* The words can be read at this time. */
    uint32_t readWatermark = ((base->WTMK_LVL & USDHC_WTMK_LVL_RD_WML_MASK) >> USDHC_WTMK_LVL_RD_WML_SHIFT);

    /* If DMA is enable, do not need to polling data port */
    if ((base->MIX_CTRL & USDHC_MIX_CTRL_DMAEN_MASK) == 0U)
    {
        /*
         * Add non aligned access support ,user need make sure your buffer size is big
         * enough to hold the data,in other words,user need make sure the buffer size
         * is 4 byte aligned
         */
        if (data->blockSize % sizeof(uint32_t) != 0U)
        {
            data->blockSize +=
                sizeof(uint32_t) - (data->blockSize % sizeof(uint32_t)); /* make the block size as word-aligned */
        }

        totalWords = ((data->blockCount * data->blockSize) / sizeof(uint32_t));

        /* If watermark level is equal or bigger than totalWords, transfers totalWords data. */
        if (readWatermark >= totalWords)
        {
            wordsCanBeRead = totalWords;
        }
        /* If watermark level is less than totalWords and left words to be sent is equal or bigger than readWatermark,
        transfers watermark level words. */
        else if ((readWatermark < totalWords) && ((totalWords - transferredWords) >= readWatermark))
        {
            wordsCanBeRead = readWatermark;
        }
        /* If watermark level is less than totalWords and left words to be sent is less than readWatermark, transfers
        left
        words. */
        else
        {
            wordsCanBeRead = (totalWords - transferredWords);
        }

        i = 0U;
        while (i < wordsCanBeRead)
        {
            data->rxData[transferredWords++] = USDHC_ReadData(base);
            i++;
        }
    }

    return transferredWords;
}

static status_t USDHC_ReadByDataPortBlocking(USDHC_Type *base, usdhc_data_t *data)
{
    uint32_t totalWords;
    uint32_t transferredWords = 0U, interruptStatus = 0U;
    status_t error = kStatus_Success;

    /*
     * Add non aligned access support ,user need make sure your buffer size is big
     * enough to hold the data,in other words,user need make sure the buffer size
     * is 4 byte aligned
     */
    if (data->blockSize % sizeof(uint32_t) != 0U)
    {
        data->blockSize +=
            sizeof(uint32_t) - (data->blockSize % sizeof(uint32_t)); /* make the block size as word-aligned */
    }

    totalWords = ((data->blockCount * data->blockSize) / sizeof(uint32_t));

    while ((error == kStatus_Success) && (transferredWords < totalWords))
    {
        while (!(interruptStatus & (kUSDHC_BufferReadReadyFlag | kUSDHC_DataErrorFlag | kUSDHC_TuningErrorFlag)))
        {
            interruptStatus = USDHC_GetInterruptStatusFlags(base);
        }

        /* during std tuning process, software do not need to read data, but wait BRR is enough */
        if ((data->dataType == kUSDHC_TransferDataTuning) && (interruptStatus & kUSDHC_BufferReadReadyFlag))
        {
            USDHC_ClearInterruptStatusFlags(base, kUSDHC_BufferReadReadyFlag | kUSDHC_TuningPassFlag);

            return kStatus_Success;
        }
        else if ((interruptStatus & kUSDHC_TuningErrorFlag) != 0U)
        {
            USDHC_ClearInterruptStatusFlags(base, kUSDHC_TuningErrorFlag);
            /* if tuning error occur ,return directly */
            error = kStatus_USDHC_TuningError;
        }
        else if ((interruptStatus & kUSDHC_DataErrorFlag) != 0U)
        {
            if (!(data->enableIgnoreError))
            {
                error = kStatus_Fail;
            }
            /* clear data error flag */
            USDHC_ClearInterruptStatusFlags(base, kUSDHC_DataErrorFlag);
        }
        else
        {
        }

        if (error == kStatus_Success)
        {
            transferredWords = USDHC_ReadDataPort(base, data, transferredWords);
            /* clear buffer read ready */
            USDHC_ClearInterruptStatusFlags(base, kUSDHC_BufferReadReadyFlag);
            interruptStatus = 0U;
        }
    }

    /* Clear data complete flag after the last read operation. */
    USDHC_ClearInterruptStatusFlags(base, kUSDHC_DataCompleteFlag);

    return error;
}

static uint32_t USDHC_WriteDataPort(USDHC_Type *base, usdhc_data_t *data, uint32_t transferredWords)
{
    uint32_t i;
    uint32_t totalWords;
    uint32_t wordsCanBeWrote; /* Words can be wrote at this time. */
    uint32_t writeWatermark = ((base->WTMK_LVL & USDHC_WTMK_LVL_WR_WML_MASK) >> USDHC_WTMK_LVL_WR_WML_SHIFT);

    /* If DMA is enable, do not need to polling data port */
    if ((base->MIX_CTRL & USDHC_MIX_CTRL_DMAEN_MASK) == 0U)
    {
        /*
         * Add non aligned access support ,user need make sure your buffer size is big
         * enough to hold the data,in other words,user need make sure the buffer size
         * is 4 byte aligned
         */
        if (data->blockSize % sizeof(uint32_t) != 0U)
        {
            data->blockSize +=
                sizeof(uint32_t) - (data->blockSize % sizeof(uint32_t)); /* make the block size as word-aligned */
        }

        totalWords = ((data->blockCount * data->blockSize) / sizeof(uint32_t));

        /* If watermark level is equal or bigger than totalWords, transfers totalWords data.*/
        if (writeWatermark >= totalWords)
        {
            wordsCanBeWrote = totalWords;
        }
        /* If watermark level is less than totalWords and left words to be sent is equal or bigger than watermark,
        transfers watermark level words. */
        else if ((writeWatermark < totalWords) && ((totalWords - transferredWords) >= writeWatermark))
        {
            wordsCanBeWrote = writeWatermark;
        }
        /* If watermark level is less than totalWords and left words to be sent is less than watermark, transfers left
        words. */
        else
        {
            wordsCanBeWrote = (totalWords - transferredWords);
        }

        i = 0U;
        while (i < wordsCanBeWrote)
        {
            USDHC_WriteData(base, data->txData[transferredWords++]);
            i++;
        }
    }

    return transferredWords;
}

static status_t USDHC_WriteByDataPortBlocking(USDHC_Type *base, usdhc_data_t *data)
{
    uint32_t totalWords;

    uint32_t transferredWords = 0U, interruptStatus = 0U;
    status_t error = kStatus_Success;

    /*
     * Add non aligned access support ,user need make sure your buffer size is big
     * enough to hold the data,in other words,user need make sure the buffer size
     * is 4 byte aligned
     */
    if (data->blockSize % sizeof(uint32_t) != 0U)
    {
        data->blockSize +=
            sizeof(uint32_t) - (data->blockSize % sizeof(uint32_t)); /* make the block size as word-aligned */
    }

    totalWords = (data->blockCount * data->blockSize) / sizeof(uint32_t);

    while ((error == kStatus_Success) && (transferredWords < totalWords))
    {
        while (!(interruptStatus & (kUSDHC_BufferWriteReadyFlag | kUSDHC_DataErrorFlag | kUSDHC_TuningErrorFlag)))
        {
            interruptStatus = USDHC_GetInterruptStatusFlags(base);
        }

        if ((interruptStatus & kUSDHC_TuningErrorFlag) != 0U)
        {
            USDHC_ClearInterruptStatusFlags(base, kUSDHC_TuningErrorFlag);
            /* if tuning error occur ,return directly */
            return kStatus_USDHC_TuningError;
        }
        else if ((interruptStatus & kUSDHC_DataErrorFlag) != 0U)
        {
            if (!(data->enableIgnoreError))
            {
                error = kStatus_Fail;
            }
            /* clear data error flag */
            USDHC_ClearInterruptStatusFlags(base, kUSDHC_DataErrorFlag);
        }
        else
        {
        }

        if (error == kStatus_Success)
        {
            transferredWords = USDHC_WriteDataPort(base, data, transferredWords);
            /* clear buffer write ready */
            USDHC_ClearInterruptStatusFlags(base, kUSDHC_BufferWriteReadyFlag);
            interruptStatus = 0U;
        }
    }

    /* Wait write data complete or data transfer error after the last writing operation. */
    while (!(interruptStatus & (kUSDHC_DataCompleteFlag | kUSDHC_DataErrorFlag)))
    {
        interruptStatus = USDHC_GetInterruptStatusFlags(base);
    }

    if ((interruptStatus & kUSDHC_DataErrorFlag) != 0U)
    {
        if (!(data->enableIgnoreError))
        {
            error = kStatus_Fail;
        }
    }
    USDHC_ClearInterruptStatusFlags(base, (kUSDHC_DataCompleteFlag | kUSDHC_DataErrorFlag));

    return error;
}

/*!
 * brief send command function
 *
 * param base USDHC peripheral base address.
 * param command configuration
 */
void USDHC_SendCommand(USDHC_Type *base, usdhc_command_t *command)
{
    assert(NULL != command);

    uint32_t xferType = base->CMD_XFR_TYP, flags = command->flags;

    if (((base->PRES_STATE & kUSDHC_CommandInhibitFlag) == 0U) && (command->type != kCARD_CommandTypeEmpty))
    {
        /* Define the flag corresponding to each response type. */
        switch (command->responseType)
        {
            case kCARD_ResponseTypeNone:
                break;
            case kCARD_ResponseTypeR1: /* Response 1 */
            case kCARD_ResponseTypeR5: /* Response 5 */
            case kCARD_ResponseTypeR6: /* Response 6 */
            case kCARD_ResponseTypeR7: /* Response 7 */
                flags |= (kUSDHC_ResponseLength48Flag | kUSDHC_EnableCrcCheckFlag | kUSDHC_EnableIndexCheckFlag);
                break;

            case kCARD_ResponseTypeR1b: /* Response 1 with busy */
            case kCARD_ResponseTypeR5b: /* Response 5 with busy */
                flags |= (kUSDHC_ResponseLength48BusyFlag | kUSDHC_EnableCrcCheckFlag | kUSDHC_EnableIndexCheckFlag);
                break;

            case kCARD_ResponseTypeR2: /* Response 2 */
                flags |= (kUSDHC_ResponseLength136Flag | kUSDHC_EnableCrcCheckFlag);
                break;

            case kCARD_ResponseTypeR3: /* Response 3 */
            case kCARD_ResponseTypeR4: /* Response 4 */
                flags |= (kUSDHC_ResponseLength48Flag);
                break;

            default:
                break;
        }

        if (command->type == kCARD_CommandTypeAbort)
        {
            flags |= kUSDHC_CommandTypeAbortFlag;
        }

        /* config cmd index */
        xferType &= ~(USDHC_CMD_XFR_TYP_CMDINX_MASK | USDHC_CMD_XFR_TYP_CMDTYP_MASK | USDHC_CMD_XFR_TYP_CICEN_MASK |
                      USDHC_CMD_XFR_TYP_CCCEN_MASK | USDHC_CMD_XFR_TYP_RSPTYP_MASK | USDHC_CMD_XFR_TYP_DPSEL_MASK);

        xferType |=
            (((command->index << USDHC_CMD_XFR_TYP_CMDINX_SHIFT) & USDHC_CMD_XFR_TYP_CMDINX_MASK) |
             ((flags) & (USDHC_CMD_XFR_TYP_CMDTYP_MASK | USDHC_CMD_XFR_TYP_CICEN_MASK | USDHC_CMD_XFR_TYP_CCCEN_MASK |
                         USDHC_CMD_XFR_TYP_RSPTYP_MASK | USDHC_CMD_XFR_TYP_DPSEL_MASK)));

        /* config the command xfertype and argument */
        base->CMD_ARG     = command->argument;
        base->CMD_XFR_TYP = xferType;
    }

    if (command->type == kCARD_CommandTypeEmpty)
    {
        /* disable CMD done interrupt for empty command */
        base->INT_SIGNAL_EN &= ~USDHC_INT_SIGNAL_EN_CCIEN_MASK;
    }
}

static status_t USDHC_WaitCommandDone(USDHC_Type *base, usdhc_command_t *command, bool pollingCmdDone)
{
    assert(NULL != command);

    status_t error           = kStatus_Success;
    uint32_t interruptStatus = 0U;
    /* check if need polling command done or not */
    if (pollingCmdDone)
    {
        /* Wait command complete or USDHC encounters error. */
        while (!(interruptStatus & (kUSDHC_CommandCompleteFlag | kUSDHC_CommandErrorFlag)))
        {
            interruptStatus = USDHC_GetInterruptStatusFlags(base);
        }

        if ((interruptStatus & kUSDHC_TuningErrorFlag) != 0U)
        {
            error = kStatus_USDHC_TuningError;
        }
        else if ((interruptStatus & kUSDHC_CommandErrorFlag) != 0U)
        {
            error = kStatus_Fail;
        }
        else
        {
        }
        /* Receive response when command completes successfully. */
        if (error == kStatus_Success)
        {
            error = USDHC_ReceiveCommandResponse(base, command);
        }

        USDHC_ClearInterruptStatusFlags(
            base, (kUSDHC_CommandCompleteFlag | kUSDHC_CommandErrorFlag | kUSDHC_TuningErrorFlag));
    }

    return error;
}

static status_t USDHC_TransferDataBlocking(USDHC_Type *base, usdhc_data_t *data, bool enDMA)
{
    status_t error           = kStatus_Success;
    uint32_t interruptStatus = 0U;

    if (enDMA)
    {
        /* Wait data complete or USDHC encounters error. */
        while (!((interruptStatus &
                  (kUSDHC_DataCompleteFlag | kUSDHC_DataErrorFlag | kUSDHC_DmaErrorFlag | kUSDHC_TuningErrorFlag))))
        {
            interruptStatus = USDHC_GetInterruptStatusFlags(base);
        }

        if ((interruptStatus & kUSDHC_TuningErrorFlag) != 0U)
        {
            error = kStatus_USDHC_TuningError;
        }
        else if (((interruptStatus & (kUSDHC_DataErrorFlag | kUSDHC_DmaErrorFlag)) != 0U))
        {
            if ((!(data->enableIgnoreError)) || (interruptStatus & kUSDHC_DataTimeoutFlag))
            {
                error = kStatus_Fail;
            }
        }
        else
        {
        }
        /* load dummy data */
        if ((data->dataType == kUSDHC_TransferDataBootcontinous) && (error == kStatus_Success))
        {
            *(data->rxData) = s_usdhcBootDummy;
        }

        USDHC_ClearInterruptStatusFlags(base, (kUSDHC_DataCompleteFlag | kUSDHC_DataErrorFlag | kUSDHC_DmaErrorFlag |
                                               kUSDHC_TuningPassFlag | kUSDHC_TuningErrorFlag));
    }
    else
    {
        if (data->rxData)
        {
            error = USDHC_ReadByDataPortBlocking(base, data);
        }
        else
        {
            error = USDHC_WriteByDataPortBlocking(base, data);
        }
    }

    return error;
}

/*!
 * brief USDHC module initialization function.
 *
 * Configures the USDHC according to the user configuration.
 *
 * Example:
   code
   usdhc_config_t config;
   config.cardDetectDat3 = false;
   config.endianMode = kUSDHC_EndianModeLittle;
   config.dmaMode = kUSDHC_DmaModeAdma2;
   config.readWatermarkLevel = 128U;
   config.writeWatermarkLevel = 128U;
   USDHC_Init(USDHC, &config);
   endcode
 *
 * param base USDHC peripheral base address.
 * param config USDHC configuration information.
 * retval kStatus_Success Operate successfully.
 */
void USDHC_Init(USDHC_Type *base, const usdhc_config_t *config)
{
    assert(config);
    assert((config->writeWatermarkLevel >= 1U) && (config->writeWatermarkLevel <= 128U));
    assert((config->readWatermarkLevel >= 1U) && (config->readWatermarkLevel <= 128U));
    assert(config->writeBurstLen <= 16U);

    uint32_t proctl, sysctl, wml;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable USDHC clock. */
    CLOCK_EnableClock(s_usdhcClock[USDHC_GetInstance(base)]);
#endif

    /* Reset USDHC. */
    USDHC_Reset(base, kUSDHC_ResetAll, 100U);

    proctl = base->PROT_CTRL;
    wml    = base->WTMK_LVL;
    sysctl = base->SYS_CTRL;

    proctl &= ~(USDHC_PROT_CTRL_EMODE_MASK | USDHC_PROT_CTRL_DMASEL_MASK);
    /* Endian mode*/
    proctl |= USDHC_PROT_CTRL_EMODE(config->endianMode);

    /* Watermark level */
    wml &= ~(USDHC_WTMK_LVL_RD_WML_MASK | USDHC_WTMK_LVL_WR_WML_MASK | USDHC_WTMK_LVL_RD_BRST_LEN_MASK |
             USDHC_WTMK_LVL_WR_BRST_LEN_MASK);
    wml |= (USDHC_WTMK_LVL_RD_WML(config->readWatermarkLevel) | USDHC_WTMK_LVL_WR_WML(config->writeWatermarkLevel) |
            USDHC_WTMK_LVL_RD_BRST_LEN(config->readBurstLen) | USDHC_WTMK_LVL_WR_BRST_LEN(config->writeBurstLen));

    /* config the data timeout value */
    sysctl &= ~USDHC_SYS_CTRL_DTOCV_MASK;
    sysctl |= USDHC_SYS_CTRL_DTOCV(config->dataTimeout);

    base->SYS_CTRL  = sysctl;
    base->WTMK_LVL  = wml;
    base->PROT_CTRL = proctl;

#if FSL_FEATURE_USDHC_HAS_EXT_DMA
    /* disable external DMA */
    base->VEND_SPEC &= ~USDHC_VEND_SPEC_EXT_DMA_EN_MASK;
#endif
    /* disable internal DMA and DDR mode */
    base->MIX_CTRL &= ~(USDHC_MIX_CTRL_DMAEN_MASK | USDHC_MIX_CTRL_DDR_EN_MASK);
    /* Enable interrupt status but doesn't enable interrupt signal. */
    USDHC_SetTransferInterrupt(base, false);
}

/*!
 * brief Deinitializes the USDHC.
 *
 * param base USDHC peripheral base address.
 */
void USDHC_Deinit(USDHC_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable clock. */
    CLOCK_DisableClock(s_usdhcClock[USDHC_GetInstance(base)]);
#endif
}

/*!
 * brief Resets the USDHC.
 *
 * param base USDHC peripheral base address.
 * param mask The reset type mask(_usdhc_reset).
 * param timeout Timeout for reset.
 * retval true Reset successfully.
 * retval false Reset failed.
 */
bool USDHC_Reset(USDHC_Type *base, uint32_t mask, uint32_t timeout)
{
    base->SYS_CTRL |= (mask & (USDHC_SYS_CTRL_RSTA_MASK | USDHC_SYS_CTRL_RSTC_MASK | USDHC_SYS_CTRL_RSTD_MASK));
    /* Delay some time to wait reset success. */
    while ((base->SYS_CTRL & mask) != 0U)
    {
        if (timeout == 0U)
        {
            break;
        }
        timeout--;
    }

    return ((!timeout) ? false : true);
}

/*!
 * brief Gets the capability information.
 *
 * param base USDHC peripheral base address.
 * param capability Structure to save capability information.
 */
void USDHC_GetCapability(USDHC_Type *base, usdhc_capability_t *capability)
{
    assert(capability);

    uint32_t htCapability;
    uint32_t maxBlockLength;

    htCapability = base->HOST_CTRL_CAP;

    /* Get the capability of USDHC. */
    maxBlockLength             = ((htCapability & USDHC_HOST_CTRL_CAP_MBL_MASK) >> USDHC_HOST_CTRL_CAP_MBL_SHIFT);
    capability->maxBlockLength = (512U << maxBlockLength);
    /* Other attributes not in HTCAPBLT register. */
    capability->maxBlockCount = USDHC_MAX_BLOCK_COUNT;
    capability->flags = (htCapability & (kUSDHC_SupportAdmaFlag | kUSDHC_SupportHighSpeedFlag | kUSDHC_SupportDmaFlag |
                                         kUSDHC_SupportSuspendResumeFlag | kUSDHC_SupportV330Flag));
    capability->flags |= (htCapability & kUSDHC_SupportV300Flag);
    capability->flags |= (htCapability & kUSDHC_SupportV180Flag);
    capability->flags |=
        (htCapability & (kUSDHC_SupportDDR50Flag | kUSDHC_SupportSDR104Flag | kUSDHC_SupportSDR50Flag));
    /* USDHC support 4/8 bit data bus width. */
    capability->flags |= (kUSDHC_Support4BitFlag | kUSDHC_Support8BitFlag);
}

/*!
 * brief Sets the SD bus clock frequency.
 *
 * param base USDHC peripheral base address.
 * param srcClock_Hz USDHC source clock frequency united in Hz.
 * param busClock_Hz SD bus clock frequency united in Hz.
 *
 * return The nearest frequency of busClock_Hz configured to SD bus.
 */
uint32_t USDHC_SetSdClock(USDHC_Type *base, uint32_t srcClock_Hz, uint32_t busClock_Hz)
{
    assert(srcClock_Hz != 0U);
    assert((busClock_Hz != 0U) && (busClock_Hz <= srcClock_Hz));

    uint32_t totalDiv         = 0U;
    uint32_t divisor          = 0U;
    uint32_t prescaler        = 0U;
    uint32_t sysctl           = 0U;
    uint32_t nearestFrequency = 0U;

    /* calucate total divisor first */
    if ((totalDiv = srcClock_Hz / busClock_Hz) > (USDHC_MAX_CLKFS * USDHC_MAX_DVS))
    {
        return 0U;
    }

    if (totalDiv != 0U)
    {
        /* calucate the divisor (srcClock_Hz / divisor) <= busClock_Hz */
        if ((srcClock_Hz / totalDiv) > busClock_Hz)
        {
            totalDiv++;
        }

        /* divide the total divisor to div and prescaler */
        if (totalDiv > USDHC_MAX_DVS)
        {
            prescaler = totalDiv / USDHC_MAX_DVS;
            /* prescaler must be a value which equal 2^n and smaller than SDHC_MAX_CLKFS */
            while (((USDHC_MAX_CLKFS % prescaler) != 0U) || (prescaler == 1U))
            {
                prescaler++;
            }
            /* calucate the divisor */
            divisor = totalDiv / prescaler;
            /* fine tuning the divisor until divisor * prescaler >= totalDiv */
            while ((divisor * prescaler) < totalDiv)
            {
                divisor++;
                if (divisor > USDHC_MAX_DVS)
                {
                    if ((prescaler <<= 1U) > USDHC_MAX_CLKFS)
                    {
                        return 0;
                    }
                    divisor = totalDiv / prescaler;
                }
            }
        }
        else
        {
            /* in this situation , divsior and SDCLKFS can generate same clock
            use SDCLKFS*/
            if (((totalDiv % 2U) != 0U) & (totalDiv != 1U))
            {
                divisor   = totalDiv;
                prescaler = 1U;
            }
            else
            {
                divisor   = 1U;
                prescaler = totalDiv;
            }
        }
        nearestFrequency = srcClock_Hz / (divisor == 0U ? 1U : divisor) / prescaler;
    }
    /* in this condition , srcClock_Hz = busClock_Hz, */
    else
    {
        /* in DDR mode , set SDCLKFS to 0, divisor = 0, actually the
        totoal divider = 2U */
        divisor          = 0U;
        prescaler        = 0U;
        nearestFrequency = srcClock_Hz;
    }

    /* calucate the value write to register */
    if (divisor != 0U)
    {
        USDHC_PREV_DVS(divisor);
    }
    /* calucate the value write to register */
    if (prescaler != 0U)
    {
        USDHC_PREV_CLKFS(prescaler, 1U);
    }

    /* Set the SD clock frequency divisor, SD clock frequency select, data timeout counter value. */
    sysctl = base->SYS_CTRL;
    sysctl &= ~(USDHC_SYS_CTRL_DVS_MASK | USDHC_SYS_CTRL_SDCLKFS_MASK);
    sysctl |= (USDHC_SYS_CTRL_DVS(divisor) | USDHC_SYS_CTRL_SDCLKFS(prescaler));
    base->SYS_CTRL = sysctl;

    /* Wait until the SD clock is stable. */
    while (!(base->PRES_STATE & USDHC_PRES_STATE_SDSTB_MASK))
    {
    }

    return nearestFrequency;
}

/*!
 * brief Sends 80 clocks to the card to set it to the active state.
 *
 * This function must be called each time the card is inserted to ensure that the card can receive the command
 * correctly.
 *
 * param base USDHC peripheral base address.
 * param timeout Timeout to initialize card.
 * retval true Set card active successfully.
 * retval false Set card active failed.
 */
bool USDHC_SetCardActive(USDHC_Type *base, uint32_t timeout)
{
    base->SYS_CTRL |= USDHC_SYS_CTRL_INITA_MASK;
    /* Delay some time to wait card become active state. */
    while ((base->SYS_CTRL & USDHC_SYS_CTRL_INITA_MASK) == USDHC_SYS_CTRL_INITA_MASK)
    {
        if (!timeout)
        {
            break;
        }
        timeout--;
    }

    return ((!timeout) ? false : true);
}

/*!
 * brief the enable/disable DDR mode
 *
 * param base USDHC peripheral base address.
 * param enable/disable flag
 * param nibble position
 */
void USDHC_EnableDDRMode(USDHC_Type *base, bool enable, uint32_t nibblePos)
{
    uint32_t prescaler = (base->SYS_CTRL & USDHC_SYS_CTRL_SDCLKFS_MASK) >> USDHC_SYS_CTRL_SDCLKFS_SHIFT;

    if (enable)
    {
        base->MIX_CTRL &= ~USDHC_MIX_CTRL_NIBBLE_POS_MASK;
        base->MIX_CTRL |= (USDHC_MIX_CTRL_DDR_EN_MASK | USDHC_MIX_CTRL_NIBBLE_POS(nibblePos));
        prescaler >>= 1U;
    }
    else
    {
        base->MIX_CTRL &= ~USDHC_MIX_CTRL_DDR_EN_MASK;

        if (prescaler == 0U)
        {
            prescaler += 1U;
        }
        else
        {
            prescaler <<= 1U;
        }
    }

    base->SYS_CTRL = (base->SYS_CTRL & (~USDHC_SYS_CTRL_SDCLKFS_MASK)) | USDHC_SYS_CTRL_SDCLKFS(prescaler);
}

/*!
 * brief Configures the MMC boot feature.
 *
 * Example:
   code
   usdhc_boot_config_t config;
   config.ackTimeoutCount = 4;
   config.bootMode = kUSDHC_BootModeNormal;
   config.blockCount = 5;
   config.enableBootAck = true;
   config.enableBoot = true;
   config.enableAutoStopAtBlockGap = true;
   USDHC_SetMmcBootConfig(USDHC, &config);
   endcode
 *
 * param base USDHC peripheral base address.
 * param config The MMC boot configuration information.
 */
void USDHC_SetMmcBootConfig(USDHC_Type *base, const usdhc_boot_config_t *config)
{
    assert(config);
    assert(config->ackTimeoutCount <= (USDHC_MMC_BOOT_DTOCV_ACK_MASK >> USDHC_MMC_BOOT_DTOCV_ACK_SHIFT));
    assert(config->blockCount <= (USDHC_MMC_BOOT_BOOT_BLK_CNT_MASK >> USDHC_MMC_BOOT_BOOT_BLK_CNT_SHIFT));

    uint32_t mmcboot = base->MMC_BOOT;

    mmcboot &= ~(USDHC_MMC_BOOT_DTOCV_ACK_MASK | USDHC_MMC_BOOT_BOOT_MODE_MASK | USDHC_MMC_BOOT_BOOT_BLK_CNT_MASK);
    mmcboot |= USDHC_MMC_BOOT_DTOCV_ACK(config->ackTimeoutCount) | USDHC_MMC_BOOT_BOOT_MODE(config->bootMode);

    if (config->enableBootAck)
    {
        mmcboot |= USDHC_MMC_BOOT_BOOT_ACK_MASK;
    }
    if (config->enableAutoStopAtBlockGap)
    {
        mmcboot |=
            USDHC_MMC_BOOT_AUTO_SABG_EN_MASK | USDHC_MMC_BOOT_BOOT_BLK_CNT(USDHC_MAX_BLOCK_COUNT - config->blockCount);
        /* always set the block count to USDHC_MAX_BLOCK_COUNT to use auto stop at block gap feature */
        base->BLK_ATT = ((base->BLK_ATT & ~(USDHC_BLK_ATT_BLKSIZE_MASK | USDHC_BLK_ATT_BLKCNT_MASK)) |
                         (USDHC_BLK_ATT_BLKSIZE(config->blockSize) | USDHC_BLK_ATT_BLKCNT(USDHC_MAX_BLOCK_COUNT)));
    }
    else
    {
        base->BLK_ATT = ((base->BLK_ATT & ~(USDHC_BLK_ATT_BLKSIZE_MASK | USDHC_BLK_ATT_BLKCNT_MASK)) |
                         (USDHC_BLK_ATT_BLKSIZE(config->blockSize) | USDHC_BLK_ATT_BLKCNT(config->blockCount)));
    }

    base->MMC_BOOT = mmcboot;
}

/*!
 * brief Sets the ADMA1 descriptor table configuration.
 *
 * param admaTable Adma table address.
 * param admaTableWords Adma table length.
 * param dataBufferAddr Data buffer address.
 * param dataBytes Data length.
 * param flags ADAM descriptor flag, used to indicate to create multiple or single descriptor, please
 *  reference _usdhc_adma_flag.
 * retval kStatus_OutOfRange ADMA descriptor table length isn't enough to describe data.
 * retval kStatus_Success Operate successfully.
 */
status_t USDHC_SetADMA1Descriptor(
    uint32_t *admaTable, uint32_t admaTableWords, const uint32_t *dataBufferAddr, uint32_t dataBytes, uint32_t flags)
{
    assert(NULL != admaTable);
    assert(NULL != dataBufferAddr);

    uint32_t miniEntries, startEntries = 0U,
                          maxEntries = (admaTableWords * sizeof(uint32_t)) / sizeof(usdhc_adma1_descriptor_t);
    usdhc_adma1_descriptor_t *adma1EntryAddress = (usdhc_adma1_descriptor_t *)(admaTable);
    uint32_t i, dmaBufferLen = 0U;
    const uint32_t *data = dataBufferAddr;

    if (((uint32_t)data % USDHC_ADMA1_ADDRESS_ALIGN) != 0U)
    {
        return kStatus_USDHC_DMADataAddrNotAlign;
    }

    if (flags == kUSDHC_AdmaDescriptorMultipleFlag)
    {
        return kStatus_USDHC_NotSupport;
    }
    /*
     * Add non aligned access support ,user need make sure your buffer size is big
     * enough to hold the data,in other words,user need make sure the buffer size
     * is 4 byte aligned
     */
    if (dataBytes % sizeof(uint32_t) != 0U)
    {
        /* make the data length as word-aligned */
        dataBytes += sizeof(uint32_t) - (dataBytes % sizeof(uint32_t));
    }

    /* Check if ADMA descriptor's number is enough. */
    if ((dataBytes % USDHC_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) == 0U)
    {
        miniEntries = dataBytes / USDHC_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
    }
    else
    {
        miniEntries = ((dataBytes / USDHC_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) + 1U);
    }

    /* ADMA1 needs two descriptors to finish a transfer */
    miniEntries <<= 1U;

    if (miniEntries + startEntries > maxEntries)
    {
        return kStatus_OutOfRange;
    }

    for (i = startEntries; i < (miniEntries + startEntries); i += 2U)
    {
        if (dataBytes > USDHC_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY)
        {
            dmaBufferLen = USDHC_ADMA1_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
        }
        else
        {
            dmaBufferLen = dataBytes;
        }

        adma1EntryAddress[i] = (dmaBufferLen << USDHC_ADMA1_DESCRIPTOR_LENGTH_SHIFT);
        adma1EntryAddress[i] |= kUSDHC_Adma1DescriptorTypeSetLength;
        adma1EntryAddress[i + 1U] = (uint32_t)(data);
        adma1EntryAddress[i + 1U] |= kUSDHC_Adma1DescriptorTypeTransfer;
        data += dmaBufferLen / sizeof(uint32_t);
        dataBytes -= dmaBufferLen;
    }
    /* the end of the descriptor */
    adma1EntryAddress[i - 1U] |= kUSDHC_Adma1DescriptorEndFlag;

    return kStatus_Success;
}

/*!
 * brief Sets the ADMA2 descriptor table configuration.
 *
 * param admaTable Adma table address.
 * param admaTableWords Adma table length.
 * param dataBufferAddr Data buffer address.
 * param dataBytes Data Data length.
 * param flags ADAM descriptor flag, used to indicate to create multiple or single descriptor, please
 *  reference _usdhc_adma_flag.
 * retval kStatus_OutOfRange ADMA descriptor table length isn't enough to describe data.
 * retval kStatus_Success Operate successfully.
 */
status_t USDHC_SetADMA2Descriptor(
    uint32_t *admaTable, uint32_t admaTableWords, const uint32_t *dataBufferAddr, uint32_t dataBytes, uint32_t flags)
{
    assert(NULL != admaTable);
    assert(NULL != dataBufferAddr);

    uint32_t miniEntries, startEntries = 0U,
                          maxEntries = (admaTableWords * sizeof(uint32_t)) / sizeof(usdhc_adma2_descriptor_t);
    usdhc_adma2_descriptor_t *adma2EntryAddress = (usdhc_adma2_descriptor_t *)(admaTable);
    uint32_t i, dmaBufferLen = 0U;
    const uint32_t *data = dataBufferAddr;

    if (((uint32_t)data % USDHC_ADMA2_ADDRESS_ALIGN) != 0U)
    {
        return kStatus_USDHC_DMADataAddrNotAlign;
    }
    /*
     * Add non aligned access support ,user need make sure your buffer size is big
     * enough to hold the data,in other words,user need make sure the buffer size
     * is 4 byte aligned
     */
    if (dataBytes % sizeof(uint32_t) != 0U)
    {
        /* make the data length as word-aligned */
        dataBytes += sizeof(uint32_t) - (dataBytes % sizeof(uint32_t));
    }

    /* Check if ADMA descriptor's number is enough. */
    if ((dataBytes % USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) == 0U)
    {
        miniEntries = dataBytes / USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
    }
    else
    {
        miniEntries = ((dataBytes / USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY) + 1U);
    }
    /* calucate the start entry for multiple descriptor mode, ADMA engine is not stop, so update the descriptor
    data adress and data size is enough */
    if (flags == kUSDHC_AdmaDescriptorMultipleFlag)
    {
        for (i = 0U; i < maxEntries; i++)
        {
            if ((adma2EntryAddress[i].attribute & kUSDHC_Adma2DescriptorValidFlag) == 0U)
            {
                break;
            }
        }
        startEntries = i;
        /* add one entry for dummy entry */
        miniEntries += 1U;
    }

    if ((miniEntries + startEntries) > maxEntries)
    {
        return kStatus_OutOfRange;
    }

    for (i = startEntries; i < (miniEntries + startEntries); i++)
    {
        if (dataBytes > USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY)
        {
            dmaBufferLen = USDHC_ADMA2_DESCRIPTOR_MAX_LENGTH_PER_ENTRY;
        }
        else
        {
            dmaBufferLen = (dataBytes == 0U ? sizeof(uint32_t) :
                                              dataBytes); /* adma don't support 0 data length transfer descriptor */
        }

        /* Each descriptor for ADMA2 is 64-bit in length */
        adma2EntryAddress[i].address   = (dataBytes == 0U) ? &s_usdhcBootDummy : data;
        adma2EntryAddress[i].attribute = (dmaBufferLen << USDHC_ADMA2_DESCRIPTOR_LENGTH_SHIFT);
        adma2EntryAddress[i].attribute |=
            (dataBytes == 0U) ? 0U : (kUSDHC_Adma2DescriptorTypeTransfer | kUSDHC_Adma2DescriptorInterruptFlag);
        data += (dmaBufferLen / sizeof(uint32_t));

        if (dataBytes != 0U)
        {
            dataBytes -= dmaBufferLen;
        }
    }

    /* add a dummy valid ADMA descriptor for multiple descriptor mode, this is useful when transfer boot data, the ADMA
    engine
    will not stop at block gap */
    if (flags == kUSDHC_AdmaDescriptorMultipleFlag)
    {
        adma2EntryAddress[startEntries + 1U].attribute |= kUSDHC_Adma2DescriptorTypeTransfer;
    }
    else
    {
        /* set the end bit */
        adma2EntryAddress[i - 1U].attribute |= kUSDHC_Adma2DescriptorEndFlag;
    }

    return kStatus_Success;
}

/*!
 * brief Internal DMA configuration.
 * This function is used to config the USDHC DMA related registers.
 * param base USDHC peripheral base address.
 * param adma configuration
 * param dataAddr transfer data address, a simple DMA parameter, if ADMA is used, leave it to NULL.
 * param enAutoCmd23 flag to indicate Auto CMD23 is enable or not, a simple DMA parameter,if ADMA is used, leave it to
 * false.
 * retval kStatus_OutOfRange ADMA descriptor table length isn't enough to describe data.
 * retval kStatus_Success Operate successfully.
 */
status_t USDHC_SetInternalDmaConfig(USDHC_Type *base,
                                    usdhc_adma_config_t *dmaConfig,
                                    const uint32_t *dataAddr,
                                    bool enAutoCmd23)
{
    assert(dmaConfig);
    assert(dataAddr);
    assert((NULL != dmaConfig->admaTable) &&
           (((USDHC_ADMA_TABLE_ADDRESS_ALIGN - 1U) & (uint32_t)dmaConfig->admaTable) == 0U));

#if FSL_FEATURE_USDHC_HAS_EXT_DMA
    /* disable the external DMA if support */
    base->VEND_SPEC &= ~USDHC_VEND_SPEC_EXT_DMA_EN_MASK;
#endif

    if (dmaConfig->dmaMode == kUSDHC_DmaModeSimple)
    {
        /* check DMA data buffer address align or not */
        if (((uint32_t)dataAddr % USDHC_ADMA2_ADDRESS_ALIGN) != 0U)
        {
            return kStatus_USDHC_DMADataAddrNotAlign;
        }
        /* in simple DMA mode if use auto CMD23, address should load to ADMA addr,
             and block count should load to DS_ADDR*/
        if (enAutoCmd23)
        {
            base->ADMA_SYS_ADDR = (uint32_t)dataAddr;
        }
        else
        {
            base->DS_ADDR = (uint32_t)dataAddr;
        }
    }
    else
    {
        /* When use ADMA, disable simple DMA */
        base->DS_ADDR       = 0U;
        base->ADMA_SYS_ADDR = (uint32_t)(dmaConfig->admaTable);
    }

    /* select DMA mode and config the burst length */
    base->PROT_CTRL &= ~(USDHC_PROT_CTRL_DMASEL_MASK | USDHC_PROT_CTRL_BURST_LEN_EN_MASK);
    base->PROT_CTRL |= USDHC_PROT_CTRL_DMASEL(dmaConfig->dmaMode) | USDHC_PROT_CTRL_BURST_LEN_EN(dmaConfig->burstLen);
    /* enable DMA */
    base->MIX_CTRL |= USDHC_MIX_CTRL_DMAEN_MASK;

    return kStatus_Success;
}

/*!
 * brief Sets the DMA descriptor table configuration.
 * A high level DMA descriptor configuration function.
 * param base USDHC peripheral base address.
 * param adma configuration
 * param data Data descriptor
 * param flags ADAM descriptor flag, used to indicate to create multiple or single descriptor, please
 *  reference _usdhc_adma_flag
 * retval kStatus_OutOfRange ADMA descriptor table length isn't enough to describe data.
 * retval kStatus_Success Operate successfully.
 */
status_t USDHC_SetAdmaTableConfig(USDHC_Type *base,
                                  usdhc_adma_config_t *dmaConfig,
                                  usdhc_data_t *dataConfig,
                                  uint32_t flags)
{
    assert(NULL != dmaConfig);
    assert((NULL != dmaConfig->admaTable) &&
           (((USDHC_ADMA_TABLE_ADDRESS_ALIGN - 1U) & (uint32_t)dmaConfig->admaTable) == 0U));
    assert(NULL != dataConfig);

    status_t error           = kStatus_Fail;
    uint32_t bootDummyOffset = dataConfig->dataType == kUSDHC_TransferDataBootcontinous ? sizeof(uint32_t) : 0U;
    const uint32_t *data =
        (const uint32_t *)((uint32_t)((dataConfig->rxData == NULL) ? dataConfig->txData : dataConfig->rxData) +
                           bootDummyOffset);
    uint32_t blockSize = dataConfig->blockSize * dataConfig->blockCount - bootDummyOffset;

    switch (dmaConfig->dmaMode)
    {
#if FSL_FEATURE_USDHC_HAS_EXT_DMA
        case kUSDHC_ExternalDMA:
            /* enable the external DMA */
            base->VEND_SPEC |= USDHC_VEND_SPEC_EXT_DMA_EN_MASK;
            break;
#endif
        case kUSDHC_DmaModeSimple:
            error = kStatus_Success;
            break;

        case kUSDHC_DmaModeAdma1:
            error = USDHC_SetADMA1Descriptor(dmaConfig->admaTable, dmaConfig->admaTableWords, data, blockSize, flags);
            break;

        case kUSDHC_DmaModeAdma2:
            error = USDHC_SetADMA2Descriptor(dmaConfig->admaTable, dmaConfig->admaTableWords, data, blockSize, flags);
            break;
        default:
            return kStatus_USDHC_PrepareAdmaDescriptorFailed;
    }

    /* for internal dma, internal DMA configurations should not update the configurations when continous transfer the
     * boot data, only the DMA descriptor need update */
    if ((dmaConfig->dmaMode != kUSDHC_ExternalDMA) && (error == kStatus_Success) &&
        (dataConfig->dataType != kUSDHC_TransferDataBootcontinous))
    {
        error = USDHC_SetInternalDmaConfig(base, dmaConfig, data, dataConfig->enableAutoCommand23);
    }

    return error;
}

/*!
 * brief Transfers the command/data using a blocking method.
 *
 * This function waits until the command response/data is received or the USDHC encounters an error by polling the
 * status
 * flag.
 * The application must not call this API in multiple threads at the same time. Because of that this API doesn't support
 * the re-entry mechanism.
 *
 * note There is no need to call the API 'USDHC_TransferCreateHandle' when calling this API.
 *
 * param base USDHC peripheral base address.
 * param adma configuration
 * param transfer Transfer content.
 * retval kStatus_InvalidArgument Argument is invalid.
 * retval kStatus_USDHC_PrepareAdmaDescriptorFailed Prepare ADMA descriptor failed.
 * retval kStatus_USDHC_SendCommandFailed Send command failed.
 * retval kStatus_USDHC_TransferDataFailed Transfer data failed.
 * retval kStatus_Success Operate successfully.
 */
status_t USDHC_TransferBlocking(USDHC_Type *base, usdhc_adma_config_t *dmaConfig, usdhc_transfer_t *transfer)
{
    assert(transfer);

    status_t error           = kStatus_Fail;
    usdhc_command_t *command = transfer->command;
    usdhc_data_t *data       = transfer->data;
    bool enDMA               = true;
    bool executeTuning       = ((data == NULL) ? false : data->dataType == kUSDHC_TransferDataTuning);

    /*check re-tuning request*/
    if ((USDHC_GetInterruptStatusFlags(base) & kUSDHC_ReTuningEventFlag) != 0U)
    {
        USDHC_ClearInterruptStatusFlags(base, kUSDHC_ReTuningEventFlag);
        return kStatus_USDHC_ReTuningRequest;
    }

    /* Update ADMA descriptor table according to different DMA mode(no DMA, ADMA1, ADMA2).*/
    if ((data != NULL) && (dmaConfig != NULL) && (!executeTuning))
    {
        error =
            USDHC_SetAdmaTableConfig(base, dmaConfig, data,
                                     (data->dataType & kUSDHC_TransferDataBoot) ? kUSDHC_AdmaDescriptorMultipleFlag :
                                                                                  kUSDHC_AdmaDescriptorSingleFlag);
    }

    /* if the DMA desciptor configure fail or not needed , disable it */
    if (error != kStatus_Success)
    {
        enDMA = false;
        /* disable DMA, using polling mode in this situation */
        USDHC_EnableInternalDMA(base, false);
    }
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
    else
    {
        if (data->txData != NULL)
        {
            /* clear the DCACHE */
            DCACHE_CleanByRange((uint32_t)data->txData, (data->blockSize) * (data->blockCount));
        }
        else
        {
            /* clear the DCACHE */
            DCACHE_CleanInvalidateByRange((uint32_t)data->rxData, (data->blockSize) * (data->blockCount));
        }
    }
#endif

    /* config the data transfer parameter */
    error = USDHC_SetDataTransferConfig(base, data, &(command->flags), enDMA);
    if (kStatus_Success != error)
    {
        return error;
    }
    /* send command first */
    USDHC_SendCommand(base, command);
    /* wait command done */
    error = USDHC_WaitCommandDone(base, command, (data == NULL) || (data->dataType == kUSDHC_TransferDataNormal));

    /* wait transfer data finsih */
    if ((data != NULL) && (error == kStatus_Success))
    {
        return USDHC_TransferDataBlocking(base, data, enDMA);
    }

    return error;
}

/*!
 * brief Transfers the command/data using an interrupt and an asynchronous method.
 *
 * This function sends a command and data and returns immediately. It doesn't wait the transfer complete or encounter an
 * error.
 * The application must not call this API in multiple threads at the same time. Because of that this API doesn't support
 * the re-entry mechanism.
 *
 * note Call the API 'USDHC_TransferCreateHandle' when calling this API.
 *
 * param base USDHC peripheral base address.
 * param handle USDHC handle.
 * param adma configuration.
 * param transfer Transfer content.
 * retval kStatus_InvalidArgument Argument is invalid.
 * retval kStatus_USDHC_BusyTransferring Busy transferring.
 * retval kStatus_USDHC_PrepareAdmaDescriptorFailed Prepare ADMA descriptor failed.
 * retval kStatus_Success Operate successfully.
 */
status_t USDHC_TransferNonBlocking(USDHC_Type *base,
                                   usdhc_handle_t *handle,
                                   usdhc_adma_config_t *dmaConfig,
                                   usdhc_transfer_t *transfer)
{
    assert(handle);
    assert(transfer);

    status_t error           = kStatus_Fail;
    usdhc_command_t *command = transfer->command;
    usdhc_data_t *data       = transfer->data;
    bool executeTuning       = ((data == NULL) ? false : data->dataType == kUSDHC_TransferDataTuning);
    bool enDMA               = true;

    /*check re-tuning request*/
    if ((USDHC_GetInterruptStatusFlags(base) & (kUSDHC_ReTuningEventFlag)) != 0U)
    {
        USDHC_ClearInterruptStatusFlags(base, kUSDHC_ReTuningEventFlag);
        return kStatus_USDHC_ReTuningRequest;
    }

    /* Save command and data into handle before transferring. */

    handle->command = command;
    handle->data    = data;
    /* transferredWords will only be updated in ISR when transfer way is DATAPORT. */
    handle->transferredWords = 0U;

    /* Update ADMA descriptor table according to different DMA mode(no DMA, ADMA1, ADMA2).*/
    if ((data != NULL) && (dmaConfig != NULL) && (!executeTuning))
    {
        error =
            USDHC_SetAdmaTableConfig(base, dmaConfig, data,
                                     (data->dataType & kUSDHC_TransferDataBoot) ? kUSDHC_AdmaDescriptorMultipleFlag :
                                                                                  kUSDHC_AdmaDescriptorSingleFlag);
    }

    /* if the DMA desciptor configure fail or not needed , disable it */
    if (error != kStatus_Success)
    {
        /* disable DMA, using polling mode in this situation */
        USDHC_EnableInternalDMA(base, false);
        enDMA = false;
    }
#if defined(FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL) && FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL
    else
    {
        if (data->txData != NULL)
        {
            /* clear the DCACHE */
            DCACHE_CleanByRange((uint32_t)data->txData, (data->blockSize) * (data->blockCount));
        }
        else
        {
            /* clear the DCACHE */
            DCACHE_CleanInvalidateByRange((uint32_t)data->rxData, (data->blockSize) * (data->blockCount));
        }
    }
#endif

    error = USDHC_SetDataTransferConfig(base, data, &(command->flags), enDMA);
    if (kStatus_Success != error)
    {
        return error;
    }

    /* send command first */
    USDHC_SendCommand(base, command);

    return kStatus_Success;
}

#if defined(FSL_FEATURE_USDHC_HAS_SDR50_MODE) && (FSL_FEATURE_USDHC_HAS_SDR50_MODE)
/*!
 * brief manual tuning trigger or abort
 * User should handle the tuning cmd and find the boundary of the delay
 * then calucate a average value which will be config to the CLK_TUNE_CTRL_STATUS
 * This function should called before USDHC_AdjustDelayforSDR104 function
 * param base USDHC peripheral base address.
 * param tuning enable flag
 */
void USDHC_EnableManualTuning(USDHC_Type *base, bool enable)
{
    if (enable)
    {
        /* make sure std_tun_en bit is clear */
        base->TUNING_CTRL &= ~USDHC_TUNING_CTRL_STD_TUNING_EN_MASK;
        /* disable auto tuning here */
        base->MIX_CTRL &= ~USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK;
        /* execute tuning for SDR104 mode */
        base->MIX_CTRL |=
            USDHC_MIX_CTRL_EXE_TUNE_MASK | USDHC_MIX_CTRL_SMP_CLK_SEL_MASK | USDHC_MIX_CTRL_FBCLK_SEL_MASK;
    }
    else
    { /* abort the tuning */
        base->MIX_CTRL &= ~(USDHC_MIX_CTRL_EXE_TUNE_MASK | USDHC_MIX_CTRL_SMP_CLK_SEL_MASK);
    }
}

/*!
 * brief the SDR104 mode delay setting adjust
 * This function should called after USDHC_ManualTuningForSDR104
 * param base USDHC peripheral base address.
 * param delay setting configuration
 * retval kStatus_Fail config the delay setting fail
 * retval kStatus_Success config the delay setting success
 */
status_t USDHC_AdjustDelayForManualTuning(USDHC_Type *base, uint32_t delay)
{
    uint32_t clkTuneCtrl = 0U;

    clkTuneCtrl = base->CLK_TUNE_CTRL_STATUS;

    clkTuneCtrl &= ~USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_PRE_MASK;

    clkTuneCtrl |= USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_PRE(delay);

    /* load the delay setting */
    base->CLK_TUNE_CTRL_STATUS = clkTuneCtrl;
    /* check delat setting error */
    if (base->CLK_TUNE_CTRL_STATUS &
        (USDHC_CLK_TUNE_CTRL_STATUS_PRE_ERR_MASK | USDHC_CLK_TUNE_CTRL_STATUS_NXT_ERR_MASK))
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

/*!
 * brief the enable standard tuning function
 * The standard tuning window and tuning counter use the default config
 * tuning cmd is send by the software, user need to check the tuning result
 * can be used for SDR50,SDR104,HS200 mode tuning
 * param base USDHC peripheral base address.
 * param tuning start tap
 * param tuning step
 * param enable/disable flag
 */
void USDHC_EnableStandardTuning(USDHC_Type *base, uint32_t tuningStartTap, uint32_t step, bool enable)
{
    uint32_t tuningCtrl = 0U;

    if (enable)
    {
        /* feedback clock */
        base->MIX_CTRL |= USDHC_MIX_CTRL_FBCLK_SEL_MASK;
        /* config tuning start and step */
        tuningCtrl = base->TUNING_CTRL;
        tuningCtrl &= ~(USDHC_TUNING_CTRL_TUNING_START_TAP_MASK | USDHC_TUNING_CTRL_TUNING_STEP_MASK);
        tuningCtrl |= (USDHC_TUNING_CTRL_TUNING_START_TAP(tuningStartTap) | USDHC_TUNING_CTRL_TUNING_STEP(step) |
                       USDHC_TUNING_CTRL_STD_TUNING_EN_MASK);
        base->TUNING_CTRL = tuningCtrl;

        /* excute tuning */
        base->AUTOCMD12_ERR_STATUS |=
            (USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK | USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK);
    }
    else
    {
        /* disable the standard tuning */
        base->TUNING_CTRL &= ~USDHC_TUNING_CTRL_STD_TUNING_EN_MASK;
        /* clear excute tuning */
        base->AUTOCMD12_ERR_STATUS &=
            ~(USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK | USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK);
    }
}

/*!
 * brief the auto tuning enbale for CMD/DATA line
 *
 * param base USDHC peripheral base address.
 */
void USDHC_EnableAutoTuningForCmdAndData(USDHC_Type *base)
{
    uint32_t busWidth = 0U;

    base->VEND_SPEC2 |= USDHC_VEND_SPEC2_TUNING_CMD_EN_MASK;
    busWidth = (base->PROT_CTRL & USDHC_PROT_CTRL_DTW_MASK) >> USDHC_PROT_CTRL_DTW_SHIFT;
    if (busWidth == kUSDHC_DataBusWidth1Bit)
    {
        base->VEND_SPEC2 &= ~USDHC_VEND_SPEC2_TUNING_8bit_EN_MASK;
        base->VEND_SPEC2 |= USDHC_VEND_SPEC2_TUNING_1bit_EN_MASK;
    }
    else if (busWidth == kUSDHC_DataBusWidth4Bit)
    {
        base->VEND_SPEC2 &= ~USDHC_VEND_SPEC2_TUNING_8bit_EN_MASK;
        base->VEND_SPEC2 &= ~USDHC_VEND_SPEC2_TUNING_1bit_EN_MASK;
    }
    else if (busWidth == kUSDHC_DataBusWidth8Bit)
    {
        base->VEND_SPEC2 |= USDHC_VEND_SPEC2_TUNING_8bit_EN_MASK;
        base->VEND_SPEC2 &= ~USDHC_VEND_SPEC2_TUNING_1bit_EN_MASK;
    }
    else
    {
    }
}
#endif /* FSL_FEATURE_USDHC_HAS_SDR50_MODE */

static void USDHC_TransferHandleCardDetect(USDHC_Type *base, usdhc_handle_t *handle, uint32_t interruptFlags)
{
    if (interruptFlags & kUSDHC_CardInsertionFlag)
    {
        if (handle->callback.CardInserted)
        {
            handle->callback.CardInserted(base, handle->userData);
        }
    }
    else
    {
        if (handle->callback.CardRemoved)
        {
            handle->callback.CardRemoved(base, handle->userData);
        }
    }
}

static void USDHC_TransferHandleCommand(USDHC_Type *base, usdhc_handle_t *handle, uint32_t interruptFlags)
{
    assert(handle->command);

    if (interruptFlags & kUSDHC_CommandErrorFlag)
    {
        if (handle->callback.TransferComplete)
        {
            handle->callback.TransferComplete(base, handle, kStatus_USDHC_SendCommandFailed, handle->userData);
        }
    }
    else
    {
        /* Receive response */
        if (kStatus_Success != USDHC_ReceiveCommandResponse(base, handle->command))
        {
            if (handle->callback.TransferComplete)
            {
                handle->callback.TransferComplete(base, handle, kStatus_USDHC_SendCommandFailed, handle->userData);
            }
        }
        else if ((!(handle->data)) && (handle->callback.TransferComplete))
        {
            if (handle->callback.TransferComplete)
            {
                handle->callback.TransferComplete(base, handle, kStatus_Success, handle->userData);
            }
        }
        else
        {
        }
    }
}

static void USDHC_TransferHandleData(USDHC_Type *base, usdhc_handle_t *handle, uint32_t interruptFlags)
{
    assert(handle->data);

    if ((!(handle->data->enableIgnoreError)) && ((interruptFlags & (kUSDHC_DataErrorFlag | kUSDHC_DmaErrorFlag))))
    {
        if (handle->callback.TransferComplete)
        {
            handle->callback.TransferComplete(base, handle, kStatus_USDHC_TransferDataFailed, handle->userData);
        }
    }
    else
    {
        if (interruptFlags & kUSDHC_BufferReadReadyFlag)
        {
            /* std tuning process only need to wait BRR */
            if (handle->data->dataType == kUSDHC_TransferDataTuning)
            {
                if (handle->callback.TransferComplete)
                {
                    handle->callback.TransferComplete(base, handle, kStatus_Success, handle->userData);
                }
            }
            else
            {
                handle->transferredWords = USDHC_ReadDataPort(base, handle->data, handle->transferredWords);
            }
        }
        else if (interruptFlags & kUSDHC_BufferWriteReadyFlag)
        {
            handle->transferredWords = USDHC_WriteDataPort(base, handle->data, handle->transferredWords);
        }
        else
        {
            if ((interruptFlags & kUSDHC_DmaCompleteFlag) &&
                (handle->data->dataType == kUSDHC_TransferDataBootcontinous))
            {
                *(handle->data->rxData) = s_usdhcBootDummy;
            }

            if ((handle->callback.TransferComplete) && (interruptFlags & kUSDHC_DataCompleteFlag))
            {
                handle->callback.TransferComplete(base, handle, kStatus_Success, handle->userData);
            }
        }
    }
}

static void USDHC_TransferHandleSdioInterrupt(USDHC_Type *base, usdhc_handle_t *handle)
{
    if (handle->callback.SdioInterrupt)
    {
        handle->callback.SdioInterrupt(base, handle->userData);
    }
}

static void USDHC_TransferHandleReTuning(USDHC_Type *base, usdhc_handle_t *handle, uint32_t interruptFlags)
{
    assert(handle->callback.ReTuning);
    /* retuning request */
    if ((interruptFlags & kUSDHC_TuningErrorFlag) == kUSDHC_TuningErrorFlag)
    {
        handle->callback.ReTuning(base, handle->userData); /* retuning fail */
    }
}

static void USDHC_TransferHandleBlockGap(USDHC_Type *base, usdhc_handle_t *handle)
{
    if (handle->callback.BlockGap)
    {
        handle->callback.BlockGap(base, handle->userData);
    }
}

/*!
 * brief Creates the USDHC handle.
 *
 * param base USDHC peripheral base address.
 * param handle USDHC handle pointer.
 * param callback Structure pointer to contain all callback functions.
 * param userData Callback function parameter.
 */
void USDHC_TransferCreateHandle(USDHC_Type *base,
                                usdhc_handle_t *handle,
                                const usdhc_transfer_callback_t *callback,
                                void *userData)
{
    assert(handle);
    assert(callback);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Set the callback. */
    handle->callback.CardInserted     = callback->CardInserted;
    handle->callback.CardRemoved      = callback->CardRemoved;
    handle->callback.SdioInterrupt    = callback->SdioInterrupt;
    handle->callback.BlockGap         = callback->BlockGap;
    handle->callback.TransferComplete = callback->TransferComplete;
    handle->callback.ReTuning         = callback->ReTuning;
    handle->userData                  = userData;

    /* Save the handle in global variables to support the double weak mechanism. */
    s_usdhcHandle[USDHC_GetInstance(base)] = handle;

    /* Enable interrupt in NVIC. */
    USDHC_SetTransferInterrupt(base, true);
    /* save IRQ handler */
    s_usdhcIsr = USDHC_TransferHandleIRQ;

    EnableIRQ(s_usdhcIRQ[USDHC_GetInstance(base)]);
}

/*!
 * brief IRQ handler for the USDHC.
 *
 * This function deals with the IRQs on the given host controller.
 *
 * param base USDHC peripheral base address.
 * param handle USDHC handle.
 */
void USDHC_TransferHandleIRQ(USDHC_Type *base, usdhc_handle_t *handle)
{
    assert(handle);

    uint32_t interruptFlags;

    interruptFlags = USDHC_GetEnabledInterruptStatusFlags(base);

    if (interruptFlags & kUSDHC_CardDetectFlag)
    {
        USDHC_TransferHandleCardDetect(base, handle, (interruptFlags & kUSDHC_CardDetectFlag));
    }
    if (interruptFlags & kUSDHC_CommandFlag)
    {
        USDHC_TransferHandleCommand(base, handle, (interruptFlags & kUSDHC_CommandFlag));
    }
    if (interruptFlags & kUSDHC_DataFlag)
    {
        USDHC_TransferHandleData(base, handle, (interruptFlags & kUSDHC_DataFlag));
    }
    if (interruptFlags & kUSDHC_CardInterruptFlag)
    {
        USDHC_TransferHandleSdioInterrupt(base, handle);
    }
    if (interruptFlags & kUSDHC_BlockGapEventFlag)
    {
        USDHC_TransferHandleBlockGap(base, handle);
    }
    if (interruptFlags & kUSDHC_SDR104TuningFlag)
    {
        USDHC_TransferHandleReTuning(base, handle, (interruptFlags & kUSDHC_SDR104TuningFlag));
    }
    USDHC_ClearInterruptStatusFlags(base, interruptFlags);
}

#ifdef USDHC0
void USDHC0_DriverIRQHandler(void)
{
    s_usdhcIsr(s_usdhcBase[0U], s_usdhcHandle[0U]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#ifdef USDHC1
void USDHC1_DriverIRQHandler(void)
{
    s_usdhcIsr(s_usdhcBase[1U], s_usdhcHandle[1U]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#ifdef USDHC2
void USDHC2_DriverIRQHandler(void)
{
    s_usdhcIsr(s_usdhcBase[2U], s_usdhcHandle[2U]);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

#endif
