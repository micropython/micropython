/**
  ******************************************************************************
  * @file    stm32n6xx_hal_pssi.c
  * @author  MCD Application Team
  * @brief   PSSI HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Parallel Synchronous Slave Interface (PSSI) peripheral:
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
    The PSSI HAL driver can be used as follows:

    (#) Declare a PSSI_HandleTypeDef handle structure, for example:
        PSSI_HandleTypeDef  hpssi;

    (#) Initialize the PSSI low level resources by implementing the @ref HAL_PSSI_MspInit() API:
        (##) Enable the PSSIx interface clock
        (##) PSSI pins configuration
            (+++) Enable the clock for the PSSI GPIOs
            (+++) Configure PSSI pins as alternate function open-drain
        (##) NVIC configuration if you need to use interrupt process
            (+++) Configure the PSSIx interrupt priority
            (+++) Enable the NVIC PSSI IRQ Channel
        (##) DMA Configuration if you need to use DMA process
            (+++) Declare  DMA_HandleTypeDef handles structure for the transmit and receive
            (+++) Enable the DMAx interface clock
            (+++) Configure the DMA handle parameters
            (+++) Configure the DMA Tx and Rx
            (+++) Associate the initialized DMA handle to the hpssi DMA Tx and Rx handle
            (+++) Configure the priority and enable the NVIC for the transfer complete interrupt on
                  the DMA Tx and Rx

    (#) Configure the Communication Bus Width,  Control Signals, Input Polarity and Output Polarity
         in the hpssi Init structure.

    (#) Initialize the PSSI registers by calling the @ref HAL_PSSI_Init(), configure also the low level Hardware
        (GPIO, CLOCK, NVIC...etc) by calling the customized @ref HAL_PSSI_MspInit(&hpssi) API.

    (#) Configure the PSSI clock as internal or external by calling the @ref HAL_PSSI_ClockConfig().

    (#) For PSSI IO operations, two operation modes are available within this driver :

    *** Polling mode IO operation ***
    =================================
    [..]
      (+) Transmit an amount of data by byte in blocking mode using @ref HAL_PSSI_Transmit()
      (+) Receive an amount of data by byte in blocking mode using @ref HAL_PSSI_Receive()

    *** DMA mode IO operation ***
    ==============================
    [..]
      (+) Transmit an amount of data in non-blocking mode (DMA) using
          @ref HAL_PSSI_Transmit_DMA()
      (+) At transmission end of transfer, @ref HAL_PSSI_TxCpltCallback() is executed and user can
           add his own code by customization of function pointer @ref HAL_PSSI_TxCpltCallback()
      (+) Receive an amount of data in non-blocking mode (DMA) using
          @ref HAL_PSSI_Receive_DMA()
      (+) At reception end of transfer, @ref HAL_PSSI_RxCpltCallback() is executed and user can
           add his own code by customization of function pointer @ref HAL_PSSI_RxCpltCallback()
      (+) In case of transfer Error, @ref HAL_PSSI_ErrorCallback() function is executed and user can
           add his own code by customization of function pointer @ref HAL_PSSI_ErrorCallback()
      (+) Abort a  PSSI process communication with Interrupt using @ref HAL_PSSI_Abort_IT()
      (+) End of abort process, @ref HAL_PSSI_AbortCpltCallback() is executed and user can
           add his own code by customization of function pointer @ref HAL_PSSI_AbortCpltCallback()

     *** PSSI HAL driver macros list ***
     ==================================
     [..]
       Below the list of most used macros in PSSI HAL driver.

      (+) @ref HAL_PSSI_ENABLE     : Enable the PSSI peripheral
      (+) @ref HAL_PSSI_DISABLE    : Disable the PSSI peripheral
      (+) @ref HAL_PSSI_GET_FLAG   : Check whether the specified PSSI flag is set or not
      (+) @ref HAL_PSSI_CLEAR_FLAG : Clear the specified PSSI pending flag
      (+) @ref HAL_PSSI_ENABLE_IT  : Enable the specified PSSI interrupt
      (+) @ref HAL_PSSI_DISABLE_IT : Disable the specified PSSI interrupt

     *** Callback registration ***
     =============================================
     Use Functions @ref HAL_PSSI_RegisterCallback() or @ref HAL_PSSI_RegisterAddrCallback()
     to register an interrupt callback.

     Function @ref HAL_PSSI_RegisterCallback() allows to register following callbacks:
       (+) TxCpltCallback       : callback for transmission end of transfer.
       (+) RxCpltCallback       : callback for reception end of transfer.
       (+) ErrorCallback        : callback for error detection.
       (+) AbortCpltCallback    : callback for abort completion process.
       (+) MspInitCallback      : callback for Msp Init.
       (+) MspDeInitCallback    : callback for Msp DeInit.
     This function takes as parameters the HAL peripheral handle, the Callback ID
     and a pointer to the user callback function.


     Use function @ref HAL_PSSI_UnRegisterCallback to reset a callback to the default
     weak function.
     @ref HAL_PSSI_UnRegisterCallback takes as parameters the HAL peripheral handle,
     and the Callback ID.
     This function allows to reset following callbacks:
       (+) TxCpltCallback       : callback for transmission end of transfer.
       (+) RxCpltCallback       : callback for reception end of transfer.
       (+) ErrorCallback        : callback for error detection.
       (+) AbortCpltCallback    : callback for abort completion process.
       (+) MspInitCallback      : callback for Msp Init.
       (+) MspDeInitCallback    : callback for Msp DeInit.


     By default, after the @ref HAL_PSSI_Init() and when the state is @ref HAL_PSSI_STATE_RESET
     all callbacks are set to the corresponding weak functions:
     examples @ref HAL_PSSI_TxCpltCallback(), @ref HAL_PSSI_RxCpltCallback().
     Exception done for MspInit and MspDeInit functions that are
     reset to the legacy weak functions in the @ref HAL_PSSI_Init()/ @ref HAL_PSSI_DeInit() only when
     these callbacks are null (not registered beforehand).
     If MspInit or MspDeInit are not null, the @ref HAL_PSSI_Init()/ @ref HAL_PSSI_DeInit()
     keep and use the user MspInit/MspDeInit callbacks (registered beforehand) whatever the state.

     Callbacks can be registered/unregistered in @ref HAL_PSSI_STATE_READY state only.
     Exception done MspInit/MspDeInit functions that can be registered/unregistered
     in @ref HAL_PSSI_STATE_READY or @ref HAL_PSSI_STATE_RESET state,
     thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
     Then, the user first registers the MspInit/MspDeInit user callbacks
     using @ref HAL_PSSI_RegisterCallback() before calling @ref HAL_PSSI_DeInit()
     or @ref HAL_PSSI_Init() function.


     [..]
       (@) You can refer to the PSSI HAL driver header file for more useful macros

  @endverbatim
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup PSSI PSSI
  * @brief PSSI HAL module driver
  * @{
  */

#ifdef HAL_PSSI_MODULE_ENABLED
#if defined(PSSI)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/** @defgroup PSSI_Private_Define PSSI Private Define
  * @{
  */


/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/** @defgroup PSSI_Private_Functions PSSI Private Functions
  * @{
  */
/* Private functions to handle DMA transfer */
#if defined(HAL_DMA_MODULE_ENABLED)
void PSSI_DMATransmitCplt(DMA_HandleTypeDef *hdma);
void PSSI_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
void PSSI_DMAError(DMA_HandleTypeDef *hdma);
void PSSI_DMAAbort(DMA_HandleTypeDef *hdma);
#endif /*HAL_DMA_MODULE_ENABLED*/

/* Private functions to handle IT transfer */
static void PSSI_Error(PSSI_HandleTypeDef *hpssi, uint32_t ErrorCode);


/* Private functions for PSSI transfer IRQ handler */


/* Private functions to handle flags during polling transfer */
static HAL_StatusTypeDef PSSI_WaitOnStatusUntilTimeout(PSSI_HandleTypeDef *hpssi, uint32_t Flag, FlagStatus Status,
                                                       uint32_t Timeout, uint32_t Tickstart);

/* Private functions to centralize the enable/disable of Interrupts */


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup PSSI_Exported_Functions PSSI Exported Functions
  * @{
  */

/** @defgroup PSSI_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This subsection provides a set of functions allowing to initialize and
          deinitialize the PSSIx peripheral:

      (+) User must implement HAL_PSSI_MspInit() function in which he configures
          all related peripherals resources (CLOCK, GPIO, DMA, IT and NVIC ).

      (+) Call the function HAL_PSSI_Init() to configure the selected device with
          the selected configuration:
        (++) Data Width
        (++) Control Signals
        (++) Input Clock polarity
        (++) Output Clock polarity

      (+) Call the function HAL_PSSI_DeInit() to restore the default configuration
          of the selected PSSIx peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the PSSI according to the specified parameters
  *         in the PSSI_InitTypeDef and initialize the associated handle.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_Init(PSSI_HandleTypeDef *hpssi)
{
  /* Check the PSSI handle allocation */
  if (hpssi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_PSSI_ALL_INSTANCE(hpssi->Instance));
  assert_param(IS_PSSI_CONTROL_SIGNAL(hpssi->Init.ControlSignal));
  assert_param(IS_PSSI_BUSWIDTH(hpssi->Init.BusWidth));
  assert_param(IS_PSSI_CLOCK_POLARITY(hpssi->Init.ClockPolarity));
  assert_param(IS_PSSI_DE_POLARITY(hpssi->Init.DataEnablePolarity));
  assert_param(IS_PSSI_RDY_POLARITY(hpssi->Init.ReadyPolarity));

  if (hpssi->State == HAL_PSSI_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hpssi->Lock = HAL_UNLOCKED;

#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
    /* Init the PSSI Callback settings */
    hpssi->TxCpltCallback = HAL_PSSI_TxCpltCallback; /* Legacy weak TxCpltCallback */
    hpssi->RxCpltCallback = HAL_PSSI_RxCpltCallback; /* Legacy weak RxCpltCallback */
    hpssi->ErrorCallback        = HAL_PSSI_ErrorCallback;        /* Legacy weak ErrorCallback        */
    hpssi->AbortCpltCallback    = HAL_PSSI_AbortCpltCallback;    /* Legacy weak AbortCpltCallback    */

    if (hpssi->MspInitCallback == NULL)
    {
      hpssi->MspInitCallback = HAL_PSSI_MspInit; /* Legacy weak MspInit  */
    }

    /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
    hpssi->MspInitCallback(hpssi);
#else
    /* Init the low level hardware : GPIO, CLOCK, CORTEX...etc */
    HAL_PSSI_MspInit(hpssi);
#endif /*USE_HAL_PSSI_REGISTER_CALLBACKS*/
  }

  hpssi->State = HAL_PSSI_STATE_BUSY;

  /* Disable the selected PSSI peripheral */
  HAL_PSSI_DISABLE(hpssi);

  /*---------------------------- PSSIx CR Configuration ----------------------*/
  /* Configure PSSIx: Control Signal and Bus Width*/

  MODIFY_REG(hpssi->Instance->CR, PSSI_CR_DERDYCFG | PSSI_CR_EDM | PSSI_CR_DEPOL | PSSI_CR_RDYPOL,
             hpssi->Init.ControlSignal | hpssi->Init.DataEnablePolarity |
             hpssi->Init.ReadyPolarity | hpssi->Init.BusWidth);

  hpssi->ErrorCode = HAL_PSSI_ERROR_NONE;
  hpssi->State = HAL_PSSI_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitialize the PSSI peripheral.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_DeInit(PSSI_HandleTypeDef *hpssi)
{
  /* Check the PSSI handle allocation */
  if (hpssi == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_PSSI_ALL_INSTANCE(hpssi->Instance));

  hpssi->State = HAL_PSSI_STATE_BUSY;

  /* Disable the PSSI Peripheral Clock */
  HAL_PSSI_DISABLE(hpssi);

#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
  if (hpssi->MspDeInitCallback == NULL)
  {
    hpssi->MspDeInitCallback = HAL_PSSI_MspDeInit; /* Legacy weak MspDeInit  */
  }

  /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
  hpssi->MspDeInitCallback(hpssi);
#else
  /* DeInit the low level hardware: GPIO, CLOCK, NVIC */
  HAL_PSSI_MspDeInit(hpssi);
#endif /*USE_HAL_PSSI_REGISTER_CALLBACKS*/

  hpssi->ErrorCode = HAL_PSSI_ERROR_NONE;
  hpssi->State = HAL_PSSI_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hpssi);

  return HAL_OK;
}

/**
  * @brief Initialize the PSSI MSP.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval None
  */
__weak void HAL_PSSI_MspInit(PSSI_HandleTypeDef *hpssi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpssi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PSSI_MspInit can be implemented in the user file
   */
}

/**
  * @brief De-Initialize the PSSI MSP.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval None
  */
__weak void HAL_PSSI_MspDeInit(PSSI_HandleTypeDef *hpssi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpssi);

  /* NOTE : This function should not be modified; when the callback is needed,
            the HAL_PSSI_MspDeInit can be implemented in the user file
   */
}

#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User PSSI Callback
  *         To be used instead of the weak predefined callback
  * @note   The HAL_PSSI_RegisterCallback() may be called before HAL_PSSI_Init() in
  *         HAL_PSSI_STATE_RESET to register callbacks for HAL_PSSI_MSPINIT_CB_ID
  *         and HAL_PSSI_MSPDEINIT_CB_ID.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @param  CallbackID ID of the callback to be registered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_PSSI_TX_COMPLETE_CB_ID  Tx Transfer completed callback ID
  *          @arg @ref HAL_PSSI_RX_COMPLETE_CB_ID  Rx Transfer completed callback ID
  *          @arg @ref HAL_PSSI_ERROR_CB_ID Error callback ID
  *          @arg @ref HAL_PSSI_ABORT_CB_ID Abort callback ID
  *          @arg @ref HAL_PSSI_MSPINIT_CB_ID MspInit callback ID
  *          @arg @ref HAL_PSSI_MSPDEINIT_CB_ID MspDeInit callback ID
  * @param  pCallback pointer to the Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_RegisterCallback(PSSI_HandleTypeDef *hpssi, HAL_PSSI_CallbackIDTypeDef CallbackID,
                                            pPSSI_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hpssi->ErrorCode |= HAL_PSSI_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }

  if (HAL_PSSI_STATE_READY == hpssi->State)
  {
    switch (CallbackID)
    {
      case HAL_PSSI_TX_COMPLETE_CB_ID :
        hpssi->TxCpltCallback = pCallback;
        break;

      case HAL_PSSI_RX_COMPLETE_CB_ID :
        hpssi->RxCpltCallback = pCallback;
        break;

      case HAL_PSSI_ERROR_CB_ID :
        hpssi->ErrorCallback = pCallback;
        break;

      case HAL_PSSI_ABORT_CB_ID :
        hpssi->AbortCpltCallback = pCallback;
        break;

      case HAL_PSSI_MSPINIT_CB_ID :
        hpssi->MspInitCallback = pCallback;
        break;

      case HAL_PSSI_MSPDEINIT_CB_ID :
        hpssi->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (HAL_PSSI_STATE_RESET == hpssi->State)
  {
    switch (CallbackID)
    {
      case HAL_PSSI_MSPINIT_CB_ID :
        hpssi->MspInitCallback = pCallback;
        break;

      case HAL_PSSI_MSPDEINIT_CB_ID :
        hpssi->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hpssi->ErrorCode |= HAL_PSSI_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister an PSSI Callback
  *         PSSI callback is redirected to the weak predefined callback
  * @note   The HAL_PSSI_UnRegisterCallback() may be called before HAL_PSSI_Init() in
  *         HAL_PSSI_STATE_RESET to un-register callbacks for HAL_PSSI_MSPINIT_CB_ID
  *         and HAL_PSSI_MSPDEINIT_CB_ID.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @param  CallbackID ID of the callback to be unregistered
  *         This parameter can be one of the following values:
  *          @arg @ref HAL_PSSI_TX_COMPLETE_CB_ID  Tx Transfer completed callback ID
  *          @arg @ref HAL_PSSI_RX_COMPLETE_CB_ID  Rx Transfer completed callback ID
  *          @arg @ref HAL_PSSI_ERROR_CB_ID Error callback ID
  *          @arg @ref HAL_PSSI_ABORT_CB_ID Abort callback ID
  *          @arg @ref HAL_PSSI_MSPINIT_CB_ID MspInit callback ID
  *          @arg @ref HAL_PSSI_MSPDEINIT_CB_ID MspDeInit callback ID
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_UnRegisterCallback(PSSI_HandleTypeDef *hpssi, HAL_PSSI_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (HAL_PSSI_STATE_READY == hpssi->State)
  {
    switch (CallbackID)
    {
      case HAL_PSSI_TX_COMPLETE_CB_ID :
        hpssi->TxCpltCallback = HAL_PSSI_TxCpltCallback;             /* Legacy weak TxCpltCallback     */
        break;

      case HAL_PSSI_RX_COMPLETE_CB_ID :
        hpssi->RxCpltCallback = HAL_PSSI_RxCpltCallback;             /* Legacy weak RxCpltCallback     */
        break;

      case HAL_PSSI_ERROR_CB_ID :
        hpssi->ErrorCallback = HAL_PSSI_ErrorCallback;               /* Legacy weak ErrorCallback      */
        break;

      case HAL_PSSI_ABORT_CB_ID :
        hpssi->AbortCpltCallback = HAL_PSSI_AbortCpltCallback;       /* Legacy weak AbortCpltCallback  */
        break;

      case HAL_PSSI_MSPINIT_CB_ID :
        hpssi->MspInitCallback = HAL_PSSI_MspInit;                   /* Legacy weak MspInit            */
        break;

      case HAL_PSSI_MSPDEINIT_CB_ID :
        hpssi->MspDeInitCallback = HAL_PSSI_MspDeInit;               /* Legacy weak MspDeInit          */
        break;

      default :
        /* Update the error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (HAL_PSSI_STATE_RESET == hpssi->State)
  {
    switch (CallbackID)
    {
      case HAL_PSSI_MSPINIT_CB_ID :
        hpssi->MspInitCallback = HAL_PSSI_MspInit;                   /* Legacy weak MspInit            */
        break;

      case HAL_PSSI_MSPDEINIT_CB_ID :
        hpssi->MspDeInitCallback = HAL_PSSI_MspDeInit;               /* Legacy weak MspDeInit          */
        break;

      default :
        /* Update the error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hpssi->ErrorCode |= HAL_PSSI_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup PSSI_Exported_Functions_Group2 Input and Output operation functions
  *  @brief   Data transfers functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the PSSI data
    transfers.

    (#) There are two modes of transfer:
       (++) Blocking mode : The communication is performed in the polling mode.
            The status of all data processing is returned by the same function
            after finishing transfer.
       (++) No-Blocking mode : The communication is performed using DMA.
            These functions return the status of the transfer startup.
            The end of the data processing will be indicated through the
            dedicated  the DMA IRQ .

    (#) Blocking mode functions are :
        (++) HAL_PSSI_Transmit()
        (++) HAL_PSSI_Receive()

    (#) No-Blocking mode functions with DMA are :
        (++) HAL_PSSI_Transmit_DMA()
        (++) HAL_PSSI_Receive_DMA()

    (#) A set of Transfer Complete Callbacks are provided in non Blocking mode:
        (++) HAL_PSSI_TxCpltCallback()
        (++) HAL_PSSI_RxCpltCallback()
        (++) HAL_PSSI_ErrorCallback()
        (++) HAL_PSSI_AbortCpltCallback()

@endverbatim
  * @{
  */

/**
  * @brief  Transmits in master mode an amount of data in blocking mode.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent (in bytes)
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_Transmit(PSSI_HandleTypeDef *hpssi, uint8_t *pData, uint32_t Size, uint32_t Timeout)
{
  uint32_t tickstart;
  uint32_t  transfer_size = Size;

  if (((hpssi->Init.DataWidth == HAL_PSSI_8BITS) && (hpssi->Init.BusWidth != HAL_PSSI_8LINES)) ||
      ((hpssi->Init.DataWidth == HAL_PSSI_16BITS) && ((Size % 2U) != 0U)) ||
      ((hpssi->Init.DataWidth == HAL_PSSI_32BITS) && ((Size % 4U) != 0U)))
  {
    hpssi->ErrorCode = HAL_PSSI_ERROR_NOT_SUPPORTED;
    return HAL_ERROR;
  }
  if (hpssi->State == HAL_PSSI_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hpssi);

    hpssi->State     = HAL_PSSI_STATE_BUSY;
    hpssi->ErrorCode = HAL_PSSI_ERROR_NONE;

    /* Disable the selected PSSI peripheral */
    HAL_PSSI_DISABLE(hpssi);

    /* Configure transfer parameters */
    MODIFY_REG(hpssi->Instance->CR, (PSSI_CR_OUTEN | PSSI_CR_CKPOL),
               (PSSI_CR_OUTEN_OUTPUT | ((hpssi->Init.ClockPolarity == HAL_PSSI_RISING_EDGE) ? 0U : PSSI_CR_CKPOL)));

#if defined(HAL_DMA_MODULE_ENABLED)
    /* DMA Disable */
    hpssi->Instance->CR &= PSSI_CR_DMA_DISABLE;
#endif /*HAL_DMA_MODULE_ENABLED*/

    /* Enable the selected PSSI peripheral */
    HAL_PSSI_ENABLE(hpssi);

    if (hpssi->Init.DataWidth == HAL_PSSI_8BITS)
    {
      uint8_t *pbuffer = pData;
      while (transfer_size > 0U)
      {
        /* Init tickstart for timeout management*/
        tickstart = HAL_GetTick();
        /* Wait until Fifo is ready to transfer one byte flag is set */
        if (PSSI_WaitOnStatusUntilTimeout(hpssi, PSSI_FLAG_RTT1B, RESET, Timeout, tickstart) != HAL_OK)
        {
          hpssi->ErrorCode = HAL_PSSI_ERROR_TIMEOUT;
          hpssi->State = HAL_PSSI_STATE_READY;
          /* Process Unlocked */
          __HAL_UNLOCK(hpssi);
          return HAL_ERROR;
        }
        /* Write data to DR */
        *(__IO uint8_t *)(&hpssi->Instance->DR) = *(uint8_t *)pbuffer;

        /* Increment Buffer pointer */
        pbuffer++;

        transfer_size--;
      }
    }
    else if (hpssi->Init.DataWidth == HAL_PSSI_16BITS)
    {
      uint16_t *pbuffer = (uint16_t *)pData;
      __IO uint16_t *dr = (__IO uint16_t *)(&hpssi->Instance->DR);

      while (transfer_size > 0U)
      {
        /* Init tickstart for timeout management*/
        tickstart = HAL_GetTick();
        /* Wait until Fifo is ready to transfer four bytes flag is set */
        if (PSSI_WaitOnStatusUntilTimeout(hpssi, PSSI_FLAG_RTT4B, RESET, Timeout, tickstart) != HAL_OK)
        {
          hpssi->ErrorCode = HAL_PSSI_ERROR_TIMEOUT;
          hpssi->State = HAL_PSSI_STATE_READY;
          /* Process Unlocked */
          __HAL_UNLOCK(hpssi);
          return HAL_ERROR;
        }
        /* Write data to DR */
        *dr = *pbuffer;

        /* Increment Buffer pointer */
        pbuffer++;
        transfer_size -= 2U;
      }
    }
    else if (hpssi->Init.DataWidth == HAL_PSSI_32BITS)
    {
      uint32_t *pbuffer = (uint32_t *)pData;
      while (transfer_size > 0U)
      {
        /* Init tickstart for timeout management*/
        tickstart = HAL_GetTick();
        /* Wait until Fifo is ready to transfer four bytes flag is set */
        if (PSSI_WaitOnStatusUntilTimeout(hpssi, PSSI_FLAG_RTT4B, RESET, Timeout, tickstart) != HAL_OK)
        {
          hpssi->ErrorCode = HAL_PSSI_ERROR_TIMEOUT;
          hpssi->State = HAL_PSSI_STATE_READY;
          /* Process Unlocked */
          __HAL_UNLOCK(hpssi);
          return HAL_ERROR;
        }
        /* Write data to DR */
        *(__IO uint32_t *)(&hpssi->Instance->DR) = *pbuffer;

        /* Increment Buffer pointer */
        pbuffer++;
        transfer_size -= 4U;
      }
    }
    else
    {
      hpssi->ErrorCode = HAL_PSSI_ERROR_NOT_SUPPORTED;
      hpssi->State = HAL_PSSI_STATE_READY;
      /* Process Unlocked */
      __HAL_UNLOCK(hpssi);
      return HAL_ERROR;
    }

    /* Check Errors Flags */
    if (HAL_PSSI_GET_FLAG(hpssi, PSSI_FLAG_OVR_RIS) != 0U)
    {
      HAL_PSSI_CLEAR_FLAG(hpssi, PSSI_FLAG_OVR_RIS);
      HAL_PSSI_DISABLE(hpssi);
      hpssi->ErrorCode = HAL_PSSI_ERROR_UNDER_RUN;
      hpssi->State = HAL_PSSI_STATE_READY;
      /* Process Unlocked */
      __HAL_UNLOCK(hpssi);
      return HAL_ERROR;
    }

    hpssi->State = HAL_PSSI_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hpssi);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receives an amount of data in blocking mode.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be received (in bytes)
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_Receive(PSSI_HandleTypeDef *hpssi, uint8_t *pData, uint32_t Size, uint32_t Timeout)
{
  uint32_t tickstart;
  uint32_t  transfer_size = Size;

  if (((hpssi->Init.DataWidth == HAL_PSSI_8BITS) && (hpssi->Init.BusWidth != HAL_PSSI_8LINES)) ||
      ((hpssi->Init.DataWidth == HAL_PSSI_16BITS) && ((Size % 2U) != 0U)) ||
      ((hpssi->Init.DataWidth == HAL_PSSI_32BITS) && ((Size % 4U) != 0U)))
  {
    hpssi->ErrorCode = HAL_PSSI_ERROR_NOT_SUPPORTED;
    return HAL_ERROR;
  }

  if (hpssi->State == HAL_PSSI_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hpssi);

    hpssi->State     = HAL_PSSI_STATE_BUSY;
    hpssi->ErrorCode = HAL_PSSI_ERROR_NONE;

    /* Disable the selected PSSI peripheral */
    HAL_PSSI_DISABLE(hpssi);
    /* Configure transfer parameters */
    MODIFY_REG(hpssi->Instance->CR, (PSSI_CR_OUTEN | PSSI_CR_CKPOL),
               (PSSI_CR_OUTEN_INPUT | ((hpssi->Init.ClockPolarity == HAL_PSSI_FALLING_EDGE) ? 0U : PSSI_CR_CKPOL)));

#if defined(HAL_DMA_MODULE_ENABLED)
    /* DMA Disable */
    hpssi->Instance->CR &= PSSI_CR_DMA_DISABLE;
#endif /*HAL_DMA_MODULE_ENABLED*/

    /* Enable the selected PSSI peripheral */
    HAL_PSSI_ENABLE(hpssi);
    if (hpssi->Init.DataWidth == HAL_PSSI_8BITS)
    {
      uint8_t *pbuffer = pData;

      while (transfer_size > 0U)
      {
        /* Init tickstart for timeout management*/
        tickstart = HAL_GetTick();
        /* Wait until Fifo is ready to receive one byte flag is set */
        if (PSSI_WaitOnStatusUntilTimeout(hpssi, PSSI_FLAG_RTT1B, RESET, Timeout, tickstart) != HAL_OK)
        {
          hpssi->ErrorCode = HAL_PSSI_ERROR_TIMEOUT;
          hpssi->State = HAL_PSSI_STATE_READY;
          /* Process Unlocked */
          __HAL_UNLOCK(hpssi);
          return HAL_ERROR;
        }
        /* Read data from DR */
        *pbuffer = *(__IO uint8_t *)(&hpssi->Instance->DR);
        pbuffer++;
        transfer_size--;
      }
    }
    else if (hpssi->Init.DataWidth == HAL_PSSI_16BITS)
    {
      uint16_t *pbuffer = (uint16_t *)pData;
      __IO uint16_t *dr = (__IO uint16_t *)(&hpssi->Instance->DR);

      while (transfer_size > 0U)
      {
        /* Init tickstart for timeout management*/
        tickstart = HAL_GetTick();
        /* Wait until Fifo is ready to receive four bytes flag is set */
        if (PSSI_WaitOnStatusUntilTimeout(hpssi, PSSI_FLAG_RTT4B, RESET, Timeout, tickstart) != HAL_OK)
        {
          hpssi->ErrorCode = HAL_PSSI_ERROR_TIMEOUT;
          hpssi->State = HAL_PSSI_STATE_READY;
          /* Process Unlocked */
          __HAL_UNLOCK(hpssi);
          return HAL_ERROR;
        }

        /* Read data from DR */
        *pbuffer = *dr;
        pbuffer++;
        transfer_size -= 2U;
      }
    }
    else if (hpssi->Init.DataWidth == HAL_PSSI_32BITS)
    {
      uint32_t *pbuffer = (uint32_t *)pData;

      while (transfer_size > 0U)
      {
        /* Init tickstart for timeout management*/
        tickstart = HAL_GetTick();
        /* Wait until Fifo is ready to receive four bytes flag is set */
        if (PSSI_WaitOnStatusUntilTimeout(hpssi, PSSI_FLAG_RTT4B, RESET, Timeout, tickstart) != HAL_OK)
        {
          hpssi->ErrorCode = HAL_PSSI_ERROR_TIMEOUT;
          hpssi->State = HAL_PSSI_STATE_READY;
          /* Process Unlocked */
          __HAL_UNLOCK(hpssi);
          return HAL_ERROR;
        }

        /* Read data from DR */
        *pbuffer = *(__IO uint32_t *)(&hpssi->Instance->DR);
        pbuffer++;
        transfer_size -= 4U;
      }
    }
    else
    {
      hpssi->ErrorCode = HAL_PSSI_ERROR_NOT_SUPPORTED;
      hpssi->State = HAL_PSSI_STATE_READY;
      /* Process Unlocked */
      __HAL_UNLOCK(hpssi);
      return HAL_ERROR;
    }
    /* Check Errors Flags */

    if (HAL_PSSI_GET_FLAG(hpssi, PSSI_FLAG_OVR_RIS) != 0U)
    {
      HAL_PSSI_CLEAR_FLAG(hpssi, PSSI_FLAG_OVR_RIS);
      hpssi->ErrorCode = HAL_PSSI_ERROR_OVER_RUN;
      __HAL_UNLOCK(hpssi);
      return HAL_ERROR;
    }

    hpssi->State = HAL_PSSI_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hpssi);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  Transmit an amount of data in non-blocking mode with DMA
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent (in bytes)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_Transmit_DMA(PSSI_HandleTypeDef *hpssi, uint32_t *pData, uint32_t Size)
{
  HAL_StatusTypeDef dmaxferstatus;

  if (hpssi->State == HAL_PSSI_STATE_READY)
  {

    /* Process Locked */
    __HAL_LOCK(hpssi);

    hpssi->State       = HAL_PSSI_STATE_BUSY_TX;
    hpssi->ErrorCode   = HAL_PSSI_ERROR_NONE;

    /* Disable the selected PSSI peripheral */
    HAL_PSSI_DISABLE(hpssi);

    /* Prepare transfer parameters */
    hpssi->pBuffPtr    = pData;
    hpssi->XferCount   = Size;

    if (hpssi->XferCount > PSSI_MAX_NBYTE_SIZE)
    {
      hpssi->XferSize = PSSI_MAX_NBYTE_SIZE;
    }
    else
    {
      hpssi->XferSize = hpssi->XferCount;
    }

    if (hpssi->XferSize > 0U)
    {
      if (hpssi->hdmatx != NULL)
      {

        /* Configure BusWidth */
        if (hpssi->hdmatx->Init.DestDataWidth == DMA_DEST_DATAWIDTH_BYTE)
        {
          MODIFY_REG(hpssi->Instance->CR, PSSI_CR_DMAEN | PSSI_CR_OUTEN | PSSI_CR_CKPOL,
                     PSSI_CR_DMA_ENABLE | PSSI_CR_OUTEN_OUTPUT |
                     ((hpssi->Init.ClockPolarity == HAL_PSSI_RISING_EDGE) ? 0U : PSSI_CR_CKPOL));
        }
        else
        {
          MODIFY_REG(hpssi->Instance->CR, PSSI_CR_DMAEN | PSSI_CR_OUTEN | PSSI_CR_CKPOL,
                     PSSI_CR_DMA_ENABLE | hpssi->Init.BusWidth | PSSI_CR_OUTEN_OUTPUT |
                     ((hpssi->Init.ClockPolarity == HAL_PSSI_RISING_EDGE) ? 0U : PSSI_CR_CKPOL));
        }

        /* Set the PSSI DMA transfer complete callback */
        hpssi->hdmatx->XferCpltCallback = PSSI_DMATransmitCplt;

        /* Set the DMA error callback */
        hpssi->hdmatx->XferErrorCallback = PSSI_DMAError;

        /* Set the unused DMA callbacks to NULL */
        hpssi->hdmatx->XferHalfCpltCallback = NULL;
        hpssi->hdmatx->XferAbortCallback = NULL;

        /* Enable the DMA  */
        if ((hpssi->hdmatx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
        {
          if (hpssi->hdmatx->LinkedListQueue != NULL)
          {
            /* Enable the DMA channel */
            /* Set DMA data size */
            hpssi->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = hpssi->XferSize;
            /* Set DMA source address */
            hpssi->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)pData;
            /* Set DMA destination address */
            hpssi->hdmatx->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] =
              (uint32_t)&hpssi->Instance->DR;

            dmaxferstatus = HAL_DMAEx_List_Start_IT(hpssi->hdmatx);
          }
          else
          {
            /* Return error status */
            return HAL_ERROR;
          }
        }
        else
        {
          dmaxferstatus = HAL_DMA_Start_IT(hpssi->hdmatx, (uint32_t)pData, (uint32_t)&hpssi->Instance->DR,
                                           hpssi->XferSize);
        }
      }
      else
      {
        /* Update PSSI state */
        hpssi->State     = HAL_PSSI_STATE_READY;

        /* Update PSSI error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_DMA;

        /* Process Unlocked */
        __HAL_UNLOCK(hpssi);

        return HAL_ERROR;
      }

      if (dmaxferstatus == HAL_OK)
      {
        /* Update XferCount value */
        hpssi->XferCount -= hpssi->XferSize;

        /* Process Unlocked */
        __HAL_UNLOCK(hpssi);

        /* Note : The PSSI interrupts must be enabled after unlocking current process
                  to avoid the risk of PSSI interrupt handle execution before current
                  process unlock */
        /* Enable ERR interrupt */
        HAL_PSSI_ENABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);

        /* Enable DMA Request */
        hpssi->Instance->CR |= PSSI_CR_DMA_ENABLE;
        /* Enable the selected PSSI peripheral */
        HAL_PSSI_ENABLE(hpssi);
      }
      else
      {
        /* Update PSSI state */
        hpssi->State     = HAL_PSSI_STATE_READY;

        /* Update PSSI error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_DMA;

        /* Process Unlocked */
        __HAL_UNLOCK(hpssi);

        return HAL_ERROR;
      }
    }
    else
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hpssi);

      /* Note : The PSSI interrupts must be enabled after unlocking current process
                to avoid the risk of PSSI interrupt handle execution before current
                process unlock */
      /* Enable ERRinterrupt */
      /* possible to enable all of these */

      HAL_PSSI_ENABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Receive an amount of data in non-blocking mode with DMA
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be received (in bytes)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_Receive_DMA(PSSI_HandleTypeDef *hpssi, uint32_t *pData, uint32_t Size)
{

  HAL_StatusTypeDef dmaxferstatus;

  if (hpssi->State == HAL_PSSI_STATE_READY)
  {

    /* Disable the selected PSSI peripheral */
    HAL_PSSI_DISABLE(hpssi);
    /* Process Locked */
    __HAL_LOCK(hpssi);

    hpssi->State       = HAL_PSSI_STATE_BUSY_RX;
    hpssi->ErrorCode   = HAL_PSSI_ERROR_NONE;

    /* Prepare transfer parameters */
    hpssi->pBuffPtr    = pData;
    hpssi->XferCount   = Size;

    if (hpssi->XferCount > PSSI_MAX_NBYTE_SIZE)
    {
      hpssi->XferSize = PSSI_MAX_NBYTE_SIZE;
    }
    else
    {
      hpssi->XferSize = hpssi->XferCount;
    }

    if (hpssi->XferSize > 0U)
    {
      if (hpssi->hdmarx != NULL)
      {
        /* Configure BusWidth */
        if (hpssi->hdmarx->Init.SrcDataWidth == DMA_SRC_DATAWIDTH_BYTE)
        {
          MODIFY_REG(hpssi->Instance->CR, PSSI_CR_DMAEN | PSSI_CR_OUTEN | PSSI_CR_CKPOL, PSSI_CR_DMA_ENABLE |
                     ((hpssi->Init.ClockPolarity == HAL_PSSI_RISING_EDGE) ? PSSI_CR_CKPOL : 0U));
        }
        else
        {
          MODIFY_REG(hpssi->Instance->CR, PSSI_CR_DMAEN | PSSI_CR_OUTEN | PSSI_CR_CKPOL,
                     PSSI_CR_DMA_ENABLE | hpssi->Init.BusWidth |
                     ((hpssi->Init.ClockPolarity == HAL_PSSI_RISING_EDGE) ? PSSI_CR_CKPOL : 0U));
        }

        /* Set the PSSI DMA transfer complete callback */
        hpssi->hdmarx->XferCpltCallback = PSSI_DMAReceiveCplt;

        /* Set the DMA error callback */
        hpssi->hdmarx->XferErrorCallback = PSSI_DMAError;

        /* Set the unused DMA callbacks to NULL */
        hpssi->hdmarx->XferHalfCpltCallback = NULL;
        hpssi->hdmarx->XferAbortCallback = NULL;

        /* Enable the DMA  */
        if ((hpssi->hdmarx->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
        {
          if (hpssi->hdmarx->LinkedListQueue != NULL)
          {
            /* Enable the DMA channel */
            /* Set DMA data size */
            hpssi->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = hpssi->XferSize;
            /* Set DMA source address */
            hpssi->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] =
              (uint32_t)&hpssi->Instance->DR;
            /* Set DMA destination address */
            hpssi->hdmarx->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)pData;

            dmaxferstatus = HAL_DMAEx_List_Start_IT(hpssi->hdmarx);
          }
          else
          {
            /* Return error status */
            return HAL_ERROR;
          }
        }
        else
        {
          dmaxferstatus = HAL_DMA_Start_IT(hpssi->hdmarx, (uint32_t)&hpssi->Instance->DR, (uint32_t)pData,
                                           hpssi->XferSize);
        }
      }
      else
      {
        /* Update PSSI state */
        hpssi->State     = HAL_PSSI_STATE_READY;

        /* Update PSSI error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_DMA;

        /* Process Unlocked */
        __HAL_UNLOCK(hpssi);

        return HAL_ERROR;
      }

      if (dmaxferstatus == HAL_OK)
      {
        /* Update XferCount value */
        hpssi->XferCount -= hpssi->XferSize;

        /* Process Unlocked */
        __HAL_UNLOCK(hpssi);

        /* Note : The PSSI interrupts must be enabled after unlocking current process
                  to avoid the risk of PSSI interrupt handle execution before current
                  process unlock */
        /* Enable ERR  interrupt */
        HAL_PSSI_ENABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);

        /* Enable DMA Request */
        hpssi->Instance->CR |= PSSI_CR_DMA_ENABLE;
        /* Enable the selected PSSI peripheral */
        HAL_PSSI_ENABLE(hpssi);
      }
      else
      {
        /* Update PSSI state */
        hpssi->State     = HAL_PSSI_STATE_READY;

        /* Update PSSI error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_DMA;

        /* Process Unlocked */
        __HAL_UNLOCK(hpssi);

        return HAL_ERROR;
      }
    }
    else
    {
      /* Process Unlocked */
      __HAL_UNLOCK(hpssi);

      /* Enable ERR,interrupt */
      HAL_PSSI_ENABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);
    }

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @brief  Abort a DMA process communication with Interrupt.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_Abort_DMA(PSSI_HandleTypeDef *hpssi)
{
  /* Process Locked */
  __HAL_LOCK(hpssi);

  /* Disable Interrupts */
  HAL_PSSI_DISABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);

  /* Set State at HAL_PSSI_STATE_ABORT */
  hpssi->State = HAL_PSSI_STATE_ABORT;

  /* Abort DMA TX transfer if any */
  if ((hpssi->Instance->CR & PSSI_CR_DMAEN) == PSSI_CR_DMAEN)
  {
    if (hpssi->State == HAL_PSSI_STATE_BUSY_TX)
    {
      hpssi->Instance->CR &= ~PSSI_CR_DMAEN;

      if (hpssi->hdmatx != NULL)
      {
        /* Set the PSSI DMA Abort callback :
        will lead to call HAL_PSSI_ErrorCallback() at end of DMA abort procedure */
        hpssi->hdmatx->XferAbortCallback = PSSI_DMAAbort;

        /* Abort DMA TX */
        if (HAL_DMA_Abort_IT(hpssi->hdmatx) != HAL_OK)
        {
          /* Call Directly XferAbortCallback function in case of error */
          hpssi->hdmatx->XferAbortCallback(hpssi->hdmatx);
        }
      }
    }
    /* Abort DMA RX transfer if any */
    else if (hpssi->State == HAL_PSSI_STATE_BUSY_RX)
    {
      hpssi->Instance->CR &= ~PSSI_CR_DMAEN;

      if (hpssi->hdmarx != NULL)
      {
        /* Set the PSSI DMA Abort callback :
        will lead to call HAL_PSSI_ErrorCallback() at end of DMA abort procedure */
        hpssi->hdmarx->XferAbortCallback = PSSI_DMAAbort;

        /* Abort DMA RX */
        if (HAL_DMA_Abort_IT(hpssi->hdmarx) != HAL_OK)
        {
          /* Call Directly hpssi->hdma->XferAbortCallback function in case of error */
          hpssi->hdmarx->XferAbortCallback(hpssi->hdmarx);
        }
      }
    }
    else
    {

      /* Call the error callback */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
      hpssi->ErrorCallback(hpssi);
#else
      HAL_PSSI_ErrorCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hpssi);

  /* Note : The PSSI interrupts must be enabled after unlocking current process
            to avoid the risk of PSSI interrupt handle execution before current
            process unlock */
  HAL_PSSI_ENABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);

  return HAL_OK;
}
#endif /*HAL_DMA_MODULE_ENABLED*/

/**
  * @}
  */

/** @addtogroup PSSI_IRQ_Handler_and_Callbacks IRQ Handler and Callbacks
  * @{
  */

/**
  * @brief  This function handles PSSI event interrupt request.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval None
  */
void HAL_PSSI_IRQHandler(PSSI_HandleTypeDef *hpssi)
{
  /* Overrun/ Underrun Errors */
  if (HAL_PSSI_GET_FLAG(hpssi, PSSI_FLAG_OVR_MIS) != 0U)
  {
    /* Reset handle parameters */
    hpssi->XferCount     = 0U;

    /* Disable all interrupts */
    HAL_PSSI_DISABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);

#if defined(HAL_DMA_MODULE_ENABLED)
    /* Abort DMA TX transfer if any */
    if ((hpssi->Instance->CR & PSSI_CR_DMAEN) == PSSI_CR_DMAEN)
    {
      if (hpssi->State == HAL_PSSI_STATE_BUSY_TX)
      {
        /* Set new error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_UNDER_RUN;

        hpssi->Instance->CR &= ~PSSI_CR_DMAEN;

        if (hpssi->hdmatx != NULL)
        {
          /* Set the PSSI DMA Abort callback :
          will lead to call HAL_PSSI_ErrorCallback() at end of DMA abort procedure */
          hpssi->hdmatx->XferAbortCallback = PSSI_DMAAbort;

          /* Process Unlocked */
          __HAL_UNLOCK(hpssi);

          /* Abort DMA TX */
          if (HAL_DMA_Abort_IT(hpssi->hdmatx) != HAL_OK)
          {
            /* Call Directly XferAbortCallback function in case of error */
            hpssi->hdmatx->XferAbortCallback(hpssi->hdmatx);
          }
        }
      }
      /* Abort DMA RX transfer if any */
      else if (hpssi->State == HAL_PSSI_STATE_BUSY_RX)
      {
        /* Set new error code */
        hpssi->ErrorCode |= HAL_PSSI_ERROR_OVER_RUN;

        hpssi->Instance->CR &= ~PSSI_CR_DMAEN;

        if (hpssi->hdmarx != NULL)
        {
          /* Set the PSSI DMA Abort callback :
          will lead to call HAL_PSSI_ErrorCallback() at end of DMA abort procedure */
          hpssi->hdmarx->XferAbortCallback = PSSI_DMAAbort;

          /* Process Unlocked */
          __HAL_UNLOCK(hpssi);

          /* Abort DMA RX */
          if (HAL_DMA_Abort_IT(hpssi->hdmarx) != HAL_OK)
          {
            /* Call Directly hpssi->hdma->XferAbortCallback function in case of error */
            hpssi->hdmarx->XferAbortCallback(hpssi->hdmarx);
          }
        }
      }
      else
      {
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
        /* Call the corresponding callback to inform upper layer of the error */
        hpssi->ErrorCallback(hpssi);
#else
        HAL_PSSI_ErrorCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
      }
    }
#endif /*HAL_DMA_MODULE_ENABLED*/

    /* If state is an abort treatment on going, don't change state */
    if (hpssi->State == HAL_PSSI_STATE_ABORT)
    {
      hpssi->State = HAL_PSSI_STATE_READY;

      /* Process Unlocked */
      __HAL_UNLOCK(hpssi);

#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
      /* Call the corresponding callback to inform upper layer of End of Transfer */
      hpssi->AbortCpltCallback(hpssi);
#else
      HAL_PSSI_AbortCpltCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
    }
    else
    {
      /* Set HAL_PSSI_STATE_READY */
      hpssi->State         = HAL_PSSI_STATE_READY;
      /* Process Unlocked */
      __HAL_UNLOCK(hpssi);

#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
      /* Call the corresponding callback to inform upper layer of End of Transfer */
      hpssi->ErrorCallback(hpssi);
#else
      HAL_PSSI_ErrorCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
    }
  }
}

/**
  * @brief   Tx Transfer complete callback.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval None
  */
__weak void HAL_PSSI_TxCpltCallback(PSSI_HandleTypeDef *hpssi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpssi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PSSI_TxCpltCallback can be implemented in the user file
   */
}

/**
  * @brief   Rx Transfer complete callback.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval None
  */
__weak void HAL_PSSI_RxCpltCallback(PSSI_HandleTypeDef *hpssi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpssi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PSSI_RxCpltCallback can be implemented in the user file
   */
}

/**
  * @brief  PSSI error callback.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval None
  */
__weak void HAL_PSSI_ErrorCallback(PSSI_HandleTypeDef *hpssi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpssi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PSSI_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  PSSI abort callback.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval None
  */
__weak void HAL_PSSI_AbortCpltCallback(PSSI_HandleTypeDef *hpssi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hpssi);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_PSSI_AbortCpltCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup PSSI_Exported_Functions_Group3 Peripheral State and Error functions
  *  @brief   Peripheral State, Mode and Error functions
  *
@verbatim
 ===============================================================================
            ##### Peripheral State, Mode and Error functions #####
 ===============================================================================
    [..]
    This subsection permit to get in run-time the status of the peripheral
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the PSSI handle state.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @retval HAL state
  */
HAL_PSSI_StateTypeDef HAL_PSSI_GetState(const PSSI_HandleTypeDef *hpssi)
{
  /* Return PSSI handle state */
  return hpssi->State;
}

/**
  * @brief  Return the PSSI error code.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *              the configuration information for the specified PSSI.
  * @retval PSSI Error Code
  */
uint32_t HAL_PSSI_GetError(const PSSI_HandleTypeDef *hpssi)
{
  return hpssi->ErrorCode;
}

/**
  * @}
  */

/** @defgroup PSSI_Exported_Functions_Group4 Clock Source Selection function
  *  @brief   Clock Source Selection function to pick between an internal or
  *           external clock.
  *
@verbatim
 ===============================================================================
                  ##### Clock Source Selection function #####
 ===============================================================================
    [..] This subsection provides a function allowing to:
      (+) Configure Clock source
       (++) The clock source could be external (default) or internal (the clock
            is generated by the device RCC).
       (++) The AHB clock frequency must be at least 2.5 times higher than the
            PSSI_PDCK frequency

@endverbatim
  * @{
  */

/**
  * @brief  Configure PSSI Clock Source.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI peripheral.
  * @param  ClockSource New Clock Configuration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PSSI_ClockConfig(PSSI_HandleTypeDef *hpssi, uint32_t ClockSource)
{
  /* Check the parameter */
  assert_param(IS_PSSI_CLOCK_SOURCE(ClockSource));

  if (hpssi->State == HAL_PSSI_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hpssi);

    HAL_PSSI_DISABLE(hpssi);

    /* Set CKSRC to ClockSource */
    MODIFY_REG(hpssi->Instance->CR, PSSI_CR_CKSRC, ClockSource);

    HAL_PSSI_ENABLE(hpssi);

    /* Process Unlocked */
    __HAL_UNLOCK(hpssi);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup PSSI_Private_Functions
  * @{
  */

/**
  * @brief  PSSI Errors process.
  * @param  hpssi PSSI handle.
  * @param  ErrorCode Error code to handle.
  * @retval None
  */
static void PSSI_Error(PSSI_HandleTypeDef *hpssi, uint32_t ErrorCode)
{
  /* Reset handle parameters */
  hpssi->XferCount     = 0U;

  /* Set new error code */
  hpssi->ErrorCode |= ErrorCode;

  /* Disable all interrupts */
  HAL_PSSI_DISABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);

#if defined(HAL_DMA_MODULE_ENABLED)
  /* Abort DMA TX transfer if any */
  if ((hpssi->Instance->CR & PSSI_CR_DMAEN) == PSSI_CR_DMAEN)
  {
    if (hpssi->State == HAL_PSSI_STATE_BUSY_TX)
    {
      hpssi->Instance->CR &= ~PSSI_CR_DMAEN;

      if (hpssi->hdmatx != NULL)
      {
        /* Set the PSSI DMA Abort callback :
        will lead to call HAL_PSSI_ErrorCallback() at end of DMA abort procedure */
        hpssi->hdmatx->XferAbortCallback = PSSI_DMAAbort;

        /* Process Unlocked */
        __HAL_UNLOCK(hpssi);

        /* Abort DMA TX */
        if (HAL_DMA_Abort_IT(hpssi->hdmatx) != HAL_OK)
        {
          /* Call Directly XferAbortCallback function in case of error */
          hpssi->hdmatx->XferAbortCallback(hpssi->hdmatx);
        }
      }
    }
    /* Abort DMA RX transfer if any */
    else if (hpssi->State == HAL_PSSI_STATE_BUSY_RX)
    {
      hpssi->Instance->CR &= ~PSSI_CR_DMAEN;

      if (hpssi->hdmarx != NULL)
      {
        /* Set the PSSI DMA Abort callback :
        will lead to call HAL_PSSI_ErrorCallback() at end of DMA abort procedure */
        hpssi->hdmarx->XferAbortCallback = PSSI_DMAAbort;

        /* Process Unlocked */
        __HAL_UNLOCK(hpssi);

        /* Abort DMA RX */
        if (HAL_DMA_Abort_IT(hpssi->hdmarx) != HAL_OK)
        {
          /* Call Directly hpssi->hdma->XferAbortCallback function in case of error */
          hpssi->hdmarx->XferAbortCallback(hpssi->hdmarx);
        }
      }
    }
    else
    {
      /*Nothing to do*/
    }
  }
#endif /*HAL_DMA_MODULE_ENABLED*/

  /* If state is an abort treatment on going, don't change state */
  if (hpssi->State == HAL_PSSI_STATE_ABORT)
  {
    hpssi->State = HAL_PSSI_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hpssi);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
    hpssi->AbortCpltCallback(hpssi);
#else
    HAL_PSSI_AbortCpltCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
  }
  else
  {
    /* Set HAL_PSSI_STATE_READY */
    hpssi->State = HAL_PSSI_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hpssi);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
    hpssi->ErrorCallback(hpssi);
#else
    HAL_PSSI_ErrorCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
  }
}

#if defined(HAL_DMA_MODULE_ENABLED)
/**
  * @brief  DMA PSSI slave transmit process complete callback.
  * @param  hdma DMA handle
  * @retval None
  */
void PSSI_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  /* Derogation MISRAC2012-Rule-11.5 */
  PSSI_HandleTypeDef *hpssi = (PSSI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);

  uint32_t tmperror;

  /* Disable Interrupts */
  HAL_PSSI_DISABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);

  /* Store current volatile hpssi->ErrorCode, misra rule */
  tmperror = hpssi->ErrorCode;

  /* Call the corresponding callback to inform upper layer of End of Transfer */
  if ((hpssi->State == HAL_PSSI_STATE_ABORT) || (tmperror != HAL_PSSI_ERROR_NONE))
  {
    /* Call the corresponding callback to inform upper layer of End of Transfer */
    PSSI_Error(hpssi, hpssi->ErrorCode);
  }
  /* hpssi->State == HAL_PSSI_STATE_BUSY_TX */
  else
  {
    hpssi->State = HAL_PSSI_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hpssi);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
    hpssi->TxCpltCallback(hpssi);
#else
    HAL_PSSI_TxCpltCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
  }
}

/**
  * @brief DMA PSSI master receive process complete callback.
  * @param  hdma DMA handle
  * @retval None
  */
void PSSI_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
  /* Derogation MISRAC2012-Rule-11.5 */
  PSSI_HandleTypeDef *hpssi = (PSSI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);

  uint32_t tmperror;

  /* Disable Interrupts */
  HAL_PSSI_DISABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);

  /* Store current volatile hpssi->ErrorCode, misra rule */
  tmperror = hpssi->ErrorCode;

  /* Call the corresponding callback to inform upper layer of End of Transfer */
  if ((hpssi->State == HAL_PSSI_STATE_ABORT) || (tmperror != HAL_PSSI_ERROR_NONE))
  {
    /* Call the corresponding callback to inform upper layer of End of Transfer */
    PSSI_Error(hpssi, hpssi->ErrorCode);
  }
  /* hpssi->State == HAL_PSSI_STATE_BUSY_RX */
  else
  {
    hpssi->State = HAL_PSSI_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hpssi);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
    hpssi->RxCpltCallback(hpssi);
#else
    HAL_PSSI_RxCpltCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
  }
}

/**
  * @brief DMA PSSI communication abort callback
  *        (To be called at end of DMA Abort procedure).
  * @param hdma DMA handle.
  * @retval None
  */
void PSSI_DMAAbort(DMA_HandleTypeDef *hdma)
{
  /* Derogation MISRAC2012-Rule-11.5 */
  PSSI_HandleTypeDef *hpssi = (PSSI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);

  /* Reset AbortCpltCallback */
  hpssi->hdmatx->XferAbortCallback = NULL;
  hpssi->hdmarx->XferAbortCallback = NULL;

  /* Check if come from abort from user */
  if (hpssi->State == HAL_PSSI_STATE_ABORT)
  {
    hpssi->State = HAL_PSSI_STATE_READY;

    /* Call the corresponding callback to inform upper layer of End of Transfer */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
    hpssi->AbortCpltCallback(hpssi);
#else
    HAL_PSSI_AbortCpltCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
  }
  else
  {
    /* Call the corresponding callback to inform upper layer of End of Transfer */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
    hpssi->ErrorCallback(hpssi);
#else
    HAL_PSSI_ErrorCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
  }
}
#endif /*HAL_DMA_MODULE_ENABLED*/

/**
  * @brief  This function handles PSSI Communication Timeout.
  * @param  hpssi Pointer to a PSSI_HandleTypeDef structure that contains
  *                the configuration information for the specified PSSI.
  * @param  Flag Specifies the PSSI flag to check.
  * @param  Status The new Flag status (SET or RESET).
  * @param  Timeout Timeout duration
  * @param  Tickstart Tick start value
  * @retval HAL status
  */
static HAL_StatusTypeDef PSSI_WaitOnStatusUntilTimeout(PSSI_HandleTypeDef *hpssi, uint32_t Flag, FlagStatus Status,
                                                       uint32_t Timeout, uint32_t Tickstart)
{
  while ((HAL_PSSI_GET_STATUS(hpssi, Flag) & Flag) == (uint32_t)Status)
  {
    /* Check for the Timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
      {
        hpssi->ErrorCode |= HAL_PSSI_ERROR_TIMEOUT;
        hpssi->State = HAL_PSSI_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hpssi);

        return HAL_ERROR;
      }
    }
  }
  return HAL_OK;
}

#if defined(HAL_DMA_MODULE_ENABLED)
void PSSI_DMAError(DMA_HandleTypeDef *hdma)
{
  /* Derogation MISRAC2012-Rule-11.5 */
  PSSI_HandleTypeDef *hpssi = (PSSI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);

  uint32_t tmperror;

  /* Disable the selected PSSI peripheral */
  HAL_PSSI_DISABLE(hpssi);

  /* Disable Interrupts */
  HAL_PSSI_DISABLE_IT(hpssi, PSSI_FLAG_OVR_RIS);

  /* Store current volatile hpssi->ErrorCode, misra rule */
  tmperror = hpssi->ErrorCode;

  /* Call the corresponding callback to inform upper layer of End of Transfer */
  if ((hpssi->State == HAL_PSSI_STATE_ABORT) || (tmperror != HAL_PSSI_ERROR_NONE))
  {
    /* Call the corresponding callback to inform upper layer of End of Transfer */
    PSSI_Error(hpssi, hpssi->ErrorCode);
  }
  else
  {
    hpssi->State = HAL_PSSI_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(hpssi);

    /* Call the corresponding callback to inform upper layer of End of Transfer */
#if (USE_HAL_PSSI_REGISTER_CALLBACKS == 1)
    hpssi->ErrorCallback(hpssi);
#else
    HAL_PSSI_ErrorCallback(hpssi);
#endif /* USE_HAL_PSSI_REGISTER_CALLBACKS */
  }
}
#endif /*HAL_DMA_MODULE_ENABLED*/


/**
  * @}
  */
#endif /* PSSI */
#endif /* HAL_PSSI_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */
