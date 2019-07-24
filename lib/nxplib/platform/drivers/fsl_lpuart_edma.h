/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_LPUART_EDMA_H_
#define _FSL_LPUART_EDMA_H_

#include "fsl_lpuart.h"
#include "fsl_edma.h"

/*!
 * @addtogroup lpuart_edma_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief LPUART EDMA driver version 2.2.7. */
#define FSL_LPUART_EDMA_DRIVER_VERSION (MAKE_VERSION(2, 2, 7))
/*@}*/

/* Forward declaration of the handle typedef. */
typedef struct _lpuart_edma_handle lpuart_edma_handle_t;

/*! @brief LPUART transfer callback function. */
typedef void (*lpuart_edma_transfer_callback_t)(LPUART_Type *base,
                                                lpuart_edma_handle_t *handle,
                                                status_t status,
                                                void *userData);

/*!
 * @brief LPUART eDMA handle
 */
struct _lpuart_edma_handle
{
    lpuart_edma_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                           /*!< LPUART callback function parameter.*/
    size_t rxDataSizeAll;                     /*!< Size of the data to receive. */
    size_t txDataSizeAll;                     /*!< Size of the data to send out. */

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
 * @brief Initializes the LPUART handle which is used in transactional functions.
 * @param base LPUART peripheral base address.
 * @param handle Pointer to lpuart_edma_handle_t structure.
 * @param callback Callback function.
 * @param userData User data.
 * @param txEdmaHandle User requested DMA handle for TX DMA transfer.
 * @param rxEdmaHandle User requested DMA handle for RX DMA transfer.
 */
void LPUART_TransferCreateHandleEDMA(LPUART_Type *base,
                                     lpuart_edma_handle_t *handle,
                                     lpuart_edma_transfer_callback_t callback,
                                     void *userData,
                                     edma_handle_t *txEdmaHandle,
                                     edma_handle_t *rxEdmaHandle);

/*!
 * @brief Sends data using eDMA.
 *
 * This function sends data using eDMA. This is a non-blocking function, which returns
 * right away. When all data is sent, the send callback function is called.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 * @param xfer LPUART eDMA transfer structure. See #lpuart_transfer_t.
 * @retval kStatus_Success if succeed, others failed.
 * @retval kStatus_LPUART_TxBusy Previous transfer on going.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t LPUART_SendEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle, lpuart_transfer_t *xfer);

/*!
 * @brief Receives data using eDMA.
 *
 * This function receives data using eDMA. This is non-blocking function, which returns
 * right away. When all data is received, the receive callback function is called.
 *
 * @param base LPUART peripheral base address.
 * @param handle Pointer to lpuart_edma_handle_t structure.
 * @param xfer LPUART eDMA transfer structure, see #lpuart_transfer_t.
 * @retval kStatus_Success if succeed, others fail.
 * @retval kStatus_LPUART_RxBusy Previous transfer ongoing.
 * @retval kStatus_InvalidArgument Invalid argument.
 */
status_t LPUART_ReceiveEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle, lpuart_transfer_t *xfer);

/*!
 * @brief Aborts the sent data using eDMA.
 *
 * This function aborts the sent data using eDMA.
 *
 * @param base LPUART peripheral base address.
 * @param handle Pointer to lpuart_edma_handle_t structure.
 */
void LPUART_TransferAbortSendEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle);

/*!
 * @brief Aborts the received data using eDMA.
 *
 * This function aborts the received data using eDMA.
 *
 * @param base LPUART peripheral base address.
 * @param handle Pointer to lpuart_edma_handle_t structure.
 */
void LPUART_TransferAbortReceiveEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle);

/*!
 * @brief Gets the number of bytes written to the LPUART TX register.
 *
 * This function gets the number of bytes written to the LPUART TX
 * register by DMA.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 * @param count Send bytes count.
 * @retval kStatus_NoTransferInProgress No send in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t LPUART_TransferGetSendCountEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle, uint32_t *count);

/*!
 * @brief Gets the number of received bytes.
 *
 * This function gets the number of received bytes.
 *
 * @param base LPUART peripheral base address.
 * @param handle LPUART handle pointer.
 * @param count Receive bytes count.
 * @retval kStatus_NoTransferInProgress No receive in progress.
 * @retval kStatus_InvalidArgument Parameter is invalid.
 * @retval kStatus_Success Get successfully through the parameter \p count;
 */
status_t LPUART_TransferGetReceiveCountEDMA(LPUART_Type *base, lpuart_edma_handle_t *handle, uint32_t *count);

/*@}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_LPUART_EDMA_H_ */
