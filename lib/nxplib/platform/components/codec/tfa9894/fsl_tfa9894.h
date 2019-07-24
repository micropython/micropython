/*
 * fsl_tfa9894.h
 *
 *  Created on: Apr 25, 2018
 *      Author: krg00719
 */

#ifndef FSL_TFA9894_H_
#define FSL_TFA9894_H_

#include "fsl_common.h"
#include "fsl_lpi2c.h"

#define LEFT_AMP 0x34
#define RIGHT_AMP 0x35

typedef struct tfa98xx_handle
{
#if defined(FSL_FEATURE_SOC_LPI2C_COUNT) && (FSL_FEATURE_SOC_LPI2C_COUNT)
    LPI2C_Type *base;
    lpi2c_master_transfer_t xfer;
    lpi2c_master_handle_t *i2cHandle;
#else
    /* I2C relevant definition. */
    I2C_Type *base;                 /*!< I2C instance. */
    i2c_master_transfer_t xfer;     /*!< I2C device setting */
    i2c_master_handle_t *i2cHandle; /*!< I2C internal state space. */
#endif
} tfa98xx_handle_t;

enum TFA_Devices
{
    TFA_MONO   = 1,
    TFA_STEREO = 2
};

/* Funtion Prototype */
int TFA9894_Init_Bypass(void);
int TFA9894_Init_Generic(void);
#endif /* FSL_TFA9894_H_ */
