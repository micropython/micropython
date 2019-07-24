/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_LPSPI_EDMA_H_
#define _FSL_LPSPI_EDMA_H_

#include "fsl_lpspi.h"
#include "fsl_edma.h"

/*!
 * @addtogroup lpspi_edma_driver
 * @{
 */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/*! @name Driver version */
/*@{*/
/*! @brief LPSPI EDMA driver version 2.0.2. */
#define FSL_LPSPI_EDMA_DRIVER_VERSION (MAKE_VERSION(2, 0, 2))
/*@}*/

/*!
 * @brief Forward declaration of the _lpspi_master_edma_handle typedefs.
 */
typedef struct _lpspi_master_edma_handle lpspi_master_edma_handle_t;

/*!
 * @brief Forward declaration of the _lpspi_slave_edma_handle typedefs.
 */
typedef struct _lpspi_slave_edma_handle lpspi_slave_edma_handle_t;

/*!
 * @brief Completion callback function pointer type.
 *
 * @param base LPSPI peripheral base address.
 * @param handle Pointer to the handle for the LPSPI master.
 * @param status Success or error code describing whether the transfer completed.
 * @param userData Arbitrary pointer-dataSized value passed from the application.
 */
typedef void (*lpspi_master_edma_transfer_callback_t)(LPSPI_Type *base,
                                                      lpspi_master_edma_handle_t *handle,
                                                      status_t status,
                                                      void *userData);
/*!
 * @brief Completion callback function pointer type.
 *
 * @param base LPSPI peripheral base address.
 * @param handle Pointer to the handle for the LPSPI slave.
 * @param status Success or error code describing whether the transfer completed.
 * @param userData Arbitrary pointer-dataSized value passed from the application.
 */
typedef void (*lpspi_slave_edma_transfer_callback_t)(LPSPI_Type *base,
                                                     lpspi_slave_edma_handle_t *handle,
                                                     status_t status,
                                                     void *userData);

/*! @brief LPSPI master eDMA transfer handle structure used for transactional API. */
struct _lpspi_master_edma_handle
{
    volatile bool isPcsContinuous; /*!< Is PCS continuous in transfer. */

    volatile bool isByteSwap; /*!< A flag that whether should byte swap. */

    volatile uint8_t fifoSize; /*!< FIFO dataSize. */

    volatile uint8_t rxWatermark; /*!< Rx watermark. */

    volatile uint8_t bytesEachWrite; /*!< Bytes for each write TDR. */
    volatile uint8_t bytesEachRead;  /*!< Bytes for each read RDR. */

    volatile uint8_t bytesLastRead;       /*!< Bytes for last read RDR. */
    volatile uint8_t isThereExtraRxBytes; /*!< Is there extra RX byte. */

    uint8_t *volatile txData;             /*!< Send buffer. */
    uint8_t *volatile rxData;             /*!< Receive buffer. */
    volatile size_t txRemainingByteCount; /*!< Number of bytes remaining to send.*/
    volatile size_t rxRemainingByteCount; /*!< Number of bytes remaining to receive.*/

    volatile uint32_t writeRegRemainingTimes; /*!< Write TDR register remaining times. */
    volatile uint32_t readRegRemainingTimes;  /*!< Read RDR register remaining times. */

    uint32_t totalByteCount; /*!< Number of transfer bytes*/

    uint32_t txBuffIfNull; /*!< Used if there is not txData for DMA purpose.*/
    uint32_t rxBuffIfNull; /*!< Used if there is not rxData for DMA purpose.*/

    uint32_t transmitCommand; /*!< Used to write TCR for DMA purpose.*/

    volatile uint8_t state; /*!< LPSPI transfer state , _lpspi_transfer_state.*/

    uint8_t nbytes; /*!< eDMA minor byte transfer count initially configured. */

    lpspi_master_edma_transfer_callback_t callback; /*!< Completion callback. */
    void *userData;                                 /*!< Callback user data. */

    edma_handle_t *edmaRxRegToRxDataHandle; /*!<edma_handle_t handle point used for RxReg to RxData buff*/
    edma_handle_t *edmaTxDataToTxRegHandle; /*!<edma_handle_t handle point used for TxData to TxReg buff*/

    edma_tcd_t lpspiSoftwareTCD[3]; /*!<SoftwareTCD, internal used*/
};

/*! @brief LPSPI slave eDMA transfer handle structure used for transactional API.*/
struct _lpspi_slave_edma_handle
{
    volatile bool isByteSwap; /*!< A flag that whether should byte swap. */

    volatile uint8_t fifoSize; /*!< FIFO dataSize. */

    volatile uint8_t rxWatermark; /*!< Rx watermark. */

    volatile uint8_t bytesEachWrite; /*!< Bytes for each write TDR. */
    volatile uint8_t bytesEachRead;  /*!< Bytes for each read RDR. */

    volatile uint8_t bytesLastRead;       /*!< Bytes for last read RDR. */
    volatile uint8_t isThereExtraRxBytes; /*!< Is there extra RX byte. */

    uint8_t nbytes; /*!< eDMA minor byte transfer count initially configured. */

    uint8_t *volatile txData;             /*!< Send buffer. */
    uint8_t *volatile rxData;             /*!< Receive buffer. */
    volatile size_t txRemainingByteCount; /*!< Number of bytes remaining to send.*/
    volatile size_t rxRemainingByteCount; /*!< Number of bytes remaining to receive.*/

    volatile uint32_t writeRegRemainingTimes; /*!< Write TDR register remaining times. */
    volatile uint32_t readRegRemainingTimes;  /*!< Read RDR register remaining times. */

    uint32_t totalByteCount; /*!< Number of transfer bytes*/

    uint32_t txBuffIfNull; /*!< Used if there is not txData for DMA purpose.*/
    uint32_t rxBuffIfNull; /*!< Used if there is not rxData for DMA purpose.*/

    volatile uint8_t state; /*!< LPSPI transfer state.*/

    uint32_t errorCount; /*!< Error count for slave transfer.*/

    lpspi_slave_edma_transfer_callback_t callback; /*!< Completion callback. */
    void *userData;                                /*!< Callback user data. */

    edma_handle_t *edmaRxRegToRxDataHandle; /*!<edma_handle_t handle point used for RxReg to RxData buff*/
    edma_handle_t *edmaTxDataToTxRegHandle; /*!<edma_handle_t handle point used for TxData to TxReg*/

    edma_tcd_t lpspiSoftwareTCD[2]; /*!<SoftwareTCD, internal used*/
};

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*Transactional APIs*/

/*!
 * @brief Initializes the LPSPI master eDMA handle.
 *
 * This function initializes the LPSPI eDMA handle which can be used for other LPSPI transactional APIs.  Usually, for a
 * specified LPSPI instance, call this API once to get the initialized handle.
 *
 * Note that the LPSPI eDMA has a separated (Rx and Rx as two sources) or shared (Rx  and Tx are the same source) DMA
 * request source.
 * (1) For a separated DMA request source, enable and set the Rx DMAMUX source for edmaRxRegToRxDataHandle and
 * Tx DMAMUX source for edmaIntermediaryToTxRegHandle.
 * (2) For a shared DMA request source, enable and set the Rx/Rx DMAMUX source for edmaRxRegToRxDataHandle.
 *
 * @param base LPSPI peripheral base address.
 * @param handle LPSPI handle pointer to lpspi_master_edma_handle_t.
 * @param callback LPSPI callback.
 * @param userData callback function parameter.
 * @param edmaRxRegToRxDataHandle edmaRxRegToRxDataHandle pointer to edma_handle_t.
 * @param edmaTxDataToTxRegHandle edmaTxDataToTxRegHandle pointer to edma_handle_t.
 */
void LPSPI_MasterTransferCreateHandleEDMA(LPSPI_Type *base,
                                          lpspi_master_edma_handle_t *handle,
                                          lpspi_master_edma_transfer_callback_t callback,
                                          void *userData,
                                          edma_handle_t *edmaRxRegToRxDataHandle,
                                          edma_handle_t *edmaTxDataToTxRegHandle);

/*!
 * @brief LPSPI master transfer data using eDMA.
 *
 * This function transfers data using eDMA. This is a non-blocking function, which returns right away. When all data
 * is transferred, the callback function is called.
 *
 * Note:
 * The transfer data size should be an integer multiple of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not an integer multiple of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * @param base LPSPI peripheral base address.
 * @param handle pointer to lpspi_master_edma_handle_t structure which stores the transfer state.
 * @param transfer pointer to lpspi_transfer_t structure.
 * @return status of status_t.
 */
status_t LPSPI_MasterTransferEDMA(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, lpspi_transfer_t *transfer);

/*!
 * @brief LPSPI master aborts a transfer which is using eDMA.
 *
 * This function aborts a transfer which is using eDMA.
 *
 * @param base LPSPI peripheral base address.
 * @param handle pointer to lpspi_master_edma_handle_t structure which stores the transfer state.
 */
void LPSPI_MasterTransferAbortEDMA(LPSPI_Type *base, lpspi_master_edma_handle_t *handle);

/*!
 * @brief Gets the master eDMA transfer remaining bytes.
 *
 * This function gets the master eDMA transfer remaining bytes.
 *
 * @param base LPSPI peripheral base address.
 * @param handle pointer to lpspi_master_edma_handle_t structure which stores the transfer state.
 * @param count Number of bytes transferred so far by the EDMA transaction.
 * @return status of status_t.
 */
status_t LPSPI_MasterTransferGetCountEDMA(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, size_t *count);

/*!
 * @brief Initializes the LPSPI slave eDMA handle.
 *
 * This function initializes the LPSPI eDMA handle which can be used for other LPSPI transactional APIs.  Usually, for a
 * specified LPSPI instance, call this API once to get the initialized handle.
 *
 * Note that LPSPI eDMA has a separated (Rx and Tx as two sources) or shared (Rx  and Tx as the same source) DMA request
 * source.
 *
 * (1) For a separated DMA request source, enable and set the Rx DMAMUX source for edmaRxRegToRxDataHandle and
 * Tx DMAMUX source for edmaTxDataToTxRegHandle.
 * (2) For a shared DMA request source, enable and set the Rx/Rx DMAMUX source for edmaRxRegToRxDataHandle .
 *
 * @param base LPSPI peripheral base address.
 * @param handle LPSPI handle pointer to lpspi_slave_edma_handle_t.
 * @param callback LPSPI callback.
 * @param userData callback function parameter.
 * @param edmaRxRegToRxDataHandle edmaRxRegToRxDataHandle pointer to edma_handle_t.
 * @param edmaTxDataToTxRegHandle edmaTxDataToTxRegHandle pointer to edma_handle_t.
 */
void LPSPI_SlaveTransferCreateHandleEDMA(LPSPI_Type *base,
                                         lpspi_slave_edma_handle_t *handle,
                                         lpspi_slave_edma_transfer_callback_t callback,
                                         void *userData,
                                         edma_handle_t *edmaRxRegToRxDataHandle,
                                         edma_handle_t *edmaTxDataToTxRegHandle);

/*!
 * @brief LPSPI slave transfers data using eDMA.
 *
 * This function transfers data using eDMA. This is a non-blocking function, which return right away. When all data
 * is transferred, the callback function is called.
 *
 * Note:
 * The transfer data size should be an integer multiple of bytesPerFrame if bytesPerFrame is less than or equal to 4.
 * For bytesPerFrame greater than 4:
 * The transfer data size should be equal to bytesPerFrame if the bytesPerFrame is not an integer multiple of 4.
 * Otherwise, the transfer data size can be an integer multiple of bytesPerFrame.
 *
 * @param base LPSPI peripheral base address.
 * @param handle pointer to lpspi_slave_edma_handle_t structure which stores the transfer state.
 * @param transfer pointer to lpspi_transfer_t structure.
 * @return status of status_t.
 */
status_t LPSPI_SlaveTransferEDMA(LPSPI_Type *base, lpspi_slave_edma_handle_t *handle, lpspi_transfer_t *transfer);

/*!
 * @brief LPSPI slave aborts a transfer which is using eDMA.
 *
 * This function aborts a transfer which is using eDMA.
 *
 * @param base LPSPI peripheral base address.
 * @param handle pointer to lpspi_slave_edma_handle_t structure which stores the transfer state.
 */
void LPSPI_SlaveTransferAbortEDMA(LPSPI_Type *base, lpspi_slave_edma_handle_t *handle);

/*!
 * @brief Gets the slave eDMA transfer remaining bytes.
 *
 * This function gets the slave eDMA transfer remaining bytes.
 *
 * @param base LPSPI peripheral base address.
 * @param handle pointer to lpspi_slave_edma_handle_t structure which stores the transfer state.
 * @param count Number of bytes transferred so far by the eDMA transaction.
 * @return status of status_t.
 */
status_t LPSPI_SlaveTransferGetCountEDMA(LPSPI_Type *base, lpspi_slave_edma_handle_t *handle, size_t *count);

#if defined(__cplusplus)
}
#endif /*_cplusplus*/
       /*!
        *@}
        */

#endif /*_FSL_LPSPI_EDMA_H_*/
