/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FLEXIO_UART_H_
#define _FSL_FLEXIO_UART_H_

#include "fsl_common.h"
#include "fsl_flexio.h"

/*!
 * @addtogroup flexio_uart
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief FlexIO UART driver version 2.1.5. */
#define FSL_FLEXIO_UART_DRIVER_VERSION (MAKE_VERSION(2, 1, 5))
/*@}*/

/*! @brief Error codes for the UART driver. */
enum _flexio_uart_status
{
    kStatus_FLEXIO_UART_TxBusy = MAKE_STATUS(kStatusGroup_FLEXIO_UART, 0), /*!< Transmitter is busy. */
    kStatus_FLEXIO_UART_RxBusy = MAKE_STATUS(kStatusGroup_FLEXIO_UART, 1), /*!< Receiver is busy. */
    kStatus_FLEXIO_UART_TxIdle = MAKE_STATUS(kStatusGroup_FLEXIO_UART, 2), /*!< UART transmitter is idle. */
    kStatus_FLEXIO_UART_RxIdle = MAKE_STATUS(kStatusGroup_FLEXIO_UART, 3), /*!< UART receiver is idle. */
    kStatus_FLEXIO_UART_ERROR  = MAKE_STATUS(kStatusGroup_FLEXIO_UART, 4), /*!< ERROR happens on UART. */
    kStatus_FLEXIO_UART_RxRingBufferOverrun =
        MAKE_STATUS(kStatusGroup_FLEXIO_UART, 5), /*!< UART RX software ring buffer overrun. */
    kStatus_FLEXIO_UART_RxHardwareOverrun = MAKE_STATUS(kStatusGroup_FLEXIO_UART, 6) /*!< UART RX receiver overrun. */
};

/*! @brief FlexIO UART bit count per char. */
typedef enum _flexio_uart_bit_count_per_char
{
    kFLEXIO_UART_7BitsPerChar = 7U, /*!< 7-bit data characters */
    kFLEXIO_UART_8BitsPerChar = 8U, /*!< 8-bit data characters */
    kFLEXIO_UART_9BitsPerChar = 9U, /*!< 9-bit data characters */
} flexio_uart_bit_count_per_char_t;

/*! @brief Define FlexIO UART interrupt mask. */
enum _flexio_uart_interrupt_enable
{
    kFLEXIO_UART_TxDataRegEmptyInterruptEnable = 0x1U, /*!< Transmit buffer empty interrupt enable. */
    kFLEXIO_UART_RxDataRegFullInterruptEnable  = 0x2U, /*!< Receive buffer full interrupt enable. */
};

/*! @brief Define FlexIO UART status mask. */
enum _flexio_uart_status_flags
{
    kFLEXIO_UART_TxDataRegEmptyFlag = 0x1U, /*!< Transmit buffer empty flag. */
    kFLEXIO_UART_RxDataRegFullFlag  = 0x2U, /*!< Receive buffer full flag. */
    kFLEXIO_UART_RxOverRunFlag      = 0x4U, /*!< Receive buffer over run flag. */
};

/*! @brief Define FlexIO UART access structure typedef. */
typedef struct _flexio_uart_type
{
    FLEXIO_Type *flexioBase; /*!< FlexIO base pointer. */
    uint8_t TxPinIndex;      /*!< Pin select for UART_Tx. */
    uint8_t RxPinIndex;      /*!< Pin select for UART_Rx. */
    uint8_t shifterIndex[2]; /*!< Shifter index used in FlexIO UART. */
    uint8_t timerIndex[2];   /*!< Timer index used in FlexIO UART. */
} FLEXIO_UART_Type;

/*! @brief Define FlexIO UART user configuration structure. */
typedef struct _flexio_uart_config
{
    bool enableUart;                                  /*!< Enable/disable FlexIO UART TX & RX. */
    bool enableInDoze;                                /*!< Enable/disable FlexIO operation in doze mode*/
    bool enableInDebug;                               /*!< Enable/disable FlexIO operation in debug mode*/
    bool enableFastAccess;                            /*!< Enable/disable fast access to FlexIO registers,
                                                       fast access requires the FlexIO clock to be at least
                                                       twice the frequency of the bus clock. */
    uint32_t baudRate_Bps;                            /*!< Baud rate in Bps. */
    flexio_uart_bit_count_per_char_t bitCountPerChar; /*!< number of bits, 7/8/9 -bit */
} flexio_uart_config_t;

/*! @brief Define FlexIO UART transfer structure. */
typedef struct _flexio_uart_transfer
{
    uint8_t *data;   /*!< Transfer buffer*/
    size_t dataSize; /*!< Transfer size*/
} flexio_uart_transfer_t;

/* Forward declaration of the handle typedef. */
typedef struct _flexio_uart_handle flexio_uart_handle_t;

/*! @brief FlexIO UART transfer callback function. */
typedef void (*flexio_uart_transfer_callback_t)(FLEXIO_UART_Type *base,
                                                flexio_uart_handle_t *handle,
                                                status_t status,
                                                void *userData);

/*! @brief Define FLEXIO UART handle structure*/
struct _flexio_uart_handle
{
    uint8_t *volatile txData;   /*!< Address of remaining data to send. */
    volatile size_t txDataSize; /*!< Size of the remaining data to send. */
    uint8_t *volatile rxData;   /*!< Address of remaining data to receive. */
    volatile size_t rxDataSize; /*!< Size of the remaining data to receive. */
    size_t txDataSizeAll;       /*!< Total bytes to be sent. */
    size_t rxDataSizeAll;       /*!< Total bytes to be received. */

    uint8_t *rxRingBuffer;              /*!< Start address of the receiver ring buffer. */
    size_t rxRingBufferSize;            /*!< Size of the ring buffer. */
    volatile uint16_t rxRingBufferHead; /*!< Index for the driver to store received data into ring buffer. */
    volatile uint16_t rxRingBufferTail; /*!< Index for the user to get data from the ring buffer. */

    flexio_uart_transfer_callback_t callback; /*!< Callback function. */
    void *userData;                           /*!< UART callback function parameter.*/

    volatile uint8_t txState; /*!< TX transfer state. */
    volatile uint8_t rxState; /*!< RX transfer state */
};

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /*_cplusplus*/

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Ungates the FlexIO clock, resets the FlexIO module, configures FlexIO UART
 * hardware, and configures the FlexIO UART with FlexIO UART configuration.
 * The configuration structure can be filled by the user or be set with
 * default values by FLEXIO_UART_GetDefaultConfig().
 *
 * Example
   @code
   FLEXIO_UART_Type base = {
   .flexioBase = FLEXIO,
   .TxPinIndex = 0,
   .RxPinIndex = 1,
   .shifterIndex = {0,1},
   .timerIndex = {0,1}
   };
   flexio_uart_config_t config = {
   .enableInDoze = false,
   .enableInDebug = true,
   .enableFastAccess = false,
   .baudRate_Bps = 115200U,
   .bitCountPerChar = 8
   };
   FLEXIO_UART_Init(base, &config, srcClock_Hz);
   @endcode
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param userConfig Pointer to the flexio_uart_config_t structure.
 * @param srcClock_Hz FlexIO source clock in Hz.
 * @retval kStatus_Success Configuration success
 * @retval kStatus_InvalidArgument Buadrate configuration out of range
*/
status_t FLEXIO_UART_Init(FLEXIO_UART_Type *base, const flexio_uart_config_t *userConfig, uint32_t srcClock_Hz);

/*!
 * @brief Resets the FlexIO UART shifter and timer config.
 *
 * @note After calling this API, call the FLEXO_UART_Init to use the FlexIO UART module.
 *
 * @param base Pointer to FLEXIO_UART_Type structure
 */
void FLEXIO_UART_Deinit(FLEXIO_UART_Type *base);

/*!
 * @brief Gets the default configuration to configure the FlexIO UART. The configuration
 * can be used directly for calling the FLEXIO_UART_Init().
 * Example:
   @code
   flexio_uart_config_t config;
   FLEXIO_UART_GetDefaultConfig(&userConfig);
   @endcode
 * @param userConfig Pointer to the flexio_uart_config_t structure.
*/
void FLEXIO_UART_GetDefaultConfig(flexio_uart_config_t *userConfig);

/* @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the FlexIO UART status flags.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @return FlexIO UART status flags.
 */

uint32_t FLEXIO_UART_GetStatusFlags(FLEXIO_UART_Type *base);

/*!
 * @brief Gets the FlexIO UART status flags.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param mask Status flag.
 *      The parameter can be any combination of the following values:
 *          @arg kFLEXIO_UART_TxDataRegEmptyFlag
 *          @arg kFLEXIO_UART_RxEmptyFlag
 *          @arg kFLEXIO_UART_RxOverRunFlag
 */

void FLEXIO_UART_ClearStatusFlags(FLEXIO_UART_Type *base, uint32_t mask);

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables the FlexIO UART interrupt.
 *
 * This function enables the FlexIO UART interrupt.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param mask Interrupt source.
 */
void FLEXIO_UART_EnableInterrupts(FLEXIO_UART_Type *base, uint32_t mask);

/*!
 * @brief Disables the FlexIO UART interrupt.
 *
 * This function disables the FlexIO UART interrupt.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param mask Interrupt source.
 */
void FLEXIO_UART_DisableInterrupts(FLEXIO_UART_Type *base, uint32_t mask);

/* @} */

/*!
 * @name DMA Control
 * @{
 */

/*!
 * @brief Gets the FlexIO UARt transmit data register address.
 *
 * This function returns the UART data register address, which is mainly used by DMA/eDMA.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @return FlexIO UART transmit data register address.
 */
static inline uint32_t FLEXIO_UART_GetTxDataRegisterAddress(FLEXIO_UART_Type *base)
{
    return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBuffer, base->shifterIndex[0]);
}

/*!
 * @brief Gets the FlexIO UART receive data register address.
 *
 * This function returns the UART data register address, which is mainly used by DMA/eDMA.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @return FlexIO UART receive data register address.
 */
static inline uint32_t FLEXIO_UART_GetRxDataRegisterAddress(FLEXIO_UART_Type *base)
{
    return FLEXIO_GetShifterBufferAddress(base->flexioBase, kFLEXIO_ShifterBufferByteSwapped, base->shifterIndex[1]);
}

/*!
 * @brief Enables/disables the FlexIO UART transmit DMA.
 * This function enables/disables the FlexIO UART Tx DMA,
 * which means asserting the kFLEXIO_UART_TxDataRegEmptyFlag does/doesn't trigger the DMA request.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param enable True to enable, false to disable.
 */
static inline void FLEXIO_UART_EnableTxDMA(FLEXIO_UART_Type *base, bool enable)
{
    FLEXIO_EnableShifterStatusDMA(base->flexioBase, 1 << base->shifterIndex[0], enable);
}

/*!
 * @brief Enables/disables the FlexIO UART receive DMA.
 * This function enables/disables the FlexIO UART Rx DMA,
 * which means asserting kFLEXIO_UART_RxDataRegFullFlag does/doesn't trigger the DMA request.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param enable True to enable, false to disable.
 */
static inline void FLEXIO_UART_EnableRxDMA(FLEXIO_UART_Type *base, bool enable)
{
    FLEXIO_EnableShifterStatusDMA(base->flexioBase, 1 << base->shifterIndex[1], enable);
}

/* @} */

/*!
 * @name Bus Operations
 * @{
 */

/*!
 * @brief Enables/disables the FlexIO UART module operation.
 *
 * @param base Pointer to the FLEXIO_UART_Type.
 * @param enable True to enable, false does not have any effect.
 */
static inline void FLEXIO_UART_Enable(FLEXIO_UART_Type *base, bool enable)
{
    if (enable)
    {
        base->flexioBase->CTRL |= FLEXIO_CTRL_FLEXEN_MASK;
    }
}

/*!
 * @brief Writes one byte of data.
 *
 * @note This is a non-blocking API, which returns directly after the data is put into the
 * data register. Ensure that the TxEmptyFlag is asserted before calling
 * this API.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param buffer The data bytes to send.
 */
static inline void FLEXIO_UART_WriteByte(FLEXIO_UART_Type *base, const uint8_t *buffer)
{
    base->flexioBase->SHIFTBUF[base->shifterIndex[0]] = *buffer;
}

/*!
 * @brief Reads one byte of data.
 *
 * @note This is a non-blocking API, which returns directly after the data is read from the
 * data register. Ensure that the RxFullFlag is asserted before calling this API.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param buffer The buffer to store the received bytes.
 */
static inline void FLEXIO_UART_ReadByte(FLEXIO_UART_Type *base, uint8_t *buffer)
{
    *buffer = base->flexioBase->SHIFTBUFBYS[base->shifterIndex[1]];
}

/*!
 * @brief Sends a buffer of data bytes.
 *
 * @note This function blocks using the polling method until all bytes have been sent.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param txData The data bytes to send.
 * @param txSize The number of data bytes to send.
 */
void FLEXIO_UART_WriteBlocking(FLEXIO_UART_Type *base, const uint8_t *txData, size_t txSize);

/*!
 * @brief Receives a buffer of bytes.
 *
 * @note This function blocks using the polling method until all bytes have been received.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param rxData The buffer to store the received bytes.
 * @param rxSize The number of data bytes to be received.
 */
void FLEXIO_UART_ReadBlocking(FLEXIO_UART_Type *base, uint8_t *rxData, size_t rxSize);

/* @} */

/*!
 * @name Transactional
 * @{
 */

/*!
 * @brief Initializes the UART handle.
 *
 * This function initializes the FlexIO UART handle, which can be used for other FlexIO
 * UART transactional APIs. Call this API once to get the
 * initialized handle.
 *
 * The UART driver supports the "background" receiving, which means that users can set up
 * a RX ring buffer optionally. Data received is stored into the ring buffer even when
 * the user doesn't call the FLEXIO_UART_TransferReceiveNonBlocking() API. If there is already data
 * received in the ring buffer, users can get the received data from the ring buffer
 * directly. The ring buffer is disabled if passing NULL as @p ringBuffer.
 *
 * @param base to FLEXIO_UART_Type structure.
 * @param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * @param callback The callback function.
 * @param userData The parameter of the callback function.
 * @retval kStatus_Success Successfully create the handle.
 * @retval kStatus_OutOfRange The FlexIO type/handle/ISR table out of range.
 */
status_t FLEXIO_UART_TransferCreateHandle(FLEXIO_UART_Type *base,
                                          flexio_uart_handle_t *handle,
                                          flexio_uart_transfer_callback_t callback,
                                          void *userData);

/*!
 * @brief Sets up the RX ring buffer.
 *
 * This function sets up the RX ring buffer to a specific UART handle.
 *
 * When the RX ring buffer is used, data received is stored into the ring buffer even when
 * the user doesn't call the UART_ReceiveNonBlocking() API. If there is already data received
 * in the ring buffer, users can get the received data from the ring buffer directly.
 *
 * @note When using the RX ring buffer, one byte is reserved for internal use. In other
 * words, if @p ringBufferSize is 32, only 31 bytes are used for saving data.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * @param ringBuffer Start address of ring buffer for background receiving. Pass NULL to disable the ring buffer.
 * @param ringBufferSize Size of the ring buffer.
 */
void FLEXIO_UART_TransferStartRingBuffer(FLEXIO_UART_Type *base,
                                         flexio_uart_handle_t *handle,
                                         uint8_t *ringBuffer,
                                         size_t ringBufferSize);

/*!
 * @brief Aborts the background transfer and uninstalls the ring buffer.
 *
 * This function aborts the background transfer and uninstalls the ring buffer.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 */
void FLEXIO_UART_TransferStopRingBuffer(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle);

/*!
 * @brief Transmits a buffer of data using the interrupt method.
 *
 * This function sends data using an interrupt method. This is a non-blocking function,
 * which returns directly without waiting for all data to be written to the TX register. When
 * all data is written to the TX register in ISR, the FlexIO UART driver calls the callback
 * function and passes the @ref kStatus_FLEXIO_UART_TxIdle as status parameter.
 *
 * @note The kStatus_FLEXIO_UART_TxIdle is passed to the upper layer when all data is written
 * to the TX register. However, it does not ensure that all data is sent out.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * @param xfer FlexIO UART transfer structure. See #flexio_uart_transfer_t.
 * @retval kStatus_Success Successfully starts the data transmission.
 * @retval kStatus_UART_TxBusy Previous transmission still not finished, data not written to the TX register.
 */
status_t FLEXIO_UART_TransferSendNonBlocking(FLEXIO_UART_Type *base,
                                             flexio_uart_handle_t *handle,
                                             flexio_uart_transfer_t *xfer);

/*!
 * @brief Aborts the interrupt-driven data transmit.
 *
 * This function aborts the interrupt-driven data sending. Get the remainBytes to find out
 * how many bytes are still not sent out.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 */
void FLEXIO_UART_TransferAbortSend(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle);

/*!
 * @brief Gets the number of bytes sent.
 *
 * This function gets the number of bytes sent driven by interrupt.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * @param count Number of bytes sent so far by the non-blocking transaction.
 * @retval kStatus_NoTransferInProgress transfer has finished or no transfer in progress.
 * @retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_UART_TransferGetSendCount(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle, size_t *count);

/*!
 * @brief Receives a buffer of data using the interrupt method.
 *
 * This function receives data using the interrupt method. This is a non-blocking function,
 * which returns without waiting for all data to be received.
 * If the RX ring buffer is used and not empty, the data in ring buffer is copied and
 * the parameter @p receivedBytes shows how many bytes are copied from the ring buffer.
 * After copying, if the data in ring buffer is not enough to read, the receive
 * request is saved by the UART driver. When new data arrives, the receive request
 * is serviced first. When all data is received, the UART driver notifies the upper layer
 * through a callback function and passes the status parameter @ref kStatus_UART_RxIdle.
 * For example, if the upper layer needs 10 bytes but there are only 5 bytes in the ring buffer,
 * the 5 bytes are copied to xfer->data. This function returns with the
 * parameter @p receivedBytes set to 5. For the last 5 bytes, newly arrived data is
 * saved from the xfer->data[5]. When 5 bytes are received, the UART driver notifies upper layer.
 * If the RX ring buffer is not enabled, this function enables the RX and RX interrupt
 * to receive data to xfer->data. When all data is received, the upper layer is notified.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * @param xfer UART transfer structure. See #flexio_uart_transfer_t.
 * @param receivedBytes Bytes received from the ring buffer directly.
 * @retval kStatus_Success Successfully queue the transfer into the transmit queue.
 * @retval kStatus_FLEXIO_UART_RxBusy Previous receive request is not finished.
 */
status_t FLEXIO_UART_TransferReceiveNonBlocking(FLEXIO_UART_Type *base,
                                                flexio_uart_handle_t *handle,
                                                flexio_uart_transfer_t *xfer,
                                                size_t *receivedBytes);

/*!
 * @brief Aborts the receive data which was using IRQ.
 *
 * This function aborts the receive data which was using IRQ.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 */
void FLEXIO_UART_TransferAbortReceive(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle);

/*!
 * @brief Gets the number of bytes received.
 *
 * This function gets the number of bytes received driven by interrupt.
 *
 * @param base Pointer to the FLEXIO_UART_Type structure.
 * @param handle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 * @param count Number of bytes received so far by the non-blocking transaction.
 * @retval kStatus_NoTransferInProgress transfer has finished or no transfer in progress.
 * @retval kStatus_Success Successfully return the count.
 */
status_t FLEXIO_UART_TransferGetReceiveCount(FLEXIO_UART_Type *base, flexio_uart_handle_t *handle, size_t *count);

/*!
 * @brief FlexIO UART IRQ handler function.
 *
 * This function processes the FlexIO UART transmit and receives the IRQ request.
 *
 * @param uartType Pointer to the FLEXIO_UART_Type structure.
 * @param uartHandle Pointer to the flexio_uart_handle_t structure to store the transfer state.
 */
void FLEXIO_UART_TransferHandleIRQ(void *uartType, void *uartHandle);

/*@}*/

#if defined(__cplusplus)
}
#endif /*_cplusplus*/
/*@}*/

#endif /*_FSL_FLEXIO_UART_H_*/
