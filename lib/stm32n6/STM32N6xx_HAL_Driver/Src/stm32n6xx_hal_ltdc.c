/**
  ******************************************************************************
  * @file    stm32n6xx_hal_ltdc.c
  * @author  MCD Application Team
  * @brief   LTDC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the LTDC peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Control functions
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
     The LTDC HAL driver can be used as follows:

     (#) Declare a LTDC_HandleTypeDef handle structure, for example: LTDC_HandleTypeDef  hltdc;

     (#) Initialize the LTDC low level resources by implementing the HAL_LTDC_MspInit() API:
         (##) Enable the LTDC interface clock
         (##) NVIC configuration if you need to use interrupt process
             (+++) Configure the LTDC interrupt priority
             (+++) Enable the NVIC LTDC IRQ Channel

     (#) Initialize the required configuration through the following parameters:
         the LTDC timing, the horizontal and vertical polarity, the pixel clock polarity,
         Data Enable polarity and the LTDC background color value using HAL_LTDC_Init() function

     *** Configuration ***
     =========================
     [..]
     (#) Program the required configuration through the following parameters:
         the pixel format, the blending factors, input alpha value, the window size
         and the image size using HAL_LTDC_ConfigLayer() function for foreground
         or/and background layer.

     (#) Optionally, configure and enable the CLUT using HAL_LTDC_ConfigCLUT() and
         HAL_LTDC_EnableCLUT functions.

     (#) Optionally, enable the Dither using HAL_LTDC_EnableDither().

     (#) Optionally, configure and enable the Color keying using HAL_LTDC_ConfigColorKeying()
         and HAL_LTDC_EnableColorKeying functions.

     (#) Optionally, configure LineInterrupt using HAL_LTDC_ProgramLineEvent()
         function

     (#) If needed, reconfigure and change the pixel format value, the alpha value
         value, the window size, the window position and the layer start address
         for foreground or/and background layer using respectively the following
         functions: HAL_LTDC_SetPixelFormat(), HAL_LTDC_SetAlpha(), HAL_LTDC_SetWindowSize(),
         HAL_LTDC_SetWindowPosition() and HAL_LTDC_SetAddress().

     (#) Variant functions with _NoReload suffix allows to set the LTDC configuration/settings without immediate reload.
         This is useful in case when the program requires to modify serval LTDC settings (on one or both layers)
         then applying(reload) these settings in one shot by calling the function HAL_LTDC_Reload().

         After calling the _NoReload functions to set different color/format/layer settings,
         the program shall call the function HAL_LTDC_Reload() to apply(reload) these settings.
         Function HAL_LTDC_Reload() can be called with the parameter ReloadType set to LTDC_RELOAD_IMMEDIATE if
         an immediate reload is required.
         Function HAL_LTDC_Reload() can be called with the parameter ReloadType set to LTDC_RELOAD_VERTICAL_BLANKING if
         the reload should be done in the next vertical blanking period,
         this option allows to avoid display flicker by applying the new settings during the vertical blanking period.


     (#) To control LTDC state you can use the following function: HAL_LTDC_GetState()

     *** LTDC HAL driver macros list ***
     =============================================
     [..]
       Below the list of most used macros in LTDC HAL driver.

      (+) __HAL_LTDC_ENABLE: Enable the LTDC.
      (+) __HAL_LTDC_DISABLE: Disable the LTDC.
      (+) __HAL_LTDC_LAYER_ENABLE: Enable an LTDC Layer.
      (+) __HAL_LTDC_LAYER_DISABLE: Disable an LTDC Layer.
      (+) __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG: Reload  Layer Configuration.
      (+) __HAL_LTDC_GET_FLAG: Get the LTDC pending flags.
      (+) __HAL_LTDC_CLEAR_FLAG: Clear the LTDC pending flags.
      (+) __HAL_LTDC_ENABLE_IT: Enable the specified LTDC interrupts.
      (+) __HAL_LTDC_DISABLE_IT: Disable the specified LTDC interrupts.
      (+) __HAL_LTDC_GET_IT_SOURCE: Check whether the specified LTDC interrupt has occurred or not.

     [..]
       (@) You can refer to the LTDC HAL driver header file for more useful macros


     *** Callback registration ***
     =============================================
     [..]
     The compilation define  USE_HAL_LTDC_REGISTER_CALLBACKS when set to 1
     allows the user to configure dynamically the driver callbacks.
     Use function HAL_LTDC_RegisterCallback() to register a callback.

    [..]
    Function HAL_LTDC_RegisterCallback() allows to register following callbacks:
      (+) LineEventCallback   : LTDC Line Event Callback.
      (+) ReloadEventCallback : LTDC Reload Event Callback.
      (+) ErrorCallback       : LTDC Error Callback
      (+) MspInitCallback     : LTDC MspInit.
      (+) MspDeInitCallback   : LTDC MspDeInit.
    [..]
    This function takes as parameters the HAL peripheral handle, the callback ID
    and a pointer to the user callback function.

    [..]
    Use function HAL_LTDC_UnRegisterCallback() to reset a callback to the default
    weak function.
    HAL_LTDC_UnRegisterCallback() takes as parameters the HAL peripheral handle
    and the callback ID.
    [..]
    This function allows to reset following callbacks:
      (+) LineEventCallback   : LTDC Line Event Callback
      (+) ReloadEventCallback : LTDC Reload Event Callback
      (+) ErrorCallback       : LTDC Error Callback
      (+) MspInitCallback     : LTDC MspInit
      (+) MspDeInitCallback   : LTDC MspDeInit.

    [..]
    By default, after the HAL_LTDC_Init and when the state is HAL_LTDC_STATE_RESET
    all callbacks are set to the corresponding weak functions:
    examples HAL_LTDC_LineEventCallback(), HAL_LTDC_ErrorCallback().
    Exception done for MspInit and MspDeInit functions that are
    reset to the legacy weak (surcharged) functions in the HAL_LTDC_Init() and HAL_LTDC_DeInit()
    only when these callbacks are null (not registered beforehand).
    If not, MspInit or MspDeInit are not null, the HAL_LTDC_Init() and HAL_LTDC_DeInit()
    keep and use the user MspInit/MspDeInit callbacks (registered beforehand).

    [..]
    Callbacks can be registered/unregistered in HAL_LTDC_STATE_READY state only.
    Exception done MspInit/MspDeInit that can be registered/unregistered
    in HAL_LTDC_STATE_READY or HAL_LTDC_STATE_RESET state,
    thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
    In that case first register the MspInit/MspDeInit user callbacks
    using HAL_LTDC_RegisterCallback() before calling HAL_LTDC_DeInit()
    or HAL_LTDC_Init() function.

    [..]
    When the compilation define USE_HAL_LTDC_REGISTER_CALLBACKS is set to 0 or
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

#ifdef HAL_LTDC_MODULE_ENABLED

#if defined (LTDC)

/** @defgroup LTDC LTDC
  * @brief LTDC HAL module driver
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup LTDC_Private_Define LTDC Private Define
  * @{
  */
#define LTDC_TIMEOUT_VALUE ((uint32_t)100U)  /* 100ms */
#define LTDC_PIXEL_FORMAT_FLEX_ARGB               0x00CU       /*!< Flexible ARGB format LTDC pixel format*/
#define LTDC_PIXEL_FORMAT_FLEX_YUV_COPLANAR       0x00DU       /*!< Flexible Co-planar format*/
#define LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR     0x10DU       /*!< Flexible Semi planar format*/
#define LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR     0x20DU       /*!< Flexible Full planar format*/
#define LTDC_FLEXIBLE_PIXEL_FORMAT                0x007U       /*!< Flexible pixel format selection */
#define LTDC_PITCH_SIGN_MSK                       0x40000000U  /*!< Mask to check Pitch sign  */
/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* YUV to RGB conversion coefficients for BT601/709 Full/Reduced Luminance */
static const uint32_t V2R[4] = {403, 459, 359, 409};
static const uint32_t U2G[4] = {48, 55, 88, 100};
static const uint32_t V2G[4] = {120, 136, 183, 208};
static const uint32_t U2B[4] = {475, 541, 454, 516};

/* Private function prototypes -----------------------------------------------*/
static void LTDC_SetConfig(LTDC_HandleTypeDef *hltdc,  uint32_t Aux0Addr, uint32_t Aux1Addr, uint32_t Mirror,
                           uint32_t LayerIdx);
static void LTDC_SetCompositionConfig(LTDC_HandleTypeDef *hltdc,  uint32_t LayerIdx);
static void LTDC_SetPredefFormat(LTDC_HandleTypeDef *hltdc,  uint32_t LayerIdx);
static void LTDC_RetrieveUserConfig(LTDC_HandleTypeDef *hltdc, uint32_t *Mirror, uint32_t *Aux0Addr,
                                    uint32_t *Aux1Addr, uint32_t LayerIdx);
/* Private functions ---------------------------------------------------------*/

/** @defgroup LTDC_Exported_Functions LTDC Exported Functions
  * @{
  */

/** @defgroup LTDC_Exported_Functions_Group1 Initialization and Configuration functions
  *  @brief   Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
                ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the LTDC
      (+) De-initialize the LTDC

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the LTDC according to the specified parameters in the LTDC_InitTypeDef.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_Init(LTDC_HandleTypeDef *hltdc)
{
  uint32_t tmp;
  uint32_t tmp1;

  /* Check the LTDC peripheral state */
  if (hltdc == NULL)
  {
    return HAL_ERROR;
  }

  /* Check function parameters */
  assert_param(IS_LTDC_ALL_INSTANCE(hltdc->Instance));
  assert_param(IS_LTDC_HSYNC(hltdc->Init.HorizontalSync));
  assert_param(IS_LTDC_VSYNC(hltdc->Init.VerticalSync));
  assert_param(IS_LTDC_AHBP(hltdc->Init.AccumulatedHBP));
  assert_param(IS_LTDC_AVBP(hltdc->Init.AccumulatedVBP));
  assert_param(IS_LTDC_AAH(hltdc->Init.AccumulatedActiveH));
  assert_param(IS_LTDC_AAW(hltdc->Init.AccumulatedActiveW));
  assert_param(IS_LTDC_TOTALH(hltdc->Init.TotalHeigh));
  assert_param(IS_LTDC_TOTALW(hltdc->Init.TotalWidth));
  assert_param(IS_LTDC_HSPOL(hltdc->Init.HSPolarity));
  assert_param(IS_LTDC_VSPOL(hltdc->Init.VSPolarity));
  assert_param(IS_LTDC_DEPOL(hltdc->Init.DEPolarity));
  assert_param(IS_LTDC_PCPOL(hltdc->Init.PCPolarity));

#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
  if (hltdc->State == HAL_LTDC_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hltdc->Lock = HAL_UNLOCKED;

    /* Reset the LTDC callback to the legacy weak callbacks */
    hltdc->LineEventCallback   = HAL_LTDC_LineEventCallback;    /* Legacy weak LineEventCallback    */
    hltdc->ReloadEventCallback = HAL_LTDC_ReloadEventCallback;  /* Legacy weak ReloadEventCallback  */
    hltdc->ErrorCallback       = HAL_LTDC_ErrorCallback;        /* Legacy weak ErrorCallback        */

    if (hltdc->MspInitCallback == NULL)
    {
      hltdc->MspInitCallback = HAL_LTDC_MspInit;
    }
    /* Init the low level hardware */
    hltdc->MspInitCallback(hltdc);
  }
#else
  if (hltdc->State == HAL_LTDC_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hltdc->Lock = HAL_UNLOCKED;
    /* Init the low level hardware */
    HAL_LTDC_MspInit(hltdc);
  }
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Configure the HS, VS, DE and PC polarity */
  hltdc->Instance->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
  hltdc->Instance->GCR |= (uint32_t)(hltdc->Init.HSPolarity | hltdc->Init.VSPolarity | \
                                     hltdc->Init.DEPolarity | hltdc->Init.PCPolarity);

  /* Set Synchronization size */
  tmp = (hltdc->Init.HorizontalSync << 16U);
  WRITE_REG(hltdc->Instance->SSCR, (tmp | hltdc->Init.VerticalSync));

  /* Set Accumulated Back porch */
  tmp = (hltdc->Init.AccumulatedHBP << 16U);
  WRITE_REG(hltdc->Instance->BPCR, (tmp | hltdc->Init.AccumulatedVBP));

  /* Set Accumulated Active Width */
  tmp = (hltdc->Init.AccumulatedActiveW << 16U);
  WRITE_REG(hltdc->Instance->AWCR, (tmp | hltdc->Init.AccumulatedActiveH));

  /* Set Total Width */
  tmp = (hltdc->Init.TotalWidth << 16U);
  WRITE_REG(hltdc->Instance->TWCR, (tmp | hltdc->Init.TotalHeigh));

  /* Set the background color value */
  tmp = ((uint32_t)(hltdc->Init.Backcolor.Green) << 8U);
  tmp1 = ((uint32_t)(hltdc->Init.Backcolor.Red) << 16U);
  hltdc->Instance->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
  hltdc->Instance->BCCR |= (tmp1 | tmp | hltdc->Init.Backcolor.Blue);

  /* Activate Global Reload for Layer 1 and Layer 2 */
  WRITE_REG(LTDC_LAYER(hltdc, LTDC_LAYER_1)->RCR, LTDC_LxRCR_GRMSK);
  WRITE_REG(LTDC_LAYER(hltdc, LTDC_LAYER_2)->RCR, LTDC_LxRCR_GRMSK);

  /* Enable the Transfer Error and FIFO underrun interrupts */
  __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_TE | LTDC_IT_FU);

  /* Enable LTDC by setting LTDCEN bit */
  __HAL_LTDC_ENABLE(hltdc);

  /* Initialize the error code */
  hltdc->ErrorCode = HAL_LTDC_ERROR_NONE;

  /* Initialize the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  De-initialize the LTDC peripheral.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */

HAL_StatusTypeDef HAL_LTDC_DeInit(LTDC_HandleTypeDef *hltdc)
{
  uint32_t tickstart;

  /* Check the LTDC peripheral state */
  if (hltdc == NULL)
  {
    return HAL_ERROR;
  }

  /* Check function parameters */
  assert_param(IS_LTDC_ALL_INSTANCE(hltdc->Instance));

  /* Disable LTDC Layer 1 */
  __HAL_LTDC_LAYER_DISABLE(hltdc, LTDC_LAYER_1);

#if defined(LTDC_Layer2_BASE)
  /* Disable LTDC Layer 2 */
  __HAL_LTDC_LAYER_DISABLE(hltdc, LTDC_LAYER_2);
#endif /* LTDC_Layer2_BASE */

  /* Reload during vertical blanking period */
  __HAL_LTDC_VERTICAL_BLANKING_RELOAD_CONFIG(hltdc);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait for VSYNC Interrupt */
  while (READ_BIT(hltdc->Instance->CDSR, LTDC_CDSR_VSYNCS) == 0U)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > LTDC_TIMEOUT_VALUE)
    {
      break;
    }
  }

  /* Disable LTDC  */
  __HAL_LTDC_DISABLE(hltdc);

#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
  if (hltdc->MspDeInitCallback == NULL)
  {
    hltdc->MspDeInitCallback = HAL_LTDC_MspDeInit;
  }
  /* DeInit the low level hardware */
  hltdc->MspDeInitCallback(hltdc);
#else
  /* DeInit the low level hardware */
  HAL_LTDC_MspDeInit(hltdc);
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */

  /* Initialize the error code */
  hltdc->ErrorCode = HAL_LTDC_ERROR_NONE;

  /* Initialize the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Initialize the LTDC MSP.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hltdc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LTDC_MspInit could be implemented in the user file
   */
}

/**
  * @brief  De-initialize the LTDC MSP.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef *hltdc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hltdc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LTDC_MspDeInit could be implemented in the user file
   */
}

#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User LTDC Callback
  *         To be used instead of the weak predefined callback
  * @param hltdc ltdc handle
  * @param CallbackID ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_LTDC_LINE_EVENT_CB_ID Line Event Callback ID
  *          @arg @ref HAL_LTDC_RELOAD_EVENT_CB_ID Reload Event Callback ID
  *          @arg @ref HAL_LTDC_ERROR_CB_ID Error Callback ID
  *          @ref @ref HAL_LTDC_WARNING_EVENT_CB_ID Warning Event Callback ID
  *          @arg @ref HAL_LTDC_MSPINIT_CB_ID MspInit callback ID
  *          @arg @ref HAL_LTDC_MSPDEINIT_CB_ID MspDeInit callback ID
  * @param pCallback pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_LTDC_RegisterCallback(LTDC_HandleTypeDef *hltdc, HAL_LTDC_CallbackIDTypeDef CallbackID,
                                            pLTDC_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hltdc->ErrorCode |= HAL_LTDC_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }
  /* Process locked */
  __HAL_LOCK(hltdc);

  if (hltdc->State == HAL_LTDC_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_LTDC_LINE_EVENT_CB_ID :
        hltdc->LineEventCallback = pCallback;
        break;

      case HAL_LTDC_RELOAD_EVENT_CB_ID :
        hltdc->ReloadEventCallback = pCallback;
        break;

      case HAL_LTDC_ERROR_CB_ID :
        hltdc->ErrorCallback = pCallback;
        break;

      case HAL_LTDC_WARNING_EVENT_CB_ID:
        hltdc->WarningEventCallback = pCallback;
        break;

      case HAL_LTDC_MSPINIT_CB_ID :
        hltdc->MspInitCallback = pCallback;
        break;

      case HAL_LTDC_MSPDEINIT_CB_ID :
        hltdc->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hltdc->ErrorCode |= HAL_LTDC_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hltdc->State == HAL_LTDC_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_LTDC_MSPINIT_CB_ID :
        hltdc->MspInitCallback = pCallback;
        break;

      case HAL_LTDC_MSPDEINIT_CB_ID :
        hltdc->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hltdc->ErrorCode |= HAL_LTDC_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hltdc->ErrorCode |= HAL_LTDC_ERROR_INVALID_CALLBACK;
    /* Return error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hltdc);

  return status;
}

/**
  * @brief  Unregister an LTDC Callback
  *         LTDC callback is redirected to the weak predefined callback
  * @param hltdc ltdc handle
  * @param CallbackID ID of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_LTDC_LINE_EVENT_CB_ID Line Event Callback ID
  *          @arg @ref HAL_LTDC_RELOAD_EVENT_CB_ID Reload Event Callback ID
  *          @arg @ref HAL_LTDC_ERROR_CB_ID Error Callback ID
  *          @arg @ref HAL_LTDC_WARNING_EVENT_CB_ID Warning Event Callback ID
  *          @arg @ref HAL_LTDC_MSPINIT_CB_ID MspInit callback ID
  *          @arg @ref HAL_LTDC_MSPDEINIT_CB_ID MspDeInit callback ID
  * @retval status
  */
HAL_StatusTypeDef HAL_LTDC_UnRegisterCallback(LTDC_HandleTypeDef *hltdc, HAL_LTDC_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Process locked */
  __HAL_LOCK(hltdc);

  if (hltdc->State == HAL_LTDC_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_LTDC_LINE_EVENT_CB_ID :
        hltdc->LineEventCallback = HAL_LTDC_LineEventCallback;      /* Legacy weak LineEventCallback    */
        break;

      case HAL_LTDC_RELOAD_EVENT_CB_ID :
        hltdc->ReloadEventCallback = HAL_LTDC_ReloadEventCallback;  /* Legacy weak ReloadEventCallback  */
        break;

      case HAL_LTDC_ERROR_CB_ID :
        hltdc->ErrorCallback       = HAL_LTDC_ErrorCallback;        /* Legacy weak ErrorCallback        */
        break;

      case HAL_LTDC_WARNING_EVENT_CB_ID :
        hltdc->WarningEventCallback = HAL_LTDC_WarningEventCallback;
        break;

      case HAL_LTDC_MSPINIT_CB_ID :
        hltdc->MspInitCallback = HAL_LTDC_MspInit;                  /* Legcay weak MspInit Callback  */
        break;

      case HAL_LTDC_MSPDEINIT_CB_ID :
        hltdc->MspDeInitCallback = HAL_LTDC_MspDeInit;              /* Legcay weak MspDeInit Callback     */
        break;

      default :
        /* Update the error code */
        hltdc->ErrorCode |= HAL_LTDC_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hltdc->State == HAL_LTDC_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_LTDC_MSPINIT_CB_ID :
        hltdc->MspInitCallback = HAL_LTDC_MspInit;                  /* Legcay weak MspInit Callback     */
        break;

      case HAL_LTDC_MSPDEINIT_CB_ID :
        hltdc->MspDeInitCallback = HAL_LTDC_MspDeInit;              /* Legcay weak MspDeInit Callback     */
        break;

      default :
        /* Update the error code */
        hltdc->ErrorCode |= HAL_LTDC_ERROR_INVALID_CALLBACK;
        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hltdc->ErrorCode |= HAL_LTDC_ERROR_INVALID_CALLBACK;
    /* Return error status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hltdc);

  return status;
}
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */

/**
  * @brief  Configure the LTDC Layer burst length
  * @param  hltdc        Pointer to a LTDC_HandleTypeDef structure that contains
  *                       the configuration information for the LTDC.
  * @param  BurstLength  Burst length.
  *                       This parameter can be a value between 1-16
  * @param  LayerIdx     LTDC Layer index.
  *                       This parameter can be one of the following values:
  *                       LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigBurstLength(LTDC_HandleTypeDef *hltdc, uint32_t BurstLength, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_BURST_LENGTH(BurstLength));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Configure the layer burst length configuration register */
  LTDC_LAYER(hltdc, LayerIdx)->BLCR  &= ~(LTDC_LxBLCR_BL);
  LTDC_LAYER(hltdc, LayerIdx)->BLCR  = BurstLength;

  /* Initialize the LTDC state*/
  hltdc->State  = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}


/**
  * @brief               Configure the LTDC Underrun Threshold.
  * @param  hltdc        Pointer to a LTDC_HandleTypeDef structure that contains
  *                       the configuration information for the LTDC.
  * @param  Threshold    Threshold above which
  *                       a FIFO underrun warning becomes a FIFO underrun error.
  *                       This parameter can be a value between 0 - 0XFFFF
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigUnderrunThreshold(LTDC_HandleTypeDef *hltdc, uint16_t Threshold)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Configure the Fifo Underrun Threshold register */
  hltdc->Instance->FUTR &= ~(LTDC_FUTR_THRE);
  hltdc->Instance->FUTR = (uint32_t) Threshold;

  /* Initialize the LTDC state*/
  hltdc->State  = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup LTDC_Exported_Functions_Group2 IO operation functions
  *  @brief   IO operation functions
  *
@verbatim
 ===============================================================================
                      #####  IO operation functions  #####
 ===============================================================================
    [..]  This section provides function allowing to:
      (+) Handle LTDC interrupt request

@endverbatim
  * @{
  */
/**
  * @brief  Handle LTDC interrupt request.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval HAL status
  */
void HAL_LTDC_IRQHandler(LTDC_HandleTypeDef *hltdc)
{
#if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  uint32_t isrflags = READ_REG(hltdc->Instance->ISR2);
  uint32_t itsources = READ_REG(hltdc->Instance->IER2);
#else
  uint32_t isrflags = READ_REG(hltdc->Instance->ISR);
  uint32_t itsources = READ_REG(hltdc->Instance->IER);
#endif /* __ARM_FEATURE_CMSE & __ARM_FEATURE_CMSE == 3U */


  /* CRC Interrupt management ***************************************/
  if (((isrflags & LTDC_ISR_CRCIF) != 0U) && ((itsources & LTDC_IER_CRCIE) != 0U))
  {
    /* Disable the transfer Error interrupt */
    __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_CRC);

    /* Clear the crc flag */
    __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_CRC);

    /* Update error code */
    hltdc->ErrorCode |= HAL_LTDC_ERROR_CRC;

    /* Change LTDC state */
    hltdc->State = HAL_LTDC_STATE_ERROR;

    /* Process unlocked */
    __HAL_UNLOCK(hltdc);

    /* Transfer error Callback */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
    /*Call registered error callback*/
    hltdc->ErrorCallback(hltdc);
#else
    /* Call legacy error callback*/
    HAL_LTDC_ErrorCallback(hltdc);
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */
  }

  /* Fifo Underrun Interrupt management ***************************************/
  if (((isrflags & LTDC_ISR_FUIF) != 0U) && ((itsources & LTDC_IER_FUIE) != 0U))
  {
    /* Disable the transfer Error interrupt */
    __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_FU);

    /* Clear the UK flag */
    __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_FU);

    /* Update error code */
    hltdc->ErrorCode |= HAL_LTDC_ERROR_FU;

    /* Change LTDC state */
    hltdc->State = HAL_LTDC_STATE_ERROR;

    /* Process unlocked */
    __HAL_UNLOCK(hltdc);

    /* Transfer error Callback */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
    /*Call registered error callback*/
    hltdc->ErrorCallback(hltdc);
#else
    /* Call legacy error callback*/
    HAL_LTDC_ErrorCallback(hltdc);
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */
  }

  /* Transfer Error Interrupt management ***************************************/
  if (((isrflags & LTDC_ISR_TERRIF) != 0U) && ((itsources & LTDC_IER_TERRIE) != 0U))
  {
    /* Disable the transfer Error interrupt */
    __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_TE);

    /* Clear the transfer error flag */
    __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_TE);

    /* Update error code */
    hltdc->ErrorCode |= HAL_LTDC_ERROR_TE;

    /* Change LTDC state */
    hltdc->State = HAL_LTDC_STATE_ERROR;

    /* Process unlocked */
    __HAL_UNLOCK(hltdc);

    /* Transfer error Callback */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
    /*Call registered error callback*/
    hltdc->ErrorCallback(hltdc);
#else
    /* Call legacy error callback*/
    HAL_LTDC_ErrorCallback(hltdc);
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */
  }

  /* FIFO underrun Interrupt management ***************************************/
  if (((isrflags & LTDC_ISR_FUIF) != 0U) && ((itsources & LTDC_IER_FUIE) != 0U))
  {
    /* Disable the FIFO underrun interrupt */
    __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_FU);

    /* Clear the FIFO underrun flag */
    __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_FU);

    /* Update error code */
    hltdc->ErrorCode |= HAL_LTDC_ERROR_FU;

    /* Change LTDC state */
    hltdc->State = HAL_LTDC_STATE_ERROR;

    /* Process unlocked */
    __HAL_UNLOCK(hltdc);

    /* Transfer error Callback */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
    /*Call registered error callback*/
    hltdc->ErrorCallback(hltdc);
#else
    /* Call legacy error callback*/
    HAL_LTDC_ErrorCallback(hltdc);
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */
  }

  /* FIFO underrun Warning Interrupt management ***************************************/
  if (((isrflags & LTDC_ISR_FUWIF) != 0U) && ((itsources & LTDC_IER_FUWIE) != 0U))
  {
    /* Disable the FIFO underrun interrupt */
    __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_FUW);

    /* Clear the FIFO underrun flag */
    __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_FUW);

    /* Process unlocked */
    __HAL_UNLOCK(hltdc);

    /* Transfer error Callback */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
    /*Call registered error callback*/
    hltdc->WarningEventCallback(hltdc);
#else
    /* Call legacy error callback*/
    HAL_LTDC_WarningEventCallback(hltdc);
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */
  }

  /* Line Interrupt management ************************************************/
  if (((isrflags & LTDC_ISR_LIF) != 0U) && ((itsources & LTDC_IER_LIE) != 0U))
  {
    /* Disable the Line interrupt */
    __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_LI);

    /* Clear the Line interrupt flag */
    __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_LI);

    /* Change LTDC state */
    hltdc->State = HAL_LTDC_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hltdc);

    /* Line interrupt Callback */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
    /*Call registered Line Event callback */
    hltdc->LineEventCallback(hltdc);
#else
    /*Call Legacy Line Event callback */
    HAL_LTDC_LineEventCallback(hltdc);
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */
  }

  /* Register reload Interrupt management ***************************************/
  if (((isrflags & LTDC_ISR_RRIF) != 0U) && ((itsources & LTDC_IER_RRIE) != 0U))
  {
    /* Disable the register reload interrupt */
    __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_RR);

    /* Clear the register reload flag */
    __HAL_LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_RR);

    /* Change LTDC state */
    hltdc->State = HAL_LTDC_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hltdc);

    /* Reload interrupt Callback */
#if (USE_HAL_LTDC_REGISTER_CALLBACKS == 1)
    /*Call registered reload Event callback */
    hltdc->ReloadEventCallback(hltdc);
#else
    /*Call Legacy Reload Event callback */
    HAL_LTDC_ReloadEventCallback(hltdc);
#endif /* USE_HAL_LTDC_REGISTER_CALLBACKS */
  }
}

/**
  * @brief  Error LTDC callback.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_ErrorCallback(LTDC_HandleTypeDef *hltdc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hltdc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LTDC_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  Line Event callback.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef *hltdc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hltdc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LTDC_LineEventCallback could be implemented in the user file
   */
}

/**
  * @brief  Reload Event callback.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_ReloadEventCallback(LTDC_HandleTypeDef *hltdc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hltdc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LTDC_ReloadEvenCallback could be implemented in the user file
   */
}

/**
  * @brief  LTDC FIFO Uderrun Warning callback.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void HAL_LTDC_WarningEventCallback(LTDC_HandleTypeDef *hltdc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hltdc);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_LTDC_SErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup LTDC_Exported_Functions_Group3 Peripheral Control functions
  *  @brief    Peripheral Control functions
  *
@verbatim
 ===============================================================================
                    ##### Peripheral Control functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Configure the LTDC foreground or/and background parameters.
      (+) Set the active layer.
      (+) Configure the color keying.
      (+) Configure the C-LUT.
      (+) Enable / Disable the color keying.
      (+) Enable / Disable the C-LUT.
      (+) Update the layer position.
      (+) Update the layer size.
      (+) Update pixel format on the fly.
      (+) Update transparency on the fly.
      (+) Update address on the fly.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the LTDC Layer according to the specified
  *         parameters in the LTDC_InitTypeDef and create the associated handle.
  * @param  hltdc      pointer to a LTDC_HandleTypeDef structure that contains
  *                    the configuration information for the LTDC.
  * @param  pLayerCfg  pointer to a LTDC_LayerCfgTypeDef structure that contains
  *                    the configuration information for the Layer.
  * @param  LayerIdx  LTDC Layer index.
  *                    This parameter can be one of the following values:
  *                    LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigLayer(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_HCONFIGST(pLayerCfg->WindowX0));
  assert_param(IS_LTDC_HCONFIGSP(pLayerCfg->WindowX1));
  assert_param(IS_LTDC_VCONFIGST(pLayerCfg->WindowY0));
  assert_param(IS_LTDC_VCONFIGSP(pLayerCfg->WindowY1));
  assert_param(IS_LTDC_PIXEL_FORMAT(pLayerCfg->PixelFormat));
  assert_param(IS_LTDC_ALPHA(pLayerCfg->Alpha));
  assert_param(IS_LTDC_ALPHA(pLayerCfg->Alpha0));
  assert_param(IS_LTDC_BLENDING_FACTOR1(pLayerCfg->BlendingFactor1));
  assert_param(IS_LTDC_BLENDING_FACTOR2(pLayerCfg->BlendingFactor2));
  assert_param(IS_LTDC_CFBLL(pLayerCfg->ImageWidth));
  assert_param(IS_LTDC_CFBLNBR(pLayerCfg->ImageHeight));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Copy new layer configuration into handle structure */
  hltdc->LayerCfg[LayerIdx] = *pLayerCfg;

  /* Configure Predefined format */
  LTDC_SetPredefFormat(hltdc, LayerIdx);

  /* Configure composition and blending*/
  LTDC_SetCompositionConfig(hltdc, LayerIdx);

  /* Disable YUV format */
  CLEAR_BIT(LTDC_LAYER(hltdc, LayerIdx)->PCR, LTDC_LxPCR_YCEN);

  /* Configure the LTDC Layer */
  LTDC_SetConfig(hltdc, 0U, 0U, LTDC_MIRROR_NONE, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Initialize the LTDC state*/
  hltdc->State  = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Configure the color keying.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  RGBValue  the color key value
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t RGBValue, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Configure the default color values */
  LTDC_LAYER(hltdc, LayerIdx)->CKCR &=  ~(LTDC_LxCKCR_CKBLUE | LTDC_LxCKCR_CKGREEN | LTDC_LxCKCR_CKRED);
  LTDC_LAYER(hltdc, LayerIdx)->CKCR  = RGBValue;

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Load the color lookup table.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  pCLUT     pointer to the color lookup table address.
  * @param  CLUTSize  the color lookup table size.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigCLUT(LTDC_HandleTypeDef *hltdc, const uint32_t *pCLUT, uint32_t CLUTSize,
                                      uint32_t LayerIdx)
{
  uint32_t tmp;
  uint32_t counter;
  const uint32_t *pcolorlut = pCLUT;
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  for (counter = 0U; (counter < CLUTSize); counter++)
  {
    if (hltdc->LayerCfg[LayerIdx].PixelFormat == LTDC_PIXEL_FORMAT_AL44)
    {
      tmp  = (((counter + (16U * counter)) << 24U) | ((uint32_t)(*pcolorlut) & 0xFFU) | \
              ((uint32_t)(*pcolorlut) & 0xFF00U) | ((uint32_t)(*pcolorlut) & 0xFF0000U));
    }
    else
    {
      tmp  = ((counter << 24U) | ((uint32_t)(*pcolorlut) & 0xFFU) | \
              ((uint32_t)(*pcolorlut) & 0xFF00U) | ((uint32_t)(*pcolorlut) & 0xFF0000U));
    }

    pcolorlut++;

    /* Specifies the C-LUT address and RGB value */
    LTDC_LAYER(hltdc, LayerIdx)->CLUTWR  = tmp;
  }

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Configures the gamma correction for the LTDC peripheral.
  *
  * This function sets up the gamma correction feature of the LTDC (Liquid Crystal Display Controller)
  * peripheral. Gamma correction helps in adjusting the brightness of the output image. This function
  * allows the configuration of the gamma curve by setting the values for ones, tenths, and the specific
  * RGB component to be adjusted.
  *
  * @param  hltdc      pointer to a LTDC_HandleTypeDef structure that contains
  *                    the configuration information for the LTDC.
  * @param GammaOnes Specifies the value for the ones place in the gamma correction factor.
  *                  This parameter can be a value between 0 and 2.
  * @param GammaTenths Specifies the value for the tenths place in the gamma correction factor.
  *                    This parameter can be a value between 0 and 9 if GammaOnes > 0 else between 4 and 9.
  * @param RGBComponent Specifies the RGB component to which the gamma correction is applied.
  *                     This parameter can be one of the following values:
  *                     @arg LTDC_RGB_COMPONENT_RED  : Gamma correction for the red component.
  *                     @arg LTDC_RGB_COMPONENT_GREEN: Gamma correction for the green component.
  *                     @arg LTDC_RGB_COMPONENT_BLUE : Gamma correction for the blue component.
  *                     @arg LTDC_RGB_COMPONENT_ALL  : Gamma correction for all components.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigGammaCorrection(LTDC_HandleTypeDef *hltdc, uint32_t GammaOnes,
                                                 uint32_t GammaTenths, uint32_t RGBComponent)
{
  const uint32_t gammaindex = (GammaOnes * 70U) + (GammaTenths  * 7U) - 28U;
  uint8_t gammasegment;
  const uint8_t GammaAdress[7] = {32, 64, 96, 128, 160, 192, 224};
  /* Gamma Mapped coefficients for segments 1 to 7 */
  const uint8_t GammaLUT[182] =
  {
    111, 147, 173, 194, 212, 228, 242,  /* Gamma = 0.4 */
    90, 128, 156, 181, 202, 221, 239,   /* Gamma = 0.5 */
    73, 111, 142, 169, 193, 215, 236,   /* Gamma = 0.6 */
    60, 97, 129, 157, 184, 209, 233,    /* Gamma = 0.7 */
    48, 84, 117, 147, 176, 203, 230,    /* Gamma = 0.8 */
    39, 73, 106, 137, 168, 198, 227,    /* Gamma = 0.9 */
    32, 64, 96, 128, 160, 192, 224,     /* Gamma = 1.0 */
    26, 56, 87, 119, 153, 187, 221,     /* Gamma = 1.1 */
    21, 49, 79, 112, 146, 181, 218,     /* Gamma = 1.2 */
    17, 42, 72, 104, 139, 176, 215,     /* Gamma = 1.3 */
    14, 37, 65, 97, 133, 171, 213,      /* Gamma = 1.4 */
    11, 32, 59, 91, 127, 167, 210,      /* Gamma = 1.5 */
    9, 28, 53, 85, 121, 162, 207,       /* Gamma = 1.6 */
    7, 24, 48, 79, 115, 157, 205,       /* Gamma = 1.7 */
    6, 21, 44, 74, 110, 153, 202,       /* Gamma = 1.8 */
    5, 18, 40, 69, 105, 149, 199,       /* Gamma = 1.9 */
    4, 16, 36, 64, 100, 145, 197,       /* Gamma = 2.0 */
    3, 14, 33, 60, 96, 141, 194,        /* Gamma = 2.1 */
    3, 12, 30, 56, 91, 137, 192,        /* Gamma = 2.2 */
    2, 11, 27, 52, 87, 133, 189,        /* Gamma = 2.3 */
    2, 9, 24, 49, 83, 129, 187,         /* Gamma = 2.4 */
    1, 8, 22, 46, 80, 125, 184,         /* Gamma = 2.5 */
    1, 7, 20, 42, 76, 122, 182,         /* Gamma = 2.6 */
    1, 6, 18, 40, 72, 119, 180,         /* Gamma = 2.7 */
    1, 5, 17, 37, 69, 115, 177,         /* Gamma = 2.8 */
    1, 5, 15, 35, 66, 112, 175,         /* Gamma = 2.9 */
  };

  /* Check the parameters */
  assert_param(IS_LTDC_RGB_COMPONENT(RGBComponent));
  assert_param(IS_LTDC_GAMMA_VALUE(GammaOnes, GammaTenths));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Set Gamma interpolated segments*/
  hltdc->Instance->GCCR = RGBComponent ;
  for (gammasegment = 0U; (gammasegment < 7U); gammasegment++)
  {
    hltdc->Instance->GCCR = RGBComponent | ((uint32_t) GammaAdress[gammasegment] << LTDC_GCCR_ADDR_Pos) |
                            ((uint32_t) GammaLUT[gammasegment + gammaindex] << LTDC_GCCR_COMP_Pos);
  }
  hltdc->Instance->GCCR = RGBComponent | (0xFFU << LTDC_GCCR_COMP_Pos) | LTDC_GCCR_ADDR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Enable the color keying.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_EnableColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable LTDC color keying by setting COLKEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR |= (uint32_t)LTDC_LxCR_CKEN;

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Disable the color keying.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_DisableColorKeying(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Disable LTDC color keying by setting COLKEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(uint32_t)LTDC_LxCR_CKEN;

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Enable the color lookup table.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_EnableCLUT(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable LTDC color lookup table by setting CLUTEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR |= (uint32_t)LTDC_LxCR_CLUTEN;

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Disable the color lookup table.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_DisableCLUT(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Disable LTDC color lookup table by setting CLUTEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(uint32_t)LTDC_LxCR_CLUTEN;

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Enable Dither.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

HAL_StatusTypeDef HAL_LTDC_EnableDither(LTDC_HandleTypeDef *hltdc)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable Dither by setting DTEN bit */
  LTDC->GCR |= (uint32_t)LTDC_GCR_DEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Disable Dither.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

HAL_StatusTypeDef HAL_LTDC_DisableDither(LTDC_HandleTypeDef *hltdc)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Disable Dither by setting DTEN bit */
  LTDC->GCR &= ~(uint32_t)LTDC_GCR_DEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}


/**
  * @brief Enables the CRC generator on the LTDC peripheral.
  *
  * Activates the Cyclic Redundancy Check (CRC) generator for the LTDC peripheral. The CRC can be used
  * to verify the integrity of the data processed by the LTDC. Once enabled, the CRC generator computes
  * a CRC value on the configured frame and can be used for error checking purposes.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

HAL_StatusTypeDef HAL_LTDC_EnableCRC(LTDC_HandleTypeDef *hltdc)
{
  uint32_t tickstart;

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable crc by setting CRCEN bit */
  LTDC->GCR |= (uint32_t)LTDC_GCR_CRCEN;

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait for CRC computing */
  while (READ_REG(hltdc->Instance->CCRCR) == 0U)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > LTDC_TIMEOUT_VALUE)
    {
      break;
    }
  }

  __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_CRC);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Disables the CRC generator on the LTDC peripheral.
  *
  * Deactivates the Cyclic Redundancy Check (CRC) generator for the LTDC peripheral. This function
  * stops the CRC computation on the data processed by the LTDC, which may be required when CRC
  * validation is no longer needed.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

HAL_StatusTypeDef HAL_LTDC_DisableCRC(LTDC_HandleTypeDef *hltdc)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_CRC);

  /* Disable crc by clearing CRCEN bit */
  LTDC->GCR &= ~(uint32_t)LTDC_GCR_CRCEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Enables gamma correction on the LTDC peripheral.
  *
  * This function enables the gamma correction feature of the LTDC peripheral, which adjusts the
  * luminance of the output image to improve visual quality. Gamma correction is applied according to
  * the previously configured parameters.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

HAL_StatusTypeDef HAL_LTDC_EnableGammaCorrection(LTDC_HandleTypeDef *hltdc)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable gamma correction by setting GAMEN bit */
  LTDC->GCR |= (uint32_t)LTDC_GCR_GAMEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Disable gamma correction.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

HAL_StatusTypeDef HAL_LTDC_DisableGammaCorrection(LTDC_HandleTypeDef *hltdc)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Disable gamma correction by clearing GAMEN bit */
  LTDC->GCR &= ~(uint32_t)LTDC_GCR_GAMEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}
/**
  * @brief  Set the LTDC window size.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  XSize     LTDC Pixel per line
  * @param  YSize     LTDC Line number
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetWindowSize(LTDC_HandleTypeDef *hltdc, uint32_t XSize, uint32_t YSize, uint32_t LayerIdx)
{
  uint32_t mirror = 0U;
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;

  /* Check the parameters (Layers parameters)*/
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_CFBLL(XSize));
  assert_param(IS_LTDC_CFBLNBR(YSize));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* update horizontal stop */
  hltdc->LayerCfg[LayerIdx].WindowX1 = XSize + hltdc->LayerCfg[LayerIdx].WindowX0;

  /* update vertical stop */
  hltdc->LayerCfg[LayerIdx].WindowY1 = YSize + hltdc->LayerCfg[LayerIdx].WindowY0;

  /* Reconfigures the color frame buffer pitch in byte */
  hltdc->LayerCfg[LayerIdx].ImageWidth = XSize;

  /* Reconfigures the frame buffer line number */
  hltdc->LayerCfg[LayerIdx].ImageHeight = YSize;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, aux0Addr, aux1Addr, mirror, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Set the LTDC window position.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  X0        LTDC window X offset
  * @param  Y0        LTDC window Y offset
  * @param  LayerIdx  LTDC Layer index.
  *                         This parameter can be one of the following values:
  *                         LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetWindowPosition(LTDC_HandleTypeDef *hltdc, uint32_t X0, uint32_t Y0, uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_CFBLL(X0));
  assert_param(IS_LTDC_CFBLNBR(Y0));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* update horizontal start/stop */
  hltdc->LayerCfg[LayerIdx].WindowX0 = X0;
  hltdc->LayerCfg[LayerIdx].WindowX1 = X0 + hltdc->LayerCfg[LayerIdx].ImageWidth;

  /* update vertical start/stop */
  hltdc->LayerCfg[LayerIdx].WindowY0 = Y0;
  hltdc->LayerCfg[LayerIdx].WindowY1 = Y0 + hltdc->LayerCfg[LayerIdx].ImageHeight;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, aux0Addr, aux1Addr, mirror, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Reconfigure the pixel format.
  * @param  hltdc        pointer to a LTDC_HandleTypeDef structure that contains
  *                      the configuration information for the LTDC.
  * @param  Pixelformat  new pixel format value.
  * @param  LayerIdx     LTDC Layer index.
  *                      This parameter can be one of the following values:
  *                      LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetPixelFormat(LTDC_HandleTypeDef *hltdc, uint32_t Pixelformat, uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_PIXEL_FORMAT(Pixelformat));
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* Reconfigure the pixel format */
  hltdc->LayerCfg[LayerIdx].PixelFormat = Pixelformat;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, aux0Addr, aux1Addr, mirror, LayerIdx);

  /* Set LTDC format */
  LTDC_SetPredefFormat(hltdc, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Reconfigure the layer alpha value.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Alpha     new alpha value.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetAlpha(LTDC_HandleTypeDef *hltdc, uint32_t Alpha, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_ALPHA(Alpha));
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Specifies the constant alpha value */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->CACR, Alpha);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}
/**
  *
  * @brief Reconfigure the memory address for a layer buffer in the LTDC peripheral.
  *
  * This function configures the memory address for a layer buffer that can contain either ARGB data or
  * YUV co-planar data.
  * This allows dynamic updating of the frame buffer address for the specified layer.
  *
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Address   The memory address where the layer data (ARGB or YUV co-planar) is stored.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetAddress(LTDC_HandleTypeDef *hltdc, uint32_t Address, uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* Reconfigure the Address */
  hltdc->LayerCfg[LayerIdx].FBStartAdress = Address;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, 0U, 0U, mirror, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Function used to reconfigure the pitch for specific cases where the attached LayerIdx buffer have a width
  *         that is larger than the one intended to be displayed on screen. Example of a buffer 800x480 attached to
  *         layer for which we want to read and display on screen only a portion 320x240 taken in the center
  *         of the buffer.
  *         The pitch in pixels will be in that case 800 pixels and not 320 pixels as initially configured by previous
  *         call to HAL_LTDC_ConfigLayer().
  * @note   This function should be called only after a previous call to HAL_LTDC_ConfigLayer() to modify the default
  *         pitch configured by HAL_LTDC_ConfigLayer() when required (refer to example described just above).
  * @param  hltdc              pointer to a LTDC_HandleTypeDef structure that contains
  *                            the configuration information for the LTDC.
  * @param  LinePitchInPixels  New line pitch in pixels to configure for LTDC layer 'LayerIdx'.
  * @param  LayerIdx           LTDC layer index concerned by the modification of line pitch.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetPitch(LTDC_HandleTypeDef *hltdc, uint32_t LinePitchInPixels, uint32_t LayerIdx)
{
  uint32_t tmp;
  uint32_t pitchUpdate;
  uint32_t pixelFormat;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* get LayerIdx used pixel format */
  pixelFormat = hltdc->LayerCfg[LayerIdx].PixelFormat;

  if (pixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    tmp = 4U;
  }
  else if (pixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    tmp = 3U;
  }
  else if ((pixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
           (pixelFormat == LTDC_PIXEL_FORMAT_RGB565)   || \
           (pixelFormat == LTDC_PIXEL_FORMAT_ARGB1555) || \
           (pixelFormat == LTDC_PIXEL_FORMAT_AL88))
  {
    tmp = 2U;
  }
  else
  {
    tmp = 1U;
  }

  pitchUpdate = ((LinePitchInPixels * tmp) << 16U);

  /* Clear previously set standard pitch */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR &= ~LTDC_LxCFBLR_CFBP;

  /* Set the Reload type as immediate update of LTDC pitch configured above */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Set new line pitch value */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR |= pitchUpdate;

  /* Set the Reload type as immediate update of LTDC pitch configured above */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Sets the expected CRC value for the LTDC peripheral.
  *
  * Programs the expected Cyclic Redundancy Check (CRC) value for comparison against the CRC
  * generated by the LTDC peripheral. This can be used for data integrity verification during
  * runtime.
  *
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param ExpectedCRC The expected CRC value to be set for comparison. This is a 16-bit value.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetExpectedCRC(LTDC_HandleTypeDef *hltdc, uint16_t ExpectedCRC)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Set the expected crc value */
  hltdc->Instance->ECRCR = ExpectedCRC;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Retrieves the computed CRC value from the LTDC peripheral.
  *
  * Obtains the CRC value computed by the LTDC peripheral for the current frame. This value can be
  * used to verify the integrity of the data processed by the LTDC against a known CRC value.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration
  *              information for the LTDC module.
  * @param ComputedCRC Pointer to a uint16_t variable where the computed CRC value will be stored.
  * @note  To get the computed CRC for the current frame (N), this function should be called at the start
  *        of the first line of the next frame (N+1).
  *        The CRC value will remain stable until frame (N+1) is fully displayed.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_GetComputedCRC(LTDC_HandleTypeDef *hltdc, uint16_t *pComputedCRC)
{
  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get the computed crc value */
  *pComputedCRC = (uint16_t) hltdc->Instance->CCRCR;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Define the position of the line interrupt.
  * @param  hltdc   pointer to a LTDC_HandleTypeDef structure that contains
  *                 the configuration information for the LTDC.
  * @param  Line    Line Interrupt Position.
  * @note   User application may resort to HAL_LTDC_LineEventCallback() at line interrupt generation.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ProgramLineEvent(LTDC_HandleTypeDef *hltdc, uint32_t Line)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LIPOS(Line));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Disable the Line interrupt */
  __HAL_LTDC_DISABLE_IT(hltdc, LTDC_IT_LI);

  /* Set the Line Interrupt position */
  LTDC->LIPCR = (uint32_t)Line;

  /* Enable the Line interrupt */
  __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_LI);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Reload LTDC Layers configuration.
  * @param  hltdc      pointer to a LTDC_HandleTypeDef structure that contains
  *                    the configuration information for the LTDC.
  * @param  ReloadType This parameter can be one of the following values :
  *                      LTDC_RELOAD_IMMEDIATE : Immediate Reload
  *                      LTDC_RELOAD_VERTICAL_BLANKING  : Reload in the next Vertical Blanking
  * @note   User application may resort to HAL_LTDC_ReloadEventCallback() at reload interrupt generation.
  * @retval  HAL status
  */
HAL_StatusTypeDef  HAL_LTDC_Reload(LTDC_HandleTypeDef *hltdc, uint32_t ReloadType)
{
  /* Check the parameters */
  assert_param(IS_LTDC_RELOAD(ReloadType));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable the Reload interrupt */
  __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_RR);

  /* Apply Reload type */
  hltdc->Instance->SRCR = ReloadType;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Reloads configuration for a specific layer of the LTDC peripheral.
  *
  * This function triggers a reload of the layer configuration for the LTDC peripheral. The type of
  * reload operation can be immediate or vertical blanking, as specified by the ReloadType parameter.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration
  *              information for the LTDC module.
  * @param ReloadType Specifies the type of reload operation. This parameter can be one of the
  *                   following values:
  *                   - LTDC_RELOAD_IMMEDIATE: Perform an immediate reload.
  *                   - LTDC_RELOAD_VERTICAL_BLANKING: Perform the reload during the vertical blanking
  *                     period.
  * @param LayerIdx Specifies the index of the layer to be reloaded. This parameter can be one of the
  *                 following values:
  *                 - LTDC_LAYER_1: Reload configuration for layer 1.
  *                 - LTDC_LAYER_2: Reload configuration for layer 2.
  * @note   User application may resort to HAL_LTDC_ReloadEventCallback() at reload interrupt generation.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ReloadLayer(LTDC_HandleTypeDef *hltdc, uint32_t ReloadType, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_RELOAD(ReloadType));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable the Reload interrupt */
  __HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_RR);

  /* Apply Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, ReloadType | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Sets the display output format for the LTDC peripheral.
  *
  * Configures the LTDC peripheral to output in a specified format. This function allows for the
  * selection of different color encoding formats, including RGB and YUV/YVU with specific conversion
  * standards.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration
  *              information for the LTDC module.
  * @param Display Specifies the display output format to be selected. This parameter can be one of
  *                the following values:
  *                - LTDC_OUT_RGB: Output in RGB format.
  *                - LTDC_OUT_YUV_HDTV: Output in YUV format using BT.709 conversion (HDTV standard).
  *                - LTDC_OUT_YUV_SDTV: Output in YUV format using BT.601 conversion (SDTV standard).
  *                - LTDC_OUT_YVU_HDTV: Output in YVU format using BT.709 conversion (HDTV standard).
  *                - LTDC_OUT_YVU_SDTV: Output in YVU format using BT.601 conversion (SDTV standard).
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetOutputDisplay(LTDC_HandleTypeDef *hltdc, uint32_t Display)
{
  /* Check the parameters */
  assert_param(IS_LTDC_DISPLAY(Display));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Configure the output format */
  hltdc->Instance->EDCR = Display;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Configure the LTDC Layer according to the specified without reloading
  *         parameters in the LTDC_InitTypeDef and create the associated handle.
  *         Variant of the function HAL_LTDC_ConfigLayer without immediate reload.
  * @param  hltdc      pointer to a LTDC_HandleTypeDef structure that contains
  *                    the configuration information for the LTDC.
  * @param  pLayerCfg  pointer to a LTDC_LayerCfgTypeDef structure that contains
  *                    the configuration information for the Layer.
  * @param  LayerIdx   LTDC Layer index.
  *                    This parameter can be one of the following values:
  *                    LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigLayer_NoReload(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg,
                                                uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_HCONFIGST(pLayerCfg->WindowX0));
  assert_param(IS_LTDC_HCONFIGSP(pLayerCfg->WindowX1));
  assert_param(IS_LTDC_VCONFIGST(pLayerCfg->WindowY0));
  assert_param(IS_LTDC_VCONFIGSP(pLayerCfg->WindowY1));
  assert_param(IS_LTDC_PIXEL_FORMAT(pLayerCfg->PixelFormat));
  assert_param(IS_LTDC_ALPHA(pLayerCfg->Alpha));
  assert_param(IS_LTDC_ALPHA(pLayerCfg->Alpha0));
  assert_param(IS_LTDC_BLENDING_FACTOR1(pLayerCfg->BlendingFactor1));
  assert_param(IS_LTDC_BLENDING_FACTOR2(pLayerCfg->BlendingFactor2));
  assert_param(IS_LTDC_CFBLL(pLayerCfg->ImageWidth));
  assert_param(IS_LTDC_CFBLNBR(pLayerCfg->ImageHeight));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Copy new layer configuration into handle structure */
  hltdc->LayerCfg[LayerIdx] = *pLayerCfg;

  /* Configure Predefined format */
  LTDC_SetPredefFormat(hltdc, LayerIdx);

  /* Configure composition and blending*/
  LTDC_SetCompositionConfig(hltdc, LayerIdx);

  /* Configure the LTDC Layer */
  LTDC_SetConfig(hltdc, 0U, 0U, LTDC_MIRROR_NONE, LayerIdx);

  /* Initialize the LTDC state*/
  hltdc->State  = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Set the LTDC window size without reloading.
  *         Variant of the function HAL_LTDC_SetWindowSize without immediate reload.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  XSize     LTDC Pixel per line
  * @param  YSize     LTDC Line number
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetWindowSize_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t XSize, uint32_t YSize,
                                                  uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters (Layers parameters)*/
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_CFBLL(XSize));
  assert_param(IS_LTDC_CFBLNBR(YSize));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* update horizontal stop */
  hltdc->LayerCfg[LayerIdx].WindowX1 = XSize + hltdc->LayerCfg[LayerIdx].WindowX0;

  /* update vertical stop */
  hltdc->LayerCfg[LayerIdx].WindowY1 = YSize + hltdc->LayerCfg[LayerIdx].WindowY0;

  /* Reconfigures the color frame buffer pitch in byte */
  hltdc->LayerCfg[LayerIdx].ImageWidth = XSize;

  /* Reconfigures the frame buffer line number */
  hltdc->LayerCfg[LayerIdx].ImageHeight = YSize;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, aux0Addr, aux1Addr, mirror, LayerIdx);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Set the LTDC window position without reloading.
  *         Variant of the function HAL_LTDC_SetWindowPosition without immediate reload.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  X0        LTDC window X offset
  * @param  Y0        LTDC window Y offset
  * @param  LayerIdx  LTDC Layer index.
  *                         This parameter can be one of the following values:
  *                         LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetWindowPosition_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t X0, uint32_t Y0,
                                                      uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_CFBLL(X0));
  assert_param(IS_LTDC_CFBLNBR(Y0));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* update horizontal start/stop */
  hltdc->LayerCfg[LayerIdx].WindowX0 = X0;
  hltdc->LayerCfg[LayerIdx].WindowX1 = X0 + hltdc->LayerCfg[LayerIdx].ImageWidth;

  /* update vertical start/stop */
  hltdc->LayerCfg[LayerIdx].WindowY0 = Y0;
  hltdc->LayerCfg[LayerIdx].WindowY1 = Y0 + hltdc->LayerCfg[LayerIdx].ImageHeight;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, aux0Addr, aux1Addr, mirror, LayerIdx);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Reconfigure the pixel format without reloading.
  *         Variant of the function HAL_LTDC_SetPixelFormat without immediate reload.
  * @param  hltdc        pointer to a LTDC_HandleTypeDfef structure that contains
  *                      the configuration information for the LTDC.
  * @param  Pixelformat  new pixel format value.
  * @param  LayerIdx     LTDC Layer index.
  *                      This parameter can be one of the following values:
  *                      LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetPixelFormat_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t Pixelformat, uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_PIXEL_FORMAT(Pixelformat));
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* Reconfigure the pixel format */
  hltdc->LayerCfg[LayerIdx].PixelFormat = Pixelformat;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, aux0Addr, aux1Addr, mirror, LayerIdx);

  /* Set LTDC format */
  LTDC_SetPredefFormat(hltdc, LayerIdx);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Reconfigure the layer alpha value without reloading.
  *         Variant of the function HAL_LTDC_SetAlpha without immediate reload.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Alpha     new alpha value.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetAlpha_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t Alpha, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_ALPHA(Alpha));
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Specifies the constant alpha value */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->CACR, Alpha);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Reconfigure the frame buffer Address without reloading.
  *         Variant of the function HAL_LTDC_SetAddress without immediate reload.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Address   new address value.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetAddress_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t Address, uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* Reconfigure the Address */
  hltdc->LayerCfg[LayerIdx].FBStartAdress = Address;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, 0, 0, mirror, LayerIdx);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Function used to reconfigure the pitch for specific cases where the attached LayerIdx buffer have a width
  *         that is larger than the one intended to be displayed on screen. Example of a buffer 800x480 attached to
  *         layer for which we want to read and display on screen only a portion 320x240 taken in the center
  *         of the buffer.
  *         The pitch in pixels will be in that case 800 pixels and not 320 pixels as initially configured by
  *         previous call to HAL_LTDC_ConfigLayer().
  * @note   This function should be called only after a previous call to HAL_LTDC_ConfigLayer() to modify the default
  *         pitch configured by HAL_LTDC_ConfigLayer() when required (refer to example described just above).
  *         Variant of the function HAL_LTDC_SetPitch without immediate reload.
  * @param  hltdc              pointer to a LTDC_HandleTypeDef structure that contains
  *                            the configuration information for the LTDC.
  * @param  LinePitchInPixels  New line pitch in pixels to configure for LTDC layer 'LayerIdx'.
  * @param  LayerIdx           LTDC layer index concerned by the modification of line pitch.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetPitch_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LinePitchInPixels, uint32_t LayerIdx)
{
  uint32_t tmp;
  uint32_t pitchUpdate;
  uint32_t pixelFormat;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* get LayerIdx used pixel format */
  pixelFormat = hltdc->LayerCfg[LayerIdx].PixelFormat;

  if (pixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    tmp = 4U;
  }
  else if (pixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    tmp = 3U;
  }
  else if ((pixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
           (pixelFormat == LTDC_PIXEL_FORMAT_RGB565)   || \
           (pixelFormat == LTDC_PIXEL_FORMAT_ARGB1555) || \
           (pixelFormat == LTDC_PIXEL_FORMAT_AL88))
  {
    tmp = 2U;
  }
  else
  {
    tmp = 1U;
  }

  pitchUpdate = ((LinePitchInPixels * tmp) << 16U);

  /* Clear previously set standard pitch */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR &= ~LTDC_LxCFBLR_CFBP;

  /* Set new line pitch value */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR |= pitchUpdate;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Enable the color keying without reloading.
  *         Variant of the function HAL_LTDC_EnableColorKeying without immediate reload.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_EnableColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable LTDC color keying by setting COLKEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR |= (uint32_t)LTDC_LxCR_CKEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Disable the color keying without reloading.
  *         Variant of the function HAL_LTDC_DisableColorKeying without immediate reload.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_DisableColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Disable LTDC color keying by setting COLKEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(uint32_t)LTDC_LxCR_CKEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Enable the color lookup table without reloading.
  *         Variant of the function HAL_LTDC_EnableCLUT without immediate reload.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_EnableCLUT_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Disable LTDC color lookup table by setting CLUTEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR |= (uint32_t)LTDC_LxCR_CLUTEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Disable the color lookup table without reloading.
  *         Variant of the function HAL_LTDC_DisableCLUT without immediate reload.
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_DisableCLUT_NoReload(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Disable LTDC color lookup table by setting CLUTEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(uint32_t)LTDC_LxCR_CLUTEN;

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup LTDC_Exported_Functions_Group4 Peripheral State and Errors functions
  *  @brief    Peripheral State and Errors functions
  *
@verbatim
 ===============================================================================
                  ##### Peripheral State and Errors functions #####
 ===============================================================================
    [..]
    This subsection provides functions allowing to
      (+) Check the LTDC handle state.
      (+) Get the LTDC handle error code.

@endverbatim
  * @{
  */

/**
  * @brief  Return the LTDC handle state.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval HAL state
  */
HAL_LTDC_StateTypeDef HAL_LTDC_GetState(const LTDC_HandleTypeDef *hltdc)
{
  return hltdc->State;
}

/**
  * @brief  Return the LTDC handle error code.
  * @param  hltdc  pointer to a LTDC_HandleTypeDef structure that contains
  *               the configuration information for the LTDC.
  * @retval LTDC Error Code
  */
uint32_t HAL_LTDC_GetError(const LTDC_HandleTypeDef *hltdc)
{
  return hltdc->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup LTDC_Private_Functions LTDC Private Functions
  * @{
  */


/**
  * @brief Reconfigure the full planar memory addresses for a YUV layer in the LTDC peripheral.
  *
  * Configures the memory addresses for the Y, U, and V planes of a full planar YUV layer. This function
  * is used when the Layer is operating in a full planar mode, where the Y, U, and V components are stored in
  * separate memory areas. It allows for dynamic updating of the frame buffer addresses.
  *
  * @note This function is applicable only to LTDC_LAYER_1.
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param pYUVFullPlanarAddress Pointer to a LTDC_LayerFlexYUVFullPlanarTypeDef structure that holds
  *                              the memory addresses for the Y, U, and V planes.
  * @param  LayerIdx  Specifies the index of the layer being configured. For this function, the only
  *                 valid value is LTDC_LAYER_1.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetFullPlanarAddress(LTDC_HandleTypeDef *hltdc,
                                                LTDC_LayerFlexYUVFullPlanarTypeDef *pYUVFullPlanarAddress,
                                                uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_PLANAR_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* Update LayerCfg structure with required parameters */
  hltdc->LayerCfg[LayerIdx].FBStartAdress = pYUVFullPlanarAddress->YUVFullPlanarAddress.YAddress;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pYUVFullPlanarAddress->YUVFullPlanarAddress.UAddress,
                 pYUVFullPlanarAddress->YUVFullPlanarAddress.VAddress, mirror, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Reconfigure the semi-planar memory addresses for a YUV layer in the LTDC peripheral.
  *
  * Configures the memory addresses for the Y plane and the combined U/V plane of a semi-planar YUV layer.
  * This function is used when the Layer is operating in a semi-planar mode, where the Y component and the
  * U/V components are stored in separate memory areas, allowing dynamic updates of the frame buffer addresses.
  *
  * @note This function is applicable only to LTDC_LAYER_1.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param pYUVSemiPlanarAddress Pointer to an LTDC_LayerFlexYUVSemiPlanarTypeDef structure that holds
  *                              the memory addresses for the Y plane and the combined U/V plane.
  * @param LayerIdx Specifies the index of the layer being configured. For this function, the only
  *                 valid value is LTDC_LAYER_1.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetSemiPlanarAddress(LTDC_HandleTypeDef *hltdc,
                                                LTDC_LayerFlexYUVSemiPlanarTypeDef *pYUVSemiPlanarAddress,
                                                uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_PLANAR_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* Update handle */
  hltdc->LayerCfg[LayerIdx].FBStartAdress = pYUVSemiPlanarAddress->YUVSemiPlanarAddress.YAddress;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pYUVSemiPlanarAddress->YUVSemiPlanarAddress.UVAddress, 0, mirror, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Reconfigure the full planar memory addresses for a YUV layer in the LTDC peripheral with no reload.
  *
  * Configures the memory addresses for the Y, U, and V planes of a full planar YUV layer. This function
  * is used when the Layer is operating in a full planar mode, where the Y, U, and V components are stored in
  * separate memory areas. It allows for dynamic updating of the frame buffer addresses.
  *
  * @note This function is applicable only to LTDC_LAYER_1.
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param pYUVFullPlanarAddress Pointer to a LTDC_LayerFlexYUVFullPlanarTypeDef structure that holds
  *                              the memory addresses for the Y, U, and V planes.
  * @param  LayerIdx  Specifies the index of the layer being configured. For this function, the only
  *                 valid value is LTDC_LAYER_1.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetFullPlanarAddress_NoReload(LTDC_HandleTypeDef *hltdc,
                                                         LTDC_LayerFlexYUVFullPlanarTypeDef *pYUVFullPlanarAddress,
                                                         uint32_t LayerIdx)
{
  uint32_t mirror = 0U;
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_PLANAR_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* Update LayerCfg structure with required parameters */
  hltdc->LayerCfg[LayerIdx].FBStartAdress = pYUVFullPlanarAddress->YUVFullPlanarAddress.YAddress;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pYUVFullPlanarAddress->YUVFullPlanarAddress.UAddress,
                 pYUVFullPlanarAddress->YUVFullPlanarAddress.VAddress, mirror, LayerIdx);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Reconfigure the semi-planar memory addresses for a YUV layer in the LTDC peripheral without reload.
  *
  * Configures the memory addresses for the Y plane and the combined U/V plane of a semi-planar YUV layer.
  * This function is used when the Layer is operating in a semi-planar mode, where the Y component and the
  * U/V components are stored in separate memory areas, allowing dynamic updates of the frame buffer addresses.
  *
  * @note This function is applicable only to LTDC_LAYER_1.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param pYUVSemiPlanarAddress Pointer to an LTDC_LayerFlexYUVSemiPlanarTypeDef structure that holds
  *                              the memory addresses for the Y plane and the combined U/V plane.
  * @param LayerIdx Specifies the index of the layer being configured. For this function, the only
  *                 valid value is LTDC_LAYER_1.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_SetSemiPlanarAddress_NoReload(LTDC_HandleTypeDef *hltdc,
                                                         LTDC_LayerFlexYUVSemiPlanarTypeDef *pYUVSemiPlanarAddress,
                                                         uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_PLANAR_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* Update handle */
  hltdc->LayerCfg[LayerIdx].FBStartAdress = pYUVSemiPlanarAddress->YUVSemiPlanarAddress.YAddress;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pYUVSemiPlanarAddress->YUVSemiPlanarAddress.UVAddress, 0, mirror, LayerIdx);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Configures the mirroring effect for a specific layer in the LTDC peripheral.
  *
  * This function sets the mirroring effect for the specified layer of the LTDC. It can configure the
  * layer to mirror the image horizontally, vertically, both horizontally and vertically, or not at all.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param Mirror Specifies the mirroring effect to be applied. This parameter can be one of the
  *               following values:
  *               - LTDC_MIRROR_HORIZONTAL: Apply horizontal mirroring to the layer.
  *               - LTDC_MIRROR_VERTICAL: Apply vertical mirroring to the layer.
  *               - LTDC_MIRROR_HORIZONTAL_VERTICAL: Apply both horizontal and vertical mirroring to
  *                 the layer.
  *               - LTDC_MIRROR_NONE: No mirroring is applied to the layer.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigMirror(LTDC_HandleTypeDef *hltdc, uint32_t Mirror, uint32_t LayerIdx)
{
  uint32_t aux0Addr = 0U;
  uint32_t aux1Addr = 0U;
  uint32_t mirror = 0U;

  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Get previous user configuration */
  LTDC_RetrieveUserConfig(hltdc, &mirror, &aux0Addr, &aux1Addr, LayerIdx);

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, aux0Addr, aux1Addr, Mirror, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Configures the layer parameters for ARGB format in the LTDC peripheral.
  *
  * This function sets up the layer-specific parameters for a layer that will handle ARGB pixel data.
  * It configures various aspects such as pixel format, blending factors, color frame buffer address,
  * color keying, and more, according to the LTDC_LayerFlexARGBTypeDef structure.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param pLayerFlexARGB Pointer to an LTDC_LayerFlexARGBTypeDef structure that specifies the layer
  *                       configuration parameters for the ARGB format.
  * @param LayerIdx Specifies the index of the layer being configured. This parameter can be one of the
  *                 following values:
  *                 - LTDC_LAYER_1: Configuration for layer 1.
  *                 - LTDC_LAYER_2: Configuration for layer 2.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigLayerFlexARGB(LTDC_HandleTypeDef *hltdc,
                                               const LTDC_LayerFlexARGBTypeDef *pLayerFlexARGB, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_ARGB_COMPONENT_WIDTH(pLayerFlexARGB->FlexARGB.RedWidth));
  assert_param(IS_LTDC_ARGB_COMPONENT_WIDTH(pLayerFlexARGB->FlexARGB.GreenWidth));
  assert_param(IS_LTDC_ARGB_COMPONENT_WIDTH(pLayerFlexARGB->FlexARGB.BlueWidth));
  assert_param(IS_LTDC_ARGB_COMPONENT_POSITION(pLayerFlexARGB->FlexARGB.RedPos));
  assert_param(IS_LTDC_ARGB_COMPONENT_POSITION(pLayerFlexARGB->FlexARGB.GreenPos));
  assert_param(IS_LTDC_ARGB_COMPONENT_POSITION(pLayerFlexARGB->FlexARGB.BluePos));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* update in the handler */
  hltdc->LayerCfg[LayerIdx].WindowX0 = pLayerFlexARGB->Layer.WindowX0;
  hltdc->LayerCfg[LayerIdx].WindowX1 = pLayerFlexARGB->Layer.WindowX1;
  hltdc->LayerCfg[LayerIdx].WindowY0 = pLayerFlexARGB->Layer.WindowY0;
  hltdc->LayerCfg[LayerIdx].WindowY1 = pLayerFlexARGB->Layer.WindowY1;
  hltdc->LayerCfg[LayerIdx].PixelFormat = LTDC_PIXEL_FORMAT_FLEX_ARGB;
  hltdc->LayerCfg[LayerIdx].Alpha = pLayerFlexARGB->Layer.Alpha;
  hltdc->LayerCfg[LayerIdx].Alpha0 = pLayerFlexARGB->Layer.Alpha0;
  hltdc->LayerCfg[LayerIdx].BlendingFactor1 = pLayerFlexARGB->Layer.BlendingFactor1;
  hltdc->LayerCfg[LayerIdx].BlendingFactor2 = pLayerFlexARGB->Layer.BlendingFactor2;
  hltdc->LayerCfg[LayerIdx].FBStartAdress = pLayerFlexARGB->ARGBAddress;
  hltdc->LayerCfg[LayerIdx].ImageWidth = pLayerFlexARGB->Layer.ImageWidth;
  hltdc->LayerCfg[LayerIdx].ImageHeight = pLayerFlexARGB->Layer.ImageHeight;

  /* Configure Flexible ARGB */
  LTDC_LAYER(hltdc, LayerIdx)->PFCR = 0x7U;
  LTDC_LAYER(hltdc, LayerIdx)->FPF0R = (pLayerFlexARGB->FlexARGB.RedWidth << LTDC_LxFPF0R_RLEN_Pos) |
                                       (pLayerFlexARGB->FlexARGB.RedPos << LTDC_LxFPF0R_RPOS_Pos) |
                                       (pLayerFlexARGB->FlexARGB.AlphaWidth << LTDC_LxFPF0R_ALEN_Pos) |
                                       pLayerFlexARGB->FlexARGB.AlphaPos;
  LTDC_LAYER(hltdc, LayerIdx)->FPF1R = (pLayerFlexARGB->FlexARGB.PixelSize << LTDC_LxFPF1R_PSIZE_Pos) |
                                       (pLayerFlexARGB->FlexARGB.BlueWidth << LTDC_LxFPF1R_BLEN_Pos) |
                                       (pLayerFlexARGB->FlexARGB.BluePos << LTDC_LxFPF1R_BPOS_Pos) |
                                       (pLayerFlexARGB->FlexARGB.GreenWidth << LTDC_LxFPF1R_GLEN_Pos) |
                                       (pLayerFlexARGB->FlexARGB.GreenPos);

  /* Set composition parameters */
  LTDC_SetCompositionConfig(hltdc, LayerIdx);


  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);
  /* Disable YUV mode */
  MODIFY_REG(LTDC_LAYER(hltdc, LayerIdx)->PCR, LTDC_LxPCR_YCEN, 0U);

  LTDC_SetConfig(hltdc, 0, 0, LTDC_MIRROR_NONE, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Configures the layer parameters for YUV co-planar format in the LTDC peripheral.
  *
  * This function sets up the layer-specific parameters for a layer that will handle YUV co-planar pixel
  * data. It allows configuration of the layer to handle pixel data where the Y component is stored
  * separately from the UV components, which are stored together. The configuration parameters include
  * pixel format, blending factors, color frame buffer addresses, and more, as defined in the
  * LTDC_LayerFlexYUVCoPlanarTypeDef structure.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param pLayerFlexYUVCoPlanar Pointer to an LTDC_LayerFlexYUVCoPlanarTypeDef structure that
  *                              specifies the layer configuration parameters for the YUV co-planar format.
  * @param LayerIdx Specifies the index of the layer being configured. This parameter can be one of the
  *                 following values:
  *                 - LTDC_LAYER_1: Configuration for layer 1.
  *                 - LTDC_LAYER_2: Configuration for layer 2.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigLayerFlexYUVCoPlanar(LTDC_HandleTypeDef *hltdc,
                                                      const LTDC_LayerFlexYUVCoPlanarTypeDef *pLayerFlexYUVCoPlanar,
                                                      uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_Y_RANHGE(pLayerFlexYUVCoPlanar->FlexYUV.LuminanceRescale));
  assert_param(IS_LTDC_Y_ORDER(pLayerFlexYUVCoPlanar->FlexYUV.LuminanceOrder));
  assert_param(IS_LTDC_YUV_ORDER(pLayerFlexYUVCoPlanar->FlexYUV.YUVOrder));
  assert_param(IS_LTDC_UV_ORDER(pLayerFlexYUVCoPlanar->FlexYUV.ChrominanceOrder));
  assert_param(IS_LTDC_YUV2RGBCONVERTOR(pLayerFlexYUVCoPlanar->ColorConverter));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* update the handler */
  hltdc->LayerCfg[LayerIdx].WindowX0 = pLayerFlexYUVCoPlanar->Layer.WindowX0;
  hltdc->LayerCfg[LayerIdx].WindowX1 = pLayerFlexYUVCoPlanar->Layer.WindowX1;
  hltdc->LayerCfg[LayerIdx].WindowY0 = pLayerFlexYUVCoPlanar->Layer.WindowY0;
  hltdc->LayerCfg[LayerIdx].WindowY1 = pLayerFlexYUVCoPlanar->Layer.WindowY1;
  hltdc->LayerCfg[LayerIdx].PixelFormat = LTDC_PIXEL_FORMAT_FLEX_YUV_COPLANAR;
  hltdc->LayerCfg[LayerIdx].Alpha = pLayerFlexYUVCoPlanar->Layer.Alpha;
  hltdc->LayerCfg[LayerIdx].Alpha0 = pLayerFlexYUVCoPlanar->Layer.Alpha0;
  hltdc->LayerCfg[LayerIdx].BlendingFactor1 = pLayerFlexYUVCoPlanar->Layer.BlendingFactor1;
  hltdc->LayerCfg[LayerIdx].BlendingFactor2 = pLayerFlexYUVCoPlanar->Layer.BlendingFactor2;
  hltdc->LayerCfg[LayerIdx].FBStartAdress = pLayerFlexYUVCoPlanar->YUVAddress;
  hltdc->LayerCfg[LayerIdx].ImageWidth = pLayerFlexYUVCoPlanar->Layer.ImageWidth;
  hltdc->LayerCfg[LayerIdx].ImageHeight = pLayerFlexYUVCoPlanar->Layer.ImageHeight;

  /* Set composition parameters */
  LTDC_SetCompositionConfig(hltdc, LayerIdx);

  /* Set flexible YUV format */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->PCR, LTDC_LxPCR_YCEN | (pLayerFlexYUVCoPlanar->FlexYUV.ChrominanceOrder) |
            (LTDC_PIXEL_FORMAT_FLEX_YUV_COPLANAR >> LTDC_LxPCR_YCM_Pos) |
            (pLayerFlexYUVCoPlanar->FlexYUV.YUVOrder) |
            (pLayerFlexYUVCoPlanar->FlexYUV.LuminanceOrder) | pLayerFlexYUVCoPlanar->FlexYUV.LuminanceRescale);

  /* Set YUV to RGB conversion */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->CYR0R, \
            (U2B[pLayerFlexYUVCoPlanar->ColorConverter] << LTDC_LxCYR0R_CB2B_Pos) |
            V2R[pLayerFlexYUVCoPlanar->ColorConverter]);
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->CYR1R, (V2G[pLayerFlexYUVCoPlanar->ColorConverter]) |
            (U2G[pLayerFlexYUVCoPlanar->ColorConverter] << LTDC_LxCYR1R_CB2G_Pos));

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, 0, 0, LTDC_MIRROR_NONE, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Configures the layer parameters for YUV semi-planar format in the LTDC peripheral.
  *
  * This function sets up the layer-specific parameters for a layer that will handle YUV semi-planar pixel
  * data. It configures the layer to process pixel data where the Y component is stored separately from
  * the UV components, which are stored together in a semi-planar format. The configuration parameters
  * include pixel format, blending factors, color frame buffer addresses, and more, as defined in the
  * LTDC_LayerFlexYUVSemiPlanarTypeDef structure.
  *
  * @note This function is applicable only to LTDC_LAYER_1.
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param pLayerFlexYUVSemiPlanar Pointer to an LTDC_LayerFlexYUVSemiPlanarTypeDef structure that
  *                                specifies the layer configuration parameters for the YUV semi-planar
  *                                format.
  * @param LayerIdx Specifies the index of the layer being configured. For this function, the only
  *                 valid value is LTDC_LAYER_1.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigLayerFlexYUVSemiPlanar(LTDC_HandleTypeDef *hltdc,
                                                        LTDC_LayerFlexYUVSemiPlanarTypeDef *pLayerFlexYUVSemiPlanar,
                                                        uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_PLANAR_LAYER(LayerIdx));
  assert_param(IS_LTDC_Y_RANHGE(pLayerFlexYUVSemiPlanar->FlexYUV.LuminanceRescale));
  assert_param(IS_LTDC_Y_ORDER(pLayerFlexYUVSemiPlanar->FlexYUV.LuminanceOrder));
  assert_param(IS_LTDC_YUV_ORDER(pLayerFlexYUVSemiPlanar->FlexYUV.YUVOrder));
  assert_param(IS_LTDC_UV_ORDER(pLayerFlexYUVSemiPlanar->FlexYUV.ChrominanceOrder));
  assert_param(IS_LTDC_YUV2RGBCONVERTOR(pLayerFlexYUVSemiPlanar->ColorConverter));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* update the handler */
  hltdc->LayerCfg[LayerIdx].WindowX0 = pLayerFlexYUVSemiPlanar->Layer.WindowX0;
  hltdc->LayerCfg[LayerIdx].WindowX1 = pLayerFlexYUVSemiPlanar->Layer.WindowX1;
  hltdc->LayerCfg[LayerIdx].WindowY0 = pLayerFlexYUVSemiPlanar->Layer.WindowY0;
  hltdc->LayerCfg[LayerIdx].WindowY1 = pLayerFlexYUVSemiPlanar->Layer.WindowY1;
  hltdc->LayerCfg[LayerIdx].PixelFormat = LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR;
  hltdc->LayerCfg[LayerIdx].Alpha = pLayerFlexYUVSemiPlanar->Layer.Alpha;
  hltdc->LayerCfg[LayerIdx].Alpha0 = pLayerFlexYUVSemiPlanar->Layer.Alpha0;
  hltdc->LayerCfg[LayerIdx].BlendingFactor1 = pLayerFlexYUVSemiPlanar->Layer.BlendingFactor1;
  hltdc->LayerCfg[LayerIdx].BlendingFactor2 = pLayerFlexYUVSemiPlanar->Layer.BlendingFactor2;
  hltdc->LayerCfg[LayerIdx].FBStartAdress = pLayerFlexYUVSemiPlanar->YUVSemiPlanarAddress.YAddress;
  hltdc->LayerCfg[LayerIdx].ImageWidth = pLayerFlexYUVSemiPlanar->Layer.ImageWidth;
  hltdc->LayerCfg[LayerIdx].ImageHeight = pLayerFlexYUVSemiPlanar->Layer.ImageHeight;

  /* Set composition parameters */
  LTDC_SetCompositionConfig(hltdc, LayerIdx);

  /* Set flexible YUV format */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->PCR, LTDC_LxPCR_YCEN | pLayerFlexYUVSemiPlanar->FlexYUV.ChrominanceOrder |
            (LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR >> LTDC_LxPCR_YCM_Pos) |
            pLayerFlexYUVSemiPlanar->FlexYUV.YUVOrder |
            pLayerFlexYUVSemiPlanar->FlexYUV.LuminanceOrder | pLayerFlexYUVSemiPlanar->FlexYUV.LuminanceRescale);

  /* Set YUV to RGB conversion */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->CYR0R, \
            (U2B[pLayerFlexYUVSemiPlanar->ColorConverter] << LTDC_LxCYR0R_CB2B_Pos) |
            V2R[pLayerFlexYUVSemiPlanar->ColorConverter]);
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->CYR1R, (V2G[pLayerFlexYUVSemiPlanar->ColorConverter]) |
            (U2G[pLayerFlexYUVSemiPlanar->ColorConverter] << LTDC_LxCYR1R_CB2G_Pos));

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerFlexYUVSemiPlanar->YUVSemiPlanarAddress.UVAddress, 0, LTDC_MIRROR_NONE, LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Configures the layer parameters for YUV full-planar format in the LTDC peripheral.
  *
  * This function sets up the layer-specific parameters for a layer that will handle YUV full-planar pixel
  * data. It configures the layer to process pixel data where the Y component is stored separately from
  * the UV components, which are stored together in a full-planar format. The configuration parameters
  * include pixel format, blending factors, color frame buffer addresses, and more, as defined in the
  * LTDC_LayerFlexYUVfullPlanarTypeDef structure.
  *
  *  @note This function is applicable only to LTDC_LAYER_1.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param pLayerFlexYUVfullPlanar Pointer to an LTDC_LayerFlexYUVfullPlanarTypeDef structure that
  *                                specifies the layer configuration parameters for the YUV full-planar
  *                                format.
  * @param LayerIdx Specifies the index of the layer being configured. For this function, the only
  *                 valid value is LTDC_LAYER_1.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigLayerFlexYUVFullPlanar(LTDC_HandleTypeDef *hltdc,
                                                        LTDC_LayerFlexYUVFullPlanarTypeDef *pLayerFlexYUVFullPlanar,
                                                        uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_PLANAR_LAYER(LayerIdx));
  assert_param(IS_LTDC_Y_RANHGE(pLayerFlexYUVFullPlanar->FlexYUV.LuminanceRescale));
  assert_param(IS_LTDC_Y_ORDER(pLayerFlexYUVFullPlanar->FlexYUV.LuminanceOrder));
  assert_param(IS_LTDC_YUV_ORDER(pLayerFlexYUVFullPlanar->FlexYUV.YUVOrder));
  assert_param(IS_LTDC_UV_ORDER(pLayerFlexYUVFullPlanar->FlexYUV.ChrominanceOrder));
  assert_param(IS_LTDC_YUV2RGBCONVERTOR(pLayerFlexYUVFullPlanar->ColorConverter));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* update the handler */
  hltdc->LayerCfg[LayerIdx].WindowX0 = pLayerFlexYUVFullPlanar->Layer.WindowX0;
  hltdc->LayerCfg[LayerIdx].WindowX1 = pLayerFlexYUVFullPlanar->Layer.WindowX1;
  hltdc->LayerCfg[LayerIdx].WindowY0 = pLayerFlexYUVFullPlanar->Layer.WindowY0;
  hltdc->LayerCfg[LayerIdx].WindowY1 = pLayerFlexYUVFullPlanar->Layer.WindowY1;
  hltdc->LayerCfg[LayerIdx].PixelFormat = LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR;
  hltdc->LayerCfg[LayerIdx].Alpha = pLayerFlexYUVFullPlanar->Layer.Alpha;
  hltdc->LayerCfg[LayerIdx].Alpha0 = pLayerFlexYUVFullPlanar->Layer.Alpha0;
  hltdc->LayerCfg[LayerIdx].BlendingFactor1 = pLayerFlexYUVFullPlanar->Layer.BlendingFactor1;
  hltdc->LayerCfg[LayerIdx].BlendingFactor2 = pLayerFlexYUVFullPlanar->Layer.BlendingFactor2;
  hltdc->LayerCfg[LayerIdx].FBStartAdress = pLayerFlexYUVFullPlanar->YUVFullPlanarAddress.YAddress;
  hltdc->LayerCfg[LayerIdx].ImageWidth = pLayerFlexYUVFullPlanar->Layer.ImageWidth;
  hltdc->LayerCfg[LayerIdx].ImageHeight = pLayerFlexYUVFullPlanar->Layer.ImageHeight;

  /* Set composition parameters */
  LTDC_SetCompositionConfig(hltdc, LayerIdx);

  /* Set flexible YUV format */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->PCR, LTDC_LxPCR_YCEN |
            pLayerFlexYUVFullPlanar->FlexYUV.ChrominanceOrder |
            (LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR >> LTDC_LxPCR_YCM_Pos) |
            pLayerFlexYUVFullPlanar->FlexYUV.YUVOrder |
            pLayerFlexYUVFullPlanar->FlexYUV.LuminanceOrder | pLayerFlexYUVFullPlanar->FlexYUV.LuminanceRescale);

  /* Set YUV to RGB conversion */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->CYR0R, \
            (U2B[pLayerFlexYUVFullPlanar->ColorConverter] << LTDC_LxCYR0R_CB2B_Pos) |
            V2R[pLayerFlexYUVFullPlanar->ColorConverter]);
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->CYR1R, V2G[pLayerFlexYUVFullPlanar->ColorConverter] |
            (U2G[pLayerFlexYUVFullPlanar->ColorConverter] << LTDC_LxCYR1R_CB2G_Pos));

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerFlexYUVFullPlanar->YUVFullPlanarAddress.UAddress,
                 pLayerFlexYUVFullPlanar->YUVFullPlanarAddress.VAddress, LTDC_MIRROR_NONE,
                 LayerIdx);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Enable the default ARGB8888 color for a specific LTDC layer.
  *
  * Each layer within the LTDC module has a default color in the ARGB8888 format, which is
  * displayed outside the defined layer window or when the layer itself is disabled.
  * This function enable the display of default color for a given layer.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_EnableDefaultColor(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable Default Color Blending */
  SET_BIT(LTDC_LAYER(hltdc, LayerIdx)->CR, LTDC_LxCR_DCBEN);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief  Disable the default ARGB8888 color for a specific LTDC layer.
  *
  * Each layer within the LTDC module has a default color in the ARGB8888 format, which is
  * displayed outside the defined layer window or when the layer itself is disabled.
  * This function prevents the default color from being displayed when a  layer is disabled.
  *
  * @param  hltdc     pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1).
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_DisableDefaultColor(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Enable Default Color Blending */
  CLEAR_BIT(LTDC_LAYER(hltdc, LayerIdx)->CR, LTDC_LxCR_DCBEN);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}

/**
  * @brief Configure the blending order for a given layer.
  *
  * This function sets the blending order of the layers managed by the LTDC module.
  * The blending order determines how the layers are superimposed on each other.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains
  *              the configuration information for the LTDC module.
  * @param Order The blending order value. This parameter can be one of the following values:
  *              @arg LTDC_BLENDING_ORDER_FOREGROUND: Layer set in the foreground.
  *              @arg LTDC_BLENDING_ORDER_BACKGROUND: Layer set in the background.
  * @param LayerIdx Index of the layer being configured. This parameter can be one of the following:
  *                 @arg LTDC_LAYER_1: Layer 1 of the LTDC.
  *                 @arg LTDC_LAYER_2: Layer 2 of the LTDC.
  * @retval  HAL status
  */
HAL_StatusTypeDef HAL_LTDC_ConfigBlendingOrder(LTDC_HandleTypeDef *hltdc, uint32_t Order, uint32_t LayerIdx)
{
  /* Check the parameters */
  assert_param(IS_LTDC_LAYER(LayerIdx));
  assert_param(IS_LTDC_BLEND_ORDER(Order));

  /* Process locked */
  __HAL_LOCK(hltdc);

  /* Change LTDC peripheral state */
  hltdc->State = HAL_LTDC_STATE_BUSY;

  /* Set the blending order */
  MODIFY_REG(LTDC_LAYER(hltdc, LayerIdx)->BFCR, LTDC_LxBFCR_BOR, Order);

  /* Set the Immediate Reload type */
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->RCR, LTDC_LxRCR_IMR | LTDC_LxRCR_GRMSK);

  /* Change the LTDC state*/
  hltdc->State = HAL_LTDC_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hltdc);

  return HAL_OK;
}
/**
  * @brief Retrieves the user configuration for a specific layer in the LTDC peripheral.
  *
  * This static function retrieves the current configuration for the specified layer, including the
  * mirroring setup and auxiliary buffer addresses.  It extracts the stride based on the pixel format
  * and calculates the buffer addresses accordingly. The retrieved configuration includes the mirror
  * orientation and the starting addresses of the color frame buffer and auxiliary buffers.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param Mirror Pointer to a uint32_t variable where the mirror configuration will be stored. The
  *               value will be one of the following:
  *               - LTDC_MIRROR_NONE: No mirroring.
  *               - LTDC_MIRROR_HORIZONTAL: Horizontal mirroring.
  *               - LTDC_MIRROR_VERTICAL: Vertical mirroring.
  *               - LTDC_MIRROR_HORIZONTAL_VERTICAL: Horizontal and vertical mirroring.
  * @param Aux0Addr Pointer to a uint32_t variable where the address of the first auxiliary buffer will
  *                 be stored, if applicable.
  * @param Aux1Addr Pointer to a uint32_t variable where the address of the second auxiliary buffer will
  *                 be stored, if applicable.
  * @param LayerIdx Specifies the index of the layer being queried. This parameter can be one of the
  *                 following values:
  *                 - LTDC_LAYER_1: Retrieve configuration for layer 1.
  *                 - LTDC_LAYER_2: Retrieve configuration for layer 2.
  *                 - Other layer indices as defined by the hardware and used within the driver.
  *
  * @note This function is intended for internal use within the LTDC driver and does not return a value.
  */
static void LTDC_RetrieveUserConfig(LTDC_HandleTypeDef *hltdc, uint32_t *Mirror, uint32_t *Aux0Addr,
                                    uint32_t *Aux1Addr, uint32_t LayerIdx)
{
  uint32_t stride;
  const uint32_t hmirror = ((LTDC_LAYER(hltdc, LayerIdx)->CR) & LTDC_LxCR_HMEN_Msk);
  const uint32_t pitchSign = (LTDC_LAYER(hltdc, LayerIdx)->CFBLR & LTDC_LxCFBLR_CFBP) & LTDC_PITCH_SIGN_MSK;

  switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:
    case LTDC_PIXEL_FORMAT_BGRA8888:
    case LTDC_PIXEL_FORMAT_ABGR8888:
    case LTDC_PIXEL_FORMAT_RGBA8888:
      stride = 4U;
      break;
    case LTDC_PIXEL_FORMAT_RGB888:
      stride = 3U;
      break;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_BGR565:
    case LTDC_PIXEL_FORMAT_ARGB1555:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:
    case LTDC_PIXEL_FORMAT_FLEX_YUV_COPLANAR:
      stride = 2U;
      break;
    case LTDC_PIXEL_FORMAT_FLEX_ARGB:
      stride = ((LTDC_LAYER(hltdc, LayerIdx)->FPF1R) & LTDC_LxFPF1R_PSIZE_Msk) >> LTDC_LxFPF1R_PSIZE_Pos;
      break;
    case LTDC_PIXEL_FORMAT_L8:
    case LTDC_PIXEL_FORMAT_AL44:
    case LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR:
    default:
      stride = 1U;
      break;
  }

  if ((pitchSign == 0U) && (hmirror == 0U))
  {
    *Mirror = LTDC_MIRROR_NONE;
    hltdc->LayerCfg[LayerIdx].FBStartAdress = LTDC_LAYER(hltdc, LayerIdx)->CFBAR;
    *Aux0Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA0R;
    *Aux1Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA1R;
  }
  else if ((pitchSign == 0U) && (hmirror == LTDC_LxCR_HMEN))
  {
    *Mirror = LTDC_MIRROR_HORIZONTAL;
    hltdc->LayerCfg[LayerIdx].FBStartAdress = LTDC_LAYER(hltdc, LayerIdx)->CFBAR - \
                                              (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                                         hltdc->LayerCfg[LayerIdx].WindowX0)) + 1U;
    switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
    {
      case LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR:
        *Aux0Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA0R - \
                    (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 - \
                               hltdc->LayerCfg[LayerIdx].WindowX0)) + 1U;
        break;
      case LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR:
        *Aux0Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA0R - \
                    (stride * ((hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U)) + 1U;
        *Aux1Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA1R - \
                    (stride * ((hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U)) + 1U;
        break;
      default:
        /* Nothing to do */
        break;
    }

  }
  else if ((pitchSign != 0U) && (hmirror == 0U))
  {
    *Mirror = LTDC_MIRROR_VERTICAL;
    hltdc->LayerCfg[LayerIdx].FBStartAdress = LTDC_LAYER(hltdc, LayerIdx)->CFBAR - \
                                              (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                                         hltdc->LayerCfg[LayerIdx].WindowX0) * \
                                               ((hltdc->LayerCfg[LayerIdx].WindowY1 - \
                                                 hltdc->LayerCfg[LayerIdx].WindowY0) - 1U));
    switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
    {
      case LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR:
        *Aux0Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA0R - (stride * \
                                                           (hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                                            hltdc->LayerCfg[LayerIdx].WindowX0) * \
                                                           (((hltdc->LayerCfg[LayerIdx].WindowY1 - \
                                                              hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U) - 1U));
        break;
      case LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR:
        *Aux0Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA0R - (stride * \
                                                           ((hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                                             hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) * \
                                                           (((hltdc->LayerCfg[LayerIdx].WindowY1 - \
                                                              hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U) - 1U));
        *Aux1Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA1R - (stride * \
                                                           ((hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                                             hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) * \
                                                           (((hltdc->LayerCfg[LayerIdx].WindowY1 - \
                                                              hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U) - 1U));
        break;
      default:
        /* Nothing to do */
        break;
    }
  }

  else
  {
    *Mirror = LTDC_MIRROR_HORIZONTAL_VERTICAL;
    hltdc->LayerCfg[LayerIdx].FBStartAdress = LTDC_LAYER(hltdc, LayerIdx)->CFBAR - \
                                              (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                                         hltdc->LayerCfg[LayerIdx].WindowX0) * \
                                               (hltdc->LayerCfg[LayerIdx].WindowY1 - \
                                                hltdc->LayerCfg[LayerIdx].WindowY0)) + 1U;
    switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
    {
      case LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR:
        *Aux0Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA0R - (stride * \
                                                           (hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                                            hltdc->LayerCfg[LayerIdx].WindowX0) * \
                                                           ((hltdc->LayerCfg[LayerIdx].WindowY1 - \
                                                             hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U)) + 1U;
        break;
      case LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR:
        *Aux0Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA0R - (stride * \
                                                           ((hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                                             hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) * \
                                                           ((hltdc->LayerCfg[LayerIdx].WindowY1 - \
                                                             hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U)) + 1U;
        *Aux1Addr = LTDC_LAYER(hltdc, LayerIdx)->AFBA1R - (stride * \
                                                           ((hltdc->LayerCfg[LayerIdx].WindowX1 - \
                                                             hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) * \
                                                           ((hltdc->LayerCfg[LayerIdx].WindowY1 - \
                                                             hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U)) + 1U;
        break;
      default:
        /* Nothing to do */
        break;
    }
  }
}

/**
  * @brief Set the configuration for a specific layer of the LTDC controller.
  *
  * This function configures a layer of the LTDC controller with the specified parameters.
  * It sets the pixel format stride, window positioning, color frame buffer address, auxiliary
  * frame buffer addresses, buffer length, and line number based on the layer configuration.
  * It also handles the mirroring configuration for the layer if specified.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains
  *              the configuration information for the specified LTDC.
  * @param Aux0Addr Auxiliary frame buffer address 0 used for certain pixel formats.
  * @param Aux1Addr Auxiliary frame buffer address 1 used for certain pixel formats.
  * @param Mirror Specifies the mirroring configuration for the layer. This parameter
  *               can be one of the following values:
  *               @arg LTDC_MIRROR_NONE: No mirroring.
  *               @arg LTDC_MIRROR_HORIZONTAL: Horizontal mirroring.
  *               @arg LTDC_MIRROR_VERTICAL: Vertical mirroring.
  *               @arg LTDC_MIRROR_HORIZONTAL_VERTICAL: Horizontal and vertical mirroring.
  * @param LayerIdx Index of the layer being configured. This parameter can be one of the following:
  *                 @arg LTDC_LAYER_1: Layer 1 of the LTDC.
  *                 @arg LTDC_LAYER_2: Layer 2 of the LTDC.
  *                 Depending on the LTDC hardware, additional layers may be supported.
  *
  * @note The stride is calculated based on the pixel format of the layer. The pixel format
  *       defines the number of bytes per pixel and hence the stride (the increment from one
  *       pixel to the next in a row of pixels). The function configures the layer window size
  *       and position, as well as the color and auxiliary frame buffer addresses, which are
  *       dependent on the mirroring configuration and pixel format. The buffer length and
  *       frame buffer line number are also set according to the pixel format and layer size.
  *       Mirroring settings are applied by adjusting the frame buffer start address and
  *       enabling the corresponding mirroring bits in the control register.
  *
  *       This function does not return a value as it is a static function used internally within the
  *        driver.
  */
static void LTDC_SetConfig(LTDC_HandleTypeDef *hltdc, uint32_t Aux0Addr, uint32_t Aux1Addr, uint32_t Mirror,
                           uint32_t LayerIdx)
{
  uint32_t stride;
  uint32_t tmp;

  /* Configure the frame buffer line number */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLNR  = (hltdc->LayerCfg[LayerIdx].ImageHeight);

  switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:
    case LTDC_PIXEL_FORMAT_BGRA8888:
    case LTDC_PIXEL_FORMAT_ABGR8888:
    case LTDC_PIXEL_FORMAT_RGBA8888:
      stride = 4U;
      break;
    case LTDC_PIXEL_FORMAT_RGB888:
      stride = 3U;
      break;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_BGR565:
    case LTDC_PIXEL_FORMAT_ARGB1555:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:
    case LTDC_PIXEL_FORMAT_FLEX_YUV_COPLANAR:
      stride = 2U;
      break;
    case LTDC_PIXEL_FORMAT_FLEX_ARGB:
      stride = (((LTDC_LAYER(hltdc, LayerIdx)->FPF1R) & LTDC_LxFPF1R_PSIZE_Msk) >> LTDC_LxFPF1R_PSIZE_Pos);
      break;
    case LTDC_PIXEL_FORMAT_L8:
    case LTDC_PIXEL_FORMAT_AL44:
    case LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR:
    default:
      stride = 1U;
      break;
  }

  /* Configure the horizontal start and stop position */
  tmp = ((hltdc->LayerCfg[LayerIdx].WindowX1 + ((hltdc->Instance->BPCR & LTDC_BPCR_AHBP) >> 16U)) << 16U);
  LTDC_LAYER(hltdc, LayerIdx)->WHPCR = ((hltdc->LayerCfg[LayerIdx].WindowX0 +
                                         ((hltdc->Instance->BPCR & LTDC_BPCR_AHBP) >> 16U) + 1U) | tmp);

  /* Configure the vertical start and stop position */
  tmp = ((hltdc->LayerCfg[LayerIdx].WindowY1 + (hltdc->Instance->BPCR & LTDC_BPCR_AVBP)) << 16U);
  LTDC_LAYER(hltdc, LayerIdx)->WVPCR = ((hltdc->LayerCfg[LayerIdx].WindowY0 +
                                         (hltdc->Instance->BPCR & LTDC_BPCR_AVBP) + 1U) | tmp);

  if (Mirror == LTDC_MIRROR_NONE)
  {
    LTDC_LAYER(hltdc, LayerIdx)->CFBAR = (hltdc->LayerCfg[LayerIdx].FBStartAdress);

    switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
    {
      case LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR:
        /* Configure the auxiliary frame buffer address 0 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA0R = Aux0Addr;

        /* Configure the buffer length */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLR = (hltdc->LayerCfg[LayerIdx].ImageWidth << 16U) |
                                             (hltdc->LayerCfg[LayerIdx].WindowX1 -
                                              hltdc->LayerCfg[LayerIdx].WindowX0 + 7U);
        /* Configure the frame buffer line number */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLNR = (hltdc->LayerCfg[LayerIdx].ImageHeight) >> 1U;
        break;
      case LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR:
        /* Configure the auxiliary frame buffer address 0 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA0R = Aux0Addr;

        /* Configure the auxiliary frame buffer address 1 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA1R = Aux1Addr;

        /* Configure the buffer length */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLR = ((hltdc->LayerCfg[LayerIdx].ImageWidth >> 1U) << 16U) |
                                             (((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) + 7U);

        /* Configure the frame buffer line number */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLNR = (hltdc->LayerCfg[LayerIdx].ImageHeight) >> 1U;
        break;
      default:
        /* Nothing to do */
        break;
    }

    /* Configure the color frame buffer pitch in byte */
    LTDC_LAYER(hltdc, LayerIdx)->CFBLR = (((hltdc->LayerCfg[LayerIdx].ImageWidth * stride) << 16U) |
                                          (((hltdc->LayerCfg[LayerIdx].WindowX1 - hltdc->LayerCfg[LayerIdx].WindowX0) *
                                            stride)  + 7U));

    /* Enable LTDC_Layer by setting LEN bit */
    MODIFY_REG(LTDC_LAYER(hltdc, LayerIdx)->CR,LTDC_LxCR_HMEN, LTDC_LxCR_LEN);
  }

  else if (Mirror == LTDC_MIRROR_HORIZONTAL)
  {
    /* Configure the color frame buffer start address */
    LTDC_LAYER(hltdc, LayerIdx)->CFBAR = hltdc->LayerCfg[LayerIdx].FBStartAdress +
                                         (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                    hltdc->LayerCfg[LayerIdx].WindowX0)) - 1U;

    switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
    {
      case LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR:
        /* Configure the auxiliary frame buffer address 0 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA0R = Aux0Addr +
                                              (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                         hltdc->LayerCfg[LayerIdx].WindowX0)) - 1U;

        /* Configure the buffer length */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLR = (hltdc->LayerCfg[LayerIdx].ImageWidth << 16U) |
                                             (hltdc->LayerCfg[LayerIdx].WindowX1 - hltdc->LayerCfg[LayerIdx].WindowX0 +
                                              7U);

        /* Configure the frame buffer line number */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLNR = hltdc->LayerCfg[LayerIdx].ImageHeight >> 1U ;
        break;
      case LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR:
        /* Configure the auxiliary frame buffer address 0 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA0R = Aux0Addr +
                                              (stride * ((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                          hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U)) - 1U;

        /* Configure the auxiliary frame buffer address 1 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA1R = Aux1Addr +
                                              (stride * ((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                          hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U)) - 1U;

        /* Configure the buffer length */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLR = ((hltdc->LayerCfg[LayerIdx].ImageWidth >> 1U) << 16U) |
                                             (((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) + 7U);

        /* Configure the frame buffer line number */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLNR = hltdc->LayerCfg[LayerIdx].ImageHeight >> 1U;
        break;
      default:
        /* Nothing to do */
        break;
    }

    /* Configure the color frame buffer pitch in byte */
    LTDC_LAYER(hltdc, LayerIdx)->CFBLR = (((hltdc->LayerCfg[LayerIdx].ImageWidth * stride) << 16U) |
                                          (((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                             hltdc->LayerCfg[LayerIdx].WindowX0) * stride)  + 7U));

    /* Enable horizontal mirroring bit & LTDC_Layer by setting LEN bit */
    SET_BIT(LTDC_LAYER(hltdc, LayerIdx)->CR, LTDC_LxCR_HMEN | LTDC_LxCR_LEN);
  }

  else if (Mirror == LTDC_MIRROR_VERTICAL)
  {
    /* Configure the color frame buffer start address */
    LTDC_LAYER(hltdc, LayerIdx)->CFBAR = hltdc->LayerCfg[LayerIdx].FBStartAdress +
                                         (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                    hltdc->LayerCfg[LayerIdx].WindowX0) *
                                          ((hltdc->LayerCfg[LayerIdx].WindowY1 -
                                            hltdc->LayerCfg[LayerIdx].WindowY0) - 1U));

    switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
    {
      case LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR:
        /* Configure the auxiliary frame buffer address 0 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA0R = Aux0Addr +
                                              (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                         hltdc->LayerCfg[LayerIdx].WindowX0) *
                                               (((hltdc->LayerCfg[LayerIdx].WindowY1 -
                                                  hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U) - 1U));

        /* Configure the buffer length */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLR = ((0x8000U - (hltdc->LayerCfg[LayerIdx].ImageWidth * stride)) << 16U) |
                                             (((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                hltdc->LayerCfg[LayerIdx].WindowX0) * stride)  + 7U);

        /* Configure the frame buffer line number */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLNR = hltdc->LayerCfg[LayerIdx].ImageHeight >> 1U;
        break;
      case LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR:
        /* Configure the auxiliary frame buffer address 0 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA0R = Aux0Addr +
                                              (stride * ((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                          hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) *
                                               (((hltdc->LayerCfg[LayerIdx].WindowY1 -
                                                  hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U) - 1U));

        /* Configure the auxiliary frame buffer address 1 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA1R = Aux1Addr +
                                              (stride * ((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                          hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) *
                                               (((hltdc->LayerCfg[LayerIdx].WindowY1 -
                                                  hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U) - 1U));

        /* Configure the buffer length */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLR = (((0x8000U - (hltdc->LayerCfg[LayerIdx].ImageWidth >> 1U)) *
                                               stride) << 16U) |
                                             ((((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                 hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) * stride) + 7U);

        /* Configure the frame buffer line number */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLNR = hltdc->LayerCfg[LayerIdx].ImageHeight >> 1U;
        break;
      default:
        /* Nothing to do */
        break;
    }

    /* set the pitch */
    LTDC_LAYER(hltdc, LayerIdx)->CFBLR  = ((((0x8000U - (hltdc->LayerCfg[LayerIdx].ImageWidth * stride))) << 16U) |
                                           (((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                              hltdc->LayerCfg[LayerIdx].WindowX0) * stride) + 7U));

    /* Enable LTDC_Layer by setting LEN bit */
    MODIFY_REG(LTDC_LAYER(hltdc, LayerIdx)->CR, LTDC_LxCR_HMEN, LTDC_LxCR_LEN);
  }

  else
    /*  Mirror = LTDC_MIRROR_HORIZONTAL_VERTICAL */
  {
    /* Configure the color frame buffer start address */
    LTDC_LAYER(hltdc, LayerIdx)->CFBAR = hltdc->LayerCfg[LayerIdx].FBStartAdress +
                                         (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                    hltdc->LayerCfg[LayerIdx].WindowX0) *
                                          (hltdc->LayerCfg[LayerIdx].WindowY1 - \
                                           hltdc->LayerCfg[LayerIdx].WindowY0)) - 1U;

    switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
    {
      case LTDC_PIXEL_FORMAT_FLEX_YUV_SEMIPLANAR:
        /* Configure the auxiliary frame buffer address 0 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA0R = Aux0Addr +
                                              (stride * (hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                         hltdc->LayerCfg[LayerIdx].WindowX0) *
                                               ((hltdc->LayerCfg[LayerIdx].WindowY1 -
                                                 hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U)) - 1U;

        /* Configure the buffer length */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLR = ((0x8000U - (hltdc->LayerCfg[LayerIdx].ImageWidth * stride)) << 16U) |
                                             (((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                hltdc->LayerCfg[LayerIdx].WindowX0) * stride) + 7U);

        /* Configure the frame buffer line number */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLNR = hltdc->LayerCfg[LayerIdx].ImageHeight >> 1U;
        break;
      case LTDC_PIXEL_FORMAT_FLEX_YUV_FULLPLANAR:
        /* Configure the auxiliary frame buffer address 0 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA0R = Aux0Addr +
                                              (stride * ((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                          hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) *
                                               ((hltdc->LayerCfg[LayerIdx].WindowY1 -
                                                 hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U)) - 1U;

        /* Configure the auxiliary frame buffer address 1 */
        LTDC_LAYER(hltdc, LayerIdx)->AFBA1R = Aux1Addr +
                                              (stride * ((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                          hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) *
                                               ((hltdc->LayerCfg[LayerIdx].WindowY1 -
                                                 hltdc->LayerCfg[LayerIdx].WindowY0) >> 1U)) - 1U;

        /* Configure the buffer length */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLR = (((0x8000U - (hltdc->LayerCfg[LayerIdx].ImageWidth >> 1U)) * stride)
                                              << 16U) |
                                             ((((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                                 hltdc->LayerCfg[LayerIdx].WindowX0) >> 1U) * stride) + 7U);

        /* Configure the frame buffer line number */
        LTDC_LAYER(hltdc, LayerIdx)->AFBLNR &= ~(LTDC_L1AFBLNR_AFBLNBR);
        LTDC_LAYER(hltdc, LayerIdx)->AFBLNR = hltdc->LayerCfg[LayerIdx].ImageHeight >> 1U;
        break;
      default:
        /* Nothing to do */
        break;
    }

    LTDC_LAYER(hltdc, LayerIdx)->CFBLR  = ((((0x8000U - (hltdc->LayerCfg[LayerIdx].ImageWidth * stride))) << 16U) |
                                           (((hltdc->LayerCfg[LayerIdx].WindowX1 -
                                              hltdc->LayerCfg[LayerIdx].WindowX0) * stride) + 7U));

    /* Enable horizontal mirroring bit & LTDC_Layer by setting LEN bit */
    SET_BIT(LTDC_LAYER(hltdc, LayerIdx)->CR, LTDC_LxCR_HMEN | LTDC_LxCR_LEN);
  }
}

/**
  * @brief Configures the composition parameters for the specified layer in the LTDC peripheral.
  *
  * This static function sets the default color values, constant alpha value, and blending factors for
  * the specified layer of the LTDC. The configuration is based on the LayerCfg structure within the
  * LTDC handle, which includes the background color, alpha values, and blending factors.
  *
  * @param hltdc Pointer to a LTDC_HandleTypeDef structure that contains the configuration information
  *              for the LTDC module.
  * @param LayerIdx Specifies the index of the layer being configured. This parameter can be one of the
  *                 following values:
  *                 - LTDC_LAYER_1: Configuration for layer 1.
  *                 - LTDC_LAYER_2: Configuration for layer 2.
  *                 - Other layer indices as defined by the hardware and used within the driver.
  *
  * @note This function is intended for internal use within the LTDC driver and does not return a value.
 */
static void LTDC_SetCompositionConfig(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  uint32_t tmp;
  uint32_t tmp1;
  uint32_t tmp2;

  /* Configure the default color values */
  tmp = ((uint32_t)(hltdc->LayerCfg[LayerIdx].Backcolor.Green) << 8U);
  tmp1 = ((uint32_t)(hltdc->LayerCfg[LayerIdx].Backcolor.Red) << 16U);
  tmp2 = (hltdc->LayerCfg[LayerIdx].Alpha0 << 24U);
  WRITE_REG(LTDC_LAYER(hltdc, LayerIdx)->DCCR, (hltdc->LayerCfg[LayerIdx].Backcolor.Blue | tmp | tmp1 | tmp2));

  /* Specifies the constant alpha value */
  LTDC_LAYER(hltdc, LayerIdx)->CACR &= ~(LTDC_LxCACR_CONSTA);
  LTDC_LAYER(hltdc, LayerIdx)->CACR = (hltdc->LayerCfg[LayerIdx].Alpha);

  /* Specifies the blending factors */
  LTDC_LAYER(hltdc, LayerIdx)->BFCR &= ~(LTDC_LxBFCR_BOR | LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
  tmp = ((uint32_t)(LTDC_LAYER(hltdc, LayerIdx)->BFCR & LTDC_LxBFCR_BOR_Msk) >> LTDC_LxBFCR_BOR_Pos) << 16U;
  LTDC_LAYER(hltdc, LayerIdx)->BFCR = (hltdc->LayerCfg[LayerIdx].BlendingFactor1 |
                                       hltdc->LayerCfg[LayerIdx].BlendingFactor2 | tmp);
}

/**
  * @brief Sets a software predefined ARGB pixel format using flexible ARGB parameters for the specified layer.
  *
  * This function configures the specified layer of the LTDC to use a software predefined ARGB pixel
  * format. The desired format is specified by the PixelFormat parameter within the LTDC handle.
  * @param  hltdc     Pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx  LTDC Layer index.
  *                   This parameter can be one of the following values: LTDC_LAYER_1 (0) or LTDC_LAYER_2 (1)
  * @retval None
  *
  *  @note This function does not return a value as it is a static function used internally within the
  *        driver.
  */
static void LTDC_SetPredefFormat(LTDC_HandleTypeDef *hltdc, uint32_t LayerIdx)
{
  uint32_t PSIZE = 0U;
  uint32_t ALEN = 0U;
  uint32_t APOS = 0U;
  uint32_t RLEN = 0U;
  uint32_t RPOS = 0U;
  uint32_t BLEN = 0U;
  uint32_t BPOS = 0U;
  uint32_t GLEN = 0U;
  uint32_t GPOS = 0U;

  /* Specify Flex ARGB parameters according to pixel format */
  switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB1555:
      PSIZE = 2U;
      ALEN = 1U;
      APOS = 15U;
      RLEN = 5U;
      RPOS = 10U;
      GLEN = 5U;
      GPOS = 5U;
      BLEN = 5U;
      BPOS = 0U;
      break;
    case LTDC_PIXEL_FORMAT_ARGB4444:
      PSIZE = 2U;
      ALEN = 4U;
      APOS = 12U;
      RLEN = 4U;
      RPOS = 8U;
      GLEN = 4U;
      GPOS = 4U;
      BLEN = 4U;
      BPOS = 0U;
      break;
    case LTDC_PIXEL_FORMAT_L8:
      PSIZE = 1U;
      ALEN = 0U;
      APOS = 0U;
      RLEN = 8U;
      RPOS = 0U;
      GLEN = 8U;
      GPOS = 0U;
      BLEN = 8U;
      BPOS = 0U;
      break;
    case LTDC_PIXEL_FORMAT_AL44:
      PSIZE = 1U;
      ALEN = 4U;
      APOS = 4U;
      RLEN = 4U;
      RPOS = 0U;
      GLEN = 4U;
      GPOS = 0U;
      BLEN = 4U;
      BPOS = 0U;
      break;
    case LTDC_PIXEL_FORMAT_AL88:
      PSIZE = 2U;
      ALEN = 8U;
      APOS = 8U;
      RLEN = 8U;
      RPOS = 0U;
      GLEN = 8U;
      GPOS = 0U;
      BLEN = 8U;
      BPOS = 0U;
      break;
    default:
      break;
  }

  switch (hltdc->LayerCfg[LayerIdx].PixelFormat)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:
    case LTDC_PIXEL_FORMAT_ABGR8888:
    case LTDC_PIXEL_FORMAT_RGBA8888:
    case LTDC_PIXEL_FORMAT_BGRA8888:
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_BGR565:
    case LTDC_PIXEL_FORMAT_RGB888:
      LTDC_LAYER(hltdc, LayerIdx)->PFCR = (hltdc->LayerCfg[LayerIdx].PixelFormat);
      LTDC_LAYER(hltdc, LayerIdx)->FPF0R = 0U;
      LTDC_LAYER(hltdc, LayerIdx)->FPF1R = 0U;
      break;
    case LTDC_PIXEL_FORMAT_ARGB1555:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_L8:
    case LTDC_PIXEL_FORMAT_AL44:
    case LTDC_PIXEL_FORMAT_AL88:
      LTDC_LAYER(hltdc, LayerIdx)->PFCR = LTDC_LxPFCR_PF;
      LTDC_LAYER(hltdc, LayerIdx)->FPF0R = (RLEN << LTDC_LxFPF0R_RLEN_Pos) +
                                           (RPOS << LTDC_LxFPF0R_RPOS_Pos) +
                                           (ALEN << LTDC_LxFPF0R_ALEN_Pos) +
                                           APOS;
      LTDC_LAYER(hltdc, LayerIdx)->FPF1R = (PSIZE << LTDC_LxFPF1R_PSIZE_Pos) +
                                           (BLEN << LTDC_LxFPF1R_BLEN_Pos)  +
                                           (BPOS << LTDC_LxFPF1R_BPOS_Pos) +
                                           (GLEN << LTDC_LxFPF1R_GLEN_Pos) +
                                           GPOS;
      break;
    default:
      break;
  }
}
/**
  * @}
  */


/**
  * @}
  */

#endif /* LTDC */

#endif /* HAL_LTDC_MODULE_ENABLED */

/**
  * @}
  */

