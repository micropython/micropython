/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_lpuart_edma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpuart_edma"
#endif

/*<! Structure definition for lpuart_edma_private_handle_t. The structure is private. */
typedef struct _lpuart_edma_private_handle
{
    LPUART_Type *base;
    lpuart_edma_handle_t *handle;
} lpuart_edma_private_handle_t;

/* LPUART EDMA transfer handle. */
enum _lpuart_edma_tansfer_states
{
    kLPUART_TxIdle, /* TX idle. */
    kLPUART_TxBusy, /* TX busy. */
    kLPUART_RxIdle, /* RX idle. */
    kLPUART_RxBusy  /* RX busy. */
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Array of LPUART handle. */
#if (defined(LPUART8))
#define LPUART_HANDLE_ARRAY_SIZE 9
#else /* LPUART8 */
#if (defined(LPUART7))
#define LPUART_HANDLE_ARRAY_SIZE 8
#else /* LPUART7 */
#if (defined(LPUART6))
#define LPUART_HANDLE_ARRAY_SIZE 7
#else /* LPUART6 */
#if (defined(LPUART5))
#define LPUART_HANDLE_ARRAY_SIZE 6
#else /* LPUART5 */
#if (defined(LPUART4))
#define LPUART_HANDLE_ARRAY_SIZE 5
#else /* LPUART4 */
#if (defined(LPUART3))
#define LPUART_HANDLE_ARRAY_SIZE 4
#else /* LPUART3 */
#if (defined(LPUART2))
#define LPUART_HANDLE_ARRAY_SIZE 3
#else /* LPUART2 */
#if (defined(LPUART1))
#define LPUART_HANDLE_ARRAY_SIZE 2
#else /* LPUART1 */
#if (defined(LPUART0))
#define LPUART_HANDLE_ARRAY_SIZE 1
#else /* LPUART0 */
#define LPUART_HANDLE_ARRAY_SIZE FSL_FEATURE_SOC_LPUART_COUNT
#endif /* LPUART 0 */
#endif /* LPUART 1 */
#endif /* LPUART 2 */
#endif /* LPUART 3 */
#endif /* LPUART 4 */
#endif /* LPUART 5 */
#endif /* LPUART 6 */
#endif /* LPUART 7 */
#endif /* LPUART 8 */

/*<! Private handle only used for internally. */
static lpuart_edma_private_handle_t s_lpuartEdmaPrivateHandle[LPUART_HANDLE_ARRAY_SIZE];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief LPUART EDMA send finished callback function.
 *
 * This function is called when LPUART EDMA send finished. It disables the LPUART
 * TX EDMA request and sends @ref kStatus_LPUART_TxIdle to LPUART callback.
 *
 * @param handle The EDMA handle.
 * @param param Callback function parameter.
 */
static void LPUART_SendEDMACallback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);

/*!
 * @brief LPUART EDMA receive finished callback function.
 *
 * This function is called when LPUART EDMA receive finished. It disables the LPUART
 * RX EDMA request and sends @ref kStatus_LPUART_RxIdle to LPUART callback.
 *
 * @param handle The EDMA handle.
 * @param param Callback function parameter.
 */
static void LPUART_ReceiveEDMACallback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);

/*******************************************************************************
 * Code
 ******************************************************************************/

static void LPUART_SendEDMACallback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    assert(NULL != param);

    lpuart_edma_private_handle_t *lpuartPrivateHandle = (lpuart_edma_private_handle_t *)param;

    /* Avoid the warning for unused variables. */
    handle = handle;
    tcds   = tcds;

    if (transferDone)
    {
        LPUART_TransferAbortSendEDMA(lpuartPrivateHandle->base, lpuartPrivateHandle->handle);

        if (NULL != lpuartPrivateHandle->handle->callback)
        {
            lpuartPrivateHandle->handle->callback(lpuartPrivateHandle->base, lpuartPrivateHandle->handle,
                                                  kStatus_LPUART_TxIdle, lpuartPrivateHandle->handle->userData);
        }
    }
}

static void LPUART_ReceiveEDMACallback(edma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    assert(NULL != param);

    lpuart_edma_private_handle_t *lpuartPrivateHandle = (lpuart_edma_private_handle_t *)param;

    /* Avoid warning for unused parameters. */
    handle = handle;
    tcds   = tcds;

    if (transferDone)
    {
        /* Disable transfer. */
        LPUART_TransferAbortReceiveEDMA(lpuartPrivateHandle->base, lpuartPrivateHandle->handle);

        if (NULL != lpuartPrivateHandle->handle->callback)
        {
            lpuartPrivateHandle->handle->callback(lpuartPrivateHandle->base, lpuartPrivateHandle->handle,
                                                  kStatus_LPUART_RxIdle, lpuartPrivateHandle->handle->userData);
        }
    }
}

/*!
 * brief Initializes the LPUART handle which is used in transactional functions.
 * param base LPUART peripheral base address.
 * param handle Pointer to lpuart_edma_handle_t structure.
 * param callback Callback function.
 * param userData User data.
 * param txEdmaHandle User requested DMA handle for TX DMA transfer.
 * param rxEdmaHandle User requested DMA handle for RX DMA transfer.
 */
void LPUART_TransferCreateHandleEDMA(LPUART_Type *base,
                                     lpuart_edma_handle_t *handle,
                                     lpuart_edma_transfer_callback_t callback,
                                     void *userData,
                                     edma_handle_t *txEdmaHandle,
                                     edma_handle_t *rxEdmaHandle)
{
    assert(NULL != handle);

    uint32_t instance = LPUART_GetInstance(base);

    s_lpuartEdmaPrivateHandle[instance].base   = base;
    s_lpuartEdmaPrivateHandle[instance].handle = handle;

    (void)memset(handle, 0, sizeof(*handle));

    handle->rxState = (uint8_t)kLPUART_RxIdle;
    handle->txState = (uint8_t)kLPUART_TxIdle;

    handle->rxEdmaHandle = rxEdmaHandle;
    handle->txEdmaHandle = txEdmaHandle;

    handle->callback = callback;
    handle->userData = userData;

#if defined(FSL_FEATURE_LPUART_HAS_FIFO) && FSL_FEATURE_LPUART_HAS_FIFO
    /* Note:
       Take care of the RX FIFO, EDMA request only assert when received bytes
       equal or more than RX water mark, there is potential issue if RX water
       mark larger than 1.
       For example, if RX FIFO water mark is 2, upper layer needs 5 bytes and
       5 bytes are received. the last byte will be saved in FIFO but not trigger
       EDMA transfer because the water mark is 2.
     */
    if (NULL != rxEdmaHandle)
    {
        base->WATER &= (~LPUART_WATER_RXWATER_MASK);
    }
#endif

    /* Configure TX. */
    if (NULL != txEdmaHandle)
    {
        EDMA_SetCallback(handle->txEdmaHandle, LPUART_SendEDMACallback, &s_lpuartEdmaPrivateHandle[instance]);
    }

    /* Configure RX. */
    if (NULL != rxEdmaHandle)
    {
        EDMA_SetCallback(handle->rxEdmaHandle, LPUART_ReceiveEDMACallback, &s_lpuartEdmaPrivateHandle[instance]);
    }
}

/*!
 * brief Sends data using eDMA.
 *
 * This function sends data using eDMA. This is a non-blocking function, which returns
 * right away. When all data is sent, the send callback function is called.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 * param xfer LPUART eDMA transfer structure. See #lpuart_transfer_t.
 * retval kStatus_Success if succeed, others failed.
 * retval kStatus_LPUART_TxBusy Previous transfer on going.
 * retval kStatus_InvalidArgument Invalid argument.
 */
status_t LPUART_SendEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle, lpuart_transfer_t *xfer)
{
    assert(NULL != handle);
    assert(NULL != handle->txEdmaHandle);
    assert(NULL != xfer);
    assert(NULL != xfer->data);
    assert(0U != xfer->dataSize);

    edma_transfer_config_t xferConfig;
    status_t status;

    /* If previous TX not finished. */
    if ((uint8_t)kLPUART_TxBusy == handle->txState)
    {
        status = kStatus_LPUART_TxBusy;
    }
    else
    {
        handle->txState       = (uint8_t)kLPUART_TxBusy;
        handle->txDataSizeAll = xfer->dataSize;

        /* Prepare transfer. */
        EDMA_PrepareTransfer(&xferConfig, xfer->data, sizeof(uint8_t),
                             (void *)(uint32_t *)LPUART_GetDataRegisterAddress(base), sizeof(uint8_t), sizeof(uint8_t),
                             xfer->dataSize, kEDMA_MemoryToPeripheral);

        /* Store the initially configured eDMA minor byte transfer count into the LPUART handle */
        handle->nbytes = (uint8_t)sizeof(uint8_t);

        /* Submit transfer. */
        if (kStatus_Success !=
            EDMA_SubmitTransfer(handle->txEdmaHandle, (const edma_transfer_config_t *)(uint32_t)&xferConfig))
        {
            return kStatus_Fail;
        }
        EDMA_StartTransfer(handle->txEdmaHandle);

        /* Enable LPUART TX EDMA. */
        LPUART_EnableTxDMA(base, true);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Receives data using eDMA.
 *
 * This function receives data using eDMA. This is non-blocking function, which returns
 * right away. When all data is received, the receive callback function is called.
 *
 * param base LPUART peripheral base address.
 * param handle Pointer to lpuart_edma_handle_t structure.
 * param xfer LPUART eDMA transfer structure, see #lpuart_transfer_t.
 * retval kStatus_Success if succeed, others fail.
 * retval kStatus_LPUART_RxBusy Previous transfer ongoing.
 * retval kStatus_InvalidArgument Invalid argument.
 */
status_t LPUART_ReceiveEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle, lpuart_transfer_t *xfer)
{
    assert(NULL != handle);
    assert(NULL != handle->rxEdmaHandle);
    assert(NULL != xfer);
    assert(NULL != xfer->data);
    assert(0U != xfer->dataSize);

    edma_transfer_config_t xferConfig;
    status_t status;

    /* If previous RX not finished. */
    if ((uint8_t)kLPUART_RxBusy == handle->rxState)
    {
        status = kStatus_LPUART_RxBusy;
    }
    else
    {
        handle->rxState       = (uint8_t)kLPUART_RxBusy;
        handle->rxDataSizeAll = xfer->dataSize;

        /* Prepare transfer. */
        EDMA_PrepareTransfer(&xferConfig, (void *)(uint32_t *)LPUART_GetDataRegisterAddress(base), sizeof(uint8_t),
                             xfer->data, sizeof(uint8_t), sizeof(uint8_t), xfer->dataSize, kEDMA_PeripheralToMemory);

        /* Store the initially configured eDMA minor byte transfer count into the LPUART handle */
        handle->nbytes = (uint8_t)sizeof(uint8_t);

        /* Submit transfer. */
        if (kStatus_Success !=
            EDMA_SubmitTransfer(handle->rxEdmaHandle, (const edma_transfer_config_t *)(uint32_t)&xferConfig))
        {
            return kStatus_Fail;
        }
        EDMA_StartTransfer(handle->rxEdmaHandle);

        /* Enable LPUART RX EDMA. */
        LPUART_EnableRxDMA(base, true);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Aborts the sent data using eDMA.
 *
 * This function aborts the sent data using eDMA.
 *
 * param base LPUART peripheral base address.
 * param handle Pointer to lpuart_edma_handle_t structure.
 */
void LPUART_TransferAbortSendEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle)
{
    assert(NULL != handle);
    assert(NULL != handle->txEdmaHandle);

    /* Disable LPUART TX EDMA. */
    LPUART_EnableTxDMA(base, false);

    /* Stop transfer. */
    EDMA_AbortTransfer(handle->txEdmaHandle);

    handle->txState = (uint8_t)kLPUART_TxIdle;
}

/*!
 * brief Aborts the received data using eDMA.
 *
 * This function aborts the received data using eDMA.
 *
 * param base LPUART peripheral base address.
 * param handle Pointer to lpuart_edma_handle_t structure.
 */
void LPUART_TransferAbortReceiveEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle)
{
    assert(NULL != handle);
    assert(NULL != handle->rxEdmaHandle);

    /* Disable LPUART RX EDMA. */
    LPUART_EnableRxDMA(base, false);

    /* Stop transfer. */
    EDMA_AbortTransfer(handle->rxEdmaHandle);

    handle->rxState = (uint8_t)kLPUART_RxIdle;
}

/*!
 * brief Gets the number of received bytes.
 *
 * This function gets the number of received bytes.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 * param count Receive bytes count.
 * retval kStatus_NoTransferInProgress No receive in progress.
 * retval kStatus_InvalidArgument Parameter is invalid.
 * retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t LPUART_TransferGetReceiveCountEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != handle->rxEdmaHandle);
    assert(NULL != count);

    if ((uint8_t)kLPUART_RxIdle == handle->rxState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->rxDataSizeAll -
             ((uint32_t)handle->nbytes *
              EDMA_GetRemainingMajorLoopCount(handle->rxEdmaHandle->base, handle->rxEdmaHandle->channel));

    return kStatus_Success;
}

/*!
 * brief Gets the number of bytes written to the LPUART TX register.
 *
 * This function gets the number of bytes written to the LPUART TX
 * register by DMA.
 *
 * param base LPUART peripheral base address.
 * param handle LPUART handle pointer.
 * param count Send bytes count.
 * retval kStatus_NoTransferInProgress No send in progress.
 * retval kStatus_InvalidArgument Parameter is invalid.
 * retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t LPUART_TransferGetSendCountEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != handle->txEdmaHandle);
    assert(NULL != count);

    if ((uint8_t)kLPUART_TxIdle == handle->txState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->txDataSizeAll -
             ((uint32_t)handle->nbytes *
              EDMA_GetRemainingMajorLoopCount(handle->txEdmaHandle->base, handle->txEdmaHandle->channel));

    return kStatus_Success;
}
