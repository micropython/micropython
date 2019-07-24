/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CODEC_I2C_H_
#define _FSL_CODEC_I2C_H_

#include "fsl_common.h"
#include "i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief CODEC device register address type. */
typedef enum _codec_reg_addr
{
    kCODEC_RegAddr8Bit  = 1U, /*!< 8-bit register address.  */
    kCODEC_RegAddr16Bit = 2U, /*!< 16-bit register address. */
} codec_reg_addr_t;

/*! @brief CODEC device register width. */
typedef enum _codec_reg_width
{
    kCODEC_RegWidth8Bit  = 1U, /*!< 8-bit register width.  */
    kCODEC_RegWidth16Bit = 2U, /*!< 16-bit register width. */
    kCODEC_RegWidth32Bit = 4U, /*!< 32-bit register width. */
} codec_reg_width_t;

/*! @brief CODEC I2C configurations structure */
typedef struct _codec_i2c_config
{
    uint32_t codecI2CInstance;    /*!< i2c bus instance */
    uint32_t codecI2CSourceClock; /*!< i2c bus source clock frequency */
} codec_i2c_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Codec i2c bus initilization.
 *
 * @param handle i2c master handle.
 * @param i2CInstance instance number of the i2c bus, such as 0 is corresponding to I2C0.
 * @param i2cBaudrate i2c baudrate.
 * @param i2cSourceClockHz i2c source clock frequency.
 * @return kStatus_HAL_I2cSuccess is success, else initial failed.
 */
status_t CODEC_I2C_Init(void *handle, uint32_t i2cInstance, uint32_t i2cBaudrate, uint32_t i2cSourceClockHz);

/*!
 * @brief Codec i2c de-initilization.
 *
 * @param handle i2c master handle.
 * @return kStatus_HAL_I2cSuccess is success, else deinitial failed.
 */
status_t CODEC_I2C_Deinit(void *handle);

/*!
 * @brief codec i2c send function.
 *
 * @param handle i2c master handle.
 * @param deviceAddress codec device address.
 * @param subAddress register address.
 * @param subaddressSize register address width.
 * @param txBuff tx buffer pointer.
 * @param txBuffSize tx buffer size.
 * @return kStatus_HAL_I2cSuccess is success, else send failed.
 */
status_t CODEC_I2C_Send(void *handle,
                        uint8_t deviceAddress,
                        uint32_t subAddress,
                        uint8_t subaddressSize,
                        uint8_t *txBuff,
                        uint8_t txBuffSize);

/*!
 * @brief codec i2c receive function.
 *
 * @param handle i2c master handle.
 * @param deviceAddress codec device address.
 * @param subAddress register address.
 * @param subaddressSize register address width.
 * @param rxBuff rx buffer pointer.
 * @param rxBuffSize rx buffer size.
 * @return kStatus_HAL_I2cSuccess is success, else receive failed.
 */
status_t CODEC_I2C_Receive(void *handle,
                           uint8_t deviceAddress,
                           uint32_t subAddress,
                           uint8_t subaddressSize,
                           uint8_t *rxBuff,
                           uint8_t rxBuffSize);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_CODEC_I2C_H_ */
