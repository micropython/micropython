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

#include "fsl_lpi2c_cmsis.h"

#if ((RTE_I2C0 && defined(LPI2C0)) || (RTE_I2C1 && defined(LPI2C1)) || (RTE_I2C2 && defined(LPI2C2)) || \
     (RTE_I2C3 && defined(LPI2C3)) || (RTE_I2C4 && defined(LPI2C4)) || (RTE_I2C5 && defined(LPI2C5)) || \
     (RTE_I2C6 && defined(LPI2C6)))

#define ARM_LPI2C_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpi2c_cmsis"
#endif

/*! @brief Common sets of flags used by the driver. */
enum _lpi2c_flag_constants
{
    /*! IRQ sources enabled by the non-blocking transactional API. */
    kMasterIrqFlags = kLPI2C_MasterArbitrationLostFlag | kLPI2C_MasterTxReadyFlag | kLPI2C_MasterRxReadyFlag |
                      kLPI2C_MasterStopDetectFlag | kLPI2C_MasterNackDetectFlag | kLPI2C_MasterPinLowTimeoutFlag |
                      kLPI2C_MasterFifoErrFlag,

    /*! IRQ sources enabled by the non-blocking transactional API. */
    kSlaveIrqFlags = kLPI2C_SlaveTxReadyFlag | kLPI2C_SlaveRxReadyFlag | kLPI2C_SlaveStopDetectFlag |
                     kLPI2C_SlaveRepeatedStartDetectFlag | kLPI2C_SlaveFifoErrFlag | kLPI2C_SlaveBitErrFlag |
                     kLPI2C_SlaveTransmitAckFlag | kLPI2C_SlaveAddressValidFlag,

};

typedef const struct _cmsis_lpi2c_resource
{
    LPI2C_Type *base;          /*!< LPI2C peripheral base address.      */
    uint32_t (*GetFreq)(void); /*!< Function to get the clock frequency. */

} cmsis_lpi2c_resource_t;

typedef union _cmsis_i2c_handle
{
    lpi2c_master_handle_t master_handle;
    lpi2c_slave_handle_t slave_handle;
} cmsis_i2c_handle_t;

typedef struct _cmsis_lpi2c_interrupt_driver_state
{
    cmsis_lpi2c_resource_t *resource; /*!< Basic LPI2C resource. */
    cmsis_i2c_handle_t *handle;
    uint8_t *slave_data;            /*!< slave Transfer buffer */
    size_t slave_dataSize;          /*!< slave Transfer size */
    ARM_I2C_SignalEvent_t cb_event; /*!< call back function */
    uint8_t flags;                  /*!< Control and state flags. */
} cmsis_lpi2c_interrupt_driver_state_t;

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
typedef const struct _cmsis_lpi2c_edma_resource
{
    DMA_Type *txEdmaBase;   /*!< EDMA peripheral base address for Tx.  */
    uint32_t txEdmaChannel; /*!< EDMA channel for Tx                   */
    uint16_t txDmaRequest;  /*!< Tx EDMA request source.               */

    DMA_Type *rxEdmaBase;   /*!< EDMA peripheral base address for Rx.  */
    uint32_t rxEdmaChannel; /*!< EDMA channel for Rx                   */
    uint16_t rxDmaRequest;  /*!< Rx EDMA request source.               */
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    DMAMUX_Type *txDmamuxBase; /*!< DMAMUX peripheral base address for Tx */
    DMAMUX_Type *rxDmamuxBase; /*!< DMAMUX peripheral base address for Rx */
#endif
} cmsis_lpi2c_edma_resource_t;

typedef struct _cmsis_lpi2c_edma_driver_state
{
    cmsis_lpi2c_resource_t *resource;               /*!< lpi2c basic resource.       */
    cmsis_lpi2c_edma_resource_t *edmaResource;      /*!< lpi2c EDMA resource.        */
    lpi2c_master_edma_handle_t *master_edma_handle; /*!< lpi2c EDMA transfer handle. */
    edma_handle_t *edmaTxHandle;                    /*!< EDMA lpi2c Tx handle.       */
    edma_handle_t *edmaRxHandle;                    /*!< EDMA lpi2c Rx handle.       */
    uint8_t flags;                                  /*!< Control and state flags. */
} cmsis_lpi2c_edma_driver_state_t;
#endif /* FSL_FEATURE_SOC_EDMA_COUNT */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static const ARM_DRIVER_VERSION s_lpi2cDriverVersion        = {ARM_I2C_API_VERSION, ARM_LPI2C_DRV_VERSION};
static const ARM_I2C_CAPABILITIES s_lpi2cDriverCapabilities = {
    0, /* Do not support 10-bit addressing.*/
};

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Returns version information */
static ARM_DRIVER_VERSION LPI2Cx_GetVersion(void)
{
    return s_lpi2cDriverVersion;
}

/* Returns information about capabilities of this driver implementation */
static ARM_I2C_CAPABILITIES LPI2Cx_GetCapabilities(void)
{
    return s_lpi2cDriverCapabilities;
}

#endif

#if (RTE_I2C0_DMA_EN || RTE_I2C1_DMA_EN || RTE_I2C2_DMA_EN || RTE_I2C3_DMA_EN || RTE_I2C4_DMA_EN || RTE_I2C5_DMA_EN || \
     RTE_I2C6_DMA_EN)

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
void KSDK_LPI2C_MASTER_EdmaCallback(LPI2C_Type *base,
                                    lpi2c_master_edma_handle_t *handle,
                                    status_t status,
                                    void *userData)
{
    uint32_t event = 0;

    /* Signal transfer success when received success status. */
    if (status == kStatus_Success)
    {
        event = ARM_I2C_EVENT_TRANSFER_DONE;
    }

    if (userData)
    {
        ((ARM_I2C_SignalEvent_t)userData)(event);
    }
}

static int32_t LPI2C_Master_EdmaInitialize(ARM_I2C_SignalEvent_t cb_event, cmsis_lpi2c_edma_driver_state_t *lpi2c)
{
    if (!(lpi2c->flags & I2C_FLAG_INIT))
    {
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
        /* TxDMAMUX init */
        DMAMUX_SetSource(lpi2c->edmaResource->txDmamuxBase, lpi2c->edmaResource->txEdmaChannel,
                         (uint8_t)lpi2c->edmaResource->txDmaRequest);
        DMAMUX_EnableChannel(lpi2c->edmaResource->txDmamuxBase, lpi2c->edmaResource->txEdmaChannel);
        /* RxDMAMUX init */
        DMAMUX_SetSource(lpi2c->edmaResource->rxDmamuxBase, lpi2c->edmaResource->rxEdmaChannel,
                         (uint8_t)lpi2c->edmaResource->rxDmaRequest);
        DMAMUX_EnableChannel(lpi2c->edmaResource->rxDmamuxBase, lpi2c->edmaResource->rxEdmaChannel);
#endif
        /* Create edmaTxHandle */
        EDMA_CreateHandle(lpi2c->edmaTxHandle, lpi2c->edmaResource->txEdmaBase, lpi2c->edmaResource->txEdmaChannel);

        /* Create edmaRxHandle */
        EDMA_CreateHandle(lpi2c->edmaRxHandle, lpi2c->edmaResource->rxEdmaBase, lpi2c->edmaResource->rxEdmaChannel);

        /* Create master_edma_handle */
        LPI2C_MasterCreateEDMAHandle(lpi2c->resource->base, lpi2c->master_edma_handle, lpi2c->edmaRxHandle,
                                     lpi2c->edmaTxHandle, KSDK_LPI2C_MASTER_EdmaCallback, (void *)cb_event);

        lpi2c->flags = I2C_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

int32_t LPI2C_Master_EdmaUninitialize(cmsis_lpi2c_edma_driver_state_t *lpi2c)
{
    lpi2c->flags = I2C_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

int32_t LPI2C_Master_EdmaTransmit(
    uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending, cmsis_lpi2c_edma_driver_state_t *lpi2c)
{
    int32_t status;
    int32_t ret;
    lpi2c_master_transfer_t masterXfer;

    /* Setup the master transfer */
    masterXfer.slaveAddress   = addr;
    masterXfer.direction      = kLPI2C_Write;
    masterXfer.subaddress     = 0;
    masterXfer.subaddressSize = 0;
    masterXfer.data           = (uint8_t *)data;
    masterXfer.dataSize       = num;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    if (xfer_pending)
    {
        /* Do not transfer stop */
        masterXfer.flags |= kLPI2C_TransferNoStopFlag;
    }

    /* Send master non-blocking data to slave */
    status = LPI2C_MasterTransferEDMA(lpi2c->resource->base, lpi2c->master_edma_handle, &masterXfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;

        case kStatus_LPI2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;

        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t LPI2C_Master_EdmaReceive(
    uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending, cmsis_lpi2c_edma_driver_state_t *lpi2c)
{
    int32_t status;
    int32_t ret;
    lpi2c_master_transfer_t masterXfer;

    /* Setup the master transfer */
    masterXfer.slaveAddress   = addr;
    masterXfer.direction      = kLPI2C_Read;
    masterXfer.subaddress     = 0;
    masterXfer.subaddressSize = 0;
    masterXfer.data           = data;
    masterXfer.dataSize       = num;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    if (xfer_pending)
    {
        masterXfer.flags |= kLPI2C_TransferNoStopFlag;
    }

    /* Receive non-blocking data from slave */
    status = LPI2C_MasterTransferEDMA(lpi2c->resource->base, lpi2c->master_edma_handle, &masterXfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_LPI2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t LPI2C_Master_EdmaGetDataCount(cmsis_lpi2c_edma_driver_state_t *lpi2c)
{
    size_t cnt;

    LPI2C_MasterTransferGetCountEDMA(lpi2c->resource->base, lpi2c->master_edma_handle, &cnt);

    return cnt;
}

int32_t LPI2C_Master_EdmaControl(uint32_t control, uint32_t arg, cmsis_lpi2c_edma_driver_state_t *lpi2c)
{
    uint32_t baudRate_Bps;

    switch (control)
    {
        /* Set Own Slave Address */
        case ARM_I2C_OWN_ADDRESS:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        /* Set Bus Speed; arg = bus speed */
        case ARM_I2C_BUS_SPEED:
            switch (arg)
            {
                case ARM_I2C_BUS_SPEED_STANDARD:
                    baudRate_Bps = 100000;
                    break;

                case ARM_I2C_BUS_SPEED_FAST:
                    baudRate_Bps = 400000;
                    break;

                case ARM_I2C_BUS_SPEED_FAST_PLUS:
                    baudRate_Bps = 1000000;
                    break;

                default:
                    return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            LPI2C_MasterSetBaudRate(lpi2c->resource->base, lpi2c->resource->GetFreq(), baudRate_Bps);

            return ARM_DRIVER_OK;

        /* Not supported. */
        case ARM_I2C_BUS_CLEAR:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        /* Only support aborting data transfer when master transmit in master mode */
        case ARM_I2C_ABORT_TRANSFER:

            if ((lpi2c->resource->base->MDER & 0x3) == 0x3)
            {
                if (lpi2c->master_edma_handle->transfer.direction == kLPI2C_Write)
                {
                    LPI2C_MasterTransferAbortEDMA(lpi2c->resource->base, lpi2c->master_edma_handle);

                    lpi2c->master_edma_handle->transfer.data     = NULL;
                    lpi2c->master_edma_handle->transfer.dataSize = 0;
                }
            }

            return ARM_DRIVER_OK;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

int32_t LPI2C_Master_EdmaPowerControl(ARM_POWER_STATE state, cmsis_lpi2c_edma_driver_state_t *lpi2c)
{
    lpi2c_master_config_t masterConfig;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (lpi2c->flags & I2C_FLAG_POWER)
            {
                /* Terminates any pending data transfers */
                LPI2C_Master_EdmaControl(ARM_I2C_ABORT_TRANSFER, 0, lpi2c);

                /* Disables peripheral */
                LPI2C_MasterDeinit(lpi2c->resource->base);
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
                /* Disable DMAMUX channel */
                DMAMUX_DisableChannel(lpi2c->edmaResource->txDmamuxBase, lpi2c->edmaResource->txEdmaChannel);
                DMAMUX_DisableChannel(lpi2c->edmaResource->rxDmamuxBase, lpi2c->edmaResource->rxEdmaChannel);
#endif
                lpi2c->flags = I2C_FLAG_INIT;
            }
            return ARM_DRIVER_OK;

        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_POWER_FULL:
            if (lpi2c->flags == I2C_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (lpi2c->flags & I2C_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            /*
             * masterConfig.debugEnable = false;
             * masterConfig.ignoreAck = false;
             * masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
             * masterConfig.baudRate_Hz = 100000U;
             * masterConfig.busIdleTimeout_ns = 0;
             * masterConfig.pinLowTimeout_ns = 0;
             * masterConfig.sdaGlitchFilterWidth_ns = 0;
             * masterConfig.sclGlitchFilterWidth_ns = 0;
             */
            LPI2C_MasterGetDefaultConfig(&masterConfig);

            /* Initialize the LPI2C master peripheral */
            LPI2C_MasterInit(lpi2c->resource->base, &masterConfig, lpi2c->resource->GetFreq());

            lpi2c->flags |= I2C_FLAG_POWER;

            return ARM_DRIVER_OK;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

ARM_I2C_STATUS LPI2C_Master_EdmaGetStatus(cmsis_lpi2c_edma_driver_state_t *lpi2c)
{
    ARM_I2C_STATUS stat               = {0};
    uint32_t ksdk_lpi2c_master_status = LPI2C_MasterGetStatusFlags(lpi2c->resource->base);

    stat.busy      = !(!(ksdk_lpi2c_master_status & kLPI2C_MasterBusyFlag)); /* Busy flag */
    stat.mode      = 1;                                                      /* Mode: 0=Slave, 1=Master */
    stat.direction = lpi2c->master_edma_handle->transfer.direction;          /* Direction: 0=Transmitter, 1=Receiver */
    stat.arbitration_lost =
        !(!(ksdk_lpi2c_master_status & kLPI2C_MasterArbitrationLostFlag)); /* Master lost arbitration */

    return stat;
}
#endif

#endif

#if ((RTE_I2C0 && !RTE_I2C0_DMA_EN) || (RTE_I2C1 && !RTE_I2C1_DMA_EN) || (RTE_I2C2 && !RTE_I2C2_DMA_EN) || \
     (RTE_I2C3 && !RTE_I2C3_DMA_EN) || (RTE_I2C4 && !RTE_I2C4_DMA_EN) || (RTE_I2C5 && !RTE_I2C5_DMA_EN) || \
     (RTE_I2C6 && !RTE_I2C6_DMA_EN))

static void KSDK_LPI2C_SLAVE_InterruptCallback(LPI2C_Type *base,
                                               lpi2c_slave_transfer_t *xfer,
                                               void *userData,
                                               cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    uint32_t event = 0;

    switch (xfer->event)
    {
        /*  Transfer done */
        case kLPI2C_SlaveCompletionEvent:
            event = ARM_I2C_EVENT_TRANSFER_DONE;
            break;

        /* Setup the slave receive buffer */
        case kLPI2C_SlaveReceiveEvent:
            xfer->data     = lpi2c->slave_data;
            xfer->dataSize = lpi2c->slave_dataSize;
            break;

        /* Setup the slave transmit buffer */
        case kLPI2C_SlaveTransmitEvent:
            xfer->data     = lpi2c->slave_data;
            xfer->dataSize = lpi2c->slave_dataSize;
            break;

        default:
            event = ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
            break;
    }

    if (userData)
    {
        ((ARM_I2C_SignalEvent_t)userData)(event);
    }
}

static void KSDK_LPI2C_MASTER_InterruptCallback(LPI2C_Type *base,
                                                lpi2c_master_handle_t *handle,
                                                status_t status,
                                                void *userData)
{
    uint32_t event = 0;

    /* Signal transfer success when received success status. */
    switch (status)
    {
        /*  Transfer done */
        case kStatus_Success:
            event = ARM_I2C_EVENT_TRANSFER_DONE;
            break;

        /* Occurs in master mode when arbitration is lost */
        case kStatus_LPI2C_ArbitrationLost:
            event = ARM_I2C_EVENT_ARBITRATION_LOST;
            break;

        default:
            event = ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
            break;
    }

    if (userData)
    {
        ((ARM_I2C_SignalEvent_t)userData)(event);
    }
}

static int32_t LPI2C_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event, cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    if (!(lpi2c->flags & I2C_FLAG_INIT))
    {
        lpi2c->cb_event = cb_event; /* Call back function */
        lpi2c->flags    = I2C_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t LPI2C_InterruptUninitialize(cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    lpi2c->flags = I2C_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

int32_t LPI2C_Master_InterruptTransmit(
    uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending, cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    int32_t status;
    int32_t ret;
    lpi2c_master_transfer_t masterXfer;

    if (lpi2c->handle->master_handle.state != 0)
    {
        return ARM_DRIVER_ERROR_BUSY; /* Master is busy */
    }

    /* Create master_handle */
    LPI2C_MasterTransferCreateHandle(lpi2c->resource->base, &(lpi2c->handle->master_handle),
                                     KSDK_LPI2C_MASTER_InterruptCallback, (void *)lpi2c->cb_event);

    /* Setup the master transfer */
    masterXfer.slaveAddress   = addr;
    masterXfer.direction      = kLPI2C_Write;
    masterXfer.subaddress     = (uint32_t)NULL;
    masterXfer.subaddressSize = 0;
    masterXfer.data           = (uint8_t *)data;
    masterXfer.dataSize       = num;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    if (xfer_pending)
    {
        /* Stop condition will not be generated */
        masterXfer.flags |= kLPI2C_TransferNoStopFlag;
    }

    /* Send master non-blocking data to slave */
    status = LPI2C_MasterTransferNonBlocking(lpi2c->resource->base, &(lpi2c->handle->master_handle), &masterXfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;

        case kStatus_LPI2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;

        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t LPI2C_Master_InterruptReceive(
    uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending, cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    int32_t status;
    int32_t ret;
    lpi2c_master_transfer_t masterXfer;

    if (lpi2c->handle->master_handle.state != 0)
    {
        /* Master is busy */
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Create master_handle */
    LPI2C_MasterTransferCreateHandle(lpi2c->resource->base, &(lpi2c->handle->master_handle),
                                     KSDK_LPI2C_MASTER_InterruptCallback, (void *)lpi2c->cb_event);

    /* Setup the master transfer */
    masterXfer.slaveAddress   = addr;
    masterXfer.direction      = kLPI2C_Read;
    masterXfer.subaddress     = (uint32_t)NULL;
    masterXfer.subaddressSize = 0;
    masterXfer.data           = data;
    masterXfer.dataSize       = num;
    masterXfer.flags          = kLPI2C_TransferDefaultFlag;

    if (xfer_pending)
    {
        /* Stop condition will not be generated */
        masterXfer.flags |= kLPI2C_TransferNoStopFlag;
    }

    /* Receive non-blocking data from slave */
    status = LPI2C_MasterTransferNonBlocking(lpi2c->resource->base, &(lpi2c->handle->master_handle), &masterXfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;

        case kStatus_LPI2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;

        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t LPI2C_Slave_InterruptTransmit(const uint8_t *data, uint32_t num, cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    int32_t status;
    int32_t ret;

    /* Create slave_handle */
    LPI2C_SlaveTransferCreateHandle(lpi2c->resource->base, &(lpi2c->handle->slave_handle),
                                    lpi2c->handle->slave_handle.callback, (void *)lpi2c->cb_event);

    /* Slave send Nonblocking data to master */
    status = LPI2C_SlaveTransferNonBlocking(lpi2c->resource->base, &(lpi2c->handle->slave_handle),
                                            kLPI2C_SlaveCompletionEvent);

    lpi2c->slave_data                            = (uint8_t *)data; /*!< slave Transfer buffer */
    lpi2c->slave_dataSize                        = num;             /*!< slave Transfer data size */
    lpi2c->handle->slave_handle.transferredCount = 0;               /*!< slave Transfered data count */

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;

        case kStatus_LPI2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;

        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t LPI2C_Slave_InterruptReceive(uint8_t *data, uint32_t num, cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    int32_t status;
    int32_t ret;

    /* Create slave_handle */
    LPI2C_SlaveTransferCreateHandle(lpi2c->resource->base, &(lpi2c->handle->slave_handle),
                                    lpi2c->handle->slave_handle.callback, (void *)lpi2c->cb_event);

    /* Slave receive Nonblocking data from master */
    status = LPI2C_SlaveTransferNonBlocking(lpi2c->resource->base, &(lpi2c->handle->slave_handle),
                                            kLPI2C_SlaveCompletionEvent);

    lpi2c->slave_data                            = data; /*!< slave Transfer buffer */
    lpi2c->slave_dataSize                        = num;  /*!< slave Transfer data size */
    lpi2c->handle->slave_handle.transferredCount = 0;    /*!< slave Transfered data count */

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;

        case kStatus_LPI2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;

        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t LPI2C_InterruptGetDataCount(cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    size_t cnt;

    if ((lpi2c->resource->base->SIER & kSlaveIrqFlags) == kSlaveIrqFlags)
    {
        /* In slave mode */
        LPI2C_SlaveTransferGetCount(lpi2c->resource->base, &lpi2c->handle->slave_handle, &cnt);
    }
    else
    {
        /* In master mode */
        LPI2C_MasterTransferGetCount(lpi2c->resource->base, &lpi2c->handle->master_handle, &cnt);
    }
    return cnt;
}

int32_t LPI2C_InterruptControl(uint32_t control, uint32_t arg, cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    uint32_t baudRate_Bps;

    switch (control)
    {
        case ARM_I2C_OWN_ADDRESS:
            /* Set Own Slave Address */
            lpi2c->resource->base->SAMR = (arg << 1U);
            return ARM_DRIVER_OK;

        case ARM_I2C_BUS_SPEED:
            /* Set Bus Speed */
            switch (arg)
            {
                case ARM_I2C_BUS_SPEED_STANDARD:
                    baudRate_Bps = 100000;
                    break;

                case ARM_I2C_BUS_SPEED_FAST:
                    baudRate_Bps = 400000;
                    break;

                case ARM_I2C_BUS_SPEED_FAST_PLUS:
                    baudRate_Bps = 1000000;
                    break;

                default:
                    return ARM_DRIVER_ERROR_UNSUPPORTED;
            }

            LPI2C_MasterSetBaudRate(lpi2c->resource->base, lpi2c->resource->GetFreq(), baudRate_Bps);

            return ARM_DRIVER_OK;

        /* Not supported. */
        case ARM_I2C_BUS_CLEAR:

            return ARM_DRIVER_ERROR_UNSUPPORTED;

        /* Only support aborting data transfer when master transmit(in master mode) or slave receive(in slave mode) */
        case ARM_I2C_ABORT_TRANSFER:
            /* Abort data transfer when slave receive(in slave mode) */
            if ((lpi2c->resource->base->SIER & kSlaveIrqFlags) == kSlaveIrqFlags)
            {
                /* Disable slave mode */
                lpi2c->resource->base->SCR = 0;

                /* Diable slave interrupt */
                LPI2C_SlaveTransferAbort(lpi2c->resource->base, &(lpi2c->handle->slave_handle));

                /* Enable slave mode */
                lpi2c->resource->base->SCR = 0x31;
            }

            /* Bort data transfer when master transmit abort(in master mode) */
            if ((lpi2c->resource->base->MIER & kMasterIrqFlags) == kMasterIrqFlags)
            {
                /* Disable master interrupt and send stop */
                LPI2C_MasterTransferAbort(lpi2c->resource->base, &(lpi2c->handle->master_handle));

                lpi2c->handle->master_handle.remainingBytes    = 0;
                lpi2c->handle->master_handle.transfer.data     = NULL;
                lpi2c->handle->master_handle.transfer.dataSize = 0;
            }

            return ARM_DRIVER_OK;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t LPI2C_InterruptPowerControl(ARM_POWER_STATE state, cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    switch (state)
    {
        lpi2c_slave_config_t slaveConfig;
        lpi2c_master_config_t masterConfig;

        case ARM_POWER_OFF:
            if (lpi2c->flags & I2C_FLAG_POWER)
            {
                /* Terminates any pending data transfers */
                LPI2C_InterruptControl(ARM_I2C_ABORT_TRANSFER, 0, lpi2c);

                /* Disables peripheral */
                LPI2C_MasterDeinit(lpi2c->resource->base);
                lpi2c->flags = I2C_FLAG_INIT;
            }
            return ARM_DRIVER_OK;

        case ARM_POWER_LOW:

            return ARM_DRIVER_ERROR_UNSUPPORTED;

        case ARM_POWER_FULL:

            if (lpi2c->flags == I2C_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (lpi2c->flags & I2C_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }

            /*
             * slaveConfig.address0 = 0U;
             * slaveConfig.address1 = 0U;
             * slaveConfig.addressMatchMode = kLPI2C_MatchAddress0;
             * slaveConfig.filterDozeEnable = true;
             * slaveConfig.filterEnable = true;
             * slaveConfig.enableGeneralCall = false;
             * slaveConfig.ignoreAck = false;
             * slaveConfig.enableReceivedAddressRead = false;
             * slaveConfig.sdaGlitchFilterWidth_ns = 0;
             * slaveConfig.sclGlitchFilterWidth_ns = 0;
             * slaveConfig.dataValidDelay_ns = 0;
             * slaveConfig.clockHoldTime_ns = 0;
             */
            LPI2C_SlaveGetDefaultConfig(&slaveConfig);

            /* Initialize the LPI2C slave peripheral */
            LPI2C_SlaveInit(lpi2c->resource->base, &slaveConfig, lpi2c->resource->GetFreq());

            /*
             * masterConfig.debugEnable = false;
             * masterConfig.ignoreAck = false;
             * masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
             * masterConfig.baudRate_Hz = 100000U;
             * masterConfig.busIdleTimeout_ns = 0;
             * masterConfig.pinLowTimeout_ns = 0;
             * masterConfig.sdaGlitchFilterWidth_ns = 0;
             * masterConfig.sclGlitchFilterWidth_ns = 0;
             */
            LPI2C_MasterGetDefaultConfig(&masterConfig);

            /* Initialize the LPI2C master peripheral */
            LPI2C_MasterInit(lpi2c->resource->base, &masterConfig, lpi2c->resource->GetFreq());

            lpi2c->flags |= I2C_FLAG_POWER;

            return ARM_DRIVER_OK;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

ARM_I2C_STATUS LPI2C_InterruptGetStatus(cmsis_lpi2c_interrupt_driver_state_t *lpi2c)
{
    ARM_I2C_STATUS stat               = {0};
    uint32_t ksdk_lpi2c_master_status = LPI2C_MasterGetStatusFlags(lpi2c->resource->base);
    uint32_t ksdk_lpi2c_slave_status  = LPI2C_SlaveGetStatusFlags(lpi2c->resource->base);

    /* Busy flag */
    stat.busy =
        !(!((ksdk_lpi2c_master_status & kLPI2C_MasterBusyFlag) | (ksdk_lpi2c_slave_status & kLPI2C_SlaveBusyFlag)));

    /* Mode: 0=Slave, 1=Master */
    if ((lpi2c->resource->base->SIER & kSlaveIrqFlags) == kSlaveIrqFlags)
    {
        stat.mode = 0;
    }
    else
    {
        stat.mode = 1;
    }

    /* Direction: 0=Transmitter, 1=Receiver */
    stat.direction = lpi2c->handle->master_handle.transfer.direction;

    /* Master lost arbitration */
    stat.arbitration_lost = !(!(ksdk_lpi2c_master_status & kLPI2C_MasterArbitrationLostFlag));

    return stat;
}

#endif

#if defined(LPI2C0) && RTE_I2C0
/* User needs to provide the implementation for LPI2C0_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPI2C0_GetFreq(void);
extern void LPI2C0_InitPins(void);
extern void LPI2C0_DeinitPins(void);

cmsis_lpi2c_resource_t LPI2C0_Resource = {LPI2C0, LPI2C0_GetFreq};

#if RTE_I2C0_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpi2c_edma_resource_t LPI2C0_EdmaResource = {
    RTE_I2C0_DMA_TX_DMA_BASE,    RTE_I2C0_DMA_TX_CH,          RTE_I2C0_DMA_TX_PERI_SEL,
    RTE_I2C0_DMA_RX_DMA_BASE,    RTE_I2C0_DMA_RX_CH,          RTE_I2C0_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_I2C0_DMA_TX_DMAMUX_BASE, RTE_I2C0_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(lpi2c_master_edma_handle_t LPI2C0_EdmaHandle);
edma_handle_t LPI2C0_EdmaTxHandle;
edma_handle_t LPI2C0_EdmaRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c0_edma_driver_state")
cmsis_lpi2c_edma_driver_state_t LPI2C0_EdmaDriverState = {
#else
cmsis_lpi2c_edma_driver_state_t LPI2C0_EdmaDriverState = {
#endif
    &LPI2C0_Resource, &LPI2C0_EdmaResource, &LPI2C0_EdmaHandle, &LPI2C0_EdmaTxHandle, &LPI2C0_EdmaRxHandle,
};

static int32_t LPI2C0_Master_EdmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C0_InitPins();
    return LPI2C_Master_EdmaInitialize(cb_event, &LPI2C0_EdmaDriverState);
}

int32_t LPI2C0_Master_EdmaUninitialize(void)
{
    LPI2C0_DeinitPins();
    return LPI2C_Master_EdmaUninitialize(&LPI2C0_EdmaDriverState);
}

int32_t LPI2C0_Master_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_Master_EdmaPowerControl(state, &LPI2C0_EdmaDriverState);
}

int32_t LPI2C0_Master_EdmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaTransmit(addr, data, num, xfer_pending, &LPI2C0_EdmaDriverState);
}

int32_t LPI2C0_Master_EdmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaReceive(addr, data, num, xfer_pending, &LPI2C0_EdmaDriverState);
}

int32_t LPI2C0_Master_EdmaGetDataCount(void)
{
    return LPI2C_Master_EdmaGetDataCount(&LPI2C0_EdmaDriverState);
}

int32_t LPI2C0_Master_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPI2C_Master_EdmaControl(control, arg, &LPI2C0_EdmaDriverState);
}

ARM_I2C_STATUS LPI2C0_Master_EdmaGetStatus(void)
{
    return LPI2C_Master_EdmaGetStatus(&LPI2C0_EdmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t LPI2C0_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c0_interrupt_driver_state")
cmsis_lpi2c_interrupt_driver_state_t LPI2C0_InterruptDriverState = {
#else
cmsis_lpi2c_interrupt_driver_state_t LPI2C0_InterruptDriverState = {
#endif
    &LPI2C0_Resource,
    &LPI2C0_Handle,

};

static void KSDK_LPI2C0_SLAVE_InterruptCallback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *userData)
{
    KSDK_LPI2C_SLAVE_InterruptCallback(base, xfer, userData, &LPI2C0_InterruptDriverState);
}

static int32_t LPI2C0_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C0_InitPins();
    return LPI2C_InterruptInitialize(cb_event, &LPI2C0_InterruptDriverState);
}

static int32_t LPI2C0_InterruptUninitialize(void)
{
    LPI2C0_DeinitPins();
    return LPI2C_InterruptUninitialize(&LPI2C0_InterruptDriverState);
}

static int32_t LPI2C0_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_InterruptPowerControl(state, &LPI2C0_InterruptDriverState);
}

int32_t LPI2C0_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &LPI2C0_InterruptDriverState);
}

int32_t LPI2C0_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptReceive(addr, data, num, xfer_pending, &LPI2C0_InterruptDriverState);
}

int32_t LPI2C0_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    LPI2C0_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C0_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptTransmit(data, num, &LPI2C0_InterruptDriverState);
}

int32_t LPI2C0_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    LPI2C0_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C0_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptReceive(data, num, &LPI2C0_InterruptDriverState);
}

int32_t LPI2C0_InterruptGetDataCount(void)
{
    return LPI2C_InterruptGetDataCount(&LPI2C0_InterruptDriverState);
}

int32_t LPI2C0_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPI2C_InterruptControl(control, arg, &LPI2C0_InterruptDriverState);
}

ARM_I2C_STATUS LPI2C0_InterruptGetStatus(void)
{
    return LPI2C_InterruptGetStatus(&LPI2C0_InterruptDriverState);
}

#endif

ARM_DRIVER_I2C Driver_I2C0 = {LPI2Cx_GetVersion,
                              LPI2Cx_GetCapabilities,
#if RTE_I2C0_DMA_EN
                              LPI2C0_Master_EdmaInitialize,
                              LPI2C0_Master_EdmaUninitialize,
                              LPI2C0_Master_EdmaPowerControl,
                              LPI2C0_Master_EdmaTransmit,
                              LPI2C0_Master_EdmaReceive,
                              NULL,
                              NULL,
                              LPI2C0_Master_EdmaGetDataCount,
                              LPI2C0_Master_EdmaControl,
                              LPI2C0_Master_EdmaGetStatus
#else
                              LPI2C0_InterruptInitialize,
                              LPI2C0_InterruptUninitialize,
                              LPI2C0_InterruptPowerControl,
                              LPI2C0_Master_InterruptTransmit,
                              LPI2C0_Master_InterruptReceive,
                              LPI2C0_Slave_InterruptTransmit,
                              LPI2C0_Slave_InterruptReceive,
                              LPI2C0_InterruptGetDataCount,
                              LPI2C0_InterruptControl,
                              LPI2C0_InterruptGetStatus
#endif
};

#endif

#if defined(LPI2C1) && RTE_I2C1
/* User needs to provide the implementation for LPI2C1_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPI2C1_GetFreq(void);
extern void LPI2C1_InitPins(void);
extern void LPI2C1_DeinitPins(void);

cmsis_lpi2c_resource_t LPI2C1_Resource = {LPI2C1, LPI2C1_GetFreq};

#if RTE_I2C1_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpi2c_edma_resource_t LPI2C1_EdmaResource = {
    RTE_I2C1_DMA_TX_DMA_BASE,    RTE_I2C1_DMA_TX_CH,          RTE_I2C1_DMA_TX_PERI_SEL,
    RTE_I2C1_DMA_RX_DMA_BASE,    RTE_I2C1_DMA_RX_CH,          RTE_I2C1_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_I2C1_DMA_TX_DMAMUX_BASE, RTE_I2C1_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(lpi2c_master_edma_handle_t LPI2C1_EdmaHandle);
edma_handle_t LPI2C1_EdmaTxHandle;
edma_handle_t LPI2C1_EdmaRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c1_edma_driver_state")
cmsis_lpi2c_edma_driver_state_t LPI2C1_EdmaDriverState = {
#else
cmsis_lpi2c_edma_driver_state_t LPI2C1_EdmaDriverState = {
#endif
    &LPI2C1_Resource, &LPI2C1_EdmaResource, &LPI2C1_EdmaHandle, &LPI2C1_EdmaTxHandle, &LPI2C1_EdmaRxHandle,
};

static int32_t LPI2C1_Master_EdmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C1_InitPins();
    return LPI2C_Master_EdmaInitialize(cb_event, &LPI2C1_EdmaDriverState);
}

int32_t LPI2C1_Master_EdmaUninitialize(void)
{
    LPI2C1_DeinitPins();
    return LPI2C_Master_EdmaUninitialize(&LPI2C1_EdmaDriverState);
}

int32_t LPI2C1_Master_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_Master_EdmaPowerControl(state, &LPI2C1_EdmaDriverState);
}

int32_t LPI2C1_Master_EdmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaTransmit(addr, data, num, xfer_pending, &LPI2C1_EdmaDriverState);
}

int32_t LPI2C1_Master_EdmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaReceive(addr, data, num, xfer_pending, &LPI2C1_EdmaDriverState);
}

int32_t LPI2C1_Master_EdmaGetDataCount(void)
{
    return LPI2C_Master_EdmaGetDataCount(&LPI2C1_EdmaDriverState);
}

int32_t LPI2C1_Master_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPI2C_Master_EdmaControl(control, arg, &LPI2C1_EdmaDriverState);
}

ARM_I2C_STATUS LPI2C1_Master_EdmaGetStatus(void)
{
    return LPI2C_Master_EdmaGetStatus(&LPI2C1_EdmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t LPI2C1_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c1_interrupt_driver_state")
cmsis_lpi2c_interrupt_driver_state_t LPI2C1_InterruptDriverState = {
#else
cmsis_lpi2c_interrupt_driver_state_t LPI2C1_InterruptDriverState = {
#endif
    &LPI2C1_Resource,
    &LPI2C1_Handle,
};

static void KSDK_LPI2C1_SLAVE_InterruptCallback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *userData)
{
    KSDK_LPI2C_SLAVE_InterruptCallback(base, xfer, userData, &LPI2C1_InterruptDriverState);
}

static int32_t LPI2C1_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C1_InitPins();
    return LPI2C_InterruptInitialize(cb_event, &LPI2C1_InterruptDriverState);
}

static int32_t LPI2C1_InterruptUninitialize(void)
{
    LPI2C1_DeinitPins();
    return LPI2C_InterruptUninitialize(&LPI2C1_InterruptDriverState);
}

static int32_t LPI2C1_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_InterruptPowerControl(state, &LPI2C1_InterruptDriverState);
}

int32_t LPI2C1_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &LPI2C1_InterruptDriverState);
}

int32_t LPI2C1_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptReceive(addr, data, num, xfer_pending, &LPI2C1_InterruptDriverState);
}

int32_t LPI2C1_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    LPI2C1_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C1_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptTransmit(data, num, &LPI2C1_InterruptDriverState);
}

int32_t LPI2C1_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    LPI2C1_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C1_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptReceive(data, num, &LPI2C1_InterruptDriverState);
}

int32_t LPI2C1_InterruptGetDataCount(void)
{
    return LPI2C_InterruptGetDataCount(&LPI2C1_InterruptDriverState);
}

int32_t LPI2C1_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPI2C_InterruptControl(control, arg, &LPI2C1_InterruptDriverState);
}

ARM_I2C_STATUS LPI2C1_InterruptGetStatus(void)
{
    return LPI2C_InterruptGetStatus(&LPI2C1_InterruptDriverState);
}

#endif

ARM_DRIVER_I2C Driver_I2C1 = {LPI2Cx_GetVersion,
                              LPI2Cx_GetCapabilities,
#if RTE_I2C1_DMA_EN
                              LPI2C1_Master_EdmaInitialize,
                              LPI2C1_Master_EdmaUninitialize,
                              LPI2C1_Master_EdmaPowerControl,
                              LPI2C1_Master_EdmaTransmit,
                              LPI2C1_Master_EdmaReceive,
                              NULL,
                              NULL,
                              LPI2C1_Master_EdmaGetDataCount,
                              LPI2C1_Master_EdmaControl,
                              LPI2C1_Master_EdmaGetStatus
#else
                              LPI2C1_InterruptInitialize,
                              LPI2C1_InterruptUninitialize,
                              LPI2C1_InterruptPowerControl,
                              LPI2C1_Master_InterruptTransmit,
                              LPI2C1_Master_InterruptReceive,
                              LPI2C1_Slave_InterruptTransmit,
                              LPI2C1_Slave_InterruptReceive,
                              LPI2C1_InterruptGetDataCount,
                              LPI2C1_InterruptControl,
                              LPI2C1_InterruptGetStatus
#endif
};

#endif

#if defined(LPI2C2) && RTE_I2C2
/* User needs to provide the implementation for LPI2C2_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPI2C2_GetFreq(void);
extern void LPI2C2_InitPins(void);
extern void LPI2C2_DeinitPins(void);

cmsis_lpi2c_resource_t LPI2C2_Resource = {LPI2C2, LPI2C2_GetFreq};

#if RTE_I2C2_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpi2c_edma_resource_t LPI2C2_EdmaResource = {
    RTE_I2C2_DMA_TX_DMA_BASE,    RTE_I2C2_DMA_TX_CH,          RTE_I2C2_DMA_TX_PERI_SEL,
    RTE_I2C2_DMA_RX_DMA_BASE,    RTE_I2C2_DMA_RX_CH,          RTE_I2C2_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_I2C2_DMA_TX_DMAMUX_BASE, RTE_I2C2_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(lpi2c_master_edma_handle_t LPI2C2_EdmaHandle);
edma_handle_t LPI2C2_EdmaTxHandle;
edma_handle_t LPI2C2_EdmaRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c2_edma_driver_state")
cmsis_lpi2c_edma_driver_state_t LPI2C2_EdmaDriverState = {
#else
cmsis_lpi2c_edma_driver_state_t LPI2C2_EdmaDriverState = {
#endif
    &LPI2C2_Resource, &LPI2C2_EdmaResource, &LPI2C2_EdmaHandle, &LPI2C2_EdmaTxHandle, &LPI2C2_EdmaRxHandle,
};

static int32_t LPI2C2_Master_EdmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C2_InitPins();
    return LPI2C_Master_EdmaInitialize(cb_event, &LPI2C2_EdmaDriverState);
}

int32_t LPI2C2_Master_EdmaUninitialize(void)
{
    LPI2C2_DeinitPins();
    return LPI2C_Master_EdmaUninitialize(&LPI2C2_EdmaDriverState);
}

int32_t LPI2C2_Master_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_Master_EdmaPowerControl(state, &LPI2C2_EdmaDriverState);
}

int32_t LPI2C2_Master_EdmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaTransmit(addr, data, num, xfer_pending, &LPI2C2_EdmaDriverState);
}

int32_t LPI2C2_Master_EdmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaReceive(addr, data, num, xfer_pending, &LPI2C2_EdmaDriverState);
}

int32_t LPI2C2_Master_EdmaGetDataCount(void)
{
    return LPI2C_Master_EdmaGetDataCount(&LPI2C2_EdmaDriverState);
}

int32_t LPI2C2_Master_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPI2C_Master_EdmaControl(control, arg, &LPI2C2_EdmaDriverState);
}

ARM_I2C_STATUS LPI2C2_Master_EdmaGetStatus(void)
{
    return LPI2C_Master_EdmaGetStatus(&LPI2C2_EdmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t LPI2C2_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c2_interrupt_driver_state")
cmsis_lpi2c_interrupt_driver_state_t LPI2C2_InterruptDriverState = {
#else
cmsis_lpi2c_interrupt_driver_state_t LPI2C2_InterruptDriverState = {
#endif
    &LPI2C2_Resource,
    &LPI2C2_Handle,

};

static void KSDK_LPI2C2_SLAVE_InterruptCallback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *userData)
{
    KSDK_LPI2C_SLAVE_InterruptCallback(base, xfer, userData, &LPI2C2_InterruptDriverState);
}

static int32_t LPI2C2_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C2_InitPins();
    return LPI2C_InterruptInitialize(cb_event, &LPI2C2_InterruptDriverState);
}

static int32_t LPI2C2_InterruptUninitialize(void)
{
    LPI2C2_DeinitPins();
    return LPI2C_InterruptUninitialize(&LPI2C2_InterruptDriverState);
}

static int32_t LPI2C2_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_InterruptPowerControl(state, &LPI2C2_InterruptDriverState);
}

int32_t LPI2C2_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &LPI2C2_InterruptDriverState);
}

int32_t LPI2C2_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptReceive(addr, data, num, xfer_pending, &LPI2C2_InterruptDriverState);
}

int32_t LPI2C2_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    LPI2C2_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C2_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptTransmit(data, num, &LPI2C2_InterruptDriverState);
}

int32_t LPI2C2_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    LPI2C2_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C2_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptReceive(data, num, &LPI2C2_InterruptDriverState);
}

int32_t LPI2C2_InterruptGetDataCount(void)
{
    return LPI2C_InterruptGetDataCount(&LPI2C2_InterruptDriverState);
}

int32_t LPI2C2_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPI2C_InterruptControl(control, arg, &LPI2C2_InterruptDriverState);
}

ARM_I2C_STATUS LPI2C2_InterruptGetStatus(void)
{
    return LPI2C_InterruptGetStatus(&LPI2C2_InterruptDriverState);
}

#endif

ARM_DRIVER_I2C Driver_I2C2 = {LPI2Cx_GetVersion,
                              LPI2Cx_GetCapabilities,
#if RTE_I2C2_DMA_EN
                              LPI2C2_Master_EdmaInitialize,
                              LPI2C2_Master_EdmaUninitialize,
                              LPI2C2_Master_EdmaPowerControl,
                              LPI2C2_Master_EdmaTransmit,
                              LPI2C2_Master_EdmaReceive,
                              NULL,
                              NULL,
                              LPI2C2_Master_EdmaGetDataCount,
                              LPI2C2_Master_EdmaControl,
                              LPI2C2_Master_EdmaGetStatus
#else
                              LPI2C2_InterruptInitialize,
                              LPI2C2_InterruptUninitialize,
                              LPI2C2_InterruptPowerControl,
                              LPI2C2_Master_InterruptTransmit,
                              LPI2C2_Master_InterruptReceive,
                              LPI2C2_Slave_InterruptTransmit,
                              LPI2C2_Slave_InterruptReceive,
                              LPI2C2_InterruptGetDataCount,
                              LPI2C2_InterruptControl,
                              LPI2C2_InterruptGetStatus
#endif
};

#endif

#if defined(LPI2C3) && RTE_I2C3
/* User needs to provide the implementation for LPI2C3_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPI2C3_GetFreq(void);
extern void LPI2C3_InitPins(void);
extern void LPI2C3_DeinitPins(void);

cmsis_lpi2c_resource_t LPI2C3_Resource = {LPI2C3, LPI2C3_GetFreq};

#if RTE_I2C3_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpi2c_edma_resource_t LPI2C3_EdmaResource = {
    RTE_I2C3_DMA_TX_DMA_BASE,    RTE_I2C3_DMA_TX_CH,          RTE_I2C3_DMA_TX_PERI_SEL,
    RTE_I2C3_DMA_RX_DMA_BASE,    RTE_I2C3_DMA_RX_CH,          RTE_I2C3_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_I2C3_DMA_TX_DMAMUX_BASE, RTE_I2C3_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(lpi2c_master_edma_handle_t LPI2C3_EdmaHandle);
edma_handle_t LPI2C3_EdmaTxHandle;
edma_handle_t LPI2C3_EdmaRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c3_edma_driver_state")
cmsis_lpi2c_edma_driver_state_t LPI2C3_EdmaDriverState = {
#else
cmsis_lpi2c_edma_driver_state_t LPI2C3_EdmaDriverState = {
#endif
    &LPI2C3_Resource, &LPI2C3_EdmaResource, &LPI2C3_EdmaHandle, &LPI2C3_EdmaTxHandle, &LPI2C3_EdmaRxHandle,
};

static int32_t LPI2C3_Master_EdmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C3_InitPins();
    return LPI2C_Master_EdmaInitialize(cb_event, &LPI2C3_EdmaDriverState);
}

int32_t LPI2C3_Master_EdmaUninitialize(void)
{
    LPI2C3_DeinitPins();
    return LPI2C_Master_EdmaUninitialize(&LPI2C3_EdmaDriverState);
}

int32_t LPI2C3_Master_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_Master_EdmaPowerControl(state, &LPI2C3_EdmaDriverState);
}

int32_t LPI2C3_Master_EdmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaTransmit(addr, data, num, xfer_pending, &LPI2C3_EdmaDriverState);
}

int32_t LPI2C3_Master_EdmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaReceive(addr, data, num, xfer_pending, &LPI2C3_EdmaDriverState);
}

int32_t LPI2C3_Master_EdmaGetDataCount(void)
{
    return LPI2C_Master_EdmaGetDataCount(&LPI2C3_EdmaDriverState);
}

int32_t LPI2C3_Master_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPI2C_Master_EdmaControl(control, arg, &LPI2C3_EdmaDriverState);
}

ARM_I2C_STATUS LPI2C3_Master_EdmaGetStatus(void)
{
    return LPI2C_Master_EdmaGetStatus(&LPI2C3_EdmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t LPI2C3_Handle;
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c3_interrupt_driver_state")
cmsis_lpi2c_interrupt_driver_state_t LPI2C3_InterruptDriverState = {
#else
cmsis_lpi2c_interrupt_driver_state_t LPI2C3_InterruptDriverState = {
#endif
    &LPI2C3_Resource,
    &LPI2C3_Handle,
};

static void KSDK_LPI2C3_SLAVE_InterruptCallback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *userData)
{
    KSDK_LPI2C_SLAVE_InterruptCallback(base, xfer, userData, &LPI2C3_InterruptDriverState);
}

static int32_t LPI2C3_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C3_InitPins();
    return LPI2C_InterruptInitialize(cb_event, &LPI2C3_InterruptDriverState);
}

static int32_t LPI2C3_InterruptUninitialize(void)
{
    LPI2C3_DeinitPins();
    return LPI2C_InterruptUninitialize(&LPI2C3_InterruptDriverState);
}

static int32_t LPI2C3_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_InterruptPowerControl(state, &LPI2C3_InterruptDriverState);
}

int32_t LPI2C3_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &LPI2C3_InterruptDriverState);
}

int32_t LPI2C3_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptReceive(addr, data, num, xfer_pending, &LPI2C3_InterruptDriverState);
}

int32_t LPI2C3_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    LPI2C3_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C3_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptTransmit(data, num, &LPI2C3_InterruptDriverState);
}

int32_t LPI2C3_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    LPI2C3_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C3_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptReceive(data, num, &LPI2C3_InterruptDriverState);
}

int32_t LPI2C3_InterruptGetDataCount(void)
{
    return LPI2C_InterruptGetDataCount(&LPI2C3_InterruptDriverState);
}

int32_t LPI2C3_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPI2C_InterruptControl(control, arg, &LPI2C3_InterruptDriverState);
}

ARM_I2C_STATUS LPI2C3_InterruptGetStatus(void)
{
    return LPI2C_InterruptGetStatus(&LPI2C3_InterruptDriverState);
}

#endif /* RTE_I2C3_DMA_EN */

ARM_DRIVER_I2C Driver_I2C3 = {LPI2Cx_GetVersion,
                              LPI2Cx_GetCapabilities,
#if RTE_I2C3_DMA_EN
                              LPI2C3_Master_EdmaInitialize,
                              LPI2C3_Master_EdmaUninitialize,
                              LPI2C3_Master_EdmaPowerControl,
                              LPI2C3_Master_EdmaTransmit,
                              LPI2C3_Master_EdmaReceive,
                              NULL,
                              NULL,
                              LPI2C3_Master_EdmaGetDataCount,
                              LPI2C3_Master_EdmaControl,
                              LPI2C3_Master_EdmaGetStatus
#else
                              LPI2C3_InterruptInitialize,
                              LPI2C3_InterruptUninitialize,
                              LPI2C3_InterruptPowerControl,
                              LPI2C3_Master_InterruptTransmit,
                              LPI2C3_Master_InterruptReceive,
                              LPI2C3_Slave_InterruptTransmit,
                              LPI2C3_Slave_InterruptReceive,
                              LPI2C3_InterruptGetDataCount,
                              LPI2C3_InterruptControl,
                              LPI2C3_InterruptGetStatus
#endif /* RTE_I2C3_DMA_EN */
};

#endif

#if defined(LPI2C4) && RTE_I2C4
/* User needs to provide the implementation for LPI2C4_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPI2C4_GetFreq(void);
extern void LPI2C4_InitPins(void);
extern void LPI2C4_DeinitPins(void);

cmsis_lpi2c_resource_t LPI2C4_Resource = {LPI2C4, LPI2C4_GetFreq};

#if RTE_I2C4_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpi2c_edma_resource_t LPI2C4_EdmaResource = {
    RTE_I2C4_DMA_TX_DMA_BASE,    RTE_I2C4_DMA_TX_CH,          RTE_I2C4_DMA_TX_PERI_SEL,
    RTE_I2C4_DMA_RX_DMA_BASE,    RTE_I2C4_DMA_RX_CH,          RTE_I2C4_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_I2C4_DMA_TX_DMAMUX_BASE, RTE_I2C4_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(lpi2c_master_edma_handle_t LPI2C4_EdmaHandle);
edma_handle_t LPI2C4_EdmaTxHandle;
edma_handle_t LPI2C4_EdmaRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c4_edma_driver_state")
cmsis_lpi2c_edma_driver_state_t LPI2C4_EdmaDriverState = {
#else
cmsis_lpi2c_edma_driver_state_t LPI2C4_EdmaDriverState = {
#endif
    &LPI2C4_Resource, &LPI2C4_EdmaResource, &LPI2C4_EdmaHandle, &LPI2C4_EdmaTxHandle, &LPI2C4_EdmaRxHandle,
};

static int32_t LPI2C4_Master_EdmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C4_InitPins();
    return LPI2C_Master_EdmaInitialize(cb_event, &LPI2C4_EdmaDriverState);
}

int32_t LPI2C4_Master_EdmaUninitialize(void)
{
    LPI2C4_DeinitPins();
    return LPI2C_Master_EdmaUninitialize(&LPI2C4_EdmaDriverState);
}

int32_t LPI2C4_Master_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_Master_EdmaPowerControl(state, &LPI2C4_EdmaDriverState);
}

int32_t LPI2C4_Master_EdmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaTransmit(addr, data, num, xfer_pending, &LPI2C4_EdmaDriverState);
}

int32_t LPI2C4_Master_EdmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaReceive(addr, data, num, xfer_pending, &LPI2C4_EdmaDriverState);
}

int32_t LPI2C4_Master_EdmaGetDataCount(void)
{
    return LPI2C_Master_EdmaGetDataCount(&LPI2C4_EdmaDriverState);
}

int32_t LPI2C4_Master_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPI2C_Master_EdmaControl(control, arg, &LPI2C4_EdmaDriverState);
}

ARM_I2C_STATUS LPI2C4_Master_EdmaGetStatus(void)
{
    return LPI2C_Master_EdmaGetStatus(&LPI2C4_EdmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t LPI2C4_Handle;
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c4_interrupt_driver_state")
cmsis_lpi2c_interrupt_driver_state_t LPI2C4_InterruptDriverState = {
#else
cmsis_lpi2c_interrupt_driver_state_t LPI2C4_InterruptDriverState = {
#endif
    &LPI2C4_Resource,
    &LPI2C4_Handle,
};

static void KSDK_LPI2C4_SLAVE_InterruptCallback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *userData)
{
    KSDK_LPI2C_SLAVE_InterruptCallback(base, xfer, userData, &LPI2C4_InterruptDriverState);
}

static int32_t LPI2C4_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C4_InitPins();
    return LPI2C_InterruptInitialize(cb_event, &LPI2C4_InterruptDriverState);
}

static int32_t LPI2C4_InterruptUninitialize(void)
{
    LPI2C4_DeinitPins();
    return LPI2C_InterruptUninitialize(&LPI2C4_InterruptDriverState);
}

static int32_t LPI2C4_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_InterruptPowerControl(state, &LPI2C4_InterruptDriverState);
}

int32_t LPI2C4_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &LPI2C4_InterruptDriverState);
}

int32_t LPI2C4_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptReceive(addr, data, num, xfer_pending, &LPI2C4_InterruptDriverState);
}

int32_t LPI2C4_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    LPI2C4_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C4_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptTransmit(data, num, &LPI2C4_InterruptDriverState);
}

int32_t LPI2C4_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    LPI2C4_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C4_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptReceive(data, num, &LPI2C4_InterruptDriverState);
}

int32_t LPI2C4_InterruptGetDataCount(void)
{
    return LPI2C_InterruptGetDataCount(&LPI2C4_InterruptDriverState);
}

int32_t LPI2C4_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPI2C_InterruptControl(control, arg, &LPI2C4_InterruptDriverState);
}

ARM_I2C_STATUS LPI2C4_InterruptGetStatus(void)
{
    return LPI2C_InterruptGetStatus(&LPI2C4_InterruptDriverState);
}

#endif /* RTE_I2C4_DMA_EN */

ARM_DRIVER_I2C Driver_I2C4 = {LPI2Cx_GetVersion,
                              LPI2Cx_GetCapabilities,
#if RTE_I2C4_DMA_EN
                              LPI2C4_Master_EdmaInitialize,
                              LPI2C4_Master_EdmaUninitialize,
                              LPI2C4_Master_EdmaPowerControl,
                              LPI2C4_Master_EdmaTransmit,
                              LPI2C4_Master_EdmaReceive,
                              NULL,
                              NULL,
                              LPI2C4_Master_EdmaGetDataCount,
                              LPI2C4_Master_EdmaControl,
                              LPI2C4_Master_EdmaGetStatus
#else
                              LPI2C4_InterruptInitialize,
                              LPI2C4_InterruptUninitialize,
                              LPI2C4_InterruptPowerControl,
                              LPI2C4_Master_InterruptTransmit,
                              LPI2C4_Master_InterruptReceive,
                              LPI2C4_Slave_InterruptTransmit,
                              LPI2C4_Slave_InterruptReceive,
                              LPI2C4_InterruptGetDataCount,
                              LPI2C4_InterruptControl,
                              LPI2C4_InterruptGetStatus
#endif /* RTE_I2C4_DMA_EN */
};

#endif

#if defined(LPI2C5) && RTE_I2C5
/* User needs to provide the implementation for LPI2C5_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPI2C5_GetFreq(void);
extern void LPI2C5_InitPins(void);
extern void LPI2C5_DeinitPins(void);

cmsis_lpi2c_resource_t LPI2C5_Resource = {LPI2C5, LPI2C5_GetFreq};

#if RTE_I2C5_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpi2c_edma_resource_t LPI2C5_EdmaResource = {
    RTE_I2C5_DMA_TX_DMA_BASE,    RTE_I2C5_DMA_TX_CH,          RTE_I2C5_DMA_TX_PERI_SEL,
    RTE_I2C5_DMA_RX_DMA_BASE,    RTE_I2C5_DMA_RX_CH,          RTE_I2C5_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_I2C5_DMA_TX_DMAMUX_BASE, RTE_I2C5_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(lpi2c_master_edma_handle_t LPI2C5_EdmaHandle);
edma_handle_t LPI2C5_EdmaTxHandle;
edma_handle_t LPI2C5_EdmaRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c5_edma_driver_state")
cmsis_lpi2c_edma_driver_state_t LPI2C5_EdmaDriverState = {
#else
cmsis_lpi2c_edma_driver_state_t LPI2C5_EdmaDriverState = {
#endif
    &LPI2C5_Resource, &LPI2C5_EdmaResource, &LPI2C5_EdmaHandle, &LPI2C5_EdmaTxHandle, &LPI2C5_EdmaRxHandle,
};

static int32_t LPI2C5_Master_EdmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C5_InitPins();
    return LPI2C_Master_EdmaInitialize(cb_event, &LPI2C5_EdmaDriverState);
}

int32_t LPI2C5_Master_EdmaUninitialize(void)
{
    LPI2C5_DeinitPins();
    return LPI2C_Master_EdmaUninitialize(&LPI2C5_EdmaDriverState);
}

int32_t LPI2C5_Master_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_Master_EdmaPowerControl(state, &LPI2C5_EdmaDriverState);
}

int32_t LPI2C5_Master_EdmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaTransmit(addr, data, num, xfer_pending, &LPI2C5_EdmaDriverState);
}

int32_t LPI2C5_Master_EdmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaReceive(addr, data, num, xfer_pending, &LPI2C5_EdmaDriverState);
}

int32_t LPI2C5_Master_EdmaGetDataCount(void)
{
    return LPI2C_Master_EdmaGetDataCount(&LPI2C5_EdmaDriverState);
}

int32_t LPI2C5_Master_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPI2C_Master_EdmaControl(control, arg, &LPI2C5_EdmaDriverState);
}

ARM_I2C_STATUS LPI2C5_Master_EdmaGetStatus(void)
{
    return LPI2C_Master_EdmaGetStatus(&LPI2C5_EdmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t LPI2C5_Handle;
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c5_interrupt_driver_state")
cmsis_lpi2c_interrupt_driver_state_t LPI2C5_InterruptDriverState = {
#else
cmsis_lpi2c_interrupt_driver_state_t LPI2C5_InterruptDriverState = {
#endif
    &LPI2C5_Resource,
    &LPI2C5_Handle,
};

static void KSDK_LPI2C5_SLAVE_InterruptCallback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *userData)
{
    KSDK_LPI2C_SLAVE_InterruptCallback(base, xfer, userData, &LPI2C5_InterruptDriverState);
}

static int32_t LPI2C5_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C5_InitPins();
    return LPI2C_InterruptInitialize(cb_event, &LPI2C5_InterruptDriverState);
}

static int32_t LPI2C5_InterruptUninitialize(void)
{
    LPI2C5_DeinitPins();
    return LPI2C_InterruptUninitialize(&LPI2C5_InterruptDriverState);
}

static int32_t LPI2C5_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_InterruptPowerControl(state, &LPI2C5_InterruptDriverState);
}

int32_t LPI2C5_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &LPI2C5_InterruptDriverState);
}

int32_t LPI2C5_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptReceive(addr, data, num, xfer_pending, &LPI2C5_InterruptDriverState);
}

int32_t LPI2C5_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    LPI2C5_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C5_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptTransmit(data, num, &LPI2C5_InterruptDriverState);
}

int32_t LPI2C5_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    LPI2C5_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C5_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptReceive(data, num, &LPI2C5_InterruptDriverState);
}

int32_t LPI2C5_InterruptGetDataCount(void)
{
    return LPI2C_InterruptGetDataCount(&LPI2C5_InterruptDriverState);
}

int32_t LPI2C5_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPI2C_InterruptControl(control, arg, &LPI2C5_InterruptDriverState);
}

ARM_I2C_STATUS LPI2C5_InterruptGetStatus(void)
{
    return LPI2C_InterruptGetStatus(&LPI2C5_InterruptDriverState);
}

#endif /* RTE_I2C5_DMA_EN */

ARM_DRIVER_I2C Driver_I2C5 = {LPI2Cx_GetVersion,
                              LPI2Cx_GetCapabilities,
#if RTE_I2C5_DMA_EN
                              LPI2C5_Master_EdmaInitialize,
                              LPI2C5_Master_EdmaUninitialize,
                              LPI2C5_Master_EdmaPowerControl,
                              LPI2C5_Master_EdmaTransmit,
                              LPI2C5_Master_EdmaReceive,
                              NULL,
                              NULL,
                              LPI2C5_Master_EdmaGetDataCount,
                              LPI2C5_Master_EdmaControl,
                              LPI2C5_Master_EdmaGetStatus
#else
                              LPI2C5_InterruptInitialize,
                              LPI2C5_InterruptUninitialize,
                              LPI2C5_InterruptPowerControl,
                              LPI2C5_Master_InterruptTransmit,
                              LPI2C5_Master_InterruptReceive,
                              LPI2C5_Slave_InterruptTransmit,
                              LPI2C5_Slave_InterruptReceive,
                              LPI2C5_InterruptGetDataCount,
                              LPI2C5_InterruptControl,
                              LPI2C5_InterruptGetStatus
#endif /* RTE_I2C5_DMA_EN */
};

#endif /* LPI2C5 */

#if defined(LPI2C6) && RTE_I2C6
/* User needs to provide the implementation for LPI2C6_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t LPI2C6_GetFreq(void);
extern void LPI2C6_InitPins(void);
extern void LPI2C6_DeinitPins(void);

cmsis_lpi2c_resource_t LPI2C6_Resource = {LPI2C6, LPI2C6_GetFreq};

#if RTE_I2C6_DMA_EN

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
cmsis_lpi2c_edma_resource_t LPI2C6_EdmaResource = {
    RTE_I2C6_DMA_TX_DMA_BASE,    RTE_I2C6_DMA_TX_CH,          RTE_I2C6_DMA_TX_PERI_SEL,
    RTE_I2C6_DMA_RX_DMA_BASE,    RTE_I2C6_DMA_RX_CH,          RTE_I2C6_DMA_RX_PERI_SEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    RTE_I2C6_DMA_TX_DMAMUX_BASE, RTE_I2C6_DMA_RX_DMAMUX_BASE,
#endif
};

AT_NONCACHEABLE_SECTION(lpi2c_master_edma_handle_t LPI2C6_EdmaHandle);
edma_handle_t LPI2C6_EdmaTxHandle;
edma_handle_t LPI2C6_EdmaRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c6_edma_driver_state")
cmsis_lpi2c_edma_driver_state_t LPI2C6_EdmaDriverState = {
#else
cmsis_lpi2c_edma_driver_state_t LPI2C6_EdmaDriverState = {
#endif
    &LPI2C6_Resource, &LPI2C6_EdmaResource, &LPI2C6_EdmaHandle, &LPI2C6_EdmaTxHandle, &LPI2C6_EdmaRxHandle,
};

static int32_t LPI2C6_Master_EdmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C6_InitPins();
    return LPI2C_Master_EdmaInitialize(cb_event, &LPI2C6_EdmaDriverState);
}

int32_t LPI2C6_Master_EdmaUninitialize(void)
{
    LPI2C6_DeinitPins();
    return LPI2C_Master_EdmaUninitialize(&LPI2C6_EdmaDriverState);
}

int32_t LPI2C6_Master_EdmaPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_Master_EdmaPowerControl(state, &LPI2C6_EdmaDriverState);
}

int32_t LPI2C6_Master_EdmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaTransmit(addr, data, num, xfer_pending, &LPI2C6_EdmaDriverState);
}

int32_t LPI2C6_Master_EdmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_EdmaReceive(addr, data, num, xfer_pending, &LPI2C6_EdmaDriverState);
}

int32_t LPI2C6_Master_EdmaGetDataCount(void)
{
    return LPI2C_Master_EdmaGetDataCount(&LPI2C6_EdmaDriverState);
}

int32_t LPI2C6_Master_EdmaControl(uint32_t control, uint32_t arg)
{
    return LPI2C_Master_EdmaControl(control, arg, &LPI2C6_EdmaDriverState);
}

ARM_I2C_STATUS LPI2C6_Master_EdmaGetStatus(void)
{
    return LPI2C_Master_EdmaGetStatus(&LPI2C6_EdmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t LPI2C6_Handle;
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("lpi2c6_interrupt_driver_state")
cmsis_lpi2c_interrupt_driver_state_t LPI2C6_InterruptDriverState = {
#else
cmsis_lpi2c_interrupt_driver_state_t LPI2C6_InterruptDriverState = {
#endif
    &LPI2C6_Resource,
    &LPI2C6_Handle,
};

static void KSDK_LPI2C6_SLAVE_InterruptCallback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *userData)
{
    KSDK_LPI2C_SLAVE_InterruptCallback(base, xfer, userData, &LPI2C6_InterruptDriverState);
}

static int32_t LPI2C6_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    LPI2C6_InitPins();
    return LPI2C_InterruptInitialize(cb_event, &LPI2C6_InterruptDriverState);
}

static int32_t LPI2C6_InterruptUninitialize(void)
{
    LPI2C6_DeinitPins();
    return LPI2C_InterruptUninitialize(&LPI2C6_InterruptDriverState);
}

static int32_t LPI2C6_InterruptPowerControl(ARM_POWER_STATE state)
{
    return LPI2C_InterruptPowerControl(state, &LPI2C6_InterruptDriverState);
}

int32_t LPI2C6_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &LPI2C6_InterruptDriverState);
}

int32_t LPI2C6_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return LPI2C_Master_InterruptReceive(addr, data, num, xfer_pending, &LPI2C6_InterruptDriverState);
}

int32_t LPI2C6_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    LPI2C6_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C6_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptTransmit(data, num, &LPI2C6_InterruptDriverState);
}

int32_t LPI2C6_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    LPI2C6_InterruptDriverState.handle->slave_handle.callback = KSDK_LPI2C6_SLAVE_InterruptCallback;
    return LPI2C_Slave_InterruptReceive(data, num, &LPI2C6_InterruptDriverState);
}

int32_t LPI2C6_InterruptGetDataCount(void)
{
    return LPI2C_InterruptGetDataCount(&LPI2C6_InterruptDriverState);
}

int32_t LPI2C6_InterruptControl(uint32_t control, uint32_t arg)
{
    return LPI2C_InterruptControl(control, arg, &LPI2C6_InterruptDriverState);
}

ARM_I2C_STATUS LPI2C6_InterruptGetStatus(void)
{
    return LPI2C_InterruptGetStatus(&LPI2C6_InterruptDriverState);
}

#endif /* RTE_I2C6_DMA_EN */

ARM_DRIVER_I2C Driver_I2C6 = {LPI2Cx_GetVersion,
                              LPI2Cx_GetCapabilities,
#if RTE_I2C6_DMA_EN
                              LPI2C6_Master_EdmaInitialize,
                              LPI2C6_Master_EdmaUninitialize,
                              LPI2C6_Master_EdmaPowerControl,
                              LPI2C6_Master_EdmaTransmit,
                              LPI2C6_Master_EdmaReceive,
                              NULL,
                              NULL,
                              LPI2C6_Master_EdmaGetDataCount,
                              LPI2C6_Master_EdmaControl,
                              LPI2C6_Master_EdmaGetStatus
#else
                              LPI2C6_InterruptInitialize,
                              LPI2C6_InterruptUninitialize,
                              LPI2C6_InterruptPowerControl,
                              LPI2C6_Master_InterruptTransmit,
                              LPI2C6_Master_InterruptReceive,
                              LPI2C6_Slave_InterruptTransmit,
                              LPI2C6_Slave_InterruptReceive,
                              LPI2C6_InterruptGetDataCount,
                              LPI2C6_InterruptControl,
                              LPI2C6_InterruptGetStatus
#endif /* RTE_I2C6_DMA_EN */
};

#endif /* LPI2C6 */
