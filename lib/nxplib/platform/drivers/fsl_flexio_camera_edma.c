/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_camera_edma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_camera_edma"
#endif

/*<! Structure definition for camera_edma_private_handle_t. The structure is private. */
typedef struct _flexio_camera_edma_private_handle
{
    FLEXIO_CAMERA_Type *base;
    flexio_camera_edma_handle_t *handle;
} flexio_camera_edma_private_handle_t;

/* CAMERA EDMA transfer handle. */
enum _flexio_camera_edma_tansfer_states
{
    kFLEXIO_CAMERA_RxIdle, /* RX idle. */
    kFLEXIO_CAMERA_RxBusy  /* RX busy. */
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*< @brief user configurable flexio camera handle count. */
#define FLEXIO_CAMERA_HANDLE_COUNT 1

/*<! Private handle only used for internally. */
static flexio_camera_edma_private_handle_t s_edmaPrivateHandle[FLEXIO_CAMERA_HANDLE_COUNT];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief FLEXIO CAMERA EDMA receive finished callback function.
 *
 * This function is called when FLEXIO CAMERA EDMA receive finished. It disables the CAMERA
 * RX EDMA request and sends @ref kStatus_FLEXIO_CAMERA_RxIdle to CAMERA callback.
 *
 * @param handle The EDMA handle.
 * @param param Callback function parameter.
 */
static void FLEXIO_CAMERA_TransferReceiveEDMACallback(edma_handle_t *handle,
                                                      void *param,
                                                      bool transferDone,
                                                      uint32_t tcds);

/*******************************************************************************
 * Code
 ******************************************************************************/

static void FLEXIO_CAMERA_TransferReceiveEDMACallback(edma_handle_t *handle,
                                                      void *param,
                                                      bool transferDone,
                                                      uint32_t tcds)
{
    flexio_camera_edma_private_handle_t *cameraPrivateHandle = (flexio_camera_edma_private_handle_t *)param;

    /* Avoid the warning for unused variables. */
    handle = handle;
    tcds   = tcds;

    if (transferDone)
    {
        FLEXIO_CAMERA_TransferAbortReceiveEDMA(cameraPrivateHandle->base, cameraPrivateHandle->handle);

        if (cameraPrivateHandle->handle->callback)
        {
            cameraPrivateHandle->handle->callback(cameraPrivateHandle->base, cameraPrivateHandle->handle,
                                                  kStatus_FLEXIO_CAMERA_RxIdle, cameraPrivateHandle->handle->userData);
        }
    }
}
/*!
 * brief Initializes the Camera handle, which is used in transactional functions.
 *
 * param base Pointer to the FLEXIO_CAMERA_Type.
 * param handle Pointer to flexio_camera_edma_handle_t structure.
 * param callback The callback function.
 * param userData The parameter of the callback function.
 * param rxEdmaHandle User requested DMA handle for RX DMA transfer.
 * retval kStatus_Success Successfully create the handle.
 * retval kStatus_OutOfRange The FlexIO Camera eDMA type/handle table out of range.
 */
status_t FLEXIO_CAMERA_TransferCreateHandleEDMA(FLEXIO_CAMERA_Type *base,
                                                flexio_camera_edma_handle_t *handle,
                                                flexio_camera_edma_transfer_callback_t callback,
                                                void *userData,
                                                edma_handle_t *rxEdmaHandle)
{
    assert(handle);

    uint8_t index = 0;

    /* Find the an empty handle pointer to store the handle. */
    for (index = 0; index < FLEXIO_CAMERA_HANDLE_COUNT; index++)
    {
        if (s_edmaPrivateHandle[index].base == NULL)
        {
            s_edmaPrivateHandle[index].base   = base;
            s_edmaPrivateHandle[index].handle = handle;
            break;
        }
    }

    if (index == FLEXIO_CAMERA_HANDLE_COUNT)
    {
        return kStatus_OutOfRange;
    }

    s_edmaPrivateHandle[index].base   = base;
    s_edmaPrivateHandle[index].handle = handle;

    memset(handle, 0, sizeof(*handle));

    handle->rxState      = kFLEXIO_CAMERA_RxIdle;
    handle->rxEdmaHandle = rxEdmaHandle;

    handle->callback = callback;
    handle->userData = userData;

    /* Configure RX. */
    if (rxEdmaHandle)
    {
        EDMA_SetCallback(handle->rxEdmaHandle, FLEXIO_CAMERA_TransferReceiveEDMACallback, &s_edmaPrivateHandle);
    }

    return kStatus_Success;
}

/*!
 * brief Receives data using eDMA.
 *
 * This function receives data using eDMA. This is a non-blocking function, which returns
 * right away. When all data is received, the receive callback function is called.
 *
 * param base Pointer to the FLEXIO_CAMERA_Type.
 * param handle Pointer to the flexio_camera_edma_handle_t structure.
 * param xfer Camera eDMA transfer structure, see #flexio_camera_transfer_t.
 * retval kStatus_Success if succeeded, others failed.
 * retval kStatus_CAMERA_RxBusy Previous transfer on going.
 */
status_t FLEXIO_CAMERA_TransferReceiveEDMA(FLEXIO_CAMERA_Type *base,
                                           flexio_camera_edma_handle_t *handle,
                                           flexio_camera_transfer_t *xfer)
{
    assert(handle->rxEdmaHandle);

    edma_transfer_config_t xferConfig;
    status_t status;

    /* If previous RX not finished. */
    if (kFLEXIO_CAMERA_RxBusy == handle->rxState)
    {
        status = kStatus_FLEXIO_CAMERA_RxBusy;
    }
    else
    {
        handle->rxState = kFLEXIO_CAMERA_RxBusy;

        /* Prepare transfer. */
        EDMA_PrepareTransfer(&xferConfig, (void *)FLEXIO_CAMERA_GetRxBufferAddress(base), 32, (void *)xfer->dataAddress,
                             32, 32, xfer->dataNum, kEDMA_PeripheralToMemory);

        /* Store the initially configured eDMA minor byte transfer count into the FLEXIO CAMERA handle */
        handle->nbytes = 32;

        /* Submit transfer. */
        EDMA_SubmitTransfer(handle->rxEdmaHandle, &xferConfig);
        EDMA_StartTransfer(handle->rxEdmaHandle);
        /* Enable CAMERA RX EDMA. */
        FLEXIO_CAMERA_EnableRxDMA(base, true);
        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Aborts the receive data which used the eDMA.
 *
 * This function aborts the receive data which used the eDMA.
 *
 * param base Pointer to the FLEXIO_CAMERA_Type.
 * param handle Pointer to the flexio_camera_edma_handle_t structure.
 */
void FLEXIO_CAMERA_TransferAbortReceiveEDMA(FLEXIO_CAMERA_Type *base, flexio_camera_edma_handle_t *handle)
{
    assert(handle->rxEdmaHandle);

    /* Disable CAMERA RX EDMA. */
    FLEXIO_CAMERA_EnableRxDMA(base, false);

    /* Stop transfer. */
    EDMA_StopTransfer(handle->rxEdmaHandle);

    handle->rxState = kFLEXIO_CAMERA_RxIdle;
}

/*!
 * brief Gets the remaining bytes to be received.
 *
 * This function gets the number of bytes still not received.
 *
 * param base Pointer to the FLEXIO_CAMERA_Type.
 * param handle Pointer to the flexio_camera_edma_handle_t structure.
 * param count Number of bytes sent so far by the non-blocking transaction.
 * retval kStatus_Success Succeed get the transfer count.
 * retval kStatus_InvalidArgument The count parameter is invalid.
 */
status_t FLEXIO_CAMERA_TransferGetReceiveCountEDMA(FLEXIO_CAMERA_Type *base,
                                                   flexio_camera_edma_handle_t *handle,
                                                   size_t *count)
{
    assert(handle->rxEdmaHandle);

    if (!count)
    {
        return kStatus_InvalidArgument;
    }

    if (kFLEXIO_CAMERA_RxBusy == handle->rxState)
    {
        *count = (handle->rxSize -
                  (uint32_t)handle->nbytes *
                      EDMA_GetRemainingMajorLoopCount(handle->rxEdmaHandle->base, handle->rxEdmaHandle->channel));
    }
    else
    {
        *count = handle->rxSize;
    }

    return kStatus_Success;
}
