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

#include "fsl_dspi_cmsis.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.dspi_cmsis"
#endif

#if ((RTE_SPI0 && defined(DSPI0)) || (RTE_SPI1 && defined(DSPI1)) || (RTE_SPI2 && defined(DSPI2)))

#define ARM_DSPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 1) /* driver version */

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

static clock_ip_name_t const s_dspiClock[] = DSPI_CLOCKS;

typedef const struct _cmsis_dspi_resource
{
    SPI_Type *base;
    uint32_t instance;
    uint32_t (*GetFreq)(void);
} cmsis_dspi_resource_t;

typedef union _cmsis_dspi_handle
{
    dspi_master_handle_t masterHandle;
    dspi_slave_handle_t slaveHandle;
} cmsis_dspi_handle_t;

typedef struct _cmsis_dspi_interrupt_driver_state
{
    cmsis_dspi_resource_t *resource;
    cmsis_dspi_handle_t *handle;
    ARM_SPI_SignalEvent_t event;

    uint32_t baudRate_Bps;
    uint8_t flags; /*!< Control and state flags. */
} cmsis_dspi_interrupt_driver_state_t;

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
typedef union _cmsis_dspi_edma_handle
{
    dspi_master_edma_handle_t masterHandle;
    dspi_slave_edma_handle_t slaveHandle;
} cmsis_dspi_edma_handle_t;

typedef const struct _cmsis_dspi_edma_resource
{
    DMA_Type *txEdmaBase;
    uint32_t txEdmaChannel;
    DMAMUX_Type *txDmamuxBase;
    uint8_t txDmaRequest;

    DMA_Type *rxEdmaBase;
    uint32_t rxEdmaChannel;
    DMAMUX_Type *rxDmamuxBase;
    uint8_t rxDmaRequest;

    DMA_Type *masterLinkEdmaBase;
    uint32_t masterLinkChannel;
} cmsis_dspi_edma_resource_t;

typedef struct _cmsis_dspi_edma_driver_state
{
    cmsis_dspi_resource_t *resource;
    cmsis_dspi_edma_resource_t *dmaResource;
    cmsis_dspi_edma_handle_t *handle;
    edma_handle_t *edmaRxRegToRxDataHandle;
    edma_handle_t *edmaTxDataToIntermediaryHandle;
    edma_handle_t *edmaIntermediaryToTxRegHandle;
    edma_handle_t *edmaTxDataToTxRegHandle;
    ARM_SPI_SignalEvent_t event;

    uint32_t baudRate_Bps;
    uint8_t flags; /*!< Control and state flags. */
} cmsis_dspi_edma_driver_state_t;
#endif

/* Driver Version */
static const ARM_DRIVER_VERSION s_dspiDriverVersion = {ARM_SPI_API_VERSION, ARM_DSPI_DRV_VERSION};

/* Driver Capabilities */
static const ARM_SPI_CAPABILITIES s_dspiDriverCapabilities = {
    1, /* Simplex Mode (Master and Slave) */
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface  */
    0  /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
};

/*
 *Common Control function used by DSPI_InterruptControl / DSPI_EdmaControl.
 */
static int32_t DSPI_CommonControl(uint32_t control,
                                  uint32_t arg,
                                  cmsis_dspi_resource_t *resource,
                                  uint8_t *isConfigured)
{
    dspi_master_config_t masterConfig;
    DSPI_MasterGetDefaultConfig(&masterConfig);
    dspi_slave_config_t slaveConfig;
    DSPI_SlaveGetDefaultConfig(&slaveConfig);
    masterConfig.ctarConfig.baudRate = arg;

#if (defined(RTE_SPI0_PCS_TO_SCK_DELAY) && defined(RTE_SPI0_SCK_TO_PSC_DELAY) && \
     defined(RTE_SPI0_BETWEEN_TRANSFER_DELAY))
    if (0U == resource->instance)
    {
        masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = RTE_SPI0_PCS_TO_SCK_DELAY;
        masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = RTE_SPI0_SCK_TO_PSC_DELAY;
        masterConfig.ctarConfig.betweenTransferDelayInNanoSec = RTE_SPI0_BETWEEN_TRANSFER_DELAY;
    }
#endif /*RTE DSPI0 trnafer delay time configure */

#if (defined(RTE_SPI1_PCS_TO_SCK_DELAY) && defined(RTE_SPI1_SCK_TO_PSC_DELAY) && \
     defined(RTE_SPI1_BETWEEN_TRANSFER_DELAY))
    if (1U == resource->instance)
    {
        masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = RTE_SPI1_PCS_TO_SCK_DELAY;
        masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = RTE_SPI1_SCK_TO_PSC_DELAY;
        masterConfig.ctarConfig.betweenTransferDelayInNanoSec = RTE_SPI1_BETWEEN_TRANSFER_DELAY;
    }
#endif /*RTE DSPI1 trnafer delay time configure */

#if (defined(RTE_SPI2_PCS_TO_SCK_DELAY) && defined(RTE_SPI2_SCK_TO_PSC_DELAY) && \
     defined(RTE_SPI2_BETWEEN_TRANSFER_DELAY))
    if (2U == resource->instance)
    {
        masterConfig.ctarConfig.pcsToSckDelayInNanoSec        = RTE_SPI2_PCS_TO_SCK_DELAY;
        masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec    = RTE_SPI2_SCK_TO_PSC_DELAY;
        masterConfig.ctarConfig.betweenTransferDelayInNanoSec = RTE_SPI2_BETWEEN_TRANSFER_DELAY;
    }
#endif /*RTE DSPI2 trnafer delay time configure */

    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
        case ARM_SPI_CPOL0_CPHA0: /* Clock Polarity 0, Clock Phase 0*/
            if (DSPI_IsMaster(resource->base))
            {
                masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
                masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
            }
            else
            {
                slaveConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
                slaveConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
            }
            break;

        case ARM_SPI_CPOL0_CPHA1: /* Clock Polarity 0, Clock Phase 1*/
            if (DSPI_IsMaster(resource->base))
            {
                masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
                masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseSecondEdge;
            }
            else
            {
                slaveConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
                slaveConfig.ctarConfig.cpha = kDSPI_ClockPhaseSecondEdge;
            }
            break;

        case ARM_SPI_CPOL1_CPHA0: /* Clock Polarity 1, Clock Phase 0*/
            if (DSPI_IsMaster(resource->base))
            {
                masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
                masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
            }
            else
            {
                slaveConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
                slaveConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
            }
            break;

        case ARM_SPI_CPOL1_CPHA1: /* Clock Polarity 1, Clock Phase 1*/
            if (DSPI_IsMaster(resource->base))
            {
                masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
                masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseSecondEdge;
            }
            else
            {
                slaveConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
                slaveConfig.ctarConfig.cpha = kDSPI_ClockPhaseSecondEdge;
            }
            break;

        default:
            break;
    }

    if (control & ARM_SPI_DATA_BITS_Msk) /* setting Number of Data bits */
    {
        if ((((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) >= 4) &&
            (((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) <= 16))
        {
            if (DSPI_IsMaster(resource->base))
            {
                masterConfig.ctarConfig.bitsPerFrame = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);
            }
            else
            {
                slaveConfig.ctarConfig.bitsPerFrame = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);
            }
        }
        else
        {
            return ARM_SPI_ERROR_DATA_BITS;
        }
    }

    switch (control & ARM_SPI_BIT_ORDER_Msk)
    {
        case ARM_SPI_LSB_MSB: /* SPI Bit order from LSB to MSB */
            if (DSPI_IsMaster(resource->base))
            {
                masterConfig.ctarConfig.direction = kDSPI_LsbFirst;
            }
            break;

        case ARM_SPI_MSB_LSB: /* SPI Bit order from MSB to LSB */
            if (DSPI_IsMaster(resource->base))
            {
                masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
            }
            break;

        default:
            break;
    }

    if (DSPI_IsMaster(resource->base))
    {
        /* The SPI slave select is controlled by hardware, the other mode is not supported by current driver. */
        switch (control & ARM_SPI_SS_MASTER_MODE_Msk)
        {
            case ARM_SPI_SS_MASTER_UNUSED:
                break;
            case ARM_SPI_SS_MASTER_SW:
                break;
            case ARM_SPI_SS_MASTER_HW_OUTPUT:
                break;
            case ARM_SPI_SS_MASTER_HW_INPUT:
                break;
            default:
                break;
        }
    }
    else
    {
        /* The SPI slave select is controlled by hardware, the other mode is not supported by current driver. */
        switch (control & ARM_SPI_SS_SLAVE_MODE_Msk)
        {
            case ARM_SPI_SS_SLAVE_HW:
                break;
            case ARM_SPI_SS_SLAVE_SW:
                break;
            default:
                break;
        }
    }
    if (DSPI_IsMaster(resource->base))
    {
        if ((*isConfigured) & SPI_FLAG_CONFIGURED)
        {
            DSPI_Deinit(resource->base);
        }
        DSPI_MasterInit(resource->base, &masterConfig, resource->GetFreq());
        *isConfigured |= SPI_FLAG_CONFIGURED;
    }
    else
    {
        if ((*isConfigured) & SPI_FLAG_CONFIGURED)
        {
            DSPI_Deinit(resource->base);
        }
        DSPI_SlaveInit(resource->base, &slaveConfig);
        *isConfigured |= SPI_FLAG_CONFIGURED;
    }

    return ARM_DRIVER_OK;
}

/*******************************************************************************
 * Code
 ******************************************************************************/

static ARM_DRIVER_VERSION DSPIx_GetVersion(void)
{
    return s_dspiDriverVersion;
}

static ARM_SPI_CAPABILITIES DSPIx_GetCapabilities(void)
{
    return s_dspiDriverCapabilities;
}

#endif

#if (RTE_SPI0_DMA_EN || RTE_SPI1_DMA_EN || RTE_SPI2_DMA_EN)

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)

void KSDK_DSPI_MasterEdmaCallback(SPI_Type *base, dspi_master_edma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_DSPI_OutOfRange == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}
void KSDK_DSPI_SlaveEdmaCallback(SPI_Type *base, dspi_slave_edma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_DSPI_OutOfRange == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

static int32_t DSPI_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_dspi_edma_driver_state_t *dspi)
{
    if (!(dspi->flags & SPI_FLAG_INIT))
    {
        dspi->event = cb_event;
        dspi->flags = SPI_FLAG_INIT;
    }
    return ARM_DRIVER_OK;
}

static int32_t DSPI_EdmaUninitialize(cmsis_dspi_edma_driver_state_t *dspi)
{
    dspi->flags = SPI_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t DSPI_EdmaPowerControl(ARM_POWER_STATE state, cmsis_dspi_edma_driver_state_t *dspi)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            if (dspi->flags & SPI_FLAG_POWER)
            {
                DSPI_Deinit(dspi->resource->base);
                DMAMUX_DisableChannel(dspi->dmaResource->rxDmamuxBase, dspi->dmaResource->rxEdmaChannel);
                DMAMUX_DisableChannel(dspi->dmaResource->txDmamuxBase, dspi->dmaResource->txEdmaChannel);
                dspi->flags = SPI_FLAG_INIT;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            if (dspi->flags == SPI_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (dspi->flags & SPI_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }
            /* Enable Clock gate */
            CLOCK_EnableClock(s_dspiClock[dspi->resource->instance]);
            dspi->flags |= SPI_FLAG_POWER;
            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

static int32_t DSPI_EdmaSend(const void *data, uint32_t num, cmsis_dspi_edma_driver_state_t *dspi)
{
    int32_t ret;
    status_t status;
    dspi_transfer_t xfer = {0};

    xfer.rxData   = NULL;
    xfer.txData   = (uint8_t *)data;
    xfer.dataSize = num;

    if (DSPI_IsMaster(dspi->resource->base))
    {
#if (defined(RTE_SPI0_MASTER_PCS_PIN_SEL))
        if (0U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI0_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI0 PCS select configuration */
#if (defined(RTE_SPI1_MASTER_PCS_PIN_SEL))
        if (1U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI1_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI1 PCS select configuration */
#if (defined(RTE_SPI2_MASTER_PCS_PIN_SEL))
        if (2U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI2_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI2 PCS select configuration */
    }
    else
    {
        /* DSPI slave use Ctar0 setting and slave can only use PCS0. */
        xfer.configFlags = kDSPI_SlaveCtar0;
    }

    if (DSPI_IsMaster(dspi->resource->base))
    {
        status = DSPI_MasterTransferEDMA(dspi->resource->base, &dspi->handle->masterHandle, &xfer);
    }
    else
    {
        status = DSPI_SlaveTransferEDMA(dspi->resource->base, &dspi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_DSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t DSPI_EdmaReceive(void *data, uint32_t num, cmsis_dspi_edma_driver_state_t *dspi)
{
    int32_t ret;
    status_t status;
    dspi_transfer_t xfer = {0};

    xfer.txData   = NULL;
    xfer.rxData   = (uint8_t *)data;
    xfer.dataSize = num;

    if (DSPI_IsMaster(dspi->resource->base))
    {
#if (defined(RTE_SPI0_MASTER_PCS_PIN_SEL))
        if (0U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI0_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI0 PCS select configuration */
#if (defined(RTE_SPI1_MASTER_PCS_PIN_SEL))
        if (1U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI1_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI1 PCS select configuration */
#if (defined(RTE_SPI2_MASTER_PCS_PIN_SEL))
        if (2U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI2_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI2 PCS select configuration */
    }
    else
    {
        /* DSPI slave use Ctar0 setting and slave can only use PCS0. */
        xfer.configFlags = kDSPI_SlaveCtar0;
    }

    if (DSPI_IsMaster(dspi->resource->base))
    {
        status = DSPI_MasterTransferEDMA(dspi->resource->base, &dspi->handle->masterHandle, &xfer);
    }
    else
    {
        status = DSPI_SlaveTransferEDMA(dspi->resource->base, &dspi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_DSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t DSPI_EdmaTransfer(const void *data_out,
                                 void *data_in,
                                 uint32_t num,
                                 cmsis_dspi_edma_driver_state_t *dspi)
{
    int32_t ret;
    status_t status;
    dspi_transfer_t xfer = {0};

    xfer.txData   = (uint8_t *)data_out;
    xfer.rxData   = (uint8_t *)data_in;
    xfer.dataSize = num;

    if (DSPI_IsMaster(dspi->resource->base))
    {
#if (defined(RTE_SPI0_MASTER_PCS_PIN_SEL))
        if (0U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI0_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI0 PCS select configuration */
#if (defined(RTE_SPI1_MASTER_PCS_PIN_SEL))
        if (1U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI1_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI1 PCS select configuration */
#if (defined(RTE_SPI2_MASTER_PCS_PIN_SEL))
        if (2U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI2_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI2 PCS select configuration */
    }
    else
    {
        /* DSPI slave use Ctar0 setting and slave can only use PCS0. */
        xfer.configFlags = kDSPI_SlaveCtar0;
    }

    if (DSPI_IsMaster(dspi->resource->base))
    {
        status = DSPI_MasterTransferEDMA(dspi->resource->base, &dspi->handle->masterHandle, &xfer);
    }
    else
    {
        status = DSPI_SlaveTransferEDMA(dspi->resource->base, &dspi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_DSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}
static uint32_t DSPI_EdmaGetCount(cmsis_dspi_edma_driver_state_t *dspi)
{
    uint32_t cnt;
    size_t bytes;

    if (DSPI_IsMaster(dspi->resource->base))
    {
        if (dspi->handle->masterHandle.state != kDSPI_Idle)
        {
            cnt = 0UL;
            return cnt;
        }
        bytes = (uint32_t)dspi->handle->masterHandle.nbytes *
                EDMA_GetRemainingMajorLoopCount(dspi->dmaResource->rxEdmaBase, dspi->dmaResource->rxEdmaChannel);
        cnt = dspi->handle->masterHandle.totalByteCount - bytes;
    }
    else
    {
        if (dspi->handle->slaveHandle.state != kDSPI_Idle)
        {
            cnt = 0UL;
            return cnt;
        }
        bytes = (uint32_t)dspi->handle->slaveHandle.nbytes *
                EDMA_GetRemainingMajorLoopCount(dspi->dmaResource->rxEdmaBase, dspi->dmaResource->rxEdmaChannel);
        cnt = dspi->handle->slaveHandle.totalByteCount - bytes;
    }

    return cnt;
}

static int32_t DSPI_EdmaControl(uint32_t control, uint32_t arg, cmsis_dspi_edma_driver_state_t *dspi)
{
    if (!(dspi->flags & SPI_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    cmsis_dspi_edma_resource_t *dmaResource = dspi->dmaResource;

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE:
            DSPI_Enable(dspi->resource->base, false);
            return ARM_DRIVER_OK;

        case ARM_SPI_MODE_MASTER: /* SPI Master (Output on SOUT, Input on SIN); arg = Bus Speed in bps */
            DSPI_SetMasterSlaveMode(dspi->resource->base, kDSPI_Master);
            memset((dspi->edmaRxRegToRxDataHandle), 0, sizeof(edma_handle_t));
            memset((dspi->edmaTxDataToIntermediaryHandle), 0, sizeof(edma_handle_t));
            memset((dspi->edmaIntermediaryToTxRegHandle), 0, sizeof(edma_handle_t));

            EDMA_CreateHandle(dspi->edmaRxRegToRxDataHandle, dmaResource->rxEdmaBase, dmaResource->rxEdmaChannel);
            DMAMUX_SetSource(dmaResource->rxDmamuxBase, dmaResource->rxEdmaChannel, dmaResource->rxDmaRequest);
            DMAMUX_EnableChannel(dmaResource->rxDmamuxBase, dmaResource->rxEdmaChannel);

            EDMA_CreateHandle(dspi->edmaTxDataToIntermediaryHandle, dmaResource->txEdmaBase,
                              dmaResource->txEdmaChannel);

            EDMA_CreateHandle(dspi->edmaIntermediaryToTxRegHandle, dmaResource->masterLinkEdmaBase,
                              dmaResource->masterLinkChannel);

            if (1 == FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(dspi->resource->base))
            {
                DMAMUX_SetSource(dmaResource->txDmamuxBase, dmaResource->masterLinkChannel, dmaResource->txDmaRequest);
                DMAMUX_EnableChannel(dmaResource->txDmamuxBase, dmaResource->masterLinkChannel);
            }
            DSPI_MasterTransferCreateHandleEDMA(dspi->resource->base, &(dspi->handle->masterHandle),
                                                KSDK_DSPI_MasterEdmaCallback, (void *)dspi->event,
                                                (dspi->edmaRxRegToRxDataHandle), (dspi->edmaTxDataToIntermediaryHandle),
                                                (dspi->edmaIntermediaryToTxRegHandle));

            dspi->baudRate_Bps = arg;
            dspi->flags |= SPI_FLAG_CONFIGURED;

            break;

        case ARM_SPI_MODE_SLAVE: /* SPI Slave  (Output on SOUT, Input on SIN) */
            DSPI_SetMasterSlaveMode(dspi->resource->base, kDSPI_Slave);
            memset(dspi->edmaRxRegToRxDataHandle, 0, sizeof(edma_handle_t));
            memset(dspi->edmaTxDataToTxRegHandle, 0, sizeof(edma_handle_t));

            EDMA_CreateHandle(dspi->edmaTxDataToTxRegHandle, dmaResource->txEdmaBase, dmaResource->txEdmaChannel);
            if (1 == FSL_FEATURE_DSPI_HAS_SEPARATE_DMA_RX_TX_REQn(dspi->resource->base))
            {
                DMAMUX_SetSource(dmaResource->txDmamuxBase, dmaResource->txEdmaChannel, dmaResource->txDmaRequest);
                DMAMUX_EnableChannel(dmaResource->txDmamuxBase, dmaResource->txEdmaChannel);
            }

            EDMA_CreateHandle(dspi->edmaRxRegToRxDataHandle, dmaResource->rxEdmaBase, dmaResource->rxEdmaChannel);
            DMAMUX_SetSource(dmaResource->rxDmamuxBase, dmaResource->rxEdmaChannel, dmaResource->rxDmaRequest);
            DMAMUX_EnableChannel(dmaResource->rxDmamuxBase, dmaResource->rxEdmaChannel);

            DSPI_SlaveTransferCreateHandleEDMA(dspi->resource->base, &(dspi->handle->slaveHandle),
                                               KSDK_DSPI_SlaveEdmaCallback, (void *)dspi->event,
                                               (dspi->edmaRxRegToRxDataHandle), (dspi->edmaTxDataToTxRegHandle));

            dspi->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_SET_BUS_SPEED: /* Set Bus Speed in bps; */
            if (!DSPI_IsMaster(dspi->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            if (0 == DSPI_MasterSetBaudRate(dspi->resource->base, kDSPI_Ctar0, arg, dspi->resource->GetFreq()))
            {
                return ARM_DRIVER_ERROR;
            }

            dspi->baudRate_Bps = arg;
            return ARM_DRIVER_OK;

        case ARM_SPI_GET_BUS_SPEED: /* Set Bus Speed in bps; arg = value */
            if (!DSPI_IsMaster(dspi->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            return dspi->baudRate_Bps;

        case ARM_SPI_CONTROL_SS: /* Control Slave Select; arg = 0:inactive, 1:active  */
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_ABORT_TRANSFER: /* Abort current data transfer */
            if (DSPI_IsMaster(dspi->resource->base))
            {
                DSPI_MasterTransferAbortEDMA(dspi->resource->base, &dspi->handle->masterHandle);
            }
            else
            {
                DSPI_SlaveTransferAbortEDMA(dspi->resource->base, &dspi->handle->slaveHandle);
            }
            return ARM_DRIVER_OK;

        case ARM_SPI_SET_DEFAULT_TX_VALUE: /* Set default Transmit value; arg = value */
            DSPI_SetDummyData(dspi->resource->base, (uint8_t)arg);
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

    return DSPI_CommonControl(control, dspi->baudRate_Bps, dspi->resource, &dspi->flags);
}

ARM_SPI_STATUS DSPI_EdmaGetStatus(cmsis_dspi_edma_driver_state_t *dspi)
{
    ARM_SPI_STATUS stat;
    uint32_t ksdk_dspi_status = DSPI_GetStatusFlags(dspi->resource->base);

    if (DSPI_IsMaster(dspi->resource->base))
    {
        stat.busy = (kDSPI_Busy == dspi->handle->masterHandle.state) ? (1U) : (0U);
    }
    else
    {
        stat.busy = (kDSPI_Busy == dspi->handle->slaveHandle.state) ? (1U) : (0U);
    }

    stat.data_lost =
        ((ksdk_dspi_status & kDSPI_TxFifoUnderflowFlag) || (ksdk_dspi_status & kDSPI_RxFifoOverflowFlag)) ? (1U) : (0U);
    stat.mode_fault = 0U;
    stat.reserved   = 0U;

    return stat;
}
#endif /* defined(FSL_FEATURE_SOC_EDMA_COUNT) */

#endif

#if ((RTE_SPI0 && !RTE_SPI0_DMA_EN) || (RTE_SPI1 && !RTE_SPI1_DMA_EN) || (RTE_SPI2 && !RTE_SPI2_DMA_EN))

void KSDK_DSPI_MasterInterruptCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_DSPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}
void KSDK_DSPI_SlaveInterruptCallback(SPI_Type *base, dspi_slave_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }

    if (kStatus_DSPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

static int32_t DSPI_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_dspi_interrupt_driver_state_t *dspi)
{
    if (!(dspi->flags & SPI_FLAG_INIT))
    {
        dspi->event = cb_event;
        dspi->flags = SPI_FLAG_INIT;
    }
    return ARM_DRIVER_OK;
}

static int32_t DSPI_InterruptUninitialize(cmsis_dspi_interrupt_driver_state_t *dspi)
{
    dspi->flags = SPI_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t DSPI_InterruptPowerControl(ARM_POWER_STATE state, cmsis_dspi_interrupt_driver_state_t *dspi)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            if (dspi->flags & SPI_FLAG_POWER)
            {
                DSPI_Deinit(dspi->resource->base);
                dspi->flags = SPI_FLAG_INIT;
            }
            break;

        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_POWER_FULL:
            if (dspi->flags == SPI_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (dspi->flags & SPI_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            CLOCK_EnableClock(s_dspiClock[dspi->resource->instance]);
            dspi->flags |= SPI_FLAG_POWER;

            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

static int32_t DSPI_InterruptSend(const void *data, uint32_t num, cmsis_dspi_interrupt_driver_state_t *dspi)
{
    int32_t ret;
    status_t status;
    dspi_transfer_t xfer = {0};

    xfer.rxData   = NULL;
    xfer.txData   = (uint8_t *)data;
    xfer.dataSize = num;

    if (DSPI_IsMaster(dspi->resource->base))
    {
#if (defined(RTE_SPI0_MASTER_PCS_PIN_SEL))
        if (0U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI0_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI0 PCS select configuration */
#if (defined(RTE_SPI1_MASTER_PCS_PIN_SEL))
        if (1U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI1_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI1 PCS select configuration */
#if (defined(RTE_SPI2_MASTER_PCS_PIN_SEL))
        if (2U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI2_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI2 PCS select configuration */
    }
    else
    {
        /* DSPI slave use Ctar0 setting and slave can only use PCS0. */
        xfer.configFlags = kDSPI_SlaveCtar0;
    }

    if (DSPI_IsMaster(dspi->resource->base))
    {
        status = DSPI_MasterTransferNonBlocking(dspi->resource->base, &dspi->handle->masterHandle, &xfer);
    }
    else
    {
        status = DSPI_SlaveTransferNonBlocking(dspi->resource->base, &dspi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_DSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t DSPI_InterruptReceive(void *data, uint32_t num, cmsis_dspi_interrupt_driver_state_t *dspi)
{
    int32_t ret;
    status_t status;
    dspi_transfer_t xfer = {0};

    xfer.txData   = NULL;
    xfer.rxData   = (uint8_t *)data;
    xfer.dataSize = num;

    if (DSPI_IsMaster(dspi->resource->base))
    {
#if (defined(RTE_SPI0_MASTER_PCS_PIN_SEL))
        if (0U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI0_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI0 PCS select configuration */
#if (defined(RTE_SPI1_MASTER_PCS_PIN_SEL))
        if (1U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI1_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI1 PCS select configuration */
#if (defined(RTE_SPI2_MASTER_PCS_PIN_SEL))
        if (2U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI2_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI2 PCS select configuration */
    }
    else
    {
        /* DSPI slave use Ctar0 setting and slave can only use PCS0. */
        xfer.configFlags = kDSPI_SlaveCtar0;
    }

    if (DSPI_IsMaster(dspi->resource->base))
    {
        status = DSPI_MasterTransferNonBlocking(dspi->resource->base, &dspi->handle->masterHandle, &xfer);
    }
    else
    {
        status = DSPI_SlaveTransferNonBlocking(dspi->resource->base, &dspi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_DSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t DSPI_InterruptTransfer(const void *data_out,
                                      void *data_in,
                                      uint32_t num,
                                      cmsis_dspi_interrupt_driver_state_t *dspi)
{
    int32_t ret;
    status_t status;
    dspi_transfer_t xfer = {0};

    xfer.txData   = (uint8_t *)data_out;
    xfer.rxData   = (uint8_t *)data_in;
    xfer.dataSize = num;

    if (DSPI_IsMaster(dspi->resource->base))
    {
#if (defined(RTE_SPI0_MASTER_PCS_PIN_SEL))
        if (0U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI0_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI0 PCS select configuration */
#if (defined(RTE_SPI1_MASTER_PCS_PIN_SEL))
        if (1U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI1_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI1 PCS select configuration */
#if (defined(RTE_SPI2_MASTER_PCS_PIN_SEL))
        if (2U == dspi->resource->instance)
        {
            xfer.configFlags = kDSPI_MasterCtar0 | RTE_SPI2_MASTER_PCS_PIN_SEL;
        }
#endif /* DSPI2 PCS select configuration */
    }
    else
    {
        /* DSPI slave use Ctar0 setting and slave can only use PCS0. */
        xfer.configFlags = kDSPI_SlaveCtar0;
    }

    if (DSPI_IsMaster(dspi->resource->base))
    {
        status = DSPI_MasterTransferNonBlocking(dspi->resource->base, &dspi->handle->masterHandle, &xfer);
    }
    else
    {
        status = DSPI_SlaveTransferNonBlocking(dspi->resource->base, &dspi->handle->slaveHandle, &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_DSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}
static uint32_t DSPI_InterruptGetCount(cmsis_dspi_interrupt_driver_state_t *dspi)
{
    if (DSPI_IsMaster(dspi->resource->base))
    {
        return dspi->handle->masterHandle.totalByteCount - dspi->handle->masterHandle.remainingReceiveByteCount;
    }
    else
    {
        return dspi->handle->slaveHandle.totalByteCount - dspi->handle->slaveHandle.remainingReceiveByteCount;
    }
}

static int32_t DSPI_InterruptControl(uint32_t control, uint32_t arg, cmsis_dspi_interrupt_driver_state_t *dspi)
{
    if (!(dspi->flags & SPI_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE:
            DSPI_Enable(dspi->resource->base, false);
            return ARM_DRIVER_OK;

        case ARM_SPI_MODE_MASTER: /* SPI Master (Output on SOUT, Input on SIN); arg = Bus Speed in bps */
            DSPI_SetMasterSlaveMode(dspi->resource->base, kDSPI_Master);
            DSPI_MasterTransferCreateHandle(dspi->resource->base, &dspi->handle->masterHandle,
                                            KSDK_DSPI_MasterInterruptCallback, (void *)dspi->event);
            dspi->baudRate_Bps = arg;
            dspi->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_MODE_SLAVE: /* SPI Slave  (Output on SOUT, Input on SIN) */
            DSPI_SetMasterSlaveMode(dspi->resource->base, kDSPI_Slave);
            DSPI_SlaveTransferCreateHandle(dspi->resource->base, &dspi->handle->slaveHandle,
                                           KSDK_DSPI_SlaveInterruptCallback, (void *)dspi->event);

            dspi->flags |= SPI_FLAG_CONFIGURED;
            break;

        case ARM_SPI_GET_BUS_SPEED: /* Get Bus Speed in bps */
            if (!DSPI_IsMaster(dspi->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            return dspi->baudRate_Bps;

        case ARM_SPI_SET_BUS_SPEED: /* Set Bus Speed in bps; */
            if (!DSPI_IsMaster(dspi->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }
            if (0 == DSPI_MasterSetBaudRate(dspi->resource->base, kDSPI_Ctar0, arg, dspi->resource->GetFreq()))
            {
                return ARM_DRIVER_ERROR;
            }

            dspi->baudRate_Bps = arg;
            return ARM_DRIVER_OK;

        case ARM_SPI_CONTROL_SS: /* Control Slave Select; arg = 0:inactive, 1:active  */
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_ABORT_TRANSFER: /* Abort current data transfer */
            if (DSPI_IsMaster(dspi->resource->base))
            {
                DSPI_MasterTransferAbort(dspi->resource->base, &dspi->handle->masterHandle);
            }
            else
            {
                DSPI_SlaveTransferAbort(dspi->resource->base, &dspi->handle->slaveHandle);
            }
            return ARM_DRIVER_OK;

        case ARM_SPI_SET_DEFAULT_TX_VALUE: /* Set default Transmit value; arg = value */
            DSPI_SetDummyData(dspi->resource->base, (uint8_t)arg);
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
    return DSPI_CommonControl(control, dspi->baudRate_Bps, dspi->resource, &dspi->flags);
}

ARM_SPI_STATUS DSPI_InterruptGetStatus(cmsis_dspi_interrupt_driver_state_t *dspi)
{
    ARM_SPI_STATUS stat;
    uint32_t ksdk_dspi_status = DSPI_GetStatusFlags(dspi->resource->base);

    if (DSPI_IsMaster(dspi->resource->base))
    {
        stat.busy = ((dspi->handle->masterHandle.remainingSendByteCount > 0) ||
                     (dspi->handle->masterHandle.remainingReceiveByteCount > 0)) ?
                        (1U) :
                        (0U);
    }
    else
    {
        stat.busy = ((dspi->handle->slaveHandle.remainingSendByteCount > 0) ||
                     (dspi->handle->slaveHandle.remainingReceiveByteCount > 0)) ?
                        (1U) :
                        (0U);
    }
    stat.data_lost =
        ((ksdk_dspi_status & kDSPI_TxFifoUnderflowFlag) || (ksdk_dspi_status & kDSPI_RxFifoOverflowFlag)) ? (1U) : (0U);
    stat.mode_fault = 0U;
    stat.reserved   = 0U;

    return stat;
}

#endif

#if defined(DSPI0) && RTE_SPI0

/* User needs to provide the implementation for DSPI0_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t DSPI0_GetFreq(void);
extern void DSPI0_InitPins(void);
extern void DSPI0_DeinitPins(void);

cmsis_dspi_resource_t DSPI0_Resource = {DSPI0, 0, DSPI0_GetFreq};

#if RTE_SPI0_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_dspi_edma_resource_t DSPI0_EdmaResource = {
    RTE_SPI0_DMA_TX_DMA_BASE,   RTE_SPI0_DMA_TX_CH,  RTE_SPI0_DMA_TX_DMAMUX_BASE, RTE_SPI0_DMA_TX_PERI_SEL,

    RTE_SPI0_DMA_RX_DMA_BASE,   RTE_SPI0_DMA_RX_CH,  RTE_SPI0_DMA_RX_DMAMUX_BASE, RTE_SPI0_DMA_RX_PERI_SEL,

    RTE_SPI0_DMA_LINK_DMA_BASE, RTE_SPI0_DMA_LINK_CH};

cmsis_dspi_edma_handle_t DSPI0_EdmaHandle;

edma_handle_t DSPI0_EdmaRxRegToRxDataHandle;
edma_handle_t DSPI0_EedmaTxDataToIntermediaryHandle;
edma_handle_t DSPI0_EedmaIntermediaryToTxRegHandle;
edma_handle_t DSPI0_EedmaTxDataToTxRegHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("dspi0_edma_driver_state")
cmsis_dspi_edma_driver_state_t DSPI0_EdmaDriverState = {
#else
cmsis_dspi_edma_driver_state_t DSPI0_EdmaDriverState = {
#endif
    &DSPI0_Resource,
    &DSPI0_EdmaResource,
    &DSPI0_EdmaHandle,
    &DSPI0_EdmaRxRegToRxDataHandle,
    &DSPI0_EedmaTxDataToIntermediaryHandle,
    &DSPI0_EedmaIntermediaryToTxRegHandle,
    &DSPI0_EedmaTxDataToTxRegHandle,
};

static int32_t DSPI0_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    DSPI0_InitPins();
    return DSPI_EdmaInitialize(cb_event, &DSPI0_EdmaDriverState);
}

static int32_t DSPI0_EdmaUninitialize(void)
{
    DSPI0_DeinitPins();
    return DSPI_EdmaUninitialize(&DSPI0_EdmaDriverState);
}

static int32_t DSPI0_EdmaPowerControl(ARM_POWER_STATE state)
{
    return DSPI_EdmaPowerControl(state, &DSPI0_EdmaDriverState);
}

static int32_t DSPI0_EdmaSend(const void *data, uint32_t num)
{
    return DSPI_EdmaSend(data, num, &DSPI0_EdmaDriverState);
}

static int32_t DSPI0_EdmaReceive(void *data, uint32_t num)
{
    return DSPI_EdmaReceive(data, num, &DSPI0_EdmaDriverState);
}

static int32_t DSPI0_EdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return DSPI_EdmaTransfer(data_out, data_in, num, &DSPI0_EdmaDriverState);
}

static uint32_t DSPI0_EdmaGetCount(void)
{
    return DSPI_EdmaGetCount(&DSPI0_EdmaDriverState);
}

static int32_t DSPI0_EdmaControl(uint32_t control, uint32_t arg)
{
    return DSPI_EdmaControl(control, arg, &DSPI0_EdmaDriverState);
}

static ARM_SPI_STATUS DSPI0_EdmaGetStatus(void)
{
    return DSPI_EdmaGetStatus(&DSPI0_EdmaDriverState);
}

#endif

#else
cmsis_dspi_handle_t DSPI0_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("dspi0_interrupt_driver_state")
cmsis_dspi_interrupt_driver_state_t DSPI0_InterruptDriverState = {
#else
cmsis_dspi_interrupt_driver_state_t DSPI0_InterruptDriverState = {
#endif
    &DSPI0_Resource,
    &DSPI0_Handle,
};

static int32_t DSPI0_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    DSPI0_InitPins();
    return DSPI_InterruptInitialize(cb_event, &DSPI0_InterruptDriverState);
}

static int32_t DSPI0_InterruptUninitialize(void)
{
    DSPI0_DeinitPins();
    return DSPI_InterruptUninitialize(&DSPI0_InterruptDriverState);
}

static int32_t DSPI0_InterruptPowerControl(ARM_POWER_STATE state)
{
    return DSPI_InterruptPowerControl(state, &DSPI0_InterruptDriverState);
}

static int32_t DSPI0_InterruptSend(const void *data, uint32_t num)
{
    return DSPI_InterruptSend(data, num, &DSPI0_InterruptDriverState);
}

static int32_t DSPI0_InterruptReceive(void *data, uint32_t num)
{
    return DSPI_InterruptReceive(data, num, &DSPI0_InterruptDriverState);
}

static int32_t DSPI0_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return DSPI_InterruptTransfer(data_out, data_in, num, &DSPI0_InterruptDriverState);
}

static uint32_t DSPI0_InterruptGetCount(void)
{
    return DSPI_InterruptGetCount(&DSPI0_InterruptDriverState);
}

static int32_t DSPI0_InterruptControl(uint32_t control, uint32_t arg)
{
    return DSPI_InterruptControl(control, arg, &DSPI0_InterruptDriverState);
}

static ARM_SPI_STATUS DSPI0_InterruptGetStatus(void)
{
    return DSPI_InterruptGetStatus(&DSPI0_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI0 = {DSPIx_GetVersion,     DSPIx_GetCapabilities,
#if RTE_SPI0_DMA_EN
                              DSPI0_EdmaInitialize, DSPI0_EdmaUninitialize, DSPI0_EdmaPowerControl, DSPI0_EdmaSend,
                              DSPI0_EdmaReceive,    DSPI0_EdmaTransfer,     DSPI0_EdmaGetCount,     DSPI0_EdmaControl,
                              DSPI0_EdmaGetStatus
#else
                              DSPI0_InterruptInitialize,
                              DSPI0_InterruptUninitialize,
                              DSPI0_InterruptPowerControl,
                              DSPI0_InterruptSend,
                              DSPI0_InterruptReceive,
                              DSPI0_InterruptTransfer,
                              DSPI0_InterruptGetCount,
                              DSPI0_InterruptControl,
                              DSPI0_InterruptGetStatus
#endif
};

#endif /*  DSPI0  */

#if defined(DSPI1) && RTE_SPI1

/* User needs to provide the implementation for DSPI1_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t DSPI1_GetFreq(void);
extern void DSPI1_InitPins(void);
extern void DSPI1_DeinitPins(void);

cmsis_dspi_resource_t DSPI1_Resource = {DSPI1, 1, DSPI1_GetFreq};

#if RTE_SPI1_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_dspi_edma_resource_t DSPI1_EdmaResource = {
    RTE_SPI1_DMA_TX_DMA_BASE,   RTE_SPI1_DMA_TX_CH,  RTE_SPI1_DMA_TX_DMAMUX_BASE, RTE_SPI1_DMA_TX_PERI_SEL,

    RTE_SPI1_DMA_RX_DMA_BASE,   RTE_SPI1_DMA_RX_CH,  RTE_SPI1_DMA_RX_DMAMUX_BASE, RTE_SPI1_DMA_RX_PERI_SEL,

    RTE_SPI1_DMA_LINK_DMA_BASE, RTE_SPI1_DMA_LINK_CH};

cmsis_dspi_edma_handle_t DSPI1_EdmaHandle;

edma_handle_t DSPI1_EdmaRxRegToRxDataHandle;
edma_handle_t DSPI1_EedmaTxDataToIntermediaryHandle;
edma_handle_t DSPI1_EedmaIntermediaryToTxRegHandle;
edma_handle_t DSPI1_EedmaTxDataToTxRegHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("dspi1_edma_driver_state")
cmsis_dspi_edma_driver_state_t DSPI1_EdmaDriverState = {
#else
cmsis_dspi_edma_driver_state_t DSPI1_EdmaDriverState = {
#endif
    &DSPI1_Resource,
    &DSPI1_EdmaResource,
    &DSPI1_EdmaHandle,
    &DSPI1_EdmaRxRegToRxDataHandle,
    &DSPI1_EedmaTxDataToIntermediaryHandle,
    &DSPI1_EedmaIntermediaryToTxRegHandle,
    &DSPI1_EedmaTxDataToTxRegHandle,
};

static int32_t DSPI1_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    DSPI1_InitPins();
    return DSPI_EdmaInitialize(cb_event, &DSPI1_EdmaDriverState);
}

static int32_t DSPI1_EdmaUninitialize(void)
{
    DSPI1_DeinitPins();
    return DSPI_EdmaUninitialize(&DSPI1_EdmaDriverState);
}

static int32_t DSPI1_EdmaPowerControl(ARM_POWER_STATE state)
{
    return DSPI_EdmaPowerControl(state, &DSPI1_EdmaDriverState);
}

static int32_t DSPI1_EdmaSend(const void *data, uint32_t num)
{
    return DSPI_EdmaSend(data, num, &DSPI1_EdmaDriverState);
}

static int32_t DSPI1_EdmaReceive(void *data, uint32_t num)
{
    return DSPI_EdmaReceive(data, num, &DSPI1_EdmaDriverState);
}

static int32_t DSPI1_EdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return DSPI_EdmaTransfer(data_out, data_in, num, &DSPI1_EdmaDriverState);
}

static uint32_t DSPI1_EdmaGetCount(void)
{
    return DSPI_EdmaGetCount(&DSPI1_EdmaDriverState);
}

static int32_t DSPI1_EdmaControl(uint32_t control, uint32_t arg)
{
    return DSPI_EdmaControl(control, arg, &DSPI1_EdmaDriverState);
}

static ARM_SPI_STATUS DSPI1_EdmaGetStatus(void)
{
    return DSPI_EdmaGetStatus(&DSPI1_EdmaDriverState);
}

#endif

#else

cmsis_dspi_handle_t DSPI1_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("dspi1_interrupt_driver_state")
cmsis_dspi_interrupt_driver_state_t DSPI1_InterruptDriverState = {
#else
cmsis_dspi_interrupt_driver_state_t DSPI1_InterruptDriverState = {
#endif
    &DSPI1_Resource,
    &DSPI1_Handle,
};

static int32_t DSPI1_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    DSPI1_InitPins();
    return DSPI_InterruptInitialize(cb_event, &DSPI1_InterruptDriverState);
}

static int32_t DSPI1_InterruptUninitialize(void)
{
    DSPI1_DeinitPins();
    return DSPI_InterruptUninitialize(&DSPI1_InterruptDriverState);
}

static int32_t DSPI1_InterruptPowerControl(ARM_POWER_STATE state)
{
    return DSPI_InterruptPowerControl(state, &DSPI1_InterruptDriverState);
}

static int32_t DSPI1_InterruptSend(const void *data, uint32_t num)
{
    return DSPI_InterruptSend(data, num, &DSPI1_InterruptDriverState);
}

static int32_t DSPI1_InterruptReceive(void *data, uint32_t num)
{
    return DSPI_InterruptReceive(data, num, &DSPI1_InterruptDriverState);
}

static int32_t DSPI1_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return DSPI_InterruptTransfer(data_out, data_in, num, &DSPI1_InterruptDriverState);
}

static uint32_t DSPI1_InterruptGetCount(void)
{
    return DSPI_InterruptGetCount(&DSPI1_InterruptDriverState);
}

static int32_t DSPI1_InterruptControl(uint32_t control, uint32_t arg)
{
    return DSPI_InterruptControl(control, arg, &DSPI1_InterruptDriverState);
}

static ARM_SPI_STATUS DSPI1_InterruptGetStatus(void)
{
    return DSPI_InterruptGetStatus(&DSPI1_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI1 = {DSPIx_GetVersion,     DSPIx_GetCapabilities,
#if RTE_SPI1_DMA_EN
                              DSPI1_EdmaInitialize, DSPI1_EdmaUninitialize, DSPI1_EdmaPowerControl, DSPI1_EdmaSend,
                              DSPI1_EdmaReceive,    DSPI1_EdmaTransfer,     DSPI1_EdmaGetCount,     DSPI1_EdmaControl,
                              DSPI1_EdmaGetStatus
#else
                              DSPI1_InterruptInitialize,
                              DSPI1_InterruptUninitialize,
                              DSPI1_InterruptPowerControl,
                              DSPI1_InterruptSend,
                              DSPI1_InterruptReceive,
                              DSPI1_InterruptTransfer,
                              DSPI1_InterruptGetCount,
                              DSPI1_InterruptControl,
                              DSPI1_InterruptGetStatus
#endif
};

#endif /*  DSPI1  */

#if defined(DSPI2) && RTE_SPI2

/* User needs to provide the implementation for DSPI2_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t DSPI2_GetFreq(void);
extern void DSPI2_InitPins(void);
extern void DSPI2_DeinitPins(void);

cmsis_dspi_resource_t DSPI2_Resource = {DSPI2, 2, DSPI2_GetFreq};

#if RTE_SPI2_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_dspi_edma_resource_t DSPI2_EdmaResource = {
    RTE_SPI2_DMA_TX_DMA_BASE,   RTE_SPI2_DMA_TX_CH,  RTE_SPI2_DMA_TX_DMAMUX_BASE, RTE_SPI2_DMA_TX_PERI_SEL,

    RTE_SPI2_DMA_RX_DMA_BASE,   RTE_SPI2_DMA_RX_CH,  RTE_SPI2_DMA_RX_DMAMUX_BASE, RTE_SPI2_DMA_RX_PERI_SEL,

    RTE_SPI2_DMA_LINK_DMA_BASE, RTE_SPI2_DMA_LINK_CH};

cmsis_dspi_edma_handle_t DSPI2_EdmaHandle;

edma_handle_t DSPI2_EdmaRxRegToRxDataHandle;
edma_handle_t DSPI2_EedmaTxDataToIntermediaryHandle;
edma_handle_t DSPI2_EedmaIntermediaryToTxRegHandle;
edma_handle_t DSPI2_EedmaTxDataToTxRegHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("dspi2_edma_driver_state")
cmsis_dspi_edma_driver_state_t DSPI2_EdmaDriverState = {
#else
cmsis_dspi_edma_driver_state_t DSPI2_EdmaDriverState = {
#endif
    &DSPI2_Resource,
    &DSPI2_EdmaResource,
    &DSPI2_EdmaHandle,
    &DSPI2_EdmaRxRegToRxDataHandle,
    &DSPI2_EedmaTxDataToIntermediaryHandle,
    &DSPI2_EedmaIntermediaryToTxRegHandle,
    &DSPI2_EedmaTxDataToTxRegHandle,
};

static int32_t DSPI2_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    DSPI2_InitPins();
    return DSPI_EdmaInitialize(cb_event, &DSPI2_EdmaDriverState);
}

static int32_t DSPI2_EdmaUninitialize(void)
{
    DSPI2_DeinitPins();
    return DSPI_EdmaUninitialize(&DSPI2_EdmaDriverState);
}

static int32_t DSPI2_EdmaPowerControl(ARM_POWER_STATE state)
{
    return DSPI_EdmaPowerControl(state, &DSPI2_EdmaDriverState);
}

static int32_t DSPI2_EdmaSend(const void *data, uint32_t num)
{
    return DSPI_EdmaSend(data, num, &DSPI2_EdmaDriverState);
}

static int32_t DSPI2_EdmaReceive(void *data, uint32_t num)
{
    return DSPI_EdmaReceive(data, num, &DSPI2_EdmaDriverState);
}

static int32_t DSPI2_EdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return DSPI_EdmaTransfer(data_out, data_in, num, &DSPI2_EdmaDriverState);
}

static uint32_t DSPI2_EdmaGetCount(void)
{
    return DSPI_EdmaGetCount(&DSPI2_EdmaDriverState);
}

static int32_t DSPI2_EdmaControl(uint32_t control, uint32_t arg)
{
    return DSPI_EdmaControl(control, arg, &DSPI2_EdmaDriverState);
}

static ARM_SPI_STATUS DSPI2_EdmaGetStatus(void)
{
    return DSPI_EdmaGetStatus(&DSPI2_EdmaDriverState);
}

#endif

#else

cmsis_dspi_handle_t DSPI2_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("dspi2_interrupt_driver_state")
cmsis_dspi_interrupt_driver_state_t DSPI2_InterruptDriverState = {
#else
cmsis_dspi_interrupt_driver_state_t DSPI2_InterruptDriverState = {
#endif
    &DSPI2_Resource,
    &DSPI2_Handle,
};

static int32_t DSPI2_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    DSPI2_InitPins();
    return DSPI_InterruptInitialize(cb_event, &DSPI2_InterruptDriverState);
}

static int32_t DSPI2_InterruptUninitialize(void)
{
    DSPI2_DeinitPins();
    return DSPI_InterruptUninitialize(&DSPI2_InterruptDriverState);
}

static int32_t DSPI2_InterruptPowerControl(ARM_POWER_STATE state)
{
    return DSPI_InterruptPowerControl(state, &DSPI2_InterruptDriverState);
}

static int32_t DSPI2_InterruptSend(const void *data, uint32_t num)
{
    return DSPI_InterruptSend(data, num, &DSPI2_InterruptDriverState);
}

static int32_t DSPI2_InterruptReceive(void *data, uint32_t num)
{
    return DSPI_InterruptReceive(data, num, &DSPI2_InterruptDriverState);
}

static int32_t DSPI2_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return DSPI_InterruptTransfer(data_out, data_in, num, &DSPI2_InterruptDriverState);
}

static uint32_t DSPI2_InterruptGetCount(void)
{
    return DSPI_InterruptGetCount(&DSPI2_InterruptDriverState);
}

static int32_t DSPI2_InterruptControl(uint32_t control, uint32_t arg)
{
    return DSPI_InterruptControl(control, arg, &DSPI2_InterruptDriverState);
}

static ARM_SPI_STATUS DSPI2_InterruptGetStatus(void)
{
    return DSPI_InterruptGetStatus(&DSPI2_InterruptDriverState);
}
#endif

ARM_DRIVER_SPI Driver_SPI2 = {DSPIx_GetVersion,     DSPIx_GetCapabilities,
#if RTE_SPI2_DMA_EN
                              DSPI2_EdmaInitialize, DSPI2_EdmaUninitialize, DSPI2_EdmaPowerControl, DSPI2_EdmaSend,
                              DSPI2_EdmaReceive,    DSPI2_EdmaTransfer,     DSPI2_EdmaGetCount,     DSPI2_EdmaControl,
                              DSPI2_EdmaGetStatus
#else
                              DSPI2_InterruptInitialize,
                              DSPI2_InterruptUninitialize,
                              DSPI2_InterruptPowerControl,
                              DSPI2_InterruptSend,
                              DSPI2_InterruptReceive,
                              DSPI2_InterruptTransfer,
                              DSPI2_InterruptGetCount,
                              DSPI2_InterruptControl,
                              DSPI2_InterruptGetStatus
#endif
};

#endif /*  DSPI2  */
