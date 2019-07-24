/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_SPDIF_EDMA_H_
#define _FSL_SPDIF_EDMA_H_

#include "fsl_spdif.h"
#include "fsl_edma.h"

/*!
 * @addtogroup spdif_edma
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_SPDIF_EDMA_DRIVER_VERSION (MAKE_VERSION(2, 0, 1)) /*!< Version 2.0.1 */
/*@}*/

typedef struct _spdif_edma_handle spdif_edma_handle_t;

/*! @brief SPDIF eDMA transfer callback function for finish and error */
typedef void (*spdif_edma_callback_t)(SPDIF_Type *base, spdif_edma_handle_t *handle, status_t status, void *userData);

/*! @brief SPDIF transfer structure */
typedef struct _spdif_edma_transfer
{
    uint8_t *leftData;  /*!< Data start address to transfer. */
    uint8_t *rightData; /*!< Data start address to transfer. */
    size_t dataSize;    /*!< Transfer size. */
} spdif_edma_transfer_t;

/*! @brief SPDIF DMA transfer handle, users should not touch the content of the handle.*/
struct _spdif_edma_handle
{
    edma_handle_t *dmaLeftHandle;                    /*!< DMA handler for SPDIF left channel */
    edma_handle_t *dmaRightHandle;                   /*!< DMA handler for SPDIF right channel */
    uint8_t nbytes;                                  /*!< eDMA minor byte transfer count initially configured. */
    uint8_t count;                                   /*!< The transfer data count in a DMA request */
    uint32_t state;                                  /*!< Internal state for SPDIF eDMA transfer */
    spdif_edma_callback_t callback;                  /*!< Callback for users while transfer finish or error occurs */
    void *userData;                                  /*!< User callback parameter */
    edma_tcd_t leftTcd[SPDIF_XFER_QUEUE_SIZE + 1U];  /*!< TCD pool for eDMA transfer. */
    edma_tcd_t rightTcd[SPDIF_XFER_QUEUE_SIZE + 1U]; /*!< TCD pool for eDMA transfer. */
    spdif_edma_transfer_t spdifQueue[SPDIF_XFER_QUEUE_SIZE]; /*!< Transfer queue storing queued transfer. */
    size_t transferSize[SPDIF_XFER_QUEUE_SIZE]; /*!< Data bytes need to transfer, left and right are the same, so use
                                                   one */
    volatile uint8_t queueUser;                 /*!< Index for user to queue transfer. */
    volatile uint8_t queueDriver;               /*!< Index for driver to get the transfer data and size */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name eDMA Transactional
 * @{
 */

/*!
 * @brief Initializes the SPDIF eDMA handle.
 *
 * This function initializes the SPDIF master DMA handle, which can be used for other SPDIF master transactional APIs.
 * Usually, for a specified SPDIF instance, call this API once to get the initialized handle.
 *
 * @param base SPDIF base pointer.
 * @param handle SPDIF eDMA handle pointer.
 * @param base SPDIF peripheral base address.
 * @param callback Pointer to user callback function.
 * @param userData User parameter passed to the callback function.
 * @param dmaLeftHandle eDMA handle pointer for left channel, this handle shall be static allocated by users.
 * @param dmaRightHandle eDMA handle pointer for right channel, this handle shall be static allocated by users.
 */
void SPDIF_TransferTxCreateHandleEDMA(SPDIF_Type *base,
                                      spdif_edma_handle_t *handle,
                                      spdif_edma_callback_t callback,
                                      void *userData,
                                      edma_handle_t *dmaLeftHandle,
                                      edma_handle_t *dmaRightHandle);

/*!
 * @brief Initializes the SPDIF Rx eDMA handle.
 *
 * This function initializes the SPDIF slave DMA handle, which can be used for other SPDIF master transactional APIs.
 * Usually, for a specified SPDIF instance, call this API once to get the initialized handle.
 *
 * @param base SPDIF base pointer.
 * @param handle SPDIF eDMA handle pointer.
 * @param base SPDIF peripheral base address.
 * @param callback Pointer to user callback function.
 * @param userData User parameter passed to the callback function.
 * @param dmaLeftHandle eDMA handle pointer for left channel, this handle shall be static allocated by users.
 * @param dmaRightHandle eDMA handle pointer for right channel, this handle shall be static allocated by users.
 */
void SPDIF_TransferRxCreateHandleEDMA(SPDIF_Type *base,
                                      spdif_edma_handle_t *handle,
                                      spdif_edma_callback_t callback,
                                      void *userData,
                                      edma_handle_t *dmaLeftHandle,
                                      edma_handle_t *dmaRightHandle);

/*!
 * @brief Performs a non-blocking SPDIF transfer using DMA.
 *
 * @note This interface returns immediately after the transfer initiates. Call
 * SPDIF_GetTransferStatus to poll the transfer status and check whether the SPDIF transfer is finished.
 *
 * @param base SPDIF base pointer.
 * @param handle SPDIF eDMA handle pointer.
 * @param xfer Pointer to the DMA transfer structure.
 * @retval kStatus_Success Start a SPDIF eDMA send successfully.
 * @retval kStatus_InvalidArgument The input argument is invalid.
 * @retval kStatus_TxBusy SPDIF is busy sending data.
 */
status_t SPDIF_TransferSendEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle, spdif_edma_transfer_t *xfer);

/*!
 * @brief Performs a non-blocking SPDIF receive using eDMA.
 *
 * @note This interface returns immediately after the transfer initiates. Call
 * the SPDIF_GetReceiveRemainingBytes to poll the transfer status and check whether the SPDIF transfer is finished.
 *
 * @param base SPDIF base pointer
 * @param handle SPDIF eDMA handle pointer.
 * @param xfer Pointer to DMA transfer structure.
 * @retval kStatus_Success Start a SPDIF eDMA receive successfully.
 * @retval kStatus_InvalidArgument The input argument is invalid.
 * @retval kStatus_RxBusy SPDIF is busy receiving data.
 */
status_t SPDIF_TransferReceiveEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle, spdif_edma_transfer_t *xfer);

/*!
 * @brief Aborts a SPDIF transfer using eDMA.
 *
 * @param base SPDIF base pointer.
 * @param handle SPDIF eDMA handle pointer.
 */
void SPDIF_TransferAbortSendEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle);

/*!
 * @brief Aborts a SPDIF receive using eDMA.
 *
 * @param base SPDIF base pointer
 * @param handle SPDIF eDMA handle pointer.
 */
void SPDIF_TransferAbortReceiveEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle);

/*!
 * @brief Gets byte count sent by SPDIF.
 *
 * @param base SPDIF base pointer.
 * @param handle SPDIF eDMA handle pointer.
 * @param count Bytes count sent by SPDIF.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is no non-blocking transaction in progress.
 */
status_t SPDIF_TransferGetSendCountEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle, size_t *count);

/*!
 * @brief Gets byte count received by SPDIF.
 *
 * @param base SPDIF base pointer
 * @param handle SPDIF eDMA handle pointer.
 * @param count Bytes count received by SPDIF.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is no non-blocking transaction in progress.
 */
status_t SPDIF_TransferGetReceiveCountEDMA(SPDIF_Type *base, spdif_edma_handle_t *handle, size_t *count);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif
