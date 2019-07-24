/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_i2s_edma.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_i2s_edma"
#endif

/*******************************************************************************
 * Definitations
 ******************************************************************************/
/* Used for 32byte aligned */
#define STCD_ADDR(address) (edma_tcd_t *)(((uint32_t)(address) + 32) & ~0x1FU)

/*<! Structure definition for flexio_i2s_edma_private_handle_t. The structure is private. */
typedef struct _flexio_i2s_edma_private_handle
{
    FLEXIO_I2S_Type *base;
    flexio_i2s_edma_handle_t *handle;
} flexio_i2s_edma_private_handle_t;

enum _flexio_i2s_edma_transfer_state
{
    kFLEXIO_I2S_Busy = 0x0U, /*!< FLEXIO I2S is busy */
    kFLEXIO_I2S_Idle,        /*!< Transfer is done. */
};

/*<! Private handle only used for internally. */
static flexio_i2s_edma_private_handle_t s_edmaPrivateHandle[2];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief FLEXIO I2S EDMA callback for send.
 *
 * @param handle pointer to flexio_i2s_edma_handle_t structure which stores the transfer state.
 * @param userData Parameter for user callback.
 * @param done If the DMA transfer finished.
 * @param tcds The TCD index.
 */
static void FLEXIO_I2S_TxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds);

/*!
 * @brief FLEXIO I2S EDMA callback for receive.
 *
 * @param handle pointer to flexio_i2s_edma_handle_t structure which stores the transfer state.
 * @param userData Parameter for user callback.
 * @param done If the DMA transfer finished.
 * @param tcds The TCD index.
 */
static void FLEXIO_I2S_RxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds);

/*******************************************************************************
 * Code
 ******************************************************************************/
static void FLEXIO_I2S_TxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds)
{
    flexio_i2s_edma_private_handle_t *privHandle = (flexio_i2s_edma_private_handle_t *)userData;
    flexio_i2s_edma_handle_t *flexio_i2sHandle   = privHandle->handle;

    /* If finished a block, call the callback function */
    memset(&flexio_i2sHandle->queue[flexio_i2sHandle->queueDriver], 0, sizeof(flexio_i2s_transfer_t));
    flexio_i2sHandle->queueDriver = (flexio_i2sHandle->queueDriver + 1) % FLEXIO_I2S_XFER_QUEUE_SIZE;
    if (flexio_i2sHandle->callback)
    {
        (flexio_i2sHandle->callback)(privHandle->base, flexio_i2sHandle, kStatus_Success, flexio_i2sHandle->userData);
    }

    /* If all data finished, just stop the transfer */
    if (flexio_i2sHandle->queue[flexio_i2sHandle->queueDriver].data == NULL)
    {
        FLEXIO_I2S_TransferAbortSendEDMA(privHandle->base, flexio_i2sHandle);
    }
}

static void FLEXIO_I2S_RxEDMACallback(edma_handle_t *handle, void *userData, bool done, uint32_t tcds)
{
    flexio_i2s_edma_private_handle_t *privHandle = (flexio_i2s_edma_private_handle_t *)userData;
    flexio_i2s_edma_handle_t *flexio_i2sHandle   = privHandle->handle;

    /* If finished a block, call the callback function */
    memset(&flexio_i2sHandle->queue[flexio_i2sHandle->queueDriver], 0, sizeof(flexio_i2s_transfer_t));
    flexio_i2sHandle->queueDriver = (flexio_i2sHandle->queueDriver + 1) % FLEXIO_I2S_XFER_QUEUE_SIZE;
    if (flexio_i2sHandle->callback)
    {
        (flexio_i2sHandle->callback)(privHandle->base, flexio_i2sHandle, kStatus_Success, flexio_i2sHandle->userData);
    }

    /* If all data finished, just stop the transfer */
    if (flexio_i2sHandle->queue[flexio_i2sHandle->queueDriver].data == NULL)
    {
        FLEXIO_I2S_TransferAbortReceiveEDMA(privHandle->base, flexio_i2sHandle);
    }
}

/*!
 * brief Initializes the FlexIO I2S eDMA handle.
 *
 * This function initializes the FlexIO I2S master DMA handle which can be used for other FlexIO I2S master
 * transactional APIs.
 * Usually, for a specified FlexIO I2S instance, call this API once to get the initialized handle.
 *
 * param base FlexIO I2S peripheral base address.
 * param handle FlexIO I2S eDMA handle pointer.
 * param callback FlexIO I2S eDMA callback function called while finished a block.
 * param userData User parameter for callback.
 * param dmaHandle eDMA handle for FlexIO I2S. This handle is a static value allocated by users.
 */
void FLEXIO_I2S_TransferTxCreateHandleEDMA(FLEXIO_I2S_Type *base,
                                           flexio_i2s_edma_handle_t *handle,
                                           flexio_i2s_edma_callback_t callback,
                                           void *userData,
                                           edma_handle_t *dmaHandle)
{
    assert(handle && dmaHandle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Set flexio_i2s base to handle */
    handle->dmaHandle = dmaHandle;
    handle->callback  = callback;
    handle->userData  = userData;

    /* Set FLEXIO I2S state to idle */
    handle->state = kFLEXIO_I2S_Idle;

    s_edmaPrivateHandle[0].base   = base;
    s_edmaPrivateHandle[0].handle = handle;

    /* Need to use scatter gather */
    EDMA_InstallTCDMemory(dmaHandle, STCD_ADDR(handle->tcd), FLEXIO_I2S_XFER_QUEUE_SIZE);

    /* Install callback for Tx dma channel */
    EDMA_SetCallback(dmaHandle, FLEXIO_I2S_TxEDMACallback, &s_edmaPrivateHandle[0]);
}

/*!
 * brief Initializes the FlexIO I2S Rx eDMA handle.
 *
 * This function initializes the FlexIO I2S slave DMA handle which can be used for other FlexIO I2S master transactional
 * APIs.
 * Usually, for a specified FlexIO I2S instance, call this API once to get the initialized handle.
 *
 * param base FlexIO I2S peripheral base address.
 * param handle FlexIO I2S eDMA handle pointer.
 * param callback FlexIO I2S eDMA callback function called while finished a block.
 * param userData User parameter for callback.
 * param dmaHandle eDMA handle for FlexIO I2S. This handle is a static value allocated by users.
 */
void FLEXIO_I2S_TransferRxCreateHandleEDMA(FLEXIO_I2S_Type *base,
                                           flexio_i2s_edma_handle_t *handle,
                                           flexio_i2s_edma_callback_t callback,
                                           void *userData,
                                           edma_handle_t *dmaHandle)
{
    assert(handle && dmaHandle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Set flexio_i2s base to handle */
    handle->dmaHandle = dmaHandle;
    handle->callback  = callback;
    handle->userData  = userData;

    /* Set FLEXIO I2S state to idle */
    handle->state = kFLEXIO_I2S_Idle;

    s_edmaPrivateHandle[1].base   = base;
    s_edmaPrivateHandle[1].handle = handle;

    /* Need to use scatter gather */
    EDMA_InstallTCDMemory(dmaHandle, STCD_ADDR(handle->tcd), FLEXIO_I2S_XFER_QUEUE_SIZE);

    /* Install callback for Tx dma channel */
    EDMA_SetCallback(dmaHandle, FLEXIO_I2S_RxEDMACallback, &s_edmaPrivateHandle[1]);
}

/*!
 * brief Configures the FlexIO I2S Tx audio format.
 *
 * Audio format can be changed in run-time of FlexIO I2S. This function configures the sample rate and audio data
 * format to be transferred. This function also sets the eDMA parameter according to format.
 *
 * param base FlexIO I2S peripheral base address.
 * param handle FlexIO I2S eDMA handle pointer
 * param format Pointer to FlexIO I2S audio data format structure.
 * param srcClock_Hz FlexIO I2S clock source frequency in Hz, it should be 0 while in slave mode.
 * retval kStatus_Success Audio format set successfully.
 * retval kStatus_InvalidArgument The input arguments is invalid.
 */
void FLEXIO_I2S_TransferSetFormatEDMA(FLEXIO_I2S_Type *base,
                                      flexio_i2s_edma_handle_t *handle,
                                      flexio_i2s_format_t *format,
                                      uint32_t srcClock_Hz)
{
    assert(handle && format);

    /* Configure the audio format to FLEXIO I2S registers */
    if (srcClock_Hz != 0)
    {
        /* It is master */
        FLEXIO_I2S_MasterSetFormat(base, format, srcClock_Hz);
    }
    else
    {
        FLEXIO_I2S_SlaveSetFormat(base, format);
    }

    /* Get the transfer size from format, this should be used in EDMA configuration */
    handle->bytesPerFrame = format->bitWidth / 8U;
}

/*!
 * brief Performs a non-blocking FlexIO I2S transfer using DMA.
 *
 * note This interface returned immediately after transfer initiates. Users should call
 * FLEXIO_I2S_GetTransferStatus to poll the transfer status and check whether the FlexIO I2S transfer is finished.
 *
 * param base FlexIO I2S peripheral base address.
 * param handle FlexIO I2S DMA handle pointer.
 * param xfer Pointer to DMA transfer structure.
 * retval kStatus_Success Start a FlexIO I2S eDMA send successfully.
 * retval kStatus_InvalidArgument The input arguments is invalid.
 * retval kStatus_TxBusy FlexIO I2S is busy sending data.
 */
status_t FLEXIO_I2S_TransferSendEDMA(FLEXIO_I2S_Type *base,
                                     flexio_i2s_edma_handle_t *handle,
                                     flexio_i2s_transfer_t *xfer)
{
    assert(handle && xfer);

    edma_transfer_config_t config = {0};
    uint32_t destAddr             = FLEXIO_I2S_TxGetDataRegisterAddress(base) + (4U - handle->bytesPerFrame);

    /* Check if input parameter invalid */
    if ((xfer->data == NULL) || (xfer->dataSize == 0U))
    {
        return kStatus_InvalidArgument;
    }

    if (handle->queue[handle->queueUser].data)
    {
        return kStatus_FLEXIO_I2S_QueueFull;
    }

    /* Change the state of handle */
    handle->state = kFLEXIO_I2S_Busy;

    /* Update the queue state */
    handle->queue[handle->queueUser].data     = xfer->data;
    handle->queue[handle->queueUser].dataSize = xfer->dataSize;
    handle->transferSize[handle->queueUser]   = xfer->dataSize;
    handle->queueUser                         = (handle->queueUser + 1) % FLEXIO_I2S_XFER_QUEUE_SIZE;

    /* Prepare edma configure */
    EDMA_PrepareTransfer(&config, xfer->data, handle->bytesPerFrame, (void *)destAddr, handle->bytesPerFrame,
                         handle->bytesPerFrame, xfer->dataSize, kEDMA_MemoryToPeripheral);

    /* Store the initially configured eDMA minor byte transfer count into the FLEXIO I2S handle */
    handle->nbytes = handle->bytesPerFrame;

    EDMA_SubmitTransfer(handle->dmaHandle, &config);

    /* Start DMA transfer */
    EDMA_StartTransfer(handle->dmaHandle);

    /* Enable DMA enable bit */
    FLEXIO_I2S_TxEnableDMA(base, true);

    /* Enable FLEXIO I2S Tx clock */
    FLEXIO_I2S_Enable(base, true);

    return kStatus_Success;
}

/*!
 * brief Performs a non-blocking FlexIO I2S receive using eDMA.
 *
 * note This interface returned immediately after transfer initiates. Users should call
 * FLEXIO_I2S_GetReceiveRemainingBytes to poll the transfer status and check whether the FlexIO I2S transfer is
 * finished.
 *
 * param base FlexIO I2S peripheral base address.
 * param handle FlexIO I2S DMA handle pointer.
 * param xfer Pointer to DMA transfer structure.
 * retval kStatus_Success Start a FlexIO I2S eDMA receive successfully.
 * retval kStatus_InvalidArgument The input arguments is invalid.
 * retval kStatus_RxBusy FlexIO I2S is busy receiving data.
 */
status_t FLEXIO_I2S_TransferReceiveEDMA(FLEXIO_I2S_Type *base,
                                        flexio_i2s_edma_handle_t *handle,
                                        flexio_i2s_transfer_t *xfer)
{
    assert(handle && xfer);

    edma_transfer_config_t config = {0};
    uint32_t srcAddr              = FLEXIO_I2S_RxGetDataRegisterAddress(base);

    /* Check if input parameter invalid */
    if ((xfer->data == NULL) || (xfer->dataSize == 0U))
    {
        return kStatus_InvalidArgument;
    }

    if (handle->queue[handle->queueUser].data)
    {
        return kStatus_FLEXIO_I2S_QueueFull;
    }

    /* Change the state of handle */
    handle->state = kFLEXIO_I2S_Busy;

    /* Update queue state  */
    handle->queue[handle->queueUser].data     = xfer->data;
    handle->queue[handle->queueUser].dataSize = xfer->dataSize;
    handle->transferSize[handle->queueUser]   = xfer->dataSize;
    handle->queueUser                         = (handle->queueUser + 1) % FLEXIO_I2S_XFER_QUEUE_SIZE;

    /* Prepare edma configure */
    EDMA_PrepareTransfer(&config, (void *)srcAddr, handle->bytesPerFrame, xfer->data, handle->bytesPerFrame,
                         handle->bytesPerFrame, xfer->dataSize, kEDMA_PeripheralToMemory);

    /* Store the initially configured eDMA minor byte transfer count into the FLEXIO I2S handle */
    handle->nbytes = handle->bytesPerFrame;

    EDMA_SubmitTransfer(handle->dmaHandle, &config);

    /* Start DMA transfer */
    EDMA_StartTransfer(handle->dmaHandle);

    /* Enable DMA enable bit */
    FLEXIO_I2S_RxEnableDMA(base, true);

    /* Enable FLEXIO I2S Rx clock */
    FLEXIO_I2S_Enable(base, true);

    return kStatus_Success;
}

/*!
 * brief Aborts a FlexIO I2S transfer using eDMA.
 *
 * param base FlexIO I2S peripheral base address.
 * param handle FlexIO I2S DMA handle pointer.
 */
void FLEXIO_I2S_TransferAbortSendEDMA(FLEXIO_I2S_Type *base, flexio_i2s_edma_handle_t *handle)
{
    assert(handle);

    /* Disable dma */
    EDMA_AbortTransfer(handle->dmaHandle);

    /* Disable DMA enable bit */
    FLEXIO_I2S_TxEnableDMA(base, false);

    /* Set the handle state */
    handle->state = kFLEXIO_I2S_Idle;
}

/*!
 * brief Aborts a FlexIO I2S receive using eDMA.
 *
 * param base FlexIO I2S peripheral base address.
 * param handle FlexIO I2S DMA handle pointer.
 */
void FLEXIO_I2S_TransferAbortReceiveEDMA(FLEXIO_I2S_Type *base, flexio_i2s_edma_handle_t *handle)
{
    assert(handle);

    /* Disable dma */
    EDMA_AbortTransfer(handle->dmaHandle);

    /* Disable DMA enable bit */
    FLEXIO_I2S_RxEnableDMA(base, false);

    /* Set the handle state */
    handle->state = kFLEXIO_I2S_Idle;
}

/*!
 * brief Gets the remaining bytes to be sent.
 *
 * param base FlexIO I2S peripheral base address.
 * param handle FlexIO I2S DMA handle pointer.
 * param count Bytes sent.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t FLEXIO_I2S_TransferGetSendCountEDMA(FLEXIO_I2S_Type *base, flexio_i2s_edma_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kFLEXIO_I2S_Busy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = handle->transferSize[handle->queueDriver] -
                 (uint32_t)handle->nbytes *
                     EDMA_GetRemainingMajorLoopCount(handle->dmaHandle->base, handle->dmaHandle->channel);
    }

    return status;
}

/*!
 * brief Get the remaining bytes to be received.
 *
 * param base FlexIO I2S peripheral base address.
 * param handle FlexIO I2S DMA handle pointer.
 * param count Bytes received.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t FLEXIO_I2S_TransferGetReceiveCountEDMA(FLEXIO_I2S_Type *base, flexio_i2s_edma_handle_t *handle, size_t *count)
{
    assert(handle);

    status_t status = kStatus_Success;

    if (handle->state != kFLEXIO_I2S_Busy)
    {
        status = kStatus_NoTransferInProgress;
    }
    else
    {
        *count = handle->transferSize[handle->queueDriver] -
                 (uint32_t)handle->nbytes *
                     EDMA_GetRemainingMajorLoopCount(handle->dmaHandle->base, handle->dmaHandle->channel);
    }

    return status;
}
