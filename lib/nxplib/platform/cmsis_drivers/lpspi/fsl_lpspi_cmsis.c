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

#include "fsl_lpspi_cmsis.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpspi_cmsis"
#endif

#if ((RTE_SPI0 && defined(LPSPI0)) || (RTE_SPI1 && defined(LPSPI1)) || (RTE_SPI2 && defined(LPSPI2)) || \
     (RTE_SPI3 && defined(LPSPI3)) || (RTE_SPI4 && defined(LPSPI4)) || (RTE_SPI5 && defined(LPSPI5)))

#define ARM_LPSPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 1) /* driver version */

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

static clock_ip_name_t const s_lpspiClock[] = LPSPI_CLOCKS;

typedef const struct _cmsis_lpspi_resource
{
    LPSPI_Type *base;
    uint32_t instance;
    uint32_t (*GetFreq)(void);
} cmsis_lpspi_resource_t;

typedef union _cmsis_lpspi_handle
{
    lpspi_master_handle_t masterHandle;
    lpspi_slave_handle_t slaveHandle;
} cmsis_lpspi_handle_t;

typedef struct _cmsis_lpspi_interrupt_driver_state
{
    cmsis_lpspi_resource_t *resource;
    cmsis_lpspi_handle_t *handle;
    ARM_SPI_SignalEvent_t cb_event;
    uint32_t baudRate_Bps;
    uint8_t flags; /*!< Control and state flags. */
} cmsis_lpspi_interrupt_driver_state_t;

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
typedef const struct _cmsis_lpspi_edma_resource
{
    DMA_Type *txEdmaBase;
    uint32_t txEdmaChannel;
    uint8_t txDmaRequest;

    DMA_Type *rxEdmaBase;
    uint32_t rxEdmaChannel;
    uint8_t rxDmaRequest;

#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    DMAMUX_Type *txDmamuxBase;
    DMAMUX_Type *rxDmamuxBase;
#endif
} cmsis_lpspi_edma_resource_t;

typedef union _cmsis_lpspi_edma_handle
{
    lpspi_master_edma_handle_t masterHandle;
    lpspi_slave_edma_handle_t slaveHandle;
} cmsis_lpspi_edma_handle_t;

typedef struct _cmsis_lpspi_edma_driver_state
{
    cmsis_lpspi_resource_t *resource;
    cmsis_lpspi_edma_resource_t *dmaResource;
    cmsis_lpspi_edma_handle_t *handle;
    edma_handle_t *edmaRxRegToRxDataHandle;
    edma_handle_t *edmaTxDataToTxRegHandle;
    ARM_SPI_SignalEvent_t cb_event;
    uint32_t baudRate_Bps;
    uint8_t flags; /*!< Control and state flags. */
} cmsis_lpspi_edma_driver_state_t;
#endif

/* Driver Version */
static const ARM_DRIVER_VERSION s_lpspiDriverVersion = {ARM_SPI_API_VERSION, ARM_LPSPI_DRV_VERSION};

/* Driver Capabilities */
static const ARM_SPI_CAPABILITIES s_lpspiDriverCapabilities = {
    1, /* Simplex Mode (Master and Slave) */
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface  */
    0  /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
};

/*
 *Common Control function used by LPSPI_InterruptControl / LPSPI_EdmaControl.
 */
static int32_t LPSPI_CommonControl(uint32_t control,
                                   uint32_t arg,
                                   cmsis_lpspi_resource_t *resource,
                                   uint8_t *isConfigured)
{
    lpspi_master_config_t masterConfig;
    LPSPI_MasterGetDefaultConfig(&masterConfig);
    lpspi_slave_config_t slaveConfig;
    LPSPI_SlaveGetDefaultConfig(&slaveConfig);
    masterConfig.baudRate = arg;

    if (ARM_SPI_MODE_MASTER_SIMPLEX == (control & ARM_SPI_CONTROL_Msk))
    {
        masterConfig.pinCfg = kLPSPI_SdoInSdoOut;
    }
    if (ARM_SPI_MODE_SLAVE_SIMPLEX == (control & ARM_SPI_CONTROL_Msk))
    {
        slaveConfig.pinCfg = kLPSPI_SdiInSdiOut;
    }

#if (defined(RTE_SPI0_PCS_TO_SCK_DELAY) && defined(RTE_SPI0_SCK_TO_PSC_DELAY) && \
     defined(RTE_SPI0_BETWEEN_TRANSFER_DELAY))
    if (0U == resource->instance)
    {
        masterConfig.pcsToSckDelayInNanoSec        = RTE_SPI0_PCS_TO_SCK_DELAY;
        masterConfig.lastSckToPcsDelayInNanoSec    = RTE_SPI0_SCK_TO_PSC_DELAY;
        masterConfig.betweenTransferDelayInNanoSec = RTE_SPI0_BETWEEN_TRANSFER_DELAY;
    }
#endif /*RTE LPSPI0 trnafer delay time configure */

#if (defined(RTE_SPI1_PCS_TO_SCK_DELAY) && defined(RTE_SPI1_SCK_TO_PSC_DELAY) && \
     defined(RTE_SPI1_BETWEEN_TRANSFER_DELAY))
    if (1U == resource->instance)
    {
        masterConfig.pcsToSckDelayInNanoSec        = RTE_SPI1_PCS_TO_SCK_DELAY;
        masterConfig.lastSckToPcsDelayInNanoSec    = RTE_SPI1_SCK_TO_PSC_DELAY;
        masterConfig.betweenTransferDelayInNanoSec = RTE_SPI1_BETWEEN_TRANSFER_DELAY;
    }
#endif /*RTE LPSPI1 trnafer delay time configure */

#if (defined(RTE_SPI2_PCS_TO_SCK_DELAY) && defined(RTE_SPI2_SCK_TO_PSC_DELAY) && \
     defined(RTE_SPI2_BETWEEN_TRANSFER_DELAY))
    if (2U == resource->instance)
    {
        masterConfig.pcsToSckDelayInNanoSec        = RTE_SPI2_PCS_TO_SCK_DELAY;
        masterConfig.lastSckToPcsDelayInNanoSec    = RTE_SPI2_SCK_TO_PSC_DELAY;
        masterConfig.betweenTransferDelayInNanoSec = RTE_SPI2_BETWEEN_TRANSFER_DELAY;
    }
#endif /*RTE LPSPI2 trnafer delay time configure */
#if (defined(RTE_SPI3_PCS_TO_SCK_DELAY) && defined(RTE_SPI3_SCK_TO_PSC_DELAY) && \
     defined(RTE_SPI3_BETWEEN_TRANSFER_DELAY))
    if (3U == resource->instance)
    {
        masterConfig.pcsToSckDelayInNanoSec        = RTE_SPI3_PCS_TO_SCK_DELAY;
        masterConfig.lastSckToPcsDelayInNanoSec    = RTE_SPI3_SCK_TO_PSC_DELAY;
        masterConfig.betweenTransferDelayInNanoSec = RTE_SPI3_BETWEEN_TRANSFER_DELAY;
    }
#endif /*RTE LPSPI3 trnafer delay time configure */

#if (defined(RTE_SPI4_PCS_TO_SCK_DELAY) && defined(RTE_SPI4_SCK_TO_PSC_DELAY) && \
     defined(RTE_SPI4_BETWEEN_TRANSFER_DELAY))
    if (4U == resource->instance)
    {
        masterConfig.pcsToSckDelayInNanoSec        = RTE_SPI4_PCS_TO_SCK_DELAY;
        masterConfig.lastSckToPcsDelayInNanoSec    = RTE_SPI4_SCK_TO_PSC_DELAY;
        masterConfig.betweenTransferDelayInNanoSec = RTE_SPI4_BETWEEN_TRANSFER_DELAY;
    }
#endif /*RTE LPSPI4 trnafer delay time configure */

#if (defined(RTE_SPI5_PCS_TO_SCK_DELAY) && defined(RTE_SPI5_SCK_TO_PSC_DELAY) && \
     defined(RTE_SPI5_BETWEEN_TRANSFER_DELAY))
    if (5U == resource->instance)
    {
        masterConfig.pcsToSckDelayInNanoSec        = RTE_SPI5_PCS_TO_SCK_DELAY;
        masterConfig.lastSckToPcsDelayInNanoSec    = RTE_SPI5_SCK_TO_PSC_DELAY;
        masterConfig.betweenTransferDelayInNanoSec = RTE_SPI5_BETWEEN_TRANSFER_DELAY;
    }
#endif /*RTE LPSPI5 trnafer delay time configure */

    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
        case ARM_SPI_CPOL0_CPHA0: /* Clock Polarity 0, Clock Phase 0*/
            if (LPSPI_IsMaster(resource->base))
            {
                masterConfig.cpol = kLPSPI_ClockPolarityActiveHigh;
                masterConfig.cpha = kLPSPI_ClockPhaseFirstEdge;
            }
            else
            {
                slaveConfig.cpol = kLPSPI_ClockPolarityActiveHigh;
                slaveConfig.cpha = kLPSPI_ClockPhaseFirstEdge;
            }
            break;
        case ARM_SPI_CPOL0_CPHA1: /* Clock Polarity 0, Clock Phase 1*/
            if (LPSPI_IsMaster(resource->base))
            {
                masterConfig.cpol = kLPSPI_ClockPolarityActiveHigh;
                masterConfig.cpha = kLPSPI_ClockPhaseSecondEdge;
            }
            else
            {
                slaveConfig.cpol = kLPSPI_ClockPolarityActiveHigh;
                slaveConfig.cpha = kLPSPI_ClockPhaseSecondEdge;
            }
            break;
        case ARM_SPI_CPOL1_CPHA0: /* Clock Polarity 1, Clock Phase 0*/
            if (LPSPI_IsMaster(resource->base))
            {
                masterConfig.cpol = kLPSPI_ClockPolarityActiveLow;
                masterConfig.cpha = kLPSPI_ClockPhaseFirstEdge;
            }
            else
            {
                slaveConfig.cpol = kLPSPI_ClockPolarityActiveLow;
                slaveConfig.cpha = kLPSPI_ClockPhaseFirstEdge;
            }
            break;
        case ARM_SPI_CPOL1_CPHA1: /* Clock Polarity 1, Clock Phase 1*/
            if (LPSPI_IsMaster(resource->base))
            {
                masterConfig.cpol = kLPSPI_ClockPolarityActiveLow;
                masterConfig.cpha = kLPSPI_ClockPhaseSecondEdge;
            }
            else
            {
                slaveConfig.cpol = kLPSPI_ClockPolarityActiveLow;
                slaveConfig.cpha = kLPSPI_ClockPhaseSecondEdge;
            }
            break;
        default:
            break;
    }

    if (control & ARM_SPI_DATA_BITS_Msk) /* Number of Data bits */
    {
        if ((((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos) >= 8))
        {
            if (LPSPI_IsMaster(resource->base))
            {
                masterConfig.bitsPerFrame = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);
            }
            else
            {
                slaveConfig.bitsPerFrame = ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos);
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
            if (LPSPI_IsMaster(resource->base))
            {
                masterConfig.direction = kLPSPI_LsbFirst;
            }
            else
            {
                slaveConfig.direction = kLPSPI_LsbFirst;
            }
            break;
        case ARM_SPI_MSB_LSB: /* SPI Bit order from MSB to LSB */
            if (LPSPI_IsMaster(resource->base))
            {
                masterConfig.direction = kLPSPI_MsbFirst;
            }
            else
            {
                slaveConfig.direction = kLPSPI_MsbFirst;
            }
            break;
        default:
            break;
    }

    if (LPSPI_IsMaster(resource->base))
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

    /* LPSPI Init*/
    if (LPSPI_IsMaster(resource->base))
    {
        if ((*isConfigured) & SPI_FLAG_CONFIGURED)
        {
            LPSPI_Deinit(resource->base);
        }
        LPSPI_MasterInit(resource->base, &masterConfig, resource->GetFreq());
        *isConfigured |= SPI_FLAG_CONFIGURED;
    }
    else
    {
        if ((*isConfigured) & SPI_FLAG_CONFIGURED)
        {
            LPSPI_Deinit(resource->base);
        }
        LPSPI_SlaveInit(resource->base, &slaveConfig);
        *isConfigured |= SPI_FLAG_CONFIGURED;
    }

    return ARM_DRIVER_OK;
}

static void LPSPI_SetTransferConfigFlags(bool isMaster, uint32_t instance, lpspi_transfer_t *xfer)
{
    if (isMaster)
    {
        /* Set default config flag */
        xfer->configFlags = kLPSPI_MasterPcs0 | kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;
#if (defined(RTE_SPI0_MASTER_PCS_PIN_SEL))
        if (0U == instance)
        {
            xfer->configFlags = RTE_SPI0_MASTER_PCS_PIN_SEL | kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;
        }
#endif /* LPSPI0 PCS pin configuration */
#if (defined(RTE_SPI1_MASTER_PCS_PIN_SEL))
        if (1U == instance)
        {
            xfer->configFlags = RTE_SPI1_MASTER_PCS_PIN_SEL | kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;
        }
#endif /* LPSPI1 PCS pin configuration */
#if (defined(RTE_SPI2_MASTER_PCS_PIN_SEL))
        if (2U == instance)
        {
            xfer->configFlags = RTE_SPI2_MASTER_PCS_PIN_SEL | kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;
        }
#endif /* LPSPI2 PCS pin configuration */
#if (defined(RTE_SPI3_MASTER_PCS_PIN_SEL))
        if (3U == instance)
        {
            xfer->configFlags = RTE_SPI3_MASTER_PCS_PIN_SEL | kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;
        }
#endif /* LPSPI3 PCS pin configuration */
#if (defined(RTE_SPI4_MASTER_PCS_PIN_SEL))
        if (4U == instance)
        {
            xfer->configFlags = RTE_SPI4_MASTER_PCS_PIN_SEL | kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;
        }
#endif /* LPSPI4 PCS pin configuration */
#if (defined(RTE_SPI5_MASTER_PCS_PIN_SEL))
        if (5U == instance)
        {
            xfer->configFlags = RTE_SPI5_MASTER_PCS_PIN_SEL | kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;
        }
#endif /* LPSPI5 PCS pin configuration */
    }
    else
    {
        /* Set default config flag */
        xfer->configFlags = kLPSPI_SlavePcs0 | kLPSPI_SlaveByteSwap;
#if (defined(RTE_SPI0_SLAVE_PCS_PIN_SEL))
        if (0U == instance)
        {
            xfer->configFlags = RTE_SPI0_SLAVE_PCS_PIN_SEL | kLPSPI_SlaveByteSwap;
        }
#endif /* LPSPI0 PCS pin configuration */
#if (defined(RTE_SPI1_SLAVE_PCS_PIN_SEL))
        if (1U == instance)
        {
            xfer->configFlags = RTE_SPI1_SLAVE_PCS_PIN_SEL | kLPSPI_SlaveByteSwap;
        }
#endif /* LPSPI1 PCS pin configuration */
#if (defined(RTE_SPI2_SLAVE_PCS_PIN_SEL))
        if (2U == instance)
        {
            xfer->configFlags = RTE_SPI2_SLAVE_PCS_PIN_SEL | kLPSPI_SlaveByteSwap;
        }
#endif /* LPSPI2 PCS pin configuration */
#if (defined(RTE_SPI3_SLAVE_PCS_PIN_SEL))
        if (3U == instance)
        {
            xfer->configFlags = RTE_SPI3_SLAVE_PCS_PIN_SEL | kLPSPI_SlaveByteSwap;
        }
#endif /* LPSPI3 PCS pin configuration */
#if (defined(RTE_SPI4_SLAVE_PCS_PIN_SEL))
        if (4U == instance)
        {
            xfer->configFlags = RTE_SPI4_SLAVE_PCS_PIN_SEL | kLPSPI_SlaveByteSwap;
        }
#endif /* LPSPI4 PCS pin configuration */
#if (defined(RTE_SPI5_SLAVE_PCS_PIN_SEL))
        if (5U == instance)
        {
            xfer->configFlags = RTE_SPI5_SLAVE_PCS_PIN_SEL | kLPSPI_SlaveByteSwap;
        }
#endif /* LPSPI5 PCS pin configuration */
    }
}

static ARM_DRIVER_VERSION LPSPIx_GetVersion(void)
{
    return s_lpspiDriverVersion;
}

static ARM_SPI_CAPABILITIES LPSPIx_GetCapabilities(void)
{
    return s_lpspiDriverCapabilities;
}

#endif

#if (RTE_SPI0_DMA_EN || RTE_SPI1_DMA_EN || RTE_SPI2_DMA_EN || RTE_SPI3_DMA_EN || RTE_SPI4_DMA_EN || RTE_SPI5_DMA_EN)

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)

void KSDK_LPSPI_MasterEdmaCallback(LPSPI_Type *base,
                                   lpspi_master_edma_handle_t *handle,
                                   status_t status,
                                   void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }
    else if (kStatus_LPSPI_OutOfRange == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    else
    {
        __NOP();
    }
    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}
void KSDK_LPSPI_SlaveEdmaCallback(LPSPI_Type *base, lpspi_slave_edma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }
    else if (kStatus_LPSPI_OutOfRange == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    else
    {
        __NOP();
    }
    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

static int32_t LPSPI_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_lpspi_edma_driver_state_t *lpspi)
{
    if (!(lpspi->flags & SPI_FLAG_INIT))
    {
        lpspi->cb_event = cb_event;
        lpspi->flags    = SPI_FLAG_INIT;
    }
    return ARM_DRIVER_OK;
}

static int32_t LPSPI_EdmaUninitialize(cmsis_lpspi_edma_driver_state_t *lpspi)
{
    lpspi->flags = SPI_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t LPSPI_EdmaPowerControl(ARM_POWER_STATE state, cmsis_lpspi_edma_driver_state_t *lpspi)
{
    cmsis_lpspi_edma_resource_t *dmaResource = lpspi->dmaResource;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (lpspi->flags & SPI_FLAG_POWER)
            {
                LPSPI_Deinit(lpspi->resource->base);
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
                DMAMUX_DisableChannel(lpspi->dmaResource->rxDmamuxBase, lpspi->dmaResource->rxEdmaChannel);
                DMAMUX_DisableChannel(lpspi->dmaResource->txDmamuxBase, lpspi->dmaResource->txEdmaChannel);
#endif
                lpspi->flags = SPI_FLAG_INIT;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
        {
            if (lpspi->flags == SPI_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (lpspi->flags & SPI_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }
            /* Enable Clock gate */
            CLOCK_EnableClock(s_lpspiClock[lpspi->resource->instance]);

            memset(lpspi->edmaRxRegToRxDataHandle, 0, sizeof(edma_handle_t));
            memset(lpspi->edmaTxDataToTxRegHandle, 0, sizeof(edma_handle_t));

            EDMA_CreateHandle(lpspi->edmaRxRegToRxDataHandle, dmaResource->rxEdmaBase, dmaResource->rxEdmaChannel);
            EDMA_CreateHandle(lpspi->edmaTxDataToTxRegHandle, dmaResource->txEdmaBase, dmaResource->txEdmaChannel);

#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
            DMAMUX_SetSource(dmaResource->rxDmamuxBase, dmaResource->rxEdmaChannel, dmaResource->rxDmaRequest);
            DMAMUX_EnableChannel(dmaResource->rxDmamuxBase, dmaResource->rxEdmaChannel);

            DMAMUX_SetSource(dmaResource->txDmamuxBase, dmaResource->txEdmaChannel, dmaResource->txDmaRequest);
            DMAMUX_EnableChannel(dmaResource->txDmamuxBase, dmaResource->txEdmaChannel);
#endif
            lpspi->flags |= SPI_FLAG_POWER;
            break;
        }
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t LPSPI_EdmaSend(const void *data, uint32_t num, cmsis_lpspi_edma_driver_state_t *lpspi)
{
    int32_t ret;
    status_t status;
    lpspi_transfer_t xfer = {0};

    xfer.rxData   = NULL;
    xfer.txData   = (uint8_t *)data;
    xfer.dataSize = num;

    LPSPI_SetTransferConfigFlags(LPSPI_IsMaster(lpspi->resource->base), lpspi->resource->instance, &xfer);

    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        status = LPSPI_MasterTransferEDMA(lpspi->resource->base, &(lpspi->handle->masterHandle), &xfer);
    }
    else
    {
        status = LPSPI_SlaveTransferEDMA(lpspi->resource->base, &(lpspi->handle->slaveHandle), &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_LPSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t LPSPI_EdmaReceive(void *data, uint32_t num, cmsis_lpspi_edma_driver_state_t *lpspi)
{
    int32_t ret;
    status_t status;
    lpspi_transfer_t xfer = {0};

    xfer.txData   = NULL;
    xfer.rxData   = (uint8_t *)data;
    xfer.dataSize = num;

    LPSPI_SetTransferConfigFlags(LPSPI_IsMaster(lpspi->resource->base), lpspi->resource->instance, &xfer);

    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        status = LPSPI_MasterTransferEDMA(lpspi->resource->base, &(lpspi->handle->masterHandle), &xfer);
    }
    else
    {
        status = LPSPI_SlaveTransferEDMA(lpspi->resource->base, &(lpspi->handle->slaveHandle), &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_LPSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t LPSPI_EdmaTransfer(const void *data_out,
                                  void *data_in,
                                  uint32_t num,
                                  cmsis_lpspi_edma_driver_state_t *lpspi)
{
    int32_t ret;
    status_t status;
    lpspi_transfer_t xfer = {0};

    xfer.txData   = (uint8_t *)data_out;
    xfer.rxData   = (uint8_t *)data_in;
    xfer.dataSize = num;

    LPSPI_SetTransferConfigFlags(LPSPI_IsMaster(lpspi->resource->base), lpspi->resource->instance, &xfer);

    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        status = LPSPI_MasterTransferEDMA(lpspi->resource->base, &(lpspi->handle->masterHandle), &xfer);
    }
    else
    {
        status = LPSPI_SlaveTransferEDMA(lpspi->resource->base, &(lpspi->handle->slaveHandle), &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_LPSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}
static uint32_t LPSPI_EdmaGetCount(cmsis_lpspi_edma_driver_state_t *lpspi)
{
    uint32_t cnt;
    size_t bytes;

    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        bytes = (uint32_t)lpspi->handle->masterHandle.nbytes *
                EDMA_GetRemainingMajorLoopCount(lpspi->dmaResource->rxEdmaBase, lpspi->dmaResource->rxEdmaChannel);
        cnt = lpspi->handle->masterHandle.totalByteCount - bytes;
    }
    else
    {
        bytes = (uint32_t)lpspi->handle->masterHandle.nbytes *
                EDMA_GetRemainingMajorLoopCount(lpspi->dmaResource->rxEdmaBase, lpspi->dmaResource->rxEdmaChannel);
        cnt = lpspi->handle->slaveHandle.totalByteCount - bytes;
    }
    return cnt;
}

static int32_t LPSPI_EdmaControl(uint32_t control, uint32_t arg, cmsis_lpspi_edma_driver_state_t *lpspi)
{
    if (!(lpspi->flags & SPI_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE:
            LPSPI_Enable(lpspi->resource->base, false);
            return ARM_DRIVER_OK;
        case ARM_SPI_MODE_MASTER: /* SPI Master (Output on SOUT, Input on SIN); arg = Bus Speed in bps */
        {
            LPSPI_SetMasterSlaveMode(lpspi->resource->base, kLPSPI_Master);
            LPSPI_MasterTransferCreateHandleEDMA(lpspi->resource->base, &(lpspi->handle->masterHandle),
                                                 KSDK_LPSPI_MasterEdmaCallback, (void *)lpspi->cb_event,
                                                 lpspi->edmaRxRegToRxDataHandle, lpspi->edmaTxDataToTxRegHandle);

            lpspi->baudRate_Bps = arg;
            break;
        }
        case ARM_SPI_MODE_SLAVE: /* SPI Slave  (Output on SOUT, Input on SIN) */
        {
            LPSPI_SetMasterSlaveMode(lpspi->resource->base, kLPSPI_Slave);
            LPSPI_SlaveTransferCreateHandleEDMA(lpspi->resource->base, &(lpspi->handle->slaveHandle),
                                                KSDK_LPSPI_SlaveEdmaCallback, (void *)lpspi->cb_event,
                                                lpspi->edmaRxRegToRxDataHandle, lpspi->edmaTxDataToTxRegHandle);
            break;
        }
        case ARM_SPI_SET_BUS_SPEED: /* Get Bus Speed in bps */
        {
            uint32_t tcrPrescaleValue = 0;
            LPSPI_Enable(lpspi->resource->base, false);

            if (!LPSPI_IsMaster(lpspi->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            if (0 == LPSPI_MasterSetBaudRate(lpspi->resource->base, arg, lpspi->resource->GetFreq(), &tcrPrescaleValue))
            {
                return ARM_DRIVER_ERROR;
            }

            LPSPI_Enable(lpspi->resource->base, true);
            lpspi->baudRate_Bps = arg;
            return ARM_DRIVER_OK;
        }

        case ARM_SPI_GET_BUS_SPEED: /* Set Bus Speed in bps; arg = value */
            if (!LPSPI_IsMaster(lpspi->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }
            return lpspi->baudRate_Bps;
        case ARM_SPI_CONTROL_SS: /* Control Slave Select; arg = 0:inactive, 1:active  */
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_SPI_ABORT_TRANSFER: /* Abort current data transfer */
            if (LPSPI_IsMaster(lpspi->resource->base))
            {
                LPSPI_MasterTransferAbortEDMA(lpspi->resource->base, &(lpspi->handle->masterHandle));
            }
            else
            {
                LPSPI_SlaveTransferAbortEDMA(lpspi->resource->base, &(lpspi->handle->slaveHandle));
            }
            return ARM_DRIVER_OK;

        case ARM_SPI_SET_DEFAULT_TX_VALUE: /* Set default Transmit value; arg = value */
            LPSPI_SetDummyData(lpspi->resource->base, (uint8_t)arg);
            return ARM_DRIVER_OK;

        case ARM_SPI_MODE_MASTER_SIMPLEX: /* SPI Master (Output/Input on SOUT); arg = Bus Speed in bps */
        {
            LPSPI_SetMasterSlaveMode(lpspi->resource->base, kLPSPI_Master);
            LPSPI_MasterTransferCreateHandleEDMA(lpspi->resource->base, &(lpspi->handle->masterHandle),
                                                 KSDK_LPSPI_MasterEdmaCallback, (void *)lpspi->cb_event,
                                                 lpspi->edmaRxRegToRxDataHandle, lpspi->edmaTxDataToTxRegHandle);

            lpspi->baudRate_Bps = arg;
            break;
        }

        case ARM_SPI_MODE_SLAVE_SIMPLEX: /* SPI Slave  (Output/Input on SIN) */
        {
            LPSPI_SetMasterSlaveMode(lpspi->resource->base, kLPSPI_Slave);
            LPSPI_SlaveTransferCreateHandleEDMA(lpspi->resource->base, &(lpspi->handle->slaveHandle),
                                                KSDK_LPSPI_SlaveEdmaCallback, (void *)lpspi->cb_event,
                                                lpspi->edmaRxRegToRxDataHandle, lpspi->edmaTxDataToTxRegHandle);
            break;
        }

        default:
            break;
    }

    return LPSPI_CommonControl(control, arg, lpspi->resource, &lpspi->flags);
}

ARM_SPI_STATUS LPSPI_EdmaGetStatus(cmsis_lpspi_edma_driver_state_t *lpspi)
{
    ARM_SPI_STATUS stat;

    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        stat.busy      = (kLPSPI_Busy == lpspi->handle->masterHandle.state) ? (1U) : (0U);
        stat.data_lost = (kLPSPI_Error == lpspi->handle->masterHandle.state) ? (1U) : (0U);
    }
    else
    {
        stat.busy      = (kLPSPI_Busy == lpspi->handle->slaveHandle.state) ? (1U) : (0U);
        stat.data_lost = (kLPSPI_Error == lpspi->handle->slaveHandle.state) ? (1U) : (0U);
    }
    stat.mode_fault = 0U;
    stat.reserved   = 0U;

    return stat;
}
#endif /* defined(FSL_FEATURE_SOC_EDMA_COUNT) */

#endif

#if ((RTE_SPI0 && !RTE_SPI0_DMA_EN) || (RTE_SPI1 && !RTE_SPI1_DMA_EN) || (RTE_SPI2 && !RTE_SPI2_DMA_EN) || \
     (RTE_SPI3 && !RTE_SPI3_DMA_EN) || (RTE_SPI4 && !RTE_SPI4_DMA_EN) || (RTE_SPI5 && !RTE_SPI5_DMA_EN))

void KSDK_LPSPI_MasterInterruptCallback(LPSPI_Type *base,
                                        lpspi_master_handle_t *handle,
                                        status_t status,
                                        void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }
    else if (kStatus_LPSPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    else
    {
        __NOP();
    }
    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

void KSDK_LPSPI_SlaveInterruptCallback(LPSPI_Type *base, lpspi_slave_handle_t *handle, status_t status, void *userData)
{
    uint32_t event = 0U;

    if (kStatus_Success == status)
    {
        event = ARM_SPI_EVENT_TRANSFER_COMPLETE;
    }
    if (kStatus_LPSPI_Error == status)
    {
        event = ARM_SPI_EVENT_DATA_LOST;
    }
    else
    {
        __NOP();
    }
    /* User data is actually CMSIS driver callback. */
    if ((0U != event) && (userData))
    {
        ((ARM_SPI_SignalEvent_t)userData)(event);
    }
}

static int32_t LPSPI_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event, cmsis_lpspi_interrupt_driver_state_t *lpspi)
{
    if (!(lpspi->flags & SPI_FLAG_INIT))
    {
        lpspi->cb_event = cb_event;
        lpspi->flags    = SPI_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t LPSPI_InterruptUninitialize(cmsis_lpspi_interrupt_driver_state_t *lpspi)
{
    lpspi->flags = SPI_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

static int32_t LPSPI_InterruptPowerControl(ARM_POWER_STATE state, cmsis_lpspi_interrupt_driver_state_t *lpspi)
{
    switch (state)
    {
        case ARM_POWER_OFF:
            if (lpspi->flags & SPI_FLAG_POWER)
            {
                LPSPI_Deinit(lpspi->resource->base);
                lpspi->flags = SPI_FLAG_INIT;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            if (lpspi->flags == SPI_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (lpspi->flags & SPI_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            CLOCK_EnableClock(s_lpspiClock[lpspi->resource->instance]);
            lpspi->flags |= SPI_FLAG_POWER;

            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t LPSPI_InterruptSend(const void *data, uint32_t num, cmsis_lpspi_interrupt_driver_state_t *lpspi)
{
    int32_t ret;
    status_t status;
    lpspi_transfer_t xfer = {0};

    xfer.rxData   = NULL;
    xfer.txData   = (uint8_t *)data;
    xfer.dataSize = num;

    LPSPI_SetTransferConfigFlags(LPSPI_IsMaster(lpspi->resource->base), lpspi->resource->instance, &xfer);

    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        status = LPSPI_MasterTransferNonBlocking(lpspi->resource->base, &(lpspi->handle->masterHandle), &xfer);
    }
    else
    {
        status = LPSPI_SlaveTransferNonBlocking(lpspi->resource->base, &(lpspi->handle->slaveHandle), &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_LPSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t LPSPI_InterruptReceive(void *data, uint32_t num, cmsis_lpspi_interrupt_driver_state_t *lpspi)
{
    int32_t ret;
    status_t status;
    lpspi_transfer_t xfer = {0};

    xfer.txData   = NULL;
    xfer.rxData   = (uint8_t *)data;
    xfer.dataSize = num;

    LPSPI_SetTransferConfigFlags(LPSPI_IsMaster(lpspi->resource->base), lpspi->resource->instance, &xfer);

    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        status = LPSPI_MasterTransferNonBlocking(lpspi->resource->base, &(lpspi->handle->masterHandle), &xfer);
    }
    else
    {
        status = LPSPI_SlaveTransferNonBlocking(lpspi->resource->base, &(lpspi->handle->slaveHandle), &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_LPSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t LPSPI_InterruptTransfer(const void *data_out,
                                       void *data_in,
                                       uint32_t num,
                                       cmsis_lpspi_interrupt_driver_state_t *lpspi)
{
    int32_t ret;
    status_t status;
    lpspi_transfer_t xfer = {0};

    xfer.txData   = (uint8_t *)data_out;
    xfer.rxData   = (uint8_t *)data_in;
    xfer.dataSize = num;

    LPSPI_SetTransferConfigFlags(LPSPI_IsMaster(lpspi->resource->base), lpspi->resource->instance, &xfer);

    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        status = LPSPI_MasterTransferNonBlocking(lpspi->resource->base, &(lpspi->handle->masterHandle), &xfer);
    }
    else
    {
        status = LPSPI_SlaveTransferNonBlocking(lpspi->resource->base, &(lpspi->handle->slaveHandle), &xfer);
    }

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_LPSPI_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}
static uint32_t LPSPI_InterruptGetCount(cmsis_lpspi_interrupt_driver_state_t *lpspi)
{
    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        return lpspi->handle->masterHandle.totalByteCount - lpspi->handle->masterHandle.rxRemainingByteCount;
    }
    else
    {
        return lpspi->handle->slaveHandle.totalByteCount - lpspi->handle->slaveHandle.rxRemainingByteCount;
    }
}

static int32_t LPSPI_InterruptControl(uint32_t control, uint32_t arg, cmsis_lpspi_interrupt_driver_state_t *lpspi)
{
    if (!(lpspi->flags & SPI_FLAG_POWER))
    {
        return ARM_DRIVER_ERROR;
    }

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        case ARM_SPI_MODE_INACTIVE:
            LPSPI_Enable(lpspi->resource->base, false);
            return ARM_DRIVER_OK;
        case ARM_SPI_MODE_MASTER: /* SPI Master (Output on SOUT, Input on SIN); arg = Bus Speed in bps */
        {
            LPSPI_SetMasterSlaveMode(lpspi->resource->base, kLPSPI_Master);
            LPSPI_MasterTransferCreateHandle(lpspi->resource->base, &(lpspi->handle->masterHandle),
                                             KSDK_LPSPI_MasterInterruptCallback, (void *)lpspi->cb_event);
            lpspi->baudRate_Bps = arg;
            break;
        }
        case ARM_SPI_MODE_SLAVE: /* SPI Slave  (Output on SOUT, Input on SIN) */
        {
            LPSPI_SetMasterSlaveMode(lpspi->resource->base, kLPSPI_Slave);
            LPSPI_SlaveTransferCreateHandle(lpspi->resource->base, &(lpspi->handle->slaveHandle),
                                            KSDK_LPSPI_SlaveInterruptCallback, (void *)lpspi->cb_event);
            break;
        }
        case ARM_SPI_SET_BUS_SPEED: /* Set Bus Speed in bps; */
        {
            uint32_t tcrPrescaleValue = 0;
            LPSPI_Enable(lpspi->resource->base, false);

            if (!LPSPI_IsMaster(lpspi->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }
            if (0 == LPSPI_MasterSetBaudRate(lpspi->resource->base, arg, lpspi->resource->GetFreq(), &tcrPrescaleValue))
            {
                return ARM_DRIVER_ERROR;
            }

            LPSPI_Enable(lpspi->resource->base, true);
            lpspi->baudRate_Bps = arg;
            return ARM_DRIVER_OK;
        }
        case ARM_SPI_GET_BUS_SPEED: /* Get Bus Speed in bps */
            if (!LPSPI_IsMaster(lpspi->resource->base))
            {
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }
            return (lpspi->baudRate_Bps);
        case ARM_SPI_CONTROL_SS: /* Control Slave Select; arg = 0:inactive, 1:active  */
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_SPI_ABORT_TRANSFER: /* Abort current data transfer */

            if (LPSPI_IsMaster(lpspi->resource->base))
            {
                LPSPI_MasterTransferAbort(lpspi->resource->base, &(lpspi->handle->masterHandle));
            }
            else
            {
                LPSPI_SlaveTransferAbort(lpspi->resource->base, &(lpspi->handle->slaveHandle));
            }
            return ARM_DRIVER_OK;

        case ARM_SPI_SET_DEFAULT_TX_VALUE: /* Set default Transmit value; arg = value */
            if (LPSPI_IsMaster(lpspi->resource->base))
            {
                LPSPI_SetDummyData(lpspi->resource->base, (uint8_t)arg);
                return ARM_DRIVER_OK;
            }
            else
            {
                /* Mode is not supported by current driver.
                 * In slave mode, if the tx buffer is NULL, the output pin will keep tristated.
                 */
                return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

        case ARM_SPI_MODE_MASTER_SIMPLEX: /* SPI Master (Output/Input on SOUT); arg = Bus Speed in bps */
        {
            LPSPI_SetMasterSlaveMode(lpspi->resource->base, kLPSPI_Master);
            LPSPI_MasterTransferCreateHandle(lpspi->resource->base, &(lpspi->handle->masterHandle),
                                             KSDK_LPSPI_MasterInterruptCallback, (void *)lpspi->cb_event);
            lpspi->baudRate_Bps = arg;
            break;
        }
        case ARM_SPI_MODE_SLAVE_SIMPLEX: /* SPI Slave  (Output/Input on SIN) */
        {
            LPSPI_SetMasterSlaveMode(lpspi->resource->base, kLPSPI_Slave);
            LPSPI_SlaveTransferCreateHandle(lpspi->resource->base, &(lpspi->handle->slaveHandle),
                                            KSDK_LPSPI_SlaveInterruptCallback, (void *)lpspi->cb_event);
            break;
        }

        default:
            break;
    }
    return LPSPI_CommonControl(control, lpspi->baudRate_Bps, lpspi->resource, &lpspi->flags);
}

ARM_SPI_STATUS LPSPI_InterruptGetStatus(cmsis_lpspi_interrupt_driver_state_t *lpspi)
{
    ARM_SPI_STATUS stat;

    if (LPSPI_IsMaster(lpspi->resource->base))
    {
        stat.busy = ((lpspi->handle->masterHandle.rxRemainingByteCount > 0) ||
                     (lpspi->handle->masterHandle.txRemainingByteCount > 0)) ?
                        (1U) :
                        (0U);
        stat.data_lost = (kLPSPI_Error == lpspi->handle->masterHandle.state) ? (1U) : (0U);
    }
    else
    {
        stat.busy = ((lpspi->handle->slaveHandle.rxRemainingByteCount > 0) ||
                     (lpspi->handle->slaveHandle.txRemainingByteCount > 0)) ?
                        (1U) :
                        (0U);
        stat.data_lost = (kLPSPI_Error == lpspi->handle->slaveHandle.state) ? (1U) : (0U);
    }
    stat.mode_fault = 0U;
    stat.reserved   = 0U;

    return stat;
}

#endif

#if defined(LPSPI0) && RTE_SPI0

/* User needs to provide the implementation for LPSPI0_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPSPI0_GetFreq(void);
extern void LPSPI0_InitPins(void);
extern void LPSPI0_DeinitPins(void);

cmsis_lpspi_resource_t LPSPI0_Resource = {LPSPI0, 0, LPSPI0_GetFreq};

#if RTE_SPI0_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpspi_edma_resource_t LPSPI0_EdmaResource = {
    RTE_SPI0_DMA_TX_DMA_BASE,    RTE_SPI0_DMA_TX_CH,          RTE_SPI0_DMA_TX_PERI_SEL,
    RTE_SPI0_DMA_RX_DMA_BASE,    RTE_SPI0_DMA_RX_CH,          RTE_SPI0_DMA_RX_PERI_SEL,

#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_SPI0_DMA_TX_DMAMUX_BASE, RTE_SPI0_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(cmsis_lpspi_edma_handle_t LPSPI0_EdmaHandle);
edma_handle_t LPSPI0_EdmaTxDataToTxRegHandle;
edma_handle_t LPSPI0_EdmaRxRegToRxDataHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi0_edma_driver_state")
cmsis_lpspi_edma_driver_state_t LPSPI0_EdmaDriverState = {
#else
cmsis_lpspi_edma_driver_state_t LPSPI0_EdmaDriverState = {
#endif
    &LPSPI0_Resource,
    &LPSPI0_EdmaResource,
    &LPSPI0_EdmaHandle,
    &LPSPI0_EdmaRxRegToRxDataHandle,
    &LPSPI0_EdmaTxDataToTxRegHandle,
};

static int32_t LPSPI0_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI0_InitPins();
    return LPSPI_EdmaInitialize(cb_event, &LPSPI0_EdmaDriverState);
}

static int32_t LPSPI0_EdmaUninitialize(void)
{
    LPSPI0_DeinitPins();
    return LPSPI_EdmaUninitialize(&LPSPI0_EdmaDriverState);
}

static int32_t LPSPI0_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_EdmaPowerControl(state, &LPSPI0_EdmaDriverState);
}

static int32_t LPSPI0_EdmaSend(const void *data, uint32_t num)
{
    return LPSPI_EdmaSend(data, num, &LPSPI0_EdmaDriverState);
}

static int32_t LPSPI0_EdmaReceive(void *data, uint32_t num)
{
    return LPSPI_EdmaReceive(data, num, &LPSPI0_EdmaDriverState);
}

static int32_t LPSPI0_EdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_EdmaTransfer(data_out, data_in, num, &LPSPI0_EdmaDriverState);
}

static uint32_t LPSPI0_EdmaGetCount(void)
{
    return LPSPI_EdmaGetCount(&LPSPI0_EdmaDriverState);
}

static int32_t LPSPI0_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPSPI_EdmaControl(control, arg, &LPSPI0_EdmaDriverState);
}

static ARM_SPI_STATUS LPSPI0_EdmaGetStatus(void)
{
    return LPSPI_EdmaGetStatus(&LPSPI0_EdmaDriverState);
}

#endif

#else

cmsis_lpspi_handle_t LPSPI0_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi0_interrupt_driver_state")
cmsis_lpspi_interrupt_driver_state_t LPSPI0_InterruptDriverState = {
#else
cmsis_lpspi_interrupt_driver_state_t LPSPI0_InterruptDriverState = {
#endif
    &LPSPI0_Resource,
    &LPSPI0_Handle,
};

static int32_t LPSPI0_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI0_InitPins();
    return LPSPI_InterruptInitialize(cb_event, &LPSPI0_InterruptDriverState);
}

static int32_t LPSPI0_InterruptUninitialize(void)
{
    LPSPI0_DeinitPins();
    return LPSPI_InterruptUninitialize(&LPSPI0_InterruptDriverState);
}

static int32_t LPSPI0_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_InterruptPowerControl(state, &LPSPI0_InterruptDriverState);
}

static int32_t LPSPI0_InterruptSend(const void *data, uint32_t num)
{
    return LPSPI_InterruptSend(data, num, &LPSPI0_InterruptDriverState);
}

static int32_t LPSPI0_InterruptReceive(void *data, uint32_t num)
{
    return LPSPI_InterruptReceive(data, num, &LPSPI0_InterruptDriverState);
}

static int32_t LPSPI0_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_InterruptTransfer(data_out, data_in, num, &LPSPI0_InterruptDriverState);
}

static uint32_t LPSPI0_InterruptGetCount(void)
{
    return LPSPI_InterruptGetCount(&LPSPI0_InterruptDriverState);
}

static int32_t LPSPI0_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPSPI_InterruptControl(control, arg, &LPSPI0_InterruptDriverState);
}

static ARM_SPI_STATUS LPSPI0_InterruptGetStatus(void)
{
    return LPSPI_InterruptGetStatus(&LPSPI0_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI0 = {
    LPSPIx_GetVersion,     LPSPIx_GetCapabilities,
#if RTE_SPI0_DMA_EN
    LPSPI0_EdmaInitialize, LPSPI0_EdmaUninitialize, LPSPI0_EdmaPowerControl, LPSPI0_EdmaSend,     LPSPI0_EdmaReceive,
    LPSPI0_EdmaTransfer,   LPSPI0_EdmaGetCount,     LPSPI0_EdmaControl,      LPSPI0_EdmaGetStatus
#else
    LPSPI0_InterruptInitialize, LPSPI0_InterruptUninitialize, LPSPI0_InterruptPowerControl, LPSPI0_InterruptSend,
    LPSPI0_InterruptReceive,    LPSPI0_InterruptTransfer,     LPSPI0_InterruptGetCount,     LPSPI0_InterruptControl,
    LPSPI0_InterruptGetStatus
#endif
};

#endif /*  LPSPI0  */

#if defined(LPSPI1) && RTE_SPI1

/* User needs to provide the implementation for LPSPI1_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPSPI1_GetFreq(void);
extern void LPSPI1_InitPins(void);
extern void LPSPI1_DeinitPins(void);

cmsis_lpspi_resource_t LPSPI1_Resource = {LPSPI1, 1, LPSPI1_GetFreq};

#if RTE_SPI1_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpspi_edma_resource_t LPSPI1_EdmaResource = {
    RTE_SPI1_DMA_TX_DMA_BASE,    RTE_SPI1_DMA_TX_CH,          RTE_SPI1_DMA_TX_PERI_SEL,
    RTE_SPI1_DMA_RX_DMA_BASE,    RTE_SPI1_DMA_RX_CH,          RTE_SPI1_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_SPI1_DMA_TX_DMAMUX_BASE, RTE_SPI1_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(cmsis_lpspi_edma_handle_t LPSPI1_EdmaHandle);
edma_handle_t LPSPI1_EdmaTxDataToTxRegHandle;
edma_handle_t LPSPI1_EdmaRxRegToRxDataHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi1_edma_driver_state")
cmsis_lpspi_edma_driver_state_t LPSPI1_EdmaDriverState = {
#else
cmsis_lpspi_edma_driver_state_t LPSPI1_EdmaDriverState = {
#endif
    &LPSPI1_Resource,
    &LPSPI1_EdmaResource,
    &LPSPI1_EdmaHandle,
    &LPSPI1_EdmaRxRegToRxDataHandle,
    &LPSPI1_EdmaTxDataToTxRegHandle,
};

static int32_t LPSPI1_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI1_InitPins();
    return LPSPI_EdmaInitialize(cb_event, &LPSPI1_EdmaDriverState);
}

static int32_t LPSPI1_EdmaUninitialize(void)
{
    LPSPI1_DeinitPins();
    return LPSPI_EdmaUninitialize(&LPSPI1_EdmaDriverState);
}

static int32_t LPSPI1_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_EdmaPowerControl(state, &LPSPI1_EdmaDriverState);
}

static int32_t LPSPI1_EdmaSend(const void *data, uint32_t num)
{
    return LPSPI_EdmaSend(data, num, &LPSPI1_EdmaDriverState);
}

static int32_t LPSPI1_EdmaReceive(void *data, uint32_t num)
{
    return LPSPI_EdmaReceive(data, num, &LPSPI1_EdmaDriverState);
}

static int32_t LPSPI1_EdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_EdmaTransfer(data_out, data_in, num, &LPSPI1_EdmaDriverState);
}

static uint32_t LPSPI1_EdmaGetCount(void)
{
    return LPSPI_EdmaGetCount(&LPSPI1_EdmaDriverState);
}

static int32_t LPSPI1_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPSPI_EdmaControl(control, arg, &LPSPI1_EdmaDriverState);
}

static ARM_SPI_STATUS LPSPI1_EdmaGetStatus(void)
{
    return LPSPI_EdmaGetStatus(&LPSPI1_EdmaDriverState);
}

#endif

#else

cmsis_lpspi_handle_t LPSPI1_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi1_interrupt_driver_state")
cmsis_lpspi_interrupt_driver_state_t LPSPI1_InterruptDriverState = {
#else
cmsis_lpspi_interrupt_driver_state_t LPSPI1_InterruptDriverState = {
#endif
    &LPSPI1_Resource,
    &LPSPI1_Handle,
};

static int32_t LPSPI1_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI1_InitPins();
    return LPSPI_InterruptInitialize(cb_event, &LPSPI1_InterruptDriverState);
}

static int32_t LPSPI1_InterruptUninitialize(void)
{
    LPSPI1_DeinitPins();
    return LPSPI_InterruptUninitialize(&LPSPI1_InterruptDriverState);
}

static int32_t LPSPI1_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_InterruptPowerControl(state, &LPSPI1_InterruptDriverState);
}

static int32_t LPSPI1_InterruptSend(const void *data, uint32_t num)
{
    return LPSPI_InterruptSend(data, num, &LPSPI1_InterruptDriverState);
}

static int32_t LPSPI1_InterruptReceive(void *data, uint32_t num)
{
    return LPSPI_InterruptReceive(data, num, &LPSPI1_InterruptDriverState);
}

static int32_t LPSPI1_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_InterruptTransfer(data_out, data_in, num, &LPSPI1_InterruptDriverState);
}

static uint32_t LPSPI1_InterruptGetCount(void)
{
    return LPSPI_InterruptGetCount(&LPSPI1_InterruptDriverState);
}

static int32_t LPSPI1_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPSPI_InterruptControl(control, arg, &LPSPI1_InterruptDriverState);
}

static ARM_SPI_STATUS LPSPI1_InterruptGetStatus(void)
{
    return LPSPI_InterruptGetStatus(&LPSPI1_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI1 = {
    LPSPIx_GetVersion,     LPSPIx_GetCapabilities,
#if RTE_SPI1_DMA_EN
    LPSPI1_EdmaInitialize, LPSPI1_EdmaUninitialize, LPSPI1_EdmaPowerControl, LPSPI1_EdmaSend,     LPSPI1_EdmaReceive,
    LPSPI1_EdmaTransfer,   LPSPI1_EdmaGetCount,     LPSPI1_EdmaControl,      LPSPI1_EdmaGetStatus
#else
    LPSPI1_InterruptInitialize, LPSPI1_InterruptUninitialize, LPSPI1_InterruptPowerControl, LPSPI1_InterruptSend,
    LPSPI1_InterruptReceive,    LPSPI1_InterruptTransfer,     LPSPI1_InterruptGetCount,     LPSPI1_InterruptControl,
    LPSPI1_InterruptGetStatus
#endif
};

#endif /*  LPSPI1  */

#if defined(LPSPI2) && RTE_SPI2

/* User needs to provide the implementation for LPSPI2_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPSPI2_GetFreq(void);
extern void LPSPI2_InitPins(void);
extern void LPSPI2_DeinitPins(void);

cmsis_lpspi_resource_t LPSPI2_Resource = {LPSPI2, 2, LPSPI2_GetFreq};

#if RTE_SPI2_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpspi_edma_resource_t LPSPI2_EdmaResource = {
    RTE_SPI2_DMA_TX_DMA_BASE,    RTE_SPI2_DMA_TX_CH,          RTE_SPI2_DMA_TX_PERI_SEL,
    RTE_SPI2_DMA_RX_DMA_BASE,    RTE_SPI2_DMA_RX_CH,          RTE_SPI2_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_SPI2_DMA_TX_DMAMUX_BASE, RTE_SPI2_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(cmsis_lpspi_edma_handle_t LPSPI2_EdmaHandle);
edma_handle_t LPSPI2_EdmaTxDataToTxRegHandle;
edma_handle_t LPSPI2_EdmaRxRegToRxDataHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi2_edma_driver_state")
cmsis_lpspi_edma_driver_state_t LPSPI2_EdmaDriverState = {
#else
cmsis_lpspi_edma_driver_state_t LPSPI2_EdmaDriverState = {
#endif
    &LPSPI2_Resource,
    &LPSPI2_EdmaResource,
    &LPSPI2_EdmaHandle,
    &LPSPI2_EdmaRxRegToRxDataHandle,
    &LPSPI2_EdmaTxDataToTxRegHandle,
};

static int32_t LPSPI2_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI2_InitPins();
    return LPSPI_EdmaInitialize(cb_event, &LPSPI2_EdmaDriverState);
}

static int32_t LPSPI2_EdmaUninitialize(void)
{
    LPSPI2_DeinitPins();
    return LPSPI_EdmaUninitialize(&LPSPI2_EdmaDriverState);
}

static int32_t LPSPI2_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_EdmaPowerControl(state, &LPSPI2_EdmaDriverState);
}

static int32_t LPSPI2_EdmaSend(const void *data, uint32_t num)
{
    return LPSPI_EdmaSend(data, num, &LPSPI2_EdmaDriverState);
}

static int32_t LPSPI2_EdmaReceive(void *data, uint32_t num)
{
    return LPSPI_EdmaReceive(data, num, &LPSPI2_EdmaDriverState);
}

static int32_t LPSPI2_EdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_EdmaTransfer(data_out, data_in, num, &LPSPI2_EdmaDriverState);
}

static uint32_t LPSPI2_EdmaGetCount(void)
{
    return LPSPI_EdmaGetCount(&LPSPI2_EdmaDriverState);
}

static int32_t LPSPI2_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPSPI_EdmaControl(control, arg, &LPSPI2_EdmaDriverState);
}

static ARM_SPI_STATUS LPSPI2_EdmaGetStatus(void)
{
    return LPSPI_EdmaGetStatus(&LPSPI2_EdmaDriverState);
}

#endif

#else

cmsis_lpspi_handle_t LPSPI2_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi2_interrupt_driver_state")
cmsis_lpspi_interrupt_driver_state_t LPSPI2_InterruptDriverState = {
#else
cmsis_lpspi_interrupt_driver_state_t LPSPI2_InterruptDriverState = {
#endif
    &LPSPI2_Resource,
    &LPSPI2_Handle,
};

static int32_t LPSPI2_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI2_InitPins();
    return LPSPI_InterruptInitialize(cb_event, &LPSPI2_InterruptDriverState);
}

static int32_t LPSPI2_InterruptUninitialize(void)
{
    LPSPI2_DeinitPins();
    return LPSPI_InterruptUninitialize(&LPSPI2_InterruptDriverState);
}

static int32_t LPSPI2_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_InterruptPowerControl(state, &LPSPI2_InterruptDriverState);
}

static int32_t LPSPI2_InterruptSend(const void *data, uint32_t num)
{
    return LPSPI_InterruptSend(data, num, &LPSPI2_InterruptDriverState);
}

static int32_t LPSPI2_InterruptReceive(void *data, uint32_t num)
{
    return LPSPI_InterruptReceive(data, num, &LPSPI2_InterruptDriverState);
}

static int32_t LPSPI2_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_InterruptTransfer(data_out, data_in, num, &LPSPI2_InterruptDriverState);
}

static uint32_t LPSPI2_InterruptGetCount(void)
{
    return LPSPI_InterruptGetCount(&LPSPI2_InterruptDriverState);
}

static int32_t LPSPI2_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPSPI_InterruptControl(control, arg, &LPSPI2_InterruptDriverState);
}

static ARM_SPI_STATUS LPSPI2_InterruptGetStatus(void)
{
    return LPSPI_InterruptGetStatus(&LPSPI2_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI2 = {
    LPSPIx_GetVersion,     LPSPIx_GetCapabilities,
#if RTE_SPI2_DMA_EN
    LPSPI2_EdmaInitialize, LPSPI2_EdmaUninitialize, LPSPI2_EdmaPowerControl, LPSPI2_EdmaSend,     LPSPI2_EdmaReceive,
    LPSPI2_EdmaTransfer,   LPSPI2_EdmaGetCount,     LPSPI2_EdmaControl,      LPSPI2_EdmaGetStatus
#else
    LPSPI2_InterruptInitialize, LPSPI2_InterruptUninitialize, LPSPI2_InterruptPowerControl, LPSPI2_InterruptSend,
    LPSPI2_InterruptReceive,    LPSPI2_InterruptTransfer,     LPSPI2_InterruptGetCount,     LPSPI2_InterruptControl,
    LPSPI2_InterruptGetStatus
#endif
};

#endif /*  LPSPI2  */

#if defined(LPSPI3) && RTE_SPI3

/* User needs to provide the implementation for LPSPI3_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPSPI3_GetFreq(void);
extern void LPSPI3_InitPins(void);
extern void LPSPI3_DeinitPins(void);

cmsis_lpspi_resource_t LPSPI3_Resource = {LPSPI3, 3, LPSPI3_GetFreq};

#if RTE_SPI3_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpspi_edma_resource_t LPSPI3_EdmaResource = {
    RTE_SPI3_DMA_TX_DMA_BASE,    RTE_SPI3_DMA_TX_CH,          RTE_SPI3_DMA_TX_PERI_SEL,
    RTE_SPI3_DMA_RX_DMA_BASE,    RTE_SPI3_DMA_RX_CH,          RTE_SPI3_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_SPI3_DMA_TX_DMAMUX_BASE, RTE_SPI3_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(cmsis_lpspi_edma_handle_t LPSPI3_EdmaHandle);
edma_handle_t LPSPI3_EdmaTxDataToTxRegHandle;
edma_handle_t LPSPI3_EdmaRxRegToRxDataHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi3_edma_driver_state")
cmsis_lpspi_edma_driver_state_t LPSPI3_EdmaDriverState = {
#else
cmsis_lpspi_edma_driver_state_t LPSPI3_EdmaDriverState = {
#endif
    &LPSPI3_Resource,
    &LPSPI3_EdmaResource,
    &LPSPI3_EdmaHandle,
    &LPSPI3_EdmaRxRegToRxDataHandle,
    &LPSPI3_EdmaTxDataToTxRegHandle,
};

static int32_t LPSPI3_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI3_InitPins();
    return LPSPI_EdmaInitialize(cb_event, &LPSPI3_EdmaDriverState);
}

static int32_t LPSPI3_EdmaUninitialize(void)
{
    LPSPI3_DeinitPins();
    return LPSPI_EdmaUninitialize(&LPSPI3_EdmaDriverState);
}

static int32_t LPSPI3_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_EdmaPowerControl(state, &LPSPI3_EdmaDriverState);
}

static int32_t LPSPI3_EdmaSend(const void *data, uint32_t num)
{
    return LPSPI_EdmaSend(data, num, &LPSPI3_EdmaDriverState);
}

static int32_t LPSPI3_EdmaReceive(void *data, uint32_t num)
{
    return LPSPI_EdmaReceive(data, num, &LPSPI3_EdmaDriverState);
}

static int32_t LPSPI3_EdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_EdmaTransfer(data_out, data_in, num, &LPSPI3_EdmaDriverState);
}

static uint32_t LPSPI3_EdmaGetCount(void)
{
    return LPSPI_EdmaGetCount(&LPSPI3_EdmaDriverState);
}

static int32_t LPSPI3_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPSPI_EdmaControl(control, arg, &LPSPI3_EdmaDriverState);
}

static ARM_SPI_STATUS LPSPI3_EdmaGetStatus(void)
{
    return LPSPI_EdmaGetStatus(&LPSPI3_EdmaDriverState);
}

#endif

#else

cmsis_lpspi_handle_t LPSPI3_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi3_interrupt_driver_state")
cmsis_lpspi_interrupt_driver_state_t LPSPI3_InterruptDriverState = {
#else
cmsis_lpspi_interrupt_driver_state_t LPSPI3_InterruptDriverState = {
#endif
    &LPSPI3_Resource,
    &LPSPI3_Handle,
};

static int32_t LPSPI3_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI3_InitPins();
    return LPSPI_InterruptInitialize(cb_event, &LPSPI3_InterruptDriverState);
}

static int32_t LPSPI3_InterruptUninitialize(void)
{
    LPSPI3_DeinitPins();
    return LPSPI_InterruptUninitialize(&LPSPI3_InterruptDriverState);
}

static int32_t LPSPI3_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_InterruptPowerControl(state, &LPSPI3_InterruptDriverState);
}

static int32_t LPSPI3_InterruptSend(const void *data, uint32_t num)
{
    return LPSPI_InterruptSend(data, num, &LPSPI3_InterruptDriverState);
}

static int32_t LPSPI3_InterruptReceive(void *data, uint32_t num)
{
    return LPSPI_InterruptReceive(data, num, &LPSPI3_InterruptDriverState);
}

static int32_t LPSPI3_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_InterruptTransfer(data_out, data_in, num, &LPSPI3_InterruptDriverState);
}

static uint32_t LPSPI3_InterruptGetCount(void)
{
    return LPSPI_InterruptGetCount(&LPSPI3_InterruptDriverState);
}

static int32_t LPSPI3_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPSPI_InterruptControl(control, arg, &LPSPI3_InterruptDriverState);
}

static ARM_SPI_STATUS LPSPI3_InterruptGetStatus(void)
{
    return LPSPI_InterruptGetStatus(&LPSPI3_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI3 = {
    LPSPIx_GetVersion,     LPSPIx_GetCapabilities,
#if RTE_SPI3_DMA_EN
    LPSPI3_EdmaInitialize, LPSPI3_EdmaUninitialize, LPSPI3_EdmaPowerControl, LPSPI3_EdmaSend,     LPSPI3_EdmaReceive,
    LPSPI3_EdmaTransfer,   LPSPI3_EdmaGetCount,     LPSPI3_EdmaControl,      LPSPI3_EdmaGetStatus
#else
    LPSPI3_InterruptInitialize, LPSPI3_InterruptUninitialize, LPSPI3_InterruptPowerControl, LPSPI3_InterruptSend,
    LPSPI3_InterruptReceive,    LPSPI3_InterruptTransfer,     LPSPI3_InterruptGetCount,     LPSPI3_InterruptControl,
    LPSPI3_InterruptGetStatus
#endif
};

#endif /*  LPSPI3  */

#if defined(LPSPI4) && RTE_SPI4

/* User needs to provide the implementation for LPSPI4_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPSPI4_GetFreq(void);
extern void LPSPI4_InitPins(void);
extern void LPSPI4_DeinitPins(void);

cmsis_lpspi_resource_t LPSPI4_Resource = {LPSPI4, 4, LPSPI4_GetFreq};

#if RTE_SPI4_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpspi_edma_resource_t LPSPI4_EdmaResource = {
    RTE_SPI4_DMA_TX_DMA_BASE,    RTE_SPI4_DMA_TX_CH,          RTE_SPI4_DMA_TX_PERI_SEL,
    RTE_SPI4_DMA_RX_DMA_BASE,    RTE_SPI4_DMA_RX_CH,          RTE_SPI4_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_SPI4_DMA_TX_DMAMUX_BASE, RTE_SPI4_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(cmsis_lpspi_edma_handle_t LPSPI4_EdmaHandle);
edma_handle_t LPSPI4_EdmaTxDataToTxRegHandle;
edma_handle_t LPSPI4_EdmaRxRegToRxDataHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi4_edma_driver_state")
cmsis_lpspi_edma_driver_state_t LPSPI4_EdmaDriverState = {
#else
cmsis_lpspi_edma_driver_state_t LPSPI4_EdmaDriverState = {
#endif
    &LPSPI4_Resource,
    &LPSPI4_EdmaResource,
    &LPSPI4_EdmaHandle,
    &LPSPI4_EdmaRxRegToRxDataHandle,
    &LPSPI4_EdmaTxDataToTxRegHandle,
};

static int32_t LPSPI4_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI4_InitPins();
    return LPSPI_EdmaInitialize(cb_event, &LPSPI4_EdmaDriverState);
}

static int32_t LPSPI4_EdmaUninitialize(void)
{
    LPSPI4_DeinitPins();
    return LPSPI_EdmaUninitialize(&LPSPI4_EdmaDriverState);
}

static int32_t LPSPI4_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_EdmaPowerControl(state, &LPSPI4_EdmaDriverState);
}

static int32_t LPSPI4_EdmaSend(const void *data, uint32_t num)
{
    return LPSPI_EdmaSend(data, num, &LPSPI4_EdmaDriverState);
}

static int32_t LPSPI4_EdmaReceive(void *data, uint32_t num)
{
    return LPSPI_EdmaReceive(data, num, &LPSPI4_EdmaDriverState);
}

static int32_t LPSPI4_EdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_EdmaTransfer(data_out, data_in, num, &LPSPI4_EdmaDriverState);
}

static uint32_t LPSPI4_EdmaGetCount(void)
{
    return LPSPI_EdmaGetCount(&LPSPI4_EdmaDriverState);
}

static int32_t LPSPI4_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPSPI_EdmaControl(control, arg, &LPSPI4_EdmaDriverState);
}

static ARM_SPI_STATUS LPSPI4_EdmaGetStatus(void)
{
    return LPSPI_EdmaGetStatus(&LPSPI4_EdmaDriverState);
}

#endif

#else

cmsis_lpspi_handle_t LPSPI4_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi4_interrupt_driver_state")
cmsis_lpspi_interrupt_driver_state_t LPSPI4_InterruptDriverState = {
#else
cmsis_lpspi_interrupt_driver_state_t LPSPI4_InterruptDriverState = {
#endif
    &LPSPI4_Resource,
    &LPSPI4_Handle,
};

static int32_t LPSPI4_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI4_InitPins();
    return LPSPI_InterruptInitialize(cb_event, &LPSPI4_InterruptDriverState);
}

static int32_t LPSPI4_InterruptUninitialize(void)
{
    LPSPI4_DeinitPins();
    return LPSPI_InterruptUninitialize(&LPSPI4_InterruptDriverState);
}

static int32_t LPSPI4_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_InterruptPowerControl(state, &LPSPI4_InterruptDriverState);
}

static int32_t LPSPI4_InterruptSend(const void *data, uint32_t num)
{
    return LPSPI_InterruptSend(data, num, &LPSPI4_InterruptDriverState);
}

static int32_t LPSPI4_InterruptReceive(void *data, uint32_t num)
{
    return LPSPI_InterruptReceive(data, num, &LPSPI4_InterruptDriverState);
}

static int32_t LPSPI4_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_InterruptTransfer(data_out, data_in, num, &LPSPI4_InterruptDriverState);
}

static uint32_t LPSPI4_InterruptGetCount(void)
{
    return LPSPI_InterruptGetCount(&LPSPI4_InterruptDriverState);
}

static int32_t LPSPI4_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPSPI_InterruptControl(control, arg, &LPSPI4_InterruptDriverState);
}

static ARM_SPI_STATUS LPSPI4_InterruptGetStatus(void)
{
    return LPSPI_InterruptGetStatus(&LPSPI4_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI4 = {
    LPSPIx_GetVersion,     LPSPIx_GetCapabilities,
#if RTE_SPI4_DMA_EN
    LPSPI4_EdmaInitialize, LPSPI4_EdmaUninitialize, LPSPI4_EdmaPowerControl, LPSPI4_EdmaSend,     LPSPI4_EdmaReceive,
    LPSPI4_EdmaTransfer,   LPSPI4_EdmaGetCount,     LPSPI4_EdmaControl,      LPSPI4_EdmaGetStatus
#else
    LPSPI4_InterruptInitialize, LPSPI4_InterruptUninitialize, LPSPI4_InterruptPowerControl, LPSPI4_InterruptSend,
    LPSPI4_InterruptReceive,    LPSPI4_InterruptTransfer,     LPSPI4_InterruptGetCount,     LPSPI4_InterruptControl,
    LPSPI4_InterruptGetStatus
#endif
};

#endif /*  LPSPI4  */

#if defined(LPSPI5) && RTE_SPI5

/* User needs to provide the implementation for LPSPI5_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPSPI5_GetFreq(void);
extern void LPSPI5_InitPins(void);
extern void LPSPI5_DeinitPins(void);

cmsis_lpspi_resource_t LPSPI5_Resource = {LPSPI5, 5, LPSPI5_GetFreq};

#if RTE_SPI5_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpspi_edma_resource_t LPSPI5_EdmaResource = {
    RTE_SPI5_DMA_TX_DMA_BASE,    RTE_SPI5_DMA_TX_CH,          RTE_SPI5_DMA_TX_PERI_SEL,
    RTE_SPI5_DMA_RX_DMA_BASE,    RTE_SPI5_DMA_RX_CH,          RTE_SPI5_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_SPI5_DMA_TX_DMAMUX_BASE, RTE_SPI5_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(cmsis_lpspi_edma_handle_t LPSPI5_EdmaHandle);
edma_handle_t LPSPI5_EdmaTxDataToTxRegHandle;
edma_handle_t LPSPI5_EdmaRxRegToRxDataHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi5_edma_driver_state")
cmsis_lpspi_edma_driver_state_t LPSPI5_EdmaDriverState = {
#else
cmsis_lpspi_edma_driver_state_t LPSPI5_EdmaDriverState = {
#endif
    &LPSPI5_Resource,
    &LPSPI5_EdmaResource,
    &LPSPI5_EdmaHandle,
    &LPSPI5_EdmaRxRegToRxDataHandle,
    &LPSPI5_EdmaTxDataToTxRegHandle,
};

static int32_t LPSPI5_EdmaInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI5_InitPins();
    return LPSPI_EdmaInitialize(cb_event, &LPSPI5_EdmaDriverState);
}

static int32_t LPSPI5_EdmaUninitialize(void)
{
    LPSPI5_DeinitPins();
    return LPSPI_EdmaUninitialize(&LPSPI5_EdmaDriverState);
}

static int32_t LPSPI5_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_EdmaPowerControl(state, &LPSPI5_EdmaDriverState);
}

static int32_t LPSPI5_EdmaSend(const void *data, uint32_t num)
{
    return LPSPI_EdmaSend(data, num, &LPSPI5_EdmaDriverState);
}

static int32_t LPSPI5_EdmaReceive(void *data, uint32_t num)
{
    return LPSPI_EdmaReceive(data, num, &LPSPI5_EdmaDriverState);
}

static int32_t LPSPI5_EdmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_EdmaTransfer(data_out, data_in, num, &LPSPI5_EdmaDriverState);
}

static uint32_t LPSPI5_EdmaGetCount(void)
{
    return LPSPI_EdmaGetCount(&LPSPI5_EdmaDriverState);
}

static int32_t LPSPI5_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPSPI_EdmaControl(control, arg, &LPSPI5_EdmaDriverState);
}

static ARM_SPI_STATUS LPSPI5_EdmaGetStatus(void)
{
    return LPSPI_EdmaGetStatus(&LPSPI5_EdmaDriverState);
}

#endif

#else

cmsis_lpspi_handle_t LPSPI5_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpspi5_interrupt_driver_state")
cmsis_lpspi_interrupt_driver_state_t LPSPI5_InterruptDriverState = {
#else
cmsis_lpspi_interrupt_driver_state_t LPSPI5_InterruptDriverState = {
#endif
    &LPSPI5_Resource,
    &LPSPI5_Handle,
};

static int32_t LPSPI5_InterruptInitialize(ARM_SPI_SignalEvent_t cb_event)
{
    LPSPI5_InitPins();
    return LPSPI_InterruptInitialize(cb_event, &LPSPI5_InterruptDriverState);
}

static int32_t LPSPI5_InterruptUninitialize(void)
{
    LPSPI5_DeinitPins();
    return LPSPI_InterruptUninitialize(&LPSPI5_InterruptDriverState);
}

static int32_t LPSPI5_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPSPI_InterruptPowerControl(state, &LPSPI5_InterruptDriverState);
}

static int32_t LPSPI5_InterruptSend(const void *data, uint32_t num)
{
    return LPSPI_InterruptSend(data, num, &LPSPI5_InterruptDriverState);
}

static int32_t LPSPI5_InterruptReceive(void *data, uint32_t num)
{
    return LPSPI_InterruptReceive(data, num, &LPSPI5_InterruptDriverState);
}

static int32_t LPSPI5_InterruptTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return LPSPI_InterruptTransfer(data_out, data_in, num, &LPSPI5_InterruptDriverState);
}

static uint32_t LPSPI5_InterruptGetCount(void)
{
    return LPSPI_InterruptGetCount(&LPSPI5_InterruptDriverState);
}

static int32_t LPSPI5_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPSPI_InterruptControl(control, arg, &LPSPI5_InterruptDriverState);
}

static ARM_SPI_STATUS LPSPI5_InterruptGetStatus(void)
{
    return LPSPI_InterruptGetStatus(&LPSPI5_InterruptDriverState);
}

#endif

ARM_DRIVER_SPI Driver_SPI5 = {
    LPSPIx_GetVersion,     LPSPIx_GetCapabilities,
#if RTE_SPI5_DMA_EN
    LPSPI5_EdmaInitialize, LPSPI5_EdmaUninitialize, LPSPI5_EdmaPowerControl, LPSPI5_EdmaSend,     LPSPI5_EdmaReceive,
    LPSPI5_EdmaTransfer,   LPSPI5_EdmaGetCount,     LPSPI5_EdmaControl,      LPSPI5_EdmaGetStatus
#else
    LPSPI5_InterruptInitialize, LPSPI5_InterruptUninitialize, LPSPI5_InterruptPowerControl, LPSPI5_InterruptSend,
    LPSPI5_InterruptReceive,    LPSPI5_InterruptTransfer,     LPSPI5_InterruptGetCount,     LPSPI5_InterruptControl,
    LPSPI5_InterruptGetStatus
#endif
};

#endif /*  LPSPI5  */
