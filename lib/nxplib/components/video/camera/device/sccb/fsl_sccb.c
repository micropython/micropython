/*
 * Copyright  2017-2018 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_sccb.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t SCCB_WriteReg(
    uint8_t i2cAddr, sccb_reg_addr_t addrType, uint32_t reg, uint8_t value, sccb_i2c_send_func_t i2cSendFunc)
{
    uint32_t regTmp;

    if (kSCCB_RegAddr16Bit == addrType)
    {
        /* Byte swap. */
        regTmp = ((reg >> 8U) & 0xFF) | ((reg & 0xFFU) << 8U);
        reg    = regTmp;
    }

    return i2cSendFunc(i2cAddr, reg, addrType, &value, 1);
}

status_t SCCB_WriteMultiRegs(uint8_t i2cAddr,
                             sccb_reg_addr_t addrType,
                             uint32_t startReg,
                             const uint8_t *value,
                             uint32_t len,
                             sccb_i2c_send_func_t i2cSendFunc)
{
    uint32_t regTmp;

    if (kSCCB_RegAddr16Bit == addrType)
    {
        /* Byte swap. */
        regTmp   = ((startReg >> 8U) & 0xFF) | ((startReg & 0xFFU) << 8U);
        startReg = regTmp;
    }

    return i2cSendFunc(i2cAddr, startReg, addrType, value, len);
}

status_t SCCB_ReadReg(
    uint8_t i2cAddr, sccb_reg_addr_t addrType, uint32_t reg, uint8_t *value, sccb_i2c_receive_func_t i2cReceiveFunc)
{
    uint32_t regTmp;

    if (kSCCB_RegAddr16Bit == addrType)
    {
        /* Byte swap. */
        regTmp = ((reg >> 8U) & 0xFF) | ((reg & 0xFFU) << 8U);
        reg    = regTmp;
    }

    return i2cReceiveFunc(i2cAddr, reg, addrType, value, 1);
}

status_t SCCB_ModifyReg(uint8_t i2cAddr,
                        sccb_reg_addr_t addrType,
                        uint32_t reg,
                        uint8_t clrMask,
                        uint8_t value,
                        sccb_i2c_receive_func_t i2cReceiveFunc,
                        sccb_i2c_send_func_t i2cSendFunc)
{
    status_t status;
    uint8_t regVal;

    status = SCCB_ReadReg(i2cAddr, addrType, reg, &regVal, i2cReceiveFunc);

    if (kStatus_Success != status)
    {
        return status;
    }

    regVal = (regVal & ~((uint32_t)clrMask)) | (value & clrMask);

    return SCCB_WriteReg(i2cAddr, addrType, reg, regVal, i2cSendFunc);
}
