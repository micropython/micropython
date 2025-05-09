/**
  ******************************************************************************
  * @file    stm32n6xx_hal_bsec.c
  * @author  MCD Application Team
  * @brief   BSEC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of Boot and SECurity (BSEC):
  *           + Initialization and de-initialization functions
  *
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]

     *** General Configuration ***
    ============================================================
    [..]

      (+) Call the function HAL_BSEC_GetStatus() to get the status of BSEC
    peripheral.

      (+) Call the function HAL_BSEC_GlobalLock() to lock the write to BSEC
    registers.

      (+) Call the function HAL_BSEC_GetGlobalLockStatus() to get the global
    write registers lock status.

      (+) Call the function HAL_BSEC_GetErrorCode() to get the error code
    raised when API return HAL_ERROR.

     *** OTP Management ***
    ============================================================
    [..]

      (+) Call the function HAL_BSEC_OTP_Read() to read the value of
    an OTP fuse.

      (+) Call the function HAL_BSEC_OTP_Program() to program an OTP
    fuse with or without permanent lock.

      (+) Call the function HAL_BSEC_OTP_Reload() to reload an OTP
    fuse value.

      (+) Call the function HAL_BSEC_OTP_Lock() to sticky lock
    an OTP fuse programming, writing shadowed register or reload.

      (+) Call the function HAL_BSEC_OTP_GetState() to get the sticky lock
    status (programming, writing shadowed register or reload), the permanent
    lock status, the shadow configuration, the ECC or hidden status of an
    OTP fuse.


     *** Shadow fuse register management ***
    [..]

      (+) Call the function HAL_BSEC_OTP_ReadShadow() to read the value of
    a shadow fuse register.

      (+) Call the function HAL_BSEC_OTP_WriteShadow() to write in a shadow
    fuse register.

      (+) Call the function HAL_BSEC_OTP_GetShadowState() to
    get the validity status of a shadow fuse register.

     *** Device lifecycle management ***
    ============================================================
    [..]

      (+) Call the function HAL_BSEC_GetDeviceLifeCycleState() to get the
    BSEC device lifecycle state.

      (+) Call the function HAL_BSEC_ReadEpochCounter() to read the value
    of epoch counter.

      (+) Call the function HAL_BSEC_SelectEpochCounter() to select the
    epoch counter used by SAES.

      (+) Call the function HAL_BSEC_GetEpochCounterSelection() to get
    the epoch counter selection for SAES.

     *** HDPL management ***
    ============================================================
        [..]

      (+) Call the function HAL_BSEC_GetHDPLValue() to get the current HDPL.

      (+) Call the function HAL_BSEC_IncrementHDPLValue() to increment the HDPL.

      (+) Call the function HAL_BSEC_ConfigSAESHDPLIncrementValue() to configure
    the increment of HDPL sent to SAES.

      (+) Call the function HAL_BSEC_GetSAESHDPLIncrementValue() to get the SAES
    increment to HDPL.

         *** Scratch register management ***
    ============================================================
    [..]

      (+) Call the function HAL_BSEC_WriteScratchValue() to write a value in
    write once scratch or scratch register.

      (+) Call the function HAL_BSEC_ReadScratchValue() to read a value from
    write once scratch or scratch register.


         *** Debug management ***
    ============================================================
    [..]

      (+) Call the function HAL_BSEC_GetDebugRequest() to get the debug
    request of host debugger.

      (+) Call the function HAL_BSEC_SendJTAGData() to send data via JTAG.

      (+) Call the function HAL_BSEC_ReceiveJTAGData() to receive data
    via JTAG.

      (+) Call the function HAL_BSEC_ConfigDebug() to configure the HDPL,
    secure and non-secure authorization for debugger.

      (+) Call the function HAL_BSEC_GetDebugConfig() to get the HDPL,
    secure and non-secure authorization of debug.

      (+) Call the function HAL_BSEC_LockDebug() to lock the debug.

      (+) Call the function HAL_BSEC_UnlockDebug() to unlock the debug.

      (+) Call the function HAL_BSEC_GetDebugLockState() to get the debug
    lock status.

         *** DHUK management ***
    ============================================================
        [..]

      (+) Call the function HAL_BSEC_GetDHUKValidity() to get the validity
    of DHUK.

      (+) Call the function HAL_BSEC_LockDHUKUse() to lock the DHUK use.

      (+) Call the function HAL_BSEC_GetDHUKLockStatus() to get the DHUK
    lock status.

         *** Reset counter ***
    ============================================================
        [..]

      (+) Call the function HAL_BSEC_GetNumberOfResets() to get the number
    of hot or warm resets.
  @endverbatim

  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#if defined(HAL_BSEC_MODULE_ENABLED)

/** @defgroup BSEC BSEC
  * @brief BSEC HAL module driver.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup BSEC_Private_Constants BSEC Private Constants
  * @{
  */
#define BSEC_HDPL_INCREMENT_CODE     0x60B166E7U
#define BSEC_NB_FUSES                376U
#define BSEC_LIMIT_UPPER_FUSES       256U
#define BSEC_NB_SHADOW_REG           BSEC_NB_FUSES
#define BSEC_NB_EPOCH_COUNTER        2U
#define BSEC_NB_SCRATCH_REG          4U
#define BSEC_NB_WOSCR_REG            8U
#define BSEC_OTPSR_RELOAD_ERRORS     (BSEC_OTPSR_DISTURBF | BSEC_OTPSR_DEDF | BSEC_OTPSR_AMEF)
#define BSEC_SHADOW_REG_WRITE_LIMIT  9U
#define BSEC_TIMEOUT                 1000U
/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @defgroup BSEC_Private_Functions BSEC Private Functions
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup BSEC_Exported_Functions BSEC Exported Functions
  * @{
  */

/** @defgroup BSEC_Exported_Functions_Group1 General configuration functions
 *  @brief   General configuration functions
 *
@verbatim
 ===============================================================================
                ##### General configuration functions #####
 ===============================================================================
    [..]
  This subsection provides a set of functions allowing to perform global
  configuration of BSEC.

@endverbatim
  * @{
  */

/**
  * @brief  Get the status of BSEC peripheral.
  *
  * @param  hbsec   BSEC handle
  * @param  pStatus  Returned value of BSEC status. The returned value is @ref BSEC_Status
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetStatus(BSEC_HandleTypeDef * hbsec, uint32_t *pStatus)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
 if ((pStatus == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  *pStatus = (hbsec->Instance->OTPSR & (BSEC_OTPSR_INIT_DONE | BSEC_OTPSR_HIDEUP | BSEC_OTPSR_OTPNVIR));

  return HAL_OK;
}

/**
  * @brief  Lock the write to BSEC registers.
  *
  * @param  hbsec  BSEC handle
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GlobalLock(BSEC_HandleTypeDef *hbsec)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
  if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Set the value of global lock */
  SET_BIT(hbsec->Instance->LOCKR, BSEC_LOCKR_GWLOCK);

  return HAL_OK;
}

/**
  * @brief  Get the global write registers lock status.
  *
  * @param  hbsec  BSEC handle
  * @param  pStatus  Returned value of global lock status. The returned value is @ref BSEC_Global_Lock
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetGlobalLockStatus(BSEC_HandleTypeDef * hbsec, uint32_t *pStatus)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pStatus == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Get the global lock status */
  *pStatus = (hbsec->Instance->LOCKR & BSEC_LOCKR_GWLOCK);

  return HAL_OK;
}

/**
  * @brief  Get the error code of BSEC driver.
  *
  * @param  hbsec  BSEC handle
  * @param  pError  Returned value of error code. The returned value is @ref BSEC_Error_Code
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetErrorCode(BSEC_HandleTypeDef * hbsec, uint32_t *pError)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value */
 if (pError == NULL)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  *pError = hbsec->ErrorCode;

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup BSEC_Exported_Functions_Group2 OTP fuse management functions
 *  @brief   OTP fuse management functions
 *
@verbatim
 ===============================================================================
                  ##### OTP fuse management functions #####
 ===============================================================================
    [..]
  This subsection provides a set of functions allowing to manage the OTP fuses.

@endverbatim
  * @{
  */

/**
  * @brief  Read OTP fuse value.
  *
  * @param  hbsec     BSEC handle
  * @param  FuseId     Fuse to be read, this parameter value is between 0 and BSEC_NB_FUSES-1
  * @param  pFuseData  Returned value of fuse. The returned value is between 0 and 0xFFFFFFFFU
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_OTP_Read(BSEC_HandleTypeDef * hbsec, uint32_t FuseId, uint32_t *pFuseData)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
 if ((pFuseData == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  if (FuseId < BSEC_NB_FUSES)
  {
    /* Reload the data :
       - Unshadowed fuse are not automatically reload and data no more available after register read
       - Shadowed fuse contains by default the shadow value in the register */
    if (HAL_BSEC_OTP_Reload(hbsec, FuseId) == HAL_OK)
    {
      /* Read data from shadow register */
      *pFuseData = hbsec->Instance->FVRw[FuseId];
    }
    else
    {
      return HAL_ERROR;
    }
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Program an OTP fuse with or without permanent lock.
  *
  * @param  hbsec    BSEC handle
  * @param  FuseId    Fuse to be written, this parameter value is between 0 and BSEC_NB_FUSES-1
  * @param  FuseData  Data to be written in fuse, this parameter value is between 0 and 0xFFFFFFFFU
  * @param  Lock      Permanent lock value, this parameter is @ref BSEC_Permanent_Lock
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_OTP_Program(BSEC_HandleTypeDef *hbsec, uint32_t FuseId, uint32_t FuseData, uint32_t Lock)
{
  uint32_t status_reg;
  uint32_t status_bit;
  uint32_t read_data;
  uint32_t tick_start = HAL_GetTick();

  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
 if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Check the permanent lock */
  assert_param(IS_BSEC_PERMANENT_LOCK(Lock));

  /* Get the correct register value */
  if (FuseId < BSEC_NB_FUSES)
  {
    status_reg = FuseId / 32U;
    status_bit = (uint32_t)(1UL << (FuseId % 32U));

    if ((hbsec->Instance->SPLOCKx[status_reg] & status_bit) == 0U)
    {
      /* Write data in register */
      hbsec->Instance->WDR = FuseData;

      /* Perform a program of the fuse register */
      MODIFY_REG(hbsec->Instance->OTPCR, (BSEC_OTPCR_PPLOCK | BSEC_OTPCR_PROG | BSEC_OTPCR_ADDR),
                                         (FuseId | BSEC_OTPCR_PROG | Lock));

      /* Wait the operation is finished */
      while ((hbsec->Instance->OTPSR & BSEC_OTPSR_BUSY) != 0U)
      {
        if ((HAL_GetTick() - tick_start) > BSEC_TIMEOUT)
        {
          hbsec->ErrorCode = HAL_BSEC_ERROR_TIMEOUT;
          return HAL_ERROR;
        }
      }

      /* Check programming errors */
      if ((hbsec->Instance->OTPSR & BSEC_OTPSR_PROGFAIL) != 0U)
      {
        hbsec->ErrorCode = HAL_BSEC_ERROR_PROGFAIL;
        return HAL_ERROR;
      }

      /* Read back programmed data */
      if (HAL_BSEC_OTP_Read(hbsec, FuseId, &read_data) == HAL_OK)
      {
        /* Verify programmed data */
        if (read_data != FuseData)
        {
          hbsec->ErrorCode = HAL_BSEC_ERROR_PROGFAIL;
          return HAL_ERROR;
        }
      }
      else
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* Fuse is sticky programming locked */
      hbsec->ErrorCode = HAL_BSEC_ERROR_LOCK;
      return HAL_ERROR;
    }
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Reload a OTP fuse value.
  *
  * @param  hbsec  BSEC handle
  * @param  FuseId  Fuse to be reload, this parameter value is between 0 and BSEC_NB_FUSES-1
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_OTP_Reload(BSEC_HandleTypeDef *hbsec, uint32_t FuseId)
{
  uint32_t status_reg;
  uint32_t status_bit;
  uint32_t tick_start = HAL_GetTick();

  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
 if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  if (FuseId < BSEC_NB_FUSES)
  {
    status_reg = FuseId / 32U;
    status_bit = (uint32_t)(1UL << (FuseId % 32U));

    if ((hbsec->Instance->SRLOCKx[status_reg] & status_bit) == 0U)
    {
      /* Perform a reload of the fuse register */
      MODIFY_REG(hbsec->Instance->OTPCR, (BSEC_OTPCR_PPLOCK | BSEC_OTPCR_PROG | BSEC_OTPCR_ADDR), FuseId);

      /* Wait the operation is finished */
      while ((hbsec->Instance->OTPSR & BSEC_OTPSR_BUSY) != 0U)
      {
        if ((HAL_GetTick() - tick_start) > BSEC_TIMEOUT)
        {
          hbsec->ErrorCode = HAL_BSEC_ERROR_TIMEOUT;
          return HAL_ERROR;
        }
      }

      if ((hbsec->Instance->OTPSR & BSEC_OTPSR_RELOAD_ERRORS) != 0U)
      {
        /* An error occurred during reloading, value can't be relied on */
        hbsec->ErrorCode = (hbsec->Instance->OTPSR & BSEC_OTPSR_RELOAD_ERRORS);
        return HAL_ERROR;
      }
    }
    else
    {
      /* Shadow register is sticky reload locked */
      hbsec->ErrorCode = HAL_BSEC_ERROR_LOCK;
      return HAL_ERROR;
    }
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Sticky lock an OTP fuse programming, writing shadowed register or reload.
  *
  * @param  hbsec  BSEC handle
  * @param  FuseId  Fuse to lock, this parameter value is between 0 and BSEC_NB_FUSES-1
  * @param  Lock    Lock status of the fuse, this parameter is a combination of
  *                   @ref HAL_BSEC_FUSE_PROG_LOCKED,
  *                   @ref HAL_BSEC_FUSE_WRITE_LOCKED,
  *                   @ref HAL_BSEC_FUSE_RELOAD_LOCKED
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_OTP_Lock(BSEC_HandleTypeDef *hbsec, uint32_t FuseId, uint32_t Lock)
{
  uint32_t status_reg;
  uint32_t status_bit;

  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
 if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Check the lock configuration */
  assert_param(IS_BSEC_LOCK_CFG(Lock));

  if (FuseId < BSEC_NB_FUSES)
  {
    status_reg = FuseId / 32U;
    status_bit = (uint32_t)(1UL << (FuseId % 32U));

    if ((Lock & HAL_BSEC_FUSE_PROG_LOCKED) != 0U)
    {
      /* Programming lock */
      SET_BIT(hbsec->Instance->SPLOCKx[status_reg], status_bit);
    }

    if ((Lock & HAL_BSEC_FUSE_WRITE_LOCKED) != 0U)
    {
      /* Write lock */
      SET_BIT(hbsec->Instance->SWLOCKx[status_reg], status_bit);
    }

    if ((Lock & HAL_BSEC_FUSE_RELOAD_LOCKED) != 0U)
    {
      /* Reload lock */
      SET_BIT(hbsec->Instance->SRLOCKx[status_reg], status_bit);
    }
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Get the sticky lock status (programming, writing shadowed register
  *         or reload), the permanent lock status, the shadow configuration,
  *         the ECC or hidden status of an OTP fuse.
  *
  * @param  hbsec  BSEC handle
  * @param  FuseId  Fuse to get the state, this parameter value is between 0 and BSEC_NB_FUSES-1
  * @param  pState  Returned value of state. The returned value is a combination of @ref BSEC_State
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_OTP_GetState(BSEC_HandleTypeDef * hbsec, uint32_t FuseId, uint32_t *pState)
{
  uint32_t status_reg;
  uint32_t status_bit;
  uint32_t otpsr_reg;
  uint32_t state;

  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
 if ((pState == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  if (FuseId < BSEC_NB_FUSES)
  {
    status_reg = FuseId / 32U;
    status_bit = (uint32_t)(1UL << (FuseId % 32U));

    /* Initialize return value */
    state = 0U;

    /* Check sticky programming lock */
    if ((hbsec->Instance->SPLOCKx[status_reg] & status_bit) != 0U)
    {
      state |= HAL_BSEC_FUSE_PROG_LOCKED;
    }

    /* Check sticky write lock */
    if ((hbsec->Instance->SWLOCKx[status_reg] & status_bit) != 0U)
    {
      state |= HAL_BSEC_FUSE_WRITE_LOCKED;
    }

    /* Check sticky reload lock */
    if ((hbsec->Instance->SRLOCKx[status_reg] & status_bit) != 0U)
    {
      state |= HAL_BSEC_FUSE_RELOAD_LOCKED;
    }
    else
    {
      /* Check permanent lock : only in case of reload allowed as OTPSR reflect
         status of most recently read word */
      if (HAL_BSEC_OTP_Reload(hbsec, FuseId) == HAL_OK)
      {
        otpsr_reg = hbsec->Instance->OTPSR;

        if ((otpsr_reg & BSEC_OTPSR_PPLF) != 0u)
        {
          /* Permanent programming lock detected */
          state |= HAL_BSEC_FUSE_LOCKED;
        }
        else if ((otpsr_reg & BSEC_OTPSR_PPLMF) != 0U)
        {
          /* Permanent programming lock on the two lower fuse values in the array are not identical */
          hbsec->ErrorCode = HAL_BSEC_ERROR_PPLM;
          return HAL_ERROR;
        }
        else
        {
          /* No permanent programming lock detected */
        }
      }
    }

    /* Check shadow configuration */
    if ((hbsec->Instance->SFSRx[status_reg] & status_bit) != 0U)
    {
      state |= HAL_BSEC_FUSE_SHADOWED;
    }

    /* Check hidden status */
    if (((hbsec->Instance->OTPSR & BSEC_OTPSR_HIDEUP) != 0U) &&
        (FuseId >= BSEC_LIMIT_UPPER_FUSES))
    {
      state |= HAL_BSEC_FUSE_HIDDEN;
    }

    /* Check ECC errors on all OTP fuses */
    state |= (hbsec->Instance->OTPSR & (BSEC_OTPSR_OTPERR | BSEC_OTPSR_OTPSEC));

  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }
  *pState = state;
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup BSEC_Exported_Functions_Group3 Shadow fuse register management functions
 *  @brief   Shadow fuse register management functions
 *
@verbatim
 ===============================================================================
             ##### Shadow fuse register management functions #####
 ===============================================================================
    [..]
  This subsection provides a set of functions allowing to manage the
  shadow fuse registers.

@endverbatim
  * @{
  */

/**
  * @brief  Read the value of a shadow fuse register.
  *
  * @param  hbsec    BSEC handle
  * @param  RegId     Shadow register to be read, this parameter value is between 0 and BSEC_NB_FUSES-1
  * @param  pRegData  Returned value of register data. The returned value is between 0 and 0xFFFFFFFFU
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_OTP_ReadShadow(BSEC_HandleTypeDef * hbsec, uint32_t RegId, uint32_t *pRegData)
{
  uint32_t status_reg;
  uint32_t status_bit;

  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pRegData == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  if (RegId < BSEC_NB_SHADOW_REG)
  {
    status_reg = RegId / 32U;
    status_bit = (uint32_t)(1UL << (RegId % 32U));

    if ((hbsec->Instance->SFSRx[status_reg] & status_bit) != 0U)
    {
      /* Read data from shadow register */
      *pRegData = hbsec->Instance->FVRw[RegId];
    }
    else
    {
      /* Fuse is not shadowed */
      hbsec->ErrorCode = HAL_BSEC_ERROR_UNALLOWED;
      return HAL_ERROR;
    }
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Write in a shadow fuse register.
  *
  * @param  hbsec    BSEC handle
  * @param  RegId     Shadow register to be written, this parameter value is between 0 and BSEC_NB_FUSES-1
  * @param  RegData   Data to be written in shadow register, this parameter value is between 0 and 0xFFFFFFFFU
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_OTP_WriteShadow(BSEC_HandleTypeDef *hbsec, uint32_t RegId, uint32_t RegData)
{
  uint32_t status_reg;
  uint32_t status_bit;

  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
 if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Get the correct register value */
  if ((RegId > BSEC_SHADOW_REG_WRITE_LIMIT) && (RegId < BSEC_NB_SHADOW_REG))
  {
    status_reg = RegId / 32U;
    status_bit = (uint32_t)(1UL << (RegId % 32U));

    if ((hbsec->Instance->SFSRx[status_reg] & status_bit) != 0U)
    {
      if ((hbsec->Instance->SWLOCKx[status_reg] & status_bit) == 0U)
      {
        /* Value is written in register */
        hbsec->Instance->FVRw[RegId] = RegData;
      }
      else
      {
        /* Shadow register is sticky write locked */
        hbsec->ErrorCode = HAL_BSEC_ERROR_LOCK;
        return HAL_ERROR;
      }
    }
    else
    {
      /* Fuse is not shadowed */
      hbsec->ErrorCode = HAL_BSEC_ERROR_UNALLOWED;
      return HAL_ERROR;
    }
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Get the validity status of a shadow register.
  *
  * @param  hbsec     BSEC handle
  * @param  RegId      Shadow register to be checked, this parameter value is between 0 and BSEC_NB_FUSES-1
  * @param  pValidity  Returned value of validity status. The returned value is @ref BSEC_Reload_Validity
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_OTP_GetShadowState(BSEC_HandleTypeDef * hbsec, uint32_t RegId, uint32_t *pValidity)
{
  uint32_t status_reg;
  uint32_t status_bit;

  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
 if ((pValidity == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  if (RegId < BSEC_NB_SHADOW_REG)
  {
    status_reg = RegId / 32U;
    status_bit = (uint32_t)(1UL << (RegId % 32U));

    if ((hbsec->Instance->SFSRx[status_reg] & status_bit) != 0U)
    {
      if ((hbsec->Instance->OTPVLDRx[status_reg] & status_bit) != 0U)
      {
        *pValidity = HAL_BSEC_RELOAD_DONE;
      }
      else
      {
        *pValidity = HAL_BSEC_RELOAD_ERROR;
      }
    }
    else
    {
      /* Fuse is not shadowed */
      hbsec->ErrorCode = HAL_BSEC_ERROR_UNALLOWED;
      return HAL_ERROR;
    }
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup BSEC_Exported_Functions_Group4 Device lifecycle management functions
 *  @brief   Device lifecycle management functions
 *
@verbatim
 ===============================================================================
               ##### Device lifecycle management functions #####
 ===============================================================================
    [..]
  This subsection provides a set of functions allowing to manage the
  device lifecycle.

@endverbatim
  * @{
  */

/**
  * @brief  Get the BSEC device lifecycle state.
  *
  * @param  hbsec  BSEC handle
  * @param  pState  Returned value of device lifecycle state. The returned value is @ref BSEC_Lifecycle_State
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetDeviceLifeCycleState(BSEC_HandleTypeDef * hbsec, uint32_t *pState)
{

  uint32_t state;
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
 if ((pState == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Return the state */
  state = (hbsec->Instance->SR & BSEC_SR_NVSTATE);

  if (IS_BSEC_STATE(state))
  {
    *pState = state;
    return HAL_OK;
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_UNDEFINED_VALUE;
    return HAL_ERROR;
  }
}

/**
  * @brief  Read the value of epoch counter.
  *
  * @param  hbsec        BSEC handle
  * @param  CounterId     Identifier of the epoch counter to be read, this parameter can be @ref BSEC_Epoch_Select
  * @param  pCounterData  Returned value of epoch counter data. The returned value is between 0 and 0xFFFFFFFFU
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_ReadEpochCounter(BSEC_HandleTypeDef * hbsec, uint32_t CounterId, uint32_t *pCounterData)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
 if ((pCounterData == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Get the correct register value */
  if (CounterId < BSEC_NB_EPOCH_COUNTER)
  {
    *pCounterData = hbsec->Instance->EPOCHRx[CounterId];
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Select the epoch counter used by SAES.
  *
  * @param  hbsec            BSEC handle
  * @param  SelectedCounter   Epoch selected counter, this parameter can be @ref BSEC_Epoch_Select
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_SelectEpochCounter(BSEC_HandleTypeDef *hbsec, uint32_t SelectedCounter)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
 if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Check the epoch counter selection */
  assert_param(IS_BSEC_EPOCHSEL(SelectedCounter));

  /* Update the selected epoch counter value */
  MODIFY_REG(hbsec->Instance->EPOCHSELR, BSEC_EPOCHSELR_EPSEL, SelectedCounter);

  return HAL_OK;
}

/**
  * @brief  Get the epoch counter selection for SAES.
  *
  * @param  hbsec            BSEC handle
  * @param  pSelectedCounter  Returned value of epoch selected counter. The returned value is @ref BSEC_Epoch_Select
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetEpochCounterSelection(BSEC_HandleTypeDef * hbsec, uint32_t *pSelectedCounter)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pSelectedCounter == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Return the debug request */
  *pSelectedCounter = (hbsec->Instance->EPOCHSELR & BSEC_EPOCHSELR_EPSEL);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup BSEC_Exported_Functions_Group5 HDPL management functions
 *  @brief   HDPL management functions
 *
@verbatim
 ===============================================================================
                     ##### HDPL management functions #####
 ===============================================================================
    [..]
  This subsection provides a set of functions allowing to manage the
  HDPL (Hide Protection Level).

@endverbatim
  * @{
  */

/**
  * @brief  Get the current HDPL.
  *
  * @param  hbsec  BSEC handle
  * @param  pHDPL   Returned value of current HDPL. The returned value is @ref BSEC_HDPL
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetHDPLValue(BSEC_HandleTypeDef * hbsec, uint32_t *pHDPL)
{

  uint32_t hdpl;
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pHDPL == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Return the HDPL */
  hdpl = (hbsec->Instance->HDPLSR & BSEC_HDPLSR_HDPL);

  if (IS_BSEC_HDPL(hdpl))
  {
    *pHDPL = hdpl;
    return HAL_OK;
  }
  else
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_UNDEFINED_VALUE;
    return HAL_ERROR;
  }
}

/**
  * @brief  Increment the HDPL.
  *
  * @param  hbsec  BSEC handle
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_IncrementHDPLValue(BSEC_HandleTypeDef *hbsec)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
 if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Increment HDPL value */
  hbsec->Instance->HDPLCR = BSEC_HDPL_INCREMENT_CODE;

  return HAL_OK;
}

/**
  * @brief  Configure the increment of HDPL sent to SAES.
  *
  * @param  hbsec      BSEC handle
  * @param  Increment   Value of HDPL increment, this parameter can be a value of @ref BSEC_INCR_HDPL
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_ConfigSAESHDPLIncrementValue(BSEC_HandleTypeDef *hbsec, uint32_t Increment)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
 if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Check the debug configuration */
  assert_param(IS_BSEC_NEXTHDPL(Increment));

  /* Write HDPL increment value */
  MODIFY_REG(hbsec->Instance->NEXTLR, BSEC_NEXTLR_INCR, Increment);

  return HAL_OK;
}

/**
  * @brief  Get the SAES increment to HDPL.
  *
  * @param  hbsec      BSEC handle
  * @param  pIncrement  Returned value of HDPL increment. The returned value is a value of @ref BSEC_INCR_HDPL
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetSAESHDPLIncrementValue(BSEC_HandleTypeDef * hbsec, uint32_t *pIncrement)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pIncrement == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Return the debug request */
  *pIncrement = (hbsec->Instance->NEXTLR & BSEC_NEXTLR_INCR);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup BSEC_Exported_Functions_Group6 Scratch registers management functions
 *  @brief   Scratch registers management functions
 *
@verbatim
 ===============================================================================
               ##### Scratch registers management functions #####
 ===============================================================================
    [..]
  This subsection provides a set of functions allowing to manage the
  write once scratch and scratch registers.

@endverbatim
  * @{
  */

/**
  * @brief  Write a value in write once scratch or scratch register.
  *
  * @param  hbsec     BSEC handle
  * @param  pRegAddr  Register to be written
  * @param  Value     Value to be written, this parameter can be a value between 0 and 0xFFFFFFFFU
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_WriteScratchValue(BSEC_HandleTypeDef *hbsec, const BSEC_ScratchRegTypeDef *pRegAddr, uint32_t Value)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the register configuration and instance */
 if ((pRegAddr == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Check the debug configuration */
  assert_param(IS_BSEC_REGTYPE(pRegAddr->RegType));

  /* Write data in correct register */
  if (pRegAddr->RegType == HAL_BSEC_SCRATCH_REG)
  {
    if (pRegAddr->RegNumber < BSEC_NB_SCRATCH_REG)
    {
      hbsec->Instance->SCRATCHRx[pRegAddr->RegNumber] = Value;
    }
    else
    {
      hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
      return HAL_ERROR;
    }
  }
  else
  {
    if (pRegAddr->RegNumber < BSEC_NB_WOSCR_REG)
    {
      hbsec->Instance->WOSCRx[pRegAddr->RegNumber] = Value;
    }
    else
    {
      hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @brief  Read a value from write once scratch or scratch register.
  *
  * @param  hbsec    BSEC handle
  * @param  pRegAddr  Register to be read
  * @param  pValue  Returned value of register data. The returned value is between 0 and 0xFFFFFFFFU
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_ReadScratchValue(BSEC_HandleTypeDef * hbsec, const BSEC_ScratchRegTypeDef *pRegAddr, uint32_t *pValue)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the register configuration, address of returned value and instance */
 if ((pRegAddr == NULL) || (pValue == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Check the debug configuration */
  assert_param(IS_BSEC_REGTYPE(pRegAddr->RegType));

  /* Get the correct register value */
  if (pRegAddr->RegType == HAL_BSEC_SCRATCH_REG)
  {
    if (pRegAddr->RegNumber < BSEC_NB_SCRATCH_REG)
    {
      *pValue = hbsec->Instance->SCRATCHRx[pRegAddr->RegNumber];
    }
    else
    {
      hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
      return HAL_ERROR;
    }
  }
  else
  {
    if (pRegAddr->RegNumber < BSEC_NB_WOSCR_REG)
    {
      *pValue = hbsec->Instance->WOSCRx[pRegAddr->RegNumber];
    }
    else
    {
      hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup BSEC_Exported_Functions_Group7 Debug management functions
 *  @brief   Debug management functions
 *
@verbatim
 ===============================================================================
                   ##### Debug management functions #####
 ===============================================================================
    [..]
  This subsection provides a set of functions allowing to manage the
  debug functionalities.

@endverbatim
  * @{
  */

/**
  * @brief  Get the debug request of host debugger.
  *
  * @param  hbsec   BSEC handle
  * @param  pDbgReq  Returned value of debug request. The returned value is @ref BSEC_Debug_Req
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetDebugRequest(BSEC_HandleTypeDef * hbsec, uint32_t *pDbgReq)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pDbgReq == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Return the debug request */
  *pDbgReq = (hbsec->Instance->SR & BSEC_SR_DBGREQ);

  return HAL_OK;
}

/**
  * @brief  Send data via JTAG.
  *
  * @param  hbsec  BSEC handle
  * @param  Data    Data to be sent via JTAG, this parameter can be a value between 0 and 0xFFFFFFFFU
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_SendJTAGData(BSEC_HandleTypeDef *hbsec, uint32_t Data)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
  if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Send the data */
  hbsec->Instance->JTAGOUTR = Data;

  return HAL_OK;
}

/**
  * @brief  Receive data via JTAG.
  *
  * @param  hbsec  BSEC handle
  * @param  pData   Returned value of received data. The returned value is between 0 and 0xFFFFFFFFU
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_ReceiveJTAGData(BSEC_HandleTypeDef * hbsec, uint32_t *pData)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pData == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Return the received data */
  *pData = hbsec->Instance->JTAGINR;

  return HAL_OK;
}

/**
  * @brief  Configure the HDPL, secure and non-secure authorization
  *         for debugger.
  *
  * @param  hbsec  BSEC handle
  * @param  pCfg    Configuration of debug
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_ConfigDebug(BSEC_HandleTypeDef *hbsec, const BSEC_DebugCfgTypeDef *pCfg)
{
  uint32_t cfg_reg;

  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the configuration pointer and instance */
  if ((pCfg == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Check the debug configuration */
  assert_param(IS_BSEC_OPENDBG(pCfg->HDPL_Open_Dbg));
  assert_param(IS_BSEC_SECDBGAUTH(pCfg->Sec_Dbg_Auth));
  assert_param(IS_BSEC_NSDBGAUTH(pCfg->NonSec_Dbg_Auth));

  cfg_reg = ((pCfg->HDPL_Open_Dbg   & BSEC_DBGCR_AUTH_HDPL) \
           | (pCfg->Sec_Dbg_Auth    & BSEC_DBGCR_AUTH_SEC)  \
           | (pCfg->NonSec_Dbg_Auth & BSEC_DBGCR_UNLOCK));

  hbsec->Instance->DBGCR = cfg_reg;

  return HAL_OK;
}

/**
  * @brief  Get the HDPL, secure and non-secure authorization of debug.
  *
  * @param  hbsec   BSEC handle
  * @param  pDbgCfg  Returned value of debug configuration
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetDebugConfig(BSEC_HandleTypeDef * hbsec, BSEC_DebugCfgTypeDef *pDbgCfg)
{
  uint32_t cfg_reg;

  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pDbgCfg == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Get the debug configuration */
  cfg_reg = hbsec->Instance->DBGCR;
  pDbgCfg->HDPL_Open_Dbg = (cfg_reg & BSEC_DBGCR_AUTH_HDPL);
  pDbgCfg->Sec_Dbg_Auth = (cfg_reg & BSEC_DBGCR_AUTH_SEC);
  pDbgCfg->NonSec_Dbg_Auth = (cfg_reg & BSEC_DBGCR_UNLOCK);

  return HAL_OK;
}

/**
  * @brief  Lock the debug.
  *
  * @param  hbsec  BSEC handle
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_LockDebug(BSEC_HandleTypeDef *hbsec)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
  if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Set the value of the debug lock */
  MODIFY_REG(hbsec->Instance->AP_UNLOCK, BSEC_AP_UNLOCK_UNLOCK, HAL_BSEC_DEBUG_LOCKED);

  return HAL_OK;
}

/**
  * @brief  Unlock the debug.
  *
  * @param  hbsec  BSEC handle
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_UnlockDebug(BSEC_HandleTypeDef *hbsec)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
  if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Set the value of the debug lock */
  MODIFY_REG(hbsec->Instance->AP_UNLOCK, BSEC_AP_UNLOCK_UNLOCK, HAL_BSEC_DEBUG_UNLOCKED);

  return HAL_OK;
}

/**
  * @brief  Get the debug lock status.
  *
  * @param  hbsec   BSEC handle
  * @param  pStatus  Returned value of debug lock status. The returned value is @ref BSEC_Debug_Lock
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetDebugLockState(BSEC_HandleTypeDef * hbsec, uint32_t *pStatus)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pStatus == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Get the debug lock status */
  *pStatus = (hbsec->Instance->AP_UNLOCK & BSEC_AP_UNLOCK_UNLOCK);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup BSEC_Exported_Functions_Group8 DHUK management functions
 *  @brief   DHUK management functions
 *
@verbatim
 ===============================================================================
                    ##### DHUK management functions #####
 ===============================================================================
    [..]
  This subsection provides a set of functions allowing to manage the
  DHUK (Derived Hardware Unique Key).

@endverbatim
  * @{
  */

/**
  * @brief  Get the validity of DHUK.
  *
  * @param  hbsec     BSEC handle
  * @param  pValidity  Returned value of DHUK validity. The returned value is @ref BSEC_DHUK_Validity
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetDHUKValidity(BSEC_HandleTypeDef * hbsec, uint32_t *pValidity)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pValidity == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Get the DHUK validity */
  *pValidity = (hbsec->Instance->SR & BSEC_SR_HVALID);

  return HAL_OK;
}

/**
  * @brief  Lock the DHUK use.
  *
  * @param  hbsec  BSEC handle
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_LockDHUKUse(BSEC_HandleTypeDef *hbsec)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the instance */
  if (hbsec->Instance != BSEC)
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Set the value of the DHUK lock */
  SET_BIT(hbsec->Instance->LOCKR, BSEC_LOCKR_HKLOCK);

  return HAL_OK;
}

/**
  * @brief  Get the DHUK lock status.
  *
  * @param  hbsec   BSEC handle
  * @param  pStatus  Returned value of DHUK lock status. The returned value is @ref BSEC_DHUK_Lock
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetDHUKLockStatus(BSEC_HandleTypeDef * hbsec, uint32_t *pStatus)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pStatus == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Get the DHUK lock status */
  *pStatus = (hbsec->Instance->LOCKR & BSEC_LOCKR_HKLOCK);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup BSEC_Exported_Functions_Group9 Reset management functions
 *  @brief   Reset management functions
 *
@verbatim
 ===============================================================================
                    ##### Reset management functions #####
 ===============================================================================
    [..]
  This subsection provides a set of functions allowing to manage the reset.

@endverbatim
  * @{
  */

/**
  * @brief  Get the number of hot or warm resets.
  *
  * @param  hbsec        BSEC handle
  * @param  ResetType     Type of the reset, this parameter can be a value of @ref BSEC_Reset_Type
  * @param  pResetNumber  Returned value of number of reset. The returned value is between 0 and 0xFFFFFFFFU
  *
  * @retval HAL_StatusTypeDef HAL Status
  */
HAL_StatusTypeDef HAL_BSEC_GetNumberOfResets(BSEC_HandleTypeDef * hbsec, uint32_t ResetType, uint32_t *pResetNumber)
{
  /* Check the handle pointer */
  if (hbsec == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the address of returned value and instance */
  if ((pResetNumber == NULL) || (hbsec->Instance != BSEC))
  {
    hbsec->ErrorCode = HAL_BSEC_ERROR_INVALID_PARAM;
    return HAL_ERROR;
  }

  /* Check the reset type */
  assert_param(IS_BSEC_RESETTYPE(ResetType));

  /* Get the correct register value */
  if (ResetType == HAL_BSEC_HOT_RESET)
  {
    *pResetNumber = hbsec->Instance->HRCR;
  }
  else
  {
    *pResetNumber = hbsec->Instance->WRCR;
  }

  return HAL_OK;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /* HAL_BSEC_MODULE_ENABLED */

/**
  * @}
  */

