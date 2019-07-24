/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_FLEXIO_CAMERA_EDMA_H_
#define _FSL_FLEXIO_CAMERA_EDMA_H_

#include "fsl_flexio_camera.h"
#include "fsl_edma.h"

/*!
 * @addtogroup flexio_edma_camera
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO Camera EDMA driver version 2.1.2. */
#define FSL_FLEXIO_CAMERA_EDMA_DRIVER_VERSION (MAKE_VERSION(2, 1, 2))
/*@}*/

/*! @brief Forward declaration of the handle typedef. */
typedef struct _flexio_camera_edma_handle flexio_camera_edma_handle_t;

/*! @brief Camera transfer callback function. */
typedef void (*flexio_camera_edma_transfer_callback_t)(FLEXIO_CAMERA_Type *base,
                                                       flexio_camera_edma_handle_t *handle,
                                                       status_t status,
                                                       void *userData);

/*!
 * @brief Camera eDMA handle
 */
struct _flexio_camera_edma_handle
{
    flexio_camera_edma_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                                  /*!< Camera callback function parameter.*/
    size_t rxSize;                                   /*!< Total bytes to be received. */
    edma_handle_t *rxEdmaHandle;                     /*!< The eDMA RX channel used. */
    uint8_t nbytes;                                  /*!< eDMA minor byte transfer count initially configured. */
    volatile uint8_t rxState;                        /*!< RX transfer state */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name eDMA transactional
 * @{
 */

/*!
 * @brief Initializes the Camera handle, which is used in transactional functions.
 *
 * @param base Pointer to the FLEXIO_CAMERA_Type.
 * @param handle Pointer to flexio_camera_edma_handle_t structure.
 * @param callback The callback function.
 * @param userData The parameter of the callback function.
 * @param rxEdmaHandle User requested DMA handle for RX DMA transfer.
 * @retval kStatus_Success Successfully create the handle.
 * @retval kStatus_OutOfRange The FlexIO Camera eDMA type/handle table out of range.
 */
status_t FLEXIO_CAMERA_TransferCreateHandleEDMA(FLEXIO_CAMERA_Type *base,
                                                flexio_camera_edma_handle_t *handle,
                                                flexio_camera_edma_transfer_callback_t callback,
                                                void *userData,
                                                edma_handle_t *rxEdmaHandle);

/*!
 * @brief Receives data using eDMA.
 *
 * This function receives data using eDMA. This is a non-blocking function, which returns
 * right away. When all data is received, the receive callback function is called.
 *
 * @param base Pointer to the FLEXIO_CAMERA_Type.
 * @param handle Pointer to the flexio_camera_edma_handle_t structure.
 * @param xfer Camera eDMA transfer structure, see #flexio_camera_transfer_t.
 * @retval kStatus_Success if succeeded, others failed.
 * @retval kStatus_CAMERA_RxBusy Previous transfer on going.
 */
status_t FLEXIO_CAMERA_TransferReceiveEDMA(FLEXIO_CAMERA_Type *base,
                                           flexio_camera_edma_handle_t *handle,
                                           flexio_camera_transfer_t *xfer);

/*!
 * @brief Aborts the receive data which used the eDMA.
 *
 * This function aborts the receive data which used the eDMA.
 *
 * @param base Pointer to the FLEXIO_CAMERA_Type.
 * @param handle Pointer to the flexio_camera_edma_handle_t structure.
 */
void FLEXIO_CAMERA_TransferAbortReceiveEDMA(FLEXIO_CAMERA_Type *base, flexio_camera_edma_handle_t *handle);

/*!
 * @brief Gets the remaining bytes to be received.
 *
 * This function gets the number of bytes still not received.
 *
 * @param base Pointer to the FLEXIO_CAMERA_Type.
 * @param handle Pointer to the flexio_camera_edma_handle_t structure.
 * @param count Number of bytes sent so far by the non-blocking transaction.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_InvalidArgument The count parameter is invalid.
 */
status_t FLEXIO_CAMERA_TransferGetReceiveCountEDMA(FLEXIO_CAMERA_Type *base,
                                                   flexio_camera_edma_handle_t *handle,
                                                   size_t *count);

/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_CAMERA_EDMA_H_ */
