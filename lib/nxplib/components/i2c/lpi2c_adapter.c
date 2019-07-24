/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_lpi2c.h"

#include "i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief i2c master state structure. */
typedef struct _hal_i2c_master
{
    hal_i2c_master_transfer_callback_t callback;
    void *callbackParam;
    lpi2c_master_handle_t hardwareHandle;
    uint8_t instance;
} hal_i2c_master_t;

/*! @brief i2c slave state structure. */
typedef struct _hal_i2c_slave
{
    hal_i2c_slave_transfer_callback_t callback;
    void *callbackParam;
    hal_i2c_slave_transfer_t transfer;
    lpi2c_slave_handle_t hardwareHandle;
    uint8_t instance;
} hal_i2c_slave_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Pointers to i2c bases for each instance. */
static LPI2C_Type *const s_i2cBases[] = LPI2C_BASE_PTRS;

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
        case kStatus_LPI2C_Busy:
        {
            returnStatus = kStatus_HAL_I2cBusy;
            break;
        }
        case kStatus_LPI2C_Idle:
        {
            returnStatus = kStatus_HAL_I2cIdle;
            break;
        }
        case kStatus_LPI2C_Nak:
        {
            returnStatus = kStatus_HAL_I2cNak;
            break;
        }
        case kStatus_LPI2C_ArbitrationLost:
        {
            returnStatus = kStatus_HAL_I2cArbitrationLost;
            break;
        }
        case kStatus_LPI2C_Timeout:
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

static void HAL_I2cMasterCallback(LPI2C_Type *base, lpi2c_master_handle_t *handle, status_t status, void *callbackParam)
{
    hal_i2c_master_t *i2cMasterHandle;
    assert(callbackParam);

    i2cMasterHandle = (hal_i2c_master_t *)callbackParam;

    if (i2cMasterHandle->callback)
    {
        i2cMasterHandle->callback(i2cMasterHandle, HAL_I2cGetStatus(status), i2cMasterHandle->callbackParam);
    }
}

static void HAL_I2cSlaveCallback(LPI2C_Type *base, lpi2c_slave_transfer_t *xfer, void *callbackParam)
{
    hal_i2c_slave_t *i2cSlaveHandle;
    assert(callbackParam);

    i2cSlaveHandle = (hal_i2c_slave_t *)callbackParam;

    if (i2cSlaveHandle->callback)
    {
        i2cSlaveHandle->transfer.event            = (hal_i2c_slave_transfer_event_t)xfer->event;
        i2cSlaveHandle->transfer.data             = xfer->data;
        i2cSlaveHandle->transfer.dataSize         = xfer->dataSize;
        i2cSlaveHandle->transfer.completionStatus = HAL_I2cGetStatus(xfer->completionStatus);
        i2cSlaveHandle->transfer.transferredCount = xfer->transferredCount;
        i2cSlaveHandle->callback(i2cSlaveHandle, &i2cSlaveHandle->transfer, i2cSlaveHandle->callbackParam);
        xfer->data     = i2cSlaveHandle->transfer.data;
        xfer->dataSize = i2cSlaveHandle->transfer.dataSize;
    }
}

hal_i2c_status_t HAL_I2cMasterInit(hal_i2c_master_handle_t handle, const hal_i2c_master_config_t *config)
{
    hal_i2c_master_t *i2cMasterHandle;
    lpi2c_master_config_t i2cConfig;

    assert(handle);
    assert(config);

    if (HAL_I2C_MASTER_HANDLE_SIZE < sizeof(hal_i2c_master_t))
    {
        return kStatus_HAL_I2cError;
    }

    i2cMasterHandle = (hal_i2c_master_t *)handle;

    LPI2C_MasterGetDefaultConfig(&i2cConfig);
    i2cConfig.enableMaster    = config->enableMaster;
    i2cConfig.baudRate_Hz     = config->baudRate_Bps;
    i2cMasterHandle->instance = config->instance;

    LPI2C_MasterInit(s_i2cBases[i2cMasterHandle->instance], &i2cConfig, config->srcClock_Hz);

    return kStatus_HAL_I2cSuccess;
}

hal_i2c_status_t HAL_I2cSlaveInit(hal_i2c_slave_handle_t handle, const hal_i2c_slave_config_t *config)
{
    hal_i2c_slave_t *i2cSlaveHandle;
    lpi2c_slave_config_t i2cConfig;

    assert(handle);
    assert(config);

    if (HAL_I2C_SLAVE_HANDLE_SIZE < sizeof(hal_i2c_slave_t))
    {
        return kStatus_HAL_I2cError;
    }

    i2cSlaveHandle = (hal_i2c_slave_t *)handle;

    LPI2C_SlaveGetDefaultConfig(&i2cConfig);
    i2cConfig.enableSlave    = config->enableSlave;
    i2cConfig.address0       = config->slaveAddress;
    i2cSlaveHandle->instance = config->instance;

    LPI2C_SlaveInit(s_i2cBases[i2cSlaveHandle->instance], &i2cConfig, config->srcClock_Hz);

    return kStatus_HAL_I2cSuccess;
}

hal_i2c_status_t HAL_I2cMasterDeinit(hal_i2c_master_handle_t handle)
{
    hal_i2c_master_t *i2cMasterHandle;

    assert(handle);

    i2cMasterHandle = (hal_i2c_master_t *)handle;

    LPI2C_MasterDeinit(s_i2cBases[i2cMasterHandle->instance]);

    return kStatus_HAL_I2cSuccess;
}

hal_i2c_status_t HAL_I2cSlaveDeinit(hal_i2c_slave_handle_t handle)
{
    hal_i2c_slave_t *i2cSlaveHandle;

    assert(handle);

    i2cSlaveHandle = (hal_i2c_slave_t *)handle;

    LPI2C_SlaveDeinit(s_i2cBases[i2cSlaveHandle->instance]);

    return kStatus_HAL_I2cSuccess;
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
    hal_i2c_master_t *i2cMasterHandle;
    lpi2c_master_transfer_t transfer;

    assert(handle);
    assert(xfer);

    i2cMasterHandle = (hal_i2c_master_t *)handle;

    transfer.flags          = xfer->flags;
    transfer.slaveAddress   = xfer->slaveAddress;
    transfer.direction      = (kHAL_I2cRead == xfer->direction) ? kLPI2C_Read : kLPI2C_Write;
    transfer.subaddress     = xfer->subaddress;
    transfer.subaddressSize = xfer->subaddressSize;
    transfer.data           = xfer->data;
    transfer.dataSize       = xfer->dataSize;

    return HAL_I2cGetStatus(LPI2C_MasterTransferBlocking(s_i2cBases[i2cMasterHandle->instance], &transfer));
}

hal_i2c_status_t HAL_I2cMasterTransferInstallCallback(hal_i2c_master_handle_t handle,
                                                      hal_i2c_master_transfer_callback_t callback,
                                                      void *callbackParam)
{
    hal_i2c_master_t *i2cMasterHandle;

    assert(handle);

    i2cMasterHandle = (hal_i2c_master_t *)handle;

    i2cMasterHandle->callback      = callback;
    i2cMasterHandle->callbackParam = callbackParam;
    LPI2C_MasterTransferCreateHandle(s_i2cBases[i2cMasterHandle->instance], &i2cMasterHandle->hardwareHandle,
                                     HAL_I2cMasterCallback, i2cMasterHandle);

    return kStatus_HAL_I2cSuccess;
}

hal_i2c_status_t HAL_I2cMasterTransferNonBlocking(hal_i2c_master_handle_t handle, hal_i2c_master_transfer_t *xfer)
{
    hal_i2c_master_t *i2cMasterHandle;
    lpi2c_master_transfer_t transfer;

    assert(handle);
    assert(xfer);

    i2cMasterHandle = (hal_i2c_master_t *)handle;

    transfer.flags          = xfer->flags;
    transfer.slaveAddress   = xfer->slaveAddress;
    transfer.direction      = (kHAL_I2cRead == xfer->direction) ? kLPI2C_Read : kLPI2C_Write;
    transfer.subaddress     = xfer->subaddress;
    transfer.subaddressSize = xfer->subaddressSize;
    transfer.data           = xfer->data;
    transfer.dataSize       = xfer->dataSize;
    return HAL_I2cGetStatus(LPI2C_MasterTransferNonBlocking(s_i2cBases[i2cMasterHandle->instance],
                                                            &i2cMasterHandle->hardwareHandle, &transfer));
}

hal_i2c_status_t HAL_I2cMasterTransferGetCount(hal_i2c_master_handle_t handle, size_t *count)
{
    hal_i2c_master_t *i2cMasterHandle;

    assert(handle);
    assert(count);

    i2cMasterHandle = (hal_i2c_master_t *)handle;
    return HAL_I2cGetStatus(
        LPI2C_MasterTransferGetCount(s_i2cBases[i2cMasterHandle->instance], &i2cMasterHandle->hardwareHandle, count));
}

hal_i2c_status_t HAL_I2cMasterTransferAbort(hal_i2c_master_handle_t handle)
{
    hal_i2c_master_t *i2cMasterHandle;

    assert(handle);

    i2cMasterHandle = (hal_i2c_master_t *)handle;
    LPI2C_MasterTransferAbort(s_i2cBases[i2cMasterHandle->instance], &i2cMasterHandle->hardwareHandle);

    return kStatus_HAL_I2cSuccess;
}

hal_i2c_status_t HAL_I2cSlaveTransferInstallCallback(hal_i2c_slave_handle_t handle,
                                                     hal_i2c_slave_transfer_callback_t callback,
                                                     void *callbackParam)
{
    hal_i2c_slave_t *i2cSlaveHandle;

    assert(handle);

    i2cSlaveHandle = (hal_i2c_slave_t *)handle;

    i2cSlaveHandle->callback      = callback;
    i2cSlaveHandle->callbackParam = callbackParam;
    LPI2C_SlaveTransferCreateHandle(s_i2cBases[i2cSlaveHandle->instance], &i2cSlaveHandle->hardwareHandle,
                                    HAL_I2cSlaveCallback, i2cSlaveHandle);

    return kStatus_HAL_I2cSuccess;
}

hal_i2c_status_t HAL_I2cSlaveTransferNonBlocking(hal_i2c_slave_handle_t handle, uint32_t eventMask)
{
    hal_i2c_slave_t *i2cSlaveHandle;

    assert(handle);

    i2cSlaveHandle = (hal_i2c_slave_t *)handle;

    return HAL_I2cGetStatus(LPI2C_SlaveTransferNonBlocking(s_i2cBases[i2cSlaveHandle->instance],
                                                           &i2cSlaveHandle->hardwareHandle, eventMask));
}

hal_i2c_status_t HAL_I2cSlaveTransferAbort(hal_i2c_slave_handle_t handle)
{
    hal_i2c_slave_t *i2cSlaveHandle;

    assert(handle);

    i2cSlaveHandle = (hal_i2c_slave_t *)handle;

    LPI2C_SlaveTransferAbort(s_i2cBases[i2cSlaveHandle->instance], &i2cSlaveHandle->hardwareHandle);

    return kStatus_HAL_I2cSuccess;
}

hal_i2c_status_t HAL_I2cSlaveTransferGetCount(hal_i2c_slave_handle_t handle, size_t *count)
{
    hal_i2c_slave_t *i2cSlaveHandle;

    assert(handle);
    assert(count);

    i2cSlaveHandle = (hal_i2c_slave_t *)handle;

    return HAL_I2cGetStatus(
        LPI2C_SlaveTransferGetCount(s_i2cBases[i2cSlaveHandle->instance], &i2cSlaveHandle->hardwareHandle, count));
}
