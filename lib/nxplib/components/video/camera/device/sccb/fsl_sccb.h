/*
 * Copyright  2017-2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SCCB_H_
#define _FSL_SCCB_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef status_t (*sccb_i2c_send_func_t)(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
typedef status_t (*sccb_i2c_receive_func_t)(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);

/*! @brief SCCB register address type. */
typedef enum _sccb_reg_addr
{
    kSCCB_RegAddr8Bit  = 1, /*!< 8-bit register address. */
    kSCCB_RegAddr16Bit = 2, /*!< 16-bit register address. */
} sccb_reg_addr_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Write value to the SCCB register.
 *
 * @param i2cAddr SCCB I2C address.
 * @param addrType SCCB register address type.
 * @param reg The register to write.
 * @param value The value to write.
 * @param i2cSendFunc The actual I2C send function.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
status_t SCCB_WriteReg(
    uint8_t i2cAddr, sccb_reg_addr_t addrType, uint32_t reg, uint8_t value, sccb_i2c_send_func_t i2cSendFunc);

/*!
 * @brief Write values to multiple SCCB registers.
 *
 * @param i2cAddr SCCB I2C address.
 * @param addrType SCCB register address type.
 * @param startReg The start register to write.
 * @param value Pointer to the value to write.
 * @param len Length of the value to write.
 * @param i2cSendFunc The actual I2C send function.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
status_t SCCB_WriteMultiRegs(uint8_t i2cAddr,
                             sccb_reg_addr_t addrType,
                             uint32_t startReg,
                             const uint8_t *value,
                             uint32_t len,
                             sccb_i2c_send_func_t i2cSendFunc);

/*!
 * @brief Read the SCCB register value.
 *
 * @param i2cAddr SCCB I2C address.
 * @param addrType SCCB register address type.
 * @param reg The register to read.
 * @param value The value read out.
 * @param i2cReceiveFunc The actual I2C receive function.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
status_t SCCB_ReadReg(
    uint8_t i2cAddr, sccb_reg_addr_t addrType, uint32_t reg, uint8_t *value, sccb_i2c_receive_func_t i2cReceiveFunc);

/*!
 * @brief Modify the SCCB register value.
 *
 * This function modifies some bit fields of a register.
 *
 * @param i2cAddr SCCB I2C address.
 * @param addrType SCCB register address type.
 * @param reg The register to modify.
 * @param clrMask The mask value to clear.
 * @param value The value to set.
 * @param i2cReceiveFunc The actual I2C receive function.
 * @param i2cSendFunc The actual I2C send function.
 * @return Returns @ref kStatus_Success if success, otherwise returns error code.
 */
status_t SCCB_ModifyReg(uint8_t i2cAddr,
                        sccb_reg_addr_t addrType,
                        uint32_t reg,
                        uint8_t clrMask,
                        uint8_t value,
                        sccb_i2c_receive_func_t i2cReceiveFunc,
                        sccb_i2c_send_func_t i2cSendFunc);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_SCCB_H_ */
