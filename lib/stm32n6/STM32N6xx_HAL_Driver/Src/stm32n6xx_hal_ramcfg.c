/**
  ******************************************************************************
  * @file    stm32n6xx_hal_ramcfg.c
  * @author  GPM Application Team
  * @brief   RAMCFG HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the RAMs configuration controller peripheral:
  *           + RAMCFG Initialization and De-initialization Functions.
  *           + RAMCFG ECC Operation Functions.
  *           + RAMCFG Erase Operation Functions.
  *           + RAMCFG Handle Interrupt and Callbacks Functions.
  *           + RAMCFG State and Error Functions.
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
                    ##### RAMCFG Peripheral features #####
  ==============================================================================
  [..]
    (+) Each SRAM is managed by a RAMCFG instance (AHBSRAM1/2, AXISRAM1 to 6,
        BKPSRAM, FLEXRAM, and VENCRAM).

    (+) Each SRAM can be erased independently through its RAMCFG instance.

    (+) AXISRAM 1 to 6 are the main SRAMs. AHBSRAM1/2 are preferably for DMA-controlled
        peripheral-to-memory data flow.
        AXISRAM 2 to 6 can be shut down when the application is in Run mode.

    (+) FLEXMEM can be either allocated as Cortex-M55 TCM, or as system RAM
        (FLEXRAM).
        80 Kbytes of the FLEXMEM are retained in Standby mode, either allocated as
        extended ITCM (64 +16-Kbyte ECC), or allocated as FLEXRAM.

    (+) The BKPSRAM content is retained in low-power modes, even when VDD is off
        in VBAT mode. On a tamper event detection, the BKPSRAM content is erased.

    (+) The VENCRAM implements hardware and software erases.

    (+) FLEXRAM and BKPRAM support ECC correction feature:
             (++) Single error detection and correction with interrupt generation.
             (++) Double error detection with interrupt generation.
             (++) Status with failing address
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
  [..]
    (#) Call HAL_RAMCFG_Init() to initialize the RAMCFG peripheral before using
       any feature. Call HAL_RAMCFG_DeInit() to de-initialize the RAMCFG when
       using this peripheral is no more needed or a hardware issue has occurred.
          (+) HAL_RAMCFG_Init() and HAL_RAMCFG_DeInit() APIs do not change the
              activation status of ECC feature. It is managed by
              HAL_RAMCFG_StartECC(), HAL_RAMCFG_StopECC() or option bytes (When
              available on the device).

     *** ECC feature ***
     ===================
    [..]
          (+) Call HAL_RAMCFG_StartECC() and HAL_RAMCFG_StopECC() to enable and
              disable ECC hardware mechanism.
                    (++) When ECC feature is previously enabled (case of option
                         byte activation), calling HAL_RAMCFG_StartECC() is
                         recommended to enable the ECC address latching feature.

          (+) Call HAL_RAMCFG_EnableNotification() and HAL_RAMCFG_DisableNotification()
              to enable and disable ECC interrupts. Interrupts can be:
                    (++) Single error interrupt.
                    (++) Double error interrupt.
                    (++) Double error interrupt redirected to Non maskable
                          interrupt (NMI).

          (+) Call HAL_RAMCFG_GetSingleErrorAddress() to get the address of the
              last fail RAM word detected (only for single error) and
              call HAL_RAMCFG_GetDoubleErrorAddress() to get the address of the
              last fail RAM word detected (only for double error).

          (+) Call HAL_RAMCFG_IsECCSingleErrorDetected() to check if an ECC single
              error was detected.
              Call HAL_RAMCFG_IsECCDoubleErrorDetected() to check if an ECC double
              error was detected.
              These APIs are used in silent mode (No ECC interrupt
              is enabled).

     *** Erase feature ***
     =====================
    [..]
          (+) Call HAL_RAMCFG_Erase() to launch a hardware erase for the given
              SRAM.

          (+) The erase value is equal to 0 when launching erase hardware through
              RAMCFG.

          (+) SRAM2 write protected pages are erased when performing an erase
              through RAMCFG.

     *** RAMCFG HAL driver macros list ***
     =====================================
     [..]
       Below the list of used macros in RAMCFG HAL driver.

      (+) __HAL_RAMCFG_ENABLE_IT     : Enable the specified RAMCFG interrupts.
      (+) __HAL_RAMCFG_DISABLE_IT    : Disable the specified RAMCFG interrupts.
      (+) __HAL_RAMCFG_GET_FLAG      : Get the RAMCFG pending flags.
      (+) __HAL_RAMCFG_CLEAR_FLAG    : Clear the RAMCFG pending flags.
      (+) __HAL_RAMCFG_GET_IT_SOURCE : Check whether the specified RAMCFG
                                       interrupt source is enabled or not.
  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup RAMCFG RAMCFG
  * @brief RAMCFG HAL module driver
  * @{
  */

#ifdef HAL_RAMCFG_MODULE_ENABLED

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/

/** @addtogroup RAMCFG_Private_Constants
  * @{
  */
#define RAMCFG_TIMEOUT_VALUE 50000U
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup RAMCFG_Exported_Functions
  * @{
  */

/** @addtogroup RAMCFG_Exported_Functions_Group1
  *
@verbatim
 ===============================================================================
             ##### Initialization and de-initialization Functions  #####
 ===============================================================================
    [..]
      This section provides functions allowing to initialize and de-initialize the
      RAMCFG instance.
    [..]
      The HAL_RAMCFG_Init() function follows the RAMCFG instance configuration
      procedures as described in the reference manual.
      The HAL_RAMCFG_DeInit() function allows to deinitialize the RAMCFG instance.
      HAL_RAMCFG_Init() and HAL_RAMCFG_DeInit() APIs do not change the activation
      status of ECC feature. It is managed by HAL_RAMCFG_StartECC(),
      HAL_RAMCFG_StopECC() or option bytes (When available on the device).

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the RAMCFG by clearing flags and disabling interrupts.
  * @param  hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that contains
  *                   the configuration information for the specified RAMCFG
  *                   instance.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_RAMCFG_Init(RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the RAMCFG peripheral handle */
  if (hramcfg == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_RAMCFG_ALL_INSTANCE(hramcfg->Instance));

  /* Update RAMCFG peripheral state */
  hramcfg->State = HAL_RAMCFG_STATE_BUSY;

#if (USE_HAL_RAMCFG_REGISTER_CALLBACKS == 1)
  /* Check if a valid MSP API was registered */
  if (hramcfg->MspInitCallback == NULL)
  {
    /* Init the low level hardware */
    hramcfg->MspInitCallback = HAL_RAMCFG_MspInit;
  }

  /* Init the low level hardware */
  hramcfg->MspInitCallback(hramcfg);
#else
  HAL_RAMCFG_MspInit(hramcfg);
#endif /* USE_HAL_RAMCFG_REGISTER_CALLBACKS */

  /* Disable the ECC Address latch */
  hramcfg->Instance->CR &= ~(RAMCFG_CR_ALE);

  /* Disable all RAMCFG interrupts */
  __HAL_RAMCFG_DISABLE_IT(hramcfg, RAMCFG_IT_ALL);

  /* Clear RAMCFG monitor flags */
  __HAL_RAMCFG_CLEAR_FLAG(hramcfg, RAMCFG_FLAGS_ALL);

  /* Initialize the RAMCFG error code */
  hramcfg->ErrorCode = HAL_RAMCFG_ERROR_NONE;

  /* Initialize the RAMCFG state */
  hramcfg->State = HAL_RAMCFG_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitialize the RAMCFG peripheral.
  * @param  hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that contains
  *                   the configuration information for the specified RAMCFG
  *                   instance.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_RAMCFG_DeInit(RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the RAMCFG peripheral handle */
  if (hramcfg == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_RAMCFG_ALL_INSTANCE(hramcfg->Instance));

  /* Disable the ECC Address latch */
  hramcfg->Instance->CR &= ~(RAMCFG_CR_ALE);

  /* Disable all RAMCFG interrupts */
  __HAL_RAMCFG_DISABLE_IT(hramcfg, RAMCFG_IT_ALL);

  /* Clear RAMCFG monitor flags */
  __HAL_RAMCFG_CLEAR_FLAG(hramcfg, RAMCFG_FLAGS_ALL);

#if (USE_HAL_RAMCFG_REGISTER_CALLBACKS == 1)
  /* Check if a valid MSP API was registered */
  if (hramcfg->MspDeInitCallback != NULL)
  {
    /* Init the low level hardware */
    hramcfg->MspDeInitCallback(hramcfg);
  }

  /* Clean callbacks */
  hramcfg->DetectSingleErrorCallback = NULL;
  hramcfg->DetectDoubleErrorCallback = NULL;
#else
  HAL_RAMCFG_MspDeInit(hramcfg);
#endif /* USE_HAL_RAMCFG_REGISTER_CALLBACKS */

  /* Reset the RAMCFG error code */
  hramcfg->ErrorCode = HAL_RAMCFG_ERROR_NONE;

  /* Reset the RAMCFG state */
  hramcfg->State = HAL_RAMCFG_STATE_RESET;

  return HAL_OK;
}

/**
  * @brief Initialize the RAMCFG MSP.
  * @param hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that contains
  *                  the configuration information for the specified RAMCFG.
  * @retval None.
  */
__weak void HAL_RAMCFG_MspInit(RAMCFG_HandleTypeDef *hramcfg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hramcfg);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RAMCFG_MspInit can be implemented in the user file      */
}

/**
  * @brief DeInitialize the RAMCFG MSP.
  * @param hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that contains
  *                  the configuration information for the specified RAMCFG.
  * @retval None.
  */
__weak void HAL_RAMCFG_MspDeInit(RAMCFG_HandleTypeDef *hramcfg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hramcfg);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RAMCFG_MspDeInit can be implemented in the user file    */
}
/**
  * @}
  */

/** @addtogroup RAMCFG_Exported_Functions_Group2
  *
@verbatim
 ===============================================================================
                      ##### ECC Operations Functions  #####
 ===============================================================================
    [..]
      This section provides functions allowing to manage ECC feature provided by
      the RAMCFG peripheral.
    [..]
      The HAL_RAMCFG_StartECC() function allows starting the ECC mechanism and
      enabling ECC address latching for the selected RAMCFG instance.
      The HAL_RAMCFG_StopECC() function allows stopping the ECC mechanism and
      disabling ECC address latching for the selected RAMCFG instance.
      The HAL_RAMCFG_EnableNotification() function allows enabling interrupts
      for single ECC error, double ECC error and NMI error.
      The HAL_RAMCFG_DisableNotification() function allows disabling interrupts
      for single ECC error, double ECC error. When NMI interrupt is enabled it
      can only be disabled by a global peripheral reset or by a system reset.
      The HAL_RAMCFG_IsECCSingleErrorDetected() function allows to check if an
      single ECC error has occurred.
      The HAL_RAMCFG_IsECCDoubleErrorDetected() function allows to check if an
      double ECC error has occurred.
      The HAL_RAMCFG_GetSingleErrorAddress() function allows to get the address of
      the last single ECC error detected.
      The HAL_RAMCFG_GetDoubleErrorAddress() function allows to get the address of
      the last double ECC error detected.

@endverbatim
  * @{
  */

/**
  * @brief  Start ECC mechanism for the given SRAM.
  * @param  hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that contains
  *                   the configuration information for the specified RAMCFG
  *                   instance.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_RAMCFG_StartECC(RAMCFG_HandleTypeDef *hramcfg)
{
  HAL_StatusTypeDef status = HAL_OK;
  /* Check the parameters */
  assert_param(IS_RAMCFG_ECC_INSTANCE(hramcfg->Instance));

  /* Check RAMCFG state */
  if (hramcfg->State == HAL_RAMCFG_STATE_READY)
  {
    /* Update RAMCFG peripheral state */
    hramcfg->State = HAL_RAMCFG_STATE_BUSY;

    /* Check if ECC mechanism is non active */
    if ((hramcfg->Instance->CR & RAMCFG_CR_ECCE) != RAMCFG_CR_ECCE)
    {
      /* Start the SRAM ECC mechanism and latching the error address */
      hramcfg->Instance->CR |= (RAMCFG_CR_ECCE | RAMCFG_CR_ALE);
    }
    else
    {
      /* Start latching the error address */
      hramcfg->Instance->CR |= RAMCFG_CR_ALE;
    }

    /* Update the RAMCFG state */
    hramcfg->State = HAL_RAMCFG_STATE_READY;
  }
  else
  {
    /* Update the RAMCFG error code and return error  */
    hramcfg->ErrorCode = HAL_RAMCFG_ERROR_BUSY;
    status = HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Stop ECC mechanism for the given SRAM.
  * @param  hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that contains
  *                   the configuration information for the specified RAMCFG
  *                   instance.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_RAMCFG_StopECC(RAMCFG_HandleTypeDef *hramcfg)
{
  HAL_StatusTypeDef status = HAL_OK;
  /* Check the parameters */
  assert_param(IS_RAMCFG_ECC_INSTANCE(hramcfg->Instance));

  /* Check RAMCFG state */
  if (hramcfg->State == HAL_RAMCFG_STATE_READY)
  {
    /* Update RAMCFG peripheral state */
    hramcfg->State = HAL_RAMCFG_STATE_BUSY;

    /* Check if ECC mechanism is active */
    if ((hramcfg->Instance->CR & RAMCFG_CR_ECCE) == RAMCFG_CR_ECCE)
    {
      /* Unlock the SRAM ECC bit */
      WRITE_REG(hramcfg->Instance->ECCKEYR, RAMCFG_ECC_KEY1);
      WRITE_REG(hramcfg->Instance->ECCKEYR, RAMCFG_ECC_KEY2);

      /* Stop the SRAM ECC mechanism and latching the error address */
      hramcfg->Instance->CR &= ~(RAMCFG_CR_ECCE | RAMCFG_CR_ALE);
    }
    /* Update the RAMCFG state */
    hramcfg->State = HAL_RAMCFG_STATE_READY;
  }
  else
  {
    /* Update the RAMCFG error code and return error  */
    hramcfg->ErrorCode = HAL_RAMCFG_ERROR_BUSY;
    status = HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Enable the RAMCFG error interrupts.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @param  Notifications : Select the notification to be enabled.
  *                         This parameter can be any value of @ref
  *                         RAMCFG_Interrupt group.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_RAMCFG_EnableNotification(RAMCFG_HandleTypeDef *hramcfg, uint32_t Notifications)
{
  HAL_StatusTypeDef status = HAL_OK;
  /* Check the parameters */
  assert_param(IS_RAMCFG_ECC_INSTANCE(hramcfg->Instance));
  assert_param(IS_RAMCFG_INTERRUPT(Notifications));

  /* Check RAMCFG state */
  if (hramcfg->State == HAL_RAMCFG_STATE_READY)
  {
    /* Update RAMCFG peripheral state */
    hramcfg->State = HAL_RAMCFG_STATE_BUSY;

    /* Enable RAMCFG interrupts */
    __HAL_RAMCFG_ENABLE_IT(hramcfg, Notifications);

    /* Update the RAMCFG state */
    hramcfg->State = HAL_RAMCFG_STATE_READY;

  }
  else
  {
    /* Update the RAMCFG error code and return error */
    hramcfg->ErrorCode = HAL_RAMCFG_ERROR_BUSY;
    status = HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Disable the RAMCFG error interrupts.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @param  Notifications : Select the notification to be disabled.
  *                         This parameter can be :
  *                         RAMCFG_IT_SINGLEERR : Single Error Interrupt.
  *                         RAMCFG_IT_DOUBLEERR : Double Error Interrupt.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_RAMCFG_DisableNotification(RAMCFG_HandleTypeDef *hramcfg, uint32_t Notifications)
{
  HAL_StatusTypeDef status = HAL_OK;
  /* Check the parameters */
  assert_param(IS_RAMCFG_ECC_INSTANCE(hramcfg->Instance));
  assert_param(IS_RAMCFG_INTERRUPT(Notifications));

  /* Check RAMCFG state */
  if (hramcfg->State == HAL_RAMCFG_STATE_READY)
  {
    /* Update RAMCFG peripheral state */
    hramcfg->State = HAL_RAMCFG_STATE_BUSY;

    /* Disable RAMCFG interrupts */
    __HAL_RAMCFG_DISABLE_IT(hramcfg, Notifications);

    /* Update the RAMCFG state */
    hramcfg->State = HAL_RAMCFG_STATE_READY;
  }
  else
  {
    /* Update the RAMCFG error code and return error */
    hramcfg->ErrorCode = HAL_RAMCFG_ERROR_BUSY;
    status = HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Check if an ECC single error has occurred.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @retval State of bit (1 or 0).
  */
uint32_t HAL_RAMCFG_IsECCSingleErrorDetected(const RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the parameters */
  assert_param(IS_RAMCFG_ECC_INSTANCE(hramcfg->Instance));

  /* Return the state of SEDC flag */
  return ((READ_BIT(hramcfg->Instance->ISR, RAMCFG_FLAG_SINGLEERR) == (RAMCFG_FLAG_SINGLEERR)) ? 1UL : 0UL);
}

/**
  * @brief  Check if an ECC double error was occurred.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @retval State of bit (1 or 0).
  */
uint32_t HAL_RAMCFG_IsECCDoubleErrorDetected(const RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the parameters */
  assert_param(IS_RAMCFG_ECC_INSTANCE(hramcfg->Instance));

  /* Return the state of DED flag */
  return ((READ_BIT(hramcfg->Instance->ISR, RAMCFG_FLAG_DOUBLEERR) == (RAMCFG_FLAG_DOUBLEERR)) ? 1UL : 0UL);
}

/**
  * @brief  Get the RAMCFG single error address.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @retval Single error address offset.
  */
uint32_t HAL_RAMCFG_GetSingleErrorAddress(const RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the parameters */
  assert_param(IS_RAMCFG_ECC_INSTANCE(hramcfg->Instance));

  return hramcfg->Instance->ESEAR;
}

/**
  * @brief  Get the RAMCFG double error address.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @retval Double error address offset.
  */
uint32_t HAL_RAMCFG_GetDoubleErrorAddress(const RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the parameters */
  assert_param(IS_RAMCFG_ECC_INSTANCE(hramcfg->Instance));

  return hramcfg->Instance->EDEAR;
}

/**
  * @}
  */

/** @addtogroup RAMCFG_Exported_Functions_Group5
  *
@verbatim
 ===============================================================================
                      ##### Erase Operation Functions  #####
 ===============================================================================
    [..]
      This section provides functions allowing a hardware erase for the given SRAM.
    [..]
      The HAL_RAMCFG_Erase() function allows a hardware mass erase for the given
      SRAM. The erase value for all SRAMs is 0.

@endverbatim
  * @{
  */

/**
  * @brief  Launch a Mass Erase for the given SRAM.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.

  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_RAMCFG_Erase(RAMCFG_HandleTypeDef *hramcfg)
{
  uint32_t tickstart = HAL_GetTick();

  /* Check the parameters */
  assert_param(IS_RAMCFG_ALL_INSTANCE(hramcfg->Instance));

  /* Check RAMCFG state */
  if (hramcfg->State == HAL_RAMCFG_STATE_READY)
  {
    /* Update RAMCFG peripheral state */
    hramcfg->State = HAL_RAMCFG_STATE_BUSY;

    /* Unlock the RAMCFG erase bit */
    WRITE_REG(hramcfg->Instance->ERKEYR, RAMCFG_ERASE_KEY1);
    WRITE_REG(hramcfg->Instance->ERKEYR, RAMCFG_ERASE_KEY2);

    /* Start the SRAM erase operation */
    hramcfg->Instance->CR |= RAMCFG_CR_SRAMER;

    /*
       Wait for the SRAM hardware erase operation to complete by polling on
       SRAMBUSY flag to be reset.
    */
    while (__HAL_RAMCFG_GET_FLAG(hramcfg, RAMCFG_FLAG_SRAMBUSY) != 0U)
    {
      if ((HAL_GetTick() - tickstart) > RAMCFG_TIMEOUT_VALUE)
      {
        /* Update the RAMCFG error code */
        hramcfg->ErrorCode = HAL_RAMCFG_ERROR_TIMEOUT;

        /* Update the RAMCFG state and return error status */
        hramcfg->State = HAL_RAMCFG_STATE_ERROR;
        return HAL_ERROR;
      }
    }
  }
  else
  {
    /* Update the error code and return error status */
    hramcfg->ErrorCode = HAL_RAMCFG_ERROR_BUSY;
    return HAL_ERROR;
  }

  /* Update the RAMCFG state */
  hramcfg->State = HAL_RAMCFG_STATE_READY;

  return HAL_OK;
}

/**
  * @}
  */

/** @addtogroup RAMCFG_Exported_Functions_Group6
  *
@verbatim
 ===============================================================================
               ##### Handle Interrupt and Callbacks Functions  #####
 ===============================================================================
    [..]
      This section provides functions to handle RAMCFG interrupts and
      Register / UnRegister the different callbacks.
    [..]
      The HAL_RAMCFG_IRQHandler() function allows the user to handle the active RAMCFG
      interrupt request.
      The HAL_RAMCFG_RegisterCallback() function allows the user to register the selected
      RAMCFG callbacks.
      The HAL_RAMCFG_UnRegisterCallback() function allows the user to unregister the
      selected RAMCFG callbacks.
@endverbatim
  * @{
  */

/**
  * @brief  Handles RAMCFG interrupt request.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @retval None.
  */
void HAL_RAMCFG_IRQHandler(RAMCFG_HandleTypeDef *hramcfg)
{
  /* Single Error Interrupt Management ****************************************/
  if (__HAL_RAMCFG_GET_IT_SOURCE(hramcfg, RAMCFG_IT_SINGLEERR) != 0U)
  {
    if (__HAL_RAMCFG_GET_FLAG(hramcfg, RAMCFG_FLAG_SINGLEERR) != 0U)
    {
      /* Clear active flags */
      __HAL_RAMCFG_CLEAR_FLAG(hramcfg, RAMCFG_FLAG_SINGLEERR);

#if (USE_HAL_RAMCFG_REGISTER_CALLBACKS == 1)
      /* Check if a valid single error callback is registered */
      if (hramcfg->DetectSingleErrorCallback != NULL)
      {
        /* Single error detection callback */
        hramcfg->DetectSingleErrorCallback(hramcfg);
      }
#else
      HAL_RAMCFG_DetectSingleErrorCallback(hramcfg);
#endif /* USE_HAL_RAMCFG_REGISTER_CALLBACKS */
    }
  }

  /* Double Error Interrupt Management ****************************************/
  if (__HAL_RAMCFG_GET_IT_SOURCE(hramcfg, RAMCFG_IT_DOUBLEERR) != 0U)
  {
    if (__HAL_RAMCFG_GET_FLAG(hramcfg, RAMCFG_FLAG_DOUBLEERR) != 0U)
    {
      /* Clear active flags */
      __HAL_RAMCFG_CLEAR_FLAG(hramcfg, RAMCFG_FLAG_DOUBLEERR);

#if (USE_HAL_RAMCFG_REGISTER_CALLBACKS == 1)
      /* Check if a valid double error callback is registered */
      if (hramcfg->DetectDoubleErrorCallback != NULL)
      {
        /* Double error detection callback */
        hramcfg->DetectDoubleErrorCallback(hramcfg);
      }
#else
      HAL_RAMCFG_DetectDoubleErrorCallback(hramcfg);
#endif /* USE_HAL_RAMCFG_REGISTER_CALLBACKS */
    }
  }
}

/**
  * @brief  RAMCFG single error detection callback.
  * @param  hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that contains
  *                   the configuration information for the specified RAMCFG.
  * @retval None.
  */
__weak void HAL_RAMCFG_DetectSingleErrorCallback(RAMCFG_HandleTypeDef *hramcfg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hramcfg);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RAMCFG_DetectSingleErrorCallback can be implemented in
            the user file.                                                    */
}

/**
  * @brief  RAMCFG double error detection callback.
  * @param  hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that contains
  *                   the configuration information for the specified RAMCFG.
  * @retval None.
  */
__weak void HAL_RAMCFG_DetectDoubleErrorCallback(RAMCFG_HandleTypeDef *hramcfg)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hramcfg);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_RAMCFG_DetectDoubleErrorCallback can be implemented in
            the user file.                                                    */
}

#if (USE_HAL_RAMCFG_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register RAMCFG callbacks.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @param  CallbackID    : User Callback identifier a HAL_RAMCFG_CallbackIDTypeDef
  *                         ENUM as parameter.
  * @param  pCallback     : Pointer to private callback function.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_RAMCFG_RegisterCallback(RAMCFG_HandleTypeDef *hramcfg,
                                              HAL_RAMCFG_CallbackIDTypeDef CallbackID,
                                              void (* pCallback)(RAMCFG_HandleTypeDef *_hramcfg))
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_RAMCFG_ALL_INSTANCE(hramcfg->Instance));

  if (pCallback == NULL)
  {
    /* Update the error code and return error */
    hramcfg->ErrorCode |= HAL_RAMCFG_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  /* Check RAMCFG state */
  if (hramcfg->State == HAL_RAMCFG_STATE_READY)
  {
    switch (CallbackID)
    {
      case  HAL_RAMCFG_SE_DETECT_CB_ID:
        /* Register single error callback */
        hramcfg->DetectSingleErrorCallback = pCallback;
        break;

      case  HAL_RAMCFG_DE_DETECT_CB_ID:
        /* Register double error callback */
        hramcfg->DetectDoubleErrorCallback = pCallback;
        break;

      case HAL_RAMCFG_MSPINIT_CB_ID :
        /* Register msp init callback */
        hramcfg->MspInitCallback = pCallback;
        break;

      case HAL_RAMCFG_MSPDEINIT_CB_ID :
        /* Register msp de-init callback */
        hramcfg->MspDeInitCallback = pCallback;
        break;

      default:
        /* Update the error code and return error */
        hramcfg->ErrorCode |= HAL_RAMCFG_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
        break;
    }
  }
  else if (hramcfg->State == HAL_RAMCFG_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_RAMCFG_MSPINIT_CB_ID :
        /* Register msp init callback */
        hramcfg->MspInitCallback = pCallback;
        break;

      case HAL_RAMCFG_MSPDEINIT_CB_ID :
        /* Register msp de-init callback */
        hramcfg->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code and return error */
        hramcfg->ErrorCode |= HAL_RAMCFG_ERROR_INVALID_CALLBACK;
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code and return error  */
    hramcfg->ErrorCode = HAL_RAMCFG_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister RAMCFG callbacks.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @param  CallbackID    : User Callback identifier a HAL_RAMCFG_CallbackIDTypeDef
  *                         ENUM as parameter.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_RAMCFG_UnRegisterCallback(RAMCFG_HandleTypeDef *hramcfg, HAL_RAMCFG_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the parameters */
  assert_param(IS_RAMCFG_ALL_INSTANCE(hramcfg->Instance));

  /* Check RAMCFG state */
  if (hramcfg->State == HAL_RAMCFG_STATE_READY)
  {
    switch (CallbackID)
    {
      case  HAL_RAMCFG_SE_DETECT_CB_ID:
        /* UnRegister single error callback */
        hramcfg->DetectSingleErrorCallback = NULL;
        break;

      case  HAL_RAMCFG_DE_DETECT_CB_ID:
        /* UnRegister double error callback */
        hramcfg->DetectDoubleErrorCallback = NULL;
        break;

      case HAL_RAMCFG_MSPINIT_CB_ID :
        /* UnRegister msp init callback */
        hramcfg->MspInitCallback = NULL;
        break;

      case HAL_RAMCFG_MSPDEINIT_CB_ID :
        /* UnRegister msp de-init callback */
        hramcfg->MspDeInitCallback = NULL;
        break;

      case  HAL_RAMCFG_ALL_CB_ID:
        /* UnRegister all available callbacks */
        hramcfg->DetectSingleErrorCallback = NULL;
        hramcfg->DetectDoubleErrorCallback = NULL;
        hramcfg->MspDeInitCallback         = NULL;
        hramcfg->MspInitCallback           = NULL;
        break;

      default:
        /* Return error status */
        status = HAL_ERROR;
        break;
    }
  }
  else if (hramcfg->State == HAL_RAMCFG_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_RAMCFG_MSPINIT_CB_ID :
        /* UnRegister msp init callback */
        hramcfg->MspInitCallback = NULL;
        break;

      case HAL_RAMCFG_MSPDEINIT_CB_ID :
        /* UnRegister msp de-init callback */
        hramcfg->MspDeInitCallback = NULL;
        break;

      case  HAL_RAMCFG_ALL_CB_ID:
        /* UnRegister all available callbacks */
        hramcfg->MspDeInitCallback = NULL;
        hramcfg->MspInitCallback   = NULL;
        break;

      default :
        /* Update the error code */
        hramcfg->ErrorCode |= HAL_RAMCFG_ERROR_INVALID_CALLBACK;

        /* Update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code and return error */
    hramcfg->ErrorCode = HAL_RAMCFG_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }

  return status;
}

/**
  * @}
  */
#endif /* USE_HAL_RAMCFG_REGISTER_CALLBACKS */

/** @addtogroup RAMCFG_Exported_Functions_Group7
  *
@verbatim
 ===============================================================================
                    ##### State and Error Functions  #####
 ===============================================================================
    [..]
      This section provides functions to check and get the RAMCFG state
      and the error code.
    [..]
      The HAL_RAMCFG_GetState() function allows the user to get the RAMCFG peripheral
      state.
      The HAL_RAMCFG_GetError() function allows the user to get the RAMCFG peripheral error
      code.

@endverbatim
  * @{
  */

/**
  * @brief  Get the RAMCFG peripheral state.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @retval RAMCFG state.
  */
HAL_RAMCFG_StateTypeDef HAL_RAMCFG_GetState(const RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the parameters */
  assert_param(IS_RAMCFG_ALL_INSTANCE(hramcfg->Instance));

  /* Return the RAMCFG state */
  return hramcfg->State;
}

/**
  * @brief  Get the RAMCFG peripheral error code.
  * @param  hramcfg       : Pointer to a RAMCFG_HandleTypeDef structure that
  *                         contains the configuration information for the
  *                         specified RAMCFG instance.
  * @retval RAMCFG error code.
  */
uint32_t HAL_RAMCFG_GetError(const RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the parameters */
  assert_param(IS_RAMCFG_ALL_INSTANCE(hramcfg->Instance));

  /* Return the RAMCFG error code */
  return hramcfg->ErrorCode;
}

/**
  * @}
  */



/** @addtogroup RAMCFG_Exported_Functions_Group9
  *
@verbatim
 ===============================================================================
                      ##### AXISRAM Powerdown Functions  #####
 ===============================================================================
    [..]
      This section provides functions allowing to shutdown some AXISRAM memories
      reduce the consumption.
    [..]
      The HAL_RAMCFG_EnableAXISRAM() function allows to power on the selected AXISRAM.
    [..]
      The HAL_RAMCFG_DisableAXISRAM() function allows to power off the selected AXISRAM.
      AXISRAMi memory is in shutdown, and its content is not retained.
@endverbatim
  * @{
  */

/**
  * @brief  Enable AXISRAM power on.
  * @note   Only AXISRAM2 to AXISRAM6 are available for this feature.
  * @param  hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that
  *                   contains the configuration information for the
  *                   specified RAMCFG instance.
  * @retval None.
  */
void HAL_RAMCFG_EnableAXISRAM(const RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the parameters */
  assert_param(IS_RAMCFG_AXISRAM_POWERDOWN_INSTANCE(hramcfg->Instance));

  /* AXISRAMi power on */
  CLEAR_BIT(hramcfg->Instance->CR, RAMCFG_AXISRAM_POWERDOWN);
}

/**
  * @brief  Disable AXISRAM power on.
  * @note   Only AXISRAM2 to AXISRAM6 are available for this feature.
  * @note   AXISRAMi memory is in shutdown, and its content is not retained.
  * @note   This bit is used to reduce the consumption by powering off the AXISRAMi.
  * @param  hramcfg : Pointer to a RAMCFG_HandleTypeDef structure that
  *                   contains the configuration information for the
  *                   specified RAMCFG instance.
  * @retval None.
  */
void HAL_RAMCFG_DisableAXISRAM(const RAMCFG_HandleTypeDef *hramcfg)
{
  /* Check the parameters */
  assert_param(IS_RAMCFG_AXISRAM_POWERDOWN_INSTANCE(hramcfg->Instance));

  /* AXISRAMi powered off */
  SET_BIT(hramcfg->Instance->CR, RAMCFG_AXISRAM_POWERDOWN);
}
#endif /* HAL_RAMCFG_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


