/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_uart"
#endif

/*<! @brief uart transfer state. */
enum _flexio_uart_transfer_states
{
    kFLEXIO_UART_TxIdle, /* TX idle. */
    kFLEXIO_UART_TxBusy, /* TX busy. */
    kFLEXIO_UART_RxIdle, /* RX idle. */
    kFLEXIO_UART_RxBusy  /* RX busy. */
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get the length of received data in RX ring buffer.
 *
 * @param handle FLEXIO UART handle pointer.
 * @return Length of received data in RX ring buffer.
 */
static size_t FLEXIO_UART_TransferGetRxRingBufferLength(flexio_uart_handle_t *handle);

/*!
 * @brief Check whether the RX ring buffer is full.
 *
 * @param handle FLEXIO UART handle pointer.
 * @retval true  RX ring buffer is full.
 * @retval false RX ring buffer is not full.
 */
static bool FLEXIO_UART_TransferIsRxRingBufferFull(flexio_uart_handle_t *handle);

/*******************************************************************************
 * Codes
 ******************************************************************************/

static uint32_t FLEXIO_UART_GetInstance(FLEXIO_UART_Type *base)
{
    return FLEXIO_GetInstance(base->flexioBase);
}

static size_t FLEXIO_UART_TransferGetRxRingBufferLength(flexio_uart_handle_t *handle)
{
    size_t size;

    if (handle->rxRingBufferTail > handle->rxRingBufferHead)
    {
        size = (size_t)(handle->rxRingBufferHead + handle->rxRingBufferSize - handle->rxRingBufferTail);
    }
    else
    {
        size = (size_t)(handle->rxRingBufferHead - handle->rxRingBufferTail);
    }

    return size;
}

static bool FLEXIO_UART_TransferIsRxRingBufferFull(flexio_uart_handle_t *handle)
{
    bool full;

    if (FLEXIO_UART_TransferGetRxRingBufferLength(handle) == (handle->rxRingBufferSize - 1U))
    {
        full = true;
    }
    else
    {
        full = false;
    }

    return full;
}

/*!
 * brief Ungates the FlexIO clock, resets the FlexIO module, configures FlexIO UART
 * hardware, and configures the FlexIO UART with FlexIO UART configuration.
 * The configuration structure can be filled by the user or be set with
 * default values by FLEXIO_UART_GetDefaultConfig().
 *
 * Example
   code
   FLEXIO_UART_Type base = {
   .flexioBase = FLEXIO,
   .TxPinIndex = 0,
   .RxPinIndex = 1,
   .shifterIndex = {0,1},
   .timerIndex = {0,1}
   };
   flexio_uart_config_t config = {
   .enableInDoze = false,
   .enableInDebug = true,
   .enableFastAccess = false,
   .baudRate_Bps = 115200U,
   .bitCountPerChar = 8
   };
   FLEXIO_UART_Init(base, &config, srcClock_Hz);
   endcode
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param userConfig Pointer to the flexio_uart_config_t structure.
 * param srcClock_Hz FlexIO source clock in Hz.
 * retval kStatus_Success Configuration success
 * retval kStatus_InvalidArgument Buadrate configuration out of range
*/
status_t FLEXIO_UART_Init(FLEXIO_UART_Type *base, const flexio_uart_config_t *userConfig, uint32_t srcClock_Hz)
{
    assert(base && userConfig);

    flexio_shifter_config_t shifterConfig;
    flexio_timer_config_t timerConfig;
    uint32_t ctrlReg  = 0;
    uint16_t timerDiv = 0;
    uint16_t timerCmp = 0;
    status_t result   = kStatus_Success;

    /* Clear the shifterConfig & timerConfig struct. */
    memset(&shifterConfig, 0, sizeof(shifterConfig));
    memset(&timerConfig, 0, sizeof(timerConfig));

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Ungate flexio clock. */
    CLOCK_EnableClock(s_flexioClocks[FLEXIO_UART_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Configure FLEXIO UART */
    ctrlReg = base->flexioBase->CTRL;
    ctrlReg &= ~(FLEXIO_CTRL_DOZEN_MASK | FLEXIO_CTRL_DBGE_MASK | FLEXIO_CTRL_FASTACC_MASK | FLEXIO_CTRL_FLEXEN_MASK);
    ctrlReg |= (FLEXIO_CTRL_DBGE(userConfig->enableInDebug) | FLEXIO_CTRL_FASTACC(userConfig->enableFastAccess) |
                FLEXIO_CTRL_FLEXEN(userConfig->enableUart));
    if (!userConfig->enableInDoze)
    {
        ctrlReg |= FLEXIO_CTRL_DOZEN_MASK;
    }

    base->flexioBase->CTRL = ctrlReg;

    /* Do hardware configuration. */
    /* 1. Configure the shifter 0 for tx. */
    shifterConfig.timerSelect   = base->timerIndex[0];
    shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
    shifterConfig.pinConfig     = kFLEXIO_PinConfigOutput;
    shifterConfig.pinSelect     = base->TxPinIndex;
    shifterConfig.pinPolarity   = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode   = kFLEXIO_ShifterModeTransmit;
    shifterConfig.inputSource   = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop   = kFLEXIO_ShifterStopBitHigh;
    shifterConfig.shifterStart  = kFLEXIO_ShifterStartBitLow;

    FLEXIO_SetShifterConfig(base->flexioBase, base->shifterIndex[0], &shifterConfig);

    /*2. Configure the timer 0 for tx. */
    timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(base->shifterIndex[0]);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow;
    timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig       = kFLEXIO_PinConfigOutputDisabled;
    timerConfig.pinSelect       = base->TxPinIndex;
    timerConfig.pinPolarity     = kFLEXIO_PinActiveHigh;
    timerConfig.timerMode       = kFLEXIO_TimerModeDual8BitBaudBit;
    timerConfig.timerOutput     = kFLEXIO_TimerOutputOneNotAffectedByReset;
    timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timerConfig.timerReset      = kFLEXIO_TimerResetNever;
    timerConfig.timerDisable    = kFLEXIO_TimerDisableOnTimerCompare;
    timerConfig.timerEnable     = kFLEXIO_TimerEnableOnTriggerHigh;
    timerConfig.timerStop       = kFLEXIO_TimerStopBitEnableOnTimerDisable;
    timerConfig.timerStart      = kFLEXIO_TimerStartBitEnabled;

    timerDiv = srcClock_Hz / userConfig->baudRate_Bps;
    timerDiv = timerDiv / 2 - 1;

    if (timerDiv > 0xFFU)
    {
        result = kStatus_InvalidArgument;
    }

    timerCmp = ((uint32_t)(userConfig->bitCountPerChar * 2 - 1)) << 8U;
    timerCmp |= timerDiv;

    timerConfig.timerCompare = timerCmp;

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIndex[0], &timerConfig);

    /* 3. Configure the shifter 1 for rx. */
    shifterConfig.timerSelect   = base->timerIndex[1];
    shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnNegitive;
    shifterConfig.pinConfig     = kFLEXIO_PinConfigOutputDisabled;
    shifterConfig.pinSelect     = base->RxPinIndex;
    shifterConfig.pinPolarity   = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode   = kFLEXIO_ShifterModeReceive;
    shifterConfig.inputSource   = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop   = kFLEXIO_ShifterStopBitHigh;
    shifterConfig.shifterStart  = kFLEXIO_ShifterStartBitLow;

    FLEXIO_SetShifterConfig(base->flexioBase, base->shifterIndex[1], &shifterConfig);

    /* 4. Configure the timer 1 for rx. */
    timerConfig.triggerSelect   = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(base->RxPinIndex);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
    timerConfig.triggerSource   = kFLEXIO_TimerTriggerSourceExternal;
    timerConfig.pinConfig       = kFLEXIO_PinConfigOutputDisabled;
    timerConfig.pinSelect       = base->RxPinIndex;
    timerConfig.pinPolarity     = kFLEXIO_PinActiveLow;
    timerConfig.timerMode       = kFLEXIO_TimerModeDual8BitBaudBit;
    timerConfig.timerOutput     = kFLEXIO_TimerOutputOneAffectedByReset;
    timerConfig.timerDecrement  = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timerConfig.timerReset      = kFLEXIO_TimerResetOnTimerPinRisingEdge;
    timerConfig.timerDisable    = kFLEXIO_TimerDisableOnTimerCompare;
    timerConfig.timerEnable     = kFLEXIO_TimerEnableOnPinRisingEdge;
    timerConfig.timerStop       = kFLEXIO_TimerStopBitEnableOnTimerDisable;
    timerConfig.timerStart      = kFLEXIO_TimerStartBitEnabled;

    timerConfig.timerCompare = timerCmp;

    FLEXIO_SetTimerConfig(base->flexioBase, base->timerIndex[1], &timerConfig);

    return result;
}

/*!
 * brief Resets the FlexIO UART shifter and timer config.
 *
 * note After calling this API, call the FLEXO_UART_Init to use the FlexIO UART module.
 *
 * param base Pointer to FLEXIO_UART_Type structure
 */
void FLEXIO_UART_Deinit(FLEXIO_UART_Type *base)
{
    base->flexioBase->SHIFTCFG[base->shifterIndex[0]] = 0;
    base->flexioBase->SHIFTCTL[base->shifterIndex[0]] = 0;
    base->flexioBase->SHIFTCFG[base->shifterIndex[1]] = 0;
    base->flexioBase->SHIFTCTL[base->shifterIndex[1]] = 0;
    base->flexioBase->TIMCFG[base->timerIndex[0]]     = 0;
    base->flexioBase->TIMCMP[base->timerIndex[0]]     = 0;
    base->flexioBase->TIMCTL[base->timerIndex[0]]     = 0;
    base->flexioBase->TIMCFG[base->timerIndex[1]]     = 0;
    base->flexioBase->TIMCMP[base->timerIndex[1]]     = 0;
    base->flexioBase->TIMCTL[base->timerIndex[1]]     = 0;
    /* Clear the shifter flag. */
    base->flexioBase->SHIFTSTAT = (1U << base->shifterIndex[0]);
    base->flexioBase->SHIFTSTAT = (1U << base->shifterIndex[1]);
    /* Clear the timer flag. */
    base->flexioBase->TIMSTAT = (1U << base->timerIndex[0]);
    base->flexioBase->TIMSTAT = (1U << base->timerIndex[1]);
}

/*!
 * brief Gets the default configuration to configure the FlexIO UART. The configuration
 * can be used directly for calling the FLEXIO_UART_Init().
 * Example:
   code
   flexio_uart_config_t config;
   FLEXIO_UART_GetDefaultConfig(&userConfig);
   endcode
 * param userConfig Pointer to the flexio_uart_config_t structure.
*/
void FLEXIO_UART_GetDefaultConfig(flexio_uart_config_t *userConfig)
{
    assert(userConfig);

    /* Initializes the configure structure to zero. */
    memset(userConfig, 0, sizeof(*userConfig));

    userConfig->enableUart       = true;
    userConfig->enableInDoze     = false;
    userConfig->enableInDebug    = true;
    userConfig->enableFastAccess = false;
    /* Default baud rate 115200. */
    userConfig->baudRate_Bps = 115200U;
    /* Default bit count at 8. */
    userConfig->bitCountPerChar = kFLEXIO_UART_8BitsPerChar;
}

/*!
 * brief Enables the FlexIO UART interrupt.
 *
 * This function enables the FlexIO UART interrupt.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param mask Interrupt source.
 */
void FLEXIO_UART_EnableInterrupts(FLEXIO_UART_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_UART_TxDataRegEmptyInterruptEnable)
    {
        FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, 1U << base->shifterIndex[0]);
    }
    if (mask & kFLEXIO_UART_RxDataRegFullInterruptEnable)
    {
        FLEXIO_EnableShifterStatusInterrupts(base->flexioBase, 1U << base->shifterIndex[1]);
    }
}

/*!
 * brief Disables the FlexIO UART interrupt.
 *
 * This function disables the FlexIO UART interrupt.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param mask Interrupt source.
 */
void FLEXIO_UART_DisableInterrupts(FLEXIO_UART_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_UART_TxDataRegEmptyInterruptEnable)
    {
        FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, 1U << base->shifterIndex[0]);
    }
    if (mask & kFLEXIO_UART_RxDataRegFullInterruptEnable)
    {
        FLEXIO_DisableShifterStatusInterrupts(base->flexioBase, 1U << base->shifterIndex[1]);
    }
}

/*!
 * brief Gets the FlexIO UART status flags.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * return FlexIO UART status flags.
 */

uint32_t FLEXIO_UART_GetStatusFlags(FLEXIO_UART_Type *base)
{
    uint32_t status = 0;
    status =
        ((FLEXIO_GetShifterStatusFlags(base->flexioBase) & (1U << base->shifterIndex[0])) >> base->shifterIndex[0]);
    status |=
        (((FLEXIO_GetShifterStatusFlags(base->flexioBase) & (1U << base->shifterIndex[1])) >> (base->shifterIndex[1]))
         << 1U);
    status |=
        (((FLEXIO_GetShifterErrorFlags(base->flexioBase) & (1U << base->shifterIndex[1])) >> (base->shifterIndex[1]))
         << 2U);
    return status;
}

/*!
 * brief Gets the FlexIO UART status flags.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param mask Status flag.
 *      The parameter can be any combination of the following values:
 *          arg kFLEXIO_UART_TxDataRegEmptyFlag
 *          arg kFLEXIO_UART_RxEmptyFlag
 *          arg kFLEXIO_UART_RxOverRunFlag
 */

void FLEXIO_UART_ClearStatusFlags(FLEXIO_UART_Type *base, uint32_t mask)
{
    if (mask & kFLEXIO_UART_TxDataRegEmptyFlag)
    {
        FLEXIO_ClearShifterStatusFlags(base->flexioBase, 1U << base->shifterIndex[0]);
    }
    if (mask & kFLEXIO_UART_RxDataRegFullFlag)
    {
        FLEXIO_ClearShifterStatusFlags(base->flexioBase, 1U << base->shifterIndex[1]);
    }
    if (mask & kFLEXIO_UART_RxOverRunFlag)
    {
        FLEXIO_ClearShifterErrorFlags(base->flexioBase, 1U << base->shifterIndex[1]);
    }
}

/*!
 * brief Sends a buffer of data bytes.
 *
 * note This function blocks using the polling method until all bytes have been sent.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param txData The data bytes to send.
 * param txSize The number of data bytes to send.
 */
void FLEXIO_UART_WriteBlocking(FLEXIO_UART_Type *base, const uint8_t *txData, size_t txSize)
{
    assert(txData);
    assert(txSize);

    while (txSize--)
    {
        /* Wait until data transfer complete. */
        while (!(FLEXIO_GetShifterStatusFlags(base->flexioBase) & (1U << base->shifterIndex[0])))
        {
        }

        base->flexioBase->SHIFTBUF[base->shifterIndex[0]] = *txData++;
    }
}

/*!
 * brief Receives a buffer of bytes.
 *
 * note This function blocks using the polling method until all bytes have been received.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param rxData The buffer to store the received bytes.
 * param rxSize The number of data bytes to be received.
 */
void FLEXIO_UART_ReadBlocking(FLEXIO_UART_Type *base, uint8_t *rxData, size_t rxSize)
{
    assert(rxData);
    assert(rxSize);

    while (rxSize--)
    {
        /* Wait until data transfer complete. */
        while (!(FLEXIO_UART_GetStatusFlags(base) & kFLEXIO_UART_RxDataRegFullFlag))
        {
        }

        *rxData++ = base->flexioBase->SHIFTBUFBYS[base->shifterIndex[1]];
    }
}

/*!
 * brief Initializes the UART handle.
 *
 * This function initializes the FlexIO UART handle, which can be used for other FlexIO
 * UART transactional APIs. Call this API once to get the
 * initialized handle.
 *
 * The UART driver supports the "background" receiving, which means that users can set up
 * a RX ring buffer optionally. Data received is stored into the ring buffer even when
 * the user doesn't call the FLEXIO_UART_TransferReceiveNonBlocking() API. If there is already data
 * received in the ring buffer, users can get the received data from the ring buffer
 * directly. The ring buffer is disabled if passing NULL as p ringBuffer.
 *
 * param base to FLEXIO_UART_Type structure.
 * param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * param callback The callback function.
 * param userData The parameter of the callback function.
 * retval kStatus_Success Successfully create the handle.
 * retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_UART_TransferCreateHandle(FLEXIO_UART_Type *base,
                                          flexio_uart_handle_t *handle,
                                          flexio_uart_transfer_callback_t callback,
                                          void *userData)
{
    assert(handle);

    IRQn_Type flexio_irqs[] = FLEXIO_IRQS;

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Set the TX/RX state. */
    handle->rxState = kFLEXIO_UART_RxIdle;
    handle->txState = kFLEXIO_UART_TxIdle;

    /* Set the callback and user data. */
    handle->callback = callback;
    handle->userData = userData;

    /* Enable interrupt in NVIC. */
    EnableIRQ(flexio_irqs[FLEXIO_UART_GetInstance(base)]);

    /* Save the context in global variables to support the double weak mechanism. */
    return FLEXIO_RegisterHandleIRQ(base, handle, FLEXIO_UART_TransferHandleIRQ);
}

/*!
 * brief Sets up the RX ring buffer.
 *
 * This function sets up the RX ring buffer to a specific UART handle.
 *
 * When the RX ring buffer is used, data received is stored into the ring buffer even when
 * the user doesn't call the UART_ReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, users can get the received data from the ring buffer directly.
 *
 * note When using the RX ring buffer, one byte is reserved for internal use. In other
 * words, if p ringBufferSize is 32, only 31 bytes are used for saving data.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * param ringBuffer Start address of ring buffer for background receiving. Pass NULL to disable the ring buffer.
 * param ringBufferSize Size of the ring buffer.
 */
void FLEXIO_UART_TransferStartRingBuffer(FLEXIO_UART_Type *base,
                                         flexio_uart_handle_t *handle,
                                         uint8_t *ringBuffer,
                                         size_t ringBufferSize)
{
    assert(handle);

    /* Setup the ringbuffer address */
    if (ringBuffer)
    {
        handle->rxRingBuffer     = ringBuffer;
        handle->rxRingBufferSize = ringBufferSize;
        handle->rxRingBufferHead = 0U;
        handle->rxRingBufferTail = 0U;

        /* Enable the interrupt to accept the data when user need the ring buffer. */
        FLEXIO_UART_EnableInterrupts(base, kFLEXIO_UART_RxDataRegFullInterruptEnable);
    }
}

/*!
 * brief Aborts the background transfer and uninstalls the ring buffer.
 *
 * This function aborts the background transfer and uninstalls the ring buffer.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 */
void FLEXIO_UART_TransferStopRingBuffer(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle)
{
    assert(handle);

    if (handle->rxState == kFLEXIO_UART_RxIdle)
    {
        FLEXIO_UART_DisableInterrupts(base, kFLEXIO_UART_RxDataRegFullInterruptEnable);
    }

    handle->rxRingBuffer     = NULL;
    handle->rxRingBufferSize = 0U;
    handle->rxRingBufferHead = 0U;
    handle->rxRingBufferTail = 0U;
}

/*!
 * brief Transmits a buffer of data using the interrupt method.
 *
 * This function sends data using an interrupt method. This is a non-blocking function,
 * which returns directly without waiting for all data to be written to the TX register. When
 * all data is written to the TX register in ISR, the FlexIO UART driver calls the callback
 * function and passes the ref kStatus_FLEXIO_UART_TxIdle as status parameter.
 *
 * note The kStatus_FLEXIO_UART_TxIdle is passed to the upper layer when all data is written
 * to the TX register. However, it does not ensure that all data is sent out.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * param xfer FlexIO UART transfer structure. See #flexio_uart_transfer_t.
 * retval kStatus_Success Successfully starts the data transmission.
 * retval kStatus_UART_TxBusy Previous transmission still not finished, data not written to the TX register.
 */
status_t FLEXIO_UART_TransferSendNonBlocking(FLEXIO_UART_Type *base,
                                             flexio_uart_handle_t *handle,
                                             flexio_uart_transfer_t *xfer)
{
    status_t status;

    /* Return error if xfer invalid. */
    if ((0U == xfer->dataSize) || (NULL == xfer->data))
    {
        return kStatus_InvalidArgument;
    }

    /* Return error if current TX busy. */
    if (kFLEXIO_UART_TxBusy == handle->txState)
    {
        status = kStatus_FLEXIO_UART_TxBusy;
    }
    else
    {
        handle->txData        = xfer->data;
        handle->txDataSize    = xfer->dataSize;
        handle->txDataSizeAll = xfer->dataSize;
        handle->txState       = kFLEXIO_UART_TxBusy;

        /* Enable transmiter interrupt. */
        FLEXIO_UART_EnableInterrupts(base, kFLEXIO_UART_TxDataRegEmptyInterruptEnable);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Aborts the interrupt-driven data transmit.
 *
 * This function aborts the interrupt-driven data sending. Get the remainBytes to find out
 * how many bytes are still not sent out.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 */
void FLEXIO_UART_TransferAbortSend(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle)
{
    /* Disable the transmitter and disable the interrupt. */
    FLEXIO_UART_DisableInterrupts(base, kFLEXIO_UART_TxDataRegEmptyInterruptEnable);

    handle->txDataSize = 0;
    handle->txState    = kFLEXIO_UART_TxIdle;
}

/*!
 * brief Gets the number of bytes sent.
 *
 * This function gets the number of bytes sent driven by interrupt.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * param count Number of bytes sent so far by the non-blocking transaction.
 * retval kStatus_NoTransferInProgress transfer has finished or no transfer in progress.
 * retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_UART_TransferGetSendCount(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle, size_t *count)
{
    assert(handle);
    assert(count);

    if (kFLEXIO_UART_TxIdle == handle->txState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->txDataSizeAll - handle->txDataSize;

    return kStatus_Success;
}

/*!
 * brief Receives a buffer of data using the interrupt method.
 *
 * This function receives data using the interrupt method. This is a non-blocking function,
 * which returns without waiting for all data to be received.
 * If the RX ring buffer is used and not empty, the data in ring buffer is copied and
 * the parameter p receivedBytes shows how many bytes are copied from the ring buffer.
 * After copying, if the data in ring buffer is not enough to read, the receive
 * request is saved by the UART driver. When new data arrives, the receive request
 * is serviced first. When all data is received, the UART driver notifies the upper layer
 * through a callback function and passes the status parameter ref kStatus_UART_RxIdle.
 * For example, if the upper layer needs 10 bytes but there are only 5 bytes in the ring buffer,
 * the 5 bytes are copied to xfer->data. This function returns with the
 * parameter p receivedBytes set to 5. For the last 5 bytes, newly arrived data is
 * saved from the xfer->data[5]. When 5 bytes are received, the UART driver notifies upper layer.
 * If the RX ring buffer is not enabled, this function enables the RX and RX interrupt
 * to receive data to xfer->data. When all data is received, the upper layer is notified.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * param xfer UART transfer structure. See #flexio_uart_transfer_t.
 * param receivedBytes Bytes received from the ring buffer directly.
 * retval kStatus_Success Successfully queue the transfer into the transmit queue.
 * retval kStatus_FLEXIO_UART_RxBusy Previous receive request is not finished.
 */
status_t FLEXIO_UART_TransferReceiveNonBlocking(FLEXIO_UART_Type *base,
                                                flexio_uart_handle_t *handle,
                                                flexio_uart_transfer_t *xfer,
                                                size_t *receivedBytes)
{
    uint32_t i;
    status_t status;
    /* How many bytes to copy from ring buffer to user memory. */
    size_t bytesToCopy = 0U;
    /* How many bytes to receive. */
    size_t bytesToReceive;
    /* How many bytes currently have received. */
    size_t bytesCurrentReceived;

    /* Return error if xfer invalid. */
    if ((0U == xfer->dataSize) || (NULL == xfer->data))
    {
        return kStatus_InvalidArgument;
    }

    /* How to get data:
       1. If RX ring buffer is not enabled, then save xfer->data and xfer->dataSize
          to uart handle, enable interrupt to store received data to xfer->data. When
          all data received, trigger callback.
       2. If RX ring buffer is enabled and not empty, get data from ring buffer first.
          If there are enough data in ring buffer, copy them to xfer->data and return.
          If there are not enough data in ring buffer, copy all of them to xfer->data,
          save the xfer->data remained empty space to uart handle, receive data
          to this empty space and trigger callback when finished. */

    if (kFLEXIO_UART_RxBusy == handle->rxState)
    {
        status = kStatus_FLEXIO_UART_RxBusy;
    }
    else
    {
        bytesToReceive       = xfer->dataSize;
        bytesCurrentReceived = 0U;

        /* If RX ring buffer is used. */
        if (handle->rxRingBuffer)
        {
            /* Disable FLEXIO_UART RX IRQ, protect ring buffer. */
            FLEXIO_UART_DisableInterrupts(base, kFLEXIO_UART_RxDataRegFullInterruptEnable);

            /* How many bytes in RX ring buffer currently. */
            bytesToCopy = FLEXIO_UART_TransferGetRxRingBufferLength(handle);

            if (bytesToCopy)
            {
                bytesToCopy = MIN(bytesToReceive, bytesToCopy);

                bytesToReceive -= bytesToCopy;

                /* Copy data from ring buffer to user memory. */
                for (i = 0U; i < bytesToCopy; i++)
                {
                    xfer->data[bytesCurrentReceived++] = handle->rxRingBuffer[handle->rxRingBufferTail];

                    /* Wrap to 0. Not use modulo (%) because it might be large and slow. */
                    if (handle->rxRingBufferTail + 1U == handle->rxRingBufferSize)
                    {
                        handle->rxRingBufferTail = 0U;
                    }
                    else
                    {
                        handle->rxRingBufferTail++;
                    }
                }
            }

            /* If ring buffer does not have enough data, still need to read more data. */
            if (bytesToReceive)
            {
                /* No data in ring buffer, save the request to UART handle. */
                handle->rxData        = xfer->data + bytesCurrentReceived;
                handle->rxDataSize    = bytesToReceive;
                handle->rxDataSizeAll = bytesToReceive;
                handle->rxState       = kFLEXIO_UART_RxBusy;
            }

            /* Enable FLEXIO_UART RX IRQ if previously enabled. */
            FLEXIO_UART_EnableInterrupts(base, kFLEXIO_UART_RxDataRegFullInterruptEnable);

            /* Call user callback since all data are received. */
            if (0 == bytesToReceive)
            {
                if (handle->callback)
                {
                    handle->callback(base, handle, kStatus_FLEXIO_UART_RxIdle, handle->userData);
                }
            }
        }
        /* Ring buffer not used. */
        else
        {
            handle->rxData        = xfer->data + bytesCurrentReceived;
            handle->rxDataSize    = bytesToReceive;
            handle->rxDataSizeAll = bytesToReceive;
            handle->rxState       = kFLEXIO_UART_RxBusy;

            /* Enable RX interrupt. */
            FLEXIO_UART_EnableInterrupts(base, kFLEXIO_UART_RxDataRegFullInterruptEnable);
        }

        /* Return the how many bytes have read. */
        if (receivedBytes)
        {
            *receivedBytes = bytesCurrentReceived;
        }

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Aborts the receive data which was using IRQ.
 *
 * This function aborts the receive data which was using IRQ.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 */
void FLEXIO_UART_TransferAbortReceive(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle)
{
    /* Only abort the receive to handle->rxData, the RX ring buffer is still working. */
    if (!handle->rxRingBuffer)
    {
        /* Disable RX interrupt. */
        FLEXIO_UART_DisableInterrupts(base, kFLEXIO_UART_RxDataRegFullInterruptEnable);
    }

    handle->rxDataSize = 0U;
    handle->rxState    = kFLEXIO_UART_RxIdle;
}

/*!
 * brief Gets the number of bytes received.
 *
 * This function gets the number of bytes received driven by interrupt.
 *
 * param base Pointer to the FLEXIO_UART_Type structure.
 * param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * param count Number of bytes received so far by the non-blocking transaction.
 * retval kStatus_NoTransferInProgress transfer has finished or no transfer in progress.
 * retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_UART_TransferGetReceiveCount(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle, size_t *count)
{
    assert(handle);
    assert(count);

    if (kFLEXIO_UART_RxIdle == handle->rxState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->rxDataSizeAll - handle->rxDataSize;

    return kStatus_Success;
}

/*!
 * brief FlexIO UART IRQ handler function.
 *
 * This function processes the FlexIO UART transmit and receives the IRQ request.
 *
 * param uartType Pointer to the FLEXIO_UART_Type structure.
 * param uartHandle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 */
void FLEXIO_UART_TransferHandleIRQ(void *uartType, void *uartHandle)
{
    uint8_t count                = 1;
    FLEXIO_UART_Type *base       = (FLEXIO_UART_Type *)uartType;
    flexio_uart_handle_t *handle = (flexio_uart_handle_t *)uartHandle;

    /* Read the status back. */
    uint8_t status = FLEXIO_UART_GetStatusFlags(base);

    /* If RX overrun. */
    if (kFLEXIO_UART_RxOverRunFlag & status)
    {
        /* Clear Overrun flag. */
        FLEXIO_UART_ClearStatusFlags(base, kFLEXIO_UART_RxOverRunFlag);

        /* Trigger callback. */
        if (handle->callback)
        {
            handle->callback(base, handle, kStatus_FLEXIO_UART_RxHardwareOverrun, handle->userData);
        }
    }

    /* Receive data register full */
    if ((kFLEXIO_UART_RxDataRegFullFlag & status) && (base->flexioBase->SHIFTSIEN & (1U << base->shifterIndex[1])))
    {
        /* If handle->rxDataSize is not 0, first save data to handle->rxData. */
        if (handle->rxDataSize)
        {
            /* Using non block API to read the data from the registers. */
            FLEXIO_UART_ReadByte(base, handle->rxData);
            handle->rxDataSize--;
            handle->rxData++;
            count--;

            /* If all the data required for upper layer is ready, trigger callback. */
            if (!handle->rxDataSize)
            {
                handle->rxState = kFLEXIO_UART_RxIdle;

                if (handle->callback)
                {
                    handle->callback(base, handle, kStatus_FLEXIO_UART_RxIdle, handle->userData);
                }
            }
        }

        if (handle->rxRingBuffer)
        {
            if (count)
            {
                /* If RX ring buffer is full, trigger callback to notify over run. */
                if (FLEXIO_UART_TransferIsRxRingBufferFull(handle))
                {
                    if (handle->callback)
                    {
                        handle->callback(base, handle, kStatus_FLEXIO_UART_RxRingBufferOverrun, handle->userData);
                    }
                }

                /* If ring buffer is still full after callback function, the oldest data is overridden. */
                if (FLEXIO_UART_TransferIsRxRingBufferFull(handle))
                {
                    /* Increase handle->rxRingBufferTail to make room for new data. */
                    if (handle->rxRingBufferTail + 1U == handle->rxRingBufferSize)
                    {
                        handle->rxRingBufferTail = 0U;
                    }
                    else
                    {
                        handle->rxRingBufferTail++;
                    }
                }

                /* Read data. */
                handle->rxRingBuffer[handle->rxRingBufferHead] = base->flexioBase->SHIFTBUFBYS[base->shifterIndex[1]];

                /* Increase handle->rxRingBufferHead. */
                if (handle->rxRingBufferHead + 1U == handle->rxRingBufferSize)
                {
                    handle->rxRingBufferHead = 0U;
                }
                else
                {
                    handle->rxRingBufferHead++;
                }
            }
        }
        /* If no receive requst pending, stop RX interrupt. */
        else if (!handle->rxDataSize)
        {
            FLEXIO_UART_DisableInterrupts(base, kFLEXIO_UART_RxDataRegFullInterruptEnable);
        }
        else
        {
        }
    }

    /* Send data register empty and the interrupt is enabled. */
    if ((kFLEXIO_UART_TxDataRegEmptyFlag & status) && (base->flexioBase->SHIFTSIEN & (1U << base->shifterIndex[0])))
    {
        if (handle->txDataSize)
        {
            /* Using non block API to write the data to the registers. */
            FLEXIO_UART_WriteByte(base, handle->txData);
            handle->txData++;
            handle->txDataSize--;
            count--;

            /* If all the data are written to data register, TX finished. */
            if (!handle->txDataSize)
            {
                handle->txState = kFLEXIO_UART_TxIdle;

                /* Disable TX register empty interrupt. */
                FLEXIO_UART_DisableInterrupts(base, kFLEXIO_UART_TxDataRegEmptyInterruptEnable);

                /* Trigger callback. */
                if (handle->callback)
                {
                    handle->callback(base, handle, kStatus_FLEXIO_UART_TxIdle, handle->userData);
                }
            }
        }
    }
}
