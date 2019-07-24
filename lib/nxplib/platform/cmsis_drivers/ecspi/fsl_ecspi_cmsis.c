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

#include "fsl_ecspi_cmsis.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.ecspi_cmsis"
#endif

#if (RTE_SPI1 || RTE_SPI2 || RTE_SPI3)

#define ARM_SPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0) /* driver version */

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

typedef const struct _cmsis_ecspi_resource
{
    ECSPI_Type *base;
    uint32_t instance;
    uint32_t (*GetFreq)(void);
} cmsis_ecspi_resource_t;

#if (RTE_SPI1_DMA_EN || RTE_SPI2_DMA_EN || RTE_SPI3_DMA_EN)
#if (defined(FSL_FEATURE_SOC_SDMA_COUNT) && FSL_FEATURE_SOC_SDMA_COUNT)
typedef const struct _cmsis_ecspi_sdma_resource
{
    SDMAARM_Type *txSdmaBase;
    uint32_t txSdmaChannel;
    uint32_t txSdmaRequest;
    uint32_t txSdmaPriority;
    sdma_context_data_t *txSdmaContext;

    SDMAARM_Type *rxSdmaBase;
    uint32_t rxSdmaChannel;
    uint32_t rxSdmaRequest;
    uint32_t rxSdmaPriority;
    sdma_context_data_t *rxSdmaContext;
} cmsis_ecspi_sdma_resource_t;

typedef struct _cmsis_ecspi_sdma_driver_state
{
    cmsis_ecspi_resource_t *resource;
    cmsis_ecspi_sdma_resource_t *sdmaResource;
    ecspi_sdma_handle_t *handle;
    sdma_handle_t *sdmaRxDataHandle;
    sdma_handle_t *sdmaTxDataHandle;

    uint32_t baudRate_Bps;
    ARM_SPI_SignalEvent_t cb_event;
    uint8_t flags; /*!< Control and state flags. */
} cmsis_ecspi_sdma_driver_state_t;
#endif
#else /* SDMA unused. */
typedef struct _cmsis_ecspi_interrupt_driver_state
{
    cmsis_ecspi_resource_t *resource;
    ecspi_master_handle_t *handle;
    ARM_SPI_SignalEvent_t cb_event;
    uint32_t baudRate_Bps;
    uint8_t flags; /*!< Control and state flags. */
} cmsis_ecspi_interrupt_driver_state_t;
#endif

/* Driver Version */
static const ARM_DRIVER_VERSION s_ECSPIDriverVersion = {ARM_SPI_API_VERSION, ARM_SPI_DRV_VERSION};

/* Driver Capabilities */
static const ARM_SPI_CAPABILITIES s_ECSPIDriverCapabilities = {
    1, /* Simplex Mode (Master and Slave) */
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface  */
    0  /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
};

/*******************************************************************************
 * Code
 ******************************************************************************/

void ECSPI_MasterCommonControl(uint32_t control, cmsis_ecspi_resource_t *resource, ecspi_master_config_t *masterConfig)
{
    switch (control & ARM_SPI_SS_MASTER_MODE_Msk)
    {
        /*
         * Note:
         * ARM_SPI_SS_MASTER_HW_OUTPUT is default configuration in driver, if ARM_SPI_SS_MASTER_UNUSED or
         * ARM_SPI_SS_MASTER_SW is wanted, please disable pin function in SPIx_InitPins() which is configured
         * by user in extern file. Besides, ARM_SPI_SS_MASTER_HW_INPUT is not supported in this driver.
         */
        case ARM_SPI_SS_MASTER_UNUSED: /*!< SPI Slave Select when Master: Not used */
            break;
        case ARM_SPI_SS_MASTER_SW: /*!< SPI Slave Select when Master: Software controlled. */
            break;
        case ARM_SPI_SS_MASTER_HW_OUTPUT: /*!< SPI Slave Select when Master: Hardware controlled Output */
            break;
        case ARM_SPI_SS_MASTER_HW_INPUT: /*!< SPI Slave Select when Master: Hardware monitored Input */
            break;
        default:
            break;
    }

    /* Configure the channel to be used. Default channel is channel0. */
    switch (resource->instance)
    {
        case 1:
#if defined(RTE_SPI1_TRANSFER_CHANNEL)
            masterConfig->channel = RTE_SPI1_TRANSFER_CHANNEL;
#endif
            break;
        case 2:
#if defined(RTE_SPI2_TRANSFER_CHANNEL)
            masterConfig->channel = RTE_SPI2_TRANSFER_CHANNEL;
#endif
            break;
        case 3:
#if defined(RTE_SPI3_TRANSFER_CHANNEL)
            masterConfig->channel = RTE_SPI3_TRANSFER_CHANNEL;
#endif
            break;
        default:
            break;
    }

    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
        case ARM_SPI_CPOL0_CPHA0:
            masterConfig->channelConfig.polarity = kECSPI_PolarityActiveHigh;
            masterConfig->channelConfig.phase    = kECSPI_ClockPhaseFirstEdge;
            break;

        case ARM_SPI_CPOL0_CPHA1:
            masterConfig->channelConfig.polarity = kECSPI_PolarityActiveHigh;
            masterConfig->channelConfig.phase    = kECSPI_ClockPhaseSecondEdge;
            break;

        case ARM_SPI_CPOL1_CPHA0:
            masterConfig->channelConfig.polarity = kECSPI_PolarityActiveLow;
            masterConfig->channelConfig.phase    = kECSPI_ClockPhaseFirstEdge;
            break;

        case ARM_SPI_CPOL1_CPHA1:
            masterConfig->channelConfig.polarity = kECSPI_PolarityActiveLow;
            masterConfig->channelConfig.phase    = kECSPI_ClockPhaseSecondEdge;
            break;
        default:
            break;
    }
    /* setting Number of burst length. */
    if (control & ARM_SPI_DATA_BITS_Msk)
    {
        masterConfig->burstLength = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);
    }
}

void ECSPI_SlaveCommonControl(uint32_t control, cmsis_ecspi_resource_t *resource, ecspi_slave_config_t *slaveConfig)
{
    /* The SPI slave select is controlled by hardware, software mode is not supported by current driver. */
    switch (control & ARM_SPI_SS_SLAVE_MODE_Msk)
    {
        case ARM_SPI_SS_SLAVE_HW:
            break;
        case ARM_SPI_SS_SLAVE_SW:
            break;
        default:
            break;
    }

    /* Configure the channel to be used. Default channel is channel0. */
    switch (resource->instance)
    {
        case 1:
#if defined(RTE_SPI1_TRANSFER_CHANNEL)
            slaveConfig->channel = RTE_SPI1_TRANSFER_CHANNEL;
#endif
            break;
        case 2:
#if defined(RTE_SPI2_TRANSFER_CHANNEL)
            slaveConfig->channel = RTE_SPI2_TRANSFER_CHANNEL;
#endif
            break;
        case 3:
#if defined(RTE_SPI3_TRANSFER_CHANNEL)
            slaveConfig->channel = RTE_SPI3_TRANSFER_CHANNEL;
#endif
            break;
        default:
            break;
    }

    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
        case ARM_SPI_CPOL0_CPHA0:
            slaveConfig->channelConfig.polarity = kECSPI_PolarityActiveHigh;
            slaveConfig->channelConfig.phase    = kECSPI_ClockPhaseFirstEdge;
            break;

        case ARM_SPI_CPOL0_CPHA1:
            slaveConfig->channelConfig.polarity = kECSPI_PolarityActiveHigh;
            slaveConfig->channelConfig.phase    = kECSPI_ClockPhaseSecondEdge;
            break;

        case ARM_SPI_CPOL1_CPHA0:
            slaveConfig->channelConfig.polarity = kECSPI_PolarityActiveLow;
            slaveConfig->channelConfig.phase    = kECSPI_ClockPhaseFirstEdge;
            break;

        case ARM_SPI_CPOL1_CPHA1:
            slaveConfig->channelConfig.polarity = kECSPI_PolarityActiveLow;
            slaveConfig->channelConfig.phase    = kECSPI_ClockPhaseSecondEdge;
            break;
        default:
            break;
    }

    if (control & ARM_SPI_DATA_BITS_Msk) /* setting Number of Data bits */
    {
        slaveConfig->burstLength = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);
    }
}

static ARM_DRIVER_VERSION ECSPIx_GetVersion(void)
{
    return s_ECSPIDriverVersion;
}

static ARM_SPI_CAPABILITIES ECSPIx_GetCapabilities(void)
{
    return s_ECSPIDriverCapabilities;
}

#endif

#if (RTE_SPI1_DMA_EN || RTE_SPI2_DMA_EN || RTE_SPI3_DMA_EN)

#if (defined(FSL_FEATURE_SOC_SDMA_COUNT) && FSL_FEATURE_SOC_SDMA_COUNT)

void KSDK_ECSPI_MasterSDMACallback(ECSPI_Type *base, ecspi_sdma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}
void KSDK_ECSPI_SlaveSDMACallback(ECSPI_Type *base, ecspi_sdma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

static int32_t ECSPI_SDMAInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_ecspi_sdma_driver_state_t *ecspi)
{
    if (!(ecspi->flags & SPI_FLAG_INIT))
    {
        ecspi->cb_event = cb_event;
        ecspi->flags    = SPI_FLAG_INIT;
    }
    return ARM_DRIVER_OK;
}

static int32_t ECSPI_SDMAUninitialize(cmsis_ecspi_sdma_driver_state_t *ecspi)
{
    ecspi->flags = SPI_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t ECSPI_SDMAPowerControl(ARM_POWER_STATE state, cmsis_ecspi_sdma_driver_state_t *ecspi)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            if (ecspi->flags & SPI_FLAG_POWER)
            {
                ECSPI_Deinit(ecspi->resource->base);
                ecspi->flags &= ~SPI_FLAG_POWER;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            if (ecspi->flags == SPI_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }
            if (ecspi->flags & SPI_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }
            ecspi->flags |= SPI_FLAG_POWER;

            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

static int32_t ECSPI_SDMASend(const void *data, uint32_t num, cmsis_ecspi_sdma_driver_state_t *ecspi)
{
    int32_t ret;
    status_t status;
    ecspi_transfer_t xfer = {0};

    xfer.rxData   = NULL;
    xfer.txData   = (uint32_t *)data;
    xfer.dataSize = num;
    xfer.channel  = kECSPI_Channel0;

    /* Configure the channel to be used. */
    switch (ecspi->resource->instance)
    {
        case 1:
#if defined(RTE_SPI1_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI1_TRANSFER_CHANNEL;
#endif
            break;
        case 2:
#if defined(RTE_SPI2_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI2_TRANSFER_CHANNEL;
#endif
            break;
        case 3:
#if defined(RTE_SPI3_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI3_TRANSFER_CHANNEL;
#endif
            break;
        default:
            break;
    }

    if (ecspi->flags & SPI_FLAG_MASTER)
    {
        status = ECSPI_MasterTransferSDMA(ecspi->resource->base, ecspi->handle, &xfer);
    }
    else
    {
        status = ECSPI_SlaveTransferSDMA(ecspi->resource->base, ecspi->handle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_ECSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t ECSPI_SDMAReceive(void *data, uint32_t num, cmsis_ecspi_sdma_driver_state_t *ecspi)
{
    int32_t ret;
    status_t status;
    ecspi_transfer_t xfer = {0};

    xfer.txData   = NULL;
    xfer.rxData   = (uint32_t *)data;
    xfer.dataSize = num;
    xfer.channel  = kECSPI_Channel0;

    /* Configure the channel to be used. */
    switch (ecspi->resource->instance)
    {
        case 1:
#if defined(RTE_SPI1_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI1_TRANSFER_CHANNEL;
#endif
            break;
        case 2:
#if defined(RTE_SPI2_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI2_TRANSFER_CHANNEL;
#endif
            break;
        case 3:
#if defined(RTE_SPI3_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI3_TRANSFER_CHANNEL;
#endif
            break;
        default:
            break;
    }

    if (ecspi->flags & SPI_FLAG_MASTER)
    {
        status = ECSPI_MasterTransferSDMA(ecspi->resource->base, ecspi->handle, &xfer);
    }
    else
    {
        status = ECSPI_SlaveTransferSDMA(ecspi->resource->base, ecspi->handle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_ECSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t ECSPI_SDMATransfer(const void *data_out,
                                  void *data_in,
                                  uint32_t num,
                                  cmsis_ecspi_sdma_driver_state_t *ecspi)
{
    int32_t ret;
    status_t status;
    ecspi_transfer_t xfer = {0};

    xfer.txData   = (uint32_t *)data_out;
    xfer.rxData   = (uint32_t *)data_in;
    xfer.dataSize = num;
    xfer.channel  = kECSPI_Channel0;

    /* Configure the channel to be used. */
    switch (ecspi->resource->instance)
    {
        case 1:
#if defined(RTE_SPI1_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI1_TRANSFER_CHANNEL;
#endif
            break;
        case 2:
#if defined(RTE_SPI2_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI2_TRANSFER_CHANNEL;
#endif
            break;
        case 3:
#if defined(RTE_SPI3_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI3_TRANSFER_CHANNEL;
#endif
            break;
        default:
            break;
    }

    if (ecspi->flags & SPI_FLAG_MASTER)
    {
        status = ECSPI_MasterTransferSDMA(ecspi->resource->base, ecspi->handle, &xfer);
    }
    else
    {
        status = ECSPI_SlaveTransferSDMA(ecspi->resource->base, ecspi->handle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_ECSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}
static uint32_t ECSPI_SDMAGetCount(cmsis_ecspi_sdma_driver_state_t *ecspi)
{
    /* Unsupported by current driver. */
    return 0U;
}

static int32_t ECSPI_SDMAControl(uint32_t control, uint32_t arg, cmsis_ecspi_sdma_driver_state_t *ecspi)
{
    if (!(ecspi->flags & SPI_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE:
            ECSPI_Enable(ecspi->resource->base, false);
            break;
        case ARM_SPI_MODE_MASTER:
            ecspi->baudRate_Bps = arg;
            ecspi->flags |= SPI_FLAG_MASTER;
            break;

        case ARM_SPI_MODE_SLAVE:
            ecspi->flags &= ~SPI_FLAG_MASTER;
            break;

        case ARM_SPI_SET_BUS_SPEED:
            if (!(ecspi->flags & SPI_FLAG_MASTER))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }
            ECSPI_SetBaudRate(ecspi->resource->base, arg, ecspi->resource->GetFreq());
            ecspi->baudRate_Bps = arg;
            return ARM_DRIVER_OK;

        case ARM_SPI_GET_BUS_SPEED: /* Set Bus Speed in bps; arg = value */
            if (!(ecspi->flags & SPI_FLAG_MASTER))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }
            return ecspi->baudRate_Bps;

        case ARM_SPI_CONTROL_SS:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_ABORT_TRANSFER:
            if (ecspi->flags & SPI_FLAG_MASTER)
            {
                ECSPI_MasterTransferAbortSDMA(ecspi->resource->base, ecspi->handle);
            }
            else
            {
                ECSPI_SlaveTransferAbortSDMA(ecspi->resource->base, ecspi->handle);
            }
            return ARM_DRIVER_OK;

        case ARM_SPI_SET_DEFAULT_TX_VALUE: /* Set default Transmit value; arg = value */
            /* Unsupported by current driver. */
            return ARM_DRIVER_OK;

        case ARM_SPI_MODE_MASTER_SIMPLEX: /* SPI Master (Output/Input on MOSI); arg = Bus Speed in bps */
            /* Mode is not supported by current driver. */
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_MODE_SLAVE_SIMPLEX: /* SPI Slave  (Output/Input on MISO) */
            /* Mode is not supported by current driver. */
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        default:
            break;
    }

    if (ecspi->flags & SPI_FLAG_MASTER)
    {
        ecspi_master_config_t masterConfig;
        ECSPI_MasterGetDefaultConfig(&masterConfig);
        masterConfig.baudRate_Bps = ecspi->baudRate_Bps;

        ECSPI_MasterCommonControl(control, ecspi->resource, &masterConfig);

        if (ecspi->flags & SPI_FLAG_CONFIGURED)
        {
            ECSPI_Deinit(ecspi->resource->base);
        }
        ECSPI_MasterInit(ecspi->resource->base, &masterConfig, ecspi->resource->GetFreq());

        SDMA_CreateHandle(ecspi->sdmaTxDataHandle, ecspi->sdmaResource->txSdmaBase, ecspi->sdmaResource->txSdmaChannel,
                          ecspi->sdmaResource->txSdmaContext);
        SDMA_CreateHandle(ecspi->sdmaRxDataHandle, ecspi->sdmaResource->rxSdmaBase, ecspi->sdmaResource->rxSdmaChannel,
                          ecspi->sdmaResource->rxSdmaContext);
        ECSPI_MasterTransferCreateHandleSDMA(ecspi->resource->base, ecspi->handle, KSDK_ECSPI_MasterSDMACallback,
                                             (void *)ecspi->cb_event, ecspi->sdmaTxDataHandle, ecspi->sdmaRxDataHandle,
                                             ecspi->sdmaResource->txSdmaRequest, ecspi->sdmaResource->rxSdmaRequest,
                                             ecspi->sdmaResource->txSdmaChannel, ecspi->sdmaResource->rxSdmaChannel);

        SDMA_SetChannelPriority(ecspi->sdmaResource->txSdmaBase, ecspi->sdmaResource->txSdmaChannel,
                                ecspi->sdmaResource->txSdmaPriority);
        SDMA_SetChannelPriority(ecspi->sdmaResource->rxSdmaBase, ecspi->sdmaResource->rxSdmaChannel,
                                ecspi->sdmaResource->rxSdmaPriority);

        ecspi->flags |= SPI_FLAG_CONFIGURED;
    }
    else
    {
        ecspi_slave_config_t slaveConfig;
        ECSPI_SlaveGetDefaultConfig(&slaveConfig);
        ECSPI_SlaveCommonControl(control, ecspi->resource, &slaveConfig);

        if (ecspi->flags & SPI_FLAG_CONFIGURED)
        {
            ECSPI_Deinit(ecspi->resource->base);
        }
        ECSPI_SlaveInit(ecspi->resource->base, &slaveConfig);

        SDMA_CreateHandle(ecspi->sdmaTxDataHandle, ecspi->sdmaResource->txSdmaBase, ecspi->sdmaResource->txSdmaChannel,
                          ecspi->sdmaResource->txSdmaContext);
        SDMA_CreateHandle(ecspi->sdmaRxDataHandle, ecspi->sdmaResource->rxSdmaBase, ecspi->sdmaResource->rxSdmaChannel,
                          ecspi->sdmaResource->rxSdmaContext);
        ECSPI_MasterTransferCreateHandleSDMA(ecspi->resource->base, ecspi->handle, KSDK_ECSPI_MasterSDMACallback,
                                             (void *)ecspi->cb_event, ecspi->sdmaTxDataHandle, ecspi->sdmaRxDataHandle,
                                             ecspi->sdmaResource->txSdmaRequest, ecspi->sdmaResource->rxSdmaRequest,
                                             ecspi->sdmaResource->txSdmaChannel, ecspi->sdmaResource->rxSdmaChannel);

        SDMA_SetChannelPriority(ecspi->sdmaResource->txSdmaBase, ecspi->sdmaResource->txSdmaChannel,
                                ecspi->sdmaResource->txSdmaPriority);
        SDMA_SetChannelPriority(ecspi->sdmaResource->rxSdmaBase, ecspi->sdmaResource->rxSdmaChannel,
                                ecspi->sdmaResource->rxSdmaPriority);

        ecspi->flags |= SPI_FLAG_CONFIGURED;
    }

    return ARM_DRIVER_OK;
}

ARM_SPI_STATUS ECSPI_SDMAGetStatus(cmsis_ecspi_sdma_driver_state_t *ecspi)
{
    ARM_SPI_STATUS stat;

    stat.busy       = ((ecspi->handle->txInProgress == true) || (ecspi->handle->rxInProgress == true)) ? (0U) : (1U);
    stat.data_lost  = 0U;
    stat.mode_fault = 0U;
    stat.reserved   = 0U;

    return stat;
}
#endif /* defined(FSL_FEATURE_SOC_DMA_COUNT) */

#endif

#if ((RTE_SPI1 && !RTE_SPI1_DMA_EN) || (RTE_SPI2 && !RTE_SPI2_DMA_EN) || (RTE_SPI3 && !RTE_SPI3_DMA_EN))

void KSDK_ECSPI_MasterInterruptCallback(ECSPI_Type *base,
                                        ecspi_master_handle_t *handle,
                                        status_t status,
                                        void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_ECSPI_HardwareOverFlow == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

void KSDK_ECSPI_SlaveInterruptCallback(ECSPI_Type *base, ecspi_slave_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_ECSPI_HardwareOverFlow == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

static int32_t ECSPI_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_ecspi_interrupt_driver_state_t *ecspi)
{
    if (!(ecspi->flags & SPI_FLAG_INIT))
    {
        ecspi->cb_event = cb_event;
        ecspi->flags    = SPI_FLAG_INIT;
    }
    return ARM_DRIVER_OK;
}

static int32_t ECSPI_InterruptUninitialize(cmsis_ecspi_interrupt_driver_state_t *ecspi)
{
    ecspi->flags = SPI_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t ECSPI_InterruptPowerControl(ARM_POWER_STATE state, cmsis_ecspi_interrupt_driver_state_t *ecspi)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            if (ecspi->flags & SPI_FLAG_POWER)
            {
                ECSPI_Deinit(ecspi->resource->base);
                ecspi->flags &= ~SPI_FLAG_POWER;
            }
            break;

        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_POWER_FULL:
            if (ecspi->flags == SPI_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }
            /* Driver already powered */
            if (ecspi->flags & SPI_FLAG_POWER)
            {
                break;
            }
            ecspi->flags |= SPI_FLAG_POWER;

            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

static int32_t ECSPI_InterruptSend(const void *data, uint32_t num, cmsis_ecspi_interrupt_driver_state_t *ecspi)
{
    int32_t ret;
    status_t status;
    ecspi_transfer_t xfer = {0};

    xfer.rxData   = NULL;
    xfer.txData   = (uint32_t *)data;
    xfer.dataSize = num;
    xfer.channel  = kECSPI_Channel0;

    /* Configure the channel to be used. */
    switch (ecspi->resource->instance)
    {
        case 1:
#if defined(RTE_SPI1_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI1_TRANSFER_CHANNEL;
#endif
            break;
        case 2:
#if defined(RTE_SPI2_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI2_TRANSFER_CHANNEL;
#endif
            break;
        case 3:
#if defined(RTE_SPI3_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI3_TRANSFER_CHANNEL;
#endif
            break;
        default:
            break;
    }

    if (ecspi->flags & SPI_FLAG_MASTER)
    {
        status = ECSPI_MasterTransferNonBlocking(ecspi->resource->base, ecspi->handle, &xfer);
    }
    else
    {
        status = ECSPI_SlaveTransferNonBlocking(ecspi->resource->base, ecspi->handle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_ECSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t ECSPI_InterruptReceive(void *data, uint32_t num, cmsis_ecspi_interrupt_driver_state_t *ecspi)
{
    int32_t ret;
    status_t status;
    ecspi_transfer_t xfer = {0};

    xfer.txData   = NULL;
    xfer.rxData   = (uint32_t *)data;
    xfer.dataSize = num;
    xfer.channel  = kECSPI_Channel0;

    /* Configure the channel to be used. */
    switch (ecspi->resource->instance)
    {
        case 1:
#if defined(RTE_SPI1_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI1_TRANSFER_CHANNEL;
#endif
            break;
        case 2:
#if defined(RTE_SPI2_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI2_TRANSFER_CHANNEL;
#endif
            break;
        case 3:
#if defined(RTE_SPI3_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI3_TRANSFER_CHANNEL;
#endif
            break;
        default:
            break;
    }

    if (ecspi->flags & SPI_FLAG_MASTER)
    {
        status = ECSPI_MasterTransferNonBlocking(ecspi->resource->base, ecspi->handle, &xfer);
    }
    else
    {
        status = ECSPI_SlaveTransferNonBlocking(ecspi->resource->base, ecspi->handle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_ECSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t ECSPI_InterruptTransfer(const void *data_out,
                                       void *data_in,
                                       uint32_t num,
                                       cmsis_ecspi_interrupt_driver_state_t *ecspi)
{
    int32_t ret;
    status_t status;
    ecspi_transfer_t xfer = {0};

    xfer.txData   = (uint32_t *)data_out;
    xfer.rxData   = (uint32_t *)data_in;
    xfer.dataSize = num;
    xfer.channel  = kECSPI_Channel0;

    /* Configure the channel to be used. */
    switch (ecspi->resource->instance)
    {
        case 1:
#if defined(RTE_SPI1_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI1_TRANSFER_CHANNEL;
#endif
            break;
        case 2:
#if defined(RTE_SPI2_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI2_TRANSFER_CHANNEL;
#endif
            break;
        case 3:
#if defined(RTE_SPI3_TRANSFER_CHANNEL)
            xfer.channel = RTE_SPI3_TRANSFER_CHANNEL;
#endif
            break;
        default:
            break;
    }

    if (ecspi->flags & SPI_FLAG_MASTER)
    {
        status = ECSPI_MasterTransferNonBlocking(ecspi->resource->base, ecspi->handle, &xfer);
    }
    else
    {
        status = ECSPI_SlaveTransferNonBlocking(ecspi->resource->base, ecspi->handle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_ECSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}
static uint32_t ECSPI_InterruptGetCount(cmsis_ecspi_interrupt_driver_state_t *ecspi)
{
    return ecspi->handle->transferSize - ecspi->handle->rxRemainingBytes;
}

static int32_t ECSPI_InterruptControl(uint32_t control, uint32_t arg, cmsis_ecspi_interrupt_driver_state_t *ecspi)
{
    if (!(ecspi->flags & SPI_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_MASTER: /* SPI Master (Output on MOSI, Input on MISO); arg = Bus Speed in bps */
            ecspi->baudRate_Bps = arg;
            ecspi->flags |= SPI_FLAG_MASTER;
            break;

        case ARM_SPI_MODE_SLAVE: /* SPI Slave  (Output on MISO, Input on MOSI) */
            ecspi->flags &= ~SPI_FLAG_MASTER;
            break;

        case ARM_SPI_GET_BUS_SPEED: /* Get Bus Speed in bps */
            if (!(ecspi->flags & SPI_FLAG_MASTER))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            return ecspi->baudRate_Bps;

        case ARM_SPI_SET_BUS_SPEED: /* Set Bus Speed in bps; */
            if (!(ecspi->flags & SPI_FLAG_MASTER))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }
            ECSPI_SetBaudRate(ecspi->resource->base, arg, ecspi->resource->GetFreq());
            ecspi->baudRate_Bps = arg;

            return ARM_DRIVER_OK;

        case ARM_SPI_CONTROL_SS:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_ABORT_TRANSFER: /* Abort current data transfer */
            if (ecspi->flags & SPI_FLAG_MASTER)
            {
                ECSPI_MasterTransferAbort(ecspi->resource->base, ecspi->handle);
            }
            else
            {
                ECSPI_SlaveTransferAbort(ecspi->resource->base, ecspi->handle);
            }
            return ARM_DRIVER_OK;

        case ARM_SPI_SET_DEFAULT_TX_VALUE: /* Set default Transmit value; arg = value */
            /* Mode is not supported by current driver. */
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_MODE_MASTER_SIMPLEX: /* SPI Master (Output/Input on MOSI); arg = Bus Speed in bps */
            /* Mode is not supported by current driver. */
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_MODE_SLAVE_SIMPLEX: /* SPI Slave  (Output/Input on MISO) */
            /* Mode is not supported by current driver. */
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        default:
            break;
    }

    if (ecspi->flags & SPI_FLAG_MASTER)
    {
        ecspi_master_config_t masterConfig;
        ECSPI_MasterGetDefaultConfig(&masterConfig);
        masterConfig.baudRate_Bps = ecspi->baudRate_Bps;

        ECSPI_MasterCommonControl(control, ecspi->resource, &masterConfig);

        if (ecspi->flags & SPI_FLAG_CONFIGURED)
        {
            ECSPI_Deinit(ecspi->resource->base);
        }
        ECSPI_MasterInit(ecspi->resource->base, &masterConfig, ecspi->resource->GetFreq());
        ECSPI_MasterTransferCreateHandle(ecspi->resource->base, ecspi->handle, KSDK_ECSPI_MasterInterruptCallback,
                                         (void *)ecspi->cb_event);
        ecspi->flags |= SPI_FLAG_CONFIGURED;
    }
    else
    {
        ecspi_slave_config_t slaveConfig;
        ECSPI_SlaveGetDefaultConfig(&slaveConfig);

        ECSPI_SlaveCommonControl(control, ecspi->resource, &slaveConfig);

        if (ecspi->flags & SPI_FLAG_CONFIGURED)
        {
            ECSPI_Deinit(ecspi->resource->base);
        }
        ECSPI_SlaveInit(ecspi->resource->base, &slaveConfig);
        ECSPI_SlaveTransferCreateHandle(ecspi->resource->base, ecspi->handle, KSDK_ECSPI_SlaveInterruptCallback,
                                        (void *)ecspi->cb_event);
        ecspi->flags |= SPI_FLAG_CONFIGURED;
    }

    return ARM_DRIVER_OK;
}

ARM_SPI_STATUS ECSPI_InterruptGetStatus(cmsis_ecspi_interrupt_driver_state_t *ecspi)
{
    ARM_SPI_STATUS stat;

    stat.busy       = ((ecspi->handle->txRemainingBytes > 0) || (ecspi->handle->rxRemainingBytes > 0)) ? (0U) : (1U);
    stat.data_lost  = 0U;
    stat.mode_fault = 0U;
    stat.reserved   = 0U;

    return stat;
}

#endif

#if defined(ECSPI1) && RTE_SPI1

/* User needs to provide the implementation for ECSPI1_GetFreq/InitPins/DeinitPins
 * in the application for enabling according instance.
 */
extern uint32_t ECSPI1_GetFreq(void);
extern void ECSPI1_InitPins(void);
extern void ECSPI1_DeinitPins(void);

cmsis_ecspi_resource_t ECSPI1_Resource = {ECSPI1, 1, ECSPI1_GetFreq};

#if RTE_SPI1_DMA_EN

AT_NONCACHEABLE_SECTION_ALIGN(ecspi_sdma_handle_t ECSPI1_SdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t ECSPI1_TxSdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t ECSPI1_RxSdmaHandle, 4);

AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t ECSPI1_TxSdmaContext, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t ECSPI1_RxSdmaContext, 4);

cmsis_ecspi_sdma_resource_t ECSPI1_SDMAResource = {
    RTE_SPI1_DMA_TX_DMA_BASE,    RTE_SPI1_DMA_TX_CH,       RTE_SPI1_DMA_TX_CH_REQUEST, RTE_SPI1_DMA_TX_CH_PRIORITY,
    &ECSPI1_TxSdmaContext,       RTE_SPI1_DMA_RX_DMA_BASE, RTE_SPI1_DMA_RX_CH,         RTE_SPI1_DMA_RX_CH_REQUEST,
    RTE_SPI1_DMA_RX_CH_PRIORITY, &ECSPI1_RxSdmaContext};

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("ecspi1_sdma_driver_state")
static cmsis_ecspi_sdma_driver_state_t ECSPI1_SDMADriverState = {
#else
static cmsis_ecspi_sdma_driver_state_t ECSPI1_SDMADriverState = {
#endif
    &ECSPI1_Resource, &ECSPI1_SDMAResource, &ECSPI1_SdmaHandle, &ECSPI1_RxSdmaHandle, &ECSPI1_TxSdmaHandle};

static int32_t ECSPI1_SDMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    ECSPI1_InitPins();
    return ECSPI_SDMAInitialize(cb_event, &ECSPI1_SDMADriverState);
}

static int32_t ECSPI1_SDMAUninitialize(void)
{
    ECSPI1_DeinitPins();
    return ECSPI_SDMAUninitialize(&ECSPI1_SDMADriverState);
}

static int32_t ECSPI1_SDMAPowerControl(ARM_POWER_STATE state)
{
    return ECSPI_SDMAPowerControl(state, &ECSPI1_SDMADriverState);
}

static int32_t ECSPI1_SDMASend(const void *data, uint32_t num)
{
    return ECSPI_SDMASend(data, num, &ECSPI1_SDMADriverState);
}

static int32_t ECSPI1_SDMAReceive(void *data, uint32_t num)
{
    return ECSPI_SDMAReceive(data, num, &ECSPI1_SDMADriverState);
}

static int32_t ECSPI1_SDMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return ECSPI_SDMATransfer(data_out, data_in, num, &ECSPI1_SDMADriverState);
}

static uint32_t ECSPI1_SDMAGetCount(void)
{
    return ECSPI_SDMAGetCount(&ECSPI1_SDMADriverState);
}

static int32_t ECSPI1_SDMAControl(uint32_t control, uint32_t arg)
{
    return ECSPI_SDMAControl(control, arg, &ECSPI1_SDMADriverState);
}

static ARM_SPI_STATUS ECSPI1_SDMAGetStatus(void)
{
    return ECSPI_SDMAGetStatus(&ECSPI1_SDMADriverState);
}

#else

static ecspi_master_handle_t ECSPI1_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("ecspi1_interrupt_driver_state")
static cmsis_ecspi_interrupt_driver_state_t ECSPI1_InterruptDriverState = {
#else
static cmsis_ecspi_interrupt_driver_state_t ECSPI1_InterruptDriverState = {
#endif
    &ECSPI1_Resource,
    &ECSPI1_Handle,
};

static int32_t ECSPI1_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    ECSPI1_InitPins();
    return ECSPI_InterruptInitialize(cb_event, &ECSPI1_InterruptDriverState);
}

static int32_t ECSPI1_InterruptUninitialize(void)
{
    ECSPI1_DeinitPins();
    return ECSPI_InterruptUninitialize(&ECSPI1_InterruptDriverState);
}

static int32_t ECSPI1_InterruptPowerControl(ARM_POWER_STATE state)
{
    return ECSPI_InterruptPowerControl(state, &ECSPI1_InterruptDriverState);
}

static int32_t ECSPI1_InterruptSend(const void *data, uint32_t num)
{
    return ECSPI_InterruptSend(data, num, &ECSPI1_InterruptDriverState);
}

static int32_t ECSPI1_InterruptReceive(void *data, uint32_t num)
{
    return ECSPI_InterruptReceive(data, num, &ECSPI1_InterruptDriverState);
}

static int32_t ECSPI1_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return ECSPI_InterruptTransfer(data_out, data_in, num, &ECSPI1_InterruptDriverState);
}

static uint32_t ECSPI1_InterruptGetCount(void)
{
    return ECSPI_InterruptGetCount(&ECSPI1_InterruptDriverState);
}

static int32_t ECSPI1_InterruptControl(uint32_t control, uint32_t arg)
{
    return ECSPI_InterruptControl(control, arg, &ECSPI1_InterruptDriverState);
}

static ARM_SPI_STATUS ECSPI1_InterruptGetStatus(void)
{
    return ECSPI_InterruptGetStatus(&ECSPI1_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI1 = {
    ECSPIx_GetVersion,     ECSPIx_GetCapabilities,
#if RTE_SPI1_DMA_EN
    ECSPI1_SDMAInitialize, ECSPI1_SDMAUninitialize, ECSPI1_SDMAPowerControl, ECSPI1_SDMASend,     ECSPI1_SDMAReceive,
    ECSPI1_SDMATransfer,   ECSPI1_SDMAGetCount,     ECSPI1_SDMAControl,      ECSPI1_SDMAGetStatus
#else
    ECSPI1_InterruptInitialize, ECSPI1_InterruptUninitialize, ECSPI1_InterruptPowerControl, ECSPI1_InterruptSend,
    ECSPI1_InterruptReceive,    ECSPI1_InterruptTransfer,     ECSPI1_InterruptGetCount,     ECSPI1_InterruptControl,
    ECSPI1_InterruptGetStatus
#endif
};

#endif /*  ECSPI1  */

#if defined(ECSPI2) && RTE_SPI2

/* User needs to provide the implementation for ECSPI2_GetFreq/InitPins/DeinitPins
 * in the application for enabling according instance.
 */
extern uint32_t ECSPI2_GetFreq(void);
extern void ECSPI2_InitPins(void);
extern void ECSPI2_DeinitPins(void);

cmsis_ecspi_resource_t ECSPI2_Resource = {ECSPI2, 2, ECSPI2_GetFreq};

#if RTE_SPI2_DMA_EN

AT_NONCACHEABLE_SECTION_ALIGN(ecspi_sdma_handle_t ECSPI2_SdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t ECSPI2_TxSdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t ECSPI2_RxSdmaHandle, 4);

AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t ECSPI2_TxSdmaContext, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t ECSPI2_RxSdmaContext, 4);

cmsis_ecspi_sdma_resource_t ECSPI2_SDMAResource = {
    RTE_SPI2_DMA_TX_DMA_BASE,    RTE_SPI2_DMA_TX_CH,       RTE_SPI2_DMA_TX_CH_REQUEST, RTE_SPI2_DMA_TX_CH_PRIORITY,
    &ECSPI2_TxSdmaContext,       RTE_SPI2_DMA_RX_DMA_BASE, RTE_SPI2_DMA_RX_CH,         RTE_SPI2_DMA_RX_CH_REQUEST,
    RTE_SPI2_DMA_RX_CH_PRIORITY, &ECSPI2_RxSdmaContext};

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("ecspi2_sdma_driver_state")
static cmsis_ecspi_sdma_driver_state_t ECSPI2_SDMADriverState = {
#else
static cmsis_ecspi_sdma_driver_state_t ECSPI2_SDMADriverState = {
#endif
    &ECSPI2_Resource, &ECSPI2_SDMAResource, &ECSPI2_SdmaHandle, &ECSPI2_TxSdmaHandle, &ECSPI2_RxSdmaHandle,

};

static int32_t ECSPI2_SDMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    ECSPI2_InitPins();
    return ECSPI_SDMAInitialize(cb_event, &ECSPI2_SDMADriverState);
}

static int32_t ECSPI2_SDMAUninitialize(void)
{
    ECSPI2_DeinitPins();
    return ECSPI_SDMAUninitialize(&ECSPI2_SDMADriverState);
}

static int32_t ECSPI2_SDMAPowerControl(ARM_POWER_STATE state)
{
    return ECSPI_SDMAPowerControl(state, &ECSPI2_SDMADriverState);
}

static int32_t ECSPI2_SDMASend(const void *data, uint32_t num)
{
    return ECSPI_SDMASend(data, num, &ECSPI2_SDMADriverState);
}

static int32_t ECSPI2_SDMAReceive(void *data, uint32_t num)
{
    return ECSPI_SDMAReceive(data, num, &ECSPI2_SDMADriverState);
}

static int32_t ECSPI2_SDMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return ECSPI_SDMATransfer(data_out, data_in, num, &ECSPI2_SDMADriverState);
}

static uint32_t ECSPI2_SDMAGetCount(void)
{
    return ECSPI_SDMAGetCount(&ECSPI2_SDMADriverState);
}

static int32_t ECSPI2_SDMAControl(uint32_t control, uint32_t arg)
{
    return ECSPI_SDMAControl(control, arg, &ECSPI2_SDMADriverState);
}

static ARM_SPI_STATUS ECSPI2_SDMAGetStatus(void)
{
    return ECSPI_SDMAGetStatus(&ECSPI2_SDMADriverState);
}

#else

static ecspi_master_handle_t ECSPI2_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("ecspi2_interrupt_driver_state")
static cmsis_ecspi_interrupt_driver_state_t ECSPI2_InterruptDriverState = {
#else
static cmsis_ecspi_interrupt_driver_state_t ECSPI2_InterruptDriverState = {
#endif
    &ECSPI2_Resource,
    &ECSPI2_Handle,
};

static int32_t ECSPI2_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    ECSPI2_InitPins();
    return ECSPI_InterruptInitialize(cb_event, &ECSPI2_InterruptDriverState);
}

static int32_t ECSPI2_InterruptUninitialize(void)
{
    ECSPI2_DeinitPins();
    return ECSPI_InterruptUninitialize(&ECSPI2_InterruptDriverState);
}

static int32_t ECSPI2_InterruptPowerControl(ARM_POWER_STATE state)
{
    return ECSPI_InterruptPowerControl(state, &ECSPI2_InterruptDriverState);
}

static int32_t ECSPI2_InterruptSend(const void *data, uint32_t num)
{
    return ECSPI_InterruptSend(data, num, &ECSPI2_InterruptDriverState);
}

static int32_t ECSPI2_InterruptReceive(void *data, uint32_t num)
{
    return ECSPI_InterruptReceive(data, num, &ECSPI2_InterruptDriverState);
}

static int32_t ECSPI2_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return ECSPI_InterruptTransfer(data_out, data_in, num, &ECSPI2_InterruptDriverState);
}

static uint32_t ECSPI2_InterruptGetCount(void)
{
    return ECSPI_InterruptGetCount(&ECSPI2_InterruptDriverState);
}

static int32_t ECSPI2_InterruptControl(uint32_t control, uint32_t arg)
{
    return ECSPI_InterruptControl(control, arg, &ECSPI2_InterruptDriverState);
}

static ARM_SPI_STATUS ECSPI2_InterruptGetStatus(void)
{
    return ECSPI_InterruptGetStatus(&ECSPI2_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI2 = {
    ECSPIx_GetVersion,     ECSPIx_GetCapabilities,
#if RTE_SPI2_DMA_EN
    ECSPI2_SDMAInitialize, ECSPI2_SDMAUninitialize, ECSPI2_SDMAPowerControl, ECSPI2_SDMASend,     ECSPI2_SDMAReceive,
    ECSPI2_SDMATransfer,   ECSPI2_SDMAGetCount,     ECSPI2_SDMAControl,      ECSPI2_SDMAGetStatus
#else
    ECSPI2_InterruptInitialize, ECSPI2_InterruptUninitialize, ECSPI2_InterruptPowerControl, ECSPI2_InterruptSend,
    ECSPI2_InterruptReceive,    ECSPI2_InterruptTransfer,     ECSPI2_InterruptGetCount,     ECSPI2_InterruptControl,
    ECSPI2_InterruptGetStatus
#endif
};

#endif /*  ECSPI2  */

#if defined(ECSPI3) && RTE_SPI3

/* User needs to provide the implementation for ECSPI3_GetFreq/InitPins/DeinitPins
 * in the application for enabling according instance.
 */
extern uint32_t ECSPI3_GetFreq(void);
extern void ECSPI3_InitPins(void);
extern void ECSPI3_DeinitPins(void);

cmsis_ecspi_resource_t ECSPI3_Resource = {ECSPI3, 3, ECSPI3_GetFreq};

#if RTE_SPI3_DMA_EN

AT_NONCACHEABLE_SECTION_ALIGN(ecspi_sdma_handle_t ECSPI3_SdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t ECSPI3_TxSdmaHandle, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_handle_t ECSPI3_RxSdmaHandle, 4);

AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t ECSPI3_TxSdmaContext, 4);
AT_NONCACHEABLE_SECTION_ALIGN(sdma_context_data_t ECSPI3_RxSdmaContext, 4);

cmsis_ecspi_sdma_resource_t ECSPI3_SDMAResource = {
    RTE_SPI3_DMA_TX_DMA_BASE,    RTE_SPI3_DMA_TX_CH,       RTE_SPI3_DMA_TX_CH_REQUEST, RTE_SPI3_DMA_TX_CH_PRIORITY,
    &ECSPI3_TxSdmaContext,       RTE_SPI3_DMA_RX_DMA_BASE, RTE_SPI3_DMA_RX_CH,         RTE_SPI3_DMA_RX_CH_REQUEST,
    RTE_SPI3_DMA_RX_CH_PRIORITY, &ECSPI3_RxSdmaContext};

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("ecspi3_sdma_driver_state")
static cmsis_ecspi_sdma_driver_state_t ECSPI3_SDMADriverState = {
#else
static cmsis_ecspi_sdma_driver_state_t ECSPI3_SDMADriverState = {
#endif
    &ECSPI3_Resource, &ECSPI3_SDMAResource, &ECSPI3_SdmaHandle, &ECSPI3_TxSdmaHandle, &ECSPI3_RxSdmaHandle,

};

static int32_t ECSPI3_SDMAInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    ECSPI3_InitPins();
    return ECSPI_SDMAInitialize(cb_event, &ECSPI3_SDMADriverState);
}

static int32_t ECSPI3_SDMAUninitialize(void)
{
    ECSPI3_DeinitPins();
    return ECSPI_SDMAUninitialize(&ECSPI3_SDMADriverState);
}

static int32_t ECSPI3_SDMAPowerControl(ARM_POWER_STATE state)
{
    return ECSPI_SDMAPowerControl(state, &ECSPI3_SDMADriverState);
}

static int32_t ECSPI3_SDMASend(const void *data, uint32_t num)
{
    return ECSPI_SDMASend(data, num, &ECSPI3_SDMADriverState);
}

static int32_t ECSPI3_SDMAReceive(void *data, uint32_t num)
{
    return ECSPI_SDMAReceive(data, num, &ECSPI3_SDMADriverState);
}

static int32_t ECSPI3_SDMATransfer(const void *data_out, void *data_in, uint32_t num)
{
    return ECSPI_SDMATransfer(data_out, data_in, num, &ECSPI3_SDMADriverState);
}

static uint32_t ECSPI3_SDMAGetCount(void)
{
    return ECSPI_SDMAGetCount(&ECSPI3_SDMADriverState);
}

static int32_t ECSPI3_SDMAControl(uint32_t control, uint32_t arg)
{
    return ECSPI_SDMAControl(control, arg, &ECSPI3_SDMADriverState);
}

static ARM_SPI_STATUS ECSPI3_SDMAGetStatus(void)
{
    return ECSPI_SDMAGetStatus(&ECSPI3_SDMADriverState);
}

#else

static ecspi_master_handle_t ECSPI3_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("ecspi3_interrupt_driver_state")
static cmsis_ecspi_interrupt_driver_state_t ECSPI3_InterruptDriverState = {
#else
static cmsis_ecspi_interrupt_driver_state_t ECSPI3_InterruptDriverState = {
#endif
    &ECSPI3_Resource,
    &ECSPI3_Handle,
};

static int32_t ECSPI3_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    ECSPI3_InitPins();
    return ECSPI_InterruptInitialize(cb_event, &ECSPI3_InterruptDriverState);
}

static int32_t ECSPI3_InterruptUninitialize(void)
{
    ECSPI3_DeinitPins();
    return ECSPI_InterruptUninitialize(&ECSPI3_InterruptDriverState);
}

static int32_t ECSPI3_InterruptPowerControl(ARM_POWER_STATE state)
{
    return ECSPI_InterruptPowerControl(state, &ECSPI3_InterruptDriverState);
}

static int32_t ECSPI3_InterruptSend(const void *data, uint32_t num)
{
    return ECSPI_InterruptSend(data, num, &ECSPI3_InterruptDriverState);
}

static int32_t ECSPI3_InterruptReceive(void *data, uint32_t num)
{
    return ECSPI_InterruptReceive(data, num, &ECSPI3_InterruptDriverState);
}

static int32_t ECSPI3_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return ECSPI_InterruptTransfer(data_out, data_in, num, &ECSPI3_InterruptDriverState);
}

static uint32_t ECSPI3_InterruptGetCount(void)
{
    return ECSPI_InterruptGetCount(&ECSPI3_InterruptDriverState);
}

static int32_t ECSPI3_InterruptControl(uint32_t control, uint32_t arg)
{
    return ECSPI_InterruptControl(control, arg, &ECSPI3_InterruptDriverState);
}

static ARM_SPI_STATUS ECSPI3_InterruptGetStatus(void)
{
    return ECSPI_InterruptGetStatus(&ECSPI3_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI3 = {
    ECSPIx_GetVersion,     ECSPIx_GetCapabilities,
#if RTE_SPI3_DMA_EN
    ECSPI3_SDMAInitialize, ECSPI3_SDMAUninitialize, ECSPI3_SDMAPowerControl, ECSPI3_SDMASend,     ECSPI3_SDMAReceive,
    ECSPI3_SDMATransfer,   ECSPI3_SDMAGetCount,     ECSPI3_SDMAControl,      ECSPI3_SDMAGetStatus
#else
    ECSPI3_InterruptInitialize, ECSPI3_InterruptUninitialize, ECSPI3_InterruptPowerControl, ECSPI3_InterruptSend,
    ECSPI3_InterruptReceive,    ECSPI3_InterruptTransfer,     ECSPI3_InterruptGetCount,     ECSPI3_InterruptControl,
    ECSPI3_InterruptGetStatus
#endif
};

#endif /*  ECSPI3  */
