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

#include "fsl_lpsci_cmsis.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpsci_cmsis"
#endif

#if ((RTE_USART0) && defined(UART0))

#define ARM_LPSCI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

typedef const struct _cmsis_lpsci_resource
{
    UART0_Type *base;          /*!< LPSCI peripheral base address.      */
    uint32_t (*GetFreq)(void); /*!< Function to get the clock frequency. */
} cmsis_lpsci_resource_t;

typedef struct _cmsis_lpsci_interrupt_driver_state
{
    cmsis_lpsci_resource_t *resource; /*!< Basic LPSCI resource. */
    lpsci_handle_t *handle;           /*!< Interupt transfer handle. */
    ARM_USART_SignalEvent_t cb_event; /*!< Callback function.     */
    uint8_t flags;                    /*!< Control and state flags. */
} cmsis_lpsci_interrupt_driver_state_t;

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
typedef const struct _cmsis_lpsci_dma_resource
{
    DMA_Type *txDmaBase;       /*!< DMA peripheral base address for TX.    */
    uint32_t txDmaChannel;     /*!< DMA channel for LPSCI TX.             */
    DMAMUX_Type *txDmamuxBase; /*!< DMAMUX peripheral base address for TX. */
    uint8_t txDmaRequest;      /*!< TX DMA request source.                 */

    DMA_Type *rxDmaBase;       /*!< DMA peripheral base address for RX.    */
    uint32_t rxDmaChannel;     /*!< DMA channel for LPSCI RX.             */
    DMAMUX_Type *rxDmamuxBase; /*!< DMAMUX peripheral base address for RX. */
    uint8_t rxDmaRequest;      /*!< RX DMA request source.                 */
} cmsis_lpsci_dma_resource_t;

typedef struct _cmsis_lpsci_dma_driver_state
{
    cmsis_lpsci_resource_t *resource;        /*!< LPSCI basic resource.      */
    cmsis_lpsci_dma_resource_t *dmaResource; /*!< LPSCI DMA resource.        */
    lpsci_dma_handle_t *handle;              /*!< LPSCI DMA transfer handle. */
    dma_handle_t *rxHandle;                  /*!< DMA RX handle.             */
    dma_handle_t *txHandle;                  /*!< DMA TX handle.             */
    ARM_USART_SignalEvent_t cb_event;        /*!< Callback function.     */
    uint8_t flags;                           /*!< Control and state flags. */
} cmsis_lpsci_dma_driver_state_t;
#endif

enum _lpsci_tansfer_state
{
    kLPSCI_TxIdle, /*!< TX idle. */
    kLPSCI_TxBusy, /*!< TX busy. */
    kLPSCI_RxIdle, /*!< RX idle. */
    kLPSCI_RxBusy  /*!< RX busy. */
};

/* Driver Version */
static const ARM_DRIVER_VERSION s_lpsciDriverVersion = {ARM_USART_API_VERSION, ARM_LPSCI_DRV_VERSION};

static const ARM_USART_CAPABILITIES s_lpsciDriverCapabilities = {
    1, /* supports LPSCI (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports LPSCI Single-wire mode */
    0, /* supports LPSCI IrDA mode */
    0, /* supports LPSCI Smart Card mode */
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
 * Common control function used by LPSCI_NonBlockingControl/LPSCI_DmaControl/LPSCI_EdmaControl
 */
static int32_t LPSCI_CommonControl(uint32_t control,
                                   uint32_t arg,
                                   cmsis_lpsci_resource_t *resource,
                                   uint8_t *isConfigured)
{
    lpsci_config_t config;

    LPSCI_GetDefaultConfig(&config);

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
                LPSCI_EnableTx(resource->base, true);
            }
            else
            {
                LPSCI_EnableTx(resource->base, false);
            }
            return ARM_DRIVER_OK;

        case ARM_USART_CONTROL_RX:
            if (arg)
            {
                LPSCI_EnableRx(resource->base, true);
            }
            else
            {
                LPSCI_EnableRx(resource->base, false);
            }

            return ARM_DRIVER_OK;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_USART_PARITY_Msk)
    {
        case ARM_USART_PARITY_NONE:
            config.parityMode = kLPSCI_ParityDisabled;
            break;

        case ARM_USART_PARITY_EVEN:
            config.parityMode = kLPSCI_ParityEven;
            break;

        case ARM_USART_PARITY_ODD:
            config.parityMode = kLPSCI_ParityOdd;
            break;

        default:
            return ARM_USART_ERROR_PARITY;
    }

    switch (control & ARM_USART_STOP_BITS_Msk)
    {
        case ARM_USART_STOP_BITS_1:
            /* The GetDefaultConfig has already set for this case. */
            break;

#if defined(FSL_FEATURE_LPSCI_HAS_STOP_BIT_CONFIG_SUPPORT) && FSL_FEATURE_LPSCI_HAS_STOP_BIT_CONFIG_SUPPORT
        case ARM_USART_STOP_BITS_2:
            config.stopBitCount = kLPSCI_TwoStopBit;
            break;
#endif
        default:
            return ARM_USART_ERROR_STOP_BITS;
    }

    /* If LPSCI is already configured, deinit it first. */
    if ((*isConfigured) & USART_FLAG_CONFIGURED)
    {
        LPSCI_Deinit(resource->base);
        *isConfigured &= ~USART_FLAG_CONFIGURED;
    }

    config.enableTx = true;
    config.enableRx = true;

    if (kStatus_LPSCI_BaudrateNotSupport == LPSCI_Init(resource->base, &config, resource->GetFreq()))
    {
        return ARM_USART_ERROR_BAUDRATE;
    }

    *isConfigured |= USART_FLAG_CONFIGURED;

    return ARM_DRIVER_OK;
}

static ARM_DRIVER_VERSION LPSCI_GetVersion(void)
{
    return s_lpsciDriverVersion;
}

static ARM_USART_CAPABILITIES LPSCI_GetCapabilities(void)
{
    return s_lpsciDriverCapabilities;
}

static int32_t LPSCI_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS LPSCI_GetModemStatus(void)
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

#if (RTE_USART0_DMA_EN && defined(UART0))

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
void KSDK_LPSCI_DmaCallback(UART0_Type *base, lpsci_dma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_LPSCI_TxIdle == status)
    {
        event = ARM_USART_EVENT_SEND_COMPLETE;
    }
    if (kStatus_LPSCI_RxIdle == status)
    {
        event = ARM_USART_EVENT_RECEIVE_COMPLETE;
    }

    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_USART_SignalEvent_t)userData)(event);
    }
}

static int32_t LPSCI_DmaInitialize(ARM_USART_SignalEvent_t cb_event, cmsis_lpsci_dma_driver_state_t *lpsci)
{
    if (!(lpsci->flags & USART_FLAG_INIT))
    {
        lpsci->cb_event = cb_event;
        lpsci->flags    = USART_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t LPSCI_DmaUninitialize(cmsis_lpsci_dma_driver_state_t *lpsci)
{
    lpsci->flags = USART_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t LPSCI_DmaPowerControl(ARM_POWER_STATE state, cmsis_lpsci_dma_driver_state_t *lpsci)
{
    lpsci_config_t config;
    cmsis_lpsci_dma_resource_t *dmaResource;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (lpsci->flags & USART_FLAG_POWER)
            {
                LPSCI_Deinit(lpsci->resource->base);
                DMAMUX_DisableChannel(lpsci->dmaResource->rxDmamuxBase, lpsci->dmaResource->rxDmaChannel);
                DMAMUX_DisableChannel(lpsci->dmaResource->txDmamuxBase, lpsci->dmaResource->txDmaChannel);
                lpsci->flags = USART_FLAG_INIT;
            }
            break;

        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_POWER_FULL:
            /* Must be initialized first. */
            if (lpsci->flags == USART_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (lpsci->flags & USART_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            LPSCI_GetDefaultConfig(&config);
            config.enableTx = true;
            config.enableRx = true;

            dmaResource = lpsci->dmaResource;

            /* Set up DMA setting. */
            DMA_CreateHandle(lpsci->rxHandle, dmaResource->rxDmaBase, dmaResource->rxDmaChannel);
            DMAMUX_SetSource(dmaResource->rxDmamuxBase, dmaResource->rxDmaChannel, dmaResource->rxDmaRequest);
            DMAMUX_EnableChannel(dmaResource->rxDmamuxBase, dmaResource->rxDmaChannel);

            DMA_CreateHandle(lpsci->txHandle, dmaResource->txDmaBase, dmaResource->txDmaChannel);
            DMAMUX_SetSource(dmaResource->txDmamuxBase, dmaResource->txDmaChannel, dmaResource->txDmaRequest);
            DMAMUX_EnableChannel(dmaResource->txDmamuxBase, dmaResource->txDmaChannel);

            /* Setup the LPSCI. */
            LPSCI_Init(lpsci->resource->base, &config, lpsci->resource->GetFreq());
            LPSCI_TransferCreateHandleDMA(lpsci->resource->base, lpsci->handle, KSDK_LPSCI_DmaCallback,
                                          (void *)lpsci->cb_event, lpsci->txHandle, lpsci->rxHandle);

            lpsci->flags |= (USART_FLAG_POWER | USART_FLAG_CONFIGURED);
            break;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t LPSCI_DmaSend(const void *data, uint32_t num, cmsis_lpsci_dma_driver_state_t *lpsci)
{
    int32_t ret;
    status_t status;
    lpsci_transfer_t xfer;

    xfer.data     = (uint8_t *)data;
    xfer.dataSize = num;

    status = LPSCI_TransferSendDMA(lpsci->resource->base, lpsci->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;

        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;

        case kStatus_LPSCI_RxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;

        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t LPSCI_DmaReceive(void *data, uint32_t num, cmsis_lpsci_dma_driver_state_t *lpsci)
{
    int32_t ret;
    status_t status;
    lpsci_transfer_t xfer;

    xfer.data     = data;
    xfer.dataSize = num;

    status = LPSCI_TransferReceiveDMA(lpsci->resource->base, lpsci->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;

        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;

        case kStatus_LPSCI_TxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;

        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t LPSCI_DmaTransfer(const void *data_out,
                                 void *data_in,
                                 uint32_t num,
                                 cmsis_lpsci_dma_driver_state_t *lpsci)
{
    /* Only in synchronous mode */
    return ARM_DRIVER_ERROR;
}

static uint32_t LPSCI_DmaGetTxCount(cmsis_lpsci_dma_driver_state_t *lpsci)
{
    uint32_t cnt;

    /* If TX not in progress, then the TX count is txDataSizeAll saved in handle. */
    if (kStatus_NoTransferInProgress == LPSCI_TransferGetSendCountDMA(lpsci->resource->base, lpsci->handle, &cnt))
    {
        cnt = lpsci->handle->txDataSizeAll;
    }

    return cnt;
}

static uint32_t LPSCI_DmaGetRxCount(cmsis_lpsci_dma_driver_state_t *lpsci)
{
    uint32_t cnt;

    if (kStatus_NoTransferInProgress == LPSCI_TransferGetReceiveCountDMA(lpsci->resource->base, lpsci->handle, &cnt))
    {
        cnt = lpsci->handle->rxDataSizeAll;
    }

    return cnt;
}

static int32_t LPSCI_DmaControl(uint32_t control, uint32_t arg, cmsis_lpsci_dma_driver_state_t *lpsci)
{
    /* Must be power on. */
    if (!(lpsci->flags & USART_FLAG_POWER))
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
            LPSCI_TransferAbortSendDMA(lpsci->resource->base, lpsci->handle);
            return ARM_DRIVER_OK;

        /* Abort receive */
        case ARM_USART_ABORT_RECEIVE:
            LPSCI_TransferAbortReceiveDMA(lpsci->resource->base, lpsci->handle);
            return ARM_DRIVER_OK;

        default:
            break;
    }

    return LPSCI_CommonControl(control, arg, lpsci->resource, &lpsci->flags);
}

static ARM_USART_STATUS LPSCI_DmaGetStatus(cmsis_lpsci_dma_driver_state_t *lpsci)
{
    ARM_USART_STATUS stat;
    uint32_t ksdk_lpsci_status = LPSCI_GetStatusFlags(lpsci->resource->base);

    stat.tx_busy = ((kLPSCI_TxBusy == lpsci->handle->txState) ? (1U) : (0U));
    stat.rx_busy = ((kLPSCI_RxBusy == lpsci->handle->rxState) ? (1U) : (0U));

    stat.tx_underflow = 0U;
    stat.rx_overflow  = (!(!(ksdk_lpsci_status & kLPSCI_RxOverrunFlag)));
#if defined(FSL_FEATURE_LPSCI_HAS_LIN_BREAK_DETECT) && FSL_FEATURE_LPSCI_HAS_LIN_BREAK_DETECT
    stat.rx_break = (!(!(ksdk_lpsci_status & kLPSCI_LinBreakFlag)));
#else
    stat.rx_break = 0U;
#endif
    stat.rx_framing_error = (!(!(ksdk_lpsci_status & kLPSCI_FramingErrorFlag)));
    stat.rx_parity_error  = (!(!(ksdk_lpsci_status & kLPSCI_ParityErrorFlag)));
    stat.reserved         = 0U;

    return stat;
}
#endif

#endif

#if ((RTE_USART0 && !RTE_USART0_DMA_EN) && defined(UART0))
void KSDK_LPSCI_NonBlockingCallback(UART0_Type *base, lpsci_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    switch (status)
    {
        case kStatus_LPSCI_TxIdle:
            event = ARM_USART_EVENT_SEND_COMPLETE;
            break;

        case kStatus_LPSCI_RxIdle:
            event = ARM_USART_EVENT_RECEIVE_COMPLETE;
            break;

        case kStatus_LPSCI_RxHardwareOverrun:
            event = ARM_USART_EVENT_RX_OVERFLOW;
            break;

        case kStatus_LPSCI_FramingError:
            event = ARM_USART_EVENT_RX_FRAMING_ERROR;
            break;
        case kStatus_LPSCI_ParityError:
            event = ARM_USART_EVENT_RX_PARITY_ERROR;
            break;
        default:
            event = 0U;
            break;
    }

    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_USART_SignalEvent_t)userData)(event);
    }
}

static int32_t LPSCI_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event,
                                           cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    if (!(lpsci->flags & USART_FLAG_INIT))
    {
        lpsci->cb_event = cb_event;
        lpsci->flags    = USART_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t LPSCI_NonBlockingUninitialize(cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    lpsci->flags = USART_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t LPSCI_NonBlockingPowerControl(ARM_POWER_STATE state, cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    lpsci_config_t config;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (lpsci->flags & USART_FLAG_POWER)
            {
                LPSCI_Deinit(lpsci->resource->base);
                lpsci->flags = USART_FLAG_INIT;
            }
            break;

        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_POWER_FULL:
            /* Must be initialized first. */
            if (lpsci->flags == USART_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (lpsci->flags & USART_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            LPSCI_GetDefaultConfig(&config);
            config.enableTx = true;
            config.enableRx = true;

            LPSCI_Init(lpsci->resource->base, &config, lpsci->resource->GetFreq());
            LPSCI_TransferCreateHandle(lpsci->resource->base, lpsci->handle, KSDK_LPSCI_NonBlockingCallback,
                                       (void *)lpsci->cb_event);
            lpsci->flags |= (USART_FLAG_POWER | USART_FLAG_CONFIGURED);

            break;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t LPSCI_NonBlockingSend(const void *data, uint32_t num, cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    int32_t ret;
    status_t status;
    lpsci_transfer_t xfer;

    xfer.data     = (uint8_t *)data;
    xfer.dataSize = num;

    status = LPSCI_TransferSendNonBlocking(lpsci->resource->base, lpsci->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;

        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;

        case kStatus_LPSCI_TxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;

        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t LPSCI_NonBlockingReceive(void *data, uint32_t num, cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    int32_t ret;
    status_t status;
    lpsci_transfer_t xfer;

    xfer.data     = data;
    xfer.dataSize = num;

    status = LPSCI_TransferReceiveNonBlocking(lpsci->resource->base, lpsci->handle, &xfer, NULL);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;

        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;

        case kStatus_LPSCI_RxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;

        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t LPSCI_NonBlockingTransfer(const void *data_out,
                                         void *data_in,
                                         uint32_t num,
                                         cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    /* Only in synchronous mode */
    return ARM_DRIVER_ERROR;
}

static uint32_t LPSCI_NonBlockingGetTxCount(cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    uint32_t cnt;

    /* If TX not in progress, then the TX count is txDataSizeAll saved in handle. */
    if (kStatus_NoTransferInProgress == LPSCI_TransferGetSendCount(lpsci->resource->base, lpsci->handle, &cnt))
    {
        cnt = lpsci->handle->txDataSizeAll;
    }

    return cnt;
}

static uint32_t LPSCI_NonBlockingGetRxCount(cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    uint32_t cnt;

    if (kStatus_NoTransferInProgress == LPSCI_TransferGetReceiveCount(lpsci->resource->base, lpsci->handle, &cnt))
    {
        cnt = lpsci->handle->rxDataSizeAll;
    }

    return cnt;
}

static int32_t LPSCI_NonBlockingControl(uint32_t control, uint32_t arg, cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    /* Must be power on. */
    if (!(lpsci->flags & USART_FLAG_POWER))
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
            LPSCI_TransferAbortSend(lpsci->resource->base, lpsci->handle);
            return ARM_DRIVER_OK;

        /* Abort receive */
        case ARM_USART_ABORT_RECEIVE:
            LPSCI_TransferAbortReceive(lpsci->resource->base, lpsci->handle);
            return ARM_DRIVER_OK;

        default:
            break;
    }

    return LPSCI_CommonControl(control, arg, lpsci->resource, &lpsci->flags);
}

static ARM_USART_STATUS LPSCI_NonBlockingGetStatus(cmsis_lpsci_interrupt_driver_state_t *lpsci)
{
    ARM_USART_STATUS stat;
    uint32_t ksdk_lpsci_status = LPSCI_GetStatusFlags(lpsci->resource->base);

    stat.tx_busy = ((kLPSCI_TxBusy == lpsci->handle->txState) ? (1U) : (0U));
    stat.rx_busy = ((kLPSCI_RxBusy == lpsci->handle->rxState) ? (1U) : (0U));

    stat.tx_underflow = 0U;
    stat.rx_overflow  = (!(!(ksdk_lpsci_status & kLPSCI_RxOverrunFlag)));
#if defined(FSL_FEATURE_LPSCI_HAS_LIN_BREAK_DETECT) && FSL_FEATURE_LPSCI_HAS_LIN_BREAK_DETECT
    stat.rx_break = (!(!(ksdk_lpsci_status & kLPSCI_LinBreakFlag)));
#else
    stat.rx_break = 0U;
#endif
    stat.rx_framing_error = (!(!(ksdk_lpsci_status & kLPSCI_FramingErrorFlag)));
    stat.rx_parity_error  = (!(!(ksdk_lpsci_status & kLPSCI_ParityErrorFlag)));
    stat.reserved         = 0U;

    return stat;
}

#endif

#if defined(UART0) && RTE_USART0

/* User needs to provide the implementation for UART0_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t UART0_GetFreq(void);
extern void UART0_InitPins(void);
extern void UART0_DeinitPins(void);

cmsis_lpsci_resource_t UART0_Resource = {UART0, UART0_GetFreq};

#if RTE_USART0_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_lpsci_dma_resource_t UART0_DmaResource = {
    RTE_USART0_DMA_TX_DMA_BASE, RTE_USART0_DMA_TX_CH, RTE_USART0_DMA_TX_DMAMUX_BASE, RTE_USART0_DMA_TX_PERI_SEL,

    RTE_USART0_DMA_RX_DMA_BASE, RTE_USART0_DMA_RX_CH, RTE_USART0_DMA_RX_DMAMUX_BASE, RTE_USART0_DMA_RX_PERI_SEL,
};

lpsci_dma_handle_t UART0_DmaHandle;
dma_handle_t UART0_DmaRxHandle;
dma_handle_t UART0_DmaTxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart0_dma_driver_state")
cmsis_lpsci_dma_driver_state_t UART0_DmaDriverState = {
#else
cmsis_lpsci_dma_driver_state_t UART0_DmaDriverState = {
#endif
    &UART0_Resource, &UART0_DmaResource, &UART0_DmaHandle, &UART0_DmaRxHandle, &UART0_DmaTxHandle,
};

static int32_t UART0_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART0_InitPins();
    return LPSCI_DmaInitialize(cb_event, &UART0_DmaDriverState);
}

static int32_t UART0_DmaUninitialize(void)
{
    UART0_DeinitPins();
    return LPSCI_DmaUninitialize(&UART0_DmaDriverState);
}

static int32_t UART0_DmaPowerControl(ARM_POWER_STATE state)
{
    return LPSCI_DmaPowerControl(state, &UART0_DmaDriverState);
}

static int32_t UART0_DmaSend(const void *data, uint32_t num)
{
    return LPSCI_DmaSend(data, num, &UART0_DmaDriverState);
}

static int32_t UART0_DmaReceive(void *data, uint32_t num)
{
    return LPSCI_DmaReceive(data, num, &UART0_DmaDriverState);
}

static int32_t UART0_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSCI_DmaTransfer(data_out, data_in, num, &UART0_DmaDriverState);
}

static uint32_t UART0_DmaGetTxCount(void)
{
    return LPSCI_DmaGetTxCount(&UART0_DmaDriverState);
}

static uint32_t UART0_DmaGetRxCount(void)
{
    return LPSCI_DmaGetRxCount(&UART0_DmaDriverState);
}

static int32_t UART0_DmaControl(uint32_t control, uint32_t arg)
{
    return LPSCI_DmaControl(control, arg, &UART0_DmaDriverState);
}

static ARM_USART_STATUS UART0_DmaGetStatus(void)
{
    return LPSCI_DmaGetStatus(&UART0_DmaDriverState);
}

#endif

#else

lpsci_handle_t UART0_Handle;

#if defined(USART0_RX_BUFFER_ENABLE) && (USART0_RX_BUFFER_ENABLE == 1)
static uint8_t uart0_rxRingBuffer[USART_RX_BUFFER_LEN];
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("uart0_non_blocking_driver_state")
cmsis_lpsci_interrupt_driver_state_t UART0_NonBlockingDriverState = {
#else
cmsis_lpsci_interrupt_driver_state_t UART0_NonBlockingDriverState = {
#endif
    &UART0_Resource,
    &UART0_Handle,
};

static int32_t UART0_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    UART0_InitPins();
    return LPSCI_NonBlockingInitialize(cb_event, &UART0_NonBlockingDriverState);
}

static int32_t UART0_NonBlockingUninitialize(void)
{
    UART0_DeinitPins();
    return LPSCI_NonBlockingUninitialize(&UART0_NonBlockingDriverState);
}

static int32_t UART0_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    uint32_t result;

    result = LPSCI_NonBlockingPowerControl(state, &UART0_NonBlockingDriverState);

#if defined(USART0_RX_BUFFER_ENABLE) && (USART0_RX_BUFFER_ENABLE == 1)
    if ((state == ARM_POWER_FULL) && (UART0_NonBlockingDriverState.handle->rxRingBuffer == NULL))
    {
        LPSCI_TransferStartRingBuffer(UART0_NonBlockingDriverState.resource->base, UART0_NonBlockingDriverState.handle,
                                      uart0_rxRingBuffer, USART_RX_BUFFER_LEN);
    }
#endif

    return result;
}

static int32_t UART0_NonBlockingSend(const void *data, uint32_t num)
{
    return LPSCI_NonBlockingSend(data, num, &UART0_NonBlockingDriverState);
}

static int32_t UART0_NonBlockingReceive(void *data, uint32_t num)
{
    return LPSCI_NonBlockingReceive(data, num, &UART0_NonBlockingDriverState);
}

static int32_t UART0_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSCI_NonBlockingTransfer(data_out, data_in, num, &UART0_NonBlockingDriverState);
}

static uint32_t UART0_NonBlockingGetTxCount(void)
{
    return LPSCI_NonBlockingGetTxCount(&UART0_NonBlockingDriverState);
}

static uint32_t UART0_NonBlockingGetRxCount(void)
{
    return LPSCI_NonBlockingGetRxCount(&UART0_NonBlockingDriverState);
}

static int32_t UART0_NonBlockingControl(uint32_t control, uint32_t arg)
{
    int32_t result;
    result = LPSCI_NonBlockingControl(control, arg, &UART0_NonBlockingDriverState);
    if (ARM_DRIVER_OK != result)
    {
        return result;
    }
    if (UART0_NonBlockingDriverState.handle->rxRingBuffer != NULL)
    {
        /* Enable the interrupt to accept the data when user need the ring buffer. */
        LPSCI_EnableInterrupts(UART0_NonBlockingDriverState.resource->base, kLPSCI_RxDataRegFullInterruptEnable |
                                                                                kLPSCI_RxOverrunInterruptEnable |
                                                                                kLPSCI_FramingErrorInterruptEnable);
        /* Enable parity error interrupt when parity mode is enable*/
        if (UART0_C1_PE_MASK & UART0_NonBlockingDriverState.resource->base->C1)
        {
            LPSCI_EnableInterrupts(UART0_NonBlockingDriverState.resource->base, kLPSCI_ParityErrorInterruptEnable);
        }
    }
    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS UART0_NonBlockingGetStatus(void)
{
    return LPSCI_NonBlockingGetStatus(&UART0_NonBlockingDriverState);
}

#endif

ARM_DRIVER_USART Driver_USART0 = {
    LPSCI_GetVersion,      LPSCI_GetCapabilities,
#if RTE_USART0_DMA_EN
    UART0_DmaInitialize,   UART0_DmaUninitialize, UART0_DmaPowerControl, UART0_DmaSend,    UART0_DmaReceive,
    UART0_DmaTransfer,     UART0_DmaGetTxCount,   UART0_DmaGetRxCount,   UART0_DmaControl, UART0_DmaGetStatus,
#else
    UART0_NonBlockingInitialize,
    UART0_NonBlockingUninitialize,
    UART0_NonBlockingPowerControl,
    UART0_NonBlockingSend,
    UART0_NonBlockingReceive,
    UART0_NonBlockingTransfer,
    UART0_NonBlockingGetTxCount,
    UART0_NonBlockingGetRxCount,
    UART0_NonBlockingControl,
    UART0_NonBlockingGetStatus,
#endif
    LPSCI_SetModemControl, LPSCI_GetModemStatus};
#endif
