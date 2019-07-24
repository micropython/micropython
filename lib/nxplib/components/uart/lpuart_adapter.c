/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_lpuart.h"

#include "uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef NDEBUG
#if (defined(DEBUG_CONSOLE_ASSERT_DISABLE) && (DEBUG_CONSOLE_ASSERT_DISABLE > 0U))
#undef assert
#define assert(n)
#endif
#endif

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
/*! @brief uart RX state structure. */
typedef struct _hal_uart_receive_state
{
    uint8_t *volatile buffer;
    volatile uint32_t bufferLength;
    volatile uint32_t bufferSofar;
} hal_uart_receive_state_t;

/*! @brief uart TX state structure. */
typedef struct _hal_uart_send_state
{
    uint8_t *volatile buffer;
    volatile uint32_t bufferLength;
    volatile uint32_t bufferSofar;
} hal_uart_send_state_t;
#endif
/*! @brief uart state structure. */
typedef struct _hal_uart_state
{
#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
    hal_uart_transfer_callback_t callback;
    void *callbackParam;
#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    lpuart_handle_t hardwareHandle;
#endif
    hal_uart_receive_state_t rx;
    hal_uart_send_state_t tx;
#endif
    uint8_t instance;
} hal_uart_state_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static LPUART_Type *const s_LpuartAdapterBase[] = LPUART_BASE_PTRS;

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))

#if !(defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
/* Array of LPUART IRQ number. */
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
static const IRQn_Type s_LpuartRxIRQ[] = LPUART_RX_IRQS;
static const IRQn_Type s_LpuartTxIRQ[] = LPUART_TX_IRQS;
#else
static const IRQn_Type s_LpuartIRQ[] = LPUART_RX_TX_IRQS;
#endif
#endif

#if !(defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
static hal_uart_state_t *s_UartState[sizeof(s_LpuartAdapterBase) / sizeof(LPUART_Type *)];
#endif

#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
static hal_uart_status_t HAL_UartGetStatus(status_t status)
{
    hal_uart_status_t uartStatus = kStatus_HAL_UartError;
    switch (status)
    {
        case (int32_t)kStatus_Success:
            uartStatus = kStatus_HAL_UartSuccess;
            break;
        case (int32_t)kStatus_LPUART_TxBusy:
            uartStatus = kStatus_HAL_UartTxBusy;
            break;
        case (int32_t)kStatus_LPUART_RxBusy:
            uartStatus = kStatus_HAL_UartRxBusy;
            break;
        case (int32_t)kStatus_LPUART_TxIdle:
            uartStatus = kStatus_HAL_UartTxIdle;
            break;
        case (int32_t)kStatus_LPUART_RxIdle:
            uartStatus = kStatus_HAL_UartRxIdle;
            break;
        case (int32_t)kStatus_LPUART_BaudrateNotSupport:
            uartStatus = kStatus_HAL_UartBaudrateNotSupport;
            break;
        case (int32_t)kStatus_LPUART_NoiseError:
        case (int32_t)kStatus_LPUART_FramingError:
        case (int32_t)kStatus_LPUART_ParityError:
            uartStatus = kStatus_HAL_UartProtocolError;
            break;
        default:
            /*MISRA rule 16.4*/
            break;
    }
    return uartStatus;
}
#else
static hal_uart_status_t HAL_UartGetStatus(status_t status)
{
    if ((int32_t)kStatus_Success == status)
    {
        return kStatus_HAL_UartSuccess;
    }
    else
    {
        return kStatus_HAL_UartError;
    }
}
#endif

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
static void HAL_UartCallback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *callbackParam)
{
    hal_uart_state_t *uartHandle;
    hal_uart_status_t uartStatus = HAL_UartGetStatus(status);
    assert(callbackParam);

    uartHandle = (hal_uart_state_t *)callbackParam;

    if (kStatus_HAL_UartProtocolError == uartStatus)
    {
        if (uartHandle->hardwareHandle.rxDataSize != 0U)
        {
            uartStatus = kStatus_HAL_UartError;
        }
    }

    if (uartHandle->callback != NULL)
    {
        uartHandle->callback(uartHandle, uartStatus, uartHandle->callbackParam);
    }
}

#else

static void HAL_UartInterruptHandle(uint8_t instance)
{
    hal_uart_state_t *uartHandle = s_UartState[instance];
    uint32_t status;

    if (NULL == uartHandle)
    {
        return;
    }

    status = LPUART_GetStatusFlags(s_LpuartAdapterBase[instance]);

    /* Receive data register full */
    if (((LPUART_STAT_RDRF_MASK & status) != 0U) && ((LPUART_GetEnabledInterrupts(s_LpuartAdapterBase[instance]) &
                                                      (uint32_t)kLPUART_RxDataRegFullInterruptEnable) != 0U))
    {
        if (uartHandle->rx.buffer != NULL)
        {
            uartHandle->rx.buffer[uartHandle->rx.bufferSofar++] = LPUART_ReadByte(s_LpuartAdapterBase[instance]);
            if (uartHandle->rx.bufferSofar >= uartHandle->rx.bufferLength)
            {
                LPUART_DisableInterrupts(s_LpuartAdapterBase[instance], (uint32_t)kLPUART_RxDataRegFullInterruptEnable |
                                                                            (uint32_t)kLPUART_RxOverrunInterruptEnable);
                uartHandle->rx.buffer = NULL;
                if (uartHandle->callback != NULL)
                {
                    uartHandle->callback(uartHandle, kStatus_HAL_UartRxIdle, uartHandle->callbackParam);
                }
            }
        }
    }

    /* Send data register empty and the interrupt is enabled. */
    if (((LPUART_STAT_TDRE_MASK & status) != 0U) && ((LPUART_GetEnabledInterrupts(s_LpuartAdapterBase[instance]) &
                                                      (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable) != 0U))
    {
        if (uartHandle->tx.buffer != NULL)
        {
            LPUART_WriteByte(s_LpuartAdapterBase[instance], uartHandle->tx.buffer[uartHandle->tx.bufferSofar++]);
            if (uartHandle->tx.bufferSofar >= uartHandle->tx.bufferLength)
            {
                LPUART_DisableInterrupts(s_LpuartAdapterBase[uartHandle->instance],
                                         (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable);
                uartHandle->tx.buffer = NULL;
                if (uartHandle->callback != NULL)
                {
                    uartHandle->callback(uartHandle, kStatus_HAL_UartTxIdle, uartHandle->callbackParam);
                }
            }
        }
    }

#if 1
    (void)LPUART_ClearStatusFlags(s_LpuartAdapterBase[instance], status);
#endif
}
#endif

#endif

hal_uart_status_t HAL_UartInit(hal_uart_handle_t handle, hal_uart_config_t *config)
{
    hal_uart_state_t *uartHandle;
    lpuart_config_t lpuartConfig;
    status_t status;
    assert(handle);
    assert(config);
    assert(config->instance < (sizeof(s_LpuartAdapterBase) / sizeof(LPUART_Type *)));
    assert(s_LpuartAdapterBase[config->instance]);
    assert(HAL_UART_HANDLE_SIZE >= sizeof(hal_uart_state_t));

    LPUART_GetDefaultConfig(&lpuartConfig);
    lpuartConfig.baudRate_Bps = config->baudRate_Bps;

    if (kHAL_UartParityEven == config->parityMode)
    {
        lpuartConfig.parityMode = kLPUART_ParityEven;
    }
    else if (kHAL_UartParityOdd == config->parityMode)
    {
        lpuartConfig.parityMode = kLPUART_ParityOdd;
    }
    else
    {
        lpuartConfig.parityMode = kLPUART_ParityDisabled;
    }

    if (kHAL_UartTwoStopBit == config->stopBitCount)
    {
        lpuartConfig.stopBitCount = kLPUART_TwoStopBit;
    }
    else
    {
        lpuartConfig.stopBitCount = kLPUART_OneStopBit;
    }
    lpuartConfig.enableRx = (bool)config->enableRx;
    lpuartConfig.enableTx = (bool)config->enableTx;

    status = LPUART_Init(s_LpuartAdapterBase[config->instance], (void *)&lpuartConfig, config->srcClock_Hz);

    if ((int32_t)kStatus_Success != status)
    {
        return HAL_UartGetStatus(status);
    }

    uartHandle           = (hal_uart_state_t *)handle;
    uartHandle->instance = config->instance;

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    LPUART_TransferCreateHandle(s_LpuartAdapterBase[config->instance], &uartHandle->hardwareHandle,
                                (lpuart_transfer_callback_t)HAL_UartCallback, handle);
#else
    s_UartState[uartHandle->instance] = uartHandle;
/* Enable interrupt in NVIC. */
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    NVIC_SetPriority((IRQn_Type)s_LpuartRxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    NVIC_SetPriority((IRQn_Type)s_LpuartTxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    NVIC_SetPriority((IRQn_Type)s_LpuartIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    (void)EnableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif

#endif

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartDeinit(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;

    assert(handle);

    uartHandle = (hal_uart_state_t *)handle;

    LPUART_Deinit(s_LpuartAdapterBase[uartHandle->instance]);

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))

#if !(defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))
    s_UartState[uartHandle->instance] = NULL;
#endif

#endif

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartReceiveBlocking(hal_uart_handle_t handle, uint8_t *data, size_t length)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(data);
    assert(length);

    uartHandle = (hal_uart_state_t *)handle;

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
    if (uartHandle->rx.buffer != NULL)
    {
        return kStatus_HAL_UartRxBusy;
    }
#endif

    status = LPUART_ReadBlocking(s_LpuartAdapterBase[uartHandle->instance], data, length);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartSendBlocking(hal_uart_handle_t handle, const uint8_t *data, size_t length)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(data);
    assert(length);

    uartHandle = (hal_uart_state_t *)handle;

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))
    if (uartHandle->tx.buffer != NULL)
    {
        return kStatus_HAL_UartTxBusy;
    }
#endif

    LPUART_WriteBlocking(s_LpuartAdapterBase[uartHandle->instance], data, length);

    return kStatus_HAL_UartSuccess;
}

#if (defined(UART_ADAPTER_NON_BLOCKING_MODE) && (UART_ADAPTER_NON_BLOCKING_MODE > 0U))

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))

hal_uart_status_t HAL_UartTransferInstallCallback(hal_uart_handle_t handle,
                                                  hal_uart_transfer_callback_t callback,
                                                  void *callbackParam)
{
    hal_uart_state_t *uartHandle;

    assert(handle);
    assert(HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    uartHandle->callbackParam = callbackParam;
    uartHandle->callback      = callback;

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartTransferReceiveNonBlocking(hal_uart_handle_t handle, hal_uart_transfer_t *transfer)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(transfer);
    assert(HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    status = LPUART_TransferReceiveNonBlocking(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle,
                                               (lpuart_transfer_t *)(void *)transfer, NULL);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartTransferSendNonBlocking(hal_uart_handle_t handle, hal_uart_transfer_t *transfer)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(transfer);
    assert(HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    status = LPUART_TransferSendNonBlocking(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle,
                                            (lpuart_transfer_t *)(void *)transfer);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartTransferGetReceiveCount(hal_uart_handle_t handle, uint32_t *count)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(count);
    assert(HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    status =
        LPUART_TransferGetReceiveCount(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle, count);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartTransferGetSendCount(hal_uart_handle_t handle, uint32_t *count)
{
    hal_uart_state_t *uartHandle;
    status_t status;
    assert(handle);
    assert(count);
    assert(HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    status = LPUART_TransferGetSendCount(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle, count);

    return HAL_UartGetStatus(status);
}

hal_uart_status_t HAL_UartTransferAbortReceive(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    LPUART_TransferAbortReceive(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle);

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartTransferAbortSend(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    LPUART_TransferAbortSend(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle);

    return kStatus_HAL_UartSuccess;
}

#else

/* None transactional API with non-blocking mode. */
hal_uart_status_t HAL_UartInstallCallback(hal_uart_handle_t handle,
                                          hal_uart_transfer_callback_t callback,
                                          void *callbackParam)
{
    hal_uart_state_t *uartHandle;

    assert(handle);
    assert(!HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    uartHandle->callbackParam = callbackParam;
    uartHandle->callback      = callback;

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartReceiveNonBlocking(hal_uart_handle_t handle, uint8_t *data, size_t length)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(data);
    assert(length);
    assert(!HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    if (uartHandle->rx.buffer != NULL)
    {
        return kStatus_HAL_UartRxBusy;
    }

    uartHandle->rx.bufferLength = length;
    uartHandle->rx.bufferSofar  = 0;
    uartHandle->rx.buffer       = data;
    LPUART_EnableInterrupts(s_LpuartAdapterBase[uartHandle->instance], (uint32_t)kLPUART_RxDataRegFullInterruptEnable |
                                                                           (uint32_t)kLPUART_RxOverrunInterruptEnable);
    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartSendNonBlocking(hal_uart_handle_t handle, uint8_t *data, size_t length)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(data);
    assert(length);
    assert(!HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    if (uartHandle->tx.buffer != NULL)
    {
        return kStatus_HAL_UartTxBusy;
    }
    uartHandle->tx.bufferLength = length;
    uartHandle->tx.bufferSofar  = 0;
    uartHandle->tx.buffer       = data;
    LPUART_EnableInterrupts(s_LpuartAdapterBase[uartHandle->instance], (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable);
    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartGetReceiveCount(hal_uart_handle_t handle, uint32_t *reCount)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(reCount);
    assert(!HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    if (uartHandle->rx.buffer != NULL)
    {
        *reCount = uartHandle->rx.bufferSofar;
        return kStatus_HAL_UartSuccess;
    }
    return kStatus_HAL_UartError;
}

hal_uart_status_t HAL_UartGetSendCount(hal_uart_handle_t handle, uint32_t *seCount)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(seCount);
    assert(!HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    if (uartHandle->tx.buffer != NULL)
    {
        *seCount = uartHandle->tx.bufferSofar;
        return kStatus_HAL_UartSuccess;
    }
    return kStatus_HAL_UartError;
}

hal_uart_status_t HAL_UartAbortReceive(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(!HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    if (uartHandle->rx.buffer != NULL)
    {
        LPUART_DisableInterrupts(
            s_LpuartAdapterBase[uartHandle->instance],
            (uint32_t)kLPUART_RxDataRegFullInterruptEnable | (uint32_t)kLPUART_RxOverrunInterruptEnable);
        uartHandle->rx.buffer = NULL;
    }

    return kStatus_HAL_UartSuccess;
}

hal_uart_status_t HAL_UartAbortSend(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(!HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

    if (uartHandle->tx.buffer != NULL)
    {
        LPUART_DisableInterrupts(s_LpuartAdapterBase[uartHandle->instance],
                                 (uint32_t)kLPUART_TxDataRegEmptyInterruptEnable);
        uartHandle->tx.buffer = NULL;
    }

    return kStatus_HAL_UartSuccess;
}

#endif

#if (defined(HAL_UART_TRANSFER_MODE) && (HAL_UART_TRANSFER_MODE > 0U))

void HAL_UartIsrFunction(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

#if 0
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    DisableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    DisableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    DisableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif
    LPUART_TransferHandleIRQ(s_LpuartAdapterBase[uartHandle->instance], &uartHandle->hardwareHandle);
#if 0
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    NVIC_SetPriority((IRQn_Type)s_LpuartRxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    NVIC_SetPriority((IRQn_Type)s_LpuartTxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    NVIC_SetPriority((IRQn_Type)s_LpuartIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif
}

#else

void HAL_UartIsrFunction(hal_uart_handle_t handle)
{
    hal_uart_state_t *uartHandle;
    assert(handle);
    assert(!HAL_UART_TRANSFER_MODE);

    uartHandle = (hal_uart_state_t *)handle;

#if 0
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    DisableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    DisableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    DisableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif
    HAL_UartInterruptHandle(uartHandle->instance);
#if 0
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
    NVIC_SetPriority((IRQn_Type)s_LpuartRxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartRxIRQ[uartHandle->instance]);
    NVIC_SetPriority((IRQn_Type)s_LpuartTxIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartTxIRQ[uartHandle->instance]);
#else
    NVIC_SetPriority((IRQn_Type)s_LpuartIRQ[uartHandle->instance], HAL_UART_ISR_PRIORITY);
    EnableIRQ(s_LpuartIRQ[uartHandle->instance]);
#endif
#endif
}

#if defined(FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1) && FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART0_LPUART1_RX_IRQHandler(void)
{
    if ((s_UartState[0]))
    {
        if ((LPUART_STAT_OR_MASK & LPUART0->STAT) ||
            ((LPUART_STAT_RDRF_MASK & LPUART0->STAT) && (LPUART_CTRL_RIE_MASK & LPUART0->CTRL)))
        {
            HAL_UartInterruptHandle(0);
        }
    }
    if ((s_UartState[1]))
    {
        if ((LPUART_STAT_OR_MASK & LPUART1->STAT) ||
            ((LPUART_STAT_RDRF_MASK & LPUART1->STAT) && (LPUART_CTRL_RIE_MASK & LPUART1->CTRL)))
        {
            HAL_UartInterruptHandle(1);
        }
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART0_LPUART1_TX_IRQHandler(void)
{
    if ((s_UartState[0]))
    {
        if ((LPUART_STAT_OR_MASK & LPUART0->STAT) ||
            ((LPUART0->STAT & LPUART_STAT_TDRE_MASK) && (LPUART0->CTRL & LPUART_CTRL_TIE_MASK)))
        {
            HAL_UartInterruptHandle(0);
        }
    }
    if ((s_UartState[1]))
    {
        if ((LPUART_STAT_OR_MASK & LPUART1->STAT) ||
            ((LPUART1->STAT & LPUART_STAT_TDRE_MASK) && (LPUART1->CTRL & LPUART_CTRL_TIE_MASK)))
        {
            HAL_UartInterruptHandle(1);
        }
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART0_LPUART1_IRQHandler(void);
void LPUART0_LPUART1_IRQHandler(void)
{
    uint32_t orMask;
    uint32_t rdrfMask;
    uint32_t rieMask;
    uint32_t tdreMask;
    uint32_t tieMask;
    if ((s_UartState[0]) != NULL)
    {
        orMask   = LPUART_STAT_OR_MASK & LPUART0->STAT;
        rdrfMask = LPUART_STAT_RDRF_MASK & LPUART0->STAT;
        rieMask  = LPUART_CTRL_RIE_MASK & LPUART0->CTRL;
        tdreMask = LPUART0->STAT & LPUART_STAT_TDRE_MASK;
        tieMask  = LPUART0->CTRL & LPUART_CTRL_TIE_MASK;
        if ((bool)orMask || ((bool)rdrfMask && (bool)rieMask) || ((bool)tdreMask && (bool)tieMask))
        {
            HAL_UartInterruptHandle(0);
        }
    }
    if ((s_UartState[1]) != NULL)
    {
        orMask   = LPUART_STAT_OR_MASK & LPUART1->STAT;
        rdrfMask = LPUART_STAT_RDRF_MASK & LPUART1->STAT;
        rieMask  = LPUART_CTRL_RIE_MASK & LPUART1->CTRL;
        tdreMask = LPUART1->STAT & LPUART_STAT_TDRE_MASK;
        tieMask  = LPUART1->CTRL & LPUART_CTRL_TIE_MASK;
        if ((bool)orMask || ((bool)rdrfMask && (bool)rieMask) || ((bool)tdreMask && (bool)tieMask))
        {
            HAL_UartInterruptHandle(1);
        }
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART0)
#if !(defined(FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1) && FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART0_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(0);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART0_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(0);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART0_IRQHandler(void)
{
    HAL_UartInterruptHandle(0);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif
#endif

#if defined(LPUART1)
#if !(defined(FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1) && FSL_FEATURE_LPUART_HAS_SHARED_IRQ0_IRQ1)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART1_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(1);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART1_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(1);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART1_IRQHandler(void)
{
    HAL_UartInterruptHandle(1);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif
#endif

#if defined(LPUART2)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART2_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(2);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART2_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(2);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART2_IRQHandler(void)
{
    HAL_UartInterruptHandle(2);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART3)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART3_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(3);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART3_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(3);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART3_IRQHandler(void)
{
    HAL_UartInterruptHandle(3);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART4)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART4_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(4);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART4_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(4);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART4_IRQHandler(void)
{
    HAL_UartInterruptHandle(4);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART5)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART5_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(5);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART5_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(5);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART5_IRQHandler(void)
{
    HAL_UartInterruptHandle(5);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART6)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART6_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(6);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART6_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(6);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART6_IRQHandler(void)
{
    HAL_UartInterruptHandle(6);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART7)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART7_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(7);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART7_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(7);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART7_IRQHandler(void)
{
    HAL_UartInterruptHandle(7);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(LPUART8)
#if defined(FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ) && FSL_FEATURE_LPUART_HAS_SEPARATE_RX_TX_IRQ
void LPUART8_TX_IRQHandler(void)
{
    HAL_UartInterruptHandle(8);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
void LPUART8_RX_IRQHandler(void)
{
    HAL_UartInterruptHandle(8);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#else
void LPUART8_IRQHandler(void)
{
    HAL_UartInterruptHandle(8);
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
#endif

#if defined(CM4_0__LPUART)
void M4_0_LPUART_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(CM4_0__LPUART));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CM4_1__LPUART)
void M4_1_LPUART_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(CM4_1__LPUART));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(CM4__LPUART)
void M4_LPUART_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(CM4__LPUART));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART0)
void DMA_UART0_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART0));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART1)
void DMA_UART1_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART1));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART2)
void DMA_UART2_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART2));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART3)
void DMA_UART3_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART3));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(DMA__LPUART4)
void DMA_UART4_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(DMA__LPUART4));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__LPUART0)
void ADMA_UART0_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(ADMA__LPUART0));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__LPUART1)
void ADMA_UART1_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(ADMA__LPUART1));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__LPUART2)
void ADMA_UART2_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(ADMA__LPUART2));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(ADMA__LPUART3)
void ADMA_UART3_INT_IRQHandler(void)
{
    HAL_UartInterruptHandle(LPUART_GetInstance(ADMA__LPUART3));
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate
  overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#endif

#endif
