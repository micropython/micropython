/**
  ******************************************************************************
  * @file    stm32n6xx_hal_pka.c
  * @author  MCD Application Team
  * @brief   PKA HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of public key accelerator(PKA):
  *           + Initialization and de-initialization functions
  *           + Start an operation
  *           + Retrieve the operation result
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
    The PKA HAL driver can be used as follows:

    (#) Declare a PKA_HandleTypeDef handle structure, for example: PKA_HandleTypeDef  hpka;

    (#) Initialize the PKA low level resources by implementing the HAL_PKA_MspInit() API:
        (##) Enable the PKA interface clock
        (##) NVIC configuration if you need to use interrupt process
            (+++) Configure the PKA interrupt priority
            (+++) Enable the NVIC PKA IRQ Channel

    (#) Initialize the PKA registers by calling the HAL_PKA_Init() API which trig
        HAL_PKA_MspInit().

    (#) Fill entirely the input structure corresponding to your operation:
        For instance: PKA_ModExpInTypeDef for HAL_PKA_ModExp().

    (#) Execute the operation (in polling or interrupt) and check the returned value.

    (#) Retrieve the result of the operation (For instance, HAL_PKA_ModExp_GetResult for
        HAL_PKA_ModExp operation). The function to gather the result is different for each
        kind of operation. The correspondence can be found in the following section.

    (#) Call the function HAL_PKA_DeInit() to restore the default configuration which trig
        HAL_PKA_MspDeInit().

    *** High level operation ***
    =================================
    [..]
      (+) Input structure requires buffers as uint8_t array.

      (+) Output structure requires buffers as uint8_t array.

      (+) Modular exponentiation using:
      (++) HAL_PKA_ModExp().
      (++) HAL_PKA_ModExp_IT().
      (++) HAL_PKA_ModExpFastMode().
      (++) HAL_PKA_ModExpFastMode_IT().
      (++) HAL_PKA_ModExpProtectMode().
      (++) HAL_PKA_ModExpProtectMode_IT().
      (++) HAL_PKA_ModExp_GetResult() to retrieve the result of the operation.

      (+) RSA Chinese Remainder Theorem (CRT) using:
      (++) HAL_PKA_RSACRTExp().
      (++) HAL_PKA_RSACRTExp_IT().
      (++) HAL_PKA_RSACRTExp_GetResult() to retrieve the result of the operation.

      (+) ECC Point Check using:
      (++) HAL_PKA_PointCheck().
      (++) HAL_PKA_PointCheck_IT().
      (++) HAL_PKA_PointCheck_IsOnCurve() to retrieve the result of the operation.

      (+) ECDSA Sign
      (++) HAL_PKA_ECDSASign().
      (++) HAL_PKA_ECDSASign_IT().
      (++) HAL_PKA_ECDSASign_GetResult() to retrieve the result of the operation.

      (+) ECDSA Verify
      (++) HAL_PKA_ECDSAVerif().
      (++) HAL_PKA_ECDSAVerif_IT().
      (++) HAL_PKA_ECDSAVerif_IsValidSignature() to retrieve the result of the operation.

      (+) ECC Scalar Multiplication using:
      (++) HAL_PKA_ECCMul().
      (++) HAL_PKA_ECCMul_IT().
      (++) HAL_PKA_ECCMul_GetResult() to retrieve the result of the operation.

      (+) ECC  double base  ladder using:
      (++) HAL_PKA_ECCDoubleBaseLadder().
      (++) HAL_PKA_ECCDoubleBaseLadder_IT().
      (++) HAL_PKA_ECCDoubleBaseLadder_GetResult() to retrieve the result of the operation.

      (+) ECC  projective to affine using:
      (++) HAL_PKA_ECCProjective2Affine().
      (++) HAL_PKA_ECCProjective2Affine_IT().
      (++) HAL_PKA_ECCProjective2Affine_GetResult() to retrieve the result of the operation.

      (+) ECC  complete addition using:
      (++) HAL_PKA_ECCCompleteAddition().
      (++) HAL_PKA_ECCCompleteAddition_IT().
      (++) HAL_PKA_ECCCompleteAddition_GetResult() to retrieve the result of the operation.

    *** Low level operation ***
    =================================
    [..]
      (+) Input structure requires buffers as uint32_t array.

      (+) Output structure requires buffers as uint32_t array.

      (+) Arithmetic addition using:
      (++) HAL_PKA_Add().
      (++) HAL_PKA_Add_IT().
      (++) HAL_PKA_Arithmetic_GetResult() to retrieve the result of the operation.
            The resulting size can be the input parameter or the input parameter size + 1 (overflow).

      (+) Arithmetic subtraction using:
      (++) HAL_PKA_Sub().
      (++) HAL_PKA_Sub_IT().
      (++) HAL_PKA_Arithmetic_GetResult() to retrieve the result of the operation.

      (+) Arithmetic multiplication using:
      (++) HAL_PKA_Mul().
      (++) HAL_PKA_Mul_IT().
      (++) HAL_PKA_Arithmetic_GetResult() to retrieve the result of the operation.

      (+) Comparison using:
      (++) HAL_PKA_Cmp().
      (++) HAL_PKA_Cmp_IT().
      (++) HAL_PKA_Arithmetic_GetResult() to retrieve the result of the operation.

      (+) Modular addition using:
      (++) HAL_PKA_ModAdd().
      (++) HAL_PKA_ModAdd_IT().
      (++) HAL_PKA_Arithmetic_GetResult() to retrieve the result of the operation.

      (+) Modular subtraction using:
      (++) HAL_PKA_ModSub().
      (++) HAL_PKA_ModSub_IT().
      (++) HAL_PKA_Arithmetic_GetResult() to retrieve the result of the operation.

      (+) Modular inversion using:
      (++) HAL_PKA_ModInv().
      (++) HAL_PKA_ModInv_IT().
      (++) HAL_PKA_Arithmetic_GetResult() to retrieve the result of the operation.

      (+) Modular reduction using:
      (++) HAL_PKA_ModRed().
      (++) HAL_PKA_ModRed_IT().
      (++) HAL_PKA_Arithmetic_GetResult() to retrieve the result of the operation.

      (+) Montgomery multiplication using:
      (++) HAL_PKA_MontgomeryMul().
      (++) HAL_PKA_MontgomeryMul_IT().
      (++) HAL_PKA_Arithmetic_GetResult() to retrieve the result of the operation.

    *** Montgomery parameter ***
    =================================
      (+) For some operation, the computation of the Montgomery parameter is a prerequisite.
      (+) Input structure requires buffers as uint8_t array.
      (+) Output structure requires buffers as uint32_t array.(Only used inside PKA).
      (+) You can compute the Montgomery parameter using:
      (++) HAL_PKA_MontgomeryParam().
      (++) HAL_PKA_MontgomeryParam_IT().
      (++) HAL_PKA_MontgomeryParam_GetResult() to retrieve the result of the operation.

    *** Polling mode operation ***
    ===================================
    [..]
      (+) When an operation is started in polling mode, the function returns when:
      (++) A timeout is encounter.
      (++) The operation is completed.

    *** Interrupt mode operation ***
    ===================================
    [..]
      (+) Add HAL_PKA_IRQHandler to the IRQHandler of PKA.
      (+) Enable the IRQ using HAL_NVIC_EnableIRQ().
      (+) When an operation is started in interrupt mode, the function returns immediately.
      (+) When the operation is completed, the callback HAL_PKA_OperationCpltCallback is called.
      (+) When an error is encountered, the callback HAL_PKA_ErrorCallback is called.
      (+) To stop any operation in interrupt mode, use HAL_PKA_Abort().

    *** Utilities ***
    ===================================
    [..]
      (+) To clear the PKA RAM, use HAL_PKA_RAMReset().
      (+) To get current state, use HAL_PKA_GetState().
      (+) To get current error, use HAL_PKA_GetError().

    *** Callback registration ***
    =============================================
    [..]

     The compilation flag USE_HAL_PKA_REGISTER_CALLBACKS, when set to 1,
     allows the user to configure dynamically the driver callbacks.
     Use Functions HAL_PKA_RegisterCallback()
     to register an interrupt callback.
    [..]

     Function HAL_PKA_RegisterCallback() allows to register following callbacks:
       (+) OperationCpltCallback : callback for End of operation.
       (+) ErrorCallback         : callback for error detection.
       (+) MspInitCallback       : callback for Msp Init.
       (+) MspDeInitCallback     : callback for Msp DeInit.
     This function takes as parameters the HAL peripheral handle, the Callback ID
     and a pointer to the user callback function.
    [..]

     Use function HAL_PKA_UnRegisterCallback to reset a callback to the default
     weak function.
    [..]

     HAL_PKA_UnRegisterCallback takes as parameters the HAL peripheral handle,
     and the Callback ID.
     This function allows to reset following callbacks:
       (+) OperationCpltCallback : callback for End of operation.
       (+) ErrorCallback         : callback for error detection.
       (+) MspInitCallback       : callback for Msp Init.
       (+) MspDeInitCallback     : callback for Msp DeInit.
     [..]

     By default, after the HAL_PKA_Init() and when the state is HAL_PKA_STATE_RESET
     all callbacks are set to the corresponding weak functions:
     examples HAL_PKA_OperationCpltCallback(), HAL_PKA_ErrorCallback().
     Exception done for MspInit and MspDeInit functions that are
     reset to the legacy weak functions in the HAL_PKA_Init()/ HAL_PKA_DeInit() only when
     these callbacks are null (not registered beforehand).
    [..]

     If MspInit or MspDeInit are not null, the HAL_PKA_Init()/ HAL_PKA_DeInit()
     keep and use the user MspInit/MspDeInit callbacks (registered beforehand) whatever the state.
     [..]

     Callbacks can be registered/unregistered in HAL_PKA_STATE_READY state only.
     Exception done MspInit/MspDeInit functions that can be registered/unregistered
     in HAL_PKA_STATE_READY or HAL_PKA_STATE_RESET state,
     thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
    [..]

     Then, the user first registers the MspInit/MspDeInit user callbacks
     using HAL_PKA_RegisterCallback() before calling HAL_PKA_DeInit()
     or HAL_PKA_Init() function.
     [..]

     When the compilation flag USE_HAL_PKA_REGISTER_CALLBACKS is set to 0 or
     not defined, the callback registration feature is not available and all callbacks
     are set to the corresponding weak functions.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#if defined(PKA) && defined(HAL_PKA_MODULE_ENABLED)

/** @defgroup PKA PKA
  * @brief    PKA HAL module driver.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup PKA_Private_Define PKA Private Define
  * @{
  */
#define PKA_RAM_SIZE 1334U

/* Private macro -------------------------------------------------------------*/
#define __PKA_RAM_PARAM_END(TAB,INDEX)                do{                                   \
                                                                    TAB[INDEX] = 0UL;       \
                                                                    TAB[INDEX + 1U] = 0UL;  \
                                                                  } while(0)
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
static uint32_t   primeordersize;
static uint32_t   opsize;
static uint32_t   modulussize;
/* Private function prototypes -----------------------------------------------*/
/** @defgroup PKA_Private_Functions PKA Private Functions
  * @{
  */
uint32_t PKA_GetMode(const PKA_HandleTypeDef *hpka);
HAL_StatusTypeDef PKA_PollEndOfOperation(const PKA_HandleTypeDef *hpka, uint32_t Timeout, uint32_t Tickstart);
uint32_t PKA_CheckError(const PKA_HandleTypeDef *hpka, uint32_t mode);
uint32_t PKA_GetBitSize_u8(uint32_t byteNumber);
uint32_t PKA_GetOptBitSize_u8(uint32_t byteNumber, uint8_t msb);
uint32_t PKA_GetBitSize_u32(uint32_t wordNumber);
uint32_t PKA_GetArraySize_u8(uint32_t bitSize);
void PKA_Memcpy_u32_to_u8(uint8_t dst[], __IO const uint32_t src[], size_t n);
void PKA_Memcpy_u8_to_u32(__IO uint32_t dst[], const uint8_t src[], size_t n);
void PKA_Memcpy_u32_to_u32(__IO uint32_t dst[], __IO const uint32_t src[], size_t n);
HAL_StatusTypeDef PKA_Process(PKA_HandleTypeDef *hpka, uint32_t mode, uint32_t Timeout);
HAL_StatusTypeDef PKA_Process_IT(PKA_HandleTypeDef *hpka, uint32_t mode);
void PKA_ModExp_Set(PKA_HandleTypeDef *hpka, PKA_ModExpInTypeDef *in);
void PKA_ModExpFastMode_Set(PKA_HandleTypeDef *hpka, PKA_ModExpFastModeInTypeDef *in);
void PKA_ModExpProtectMode_Set(PKA_HandleTypeDef *hpka, PKA_ModExpProtectModeInTypeDef *in);
void PKA_ECCMulEx_Set(PKA_HandleTypeDef *hpka, PKA_ECCMulExInTypeDef *in);
void PKA_ECDSASign_Set(PKA_HandleTypeDef *hpka, PKA_ECDSASignInTypeDef *in);
void PKA_ECDSAVerif_Set(PKA_HandleTypeDef *hpka, PKA_ECDSAVerifInTypeDef *in);
void PKA_RSACRTExp_Set(PKA_HandleTypeDef *hpka, PKA_RSACRTExpInTypeDef *in);
void PKA_PointCheck_Set(PKA_HandleTypeDef *hpka, PKA_PointCheckInTypeDef *in);
void PKA_ECCMul_Set(PKA_HandleTypeDef *hpka, PKA_ECCMulInTypeDef *in);
void PKA_ModRed_Set(PKA_HandleTypeDef *hpka, PKA_ModRedInTypeDef *in);
void PKA_ModInv_Set(PKA_HandleTypeDef *hpka, PKA_ModInvInTypeDef *in);
void PKA_MontgomeryParam_Set(PKA_HandleTypeDef *hpka, const uint32_t size, const uint8_t *pOp1);
void PKA_ARI_Set(PKA_HandleTypeDef *hpka, const uint32_t size, const uint32_t *pOp1, const uint32_t *pOp2,
                 const uint8_t *pOp3);
void PKA_ECCDoubleBaseLadder_Set(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderInTypeDef *in);
void PKA_ECCProjective2Affine_Set(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineInTypeDef *in);
void PKA_ECCCompleteAddition_Set(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionInTypeDef *in);
HAL_StatusTypeDef PKA_WaitOnFlagUntilTimeout(PKA_HandleTypeDef *hpka, uint32_t Flag, FlagStatus Status,
                                             uint32_t Tickstart, uint32_t Timeout);
uint32_t PKA_Result_GetSize(const PKA_HandleTypeDef *hpka, uint32_t Startindex, uint32_t Maxsize);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup PKA_Exported_Functions PKA Exported Functions
  * @{
  */

/** @defgroup PKA_Exported_Functions_Group1 Initialization and de-initialization functions
  * @brief    Initialization and de-initialization functions
  *
@verbatim
 ===============================================================================
             ##### Initialization and de-initialization functions  #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          deinitialize the PKAx peripheral:

      (+) User must implement HAL_PKA_MspInit() function in which he configures
          all related peripherals resources (CLOCK, IT and NVIC ).

      (+) Call the function HAL_PKA_Init() to configure the device.

      (+) Call the function HAL_PKA_DeInit() to restore the default configuration
          of the selected PKAx peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the PKA according to the specified
  *         parameters in the PKA_InitTypeDef and initialize the associated handle.
  * @param  hpka PKA handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Init(PKA_HandleTypeDef *hpka)
{
  HAL_StatusTypeDef err = HAL_OK;
  uint32_t tickstart;

  /* Check the PKA handle allocation */
  if (hpka != NULL)
  {
    /* Check the parameters */
    assert_param(IS_PKA_ALL_INSTANCE(hpka->Instance));

    if (hpka->State == HAL_PKA_STATE_RESET)
    {

#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
      /* Init the PKA Callback settings */
      hpka->OperationCpltCallback = HAL_PKA_OperationCpltCallback; /* Legacy weak OperationCpltCallback */
      hpka->ErrorCallback         = HAL_PKA_ErrorCallback;         /* Legacy weak ErrorCallback         */

      if (hpka->MspInitCallback == NULL)
      {
        hpka->MspInitCallback = HAL_PKA_MspInit; /* Legacy weak MspInit  */
      }

      /* Init the low level hardware */
      hpka->MspInitCallback(hpka);
#else
      /* Init the low level hardware */
      HAL_PKA_MspInit(hpka);
#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */
    }

    /* Set the state to busy */
    hpka->State = HAL_PKA_STATE_BUSY;

    /* Reset the control register and enable the PKA */
    hpka->Instance->CR = PKA_CR_EN;

    /* Get current tick */
    tickstart = HAL_GetTick();

    /* Wait the INITOK flag Setting */
    if (PKA_WaitOnFlagUntilTimeout(hpka, PKA_SR_INITOK, RESET, tickstart, 5000) != HAL_OK)
    {
      return HAL_TIMEOUT;
    }

    /* Reset any pending flag */
    SET_BIT(hpka->Instance->CLRFR, PKA_CLRFR_PROCENDFC | PKA_CLRFR_RAMERRFC | PKA_CLRFR_ADDRERRFC | PKA_CLRFR_OPERRFC);

    /* Initialize the error code */
    hpka->ErrorCode = HAL_PKA_ERROR_NONE;

    /* Set the state to ready */
    hpka->State = HAL_PKA_STATE_READY;
  }
  else
  {
    err = HAL_ERROR;
  }

  return err;
}

/**
  * @brief  DeInitialize the PKA peripheral.
  * @param  hpka PKA handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_DeInit(PKA_HandleTypeDef *hpka)
{
  HAL_StatusTypeDef err = HAL_OK;

  /* Check the PKA handle allocation */
  if (hpka != NULL)
  {
    /* Check the parameters */
    assert_param(IS_PKA_ALL_INSTANCE(hpka->Instance));

    /* Set the state to busy */
    hpka->State = HAL_PKA_STATE_BUSY;

    /* Reset the control register */
    /* This abort any operation in progress (PKA RAM content is not guaranteed in this case) */
    hpka->Instance->CR = 0;

    /* Reset any pending flag */
    SET_BIT(hpka->Instance->CLRFR, PKA_CLRFR_PROCENDFC | PKA_CLRFR_RAMERRFC | PKA_CLRFR_ADDRERRFC | PKA_CLRFR_OPERRFC);

#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
    if (hpka->MspDeInitCallback == NULL)
    {
      hpka->MspDeInitCallback = HAL_PKA_MspDeInit; /* Legacy weak MspDeInit  */
    }

    /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
    hpka->MspDeInitCallback(hpka);
#else
    /* DeInit the low level hardware: CLOCK, NVIC */
    HAL_PKA_MspDeInit(hpka);
#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */

    /* Reset the error code */
    hpka->ErrorCode = HAL_PKA_ERROR_NONE;

    /* Reset the state */
    hpka->State = HAL_PKA_STATE_RESET;
  }
  else
  {
    err = HAL_ERROR;
  }

  return err;
}

/**
  * @brief  Initialize the PKA MSP.
  * @param  hpka PKA handle
  * @retval None
  */
__weak void HAL_PKA_MspInit(PKA_HandleTypeDef *hpka)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpka);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PKA_MspInit can be implemented in the user file
   */
}

/**
  * @brief  DeInitialize the PKA MSP.
  * @param  hpka PKA handle
  * @retval None
  */
__weak void HAL_PKA_MspDeInit(PKA_HandleTypeDef *hpka)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpka);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PKA_MspDeInit can be implemented in the user file
   */
}

#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User PKA Callback
  *         To be used instead of the weak predefined callback
  * @param  hpka Pointer to a PKA_HandleTypeDef structure that contains
  *                the configuration information for the specified PKA.
  * @param  CallbackID ID of the callback to be registered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_PKA_OPERATION_COMPLETE_CB_ID End of operation callback ID
  *          @arg @ref HAL_PKA_ERROR_CB_ID Error callback ID
  *          @arg @ref HAL_PKA_MSPINIT_CB_ID MspInit callback ID
  *          @arg @ref HAL_PKA_MSPDEINIT_CB_ID MspDeInit callback ID
  * @param  pCallback pointer to the Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_RegisterCallback(PKA_HandleTypeDef *hpka, HAL_PKA_CallbackIDTypeDef CallbackID,
                                           pPKA_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hpka->ErrorCode |= HAL_PKA_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }

  if (HAL_PKA_STATE_READY == hpka->State)
  {
    switch (CallbackID)
    {
      case HAL_PKA_OPERATION_COMPLETE_CB_ID :
        hpka->OperationCpltCallback = pCallback;
        break;

      case HAL_PKA_ERROR_CB_ID :
        hpka->ErrorCallback = pCallback;
        break;

      case HAL_PKA_MSPINIT_CB_ID :
        hpka->MspInitCallback = pCallback;
        break;

      case HAL_PKA_MSPDEINIT_CB_ID :
        hpka->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hpka->ErrorCode |= HAL_PKA_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status = HAL_ERROR;
        break;
    }
  }
  else if (HAL_PKA_STATE_RESET == hpka->State)
  {
    switch (CallbackID)
    {
      case HAL_PKA_MSPINIT_CB_ID :
        hpka->MspInitCallback = pCallback;
        break;

      case HAL_PKA_MSPDEINIT_CB_ID :
        hpka->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hpka->ErrorCode |= HAL_PKA_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status = HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hpka->ErrorCode |= HAL_PKA_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a PKA Callback
  *         PKA callback is redirected to the weak predefined callback
  * @param  hpka Pointer to a PKA_HandleTypeDef structure that contains
  *                the configuration information for the specified PKA.
  * @param  CallbackID ID of the callback to be unregistered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_PKA_OPERATION_COMPLETE_CB_ID End of operation callback ID
  *          @arg @ref HAL_PKA_ERROR_CB_ID Error callback ID
  *          @arg @ref HAL_PKA_MSPINIT_CB_ID MspInit callback ID
  *          @arg @ref HAL_PKA_MSPDEINIT_CB_ID MspDeInit callback ID
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_UnRegisterCallback(PKA_HandleTypeDef *hpka, HAL_PKA_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (HAL_PKA_STATE_READY == hpka->State)
  {
    switch (CallbackID)
    {
      case HAL_PKA_OPERATION_COMPLETE_CB_ID :
        hpka->OperationCpltCallback = HAL_PKA_OperationCpltCallback; /* Legacy weak OperationCpltCallback */
        break;

      case HAL_PKA_ERROR_CB_ID :
        hpka->ErrorCallback = HAL_PKA_ErrorCallback;                 /* Legacy weak ErrorCallback        */
        break;

      case HAL_PKA_MSPINIT_CB_ID :
        hpka->MspInitCallback = HAL_PKA_MspInit;                     /* Legacy weak MspInit              */
        break;

      case HAL_PKA_MSPDEINIT_CB_ID :
        hpka->MspDeInitCallback = HAL_PKA_MspDeInit;                 /* Legacy weak MspDeInit            */
        break;

      default :
        /* Update the error code */
        hpka->ErrorCode |= HAL_PKA_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (HAL_PKA_STATE_RESET == hpka->State)
  {
    switch (CallbackID)
    {
      case HAL_PKA_MSPINIT_CB_ID :
        hpka->MspInitCallback = HAL_PKA_MspInit;                   /* Legacy weak MspInit              */
        break;

      case HAL_PKA_MSPDEINIT_CB_ID :
        hpka->MspDeInitCallback = HAL_PKA_MspDeInit;               /* Legacy weak MspDeInit            */
        break;

      default :
        /* Update the error code */
        hpka->ErrorCode |= HAL_PKA_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hpka->ErrorCode |= HAL_PKA_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup PKA_Exported_Functions_Group2 IO operation functions
  * @brief    IO operation functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the PKA operations.

    (#) There are two modes of operation:

       (++) Blocking mode : The operation is performed in the polling mode.
            These functions return when data operation is completed.
       (++) No-Blocking mode : The operation is performed using Interrupts.
            These functions return immediately.
            The end of the operation is indicated by HAL_PKA_ErrorCallback in case of error.
            The end of the operation is indicated by HAL_PKA_OperationCpltCallback in case of success.
            To stop any operation in interrupt mode, use HAL_PKA_Abort().

    (#) Blocking mode functions are :

        (++) HAL_PKA_ModExp()
        (++) HAL_PKA_ModExpFastMode()
        (++) HAL_PKA_ModExpProtectMode()
        (++) HAL_PKA_ModExp_GetResult();

        (++) HAL_PKA_ECDSASign()
        (++) HAL_PKA_ECDSASign_GetResult();

        (++) HAL_PKA_ECDSAVerif()
        (++) HAL_PKA_ECDSAVerif_IsValidSignature();

        (++) HAL_PKA_RSACRTExp()
        (++) HAL_PKA_RSACRTExp_GetResult();

        (++) HAL_PKA_PointCheck()
        (++) HAL_PKA_PointCheck_IsOnCurve();

        (++) HAL_PKA_ECCMul()
        (++) HAL_PKA_ECCMulFastMode()
        (++) HAL_PKA_ECCMul_GetResult();

        (++) HAL_PKA_ECCMulEx()
        (++) HAL_PKA_ECCDoubleBaseLadder()
        (++) HAL_PKA_ECCDoubleBaseLadder_GetResult();
        (++) HAL_PKA_ECCProjective2Affine()
        (++) HAL_PKA_ECCProjective2Affine_GetResult();
        (++) HAL_PKA_ECCCompleteAddition()
        (++) HAL_PKA_ECCCompleteAddition_GetResult();

        (++) HAL_PKA_Add()
        (++) HAL_PKA_Sub()
        (++) HAL_PKA_Cmp()
        (++) HAL_PKA_Mul()
        (++) HAL_PKA_ModAdd()
        (++) HAL_PKA_ModSub()
        (++) HAL_PKA_ModInv()
        (++) HAL_PKA_ModRed()
        (++) HAL_PKA_MontgomeryMul()
        (++) HAL_PKA_Arithmetic_GetResult(P);

        (++) HAL_PKA_MontgomeryParam()
        (++) HAL_PKA_MontgomeryParam_GetResult();

    (#) No-Blocking mode functions with Interrupt are :

        (++) HAL_PKA_ModExp_IT();
        (++) HAL_PKA_ModExpFastMode_IT();
        (++) HAL_PKA_ModExpProtectMode_IT()
        (++) HAL_PKA_ModExp_GetResult();

        (++) HAL_PKA_ECDSASign_IT();
        (++) HAL_PKA_ECDSASign_GetResult();

        (++) HAL_PKA_ECDSAVerif_IT();
        (++) HAL_PKA_ECDSAVerif_IsValidSignature();

        (++) HAL_PKA_RSACRTExp_IT();
        (++) HAL_PKA_RSACRTExp_GetResult();

        (++) HAL_PKA_PointCheck_IT();
        (++) HAL_PKA_PointCheck_IsOnCurve();

        (++) HAL_PKA_ECCMul_IT();
        (++) HAL_PKA_ECCMulFastMode_IT();
        (++) HAL_PKA_ECCMul_GetResult();

        (++) HAL_PKA_ECCMulEx_IT();
        (++) HAL_PKA_ECCDoubleBaseLadder_IT()
        (++) HAL_PKA_ECCDoubleBaseLadder_GetResult();
        (++) HAL_PKA_ECCProjective2Affine_IT()
        (++) HAL_PKA_ECCProjective2Affine_GetResult();
        (++) HAL_PKA_ECCCompleteAddition_IT()
        (++) HAL_PKA_ECCCompleteAddition_GetResult();
        (++) HAL_PKA_Add_IT();
        (++) HAL_PKA_Sub_IT();
        (++) HAL_PKA_Cmp_IT();
        (++) HAL_PKA_Mul_IT();
        (++) HAL_PKA_ModAdd_IT();
        (++) HAL_PKA_ModSub_IT();
        (++) HAL_PKA_ModInv_IT();
        (++) HAL_PKA_ModRed_IT();
        (++) HAL_PKA_MontgomeryMul_IT();
        (++) HAL_PKA_Arithmetic_GetResult();

        (++) HAL_PKA_MontgomeryParam_IT();
        (++) HAL_PKA_MontgomeryParam_GetResult();

        (++) HAL_PKA_Abort();

@endverbatim
  * @{
  */

/**
  * @brief  Modular exponentiation in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModExp(PKA_HandleTypeDef *hpka, PKA_ModExpInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ModExp_Set(hpka, in);

  opsize = in->OpSize;

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_MODULAR_EXP, Timeout);
}

/**
  * @brief  Modular exponentiation in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModExp_IT(PKA_HandleTypeDef *hpka, PKA_ModExpInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ModExp_Set(hpka, in);

  opsize = in->OpSize;

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_MODULAR_EXP);
}

/**
  * @brief  Modular exponentiation in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModExpFastMode(PKA_HandleTypeDef *hpka, PKA_ModExpFastModeInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ModExpFastMode_Set(hpka, in);

  opsize = in->OpSize;

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_MODULAR_EXP_FAST_MODE, Timeout);
}

/**
  * @brief  Modular exponentiation in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModExpFastMode_IT(PKA_HandleTypeDef *hpka, PKA_ModExpFastModeInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ModExpFastMode_Set(hpka, in);

  opsize = in->OpSize;

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_MODULAR_EXP_FAST_MODE);
}

/**
  * @brief  Modular exponentiation (protected) in blocking mode.
  *         Useful when a secret information is involved (RSA decryption)
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModExpProtectMode(PKA_HandleTypeDef *hpka, PKA_ModExpProtectModeInTypeDef *in,
                                            uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ModExpProtectMode_Set(hpka, in);

  opsize = in->OpSize;

  return PKA_Process(hpka, PKA_MODE_MODULAR_EXP_PROTECT, Timeout);
}

/**
  * @brief  Modular exponentiation (protected) in non-blocking mode with Interrupt.
  *         Useful when a secret information is involved (RSA decryption)
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModExpProtectMode_IT(PKA_HandleTypeDef *hpka, PKA_ModExpProtectModeInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ModExpProtectMode_Set(hpka, in);

  opsize = in->OpSize;

  return PKA_Process_IT(hpka, PKA_MODE_MODULAR_EXP_PROTECT);
}


/**
  * @brief  Retrieve operation result.
  * @param  hpka PKA handle
  * @param  pRes Output buffer
  * @retval HAL status
  */
void HAL_PKA_ModExp_GetResult(PKA_HandleTypeDef *hpka, uint8_t *pRes)
{
  uint32_t size;

  /* Get output result size */
  size = opsize;

  /* Move the result to appropriate location (indicated in out parameter) */
  PKA_Memcpy_u32_to_u8(pRes, &hpka->Instance->RAM[PKA_MODULAR_EXP_OUT_RESULT], size);
}

/**
  * @brief  Sign a message using elliptic curves over prime fields in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECDSASign(PKA_HandleTypeDef *hpka, PKA_ECDSASignInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ECDSASign_Set(hpka, in);

  primeordersize = in->primeOrderSize;

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_ECDSA_SIGNATURE, Timeout);
}

/**
  * @brief  Sign a message using elliptic curves over prime fields in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECDSASign_IT(PKA_HandleTypeDef *hpka, PKA_ECDSASignInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ECDSASign_Set(hpka, in);

  primeordersize = in->primeOrderSize;

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_ECDSA_SIGNATURE);
}

/**
  * @brief  Retrieve operation result.
  * @param  hpka PKA handle
  * @param  out Output information
  * @param  outExt Additional Output information (facultative)
  */
void HAL_PKA_ECDSASign_GetResult(PKA_HandleTypeDef *hpka, PKA_ECDSASignOutTypeDef *out,
                                 PKA_ECDSASignOutExtParamTypeDef *outExt)
{
  uint32_t size;

  /* Get output result size */
  size = primeordersize;


  if (out != NULL)
  {
    PKA_Memcpy_u32_to_u8(out->RSign, &hpka->Instance->RAM[PKA_ECDSA_SIGN_OUT_SIGNATURE_R], size);
    PKA_Memcpy_u32_to_u8(out->SSign, &hpka->Instance->RAM[PKA_ECDSA_SIGN_OUT_SIGNATURE_S], size);
  }

  /* If user requires the additional information */
  if (outExt != NULL)
  {
    /* Move the result to appropriate location (indicated in outExt parameter) */
    PKA_Memcpy_u32_to_u8(outExt->ptX, &hpka->Instance->RAM[PKA_ECDSA_SIGN_OUT_FINAL_POINT_X], size);
    PKA_Memcpy_u32_to_u8(outExt->ptY, &hpka->Instance->RAM[PKA_ECDSA_SIGN_OUT_FINAL_POINT_Y], size);
  }
}

/**
  * @brief  Verify the validity of a signature using elliptic curves over prime fields in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECDSAVerif(PKA_HandleTypeDef *hpka, PKA_ECDSAVerifInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ECDSAVerif_Set(hpka, in);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_ECDSA_VERIFICATION, Timeout);
}

/**
  * @brief  Verify the validity of a signature using elliptic curves
  *         over prime fields in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECDSAVerif_IT(PKA_HandleTypeDef *hpka, PKA_ECDSAVerifInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ECDSAVerif_Set(hpka, in);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_ECDSA_VERIFICATION);
}

/**
  * @brief  Return the result of the ECDSA verification operation.
  * @param  hpka PKA handle
  * @retval 1 if signature is verified, 0 in other case
  */
uint32_t HAL_PKA_ECDSAVerif_IsValidSignature(PKA_HandleTypeDef const *const hpka)
{
  return (hpka->Instance->RAM[PKA_ECDSA_VERIF_OUT_RESULT] == 0xD60DU) ? 1UL : 0UL;
}

/**
  * @brief  RSA CRT exponentiation in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_RSACRTExp(PKA_HandleTypeDef *hpka, PKA_RSACRTExpInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_RSACRTExp_Set(hpka, in);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_RSA_CRT_EXP, Timeout);
}

/**
  * @brief  RSA CRT exponentiation in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_RSACRTExp_IT(PKA_HandleTypeDef *hpka, PKA_RSACRTExpInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_RSACRTExp_Set(hpka, in);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_RSA_CRT_EXP);
}

/**
  * @brief  Retrieve operation result.
  * @param  hpka PKA handle
  * @param  pRes Pointer to memory location to receive the result of the operation
  * @retval HAL status
  */
void HAL_PKA_RSACRTExp_GetResult(PKA_HandleTypeDef *hpka, uint8_t *pRes)
{
  uint32_t size;

  /* Move the result to appropriate location (indicated in out parameter) */
  size = (hpka->Instance->RAM[PKA_RSA_CRT_EXP_IN_MOD_NB_BITS] + 7UL) / 8UL;

  PKA_Memcpy_u32_to_u8(pRes, &hpka->Instance->RAM[PKA_RSA_CRT_EXP_OUT_RESULT], size);
}

/**
  * @brief  Point on elliptic curve check in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_PointCheck(PKA_HandleTypeDef *hpka, PKA_PointCheckInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_PointCheck_Set(hpka, in);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_POINT_CHECK, Timeout);
}

/**
  * @brief  Point on elliptic curve check in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_PointCheck_IT(PKA_HandleTypeDef *hpka, PKA_PointCheckInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_PointCheck_Set(hpka, in);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_POINT_CHECK);
}

/**
  * @brief  Return the result of the point check operation.
  * @param  hpka PKA handle
  * @retval 1 if point is on curve, 0 in other case
  */
uint32_t HAL_PKA_PointCheck_IsOnCurve(PKA_HandleTypeDef const *const hpka)
{
#define PKA_POINT_IS_ON_CURVE 0xD60DUL
  /* Invert the value of the PKA RAM containing the result of the operation */
  return (hpka->Instance->RAM[PKA_POINT_CHECK_OUT_ERROR] == PKA_POINT_IS_ON_CURVE) ? 1UL : 0UL;
}

/**
  * @brief  ECC scalar multiplication in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCMul(PKA_HandleTypeDef *hpka, PKA_ECCMulInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCMul_Set(hpka, in);

  modulussize = in->modulusSize;

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_ECC_MUL, Timeout);
}

/**
  * @brief  ECC scalar multiplication in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCMul_IT(PKA_HandleTypeDef *hpka, PKA_ECCMulInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCMul_Set(hpka, in);

  modulussize = in->modulusSize;

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_ECC_MUL);
}
/**
  * @brief  ECC scalar multiplication extended in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCMulEx(PKA_HandleTypeDef *hpka, PKA_ECCMulExInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCMulEx_Set(hpka, in);

  modulussize = in->modulusSize;

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_ECC_MUL, Timeout);
}

/**
  * @brief  ECC scalar multiplication extended in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCMulEx_IT(PKA_HandleTypeDef *hpka, PKA_ECCMulExInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCMulEx_Set(hpka, in);

  modulussize = in->modulusSize;

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_ECC_MUL);
}
/**
  * @brief  Retrieve operation result.
  * @param  hpka PKA handle
  * @param  out Output information
  * @retval HAL status
  */
void HAL_PKA_ECCMul_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCMulOutTypeDef *out)
{
  uint32_t size;

  /* Get output result size */
  size = modulussize;

  /* If a destination buffer is provided */
  if (out != NULL)
  {
    /* Move the result to appropriate location (indicated in out parameter) */
    PKA_Memcpy_u32_to_u8(out->ptX, &hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_OUT_RESULT_X], size);
    PKA_Memcpy_u32_to_u8(out->ptY, &hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_OUT_RESULT_Y], size);
  }
}

/**
  * @brief  Arithmetic addition in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Add(PKA_HandleTypeDef *hpka, PKA_AddInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, NULL);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_ARITHMETIC_ADD, Timeout);
}

/**
  * @brief  Arithmetic addition in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Add_IT(PKA_HandleTypeDef *hpka, PKA_AddInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, NULL);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_ARITHMETIC_ADD);
}

/**
  * @brief  Arithmetic subtraction in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Sub(PKA_HandleTypeDef *hpka, PKA_SubInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, NULL);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_ARITHMETIC_SUB, Timeout);
}

/**
  * @brief  Arithmetic subtraction in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Sub_IT(PKA_HandleTypeDef *hpka, PKA_SubInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, NULL);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_ARITHMETIC_SUB);
}

/**
  * @brief  Arithmetic multiplication in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Mul(PKA_HandleTypeDef *hpka, PKA_MulInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, NULL);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_ARITHMETIC_MUL, Timeout);
}

/**
  * @brief  Arithmetic multiplication in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Mul_IT(PKA_HandleTypeDef *hpka, PKA_MulInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, NULL);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_ARITHMETIC_MUL);
}

/**
  * @brief  Comparison in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Cmp(PKA_HandleTypeDef *hpka, PKA_CmpInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, NULL);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_COMPARISON, Timeout);
}

/**
  * @brief  Comparison in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Cmp_IT(PKA_HandleTypeDef *hpka, PKA_CmpInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, NULL);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_COMPARISON);
}

/**
  * @brief  Modular addition in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModAdd(PKA_HandleTypeDef *hpka, PKA_ModAddInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, in->pOp3);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_MODULAR_ADD, Timeout);
}

/**
  * @brief  Modular addition in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModAdd_IT(PKA_HandleTypeDef *hpka, PKA_ModAddInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, in->pOp3);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_MODULAR_ADD);
}

/**
  * @brief  Modular inversion in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModInv(PKA_HandleTypeDef *hpka, PKA_ModInvInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ModInv_Set(hpka, in);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_MODULAR_INV, Timeout);
}

/**
  * @brief  Modular inversion in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModInv_IT(PKA_HandleTypeDef *hpka, PKA_ModInvInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ModInv_Set(hpka, in);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_MODULAR_INV);
}

/**
  * @brief  Modular subtraction in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModSub(PKA_HandleTypeDef *hpka, PKA_ModSubInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, in->pOp3);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_MODULAR_SUB, Timeout);
}

/**
  * @brief  Modular subtraction in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModSub_IT(PKA_HandleTypeDef *hpka, PKA_ModSubInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, in->pOp3);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_MODULAR_SUB);
}

/**
  * @brief  Modular reduction in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModRed(PKA_HandleTypeDef *hpka, PKA_ModRedInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ModRed_Set(hpka, in);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_MODULAR_RED, Timeout);
}

/**
  * @brief  Modular reduction in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ModRed_IT(PKA_HandleTypeDef *hpka, PKA_ModRedInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ModRed_Set(hpka, in);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_MODULAR_RED);
}

/**
  * @brief  Montgomery multiplication in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_MontgomeryMul(PKA_HandleTypeDef *hpka, PKA_MontgomeryMulInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, in->pOp3);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_MONTGOMERY_MUL, Timeout);
}

/**
  * @brief  Montgomery multiplication in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_MontgomeryMul_IT(PKA_HandleTypeDef *hpka, PKA_MontgomeryMulInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ARI_Set(hpka, in->size, in->pOp1, in->pOp2, in->pOp3);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_MONTGOMERY_MUL);
}

/**
  * @brief  Retrieve operation result.
  * @param  hpka PKA handle
  * @param  pRes Pointer to memory location to receive the result of the operation
  */
void HAL_PKA_Arithmetic_GetResult(PKA_HandleTypeDef *hpka, uint32_t *pRes)
{
  uint32_t mode = (hpka->Instance->CR & PKA_CR_MODE_Msk) >> PKA_CR_MODE_Pos;
  uint32_t size = 0;

  /* Move the result to appropriate location (indicated in pRes parameter) */
  switch (mode)
  {
    case PKA_MODE_MONTGOMERY_PARAM:
    case PKA_MODE_ARITHMETIC_SUB:
    case PKA_MODE_MODULAR_ADD:
    case PKA_MODE_MODULAR_RED:
    case PKA_MODE_MODULAR_INV:
    case PKA_MODE_MONTGOMERY_MUL:
      size = hpka->Instance->RAM[2] / 32UL;
      break;
    case PKA_MODE_ARITHMETIC_ADD:
    case PKA_MODE_MODULAR_SUB:
      size = hpka->Instance->RAM[PKA_ARITHMETIC_ALL_OPS_NB_BITS] / 32UL;

      /* Manage the overflow of the addition */
      if (hpka->Instance->RAM[PKA_ARITHMETIC_ALL_OPS_OUT_RESULT + size] != 0UL)
      {
        size += 1UL;
      }

      break;
    case PKA_MODE_COMPARISON:
      size = 1;
      break;
    case PKA_MODE_ARITHMETIC_MUL:
      size = hpka->Instance->RAM[PKA_ARITHMETIC_MUL_NB_BITS] / 32UL * 2UL;
      break;
    default:
      break;
  }

  if (pRes != NULL)
  {
    switch (mode)
    {
      case PKA_MODE_ARITHMETIC_SUB:
      case PKA_MODE_MODULAR_ADD:
      case PKA_MODE_MODULAR_RED:
      case PKA_MODE_MODULAR_INV:
      case PKA_MODE_MODULAR_SUB:
      case PKA_MODE_MONTGOMERY_MUL:
      case PKA_MODE_ARITHMETIC_ADD:
      case PKA_MODE_COMPARISON:
      case PKA_MODE_ARITHMETIC_MUL:
        PKA_Memcpy_u32_to_u32(pRes, &hpka->Instance->RAM[PKA_ARITHMETIC_ALL_OPS_OUT_RESULT], size);
        break;
      default:
        break;
    }
  }
}

/**
  * @brief  Montgomery parameter computation in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_MontgomeryParam(PKA_HandleTypeDef *hpka, PKA_MontgomeryParamInTypeDef *in, uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_MontgomeryParam_Set(hpka, in->size, in->pOp1);

  /* Start the operation */
  return PKA_Process(hpka, PKA_MODE_MONTGOMERY_PARAM, Timeout);
}

/**
  * @brief  Montgomery parameter computation in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_MontgomeryParam_IT(PKA_HandleTypeDef *hpka, PKA_MontgomeryParamInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_MontgomeryParam_Set(hpka, in->size, in->pOp1);

  /* Start the operation */
  return PKA_Process_IT(hpka, PKA_MODE_MONTGOMERY_PARAM);
}

/**
  * @brief  ECC double base ladder in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCDoubleBaseLadder(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderInTypeDef *in,
                                              uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCDoubleBaseLadder_Set(hpka, in);

  return PKA_Process(hpka, PKA_MODE_DOUBLE_BASE_LADDER, Timeout);
}

/**
  * @brief  ECC double base ladder in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCDoubleBaseLadder_IT(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCDoubleBaseLadder_Set(hpka, in);

  return PKA_Process_IT(hpka, PKA_MODE_DOUBLE_BASE_LADDER);
}

/**
  * @brief  ECC projective to affine in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCProjective2Affine(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineInTypeDef *in,
                                               uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCProjective2Affine_Set(hpka, in);

  return PKA_Process(hpka, PKA_MODE_ECC_PROJECTIVE_AFF, Timeout);
}

/**
  * @brief  ECC projective to affine in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCProjective2Affine_IT(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCProjective2Affine_Set(hpka, in);

  return PKA_Process_IT(hpka, PKA_MODE_ECC_PROJECTIVE_AFF);
}

/**
  * @brief  ECC complete addition in blocking mode.
  * @param  hpka PKA handle
  * @param  in Input information
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCCompleteAddition(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionInTypeDef *in,
                                              uint32_t Timeout)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCCompleteAddition_Set(hpka, in);

  return PKA_Process(hpka, PKA_MODE_ECC_COMPLETE_ADD, Timeout);
}

/**
  * @brief  ECC complete addition in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  in Input information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_ECCCompleteAddition_IT(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionInTypeDef *in)
{
  /* Set input parameter in PKA RAM */
  PKA_ECCCompleteAddition_Set(hpka, in);

  return PKA_Process_IT(hpka, PKA_MODE_ECC_COMPLETE_ADD);
}

/**
  * @brief  Retrieve operation result.
  * @param  hpka PKA handle
  * @param  pRes pointer to buffer where the result will be copied
  * @retval HAL status
  */
void HAL_PKA_MontgomeryParam_GetResult(PKA_HandleTypeDef *hpka, uint32_t *pRes)
{
  uint32_t size;

  /* Retrieve the size of the buffer from the PKA RAM */
  size = (hpka->Instance->RAM[PKA_MONTGOMERY_PARAM_IN_MOD_NB_BITS] + 31UL) / 32UL;

  /* Move the result to appropriate location (indicated in out parameter) */
  PKA_Memcpy_u32_to_u32(pRes, &hpka->Instance->RAM[PKA_MONTGOMERY_PARAM_OUT_PARAMETER], size);
}

/**
  * @brief  Abort any ongoing operation.
  * @param  hpka PKA handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PKA_Abort(PKA_HandleTypeDef *hpka)
{
  HAL_StatusTypeDef err = HAL_OK;

  /* Clear EN bit */
  /* This abort any operation in progress (PKA RAM content is not guaranteed in this case) */
  CLEAR_BIT(hpka->Instance->CR, PKA_CR_EN);
  SET_BIT(hpka->Instance->CR, PKA_CR_EN);

  /* Reset any pending flag */
  SET_BIT(hpka->Instance->CLRFR, PKA_CLRFR_PROCENDFC | PKA_CLRFR_RAMERRFC | PKA_CLRFR_ADDRERRFC | PKA_CLRFR_OPERRFC);

  /* Reset the error code */
  hpka->ErrorCode = HAL_PKA_ERROR_NONE;

  /* Reset the state */
  hpka->State = HAL_PKA_STATE_READY;

  return err;
}

/**
  * @brief  Reset the PKA RAM.
  * @param  hpka PKA handle
  * @retval None
  */
void HAL_PKA_RAMReset(PKA_HandleTypeDef *hpka)
{
  uint32_t index;

  /* For each element in the PKA RAM */
  for (index = 0; index < PKA_RAM_SIZE; index++)
  {
    /* Clear the content */
    hpka->Instance->RAM[index] = 0UL;
  }
}

/**
  * @brief  This function handles PKA event interrupt request.
  * @param  hpka PKA handle
  * @retval None
  */
void HAL_PKA_IRQHandler(PKA_HandleTypeDef *hpka)
{
  uint32_t mode = PKA_GetMode(hpka);
  uint32_t itsource = READ_REG(hpka->Instance->CR);
  uint32_t flag  =   READ_REG(hpka->Instance->SR);

  /* Address error interrupt occurred */
  if (((itsource & PKA_IT_ADDRERR) == PKA_IT_ADDRERR) && ((flag & PKA_FLAG_ADDRERR) == PKA_FLAG_ADDRERR))
  {
    hpka->ErrorCode |= HAL_PKA_ERROR_ADDRERR;

    /* Clear ADDRERR flag */
    __HAL_PKA_CLEAR_FLAG(hpka, PKA_FLAG_ADDRERR);
  }

  /* RAM access error interrupt occurred */
  if (((itsource & PKA_IT_RAMERR) == PKA_IT_RAMERR) && ((flag & PKA_FLAG_RAMERR) == PKA_FLAG_RAMERR))
  {
    hpka->ErrorCode |= HAL_PKA_ERROR_RAMERR;

    /* Clear RAMERR flag */
    __HAL_PKA_CLEAR_FLAG(hpka, PKA_FLAG_RAMERR);
  }

  /* OPERATION access error interrupt occurred */
  if (((itsource & PKA_IT_OPERR) == PKA_IT_OPERR) && ((flag & PKA_FLAG_OPERR) == PKA_FLAG_OPERR))
  {
    hpka->ErrorCode |= HAL_PKA_ERROR_OPERATION;

    /* Clear OPERR flag */
    __HAL_PKA_CLEAR_FLAG(hpka, PKA_FLAG_OPERR);
  }

  /* Check the operation success in case of ECDSA signature */
  switch (mode)
  {
    case PKA_MODE_ECDSA_SIGNATURE :
      /* If error output result is different from no error, operation need to be repeated */
      if (hpka->Instance->RAM[PKA_ECDSA_SIGN_OUT_ERROR] != PKA_NO_ERROR)
      {
        hpka->ErrorCode |= HAL_PKA_ERROR_OPERATION;
      }
      break;

    case PKA_MODE_DOUBLE_BASE_LADDER :
      /* If error output result is different from no error, operation need to be repeated */
      if (hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_OUT_ERROR] != PKA_NO_ERROR)
      {
        hpka->ErrorCode |= HAL_PKA_ERROR_OPERATION;
      }
      break;

    case PKA_MODE_ECC_PROJECTIVE_AFF :
      /* If error output result is different from no error, operation need to be repeated */
      if (hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_OUT_ERROR] != PKA_NO_ERROR)
      {
        hpka->ErrorCode |= HAL_PKA_ERROR_OPERATION;
      }
      break;

    case PKA_MODE_ECC_MUL :
      /* If error output result is different from no error, operation need to be repeated */
      if (hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_OUT_ERROR] != PKA_NO_ERROR)
      {
        hpka->ErrorCode |= HAL_PKA_ERROR_OPERATION;
      }
      break;

    case PKA_MODE_MODULAR_EXP_PROTECT :
      /* If error output result is different from no error, operation need to be repeated */
      if (hpka->Instance->RAM[PKA_MODULAR_EXP_OUT_ERROR] != PKA_NO_ERROR)
      {
        hpka->ErrorCode |= HAL_PKA_ERROR_OPERATION;
      }
      break;
    default :
      break;
  }
  /* Trigger the error callback if an error is present */
  if (hpka->ErrorCode != HAL_PKA_ERROR_NONE)
  {
#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
    hpka->ErrorCallback(hpka);
#else
    HAL_PKA_ErrorCallback(hpka);
#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */
  }

  /* End Of Operation interrupt occurred */
  if (((itsource & PKA_IT_PROCEND) == PKA_IT_PROCEND) && ((flag & PKA_FLAG_PROCEND) == PKA_FLAG_PROCEND))
  {
    /* Clear PROCEND flag */
    __HAL_PKA_CLEAR_FLAG(hpka, PKA_FLAG_PROCEND);

    /* Set the state to ready */
    hpka->State = HAL_PKA_STATE_READY;

#if (USE_HAL_PKA_REGISTER_CALLBACKS == 1)
    hpka->OperationCpltCallback(hpka);
#else
    HAL_PKA_OperationCpltCallback(hpka);
#endif /* USE_HAL_PKA_REGISTER_CALLBACKS */
  }
}

/**
  * @brief  Process completed callback.
  * @param  hpka PKA handle
  * @retval None
  */
__weak void HAL_PKA_OperationCpltCallback(PKA_HandleTypeDef *hpka)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpka);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PKA_OperationCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Error callback.
  * @param  hpka PKA handle
  * @retval None
  */
__weak void HAL_PKA_ErrorCallback(PKA_HandleTypeDef *hpka)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpka);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PKA_ErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup PKA_Exported_Functions_Group3 Peripheral State and Error functions
  * @brief    Peripheral State and Error functions
  *
  @verbatim
 ===============================================================================
            ##### Peripheral State and Error functions #####
 ===============================================================================
    [..]
    This subsection permit to get in run-time the status of the peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Return the PKA handle state.
  * @param  hpka PKA handle
  * @retval HAL status
  */
HAL_PKA_StateTypeDef HAL_PKA_GetState(const PKA_HandleTypeDef *hpka)
{
  /* Return PKA handle state */
  return hpka->State;
}

/**
  * @brief  Return the PKA error code.
  * @param  hpka PKA handle
  * @retval PKA error code
  */
uint32_t HAL_PKA_GetError(const PKA_HandleTypeDef *hpka)
{
  /* Return PKA handle error code */
  return hpka->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup PKA_Private_Functions
  * @{
  */

/**
  * @brief  Get PKA operating mode.
  * @param  hpka PKA handle
  * @retval Return the current mode
  */
uint32_t PKA_GetMode(const PKA_HandleTypeDef *hpka)
{
  /* return the shifted PKA_CR_MODE value */
  return (uint32_t)(READ_BIT(hpka->Instance->CR, PKA_CR_MODE) >> PKA_CR_MODE_Pos);
}

/**
  * @brief  Wait for operation completion or timeout.
  * @param  hpka PKA handle
  * @param  Timeout Timeout duration in millisecond.
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
HAL_StatusTypeDef PKA_PollEndOfOperation(const PKA_HandleTypeDef *hpka, uint32_t Timeout, uint32_t Tickstart)
{
  /* Wait for the end of operation or timeout */
  while ((hpka->Instance->SR & PKA_SR_PROCENDF) == 0UL)
  {
    /* Check if timeout is disabled (set to infinite wait) */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0UL))
      {
        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Return a hal error code based on PKA error flags.
  * @param  hpka PKA handle
  * @param  mode PKA operating mode
  * @retval error code
  */
uint32_t PKA_CheckError(const PKA_HandleTypeDef *hpka, uint32_t mode)
{
  uint32_t err = HAL_PKA_ERROR_NONE;

  /* Check RAMERR error */
  if (__HAL_PKA_GET_FLAG(hpka, PKA_FLAG_RAMERR) == SET)
  {
    err |= HAL_PKA_ERROR_RAMERR;
  }

  /* Check ADDRERR error */
  if (__HAL_PKA_GET_FLAG(hpka, PKA_FLAG_ADDRERR) == SET)
  {
    err |= HAL_PKA_ERROR_ADDRERR;
  }

  /* Check OPEERR error */
  if (__HAL_PKA_GET_FLAG(hpka, PKA_FLAG_OPERR) == SET)
  {
    err |= HAL_PKA_ERROR_OPERATION;
  }

  /* Check the operation success in case of ECDSA signature */
  if (mode == PKA_MODE_ECDSA_SIGNATURE)
  {
#define EDCSA_SIGN_NOERROR PKA_NO_ERROR
    /* If error output result is different from no error, ecsa sign operation need to be repeated */
    if (hpka->Instance->RAM[PKA_ECDSA_SIGN_OUT_ERROR] != EDCSA_SIGN_NOERROR)
    {
      err |= HAL_PKA_ERROR_OPERATION;
    }
  }

  /* Check the operation success in case of ECC double base ladder*/
  if (mode == PKA_MODE_DOUBLE_BASE_LADDER)
  {
    /* If error output result is different from no error, PKA operation need to be repeated */
    if (hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_OUT_ERROR] != PKA_NO_ERROR)
    {
      err |= HAL_PKA_ERROR_OPERATION;
    }
  }

  /* Check the operation success in case of ECC projective to affine*/
  if (mode == PKA_MODE_ECC_PROJECTIVE_AFF)
  {
    /* If error output result is different from no error, PKA operation need to be repeated */
    if (hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_OUT_ERROR] != PKA_NO_ERROR)
    {
      err |= HAL_PKA_ERROR_OPERATION;
    }
  }

  /* Check the operation success in case of ECC Fp scalar multiplication*/
  if (mode == PKA_MODE_ECC_MUL)
  {
    /* If error output result is different from no error, PKA operation need to be repeated */
    if (hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_OUT_ERROR] != PKA_NO_ERROR)
    {
      err |= HAL_PKA_ERROR_OPERATION;
    }
  }

  /* Check the operation success in case of protected modular exponentiation*/
  if (mode == PKA_MODE_MODULAR_EXP_PROTECT)
  {
    /* If error output result is different from no error, PKA operation need to be repeated */
    if (hpka->Instance->RAM[PKA_MODULAR_EXP_OUT_ERROR] != PKA_NO_ERROR)
    {
      err |= HAL_PKA_ERROR_OPERATION;
    }
  }

  return err;
}

/**
  * @brief  Get number of bits inside an array of u8.
  * @param  byteNumber Number of u8 inside the array
  */
uint32_t PKA_GetBitSize_u8(uint32_t byteNumber)
{
  /* Convert from number of uint8_t in an array to the associated number of bits in this array */
  return byteNumber * 8UL;
}

/**
  * @brief  Get optimal number of bits inside an array of u8.
  * @param  byteNumber Number of u8 inside the array
  * @param  msb Most significant uint8_t of the array
  */
uint32_t PKA_GetOptBitSize_u8(uint32_t byteNumber, uint8_t msb)
{
  uint32_t position;

  position = 32UL - __CLZ(msb);

  return (((byteNumber - 1UL) * 8UL) + position);
}

/**
  * @brief  Get number of bits inside an array of u32.
  * @param  wordNumber Number of u32 inside the array
  */
uint32_t PKA_GetBitSize_u32(uint32_t wordNumber)
{
  /* Convert from number of uint32_t in an array to the associated number of bits in this array */
  return wordNumber * 32UL;
}

/**
  * @brief  Get number of uint8_t element in an array of bitSize bits.
  * @param  bitSize Number of bits in an array
  */
uint32_t PKA_GetArraySize_u8(uint32_t bitSize)
{
  /* Manage the non aligned on uint8_t bitsize: */
  /*   512 bits requires 64 uint8_t             */
  /*   521 bits requires 66 uint8_t             */
  return ((bitSize + 7UL) / 8UL);
}

/**
  * @brief  Copy uint32_t array to uint8_t array to fit PKA number representation.
  * @param  dst Pointer to destination
  * @param  src Pointer to source
  * @param  n Number of uint8_t to copy
  * @retval dst
  */
void PKA_Memcpy_u32_to_u8(uint8_t dst[], __IO const uint32_t src[], size_t n)
{
  if (dst != NULL)
  {
    if (src != NULL)
    {
      uint32_t index_uint32_t = 0UL; /* This index is used outside of the loop */

      for (; index_uint32_t < (n / 4UL); index_uint32_t++)
      {
        /* Avoid casting from uint8_t* to uint32_t* by copying 4 uint8_t in a row */
        /* Apply __REV equivalent */
        uint32_t index_uint8_t = n - 4UL - (index_uint32_t * 4UL);
        dst[index_uint8_t + 3UL] = (uint8_t)((src[index_uint32_t] & 0x000000FFU));
        dst[index_uint8_t + 2UL] = (uint8_t)((src[index_uint32_t] & 0x0000FF00U) >> 8UL);
        dst[index_uint8_t + 1UL] = (uint8_t)((src[index_uint32_t] & 0x00FF0000U) >> 16UL);
        dst[index_uint8_t + 0UL] = (uint8_t)((src[index_uint32_t] & 0xFF000000U) >> 24UL);
      }

      /* Manage the buffers not aligned on uint32_t */
      if ((n % 4UL) == 1UL)
      {
        dst[0UL] = (uint8_t)((src[index_uint32_t] & 0x000000FFU));
      }
      else if ((n % 4UL) == 2UL)
      {
        dst[1UL] = (uint8_t)((src[index_uint32_t] & 0x000000FFU));
        dst[0UL] = (uint8_t)((src[index_uint32_t] & 0x0000FF00U) >> 8UL);
      }
      else if ((n % 4UL) == 3UL)
      {
        dst[2UL] = (uint8_t)((src[index_uint32_t] & 0x000000FFU));
        dst[1UL] = (uint8_t)((src[index_uint32_t] & 0x0000FF00U) >> 8UL);
        dst[0UL] = (uint8_t)((src[index_uint32_t] & 0x00FF0000U) >> 16UL);
      }
      else
      {
        /* The last element is already handle in the loop */
      }
    }
  }
}

/**
  * @brief  Copy uint8_t array to uint32_t array to fit PKA number representation.
  * @param  dst Pointer to destination
  * @param  src Pointer to source
  * @param  n Number of uint8_t to copy (must be multiple of 4)
  * @retval dst
  */
void PKA_Memcpy_u8_to_u32(__IO uint32_t dst[], const uint8_t src[], size_t n)
{
  if (dst != NULL)
  {
    if (src != NULL)
    {
      uint32_t index = 0UL; /* This index is used outside of the loop */

      for (; index < (n / 4UL); index++)
      {
        /* Apply the equivalent of __REV from uint8_t to uint32_t */
        dst[index] = ((uint32_t)src[(n - (index * 4UL) - 1UL)]) \
                     | ((uint32_t)src[(n - (index * 4UL) - 2UL)] << 8UL) \
                     | ((uint32_t)src[(n - (index * 4UL) - 3UL)] << 16UL) \
                     | ((uint32_t)src[(n - (index * 4UL) - 4UL)] << 24UL);
      }

      /* Manage the buffers not aligned on uint32_t */
      if ((n % 4UL) == 1UL)
      {
        dst[index] = (uint32_t)src[(n - (index * 4UL) - 1UL)];
      }
      else if ((n % 4UL) == 2UL)
      {
        dst[index] = ((uint32_t)src[(n - (index * 4UL) - 1UL)]) \
                     | ((uint32_t)src[(n - (index * 4UL) - 2UL)] << 8UL);
      }
      else if ((n % 4UL) == 3UL)
      {
        dst[index] = ((uint32_t)src[(n - (index * 4UL) - 1UL)]) \
                     | ((uint32_t)src[(n - (index * 4UL) - 2UL)] << 8UL) \
                     | ((uint32_t)src[(n - (index * 4UL) - 3UL)] << 16UL);
      }
      else
      {
        /* The last element is already handle in the loop */
      }
    }
  }
}

/**
  * @brief  Copy uint32_t array to uint32_t array.
  * @param  dst Pointer to destination
  * @param  src Pointer to source
  * @param  n Number of u32 to be handled
  * @retval dst
  */
void PKA_Memcpy_u32_to_u32(__IO uint32_t dst[], __IO const uint32_t src[], size_t n)
{
  /* If a destination buffer is provided */
  if (dst != NULL)
  {
    /* If a source buffer is provided */
    if (src != NULL)
    {
      /* For each element in the array */
      for (uint32_t index = 0UL; index < n; index++)
      {
        /* Copy the content */
        dst[index] = src[index];
      }
    }
  }
}

/**
  * @brief  Generic function to start a PKA operation in blocking mode.
  * @param  hpka PKA handle
  * @param  mode PKA operation
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef PKA_Process(PKA_HandleTypeDef *hpka, uint32_t mode, uint32_t Timeout)
{
  HAL_StatusTypeDef err = HAL_OK;
  uint32_t tickstart;

  if (hpka->State == HAL_PKA_STATE_READY)
  {
    /* Set the state to busy */
    hpka->State = HAL_PKA_STATE_BUSY;

    /* Clear any pending error */
    hpka->ErrorCode = HAL_PKA_ERROR_NONE;

    /* Init tickstart for timeout management*/
    tickstart = HAL_GetTick();

    /* Set the mode and deactivate the interrupts */
    MODIFY_REG(hpka->Instance->CR, PKA_CR_MODE | PKA_CR_PROCENDIE | PKA_CR_RAMERRIE | PKA_CR_ADDRERRIE | PKA_CR_OPERRIE,
               mode << PKA_CR_MODE_Pos);

    /* Start the computation */
    hpka->Instance->CR |= PKA_CR_START;

    /* Wait for the end of operation or timeout */
    if (PKA_PollEndOfOperation(hpka, Timeout, tickstart) != HAL_OK)
    {
      /* Abort any ongoing operation */
      CLEAR_BIT(hpka->Instance->CR, PKA_CR_EN);

      hpka->ErrorCode |= HAL_PKA_ERROR_TIMEOUT;

      /* Make ready for the next operation */
      SET_BIT(hpka->Instance->CR, PKA_CR_EN);
    }

    /* Check error */
    hpka->ErrorCode |= PKA_CheckError(hpka, mode);

    /* Clear all flags */
    hpka->Instance->CLRFR |= (PKA_CLRFR_PROCENDFC | PKA_CLRFR_RAMERRFC | PKA_CLRFR_ADDRERRFC | PKA_CLRFR_OPERRFC);

    /* Set the state to ready */
    hpka->State = HAL_PKA_STATE_READY;

    /* Manage the result based on encountered errors */
    if (hpka->ErrorCode != HAL_PKA_ERROR_NONE)
    {
      err = HAL_ERROR;
    }
  }
  else
  {
    err = HAL_ERROR;
  }
  return err;
}

/**
  * @brief  Generic function to start a PKA operation in non-blocking mode with Interrupt.
  * @param  hpka PKA handle
  * @param  mode PKA operation
  * @retval HAL status
  */
HAL_StatusTypeDef PKA_Process_IT(PKA_HandleTypeDef *hpka, uint32_t mode)
{
  HAL_StatusTypeDef err = HAL_OK;

  if (hpka->State == HAL_PKA_STATE_READY)
  {
    /* Set the state to busy */
    hpka->State = HAL_PKA_STATE_BUSY;

    /* Clear any pending error */
    hpka->ErrorCode = HAL_PKA_ERROR_NONE;

    /* Set the mode and activate interrupts */
    MODIFY_REG(hpka->Instance->CR, PKA_CR_MODE | PKA_CR_PROCENDIE | PKA_CR_RAMERRIE | PKA_CR_ADDRERRIE | PKA_CR_OPERRIE,
               (mode << PKA_CR_MODE_Pos) | PKA_CR_PROCENDIE | PKA_CR_RAMERRIE | PKA_CR_ADDRERRIE | PKA_CR_OPERRIE);

    /* Start the computation */
    hpka->Instance->CR |= PKA_CR_START;
  }
  else
  {
    err = HAL_ERROR;
  }
  return err;
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ModExp_Set(PKA_HandleTypeDef *hpka, PKA_ModExpInTypeDef *in)
{
  /* Get the number of bit per operand */
  hpka->Instance->RAM[PKA_MODULAR_EXP_IN_OP_NB_BITS] = PKA_GetBitSize_u8(in->OpSize);

  /* Get the number of bit of the exponent */
  hpka->Instance->RAM[PKA_MODULAR_EXP_IN_EXP_NB_BITS] = PKA_GetBitSize_u8(in->expSize);

  /* Move the input parameters pOp1 to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE], in->pOp1, in->OpSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_IN_EXPONENT_BASE + ((in->OpSize + 3UL) / 4UL));

  /* Move the exponent to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_IN_EXPONENT], in->pExp, in->expSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_IN_EXPONENT + ((in->expSize + 3UL) / 4UL));

  /* Move the modulus to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_IN_MODULUS], in->pMod, in->OpSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_IN_MODULUS + ((in->OpSize + 3UL) / 4UL));
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ModExpFastMode_Set(PKA_HandleTypeDef *hpka, PKA_ModExpFastModeInTypeDef *in)
{
  /* Get the number of bit per operand */
  hpka->Instance->RAM[PKA_MODULAR_EXP_IN_OP_NB_BITS] = PKA_GetBitSize_u8(in->OpSize);

  /* Get the number of bit of the exponent */
  hpka->Instance->RAM[PKA_MODULAR_EXP_IN_EXP_NB_BITS] = PKA_GetBitSize_u8(in->expSize);

  /* Move the input parameters pOp1 to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_IN_EXPONENT_BASE], in->pOp1, in->OpSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_IN_EXPONENT_BASE + (in->OpSize / 4UL));

  /* Move the exponent to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_IN_EXPONENT], in->pExp, in->expSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_IN_EXPONENT + (in->expSize / 4UL));

  /* Move the modulus to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_IN_MODULUS], in->pMod, in->OpSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_IN_MODULUS + (in->OpSize / 4UL));

  /* Move the Montgomery parameter to PKA RAM */
  PKA_Memcpy_u32_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_IN_MONTGOMERY_PARAM], in->pMontgomeryParam,
                        in->OpSize / 4UL);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_IN_MONTGOMERY_PARAM + (in->OpSize / 4UL));
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ModExpProtectMode_Set(PKA_HandleTypeDef *hpka, PKA_ModExpProtectModeInTypeDef *in)
{
  /* Get the number of bit per operand */
  hpka->Instance->RAM[PKA_MODULAR_EXP_IN_OP_NB_BITS] = PKA_GetBitSize_u8(in->OpSize);

  /* Get the number of bit of the exponent */
  hpka->Instance->RAM[PKA_MODULAR_EXP_IN_EXP_NB_BITS] = PKA_GetBitSize_u8(in->expSize);

  /* Move the input parameters pOp1 to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_PROTECT_IN_EXPONENT_BASE], in->pOp1, in->OpSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_PROTECT_IN_EXPONENT_BASE + (in->OpSize / 4UL));

  /* Move the exponent to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_PROTECT_IN_EXPONENT], in->pExp, in->expSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_PROTECT_IN_EXPONENT + (in->expSize / 4UL));

  /* Move the modulus to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_PROTECT_IN_MODULUS], in->pMod, in->OpSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_PROTECT_IN_MODULUS + (in->OpSize / 4UL));

  /* Move Phi value to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_EXP_PROTECT_IN_PHI], in->pPhi, in->OpSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_EXP_PROTECT_IN_PHI + (in->OpSize / 4UL));
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  * @note   If the modulus size is bigger than the hash size (with a curve SECP521R1 when using a SHA256 hash
  *         for example)the hash value should be written at the end of the buffer with zeros padding at beginning.
  */
void PKA_ECDSASign_Set(PKA_HandleTypeDef *hpka, PKA_ECDSASignInTypeDef *in)
{
  /* Get the prime order n length */
  hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_ORDER_NB_BITS] = PKA_GetOptBitSize_u8(in->primeOrderSize, *(in->primeOrder));

  /* Get the modulus p length */
  hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_MOD_NB_BITS] = PKA_GetOptBitSize_u8(in->modulusSize, *(in->modulus));

  /* Get the coefficient a sign */
  hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_A_COEFF_SIGN] = in->coefSign;

  /* Move the input parameters coefficient |a| to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_A_COEFF], in->coef, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_SIGN_IN_A_COEFF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters coefficient B to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_B_COEFF], in->coefB, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_SIGN_IN_B_COEFF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters modulus value p to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_MOD_GF], in->modulus, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_SIGN_IN_MOD_GF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters integer k to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_K], in->integer, in->primeOrderSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_SIGN_IN_K + ((in->primeOrderSize + 3UL) / 4UL));

  /* Move the input parameters base point G coordinate x to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_INITIAL_POINT_X], in->basePointX, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_SIGN_IN_INITIAL_POINT_X + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters base point G coordinate y to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_INITIAL_POINT_Y], in->basePointY, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_SIGN_IN_INITIAL_POINT_Y + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters hash of message z to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_HASH_E], in->hash, in->primeOrderSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_SIGN_IN_HASH_E + ((in->primeOrderSize + 3UL) / 4UL));

  /* Move the input parameters private key d to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_PRIVATE_KEY_D], in->privateKey, in->primeOrderSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_SIGN_IN_PRIVATE_KEY_D + ((in->primeOrderSize + 3UL) / 4UL));

  /* Move the input parameters prime order n to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_SIGN_IN_ORDER_N], in->primeOrder, in->primeOrderSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_SIGN_IN_ORDER_N + ((in->primeOrderSize + 3UL) / 4UL));
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ECDSAVerif_Set(PKA_HandleTypeDef *hpka, PKA_ECDSAVerifInTypeDef *in)
{
  /* Get the prime order n length */
  hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_ORDER_NB_BITS] = PKA_GetOptBitSize_u8(in->primeOrderSize, *(in->primeOrder));

  /* Get the modulus p length */
  hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_MOD_NB_BITS] = PKA_GetOptBitSize_u8(in->modulusSize, *(in->modulus));

  /* Get the coefficient a sign */
  hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_A_COEFF_SIGN] = in->coefSign;

  /* Move the input parameters coefficient |a| to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_A_COEFF], in->coef, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_A_COEFF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters modulus value p to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_MOD_GF], in->modulus, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_MOD_GF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters base point G coordinate x to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_INITIAL_POINT_X], in->basePointX, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_INITIAL_POINT_X + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters base point G coordinate y to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_INITIAL_POINT_Y], in->basePointY, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_INITIAL_POINT_Y + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters public-key curve point Q coordinate xQ to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_PUBLIC_KEY_POINT_X], in->pPubKeyCurvePtX,
                       in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_PUBLIC_KEY_POINT_X + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters public-key curve point Q coordinate xQ to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_PUBLIC_KEY_POINT_Y], in->pPubKeyCurvePtY,
                       in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_PUBLIC_KEY_POINT_Y + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters signature part r to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_SIGNATURE_R], in->RSign, in->primeOrderSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_SIGNATURE_R + ((in->primeOrderSize + 3UL) / 4UL));

  /* Move the input parameters signature part s to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_SIGNATURE_S], in->SSign, in->primeOrderSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_SIGNATURE_S + ((in->primeOrderSize + 3UL) / 4UL));

  /* Move the input parameters hash of message z to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_HASH_E], in->hash, in->primeOrderSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_HASH_E + ((in->primeOrderSize + 3UL) / 4UL));

  /* Move the input parameters curve prime order n to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECDSA_VERIF_IN_ORDER_N], in->primeOrder, in->primeOrderSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECDSA_VERIF_IN_ORDER_N + ((in->primeOrderSize + 3UL) / 4UL));
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_RSACRTExp_Set(PKA_HandleTypeDef *hpka, PKA_RSACRTExpInTypeDef *in)
{
  /* Get the operand length M */
  hpka->Instance->RAM[PKA_RSA_CRT_EXP_IN_MOD_NB_BITS] = PKA_GetBitSize_u8(in->size);

  /* Move the input parameters operand dP to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_RSA_CRT_EXP_IN_DP_CRT], in->pOpDp, in->size / 2UL);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_RSA_CRT_EXP_IN_DP_CRT + (in->size / 8UL));

  /* Move the input parameters operand dQ to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_RSA_CRT_EXP_IN_DQ_CRT], in->pOpDq, in->size / 2UL);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_RSA_CRT_EXP_IN_DQ_CRT + (in->size / 8UL));

  /* Move the input parameters operand qinv to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_RSA_CRT_EXP_IN_QINV_CRT], in->pOpQinv, in->size / 2UL);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_RSA_CRT_EXP_IN_QINV_CRT + (in->size / 8UL));

  /* Move the input parameters prime p to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_RSA_CRT_EXP_IN_PRIME_P], in->pPrimeP, in->size / 2UL);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_RSA_CRT_EXP_IN_PRIME_P + (in->size / 8UL));

  /* Move the input parameters prime q to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_RSA_CRT_EXP_IN_PRIME_Q], in->pPrimeQ, in->size / 2UL);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_RSA_CRT_EXP_IN_PRIME_Q + (in->size / 8UL));

  /* Move the input parameters operand A to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_RSA_CRT_EXP_IN_EXPONENT_BASE], in->popA, in->size);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_RSA_CRT_EXP_IN_EXPONENT_BASE + (in->size / 4UL));
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_PointCheck_Set(PKA_HandleTypeDef *hpka, PKA_PointCheckInTypeDef *in)
{
  /* Get the modulus length */
  hpka->Instance->RAM[PKA_POINT_CHECK_IN_MOD_NB_BITS] = PKA_GetOptBitSize_u8(in->modulusSize, *(in->modulus));

  /* Get the coefficient a sign */
  hpka->Instance->RAM[PKA_POINT_CHECK_IN_A_COEFF_SIGN] = in->coefSign;

  /* Move the input parameters coefficient |a| to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_POINT_CHECK_IN_A_COEFF], in->coefA, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_POINT_CHECK_IN_A_COEFF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters coefficient b to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_POINT_CHECK_IN_B_COEFF], in->coefB, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_POINT_CHECK_IN_B_COEFF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters modulus value p to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_POINT_CHECK_IN_MOD_GF], in->modulus, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_POINT_CHECK_IN_MOD_GF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters Point P coordinate x to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_POINT_CHECK_IN_INITIAL_POINT_X], in->pointX, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_POINT_CHECK_IN_INITIAL_POINT_X + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters Point P coordinate y to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_POINT_CHECK_IN_INITIAL_POINT_Y], in->pointY, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_POINT_CHECK_IN_INITIAL_POINT_Y + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters montgomery param R2 modulus N to PKA RAM */
  PKA_Memcpy_u32_to_u32(&hpka->Instance->RAM[PKA_POINT_CHECK_IN_MONTGOMERY_PARAM], in->pMontgomeryParam,
                        (in->modulusSize / 4UL));
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_POINT_CHECK_IN_MONTGOMERY_PARAM + ((in->modulusSize + 3UL) / 4UL));
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ECCMul_Set(PKA_HandleTypeDef *hpka, PKA_ECCMulInTypeDef *in)
{
  /* Get the prime order n length */
  hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_EXP_NB_BITS] = PKA_GetOptBitSize_u8(in->scalarMulSize, *(in->primeOrder));

  /* Get the modulus length */
  hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_OP_NB_BITS] = PKA_GetOptBitSize_u8(in->modulusSize, *(in->modulus));

  /* Get the coefficient a sign */
  hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_A_COEFF_SIGN] = in->coefSign;

  /* Move the input parameters coefficient |a| to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_A_COEFF], in->coefA, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_A_COEFF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters coefficient b to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_B_COEFF], in->coefB, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_B_COEFF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters modulus value p to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_MOD_GF], in->modulus, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_MOD_GF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters scalar multiplier k to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_K], in->scalarMul, in->scalarMulSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_K + ((in->scalarMulSize + 3UL) / 4UL));

  /* Move the input parameters Point P coordinate x to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_INITIAL_POINT_X], in->pointX, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_INITIAL_POINT_X + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters Point P coordinate y to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_INITIAL_POINT_Y], in->pointY, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_INITIAL_POINT_Y + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters curve prime order N to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_N_PRIME_ORDER], in->primeOrder, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_N_PRIME_ORDER + ((in->modulusSize + 3UL) / 4UL));
}
/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ECCMulEx_Set(PKA_HandleTypeDef *hpka, PKA_ECCMulExInTypeDef *in)
{
  /* Get the prime order n length */
  hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_EXP_NB_BITS] = PKA_GetOptBitSize_u8(in->primeOrderSize, *(in->primeOrder));

  /* Get the modulus length */
  hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_OP_NB_BITS] = PKA_GetOptBitSize_u8(in->modulusSize, *(in->modulus));

  /* Get the coefficient a sign */
  hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_A_COEFF_SIGN] = in->coefSign;

  /* Move the input parameters coefficient |a| to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_A_COEFF], in->coefA, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_A_COEFF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters coefficient b to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_B_COEFF], in->coefB, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_B_COEFF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters modulus value p to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_MOD_GF], in->modulus, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_MOD_GF + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters scalar multiplier k to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_K], in->scalarMul, in->scalarMulSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_K + ((in->scalarMulSize + 3UL) / 4UL));

  /* Move the input parameters Point P coordinate x to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_INITIAL_POINT_X], in->pointX, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_INITIAL_POINT_X + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters Point P coordinate y to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_INITIAL_POINT_Y], in->pointY, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_INITIAL_POINT_Y + ((in->modulusSize + 3UL) / 4UL));

  /* Move the input parameters curve prime order N to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_SCALAR_MUL_IN_N_PRIME_ORDER], in->primeOrder, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_SCALAR_MUL_IN_N_PRIME_ORDER + ((in->modulusSize + 3UL) / 4UL));
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ModInv_Set(PKA_HandleTypeDef *hpka, PKA_ModInvInTypeDef *in)
{
  /* Get the number of bit per operand */
  hpka->Instance->RAM[PKA_MODULAR_INV_NB_BITS] = PKA_GetBitSize_u32(in->size);

  /* Move the input parameters operand A to PKA RAM */
  PKA_Memcpy_u32_to_u32(&hpka->Instance->RAM[PKA_MODULAR_INV_IN_OP1], in->pOp1, in->size);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_INV_IN_OP1 + in->size);

  /* Move the input parameters modulus value n to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_INV_IN_OP2_MOD], in->pMod, in->size * 4UL);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_INV_IN_OP2_MOD + in->size);
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ModRed_Set(PKA_HandleTypeDef *hpka, PKA_ModRedInTypeDef *in)
{
  /* Get the number of bit per operand */
  hpka->Instance->RAM[PKA_MODULAR_REDUC_IN_OP_LENGTH] = PKA_GetBitSize_u32(in->OpSize);

  /* Get the number of bit per modulus */
  hpka->Instance->RAM[PKA_MODULAR_REDUC_IN_MOD_LENGTH] = PKA_GetBitSize_u8(in->modSize);

  /* Move the input parameters operand A to PKA RAM */
  PKA_Memcpy_u32_to_u32(&hpka->Instance->RAM[PKA_MODULAR_REDUC_IN_OPERAND], in->pOp1, in->OpSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_REDUC_IN_OPERAND + in->OpSize);

  /* Move the input parameters modulus value n to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MODULAR_REDUC_IN_MODULUS], in->pMod, in->modSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MODULAR_REDUC_IN_MODULUS + ((in->modSize + 3UL) / 4UL));
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  size Size of the operand
  * @param  pOp1 Generic pointer to input data
  */
void PKA_MontgomeryParam_Set(PKA_HandleTypeDef *hpka, const uint32_t size, const uint8_t *pOp1)
{
  uint32_t bytetoskip = 0UL;
  uint32_t newSize;

  if (pOp1 != NULL)
  {
    /* Count the number of zero bytes */
    while ((bytetoskip < size) && (pOp1[bytetoskip] == 0UL))
    {
      bytetoskip++;
    }

    /* Get new size after skipping zero bytes */
    newSize = size - bytetoskip;

    /* Get the number of bit per operand */
    hpka->Instance->RAM[PKA_MONTGOMERY_PARAM_IN_MOD_NB_BITS] = PKA_GetOptBitSize_u8(newSize, pOp1[bytetoskip]);

    /* Move the input parameters pOp1 to PKA RAM */
    PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_MONTGOMERY_PARAM_IN_MODULUS], pOp1, size);
    __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_MONTGOMERY_PARAM_IN_MODULUS + ((size + 3UL) / 4UL));
  }
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ECCDoubleBaseLadder_Set(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderInTypeDef *in)
{
  /* Get the prime order n length */
  hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_PRIME_ORDER_NB_BITS] = PKA_GetBitSize_u8(in->primeOrderSize);

  /* Get the modulus p length */
  hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_MOD_NB_BITS] = PKA_GetBitSize_u8(in->modulusSize);

  /* Get the coefficient a sign */
  hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_A_COEFF_SIGN] = in->coefSign;

  /* Move the input parameters coefficient |a| to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_A_COEFF], in->coefA, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_A_COEFF + (in->modulusSize / 4UL));

  /* Move the input parameters modulus value p to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_MOD_P], in->modulus, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_MOD_P + (in->modulusSize / 4UL));

  /* Move the input parameters integer k to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_K_INTEGER], in->integerK, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_K_INTEGER + (in->modulusSize / 4UL));

  /* Move the input parameters integer m to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_M_INTEGER], in->integerM, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_M_INTEGER + (in->modulusSize / 4UL));

  /* Move the input parameters first point coordinate x to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_POINT1_X], in->basePointX1, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_POINT1_X + (in->modulusSize / 4UL));

  /* Move the input parameters first point  coordinate y to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_POINT1_Y], in->basePointY1, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_POINT1_Y + (in->modulusSize / 4UL));

  /* Move the input parameters first point  coordinate z to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_POINT1_Z], in->basePointZ1, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_POINT1_Z + (in->modulusSize / 4UL));

  /* Move the input parameters second point coordinate x to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_POINT2_X], in->basePointX2, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_POINT2_X + (in->modulusSize / 4UL));

  /* Move the input parameters second point  coordinate y to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_POINT2_Y], in->basePointY2, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_POINT2_Y + (in->modulusSize / 4UL));

  /* Move the input parameters second point  coordinate z to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_POINT2_Z], in->basePointZ2, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_DOUBLE_LADDER_IN_POINT2_Z + (in->modulusSize / 4UL));
}

/**
  * @brief  Retrieve operation result.
  * @param  hpka PKA handle
  * @param  out Output information
  * @retval HAL status
  */
void HAL_PKA_ECCDoubleBaseLadder_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderOutTypeDef *out)
{
  uint32_t size;

  /* Move the result to appropriate location (indicated in out parameter) */
  size = hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_MOD_NB_BITS] / 8UL;
  if (out != NULL)
  {
    PKA_Memcpy_u32_to_u8(out->ptX, &hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_OUT_RESULT_X], size);
    PKA_Memcpy_u32_to_u8(out->ptY, &hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_OUT_RESULT_Y], size);
  }
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ECCProjective2Affine_Set(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineInTypeDef *in)
{
  /* Get the modulus p length */
  hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_IN_MOD_NB_BITS] = PKA_GetBitSize_u8(in->modulusSize);

  /* Move the input parameters modulus value p to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_IN_MOD_P], in->modulus, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_PROJECTIVE_AFF_IN_MOD_P + (in->modulusSize / 4UL));

  /* Move the input parameters point coordinate x to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_IN_POINT_X], in->basePointX, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_PROJECTIVE_AFF_IN_POINT_X + (in->modulusSize / 4UL));

  /* Move the input parameters point coordinate y to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_IN_POINT_Y], in->basePointY, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_PROJECTIVE_AFF_IN_POINT_Y + (in->modulusSize / 4UL));

  /* Move the input parameters point coordinate z to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_IN_POINT_Z], in->basePointZ, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_PROJECTIVE_AFF_IN_POINT_Z + (in->modulusSize / 4UL));

  /* Move the input parameters montgomery parameter R2 modulus n to PKA RAM */
  PKA_Memcpy_u32_to_u32(&hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_IN_MONTGOMERY_PARAM_R2], in->pMontgomeryParam,
                        (in->modulusSize / 4UL));
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_PROJECTIVE_AFF_IN_MONTGOMERY_PARAM_R2 + (in->modulusSize / 4UL));
}

/**
  * @brief  Retrieve operation result.
  * @param  hpka PKA handle
  * @param  out Output information
  * @retval HAL status
  */
void HAL_PKA_ECCProjective2Affine_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineOutTypeDef *out)
{
  uint32_t size;

  /* Move the result to appropriate location (indicated in out parameter) */
  size = hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_IN_MOD_NB_BITS] / 8UL;
  if (out != NULL)
  {
    PKA_Memcpy_u32_to_u8(out->ptX, &hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_OUT_RESULT_X], size);
    PKA_Memcpy_u32_to_u8(out->ptY, &hpka->Instance->RAM[PKA_ECC_PROJECTIVE_AFF_OUT_RESULT_Y], size);
  }
}

/**
  * @brief  Set input parameters.
  * @param  hpka PKA handle
  * @param  in Input information
  */
void PKA_ECCCompleteAddition_Set(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionInTypeDef *in)
{
  /* Get the modulus p length */
  hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_MOD_NB_BITS] = PKA_GetBitSize_u8(in->modulusSize);

  /* Get the coefficient a sign */
  hpka->Instance->RAM[PKA_ECC_DOUBLE_LADDER_IN_A_COEFF_SIGN] = in->coefSign;

  /* Move the input parameters modulus value p to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_MOD_P], in->modulus, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_COMPLETE_ADD_IN_MOD_P + (in->modulusSize / 4UL));

  /* Move the input parameters coefA value to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_A_COEFF], in->coefA, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_COMPLETE_ADD_IN_A_COEFF + (in->modulusSize / 4UL));

  /* Move the input parameters first point x value  to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_POINT1_X], in->basePointX1, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_COMPLETE_ADD_IN_POINT1_X + (in->modulusSize / 4UL));

  /* Move the input parameters first point y value  to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_POINT1_Y], in->basePointY1, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_COMPLETE_ADD_IN_POINT1_Y + (in->modulusSize / 4UL));

  /* Move the input parameters first point z value  to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_POINT1_Z], in->basePointZ1, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_COMPLETE_ADD_IN_POINT1_Z + (in->modulusSize / 4UL));

  /* Move the input parameters second point x value  to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_POINT2_X], in->basePointX2, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_COMPLETE_ADD_IN_POINT2_X + (in->modulusSize / 4UL));

  /* Move the input parameters second point y value  to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_POINT2_Y], in->basePointY2, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_COMPLETE_ADD_IN_POINT2_Y + (in->modulusSize / 4UL));

  /* Move the input parameters second point z value  to PKA RAM */
  PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_POINT2_Z], in->basePointZ2, in->modulusSize);
  __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ECC_COMPLETE_ADD_IN_POINT2_Z + (in->modulusSize / 4UL));
}

/**
  * @brief  Retrieve operation result.
  * @param  hpka PKA handle
  * @param  out Output information
  * @retval HAL status
  */
void HAL_PKA_ECCCompleteAddition_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionOutTypeDef *out)
{
  uint32_t size;

  /* Move the result to appropriate location (indicated in out parameter) */
  size = (hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_IN_MOD_NB_BITS] + 7UL) / 8UL;
  if (out != NULL)
  {
    PKA_Memcpy_u32_to_u8(out->ptX, &hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_OUT_RESULT_X], size);
    PKA_Memcpy_u32_to_u8(out->ptY, &hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_OUT_RESULT_Y], size);
    PKA_Memcpy_u32_to_u8(out->ptZ, &hpka->Instance->RAM[PKA_ECC_COMPLETE_ADD_OUT_RESULT_Z], size);
  }
}
/**
  * @brief  Generic function to set input parameters.
  * @param  hpka PKA handle
  * @param  size Size of the operand
  * @param  pOp1 Generic pointer to input data
  * @param  pOp2 Generic pointer to input data
  * @param  pOp3 Generic pointer to input data
  */
void PKA_ARI_Set(PKA_HandleTypeDef *hpka, const uint32_t size, const uint32_t *pOp1, const uint32_t *pOp2,
                 const uint8_t *pOp3)
{
  /* Get the number of bit per operand */
  hpka->Instance->RAM[PKA_ARITHMETIC_ALL_OPS_NB_BITS] = PKA_GetBitSize_u32(size);

  if (pOp1 != NULL)
  {
    /* Move the input parameters pOp1 to PKA RAM */
    PKA_Memcpy_u32_to_u32(&hpka->Instance->RAM[PKA_ARITHMETIC_ALL_OPS_IN_OP1], pOp1, size);
    __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ARITHMETIC_ALL_OPS_IN_OP1 + size);
  }

  if (pOp2 != NULL)
  {
    /* Move the input parameters pOp2 to PKA RAM */
    PKA_Memcpy_u32_to_u32(&hpka->Instance->RAM[PKA_ARITHMETIC_ALL_OPS_IN_OP2], pOp2, size);
    __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ARITHMETIC_ALL_OPS_IN_OP2 + size);
  }

  if (pOp3 != NULL)
  {
    /* Move the input parameters pOp3 to PKA RAM */
    PKA_Memcpy_u8_to_u32(&hpka->Instance->RAM[PKA_ARITHMETIC_ALL_OPS_IN_OP3], pOp3, size * 4UL);
    __PKA_RAM_PARAM_END(hpka->Instance->RAM, PKA_ARITHMETIC_ALL_OPS_IN_OP3 + size);
  }
}
/**
  * @brief  Handle PKA init Timeout.
  * @param  hpka      PKA handle.
  * @param  Flag      Specifies the PKA flag to check
  * @param  Status    Flag status (SET or RESET)
  * @param  Tickstart Tick start value
  * @param  Timeout   Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef PKA_WaitOnFlagUntilTimeout(PKA_HandleTypeDef *hpka, uint32_t Flag, FlagStatus Status,
                                             uint32_t Tickstart, uint32_t Timeout)
{
  /* Wait until flag is set */
  while (__HAL_PKA_GET_FLAG(hpka, Flag) == Status)
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        /* Set the state to ready */
        hpka->State = HAL_PKA_STATE_READY;

        /* Set the error code to timeout error */
        hpka->ErrorCode = HAL_PKA_ERROR_TIMEOUT;

        return HAL_TIMEOUT;
      }
    }
  }
  return HAL_OK;
}

/**
  * @brief  Get the size of output result.
  * @param  hpka           PKA handle
  * @param  Startindex     Specifies the start index of the result in the PKA RAM
  * @param  Maxsize        Specifies the possible max size of the result in words
  * @retval size
  */
uint32_t PKA_Result_GetSize(const PKA_HandleTypeDef *hpka, uint32_t Startindex, uint32_t Maxsize)
{
  uint32_t size;
  uint32_t current_index = Maxsize - 1UL;

  /* Determinate the last index of the result in the PKA RAM */
  while ((hpka->Instance->RAM[Startindex + current_index] == 0UL) && (current_index != 0UL))
  {
    current_index--;
  }
  /* Get the size in bytes */
  size = (current_index + 1UL) * 4UL;

  return size;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* defined(PKA) && defined(HAL_PKA_MODULE_ENABLED) */

/**
  * @}
  */
