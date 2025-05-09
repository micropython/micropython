/**
  ******************************************************************************
  * @file    stm32n6xx_hal_mdios.c
  * @author  MCD Application Team
  * @brief   MDIOS HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the MDIOS Peripheral.
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
  *
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
    [..]
    The MDIOS HAL driver can be used as follow:

    (#) Declare a MDIOS_HandleTypeDef handle structure.

    (#) Initialize the MDIOS low level resources by implementing the HAL_MDIOS_MspInit() API:
        (##) Enable the MDIOS interface clock.
        (##) MDIOS pins configuration:
            (+++) Enable clocks for the MDIOS GPIOs.
            (+++) Configure the MDIOS pins as alternate function.
        (##) NVIC configuration if you need to use interrupt process:
            (+++) Configure the MDIOS interrupt priority.
            (+++) Enable the NVIC MDIOS IRQ handle.

    (#) Program the Port Address and the Preamble Check in the Init structure.

    (#) Initialize the MDIOS registers by calling the HAL_MDIOS_Init() API.

    (#) Perform direct slave read/write operations using the following APIs:
        (##) Read the value of a DINn register: HAL_MDIOS_ReadReg()
        (##) Write a value to a DOUTn register: HAL_MDIOS_WriteReg()

    (#) Get the Master read/write operations flags using the following APIs:
        (##) Bit map of DOUTn registers read by Master: HAL_MDIOS_GetReadRegAddress()
        (##) Bit map of DINn registers written by Master : HAL_MDIOS_GetWrittenRegAddress()

    (#) Clear the read/write flags using the following APIs:
        (##) Clear read flags of a set of registers: HAL_MDIOS_ClearReadRegAddress()
        (##) Clear write flags of a set of registers: HAL_MDIOS_ClearWriteRegAddress()

    (#) Enable interrupts on events using HAL_MDIOS_EnableEvents(), when called
        the MDIOS will generate an interrupt in the following cases:
        (##) a DINn register written by the Master
        (##) a DOUTn register read by the Master
        (##) an error occur

        (@) A callback is executed for each generated interrupt, so the driver provide the following
            HAL_MDIOS_WriteCpltCallback(), HAL_MDIOS_ReadCpltCallback() and HAL_MDIOS_ErrorCallback()
        (@) HAL_MDIOS_IRQHandler() must be called from the MDIOS IRQ Handler, to handle the interrupt
            and execute the previous callbacks

    (#) Reset the MDIOS peripheral and all related resources by calling the HAL_MDIOS_DeInit() API.
        (##) HAL_MDIOS_MspDeInit() must be implemented to reset low level resources
            (GPIO, Clocks, NVIC configuration ...)

  *** Callback registration ***
  =============================================

  The compilation define  USE_HAL_MDIOS_REGISTER_CALLBACKS when set to 1
  allows the user to configure dynamically the driver callbacks.
  Use Function @ref HAL_MDIOS_RegisterCallback() to register an interrupt callback.

  Function @ref HAL_MDIOS_RegisterCallback() allows to register following callbacks:
    (+) WriteCpltCallback  : Write Complete Callback.
    (+) ReadCpltCallback   : Read Complete Callback.
    (+) ErrorCallback      : Error Callback.
    (+) WakeUpCallback     : Wake UP Callback
    (+) MspInitCallback    : MspInit Callback.
    (+) MspDeInitCallback  : MspDeInit Callback.

  This function takes as parameters the HAL peripheral handle, the Callback ID
  and a pointer to the user callback function.

  Use function @ref HAL_MDIOS_UnRegisterCallback() to reset a callback to the default
  weak function.
  @ref HAL_MDIOS_UnRegisterCallback takes as parameters the HAL peripheral handle,
  and the Callback ID.
  This function allows to reset following callbacks:
    (+) WriteCpltCallback  : Write Complete Callback.
    (+) ReadCpltCallback   : Read Complete Callback.
    (+) ErrorCallback      : Error Callback.
    (+) WakeUpCallback     : Wake UP Callback
    (+) MspInitCallback    : MspInit Callback.
    (+) MspDeInitCallback  : MspDeInit Callback.

  By default, after the HAL_MDIOS_Init and when the state is HAL_MDIOS_STATE_RESET
  all callbacks are set to the corresponding weak functions:
  examples @ref HAL_MDIOS_WriteCpltCallback(), @ref HAL_MDIOS_ReadCpltCallback().
  Exception done for MspInit and MspDeInit functions that are
  reset to the legacy weak function in the HAL_MDIOS_Init/ @ref HAL_MDIOS_DeInit only when
  these callbacks are null (not registered beforehand).
  if not, MspInit or MspDeInit are not null, the HAL_MDIOS_Init/ @ref HAL_MDIOS_DeInit
  keep and use the user MspInit/MspDeInit callbacks (registered beforehand)

  Callbacks can be registered/unregistered in HAL_MDIOS_STATE_READY state only.
  Exception done MspInit/MspDeInit that can be registered/unregistered
  in HAL_MDIOS_STATE_READY or HAL_MDIOS_STATE_RESET state,
  thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
  In that case first register the MspInit/MspDeInit user callbacks
  using @ref HAL_MDIOS_RegisterCallback() before calling @ref HAL_MDIOS_DeInit
  or HAL_MDIOS_Init function.

  When The compilation define USE_HAL_MDIOS_REGISTER_CALLBACKS is set to 0 or
  not defined, the callback registration feature is not available and all callbacks
  are set to the corresponding weak functions.


  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined (MDIOS)
/** @defgroup MDIOS  MDIOS
  * @brief HAL MDIOS module driver
  * @{
  */
#ifdef HAL_MDIOS_MODULE_ENABLED



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MDIOS_PORT_ADDRESS_SHIFT        ((uint32_t)8)
#define  MDIOS_ALL_REG_FLAG             ((uint32_t)0xFFFFFFFFU)
#define  MDIOS_ALL_ERRORS_FLAG          ((uint32_t)(MDIOS_SR_PERF | MDIOS_SR_SERF | MDIOS_SR_TERF))

#define MDIOS_DIN_BASE_ADDR             (MDIOS_BASE + 0x100U)
#define MDIOS_DOUT_BASE_ADDR            (MDIOS_BASE + 0x180U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if (USE_HAL_MDIOS_REGISTER_CALLBACKS == 1)
static void MDIOS_InitCallbacksToDefault(MDIOS_HandleTypeDef *hmdios);
#endif /* USE_HAL_MDIOS_REGISTER_CALLBACKS */
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup MDIOS_Exported_Functions MDIOS Exported Functions
  * @{
  */

/** @defgroup MDIOS_Exported_Functions_Group1 Initialization/de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to initialize the MDIOS
      (+) The following parameters can be configured:
        (++) Port Address
        (++) Preamble Check

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the MDIOS according to the specified parameters in
  *         the MDIOS_InitTypeDef and creates the associated handle .
  * @param  hmdios: pointer to a MDIOS_HandleTypeDef structure that contains
  *         the configuration information for MDIOS module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MDIOS_Init(MDIOS_HandleTypeDef *hmdios)
{
  uint32_t tmpcr;

  /* Check the MDIOS handle allocation */
  if (hmdios == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_MDIOS_ALL_INSTANCE(hmdios->Instance));
  assert_param(IS_MDIOS_PORTADDRESS(hmdios->Init.PortAddress));
  assert_param(IS_MDIOS_PREAMBLECHECK(hmdios->Init.PreambleCheck));

#if (USE_HAL_MDIOS_REGISTER_CALLBACKS == 1)

  if (hmdios->State == HAL_MDIOS_STATE_RESET)
  {
    MDIOS_InitCallbacksToDefault(hmdios);

    if (hmdios->MspInitCallback == NULL)
    {
      hmdios->MspInitCallback = HAL_MDIOS_MspInit;
    }

    /* Init the low level hardware */
    hmdios->MspInitCallback(hmdios);
  }

#else

  if (hmdios->State == HAL_MDIOS_STATE_RESET)
  {
    /* Init the low level hardware */
    HAL_MDIOS_MspInit(hmdios);
  }

#endif /* (USE_HAL_MDIOS_REGISTER_CALLBACKS) */

  /* Change the MDIOS state */
  hmdios->State = HAL_MDIOS_STATE_BUSY;

  /* Get the MDIOS CR value */
  tmpcr = hmdios->Instance->CR;

  /* Clear PORT_ADDRESS, DPC and EN bits */
  tmpcr &= ((uint32_t)~(MDIOS_CR_EN | MDIOS_CR_DPC | MDIOS_CR_PORT_ADDRESS));

  /* Set MDIOS control parameters and enable the peripheral */
  tmpcr |= (uint32_t)(((hmdios->Init.PortAddress) << MDIOS_PORT_ADDRESS_SHIFT)    | \
                      (hmdios->Init.PreambleCheck) | \
                      (MDIOS_CR_EN));

  /* Write the MDIOS CR */
  hmdios->Instance->CR = tmpcr;

  hmdios->ErrorCode = HAL_MDIOS_ERROR_NONE;

  /* Change the MDIOS state */
  hmdios->State = HAL_MDIOS_STATE_READY;

  /* Release Lock */
  __HAL_UNLOCK(hmdios);

  /* Return function status */
  return HAL_OK;

}

/**
  * @brief  DeInitializes the MDIOS peripheral.
  * @param  hmdios: MDIOS handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MDIOS_DeInit(MDIOS_HandleTypeDef *hmdios)
{
  /* Check the MDIOS handle allocation */
  if (hmdios == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_MDIOS_ALL_INSTANCE(hmdios->Instance));

  /* Change the MDIOS state */
  hmdios->State = HAL_MDIOS_STATE_BUSY;

  /* Disable the Peripheral */
  __HAL_MDIOS_DISABLE(hmdios);

#if (USE_HAL_MDIOS_REGISTER_CALLBACKS == 1)

  if (hmdios->MspDeInitCallback == NULL)
  {
    hmdios->MspDeInitCallback = HAL_MDIOS_MspDeInit;
  }
  /* DeInit the low level hardware */
  hmdios->MspDeInitCallback(hmdios);
#else

  /* DeInit the low level hardware */
  HAL_MDIOS_MspDeInit(hmdios);

#endif /* (USE_HAL_MDIOS_REGISTER_CALLBACKS) */

  /* Change the MDIOS state */
  hmdios->State = HAL_MDIOS_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hmdios);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  MDIOS MSP Init
  * @param  hmdios: mdios handle
  * @retval None
  */
__weak void HAL_MDIOS_MspInit(MDIOS_HandleTypeDef *hmdios)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdios);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MDIOS_MspInit can be implemented in the user file
   */
}

/**
  * @brief  MDIOS MSP DeInit
  * @param  hmdios: mdios handle
  * @retval None
  */
__weak void HAL_MDIOS_MspDeInit(MDIOS_HandleTypeDef *hmdios)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdios);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MDIOS_MspDeInit can be implemented in the user file
   */
}

#if (USE_HAL_MDIOS_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User MDIOS Callback
  *         To be used instead of the weak predefined callback
  * @param hmdios mdios handle
  * @param CallbackID ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_MDIOS_WRITE_COMPLETE_CB_ID Write Complete Callback ID
  *          @arg @ref HAL_MDIOS_READ_COMPLETE_CB_ID  Read Complete Callback ID
  *          @arg @ref HAL_MDIOS_ERROR_CB_ID          Error Callback ID
  *          @arg @ref HAL_MDIOS_WAKEUP_CB_ID         Wake Up Callback ID
  *          @arg @ref HAL_MDIOS_MSPINIT_CB_ID        MspInit callback ID
  *          @arg @ref HAL_MDIOS_MSPDEINIT_CB_ID      MspDeInit callback ID
  * @param pCallback pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_MDIOS_RegisterCallback(MDIOS_HandleTypeDef *hmdios, HAL_MDIOS_CallbackIDTypeDef CallbackID,
                                             pMDIOS_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hmdios->ErrorCode |= HAL_MDIOS_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hmdios);

  if (hmdios->State == HAL_MDIOS_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_MDIOS_WRITE_COMPLETE_CB_ID :
        hmdios->WriteCpltCallback = pCallback;
        break;

      case HAL_MDIOS_READ_COMPLETE_CB_ID :
        hmdios->ReadCpltCallback = pCallback;
        break;

      case HAL_MDIOS_ERROR_CB_ID :
        hmdios->ErrorCallback = pCallback;
        break;

      case HAL_MDIOS_WAKEUP_CB_ID :
        hmdios->WakeUpCallback = pCallback;
        break;

      case HAL_MDIOS_MSPINIT_CB_ID :
        hmdios->MspInitCallback = pCallback;
        break;

      case HAL_MDIOS_MSPDEINIT_CB_ID :
        hmdios->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hmdios->ErrorCode |= HAL_MDIOS_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hmdios->State == HAL_MDIOS_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_MDIOS_MSPINIT_CB_ID :
        hmdios->MspInitCallback = pCallback;
        break;

      case HAL_MDIOS_MSPDEINIT_CB_ID :
        hmdios->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hmdios->ErrorCode |= HAL_MDIOS_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hmdios->ErrorCode |= HAL_MDIOS_ERROR_INVALID_CALLBACK;
    /* Return error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hmdios);

  return status;
}

/**
  * @brief  Unregister an MDIOS Callback
  *         MDIOS callback is redirected to the weak predefined callback
  * @param hmdios mdios handle
  * @param CallbackID ID of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_MDIOS_WRITE_COMPLETE_CB_ID Write Complete Callback ID
  *          @arg @ref HAL_MDIOS_READ_COMPLETE_CB_ID  Read Complete Callback ID
  *          @arg @ref HAL_MDIOS_ERROR_CB_ID          Error Callback ID
  *          @arg @ref HAL_MDIOS_WAKEUP_CB_ID         Wake Up Callback ID
  *          @arg @ref HAL_MDIOS_MSPINIT_CB_ID        MspInit callback ID
  *          @arg @ref HAL_MDIOS_MSPDEINIT_CB_ID      MspDeInit callback ID
  * @retval status
  */
HAL_StatusTypeDef HAL_MDIOS_UnRegisterCallback(MDIOS_HandleTypeDef *hmdios, HAL_MDIOS_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hmdios);

  if (hmdios->State == HAL_MDIOS_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_MDIOS_WRITE_COMPLETE_CB_ID :
        hmdios->WriteCpltCallback = HAL_MDIOS_WriteCpltCallback;
        break;

      case HAL_MDIOS_READ_COMPLETE_CB_ID :
        hmdios->ReadCpltCallback = HAL_MDIOS_ReadCpltCallback;
        break;

      case HAL_MDIOS_ERROR_CB_ID :
        hmdios->ErrorCallback = HAL_MDIOS_ErrorCallback;
        break;

      case HAL_MDIOS_WAKEUP_CB_ID :
        hmdios->WakeUpCallback = HAL_MDIOS_WakeUpCallback;
        break;

      case HAL_MDIOS_MSPINIT_CB_ID :
        hmdios->MspInitCallback = HAL_MDIOS_MspInit;
        break;

      case HAL_MDIOS_MSPDEINIT_CB_ID :
        hmdios->MspDeInitCallback = HAL_MDIOS_MspDeInit;
        break;

      default :
        /* Update the error code */
        hmdios->ErrorCode |= HAL_MDIOS_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hmdios->State == HAL_MDIOS_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_MDIOS_MSPINIT_CB_ID :
        hmdios->MspInitCallback = HAL_MDIOS_MspInit;
        break;

      case HAL_MDIOS_MSPDEINIT_CB_ID :
        hmdios->MspDeInitCallback = HAL_MDIOS_MspDeInit;
        break;

      default :
        /* Update the error code */
        hmdios->ErrorCode |= HAL_MDIOS_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hmdios->ErrorCode |= HAL_MDIOS_ERROR_INVALID_CALLBACK;
    /* Return error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hmdios);

  return status;
}
#endif /* USE_HAL_MDIOS_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup MDIOS_Exported_Functions_Group2 IO operation functions
  *  @brief MDIOS Read/Write functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    This subsection provides a set of functions allowing to manage the MDIOS
    read and write operations.

    (#) APIs that allow to the MDIOS to read/write from/to the
        values of one of the DINn/DOUTn registers:
        (+) Read the value of a DINn register: HAL_MDIOS_ReadReg()
        (+) Write a value to a DOUTn register: HAL_MDIOS_WriteReg()

    (#) APIs that provide if there are some Slave registres have been
        read or written by the Master:
        (+) DOUTn registers read by Master: HAL_MDIOS_GetReadRegAddress()
        (+) DINn registers written by Master : HAL_MDIOS_GetWrittenRegAddress()

    (#) APIs that Clear the read/write flags:
        (+) Clear read registers flags: HAL_MDIOS_ClearReadRegAddress()
        (+) Clear write registers flags: HAL_MDIOS_ClearWriteRegAddress()

    (#) A set of Callbacks are provided:
        (+) HAL_MDIOS_WriteCpltCallback()
        (+) HAL_MDIOS_ReadCpltCallback()
        (+) HAL_MDIOS_ErrorCallback()

@endverbatim
  * @{
  */

/**
  * @brief  Writes to an MDIOS output register
  * @param  hmdios: mdios handle
  * @param  RegNum: MDIOS output register address
  * @param  Data: Data to write
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MDIOS_WriteReg(MDIOS_HandleTypeDef *hmdios, uint32_t RegNum, uint16_t Data)
{
  uint32_t tmpreg;

  /* Check the parameters */
  assert_param(IS_MDIOS_REGISTER(RegNum));

  /* Process Locked */
  __HAL_LOCK(hmdios);

  /* Get the addr of output register to be written by the MDIOS */
  tmpreg = MDIOS_DOUT_BASE_ADDR + (4U * RegNum);

  /* Write to DOUTn register */
  *((uint32_t *)tmpreg) = Data;

  /* Process Unlocked */
  __HAL_UNLOCK(hmdios);

  return HAL_OK;
}

/**
  * @brief  Reads an MDIOS input register
  * @param  hmdios: mdios handle
  * @param  RegNum: MDIOS input register address
  * @param  pData: pointer to Data
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MDIOS_ReadReg(MDIOS_HandleTypeDef *hmdios, uint32_t RegNum, uint16_t *pData)
{
  uint32_t tmpreg;

  /* Check the parameters */
  assert_param(IS_MDIOS_REGISTER(RegNum));

  /* Process Locked */
  __HAL_LOCK(hmdios);

  /* Get the addr of input register to be read by the MDIOS */
  tmpreg = MDIOS_DIN_BASE_ADDR + (4U * RegNum);

  /* Read DINn register */
  *pData = (uint16_t)(*((uint32_t *)tmpreg));

  /* Process Unlocked */
  __HAL_UNLOCK(hmdios);

  return HAL_OK;
}

/**
  * @brief  Gets Written registers by MDIO master
  * @param  hmdios: mdios handle
  * @retval bit map of written registers addresses
  */
uint32_t HAL_MDIOS_GetWrittenRegAddress(const MDIOS_HandleTypeDef *hmdios)
{
  return hmdios->Instance->WRFR;
}

/**
  * @brief  Gets Read registers by MDIO master
  * @param  hmdios: mdios handle
  * @retval bit map of read registers addresses
  */
uint32_t HAL_MDIOS_GetReadRegAddress(const MDIOS_HandleTypeDef *hmdios)
{
  return hmdios->Instance->RDFR;
}

/**
  * @brief  Clears Write registers flag
  * @param  hmdios: mdios handle
  * @param  RegNum: registers addresses to be cleared
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MDIOS_ClearWriteRegAddress(MDIOS_HandleTypeDef *hmdios, uint32_t RegNum)
{
  /* Check the parameters */
  assert_param(IS_MDIOS_REGISTER(RegNum));

  /* Process Locked */
  __HAL_LOCK(hmdios);

  /* Clear write registers flags */
  hmdios->Instance->CWRFR |= (RegNum);

  /* Release Lock */
  __HAL_UNLOCK(hmdios);

  return HAL_OK;
}

/**
  * @brief  Clears Read register flag
  * @param  hmdios: mdios handle
  * @param  RegNum: registers addresses to be cleared
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MDIOS_ClearReadRegAddress(MDIOS_HandleTypeDef *hmdios, uint32_t RegNum)
{
  /* Check the parameters */
  assert_param(IS_MDIOS_REGISTER(RegNum));

  /* Process Locked */
  __HAL_LOCK(hmdios);

  /* Clear read registers flags */
  hmdios->Instance->CRDFR |= (RegNum);

  /* Release Lock */
  __HAL_UNLOCK(hmdios);

  return HAL_OK;
}

/**
  * @brief  Enables Events for MDIOS peripheral
  * @param  hmdios: mdios handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MDIOS_EnableEvents(MDIOS_HandleTypeDef *hmdios)
{
  /* Process Locked */
  __HAL_LOCK(hmdios);

  /* Enable MDIOS interrupts: Register Write, Register Read and Error ITs */
  __HAL_MDIOS_ENABLE_IT(hmdios, (MDIOS_IT_WRITE | MDIOS_IT_READ | MDIOS_IT_ERROR));

  /* Process Unlocked */
  __HAL_UNLOCK(hmdios);

  return HAL_OK;
}

/**
  * @brief This function handles MDIOS interrupt request.
  * @param hmdios: MDIOS handle
  * @retval None
  */
void HAL_MDIOS_IRQHandler(MDIOS_HandleTypeDef *hmdios)
{
  uint32_t itsource = READ_REG(hmdios->Instance->CR);
  uint32_t itflag = READ_REG(hmdios->Instance->SR);
  uint32_t exti_flag = READ_REG(EXTI->IMR2);

  /* Write Register Interrupt enabled ? */
  if ((itsource & MDIOS_IT_WRITE) != 0U)
  {
    /* Write register flag */
    if (HAL_MDIOS_GetWrittenRegAddress(hmdios) != (uint32_t)RESET)
    {
#if (USE_HAL_MDIOS_REGISTER_CALLBACKS == 1)
      /*Call registered Write complete callback*/
      hmdios->WriteCpltCallback(hmdios);
#else
      /* Write callback function */
      HAL_MDIOS_WriteCpltCallback(hmdios);
#endif  /* USE_HAL_MDIOS_REGISTER_CALLBACKS */

      /* Clear write register flag */
      hmdios->Instance->CWRFR = MDIOS_ALL_REG_FLAG;
    }
  }

  /* Read Register Interrupt enabled ? */
  if ((itsource & MDIOS_IT_READ) != 0U)
  {
    /* Read register flag */
    if (HAL_MDIOS_GetReadRegAddress(hmdios) != (uint32_t)RESET)
    {
#if (USE_HAL_MDIOS_REGISTER_CALLBACKS == 1)
      /*Call registered Read complete callback*/
      hmdios->ReadCpltCallback(hmdios);
#else
      /* Read callback function  */
      HAL_MDIOS_ReadCpltCallback(hmdios);
#endif  /* USE_HAL_MDIOS_REGISTER_CALLBACKS */

      /* Clear read register flag */
      hmdios->Instance->CRDFR = MDIOS_ALL_REG_FLAG;
    }
  }

  /* Error Interrupt enabled ? */
  if ((itsource & MDIOS_IT_ERROR) != 0U)
  {
    /* All Errors Flag */
    if ((itflag & MDIOS_ALL_ERRORS_FLAG) != 0U)
    {
      hmdios->ErrorCode |= HAL_MDIOS_ERROR_DATA;

#if (USE_HAL_MDIOS_REGISTER_CALLBACKS == 1)
      /*Call registered Error callback*/
      hmdios->ErrorCallback(hmdios);
#else
      /* Error Callback */
      HAL_MDIOS_ErrorCallback(hmdios);
#endif  /* USE_HAL_MDIOS_REGISTER_CALLBACKS */

      /* Clear errors flag */
      __HAL_MDIOS_CLEAR_ERROR_FLAG(hmdios, MDIOS_ALL_ERRORS_FLAG);
    }
    hmdios->ErrorCode = HAL_MDIOS_ERROR_NONE;
  }
  /* check MDIOS WAKEUP exti flag */
  if ((exti_flag & MDIOS_WAKEUP_EXTI_LINE) != 0U)
  {
    /* Clear MDIOS WAKEUP Exti pending bit */
    __HAL_MDIOS_WAKEUP_EXTI_CLEAR_FLAG(MDIOS_WAKEUP_EXTI_LINE);
#if (USE_HAL_MDIOS_REGISTER_CALLBACKS == 1)
    /*Call registered WakeUp callback*/
    hmdios->WakeUpCallback(hmdios);
#else
    /* MDIOS WAKEUP callback */
    HAL_MDIOS_WakeUpCallback(hmdios);
#endif  /* USE_HAL_MDIOS_REGISTER_CALLBACKS */
  }

}

/**
  * @brief  Write Complete Callback
  * @param  hmdios: mdios handle
  * @retval None
  */
__weak void HAL_MDIOS_WriteCpltCallback(MDIOS_HandleTypeDef *hmdios)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdios);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MDIOS_WriteCpltCallback can be implemented in the user file
   */
}

/**
  * @brief  Read Complete Callback
  * @param  hmdios: mdios handle
  * @retval None
  */
__weak void HAL_MDIOS_ReadCpltCallback(MDIOS_HandleTypeDef *hmdios)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdios);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MDIOS_ReadCpltCallback can be implemented in the user file
   */
}

/**
  * @brief Error Callback
  * @param hmdios: mdios handle
  * @retval None
  */
__weak void HAL_MDIOS_ErrorCallback(MDIOS_HandleTypeDef *hmdios)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdios);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_MDIOS_ErrorCallback can be implemented in the user file
   */
}

/**
  * @brief  MDIOS WAKEUP interrupt callback
  * @param hmdios: mdios handle
  * @retval None
  */
__weak void HAL_MDIOS_WakeUpCallback(MDIOS_HandleTypeDef *hmdios)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hmdios);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MDIOS_WakeUpCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup MDIOS_Exported_Functions_Group3 Peripheral Control functions
  *  @brief   MDIOS control functions
  *
@verbatim
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to control the MDIOS.
     (+) HAL_MDIOS_GetState() API, helpful to check in run-time the state.
     (+) HAL_MDIOS_GetError() API, returns the errors code of the HAL state machine.

@endverbatim
  * @{
  */

/**
  * @brief  Gets MDIOS error code
  * @param  hmdios: mdios handle
  * @retval mdios error code
  */
uint32_t HAL_MDIOS_GetError(const MDIOS_HandleTypeDef *hmdios)
{
  /* return the error code */
  return hmdios->ErrorCode;
}

/**
  * @brief  Return the MDIOS HAL state
  * @param  hmdios: mdios handle
  * @retval HAL state
  */
HAL_MDIOS_StateTypeDef HAL_MDIOS_GetState(const MDIOS_HandleTypeDef *hmdios)
{
  /* Return MDIOS state */
  return hmdios->State;
}

/**
  * @}
  */

/**
  * @}
  */

#if (USE_HAL_MDIOS_REGISTER_CALLBACKS == 1)
static void MDIOS_InitCallbacksToDefault(MDIOS_HandleTypeDef *hmdios)
{
  /* Init the MDIOS Callback settings */
  hmdios->WriteCpltCallback  = HAL_MDIOS_WriteCpltCallback;   /* Legacy weak WriteCpltCallback   */
  hmdios->ReadCpltCallback   = HAL_MDIOS_ReadCpltCallback;    /* Legacy weak ReadCpltCallback   */
  hmdios->ErrorCallback      = HAL_MDIOS_ErrorCallback;       /* Legacy weak ErrorCallback */
  hmdios->WakeUpCallback     = HAL_MDIOS_WakeUpCallback;      /* Legacy weak WakeUpCallback   */
}
#endif /* USE_HAL_MDIOS_REGISTER_CALLBACKS */

/**
  * @}
  */
#endif /* HAL_MDIOS_MODULE_ENABLED */
/**
  * @}
  */
#endif /* MDIOS */
/**
  * @}
  */
