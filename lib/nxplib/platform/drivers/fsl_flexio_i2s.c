/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_i2s.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_i2s"
#endif

/*******************************************************************************
 * Definitations
 ******************************************************************************/
enum _sai_transfer_state
{
    kFLEXIO_I2S_Busy = 0x0U, /*!< FLEXIO_I2S is busy */
    kFLEXIO_I2S_Idle,        /*!< Transfer is done. */
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Receive a piece of data in non-blocking way.
 *
 * @param base FLEXIO I2S base pointer
 * @param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * @param buffer Pointer to the data to be read.
 * @param size Bytes to be read.
 */
static void FLEXIO_I2S_ReadNonBlocking(FLEXIO_I2S_Type *base, uint8_t bitWidth, uint8_t *rxData, size_t size);

/*!
 * @brief sends a piece of data in non-blocking way.
 *
 * @param base FLEXIO I2S base pointer
 * @param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * @param buffer Pointer to the data to be written.
 * @param size Bytes to be written.
 */
static void FLEXIO_I2S_WriteNonBlocking(FLEXIO_I2S_Type *base, uint8_t bitWidth, uint8_t *txData, size_t size);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static uint32_t FLEXIO_I2S_GetInstance(FLEXIO_I2S_Type *base)
{
    return FLEXIO_GetInstance(base->flexioBase);
}

static void FLEXIO_I2S_WriteNonBlocking(FLEXIO_I2S_Type *base, uint8_t bitWidth, uint8_t *txData, size_t size)
{
    uint32_t i           = 0;
    uint8_t j            = 0;
    uint8_t bytesPerWord = bitWidth / 8U;
    uint32_t data        = 0;
    uint32_t temp        = 0;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        for (j = 0; j < bytesPerWord; j++)
        {
            temp = (uint32_t)(*txData);
            data |= (temp << (8U * j));
            txData++;
        }
        base->flexioBase->SHIFTBUFBIS[base->txShifterIndex] = data << (32U - bitWidth);
        data                                                = 0;
    }
}

static void FLEXIO_I2S_ReadNonBlocking(FLEXIO_I2S_Type *base, uint8_t bitWidth, uint8_t *rxData, size_t size)
{
    uint32_t i           = 0;
    uint8_t j            = 0;
    uint8_t bytesPerWord = bitWidth / 8U;
    uint32_t data        = 0;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        data = (base->flexioBase->SHIFTBUFBIS[base->rxShifterIndex]);
        for (j = 0; j < bytesPerWord; j++)
        {
            *rxData = (data >> (8U * j)) & 0xFF;
            rxData++;
        }
    }
}

/*!
 * brief Initializes the FlexIO I2S.
 *
 * This API configures FlexIO pins and shifter to I2S and configures the FlexIO I2S with a configuration structure.
 * The configuration structure can be filled by the user, or be set with default values by
 * FLEXIO_I2S_GetDefaultConfig().
 *
 * note  This API should be called at the beginning of the application to use
 * the FlexIO I2S driver. Otherwise, any access to the FlexIO I2S module can cause hard fault
 * because the clock is not enabled.
 *
 * param base FlexIO I2S base pointer
 * param config FlexIO I2S configure structure.
 */
void FLEXIO_I2S_Init(FLEXIO_I2S_Type *base, const flexio_i2s_config_t *config)
{
    assert(base && config);

    flexio_shifter_config_t shifterConfig = {0};
    flexio_timer_config_t timerConfig     = {0};

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate flexio clock. */
    CLOCK_EnableClock(s_flexioClocks[FLEXIO_I2S_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* reset Flexio */
    FLEXIO_Reset(base->flexioBase);

    /* Set shifter for I2S Tx data */
    shifterConfig.timerSelect   = base->bclkTimerIndex;
    shifterConfig.pinSelect     = base->txPinIndex;
    shifterConfig.timerPolarity = config->txTimerPolarity;
    shifterConfig.pinConfig     = kFLEXIO_PinConfigOutput;
    shifterConfig.pinPolarity   = config->txPinPolarity;
    shifterConfig.shifterMode   = kFLEXIO_ShifterModeTransmit;
    shifterConfig.inputSource   = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop   = kFLEXIO_ShifterStopBitDisable;
    if (config->masterSlave == kFLEXIO_I2S_Master)
    {
        shifterConfig.shifterStart = kFLEXIO_ShifterStartBitDisabledLoadDataOnShift;
    }
    else
    {
        shifterConfig.shifterStart = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;
    }

    FLEXIO_SetShifterConfig(base->flexioBase, base->txShifterIndex, &shifterConfig);

    /* Set shifter for I2S Rx Data */
    shifterConfig.timerSelect   = base->bclkTimerIndex;
    shifterConfig.pinSelect     = base->rxPinIndex;
    shifterConfig.timerPolarity = config->rxTimerPolarity;
    shifterConfig.pinConfig     = kFLEXIO_PinConfigOutputDisabled;
    shifterConfig.pinPolarity   = config->rxPinPolarity;
    shifterConfig.shifterMode   = kFLEXIO_ShifterModeReceive;
    shifterConfig.inputSource   = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop   = kFLEXIO_ShifterStopBitDisable;
    shifterConfig.shifterStart  = kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable;

    FLEXIO_SetShifterConfig(base->flexioBase, base->rxShifterIndex, &shifterConfig);

    /* Set Timer to I2S frame sync */
    if (config->masterSlave == kFLEXIO_I2S_Master)
    {
        timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(base->txPinIndex);
        timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
        timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceExternal;
        timerConfig.pinConfig       = kFLEXIO_PinConfigOutput;
        timerConfig.pinSelect       = base->fsPinIndex;
        timerConfig.pinPolarity     = config->fsPinPolarity;
        timerConfig.timerMode       = kFLEXIO_TimerModeSingle16Bit;
        timerConfig.timerOutput     = kFLEXIO_TimerOutputOneNotAffectedByReset;
        timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
        timerConfig.timerReset      = kFLEXIO_TimerResetNever;
        timerConfig.timerDisable    = kFLEXIO_TimerDisableNever;
        timerConfig.timerEnable     = kFLEXIO_TimerEnableOnPrevTimerEnable;
        timerConfig.timerStart      = kFLEXIO_TimerStartBitDisabled;
        timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
    }
    else
    {
        timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(base->bclkPinIndex);
        timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
        timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
        timerConfig.pinConfig       = kFLEXIO_PinConfigOutputDisabled;
        timerConfig.pinSelect       = base->fsPinIndex;
        timerConfig.pinPolarity     = config->fsPinPolarity;
        timerConfig.timerMode       = kFLEXIO_TimerModeSingle16Bit;
        timerConfig.timerOutput     = kFLEXIO_TimerOutputOneNotAffectedByReset;
        timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnTriggerInputShiftTriggerInput;
        timerConfig.timerReset      = kFLEXIO_TimerResetNever;
        timerConfig.timerDisable    = kFLEXIO_TimerDisableOnTimerCompare;
        timerConfig.timerEnable     = kFLEXIO_TimerEnableOnPinRisingEdge;
        timerConfig.timerStart      = kFLEXIO_TimerStartBitDisabled;
        timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
    }
    FLEXIO_SetTimerConfig(base->flexioBase, base->fsTimerIndex, &timerConfig);

    /* Set Timer to I2S bit clock */
    if (config->masterSlave == kFLEXIO_I2S_Master)
    {
        timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(base->txShifterIndex);
        timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow;
        timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
        timerConfig.pinSelect       = base->bclkPinIndex;
        timerConfig.pinConfig       = kFLEXIO_PinConfigOutput;
        timerConfig.pinPolarity     = config->bclkPinPolarity;
        timerConfig.timerMode       = kFLEXIO_TimerModeDual8BitBaudBit;
        timerConfig.timerOutput     = kFLEXIO_TimerOutputOneNotAffectedByReset;
        timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
        timerConfig.timerReset      = kFLEXIO_TimerResetNever;
        timerConfig.timerDisable    = kFLEXIO_TimerDisableNever;
        timerConfig.timerEnable     = kFLEXIO_TimerEnableOnTriggerHigh;
        timerConfig.timerStart      = kFLEXIO_TimerStartBitEnabled;
        timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
    }
    else
    {
        timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_TIMn(base->fsTimerIndex);
        timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
        timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
        timerConfig.pinSelect       = base->bclkPinIndex;
        timerConfig.pinConfig       = kFLEXIO_PinConfigOutputDisabled;
        timerConfig.pinPolarity     = config->bclkPinPolarity;
        timerConfig.timerMode       = kFLEXIO_TimerModeSingle16Bit;
        timerConfig.timerOutput     = kFLEXIO_TimerOutputOneNotAffectedByReset;
        timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnPinInputShiftPinInput;
        timerConfig.timerReset      = kFLEXIO_TimerResetNever;
        timerConfig.timerDisable    = kFLEXIO_TimerDisableOnTimerCompareTriggerLow;
        timerConfig.timerEnable     = kFLEXIO_TimerEnableOnPinRisingEdgeTriggerHigh;
        timerConfig.timerStart      = kFLEXIO_TimerStartBitDisabled;
        timerConfig.timerStop       = kFLEXIO_TimerStopBitDisabled;
    }
    FLEXIO_SetTimerConfig(base->flexioBase, base->bclkTimerIndex, &timerConfig);

    /* If enable flexio I2S */
    if (config->enableI2S)
    {
        base->flexioBase->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
    }
    else
    {
        base->flexioBase->CTRL &= ~FLEXIO_CTRL_FLEXEN_MASK;
    }
}

/*!
 * brief  Sets the FlexIO I2S configuration structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for use in FLEXIO_I2S_Init().
 * Users may use the initialized structure unchanged in FLEXIO_I2S_Init() or modify
 * some fields of the structure before calling FLEXIO_I2S_Init().
 *
 * param config pointer to master configuration structure
 */
void FLEXIO_I2S_GetDefaultConfig(flexio_i2s_config_t *config)
{
    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->masterSlave     = kFLEXIO_I2S_Master;
    config->enableI2S       = true;
    config->txPinPolarity   = kFLEXIO_PinActiveHigh;
    config->rxPinPolarity   = kFLEXIO_PinActiveHigh;
    config->bclkPinPolarity = kFLEXIO_PinActiveHigh;
    config->fsPinPolarity   = kFLEXIO_PinActiveLow;
    config->txTimerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
    config->rxTimerPolarity = kFLEXIO_ShifterTimerPolarityOnNegitive;
}

/*!
 * brief De-initializes the FlexIO I2S.
 *
 * Calling this API resets the FlexIO I2S shifter and timer config. After calling this API,
 * call the FLEXO_I2S_Init to use the FlexIO I2S module.
 *
 * param base FlexIO I2S base pointer
 */
void FLEXIO_I2S_Deinit(FLEXIO_I2S_Type *base)
{
    base->flexioBase->SHIFTCFG[base->txShifterIndex] = 0;
    base->flexioBase->SHIFTCTL[base->txShifterIndex] = 0;
    base->flexioBase->SHIFTCFG[base->rxShifterIndex] = 0;
    base->flexioBase->SHIFTCTL[base->rxShifterIndex] = 0;
    base->flexioBase->TIMCFG[base->fsTimerIndex]     = 0;
    base->flexioBase->TIMCMP[base->fsTimerIndex]     = 0;
    base->flexioBase->TIMCTL[base->fsTimerIndex]     = 0;
    base->flexioBase->TIMCFG[base->bclkTimerIndex]   = 0;
    base->flexioBase->TIMCMP[base->bclkTimerIndex]   = 0;
    base->flexioBase->TIMCTL[base->bclkTimerIndex]   = 0;
}

/*!
 * brief Enables the FlexIO I2S interrupt.
 *
 * This function enables the FlexIO UART interrupt.
 *
 * param base Pointer to FLEXIO_I2S_Type structure
 * param mask interrupt source
 */
void FLEXIO_I2S_EnableInterrupts(FLEXIO_I2S_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_I2S_TxDataRegEmptyInterruptEnable)
    {
        FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, 1U << base->txShifterIndex);
    }
    if (mask & kFLEXIO_I2S_RxDataRegFullInterruptEnable)
    {
        FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, 1U << base->rxShifterIndex);
    }
}

/*!
 * brief Gets the FlexIO I2S status flags.
 *
 * param base Pointer to FLEXIO_I2S_Type structure
 * return Status flag, which are ORed by the enumerators in the _flexio_i2s_status_flags.
 */
uint32_t FLEXIO_I2S_GetStatusFlags(FLEXIO_I2S_Type *base)
{
    uint32_t status = 0;
    status = ((FLEXIO_GetShifterStatusFlags(base->flexioBase) & (1U << base->txShifterIndex)) >> base->txShifterIndex);
    status |=
        (((FLEXIO_GetShifterStatusFlags(base->flexioBase) & (1U << base->rxShifterIndex)) >> (base->rxShifterIndex))
         << 1U);
    return status;
}

/*!
 * brief Disables the FlexIO I2S interrupt.
 *
 * This function enables the FlexIO UART interrupt.
 *
 * param base pointer to FLEXIO_I2S_Type structure
 * param mask interrupt source
 */
void FLEXIO_I2S_DisableInterrupts(FLEXIO_I2S_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_I2S_TxDataRegEmptyInterruptEnable)
    {
        FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, 1U << base->txShifterIndex);
    }
    if (mask & kFLEXIO_I2S_RxDataRegFullInterruptEnable)
    {
        FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, 1U << base->rxShifterIndex);
    }
}

/*!
 * brief Configures the FlexIO I2S audio format in master mode.
 *
 * Audio format can be changed in run-time of FlexIO I2S. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * param base Pointer to FLEXIO_I2S_Type structure
 * param format Pointer to FlexIO I2S audio data format structure.
 * param srcClock_Hz I2S master clock source frequency in Hz.
 */
void FLEXIO_I2S_MasterSetFormat(FLEXIO_I2S_Type *base, flexio_i2s_format_t *format, uint32_t srcClock_Hz)
{
    uint32_t timDiv  = srcClock_Hz / (format->sampleRate_Hz * format->bitWidth * 2U);
    uint32_t bclkDiv = 0;

    /* Shall keep bclk and fs div an integer */
    if (timDiv % 2)
    {
        timDiv += 1U;
    }
    /* Set Frame sync timer cmp */
    base->flexioBase->TIMCMP[base->fsTimerIndex] = FLEXIO_TIMCMP_CMP(format->bitWidth * timDiv - 1U);

    /* Set bit clock timer cmp */
    bclkDiv                                        = ((timDiv / 2U - 1U) | ((format->bitWidth * 2U - 1U) << 8U));
    base->flexioBase->TIMCMP[base->bclkTimerIndex] = FLEXIO_TIMCMP_CMP(bclkDiv);
}

/*!
 * brief Configures the FlexIO I2S audio format in slave mode.
 *
 * Audio format can be changed in run-time of FlexIO I2S. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * param base Pointer to FLEXIO_I2S_Type structure
 * param format Pointer to FlexIO I2S audio data format structure.
 */
void FLEXIO_I2S_SlaveSetFormat(FLEXIO_I2S_Type *base, flexio_i2s_format_t *format)
{
    /* Set Frame sync timer cmp */
    base->flexioBase->TIMCMP[base->fsTimerIndex] = FLEXIO_TIMCMP_CMP(format->bitWidth * 4U - 3U);

    /* Set bit clock timer cmp */
    base->flexioBase->TIMCMP[base->bclkTimerIndex] = FLEXIO_TIMCMP_CMP(format->bitWidth * 2U - 1U);
}

/*!
 * brief Sends data using a blocking method.
 *
 * note This function blocks via polling until data is ready to be sent.
 *
 * param base FlexIO I2S base pointer.
 * param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * param txData Pointer to the data to be written.
 * param size Bytes to be written.
 */
void FLEXIO_I2S_WriteBlocking(FLEXIO_I2S_Type *base, uint8_t bitWidth, uint8_t *txData, size_t size)
{
    uint32_t i           = 0;
    uint8_t bytesPerWord = bitWidth / 8U;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        /* Wait until it can write data */
        while ((FLEXIO_I2S_GetStatusFlags(base) & kFLEXIO_I2S_TxDataRegEmptyFlag) == 0)
        {
        }

        FLEXIO_I2S_WriteNonBlocking(base, bitWidth, txData, bytesPerWord);
        txData += bytesPerWord;
    }

    /* Wait until the last data is sent */
    while ((FLEXIO_I2S_GetStatusFlags(base) & kFLEXIO_I2S_TxDataRegEmptyFlag) == 0)
    {
    }
}

/*!
 * brief Receives a piece of data using a blocking method.
 *
 * note This function blocks via polling until data is ready to be sent.
 *
 * param base FlexIO I2S base pointer
 * param bitWidth How many bits in a audio word, usually 8/16/24/32 bits.
 * param rxData Pointer to the data to be read.
 * param size Bytes to be read.
 */
void FLEXIO_I2S_ReadBlocking(FLEXIO_I2S_Type *base, uint8_t bitWidth, uint8_t *rxData, size_t size)
{
    uint32_t i           = 0;
    uint8_t bytesPerWord = bitWidth / 8U;

    for (i = 0; i < size / bytesPerWord; i++)
    {
        /* Wait until data is received */
        while (!(FLEXIO_GetShifterStatusFlags(base->flexioBase) & (1U << base->rxShifterIndex)))
        {
        }

        FLEXIO_I2S_ReadNonBlocking(base, bitWidth, rxData, bytesPerWord);
        rxData += bytesPerWord;
    }
}

/*!
 * brief Initializes the FlexIO I2S handle.
 *
 * This function initializes the FlexIO I2S handle which can be used for other
 * FlexIO I2S transactional APIs. Call this API once to get the
 * initialized handle.
 *
 * param base Pointer to FLEXIO_I2S_Type structure
 * param handle Pointer to flexio_i2s_handle_t structure to store the transfer state.
 * param callback FlexIO I2S callback function, which is called while finished a block.
 * param userData User parameter for the FlexIO I2S callback.
 */
void FLEXIO_I2S_TransferTxCreateHandle(FLEXIO_I2S_Type *base,
                                       flexio_i2s_handle_t *handle,
                                       flexio_i2s_callback_t callback,
                                       void *userData)
{
    assert(handle);

    IRQn_Type flexio_irqs[] = FLEXIO_IRQS;

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Store callback and user data. */
    handle->callback = callback;
    handle->userData = userData;

    /* Save the context in global variables to support the double weak mechanism. */
    FLEXIO_RegisterHandleIRQ(base, handle, FLEXIO_I2S_TransferTxHandleIRQ);

    /* Set the TX/RX state. */
    handle->state = kFLEXIO_I2S_Idle;

    /* Enable interrupt in NVIC. */
    EnableIRQ(flexio_irqs[FLEXIO_I2S_GetInstance(base)]);
}

/*!
 * brief Initializes the FlexIO I2S receive handle.
 *
 * This function initializes the FlexIO I2S handle which can be used for other
 * FlexIO I2S transactional APIs. Call this API once to get the
 * initialized handle.
 *
 * param base Pointer to FLEXIO_I2S_Type structure.
 * param handle Pointer to flexio_i2s_handle_t structure to store the transfer state.
 * param callback FlexIO I2S callback function, which is called while finished a block.
 * param userData User parameter for the FlexIO I2S callback.
 */
void FLEXIO_I2S_TransferRxCreateHandle(FLEXIO_I2S_Type *base,
                                       flexio_i2s_handle_t *handle,
                                       flexio_i2s_callback_t callback,
                                       void *userData)
{
    assert(handle);

    IRQn_Type flexio_irqs[] = FLEXIO_IRQS;

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Store callback and user data. */
    handle->callback = callback;
    handle->userData = userData;

    /* Save the context in global variables to support the double weak mechanism. */
    FLEXIO_RegisterHandleIRQ(base, handle, FLEXIO_I2S_TransferRxHandleIRQ);

    /* Set the TX/RX state. */
    handle->state = kFLEXIO_I2S_Idle;

    /* Enable interrupt in NVIC. */
    EnableIRQ(flexio_irqs[FLEXIO_I2S_GetInstance(base)]);
}

/*!
 * brief Configures the FlexIO I2S audio format.
 *
 * Audio format can be changed at run-time of FlexIO I2S. This function configures the sample rate and audio data
 * format to be transferred.
 *
 * param base Pointer to FLEXIO_I2S_Type structure.
 * param handle FlexIO I2S handle pointer.
 * param format Pointer to audio data format structure.
 * param srcClock_Hz FlexIO I2S bit clock source frequency in Hz. This parameter should be 0 while in slave mode.
 */
void FLEXIO_I2S_TransferSetFormat(FLEXIO_I2S_Type *base,
                                  flexio_i2s_handle_t *handle,
                                  flexio_i2s_format_t *format,
                                  uint32_t srcClock_Hz)
{
    assert(handle && format);

    /* Set the bitWidth to handle */
    handle->bitWidth = format->bitWidth;

    /* Set sample rate */
    if (srcClock_Hz != 0)
    {
        /* It is master */
        FLEXIO_I2S_MasterSetFormat(base, format, srcClock_Hz);
    }
    else
    {
        FLEXIO_I2S_SlaveSetFormat(base, format);
    }
}

/*!
 * brief Performs an interrupt non-blocking send transfer on FlexIO I2S.
 *
 * note The API returns immediately after transfer initiates.
 * Call FLEXIO_I2S_GetRemainingBytes to poll the transfer status and check whether
 * the transfer is finished. If the return status is 0, the transfer is finished.
 *
 * param base Pointer to FLEXIO_I2S_Type structure.
 * param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 * param xfer Pointer to flexio_i2s_transfer_t structure
 * retval kStatus_Success Successfully start the data transmission.
 * retval kStatus_FLEXIO_I2S_TxBusy Previous transmission still not finished, data not all written to TX register yet.
 * retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t FLEXIO_I2S_TransferSendNonBlocking(FLEXIO_I2S_Type *base,
                                            flexio_i2s_handle_t *handle,
                                            flexio_i2s_transfer_t *xfer)
{
    assert(handle);

    /* Check if the queue is full */
    if (handle->queue[handle->queueUser].data)
    {
        return kStatus_FLEXIO_I2S_QueueFull;
    }
    if ((xfer->dataSize == 0) || (xfer->data == NULL))
    {
        return kStatus_InvalidArgument;
    }

    /* Add into queue */
    handle->queue[handle->queueUser].data     = xfer->data;
    handle->queue[handle->queueUser].dataSize = xfer->dataSize;
    handle->transferSize[handle->queueUser]   = xfer->dataSize;
    handle->queueUser                         = (handle->queueUser + 1) % FLEXIO_I2S_XFER_QUEUE_SIZE;

    /* Set the state to busy */
    handle->state = kFLEXIO_I2S_Busy;

    FLEXIO_I2S_EnableInterrupts(base, kFLEXIO_I2S_TxDataRegEmptyInterruptEnable);

    /* Enable Tx transfer */
    FLEXIO_I2S_Enable(base, true);

    return kStatus_Success;
}

/*!
 * brief Performs an interrupt non-blocking receive transfer on FlexIO I2S.
 *
 * note The API returns immediately after transfer initiates.
 * Call FLEXIO_I2S_GetRemainingBytes to poll the transfer status to check whether
 * the transfer is finished. If the return status is 0, the transfer is finished.
 *
 * param base Pointer to FLEXIO_I2S_Type structure.
 * param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 * param xfer Pointer to flexio_i2s_transfer_t structure
 * retval kStatus_Success Successfully start the data receive.
 * retval kStatus_FLEXIO_I2S_RxBusy Previous receive still not finished.
 * retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t FLEXIO_I2S_TransferReceiveNonBlocking(FLEXIO_I2S_Type *base,
                                               flexio_i2s_handle_t *handle,
                                               flexio_i2s_transfer_t *xfer)
{
    assert(handle);

    /* Check if the queue is full */
    if (handle->queue[handle->queueUser].data)
    {
        return kStatus_FLEXIO_I2S_QueueFull;
    }

    if ((xfer->dataSize == 0) || (xfer->data == NULL))
    {
        return kStatus_InvalidArgument;
    }

    /* Add into queue */
    handle->queue[handle->queueUser].data     = xfer->data;
    handle->queue[handle->queueUser].dataSize = xfer->dataSize;
    handle->transferSize[handle->queueUser]   = xfer->dataSize;
    handle->queueUser                         = (handle->queueUser + 1) % FLEXIO_I2S_XFER_QUEUE_SIZE;

    /* Set state to busy */
    handle->state = kFLEXIO_I2S_Busy;

    /* Enable interrupt */
    FLEXIO_I2S_EnableInterrupts(base, kFLEXIO_I2S_RxDataRegFullInterruptEnable);

    /* Enable Rx transfer */
    FLEXIO_I2S_Enable(base, true);

    return kStatus_Success;
}

/*!
 * brief Aborts the current send.
 *
 * note This API can be called at any time when interrupt non-blocking transfer initiates
 * to abort the transfer in a early time.
 *
 * param base Pointer to FLEXIO_I2S_Type structure.
 * param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 */
void FLEXIO_I2S_TransferAbortSend(FLEXIO_I2S_Type *base, flexio_i2s_handle_t *handle)
{
    assert(handle);

    /* Stop Tx transfer and disable interrupt */
    FLEXIO_I2S_DisableInterrupts(base, kFLEXIO_I2S_TxDataRegEmptyInterruptEnable);
    handle->state = kFLEXIO_I2S_Idle;

    /* Clear the queue */
    memset(handle->queue, 0, sizeof(flexio_i2s_transfer_t) * FLEXIO_I2S_XFER_QUEUE_SIZE);
    handle->queueDriver = 0;
    handle->queueUser   = 0;
}

/*!
 * brief Aborts the current receive.
 *
 * note This API can be called at any time when interrupt non-blocking transfer initiates
 * to abort the transfer in a early time.
 *
 * param base Pointer to FLEXIO_I2S_Type structure.
 * param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 */
void FLEXIO_I2S_TransferAbortReceive(FLEXIO_I2S_Type *base, flexio_i2s_handle_t *handle)
{
    assert(handle);

    /* Stop rx transfer and disable interrupt */
    FLEXIO_I2S_DisableInterrupts(base, kFLEXIO_I2S_RxDataRegFullInterruptEnable);
    handle->state = kFLEXIO_I2S_Idle;

    /* Clear the queue */
    memset(handle->queue, 0, sizeof(flexio_i2s_transfer_t) * FLEXIO_I2S_XFER_QUEUE_SIZE);
    handle->queueDriver = 0;
    handle->queueUser   = 0;
}

/*!
 * brief Gets the remaining bytes to be sent.
 *
 * param base Pointer to FLEXIO_I2S_Type structure.
 * param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 * param count Bytes sent.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t FLEXIO_I2S_TransferGetSendCount(FLEXIO_I2S_Type *base, flexio_i2s_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kFLEXIO_I2S_Busy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = (handle->transferSize[handle->queueDriver] - handle->queue[handle->queueDriver].dataSize);
    }

    return status;
}

/*!
 * brief Gets the remaining bytes to be received.
 *
 * param base Pointer to FLEXIO_I2S_Type structure.
 * param handle Pointer to flexio_i2s_handle_t structure which stores the transfer state
 * return count Bytes received.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t FLEXIO_I2S_TransferGetReceiveCount(FLEXIO_I2S_Type *base, flexio_i2s_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kFLEXIO_I2S_Busy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = (handle->transferSize[handle->queueDriver] - handle->queue[handle->queueDriver].dataSize);
    }

    return status;
}

/*!
 * brief Tx interrupt handler.
 *
 * param i2sBase Pointer to FLEXIO_I2S_Type structure.
 * param i2sHandle Pointer to flexio_i2s_handle_t structure
 */
void FLEXIO_I2S_TransferTxHandleIRQ(void *i2sBase, void *i2sHandle)
{
    assert(i2sHandle);

    flexio_i2s_handle_t *handle = (flexio_i2s_handle_t *)i2sHandle;
    FLEXIO_I2S_Type *base       = (FLEXIO_I2S_Type *)i2sBase;
    uint8_t *buffer             = handle->queue[handle->queueDriver].data;
    uint8_t dataSize            = handle->bitWidth / 8U;

    /* Handle error */
    if (FLEXIO_GetShifterErrorFlags(base->flexioBase) & (1U << base->txShifterIndex))
    {
        FLEXIO_ClearShifterErrorFlags(base->flexioBase, (1U << base->txShifterIndex));
    }
    /* Handle transfer */
    if (((FLEXIO_I2S_GetStatusFlags(base) & kFLEXIO_I2S_TxDataRegEmptyFlag) != 0) &&
        (handle->queue[handle->queueDriver].data != NULL))
    {
        FLEXIO_I2S_WriteNonBlocking(base, handle->bitWidth, buffer, dataSize);

        /* Update internal counter */
        handle->queue[handle->queueDriver].dataSize -= dataSize;
        handle->queue[handle->queueDriver].data += dataSize;
    }

    /* If finished a block, call the callback function */
    if ((handle->queue[handle->queueDriver].dataSize == 0U) && (handle->queue[handle->queueDriver].data != NULL))
    {
        memset(&handle->queue[handle->queueDriver], 0, sizeof(flexio_i2s_transfer_t));
        handle->queueDriver = (handle->queueDriver + 1) % FLEXIO_I2S_XFER_QUEUE_SIZE;
        if (handle->callback)
        {
            (handle->callback)(base, handle, kStatus_Success, handle->userData);
        }
    }

    /* If all data finished, just stop the transfer */
    if (handle->queue[handle->queueDriver].data == NULL)
    {
        FLEXIO_I2S_TransferAbortSend(base, handle);
    }
}

/*!
 * brief Rx interrupt handler.
 *
 * param i2sBase Pointer to FLEXIO_I2S_Type structure.
 * param i2sHandle Pointer to flexio_i2s_handle_t structure.
 */
void FLEXIO_I2S_TransferRxHandleIRQ(void *i2sBase, void *i2sHandle)
{
    assert(i2sHandle);

    flexio_i2s_handle_t *handle = (flexio_i2s_handle_t *)i2sHandle;
    FLEXIO_I2S_Type *base       = (FLEXIO_I2S_Type *)i2sBase;
    uint8_t *buffer             = handle->queue[handle->queueDriver].data;
    uint8_t dataSize            = handle->bitWidth / 8U;

    /* Handle transfer */
    if (((FLEXIO_I2S_GetStatusFlags(base) & kFLEXIO_I2S_RxDataRegFullFlag) != 0) &&
        (handle->queue[handle->queueDriver].data != NULL))
    {
        FLEXIO_I2S_ReadNonBlocking(base, handle->bitWidth, buffer, dataSize);

        /* Update internal state */
        handle->queue[handle->queueDriver].dataSize -= dataSize;
        handle->queue[handle->queueDriver].data += dataSize;
    }

    /* If finished a block, call the callback function */
    if ((handle->queue[handle->queueDriver].dataSize == 0U) && (handle->queue[handle->queueDriver].data != NULL))
    {
        memset(&handle->queue[handle->queueDriver], 0, sizeof(flexio_i2s_transfer_t));
        handle->queueDriver = (handle->queueDriver + 1) % FLEXIO_I2S_XFER_QUEUE_SIZE;
        if (handle->callback)
        {
            (handle->callback)(base, handle, kStatus_Success, handle->userData);
        }
    }

    /* If all data finished, just stop the transfer */
    if (handle->queue[handle->queueDriver].data == NULL)
    {
        FLEXIO_I2S_TransferAbortReceive(base, handle);
    }
}
