/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_uart_edma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_uart_edma"
#endif

/*<! Structure definition for uart_edma_private_handle_t. The structure is private. */
typedef struct _flexio_uart_edma_private_handle
{
    FLEXIO_UART_Type *base;
    flexio_uart_edma_handle_t *handle;
} flexio_uart_edma_private_handle_t;

/* UART EDMA transfer handle. */
enum _flexio_uart_edma_tansfer_states
{
    kFLEXIO_UART_TxIdle, /* TX idle. */
    kFLEXIO_UART_TxBusy, /* TX busy. */
    kFLEXIO_UART_RxIdle, /* RX idle. */
    kFLEXIO_UART_RxBusy  /* RX busy. */
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*< @brief user configurable flexio uart handle count. */
#define FLEXIO_UART_HANDLE_COUNT 2

/*<! Private handle only used for internally. */
static flexio_uart_edma_private_handle_t s_edmaPrivateHandle[FLEXIO_UART_HANDLE_COUNT];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief FLEXIO UART EDMA send finished callback function.
 *
 * This function is called when FLEXIO UART EDMA send finished. It disables the UART
 * TX EDMA request and sends @ref kStatus_FLEXIO_UART_TxIdle to FLEXIO UART callback.
 *
 * @param handle The EDMA handle.
 * @param param Callback function parameter.
 */
static void FLEXIO_UART_TransferSendEDMACallback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);

/*!
 * @brief FLEXIO UART EDMA receive finished callback function.
 *
 * This function is called when FLEXIO UART EDMA receive finished. It disables the UART
 * RX EDMA request and sends @ref kStatus_FLEXIO_UART_RxIdle to UART callback.
 *
 * @param handle The EDMA handle.
 * @param param Callback function parameter.
 */
static void FLEXIO_UART_TransferReceiveEDMACallback(edma_handle_t *handle,
                                                    void *param,
                                                    bool transferDone,
                                                    uint32_t tcds);

/*******************************************************************************
 * Code
 ******************************************************************************/

static void FLEXIO_UART_TransferSendEDMACallback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    flexio_uart_edma_private_handle_t *uartPrivateHandle = (flexio_uart_edma_private_handle_t *)param;

    assert(uartPrivateHandle->handle);

    /* Avoid the warning for unused variables. */
    handle = handle;
    tcds   = tcds;

    if (transferDone)
    {
        FLEXIO_UART_TransferAbortSendEDMA(uartPrivateHandle->base, uartPrivateHandle->handle);

        if (uartPrivateHandle->handle->callback)
        {
            uartPrivateHandle->handle->callback(uartPrivateHandle->base, uartPrivateHandle->handle,
                                                kStatus_FLEXIO_UART_TxIdle, uartPrivateHandle->handle->userData);
        }
    }
}

static void FLEXIO_UART_TransferReceiveEDMACallback(edma_handle_t *handle,
                                                    void *param,
                                                    bool transferDone,
                                                    uint32_t tcds)
{
    flexio_uart_edma_private_handle_t *uartPrivateHandle = (flexio_uart_edma_private_handle_t *)param;

    assert(uartPrivateHandle->handle);

    /* Avoid the warning for unused variables. */
    handle = handle;
    tcds   = tcds;

    if (transferDone)
    {
        /* Disable transfer. */
        FLEXIO_UART_TransferAbortReceiveEDMA(uartPrivateHandle->base, uartPrivateHandle->handle);

        if (uartPrivateHandle->handle->callback)
        {
            uartPrivateHandle->handle->callback(uartPrivateHandle->base, uartPrivateHandle->handle,
                                                kStatus_FLEXIO_UART_RxIdle, uartPrivateHandle->handle->userData);
        }
    }
}

/*!
 * brief Initializes the UART handle which is used in transactional functions.
 *
 * param base Pointer to FLEXIO_UART_Type.
 * param handle Pointer to flexio_uart_edma_handle_t structure.
 * param callback The callback function.
 * param userData The parameter of the callback function.
 * param rxEdmaHandle User requested DMA handle for RX DMA transfer.
 * param txEdmaHandle User requested DMA handle for TX DMA transfer.
 * retval kStatus_Success Successfully create the handle.
 * retval kStatus_OutOfRange The FlexIO SPI eDMA type/handle table out of range.
 */
status_t FLEXIO_UART_TransferCreateHandleEDMA(FLEXIO_UART_Type *base,
                                              flexio_uart_edma_handle_t *handle,
                                              flexio_uart_edma_transfer_callback_t callback,
                                              void *userData,
                                              edma_handle_t *txEdmaHandle,
                                              edma_handle_t *rxEdmaHandle)
{
    assert(handle);

    uint8_t index = 0;

    /* Find the an empty handle pointer to store the handle. */
    for (index = 0; index < FLEXIO_UART_HANDLE_COUNT; index++)
    {
        if (s_edmaPrivateHandle[index].base == NULL)
        {
            s_edmaPrivateHandle[index].base   = base;
            s_edmaPrivateHandle[index].handle = handle;
            break;
        }
    }

    if (index == FLEXIO_UART_HANDLE_COUNT)
    {
        return kStatus_OutOfRange;
    }

    memset(handle, 0, sizeof(*handle));

    handle->rxState = kFLEXIO_UART_RxIdle;
    handle->txState = kFLEXIO_UART_TxIdle;

    handle->rxEdmaHandle = rxEdmaHandle;
    handle->txEdmaHandle = txEdmaHandle;

    handle->callback = callback;
    handle->userData = userData;

    /* Configure TX. */
    if (txEdmaHandle)
    {
        EDMA_SetCallback(handle->txEdmaHandle, FLEXIO_UART_TransferSendEDMACallback, &s_edmaPrivateHandle);
    }

    /* Configure RX. */
    if (rxEdmaHandle)
    {
        EDMA_SetCallback(handle->rxEdmaHandle, FLEXIO_UART_TransferReceiveEDMACallback, &s_edmaPrivateHandle);
    }

    return kStatus_Success;
}

/*!
 * brief Sends data using eDMA.
 *
 * This function sends data using eDMA. This is a non-blocking function, which returns
 * right away. When all data is sent out, the send callback function is called.
 *
 * param base Pointer to FLEXIO_UART_Type
 * param handle UART handle pointer.
 * param xfer UART eDMA transfer structure, see #flexio_uart_transfer_t.
 * retval kStatus_Success if succeed, others failed.
 * retval kStatus_FLEXIO_UART_TxBusy Previous transfer on going.
 */
status_t FLEXIO_UART_TransferSendEDMA(FLEXIO_UART_Type *base,
                                      flexio_uart_edma_handle_t *handle,
                                      flexio_uart_transfer_t *xfer)
{
    assert(handle->txEdmaHandle);

    edma_transfer_config_t xferConfig;
    status_t status;

    /* Return error if xfer invalid. */
    if ((0U == xfer->dataSize) || (NULL == xfer->data))
    {
        return kStatus_InvalidArgument;
    }

    /* If previous TX not finished. */
    if (kFLEXIO_UART_TxBusy == handle->txState)
    {
        status = kStatus_FLEXIO_UART_TxBusy;
    }
    else
    {
        handle->txState       = kFLEXIO_UART_TxBusy;
        handle->txDataSizeAll = xfer->dataSize;

        /* Prepare transfer. */
        EDMA_PrepareTransfer(&xferConfig, xfer->data, sizeof(uint8_t),
                             (void *)FLEXIO_UART_GetTxDataRegisterAddress(base), sizeof(uint8_t), sizeof(uint8_t),
                             xfer->dataSize, kEDMA_MemoryToPeripheral);

        /* Store the initially configured eDMA minor byte transfer count into the FLEXIO UART handle */
        handle->nbytes = sizeof(uint8_t);

        /* Submit transfer. */
        EDMA_SubmitTransfer(handle->txEdmaHandle, &xferConfig);
        EDMA_StartTransfer(handle->txEdmaHandle);

        /* Enable UART TX EDMA. */
        FLEXIO_UART_EnableTxDMA(base, true);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Receives data using eDMA.
 *
 * This function receives data using eDMA. This is a non-blocking function, which returns
 * right away. When all data is received, the receive callback function is called.
 *
 * param base Pointer to FLEXIO_UART_Type
 * param handle Pointer to flexio_uart_edma_handle_t structure
 * param xfer UART eDMA transfer structure, see #flexio_uart_transfer_t.
 * retval kStatus_Success if succeed, others failed.
 * retval kStatus_UART_RxBusy Previous transfer on going.
 */
status_t FLEXIO_UART_TransferReceiveEDMA(FLEXIO_UART_Type *base,
                                         flexio_uart_edma_handle_t *handle,
                                         flexio_uart_transfer_t *xfer)
{
    assert(handle->rxEdmaHandle);

    edma_transfer_config_t xferConfig;
    status_t status;

    /* Return error if xfer invalid. */
    if ((0U == xfer->dataSize) || (NULL == xfer->data))
    {
        return kStatus_InvalidArgument;
    }

    /* If previous RX not finished. */
    if (kFLEXIO_UART_RxBusy == handle->rxState)
    {
        status = kStatus_FLEXIO_UART_RxBusy;
    }
    else
    {
        handle->rxState       = kFLEXIO_UART_RxBusy;
        handle->rxDataSizeAll = xfer->dataSize;

        /* Prepare transfer. */
        EDMA_PrepareTransfer(&xferConfig, (void *)FLEXIO_UART_GetRxDataRegisterAddress(base), sizeof(uint8_t),
                             xfer->data, sizeof(uint8_t), sizeof(uint8_t), xfer->dataSize, kEDMA_PeripheralToMemory);

        /* Store the initially configured eDMA minor byte transfer count into the FLEXIO UART handle */
        handle->nbytes = sizeof(uint8_t);

        /* Submit transfer. */
        EDMA_SubmitTransfer(handle->rxEdmaHandle, &xferConfig);
        EDMA_StartTransfer(handle->rxEdmaHandle);

        /* Enable UART RX EDMA. */
        FLEXIO_UART_EnableRxDMA(base, true);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Aborts the sent data which using eDMA.
 *
 * This function aborts sent data which using eDMA.
 *
 * param base Pointer to FLEXIO_UART_Type
 * param handle Pointer to flexio_uart_edma_handle_t structure
 */
void FLEXIO_UART_TransferAbortSendEDMA(FLEXIO_UART_Type *base, flexio_uart_edma_handle_t *handle)
{
    assert(handle->txEdmaHandle);

    /* Disable UART TX EDMA. */
    FLEXIO_UART_EnableTxDMA(base, false);

    /* Stop transfer. */
    EDMA_StopTransfer(handle->txEdmaHandle);

    handle->txState = kFLEXIO_UART_TxIdle;
}

/*!
 * brief Aborts the receive data which using eDMA.
 *
 * This function aborts the receive data which using eDMA.
 *
 * param base Pointer to FLEXIO_UART_Type
 * param handle Pointer to flexio_uart_edma_handle_t structure
 */
void FLEXIO_UART_TransferAbortReceiveEDMA(FLEXIO_UART_Type *base, flexio_uart_edma_handle_t *handle)
{
    assert(handle->rxEdmaHandle);

    /* Disable UART RX EDMA. */
    FLEXIO_UART_EnableRxDMA(base, false);

    /* Stop transfer. */
    EDMA_StopTransfer(handle->rxEdmaHandle);

    handle->rxState = kFLEXIO_UART_RxIdle;
}

/*!
 * brief Gets the number of bytes received.
 *
 * This function gets the number of bytes received.
 *
 * param base Pointer to FLEXIO_UART_Type
 * param handle Pointer to flexio_uart_edma_handle_t structure
 * param count Number of bytes received so far by the non-blocking transaction.
 * retval kStatus_NoTransferInProgress transfer has finished or no transfer in progress.
 * retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_UART_TransferGetReceiveCountEDMA(FLEXIO_UART_Type *base,
                                                 flexio_uart_edma_handle_t *handle,
                                                 size_t *count)
{
    assert(handle);
    assert(handle->rxEdmaHandle);
    assert(count);

    if (kFLEXIO_UART_RxIdle == handle->rxState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->rxDataSizeAll -
             (uint32_t)handle->nbytes *
                 EDMA_GetRemainingMajorLoopCount(handle->rxEdmaHandle->base, handle->rxEdmaHandle->channel);

    return kStatus_Success;
}

/*!
 * brief Gets the number of bytes sent out.
 *
 * This function gets the number of bytes sent out.
 *
 * param base Pointer to FLEXIO_UART_Type
 * param handle Pointer to flexio_uart_edma_handle_t structure
 * param count Number of bytes sent so far by the non-blocking transaction.
 * retval kStatus_NoTransferInProgress transfer has finished or no transfer in progress.
 * retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_UART_TransferGetSendCountEDMA(FLEXIO_UART_Type *base, flexio_uart_edma_handle_t *handle, size_t *count)
{
    assert(handle);
    assert(handle->txEdmaHandle);
    assert(count);

    if (kFLEXIO_UART_TxIdle == handle->txState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->txDataSizeAll -
             (uint32_t)handle->nbytes *
                 EDMA_GetRemainingMajorLoopCount(handle->txEdmaHandle->base, handle->txEdmaHandle->channel);

    return kStatus_Success;
}
