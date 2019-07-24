/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 * Copyright (c) 2016, Freescale Semiconductor, Inc. Not a Contribution.
 * Copyright 2016-2017 NXP. Not a Contribution.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fsl_uart_cmsis.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.iuart_cmsis"
#endif

#if ((RTE_USART1 && defined(UART1)) || (RTE_USART2 && defined(UART2)) || (RTE_USART3 && defined(UART3)) || \
     (RTE_USART4 && defined(UART4)))

#define ARM_UART_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

typedef const struct _cmsis_uart_resource
{
    UART_Type *base;           /*!< UART peripheral base address.        */
    uint32_t (*GetFreq)(void); /*!< Function to get the clock frequency. */
} cmsis_uart_resource_t;

typedef struct _cmsis_uart_non_blocking_driver_state
{
    cmsis_uart_resource_t *resource;  /*!< Basic UART resource.      */
    uart_handle_t *handle;            /*!< Interupt transfer handle. */
    ARM_USART_SignalEvent_t cb_event; /*!< Callback function.        */
    uint8_t flags;                    /*!< Control and state flags.  */
} cmsis_uart_non_blocking_driver_state_t;
#if (defined(FSL_FEATURE_SOC_SDMA_COUNT) && FSL_FEATURE_SOC_SDMA_COUNT)
typedef struct _cmsis_uart_sdma_resource
{
    SDMAARM_Type *txSdmaBase; /*!< SDMA peripheral base address for TX. */
    uint32_t txSdmaChannel;   /*!< SDMA channel for UART TX.            */
    uint32_t txSdmaRequest;   /*!< TX SDMA request source.              */
    uint32_t txSdmaPriority;  /*!< TX SDMA channel priority.            */

    SDMAARM_Type *rxSdmaBase; /*!< SDMA peripheral base address for RX. */
    uint32_t rxSdmaChannel;   /*!< SDMA channel for UART RX.            */
    uint32_t rxSdmaRequest;   /*!< RX SDMA request source.              */
    uint32_t rxSdmaPriority;  /*!< RX SDMA channel priority.            */
} cmsis_uart_sdma_resource_t;

typedef struct _cmsis_uart_sdma_driver_state
{
    cmsis_uart_resource_t *resource;          /*!< UART basic resource.       */
    cmsis_uart_sdma_resource_t *sdmaResource; /*!< UART SDMA resource.        */
    uart_sdma_handle_t *handle;               /*!< UART SDMA transfer handle. */
    sdma_handle_t *rxHandle;                  /*!< SDMA RX handle.            */
    sdma_handle_t *txHandle;                  /*!< SDMA TX handle.            */
    ARM_USART_SignalEvent_t cb_event;         /*!< Callback function.         */
    uint8_t flags;                            /*!< Control and state flags.   */
} cmsis_uart_sdma_driver_state_t;
#endif

enum _uart_transfer_states
{
    kUART_TxIdle, /*!< TX idle. */
    kUART_TxBusy, /*!< TX busy. */
    kUART_RxIdle, /*!< RX idle. */
    kUART_RxBusy  /*!< RX busy. */
};

/* Driver Version */
static const ARM_DRIVER_VERSION s_uartDriverVersion = {ARM_USART_API_VERSION, ARM_UART_DRV_VERSION};

static const ARM_USART_CAPABILITIES s_uartDriverCapabilities = {
    1, /* supports UART (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports UART Single-wire mode */
    0, /* supports UART IrDA mode */
    0, /* supports UART Smart Card mode */
    0, /* Smart Card Clock generator */
    0, /* RTS Flow Control available */
    0, /* CTS Flow Control available */
    0, /* Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE */
    0, /* Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USART_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USART_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USART_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USART_EVENT_RI */
};

/*
 * Common control function used by UART_NonBlockingControl/UART_DmaControl
 */
static int32_t UART_CommonControl(uint32_t control,
                                  uint32_t arg,
                                  cmsis_uart_resource_t *resource,
                                  uint8_t *isConfigured)
{
    uart_config_t config;

    UART_GetDefaultConfig(&config);

    switch (control & ARM_USART_CONTROL_Msk)
    {
        case ARM_USART_MODE_ASYNCHRONOUS:
            /* USART Baudrate */
            config.baudRate_Bps = arg;
            break;

        /* TX/RX IO is controlled in application layer. */
        case ARM_USART_CONTROL_TX:
            if (arg)
            {
                UART_EnableTx(resource->base, true);
            }
            else
            {
                UART_EnableTx(resource->base, false);
            }
            return ARM_DRIVER_OK;

        case ARM_USART_CONTROL_RX:
            if (arg)
            {
                UART_EnableRx(resource->base, true);
            }
            else
            {
                UART_EnableRx(resource->base, false);
            }

            return ARM_DRIVER_OK;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_USART_DATA_BITS_Msk)
    {
        case ARM_USART_DATA_BITS_7:
            config.dataBitsCount = kUART_SevenDataBits;
            break;

        case ARM_USART_DATA_BITS_8:
            config.dataBitsCount = kUART_EightDataBits;
            break;

        default:
            return ARM_USART_ERROR_DATA_BITS;
    }

    switch (control & ARM_USART_PARITY_Msk)
    {
        case ARM_USART_PARITY_NONE:
            config.parityMode = kUART_ParityDisabled;
            break;
        case ARM_USART_PARITY_EVEN:
            config.parityMode = kUART_ParityEven;
            break;
        case ARM_USART_PARITY_ODD:
            config.parityMode = kUART_ParityOdd;
            break;
        default:
            return ARM_USART_ERROR_PARITY;
    }

    switch (control & ARM_USART_STOP_BITS_Msk)
    {
        case ARM_USART_STOP_BITS_1:
            /* The GetDefaultConfig has already set for this case. */
            break;
        case ARM_USART_STOP_BITS_2:
            config.stopBitCount = kUART_TwoStopBit;
            break;
        default:
            return ARM_USART_ERROR_STOP_BITS;
    }

    /* If UART is already configured, deinit it first. */
    if ((*isConfigured) & USART_FLAG_CONFIGURED)
    {
        UART_Deinit(resource->base);
        *isConfigured &= ~USART_FLAG_CONFIGURED;
    }

    config.enableTx = true;
    config.enableRx = true;

    if (kStatus_UART_BaudrateNotSupport == UART_Init(resource->base, &config, resource->GetFreq()))
    {
        return ARM_USART_ERROR_BAUDRATE;
    }

    *isConfigured |= USART_FLAG_CONFIGURED;

    return ARM_DRIVER_OK;
}

static ARM_DRIVER_VERSION UARTx_GetVersion(void)
{
    return s_uartDriverVersion;
}

static ARM_USART_CAPABILITIES UARTx_GetCapabilities(void)
{
    return s_uartDriverCapabilities;
}

static int32_t UARTx_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS UARTx_GetModemStatus(void)
{
    ARM_USART_MODEM_STATUS modem_status;

    modem_status.cts      = 0U;
    modem_status.dsr      = 0U;
    modem_status.ri       = 0U;
    modem_status.dcd      = 0U;
    modem_status.reserved = 0U;

    return modem_status;
}

#endif

#if ((RTE_USART1_DMA_EN && defined(UART1)) || (RTE_USART2_DMA_EN && defined(UART2)) || \
     (RTE_USART3_DMA_EN && defined(UART3)) || (RTE_USART4_DMA_EN && defined(UART4)) || \
     (RTE_USART5_DMA_EN && defined(UART5)))

void KSDK_UART_SdmaCallback(UART_Type *base, uart_sdma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_UART_TxIdle == status)
    {
        event = ARM_USART_EVENT_SEND_COMPLETE;
    }
    else if (kStatus_UART_RxIdle == status)
    {
        event = ARM_USART_EVENT_RECEIVE_COMPLETE;
    }

    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_USART_SignalEvent_t)userData)(event);
    }
}

static int32_t UART_SdmaInitialize(ARM_USART_SignalEvent_t cb_event, cmsis_uart_sdma_driver_state_t *uart)
{
    if (!(uart->flags & USART_FLAG_INIT))
    {
        uart->cb_event = cb_event;
        uart->flags    = USART_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t UART_SdmaUninitialize(cmsis_uart_sdma_driver_state_t *uart)
{
    uart->flags = USART_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t UART_SdmaPowerControl(ARM_POWER_STATE state,
                                     cmsis_uart_sdma_driver_state_t *uart,
                                     sdma_context_data_t *rxContext,
                                     sdma_context_data_t *txContext)
{
    status_t status;
    uart_config_t config;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (uart->flags & USART_FLAG_POWER)
            {
                UART_Deinit(uart->resource->base);
                uart->flags = USART_FLAG_INIT;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            /* Must be initialized first. */
            if (uart->flags == USART_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (uart->flags & USART_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            UART_GetDefaultConfig(&config);
            config.enableTx = true;
            config.enableRx = true;

            /* Set up DMA setting. */
            SDMA_CreateHandle(uart->txHandle, uart->sdmaResource->txSdmaBase, uart->sdmaResource->txSdmaChannel,
                              txContext);
            SDMA_CreateHandle(uart->rxHandle, uart->sdmaResource->rxSdmaBase, uart->sdmaResource->rxSdmaChannel,
                              rxContext);
            SDMA_SetChannelPriority(uart->sdmaResource->txSdmaBase, uart->sdmaResource->txSdmaChannel,
                                    uart->sdmaResource->txSdmaPriority);
            SDMA_SetChannelPriority(uart->sdmaResource->rxSdmaBase, uart->sdmaResource->rxSdmaChannel,
                                    uart->sdmaResource->rxSdmaPriority);
            /* Setup the UART. */
            status = UART_Init(uart->resource->base, &config, uart->resource->GetFreq());
            if (kStatus_Success != status)
            {
                return kStatus_Fail;
            }

            /* Create UART SDMA handle. */
            UART_TransferCreateHandleSDMA(uart->resource->base, uart->handle, KSDK_UART_SdmaCallback,
                                          (void *)uart->cb_event, uart->txHandle, uart->rxHandle,
                                          uart->sdmaResource->txSdmaRequest, uart->sdmaResource->rxSdmaRequest);

            uart->flags |= (USART_FLAG_POWER | USART_FLAG_CONFIGURED);
            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t UART_SdmaSend(const void *data, uint32_t num, cmsis_uart_sdma_driver_state_t *uart)
{
    int32_t ret;
    status_t status;
    uart_transfer_t xfer;

    xfer.data     = (uint8_t *)data;
    xfer.dataSize = num;

    status = UART_SendSDMA(uart->resource->base, uart->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_UART_RxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t UART_SdmaReceive(void *data, uint32_t num, cmsis_uart_sdma_driver_state_t *uart)
{
    int32_t ret;
    status_t status;
    uart_transfer_t xfer;

    xfer.data     = data;
    xfer.dataSize = num;

    status = UART_ReceiveSDMA(uart->resource->base, uart->handle, &xfer);
    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_UART_TxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t UART_SdmaTransfer(const void *data_out,
                                 void *data_in,
                                 uint32_t num,
                                 cmsis_uart_sdma_driver_state_t *uart)
{
    /* Only in synchronous mode */
    return ARM_DRIVER_ERROR;
}

static uint32_t UART_SdmaGetTxCount(cmsis_uart_sdma_driver_state_t *uart)
{
    /* Not supported by current driver. */
    return 0;
}

static uint32_t UART_SdmaGetRxCount(cmsis_uart_sdma_driver_state_t *uart)
{
    /* Not supported by current driver. */
    return 0;
}

static int32_t UART_SdmaControl(uint32_t control, uint32_t arg, cmsis_uart_sdma_driver_state_t *uart)
{
    /* Must be power on. */
    if (!(uart->flags & USART_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    /* Does not support these features. */
    if (control & (ARM_USART_FLOW_CONTROL_Msk | ARM_USART_CPOL_Msk | ARM_USART_CPHA_Msk))
    {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_USART_CONTROL_Msk)
    {
        /* Abort Send */
        case ARM_USART_ABORT_SEND:
            UART_TransferAbortSendSDMA(uart->resource->base, uart->handle);
            return ARM_DRIVER_OK;

        /* Abort receive */
        case ARM_USART_ABORT_RECEIVE:
            UART_TransferAbortReceiveSDMA(uart->resource->base, uart->handle);
            return ARM_DRIVER_OK;

        default:
            break;
    }

    return UART_CommonControl(control, arg, uart->resource, &uart->flags);
}

static ARM_USART_STATUS UART_SdmaGetStatus(cmsis_uart_sdma_driver_state_t *uart)
{
    ARM_USART_STATUS stat;

    stat.tx_busy = ((kUART_TxBusy == uart->handle->txState) ? (1U) : (0U));
    stat.rx_busy = ((kUART_RxBusy == uart->handle->rxState) ? (1U) : (0U));

    stat.tx_underflow     = 0U;
    stat.rx_overflow      = UART_GetStatusFlag(uart->resource->base, kUART_RxOverrunFlag);
    stat.rx_break         = UART_GetStatusFlag(uart->resource->base, kUART_BreakDetectFlag);
    stat.rx_framing_error = UART_GetStatusFlag(uart->resource->base, kUART_FrameErrorFlag);
    stat.rx_parity_error  = UART_GetStatusFlag(uart->resource->base, kUART_ParityErrorFlag);
    stat.reserved         = 0U;

    return stat;
}
#endif

#if (((RTE_USART1 && !RTE_USART1_DMA_EN) && defined(UART1)) || \
     ((RTE_USART2 && !RTE_USART2_DMA_EN) && defined(UART2)) || \
     ((RTE_USART3 && !RTE_USART3_DMA_EN) && defined(UART3)) || ((RTE_USART4 && !RTE_USART4_DMA_EN) && defined(UART4)))

void KSDK_UART_NonBlockingCallback(UART_Type *base, uart_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_UART_TxIdle == status)
    {
        event = ARM_USART_EVENT_SEND_COMPLETE;
    }
    if (kStatus_UART_RxIdle == status)
    {
        event = ARM_USART_EVENT_RECEIVE_COMPLETE;
    }
    if (kStatus_UART_RxHardwareOverrun == status)
    {
        event = ARM_USART_EVENT_RX_OVERFLOW;
    }

    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_USART_SignalEvent_t)userData)(event);
    }
}

static int32_t UART_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event,
                                          cmsis_uart_non_blocking_driver_state_t *uart)
{
    if (!(uart->flags & USART_FLAG_INIT))
    {
        uart->cb_event = cb_event;
        uart->flags    = USART_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t UART_NonBlockingUninitialize(cmsis_uart_non_blocking_driver_state_t *uart)
{
    uart->flags = USART_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t UART_NonBlockingPowerControl(ARM_POWER_STATE state, cmsis_uart_non_blocking_driver_state_t *uart)
{
    status_t status;
    uart_config_t config;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (uart->flags & USART_FLAG_POWER)
            {
                UART_Deinit(uart->resource->base);
                uart->flags = USART_FLAG_INIT;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            /* Must be initialized first. */
            if (uart->flags == USART_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (uart->flags & USART_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            UART_GetDefaultConfig(&config);
            config.enableTx = true;
            config.enableRx = true;

            status = UART_Init(uart->resource->base, &config, uart->resource->GetFreq());
            if (kStatus_Success != status)
            {
                return kStatus_Fail;
            }
            UART_TransferCreateHandle(uart->resource->base, uart->handle, KSDK_UART_NonBlockingCallback,
                                      (void *)uart->cb_event);
            uart->flags |= (USART_FLAG_POWER | USART_FLAG_CONFIGURED);

            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t UART_NonBlockingSend(const void *data, uint32_t num, cmsis_uart_non_blocking_driver_state_t *uart)
{
    int32_t ret;
    status_t status;
    uart_transfer_t xfer;

    xfer.data     = (uint8_t *)data;
    xfer.dataSize = num;

    status = UART_TransferSendNonBlocking(uart->resource->base, uart->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_UART_RxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t UART_NonBlockingReceive(void *data, uint32_t num, cmsis_uart_non_blocking_driver_state_t *uart)
{
    int32_t ret;
    status_t status;
    uart_transfer_t xfer;

    xfer.data     = data;
    xfer.dataSize = num;

    status = UART_TransferReceiveNonBlocking(uart->resource->base, uart->handle, &xfer, NULL);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_UART_TxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t UART_NonBlockingTransfer(const void *data_out,
                                        void *data_in,
                                        uint32_t num,
                                        cmsis_uart_non_blocking_driver_state_t *uart)
{
    /* Only in synchronous mode */
    return ARM_DRIVER_ERROR;
}

static uint32_t UART_NonBlockingGetTxCount(cmsis_uart_non_blocking_driver_state_t *uart)
{
    uint32_t cnt;

    /* If TX not in progress, then the TX count is txDataSizeAll saved in handle. */
    if (kStatus_NoTransferInProgress == UART_TransferGetSendCount(uart->resource->base, uart->handle, &cnt))
    {
        cnt = uart->handle->txDataSizeAll;
    }

    return cnt;
}

static uint32_t UART_NonBlockingGetRxCount(cmsis_uart_non_blocking_driver_state_t *uart)
{
    uint32_t cnt;

    if (kStatus_NoTransferInProgress == UART_TransferGetReceiveCount(uart->resource->base, uart->handle, &cnt))
    {
        cnt = uart->handle->rxDataSizeAll;
    }

    return cnt;
}

static int32_t UART_NonBlockingControl(uint32_t control, uint32_t arg, cmsis_uart_non_blocking_driver_state_t *uart)
{
    /* Must be power on. */
    if (!(uart->flags & USART_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    /* Does not support these features. */
    if (control & (ARM_USART_FLOW_CONTROL_Msk | ARM_USART_CPOL_Msk | ARM_USART_CPHA_Msk))
    {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_USART_CONTROL_Msk)
    {
        /* Abort Send */
        case ARM_USART_ABORT_SEND:
            UART_TransferAbortSend(uart->resource->base, uart->handle);
            return ARM_DRIVER_OK;

        /* Abort receive */
        case ARM_USART_ABORT_RECEIVE:
            UART_TransferAbortReceive(uart->resource->base, uart->handle);
            return ARM_DRIVER_OK;

        default:
            break;
    }

    return UART_CommonControl(control, arg, uart->resource, &uart->flags);
}

static ARM_USART_STATUS UART_NonBlockingGetStatus(cmsis_uart_non_blocking_driver_state_t *uart)
{
    ARM_USART_STATUS stat;

    stat.tx_busy = ((kUART_TxBusy == uart->handle->txState) ? (1U) : (0U));
    stat.rx_busy = ((kUART_RxBusy == uart->handle->rxState) ? (1U) : (0U));

    stat.tx_underflow     = 0U;
    stat.rx_overflow      = UART_GetStatusFlag(uart->resource->base, kUART_RxOverrunFlag);
    stat.rx_break         = UART_GetStatusFlag(uart->resource->base, kUART_BreakDetectFlag);
    stat.rx_framing_error = UART_GetStatusFlag(uart->resource->base, kUART_FrameErrorFlag);
    stat.rx_parity_error  = UART_GetStatusFlag(uart->resource->base, kUART_ParityErrorFlag);
    stat.reserved         = 0U;

    return stat;
}

#endif

#if defined(UART1) && RTE_USART1

/* User needs to provide the implementation for UART1_GetFreq/InitPins/DeinitPins
 * in the application for enabling according instance.
 */
extern uint32_t UART1_GetFreq(void);
extern void UART1_InitPins(void);
extern void UART1_DeinitPins(void);

cmsis_uart_resource_t UART1_Resource = {UART1, UART1_GetFreq};

#if RTE_USART1_DMA_EN

cmsis_uart_sdma_resource_t UART1_SdmaResource = {
    RTE_USART1_SDMA_TX_DMA_BASE, RTE_USART1_SDMA_TX_CH, RTE_USART1_SDMA_TX_REQUEST, RTE_USART1_SDMA_TX_PRIORITY,
    RTE_USART1_SDMA_RX_DMA_BASE, RTE_USART1_SDMA_RX_CH, RTE_USART1_SDMA_RX_REQUEST, RTE_USART1_SDMA_RX_PRIORITY,
};

AT_NONCACHEABLE_SECTION_ALIGN(uart_sdma_handle_t UART1_SdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t UART1_SdmaTxHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t UART1_SdmaRxHandle, 4);

AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t UART1_SdmaTxContext, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t UART1_SdmaRxContext, 4);

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart1_sdma_driver_state")
cmsis_uart_sdma_driver_state_t UART1_SdmaDriverState = {
#else
cmsis_uart_sdma_driver_state_t UART1_SdmaDriverState = {
#endif
    &UART1_Resource, &UART1_SdmaResource, &UART1_SdmaHandle, &UART1_SdmaRxHandle, &UART1_SdmaTxHandle,
};

static int32_t UART1_SdmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART1_InitPins();
    return UART_SdmaInitialize(cb_event, &UART1_SdmaDriverState);
}

static int32_t UART1_SdmaUninitialize(void)
{
    UART1_DeinitPins();
    return UART_SdmaUninitialize(&UART1_SdmaDriverState);
}

static int32_t UART1_SdmaPowerControl(ARM_POWER_STATE state)
{
    return UART_SdmaPowerControl(state, &UART1_SdmaDriverState, &UART1_SdmaRxContext, &UART1_SdmaTxContext);
}

static int32_t UART1_SdmaSend(const void *data, uint32_t num)
{
    return UART_SdmaSend(data, num, &UART1_SdmaDriverState);
}

static int32_t UART1_SdmaReceive(void *data, uint32_t num)
{
    return UART_SdmaReceive(data, num, &UART1_SdmaDriverState);
}

static int32_t UART1_SdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return UART_SdmaTransfer(data_out, data_in, num, &UART1_SdmaDriverState);
}

static uint32_t UART1_SdmaGetTxCount(void)
{
    return UART_SdmaGetTxCount(&UART1_SdmaDriverState);
}

static uint32_t UART1_SdmaGetRxCount(void)
{
    return UART_SdmaGetRxCount(&UART1_SdmaDriverState);
}

static int32_t UART1_SdmaControl(uint32_t control, uint32_t arg)
{
    return UART_SdmaControl(control, arg, &UART1_SdmaDriverState);
}

static ARM_USART_STATUS UART1_SdmaGetStatus(void)
{
    return UART_SdmaGetStatus(&UART1_SdmaDriverState);
}

#else

uart_handle_t UART1_Handle;

#if defined(USART1_RX_BUFFER_ENABLE) && (USART1_RX_BUFFER_ENABLE == 1)
static uint8_t uart1_rxRingBuffer[USART_RX_BUFFER_LEN];
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart1_non_blocking_driver_state")
cmsis_uart_non_blocking_driver_state_t UART1_NonBlockingDriverState = {
#else
cmsis_uart_non_blocking_driver_state_t UART1_NonBlockingDriverState = {
#endif
    &UART1_Resource,
    &UART1_Handle,
};

static int32_t UART1_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART1_InitPins();
    return UART_NonBlockingInitialize(cb_event, &UART1_NonBlockingDriverState);
}

static int32_t UART1_NonBlockingUninitialize(void)
{
    UART1_DeinitPins();
    return UART_NonBlockingUninitialize(&UART1_NonBlockingDriverState);
}

static int32_t UART1_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    uint32_t result;

    result = UART_NonBlockingPowerControl(state, &UART1_NonBlockingDriverState);

#if defined(USART1_RX_BUFFER_ENABLE) && (USART1_RX_BUFFER_ENABLE == 1)
    if ((state == ARM_POWER_FULL) && (UART1_NonBlockingDriverState.handle->rxRingBuffer == NULL))
    {
        UART_TransferStartRingBuffer(UART1_NonBlockingDriverState.resource->base, UART1_NonBlockingDriverState.handle,
                                     uart1_rxRingBuffer, USART_RX_BUFFER_LEN);
    }
#endif

    return result;
}

static int32_t UART1_NonBlockingSend(const void *data, uint32_t num)
{
    return UART_NonBlockingSend(data, num, &UART1_NonBlockingDriverState);
}

static int32_t UART1_NonBlockingReceive(void *data, uint32_t num)
{
    return UART_NonBlockingReceive(data, num, &UART1_NonBlockingDriverState);
}

static int32_t UART1_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return UART_NonBlockingTransfer(data_out, data_in, num, &UART1_NonBlockingDriverState);
}

static uint32_t UART1_NonBlockingGetTxCount(void)
{
    return UART_NonBlockingGetTxCount(&UART1_NonBlockingDriverState);
}

static uint32_t UART1_NonBlockingGetRxCount(void)
{
    return UART_NonBlockingGetRxCount(&UART1_NonBlockingDriverState);
}

static int32_t UART1_NonBlockingControl(uint32_t control, uint32_t arg)
{
    int32_t result;

    result = UART_NonBlockingControl(control, arg, &UART1_NonBlockingDriverState);
    if (ARM_DRIVER_OK != result)
    {
        return result;
    }
    /* Enable the receive interrupts if ring buffer is used */
    if (UART1_NonBlockingDriverState.handle->rxRingBuffer != NULL)
    {
        UART_EnableInterrupts(UART1_NonBlockingDriverState.resource->base,
                              (kUART_RxReadyEnable | kUART_AgingTimerEnable | kUART_RxOverrunEnable |
                               kUART_ParityErrorEnable | kUART_FrameErrorEnable));
    }

    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS UART1_NonBlockingGetStatus(void)
{
    return UART_NonBlockingGetStatus(&UART1_NonBlockingDriverState);
}
#endif

ARM_DRIVER_USART Driver_USART1 = {
    UARTx_GetVersion,      UARTx_GetCapabilities,
#if RTE_USART1_DMA_EN
    UART1_SdmaInitialize,  UART1_SdmaUninitialize, UART1_SdmaPowerControl, UART1_SdmaSend,    UART1_SdmaReceive,
    UART1_SdmaTransfer,    UART1_SdmaGetTxCount,   UART1_SdmaGetRxCount,   UART1_SdmaControl, UART1_SdmaGetStatus,
#else
    UART1_NonBlockingInitialize,
    UART1_NonBlockingUninitialize,
    UART1_NonBlockingPowerControl,
    UART1_NonBlockingSend,
    UART1_NonBlockingReceive,
    UART1_NonBlockingTransfer,
    UART1_NonBlockingGetTxCount,
    UART1_NonBlockingGetRxCount,
    UART1_NonBlockingControl,
    UART1_NonBlockingGetStatus,
#endif
    UARTx_SetModemControl, UARTx_GetModemStatus};
#endif /* UART1 */

#if defined(UART2) && RTE_USART2

/* User needs to provide the implementation for UART2_GetFreq/InitPins/DeinitPins
 * in the application for enabling according instance.
 */
extern uint32_t UART2_GetFreq(void);
extern void UART2_InitPins(void);
extern void UART2_DeinitPins(void);

cmsis_uart_resource_t UART2_Resource = {UART2, UART2_GetFreq};

#if RTE_USART2_DMA_EN

cmsis_uart_sdma_resource_t UART2_SdmaResource = {
    RTE_USART2_SDMA_TX_DMA_BASE, RTE_USART2_SDMA_TX_CH, RTE_USART2_SDMA_TX_REQUEST, RTE_USART2_SDMA_TX_PRIORITY,
    RTE_USART2_SDMA_RX_DMA_BASE, RTE_USART2_SDMA_RX_CH, RTE_USART2_SDMA_RX_REQUEST, RTE_USART2_SDMA_RX_PRIORITY,
};

AT_NONCACHEABLE_SECTION_ALIGN(uart_sdma_handle_t UART2_SdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t UART2_SdmaTxHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t UART2_SdmaRxHandle, 4);

AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t UART2_SdmaTxContext, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t UART2_SdmaRxContext, 4);

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart2_sdma_driver_state")
cmsis_uart_sdma_driver_state_t UART2_SdmaDriverState = {
#else
cmsis_uart_sdma_driver_state_t UART2_SdmaDriverState = {
#endif
    &UART2_Resource, &UART2_SdmaResource, &UART2_SdmaHandle, &UART2_SdmaRxHandle, &UART2_SdmaTxHandle,
};

static int32_t UART2_SdmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART2_InitPins();
    return UART_SdmaInitialize(cb_event, &UART2_SdmaDriverState);
}

static int32_t UART2_SdmaUninitialize(void)
{
    UART2_DeinitPins();
    return UART_SdmaUninitialize(&UART2_SdmaDriverState);
}

static int32_t UART2_SdmaPowerControl(ARM_POWER_STATE state)
{
    return UART_SdmaPowerControl(state, &UART2_SdmaDriverState, &UART2_SdmaRxContext, &UART2_SdmaTxContext);
}

static int32_t UART2_SdmaSend(const void *data, uint32_t num)
{
    return UART_SdmaSend(data, num, &UART2_SdmaDriverState);
}

static int32_t UART2_SdmaReceive(void *data, uint32_t num)
{
    return UART_SdmaReceive(data, num, &UART2_SdmaDriverState);
}

static int32_t UART2_SdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return UART_SdmaTransfer(data_out, data_in, num, &UART2_SdmaDriverState);
}

static uint32_t UART2_SdmaGetTxCount(void)
{
    return UART_SdmaGetTxCount(&UART2_SdmaDriverState);
}

static uint32_t UART2_SdmaGetRxCount(void)
{
    return UART_SdmaGetRxCount(&UART2_SdmaDriverState);
}

static int32_t UART2_SdmaControl(uint32_t control, uint32_t arg)
{
    return UART_SdmaControl(control, arg, &UART2_SdmaDriverState);
}

static ARM_USART_STATUS UART2_SdmaGetStatus(void)
{
    return UART_SdmaGetStatus(&UART2_SdmaDriverState);
}

#else

uart_handle_t UART2_Handle;

#if defined(USART2_RX_BUFFER_ENABLE) && (USART2_RX_BUFFER_ENABLE == 1)
static uint8_t uart2_rxRingBuffer[USART_RX_BUFFER_LEN];
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart2_non_blocking_driver_state")
cmsis_uart_non_blocking_driver_state_t UART2_NonBlockingDriverState = {
#else
cmsis_uart_non_blocking_driver_state_t UART2_NonBlockingDriverState = {
#endif
    &UART2_Resource,
    &UART2_Handle,
};

static int32_t UART2_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART2_InitPins();
    return UART_NonBlockingInitialize(cb_event, &UART2_NonBlockingDriverState);
}

static int32_t UART2_NonBlockingUninitialize(void)
{
    UART2_DeinitPins();
    return UART_NonBlockingUninitialize(&UART2_NonBlockingDriverState);
}

static int32_t UART2_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    uint32_t result;

    result = UART_NonBlockingPowerControl(state, &UART2_NonBlockingDriverState);

#if defined(USART2_RX_BUFFER_ENABLE) && (USART2_RX_BUFFER_ENABLE == 1)
    if ((state == ARM_POWER_FULL) && (UART2_NonBlockingDriverState.handle->rxRingBuffer == NULL))
    {
        UART_TransferStartRingBuffer(UART2_NonBlockingDriverState.resource->base, UART2_NonBlockingDriverState.handle,
                                     uart2_rxRingBuffer, USART_RX_BUFFER_LEN);
    }
#endif

    return result;
}

static int32_t UART2_NonBlockingSend(const void *data, uint32_t num)
{
    return UART_NonBlockingSend(data, num, &UART2_NonBlockingDriverState);
}

static int32_t UART2_NonBlockingReceive(void *data, uint32_t num)
{
    return UART_NonBlockingReceive(data, num, &UART2_NonBlockingDriverState);
}

static int32_t UART2_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return UART_NonBlockingTransfer(data_out, data_in, num, &UART2_NonBlockingDriverState);
}

static uint32_t UART2_NonBlockingGetTxCount(void)
{
    return UART_NonBlockingGetTxCount(&UART2_NonBlockingDriverState);
}

static uint32_t UART2_NonBlockingGetRxCount(void)
{
    return UART_NonBlockingGetRxCount(&UART2_NonBlockingDriverState);
}

static int32_t UART2_NonBlockingControl(uint32_t control, uint32_t arg)
{
    int32_t result;

    result = UART_NonBlockingControl(control, arg, &UART2_NonBlockingDriverState);
    if (ARM_DRIVER_OK != result)
    {
        return result;
    }
    /* Enable the receive interrupts if ring buffer is used */
    if (UART2_NonBlockingDriverState.handle->rxRingBuffer != NULL)
    {
        UART_EnableInterrupts(UART2_NonBlockingDriverState.resource->base,
                              (kUART_RxReadyEnable | kUART_AgingTimerEnable | kUART_RxOverrunEnable |
                               kUART_ParityErrorEnable | kUART_FrameErrorEnable));
    }

    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS UART2_NonBlockingGetStatus(void)
{
    return UART_NonBlockingGetStatus(&UART2_NonBlockingDriverState);
}
#endif

ARM_DRIVER_USART Driver_USART2 = {
    UARTx_GetVersion,      UARTx_GetCapabilities,
#if RTE_USART2_DMA_EN
    UART2_SdmaInitialize,  UART2_SdmaUninitialize, UART2_SdmaPowerControl, UART2_SdmaSend,    UART2_SdmaReceive,
    UART2_SdmaTransfer,    UART2_SdmaGetTxCount,   UART2_SdmaGetRxCount,   UART2_SdmaControl, UART2_SdmaGetStatus,
#else
    UART2_NonBlockingInitialize,
    UART2_NonBlockingUninitialize,
    UART2_NonBlockingPowerControl,
    UART2_NonBlockingSend,
    UART2_NonBlockingReceive,
    UART2_NonBlockingTransfer,
    UART2_NonBlockingGetTxCount,
    UART2_NonBlockingGetRxCount,
    UART2_NonBlockingControl,
    UART2_NonBlockingGetStatus,
#endif
    UARTx_SetModemControl, UARTx_GetModemStatus};
#endif /* UART2 */

#if defined(UART3) && RTE_USART3

/* User needs to provide the implementation for UART3_GetFreq/InitPins/DeinitPins
 * in the application for enabling according instance.
 */
extern uint32_t UART3_GetFreq(void);
extern void UART3_InitPins(void);
extern void UART3_DeinitPins(void);

cmsis_uart_resource_t UART3_Resource = {UART3, UART3_GetFreq};

#if RTE_USART3_DMA_EN

cmsis_uart_sdma_resource_t UART3_SdmaResource = {
    RTE_USART3_SDMA_TX_DMA_BASE, RTE_USART3_SDMA_TX_CH, RTE_USART3_SDMA_TX_REQUEST, RTE_USART3_SDMA_TX_PRIORITY,
    RTE_USART3_SDMA_RX_DMA_BASE, RTE_USART3_SDMA_RX_CH, RTE_USART3_SDMA_RX_REQUEST, RTE_USART3_SDMA_RX_PRIORITY,
};

AT_NONCACHEABLE_SECTION_ALIGN(uart_sdma_handle_t UART3_SdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t UART3_SdmaTxHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t UART3_SdmaRxHandle, 4);

AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t UART3_SdmaTxContext, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t UART3_SdmaRxContext, 4);

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart3_sdma_driver_state")
cmsis_uart_sdma_driver_state_t UART3_SdmaDriverState = {
#else
cmsis_uart_sdma_driver_state_t UART3_SdmaDriverState = {
#endif
    &UART3_Resource, &UART3_SdmaResource, &UART3_SdmaHandle, &UART3_SdmaRxHandle, &UART3_SdmaTxHandle,
};

static int32_t UART3_SdmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART3_InitPins();
    return UART_SdmaInitialize(cb_event, &UART3_SdmaDriverState);
}

static int32_t UART3_SdmaUninitialize(void)
{
    UART3_DeinitPins();
    return UART_SdmaUninitialize(&UART3_SdmaDriverState);
}

static int32_t UART3_SdmaPowerControl(ARM_POWER_STATE state)
{
    return UART_SdmaPowerControl(state, &UART3_SdmaDriverState, &UART3_SdmaRxContext, &UART3_SdmaTxContext);
}

static int32_t UART3_SdmaSend(const void *data, uint32_t num)
{
    return UART_SdmaSend(data, num, &UART3_SdmaDriverState);
}

static int32_t UART3_SdmaReceive(void *data, uint32_t num)
{
    return UART_SdmaReceive(data, num, &UART3_SdmaDriverState);
}

static int32_t UART3_SdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return UART_SdmaTransfer(data_out, data_in, num, &UART3_SdmaDriverState);
}

static uint32_t UART3_SdmaGetTxCount(void)
{
    return UART_SdmaGetTxCount(&UART3_SdmaDriverState);
}

static uint32_t UART3_SdmaGetRxCount(void)
{
    return UART_SdmaGetRxCount(&UART3_SdmaDriverState);
}

static int32_t UART3_SdmaControl(uint32_t control, uint32_t arg)
{
    return UART_SdmaControl(control, arg, &UART3_SdmaDriverState);
}

static ARM_USART_STATUS UART3_SdmaGetStatus(void)
{
    return UART_SdmaGetStatus(&UART3_SdmaDriverState);
}

#else

uart_handle_t UART3_Handle;

#if defined(USART3_RX_BUFFER_ENABLE) && (USART3_RX_BUFFER_ENABLE == 1)
static uint8_t uart3_rxRingBuffer[USART_RX_BUFFER_LEN];
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart3_non_blocking_driver_state")
cmsis_uart_non_blocking_driver_state_t UART3_NonBlockingDriverState = {
#else
cmsis_uart_non_blocking_driver_state_t UART3_NonBlockingDriverState = {
#endif
    &UART3_Resource,
    &UART3_Handle,
};

static int32_t UART3_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART3_InitPins();
    return UART_NonBlockingInitialize(cb_event, &UART3_NonBlockingDriverState);
}

static int32_t UART3_NonBlockingUninitialize(void)
{
    UART3_DeinitPins();
    return UART_NonBlockingUninitialize(&UART3_NonBlockingDriverState);
}

static int32_t UART3_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    uint32_t result;

    result = UART_NonBlockingPowerControl(state, &UART3_NonBlockingDriverState);

#if defined(USART3_RX_BUFFER_ENABLE) && (USART3_RX_BUFFER_ENABLE == 1)
    if ((state == ARM_POWER_FULL) && (UART3_NonBlockingDriverState.handle->rxRingBuffer == NULL))
    {
        UART_TransferStartRingBuffer(UART3_NonBlockingDriverState.resource->base, UART3_NonBlockingDriverState.handle,
                                     uart3_rxRingBuffer, USART_RX_BUFFER_LEN);
    }
#endif

    return result;
}

static int32_t UART3_NonBlockingSend(const void *data, uint32_t num)
{
    return UART_NonBlockingSend(data, num, &UART3_NonBlockingDriverState);
}

static int32_t UART3_NonBlockingReceive(void *data, uint32_t num)
{
    return UART_NonBlockingReceive(data, num, &UART3_NonBlockingDriverState);
}

static int32_t UART3_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return UART_NonBlockingTransfer(data_out, data_in, num, &UART3_NonBlockingDriverState);
}

static uint32_t UART3_NonBlockingGetTxCount(void)
{
    return UART_NonBlockingGetTxCount(&UART3_NonBlockingDriverState);
}

static uint32_t UART3_NonBlockingGetRxCount(void)
{
    return UART_NonBlockingGetRxCount(&UART3_NonBlockingDriverState);
}

static int32_t UART3_NonBlockingControl(uint32_t control, uint32_t arg)
{
    int32_t result;

    result = UART_NonBlockingControl(control, arg, &UART3_NonBlockingDriverState);
    if (ARM_DRIVER_OK != result)
    {
        return result;
    }
    /* Enable the receive interrupts if ring buffer is used */
    if (UART3_NonBlockingDriverState.handle->rxRingBuffer != NULL)
    {
        UART_EnableInterrupts(UART3_NonBlockingDriverState.resource->base,
                              (kUART_RxReadyEnable | kUART_AgingTimerEnable | kUART_RxOverrunEnable |
                               kUART_ParityErrorEnable | kUART_FrameErrorEnable));
    }

    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS UART3_NonBlockingGetStatus(void)
{
    return UART_NonBlockingGetStatus(&UART3_NonBlockingDriverState);
}
#endif

ARM_DRIVER_USART Driver_USART3 = {
    UARTx_GetVersion,      UARTx_GetCapabilities,
#if RTE_USART3_DMA_EN
    UART3_SdmaInitialize,  UART3_SdmaUninitialize, UART3_SdmaPowerControl, UART3_SdmaSend,    UART3_SdmaReceive,
    UART3_SdmaTransfer,    UART3_SdmaGetTxCount,   UART3_SdmaGetRxCount,   UART3_SdmaControl, UART3_SdmaGetStatus,
#else
    UART3_NonBlockingInitialize,
    UART3_NonBlockingUninitialize,
    UART3_NonBlockingPowerControl,
    UART3_NonBlockingSend,
    UART3_NonBlockingReceive,
    UART3_NonBlockingTransfer,
    UART3_NonBlockingGetTxCount,
    UART3_NonBlockingGetRxCount,
    UART3_NonBlockingControl,
    UART3_NonBlockingGetStatus,
#endif
    UARTx_SetModemControl, UARTx_GetModemStatus};
#endif /* UART3 */

#if defined(UART4) && RTE_USART4

/* User needs to provide the implementation for UART4_GetFreq/InitPins/DeinitPins
 * in the application for enabling according instance.
 */
extern uint32_t UART4_GetFreq(void);
extern void UART4_InitPins(void);
extern void UART4_DeinitPins(void);

cmsis_uart_resource_t UART4_Resource = {UART4, UART4_GetFreq};

#if RTE_USART4_DMA_EN

cmsis_uart_sdma_resource_t UART4_SdmaResource = {
    RTE_USART4_SDMA_TX_DMA_BASE, RTE_USART4_SDMA_TX_CH, RTE_USART4_SDMA_TX_REQUEST, RTE_USART4_SDMA_TX_PRIORITY,
    RTE_USART4_SDMA_RX_DMA_BASE, RTE_USART4_SDMA_RX_CH, RTE_USART4_SDMA_RX_REQUEST, RTE_USART4_SDMA_RX_PRIORITY,
};

AT_NONCACHEABLE_SECTION_ALIGN(uart_sdma_handle_t UART4_SdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t UART4_SdmaTxHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t UART4_SdmaRxHandle, 4);

AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t UART4_SdmaTxContext, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t UART4_SdmaRxContext, 4);

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart4_sdma_driver_state")
cmsis_uart_sdma_driver_state_t UART4_SdmaDriverState = {
#else
cmsis_uart_sdma_driver_state_t UART4_SdmaDriverState = {
#endif
    &UART4_Resource, &UART4_SdmaResource, &UART4_SdmaHandle, &UART4_SdmaRxHandle, &UART4_SdmaTxHandle,
};

static int32_t UART4_SdmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART4_InitPins();
    return UART_SdmaInitialize(cb_event, &UART4_SdmaDriverState);
}

static int32_t UART4_SdmaUninitialize(void)
{
    UART4_DeinitPins();
    return UART_SdmaUninitialize(&UART4_SdmaDriverState);
}

static int32_t UART4_SdmaPowerControl(ARM_POWER_STATE state)
{
    return UART_SdmaPowerControl(state, &UART4_SdmaDriverState, &UART4_SdmaRxContext, &UART4_SdmaTxContext);
}

static int32_t UART4_SdmaSend(const void *data, uint32_t num)
{
    return UART_SdmaSend(data, num, &UART4_SdmaDriverState);
}

static int32_t UART4_SdmaReceive(void *data, uint32_t num)
{
    return UART_SdmaReceive(data, num, &UART4_SdmaDriverState);
}

static int32_t UART4_SdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return UART_SdmaTransfer(data_out, data_in, num, &UART4_SdmaDriverState);
}

static uint32_t UART4_SdmaGetTxCount(void)
{
    return UART_SdmaGetTxCount(&UART4_SdmaDriverState);
}

static uint32_t UART4_SdmaGetRxCount(void)
{
    return UART_SdmaGetRxCount(&UART4_SdmaDriverState);
}

static int32_t UART4_SdmaControl(uint32_t control, uint32_t arg)
{
    return UART_SdmaControl(control, arg, &UART4_SdmaDriverState);
}

static ARM_USART_STATUS UART4_SdmaGetStatus(void)
{
    return UART_SdmaGetStatus(&UART4_SdmaDriverState);
}

#else

uart_handle_t UART4_Handle;

#if defined(USART4_RX_BUFFER_ENABLE) && (USART4_RX_BUFFER_ENABLE == 1)
static uint8_t uart4_rxRingBuffer[USART_RX_BUFFER_LEN];
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart4_non_blocking_driver_state")
cmsis_uart_non_blocking_driver_state_t UART4_NonBlockingDriverState = {
#else
cmsis_uart_non_blocking_driver_state_t UART4_NonBlockingDriverState = {
#endif
    &UART4_Resource,
    &UART4_Handle,
};

static int32_t UART4_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART4_InitPins();
    return UART_NonBlockingInitialize(cb_event, &UART4_NonBlockingDriverState);
}

static int32_t UART4_NonBlockingUninitialize(void)
{
    UART4_DeinitPins();
    return UART_NonBlockingUninitialize(&UART4_NonBlockingDriverState);
}

static int32_t UART4_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    uint32_t result;

    result = UART_NonBlockingPowerControl(state, &UART4_NonBlockingDriverState);

#if defined(USART4_RX_BUFFER_ENABLE) && (USART4_RX_BUFFER_ENABLE == 1)
    if ((state == ARM_POWER_FULL) && (UART4_NonBlockingDriverState.handle->rxRingBuffer == NULL))
    {
        UART_TransferStartRingBuffer(UART4_NonBlockingDriverState.resource->base, UART4_NonBlockingDriverState.handle,
                                     uart4_rxRingBuffer, USART_RX_BUFFER_LEN);
    }
#endif

    return result;
}

static int32_t UART4_NonBlockingSend(const void *data, uint32_t num)
{
    return UART_NonBlockingSend(data, num, &UART4_NonBlockingDriverState);
}

static int32_t UART4_NonBlockingReceive(void *data, uint32_t num)
{
    return UART_NonBlockingReceive(data, num, &UART4_NonBlockingDriverState);
}

static int32_t UART4_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return UART_NonBlockingTransfer(data_out, data_in, num, &UART4_NonBlockingDriverState);
}

static uint32_t UART4_NonBlockingGetTxCount(void)
{
    return UART_NonBlockingGetTxCount(&UART4_NonBlockingDriverState);
}

static uint32_t UART4_NonBlockingGetRxCount(void)
{
    return UART_NonBlockingGetRxCount(&UART4_NonBlockingDriverState);
}

static int32_t UART4_NonBlockingControl(uint32_t control, uint32_t arg)
{
    int32_t result;

    result = UART_NonBlockingControl(control, arg, &UART4_NonBlockingDriverState);
    if (ARM_DRIVER_OK != result)
    {
        return result;
    }
    /* Enable the receive interrupts if ring buffer is used */
    if (UART4_NonBlockingDriverState.handle->rxRingBuffer != NULL)
    {
        UART_EnableInterrupts(UART4_NonBlockingDriverState.resource->base,
                              (kUART_RxReadyEnable | kUART_AgingTimerEnable | kUART_RxOverrunEnable |
                               kUART_ParityErrorEnable | kUART_FrameErrorEnable));
    }

    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS UART4_NonBlockingGetStatus(void)
{
    return UART_NonBlockingGetStatus(&UART4_NonBlockingDriverState);
}
#endif

ARM_DRIVER_USART Driver_USART4 = {
    UARTx_GetVersion,      UARTx_GetCapabilities,
#if RTE_USART4_DMA_EN
    UART4_SdmaInitialize,  UART4_SdmaUninitialize, UART4_SdmaPowerControl, UART4_SdmaSend,    UART4_SdmaReceive,
    UART4_SdmaTransfer,    UART4_SdmaGetTxCount,   UART4_SdmaGetRxCount,   UART4_SdmaControl, UART4_SdmaGetStatus,
#else
    UART4_NonBlockingInitialize,
    UART4_NonBlockingUninitialize,
    UART4_NonBlockingPowerControl,
    UART4_NonBlockingSend,
    UART4_NonBlockingReceive,
    UART4_NonBlockingTransfer,
    UART4_NonBlockingGetTxCount,
    UART4_NonBlockingGetRxCount,
    UART4_NonBlockingControl,
    UART4_NonBlockingGetStatus,
#endif
    UARTx_SetModemControl, UARTx_GetModemStatus};
#endif /* UART4 */
