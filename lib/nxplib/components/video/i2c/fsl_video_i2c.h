/*
 * Copyright  2017-2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_VIDEO_I2C_H_
#define _FSL_VIDEO_I2C_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Define I2C access function. */
typedef status_t (*video_i2c_send_func_t)(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
typedef status_t (*video_i2c_receive_func_t)(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);

/*! @brief Video device register address type. */
typedef enum _video_reg_addr
{
    kVIDEO_RegAddr8Bit  = 1U, /*!< 8-bit register address.  */
    kVIDEO_RegAddr16Bit = 2U, /*!< 16-bit register address. */
} video_reg_addr_t;

/*! @brief Video device register width. */
typedef enum _video_reg_width
{
    kVIDEO_RegWidth8Bit  = 1U, /*!< 8-bit register width.  */
    kVIDEO_RegWidth16Bit = 2U, /*!< 16-bit register width. */
    kVIDEO_RegWidth32Bit = 4U, /*!< 32-bit register width. */
} video_reg_width_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Write value to the register.
 *
 * @param i2cAddr I2C address.
 * @param addrType Register address type.
 * @param reg The register to write.
 * @param regWidth The width of the register.
 * @param value The value to write.
 * @param i2cSendFunc The actual I2C send function.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
status_t VIDEO_I2C_WriteReg(uint8_t i2cAddr,
                            video_reg_addr_t addrType,
                            uint32_t reg,
                            video_reg_width_t regWidth,
                            uint32_t value,
                            video_i2c_send_func_t i2cSendFunc);

/*!
 * @brief Read the register value.
 *
 * @param i2cAddr I2C address.
 * @param addrType Register address type.
 * @param reg The register to read.
 * @param regWidth The width of the register.
 * @param value The value read out.
 * @param i2cReceiveFunc The actual I2C receive function.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
status_t VIDEO_I2C_ReadReg(uint8_t i2cAddr,
                           video_reg_addr_t addrType,
                           uint32_t reg,
                           video_reg_width_t regWidth,
                           void *value,
                           video_i2c_receive_func_t i2cReceiveFunc);

/*!
 * @brief Modify the register value.
 *
 * This function modifies some bit fields of a register.
 * reg[clrMask] = value & clrMask
 *
 * @param i2cAddr I2C address.
 * @param addrType Register address type.
 * @param reg The register to modify.
 * @param regWidth The width of the register.
 * @param clrMask The mask value to clear.
 * @param value The value to set.
 * @param i2cReceiveFunc The actual I2C receive function.
 * @param i2cSendFunc The actual I2C send function.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
status_t VIDEO_I2C_ModifyReg(uint8_t i2cAddr,
                             video_reg_addr_t addrType,
                             uint32_t reg,
                             video_reg_width_t regWidth,
                             uint32_t clrMask,
                             uint32_t value,
                             video_i2c_receive_func_t i2cReceiveFunc,
                             video_i2c_send_func_t i2cSendFunc);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_VIDEO_I2C_H_ */
