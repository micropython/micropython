/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FLEXIO_MCULCD_EDMA_H_
#define _FSL_FLEXIO_MCULCD_EDMA_H_

#include "fsl_edma.h"
#include "fsl_flexio_mculcd.h"

/*!
 * @addtogroup flexio_edma_mculcd
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*@{*/
/*! @brief FlexIO MCULCD EDMA driver version 2.0.2. */
#define FSL_FLEXIO_MCULCD_EDMA_DRIVER_VERSION (MAKE_VERSION(2, 0, 2))
/*@}*/

/*! @brief  typedef for flexio_mculcd_edma_handle_t in advance. */
typedef struct _flexio_mculcd_edma_handle flexio_mculcd_edma_handle_t;

/*! @brief FlexIO MCULCD master callback for transfer complete.
 *
 * When transfer finished, the callback function is called and returns the
 * @p status as kStatus_FLEXIO_MCULCD_Idle.
 */
typedef void (*flexio_mculcd_edma_transfer_callback_t)(FLEXIO_MCULCD_Type *base,
                                                       flexio_mculcd_edma_handle_t *handle,
                                                       status_t status,
                                                       void *userData);

/*! @brief FlexIO MCULCD eDMA transfer handle, users should not touch the
 * content of the handle.*/
struct _flexio_mculcd_edma_handle
{
    FLEXIO_MCULCD_Type *base;       /*!< Pointer to the FLEXIO_MCULCD_Type. */
    uint8_t txShifterNum;           /*!< Number of shifters used for TX. */
    uint8_t rxShifterNum;           /*!< Number of shifters used for RX. */
    uint32_t minorLoopBytes;        /*!< eDMA transfer minor loop bytes. */
    edma_modulo_t txEdmaModulo;     /*!< Modulo value for the FlexIO shifter buffer access. */
    edma_modulo_t rxEdmaModulo;     /*!< Modulo value for the FlexIO shifter buffer access. */
    uint32_t dataAddrOrSameValue;   /*!< When sending the same value for many times,
                                         this is the value to send. When writing or
                                         reading array, this is the address of the
                                         data array. */
    size_t dataCount;               /*!< Total count to be transferred. */
    volatile size_t remainingCount; /*!< Remaining count still not transfered. */
    volatile uint32_t state;        /*!< FlexIO MCULCD driver internal state. */
    edma_handle_t *txDmaHandle;     /*!< DMA handle for MCULCD TX */
    edma_handle_t *rxDmaHandle;     /*!< DMA handle for MCULCD RX */
    flexio_mculcd_edma_transfer_callback_t completionCallback; /*!< Callback for MCULCD DMA transfer */
    void *userData;                                            /*!< User Data for MCULCD DMA callback */
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
 * @brief Initializes the FLEXO MCULCD master eDMA handle.
 *
 * This function initializes the FLEXO MCULCD master eDMA handle which can be
 * used for other FLEXO MCULCD transactional APIs. For a specified FLEXO MCULCD
 * instance, call this API once to get the initialized handle.
 *
 * @param base Pointer to FLEXIO_MCULCD_Type structure.
 * @param handle Pointer to flexio_mculcd_edma_handle_t structure to store the
 * transfer state.
 * @param callback MCULCD transfer complete callback, NULL means no callback.
 * @param userData callback function parameter.
 * @param txDmaHandle User requested eDMA handle for FlexIO MCULCD eDMA TX,
 * the DMA request source of this handle should be the first of TX shifters.
 * @param rxDmaHandle User requested eDMA handle for FlexIO MCULCD eDMA RX,
 * the DMA request source of this handle should be the last of RX shifters.
 * @retval kStatus_Success Successfully create the handle.
 */
status_t FLEXIO_MCULCD_TransferCreateHandleEDMA(FLEXIO_MCULCD_Type *base,
                                                flexio_mculcd_edma_handle_t *handle,
                                                flexio_mculcd_edma_transfer_callback_t callback,
                                                void *userData,
                                                edma_handle_t *txDmaHandle,
                                                edma_handle_t *rxDmaHandle);

/*!
 * @brief Performs a non-blocking FlexIO MCULCD transfer using eDMA.
 *
 * This function returns immediately after transfer initiates. To check whether
 * the transfer is completed, user could:
 * 1. Use the transfer completed callback;
 * 2. Polling function @ref FLEXIO_MCULCD_GetTransferCountEDMA
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle pointer to flexio_mculcd_edma_handle_t structure to store the
 * transfer state.
 * @param xfer Pointer to FlexIO MCULCD transfer structure.
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_FLEXIO_MCULCD_Busy FlexIO MCULCD is not idle, it is running another
 * transfer.
 */
status_t FLEXIO_MCULCD_TransferEDMA(FLEXIO_MCULCD_Type *base,
                                    flexio_mculcd_edma_handle_t *handle,
                                    flexio_mculcd_transfer_t *xfer);

/*!
 * @brief Aborts a FlexIO MCULCD transfer using eDMA.
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle FlexIO MCULCD eDMA handle pointer.
 */
void FLEXIO_MCULCD_TransferAbortEDMA(FLEXIO_MCULCD_Type *base, flexio_mculcd_edma_handle_t *handle);

/*!
 * @brief Gets the remaining bytes for FlexIO MCULCD eDMA transfer.
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle FlexIO MCULCD eDMA handle pointer.
 * @param count Number of count transferred so far by the eDMA transaction.
 * @retval kStatus_Success Get the transferred count Successfully.
 * @retval kStatus_NoTransferInProgress No transfer in process.
 */
status_t FLEXIO_MCULCD_TransferGetCountEDMA(FLEXIO_MCULCD_Type *base,
                                            flexio_mculcd_edma_handle_t *handle,
                                            size_t *count);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _FSL_FLEXIO_MCULCD_EDMA_H_ */
