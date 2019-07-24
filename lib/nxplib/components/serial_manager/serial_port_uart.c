/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "serial_manager.h"
#include "serial_port_internal.h"

#if (defined(SERIAL_PORT_TYPE_UART) && (SERIAL_PORT_TYPE_UART > 0U))
#include "uart.h"

#include "serial_port_uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef NDEBUG
#if (defined(DEBUG_CONSOLE_ASSERT_DISABLE) && (DEBUG_CONSOLE_ASSERT_DISABLE > 0U))
#undef assert
#define assert(n)
#endif
#endif

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
#define SERIAL_PORT_UART_RECEIVE_DATA_LENGTH 1U

typedef struct _serial_uart_send_state
{
    serial_manager_callback_t callback;
    void *callbackParam;
    uint8_t *buffer;
    uint32_t length;
    volatile uint8_t busy;
} serial_uart_send_state_t;

typedef struct _serial_uart_recv_state
{
    serial_manager_callback_t callback;
    void *callbackParam;
    volatile uint8_t busy;
    uint8_t readBuffer[SERIAL_PORT_UART_RECEIVE_DATA_LENGTH];
} serial_uart_recv_state_t;
#endif

typedef struct _serial_uart_state
{
#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
    serial_uart_send_state_t tx;
    serial_uart_recv_state_t rx;
#endif
    uint8_t usartHandleBuffer[HAL_UART_HANDLE_SIZE];
} serial_uart_state_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
/* UART user callback */
static void Serial_UartCallback(hal_uart_handle_t handle, hal_uart_status_t status, void *userData)
{
    serial_uart_state_t *serialUartHandle;
    serial_manager_callback_message_t msg;
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    hal_uart_transfer_t transfer;
#endif

    if (NULL == userData)
    {
        return;
    }

    serialUartHandle = (serial_uart_state_t *)userData;

    if ((hal_uart_status_t)kStatus_HAL_UartRxIdle == status)
    {
        if ((NULL != serialUartHandle->rx.callback))
        {
            msg.buffer = &serialUartHandle->rx.readBuffer[0];
            msg.length = sizeof(serialUartHandle->rx.readBuffer);
            serialUartHandle->rx.callback(serialUartHandle->rx.callbackParam, &msg, kStatus_SerialManager_Success);
        }
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
        transfer.data     = &serialUartHandle->rx.readBuffer[0];
        transfer.dataSize = sizeof(serialUartHandle->rx.readBuffer);
        if (kStatus_HAL_UartSuccess ==
            HAL_UartTransferReceiveNonBlocking(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]), &transfer))
#else
        if ((hal_uart_status_t)kStatus_HAL_UartSuccess ==
            HAL_UartReceiveNonBlocking(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]),
                                       &serialUartHandle->rx.readBuffer[0], sizeof(serialUartHandle->rx.readBuffer)))
#endif
        {
            serialUartHandle->rx.busy = 1U;
        }
        else
        {
            serialUartHandle->rx.busy = 0U;
        }
    }
    else if ((hal_uart_status_t)kStatus_HAL_UartTxIdle == status)
    {
        if (serialUartHandle->tx.busy != 0U)
        {
            serialUartHandle->tx.busy = 0U;
            if ((NULL != serialUartHandle->tx.callback))
            {
                msg.buffer = serialUartHandle->tx.buffer;
                msg.length = serialUartHandle->tx.length;
                serialUartHandle->tx.callback(serialUartHandle->tx.callbackParam, &msg, kStatus_SerialManager_Success);
            }
        }
    }
    else
    {
    }
}
#endif

serial_manager_status_t Serial_UartInit(serial_handle_t serialHandle, void *serialConfig)
{
    serial_uart_state_t *serialUartHandle;
    serial_port_uart_config_t *uartConfig;
    hal_uart_config_t config;
#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    hal_uart_transfer_t transfer;
#endif
#endif

    assert(serialConfig);
    assert(serialHandle);
    assert(SERIAL_PORT_UART_HANDLE_SIZE >= sizeof(serial_uart_state_t));

    uartConfig       = (serial_port_uart_config_t *)serialConfig;
    serialUartHandle = (serial_uart_state_t *)serialHandle;

    config.baudRate_Bps = uartConfig->baudRate;
    config.parityMode   = (hal_uart_parity_mode_t)uartConfig->parityMode;
    config.stopBitCount = (hal_uart_stop_bit_count_t)uartConfig->stopBitCount;
    config.enableRx     = uartConfig->enableRx;
    config.enableTx     = uartConfig->enableTx;
    config.srcClock_Hz  = uartConfig->clockRate;
    config.instance     = uartConfig->instance;

    if (kStatus_HAL_UartSuccess != HAL_UartInit(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]), &config))
    {
        return kStatus_SerialManager_Error;
    }

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    if (kStatus_HAL_UartSuccess !=
        HAL_UartTransferInstallCallback(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]),
                                        Serial_UartCallback, serialUartHandle))
#else
    if (kStatus_HAL_UartSuccess != HAL_UartInstallCallback(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]),
                                                           Serial_UartCallback, serialUartHandle))
#endif
    {
        return kStatus_SerialManager_Error;
    }

    if (uartConfig->enableRx != 0U)
    {
        serialUartHandle->rx.busy = 1U;
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
        transfer.data     = &serialUartHandle->rx.readBuffer[0];
        transfer.dataSize = sizeof(serialUartHandle->rx.readBuffer);
        if (kStatus_HAL_UartSuccess !=
            HAL_UartTransferReceiveNonBlocking(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]), &transfer))
#else
        if (kStatus_HAL_UartSuccess !=
            HAL_UartReceiveNonBlocking(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]),
                                       &serialUartHandle->rx.readBuffer[0], sizeof(serialUartHandle->rx.readBuffer)))
#endif
        {
            serialUartHandle->rx.busy = 0U;
            return kStatus_SerialManager_Error;
        }
    }
#endif

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UartDeinit(serial_handle_t serialHandle)
{
    serial_uart_state_t *serialUartHandle;

    assert(serialHandle);

    serialUartHandle = (serial_uart_state_t *)serialHandle;

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    (void)HAL_UartTransferAbortReceive(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]));
#else
    (void)HAL_UartAbortReceive(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]));
#endif
#endif
    (void)HAL_UartDeinit(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]));

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
    serialUartHandle->tx.busy = 0U;
    serialUartHandle->rx.busy = 0U;
#endif

    return kStatus_SerialManager_Success;
}

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))

serial_manager_status_t Serial_UartWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_uart_state_t *serialUartHandle;
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    hal_uart_transfer_t transfer;
#endif

    assert(serialHandle);
    assert(buffer);
    assert(length);

    serialUartHandle = (serial_uart_state_t *)serialHandle;

    if (serialUartHandle->tx.busy != 0U)
    {
        return kStatus_SerialManager_Busy;
    }
    serialUartHandle->tx.busy = 1U;

    serialUartHandle->tx.buffer = buffer;
    serialUartHandle->tx.length = length;

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    transfer.data     = buffer;
    transfer.dataSize = length;
    if (kStatus_HAL_UartSuccess !=
        HAL_UartTransferSendNonBlocking(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]), &transfer))
#else
    if (kStatus_HAL_UartSuccess !=
        HAL_UartSendNonBlocking(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]), buffer, length))
#endif
    {
        serialUartHandle->tx.busy = 0U;
        return kStatus_SerialManager_Error;
    }
    return kStatus_SerialManager_Success;
}

#else

serial_manager_status_t Serial_UartWrite(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_uart_state_t *serialUartHandle;

    assert(serialHandle);
    assert(buffer);
    assert(length);

    serialUartHandle = (serial_uart_state_t *)serialHandle;

    return (serial_manager_status_t)HAL_UartSendBlocking(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]),
                                                         buffer, length);
}

serial_manager_status_t Serial_UartRead(serial_handle_t serialHandle, uint8_t *buffer, uint32_t length)
{
    serial_uart_state_t *serialUartHandle;

    assert(serialHandle);
    assert(buffer);
    assert(length);

    serialUartHandle = (serial_uart_state_t *)serialHandle;

    return (serial_manager_status_t)HAL_UartReceiveBlocking(
        ((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]), buffer, length);
}

#endif

#if (defined(SERIAL_MANAGER_NON_BLOCKING_MODE) && (SERIAL_MANAGER_NON_BLOCKING_MODE > 0U))
serial_manager_status_t Serial_UartCancelWrite(serial_handle_t serialHandle)
{
    serial_uart_state_t *serialUartHandle;
    serial_manager_callback_message_t msg;
    uint32_t primask;
    uint8_t isBusy = 0U;

    assert(serialHandle);

    serialUartHandle = (serial_uart_state_t *)serialHandle;

    primask                   = DisableGlobalIRQ();
    isBusy                    = serialUartHandle->tx.busy;
    serialUartHandle->tx.busy = 0U;
    EnableGlobalIRQ(primask);

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    (void)HAL_UartTransferAbortSend(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]));
#else
    (void)HAL_UartAbortSend(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]));
#endif
    if (isBusy != 0U)
    {
        if ((NULL != serialUartHandle->tx.callback))
        {
            msg.buffer = serialUartHandle->tx.buffer;
            msg.length = serialUartHandle->tx.length;
            serialUartHandle->tx.callback(serialUartHandle->tx.callbackParam, &msg, kStatus_SerialManager_Canceled);
        }
    }
    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UartInstallTxCallback(serial_handle_t serialHandle,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam)
{
    serial_uart_state_t *serialUartHandle;

    assert(serialHandle);

    serialUartHandle = (serial_uart_state_t *)serialHandle;

    serialUartHandle->tx.callback      = callback;
    serialUartHandle->tx.callbackParam = callbackParam;

    return kStatus_SerialManager_Success;
}

serial_manager_status_t Serial_UartInstallRxCallback(serial_handle_t serialHandle,
                                                     serial_manager_callback_t callback,
                                                     void *callbackParam)
{
    serial_uart_state_t *serialUartHandle;

    assert(serialHandle);

    serialUartHandle = (serial_uart_state_t *)serialHandle;

    serialUartHandle->rx.callback      = callback;
    serialUartHandle->rx.callbackParam = callbackParam;

    return kStatus_SerialManager_Success;
}

void Serial_UartIsrFunction(serial_handle_t serialHandle)
{
    serial_uart_state_t *serialUartHandle;

    assert(serialHandle);

    serialUartHandle = (serial_uart_state_t *)serialHandle;

    HAL_UartIsrFunction(((hal_uart_handle_t)&serialUartHandle->usartHandleBuffer[0]));
}
#endif

#endif
