/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_fxos.h"

/******************************************************************************
 * Code
 ******************************************************************************/
status_t FXOS_Init(fxos_handle_t *fxos_handle, fxos_config_t *config)
{
    assert(fxos_handle);
    assert(config);
    assert(config->I2C_SendFunc);
    assert(config->I2C_ReceiveFunc);

    uint8_t tmp[1] = {0};

    /* Initialize the I2C access function. */
    fxos_handle->I2C_SendFunc    = config->I2C_SendFunc;
    fxos_handle->I2C_ReceiveFunc = config->I2C_ReceiveFunc;
    /* Set Slave Address. */
    fxos_handle->slaveAddress = config->slaveAddress;

    if (FXOS_ReadReg(fxos_handle, WHO_AM_I_REG, tmp, 1) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    if (tmp[0] != kFXOS_WHO_AM_I_Device_ID)
    {
        return kStatus_Fail;
    }

    /* setup auto sleep with FFMT trigger */
    /* go to standby */
    if (FXOS_ReadReg(fxos_handle, CTRL_REG1, tmp, 1) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    if (FXOS_WriteReg(fxos_handle, CTRL_REG1, tmp[0] & (uint8_t)~ACTIVE_MASK) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    /* Read again to make sure we are in standby mode. */
    if (FXOS_ReadReg(fxos_handle, CTRL_REG1, tmp, 1) != kStatus_Success)
    {
        return kStatus_Fail;
    }
    if ((tmp[0] & ACTIVE_MASK) == ACTIVE_MASK)
    {
        return kStatus_Fail;
    }

    /* Disable the FIFO */
    if (FXOS_WriteReg(fxos_handle, F_SETUP_REG, F_MODE_DISABLED) != kStatus_Success)
    {
        return kStatus_Fail;
    }

#ifdef LPSLEEP_HIRES
    /* enable auto-sleep, low power in sleep, high res in wake */
    if (FXOS_WriteReg(fxos_handle, CTRL_REG2, SLPE_MASK | SMOD_LOW_POWER | MOD_HIGH_RES) != kStatus_Success)
    {
        return kStatus_Fail;
    }
#else
    /* enable auto-sleep, low power in sleep, high res in wake */
    if (FXOS_WriteReg(fxos_handle, CTRL_REG2, MOD_HIGH_RES) != kStatus_Success)
    {
        return kStatus_Fail;
    }

#endif

    /* set up Mag OSR and Hybrid mode using M_CTRL_REG1, use default for Acc */
    if (FXOS_WriteReg(fxos_handle, M_CTRL_REG1, (M_RST_MASK | M_OSR_MASK | M_HMS_MASK)) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    /* Enable hyrid mode auto increment using M_CTRL_REG2 */
    if (FXOS_WriteReg(fxos_handle, M_CTRL_REG2, (M_HYB_AUTOINC_MASK)) != kStatus_Success)
    {
        return kStatus_Fail;
    }

#ifdef EN_FFMT
    /* enable FFMT for motion detect for X and Y axes, latch enable */
    if (FXOS_WriteReg(fxos_handle, FF_MT_CFG_REG, XEFE_MASK | YEFE_MASK | ELE_MASK | OAE_MASK) != kStatus_Success)
    {
        return kStatus_Fail;
    }
#endif

#ifdef SET_THRESHOLD
    /* set threshold to about 0.25g */
    if (FXOS_WriteReg(fxos_handle, FT_MT_THS_REG, 0x04) != kStatus_Success)
    {
        return kStatus_Fail;
    }
#endif

#ifdef SET_DEBOUNCE
    /* set debounce to zero */
    if (FXOS_WriteReg(fxos_handle, FF_MT_COUNT_REG, 0x00) != kStatus_Success)
    {
        return kStatus_Fail;
    }
#endif

#ifdef EN_AUTO_SLEEP
    /* set auto-sleep wait period to 5s (=5/0.64=~8) */
    if (FXOS_WriteReg(fxos_handle, ASLP_COUNT_REG, 8) != kStatus_Success)
    {
        return kStatus_Fail;
    }
#endif
    /* default set to 4g mode */
    if (FXOS_WriteReg(fxos_handle, XYZ_DATA_CFG_REG, FULL_SCALE_4G) != kStatus_Success)
    {
        return kStatus_Fail;
    }
#ifdef EN_INTERRUPTS
    /* enable data-ready, auto-sleep and motion detection interrupts */
    /* FXOS1_WriteRegister(CTRL_REG4, INT_EN_DRDY_MASK | INT_EN_ASLP_MASK | INT_EN_FF_MT_MASK); */
    if (FXOS_WriteReg(fxos_handle, CTRL_REG4, 0x0) != kStatus_Success)
    {
        return kStatus_Fail;
    }
    /* route data-ready interrupts to INT1, others INT2 (default) */
    if (FXOS_WriteReg(fxos_handle, CTRL_REG5, INT_CFG_DRDY_MASK) != kStatus_Success)
    {
        return kStatus_Fail;
    }
    /* enable ffmt as a wake-up source */
    if (FXOS_WriteReg(fxos_handle, CTRL_REG3, WAKE_FF_MT_MASK) != kStatus_Success)
    {
        return kStatus_Fail;
    }
    /* finally activate accel_device with ASLP ODR=0.8Hz, ODR=100Hz, FSR=2g */
    if (FXOS_WriteReg(fxos_handle, CTRL_REG1, HYB_ASLP_RATE_0_8HZ | HYB_DATA_RATE_100HZ | ACTIVE_MASK) !=
        kStatus_Success)
    {
        return kStatus_Fail;
    }
#else
    /* Setup the ODR for 50 Hz and activate the accelerometer */
    if (FXOS_WriteReg(fxos_handle, CTRL_REG1, (HYB_DATA_RATE_200HZ | ACTIVE_MASK)) != kStatus_Success)
    {
        return kStatus_Fail;
    }
#endif

    /* Read Control register again to ensure we are in active mode */
    if (FXOS_ReadReg(fxos_handle, CTRL_REG1, tmp, 1) != kStatus_Success)
    {
        return kStatus_Fail;
    }

    if ((tmp[0] & ACTIVE_MASK) != ACTIVE_MASK)
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

status_t FXOS_ReadSensorData(fxos_handle_t *fxos_handle, fxos_data_t *sensorData)
{
    status_t status     = kStatus_Success;
    uint8_t tmp_buff[6] = {0};
    uint8_t i           = 0;

    if (!FXOS_ReadReg(fxos_handle, OUT_X_MSB_REG, tmp_buff, 6) == kStatus_Success)
    {
        status = kStatus_Fail;
    }

    for (i = 0; i < 6; i++)
    {
        ((int8_t *)sensorData)[i] = tmp_buff[i];
    }

    if (!FXOS_ReadReg(fxos_handle, M_OUT_X_MSB_REG, tmp_buff, 6) == kStatus_Success)
    {
        status = kStatus_Fail;
    }

    for (i = 0; i < 6; i++)
    {
        ((int8_t *)sensorData)[i + 6] = tmp_buff[i];
    }

    return status;
}

status_t FXOS_ReadReg(fxos_handle_t *handle, uint8_t reg, uint8_t *val, uint8_t bytesNumber)
{
    assert(handle);
    assert(val);

    if (!handle->I2C_ReceiveFunc)
    {
        return kStatus_Fail;
    }

    return handle->I2C_ReceiveFunc(handle->slaveAddress, reg, 1, val, bytesNumber);
}

status_t FXOS_WriteReg(fxos_handle_t *handle, uint8_t reg, uint8_t val)
{
    assert(handle);

    if (!handle->I2C_SendFunc)
    {
        return kStatus_Fail;
    }

    return handle->I2C_SendFunc(handle->slaveAddress, reg, 1, val);
}
