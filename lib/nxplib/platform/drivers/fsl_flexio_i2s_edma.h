/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_FLEXIO_I2S_EDMA_H_
#define _FSL_FLEXIO_I2S_EDMA_H_

#include "fsl_flexio_i2s.h"
#include "fsl_edma.h"

/*!
 * @addtogroup flexio_edma_i2s
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO I2S EDMA driver version 2.1.5. */
#define FSL_FLEXIO_I2S_EDMA_DRIVER_VERSION (MAKE_VERSION(2, 1, 5))
/*@}*/

typedef struct _flexio_i2s_edma_handle flexio_i2s_edma_handle_t;

/*! @brief FlexIO I2S eDMA transfer callback function for finish and error */
typedef void (*flexio_i2s_edma_callback_t)(FLEXIO_I2S_Type *base,
                                           flexio_i2s_edma_handle_t *handle,
                                           status_t status,
                                           void *userData);

/*! @brief FlexIO I2S DMA transfer handle, users should not touch the content of the handle.*/
struct _flexio_i2s_edma_handle
{
    edma_handle_t *dmaHandle;                        /*!< DMA handler for FlexIO I2S send */
    uint8_t bytesPerFrame;                           /*!< Bytes in a frame */
    uint8_t nbytes;                                  /*!< eDMA minor byte transfer count initially configured. */
    uint32_t state;                                  /*!< Internal state for FlexIO I2S eDMA transfer */
    flexio_i2s_edma_callback_t callback;             /*!< Callback for users while transfer finish or error occurred */
    void *userData;                                  /*!< User callback parameter */
    edma_tcd_t tcd[FLEXIO_I2S_XFER_QUEUE_SIZE + 1U]; /*!< TCD pool for eDMA transfer. */
    flexio_i2s_transfer_t queue[FLEXIO_I2S_XFER_QUEUE_SIZE]; /*!< Transfer queue storing queued transfer. */
    size_t transferSize[FLEXIO_I2S_XFER_QUEUE_SIZE];         /*!< Data bytes need to transfer */
    volatile uint8_t queueUser;                              /*!< Index for user to queue transfer. */
    volatile uint8_t queueDriver;                            /*!< Index for driver to get the transfer data and size */
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
 * @brief Initializes the FlexIO I2S eDMA handle.
 *
 * This function initializes the FlexIO I2S master DMA handle which can be used for other FlexIO I2S master
 * transactional APIs.
 * Usually, for a specified FlexIO I2S instance, call this API once to get the initialized handle.
 *
 * @param base FlexIO I2S peripheral base address.
 * @param handle FlexIO I2S eDMA handle pointer.
 * @param callback FlexIO I2S eDMA callback function called while finished a block.
 * @param userData User parameter for callback.
 * @param dmaHandle eDMA handle for FlexIO I2S. This handle is a static value allocated by users.
 */
void FLEXIO_I2S_TransferTxCreateHandleEDMA(FLEXIO_I2S_Type *base,
                                           flexio_i2s_edma_handle_t *handle,
                                           flexio_i2s_edma_callback_t callback,
                                           void *userData,
                                           edma_handle_t *dmaHandle);

/*!
 * @brief Initializes the FlexIO I2S Rx eDMA handle.
 *
 * This function initializes the FlexIO I2S slave DMA handle which can be used for other FlexIO I2S master transactional
 * APIs.
 * Usually, for a specified FlexIO I2S instance, call this API once to get the initialized handle.
 *
 * @param base FlexIO I2S peripheral base address.
 * @param handle FlexIO I2S eDMA handle pointer.
 * @param callback FlexIO I2S eDMA callback function called while finished a block.
 * @param userData User parameter for callback.
 * @param dmaHandle eDMA handle for FlexIO I2S. This handle is a static value allocated by users.
 */
void FLEXIO_I2S_TransferRxCreateHandleEDMA(FLEXIO_I2S_Type *base,
                                           flexio_i2s_edma_handle_t *handle,
                                           flexio_i2s_edma_callback_t callback,
                                           void *userData,
                                           edma_handle_t *dmaHandle);

/*!
 * @brief Configures the FlexIO I2S Tx audio format.
 *
 * Audio format can be changed in run-time of FlexIO I2S. This function configures the sample rate and audio data
 * format to be transferred. This function also sets the eDMA parameter according to format.
 *
 * @param base FlexIO I2S peripheral base address.
 * @param handle FlexIO I2S eDMA handle pointer
 * @param format Pointer to FlexIO I2S audio data format structure.
 * @param srcClock_Hz FlexIO I2S clock source frequency in Hz, it should be 0 while in slave mode.
 * @retval kStatus_Success Audio format set successfully.
 * @retval kStatus_InvalidArgument The input arguments is invalid.
 */
void FLEXIO_I2S_TransferSetFormatEDMA(FLEXIO_I2S_Type *base,
                                      flexio_i2s_edma_handle_t *handle,
                                      flexio_i2s_format_t *format,
                                      uint32_t srcClock_Hz);

/*!
 * @brief Performs a non-blocking FlexIO I2S transfer using DMA.
 *
 * @note This interface returned immediately after transfer initiates. Users should call
 * FLEXIO_I2S_GetTransferStatus to poll the transfer status and check whether the FlexIO I2S transfer is finished.
 *
 * @param base FlexIO I2S peripheral base address.
 * @param handle FlexIO I2S DMA handle pointer.
 * @param xfer Pointer to DMA transfer structure.
 * @retval kStatus_Success Start a FlexIO I2S eDMA send successfully.
 * @retval kStatus_InvalidArgument The input arguments is invalid.
 * @retval kStatus_TxBusy FlexIO I2S is busy sending data.
 */
status_t FLEXIO_I2S_TransferSendEDMA(FLEXIO_I2S_Type *base,
                                     flexio_i2s_edma_handle_t *handle,
                                     flexio_i2s_transfer_t *xfer);

/*!
 * @brief Performs a non-blocking FlexIO I2S receive using eDMA.
 *
 * @note This interface returned immediately after transfer initiates. Users should call
 * FLEXIO_I2S_GetReceiveRemainingBytes to poll the transfer status and check whether the FlexIO I2S transfer is
 * finished.
 *
 * @param base FlexIO I2S peripheral base address.
 * @param handle FlexIO I2S DMA handle pointer.
 * @param xfer Pointer to DMA transfer structure.
 * @retval kStatus_Success Start a FlexIO I2S eDMA receive successfully.
 * @retval kStatus_InvalidArgument The input arguments is invalid.
 * @retval kStatus_RxBusy FlexIO I2S is busy receiving data.
 */
status_t FLEXIO_I2S_TransferReceiveEDMA(FLEXIO_I2S_Type *base,
                                        flexio_i2s_edma_handle_t *handle,
                                        flexio_i2s_transfer_t *xfer);

/*!
 * @brief Aborts a FlexIO I2S transfer using eDMA.
 *
 * @param base FlexIO I2S peripheral base address.
 * @param handle FlexIO I2S DMA handle pointer.
 */
void FLEXIO_I2S_TransferAbortSendEDMA(FLEXIO_I2S_Type *base, flexio_i2s_edma_handle_t *handle);

/*!
 * @brief Aborts a FlexIO I2S receive using eDMA.
 *
 * @param base FlexIO I2S peripheral base address.
 * @param handle FlexIO I2S DMA handle pointer.
 */
void FLEXIO_I2S_TransferAbortReceiveEDMA(FLEXIO_I2S_Type *base, flexio_i2s_edma_handle_t *handle);

/*!
 * @brief Gets the remaining bytes to be sent.
 *
 * @param base FlexIO I2S peripheral base address.
 * @param handle FlexIO I2S DMA handle pointer.
 * @param count Bytes sent.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t FLEXIO_I2S_TransferGetSendCountEDMA(FLEXIO_I2S_Type *base, flexio_i2s_edma_handle_t *handle, size_t *count);

/*!
 * @brief Get the remaining bytes to be received.
 *
 * @param base FlexIO I2S peripheral base address.
 * @param handle FlexIO I2S DMA handle pointer.
 * @param count Bytes received.
 * @retval kStatus_Success Succeed get the transfer count.
 * @retval kStatus_NoTransferInProgress There is not a non-blocking transaction currently in progress.
 */
status_t FLEXIO_I2S_TransferGetReceiveCountEDMA(FLEXIO_I2S_Type *base, flexio_i2s_edma_handle_t *handle, size_t *count);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif
