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

#include "fsl_i2c_cmsis.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpc_i2c_cmsis"
#endif

#if (RTE_I2C0 || RTE_I2C1 || RTE_I2C2)

#define ARM_I2C_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

typedef const struct _cmsis_i2c_resource
{
    I2C_Type *base;            /*!< I2C peripheral base address.      */
    uint32_t (*GetFreq)(void); /*!< Function to get the clock frequency. */

} cmsis_i2c_resource_t;

typedef union _cmsis_i2c_handle
{
    i2c_master_handle_t master_handle; /*!< master Interupt transfer handle. */
    i2c_slave_handle_t slave_handle;   /*!< slave Interupt transfer handle. */
} cmsis_i2c_handle_t;

typedef struct _cmsis_i2c_interrupt_driver_state
{
    cmsis_i2c_resource_t *resource; /*!< Basic I2C resource. */
    cmsis_i2c_handle_t *handle;
    ARM_I2C_SignalEvent_t cb_event; /*!< Callback function.     */
    uint8_t flags;                  /*!< Control and state flags. */
} cmsis_i2c_interrupt_driver_state_t;
#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
typedef const struct _cmsis_i2c_dma_resource
{
    DMA_Type *i2cDmaBase;   /*!< DMA peripheral base address for i2c.    */
    uint32_t i2cDmaChannel; /*!< DMA channel for i2c.             */
} cmsis_i2c_dma_resource_t;

typedef struct _cmsis_i2c_dma_driver_state
{
    cmsis_i2c_resource_t *resource;             /*!< i2c basic resource.      */
    cmsis_i2c_dma_resource_t *dmaResource;      /*!< i2c DMA resource.        */
    i2c_master_dma_handle_t *master_dma_handle; /*!< i2c DMA transfer handle. */
    dma_handle_t *dmaHandle;                    /*!< DMA i2c handle.          */
    uint8_t flags;                              /*!< Control and state flags. */
} cmsis_i2c_dma_driver_state_t;
#endif

static const ARM_DRIVER_VERSION s_i2cDriverVersion = {ARM_I2C_API_VERSION, ARM_I2C_DRV_VERSION};

static const ARM_I2C_CAPABILITIES s_i2cDriverCapabilities = {
    0, /*< supports 10-bit addressing*/
};

static const clock_ip_name_t s_i2cClocks[] = I2C_CLOCKS;
extern uint32_t I2C_GetInstance(I2C_Type *base);

static ARM_DRIVER_VERSION I2Cx_GetVersion(void)
{
    return s_i2cDriverVersion;
}

static ARM_I2C_CAPABILITIES I2Cx_GetCapabilities(void)
{
    return s_i2cDriverCapabilities;
}

#endif

#if (RTE_I2C0_DMA_EN || RTE_I2C1_DMA_EN || RTE_I2C2_DMA_EN)

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
void KSDK_I2C_MASTER_DmaCallback(I2C_Type *base, i2c_master_dma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event;

    if (status == kStatus_Success) /* Occurs after Master Transmit/Receive operation has finished. */
    {
        event = ARM_I2C_EVENT_TRANSFER_DONE;
    }

    if (userData)
    {
        ((ARM_I2C_SignalEvent_t)userData)(event);
    }
}

static int32_t I2C_Master_DmaInitialize(ARM_I2C_SignalEvent_t cb_event, cmsis_i2c_dma_driver_state_t *i2c)
{
    if (!(i2c->flags & I2C_FLAG_INIT))
    {
        DMA_EnableChannel(i2c->dmaResource->i2cDmaBase, i2c->dmaResource->i2cDmaChannel);
        DMA_CreateHandle(i2c->dmaHandle, i2c->dmaResource->i2cDmaBase, i2c->dmaResource->i2cDmaChannel);

        I2C_MasterTransferCreateHandleDMA(i2c->resource->base, i2c->master_dma_handle, KSDK_I2C_MASTER_DmaCallback,
                                          (void *)cb_event, i2c->dmaHandle);
        i2c->flags = I2C_FLAG_INIT;
    }
    return ARM_DRIVER_OK;
}

int32_t I2C_Master_DmaUninitialize(cmsis_i2c_dma_driver_state_t *i2c)
{
    i2c->flags = I2C_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

int32_t I2C_Master_DmaTransmit(
    uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending, cmsis_i2c_dma_driver_state_t *i2c)
{
    int32_t status;
    int32_t ret;
    i2c_master_transfer_t masterXfer;

    /* Check if the I2C bus is idle - if not return busy status. */
    if (i2c->master_dma_handle->state != 0)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    masterXfer.slaveAddress   = addr;                     /*7-bit slave address.*/
    masterXfer.direction      = kI2C_Write;               /*Transfer direction.*/
    masterXfer.subaddress     = 0;                        /*Sub address*/
    masterXfer.subaddressSize = 0;                        /*Size of command buffer.*/
    masterXfer.data           = (uint8_t *)data;          /*Transfer buffer.*/
    masterXfer.dataSize       = num;                      /*Transfer size.*/
    masterXfer.flags          = kI2C_TransferDefaultFlag; /*Transfer flag which controls the transfer.*/

    if (xfer_pending)
    {
        masterXfer.flags |= kI2C_TransferNoStopFlag;
    }

    status = I2C_MasterTransferDMA(i2c->resource->base, i2c->master_dma_handle, &masterXfer);
    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_I2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t I2C_Master_DmaReceive(
    uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending, cmsis_i2c_dma_driver_state_t *i2c)
{
    int32_t status;
    int32_t ret;
    i2c_master_transfer_t masterXfer;

    /* Check if the I2C bus is idle - if not return busy status. */
    if (i2c->master_dma_handle->state != 0)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    masterXfer.slaveAddress   = addr;                     /*7-bit slave address.*/
    masterXfer.direction      = kI2C_Read;                /*Transfer direction.*/
    masterXfer.subaddress     = 0;                        /*Sub address*/
    masterXfer.subaddressSize = 0;                        /*Size of command buffer.*/
    masterXfer.data           = data;                     /*Transfer buffer.*/
    masterXfer.dataSize       = num;                      /*Transfer size.*/
    masterXfer.flags          = kI2C_TransferDefaultFlag; /*Transfer flag which controls the transfer.*/

    if (xfer_pending)
    {
        masterXfer.flags |= kI2C_TransferNoStopFlag;
    }

    status = I2C_MasterTransferDMA(i2c->resource->base, i2c->master_dma_handle, &masterXfer);
    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_I2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t I2C_Master_DmaGetDataCount(cmsis_i2c_dma_driver_state_t *i2c)
{
    size_t cnt; /*the number of currently transferred data bytes*/

    I2C_MasterTransferGetCountDMA(i2c->resource->base, i2c->master_dma_handle, &cnt);
    return cnt;
}

int32_t I2C_Master_DmaControl(uint32_t control, uint32_t arg, cmsis_i2c_dma_driver_state_t *i2c)
{
    uint32_t baudRate_Bps;

    switch (control)
    {
        /* Not supported */
        case ARM_I2C_OWN_ADDRESS:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        /*Set Bus Speed; arg = bus speed*/
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
            I2C_MasterSetBaudRate(i2c->resource->base, baudRate_Bps, i2c->resource->GetFreq());
            return ARM_DRIVER_OK;
        /* Not supported */
        case ARM_I2C_BUS_CLEAR:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        /*Aborts the data transfer  when Master for Transmit or Receive*/
        case ARM_I2C_ABORT_TRANSFER:
            /*disable dma*/
            I2C_MasterTransferAbortDMA(i2c->resource->base, i2c->master_dma_handle);

            i2c->master_dma_handle->transferCount     = 0;
            i2c->master_dma_handle->transfer.data     = NULL;
            i2c->master_dma_handle->transfer.dataSize = 0;
            return ARM_DRIVER_OK;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

int32_t I2C_Master_DmaPowerControl(ARM_POWER_STATE state, cmsis_i2c_dma_driver_state_t *i2c)
{
    switch (state)
    {
        /*terminates any pending data transfers, disable i2c moduole and i2c clock and related dma*/
        case ARM_POWER_OFF:
            if (i2c->flags & I2C_FLAG_POWER)
            {
                I2C_Master_DmaControl(ARM_I2C_ABORT_TRANSFER, 0, i2c);
                I2C_MasterDeinit(i2c->resource->base);
                DMA_DisableChannel(i2c->dmaResource->i2cDmaBase, i2c->dmaResource->i2cDmaChannel);
                i2c->flags = I2C_FLAG_INIT;
            }

            return ARM_DRIVER_OK;
        /* Not supported */
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        /*enable i2c moduole and i2c clock*/
        case ARM_POWER_FULL:
            if (i2c->flags == I2C_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (i2c->flags & I2C_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }
            CLOCK_EnableClock(s_i2cClocks[I2C_GetInstance(i2c->resource->base)]);
            I2C_MasterEnable(i2c->resource->base, true);
            i2c->flags |= I2C_FLAG_POWER;

            return ARM_DRIVER_OK;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

ARM_I2C_STATUS I2C_Master_DmaGetStatus(cmsis_i2c_dma_driver_state_t *i2c)
{
    ARM_I2C_STATUS stat;
    uint32_t ksdk_i2c_status = I2C_GetStatusFlags(i2c->resource->base);

    stat.busy             = !(ksdk_i2c_status & I2C_STAT_MSTPENDING_MASK); /*Busy flag.*/
    stat.direction        = i2c->master_dma_handle->transfer.direction;    /*Direction: 0=Transmitter, 1=Receiver.*/
    stat.mode             = 1;                                             /*Mode: 0=Slave, 1=Master.*/
    stat.arbitration_lost = !(!(ksdk_i2c_status & I2C_STAT_MSTARBLOSS_MASK));
    /*Master lost arbitration (cleared on start of next Master operation)*/

    return stat;
}
#endif

#endif

#if ((RTE_I2C0 && !RTE_I2C0_DMA_EN) || (RTE_I2C1 && !RTE_I2C1_DMA_EN) || (RTE_I2C2 && !RTE_I2C2_DMA_EN) || \
     (RTE_I2C3 && !RTE_I2C3_DMA_EN))

static void KSDK_I2C_SLAVE_InterruptCallback(I2C_Type *base, volatile i2c_slave_transfer_t *xfer, void *param)
{
    uint32_t event;

    switch (xfer->event)
    {
        case kI2C_SlaveCompletionEvent: /* Occurs after Slave Transmit/Receive operation has finished. */
            event = ARM_I2C_EVENT_TRANSFER_DONE;
            break;
        default:
            event = ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
            break;
    }

    if (param)
    {
        ((ARM_I2C_SignalEvent_t)param)(event);
    }
}

static void KSDK_I2C_MASTER_InterruptCallback(I2C_Type *base,
                                              i2c_master_handle_t *handle,
                                              status_t status,
                                              void *userData)
{
    uint32_t event;

    switch (status)
    {
        case kStatus_Success: /* Occurs after Master Transmit/Receive operation has finished. */
            event = ARM_I2C_EVENT_TRANSFER_DONE;
            break;
        case kStatus_I2C_ArbitrationLost: /*Occurs in master mode when arbitration is lost.*/
            event = ARM_I2C_EVENT_ARBITRATION_LOST;
            break;
        default:
            event = ARM_I2C_EVENT_TRANSFER_INCOMPLETE;
            break;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_I2C_SignalEvent_t)userData)(event);
    }
}

static int32_t I2C_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event, cmsis_i2c_interrupt_driver_state_t *i2c)
{
    if (!(i2c->flags & I2C_FLAG_INIT))
    {
        i2c->cb_event = cb_event; /* cb_event is CMSIS driver callback. */
        i2c->flags    = I2C_FLAG_INIT;
    }

    return ARM_DRIVER_OK;
}

static int32_t I2C_InterruptUninitialize(cmsis_i2c_interrupt_driver_state_t *i2c)
{
    i2c->flags = I2C_FLAG_UNINIT;
    return ARM_DRIVER_OK;
}

int32_t I2C_Master_InterruptTransmit(
    uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending, cmsis_i2c_interrupt_driver_state_t *i2c)
{
    int32_t status;
    int32_t ret;
    i2c_master_transfer_t masterXfer;

    /* Check if the I2C bus is idle - if not return busy status. */
    if (i2c->handle->master_handle.state != 0)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    I2C_MasterEnable(i2c->resource->base, true);

    /*create master_handle*/
    I2C_MasterTransferCreateHandle(i2c->resource->base, &(i2c->handle->master_handle),
                                   KSDK_I2C_MASTER_InterruptCallback, (void *)i2c->cb_event);

    masterXfer.slaveAddress   = addr;                     /*7-bit slave address.*/
    masterXfer.direction      = kI2C_Write;               /*Transfer direction.*/
    masterXfer.subaddress     = (uint32_t)NULL;           /*Sub address*/
    masterXfer.subaddressSize = 0;                        /*Size of command buffer.*/
    masterXfer.data           = (uint8_t *)data;          /*Transfer buffer.*/
    masterXfer.dataSize       = num;                      /*Transfer size.*/
    masterXfer.flags          = kI2C_TransferDefaultFlag; /*Transfer flag which controls the transfer.*/

    if (xfer_pending)
    {
        masterXfer.flags |= kI2C_TransferNoStopFlag;
    }

    status = I2C_MasterTransferNonBlocking(i2c->resource->base, &(i2c->handle->master_handle), &masterXfer);
    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_I2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t I2C_Master_InterruptReceive(
    uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending, cmsis_i2c_interrupt_driver_state_t *i2c)
{
    int32_t status;
    int32_t ret;
    i2c_master_transfer_t masterXfer;

    /* Check if the I2C bus is idle - if not return busy status. */
    if (i2c->handle->master_handle.state != 0)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    I2C_MasterEnable(i2c->resource->base, true);

    /*create master_handle*/
    I2C_MasterTransferCreateHandle(i2c->resource->base, &(i2c->handle->master_handle),
                                   KSDK_I2C_MASTER_InterruptCallback, (void *)i2c->cb_event);

    masterXfer.slaveAddress   = addr;                     /*7-bit slave address.*/
    masterXfer.direction      = kI2C_Read;                /*Transfer direction.*/
    masterXfer.subaddress     = (uint32_t)NULL;           /*Sub address*/
    masterXfer.subaddressSize = 0;                        /*Size of command buffer.*/
    masterXfer.data           = data;                     /*Transfer buffer.*/
    masterXfer.dataSize       = num;                      /*Transfer size.*/
    masterXfer.flags          = kI2C_TransferDefaultFlag; /*Transfer flag which controls the transfer.*/

    if (xfer_pending)
    {
        masterXfer.flags |= kI2C_TransferNoStopFlag;
    }

    status = I2C_MasterTransferNonBlocking(i2c->resource->base, &(i2c->handle->master_handle), &masterXfer);
    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_I2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t I2C_Slave_InterruptTransmit(const uint8_t *data, uint32_t num, cmsis_i2c_interrupt_driver_state_t *i2c)
{
    int32_t status;
    int32_t ret;

    /* set Slave enable */
    I2C_SlaveEnable(i2c->resource->base, true);

    /*create slave_handle*/
    I2C_SlaveTransferCreateHandle(i2c->resource->base, &(i2c->handle->slave_handle), KSDK_I2C_SLAVE_InterruptCallback,
                                  (void *)i2c->cb_event);

    status = I2C_SlaveTransferNonBlocking(i2c->resource->base, &(i2c->handle->slave_handle), kI2C_SlaveCompletionEvent);

    i2c->handle->slave_handle.transfer.txData =
        (uint8_t *)data;                             /*Pointer to buffer with data to transmit to I2C Master*/
    i2c->handle->slave_handle.transfer.txSize = num; /*Number of data bytes to transmit*/
    i2c->handle->slave_handle.transfer.transferredCount =
        0; /*Number of bytes actually transferred since start or last repeated start. */

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_I2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t I2C_Slave_InterruptReceive(uint8_t *data, uint32_t num, cmsis_i2c_interrupt_driver_state_t *i2c)
{
    int32_t status;
    int32_t ret;

    /* set Slave enable */
    I2C_SlaveEnable(i2c->resource->base, true);

    /*create slave_handle*/
    I2C_SlaveTransferCreateHandle(i2c->resource->base, &(i2c->handle->slave_handle), KSDK_I2C_SLAVE_InterruptCallback,
                                  (void *)i2c->cb_event);

    status = I2C_SlaveTransferNonBlocking(i2c->resource->base, &(i2c->handle->slave_handle), kI2C_SlaveCompletionEvent);

    i2c->handle->slave_handle.transfer.rxData = data; /*Pointer to buffer with data to transmit to I2C Master*/
    i2c->handle->slave_handle.transfer.rxSize = num;  /*Number of data bytes to transmit*/
    i2c->handle->slave_handle.transfer.transferredCount =
        0; /*Number of bytes actually transferred since start or last repeated start. */

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_I2C_Busy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

int32_t I2C_InterruptGetDataCount(cmsis_i2c_interrupt_driver_state_t *i2c)
{
    uint32_t cnt = 0; /*the number of currently transferred data bytes*/

    if (i2c->resource->base->CFG & I2C_CFG_MSTEN_MASK)
    {
        cnt = i2c->handle->master_handle.transferCount;
    }
    else
    {
        cnt = i2c->handle->slave_handle.transfer.transferredCount;
    }

    return cnt;
}

int32_t I2C_InterruptControl(uint32_t control, uint32_t arg, cmsis_i2c_interrupt_driver_state_t *i2c)
{
    uint32_t baudRate_Bps;
    uint32_t clkDiv;

    switch (control)
    {
        /*Set Own Slave Address; arg = slave address*/
        case ARM_I2C_OWN_ADDRESS:
            /* Use as slave, set CLKDIV for clock stretching, ensure data set up time for standard mode 250ns. */
            /* divVal = (sourceClock_Hz / 1000000) * (dataSetupTime_ns / 1000) */
            clkDiv                      = i2c->resource->GetFreq() / 1000U;
            clkDiv                      = (clkDiv * 250) / 1000000U;
            i2c->resource->base->CLKDIV = clkDiv & I2C_CLKDIV_DIVVAL_MASK;

            /* Set slave address. */
            I2C_SlaveSetAddress(i2c->resource->base, kI2C_SlaveAddressRegister0, arg, false);
            /* set Slave address 0 qual */
            i2c->resource->base->SLVQUAL0 = I2C_SLVQUAL0_QUALMODE0(0) | I2C_SLVQUAL0_SLVQUAL0(0);
            return ARM_DRIVER_OK;
        /*Set Bus Speed; arg = bus speed*/
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
            I2C_MasterSetBaudRate(i2c->resource->base, baudRate_Bps, i2c->resource->GetFreq());
            return ARM_DRIVER_OK;
        // Not supported
        case ARM_I2C_BUS_CLEAR:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        /*Aborts the data transfer between Master and Slave for Transmit or Receive*/
        case ARM_I2C_ABORT_TRANSFER:
            if (i2c->resource->base->CFG & I2C_CFG_MSTEN_MASK)
            {
                /*disable master interrupt and send STOP signal*/
                I2C_MasterTransferAbort(i2c->resource->base, &(i2c->handle->master_handle));

                i2c->handle->master_handle.transferCount     = 0;
                i2c->handle->master_handle.transfer.data     = NULL;
                i2c->handle->master_handle.transfer.dataSize = 0;
            }
            /*if slave receive*/
            if ((i2c->resource->base->CFG & I2C_CFG_SLVEN_MASK) &&
                ((i2c->handle->slave_handle.slaveFsm) == kI2C_SlaveFsmReceive))
            {
                /*disable slave interrupt*/
                I2C_SlaveTransferAbort(i2c->resource->base, &(i2c->handle->slave_handle));

                i2c->handle->slave_handle.transfer.transferredCount = 0;
                i2c->handle->slave_handle.transfer.txData           = NULL;
                i2c->handle->slave_handle.transfer.rxData           = NULL;
            }
            return ARM_DRIVER_OK;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t I2C_InterruptPowerControl(ARM_POWER_STATE state, cmsis_i2c_interrupt_driver_state_t *i2c)
{
    switch (state)
    {
        /*terminates any pending data transfers, disable i2c moduole and i2c clock*/
        case ARM_POWER_OFF:
            if (i2c->flags & I2C_FLAG_POWER)
            {
                I2C_InterruptControl(ARM_I2C_ABORT_TRANSFER, 0, i2c);

                I2C_MasterDeinit(i2c->resource->base);
                I2C_SlaveDeinit(i2c->resource->base);
                i2c->flags = I2C_FLAG_INIT;
            }

            return ARM_DRIVER_OK;

        /* Not supported */
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        /*enable i2c moduole and i2c clock*/
        case ARM_POWER_FULL:
            if (i2c->flags == I2C_FLAG_UNINIT)
            {
                return ARM_DRIVER_ERROR;
            }

            if (i2c->flags & I2C_FLAG_POWER)
            {
                /* Driver already powered */
                break;
            }
            CLOCK_EnableClock(s_i2cClocks[I2C_GetInstance(i2c->resource->base)]);
            I2C_MasterEnable(i2c->resource->base, true);
            i2c->flags |= I2C_FLAG_POWER;

            return ARM_DRIVER_OK;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

ARM_I2C_STATUS I2C_InterruptGetStatus(cmsis_i2c_interrupt_driver_state_t *i2c)
{
    ARM_I2C_STATUS stat;
    uint32_t ksdk_i2c_status = I2C_GetStatusFlags(i2c->resource->base);

    if (i2c->resource->base->CFG & I2C_CFG_MSTEN_MASK)
    {
        stat.busy             = !(ksdk_i2c_status & I2C_STAT_MSTPENDING_MASK); /*Busy flag.*/
        stat.direction        = i2c->handle->master_handle.transfer.direction; /*Direction: 0=Transmitter, 1=Receiver.*/
        stat.mode             = 1;                                             /*Mode: 0=Slave, 1=Master.*/
        stat.arbitration_lost = !(!(ksdk_i2c_status & I2C_STAT_MSTARBLOSS_MASK));
        /*Master lost arbitration (cleared on start of next Master operation)*/
    }

    if (i2c->resource->base->CFG & I2C_CFG_SLVEN_MASK)
    {
        stat.busy = !(ksdk_i2c_status & I2C_STAT_SLVPENDING_MASK); /*Busy flag.*/
        if ((i2c->handle->slave_handle.slaveFsm) == kI2C_SlaveFsmReceive)
        {
            stat.direction = 1; /*Direction: 0=Transmitter, 1=Receiver.*/
        }
        else
        {
            stat.direction = 0; /*Direction: 0=Transmitter, 1=Receiver.*/
        }
        stat.mode = 0; /*Mode: 0=Slave, 1=Master.*/
    }

    return stat;
}

#endif

#if defined(I2C0) && RTE_I2C0
/* User needs to provide the implementation for I2C0_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t I2C0_GetFreq(void);
extern void I2C0_InitPins(void);
extern void I2C0_DeinitPins(void);

cmsis_i2c_resource_t I2C0_Resource = {I2C0, I2C0_GetFreq};

#if RTE_I2C0_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_i2c_dma_resource_t I2C0_DmaResource = {RTE_I2C0_Master_DMA_BASE, RTE_I2C0_Master_DMA_CH};

i2c_master_dma_handle_t I2C0_DmaHandle;
dma_handle_t I2C0_DmaTxRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("i2c0_dma_driver_state")
cmsis_i2c_dma_driver_state_t I2C0_DmaDriverState = {
#else
cmsis_i2c_dma_driver_state_t I2C0_DmaDriverState = {
#endif
    &I2C0_Resource,
    &I2C0_DmaResource,
    &I2C0_DmaHandle,
    &I2C0_DmaTxRxHandle,
};

static int32_t I2C0_Master_DmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    I2C0_InitPins();
    return I2C_Master_DmaInitialize(cb_event, &I2C0_DmaDriverState);
}

int32_t I2C0_Master_DmaUninitialize(void)
{
    I2C0_DeinitPins();
    return I2C_Master_DmaUninitialize(&I2C0_DmaDriverState);
}

int32_t I2C0_Master_DmaPowerControl(ARM_POWER_STATE state)
{
    return I2C_Master_DmaPowerControl(state, &I2C0_DmaDriverState);
}

int32_t I2C0_Master_DmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_DmaTransmit(addr, data, num, xfer_pending, &I2C0_DmaDriverState);
}

int32_t I2C0_Master_DmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_DmaReceive(addr, data, num, xfer_pending, &I2C0_DmaDriverState);
}

int32_t I2C0_Master_DmaGetDataCount(void)
{
    return I2C_Master_DmaGetDataCount(&I2C0_DmaDriverState);
}

int32_t I2C0_Master_DmaControl(uint32_t control, uint32_t arg)
{
    return I2C_Master_DmaControl(control, arg, &I2C0_DmaDriverState);
}

ARM_I2C_STATUS I2C0_Master_DmaGetStatus(void)
{
    return I2C_Master_DmaGetStatus(&I2C0_DmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t I2C0_handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("i2c0_interrupt_driver_state")
cmsis_i2c_interrupt_driver_state_t I2C0_InterruptDriverState = {
#else
cmsis_i2c_interrupt_driver_state_t I2C0_InterruptDriverState = {
#endif
    &I2C0_Resource,
    &I2C0_handle,

};

static int32_t I2C0_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    I2C0_InitPins();
    return I2C_InterruptInitialize(cb_event, &I2C0_InterruptDriverState);
}

static int32_t I2C0_InterruptUninitialize(void)
{
    I2C0_DeinitPins();
    return I2C_InterruptUninitialize(&I2C0_InterruptDriverState);
}

static int32_t I2C0_InterruptPowerControl(ARM_POWER_STATE state)
{
    return I2C_InterruptPowerControl(state, &I2C0_InterruptDriverState);
}

int32_t I2C0_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &I2C0_InterruptDriverState);
}

int32_t I2C0_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_InterruptReceive(addr, data, num, xfer_pending, &I2C0_InterruptDriverState);
}

int32_t I2C0_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    return I2C_Slave_InterruptTransmit(data, num, &I2C0_InterruptDriverState);
}

int32_t I2C0_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    return I2C_Slave_InterruptReceive(data, num, &I2C0_InterruptDriverState);
}

int32_t I2C0_InterruptGetDataCount(void)
{
    return I2C_InterruptGetDataCount(&I2C0_InterruptDriverState);
}

int32_t I2C0_InterruptControl(uint32_t control, uint32_t arg)
{
    return I2C_InterruptControl(control, arg, &I2C0_InterruptDriverState);
}

ARM_I2C_STATUS I2C0_InterruptGetStatus(void)
{
    return I2C_InterruptGetStatus(&I2C0_InterruptDriverState);
}

#endif

ARM_DRIVER_I2C Driver_I2C0 = {I2Cx_GetVersion,
                              I2Cx_GetCapabilities,
#if RTE_I2C0_DMA_EN
                              I2C0_Master_DmaInitialize,
                              I2C0_Master_DmaUninitialize,
                              I2C0_Master_DmaPowerControl,
                              I2C0_Master_DmaTransmit,
                              I2C0_Master_DmaReceive,
                              NULL,
                              NULL,
                              I2C0_Master_DmaGetDataCount,
                              I2C0_Master_DmaControl,
                              I2C0_Master_DmaGetStatus
#else
                              I2C0_InterruptInitialize,
                              I2C0_InterruptUninitialize,
                              I2C0_InterruptPowerControl,
                              I2C0_Master_InterruptTransmit,
                              I2C0_Master_InterruptReceive,
                              I2C0_Slave_InterruptTransmit,
                              I2C0_Slave_InterruptReceive,
                              I2C0_InterruptGetDataCount,
                              I2C0_InterruptControl,
                              I2C0_InterruptGetStatus
#endif
};

#endif

#if defined(I2C1) && RTE_I2C1

/* User needs to provide the implementation for I2C1_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t I2C1_GetFreq(void);
extern void I2C1_InitPins(void);
extern void I2C1_DeinitPins(void);

cmsis_i2c_resource_t I2C1_Resource = {I2C1, I2C1_GetFreq};

#if RTE_I2C1_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_i2c_dma_resource_t I2C1_DmaResource = {RTE_I2C1_Master_DMA_BASE, RTE_I2C1_Master_DMA_CH};

i2c_master_dma_handle_t I2C1_DmaHandle;
dma_handle_t I2C1_DmaTxRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("i2c1_dma_driver_state")
cmsis_i2c_dma_driver_state_t I2C1_DmaDriverState = {
#else
cmsis_i2c_dma_driver_state_t I2C1_DmaDriverState = {
#endif
    &I2C1_Resource,
    &I2C1_DmaResource,
    &I2C1_DmaHandle,
    &I2C1_DmaTxRxHandle,
};

static int32_t I2C1_Master_DmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    I2C1_InitPins();
    return I2C_Master_DmaInitialize(cb_event, &I2C1_DmaDriverState);
}

int32_t I2C1_Master_DmaUninitialize(void)
{
    I2C1_DeinitPins();
    return I2C_Master_DmaUninitialize(&I2C1_DmaDriverState);
}

int32_t I2C1_Master_DmaPowerControl(ARM_POWER_STATE state)
{
    return I2C_Master_DmaPowerControl(state, &I2C1_DmaDriverState);
}

int32_t I2C1_Master_DmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_DmaTransmit(addr, data, num, xfer_pending, &I2C1_DmaDriverState);
}

int32_t I2C1_Master_DmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_DmaReceive(addr, data, num, xfer_pending, &I2C1_DmaDriverState);
}

int32_t I2C1_Master_DmaGetDataCount(void)
{
    return I2C_Master_DmaGetDataCount(&I2C1_DmaDriverState);
}

int32_t I2C1_Master_DmaControl(uint32_t control, uint32_t arg)
{
    return I2C_Master_DmaControl(control, arg, &I2C1_DmaDriverState);
}

ARM_I2C_STATUS I2C1_Master_DmaGetStatus(void)
{
    return I2C_Master_DmaGetStatus(&I2C1_DmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t I2C1_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("i2c1_interrupt_driver_state")
cmsis_i2c_interrupt_driver_state_t I2C1_InterruptDriverState = {
#else
cmsis_i2c_interrupt_driver_state_t I2C1_InterruptDriverState = {
#endif
    &I2C1_Resource,
    &I2C1_Handle,
};

static int32_t I2C1_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    I2C1_InitPins();
    return I2C_InterruptInitialize(cb_event, &I2C1_InterruptDriverState);
}

static int32_t I2C1_InterruptUninitialize(void)
{
    I2C1_DeinitPins();
    return I2C_InterruptUninitialize(&I2C1_InterruptDriverState);
}

static int32_t I2C1_InterruptPowerControl(ARM_POWER_STATE state)
{
    return I2C_InterruptPowerControl(state, &I2C1_InterruptDriverState);
}

int32_t I2C1_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &I2C1_InterruptDriverState);
}

int32_t I2C1_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_InterruptReceive(addr, data, num, xfer_pending, &I2C1_InterruptDriverState);
}

int32_t I2C1_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    return I2C_Slave_InterruptTransmit(data, num, &I2C1_InterruptDriverState);
}

int32_t I2C1_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    return I2C_Slave_InterruptReceive(data, num, &I2C1_InterruptDriverState);
}

int32_t I2C1_InterruptGetDataCount(void)
{
    return I2C_InterruptGetDataCount(&I2C1_InterruptDriverState);
}

int32_t I2C1_InterruptControl(uint32_t control, uint32_t arg)
{
    return I2C_InterruptControl(control, arg, &I2C1_InterruptDriverState);
}

ARM_I2C_STATUS I2C1_InterruptGetStatus(void)
{
    return I2C_InterruptGetStatus(&I2C1_InterruptDriverState);
}

#endif

ARM_DRIVER_I2C Driver_I2C1 = {I2Cx_GetVersion,
                              I2Cx_GetCapabilities,
#if RTE_I2C1_DMA_EN
                              I2C1_Master_DmaInitialize,
                              I2C1_Master_DmaUninitialize,
                              I2C1_Master_DmaPowerControl,
                              I2C1_Master_DmaTransmit,
                              I2C1_Master_DmaReceive,
                              NULL,
                              NULL,
                              I2C1_Master_DmaGetDataCount,
                              I2C1_Master_DmaControl,
                              I2C1_Master_DmaGetStatus
#else
                              I2C1_InterruptInitialize,
                              I2C1_InterruptUninitialize,
                              I2C1_InterruptPowerControl,
                              I2C1_Master_InterruptTransmit,
                              I2C1_Master_InterruptReceive,
                              I2C1_Slave_InterruptTransmit,
                              I2C1_Slave_InterruptReceive,
                              I2C1_InterruptGetDataCount,
                              I2C1_InterruptControl,
                              I2C1_InterruptGetStatus
#endif
};

#endif

#if defined(I2C2) && RTE_I2C2

/* User needs to provide the implementation for I2C2_GetFreq/InitPins/DeinitPins
in the application for enabling according instance. */
extern uint32_t I2C2_GetFreq(void);
extern void I2C2_InitPins(void);
extern void I2C2_DeinitPins(void);

cmsis_i2c_resource_t I2C2_Resource = {I2C2, I2C2_GetFreq};

#if RTE_I2C2_DMA_EN

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_i2c_dma_resource_t I2C2_DmaResource = {RTE_I2C2_Master_DMA_BASE, RTE_I2C2_Master_DMA_CH};

i2c_master_dma_handle_t I2C2_DmaHandle;
dma_handle_t I2C2_DmaTxRxHandle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("i2c2_dma_driver_state")
cmsis_i2c_dma_driver_state_t I2C2_DmaDriverState = {
#else
cmsis_i2c_dma_driver_state_t I2C2_DmaDriverState = {
#endif
    &I2C2_Resource,
    &I2C2_DmaResource,
    &I2C2_DmaHandle,
    &I2C2_DmaTxRxHandle,
};

static int32_t I2C2_Master_DmaInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    I2C2_InitPins();
    return I2C_Master_DmaInitialize(cb_event, &I2C2_DmaDriverState);
}

int32_t I2C2_Master_DmaUninitialize(void)
{
    I2C2_DeinitPins();
    return I2C_Master_DmaUninitialize(&I2C2_DmaDriverState);
}

int32_t I2C2_Master_DmaPowerControl(ARM_POWER_STATE state)
{
    return I2C_Master_DmaPowerControl(state, &I2C2_DmaDriverState);
}

int32_t I2C2_Master_DmaTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_DmaTransmit(addr, data, num, xfer_pending, &I2C2_DmaDriverState);
}

int32_t I2C2_Master_DmaReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_DmaReceive(addr, data, num, xfer_pending, &I2C2_DmaDriverState);
}

int32_t I2C2_Master_DmaGetDataCount(void)
{
    return I2C_Master_DmaGetDataCount(&I2C2_DmaDriverState);
}

int32_t I2C2_Master_DmaControl(uint32_t control, uint32_t arg)
{
    return I2C_Master_DmaControl(control, arg, &I2C2_DmaDriverState);
}

ARM_I2C_STATUS I2C2_Master_DmaGetStatus(void)
{
    return I2C_Master_DmaGetStatus(&I2C2_DmaDriverState);
}

#endif

#else

cmsis_i2c_handle_t I2C2_Handle;

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
ARMCC_SECTION("i2c2_interrupt_driver_state")
cmsis_i2c_interrupt_driver_state_t I2C2_InterruptDriverState = {
#else
cmsis_i2c_interrupt_driver_state_t I2C2_InterruptDriverState = {
#endif
    &I2C2_Resource,
    &I2C2_Handle,

};

static int32_t I2C2_InterruptInitialize(ARM_I2C_SignalEvent_t cb_event)
{
    I2C2_InitPins();
    return I2C_InterruptInitialize(cb_event, &I2C2_InterruptDriverState);
}

static int32_t I2C2_InterruptUninitialize(void)
{
    I2C2_DeinitPins();
    return I2C_InterruptUninitialize(&I2C2_InterruptDriverState);
}

static int32_t I2C2_InterruptPowerControl(ARM_POWER_STATE state)
{
    return I2C_InterruptPowerControl(state, &I2C2_InterruptDriverState);
}

int32_t I2C2_Master_InterruptTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_InterruptTransmit(addr, data, num, xfer_pending, &I2C2_InterruptDriverState);
}

int32_t I2C2_Master_InterruptReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
{
    return I2C_Master_InterruptReceive(addr, data, num, xfer_pending, &I2C2_InterruptDriverState);
}

int32_t I2C2_Slave_InterruptTransmit(const uint8_t *data, uint32_t num)
{
    return I2C_Slave_InterruptTransmit(data, num, &I2C2_InterruptDriverState);
}

int32_t I2C2_Slave_InterruptReceive(uint8_t *data, uint32_t num)
{
    return I2C_Slave_InterruptReceive(data, num, &I2C2_InterruptDriverState);
}

int32_t I2C2_InterruptGetDataCount(void)
{
    return I2C_InterruptGetDataCount(&I2C2_InterruptDriverState);
}

int32_t I2C2_InterruptControl(uint32_t control, uint32_t arg)
{
    return I2C_InterruptControl(control, arg, &I2C2_InterruptDriverState);
}

ARM_I2C_STATUS I2C2_InterruptGetStatus(void)
{
    return I2C_InterruptGetStatus(&I2C2_InterruptDriverState);
}

#endif

ARM_DRIVER_I2C Driver_I2C2 = {I2Cx_GetVersion,
                              I2Cx_GetCapabilities,
#if RTE_I2C2_DMA_EN
                              I2C2_Master_DmaInitialize,
                              I2C2_Master_DmaUninitialize,
                              I2C2_Master_DmaPowerControl,
                              I2C2_Master_DmaTransmit,
                              I2C2_Master_DmaReceive,
                              NULL,
                              NULL,
                              I2C2_Master_DmaGetDataCount,
                              I2C2_Master_DmaControl,
                              I2C2_Master_DmaGetStatus
#else
                              I2C2_InterruptInitialize,
                              I2C2_InterruptUninitialize,
                              I2C2_InterruptPowerControl,
                              I2C2_Master_InterruptTransmit,
                              I2C2_Master_InterruptReceive,
                              I2C2_Slave_InterruptTransmit,
                              I2C2_Slave_InterruptReceive,
                              I2C2_InterruptGetDataCount,
                              I2C2_InterruptControl,
                              I2C2_InterruptGetStatus
#endif
};

#endif
