/**
  ******************************************************************************
  * @file    stm32n6xx_hal_gpu2d.c
  * @author  MCD Application Team
  * @brief   GPU2D HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the GPU2D peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral State and Errors functions
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
      (#) Peripheral control is exclusively done by the accompanying middleware library.

     *** Interrupt mode IO operation ***
     ===================================
     [..]
       (#) Configure the GPU2D hardware to perform graphics operation using the Third Party MW
           Library APIs.
       (#) Submit command List to the hardware.
       (#) Wait indefinitely for the completion of submitted Command List by GPU2D hardware.
       (#) Use HAL_GPU2D_IRQHandler() called under GPU2D_IRQHandler() interrupt subroutine.
       (#) At the end of Command List execution HAL_GPU2D_IRQHandler() function is executed
           and user can add his own function by customization of function pointer
       (#) CommandListCpltCallback (member of GPU2D handle structure) to notify the upper level
           about the completion of Command List execution.

     (#) Callback HAL_GPU2D_CommandListCpltCallback is invoked when the GPU2D hardware executes
         the programmed command list (Command List execution completion).

    (++) This callback is called when the compilation defines USE_HAL_GPU2D_REGISTER_CALLBACKS
         is set to 0 or not defined.

        (++) This callback should be implemented in the application side. It should notify
             the upper level that the programmed command list is completed.

      (#) To control the GPU2D state, use the following function: HAL_GPU2D_GetState().

      (#) To read the GPU2D error code, use the following function: HAL_GPU2D_GetError().

     *** GPU2D HAL driver macros list ***
     =============================================
     [..]
       Below the list of most used macros in GPU2D HAL driver :

      (+) __HAL_GPU2D_RESET_HANDLE_STATE: Reset GPU2D handle state.
      (+) __HAL_GPU2D_GET_FLAG: Get the GPU2D pending flags.
      (+) __HAL_GPU2D_CLEAR_FLAG: Clear the GPU2D pending flags.
      (+) __HAL_GPU2D_GET_IT_SOURCE: Check whether the specified GPU2D interrupt is enabled or not.

     *** Callback registration ***
     ===================================
     [..]
      (#) The compilation define  USE_HAL_GPU2D_REGISTER_CALLBACKS when set to 1
          allows the user to configure dynamically the driver callbacks.
          Use function @ref HAL_GPU2D_RegisterCallback() to register a user callback.

      (#) Function @ref HAL_GPU2D_RegisterCallback() allows to register following callbacks:
            (+) CommandListCpltCallback : callback for Command List completion.
            (+) MspInitCallback    : GPU2D MspInit.
            (+) MspDeInitCallback  : GPU2D MspDeInit.
          This function takes as parameters the HAL peripheral handle, the Callback ID
          and a pointer to the user callback function.

      (#) Use function @ref HAL_GPU2D_UnRegisterCallback() to reset a callback to the default
          weak (surcharged) function.
          @ref HAL_GPU2D_UnRegisterCallback() takes as parameters the HAL peripheral handle,
          and the Callback ID.
          This function allows to reset following callbacks:
            (+) CommandListCpltCallback : callback for Command List completion.
            (+) MspInitCallback    : GPU2D MspInit.
            (+) MspDeInitCallback  : GPU2D MspDeInit.

          Callbacks can be registered/unregistered in READY state only.
          Exception done for MspInit/MspDeInit callbacks that can be registered/unregistered
          in READY or RESET state, thus registered (user) MspInit/DeInit callbacks can be used
          during the Init/DeInit.
          In that case first register the MspInit/MspDeInit user callbacks
          using @ref HAL_GPU2D_RegisterCallback before calling @ref HAL_GPU2D_DeInit
          or @ref HAL_GPU2D_Init function.

          When The compilation define USE_HAL_GPU2D_REGISTER_CALLBACKS is set to 0 or
          not defined, the callback registering feature is not available
          and weak (surcharged) callbacks are used.

     [..]
      (@) You can refer to the GPU2D HAL driver header file for more useful macros

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

#ifdef HAL_GPU2D_MODULE_ENABLED
#if defined (GPU2D)

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup GPU2D  GPU2D
  * @brief GPU2D HAL module driver
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/** @defgroup GPU2D_Private_Macros GPU2D Private Macros
  * @{
  */

/** @defgroup GPU2D_Write_Read Common write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in GPU2D register
  * @param  __INSTANCE__ GPU2D Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define GPU2D_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(*(__IO uint32_t *)(__INSTANCE__\
                                                                   + __REG__), __VALUE__)

/**
  * @brief  Read a value in GPU2D register
  * @param  __INSTANCE__ GPU2D Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define GPU2D_ReadReg(__INSTANCE__, __REG__) READ_REG(*(__IO uint32_t *)(__INSTANCE__ + __REG__))
/**
  * @}
  */

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @defgroup GPU2D_Exported_Functions GPU2D Exported Functions
  * @{
  */

/** @defgroup GPU2D_Exported_Functions_Group1 Initialization and de-initialization functions
  * @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
                ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the GPU2D
      (+) De-initialize the GPU2D

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the GPU2D according to the specified
  *         parameters in the GPU2D_InitTypeDef and create the associated handle.
  * @param  hgpu2d pointer to a GPU2D_HandleTypeDef structure that contains
  *                the configuration information for the GPU2D.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_GPU2D_Init(GPU2D_HandleTypeDef *hgpu2d)
{
  /* Check the GPU2D handle validity */
  if (hgpu2d == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_GPU2D_ALL_INSTANCE(hgpu2d->Instance));

  if (hgpu2d->State == HAL_GPU2D_STATE_RESET)
  {
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    /* Reset Callback pointers in HAL_GPU2D_STATE_RESET only */
    hgpu2d->CommandListCpltCallback = HAL_GPU2D_CommandListCpltCallback;
    if (hgpu2d->MspInitCallback == NULL)
    {
      hgpu2d->MspInitCallback = HAL_GPU2D_MspInit;
    }

    /* Init the low level hardware */
    hgpu2d->MspInitCallback(hgpu2d);
#else /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 0 */
    /* Init the low level hardware */
    HAL_GPU2D_MspInit(hgpu2d);
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */

    /* Allocate lock resource and initialize it */
    hgpu2d->Lock = HAL_UNLOCKED;
  }

  /* Process locked */
  __HAL_LOCK(hgpu2d);

#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
  /* Reset the CommandListCpltCallback handler */
  hgpu2d->CommandListCpltCallback = NULL;
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */

  /* Update error code */
  hgpu2d->ErrorCode = HAL_GPU2D_ERROR_NONE;

  /* Initialize the GPU2D state*/
  hgpu2d->State = HAL_GPU2D_STATE_READY;

  /* Release Lock */
  __HAL_UNLOCK(hgpu2d);

  return HAL_OK;
}

/**
  * @brief  Deinitializes the GPU2D peripheral registers to their default reset
  *         values.
  * @param  hgpu2d pointer to a GPU2D_HandleTypeDef structure that contains
  *                the configuration information for the GPU2D.
  * @retval None
  */
HAL_StatusTypeDef HAL_GPU2D_DeInit(GPU2D_HandleTypeDef *hgpu2d)
{
  /* Check the GPU2D handle validity */
  if (hgpu2d == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_GPU2D_ALL_INSTANCE(hgpu2d->Instance));

  /* Process locked */
  __HAL_LOCK(hgpu2d);

  if (hgpu2d->State == HAL_GPU2D_STATE_READY)
  {
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    if (hgpu2d->MspDeInitCallback == NULL)
    {
      hgpu2d->MspDeInitCallback = HAL_GPU2D_MspDeInit;
    }

    /* DeInit the low level hardware */
    hgpu2d->MspDeInitCallback(hgpu2d);
#else /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 0 */
    /* Carry on with de-initialization of low level hardware */
    HAL_GPU2D_MspDeInit(hgpu2d);
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */
  }

#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
  /* Reset the CommandListCpltCallback handler */
  hgpu2d->CommandListCpltCallback = NULL;
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */

  /* Update error code */
  hgpu2d->ErrorCode = HAL_GPU2D_ERROR_NONE;

  /* Reset the GPU2D state*/
  hgpu2d->State = HAL_GPU2D_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hgpu2d);

  return HAL_OK;
}

/**
  * @brief  Initializes the GPU2D MSP.
  * @param  hgpu2d pointer to a GPU2D_HandleTypeDef structure that contains
  *                the configuration information for the GPU2D.
  * @retval None
  */
__weak void HAL_GPU2D_MspInit(GPU2D_HandleTypeDef *hgpu2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgpu2d);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_GPU2D_MspInit can be implemented in the user file.
   */
}

/**
  * @brief  DeInitializes the GPU2D MSP.
  * @param  hgpu2d pointer to a GPU2D_HandleTypeDef structure that contains
  *                 the configuration information for the GPU2D.
  * @retval None
  */
__weak void HAL_GPU2D_MspDeInit(GPU2D_HandleTypeDef *hgpu2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgpu2d);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_GPU2D_MspDeInit can be implemented in the user file.
   */
}

#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User GPU2D callback
  *         To be used instead of the weak (surcharged) predefined callback
  * @param hgpu2d GPU2D handle
  * @param CallbackID ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_GPU2D_MSPINIT_CB_ID GPU2D MspInit callback ID
  *          @arg @ref HAL_GPU2D_MSPDEINIT_CB_ID GPU2D MspDeInit callback ID
  * @param pCallback pointer to the callback function
  * @note Weak predefined callback is defined for HAL_GPU2D_MSPINIT_CB_ID and HAL_GPU2D_MSPDEINIT_CB_ID
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_GPU2D_RegisterCallback(GPU2D_HandleTypeDef *hgpu2d, HAL_GPU2D_CallbackIDTypeDef CallbackID,
                                             pGPU2D_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the GPU2D handle validity */
  if (hgpu2d == NULL)
  {
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hgpu2d);

  /* Check the pCallback parameter is valid or not */
  if (pCallback == NULL)
  {
    /* Update the error code */
    hgpu2d->ErrorCode |= HAL_GPU2D_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }
  else
  {
    if ((hgpu2d->State == HAL_GPU2D_STATE_READY)
        || (hgpu2d->State == HAL_GPU2D_STATE_RESET))
    {
      switch (CallbackID)
      {
        case HAL_GPU2D_MSPINIT_CB_ID:
          hgpu2d->MspInitCallback = pCallback;
          break;

        case HAL_GPU2D_MSPDEINIT_CB_ID:
          hgpu2d->MspDeInitCallback = pCallback;
          break;

        default :
          /* Update the error code */
          hgpu2d->ErrorCode |= HAL_GPU2D_ERROR_INVALID_CALLBACK;
          status = HAL_ERROR;
          break;
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  /* Release lock */
  __HAL_UNLOCK(hgpu2d);

  return status;
}

/**
  * @brief  Unregister a GPU2D callback
  *         GPU2D Callback is redirected to the weak (surcharged) predefined callback
  * @param hgpu2d GPU2D handle
  * @param CallbackID ID of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_GPU2D_MSPINIT_CB_ID GPU2D MspInit callback ID
  *          @arg @ref HAL_GPU2D_MSPDEINIT_CB_ID GPU2D MspDeInit callback ID
  * @note Callback pointers will be set to legacy weak predefined callbacks for HAL_GPU2D_MSPINIT_CB_ID and
  *       HAL_GPU2D_MSPDEINIT_CB_ID
  * @retval status
  */
HAL_StatusTypeDef HAL_GPU2D_UnRegisterCallback(GPU2D_HandleTypeDef *hgpu2d, HAL_GPU2D_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the GPU2D handle validity */
  if (hgpu2d == NULL)
  {
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hgpu2d);

  if ((HAL_GPU2D_STATE_READY == hgpu2d->State)
      || (HAL_GPU2D_STATE_RESET == hgpu2d->State))
  {
    switch (CallbackID)
    {
      case HAL_GPU2D_MSPINIT_CB_ID:
        hgpu2d->MspInitCallback = HAL_GPU2D_MspInit; /* Legacy weak Msp Init */
        break;

      case HAL_GPU2D_MSPDEINIT_CB_ID:
        hgpu2d->MspDeInitCallback = HAL_GPU2D_MspDeInit; /* Legacy weak Msp DeInit */
        break;

      default :
        /* Update the error code */
        hgpu2d->ErrorCode |= HAL_GPU2D_ERROR_INVALID_CALLBACK;
        status = HAL_ERROR;
        break;
    }
  }
  else
  {
    status = HAL_ERROR;
  }

  /* Release lock */
  __HAL_UNLOCK(hgpu2d);

  return status;
}

/**
  * @brief  Register GPU2D Command List Complete Callback
  *         To be used instead of the weak (surcharged) predefined callback
  * @param hgpu2d GPU2D handle
  * @param pCallback pointer to the Command List Complete Callback function
  * @note Weak predefined callback is defined for Command List Complete
  * @retval status
  */
HAL_StatusTypeDef HAL_GPU2D_RegisterCommandListCpltCallback(GPU2D_HandleTypeDef *hgpu2d,
                                                            pGPU2D_CommandListCpltCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the GPU2D handle validity */
  if (hgpu2d == NULL)
  {
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hgpu2d);

  /* Check the CallbackID is valid or not */
  if (pCallback == NULL)
  {
    /* Update the error code */
    hgpu2d->ErrorCode |= HAL_GPU2D_ERROR_INVALID_CALLBACK;
    status = HAL_ERROR;
  }
  else
  {
    if ((HAL_GPU2D_STATE_READY == hgpu2d->State)
        || (HAL_GPU2D_STATE_RESET == hgpu2d->State))
    {
      hgpu2d->CommandListCpltCallback = pCallback;
    }
    else
    {
      status = HAL_ERROR;
    }
  }

  /* Release lock */
  __HAL_UNLOCK(hgpu2d);

  return status;
}

/**
  * @brief  Unregister a GPU2D Command List Complete Callback
  *         GPU2D Command List Complete Callback is redirected to the weak (surcharged) predefined callback
  * @param hgpu2d GPU2D handle
  * @note Callback pointer will be invalidate (NULL value)
  * @retval status
  */
HAL_StatusTypeDef HAL_GPU2D_UnRegisterCommandListCpltCallback(GPU2D_HandleTypeDef *hgpu2d)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the GPU2D handle validity */
  if (hgpu2d == NULL)
  {
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hgpu2d);

  if ((hgpu2d->State == HAL_GPU2D_STATE_READY)
      || (hgpu2d->State == HAL_GPU2D_STATE_RESET))
  {
    hgpu2d->CommandListCpltCallback = NULL; /* Invalidate the Callback pointer */
  }
  else
  {
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hgpu2d);

  return status;
}
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */

/**
  * @}
  */


/** @defgroup GPU2D_Exported_Functions_Group2 IO operation functions
  *  @brief   IO operation functions
  *
@verbatim
 ===============================================================================
                      #####  IO operation functions  #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Read GPU2D Register value.
      (+) Write a value to GPU2D Register.
      (+) handle GPU2D interrupt request.
      (+) Command List Complete Transfer Complete callback.


@endverbatim
  * @{
  */

/**
  * @brief  Read GPU2D Register. Helper function for the higher-level library.
  * @param  hgpu2d    Pointer to a GPU2D_HandleTypeDef structure that contains
  *                   the configuration information for the GPU2D.
  * @param  offset    The register offset from GPU2D base address to read.
  * @retval Register value
  */
uint32_t HAL_GPU2D_ReadRegister(const GPU2D_HandleTypeDef *hgpu2d, uint32_t offset)
{
  uint32_t value;

  /* Check the GPU2D handle validity */
  assert_param(hgpu2d != NULL);

  /* Check the parameters */
  assert_param(IS_GPU2D_ALL_INSTANCE(hgpu2d->Instance));
  assert_param(IS_GPU2D_OFFSET(offset));

  /* No locking is required since reading a register is an atomic operation
   * and doesn't incur a state change in hal_gpu2d. */
  value = GPU2D_ReadReg(hgpu2d->Instance, offset);

  return value;
}

/**
  * @brief  Write a value to GPU2D Register. Helper function for the higher-level library.
  * @param  hgpu2d    Pointer to a GPU2D_HandleTypeDef structure that contains
  *                   the configuration information for the GPU2D.
  * @param  offset    The register offset from GPU2D base address to write.
  * @param  value     The value to be written to provided register.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_GPU2D_WriteRegister(GPU2D_HandleTypeDef *hgpu2d, uint32_t offset, uint32_t value)
{
  /* Check the GPU2D handle validity */
  assert_param(hgpu2d != NULL);

  /* Check the parameters */
  assert_param(IS_GPU2D_ALL_INSTANCE(hgpu2d->Instance));
  assert_param(IS_GPU2D_OFFSET(offset));

  /* No locking is required since writing a register is an atomic operation
   * and doesn't incur a state change in hal_gpu2d. */
  GPU2D_WriteReg(hgpu2d->Instance, offset, value);

  return HAL_OK;
}

/**
  * @brief  Handle GPU2D interrupt request.
  * @param  hgpu2d Pointer to a GPU2D_HandleTypeDef structure that contains
  *                the configuration information for the GPU2D.
  * @retval None
  */
void HAL_GPU2D_IRQHandler(GPU2D_HandleTypeDef *hgpu2d)
{
  uint32_t isr_flags = GPU2D_ReadReg(hgpu2d->Instance, GPU2D_ITCTRL);

  /* Command List Complete Interrupt management */
  if ((isr_flags & GPU2D_FLAG_CLC) != 0U)
  {
    uint32_t last_cl_id;

    /* Clear the completion flag */
    __HAL_GPU2D_CLEAR_FLAG(hgpu2d, GPU2D_FLAG_CLC);

    last_cl_id = GPU2D_ReadReg(hgpu2d->Instance, GPU2D_CLID);

    /* Command List Complete Callback */
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    if (hgpu2d->CommandListCpltCallback != NULL)
    {
      hgpu2d->CommandListCpltCallback(hgpu2d, last_cl_id);
    }
#else /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 0 */
    HAL_GPU2D_CommandListCpltCallback(hgpu2d, last_cl_id);
#endif /* USE_HAL_GPU2D_REGISTER_CALLBACKS = 1 */
  }
}

/**
  * @brief  Handle GPU2D Error interrupt request.
  * @param  hgpu2d Pointer to a GPU2D_HandleTypeDef structure that contains
  *                the configuration information for the GPU2D.
  * @retval None
  */
void HAL_GPU2D_ER_IRQHandler(GPU2D_HandleTypeDef *hgpu2d)
{
  HAL_GPU2D_ErrorCallback(hgpu2d);
}

/**
  * @brief  Command List Complete callback.
  * @param  hgpu2d pointer to a GPU2D_HandleTypeDef structure that contains
  *                the configuration information for the GPU2D.
  * @param  CmdListID Command list ID that got completed.
  * @retval None
  */
__weak void HAL_GPU2D_CommandListCpltCallback(GPU2D_HandleTypeDef *hgpu2d, uint32_t CmdListID)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgpu2d);
  UNUSED(CmdListID);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_GPU2D_CommandListCpltCallback can be implemented in the user file.
   */
}

/**
  * @brief  Error handler callback.
  * @param  hgpu2d pointer to a GPU2D_HandleTypeDef structure that contains
  *                the configuration information for the GPU2D.
  * @retval None
  */
__weak void HAL_GPU2D_ErrorCallback(GPU2D_HandleTypeDef *hgpu2d)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hgpu2d);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_GPU2D_ErrorCallback can be implemented in the user file.
            The default implementation stops the execution as an error is considered
            fatal and non recoverable.
   */

  for (;;)
  {
    /* infinite loop */
  }
}

/**
  * @}
  */


/** @defgroup GPU2D_Exported_Functions_Group3 Peripheral State and Error functions
  *  @brief    Peripheral State functions
  *
@verbatim
 ===============================================================================
                  ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection provides functions allowing to:
      (+) Get the GPU2D state
      (+) Get the GPU2D error code

@endverbatim
  * @{
  */

/**
  * @brief  Return the GPU2D state
  * @param  hgpu2d pointer to a GPU2D_HandleTypeDef structure that contains
  *                the configuration information for the GPU2D.
  * @retval GPU2D state
  */
HAL_GPU2D_StateTypeDef HAL_GPU2D_GetState(GPU2D_HandleTypeDef const *const hgpu2d)
{
  return hgpu2d->State;
}

/**
  * @brief  Return the GPU2D error code
  * @param  hgpu2d  pointer to a GPU2D_HandleTypeDef structure that contains
  *                 the configuration information for GPU2D.
  * @retval GPU2D Error Code
  */
uint32_t HAL_GPU2D_GetError(GPU2D_HandleTypeDef const *const hgpu2d)
{
  return hgpu2d->ErrorCode;
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

/**
  * @}
  */
#endif /* GPU2D */
#endif /* HAL_GPU2D_MODULE_ENABLED */
