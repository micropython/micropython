/*
 * Copyright  2017-2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_video_i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
status_t VIDEO_I2C_WriteReg(uint8_t i2cAddr,
                            video_reg_addr_t addrType,
                            uint32_t reg,
                            video_reg_width_t regWidth,
                            uint32_t value,
                            video_i2c_send_func_t i2cSendFunc)
{
    uint8_t data[4];
    uint8_t i;
    uint32_t regTmp;

    if (kVIDEO_RegAddr16Bit == addrType)
    {
        /* Byte swap. */
        regTmp = ((reg >> 8U) & 0xFF) | ((reg & 0xFFU) << 8U);
        reg    = regTmp;
    }

    i = regWidth;
    while (i--)
    {
        data[i] = (uint8_t)value;
        value >>= 8;
    }

    return i2cSendFunc(i2cAddr, reg, addrType, data, regWidth);
}

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
                           video_i2c_receive_func_t i2cReceiveFunc)
{
    uint8_t data[4];
    uint8_t i = 0;
    uint32_t regTmp;
    status_t status;

    if (kVIDEO_RegAddr16Bit == addrType)
    {
        /* Byte swap. */
        regTmp = ((reg >> 8U) & 0xFF) | ((reg & 0xFFU) << 8U);
        reg    = regTmp;
    }

    status = i2cReceiveFunc(i2cAddr, reg, addrType, data, regWidth);

    if (kStatus_Success == status)
    {
        while (regWidth--)
        {
            ((uint8_t *)value)[i++] = data[regWidth];
        }
    }

    return status;
}

status_t VIDEO_I2C_ModifyReg(uint8_t i2cAddr,
                             video_reg_addr_t addrType,
                             uint32_t reg,
                             video_reg_width_t regWidth,
                             uint32_t clrMask,
                             uint32_t value,
                             video_i2c_receive_func_t i2cReceiveFunc,
                             video_i2c_send_func_t i2cSendFunc)
{
    status_t status;
    uint32_t regVal;

    status = VIDEO_I2C_ReadReg(i2cAddr, addrType, reg, regWidth, &regVal, i2cReceiveFunc);

    if (kStatus_Success != status)
    {
        return status;
    }

    regVal = (regVal & ~(clrMask)) | (value & clrMask);

    return VIDEO_I2C_WriteReg(i2cAddr, addrType, reg, regWidth, regVal, i2cSendFunc);
}
