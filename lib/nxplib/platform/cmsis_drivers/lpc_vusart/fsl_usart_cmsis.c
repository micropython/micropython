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

#include "fsl_usart_cmsis.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.vusart_cmsis"
#endif

#if (RTE_USART0 || RTE_USART1 || RTE_USART2 || RTE_USART3)

#define ARM_USART_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

typedef const struct _cmsis_usart_resource
{
    USART_Type *base;          /*!< usart peripheral base address.       */
    uint32_t (*GetFreq)(void); /*!< Function to get the clock frequency. */
} cmsis_usart_resource_t;

typedef struct _cmsis_usart_non_blocking_driver_state
{
    cmsis_usart_resource_t *resource; /*!< Basic usart resource.      */
    usart_handle_t *handle;           /*!< Interupt transfer handle.  */
    ARM_USART_SignalEvent_t cb_event; /*!< Callback function.         */
    uint8_t flags;                    /*!< Control and state flags. */
} cmsis_usart_non_blocking_driver_state_t;

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
typedef const struct _cmsis_usart_dma_resource
{
    DMA_Type *txDmaBase;   /*!< DMA peripheral base address for TX.   */
    uint32_t txDmaChannel; /*!< DMA channel for usart TX.             */

    DMA_Type *rxDmaBase;   /*!< DMA peripheral base address for RX.   */
    uint32_t rxDmaChannel; /*!< DMA channel for usart RX.             */
} cmsis_usart_dma_resource_t;

typedef struct _cmsis_usart_dma_driver_state
{
    cmsis_usart_resource_t *resource;        /*!< usart basic resource.       */
    cmsis_usart_dma_resource_t *dmaResource; /*!< usart DMA resource.         */
    usart_dma_handle_t *handle;              /*!< usart DMA transfer handle.  */
    dma_handle_t *rxHandle;                  /*!< DMA RX handle.              */
    dma_handle_t *txHandle;                  /*!< DMA TX handle.              */
    ARM_USART_SignalEvent_t cb_event;        /*!< Callback function.          */
    uint8_t flags;                           /*!< Control and state flags. */
} cmsis_usart_dma_driver_state_t;
#endif

enum _usart_transfer_states
{
    kUSART_TxIdle, /*!< TX idle. */
    kUSART_TxBusy, /*!< TX busy. */
    kUSART_RxIdle, /*!< RX idle. */
    kUSART_RxBusy  /*!< RX busy. */
};

/* Driver Version */
static const ARM_DRIVER_VERSION s_usartDriverVersion = {ARM_USART_API_VERSION, ARM_USART_DRV_VERSION};

static const ARM_USART_CAPABILITIES s_usartDriverCapabilities = {
    1, /* supports usart (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports usart Single-wire mode */
    0, /* supports usart IrDA mode */
    0, /* supports usart Smart Card mode */
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
 * Common control function used by usart_NonBlockingControl/usart_DmaControl/usart_EdmaControl
 */
static int32_t USART_CommonControl(uint32_t control,
                                   uint32_t arg,
                                   cmsis_usart_resource_t *resource,
                                   uint8_t *isConfigured)
{
    usart_config_t config;
    uint32_t instance = USART_GetInstance(resource->base);

    USART_GetDefaultConfig(&config);

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
                config.enableTx = true;
            }
            else
            {
                config.enableTx = false;
            }
            return ARM_DRIVER_OK;

        case ARM_USART_CONTROL_RX:
            if (arg)
            {
                config.enableRx = true;
            }
            else
            {
                config.enableRx = false;
            }

            return ARM_DRIVER_OK;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_USART_PARITY_Msk)
    {
        case ARM_USART_PARITY_NONE:
            config.parityMode = kUSART_ParityDisabled;
            break;
        case ARM_USART_PARITY_EVEN:
            config.parityMode = kUSART_ParityEven;
            break;
        case ARM_USART_PARITY_ODD:
            config.parityMode = kUSART_ParityOdd;
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
            config.stopBitCount = kUSART_TwoStopBit;
            break;
        default:
            return ARM_USART_ERROR_STOP_BITS;
    }

/* FIFO configurations. */
#if defined(RTE_USART_FIFO_ENABLE) && (RTE_USART_FIFO_ENABLE == 1U)
    switch (instance)
    {
        case 0:
            config.fifoConfig.txFifoSize   = USART0_TX_FIFO_SIZE;
            config.fifoConfig.rxFifoSize   = USART0_RX_FIFO_SIZE;
            config.fifoConfig.enableRxFifo = (bool)USART0_RX_FIFO_SIZE;
            config.fifoConfig.enableTxFifo = (bool)USART0_TX_FIFO_SIZE;
            break;
        case 1:
            config.fifoConfig.txFifoSize   = USART1_TX_FIFO_SIZE;
            config.fifoConfig.rxFifoSize   = USART1_RX_FIFO_SIZE;
            config.fifoConfig.enableRxFifo = (bool)USART1_RX_FIFO_SIZE;
            config.fifoConfig.enableTxFifo = (bool)USART1_TX_FIFO_SIZE;
            break;
        case 2:
            config.fifoConfig.txFifoSize   = USART2_TX_FIFO_SIZE;
            config.fifoConfig.rxFifoSize   = USART2_RX_FIFO_SIZE;
            config.fifoConfig.enableRxFifo = (bool)USART2_RX_FIFO_SIZE;
            config.fifoConfig.enableTxFifo = (bool)USART2_TX_FIFO_SIZE;
            break;
        case 3:
            config.fifoConfig.txFifoSize   = USART3_TX_FIFO_SIZE;
            config.fifoConfig.rxFifoSize   = USART3_RX_FIFO_SIZE;
            config.fifoConfig.enableRxFifo = (bool)USART3_RX_FIFO_SIZE;
            config.fifoConfig.enableTxFifo = (bool)USART3_TX_FIFO_SIZE;
            break;
        default:
            break;
    }
#endif /* FIFO configurations. */

    /* If usart is already configured, deinit it first. */
    if ((*isConfigured) & USART_FLAG_CONFIGURED)
    {
        USART_Deinit(resource->base);
        *isConfigured &= ~USART_FLAG_CONFIGURED;
    }

    config.enableTx = true;
    config.enableRx = true;

    if (kStatus_USART_BaudrateNotSupport == USART_Init(resource->base, &config, resource->GetFreq()))
    {
        return ARM_USART_ERROR_BAUDRATE;
    }

    *isConfigured |= USART_FLAG_CONFIGURED;

    return ARM_DRIVER_OK;
}

static ARM_DRIVER_VERSION USARTx_GetVersion(void)
{
    return s_usartDriverVersion;
}

static ARM_USART_CAPABILITIES USARTx_GetCapabilities(void)
{
    return s_usartDriverCapabilities;
}

static int32_t USARTx_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS USARTx_GetModemStatus(void)
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

#if (RTE_USART0_DMA_EN || RTE_USART1_DMA_EN || RTE_USART2_DMA_EN || RTE_USART3_DMA_EN)

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
void KSDK_USART_DmaCallback(USART_Type *base, usart_dma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_USART_TxIdle == status)
    {
        event = ARM_USART_EVENT_SEND_COMPLETE;
    }

    if (kStatus_USART_RxIdle == status)
    {
        event = ARM_USART_EVENT_RECEIVE_COMPLETE;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_USART_SignalEvent_t)userData)(event);
    }
}

static int32_t USART_DmaInitialize(ARM_USART_SignalEvent_t cb_event, cmsis_usart_dma_driver_state_t *usart)
{
    if (!(usart->flags & USART_FLAG_INIT))
    {
        usart->cb_event = cb_event;
        usart->flags    = USART_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t USART_DmaUninitialize(cmsis_usart_dma_driver_state_t *usart)
{
    usart->flags = USART_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t USART_DmaPowerControl(ARM_POWER_STATE state, cmsis_usart_dma_driver_state_t *usart)
{
    usart_config_t config;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (usart->flags & USART_FLAG_POWER)
            {
                USART_Deinit(usart->resource->base);
                DMA_DisableChannel(usart->dmaResource->rxDmaBase, usart->dmaResource->rxDmaChannel);
                DMA_DisableChannel(usart->dmaResource->txDmaBase, usart->dmaResource->txDmaChannel);
                usart->flags = USART_FLAG_INIT;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            /* Must be initialized first. */
            if (usart->flags == USART_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (usart->flags & USART_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            USART_GetDefaultConfig(&config);
            config.enableTx = true;
            config.enableRx = true;

            /* Set up DMA setting. */
            DMA_EnableChannel(usart->dmaResource->txDmaBase, usart->dmaResource->txDmaChannel);
            DMA_EnableChannel(usart->dmaResource->rxDmaBase, usart->dmaResource->rxDmaChannel);

            DMA_CreateHandle(usart->rxHandle, usart->dmaResource->rxDmaBase, usart->dmaResource->rxDmaChannel);
            DMA_CreateHandle(usart->txHandle, usart->dmaResource->txDmaBase, usart->dmaResource->txDmaChannel);

            /* Setup the usart. */
            USART_Init(usart->resource->base, &config, usart->resource->GetFreq());
            USART_TransferCreateHandleDMA(usart->resource->base, usart->handle, KSDK_USART_DmaCallback,
                                          (void *)usart->cb_event, usart->txHandle, usart->rxHandle);

            usart->flags |= (USART_FLAG_POWER | USART_FLAG_CONFIGURED);

            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t USART_DmaSend(const void *data, uint32_t num, cmsis_usart_dma_driver_state_t *usart)
{
    int32_t ret;
    status_t status;
    usart_transfer_t xfer;

    xfer.data     = (uint8_t *)data;
    xfer.dataSize = num;

    status = USART_TransferSendDMA(usart->resource->base, usart->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_USART_TxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t USART_DmaReceive(void *data, uint32_t num, cmsis_usart_dma_driver_state_t *usart)
{
    int32_t ret;
    status_t status;
    usart_transfer_t xfer;

    xfer.data     = data;
    xfer.dataSize = num;

    status = USART_TransferReceiveDMA(usart->resource->base, usart->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_USART_RxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t USART_DmaTransfer(const void *data_out,
                                 void *data_in,
                                 uint32_t num,
                                 cmsis_usart_dma_driver_state_t *usart)
{
    /* Only in synchronous mode */
    return ARM_DRIVER_ERROR;
}

static int32_t USART_DmaGetTxCount(cmsis_usart_dma_driver_state_t *usart)
{
    /* Does not support */
    return ARM_DRIVER_ERROR;
}

static int32_t USART_DmaGetRxCount(cmsis_usart_dma_driver_state_t *usart)
{
    /* Does not support */
    return ARM_DRIVER_ERROR;
}

static int32_t USART_DmaControl(uint32_t control, uint32_t arg, cmsis_usart_dma_driver_state_t *usart)
{
    /* Must be power on. */
    if (!(usart->flags & USART_FLAG_POWER))
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
            DMA_AbortTransfer(usart->handle->txDmaHandle);
            usart->handle->txState = kUSART_TxIdle;
            return ARM_DRIVER_OK;

        /* Abort receive */
        case ARM_USART_ABORT_RECEIVE:
            DMA_AbortTransfer(usart->handle->rxDmaHandle);
            usart->handle->rxState = kUSART_RxIdle;
            return ARM_DRIVER_OK;

        default:
            break;
    }

    return USART_CommonControl(control, arg, usart->resource, &usart->flags);
}

static ARM_USART_STATUS USART_DmaGetStatus(cmsis_usart_dma_driver_state_t *usart)
{
    ARM_USART_STATUS stat;
    uint32_t ksdk_usart_status = usart->resource->base->STAT;

    stat.tx_busy = ((kUSART_TxBusy == usart->handle->txState) ? (1U) : (0U));
    stat.rx_busy = ((kUSART_RxBusy == usart->handle->rxState) ? (1U) : (0U));

    stat.tx_underflow = 0U;
    stat.rx_overflow  = 0U;

    stat.rx_break = (!(!(ksdk_usart_status & USART_STAT_RXBRK_MASK)));

    stat.rx_framing_error = (!(!(ksdk_usart_status & USART_STAT_FRAMERRINT_MASK)));
    stat.rx_parity_error  = (!(!(ksdk_usart_status & USART_STAT_PARITYERRINT_MASK)));
    stat.reserved         = 0U;

    return stat;
}
#endif

#endif

#if ((RTE_USART0 && !RTE_USART0_DMA_EN) || (RTE_USART1 && !RTE_USART1_DMA_EN) || (RTE_USART2 && !RTE_USART2_DMA_EN) || \
     (RTE_USART3 && !RTE_USART3_DMA_EN))

void KSDK_USART_NonBlockingCallback(USART_Type *base, usart_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_USART_TxIdle == status)
    {
        event = ARM_USART_EVENT_SEND_COMPLETE;
    }
    if (kStatus_USART_RxIdle == status)
    {
        event = ARM_USART_EVENT_RECEIVE_COMPLETE;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_USART_SignalEvent_t)userData)(event);
    }
}

static int32_t USART_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event,
                                           cmsis_usart_non_blocking_driver_state_t *usart)
{
    if (!(usart->flags & USART_FLAG_INIT))
    {
        usart->cb_event = cb_event;
        usart->flags    = USART_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t USART_NonBlockingUninitialize(cmsis_usart_non_blocking_driver_state_t *usart)
{
    usart->flags = USART_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t USART_NonBlockingPowerControl(ARM_POWER_STATE state, cmsis_usart_non_blocking_driver_state_t *usart)
{
    usart_config_t config;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (usart->flags & USART_FLAG_POWER)
            {
                USART_Deinit(usart->resource->base);
                usart->flags = USART_FLAG_INIT;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            /* Must be initialized first. */
            if (usart->flags == USART_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (usart->flags & USART_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            USART_GetDefaultConfig(&config);
            config.enableTx = true;
            config.enableRx = true;

            USART_Init(usart->resource->base, &config, usart->resource->GetFreq());
            USART_TransferCreateHandle(usart->resource->base, usart->handle, KSDK_USART_NonBlockingCallback,
                                       (void *)usart->cb_event);
            usart->flags |= (USART_FLAG_POWER | USART_FLAG_CONFIGURED);

            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t USART_NonBlockingSend(const void *data, uint32_t num, cmsis_usart_non_blocking_driver_state_t *usart)
{
    int32_t ret;
    status_t status;
    usart_transfer_t xfer;

    xfer.data     = (uint8_t *)data;
    xfer.dataSize = num;

    status = USART_TransferSendNonBlocking(usart->resource->base, usart->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_USART_TxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t USART_NonBlockingReceive(void *data, uint32_t num, cmsis_usart_non_blocking_driver_state_t *usart)
{
    int32_t ret;
    status_t status;
    usart_transfer_t xfer;

    xfer.data     = data;
    xfer.dataSize = num;

    status = USART_TransferReceiveNonBlocking(usart->resource->base, usart->handle, &xfer, NULL);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_USART_RxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t USART_NonBlockingTransfer(const void *data_out,
                                         void *data_in,
                                         uint32_t num,
                                         cmsis_usart_non_blocking_driver_state_t *usart)
{
    /* Only in synchronous mode */
    return ARM_DRIVER_ERROR;
}

static uint32_t USART_NonBlockingGetTxCount(cmsis_usart_non_blocking_driver_state_t *usart)
{
    uint32_t cnt;

    /* If TX not in progress, then the TX count is txDataSizeAll saved in handle. */
    if (kUSART_TxIdle == usart->handle->txState)
    {
        cnt = usart->handle->txDataSizeAll;
    }
    else
    {
        cnt = usart->handle->txDataSizeAll - usart->handle->txDataSize;
    }

    return cnt;
}

static uint32_t USART_NonBlockingGetRxCount(cmsis_usart_non_blocking_driver_state_t *usart)
{
    uint32_t cnt;

    if (kUSART_RxIdle == usart->handle->rxState)
    {
        cnt = usart->handle->rxDataSizeAll;
    }
    else
    {
        cnt = usart->handle->rxDataSizeAll - usart->handle->rxDataSize;
    }

    return cnt;
}

static int32_t USART_NonBlockingControl(uint32_t control, uint32_t arg, cmsis_usart_non_blocking_driver_state_t *usart)
{
    /* Must be power on. */
    if (!(usart->flags & USART_FLAG_POWER))
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
            USART_TransferAbortSend(usart->resource->base, usart->handle);
            return ARM_DRIVER_OK;

        /* Abort receive */
        case ARM_USART_ABORT_RECEIVE:
            USART_TransferAbortReceive(usart->resource->base, usart->handle);
            return ARM_DRIVER_OK;

        default:
            break;
    }

    return USART_CommonControl(control, arg, usart->resource, &usart->flags);
}

static ARM_USART_STATUS USART_NonBlockingGetStatus(cmsis_usart_non_blocking_driver_state_t *usart)
{
    ARM_USART_STATUS stat;
    uint32_t ksdk_usart_status = usart->resource->base->STAT;

    stat.tx_busy = ((kUSART_TxBusy == usart->handle->txState) ? (1U) : (0U));
    stat.rx_busy = ((kUSART_RxBusy == usart->handle->rxState) ? (1U) : (0U));

    stat.tx_underflow = 0U;
    stat.rx_overflow  = 0U;

    stat.rx_break = (!(!(ksdk_usart_status & USART_STAT_RXBRK_MASK)));

    stat.rx_framing_error = (!(!(ksdk_usart_status & USART_STAT_FRAMERRINT_MASK)));
    stat.rx_parity_error  = (!(!(ksdk_usart_status & USART_STAT_PARITYERRINT_MASK)));
    stat.reserved         = 0U;

    return stat;
}

#endif

#if defined(USART0) && RTE_USART0

/* User needs to provide the implementation for USART0_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t USART0_GetFreq(void);
extern void USART0_InitPins(void);
extern void USART0_DeinitPins(void);

cmsis_usart_resource_t usart0_Resource = {USART0, USART0_GetFreq};

/* usart0 Driver Control Block */

#if RTE_USART0_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart0_DmaResource = {
    RTE_USART0_DMA_TX_DMA_BASE,
    RTE_USART0_DMA_TX_CH,
    RTE_USART0_DMA_RX_DMA_BASE,
    RTE_USART0_DMA_RX_CH,
};

usart_dma_handle_t USART0_DmaHandle;
dma_handle_t USART0_DmaRxHandle;
dma_handle_t USART0_DmaTxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("usart0_dma_driver_state")
cmsis_usart_dma_driver_state_t usart0_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart0_DmaDriverState = {
#endif
    &usart0_Resource, &usart0_DmaResource, &USART0_DmaHandle, &USART0_DmaRxHandle, &USART0_DmaTxHandle,
};

static int32_t USART0_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART0_InitPins();
    return USART_DmaInitialize(cb_event, &usart0_DmaDriverState);
}

static int32_t USART0_DmaUninitialize(void)
{
    USART0_DeinitPins();
    return USART_DmaUninitialize(&usart0_DmaDriverState);
}

static int32_t USART0_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart0_DmaDriverState);
}

static int32_t USART0_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart0_DmaDriverState);
}

static int32_t USART0_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart0_DmaDriverState);
}

static int32_t USART0_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart0_DmaDriverState);
}

static uint32_t USART0_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart0_DmaDriverState);
}

static uint32_t USART0_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart0_DmaDriverState);
}

static int32_t USART0_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart0_DmaDriverState);
}

static ARM_USART_STATUS USART0_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart0_DmaDriverState);
}

#endif

#else

usart_handle_t USART0_Handle;
#if defined(USART0_RX_BUFFER_ENABLE) && (USART0_RX_BUFFER_ENABLE == 1)
static uint8_t usart0_rxRingBuffer[USART_RX_BUFFER_LEN];
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("usart0_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart0_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart0_NonBlockingDriverState = {
#endif
    &usart0_Resource,
    &USART0_Handle,
};

static int32_t USART0_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART0_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingUninitialize(void)
{
    USART0_DeinitPins();
    return USART_NonBlockingUninitialize(&usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    uint32_t result;

    result = USART_NonBlockingPowerControl(state, &usart0_NonBlockingDriverState);
#if defined(USART0_RX_BUFFER_ENABLE) && (USART0_RX_BUFFER_ENABLE == 1)
    if ((state == ARM_POWER_FULL) && (usart0_NonBlockingDriverState.handle->rxRingBuffer == NULL))
    {
        USART_TransferStartRingBuffer(usart0_NonBlockingDriverState.resource->base,
                                      usart0_NonBlockingDriverState.handle, usart0_rxRingBuffer, USART_RX_BUFFER_LEN);
    }
#endif
    return result;
}

static int32_t USART0_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart0_NonBlockingDriverState);
}

static uint32_t USART0_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart0_NonBlockingDriverState);
}

static uint32_t USART0_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingControl(uint32_t control, uint32_t arg)
{
    int32_t result;

    result = USART_NonBlockingControl(control, arg, &usart0_NonBlockingDriverState);
    if (ARM_DRIVER_OK != result)
    {
        return result;
    }
#if defined(USART0_RX_BUFFER_ENABLE) && (USART0_RX_BUFFER_ENABLE == 1)
    /* Start receiving interrupts */
    if (USART_IsRxFifoEnable(usart0_NonBlockingDriverState.resource->base))
    {
        USART_EnableFifoInterrupts(usart0_NonBlockingDriverState.resource->base, kUSART_RxFifoThresholdInterruptEnable);
    }
    else
    {
        USART_EnableInterrupts(usart0_NonBlockingDriverState.resource->base, kUSART_RxReadyInterruptEnable);
    }
#endif
    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS USART0_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart0_NonBlockingDriverState);
}

#endif

ARM_DRIVER_USART Driver_USART0 = {
    USARTx_GetVersion,      USARTx_GetCapabilities,
#if RTE_USART0_DMA_EN
    USART0_DmaInitialize,   USART0_DmaUninitialize, USART0_DmaPowerControl, USART0_DmaSend,    USART0_DmaReceive,
    USART0_DmaTransfer,     USART0_DmaGetTxCount,   USART0_DmaGetRxCount,   USART0_DmaControl, USART0_DmaGetStatus,
#else
    USART0_NonBlockingInitialize,
    USART0_NonBlockingUninitialize,
    USART0_NonBlockingPowerControl,
    USART0_NonBlockingSend,
    USART0_NonBlockingReceive,
    USART0_NonBlockingTransfer,
    USART0_NonBlockingGetTxCount,
    USART0_NonBlockingGetRxCount,
    USART0_NonBlockingControl,
    USART0_NonBlockingGetStatus,
#endif
    USARTx_SetModemControl, USARTx_GetModemStatus};

#endif /* usart0 */

#if defined(USART1) && RTE_USART1

/* User needs to provide the implementation for USART1_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t USART1_GetFreq(void);
extern void USART1_InitPins(void);
extern void USART1_DeinitPins(void);

cmsis_usart_resource_t usart1_Resource = {USART1, USART1_GetFreq};

#if RTE_USART1_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart1_DmaResource = {
    RTE_USART1_DMA_TX_DMA_BASE,
    RTE_USART1_DMA_TX_CH,
    RTE_USART1_DMA_RX_DMA_BASE,
    RTE_USART1_DMA_RX_CH,
};

usart_dma_handle_t USART1_DmaHandle;
dma_handle_t USART1_DmaRxHandle;
dma_handle_t USART1_DmaTxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("usart1_dma_driver_state")
cmsis_usart_dma_driver_state_t usart1_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart1_DmaDriverState = {
#endif
    &usart1_Resource, &usart1_DmaResource, &USART1_DmaHandle, &USART1_DmaRxHandle, &USART1_DmaTxHandle,
};

static int32_t USART1_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART1_InitPins();
    return USART_DmaInitialize(cb_event, &usart1_DmaDriverState);
}

static int32_t USART1_DmaUninitialize(void)
{
    USART1_DeinitPins();
    return USART_DmaUninitialize(&usart1_DmaDriverState);
}

static int32_t USART1_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart1_DmaDriverState);
}

static int32_t USART1_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart1_DmaDriverState);
}

static int32_t USART1_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart1_DmaDriverState);
}

static int32_t USART1_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart1_DmaDriverState);
}

static uint32_t USART1_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart1_DmaDriverState);
}

static uint32_t USART1_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart1_DmaDriverState);
}

static int32_t USART1_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart1_DmaDriverState);
}

static ARM_USART_STATUS USART1_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart1_DmaDriverState);
}

#endif

#else

usart_handle_t USART1_Handle;
#if defined(USART1_RX_BUFFER_ENABLE) && (USART1_RX_BUFFER_ENABLE == 1)
static uint8_t usart1_rxRingBuffer[USART_RX_BUFFER_LEN];
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("usart1_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart1_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart1_NonBlockingDriverState = {
#endif
    &usart1_Resource,
    &USART1_Handle,
};

static int32_t USART1_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART1_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingUninitialize(void)
{
    USART1_DeinitPins();
    return USART_NonBlockingUninitialize(&usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    uint32_t result;

    result = USART_NonBlockingPowerControl(state, &usart1_NonBlockingDriverState);
#if defined(USART1_RX_BUFFER_ENABLE) && (USART1_RX_BUFFER_ENABLE == 1)
    if ((state == ARM_POWER_FULL) && (usart1_NonBlockingDriverState.handle->rxRingBuffer == NULL))
    {
        USART_TransferStartRingBuffer(usart1_NonBlockingDriverState.resource->base,
                                      usart1_NonBlockingDriverState.handle, usart1_rxRingBuffer, USART_RX_BUFFER_LEN);
    }
#endif
    return result;
}

static int32_t USART1_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart1_NonBlockingDriverState);
}

static uint32_t USART1_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart1_NonBlockingDriverState);
}

static uint32_t USART1_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingControl(uint32_t control, uint32_t arg)
{
    int32_t result;

    result = USART_NonBlockingControl(control, arg, &usart1_NonBlockingDriverState);
    if (ARM_DRIVER_OK != result)
    {
        return result;
    }
#if defined(USART1_RX_BUFFER_ENABLE) && (USART1_RX_BUFFER_ENABLE == 1)
    /* Start receiving interrupts */
    if (USART_IsRxFifoEnable(usart1_NonBlockingDriverState.resource->base))
    {
        USART_EnableFifoInterrupts(usart1_NonBlockingDriverState.resource->base, kUSART_RxFifoThresholdInterruptEnable);
    }
    else
    {
        USART_EnableInterrupts(usart1_NonBlockingDriverState.resource->base, kUSART_RxReadyInterruptEnable);
    }
#endif
    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS USART1_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart1_NonBlockingDriverState);
}

#endif

ARM_DRIVER_USART Driver_USART1 = {
    USARTx_GetVersion,      USARTx_GetCapabilities,
#if RTE_USART1_DMA_EN
    USART1_DmaInitialize,   USART1_DmaUninitialize, USART1_DmaPowerControl, USART1_DmaSend,    USART1_DmaReceive,
    USART1_DmaTransfer,     USART1_DmaGetTxCount,   USART1_DmaGetRxCount,   USART1_DmaControl, USART1_DmaGetStatus,
#else
    USART1_NonBlockingInitialize,
    USART1_NonBlockingUninitialize,
    USART1_NonBlockingPowerControl,
    USART1_NonBlockingSend,
    USART1_NonBlockingReceive,
    USART1_NonBlockingTransfer,
    USART1_NonBlockingGetTxCount,
    USART1_NonBlockingGetRxCount,
    USART1_NonBlockingControl,
    USART1_NonBlockingGetStatus,
#endif
    USARTx_SetModemControl, USARTx_GetModemStatus};

#endif /* usart1 */

#if defined(USART2) && RTE_USART2

/* User needs to provide the implementation for USART2_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t USART2_GetFreq(void);
extern void USART2_InitPins(void);
extern void USART2_DeinitPins(void);

cmsis_usart_resource_t usart2_Resource = {USART2, USART2_GetFreq};

/* usart2 Driver Control Block */

#if RTE_USART2_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart2_DmaResource = {
    RTE_USART2_DMA_TX_DMA_BASE,
    RTE_USART2_DMA_TX_CH,
    RTE_USART2_DMA_RX_DMA_BASE,
    RTE_USART2_DMA_RX_CH,
};

usart_dma_handle_t USART2_DmaHandle;
dma_handle_t USART2_DmaRxHandle;
dma_handle_t USART2_DmaTxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("usart2_dma_driver_state")
cmsis_usart_dma_driver_state_t usart2_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart2_DmaDriverState = {
#endif
    &usart2_Resource, &usart2_DmaResource, &USART2_DmaHandle, &USART2_DmaRxHandle, &USART2_DmaTxHandle,
};

static int32_t USART2_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART2_InitPins();
    return USART_DmaInitialize(cb_event, &usart2_DmaDriverState);
}

static int32_t USART2_DmaUninitialize(void)
{
    USART2_DeinitPins();
    return USART_DmaUninitialize(&usart2_DmaDriverState);
}

static int32_t USART2_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart2_DmaDriverState);
}

static int32_t USART2_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart2_DmaDriverState);
}

static int32_t USART2_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart2_DmaDriverState);
}

static int32_t USART2_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart2_DmaDriverState);
}

static uint32_t USART2_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart2_DmaDriverState);
}

static uint32_t USART2_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart2_DmaDriverState);
}

static int32_t USART2_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart2_DmaDriverState);
}

static ARM_USART_STATUS USART2_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart2_DmaDriverState);
}

#endif

#else

usart_handle_t USART2_Handle;
#if defined(USART2_RX_BUFFER_ENABLE) && (USART2_RX_BUFFER_ENABLE == 1)
static uint8_t usart2_rxRingBuffer[USART_RX_BUFFER_LEN];
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("usart2_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart2_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart2_NonBlockingDriverState = {
#endif
    &usart2_Resource,
    &USART2_Handle,
};

static int32_t USART2_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART2_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingUninitialize(void)
{
    USART2_DeinitPins();
    return USART_NonBlockingUninitialize(&usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    uint32_t result;

    result = USART_NonBlockingPowerControl(state, &usart2_NonBlockingDriverState);
#if defined(USART2_RX_BUFFER_ENABLE) && (USART2_RX_BUFFER_ENABLE == 1)
    if ((state == ARM_POWER_FULL) && (usart2_NonBlockingDriverState.handle->rxRingBuffer == NULL))
    {
        USART_TransferStartRingBuffer(usart2_NonBlockingDriverState.resource->base,
                                      usart2_NonBlockingDriverState.handle, usart2_rxRingBuffer, USART_RX_BUFFER_LEN);
    }
#endif
    return result;
}

static int32_t USART2_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart2_NonBlockingDriverState);
}

static uint32_t USART2_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart2_NonBlockingDriverState);
}

static uint32_t USART2_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingControl(uint32_t control, uint32_t arg)
{
    int32_t result;

    result = USART_NonBlockingControl(control, arg, &usart2_NonBlockingDriverState);
    if (ARM_DRIVER_OK != result)
    {
        return result;
    }
#if defined(USART2_RX_BUFFER_ENABLE) && (USART2_RX_BUFFER_ENABLE == 1)
    /* Start receiving interrupts */
    if (USART_IsRxFifoEnable(usart2_NonBlockingDriverState.resource->base))
    {
        USART_EnableFifoInterrupts(usart2_NonBlockingDriverState.resource->base, kUSART_RxFifoThresholdInterruptEnable);
    }
    else
    {
        USART_EnableInterrupts(usart2_NonBlockingDriverState.resource->base, kUSART_RxReadyInterruptEnable);
    }
#endif
    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS USART2_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart2_NonBlockingDriverState);
}

#endif

ARM_DRIVER_USART Driver_USART2 = {
    USARTx_GetVersion,      USARTx_GetCapabilities,
#if RTE_USART2_DMA_EN
    USART2_DmaInitialize,   USART2_DmaUninitialize, USART2_DmaPowerControl, USART2_DmaSend,    USART2_DmaReceive,
    USART2_DmaTransfer,     USART2_DmaGetTxCount,   USART2_DmaGetRxCount,   USART2_DmaControl, USART2_DmaGetStatus,
#else
    USART2_NonBlockingInitialize,
    USART2_NonBlockingUninitialize,
    USART2_NonBlockingPowerControl,
    USART2_NonBlockingSend,
    USART2_NonBlockingReceive,
    USART2_NonBlockingTransfer,
    USART2_NonBlockingGetTxCount,
    USART2_NonBlockingGetRxCount,
    USART2_NonBlockingControl,
    USART2_NonBlockingGetStatus,
#endif
    USARTx_SetModemControl, USARTx_GetModemStatus};

#endif /* usart2 */

#if defined(USART3) && RTE_USART3

/* User needs to provide the implementation for USART3_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t USART3_GetFreq(void);
extern void USART3_InitPins(void);
extern void USART3_DeinitPins(void);

cmsis_usart_resource_t usart3_Resource = {USART3, USART3_GetFreq};

/* usart3 Driver Control Block */
#if RTE_USART3_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart3_DmaResource = {
    RTE_USART3_DMA_TX_DMA_BASE,
    RTE_USART3_DMA_TX_CH,
    RTE_USART3_DMA_RX_DMA_BASE,
    RTE_USART3_DMA_RX_CH,
};

usart_dma_handle_t USART3_DmaHandle;
dma_handle_t USART3_DmaRxHandle;
dma_handle_t USART3_DmaTxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("usart3_dma_driver_state")
cmsis_usart_dma_driver_state_t usart3_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart3_DmaDriverState = {
#endif
    &usart3_Resource, &usart3_DmaResource, &USART3_DmaHandle, &USART3_DmaRxHandle, &USART3_DmaTxHandle,
};

static int32_t USART3_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART3_InitPins();
    return USART_DmaInitialize(cb_event, &usart3_DmaDriverState);
}

static int32_t USART3_DmaUninitialize(void)
{
    USART3_DeinitPins();
    return USART_DmaUninitialize(&usart3_DmaDriverState);
}

static int32_t USART3_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart3_DmaDriverState);
}

static int32_t USART3_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart3_DmaDriverState);
}

static int32_t USART3_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart3_DmaDriverState);
}

static int32_t USART3_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart3_DmaDriverState);
}

static uint32_t USART3_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart3_DmaDriverState);
}

static uint32_t USART3_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart3_DmaDriverState);
}

static int32_t USART3_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart3_DmaDriverState);
}

static ARM_USART_STATUS USART3_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart3_DmaDriverState);
}

#endif

#else

usart_handle_t USART3_Handle;
#if defined(USART3_RX_BUFFER_ENABLE) && (USART3_RX_BUFFER_ENABLE == 1)
static uint8_t usart3_rxRingBuffer[USART_RX_BUFFER_LEN];
#endif

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("usart3_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart3_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart3_NonBlockingDriverState = {
#endif
    &usart3_Resource,
    &USART3_Handle,
};

static int32_t USART3_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART3_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingUninitialize(void)
{
    USART3_DeinitPins();
    return USART_NonBlockingUninitialize(&usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    uint32_t result;

    result = USART_NonBlockingPowerControl(state, &usart3_NonBlockingDriverState);
#if defined(USART3_RX_BUFFER_ENABLE) && (USART3_RX_BUFFER_ENABLE == 1)
    if ((state == ARM_POWER_FULL) && (usart3_NonBlockingDriverState.handle->rxRingBuffer == NULL))
    {
        USART_TransferStartRingBuffer(usart3_NonBlockingDriverState.resource->base,
                                      usart3_NonBlockingDriverState.handle, usart3_rxRingBuffer, USART_RX_BUFFER_LEN);
    }
#endif
    return result;
}

static int32_t USART3_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart3_NonBlockingDriverState);
}

static uint32_t USART3_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart3_NonBlockingDriverState);
}

static uint32_t USART3_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingControl(uint32_t control, uint32_t arg)
{
    int32_t result;

    result = USART_NonBlockingControl(control, arg, &usart3_NonBlockingDriverState);
    if (ARM_DRIVER_OK != result)
    {
        return result;
    }
#if defined(USART3_RX_BUFFER_ENABLE) && (USART3_RX_BUFFER_ENABLE == 1)
    /* Start receiving interrupts */
    if (USART_IsRxFifoEnable(usart3_NonBlockingDriverState.resource->base))
    {
        USART_EnableFifoInterrupts(usart3_NonBlockingDriverState.resource->base, kUSART_RxFifoThresholdInterruptEnable);
    }
    else
    {
        USART_EnableInterrupts(usart3_NonBlockingDriverState.resource->base, kUSART_RxReadyInterruptEnable);
    }
#endif
    return ARM_DRIVER_OK;
}

static ARM_USART_STATUS USART3_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart3_NonBlockingDriverState);
}

#endif

ARM_DRIVER_USART Driver_USART3 = {
    USARTx_GetVersion,      USARTx_GetCapabilities,
#if RTE_USART3_DMA_EN
    USART3_DmaInitialize,   USART3_DmaUninitialize, USART3_DmaPowerControl, USART3_DmaSend,    USART3_DmaReceive,
    USART3_DmaTransfer,     USART3_DmaGetTxCount,   USART3_DmaGetRxCount,   USART3_DmaControl, USART3_DmaGetStatus,
#else
    USART3_NonBlockingInitialize,
    USART3_NonBlockingUninitialize,
    USART3_NonBlockingPowerControl,
    USART3_NonBlockingSend,
    USART3_NonBlockingReceive,
    USART3_NonBlockingTransfer,
    USART3_NonBlockingGetTxCount,
    USART3_NonBlockingGetRxCount,
    USART3_NonBlockingControl,
    USART3_NonBlockingGetStatus,
#endif
    USARTx_SetModemControl, USARTx_GetModemStatus};

#endif /* usart3 */
