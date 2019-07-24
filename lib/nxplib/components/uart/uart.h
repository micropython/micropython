/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_UART_ADAPTER_H__
#define __HAL_UART_ADAPTER_H__

#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#endif

/*!
 * @addtogroup UART_Adapter
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Enable or disable UART adapter non-blocking mode (1 - enable, 0 - disable) */
#ifdef DEBUG_CONSOLE_TRANSFER_NON_BLOCKING
#define UART_ADAPTER_NON_BLOCKING_MODE (1U)
#else
#ifndef SERIAL_MANAGER_NON_BLOCKING_MODE
#define UART_ADAPTER_NON_BLOCKING_MODE (0U)
#else
#define UART_ADAPTER_NON_BLOCKING_MODE SERIAL_MANAGER_NON_BLOCKING_MODE
#endif
#endif

#if defined(__GIC_PRIO_BITS)
#define HAL_UART_ISR_PRIORITY (25U)
#else
#if defined(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)
#define HAL_UART_ISR_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY)
#else
/* The default value 3 is used to support different ARM Core, such as CM0P, CM4, CM7, and CM33, etc.
 * The minimum number of priority bits implemented in the NVIC is 2 on these SOCs. The value of mininum
 * priority is 3 (2^2 - 1). So, the default value is 3.
 */
#define HAL_UART_ISR_PRIORITY (3U)
#endif
#endif

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
#define HAL_UART_HANDLE_SIZE (90U)
#else
#define HAL_UART_HANDLE_SIZE (4U)
#endif

/*! @brief Whether enable transactional function of the UART. (0 - disable, 1 - enable) */
#define HAL_UART_TRANSFER_MODE (0U)

typedef void *hal_uart_handle_t;

/*! @brief UART status */
typedef enum _hal_uart_status
{
    kStatus_HAL_UartSuccess = kStatus_Success,                       /*!< Successfully */
    kStatus_HAL_UartTxBusy  = MAKE_STATUS(kStatusGroup_HAL_UART, 1), /*!< TX busy */
    kStatus_HAL_UartRxBusy  = MAKE_STATUS(kStatusGroup_HAL_UART, 2), /*!< RX busy */
    kStatus_HAL_UartTxIdle  = MAKE_STATUS(kStatusGroup_HAL_UART, 3), /*!< HAL UART transmitter is idle. */
    kStatus_HAL_UartRxIdle  = MAKE_STATUS(kStatusGroup_HAL_UART, 4), /*!< HAL UART receiver is idle */
    kStatus_HAL_UartBaudrateNotSupport =
        MAKE_STATUS(kStatusGroup_HAL_UART, 5), /*!< Baudrate is not support in current clock source */
    kStatus_HAL_UartProtocolError = MAKE_STATUS(
        kStatusGroup_HAL_UART,
        6),                                                        /*!< Error occurs for Noise, Framing, Parity, etc.
                                                                        For transactional transfer, The up layer needs to abort the transfer and then starts again */
    kStatus_HAL_UartError = MAKE_STATUS(kStatusGroup_HAL_UART, 7), /*!< Error occurs on HAL UART */
} hal_uart_status_t;

/*! @brief UART parity mode. */
typedef enum _hal_uart_parity_mode
{
    kHAL_UartParityDisabled = 0x0U, /*!< Parity disabled */
    kHAL_UartParityEven     = 0x1U, /*!< Parity even enabled */
    kHAL_UartParityOdd      = 0x2U, /*!< Parity odd enabled */
} hal_uart_parity_mode_t;

/*! @brief UART stop bit count. */
typedef enum _hal_uart_stop_bit_count
{
    kHAL_UartOneStopBit = 0U, /*!< One stop bit */
    kHAL_UartTwoStopBit = 1U, /*!< Two stop bits */
} hal_uart_stop_bit_count_t;

/*! @brief UART configuration structure. */
typedef struct _hal_uart_config
{
    uint32_t srcClock_Hz;                   /*!< Source clock */
    uint32_t baudRate_Bps;                  /*!< Baud rate  */
    hal_uart_parity_mode_t parityMode;      /*!< Parity mode, disabled (default), even, odd */
    hal_uart_stop_bit_count_t stopBitCount; /*!< Number of stop bits, 1 stop bit (default) or 2 stop bits  */
    uint8_t enableRx;                       /*!< Enable RX */
    uint8_t enableTx;                       /*!< Enable TX */
    uint8_t instance; /*!< Instance (0 - UART0, 1 - UART1, ...), detail information please refer to the
                           SOC corresponding RM.
                           Invalid instance value will cause initialization failure. */
} hal_uart_config_t;

/*! @brief UART transfer callback function. */
typedef void (*hal_uart_transfer_callback_t)(hal_uart_handle_t handle, hal_uart_status_t status, void *callbackParam);

/*! @brief UART transfer structure. */
typedef struct _hal_uart_transfer
{
    uint8_t *data;   /*!< The buffer of data to be transfer.*/
    size_t dataSize; /*!< The byte count to be transfer. */
} hal_uart_transfer_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initializes a UART instance with the UART handle and the user configuration structure.
 *
 * This function configures the UART module with user-defined settings. The user can configure the configuration
 * structure. The parameter handle is a pointer to point to a memory space of size #HAL_UART_HANDLE_SIZE allocated by
 * the caller. Example below shows how to use this API to configure the UART.
 *  @code
 *   uint8_t g_UartHandleBuffer[HAL_UART_HANDLE_SIZE];
 *   hal_uart_handle_t g_UartHandle = &g_UartHandleBuffer[0];
 *   hal_uart_config_t config;
 *   config.srcClock_Hz = 48000000;
 *   config.baudRate_Bps = 115200U;
 *   config.parityMode = kHAL_UartParityDisabled;
 *   config.stopBitCount = kHAL_UartOneStopBit;
 *   config.enableRx = 1;
 *   config.enableTx = 1;
 *   config.instance = 0;
 *   HAL_UartInit(g_UartHandle, &config);
 *  @endcode
 *
 * @param handle Pointer to point to a memory space of size #HAL_UART_HANDLE_SIZE allocated by the caller.
 * @param config Pointer to user-defined configuration structure.
 * @retval kStatus_HAL_UartBaudrateNotSupport Baudrate is not support in current clock source.
 * @retval kStatus_HAL_UartSuccess UART initialization succeed
 */
hal_uart_status_t HAL_UartInit(hal_uart_handle_t handle, hal_uart_config_t *config);

/*!
 * @brief Deinitializes a UART instance.
 *
 * This function waits for TX complete, disables TX and RX, and disables the UART clock.
 *
 * @param handle UART handle pointer.
 * @retval kStatus_HAL_UartSuccess UART de-initialization succeed
 */
hal_uart_status_t HAL_UartDeinit(hal_uart_handle_t handle);

/*! @}*/

/*!
 * @name Blocking bus Operations
 * @{
 */

/*!
 * @brief Reads RX data register using a blocking method.
 *
 * This function polls the RX register, waits for the RX register to be full or for RX FIFO to
 * have data, and reads data from the RX register.
 *
 * @note The function #HAL_UartReceiveBlocking and the function #HAL_UartTransferReceiveNonBlocking
 * cannot be used at the same time.
 * And, the function #HAL_UartTransferAbortReceive cannot be used to abort the transmission of this function.
 *
 * @param handle UART handle pointer.
 * @param data Start address of the buffer to store the received data.
 * @param length Size of the buffer.
 * @retval kStatus_HAL_UartError An error occurred while receiving data.
 * @retval kStatus_HAL_UartParityError A parity error occurred while receiving data.
 * @retval kStatus_HAL_UartSuccess Successfully received all data.
 */
hal_uart_status_t HAL_UartReceiveBlocking(hal_uart_handle_t handle, uint8_t *data, size_t length);

/*!
 * @brief Writes to the TX register using a blocking method.
 *
 * This function polls the TX register, waits for the TX register to be empty or for the TX FIFO
 * to have room and writes data to the TX buffer.
 *
 * @note The function #HAL_UartSendBlocking and the function #HAL_UartTransferSendNonBlocking
 * cannot be used at the same time.
 * And, the function #HAL_UartTransferAbortSend cannot be used to abort the transmission of this function.
 *
 * @param handle UART handle pointer.
 * @param data Start address of the data to write.
 * @param length Size of the data to write.
 * @retval kStatus_HAL_UartSuccess Successfully sent all data.
 */
hal_uart_status_t HAL_UartSendBlocking(hal_uart_handle_t handle, const uint8_t *data, size_t length);

/*! @}*/

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))

/*!
 * @name Transactional
 * @note The transactional API and the functional API cannot be used at the same time. The macro
 * #HAL_UART_TRANSFER_MODE is used to set which one will be used. If #HAL_UART_TRANSFER_MODE is zero, the
 * functional API with non-blocking mode will be used. Otherwise, transactional API will be used.
 * @{
 */

/*!
 * @brief Installs a callback and callback parameter.
 *
 * This function is used to install the callback and callback parameter for UART module.
 * When any status of the UART changed, the driver will notify the upper layer by the installed callback
 * function. And the status is also passed as status parameter when the callback is called.
 *
 * @param handle UART handle pointer.
 * @param callback The callback function.
 * @param callbackParam The parameter of the callback function.
 * @retval kStatus_HAL_UartSuccess Successfully install the callback.
 */
hal_uart_status_t HAL_UartTransferInstallCallback(hal_uart_handle_t handle,
                                                  hal_uart_transfer_callback_t callback,
                                                  void *callbackParam);

/*!
 * @brief Receives a buffer of data using an interrupt method.
 *
 * This function receives data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data to be received.
 * The receive request is saved by the UART driver.
 * When the new data arrives, the receive request is serviced first.
 * When all data is received, the UART driver notifies the upper layer
 * through a callback function and passes the status parameter @ref kStatus_UART_RxIdle.
 *
 * @note The function #HAL_UartReceiveBlocking and the function #HAL_UartTransferReceiveNonBlocking
 * cannot be used at the same time.
 *
 * @param handle UART handle pointer.
 * @param transfer UART transfer structure, see #hal_uart_transfer_t.
 * @retval kStatus_HAL_UartSuccess Successfully queue the transfer into transmit queue.
 * @retval kStatus_HAL_UartRxBusy Previous receive request is not finished.
 * @retval kStatus_HAL_UartError An error occurred.
 */
hal_uart_status_t HAL_UartTransferReceiveNonBlocking(hal_uart_handle_t handle, hal_uart_transfer_t *transfer);

/*!
 * @brief Transmits a buffer of data using the interrupt method.
 *
 * This function sends data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data to be written to the TX register. When
 * all data is written to the TX register in the ISR, the UART driver calls the callback
 * function and passes the @ref kStatus_UART_TxIdle as status parameter.
 *
 * @note The function #HAL_UartSendBlocking and the function #HAL_UartTransferSendNonBlocking
 * cannot be used at the same time.
 *
 * @param handle UART handle pointer.
 * @param transfer UART transfer structure. See #hal_uart_transfer_t.
 * @retval kStatus_HAL_UartSuccess Successfully start the data transmission.
 * @retval kStatus_HAL_UartTxBusy Previous transmission still not finished; data not all written to TX register yet.
 * @retval kStatus_HAL_UartError An error occurred.
 */
hal_uart_status_t HAL_UartTransferSendNonBlocking(hal_uart_handle_t handle, hal_uart_transfer_t *transfer);

/*!
 * @brief Gets the number of bytes that have been received.
 *
 * This function gets the number of bytes that have been received.
 *
 * @param handle UART handle pointer.
 * @param count Receive bytes count.
 * @retval kStatus_HAL_UartError An error occurred.
 * @retval kStatus_Success Get successfully through the parameter \p count.
 */
hal_uart_status_t HAL_UartTransferGetReceiveCount(hal_uart_handle_t handle, uint32_t *count);

/*!
 * @brief Gets the number of bytes written to the UART TX register.
 *
 * This function gets the number of bytes written to the UART TX
 * register by using the interrupt method.
 *
 * @param handle UART handle pointer.
 * @param count Send bytes count.
 * @retval kStatus_HAL_UartError An error occurred.
 * @retval kStatus_Success Get successfully through the parameter \p count.
 */
hal_uart_status_t HAL_UartTransferGetSendCount(hal_uart_handle_t handle, uint32_t *count);

/*!
 * @brief Aborts the interrupt-driven data receiving.
 *
 * This function aborts the interrupt-driven data receiving. The user can get the remainBytes to know
 * how many bytes are not received yet.
 *
 * @note The function #HAL_UartTransferAbortReceive cannot be used to abort the transmission of
 * the function #HAL_UartReceiveBlocking.
 *
 * @param handle UART handle pointer.
 * @retval kStatus_Success Get successfully abort the receiving.
 */
hal_uart_status_t HAL_UartTransferAbortReceive(hal_uart_handle_t handle);

/*!
 * @brief Aborts the interrupt-driven data sending.
 *
 * This function aborts the interrupt-driven data sending. The user can get the remainBytes to find out
 * how many bytes are not sent out.
 *
 * @note The function #HAL_UartTransferAbortSend cannot be used to abort the transmission of
 * the function #HAL_UartSendBlocking.
 *
 * @param handle UART handle pointer.
 * @retval kStatus_Success Get successfully abort the sending.
 */
hal_uart_status_t HAL_UartTransferAbortSend(hal_uart_handle_t handle);

/*! @}*/

#else

/*!
 * @name Functional API with non-blocking mode.
 * @note The functional API and the transactional API cannot be used at the same time. The macro
 * #HAL_UART_TRANSFER_MODE is used to set which one will be used. If #HAL_UART_TRANSFER_MODE is zero, the
 * functional API with non-blocking mode will be used. Otherwise, transactional API will be used.
 * @{
 */

/*!
 * @brief Installs a callback and callback parameter.
 *
 * This function is used to install the callback and callback parameter for UART module.
 * When non-blocking sending or receiving finished, the adapter will notify the upper layer by the installed callback
 * function. And the status is also passed as status parameter when the callback is called.
 *
 * @param handle UART handle pointer.
 * @param callback The callback function.
 * @param callbackParam The parameter of the callback function.
 * @retval kStatus_HAL_UartSuccess Successfully install the callback.
 */
hal_uart_status_t HAL_UartInstallCallback(hal_uart_handle_t handle,
                                          hal_uart_transfer_callback_t callback,
                                          void *callbackParam);

/*!
 * @brief Receives a buffer of data using an interrupt method.
 *
 * This function receives data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data to be received.
 * The receive request is saved by the UART adapter.
 * When the new data arrives, the receive request is serviced first.
 * When all data is received, the UART adapter notifies the upper layer
 * through a callback function and passes the status parameter @ref kStatus_UART_RxIdle.
 *
 * @note The function #HAL_UartReceiveBlocking and the function #HAL_UartReceiveNonBlocking
 * cannot be used at the same time.
 *
 * @param handle UART handle pointer.
 * @param data Start address of the data to write.
 * @param length Size of the data to write.
 * @retval kStatus_HAL_UartSuccess Successfully queue the transfer into transmit queue.
 * @retval kStatus_HAL_UartRxBusy Previous receive request is not finished.
 * @retval kStatus_HAL_UartError An error occurred.
 */
hal_uart_status_t HAL_UartReceiveNonBlocking(hal_uart_handle_t handle, uint8_t *data, size_t length);

/*!
 * @brief Transmits a buffer of data using the interrupt method.
 *
 * This function sends data using an interrupt method. This is a non-blocking function, which
 * returns directly without waiting for all data to be written to the TX register. When
 * all data is written to the TX register in the ISR, the UART driver calls the callback
 * function and passes the @ref kStatus_UART_TxIdle as status parameter.
 *
 * @note The function #HAL_UartSendBlocking and the function #HAL_UartSendNonBlocking
 * cannot be used at the same time.
 *
 * @param handle UART handle pointer.
 * @param data Start address of the data to write.
 * @param length Size of the data to write.
 * @retval kStatus_HAL_UartSuccess Successfully start the data transmission.
 * @retval kStatus_HAL_UartTxBusy Previous transmission still not finished; data not all written to TX register yet.
 * @retval kStatus_HAL_UartError An error occurred.
 */
hal_uart_status_t HAL_UartSendNonBlocking(hal_uart_handle_t handle, uint8_t *data, size_t length);

/*!
 * @brief Gets the number of bytes that have been received.
 *
 * This function gets the number of bytes that have been received.
 *
 * @param handle UART handle pointer.
 * @param count Receive bytes count.
 * @retval kStatus_HAL_UartError An error occurred.
 * @retval kStatus_Success Get successfully through the parameter \p count.
 */
hal_uart_status_t HAL_UartGetReceiveCount(hal_uart_handle_t handle, uint32_t *reCount);

/*!
 * @brief Gets the number of bytes written to the UART TX register.
 *
 * This function gets the number of bytes written to the UART TX
 * register by using the interrupt method.
 *
 * @param handle UART handle pointer.
 * @param count Send bytes count.
 * @retval kStatus_HAL_UartError An error occurred.
 * @retval kStatus_Success Get successfully through the parameter \p count.
 */
hal_uart_status_t HAL_UartGetSendCount(hal_uart_handle_t handle, uint32_t *seCount);

/*!
 * @brief Aborts the interrupt-driven data receiving.
 *
 * This function aborts the interrupt-driven data receiving. The user can get the remainBytes to know
 * how many bytes are not received yet.
 *
 * @note The function #HAL_UartAbortReceive cannot be used to abort the transmission of
 * the function #HAL_UartReceiveBlocking.
 *
 * @param handle UART handle pointer.
 * @retval kStatus_Success Get successfully abort the receiving.
 */
hal_uart_status_t HAL_UartAbortReceive(hal_uart_handle_t handle);

/*!
 * @brief Aborts the interrupt-driven data sending.
 *
 * This function aborts the interrupt-driven data sending. The user can get the remainBytes to find out
 * how many bytes are not sent out.
 *
 * @note The function #HAL_UartAbortSend cannot be used to abort the transmission of
 * the function #HAL_UartSendBlocking.
 *
 * @param handle UART handle pointer.
 * @retval kStatus_Success Get successfully abort the sending.
 */
hal_uart_status_t HAL_UartAbortSend(hal_uart_handle_t handle);

/*! @}*/

#endif
#endif

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
/*!
 * @brief UART IRQ handle function.
 *
 * This function handles the UART transmit and receive IRQ request.
 *
 * @param handle UART handle pointer.
 */
void HAL_UartIsrFunction(hal_uart_handle_t handle);
#endif

#if defined(__cplusplus)
}
#endif
/*! @}*/
#endif /* __HAL_UART_ADAPTER_H__ */
