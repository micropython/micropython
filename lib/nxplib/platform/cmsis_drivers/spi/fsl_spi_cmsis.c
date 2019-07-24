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

#include "fsl_spi_cmsis.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.spi_cmsis"
#endif

#if ((RTE_SPI0 && defined(SPI0)) || (RTE_SPI1 && defined(SPI1)))

#define ARM_SPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 1) /* driver version */

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

extern uint32_t SPI_GetInstance(SPI_Type *base);
static clock_ip_name_t const s_SPIClock[] = SPI_CLOCKS;

typedef const struct _cmsis_spi_resource
{
    SPI_Type *base;
    uint32_t instance;
    uint32_t (*GetFreq)(void);
} cmsis_spi_resource_t;

typedef union _cmsis_spi_handle
{
    spi_master_handle_t masterHandle;
    spi_slave_handle_t slaveHandle;
} cmsis_spi_handle_t;

typedef struct _cmsis_spi_interrupt_driver_state
{
    cmsis_spi_resource_t *resource;
    cmsis_spi_handle_t *handle;
    ARM_SPI_SignalEvent_t cb_event;
    uint32_t baudRate_Bps;
    uint8_t flags; /*!< Control and state flags. */
} cmsis_spi_interrupt_driver_state_t;

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

typedef const struct _cmsis_spi_dma_resource
{
    DMA_Type *txdmaBase;
    uint32_t txdmaChannel;
    DMAMUX_Type *txDmamuxBase;
    uint8_t txDmaRequest;

    DMA_Type *rxdmaBase;
    uint32_t rxdmaChannel;
    DMAMUX_Type *rxDmamuxBase;
    uint8_t rxDmaRequest;

} cmsis_spi_dma_resource_t;

typedef union _cmsis_spi_dma_handle
{
    spi_dma_handle_t masterHandle;
    spi_dma_handle_t slaveHandle;
} cmsis_spi_dma_handle_t;

typedef struct _cmsis_spi_dma_driver_state
{
    cmsis_spi_resource_t *resource;
    cmsis_spi_dma_resource_t *dmaResource;
    cmsis_spi_dma_handle_t *handle;
    dma_handle_t *dmaRxDataHandle;
    dma_handle_t *dmaTxDataHandle;

    uint8_t flags; /*!< Control and state flags. */
    uint32_t baudRate_Bps;
    ARM_SPI_SignalEvent_t event;
} cmsis_SPI_dma_driver_state_t;
#endif

/* Driver Version */
static const ARM_DRIVER_VERSION s_SPIDriverVersion = {ARM_SPI_API_VERSION, ARM_SPI_DRV_VERSION};

/* Driver Capabilities */
static const ARM_SPI_CAPABILITIES s_SPIDriverCapabilities = {
    1, /* Simplex Mode (Master and Slave) */
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface  */
    0  /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
};

/*******************************************************************************
 * Code
 ******************************************************************************/
static inline bool SPI_IsMaster(SPI_Type *base)
{
    return (bool)((base->C1) & SPI_C1_MSTR_MASK);
}
static int32_t SPI_CommonControl(uint32_t control, uint32_t arg, cmsis_spi_resource_t *resource, uint8_t *isConfigured)
{
    spi_master_config_t masterConfig;
    SPI_MasterGetDefaultConfig(&masterConfig);
    spi_slave_config_t slaveConfig;
    SPI_SlaveGetDefaultConfig(&slaveConfig);

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_MASTER_SIMPLEX:
            masterConfig.pinMode = kSPI_PinModeOutput;
            break;
        case ARM_SPI_MODE_SLAVE_SIMPLEX:
            slaveConfig.pinMode = kSPI_PinModeOutput;
            break;
        default:
            break;
    }
    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
        case ARM_SPI_CPOL0_CPHA0:
            if (SPI_IsMaster(resource->base))
            {
                masterConfig.polarity = kSPI_ClockPolarityActiveHigh;

                masterConfig.phase = kSPI_ClockPhaseFirstEdge;
            }
            else
            {
                slaveConfig.polarity = kSPI_ClockPolarityActiveHigh;

                slaveConfig.phase = kSPI_ClockPhaseFirstEdge;
            }
            break;

        case ARM_SPI_CPOL0_CPHA1:
            if (SPI_IsMaster(resource->base))
            {
                masterConfig.polarity = kSPI_ClockPolarityActiveHigh;
                masterConfig.phase    = kSPI_ClockPhaseSecondEdge;
            }
            else
            {
                slaveConfig.polarity = kSPI_ClockPolarityActiveHigh;
                slaveConfig.phase    = kSPI_ClockPhaseSecondEdge;
            }
            break;

        case ARM_SPI_CPOL1_CPHA0:
            if (SPI_IsMaster(resource->base))
            {
                masterConfig.polarity = kSPI_ClockPolarityActiveLow;
                masterConfig.phase    = kSPI_ClockPhaseFirstEdge;
            }
            else
            {
                slaveConfig.polarity = kSPI_ClockPolarityActiveLow;
                slaveConfig.phase    = kSPI_ClockPhaseFirstEdge;
            }
            break;

        case ARM_SPI_CPOL1_CPHA1:
            if (SPI_IsMaster(resource->base))
            {
                masterConfig.polarity = kSPI_ClockPolarityActiveLow;
                masterConfig.phase    = kSPI_ClockPhaseSecondEdge;
            }
            else
            {
                slaveConfig.polarity = kSPI_ClockPolarityActiveLow;
                slaveConfig.phase    = kSPI_ClockPhaseSecondEdge;
            }
            break;

        default:
            break;
    }
#if defined(FSL_FEATURE_SPI_16BIT_TRANSFERS) && FSL_FEATURE_SPI_16BIT_TRANSFERS
    if (control & ARM_SPI_DATA_BITS_Msk)
    {
        if (((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) == 8)
        {
            if (SPI_IsMaster(resource->base))
            {
                masterConfig.dataMode = kSPI_8BitMode;
            }
            else
            {
                slaveConfig.dataMode = kSPI_8BitMode;
            }
        }
        else if (((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) == 16)
        {
            if (SPI_IsMaster(resource->base))
            {
                masterConfig.dataMode = kSPI_16BitMode;
            }
            else
            {
                slaveConfig.dataMode = kSPI_16BitMode;
            }
        }
        else
        {
            return ARM_SPI_ERROR_DATA_BITS;
        }
    }
#endif
    switch (control & ARM_SPI_BIT_ORDER_Msk)
    {
        case ARM_SPI_LSB_MSB:
            if (SPI_IsMaster(resource->base))
            {
                masterConfig.direction = kSPI_LsbFirst;
            }
            else
            {
                slaveConfig.direction = kSPI_LsbFirst;
            }
            break;
        case ARM_SPI_MSB_LSB:
            if (SPI_IsMaster(resource->base))
            {
                masterConfig.direction = kSPI_MsbFirst;
            }
            else
            {
                slaveConfig.direction = kSPI_MsbFirst;
            }

            break;

        default:
            break;
    }
    if (SPI_IsMaster(resource->base))
    {
        switch (control & ARM_SPI_SS_MASTER_MODE_Msk)
        {
            case ARM_SPI_SS_MASTER_UNUSED:
                masterConfig.outputMode = kSPI_SlaveSelectAsGpio; /*!< Slave select pin configured as GPIO. */
                break;

            case ARM_SPI_SS_MASTER_SW:
                return ARM_DRIVER_ERROR_UNSUPPORTED;

            case ARM_SPI_SS_MASTER_HW_OUTPUT:
                masterConfig.outputMode =
                    kSPI_SlaveSelectAutomaticOutput; /*!< Slave select pin configured for automatic SPI output. */
                break;

            case ARM_SPI_SS_MASTER_HW_INPUT:
                masterConfig.outputMode =
                    kSPI_SlaveSelectFaultInput; /*!< Slave select pin configured for fault detection. */
                break;

            default:
                break;
        }
    }

    if (SPI_IsMaster(resource->base))
    {
        if ((*isConfigured) & SPI_FLAG_CONFIGURED)
        {
            SPI_Deinit(resource->base);
        }
        SPI_MasterInit(resource->base, &masterConfig, resource->GetFreq());
        *isConfigured |= SPI_FLAG_CONFIGURED;
    }
    else
    {
        if ((*isConfigured) & SPI_FLAG_CONFIGURED)
        {
            SPI_Deinit(resource->base);
        }
        SPI_SlaveInit(resource->base, &slaveConfig);
        *isConfigured |= SPI_FLAG_CONFIGURED;
    }

    return ARM_DRIVER_OK;
}

static ARM_DRIVER_VERSION SPIx_GetVersion(void)
{
    return s_SPIDriverVersion;
}

static ARM_SPI_CAPABILITIES SPIx_GetCapabilities(void)
{
    return s_SPIDriverCapabilities;
}

#endif

#if (RTE_SPI0_DMA_EN || RTE_SPI1_DMA_EN)

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

void KSDK_SPI_MasterDMACallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_SPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }

    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}
void KSDK_SPI_SlaveDMACallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_SPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

static int32_t SPI_DMAInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_SPI_dma_driver_state_t *SPI)
{
    if (!(SPI->flags & SPI_FLAG_INIT))
    {
        SPI->event = cb_event;
        SPI->flags = SPI_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t SPI_DMAUninitialize(cmsis_SPI_dma_driver_state_t *SPI)
{
    SPI->flags = SPI_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t SPI_DMAPowerControl(ARM_POWER_STATE state, cmsis_SPI_dma_driver_state_t *SPI)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            if (SPI->flags & SPI_FLAG_POWER)
            {
                SPI_Deinit(SPI->resource->base);
                DMAMUX_DisableChannel(SPI->dmaResource->rxDmamuxBase, SPI->dmaResource->rxdmaChannel);
                DMAMUX_DisableChannel(SPI->dmaResource->txDmamuxBase, SPI->dmaResource->txdmaChannel);
                SPI->flags = SPI_FLAG_INIT;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            if (SPI->flags == SPI_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (SPI->flags & SPI_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }
            /* Enable Clock gate */
            CLOCK_EnableClock(s_SPIClock[SPI->resource->instance]);
            SPI->flags |= SPI_FLAG_POWER;
            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

static int32_t SPI_DMASend(const void *data, uint32_t num, cmsis_SPI_dma_driver_state_t *SPI)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer = {0};

    xfer.rxData   = NULL;
    xfer.txData   = (uint8_t *)data;
    xfer.dataSize = num;

    /* If transfer mode is single wire. */
    if (SPI_C2_SPC0_MASK & SPI->resource->base->C2)
    {
        SPI_SetPinMode(SPI->resource->base, kSPI_PinModeOutput);
    }

    if (SPI_IsMaster(SPI->resource->base))
    {
        status = SPI_MasterTransferDMA(SPI->resource->base, &SPI->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferDMA(SPI->resource->base, &SPI->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t SPI_DMAReceive(void *data, uint32_t num, cmsis_SPI_dma_driver_state_t *SPI)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer = {0};

    xfer.txData   = NULL;
    xfer.rxData   = (uint8_t *)data;
    xfer.dataSize = num;

#if defined(FSL_FEATURE_SPI_HAS_FIFO) && (FSL_FEATURE_SPI_HAS_FIFO)
    /* If using DMA, disable FIFO, as the FIFO may cause data loss if the data size is not integer
       times of 2bytes. As SPI cannot set watermark to 0, only can set to 1/2 FIFO size or 3/4 FIFO
       size. */
    if (FSL_FEATURE_SPI_FIFO_SIZEn(SPI->resource->base) != 0)
    {
        SPI->resource->base->C3 &= ~SPI_C3_FIFOMODE_MASK;
    }
#endif /* FSL_FEATURE_SPI_HAS_FIFO */

    /* If transfer mode is single wire. */
    if (SPI_C2_SPC0_MASK & SPI->resource->base->C2)
    {
        SPI_SetPinMode(SPI->resource->base, kSPI_PinModeInput);
    }

    if (SPI_IsMaster(SPI->resource->base))
    {
        status = SPI_MasterTransferDMA(SPI->resource->base, &SPI->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferDMA(SPI->resource->base, &SPI->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t SPI_DMATransfer(const void *data_out, void *data_in, uint32_t num, cmsis_SPI_dma_driver_state_t *SPI)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer = {0};

    if ((NULL == data_in) && (NULL == data_out))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    xfer.txData   = (uint8_t *)data_out;
    xfer.rxData   = (uint8_t *)data_in;
    xfer.dataSize = num;

#if defined(FSL_FEATURE_SPI_HAS_FIFO) && (FSL_FEATURE_SPI_HAS_FIFO)
    /* If using DMA, disable FIFO, as the FIFO may cause data loss if the data size is not integer
       times of 2bytes. As SPI cannot set watermark to 0, only can set to 1/2 FIFO size or 3/4 FIFO
       size. */
    if (FSL_FEATURE_SPI_FIFO_SIZEn(SPI->resource->base) != 0)
    {
        SPI->resource->base->C3 &= ~SPI_C3_FIFOMODE_MASK;
    }
#endif /* FSL_FEATURE_SPI_HAS_FIFO */

    /* If transfer mode is single wire. */
    if (SPI_C2_SPC0_MASK & SPI->resource->base->C2)
    {
        if (NULL == data_out)
        {
            SPI_SetPinMode(SPI->resource->base, kSPI_PinModeInput);
        }
        if (NULL == data_in)
        {
            SPI_SetPinMode(SPI->resource->base, kSPI_PinModeOutput);
        }
    }

    if (SPI_IsMaster(SPI->resource->base))
    {
        status = SPI_MasterTransferDMA(SPI->resource->base, &SPI->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferDMA(SPI->resource->base, &SPI->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}
static uint32_t SPI_DMAGetCount(cmsis_SPI_dma_driver_state_t *SPI)
{
    uint32_t cnt;
    size_t bytes;

    bytes = DMA_GetRemainingBytes(SPI->dmaResource->rxdmaBase, SPI->dmaResource->rxdmaChannel);

    if (SPI_IsMaster(SPI->resource->base))
    {
        cnt = SPI->handle->masterHandle.transferSize - bytes;
    }
    else
    {
        cnt = SPI->handle->slaveHandle.transferSize - bytes;
    }

    return cnt;
}

static int32_t SPI_DMAControl(uint32_t control, uint32_t arg, cmsis_SPI_dma_driver_state_t *SPI)
{
    if (!(SPI->flags & SPI_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    cmsis_spi_dma_resource_t *dmaResource = SPI->dmaResource;

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE:
            SPI_Enable(SPI->resource->base, false);
            break;
        case ARM_SPI_MODE_MASTER:
            SPI->resource->base->C1 |= SPI_C1_MSTR_MASK;

            memset(SPI->dmaRxDataHandle, 0, sizeof(dma_handle_t));
            memset(SPI->dmaTxDataHandle, 0, sizeof(dma_handle_t));

            DMA_CreateHandle(SPI->dmaRxDataHandle, dmaResource->rxdmaBase, dmaResource->rxdmaChannel);
            DMAMUX_SetSource(dmaResource->rxDmamuxBase, dmaResource->rxdmaChannel, dmaResource->rxDmaRequest);
            DMAMUX_EnableChannel(dmaResource->rxDmamuxBase, dmaResource->rxdmaChannel);

            DMA_CreateHandle(SPI->dmaTxDataHandle, dmaResource->txdmaBase, dmaResource->txdmaChannel);
            DMAMUX_SetSource(dmaResource->txDmamuxBase, dmaResource->txdmaChannel, dmaResource->txDmaRequest);
            DMAMUX_EnableChannel(dmaResource->txDmamuxBase, dmaResource->txdmaChannel);

            SPI_MasterTransferCreateHandleDMA(SPI->resource->base, &SPI->handle->masterHandle,
                                              KSDK_SPI_MasterDMACallback, (void *)SPI->event, SPI->dmaTxDataHandle,
                                              SPI->dmaRxDataHandle);

            SPI->baudRate_Bps = arg;
            SPI->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_MODE_SLAVE:
            SPI->resource->base->C1 &= ~SPI_C1_MSTR_MASK;

            memset(SPI->dmaRxDataHandle, 0, sizeof(dma_handle_t));
            memset(SPI->dmaTxDataHandle, 0, sizeof(dma_handle_t));

            DMA_CreateHandle(SPI->dmaTxDataHandle, dmaResource->txdmaBase, dmaResource->txdmaChannel);
            DMAMUX_SetSource(dmaResource->txDmamuxBase, dmaResource->txdmaChannel, dmaResource->txDmaRequest);
            DMAMUX_EnableChannel(dmaResource->txDmamuxBase, dmaResource->txdmaChannel);

            DMA_CreateHandle(SPI->dmaRxDataHandle, dmaResource->rxdmaBase, dmaResource->rxdmaChannel);
            DMAMUX_SetSource(dmaResource->rxDmamuxBase, dmaResource->rxdmaChannel, dmaResource->rxDmaRequest);
            DMAMUX_EnableChannel(dmaResource->rxDmamuxBase, dmaResource->rxdmaChannel);

            SPI_SlaveTransferCreateHandleDMA(SPI->resource->base, &SPI->handle->slaveHandle, KSDK_SPI_SlaveDMACallback,
                                             (void *)SPI->event, SPI->dmaTxDataHandle, SPI->dmaRxDataHandle);

            SPI->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_SET_BUS_SPEED:
            if (!SPI_IsMaster(SPI->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            SPI_MasterSetBaudRate(SPI->resource->base, arg, SPI->resource->GetFreq());

            SPI->baudRate_Bps = arg;
            return ARM_DRIVER_OK;

        case ARM_SPI_GET_BUS_SPEED: /* Set Bus Speed in bps; arg = value */
            if (!SPI_IsMaster(SPI->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            return SPI->baudRate_Bps;

        case ARM_SPI_CONTROL_SS:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_ABORT_TRANSFER:
            if (SPI_IsMaster(SPI->resource->base))
            {
                SPI_MasterTransferAbortDMA(SPI->resource->base, &SPI->handle->masterHandle);
            }
            else
            {
                SPI_SlaveTransferAbortDMA(SPI->resource->base, &SPI->handle->slaveHandle);
            }
            return ARM_DRIVER_OK;
        case ARM_SPI_MODE_MASTER_SIMPLEX:
            SPI->resource->base->C1 |= SPI_C1_MSTR_MASK;

            memset(SPI->dmaRxDataHandle, 0, sizeof(dma_handle_t));
            memset(SPI->dmaTxDataHandle, 0, sizeof(dma_handle_t));

            DMA_CreateHandle(SPI->dmaRxDataHandle, dmaResource->rxdmaBase, dmaResource->rxdmaChannel);
            DMAMUX_SetSource(dmaResource->rxDmamuxBase, dmaResource->rxdmaChannel, dmaResource->rxDmaRequest);
            DMAMUX_EnableChannel(dmaResource->rxDmamuxBase, dmaResource->rxdmaChannel);

            DMA_CreateHandle(SPI->dmaTxDataHandle, dmaResource->txdmaBase, dmaResource->txdmaChannel);
            DMAMUX_SetSource(dmaResource->txDmamuxBase, dmaResource->txdmaChannel, dmaResource->txDmaRequest);
            DMAMUX_EnableChannel(dmaResource->txDmamuxBase, dmaResource->txdmaChannel);

            SPI_MasterTransferCreateHandleDMA(SPI->resource->base, &SPI->handle->masterHandle,
                                              KSDK_SPI_MasterDMACallback, (void *)SPI->event, SPI->dmaTxDataHandle,
                                              SPI->dmaRxDataHandle);

            SPI->baudRate_Bps = arg;
            SPI->flags |= SPI_FLAG_CONFIGURED;
            break;
        case ARM_SPI_MODE_SLAVE_SIMPLEX:
            SPI->resource->base->C1 &= ~SPI_C1_MSTR_MASK;

            memset(SPI->dmaRxDataHandle, 0, sizeof(dma_handle_t));
            memset(SPI->dmaTxDataHandle, 0, sizeof(dma_handle_t));

            DMA_CreateHandle(SPI->dmaTxDataHandle, dmaResource->txdmaBase, dmaResource->txdmaChannel);
            DMAMUX_SetSource(dmaResource->txDmamuxBase, dmaResource->txdmaChannel, dmaResource->txDmaRequest);
            DMAMUX_EnableChannel(dmaResource->txDmamuxBase, dmaResource->txdmaChannel);

            DMA_CreateHandle(SPI->dmaRxDataHandle, dmaResource->rxdmaBase, dmaResource->rxdmaChannel);
            DMAMUX_SetSource(dmaResource->rxDmamuxBase, dmaResource->rxdmaChannel, dmaResource->rxDmaRequest);
            DMAMUX_EnableChannel(dmaResource->rxDmamuxBase, dmaResource->rxdmaChannel);

            SPI_SlaveTransferCreateHandleDMA(SPI->resource->base, &SPI->handle->slaveHandle, KSDK_SPI_SlaveDMACallback,
                                             (void *)SPI->event, SPI->dmaTxDataHandle, SPI->dmaRxDataHandle);

            SPI->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_SET_DEFAULT_TX_VALUE: /* Set default Transmit value; arg = value */
            SPI_SetDummyData(SPI->resource->base, (uint8_t)arg);
            return ARM_DRIVER_OK;

        default:
            break;
    }

    return SPI_CommonControl(control, arg, SPI->resource, &SPI->flags);
}

ARM_SPI_STATUS SPI_DMAGetStatus(cmsis_SPI_dma_driver_state_t *SPI)
{
    ARM_SPI_STATUS stat;

    if (SPI_IsMaster(SPI->resource->base))
    {
        stat.busy =
            ((SPI->handle->masterHandle.txInProgress == true) || (SPI->handle->masterHandle.rxInProgress == true)) ?
                (0U) :
                (1U);
    }
    else
    {
        stat.busy =
            ((SPI->handle->slaveHandle.txInProgress == true) || (SPI->handle->slaveHandle.rxInProgress == true)) ?
                (0U) :
                (1U);
    }
#if defined(FSL_FEATURE_SPI_HAS_FIFO) && FSL_FEATURE_SPI_HAS_FIFO
    stat.data_lost = ((SPI_GetStatusFlags(SPI->resource->base) & kSPI_RxOverflow) ||
                      (SPI_GetStatusFlags(SPI->resource->base) & kSPI_TxOverflow)) ?
                         (1U) :
                         (0U);
#else
    stat.data_lost = (kStatus_SPI_Error == SPI->handle->slaveHandle.state) ? (1U) : (0U);
#endif

    stat.mode_fault = 0U;
    stat.reserved   = 0U;

    return stat;
}
#endif /* defined(FSL_FEATURE_SOC_DMA_COUNT) */

#endif

#if ((RTE_SPI0 && !RTE_SPI0_DMA_EN) || (RTE_SPI1 && !RTE_SPI1_DMA_EN))

void KSDK_SPI_MasterInterruptCallback(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if ((kStatus_Success == status) || (kStatus_SPI_Idle == status))
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_SPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }

    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

void KSDK_SPI_SlaveInterruptCallback(SPI_Type *base, spi_slave_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if ((kStatus_Success == status) || (kStatus_SPI_Idle == status))
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_SPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }

    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

static int32_t SPI_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_spi_interrupt_driver_state_t *SPI)
{
    if (!(SPI->flags & SPI_FLAG_INIT))
    {
        SPI->cb_event = cb_event;
        SPI->flags    = SPI_FLAG_INIT;
    }
    return ARM_DRIVER_OK;
}

static int32_t SPI_InterruptUninitialize(cmsis_spi_interrupt_driver_state_t *SPI)
{
    SPI->flags = SPI_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t SPI_InterruptPowerControl(ARM_POWER_STATE state, cmsis_spi_interrupt_driver_state_t *SPI)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            if (SPI->flags & SPI_FLAG_POWER)
            {
                SPI_Deinit(SPI->resource->base);
                SPI->flags = SPI_FLAG_INIT;
            }
            break;

        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_POWER_FULL:
            if (SPI->flags == SPI_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (SPI->flags & SPI_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            CLOCK_EnableClock(s_SPIClock[SPI->resource->instance]);
            SPI->flags |= SPI_FLAG_POWER;
            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

static int32_t SPI_InterruptSend(const void *data, uint32_t num, cmsis_spi_interrupt_driver_state_t *SPI)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer = {0};

    xfer.rxData   = NULL;
    xfer.txData   = (uint8_t *)data;
    xfer.dataSize = num;

#if defined(FSL_FEATURE_SPI_HAS_FIFO) && (FSL_FEATURE_SPI_HAS_FIFO)
    /* If using DMA, disable FIFO, as the FIFO may cause data loss if the data size is not integer
       times of 2bytes. As SPI cannot set watermark to 0, only can set to 1/2 FIFO size or 3/4 FIFO
       size. */
    if (FSL_FEATURE_SPI_FIFO_SIZEn(SPI->resource->base) != 0)
    {
        SPI->resource->base->C3 &= ~SPI_C3_FIFOMODE_MASK;
    }
#endif /* FSL_FEATURE_SPI_HAS_FIFO */

    /* If transfer mode is single wire. */
    if (SPI_C2_SPC0_MASK & SPI->resource->base->C2)
    {
        SPI_SetPinMode(SPI->resource->base, kSPI_PinModeOutput);
    }

    if (SPI_IsMaster(SPI->resource->base))
    {
        status = SPI_MasterTransferNonBlocking(SPI->resource->base, &SPI->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferNonBlocking(SPI->resource->base, &SPI->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t SPI_InterruptReceive(void *data, uint32_t num, cmsis_spi_interrupt_driver_state_t *SPI)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer = {0};

    xfer.txData   = NULL;
    xfer.rxData   = (uint8_t *)data;
    xfer.dataSize = num;

    /* If transfer mode is single wire. */
    if (SPI_C2_SPC0_MASK & SPI->resource->base->C2)
    {
        SPI_SetPinMode(SPI->resource->base, kSPI_PinModeInput);
    }

    if (SPI_IsMaster(SPI->resource->base))
    {
        status = SPI_MasterTransferNonBlocking(SPI->resource->base, &SPI->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferNonBlocking(SPI->resource->base, &SPI->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t SPI_InterruptTransfer(const void *data_out,
                                     void *data_in,
                                     uint32_t num,
                                     cmsis_spi_interrupt_driver_state_t *SPI)
{
    int32_t ret;
    status_t status;
    spi_transfer_t xfer = {0};

    if ((NULL == data_in) && (NULL == data_out))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    xfer.txData   = (uint8_t *)data_out;
    xfer.rxData   = (uint8_t *)data_in;
    xfer.dataSize = num;

    /* If transfer mode is single wire. */
    if (SPI_C2_SPC0_MASK & SPI->resource->base->C2)
    {
        if (NULL == data_out)
        {
            SPI_SetPinMode(SPI->resource->base, kSPI_PinModeInput);
        }
        if (NULL == data_in)
        {
            SPI_SetPinMode(SPI->resource->base, kSPI_PinModeOutput);
        }
    }

    if (SPI_IsMaster(SPI->resource->base))
    {
        status = SPI_MasterTransferNonBlocking(SPI->resource->base, &SPI->handle->masterHandle, &xfer);
    }
    else
    {
        status = SPI_SlaveTransferNonBlocking(SPI->resource->base, &SPI->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_SPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}
static uint32_t SPI_InterruptGetCount(cmsis_spi_interrupt_driver_state_t *SPI)
{
    if (SPI_IsMaster(SPI->resource->base))
    {
        return SPI->handle->masterHandle.transferSize - SPI->handle->masterHandle.rxRemainingBytes;
    }
    else
    {
        return SPI->handle->slaveHandle.transferSize - SPI->handle->slaveHandle.rxRemainingBytes;
    }
}

static int32_t SPI_InterruptControl(uint32_t control, uint32_t arg, cmsis_spi_interrupt_driver_state_t *SPI)
{
    if (!(SPI->flags & SPI_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE: /* SPI Inactive */
            SPI_Enable(SPI->resource->base, false);
            break;

        case ARM_SPI_MODE_MASTER: /* SPI Master (Output on MOSI, Input on MISO); arg = Bus Speed in bps */
            SPI->resource->base->C1 |= SPI_C1_MSTR_MASK;
            SPI_MasterTransferCreateHandle(SPI->resource->base, &SPI->handle->masterHandle,
                                           KSDK_SPI_MasterInterruptCallback, (void *)SPI->cb_event);
            SPI->baudRate_Bps = arg;
            SPI->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_MODE_SLAVE: /* SPI Slave  (Output on MISO, Input on MOSI) */
            SPI->resource->base->C1 &= ~SPI_C1_MSTR_MASK;
            SPI_SlaveTransferCreateHandle(SPI->resource->base, &SPI->handle->slaveHandle,
                                          KSDK_SPI_SlaveInterruptCallback, (void *)SPI->cb_event);
            SPI->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_GET_BUS_SPEED: /* Get Bus Speed in bps */
            if (!SPI_IsMaster(SPI->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            return SPI->baudRate_Bps;

        case ARM_SPI_SET_BUS_SPEED: /* Set Bus Speed in bps; */
            if (!SPI_IsMaster(SPI->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            SPI_MasterSetBaudRate(SPI->resource->base, arg, SPI->resource->GetFreq());

            SPI->baudRate_Bps = arg;
            return ARM_DRIVER_OK;

        case ARM_SPI_CONTROL_SS: /* Control Slave Select; arg = 0:inactive, 1:active  */
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_ABORT_TRANSFER: /* Abort current data transfer */
            if (SPI_IsMaster(SPI->resource->base))
            {
                SPI_MasterTransferAbort(SPI->resource->base, &SPI->handle->masterHandle);
            }
            else
            {
                SPI_SlaveTransferAbort(SPI->resource->base, &SPI->handle->slaveHandle);
            }
            return ARM_DRIVER_OK;

        case ARM_SPI_MODE_MASTER_SIMPLEX: /* SPI Master (Output/Input on MOSI); arg = Bus Speed in bps */
            SPI->resource->base->C1 |= SPI_C1_MSTR_MASK;
            SPI_MasterTransferCreateHandle(SPI->resource->base, &SPI->handle->masterHandle,
                                           KSDK_SPI_MasterInterruptCallback, (void *)SPI->cb_event);
            SPI->baudRate_Bps = arg;
            SPI->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_MODE_SLAVE_SIMPLEX: /* SPI Slave  (Output/Input on MISO) */
            SPI->resource->base->C1 &= ~SPI_C1_MSTR_MASK;
            SPI_SlaveTransferCreateHandle(SPI->resource->base, &SPI->handle->slaveHandle,
                                          KSDK_SPI_SlaveInterruptCallback, (void *)SPI->cb_event);
            SPI->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_SET_DEFAULT_TX_VALUE: /* Set default Transmit value; arg = value */
            SPI_SetDummyData(SPI->resource->base, (uint8_t)arg);
            return ARM_DRIVER_OK;

        default:
            break;
    }
    return SPI_CommonControl(control, arg, SPI->resource, &SPI->flags);
}

ARM_SPI_STATUS SPI_InterruptGetStatus(cmsis_spi_interrupt_driver_state_t *SPI)
{
    ARM_SPI_STATUS stat;

    if (SPI_IsMaster(SPI->resource->base))
    {
        stat.busy =
            ((SPI->handle->masterHandle.txRemainingBytes > 0) || (SPI->handle->masterHandle.rxRemainingBytes > 0)) ?
                (0U) :
                (1U);
    }
    else
    {
        stat.busy =
            ((SPI->handle->slaveHandle.txRemainingBytes > 0) || (SPI->handle->slaveHandle.rxRemainingBytes > 0)) ?
                (0U) :
                (1U);
    }
#if defined(FSL_FEATURE_SPI_HAS_FIFO) && FSL_FEATURE_SPI_HAS_FIFO
    stat.data_lost = ((SPI_GetStatusFlags(SPI->resource->base) & kSPI_RxOverflow) ||
                      (SPI_GetStatusFlags(SPI->resource->base) & kSPI_TxOverflow)) ?
                         (1U) :
                         (0U);
#else
    stat.data_lost = (kStatus_SPI_Error == SPI->handle->slaveHandle.state) ? (1U) : (0U);
#endif
    stat.mode_fault = 0U;
    stat.reserved   = 0U;

    return stat;
}

#endif

#if defined(SPI0) && RTE_SPI0

extern void SPI0_InitPins(void);
extern void SPI0_DeinitPins(void);
/* User needs to provide the implementation for SPI0_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t SPI0_GetFreq(void);

cmsis_spi_resource_t SPI0_Resource = {SPI0, 0, SPI0_GetFreq};

#if RTE_SPI0_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI0_DMAResource = {
    RTE_SPI0_DMA_TX_DMA_BASE, RTE_SPI0_DMA_TX_CH, RTE_SPI0_DMA_TX_DMAMUX_BASE, RTE_SPI0_DMA_TX_PERI_SEL,

    RTE_SPI0_DMA_RX_DMA_BASE, RTE_SPI0_DMA_RX_CH, RTE_SPI0_DMA_RX_DMAMUX_BASE, RTE_SPI0_DMA_RX_PERI_SEL,

};

cmsis_spi_dma_handle_t SPI0_DmaHandle;
dma_handle_t SPI0_DmaTxDataHandle;
dma_handle_t SPI0_DmaRxDataHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("spi0_d_m_a_driver_state")
cmsis_SPI_dma_driver_state_t SPI0_DMADriverState = {
#else
cmsis_SPI_dma_driver_state_t SPI0_DMADriverState = {
#endif
    &SPI0_Resource, &SPI0_DMAResource, &SPI0_DmaHandle, &SPI0_DmaRxDataHandle, &SPI0_DmaTxDataHandle,

};

static int32_t SPI0_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI0_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI0_DMADriverState);
}

static int32_t SPI0_DMAUninitialize(void)
{
    SPI0_DeinitPins();
    return SPI_DMAUninitialize(&SPI0_DMADriverState);
}

static int32_t SPI0_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI0_DMADriverState);
}

static int32_t SPI0_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI0_DMADriverState);
}

static int32_t SPI0_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI0_DMADriverState);
}

static int32_t SPI0_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI0_DMADriverState);
}

static uint32_t SPI0_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI0_DMADriverState);
}

static int32_t SPI0_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI0_DMADriverState);
}

static ARM_SPI_STATUS SPI0_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI0_DMADriverState);
}

#endif

#else

cmsis_spi_handle_t SPI0_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("spi0_interrupt_driver_state")
cmsis_spi_interrupt_driver_state_t SPI0_InterruptDriverState = {
#else
cmsis_spi_interrupt_driver_state_t SPI0_InterruptDriverState = {
#endif
    &SPI0_Resource,
    &SPI0_Handle,
};

static int32_t SPI0_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI0_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptUninitialize(void)
{
    SPI0_DeinitPins();
    return SPI_InterruptUninitialize(&SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI0_InterruptDriverState);
}

static uint32_t SPI0_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI0_InterruptDriverState);
}

static int32_t SPI0_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI0_InterruptDriverState);
}

static ARM_SPI_STATUS SPI0_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI0_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI0 = {SPIx_GetVersion,    SPIx_GetCapabilities,
#if RTE_SPI0_DMA_EN
                              SPI0_DMAInitialize, SPI0_DMAUninitialize, SPI0_DMAPowerControl, SPI0_DMASend,
                              SPI0_DMAReceive,    SPI0_DMATransfer,     SPI0_DMAGetCount,     SPI0_DMAControl,
                              SPI0_DMAGetStatus
#else
                              SPI0_InterruptInitialize,
                              SPI0_InterruptUninitialize,
                              SPI0_InterruptPowerControl,
                              SPI0_InterruptSend,
                              SPI0_InterruptReceive,
                              SPI0_InterruptTransfer,
                              SPI0_InterruptGetCount,
                              SPI0_InterruptControl,
                              SPI0_InterruptGetStatus
#endif

};

#endif /*  SPI0  */

#if defined(SPI1) && RTE_SPI1

extern void SPI1_InitPins(void);
extern void SPI1_DeinitPins(void);
/* User needs to provide the implementation for SPI1_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t SPI1_GetFreq(void);

cmsis_spi_resource_t SPI1_Resource = {SPI1, 1, SPI1_GetFreq};

#if RTE_SPI1_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
cmsis_spi_dma_resource_t SPI1_DMAResource = {
    RTE_SPI1_DMA_TX_DMA_BASE, RTE_SPI1_DMA_TX_CH, RTE_SPI1_DMA_TX_DMAMUX_BASE, RTE_SPI1_DMA_TX_PERI_SEL,

    RTE_SPI1_DMA_RX_DMA_BASE, RTE_SPI1_DMA_RX_CH, RTE_SPI1_DMA_RX_DMAMUX_BASE, RTE_SPI1_DMA_RX_PERI_SEL,

};

cmsis_spi_dma_handle_t SPI1_DmaHandle;
dma_handle_t SPI1_DmaTxDataHandle;
dma_handle_t SPI1_DmaRxDataHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("spi1_d_m_a_driver_state")
cmsis_SPI_dma_driver_state_t SPI1_DMADriverState = {
#else
cmsis_SPI_dma_driver_state_t SPI1_DMADriverState = {
#endif
    &SPI1_Resource, &SPI1_DMAResource, &SPI1_DmaHandle, &SPI1_DmaRxDataHandle, &SPI1_DmaTxDataHandle,
};

static int32_t SPI1_DMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI1_InitPins();
    return SPI_DMAInitialize(cb_event, &SPI1_DMADriverState);
}

static int32_t SPI1_DMAUninitialize(void)
{
    SPI1_DeinitPins();
    return SPI_DMAUninitialize(&SPI1_DMADriverState);
}

static int32_t SPI1_DMAPowerControl(ARM_POWER_STATE state)
{
    return SPI_DMAPowerControl(state, &SPI1_DMADriverState);
}

static int32_t SPI1_DMASend(const void *data, uint32_t num)
{
    return SPI_DMASend(data, num, &SPI1_DMADriverState);
}

static int32_t SPI1_DMAReceive(void *data, uint32_t num)
{
    return SPI_DMAReceive(data, num, &SPI1_DMADriverState);
}

static int32_t SPI1_DMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_DMATransfer(data_out, data_in, num, &SPI1_DMADriverState);
}

static uint32_t SPI1_DMAGetCount(void)
{
    return SPI_DMAGetCount(&SPI1_DMADriverState);
}

static int32_t SPI1_DMAControl(uint32_t control, uint32_t arg)
{
    return SPI_DMAControl(control, arg, &SPI1_DMADriverState);
}

static ARM_SPI_STATUS SPI1_DMAGetStatus(void)
{
    return SPI_DMAGetStatus(&SPI1_DMADriverState);
}

#endif

#else

cmsis_spi_handle_t SPI1_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("spi1_interrupt_driver_state")
cmsis_spi_interrupt_driver_state_t SPI1_InterruptDriverState = {
#else
cmsis_spi_interrupt_driver_state_t SPI1_InterruptDriverState = {
#endif
    &SPI1_Resource,
    &SPI1_Handle,
};

static int32_t SPI1_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    SPI1_InitPins();
    return SPI_InterruptInitialize(cb_event, &SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptUninitialize(void)
{
    SPI1_DeinitPins();
    return SPI_InterruptUninitialize(&SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptPowerControl(ARM_POWER_STATE state)
{
    return SPI_InterruptPowerControl(state, &SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptSend(const void *data, uint32_t num)
{
    return SPI_InterruptSend(data, num, &SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptReceive(void *data, uint32_t num)
{
    return SPI_InterruptReceive(data, num, &SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return SPI_InterruptTransfer(data_out, data_in, num, &SPI1_InterruptDriverState);
}

static uint32_t SPI1_InterruptGetCount(void)
{
    return SPI_InterruptGetCount(&SPI1_InterruptDriverState);
}

static int32_t SPI1_InterruptControl(uint32_t control, uint32_t arg)
{
    return SPI_InterruptControl(control, arg, &SPI1_InterruptDriverState);
}

static ARM_SPI_STATUS SPI1_InterruptGetStatus(void)
{
    return SPI_InterruptGetStatus(&SPI1_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI1 = {SPIx_GetVersion,    SPIx_GetCapabilities,
#if RTE_SPI1_DMA_EN
                              SPI1_DMAInitialize, SPI1_DMAUninitialize, SPI1_DMAPowerControl, SPI1_DMASend,
                              SPI1_DMAReceive,    SPI1_DMATransfer,     SPI1_DMAGetCount,     SPI1_DMAControl,
                              SPI1_DMAGetStatus
#else
                              SPI1_InterruptInitialize,
                              SPI1_InterruptUninitialize,
                              SPI1_InterruptPowerControl,
                              SPI1_InterruptSend,
                              SPI1_InterruptReceive,
                              SPI1_InterruptTransfer,
                              SPI1_InterruptGetCount,
                              SPI1_InterruptControl,
                              SPI1_InterruptGetStatus
#endif

};

#endif /*  SPI1  */
