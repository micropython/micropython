/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_FLEXIO_SPI_EDMA_H_
#define _FSL_FLEXIO_SPI_EDMA_H_

#include "fsl_flexio_spi.h"
#include "fsl_edma.h"

/*!
 * @addtogroup flexio_edma_spi
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO SPI EDMA driver version 2.1.3. */
#define FSL_FLEXIO_SPI_EDMA_DRIVER_VERSION (MAKE_VERSION(2, 1, 3))
/*@}*/

/*! @brief  typedef for flexio_spi_master_edma_handle_t in advance. */
typedef struct _flexio_spi_master_edma_handle flexio_spi_master_edma_handle_t;

/*! @brief  Slave handle is the same with master handle. */
typedef flexio_spi_master_edma_handle_t flexio_spi_slave_edma_handle_t;

/*! @brief FlexIO SPI master callback for finished transmit */
typedef void (*flexio_spi_master_edma_transfer_callback_t)(FLEXIO_SPI_Type *base,
                                                           flexio_spi_master_edma_handle_t *handle,
                                                           status_t status,
                                                           void *userData);

/*! @brief FlexIO SPI slave callback for finished transmit */
typedef void (*flexio_spi_slave_edma_transfer_callback_t)(FLEXIO_SPI_Type *base,
                                                          flexio_spi_slave_edma_handle_t *handle,
                                                          status_t status,
                                                          void *userData);

/*! @brief FlexIO SPI eDMA transfer handle, users should not touch the content of the handle.*/
struct _flexio_spi_master_edma_handle
{
    size_t transferSize;                                 /*!< Total bytes to be transferred. */
    uint8_t nbytes;                                      /*!< eDMA minor byte transfer count initially configured. */
    bool txInProgress;                                   /*!< Send transfer in progress */
    bool rxInProgress;                                   /*!< Receive transfer in progress */
    edma_handle_t *txHandle;                             /*!< DMA handler for SPI send */
    edma_handle_t *rxHandle;                             /*!< DMA handler for SPI receive */
    flexio_spi_master_edma_transfer_callback_t callback; /*!< Callback for SPI DMA transfer */
    void *userData;                                      /*!< User Data for SPI DMA callback */
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
 * @brief Initializes the FlexIO SPI master eDMA handle.
 *
 * This function initializes the FlexIO SPI master eDMA handle which can be used for other FlexIO SPI master
 * transactional
 * APIs.
 * For a specified FlexIO SPI instance, call this API once to get the initialized handle.
 *
 * @param base Pointer to FLEXIO_SPI_Type structure.
 * @param handle Pointer to flexio_spi_master_edma_handle_t structure to store the transfer state.
 * @param callback SPI callback, NULL means no callback.
 * @param userData callback function parameter.
 * @param txHandle User requested eDMA handle for FlexIO SPI RX eDMA transfer.
 * @param rxHandle User requested eDMA handle for FlexIO SPI TX eDMA transfer.
 * @retval kStatus_Success Successfully create the handle.
 * @retval kStatus_OutOfRange The FlexIO SPI eDMA type/handle table out of range.
 */
status_t FLEXIO_SPI_MasterTransferCreateHandleEDMA(FLEXIO_SPI_Type *base,
                                                   flexio_spi_master_edma_handle_t *handle,
                                                   flexio_spi_master_edma_transfer_callback_t callback,
                                                   void *userData,
                                                   edma_handle_t *txHandle,
                                                   edma_handle_t *rxHandle);

/*!
 * @brief Performs a non-blocking FlexIO SPI transfer using eDMA.
 *
 * @note This interface returns immediately after transfer initiates. Call
 * FLEXIO_SPI_MasterGetTransferCountEDMA to poll the transfer status and check
 * whether the FlexIO SPI transfer is finished.
 *
 * @param base Pointer to FLEXIO_SPI_Type structure.
 * @param handle Pointer to flexio_spi_master_edma_handle_t structure to store the transfer state.
 * @param xfer Pointer to FlexIO SPI transfer structure.
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_FLEXIO_SPI_Busy FlexIO SPI is not idle, is running another transfer.
 */
status_t FLEXIO_SPI_MasterTransferEDMA(FLEXIO_SPI_Type *base,
                                       flexio_spi_master_edma_handle_t *handle,
                                       flexio_spi_transfer_t *xfer);

/*!
 * @brief Aborts a FlexIO SPI transfer using eDMA.
 *
 * @param base Pointer to FLEXIO_SPI_Type structure.
 * @param handle FlexIO SPI eDMA handle pointer.
 */
void FLEXIO_SPI_MasterTransferAbortEDMA(FLEXIO_SPI_Type *base, flexio_spi_master_edma_handle_t *handle);

/*!
 * @brief Gets the remaining bytes for FlexIO SPI eDMA transfer.
 *
 * @param base Pointer to FLEXIO_SPI_Type structure.
 * @param handle FlexIO SPI eDMA handle pointer.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 */
status_t FLEXIO_SPI_MasterTransferGetCountEDMA(FLEXIO_SPI_Type *base,
                                               flexio_spi_master_edma_handle_t *handle,
                                               size_t *count);

/*!
 * @brief Initializes the FlexIO SPI slave eDMA handle.
 *
 * This function initializes the FlexIO SPI slave eDMA handle.
 *
 * @param base Pointer to FLEXIO_SPI_Type structure.
 * @param handle Pointer to flexio_spi_slave_edma_handle_t structure to store the transfer state.
 * @param callback SPI callback, NULL means no callback.
 * @param userData callback function parameter.
 * @param txHandle User requested eDMA handle for FlexIO SPI TX eDMA transfer.
 * @param rxHandle User requested eDMA handle for FlexIO SPI RX eDMA transfer.
 */
static inline void FLEXIO_SPI_SlaveTransferCreateHandleEDMA(FLEXIO_SPI_Type *base,
                                                            flexio_spi_slave_edma_handle_t *handle,
                                                            flexio_spi_slave_edma_transfer_callback_t callback,
                                                            void *userData,
                                                            edma_handle_t *txHandle,
                                                            edma_handle_t *rxHandle)
{
    FLEXIO_SPI_MasterTransferCreateHandleEDMA(base, handle, callback, userData, txHandle, rxHandle);
}

/*!
 * @brief Performs a non-blocking FlexIO SPI transfer using eDMA.
 *
 * @note This interface returns immediately after transfer initiates. Call
 * FLEXIO_SPI_SlaveGetTransferCountEDMA to poll the transfer status and
 * check whether the FlexIO SPI transfer is finished.
 *
 * @param base Pointer to FLEXIO_SPI_Type structure.
 * @param handle Pointer to flexio_spi_slave_edma_handle_t structure to store the transfer state.
 * @param xfer Pointer to FlexIO SPI transfer structure.
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_FLEXIO_SPI_Busy FlexIO SPI is not idle, is running another transfer.
 */
status_t FLEXIO_SPI_SlaveTransferEDMA(FLEXIO_SPI_Type *base,
                                      flexio_spi_slave_edma_handle_t *handle,
                                      flexio_spi_transfer_t *xfer);

/*!
 * @brief Aborts a FlexIO SPI transfer using eDMA.
 *
 * @param base Pointer to FLEXIO_SPI_Type structure.
 * @param handle Pointer to flexio_spi_slave_edma_handle_t structure to store the transfer state.
 */
static inline void FLEXIO_SPI_SlaveTransferAbortEDMA(FLEXIO_SPI_Type *base, flexio_spi_slave_edma_handle_t *handle)
{
    FLEXIO_SPI_MasterTransferAbortEDMA(base, handle);
}

/*!
 * @brief Gets the remaining bytes to be transferred for FlexIO SPI eDMA.
 *
 * @param base Pointer to FLEXIO_SPI_Type structure.
 * @param handle FlexIO SPI eDMA handle pointer.
 * @param count Number of bytes transferred so far by the non-blocking transaction.
 */
static inline status_t FLEXIO_SPI_SlaveTransferGetCountEDMA(FLEXIO_SPI_Type *base,
                                                            flexio_spi_slave_edma_handle_t *handle,
                                                            size_t *count)
{
    return FLEXIO_SPI_MasterTransferGetCountEDMA(base, handle, count);
}

/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif
