/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_i3c.h"

#include "i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief i2c master state structure. */
typedef struct _hal_i2c_master
{
#if 0
    hal_i2c_master_transfer_callback_t callback;
    void *callbackParam;
#endif
    i3c_master_handle_t hardwareHandle;
    uint8_t instance;
} hal_i3c_master_t;

#if 0
/*! @brief i2c slave state structure. */
typedef struct _hal_i2c_slave
{
    hal_i2c_slave_transfer_callback_t callback;
    void *callbackParam;
    hal_i3c_slave_transfer_t transfer;
    i3c_slave_handle_t hardwareHandle;
    uint8_t instance;
} hal_i3c_slave_t;
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to i2c bases for each instance. */
static I3C_Type *const s_i3cBases[] = I3C_BASE_PTRS;

/*******************************************************************************
 * Code
 ******************************************************************************/

hal_i2c_status_t HAL_I2cGetStatus(status_t status)
{
    hal_i2c_status_t returnStatus;
    switch (status)
    {
        case kStatus_Success:
        {
            returnStatus = kStatus_HAL_I2cSuccess;
            break;
        }
        case kStatus_I3C_Busy:
        {
            returnStatus = kStatus_HAL_I2cBusy;
            break;
        }
        case kStatus_I3C_Idle:
        {
            returnStatus = kStatus_HAL_I2cIdle;
            break;
        }
        case kStatus_I3C_Nak:
        {
            returnStatus = kStatus_HAL_I2cNak;
            break;
        }
        case kStatus_I3C_Timeout:
        {
            returnStatus = kStatus_HAL_I2cTimeout;
            break;
        }
        default:
        {
            returnStatus = kStatus_HAL_I2cError;
            break;
        }
    }
    return returnStatus;
}

#if 0
static void HAL_I2cMasterCallback(I3C_Type *base, i3c_master_handle_t *handle, status_t status, void *callbackParam)
{
    hal_i3c_master_t *i3cMasterHandle;
    assert(callbackParam);

    i3cMasterHandle = (hal_i3c_master_t *)callbackParam;

    if (i3cMasterHandle->callback)
    {
        i3cMasterHandle->callback(i3cMasterHandle, HAL_I2cGetStatus(status), i3cMasterHandle->callbackParam);
    }
}

static void HAL_I2cSlaveCallback(I3C_Type *base, i3c_slave_transfer_t *xfer, void *callbackParam)
{
    hal_i3c_slave_t *i2cSlaveHandle;
    assert(callbackParam);

    i2cSlaveHandle = (hal_i3c_slave_t *)callbackParam;

    if (i2cSlaveHandle->callback)
    {
        i2cSlaveHandle->transfer.event            = (hal_i3c_slave_transfer_event_t)xfer->event;
        i2cSlaveHandle->transfer.data             = xfer->data;
        i2cSlaveHandle->transfer.dataSize         = xfer->dataSize;
        i2cSlaveHandle->transfer.completionStatus = HAL_I2cGetStatus(xfer->completionStatus);
        i2cSlaveHandle->transfer.transferredCount = xfer->transferredCount;
        i2cSlaveHandle->callback(i2cSlaveHandle, &i2cSlaveHandle->transfer, i2cSlaveHandle->callbackParam);
        xfer->data     = i2cSlaveHandle->transfer.data;
        xfer->dataSize = i2cSlaveHandle->transfer.dataSize;
    }
}
#endif

hal_i2c_status_t HAL_I2cMasterInit(hal_i2c_master_handle_t handle, const hal_i2c_master_config_t *config)
{
    hal_i3c_master_t *i3cMasterHandle;
    i3c_master_config_t i3cConfig;

    assert(handle);
    assert(config);

    if (HAL_I2C_MASTER_HANDLE_SIZE < sizeof(hal_i3c_master_t))
    {
        return kStatus_HAL_I2cError;
    }

    i3cMasterHandle = (hal_i3c_master_t *)handle;

    I3C_MasterGetDefaultConfig(&i3cConfig);
    i3cConfig.enableMaster    = (config->enableMaster > 0) ? kI3C_MasterOn : kI3C_MasterOff;
    i3cConfig.baudRate_Hz     = config->baudRate_Bps;
    i3cConfig.busType         = kI3C_TypeI2C;
    i3cMasterHandle->instance = config->instance;

    I3C_MasterInit(s_i3cBases[i3cMasterHandle->instance], &i3cConfig, config->srcClock_Hz);

    return kStatus_HAL_I2cSuccess;
}
hal_i2c_status_t HAL_I2cSlaveInit(hal_i2c_slave_handle_t handle, const hal_i2c_slave_config_t *config)
{
#if 0
    hal_i3c_slave_t *i2cSlaveHandle;
    i3c_slave_config_t i3cConfig;

    assert(handle);
    assert(config);

    if (HAL_I2C_SLAVE_HANDLE_SIZE < sizeof(hal_i3c_slave_t))
    {
        return kStatus_HAL_I2cError;
    }

    i2cSlaveHandle = (hal_i3c_slave_t *)handle;

    I3C_SlaveGetDefaultConfig(&i3cConfig);
    i3cConfig.enableSlave    = config->enableSlave;
    i3cConfig.address0       = config->slaveAddress;
    i2cSlaveHandle->instance = config->instance;

    I3C_SlaveInit(s_i3cBases[i2cSlaveHandle->instance], &i3cConfig, config->srcClock_Hz);

    return kStatus_HAL_I2cSuccess;
#else
    return kStatus_HAL_I2cError;
#endif
}

hal_i2c_status_t HAL_I2cMasterDeinit(hal_i2c_master_handle_t handle)
{
    hal_i3c_master_t *i3cMasterHandle;

    assert(handle);

    i3cMasterHandle = (hal_i3c_master_t *)handle;

    I3C_MasterDeinit(s_i3cBases[i3cMasterHandle->instance]);

    return kStatus_HAL_I2cSuccess;
}

hal_i2c_status_t HAL_I2cSlaveDeinit(hal_i2c_slave_handle_t handle)
{
#if 0
    hal_i3c_slave_t *i2cSlaveHandle;

    assert(handle);

    i2cSlaveHandle = (hal_i3c_slave_t *)handle;

    I3C_SlaveDeinit(s_i3cBases[i2cSlaveHandle->instance]);

    return kStatus_HAL_I2cSuccess;
#else
    return kStatus_HAL_I2cError;
#endif
}

hal_i2c_status_t HAL_I2cMasterWriteBlocking(hal_i2c_master_handle_t handle,
                                            const uint8_t *txBuff,
                                            size_t txSize,
                                            uint32_t flags)
{
    return kStatus_HAL_I2cError;
}

hal_i2c_status_t HAL_I2cMasterReadBlocking(hal_i2c_master_handle_t handle,
                                           uint8_t *rxBuff,
                                           size_t rxSize,
                                           uint32_t flags)
{
    return kStatus_HAL_I2cError;
}

hal_i2c_status_t HAL_I2cSlaveWriteBlocking(hal_i2c_slave_handle_t handle, const uint8_t *txBuff, size_t txSize)
{
    return kStatus_HAL_I2cError;
}

hal_i2c_status_t HAL_I2cSlaveReadBlocking(hal_i2c_slave_handle_t handle, uint8_t *rxBuff, size_t rxSize)
{
    return kStatus_HAL_I2cError;
}

hal_i2c_status_t HAL_I2cMasterTransferBlocking(hal_i2c_master_handle_t handle, hal_i2c_master_transfer_t *xfer)
{
    hal_i3c_master_t *i3cMasterHandle;
    i3c_master_transfer_t transfer;

    assert(handle);
    assert(xfer);

    i3cMasterHandle = (hal_i3c_master_t *)handle;

    transfer.flags          = xfer->flags;
    transfer.slaveAddress   = xfer->slaveAddress;
    transfer.direction      = (kHAL_I2cRead == xfer->direction) ? kI3C_Read : kI3C_Write;
    transfer.subaddress     = xfer->subaddress;
    transfer.subaddressSize = xfer->subaddressSize;
    transfer.data           = xfer->data;
    transfer.dataSize       = xfer->dataSize;

    return HAL_I2cGetStatus(I3C_MasterTransferBlocking(s_i3cBases[i3cMasterHandle->instance], kI3C_TypeI2C, &transfer));
}

hal_i2c_status_t HAL_I2cMasterTransferInstallCallback(hal_i2c_master_handle_t handle,
                                                      hal_i2c_master_transfer_callback_t callback,
                                                      void *callbackParam)
{
#if 0
    hal_i3c_master_t *i3cMasterHandle;

    assert(handle);

    i3cMasterHandle = (hal_i3c_master_t *)handle;

    i3cMasterHandle->callback      = callback;
    i3cMasterHandle->callbackParam = callbackParam;
    I3C_MasterTransferCreateHandle(s_i3cBases[i3cMasterHandle->instance], &i3cMasterHandle->hardwareHandle,
                                     HAL_I2cMasterCallback, i3cMasterHandle);

    return kStatus_HAL_I2cSuccess;
#else
    return kStatus_HAL_I2cError;
#endif
}

hal_i2c_status_t HAL_I2cMasterTransferNonBlocking(hal_i2c_master_handle_t handle, hal_i2c_master_transfer_t *xfer)
{
#if 0
    hal_i3c_master_t *i3cMasterHandle;
    i3c_master_transfer_t transfer;

    assert(handle);
    assert(xfer);

    i3cMasterHandle = (hal_i3c_master_t *)handle;

    transfer.flags          = xfer->flags;
    transfer.slaveAddress   = xfer->slaveAddress;
    transfer.direction      = (kHAL_I2cRead == xfer->direction) ? kI3C_Read : kI3C_Write;
    transfer.subaddress     = xfer->subaddress;
    transfer.subaddressSize = xfer->subaddressSize;
    transfer.data           = xfer->data;
    transfer.dataSize       = xfer->dataSize;
    return HAL_I2cGetStatus(I3C_MasterTransferNonBlocking(s_i3cBases[i3cMasterHandle->instance],
                                                            &i3cMasterHandle->hardwareHandle, &transfer));
#else
    return kStatus_HAL_I2cError;
#endif
}

hal_i2c_status_t HAL_I2cMasterTransferGetCount(hal_i2c_master_handle_t handle, size_t *count)
{
#if 0
    hal_i3c_master_t *i3cMasterHandle;

    assert(handle);
    assert(count);

    i3cMasterHandle = (hal_i3c_master_t *)handle;
    return HAL_I2cGetStatus(
        I3C_MasterTransferGetCount(s_i3cBases[i3cMasterHandle->instance], &i3cMasterHandle->hardwareHandle, count));
#else
    return kStatus_HAL_I2cError;
#endif
}

hal_i2c_status_t HAL_I2cMasterTransferAbort(hal_i2c_master_handle_t handle)
{
#if 0
    hal_i3c_master_t *i3cMasterHandle;

    assert(handle);

    i3cMasterHandle = (hal_i3c_master_t *)handle;
    I3C_MasterTransferAbort(s_i3cBases[i3cMasterHandle->instance], &i3cMasterHandle->hardwareHandle);

    return kStatus_HAL_I2cSuccess;
#else
    return kStatus_HAL_I2cError;
#endif
}

hal_i2c_status_t HAL_I2cSlaveTransferInstallCallback(hal_i2c_slave_handle_t handle,
                                                     hal_i2c_slave_transfer_callback_t callback,
                                                     void *callbackParam)
{
#if 0
    hal_i3c_slave_t *i2cSlaveHandle;

    assert(handle);

    i2cSlaveHandle = (hal_i3c_slave_t *)handle;

    i2cSlaveHandle->callback      = callback;
    i2cSlaveHandle->callbackParam = callbackParam;
    I3C_SlaveTransferCreateHandle(s_i3cBases[i2cSlaveHandle->instance], &i2cSlaveHandle->hardwareHandle,
                                    HAL_I2cSlaveCallback, i2cSlaveHandle);

    return kStatus_HAL_I2cSuccess;
#else
    return kStatus_HAL_I2cError;
#endif
}

hal_i2c_status_t HAL_I2cSlaveTransferNonBlocking(hal_i2c_slave_handle_t handle, uint32_t eventMask)
{
#if 0
    hal_i3c_slave_t *i2cSlaveHandle;

    assert(handle);

    i2cSlaveHandle = (hal_i3c_slave_t *)handle;

    return HAL_I2cGetStatus(I3C_SlaveTransferNonBlocking(s_i3cBases[i2cSlaveHandle->instance],
                                                           &i2cSlaveHandle->hardwareHandle, eventMask));
#else
    return kStatus_HAL_I2cError;
#endif
}

hal_i2c_status_t HAL_I2cSlaveTransferAbort(hal_i2c_slave_handle_t handle)
{
#if 0
    hal_i3c_slave_t *i2cSlaveHandle;

    assert(handle);

    i2cSlaveHandle = (hal_i3c_slave_t *)handle;

    I3C_SlaveTransferAbort(s_i3cBases[i2cSlaveHandle->instance], &i2cSlaveHandle->hardwareHandle);

    return kStatus_HAL_I2cSuccess;
#else
    return kStatus_HAL_I2cError;
#endif
}

hal_i2c_status_t HAL_I2cSlaveTransferGetCount(hal_i2c_slave_handle_t handle, size_t *count)
{
#if 0
    hal_i3c_slave_t *i2cSlaveHandle;

    assert(handle);
    assert(count);

    i2cSlaveHandle = (hal_i3c_slave_t *)handle;

    return HAL_I2cGetStatus(
        I3C_SlaveTransferGetCount(s_i3cBases[i2cSlaveHandle->instance], &i2cSlaveHandle->hardwareHandle, count));
#else
    return kStatus_HAL_I2cError;
#endif
}
