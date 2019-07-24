/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_codec_i2c.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * brief Codec i2c bus initilization.
 *
 * param handle i2c master handle.
 * param i2CInstance instance number of the i2c bus, such as 0 is corresponding to I2C0.
 * param i2cBaudrate i2c baudrate.
 * param i2cSourceClockHz i2c source clock frequency.
 * return kStatus_HAL_I2cSuccess is success, else initial failed.
 */
status_t CODEC_I2C_Init(void *handle, uint32_t i2cInstance, uint32_t i2cBaudrate, uint32_t i2cSourceClockHz)
{
    hal_i2c_master_config_t masterConfig;

    masterConfig.enableMaster = true;
    masterConfig.baudRate_Bps = i2cBaudrate;
    masterConfig.srcClock_Hz  = i2cSourceClockHz;
    masterConfig.instance     = i2cInstance;

    return HAL_I2cMasterInit((hal_i2c_master_handle_t *)handle, &masterConfig);
}

/*!
 * brief Codec i2c de-initilization.
 *
 * param handle i2c master handle.
 * return kStatus_HAL_I2cSuccess is success, else deinitial failed.
 */
status_t CODEC_I2C_Deinit(void *handle)
{
    return HAL_I2cMasterDeinit((hal_i2c_master_handle_t *)handle);
}

/*!
 * brief codec i2c send function.
 *
 * param handle i2c master handle.
 * param deviceAddress codec device address.
 * param subAddress register address.
 * param subaddressSize register address width.
 * param txBuff tx buffer pointer.
 * param txBuffSize tx buffer size.
 * return kStatus_HAL_I2cSuccess is success, else send failed.
 */
status_t CODEC_I2C_Send(void *handle,
                        uint8_t deviceAddress,
                        uint32_t subAddress,
                        uint8_t subaddressSize,
                        uint8_t *txBuff,
                        uint8_t txBuffSize)
{
    hal_i2c_master_transfer_t masterXfer;

    masterXfer.slaveAddress   = deviceAddress;
    masterXfer.direction      = kHAL_I2cWrite;
    masterXfer.subaddress     = (uint32_t)subAddress;
    masterXfer.subaddressSize = subaddressSize;
    masterXfer.data           = txBuff;
    masterXfer.dataSize       = txBuffSize;
    masterXfer.flags          = kHAL_I2cTransferDefaultFlag;

    return HAL_I2cMasterTransferBlocking((hal_i2c_master_handle_t *)handle, &masterXfer);
}

/*!
 * brief codec i2c receive function.
 *
 * param handle i2c master handle.
 * param deviceAddress codec device address.
 * param subAddress register address.
 * param subaddressSize register address width.
 * param rxBuff rx buffer pointer.
 * param rxBuffSize rx buffer size.
 * return kStatus_HAL_I2cSuccess is success, else receive failed.
 */
status_t CODEC_I2C_Receive(void *handle,
                           uint8_t deviceAddress,
                           uint32_t subAddress,
                           uint8_t subaddressSize,
                           uint8_t *rxBuff,
                           uint8_t rxBuffSize)
{
    hal_i2c_master_transfer_t masterXfer;

    masterXfer.slaveAddress   = deviceAddress;
    masterXfer.direction      = kHAL_I2cRead;
    masterXfer.subaddress     = (uint32_t)subAddress;
    masterXfer.subaddressSize = subaddressSize;
    masterXfer.data           = rxBuff;
    masterXfer.dataSize       = rxBuffSize;
    masterXfer.flags          = kHAL_I2cTransferDefaultFlag;

    return HAL_I2cMasterTransferBlocking((hal_i2c_master_handle_t *)handle, &masterXfer);
}
