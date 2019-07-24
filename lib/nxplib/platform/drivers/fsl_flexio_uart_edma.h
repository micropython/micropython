/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_FLEXIO_UART_EDMA_H_
#define _FSL_FLEXIO_UART_EDMA_H_

#include "fsl_flexio_uart.h"
#include "fsl_edma.h"

/*!
 * @addtogroup flexio_edma_uart
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO UART EDMA driver version 2.1.4. */
#define FSL_FLEXIO_UART_EDMA_DRIVER_VERSION (MAKE_VERSION(2, 1, 4))
/*@}*/

/* Forward declaration of the handle typedef. */
typedef struct _flexio_uart_edma_handle flexio_uart_edma_handle_t;

/*! @brief UART transfer callback function. */
typedef void (*flexio_uart_edma_transfer_callback_t)(FLEXIO_UART_Type *base,
                                                     flexio_uart_edma_handle_t *handle,
                                                     status_t status,
                                                     void *userData);

/*!
 * @brief UART eDMA handle
 */
struct _flexio_uart_edma_handle
{
    flexio_uart_edma_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                                /*!< UART callback function parameter.*/

    size_t txDataSizeAll; /*!< Total bytes to be sent. */
    size_t rxDataSizeAll; /*!< Total bytes to be received. */

    edma_handle_t *txEdmaHandle; /*!< The eDMA TX channel used. */
    edma_handle_t *rxEdmaHandle; /*!< The eDMA RX channel used. */

    uint8_t nbytes; /*!< eDMA minor byte transfer count initially configured. */

    volatile uint8_t txState; /*!< TX transfer state. */
    volatile uint8_t rxState; /*!< RX transfer state */
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
 * @brief Initializes the UART handle which is used in transactional functions.
 *
 * @param base Pointer to FLEXIO_UART_Type.
 * @param handle Pointer to flexio_uart_edma_handle_t structure.
 * @param callback The callback function.
 * @param userData The parameter of the callback function.
 * @param rxEdmaHandle User requested DMA handle for RX DMA transfer.
 * @param txEdmaHandle User requested DMA handle for TX DMA transfer.
 * @retval kStatus_Success Successfully create the handle.
 * @retval kStatus_OutOfRange The FlexIO SPI eDMA type/handle table out of range.
 */
status_t FLEXIO_UART_TransferCreateHandleEDMA(FLEXIO_UART_Type *base,
                                              flexio_uart_edma_handle_t *handle,
                                              flexio_uart_edma_transfer_callback_t callback,
                                              void *userData,
                                              edma_handle_t *txEdmaHandle,
                                              edma_handle_t *rxEdmaHandle);

/*!
 * @brief Sends data using eDMA.
 *
 * This function sends data using eDMA. This is a non-blocking function, which returns
 * right away. When all data is sent out, the send callback function is called.
 *
 * @param base Pointer to FLEXIO_UART_Type
 * @param handle UART handle pointer.
 * @param xfer UART eDMA transfer structure, see #flexio_uart_transfer_t.
 * @retval kStatus_Success if succeed, others failed.
 * @retval kStatus_FLEXIO_UART_TxBusy Previous transfer on going.
 */
status_t FLEXIO_UART_TransferSendEDMA(FLEXIO_UART_Type *base,
                                      flexio_uart_edma_handle_t *handle,
                                      flexio_uart_transfer_t *xfer);

/*!
 * @brief Receives data using eDMA.
 *
 * This function receives data using eDMA. This is a non-blocking function, which returns
 * right away. When all data is received, the receive callback function is called.
 *
 * @param base Pointer to FLEXIO_UART_Type
 * @param handle Pointer to flexio_uart_edma_handle_t structure
 * @param xfer UART eDMA transfer structure, see #flexio_uart_transfer_t.
 * @retval kStatus_Success if succeed, others failed.
 * @retval kStatus_UART_RxBusy Previous transfer on going.
 */
status_t FLEXIO_UART_TransferReceiveEDMA(FLEXIO_UART_Type *base,
                                         flexio_uart_edma_handle_t *handle,
                                         flexio_uart_transfer_t *xfer);

/*!
 * @brief Aborts the sent data which using eDMA.
 *
 * This function aborts sent data which using eDMA.
 *
 * @param base Pointer to FLEXIO_UART_Type
 * @param handle Pointer to flexio_uart_edma_handle_t structure
 */
void FLEXIO_UART_TransferAbortSendEDMA(FLEXIO_UART_Type *base, flexio_uart_edma_handle_t *handle);

/*!
 * @brief Aborts the receive data which using eDMA.
 *
 * This function aborts the receive data which using eDMA.
 *
 * @param base Pointer to FLEXIO_UART_Type
 * @param handle Pointer to flexio_uart_edma_handle_t structure
 */
void FLEXIO_UART_TransferAbortReceiveEDMA(FLEXIO_UART_Type *base, flexio_uart_edma_handle_t *handle);

/*!
 * @brief Gets the number of bytes sent out.
 *
 * This function gets the number of bytes sent out.
 *
 * @param base Pointer to FLEXIO_UART_Type
 * @param handle Pointer to flexio_uart_edma_handle_t structure
 * @param count Number of bytes sent so far by the non-blocking transaction.
 * @retval kStatus_NoTransferInProgress transfer has finished or no transfer in progress.
 * @retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_UART_TransferGetSendCountEDMA(FLEXIO_UART_Type *base, flexio_uart_edma_handle_t *handle, size_t *count);

/*!
 * @brief Gets the number of bytes received.
 *
 * This function gets the number of bytes received.
 *
 * @param base Pointer to FLEXIO_UART_Type
 * @param handle Pointer to flexio_uart_edma_handle_t structure
 * @param count Number of bytes received so far by the non-blocking transaction.
 * @retval kStatus_NoTransferInProgress transfer has finished or no transfer in progress.
 * @retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_UART_TransferGetReceiveCountEDMA(FLEXIO_UART_Type *base,
                                                 flexio_uart_edma_handle_t *handle,
                                                 size_t *count);

/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_UART_EDMA_H_ */
