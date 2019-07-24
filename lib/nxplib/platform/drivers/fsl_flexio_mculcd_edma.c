/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_mculcd_edma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_mculcd_edma"
#endif

#define EDMA_MAX_MAJOR_COUNT (DMA_CITER_ELINKNO_CITER_MASK >> DMA_CITER_ELINKNO_CITER_SHIFT)

enum _MCULCD_transfer_state
{
    kFLEXIO_MCULCD_StateIdle,           /*!< No transfer in progress. */
    kFLEXIO_MCULCD_StateReadArray,      /*!< Reading array in progress. */
    kFLEXIO_MCULCD_StateWriteArray,     /*!< Writing array in progress. */
    kFLEXIO_MCULCD_StateWriteSameValue, /*!< Writing the same value in progress.
                                         */
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief EDMA callback function for FLEXIO MCULCD TX.
 *
 * For details, see @ref edma_callback.
 */
static void FLEXIO_MCULCD_TxEDMACallback(edma_handle_t *DmaHandle, void *param, bool transferDone, uint32_t tcds);

/*!
 * @brief EDMA callback function for FLEXIO MCULCD RX.
 *
 * For details, see @ref edma_callback.
 */
static void FLEXIO_MCULCD_RxEDMACallback(edma_handle_t *DmaHandle, void *param, bool transferDone, uint32_t tcds);

/*!
 * @brief Set EDMA config for FLEXIO MCULCD transfer.
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle pointer to flexio_mculcd_edma_handle_t structure to store the
 * transfer state.
 */
static void FLEXIO_MCULCD_EDMAConfig(FLEXIO_MCULCD_Type *base, flexio_mculcd_edma_handle_t *handle);

/*!
 * @brief Convert the FlexIO shifter number to eDMA modulo.
 *
 * @param shifterNum The FlexIO shifter number.
 * @param modulo The modulo number.
 * @retval Get the modulo successfully.
 * @retval Could not get the modulo for the shifter number.
 */
static bool FLEXIO_MCULCD_GetEDMAModulo(uint8_t shifterNum, edma_modulo_t *modulo);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void FLEXIO_MCULCD_TxEDMACallback(edma_handle_t *DmaHandle, void *param, bool transferDone, uint32_t tcds)
{
    tcds                                            = tcds;
    flexio_mculcd_edma_handle_t *flexioLcdMcuHandle = (flexio_mculcd_edma_handle_t *)param;
    FLEXIO_MCULCD_Type *flexioLcdMcuBase            = flexioLcdMcuHandle->base;

    if (transferDone)
    {
        if (flexioLcdMcuHandle->remainingCount >= flexioLcdMcuHandle->minorLoopBytes)
        {
            FLEXIO_MCULCD_EDMAConfig(flexioLcdMcuBase, flexioLcdMcuHandle);
            EDMA_StartTransfer(flexioLcdMcuHandle->txDmaHandle);
        }
        else
        {
            FLEXIO_MCULCD_EnableTxDMA(flexioLcdMcuBase, false);

            /* Now the data are in shifter, wait for the data send out from the shifter. */
            FLEXIO_MCULCD_WaitTransmitComplete();

            /* Disable the TX shifter and the timer. */
            FLEXIO_MCULCD_ClearMultiBeatsWriteConfig(flexioLcdMcuBase);

            /* Send the remaining data. */
            if (flexioLcdMcuHandle->remainingCount)
            {
                if (kFLEXIO_MCULCD_StateWriteSameValue == flexioLcdMcuHandle->state)
                {
                    FLEXIO_MCULCD_WriteSameValueBlocking(flexioLcdMcuBase, flexioLcdMcuHandle->dataAddrOrSameValue,
                                                         flexioLcdMcuHandle->remainingCount);
                }
                else
                {
                    FLEXIO_MCULCD_WriteDataArrayBlocking(flexioLcdMcuBase,
                                                         (void *)flexioLcdMcuHandle->dataAddrOrSameValue,
                                                         flexioLcdMcuHandle->remainingCount);
                }
            }

            /* De-assert nCS. */
            FLEXIO_MCULCD_StopTransfer(flexioLcdMcuBase);

            /* Change the state. */
            flexioLcdMcuHandle->state          = kFLEXIO_MCULCD_StateIdle;
            flexioLcdMcuHandle->dataCount      = 0;
            flexioLcdMcuHandle->remainingCount = 0;

            /* Callback to inform upper layer. */
            if (flexioLcdMcuHandle->completionCallback)
            {
                flexioLcdMcuHandle->completionCallback(flexioLcdMcuBase, flexioLcdMcuHandle, kStatus_FLEXIO_MCULCD_Idle,
                                                       flexioLcdMcuHandle->userData);
            }
        }
    }
}

static void FLEXIO_MCULCD_RxEDMACallback(edma_handle_t *DmaHandle, void *param, bool transferDone, uint32_t tcds)
{
    tcds = tcds;
    uint32_t i;
    uint32_t rxBufAddr;
    flexio_mculcd_edma_handle_t *flexioLcdMcuHandle = (flexio_mculcd_edma_handle_t *)param;
    FLEXIO_MCULCD_Type *flexioLcdMcuBase            = flexioLcdMcuHandle->base;
    FLEXIO_Type *flexioBase                         = flexioLcdMcuBase->flexioBase;

    if (transferDone)
    {
        if (flexioLcdMcuHandle->remainingCount >= (2 * flexioLcdMcuHandle->minorLoopBytes))
        {
            FLEXIO_MCULCD_EDMAConfig(flexioLcdMcuBase, flexioLcdMcuHandle);
            EDMA_StartTransfer(flexioLcdMcuHandle->rxDmaHandle);
        }
        else
        {
            FLEXIO_MCULCD_EnableRxDMA(flexioLcdMcuBase, false);

            /* Wait the data saved to the shifter buffer. */
            while (!((1U << flexioLcdMcuBase->rxShifterEndIndex) & FLEXIO_GetShifterStatusFlags(flexioBase)))
            {
            }

            /* Disable the RX shifter and the timer. */
            FLEXIO_MCULCD_ClearMultiBeatsReadConfig(flexioLcdMcuBase);

            rxBufAddr = FLEXIO_MCULCD_GetRxDataRegisterAddress(flexioLcdMcuBase);

/* Read out the data. */
#if (defined(__CORTEX_M) && (__CORTEX_M == 0))
            /* Cortex M0 and M0+ only support aligned access. */
            for (i = 0; i < flexioLcdMcuHandle->rxShifterNum * 4; i++)
            {
                ((uint8_t *)(flexioLcdMcuHandle->dataAddrOrSameValue))[i] = ((volatile uint8_t *)rxBufAddr)[i];
            }
#else
            for (i = 0; i < flexioLcdMcuHandle->rxShifterNum; i++)
            {
                ((uint32_t *)(flexioLcdMcuHandle->dataAddrOrSameValue))[i] = ((volatile uint32_t *)rxBufAddr)[i];
            }
#endif
            flexioLcdMcuHandle->remainingCount -= flexioLcdMcuHandle->minorLoopBytes;

            if (flexioLcdMcuHandle->remainingCount)
            {
                FLEXIO_MCULCD_ReadDataArrayBlocking(
                    flexioLcdMcuBase,
                    (void *)(flexioLcdMcuHandle->dataAddrOrSameValue + flexioLcdMcuHandle->minorLoopBytes),
                    flexioLcdMcuHandle->remainingCount);
            }

            /* De-assert nCS. */
            FLEXIO_MCULCD_StopTransfer(flexioLcdMcuBase);

            /* Change the state. */
            flexioLcdMcuHandle->state          = kFLEXIO_MCULCD_StateIdle;
            flexioLcdMcuHandle->dataCount      = 0;
            flexioLcdMcuHandle->remainingCount = 0;

            /* Callback to inform upper layer. */
            if (flexioLcdMcuHandle->completionCallback)
            {
                flexioLcdMcuHandle->completionCallback(flexioLcdMcuBase, flexioLcdMcuHandle, kStatus_FLEXIO_MCULCD_Idle,
                                                       flexioLcdMcuHandle->userData);
            }
        }
    }
}

static void FLEXIO_MCULCD_EDMAConfig(FLEXIO_MCULCD_Type *base, flexio_mculcd_edma_handle_t *handle)
{
    edma_transfer_config_t xferConfig = {0};
    edma_transfer_size_t transferSize = kEDMA_TransferSize1Bytes;
    int16_t offset;
    uint32_t majorLoopCounts;
    uint32_t transferCount;

#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
    transferSize = kEDMA_TransferSize1Bytes;
    offset       = 1;
#else
    transferSize = kEDMA_TransferSize2Bytes;
    offset = 2;
#endif

    majorLoopCounts = handle->remainingCount / handle->minorLoopBytes;

    /* For reading, the last minor loop data is not tranfered by DMA. */
    if (kFLEXIO_MCULCD_StateReadArray == handle->state)
    {
        majorLoopCounts--;
    }

    if (majorLoopCounts > EDMA_MAX_MAJOR_COUNT)
    {
        majorLoopCounts = EDMA_MAX_MAJOR_COUNT;
    }

    transferCount = majorLoopCounts * handle->minorLoopBytes;

    if (kFLEXIO_MCULCD_StateReadArray == handle->state)
    {
        xferConfig.srcAddr          = FLEXIO_MCULCD_GetRxDataRegisterAddress(base);
        xferConfig.destAddr         = handle->dataAddrOrSameValue;
        xferConfig.srcTransferSize  = kEDMA_TransferSize4Bytes;
        xferConfig.destTransferSize = transferSize;
        xferConfig.srcOffset        = 4;
        xferConfig.destOffset       = offset;
        xferConfig.minorLoopBytes   = handle->minorLoopBytes;
        xferConfig.majorLoopCounts  = majorLoopCounts;
        handle->remainingCount -= transferCount;
        handle->dataAddrOrSameValue += transferCount;
        EDMA_SubmitTransfer(handle->rxDmaHandle, &xferConfig);
        EDMA_SetModulo(handle->rxDmaHandle->base, handle->rxDmaHandle->channel, handle->rxEdmaModulo,
                       kEDMA_ModuloDisable);
    }
    else
    {
        if (kFLEXIO_MCULCD_StateWriteArray == handle->state)
        {
            xferConfig.srcAddr   = handle->dataAddrOrSameValue;
            xferConfig.srcOffset = offset;
            handle->dataAddrOrSameValue += transferCount;
        }
        else
        {
            xferConfig.srcAddr   = (uint32_t)(&(handle->dataAddrOrSameValue));
            xferConfig.srcOffset = 0;
        }
        xferConfig.destAddr         = FLEXIO_MCULCD_GetTxDataRegisterAddress(base);
        xferConfig.srcTransferSize  = transferSize;
        xferConfig.destTransferSize = kEDMA_TransferSize4Bytes;
        xferConfig.destOffset       = 4;
        xferConfig.minorLoopBytes   = handle->minorLoopBytes;
        xferConfig.majorLoopCounts  = majorLoopCounts;
        handle->remainingCount -= transferCount;
        EDMA_SubmitTransfer(handle->txDmaHandle, &xferConfig);
        EDMA_SetModulo(handle->txDmaHandle->base, handle->txDmaHandle->channel, kEDMA_ModuloDisable,
                       handle->txEdmaModulo);
    }
}

static bool FLEXIO_MCULCD_GetEDMAModulo(uint8_t shifterNum, edma_modulo_t *modulo)
{
    bool ret = true;

    switch (shifterNum)
    {
        case 1U:
            *modulo = kEDMA_Modulo4bytes;
            break;
        case 2U:
            *modulo = kEDMA_Modulo8bytes;
            break;
        case 4U:
            *modulo = kEDMA_Modulo16bytes;
            break;
        case 8U:
            *modulo = kEDMA_Modulo32bytes;
            break;
        default:
            ret = false;
            break;
    }

    return ret;
}

/*!
 * brief Initializes the FLEXO MCULCD master eDMA handle.
 *
 * This function initializes the FLEXO MCULCD master eDMA handle which can be
 * used for other FLEXO MCULCD transactional APIs. For a specified FLEXO MCULCD
 * instance, call this API once to get the initialized handle.
 *
 * param base Pointer to FLEXIO_MCULCD_Type structure.
 * param handle Pointer to flexio_mculcd_edma_handle_t structure to store the
 * transfer state.
 * param callback MCULCD transfer complete callback, NULL means no callback.
 * param userData callback function parameter.
 * param txDmaHandle User requested eDMA handle for FlexIO MCULCD eDMA TX,
 * the DMA request source of this handle should be the first of TX shifters.
 * param rxDmaHandle User requested eDMA handle for FlexIO MCULCD eDMA RX,
 * the DMA request source of this handle should be the last of RX shifters.
 * retval kStatus_Success Successfully create the handle.
 */
status_t FLEXIO_MCULCD_TransferCreateHandleEDMA(FLEXIO_MCULCD_Type *base,
                                                flexio_mculcd_edma_handle_t *handle,
                                                flexio_mculcd_edma_transfer_callback_t callback,
                                                void *userData,
                                                edma_handle_t *txDmaHandle,
                                                edma_handle_t *rxDmaHandle)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Initialize the state. */
    handle->state = kFLEXIO_MCULCD_StateIdle;

    /* Register callback and userData. */
    handle->completionCallback = callback;
    handle->userData           = userData;

    handle->base         = base;
    handle->txShifterNum = base->txShifterEndIndex - base->txShifterStartIndex + 1U;
    handle->rxShifterNum = base->rxShifterEndIndex - base->rxShifterStartIndex + 1U;

    if (rxDmaHandle)
    {
        if (!FLEXIO_MCULCD_GetEDMAModulo(handle->rxShifterNum, &handle->rxEdmaModulo))
        {
            return kStatus_InvalidArgument;
        }

        handle->rxDmaHandle = rxDmaHandle;
        EDMA_SetCallback(rxDmaHandle, FLEXIO_MCULCD_RxEDMACallback, handle);
    }

    if (txDmaHandle)
    {
        if (!FLEXIO_MCULCD_GetEDMAModulo(handle->txShifterNum, &handle->txEdmaModulo))
        {
            return kStatus_InvalidArgument;
        }

        handle->txDmaHandle = txDmaHandle;
        EDMA_SetCallback(txDmaHandle, FLEXIO_MCULCD_TxEDMACallback, handle);
    }

    return kStatus_Success;
}

/*!
 * brief Performs a non-blocking FlexIO MCULCD transfer using eDMA.
 *
 * This function returns immediately after transfer initiates. To check whether
 * the transfer is completed, user could:
 * 1. Use the transfer completed callback;
 * 2. Polling function ref FLEXIO_MCULCD_GetTransferCountEDMA
 *
 * param base pointer to FLEXIO_MCULCD_Type structure.
 * param handle pointer to flexio_mculcd_edma_handle_t structure to store the
 * transfer state.
 * param xfer Pointer to FlexIO MCULCD transfer structure.
 * retval kStatus_Success Successfully start a transfer.
 * retval kStatus_InvalidArgument Input argument is invalid.
 * retval kStatus_FLEXIO_MCULCD_Busy FlexIO MCULCD is not idle, it is running another
 * transfer.
 */
status_t FLEXIO_MCULCD_TransferEDMA(FLEXIO_MCULCD_Type *base,
                                    flexio_mculcd_edma_handle_t *handle,
                                    flexio_mculcd_transfer_t *xfer)
{
    assert(handle);
    assert(xfer);

    /*
     * The data transfer mechanism:
     *
     * Read:
     * Assume the data length is Lr = (n1 * minorLoopBytes + n2), where
     * n2 < minorLoopBytes.
     * If (n1 <= 1), then all data are sent using blocking method.
     * If (n1 > 1), then the beginning ((n1-1) * minorLoopBytes) are read
     * using DMA, the left (minorLoopBytes + n2) are read using blocking method.
     *
     * Write:
     * Assume the data length is Lw = (n1 * minorLoopBytes + n2), where
     * n2 < minorLoopBytes.
     * If (n1 = 0), then all data are sent using blocking method.
     * If (n1 >= 1), then the beginning (n1 * minorLoopBytes) are sent
     * using DMA, the left n2 are sent using blocking method.
     */

    /* Check if the device is busy. */
    if (kFLEXIO_MCULCD_StateIdle != handle->state)
    {
        return kStatus_FLEXIO_MCULCD_Busy;
    }

    /* Set the state in handle. */
    if (kFLEXIO_MCULCD_ReadArray == xfer->mode)
    {
        handle->state          = kFLEXIO_MCULCD_StateReadArray;
        handle->minorLoopBytes = handle->rxShifterNum * 4U;
    }
    else
    {
        handle->minorLoopBytes = handle->txShifterNum * 4U;

        if (kFLEXIO_MCULCD_WriteArray == xfer->mode)
        {
            handle->state = kFLEXIO_MCULCD_StateWriteArray;
        }
        else
        {
            handle->state = kFLEXIO_MCULCD_StateWriteSameValue;
        }
    }

    /*
     * For TX, if data is less than one minor loop, then use polling method.
     * For RX, if data is less than two minor loop, then use polling method.
     */
    if ((xfer->dataSize < handle->minorLoopBytes) ||
        ((kFLEXIO_MCULCD_ReadArray == xfer->mode) && (xfer->dataSize < 2 * (handle->minorLoopBytes))))
    {
        FLEXIO_MCULCD_TransferBlocking(base, xfer);

        handle->state = kFLEXIO_MCULCD_StateIdle;

        /* Callback to inform upper layer. */
        if (handle->completionCallback)
        {
            handle->completionCallback(base, handle, kStatus_FLEXIO_MCULCD_Idle, handle->userData);
        }
    }
    else
    {
        handle->dataCount           = xfer->dataSize;
        handle->remainingCount      = xfer->dataSize;
        handle->dataAddrOrSameValue = xfer->dataAddrOrSameValue;

        /* Setup DMA to transfer data.  */
        /* Assert the nCS. */
        FLEXIO_MCULCD_StartTransfer(base);
        /* Send the command. */
        FLEXIO_MCULCD_WriteCommandBlocking(base, xfer->command);

        /* Setup the DMA configuration. */
        FLEXIO_MCULCD_EDMAConfig(base, handle);

        /* Start the transfer. */
        if (kFLEXIO_MCULCD_ReadArray == xfer->mode)
        {
            /* For 6800, assert the RDWR pin. */
            if (kFLEXIO_MCULCD_6800 == base->busType)
            {
                base->setRDWRPin(true);
            }
            FLEXIO_MCULCD_SetMultiBeatsReadConfig(base);
            FLEXIO_MCULCD_EnableRxDMA(base, true);
            EDMA_StartTransfer(handle->rxDmaHandle);
        }
        else
        {
            /* For 6800, de-assert the RDWR pin. */
            if (kFLEXIO_MCULCD_6800 == base->busType)
            {
                base->setRDWRPin(false);
            }
            FLEXIO_MCULCD_SetMultiBeatsWriteConfig(base);
            FLEXIO_MCULCD_EnableTxDMA(base, true);
            EDMA_StartTransfer(handle->txDmaHandle);
        }
    }

    return kStatus_Success;
}

/*!
 * brief Aborts a FlexIO MCULCD transfer using eDMA.
 *
 * param base pointer to FLEXIO_MCULCD_Type structure.
 * param handle FlexIO MCULCD eDMA handle pointer.
 */
void FLEXIO_MCULCD_TransferAbortEDMA(FLEXIO_MCULCD_Type *base, flexio_mculcd_edma_handle_t *handle)
{
    assert(handle);

    /* Disable dma. */
    if (handle->txDmaHandle)
    {
        EDMA_AbortTransfer(handle->txDmaHandle);
    }
    if (handle->rxDmaHandle)
    {
        EDMA_AbortTransfer(handle->rxDmaHandle);
    }

    /* Disable DMA enable bit. */
    FLEXIO_MCULCD_EnableTxDMA(handle->base, false);
    FLEXIO_MCULCD_EnableRxDMA(handle->base, false);

    /* Set the handle state. */
    handle->state     = kFLEXIO_MCULCD_StateIdle;
    handle->dataCount = 0;
}

/*!
 * brief Gets the remaining bytes for FlexIO MCULCD eDMA transfer.
 *
 * param base pointer to FLEXIO_MCULCD_Type structure.
 * param handle FlexIO MCULCD eDMA handle pointer.
 * param count Number of count transferred so far by the eDMA transaction.
 * retval kStatus_Success Get the transferred count Successfully.
 * retval kStatus_NoTransferInProgress No transfer in process.
 */
status_t FLEXIO_MCULCD_TransferGetCountEDMA(FLEXIO_MCULCD_Type *base,
                                            flexio_mculcd_edma_handle_t *handle,
                                            size_t *count)
{
    assert(handle);
    assert(count);
    uint32_t state = handle->state;

    if (kFLEXIO_MCULCD_StateIdle == state)
    {
        return kStatus_NoTransferInProgress;
    }
    else
    {
        *count = handle->dataCount - handle->remainingCount;

        if (kFLEXIO_MCULCD_StateReadArray == state)
        {
            *count -= handle->minorLoopBytes *
                      EDMA_GetRemainingMajorLoopCount(handle->rxDmaHandle->base, handle->rxDmaHandle->channel);
        }
        else
        {
            *count -= handle->minorLoopBytes *
                      EDMA_GetRemainingMajorLoopCount(handle->txDmaHandle->base, handle->txDmaHandle->channel);
        }
    }

    return kStatus_Success;
}
