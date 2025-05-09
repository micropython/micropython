/**
  ******************************************************************************
  * @file    stm32n6xx_hal_fdcan.c
  * @author  MCD Application Team
  * @brief   FDCAN HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Flexible DataRate Controller Area Network
  *          (FDCAN) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *           + Peripheral Configuration and Control functions
  *           + Peripheral State and Error functions
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
      (#) Initialize the FDCAN peripheral using HAL_FDCAN_Init function.

      (#) If needed , configure the reception filters and optional features using
          the following configuration functions:
            (++) HAL_FDCAN_ConfigClockCalibration
            (++) HAL_FDCAN_ConfigFilter
            (++) HAL_FDCAN_ConfigGlobalFilter
            (++) HAL_FDCAN_ConfigExtendedIdMask
            (++) HAL_FDCAN_ConfigRxFifoOverwrite
            (++) HAL_FDCAN_ConfigFifoWatermark
            (++) HAL_FDCAN_ConfigRamWatchdog
            (++) HAL_FDCAN_ConfigTimestampCounter
            (++) HAL_FDCAN_EnableTimestampCounter
            (++) HAL_FDCAN_DisableTimestampCounter
            (++) HAL_FDCAN_ConfigTimeoutCounter
            (++) HAL_FDCAN_EnableTimeoutCounter
            (++) HAL_FDCAN_DisableTimeoutCounter
            (++) HAL_FDCAN_ConfigTxDelayCompensation
            (++) HAL_FDCAN_EnableTxDelayCompensation
            (++) HAL_FDCAN_DisableTxDelayCompensation
            (++) HAL_FDCAN_EnableISOMode
            (++) HAL_FDCAN_DisableISOMode
            (++) HAL_FDCAN_EnableEdgeFiltering
            (++) HAL_FDCAN_DisableEdgeFiltering
            (++) HAL_FDCAN_TT_ConfigOperation
            (++) HAL_FDCAN_TT_ConfigReferenceMessage
            (++) HAL_FDCAN_TT_ConfigTrigger

      (#) Start the FDCAN module using HAL_FDCAN_Start function. At this level
          the node is active on the bus: it can send and receive messages.

      (#) The following Tx control functions can only be called when the FDCAN
          module is started:
            (++) HAL_FDCAN_AddMessageToTxFifoQ
            (++) HAL_FDCAN_EnableTxBufferRequest
            (++) HAL_FDCAN_AbortTxRequest

      (#) After having submitted a Tx request in Tx Fifo or Queue, it is possible to
          get Tx buffer location used to place the Tx request thanks to
          HAL_FDCAN_GetLatestTxFifoQRequestBuffer API.
          It is then possible to abort later on the corresponding Tx Request using
          HAL_FDCAN_AbortTxRequest API.

      (#) When a message is received into the FDCAN message RAM, it can be
          retrieved using the HAL_FDCAN_GetRxMessage function.

      (#) Calling the HAL_FDCAN_Stop function stops the FDCAN module by entering
          it to initialization mode and re-enabling access to configuration
          registers through the configuration functions listed here above.

      (#) All other control functions can be called any time after initialization
          phase, no matter if the FDCAN module is started or stopped.

      *** Polling mode operation ***
      ==============================
    [..]
        (#) Reception and transmission states can be monitored via the following
            functions:
              (++) HAL_FDCAN_IsRxBufferMessageAvailable
              (++) HAL_FDCAN_IsTxBufferMessagePending
              (++) HAL_FDCAN_GetRxFifoFillLevel
              (++) HAL_FDCAN_GetTxFifoFreeLevel

      *** Interrupt mode operation ***
      ================================
      [..]
        (#) There are two interrupt lines: line 0 and 1.
            By default, all interrupts are assigned to line 0. Interrupt lines
            can be configured using HAL_FDCAN_ConfigInterruptLines function.

        (#) Notifications are activated using HAL_FDCAN_ActivateNotification
            function. Then, the process can be controlled through one of the
            available user callbacks: HAL_FDCAN_xxxCallback.

  *** Callback registration ***
  =============================================

  The compilation define USE_HAL_FDCAN_REGISTER_CALLBACKS when set to 1
  allows the user to configure dynamically the driver callbacks.
  Use Function HAL_FDCAN_RegisterCallback() or HAL_FDCAN_RegisterXXXCallback()
  to register an interrupt callback.

  Function HAL_FDCAN_RegisterCallback() allows to register following callbacks:
    (+) TxFifoEmptyCallback          : Tx Fifo Empty Callback.
    (+) RxBufferNewMessageCallback   : Rx Buffer New Message Callback.
    (+) HighPriorityMessageCallback  : High Priority Message Callback.
    (+) TimestampWraparoundCallback  : Timestamp Wraparound Callback.
    (+) TimeoutOccurredCallback      : Timeout Occurred Callback.
    (+) ErrorCallback                : Error Callback.
    (+) MspInitCallback              : FDCAN MspInit.
    (+) MspDeInitCallback            : FDCAN MspDeInit.
  This function takes as parameters the HAL peripheral handle, the Callback ID
  and a pointer to the user callback function.

  For specific callbacks ClockCalibrationCallback, TxEventFifoCallback, RxFifo0Callback, RxFifo1Callback,
  TxBufferCompleteCallback, TxBufferAbortCallback, ErrorStatusCallback, TT_ScheduleSyncCallback, TT_TimeMarkCallback,
  TT_StopWatchCallback and TT_GlobalTimeCallback, use dedicated register callbacks:
  respectively HAL_FDCAN_RegisterClockCalibrationCallback(), HAL_FDCAN_RegisterTxEventFifoCallback(),
  HAL_FDCAN_RegisterRxFifo0Callback(), HAL_FDCAN_RegisterRxFifo1Callback(),
  HAL_FDCAN_RegisterTxBufferCompleCallback(), HAL_FDCAN_RegisterTxBufferAbortCallback(),
  HAL_FDCAN_RegisterErrorStatusCallback(), HAL_FDCAN_TT_RegisterScheduleSyncCallback(),
  HAL_FDCAN_TT_RegisterTimeMarkCallback(), HAL_FDCAN_TT_RegisterStopWatchCallback() and
  HAL_FDCAN_TT_RegisterGlobalTimeCallback().

  Use function HAL_FDCAN_UnRegisterCallback() to reset a callback to the default
  weak function.
  HAL_FDCAN_UnRegisterCallback takes as parameters the HAL peripheral handle,
  and the Callback ID.
  This function allows to reset following callbacks:
    (+) TxFifoEmptyCallback          : Tx Fifo Empty Callback.
    (+) RxBufferNewMessageCallback   : Rx Buffer New Message Callback.
    (+) HighPriorityMessageCallback  : High Priority Message Callback.
    (+) TimestampWraparoundCallback  : Timestamp Wraparound Callback.
    (+) TimeoutOccurredCallback      : Timeout Occurred Callback.
    (+) ErrorCallback                : Error Callback.
    (+) MspInitCallback              : FDCAN MspInit.
    (+) MspDeInitCallback            : FDCAN MspDeInit.

  For specific callbacks ClockCalibrationCallback, TxEventFifoCallback, RxFifo0Callback,
  RxFifo1Callback, TxBufferCompleteCallback, TxBufferAbortCallback, TT_ScheduleSyncCallback,
  TT_TimeMarkCallback, TT_StopWatchCallback and TT_GlobalTimeCallback, use dedicated
  register callbacks: respectively HAL_FDCAN_UnRegisterClockCalibrationCallback(),
  HAL_FDCAN_UnRegisterTxEventFifoCallback(), HAL_FDCAN_UnRegisterRxFifo0Callback(),
  HAL_FDCAN_UnRegisterRxFifo1Callback(), HAL_FDCAN_UnRegisterTxBufferCompleCallback(),
  HAL_FDCAN_UnRegisterTxBufferAbortCallback(), HAL_FDCAN_UnRegisterErrorStatusCallback(),
  HAL_FDCAN_TT_UnRegisterScheduleSyncCallback(), HAL_FDCAN_TT_UnRegisterTimeMarkCallback(),
  HAL_FDCAN_TT_UnRegisterStopWatchCallback() and HAL_FDCAN_TT_UnRegisterGlobalTimeCallback().

  By default, after the HAL_FDCAN_Init() and when the state is HAL_FDCAN_STATE_RESET,
  all callbacks are set to the corresponding weak functions:
  examples HAL_FDCAN_ErrorCallback().
  Exception done for MspInit and MspDeInit functions that are
  reset to the legacy weak function in the HAL_FDCAN_Init()/ HAL_FDCAN_DeInit() only when
  these callbacks are null (not registered beforehand).
  if not, MspInit or MspDeInit are not null, the HAL_FDCAN_Init()/ HAL_FDCAN_DeInit()
  keep and use the user MspInit/MspDeInit callbacks (registered beforehand)

  Callbacks can be registered/unregistered in HAL_FDCAN_STATE_READY state only.
  Exception done MspInit/MspDeInit that can be registered/unregistered
  in HAL_FDCAN_STATE_READY or HAL_FDCAN_STATE_RESET state,
  thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
  In that case first register the MspInit/MspDeInit user callbacks
  using HAL_FDCAN_RegisterCallback() before calling HAL_FDCAN_DeInit()
  or HAL_FDCAN_Init() function.

  When The compilation define USE_HAL_FDCAN_REGISTER_CALLBACKS is set to 0 or
  not defined, the callback registration feature is not available and all callbacks
  are set to the corresponding weak functions.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

#if defined(FDCAN1)

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup FDCAN FDCAN
  * @brief FDCAN HAL module driver
  * @{
  */

#ifdef HAL_FDCAN_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup FDCAN_Private_Constants
  * @{
  */
#define FDCAN_TIMEOUT_VALUE 10U
#define FDCAN_TIMEOUT_COUNT 50U

#define FDCAN_TX_EVENT_FIFO_MASK (FDCAN_IR_TEFL | FDCAN_IR_TEFF | FDCAN_IR_TEFW | FDCAN_IR_TEFN)
#define FDCAN_RX_FIFO0_MASK (FDCAN_IR_RF0L | FDCAN_IR_RF0F | FDCAN_IR_RF0W | FDCAN_IR_RF0N)
#define FDCAN_RX_FIFO1_MASK (FDCAN_IR_RF1L | FDCAN_IR_RF1F | FDCAN_IR_RF1W | FDCAN_IR_RF1N)
#define FDCAN_ERROR_MASK (FDCAN_IR_ELO | FDCAN_IR_WDI | FDCAN_IR_PEA | FDCAN_IR_PED | FDCAN_IR_ARA)
#define FDCAN_ERROR_STATUS_MASK (FDCAN_IR_EP | FDCAN_IR_EW | FDCAN_IR_BO)
#define FDCAN_TT_SCHEDULE_SYNC_MASK (FDCAN_TTIR_SBC | FDCAN_TTIR_SMC | FDCAN_TTIR_CSM | FDCAN_TTIR_SOG)
#define FDCAN_TT_TIME_MARK_MASK (FDCAN_TTIR_RTMI | FDCAN_TTIR_TTMI)
#define FDCAN_TT_GLOBAL_TIME_MASK (FDCAN_TTIR_GTW | FDCAN_TTIR_GTD)
#define FDCAN_TT_DISTURBING_ERROR_MASK (FDCAN_TTIR_GTE | FDCAN_TTIR_TXU | FDCAN_TTIR_TXO | \
                                        FDCAN_TTIR_SE1 | FDCAN_TTIR_SE2 | FDCAN_TTIR_ELC)
#define FDCAN_TT_FATAL_ERROR_MASK (FDCAN_TTIR_IWT | FDCAN_TTIR_WT | FDCAN_TTIR_AW | FDCAN_TTIR_CER)

#define FDCAN_ELEMENT_MASK_STDID ((uint32_t)0x1FFC0000U) /* Standard Identifier         */
#define FDCAN_ELEMENT_MASK_EXTID ((uint32_t)0x1FFFFFFFU) /* Extended Identifier         */
#define FDCAN_ELEMENT_MASK_RTR   ((uint32_t)0x20000000U) /* Remote Transmission Request */
#define FDCAN_ELEMENT_MASK_XTD   ((uint32_t)0x40000000U) /* Extended Identifier         */
#define FDCAN_ELEMENT_MASK_ESI   ((uint32_t)0x80000000U) /* Error State Indicator       */
#define FDCAN_ELEMENT_MASK_TS    ((uint32_t)0x0000FFFFU) /* Timestamp                   */
#define FDCAN_ELEMENT_MASK_DLC   ((uint32_t)0x000F0000U) /* Data Length Code            */
#define FDCAN_ELEMENT_MASK_BRS   ((uint32_t)0x00100000U) /* Bit Rate Switch             */
#define FDCAN_ELEMENT_MASK_FDF   ((uint32_t)0x00200000U) /* FD Format                   */
#define FDCAN_ELEMENT_MASK_EFC   ((uint32_t)0x00800000U) /* Event FIFO Control          */
#define FDCAN_ELEMENT_MASK_MM    ((uint32_t)0xFF000000U) /* Message Marker              */
#define FDCAN_ELEMENT_MASK_FIDX  ((uint32_t)0x7F000000U) /* Filter Index                */
#define FDCAN_ELEMENT_MASK_ANMF  ((uint32_t)0x80000000U) /* Accepted Non-matching Frame */
#define FDCAN_ELEMENT_MASK_ET    ((uint32_t)0x00C00000U) /* Event type                  */

#define FDCAN_MESSAGE_RAM_SIZE 0x2800U
#define FDCAN_MESSAGE_RAM_END_ADDRESS (SRAMCAN_BASE + FDCAN_MESSAGE_RAM_SIZE - 0x4U) /* Message RAM width is 4 Bytes */

/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @addtogroup FDCAN_Private_Variables
  * @{
  */
static const uint8_t DLCtoBytes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/** @addtogroup FDCAN_Private_Functions_Prototypes
  * @{
  */
static HAL_StatusTypeDef FDCAN_CalcultateRamBlockAddresses(FDCAN_HandleTypeDef *hfdcan);
static void FDCAN_CopyMessageToRAM(const FDCAN_HandleTypeDef *hfdcan, const FDCAN_TxHeaderTypeDef *pTxHeader,
                                   const uint8_t *pTxData, uint32_t BufferIndex);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup FDCAN_Exported_Functions FDCAN Exported Functions
  * @{
  */

/** @defgroup FDCAN_Exported_Functions_Group1 Initialization and de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
  ==============================================================================
              ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the FDCAN.
      (+) De-initialize the FDCAN.
      (+) Enter FDCAN peripheral in power down mode.
      (+) Exit power down mode.
      (+) Register callbacks.
      (+) Unregister callbacks.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the FDCAN peripheral according to the specified
  *         parameters in the FDCAN_InitTypeDef structure.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_Init(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t tickstart;
  HAL_StatusTypeDef status;
  const uint32_t CvtEltSize[] = {0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 5, 0, 0, 0, 6, 0, 0, 0, 7};

  /* Check FDCAN handle */
  if (hfdcan == NULL)
  {
    return HAL_ERROR;
  }

  /* Check FDCAN instance */
  if (hfdcan->Instance == FDCAN1)
  {
    hfdcan->ttcan = (TTCAN_TypeDef *)((uint32_t)hfdcan->Instance + 0x100U);
  }

  /* Check function parameters */
  assert_param(IS_FDCAN_ALL_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_FRAME_FORMAT(hfdcan->Init.FrameFormat));
  assert_param(IS_FDCAN_MODE(hfdcan->Init.Mode));
  assert_param(IS_FUNCTIONAL_STATE(hfdcan->Init.AutoRetransmission));
  assert_param(IS_FUNCTIONAL_STATE(hfdcan->Init.TransmitPause));
  assert_param(IS_FUNCTIONAL_STATE(hfdcan->Init.ProtocolException));
  assert_param(IS_FDCAN_NOMINAL_PRESCALER(hfdcan->Init.NominalPrescaler));
  assert_param(IS_FDCAN_NOMINAL_SJW(hfdcan->Init.NominalSyncJumpWidth));
  assert_param(IS_FDCAN_NOMINAL_TSEG1(hfdcan->Init.NominalTimeSeg1));
  assert_param(IS_FDCAN_NOMINAL_TSEG2(hfdcan->Init.NominalTimeSeg2));
  if (hfdcan->Init.FrameFormat == FDCAN_FRAME_FD_BRS)
  {
    assert_param(IS_FDCAN_DATA_PRESCALER(hfdcan->Init.DataPrescaler));
    assert_param(IS_FDCAN_DATA_SJW(hfdcan->Init.DataSyncJumpWidth));
    assert_param(IS_FDCAN_DATA_TSEG1(hfdcan->Init.DataTimeSeg1));
    assert_param(IS_FDCAN_DATA_TSEG2(hfdcan->Init.DataTimeSeg2));
  }
  assert_param(IS_FDCAN_MAX_VALUE(hfdcan->Init.StdFiltersNbr, 128U));
  assert_param(IS_FDCAN_MAX_VALUE(hfdcan->Init.ExtFiltersNbr, 64U));
  assert_param(IS_FDCAN_MAX_VALUE(hfdcan->Init.RxFifo0ElmtsNbr, 64U));
  if (hfdcan->Init.RxFifo0ElmtsNbr > 0U)
  {
    assert_param(IS_FDCAN_DATA_SIZE(hfdcan->Init.RxFifo0ElmtSize));
  }
  assert_param(IS_FDCAN_MAX_VALUE(hfdcan->Init.RxFifo1ElmtsNbr, 64U));
  if (hfdcan->Init.RxFifo1ElmtsNbr > 0U)
  {
    assert_param(IS_FDCAN_DATA_SIZE(hfdcan->Init.RxFifo1ElmtSize));
  }
  assert_param(IS_FDCAN_MAX_VALUE(hfdcan->Init.RxBuffersNbr, 64U));
  if (hfdcan->Init.RxBuffersNbr > 0U)
  {
    assert_param(IS_FDCAN_DATA_SIZE(hfdcan->Init.RxBufferSize));
  }
  assert_param(IS_FDCAN_MAX_VALUE(hfdcan->Init.TxEventsNbr, 32U));
  assert_param(IS_FDCAN_MAX_VALUE((hfdcan->Init.TxBuffersNbr + hfdcan->Init.TxFifoQueueElmtsNbr), 32U));
  if (hfdcan->Init.TxFifoQueueElmtsNbr > 0U)
  {
    assert_param(IS_FDCAN_TX_FIFO_QUEUE_MODE(hfdcan->Init.TxFifoQueueMode));
  }
  if ((hfdcan->Init.TxBuffersNbr + hfdcan->Init.TxFifoQueueElmtsNbr) > 0U)
  {
    assert_param(IS_FDCAN_DATA_SIZE(hfdcan->Init.TxElmtSize));
  }

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
  if (hfdcan->State == HAL_FDCAN_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hfdcan->Lock = HAL_UNLOCKED;

    /* Reset callbacks to legacy functions */
    hfdcan->ClockCalibrationCallback    = HAL_FDCAN_ClockCalibrationCallback;    /* ClockCalibrationCallback    */
    hfdcan->TxEventFifoCallback         = HAL_FDCAN_TxEventFifoCallback;         /* TxEventFifoCallback         */
    hfdcan->RxFifo0Callback             = HAL_FDCAN_RxFifo0Callback;             /* RxFifo0Callback             */
    hfdcan->RxFifo1Callback             = HAL_FDCAN_RxFifo1Callback;             /* RxFifo1Callback             */
    hfdcan->TxFifoEmptyCallback         = HAL_FDCAN_TxFifoEmptyCallback;         /* TxFifoEmptyCallback         */
    hfdcan->TxBufferCompleteCallback    = HAL_FDCAN_TxBufferCompleteCallback;    /* TxBufferCompleteCallback    */
    hfdcan->TxBufferAbortCallback       = HAL_FDCAN_TxBufferAbortCallback;       /* TxBufferAbortCallback       */
    hfdcan->RxBufferNewMessageCallback  = HAL_FDCAN_RxBufferNewMessageCallback;  /* RxBufferNewMessageCallback  */
    hfdcan->HighPriorityMessageCallback = HAL_FDCAN_HighPriorityMessageCallback; /* HighPriorityMessageCallback */
    hfdcan->TimestampWraparoundCallback = HAL_FDCAN_TimestampWraparoundCallback; /* TimestampWraparoundCallback */
    hfdcan->TimeoutOccurredCallback     = HAL_FDCAN_TimeoutOccurredCallback;     /* TimeoutOccurredCallback     */
    hfdcan->ErrorCallback               = HAL_FDCAN_ErrorCallback;               /* ErrorCallback               */
    hfdcan->ErrorStatusCallback         = HAL_FDCAN_ErrorStatusCallback;         /* ErrorStatusCallback         */
    hfdcan->TT_ScheduleSyncCallback     = HAL_FDCAN_TT_ScheduleSyncCallback;     /* TT_ScheduleSyncCallback     */
    hfdcan->TT_TimeMarkCallback         = HAL_FDCAN_TT_TimeMarkCallback;         /* TT_TimeMarkCallback         */
    hfdcan->TT_StopWatchCallback        = HAL_FDCAN_TT_StopWatchCallback;        /* TT_StopWatchCallback        */
    hfdcan->TT_GlobalTimeCallback       = HAL_FDCAN_TT_GlobalTimeCallback;       /* TT_GlobalTimeCallback       */

    if (hfdcan->MspInitCallback == NULL)
    {
      hfdcan->MspInitCallback = HAL_FDCAN_MspInit;  /* Legacy weak MspInit */
    }

    /* Init the low level hardware: CLOCK, NVIC */
    hfdcan->MspInitCallback(hfdcan);
  }
#else
  if (hfdcan->State == HAL_FDCAN_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hfdcan->Lock = HAL_UNLOCKED;

    /* Init the low level hardware: CLOCK, NVIC */
    HAL_FDCAN_MspInit(hfdcan);
  }
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

  /* Exit from Sleep mode */
  CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_CSR);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check Sleep mode acknowledge */
  while ((hfdcan->Instance->CCCR & FDCAN_CCCR_CSA) == FDCAN_CCCR_CSA)
  {
    if ((HAL_GetTick() - tickstart) > FDCAN_TIMEOUT_VALUE)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

      /* Change FDCAN state */
      hfdcan->State = HAL_FDCAN_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Request initialisation */
  SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait until the INIT bit into CCCR register is set */
  while ((hfdcan->Instance->CCCR & FDCAN_CCCR_INIT) == 0U)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > FDCAN_TIMEOUT_VALUE)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

      /* Change FDCAN state */
      hfdcan->State = HAL_FDCAN_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Enable configuration change */
  SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_CCE);

  /* Set the no automatic retransmission */
  if (hfdcan->Init.AutoRetransmission == ENABLE)
  {
    CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_DAR);
  }
  else
  {
    SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_DAR);
  }

  /* Set the transmit pause feature */
  if (hfdcan->Init.TransmitPause == ENABLE)
  {
    SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_TXP);
  }
  else
  {
    CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_TXP);
  }

  /* Set the Protocol Exception Handling */
  if (hfdcan->Init.ProtocolException == ENABLE)
  {
    CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_PXHD);
  }
  else
  {
    SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_PXHD);
  }

  /* Set FDCAN Frame Format */
  MODIFY_REG(hfdcan->Instance->CCCR, FDCAN_FRAME_FD_BRS, hfdcan->Init.FrameFormat);

  /* Reset FDCAN Operation Mode */
  CLEAR_BIT(hfdcan->Instance->CCCR, (FDCAN_CCCR_TEST | FDCAN_CCCR_MON | FDCAN_CCCR_ASM));
  CLEAR_BIT(hfdcan->Instance->TEST, FDCAN_TEST_LBCK);

  /* Set FDCAN Operating Mode:
               | Normal | Restricted |    Bus     | Internal | External
               |        | Operation  | Monitoring | LoopBack | LoopBack
     CCCR.TEST |   0    |     0      |     0      |    1     |    1
     CCCR.MON  |   0    |     0      |     1      |    1     |    0
     TEST.LBCK |   0    |     0      |     0      |    1     |    1
     CCCR.ASM  |   0    |     1      |     0      |    0     |    0
  */
  if (hfdcan->Init.Mode == FDCAN_MODE_RESTRICTED_OPERATION)
  {
    /* Enable Restricted Operation mode */
    SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_ASM);
  }
  else if (hfdcan->Init.Mode != FDCAN_MODE_NORMAL)
  {
    if (hfdcan->Init.Mode != FDCAN_MODE_BUS_MONITORING)
    {
      /* Enable write access to TEST register */
      SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_TEST);

      /* Enable LoopBack mode */
      SET_BIT(hfdcan->Instance->TEST, FDCAN_TEST_LBCK);

      if (hfdcan->Init.Mode == FDCAN_MODE_INTERNAL_LOOPBACK)
      {
        SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_MON);
      }
    }
    else
    {
      /* Enable bus monitoring mode */
      SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_MON);
    }
  }
  else
  {
    /* Nothing to do: normal mode */
  }

  /* Set the nominal bit timing register */
  hfdcan->Instance->NBTP = ((((uint32_t)hfdcan->Init.NominalSyncJumpWidth - 1U) << FDCAN_NBTP_NSJW_Pos) | \
                            (((uint32_t)hfdcan->Init.NominalTimeSeg1 - 1U) << FDCAN_NBTP_NTSEG1_Pos)    | \
                            (((uint32_t)hfdcan->Init.NominalTimeSeg2 - 1U) << FDCAN_NBTP_NTSEG2_Pos)    | \
                            (((uint32_t)hfdcan->Init.NominalPrescaler - 1U) << FDCAN_NBTP_NBRP_Pos));

  /* If FD operation with BRS is selected, set the data bit timing register */
  if (hfdcan->Init.FrameFormat == FDCAN_FRAME_FD_BRS)
  {
    hfdcan->Instance->DBTP = ((((uint32_t)hfdcan->Init.DataSyncJumpWidth - 1U) << FDCAN_DBTP_DSJW_Pos)  | \
                              (((uint32_t)hfdcan->Init.DataTimeSeg1 - 1U) << FDCAN_DBTP_DTSEG1_Pos)     | \
                              (((uint32_t)hfdcan->Init.DataTimeSeg2 - 1U) << FDCAN_DBTP_DTSEG2_Pos)     | \
                              (((uint32_t)hfdcan->Init.DataPrescaler - 1U) << FDCAN_DBTP_DBRP_Pos));
  }

  if (hfdcan->Init.TxFifoQueueElmtsNbr > 0U)
  {
    /* Select between Tx FIFO and Tx Queue operation modes */
    SET_BIT(hfdcan->Instance->TXBC, hfdcan->Init.TxFifoQueueMode);
  }

  /* Configure Tx element size */
  if ((hfdcan->Init.TxBuffersNbr + hfdcan->Init.TxFifoQueueElmtsNbr) > 0U)
  {
    MODIFY_REG(hfdcan->Instance->TXESC, FDCAN_TXESC_TBDS, CvtEltSize[hfdcan->Init.TxElmtSize]);
  }

  /* Configure Rx FIFO 0 element size */
  if (hfdcan->Init.RxFifo0ElmtsNbr > 0U)
  {
    MODIFY_REG(hfdcan->Instance->RXESC, FDCAN_RXESC_F0DS,
               (CvtEltSize[hfdcan->Init.RxFifo0ElmtSize] << FDCAN_RXESC_F0DS_Pos));
  }

  /* Configure Rx FIFO 1 element size */
  if (hfdcan->Init.RxFifo1ElmtsNbr > 0U)
  {
    MODIFY_REG(hfdcan->Instance->RXESC, FDCAN_RXESC_F1DS,
               (CvtEltSize[hfdcan->Init.RxFifo1ElmtSize] << FDCAN_RXESC_F1DS_Pos));
  }

  /* Configure Rx buffer element size */
  if (hfdcan->Init.RxBuffersNbr > 0U)
  {
    MODIFY_REG(hfdcan->Instance->RXESC, FDCAN_RXESC_RBDS,
               (CvtEltSize[hfdcan->Init.RxBufferSize] << FDCAN_RXESC_RBDS_Pos));
  }

  /* By default operation mode is set to Event-driven communication.
     If Time-triggered communication is needed, user should call the
     HAL_FDCAN_TT_ConfigOperation function just after the HAL_FDCAN_Init */
  if (hfdcan->Instance == FDCAN1)
  {
    CLEAR_BIT(hfdcan->ttcan->TTOCF, FDCAN_TTOCF_OM);
  }

  /* Initialize the Latest Tx FIFO/Queue request buffer index */
  hfdcan->LatestTxFifoQRequest = 0U;

  /* Initialize the error code */
  hfdcan->ErrorCode = HAL_FDCAN_ERROR_NONE;

  /* Initialize the FDCAN state */
  hfdcan->State = HAL_FDCAN_STATE_READY;

  /* Calculate each RAM block address */
  status = FDCAN_CalcultateRamBlockAddresses(hfdcan);

  /* Return function status */
  return status;
}

/**
  * @brief  Deinitializes the FDCAN peripheral registers to their default reset values.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_DeInit(FDCAN_HandleTypeDef *hfdcan)
{
  /* Check FDCAN handle */
  if (hfdcan == NULL)
  {
    return HAL_ERROR;
  }

  /* Check function parameters */
  assert_param(IS_FDCAN_ALL_INSTANCE(hfdcan->Instance));

  /* Stop the FDCAN module: return value is voluntary ignored */
  (void)HAL_FDCAN_Stop(hfdcan);

  /* Disable Interrupt lines */
  CLEAR_BIT(hfdcan->Instance->ILE, (FDCAN_INTERRUPT_LINE0 | FDCAN_INTERRUPT_LINE1));

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
  if (hfdcan->MspDeInitCallback == NULL)
  {
    hfdcan->MspDeInitCallback = HAL_FDCAN_MspDeInit; /* Legacy weak MspDeInit */
  }

  /* DeInit the low level hardware: CLOCK, NVIC */
  hfdcan->MspDeInitCallback(hfdcan);
#else
  /* DeInit the low level hardware: CLOCK, NVIC */
  HAL_FDCAN_MspDeInit(hfdcan);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

  /* Reset the FDCAN ErrorCode */
  hfdcan->ErrorCode = HAL_FDCAN_ERROR_NONE;

  /* Change FDCAN state */
  hfdcan->State = HAL_FDCAN_STATE_RESET;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the FDCAN MSP.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
__weak void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes the FDCAN MSP.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
__weak void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *hfdcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_MspDeInit could be implemented in the user file
   */
}

/**
  * @brief  Enter FDCAN peripheral in sleep mode.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_EnterPowerDownMode(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t tickstart;

  /* Request clock stop */
  SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_CSR);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait until FDCAN is ready for power down */
  while ((hfdcan->Instance->CCCR & FDCAN_CCCR_CSA) == 0U)
  {
    if ((HAL_GetTick() - tickstart) > FDCAN_TIMEOUT_VALUE)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

      /* Change FDCAN state */
      hfdcan->State = HAL_FDCAN_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Exit power down mode.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ExitPowerDownMode(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t tickstart;

  /* Reset clock stop request */
  CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_CSR);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait until FDCAN exits sleep mode */
  while ((hfdcan->Instance->CCCR & FDCAN_CCCR_CSA) == FDCAN_CCCR_CSA)
  {
    if ((HAL_GetTick() - tickstart) > FDCAN_TIMEOUT_VALUE)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

      /* Change FDCAN state */
      hfdcan->State = HAL_FDCAN_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Enter normal operation */
  CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);

  /* Return function status */
  return HAL_OK;
}

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
/**
  * @brief  Register a FDCAN CallBack.
  *         To be used instead of the weak predefined callback
  * @param  hfdcan pointer to a FDCAN_HandleTypeDef structure that contains
  *         the configuration information for FDCAN module
  * @param  CallbackID ID of the callback to be registered
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_FDCAN_TX_FIFO_EMPTY_CB_ID Tx Fifo Empty callback ID
  *           @arg @ref HAL_FDCAN_RX_BUFFER_NEW_MSG_CB_ID Rx buffer new message callback ID
  *           @arg @ref HAL_FDCAN_HIGH_PRIO_MESSAGE_CB_ID High priority message callback ID
  *           @arg @ref HAL_FDCAN_TIMESTAMP_WRAPAROUND_CB_ID Timestamp wraparound callback ID
  *           @arg @ref HAL_FDCAN_TIMEOUT_OCCURRED_CB_ID Timeout occurred callback ID
  *           @arg @ref HAL_FDCAN_ERROR_CALLBACK_CB_ID Error callback ID
  *           @arg @ref HAL_FDCAN_MSPINIT_CB_ID MspInit callback ID
  *           @arg @ref HAL_FDCAN_MSPDEINIT_CB_ID MspDeInit callback ID
  * @param  pCallback pointer to the Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterCallback(FDCAN_HandleTypeDef *hfdcan, HAL_FDCAN_CallbackIDTypeDef CallbackID,
                                             void (* pCallback)(FDCAN_HandleTypeDef *_hFDCAN))
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_FDCAN_TX_FIFO_EMPTY_CB_ID :
        hfdcan->TxFifoEmptyCallback = pCallback;
        break;

      case HAL_FDCAN_RX_BUFFER_NEW_MSG_CB_ID :
        hfdcan->RxBufferNewMessageCallback = pCallback;
        break;

      case HAL_FDCAN_HIGH_PRIO_MESSAGE_CB_ID :
        hfdcan->HighPriorityMessageCallback = pCallback;
        break;

      case HAL_FDCAN_TIMESTAMP_WRAPAROUND_CB_ID :
        hfdcan->TimestampWraparoundCallback = pCallback;
        break;

      case HAL_FDCAN_TIMEOUT_OCCURRED_CB_ID :
        hfdcan->TimeoutOccurredCallback = pCallback;
        break;

      case HAL_FDCAN_ERROR_CALLBACK_CB_ID :
        hfdcan->ErrorCallback = pCallback;
        break;

      case HAL_FDCAN_MSPINIT_CB_ID :
        hfdcan->MspInitCallback = pCallback;
        break;

      case HAL_FDCAN_MSPDEINIT_CB_ID :
        hfdcan->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hfdcan->State == HAL_FDCAN_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_FDCAN_MSPINIT_CB_ID :
        hfdcan->MspInitCallback = pCallback;
        break;

      case HAL_FDCAN_MSPDEINIT_CB_ID :
        hfdcan->MspDeInitCallback = pCallback;
        break;

      default :
        /* Update the error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a FDCAN CallBack.
  *         FDCAN callback is redirected to the weak predefined callback
  * @param  hfdcan pointer to a FDCAN_HandleTypeDef structure that contains
  *         the configuration information for FDCAN module
  * @param  CallbackID ID of the callback to be unregistered
  *         This parameter can be one of the following values:
  *           @arg @ref HAL_FDCAN_TX_FIFO_EMPTY_CB_ID Tx Fifo Empty callback ID
  *           @arg @ref HAL_FDCAN_RX_BUFFER_NEW_MSG_CB_ID Rx buffer new message callback ID
  *           @arg @ref HAL_FDCAN_HIGH_PRIO_MESSAGE_CB_ID High priority message callback ID
  *           @arg @ref HAL_FDCAN_TIMESTAMP_WRAPAROUND_CB_ID Timestamp wraparound callback ID
  *           @arg @ref HAL_FDCAN_TIMEOUT_OCCURRED_CB_ID Timeout occurred callback ID
  *           @arg @ref HAL_FDCAN_ERROR_CALLBACK_CB_ID Error callback ID
  *           @arg @ref HAL_FDCAN_MSPINIT_CB_ID MspInit callback ID
  *           @arg @ref HAL_FDCAN_MSPDEINIT_CB_ID MspDeInit callback ID
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterCallback(FDCAN_HandleTypeDef *hfdcan, HAL_FDCAN_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    switch (CallbackID)
    {
      case HAL_FDCAN_TX_FIFO_EMPTY_CB_ID :
        hfdcan->TxFifoEmptyCallback = HAL_FDCAN_TxFifoEmptyCallback;
        break;

      case HAL_FDCAN_RX_BUFFER_NEW_MSG_CB_ID :
        hfdcan->RxBufferNewMessageCallback = HAL_FDCAN_RxBufferNewMessageCallback;
        break;

      case HAL_FDCAN_HIGH_PRIO_MESSAGE_CB_ID :
        hfdcan->HighPriorityMessageCallback = HAL_FDCAN_HighPriorityMessageCallback;
        break;

      case HAL_FDCAN_TIMESTAMP_WRAPAROUND_CB_ID :
        hfdcan->TimestampWraparoundCallback = HAL_FDCAN_TimestampWraparoundCallback;
        break;

      case HAL_FDCAN_TIMEOUT_OCCURRED_CB_ID :
        hfdcan->TimeoutOccurredCallback = HAL_FDCAN_TimeoutOccurredCallback;
        break;

      case HAL_FDCAN_ERROR_CALLBACK_CB_ID :
        hfdcan->ErrorCallback = HAL_FDCAN_ErrorCallback;
        break;

      case HAL_FDCAN_MSPINIT_CB_ID :
        hfdcan->MspInitCallback = HAL_FDCAN_MspInit;
        break;

      case HAL_FDCAN_MSPDEINIT_CB_ID :
        hfdcan->MspDeInitCallback = HAL_FDCAN_MspDeInit;
        break;

      default :
        /* Update the error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (hfdcan->State == HAL_FDCAN_STATE_RESET)
  {
    switch (CallbackID)
    {
      case HAL_FDCAN_MSPINIT_CB_ID :
        hfdcan->MspInitCallback = HAL_FDCAN_MspInit;
        break;

      case HAL_FDCAN_MSPDEINIT_CB_ID :
        hfdcan->MspDeInitCallback = HAL_FDCAN_MspDeInit;
        break;

      default :
        /* Update the error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

        /* Return error status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register Clock Calibration FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_ClockCalibrationCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the Clock Calibration Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterClockCalibrationCallback(FDCAN_HandleTypeDef *hfdcan,
                                                             pFDCAN_ClockCalibrationCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->ClockCalibrationCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the Clock Calibration FDCAN Callback
  *         Clock Calibration FDCAN Callback is redirected to the weak
  *         HAL_FDCAN_ClockCalibrationCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterClockCalibrationCallback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->ClockCalibrationCallback = HAL_FDCAN_ClockCalibrationCallback; /* Legacy weak ClockCalibrationCallback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register Tx Event Fifo FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_TxEventFifoCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the Tx Event Fifo Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterTxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan,
                                                        pFDCAN_TxEventFifoCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TxEventFifoCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the Tx Event Fifo FDCAN Callback
  *         Tx Event Fifo FDCAN Callback is redirected to the weak HAL_FDCAN_TxEventFifoCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TxEventFifoCallback = HAL_FDCAN_TxEventFifoCallback; /* Legacy weak TxEventFifoCallback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register Rx Fifo 0 FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_RxFifo0Callback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the Rx Fifo 0 Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterRxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                                                    pFDCAN_RxFifo0CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->RxFifo0Callback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the Rx Fifo 0 FDCAN Callback
  *         Rx Fifo 0 FDCAN Callback is redirected to the weak HAL_FDCAN_RxFifo0Callback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterRxFifo0Callback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->RxFifo0Callback = HAL_FDCAN_RxFifo0Callback; /* Legacy weak RxFifo0Callback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register Rx Fifo 1 FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_RxFifo1Callback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the Rx Fifo 1 Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterRxFifo1Callback(FDCAN_HandleTypeDef *hfdcan,
                                                    pFDCAN_RxFifo1CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->RxFifo1Callback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the Rx Fifo 1 FDCAN Callback
  *         Rx Fifo 1 FDCAN Callback is redirected to the weak HAL_FDCAN_RxFifo1Callback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterRxFifo1Callback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->RxFifo1Callback = HAL_FDCAN_RxFifo1Callback; /* Legacy weak RxFifo1Callback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register Tx Buffer Complete FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_TxBufferCompleteCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the Tx Buffer Complete Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterTxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan,
                                                             pFDCAN_TxBufferCompleteCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TxBufferCompleteCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the Tx Buffer Complete FDCAN Callback
  *         Tx Buffer Complete FDCAN Callback is redirected to
  *         the weak HAL_FDCAN_TxBufferCompleteCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TxBufferCompleteCallback = HAL_FDCAN_TxBufferCompleteCallback; /* Legacy weak TxBufferCompleteCallback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register Tx Buffer Abort FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_TxBufferAbortCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the Tx Buffer Abort Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterTxBufferAbortCallback(FDCAN_HandleTypeDef *hfdcan,
                                                          pFDCAN_TxBufferAbortCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TxBufferAbortCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the Tx Buffer Abort FDCAN Callback
  *         Tx Buffer Abort FDCAN Callback is redirected to
  *         the weak HAL_FDCAN_TxBufferAbortCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTxBufferAbortCallback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TxBufferAbortCallback = HAL_FDCAN_TxBufferAbortCallback; /* Legacy weak TxBufferAbortCallback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register Error Status FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_ErrorStatusCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the Error Status Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan,
                                                        pFDCAN_ErrorStatusCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->ErrorStatusCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the Error Status FDCAN Callback
  *         Error Status FDCAN Callback is redirected to the weak HAL_FDCAN_ErrorStatusCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->ErrorStatusCallback = HAL_FDCAN_ErrorStatusCallback; /* Legacy weak ErrorStatusCallback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register TT Schedule Synchronization FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_TT_ScheduleSyncCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the TT Schedule Synchronization Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterTTScheduleSyncCallback(FDCAN_HandleTypeDef *hfdcan,
                                                           pFDCAN_TT_ScheduleSyncCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TT_ScheduleSyncCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the TT Schedule Synchronization FDCAN Callback
  *         TT Schedule Synchronization Callback is redirected to the weak
  *         HAL_FDCAN_TT_ScheduleSyncCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTTScheduleSyncCallback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TT_ScheduleSyncCallback = HAL_FDCAN_TT_ScheduleSyncCallback; /* Legacy weak TT_ScheduleSyncCallback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register TT Time Mark FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_TT_TimeMarkCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the TT Time Mark Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterTTTimeMarkCallback(FDCAN_HandleTypeDef *hfdcan,
                                                       pFDCAN_TT_TimeMarkCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TT_TimeMarkCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the TT Time Mark FDCAN Callback
  *         TT Time Mark Callback is redirected to the weak HAL_FDCAN_TT_TimeMarkCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTTTimeMarkCallback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TT_TimeMarkCallback = HAL_FDCAN_TT_TimeMarkCallback; /* Legacy weak TT_TimeMarkCallback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register TT Stop Watch FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_TT_StopWatchCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the TT Stop Watch Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterTTStopWatchCallback(FDCAN_HandleTypeDef *hfdcan,
                                                        pFDCAN_TT_StopWatchCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TT_StopWatchCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the TT Stop Watch FDCAN Callback
  *         TT Stop Watch Callback is redirected to the weak HAL_FDCAN_TT_StopWatchCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTTStopWatchCallback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TT_StopWatchCallback = HAL_FDCAN_TT_StopWatchCallback; /* Legacy weak TT_StopWatchCallback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register TT Global Time FDCAN Callback
  *         To be used instead of the weak HAL_FDCAN_TT_GlobalTimeCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @param  pCallback pointer to the TT Global Time Callback function
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_RegisterTTGlobalTimeCallback(FDCAN_HandleTypeDef *hfdcan,
                                                         pFDCAN_TT_GlobalTimeCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (pCallback == NULL)
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;
    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TT_GlobalTimeCallback = pCallback;
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  UnRegister the TT Global Time FDCAN Callback
  *         TT Global Time Callback is redirected to the weak HAL_FDCAN_TT_GlobalTimeCallback() predefined callback
  * @param  hfdcan FDCAN handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTTGlobalTimeCallback(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    hfdcan->TT_GlobalTimeCallback = HAL_FDCAN_TT_GlobalTimeCallback; /* Legacy weak TT_GlobalTimeCallback  */
  }
  else
  {
    /* Update the error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_INVALID_CALLBACK;

    /* Return error status */
    status =  HAL_ERROR;
  }

  return status;
}

#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup FDCAN_Exported_Functions_Group2 Configuration functions
  *  @brief    FDCAN Configuration functions.
  *
@verbatim
  ==============================================================================
              ##### Configuration functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) HAL_FDCAN_ConfigClockCalibration        : Configure the FDCAN clock calibration unit
        (+) HAL_FDCAN_GetClockCalibrationState      : Get the clock calibration state
        (+) HAL_FDCAN_ResetClockCalibrationState    : Reset the clock calibration state
        (+) HAL_FDCAN_GetClockCalibrationCounter    : Get the clock calibration counters values
      (+) HAL_FDCAN_ConfigFilter                  : Configure the FDCAN reception filters
      (+) HAL_FDCAN_ConfigGlobalFilter            : Configure the FDCAN global filter
      (+) HAL_FDCAN_ConfigExtendedIdMask          : Configure the extended ID mask
      (+) HAL_FDCAN_ConfigRxFifoOverwrite         : Configure the Rx FIFO operation mode
      (+) HAL_FDCAN_ConfigFifoWatermark           : Configure the FIFO watermark
      (+) HAL_FDCAN_ConfigRamWatchdog             : Configure the RAM watchdog
      (+) HAL_FDCAN_ConfigTimestampCounter        : Configure the timestamp counter
        (+) HAL_FDCAN_EnableTimestampCounter        : Enable the timestamp counter
        (+) HAL_FDCAN_DisableTimestampCounter       : Disable the timestamp counter
        (+) HAL_FDCAN_GetTimestampCounter           : Get the timestamp counter value
        (+) HAL_FDCAN_ResetTimestampCounter         : Reset the timestamp counter to zero
      (+) HAL_FDCAN_ConfigTimeoutCounter          : Configure the timeout counter
        (+) HAL_FDCAN_EnableTimeoutCounter          : Enable the timeout counter
        (+) HAL_FDCAN_DisableTimeoutCounter         : Disable the timeout counter
        (+) HAL_FDCAN_GetTimeoutCounter             : Get the timeout counter value
        (+) HAL_FDCAN_ResetTimeoutCounter           : Reset the timeout counter to its start value
      (+) HAL_FDCAN_ConfigTxDelayCompensation     : Configure the transmitter delay compensation
        (+) HAL_FDCAN_EnableTxDelayCompensation     : Enable the transmitter delay compensation
        (+) HAL_FDCAN_DisableTxDelayCompensation    : Disable the transmitter delay compensation
      (+) HAL_FDCAN_EnableISOMode                 : Enable ISO 11898-1 protocol mode
      (+) HAL_FDCAN_DisableISOMode                : Disable ISO 11898-1 protocol mode
      (+) HAL_FDCAN_EnableEdgeFiltering           : Enable edge filtering during bus integration
      (+) HAL_FDCAN_DisableEdgeFiltering          : Disable edge filtering during bus integration

@endverbatim
  * @{
  */

/**
  * @brief  Configure the FDCAN clock calibration unit according to the specified
  *         parameters in the FDCAN_ClkCalUnitTypeDef structure.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  sCcuConfig pointer to an FDCAN_ClkCalUnitTypeDef structure that
  *         contains the clock calibration information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigClockCalibration(FDCAN_HandleTypeDef *hfdcan,
                                                   const FDCAN_ClkCalUnitTypeDef *sCcuConfig)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_CLOCK_CALIBRATION(sCcuConfig->ClockCalibration));
  if (sCcuConfig->ClockCalibration == FDCAN_CLOCK_CALIBRATION_DISABLE)
  {
    assert_param(IS_FDCAN_CKDIV(sCcuConfig->ClockDivider));
  }
  else
  {
    assert_param(IS_FDCAN_MAX_VALUE(sCcuConfig->MinOscClkPeriods, 0xFFU));
    assert_param(IS_FDCAN_CALIBRATION_FIELD_LENGTH(sCcuConfig->CalFieldLength));
    assert_param(IS_FDCAN_MIN_VALUE(sCcuConfig->TimeQuantaPerBitTime, 4U));
    assert_param(IS_FDCAN_MAX_VALUE(sCcuConfig->TimeQuantaPerBitTime, 0x25U));
    assert_param(IS_FDCAN_MAX_VALUE(sCcuConfig->WatchdogStartValue, 0xFFFFU));
  }

  /* FDCAN1 should be initialized in order to use clock calibration */
  if (hfdcan->Instance != FDCAN1)
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    if (sCcuConfig->ClockCalibration == FDCAN_CLOCK_CALIBRATION_DISABLE)
    {
      /* Bypass clock calibration */
      SET_BIT(FDCAN_CCU->CCFG, FDCANCCU_CCFG_BCC);

      /* Configure clock divider */
      MODIFY_REG(FDCAN_CCU->CCFG, FDCANCCU_CCFG_CDIV,
                 (sCcuConfig->ClockDivider << FDCANCCU_CCFG_CDIV_Pos));
    }
    else /* sCcuConfig->ClockCalibration == ENABLE */
    {
      /* Clock calibration unit generates time quanta clock */
      CLEAR_BIT(FDCAN_CCU->CCFG, FDCANCCU_CCFG_BCC);

      /* Configure clock calibration unit */
      MODIFY_REG(FDCAN_CCU->CCFG,
                 (FDCANCCU_CCFG_TQBT | FDCANCCU_CCFG_CFL | FDCANCCU_CCFG_OCPM),
                 ((sCcuConfig->TimeQuantaPerBitTime << FDCANCCU_CCFG_TQBT_Pos) |
                  sCcuConfig->CalFieldLength | (sCcuConfig->MinOscClkPeriods << FDCANCCU_CCFG_OCPM_Pos)));

      /* Configure the start value of the calibration watchdog counter */
      MODIFY_REG(FDCAN_CCU->CWD, FDCANCCU_CWD_WDC, sCcuConfig->WatchdogStartValue);
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Get the clock calibration state.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval State clock calibration state (can be a value of @arg FDCAN_calibration_state)
  */
uint32_t HAL_FDCAN_GetClockCalibrationState(const FDCAN_HandleTypeDef *hfdcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);

  return (FDCAN_CCU->CSTAT & FDCANCCU_CSTAT_CALS);
}

/**
  * @brief  Reset the clock calibration state.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ResetClockCalibrationState(FDCAN_HandleTypeDef *hfdcan)
{
  /* FDCAN1 should be initialized in order to use clock calibration */
  if (hfdcan->Instance != FDCAN1)
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

    return HAL_ERROR;
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Calibration software reset */
    SET_BIT(FDCAN_CCU->CCFG, FDCANCCU_CCFG_SWR);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Get the clock calibration counter value.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  Counter clock calibration counter.
  *         This parameter can be a value of @arg FDCAN_calibration_counter.
  * @retval Value clock calibration counter value
  */
uint32_t HAL_FDCAN_GetClockCalibrationCounter(const FDCAN_HandleTypeDef *hfdcan, uint32_t Counter)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);

  /* Check function parameters */
  assert_param(IS_FDCAN_CALIBRATION_COUNTER(Counter));

  if (Counter == FDCAN_CALIB_TIME_QUANTA_COUNTER)
  {
    return ((FDCAN_CCU->CSTAT & FDCANCCU_CSTAT_TQC) >> FDCANCCU_CSTAT_TQC_Pos);
  }
  else if (Counter == FDCAN_CALIB_CLOCK_PERIOD_COUNTER)
  {
    return (FDCAN_CCU->CSTAT & FDCANCCU_CSTAT_OCPC);
  }
  else /* Counter == FDCAN_CALIB_WATCHDOG_COUNTER */
  {
    return ((FDCAN_CCU->CWD & FDCANCCU_CWD_WDV) >> FDCANCCU_CWD_WDV_Pos);
  }
}

/**
  * @brief  Configure the FDCAN reception filter according to the specified
  *         parameters in the FDCAN_FilterTypeDef structure.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  sFilterConfig pointer to an FDCAN_FilterTypeDef structure that
  *         contains the filter configuration information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigFilter(FDCAN_HandleTypeDef *hfdcan, const FDCAN_FilterTypeDef *sFilterConfig)
{
  uint32_t FilterElementW1;
  uint32_t FilterElementW2;
  uint32_t *FilterAddress;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Check function parameters */
    assert_param(IS_FDCAN_ID_TYPE(sFilterConfig->IdType));
    assert_param(IS_FDCAN_FILTER_CFG(sFilterConfig->FilterConfig));
    if (sFilterConfig->FilterConfig == FDCAN_FILTER_TO_RXBUFFER)
    {
      assert_param(IS_FDCAN_MAX_VALUE(sFilterConfig->RxBufferIndex, 63U));
      assert_param(IS_FDCAN_MAX_VALUE(sFilterConfig->IsCalibrationMsg, 1U));
    }

    if (sFilterConfig->IdType == FDCAN_STANDARD_ID)
    {
      /* Check function parameters */
      assert_param(IS_FDCAN_MAX_VALUE(sFilterConfig->FilterIndex, (hfdcan->Init.StdFiltersNbr - 1U)));
      assert_param(IS_FDCAN_MAX_VALUE(sFilterConfig->FilterID1, 0x7FFU));
      if (sFilterConfig->FilterConfig != FDCAN_FILTER_TO_RXBUFFER)
      {
        assert_param(IS_FDCAN_MAX_VALUE(sFilterConfig->FilterID2, 0x7FFU));
        assert_param(IS_FDCAN_STD_FILTER_TYPE(sFilterConfig->FilterType));
      }

      /* Build filter element */
      if (sFilterConfig->FilterConfig == FDCAN_FILTER_TO_RXBUFFER)
      {
        FilterElementW1 = ((FDCAN_FILTER_TO_RXBUFFER << 27U)       |
                           (sFilterConfig->FilterID1 << 16U)       |
                           (sFilterConfig->IsCalibrationMsg << 8U) |
                           sFilterConfig->RxBufferIndex);
      }
      else
      {
        FilterElementW1 = ((sFilterConfig->FilterType << 30U)   |
                           (sFilterConfig->FilterConfig << 27U) |
                           (sFilterConfig->FilterID1 << 16U)    |
                           sFilterConfig->FilterID2);
      }

      /* Calculate filter address */
      FilterAddress = (uint32_t *)(hfdcan->msgRam.StandardFilterSA + (sFilterConfig->FilterIndex * 4U));

      /* Write filter element to the message RAM */
      *FilterAddress = FilterElementW1;
    }
    else /* sFilterConfig->IdType == FDCAN_EXTENDED_ID */
    {
      /* Check function parameters */
      assert_param(IS_FDCAN_MAX_VALUE(sFilterConfig->FilterIndex, (hfdcan->Init.ExtFiltersNbr - 1U)));
      assert_param(IS_FDCAN_MAX_VALUE(sFilterConfig->FilterID1, 0x1FFFFFFFU));
      if (sFilterConfig->FilterConfig != FDCAN_FILTER_TO_RXBUFFER)
      {
        assert_param(IS_FDCAN_MAX_VALUE(sFilterConfig->FilterID2, 0x1FFFFFFFU));
        assert_param(IS_FDCAN_EXT_FILTER_TYPE(sFilterConfig->FilterType));
      }

      /* Build first word of filter element */
      FilterElementW1 = ((sFilterConfig->FilterConfig << 29U) | sFilterConfig->FilterID1);

      /* Build second word of filter element */
      if (sFilterConfig->FilterConfig == FDCAN_FILTER_TO_RXBUFFER)
      {
        FilterElementW2 = sFilterConfig->RxBufferIndex;
      }
      else
      {
        FilterElementW2 = ((sFilterConfig->FilterType << 30U) | sFilterConfig->FilterID2);
      }

      /* Calculate filter address */
      FilterAddress = (uint32_t *)(hfdcan->msgRam.ExtendedFilterSA + (sFilterConfig->FilterIndex * 4U * 2U));

      /* Write filter element to the message RAM */
      *FilterAddress = FilterElementW1;
      FilterAddress++;
      *FilterAddress = FilterElementW2;
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure the FDCAN global filter.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  NonMatchingStd Defines how received messages with 11-bit IDs that
  *         do not match any element of the filter list are treated.
  *         This parameter can be a value of @arg FDCAN_Non_Matching_Frames.
  * @param  NonMatchingExt Defines how received messages with 29-bit IDs that
  *         do not match any element of the filter list are treated.
  *         This parameter can be a value of @arg FDCAN_Non_Matching_Frames.
  * @param  RejectRemoteStd Filter or reject all the remote 11-bit IDs frames.
  *         This parameter can be a value of @arg FDCAN_Reject_Remote_Frames.
  * @param  RejectRemoteExt Filter or reject all the remote 29-bit IDs frames.
  *         This parameter can be a value of @arg FDCAN_Reject_Remote_Frames.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigGlobalFilter(FDCAN_HandleTypeDef *hfdcan,
                                               uint32_t NonMatchingStd,
                                               uint32_t NonMatchingExt,
                                               uint32_t RejectRemoteStd,
                                               uint32_t RejectRemoteExt)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_NON_MATCHING(NonMatchingStd));
  assert_param(IS_FDCAN_NON_MATCHING(NonMatchingExt));
  assert_param(IS_FDCAN_REJECT_REMOTE(RejectRemoteStd));
  assert_param(IS_FDCAN_REJECT_REMOTE(RejectRemoteExt));

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Configure global filter */
    hfdcan->Instance->GFC = ((NonMatchingStd << FDCAN_GFC_ANFS_Pos)  |
                             (NonMatchingExt << FDCAN_GFC_ANFE_Pos)  |
                             (RejectRemoteStd << FDCAN_GFC_RRFS_Pos) |
                             (RejectRemoteExt << FDCAN_GFC_RRFE_Pos));

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure the extended ID mask.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  Mask Extended ID Mask.
  *         This parameter must be a number between 0 and 0x1FFFFFFF.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigExtendedIdMask(FDCAN_HandleTypeDef *hfdcan, uint32_t Mask)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_MAX_VALUE(Mask, 0x1FFFFFFFU));

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Configure the extended ID mask */
    hfdcan->Instance->XIDAM = Mask;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure the Rx FIFO operation mode.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  RxFifo Rx FIFO.
  *         This parameter can be one of the following values:
  *           @arg FDCAN_RX_FIFO0: Rx FIFO 0
  *           @arg FDCAN_RX_FIFO1: Rx FIFO 1
  * @param  OperationMode operation mode.
  *         This parameter can be a value of @arg FDCAN_Rx_FIFO_operation_mode.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigRxFifoOverwrite(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo, uint32_t OperationMode)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_RX_FIFO(RxFifo));
  assert_param(IS_FDCAN_RX_FIFO_MODE(OperationMode));

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    if (RxFifo == FDCAN_RX_FIFO0)
    {
      /* Select FIFO 0 Operation Mode */
      MODIFY_REG(hfdcan->Instance->RXF0C, FDCAN_RXF0C_F0OM, (OperationMode << FDCAN_RXF0C_F0OM_Pos));
    }
    else /* RxFifo == FDCAN_RX_FIFO1 */
    {
      /* Select FIFO 1 Operation Mode */
      MODIFY_REG(hfdcan->Instance->RXF1C, FDCAN_RXF1C_F1OM, (OperationMode << FDCAN_RXF1C_F1OM_Pos));
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure the FIFO watermark.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  FIFO select the FIFO to be configured.
  *         This parameter can be a value of @arg FDCAN_FIFO_watermark.
  * @param  Watermark level for FIFO watermark interrupt.
  *         This parameter must be a number between:
  *           - 0 and 32, if FIFO is FDCAN_CFG_TX_EVENT_FIFO
  *           - 0 and 64, if FIFO is FDCAN_CFG_RX_FIFO0 or FDCAN_CFG_RX_FIFO1
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigFifoWatermark(FDCAN_HandleTypeDef *hfdcan, uint32_t FIFO, uint32_t Watermark)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_FIFO_WATERMARK(FIFO));
  if (FIFO == FDCAN_CFG_TX_EVENT_FIFO)
  {
    assert_param(IS_FDCAN_MAX_VALUE(Watermark, 32U));
  }
  else /* (FIFO == FDCAN_CFG_RX_FIFO0) || (FIFO == FDCAN_CFG_RX_FIFO1) */
  {
    assert_param(IS_FDCAN_MAX_VALUE(Watermark, 64U));
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Set the level for FIFO watermark interrupt */
    if (FIFO == FDCAN_CFG_TX_EVENT_FIFO)
    {
      MODIFY_REG(hfdcan->Instance->TXEFC, FDCAN_TXEFC_EFWM, (Watermark << FDCAN_TXEFC_EFWM_Pos));
    }
    else if (FIFO == FDCAN_CFG_RX_FIFO0)
    {
      MODIFY_REG(hfdcan->Instance->RXF0C, FDCAN_RXF0C_F0WM, (Watermark << FDCAN_RXF0C_F0WM_Pos));
    }
    else /* FIFO == FDCAN_CFG_RX_FIFO1 */
    {
      MODIFY_REG(hfdcan->Instance->RXF1C, FDCAN_RXF1C_F1WM, (Watermark << FDCAN_RXF1C_F1WM_Pos));
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure the RAM watchdog.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  CounterStartValue Start value of the Message RAM Watchdog Counter,
  *         This parameter must be a number between 0x00 and 0xFF,
  *         with the reset value of 0x00 the counter is disabled.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigRamWatchdog(FDCAN_HandleTypeDef *hfdcan, uint32_t CounterStartValue)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_MAX_VALUE(CounterStartValue, 0xFFU));

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Configure the RAM watchdog counter start value */
    MODIFY_REG(hfdcan->Instance->RWD, FDCAN_RWD_WDC, CounterStartValue);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure the timestamp counter.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TimestampPrescaler Timestamp Counter Prescaler.
  *         This parameter can be a value of @arg FDCAN_Timestamp_Prescaler.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigTimestampCounter(FDCAN_HandleTypeDef *hfdcan, uint32_t TimestampPrescaler)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_TIMESTAMP_PRESCALER(TimestampPrescaler));

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Configure prescaler */
    MODIFY_REG(hfdcan->Instance->TSCC, FDCAN_TSCC_TCP, TimestampPrescaler);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable the timestamp counter.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TimestampOperation Timestamp counter operation.
  *         This parameter can be a value of @arg FDCAN_Timestamp.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_EnableTimestampCounter(FDCAN_HandleTypeDef *hfdcan, uint32_t TimestampOperation)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_TIMESTAMP(TimestampOperation));

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Enable timestamp counter */
    MODIFY_REG(hfdcan->Instance->TSCC, FDCAN_TSCC_TSS, TimestampOperation);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable the timestamp counter.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_DisableTimestampCounter(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Disable timestamp counter */
    CLEAR_BIT(hfdcan->Instance->TSCC, FDCAN_TSCC_TSS);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Get the timestamp counter value.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval Timestamp counter value
  */
uint16_t HAL_FDCAN_GetTimestampCounter(const FDCAN_HandleTypeDef *hfdcan)
{
  return (uint16_t)(hfdcan->Instance->TSCV);
}

/**
  * @brief  Reset the timestamp counter to zero.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ResetTimestampCounter(FDCAN_HandleTypeDef *hfdcan)
{
  if ((hfdcan->Instance->TSCC & FDCAN_TSCC_TSS) != FDCAN_TIMESTAMP_EXTERNAL)
  {
    /* Reset timestamp counter.
       Actually any write operation to TSCV clears the counter */
    CLEAR_REG(hfdcan->Instance->TSCV);
  }
  else
  {
    /* Update error code.
       Unable to reset external counter */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

    return HAL_ERROR;
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Configure the timeout counter.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TimeoutOperation Timeout counter operation.
  *         This parameter can be a value of @arg FDCAN_Timeout_Operation.
  * @param  TimeoutPeriod Start value of the timeout down-counter.
  *         This parameter must be a number between 0x0000 and 0xFFFF
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigTimeoutCounter(FDCAN_HandleTypeDef *hfdcan, uint32_t TimeoutOperation,
                                                 uint32_t TimeoutPeriod)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_TIMEOUT(TimeoutOperation));
  assert_param(IS_FDCAN_MAX_VALUE(TimeoutPeriod, 0xFFFFU));

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Select timeout operation and configure period */
    MODIFY_REG(hfdcan->Instance->TOCC,
               (FDCAN_TOCC_TOS | FDCAN_TOCC_TOP), (TimeoutOperation | (TimeoutPeriod << FDCAN_TOCC_TOP_Pos)));

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable the timeout counter.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_EnableTimeoutCounter(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Enable timeout counter */
    SET_BIT(hfdcan->Instance->TOCC, FDCAN_TOCC_ETOC);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable the timeout counter.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_DisableTimeoutCounter(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Disable timeout counter */
    CLEAR_BIT(hfdcan->Instance->TOCC, FDCAN_TOCC_ETOC);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Get the timeout counter value.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval Timeout counter value
  */
uint16_t HAL_FDCAN_GetTimeoutCounter(const FDCAN_HandleTypeDef *hfdcan)
{
  return (uint16_t)(hfdcan->Instance->TOCV);
}

/**
  * @brief  Reset the timeout counter to its start value.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ResetTimeoutCounter(FDCAN_HandleTypeDef *hfdcan)
{
  if ((hfdcan->Instance->TOCC & FDCAN_TOCC_TOS) == FDCAN_TIMEOUT_CONTINUOUS)
  {
    /* Reset timeout counter to start value */
    CLEAR_REG(hfdcan->Instance->TOCV);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code.
       Unable to reset counter: controlled only by FIFO empty state */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure the transmitter delay compensation.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TdcOffset Transmitter Delay Compensation Offset.
  *         This parameter must be a number between 0x00 and 0x7F.
  * @param  TdcFilter Transmitter Delay Compensation Filter Window Length.
  *         This parameter must be a number between 0x00 and 0x7F.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigTxDelayCompensation(FDCAN_HandleTypeDef *hfdcan, uint32_t TdcOffset,
                                                      uint32_t TdcFilter)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_MAX_VALUE(TdcOffset, 0x7FU));
  assert_param(IS_FDCAN_MAX_VALUE(TdcFilter, 0x7FU));

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Configure TDC offset and filter window */
    hfdcan->Instance->TDCR = ((TdcFilter << FDCAN_TDCR_TDCF_Pos) | (TdcOffset << FDCAN_TDCR_TDCO_Pos));

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable the transmitter delay compensation.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_EnableTxDelayCompensation(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Enable transmitter delay compensation */
    SET_BIT(hfdcan->Instance->DBTP, FDCAN_DBTP_TDC);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable the transmitter delay compensation.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_DisableTxDelayCompensation(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Disable transmitter delay compensation */
    CLEAR_BIT(hfdcan->Instance->DBTP, FDCAN_DBTP_TDC);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable ISO 11898-1 protocol mode.
  *         CAN FD frame format is according to ISO 11898-1 standard.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_EnableISOMode(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Disable Non ISO protocol mode */
    CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_NISO);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable ISO 11898-1 protocol mode.
  *         CAN FD frame format is according to Bosch CAN FD specification V1.0.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_DisableISOMode(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Enable Non ISO protocol mode */
    SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_NISO);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable edge filtering during bus integration.
  *         Two consecutive dominant tq are required to detect an edge for hard synchronization.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_EnableEdgeFiltering(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Enable edge filtering */
    SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_EFBI);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable edge filtering during bus integration.
  *         One dominant tq is required to detect an edge for hard synchronization.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_DisableEdgeFiltering(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Disable edge filtering */
    CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_EFBI);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @}
  */

/** @defgroup FDCAN_Exported_Functions_Group3 Control functions
  *  @brief    Control functions
  *
@verbatim
  ==============================================================================
                          ##### Control functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) HAL_FDCAN_Start                         : Start the FDCAN module
      (+) HAL_FDCAN_Stop                          : Stop the FDCAN module and enable access to configuration registers
      (+) HAL_FDCAN_AddMessageToTxFifoQ           : Add a message to the Tx FIFO/Queue and activate the corresponding
                                                    transmission request
      (+) HAL_FDCAN_AddMessageToTxBuffer          : Add a message to a dedicated Tx buffer
      (+) HAL_FDCAN_EnableTxBufferRequest         : Enable transmission request
      (+) HAL_FDCAN_GetLatestTxFifoQRequestBuffer : Get Tx buffer index of latest Tx FIFO/Queue request
      (+) HAL_FDCAN_AbortTxRequest                : Abort transmission request
      (+) HAL_FDCAN_GetRxMessage                  : Get an FDCAN frame from the Rx Buffer/FIFO zone into the
                                                    message RAM
      (+) HAL_FDCAN_GetTxEvent                    : Get an FDCAN Tx event from the Tx Event FIFO zone
                                                    into the message RAM
      (+) HAL_FDCAN_GetHighPriorityMessageStatus  : Get high priority message status
      (+) HAL_FDCAN_GetProtocolStatus             : Get protocol status
      (+) HAL_FDCAN_GetErrorCounters              : Get error counter values
      (+) HAL_FDCAN_IsRxBufferMessageAvailable    : Check if a new message is received in the selected Rx buffer
      (+) HAL_FDCAN_IsTxBufferMessagePending      : Check if a transmission request is pending
                                                    on the selected Tx buffer
      (+) HAL_FDCAN_GetRxFifoFillLevel            : Return Rx FIFO fill level
      (+) HAL_FDCAN_GetTxFifoFreeLevel            : Return Tx FIFO free level
      (+) HAL_FDCAN_IsRestrictedOperationMode     : Check if the FDCAN peripheral entered Restricted Operation Mode
      (+) HAL_FDCAN_ExitRestrictedOperationMode   : Exit Restricted Operation Mode

@endverbatim
  * @{
  */

/**
  * @brief  Start the FDCAN module.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_Start(FDCAN_HandleTypeDef *hfdcan)
{
  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Change FDCAN peripheral state */
    hfdcan->State = HAL_FDCAN_STATE_BUSY;

    /* Request leave initialisation */
    CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);

    /* Reset the FDCAN ErrorCode */
    hfdcan->ErrorCode = HAL_FDCAN_ERROR_NONE;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Stop the FDCAN module and enable access to configuration registers.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_Stop(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;

  if (hfdcan->State == HAL_FDCAN_STATE_BUSY)
  {
    /* Request initialisation */
    SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);

    /* Wait until the INIT bit into CCCR register is set */
    while ((hfdcan->Instance->CCCR & FDCAN_CCCR_INIT) == 0U)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Reset counter */
    Counter = 0U;

    /* Exit from Sleep mode */
    CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_CSR);

    /* Wait until FDCAN exits sleep mode */
    while ((hfdcan->Instance->CCCR & FDCAN_CCCR_CSA) == FDCAN_CCCR_CSA)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Enable configuration change */
    SET_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_CCE);

    /* Reset Latest Tx FIFO/Queue Request Buffer Index */
    hfdcan->LatestTxFifoQRequest = 0U;

    /* Change FDCAN peripheral state */
    hfdcan->State = HAL_FDCAN_STATE_READY;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_STARTED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Add a message to the Tx FIFO/Queue and activate the corresponding transmission request
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  pTxHeader pointer to a FDCAN_TxHeaderTypeDef structure.
  * @param  pTxData pointer to a buffer containing the payload of the Tx frame.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxFifoQ(FDCAN_HandleTypeDef *hfdcan, const FDCAN_TxHeaderTypeDef *pTxHeader,
                                                const uint8_t *pTxData)
{
  uint32_t PutIndex;

  /* Check function parameters */
  assert_param(IS_FDCAN_ID_TYPE(pTxHeader->IdType));
  if (pTxHeader->IdType == FDCAN_STANDARD_ID)
  {
    assert_param(IS_FDCAN_MAX_VALUE(pTxHeader->Identifier, 0x7FFU));
  }
  else /* pTxHeader->IdType == FDCAN_EXTENDED_ID */
  {
    assert_param(IS_FDCAN_MAX_VALUE(pTxHeader->Identifier, 0x1FFFFFFFU));
  }
  assert_param(IS_FDCAN_FRAME_TYPE(pTxHeader->TxFrameType));
  assert_param(IS_FDCAN_DLC(pTxHeader->DataLength));
  assert_param(IS_FDCAN_ESI(pTxHeader->ErrorStateIndicator));
  assert_param(IS_FDCAN_BRS(pTxHeader->BitRateSwitch));
  assert_param(IS_FDCAN_FDF(pTxHeader->FDFormat));
  assert_param(IS_FDCAN_EFC(pTxHeader->TxEventFifoControl));
  assert_param(IS_FDCAN_MAX_VALUE(pTxHeader->MessageMarker, 0xFFU));

  if (hfdcan->State == HAL_FDCAN_STATE_BUSY)
  {
    /* Check that the Tx FIFO/Queue has an allocated area into the RAM */
    if ((hfdcan->Instance->TXBC & FDCAN_TXBC_TFQS) == 0U)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

      return HAL_ERROR;
    }

    /* Check that the Tx FIFO/Queue is not full */
    if ((hfdcan->Instance->TXFQS & FDCAN_TXFQS_TFQF) != 0U)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_FIFO_FULL;

      return HAL_ERROR;
    }
    else
    {
      /* Retrieve the Tx FIFO PutIndex */
      PutIndex = ((hfdcan->Instance->TXFQS & FDCAN_TXFQS_TFQPI) >> FDCAN_TXFQS_TFQPI_Pos);

      /* Add the message to the Tx FIFO/Queue */
      FDCAN_CopyMessageToRAM(hfdcan, pTxHeader, pTxData, PutIndex);

      /* Activate the corresponding transmission request */
      hfdcan->Instance->TXBAR = ((uint32_t)1 << PutIndex);

      /* Store the Latest Tx FIFO/Queue Request Buffer Index */
      hfdcan->LatestTxFifoQRequest = ((uint32_t)1 << PutIndex);
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_STARTED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Add a message to a dedicated Tx buffer
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  pTxHeader pointer to a FDCAN_TxHeaderTypeDef structure.
  * @param  pTxData pointer to a buffer containing the payload of the Tx frame.
  * @param  BufferIndex index of the buffer to be configured.
  *         This parameter can be a value of @arg FDCAN_Tx_location.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxBuffer(FDCAN_HandleTypeDef *hfdcan, const FDCAN_TxHeaderTypeDef *pTxHeader,
                                                 const uint8_t *pTxData, uint32_t BufferIndex)
{
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_ID_TYPE(pTxHeader->IdType));
  if (pTxHeader->IdType == FDCAN_STANDARD_ID)
  {
    assert_param(IS_FDCAN_MAX_VALUE(pTxHeader->Identifier, 0x7FFU));
  }
  else /* pTxHeader->IdType == FDCAN_EXTENDED_ID */
  {
    assert_param(IS_FDCAN_MAX_VALUE(pTxHeader->Identifier, 0x1FFFFFFFU));
  }
  assert_param(IS_FDCAN_FRAME_TYPE(pTxHeader->TxFrameType));
  assert_param(IS_FDCAN_DLC(pTxHeader->DataLength));
  assert_param(IS_FDCAN_ESI(pTxHeader->ErrorStateIndicator));
  assert_param(IS_FDCAN_BRS(pTxHeader->BitRateSwitch));
  assert_param(IS_FDCAN_FDF(pTxHeader->FDFormat));
  assert_param(IS_FDCAN_EFC(pTxHeader->TxEventFifoControl));
  assert_param(IS_FDCAN_MAX_VALUE(pTxHeader->MessageMarker, 0xFFU));
  assert_param(IS_FDCAN_TX_LOCATION(BufferIndex));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Check that the selected buffer has an allocated area into the RAM */
    if (POSITION_VAL(BufferIndex) >= ((hfdcan->Instance->TXBC & FDCAN_TXBC_NDTB) >> FDCAN_TXBC_NDTB_Pos))
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

      return HAL_ERROR;
    }

    /* Check that there is no transmission request pending for the selected buffer */
    if ((hfdcan->Instance->TXBRP & BufferIndex) != 0U)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PENDING;

      return HAL_ERROR;
    }
    else
    {
      /* Add the message to the Tx buffer */
      FDCAN_CopyMessageToRAM(hfdcan, pTxHeader, pTxData, POSITION_VAL(BufferIndex));
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable transmission request.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  BufferIndex buffer index.
  *         This parameter can be any combination of @arg FDCAN_Tx_location.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_EnableTxBufferRequest(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndex)
{
  if (hfdcan->State == HAL_FDCAN_STATE_BUSY)
  {
    /* Add transmission request */
    hfdcan->Instance->TXBAR = BufferIndex;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_STARTED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Get Tx buffer index of latest Tx FIFO/Queue request
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval Tx buffer index of last Tx FIFO/Queue request
  *          - Any value of @arg FDCAN_Tx_location if Tx request has been submitted.
  *          - 0 if no Tx FIFO/Queue request have been submitted.
  */
uint32_t HAL_FDCAN_GetLatestTxFifoQRequestBuffer(const FDCAN_HandleTypeDef *hfdcan)
{
  /* Return Last Tx FIFO/Queue Request Buffer */
  return hfdcan->LatestTxFifoQRequest;
}

/**
  * @brief  Abort transmission request
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  BufferIndex buffer index.
  *         This parameter can be any combination of @arg FDCAN_Tx_location.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_AbortTxRequest(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndex)
{
  if (hfdcan->State == HAL_FDCAN_STATE_BUSY)
  {
    /* Add cancellation request */
    hfdcan->Instance->TXBCR = BufferIndex;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_STARTED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Get an FDCAN frame from the Rx Buffer/FIFO zone into the message RAM.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  RxLocation Location of the received message to be read.
  *         This parameter can be a value of @arg FDCAN_Rx_location.
  * @param  pRxHeader pointer to a FDCAN_RxHeaderTypeDef structure.
  * @param  pRxData pointer to a buffer where the payload of the Rx frame will be stored.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *hfdcan, uint32_t RxLocation,
                                         FDCAN_RxHeaderTypeDef *pRxHeader, uint8_t *pRxData)
{
  uint32_t *RxAddress;
  uint8_t  *pData;
  uint32_t ByteCounter;
  uint32_t GetIndex = 0;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  if (state == HAL_FDCAN_STATE_BUSY)
  {
    if (RxLocation == FDCAN_RX_FIFO0) /* Rx element is assigned to the Rx FIFO 0 */
    {
      /* Check that the Rx FIFO 0 has an allocated area into the RAM */
      if ((hfdcan->Instance->RXF0C & FDCAN_RXF0C_F0S) == 0U)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

        return HAL_ERROR;
      }

      /* Check that the Rx FIFO 0 is not empty */
      if ((hfdcan->Instance->RXF0S & FDCAN_RXF0S_F0FL) == 0U)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_FIFO_EMPTY;

        return HAL_ERROR;
      }
      else
      {
        /* Check that the Rx FIFO 0 is full & overwrite mode is on */
        if (((hfdcan->Instance->RXF0S & FDCAN_RXF0S_F0F) >> FDCAN_RXF0S_F0F_Pos) == 1U)
        {
          if (((hfdcan->Instance->RXF0C & FDCAN_RXF0C_F0OM) >> FDCAN_RXF0C_F0OM_Pos) == FDCAN_RX_FIFO_OVERWRITE)
          {
            /* When overwrite status is on discard first message in FIFO */
            GetIndex = 1U;
          }
        }

        /* Calculate Rx FIFO 0 element index */
        GetIndex += ((hfdcan->Instance->RXF0S & FDCAN_RXF0S_F0GI) >> FDCAN_RXF0S_F0GI_Pos);

        /* Calculate Rx FIFO 0 element address */
        RxAddress = (uint32_t *)(hfdcan->msgRam.RxFIFO0SA + (GetIndex * hfdcan->Init.RxFifo0ElmtSize * 4U));
      }
    }
    else if (RxLocation == FDCAN_RX_FIFO1) /* Rx element is assigned to the Rx FIFO 1 */
    {
      /* Check that the Rx FIFO 1 has an allocated area into the RAM */
      if ((hfdcan->Instance->RXF1C & FDCAN_RXF1C_F1S) == 0U)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

        return HAL_ERROR;
      }

      /* Check that the Rx FIFO 1 is not empty */
      if ((hfdcan->Instance->RXF1S & FDCAN_RXF1S_F1FL) == 0U)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_FIFO_EMPTY;

        return HAL_ERROR;
      }
      else
      {
        /* Check that the Rx FIFO 1 is full & overwrite mode is on */
        if (((hfdcan->Instance->RXF1S & FDCAN_RXF1S_F1F) >> FDCAN_RXF1S_F1F_Pos) == 1U)
        {
          if (((hfdcan->Instance->RXF1C & FDCAN_RXF1C_F1OM) >> FDCAN_RXF1C_F1OM_Pos) == FDCAN_RX_FIFO_OVERWRITE)
          {
            /* When overwrite status is on discard first message in FIFO */
            GetIndex = 1U;
          }
        }

        /* Calculate Rx FIFO 1 element index */
        GetIndex += ((hfdcan->Instance->RXF1S & FDCAN_RXF1S_F1GI) >> FDCAN_RXF1S_F1GI_Pos);

        /* Calculate Rx FIFO 1 element address */
        RxAddress = (uint32_t *)(hfdcan->msgRam.RxFIFO1SA + (GetIndex * hfdcan->Init.RxFifo1ElmtSize * 4U));
      }
    }
    else /* Rx element is assigned to a dedicated Rx buffer */
    {
      /* Check that the selected buffer has an allocated area into the RAM */
      if (RxLocation >= hfdcan->Init.RxBuffersNbr)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

        return HAL_ERROR;
      }
      else
      {
        /* Calculate Rx buffer address */
        RxAddress = (uint32_t *)(hfdcan->msgRam.RxBufferSA + (RxLocation * hfdcan->Init.RxBufferSize * 4U));
      }
    }

    /* Retrieve IdType */
    pRxHeader->IdType = *RxAddress & FDCAN_ELEMENT_MASK_XTD;

    /* Retrieve Identifier */
    if (pRxHeader->IdType == FDCAN_STANDARD_ID) /* Standard ID element */
    {
      pRxHeader->Identifier = ((*RxAddress & FDCAN_ELEMENT_MASK_STDID) >> 18U);
    }
    else /* Extended ID element */
    {
      pRxHeader->Identifier = (*RxAddress & FDCAN_ELEMENT_MASK_EXTID);
    }

    /* Retrieve RxFrameType */
    pRxHeader->RxFrameType = (*RxAddress & FDCAN_ELEMENT_MASK_RTR);

    /* Retrieve ErrorStateIndicator */
    pRxHeader->ErrorStateIndicator = (*RxAddress & FDCAN_ELEMENT_MASK_ESI);

    /* Increment RxAddress pointer to second word of Rx FIFO element */
    RxAddress++;

    /* Retrieve RxTimestamp */
    pRxHeader->RxTimestamp = (*RxAddress & FDCAN_ELEMENT_MASK_TS);

    /* Retrieve DataLength */
    pRxHeader->DataLength = ((*RxAddress & FDCAN_ELEMENT_MASK_DLC) >> 16U);

    /* Retrieve BitRateSwitch */
    pRxHeader->BitRateSwitch = (*RxAddress & FDCAN_ELEMENT_MASK_BRS);

    /* Retrieve FDFormat */
    pRxHeader->FDFormat = (*RxAddress & FDCAN_ELEMENT_MASK_FDF);

    /* Retrieve FilterIndex */
    pRxHeader->FilterIndex = ((*RxAddress & FDCAN_ELEMENT_MASK_FIDX) >> 24U);

    /* Retrieve NonMatchingFrame */
    pRxHeader->IsFilterMatchingFrame = ((*RxAddress & FDCAN_ELEMENT_MASK_ANMF) >> 31U);

    /* Increment RxAddress pointer to payload of Rx FIFO element */
    RxAddress++;

    /* Retrieve Rx payload */
    pData = (uint8_t *)RxAddress;
    for (ByteCounter = 0; ByteCounter < DLCtoBytes[pRxHeader->DataLength]; ByteCounter++)
    {
      pRxData[ByteCounter] = pData[ByteCounter];
    }

    if (RxLocation == FDCAN_RX_FIFO0) /* Rx element is assigned to the Rx FIFO 0 */
    {
      /* Acknowledge the Rx FIFO 0 that the oldest element is read so that it increments the GetIndex */
      hfdcan->Instance->RXF0A = GetIndex;
    }
    else if (RxLocation == FDCAN_RX_FIFO1) /* Rx element is assigned to the Rx FIFO 1 */
    {
      /* Acknowledge the Rx FIFO 1 that the oldest element is read so that it increments the GetIndex */
      hfdcan->Instance->RXF1A = GetIndex;
    }
    else /* Rx element is assigned to a dedicated Rx buffer */
    {
      /* Clear the New Data flag of the current Rx buffer */
      if (RxLocation < FDCAN_RX_BUFFER32)
      {
        hfdcan->Instance->NDAT1 = ((uint32_t)1U << RxLocation);
      }
      else /* FDCAN_RX_BUFFER32 <= RxLocation <= FDCAN_RX_BUFFER63 */
      {
        hfdcan->Instance->NDAT2 = ((uint32_t)1U << (RxLocation & 0x1FU));
      }
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_STARTED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Get an FDCAN Tx event from the Tx Event FIFO zone into the message RAM.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  pTxEvent pointer to a FDCAN_TxEventFifoTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_GetTxEvent(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxEventFifoTypeDef *pTxEvent)
{
  uint32_t *TxEventAddress;
  uint32_t GetIndex;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_MIN_VALUE(hfdcan->Init.TxEventsNbr, 1U));

  if (state == HAL_FDCAN_STATE_BUSY)
  {
    /* Check that the Tx Event FIFO has an allocated area into the RAM */
    if ((hfdcan->Instance->TXEFC & FDCAN_TXEFC_EFS) == 0U)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

      return HAL_ERROR;
    }

    /* Check that the Tx event FIFO is not empty */
    if ((hfdcan->Instance->TXEFS & FDCAN_TXEFS_EFFL) == 0U)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_FIFO_EMPTY;

      return HAL_ERROR;
    }

    /* Calculate Tx event FIFO element address */
    GetIndex = ((hfdcan->Instance->TXEFS & FDCAN_TXEFS_EFGI) >> FDCAN_TXEFS_EFGI_Pos);
    TxEventAddress = (uint32_t *)(hfdcan->msgRam.TxEventFIFOSA + (GetIndex * 2U * 4U));

    /* Retrieve IdType */
    pTxEvent->IdType = *TxEventAddress & FDCAN_ELEMENT_MASK_XTD;

    /* Retrieve Identifier */
    if (pTxEvent->IdType == FDCAN_STANDARD_ID) /* Standard ID element */
    {
      pTxEvent->Identifier = ((*TxEventAddress & FDCAN_ELEMENT_MASK_STDID) >> 18U);
    }
    else /* Extended ID element */
    {
      pTxEvent->Identifier = (*TxEventAddress & FDCAN_ELEMENT_MASK_EXTID);
    }

    /* Retrieve TxFrameType */
    pTxEvent->TxFrameType = (*TxEventAddress & FDCAN_ELEMENT_MASK_RTR);

    /* Retrieve ErrorStateIndicator */
    pTxEvent->ErrorStateIndicator = (*TxEventAddress & FDCAN_ELEMENT_MASK_ESI);

    /* Increment TxEventAddress pointer to second word of Tx Event FIFO element */
    TxEventAddress++;

    /* Retrieve TxTimestamp */
    pTxEvent->TxTimestamp = (*TxEventAddress & FDCAN_ELEMENT_MASK_TS);

    /* Retrieve DataLength */
    pTxEvent->DataLength = ((*TxEventAddress & FDCAN_ELEMENT_MASK_DLC) >> 16U);

    /* Retrieve BitRateSwitch */
    pTxEvent->BitRateSwitch = (*TxEventAddress & FDCAN_ELEMENT_MASK_BRS);

    /* Retrieve FDFormat */
    pTxEvent->FDFormat = (*TxEventAddress & FDCAN_ELEMENT_MASK_FDF);

    /* Retrieve EventType */
    pTxEvent->EventType = (*TxEventAddress & FDCAN_ELEMENT_MASK_ET);

    /* Retrieve MessageMarker */
    pTxEvent->MessageMarker = ((*TxEventAddress & FDCAN_ELEMENT_MASK_MM) >> 24U);

    /* Acknowledge the Tx Event FIFO that the oldest element is read so that it increments the GetIndex */
    hfdcan->Instance->TXEFA = GetIndex;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_STARTED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Get high priority message status.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  HpMsgStatus pointer to an FDCAN_HpMsgStatusTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_GetHighPriorityMessageStatus(const FDCAN_HandleTypeDef *hfdcan,
                                                         FDCAN_HpMsgStatusTypeDef *HpMsgStatus)
{
  HpMsgStatus->FilterList = ((hfdcan->Instance->HPMS & FDCAN_HPMS_FLST) >> FDCAN_HPMS_FLST_Pos);
  HpMsgStatus->FilterIndex = ((hfdcan->Instance->HPMS & FDCAN_HPMS_FIDX) >> FDCAN_HPMS_FIDX_Pos);
  HpMsgStatus->MessageStorage = (hfdcan->Instance->HPMS & FDCAN_HPMS_MSI);
  HpMsgStatus->MessageIndex = (hfdcan->Instance->HPMS & FDCAN_HPMS_BIDX);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Get protocol status.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  ProtocolStatus pointer to an FDCAN_ProtocolStatusTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_GetProtocolStatus(const FDCAN_HandleTypeDef *hfdcan,
                                              FDCAN_ProtocolStatusTypeDef *ProtocolStatus)
{
  uint32_t StatusReg;

  /* Read the protocol status register */
  StatusReg = READ_REG(hfdcan->Instance->PSR);

  /* Fill the protocol status structure */
  ProtocolStatus->LastErrorCode = (StatusReg & FDCAN_PSR_LEC);
  ProtocolStatus->DataLastErrorCode = ((StatusReg & FDCAN_PSR_DLEC) >> FDCAN_PSR_DLEC_Pos);
  ProtocolStatus->Activity = (StatusReg & FDCAN_PSR_ACT);
  ProtocolStatus->ErrorPassive = ((StatusReg & FDCAN_PSR_EP) >> FDCAN_PSR_EP_Pos);
  ProtocolStatus->Warning = ((StatusReg & FDCAN_PSR_EW) >> FDCAN_PSR_EW_Pos);
  ProtocolStatus->BusOff = ((StatusReg & FDCAN_PSR_BO) >> FDCAN_PSR_BO_Pos);
  ProtocolStatus->RxESIflag = ((StatusReg & FDCAN_PSR_RESI) >> FDCAN_PSR_RESI_Pos);
  ProtocolStatus->RxBRSflag = ((StatusReg & FDCAN_PSR_RBRS) >> FDCAN_PSR_RBRS_Pos);
  ProtocolStatus->RxFDFflag = ((StatusReg & FDCAN_PSR_REDL) >> FDCAN_PSR_REDL_Pos);
  ProtocolStatus->ProtocolException = ((StatusReg & FDCAN_PSR_PXE) >> FDCAN_PSR_PXE_Pos);
  ProtocolStatus->TDCvalue = ((StatusReg & FDCAN_PSR_TDCV) >> FDCAN_PSR_TDCV_Pos);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Get error counter values.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  ErrorCounters pointer to an FDCAN_ErrorCountersTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_GetErrorCounters(const FDCAN_HandleTypeDef *hfdcan,
                                             FDCAN_ErrorCountersTypeDef *ErrorCounters)
{
  uint32_t CountersReg;

  /* Read the error counters register */
  CountersReg = READ_REG(hfdcan->Instance->ECR);

  /* Fill the error counters structure */
  ErrorCounters->TxErrorCnt = ((CountersReg & FDCAN_ECR_TEC) >> FDCAN_ECR_TEC_Pos);
  ErrorCounters->RxErrorCnt = ((CountersReg & FDCAN_ECR_REC) >> FDCAN_ECR_REC_Pos);
  ErrorCounters->RxErrorPassive = ((CountersReg & FDCAN_ECR_RP) >> FDCAN_ECR_RP_Pos);
  ErrorCounters->ErrorLogging = ((CountersReg & FDCAN_ECR_CEL) >> FDCAN_ECR_CEL_Pos);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Check if a new message is received in the selected Rx buffer.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  RxBufferIndex Rx buffer index.
  *         This parameter must be a number between 0 and 63.
  * @retval Status
  *          - 0 : No new message on RxBufferIndex.
  *          - 1 : New message received on RxBufferIndex.
  */
uint32_t HAL_FDCAN_IsRxBufferMessageAvailable(FDCAN_HandleTypeDef *hfdcan, uint32_t RxBufferIndex)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_MAX_VALUE(RxBufferIndex, 63U));
  uint32_t NewData1 = hfdcan->Instance->NDAT1;
  uint32_t NewData2 = hfdcan->Instance->NDAT2;

  /* Check new message reception on the selected buffer */
  if (((RxBufferIndex < 32U) && ((NewData1 & (uint32_t)((uint32_t)1 << RxBufferIndex)) == 0U)) ||
      ((RxBufferIndex >= 32U) && ((NewData2 & (uint32_t)((uint32_t)1 << (RxBufferIndex & 0x1FU))) == 0U)))
  {
    return 0;
  }

  /* Clear the New Data flag of the current Rx buffer */
  if (RxBufferIndex < 32U)
  {
    hfdcan->Instance->NDAT1 = ((uint32_t)1 << RxBufferIndex);
  }
  else /* 32 <= RxBufferIndex <= 63 */
  {
    hfdcan->Instance->NDAT2 = ((uint32_t)1 << (RxBufferIndex & 0x1FU));
  }

  return 1;
}

/**
  * @brief  Check if a transmission request is pending on the selected Tx buffer.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TxBufferIndex Tx buffer index.
  *         This parameter can be any combination of @arg FDCAN_Tx_location.
  * @retval Status
  *          - 0 : No pending transmission request on TxBufferIndex.
  *          - 1 : Pending transmission request on TxBufferIndex.
  */
uint32_t HAL_FDCAN_IsTxBufferMessagePending(const FDCAN_HandleTypeDef *hfdcan, uint32_t TxBufferIndex)
{
  /* Check pending transmission request on the selected buffer */
  if ((hfdcan->Instance->TXBRP & TxBufferIndex) == 0U)
  {
    return 0;
  }
  return 1;
}

/**
  * @brief  Return Rx FIFO fill level.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  RxFifo Rx FIFO.
  *         This parameter can be one of the following values:
  *           @arg FDCAN_RX_FIFO0: Rx FIFO 0
  *           @arg FDCAN_RX_FIFO1: Rx FIFO 1
  * @retval Rx FIFO fill level.
  */
uint32_t HAL_FDCAN_GetRxFifoFillLevel(const FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo)
{
  uint32_t FillLevel;

  /* Check function parameters */
  assert_param(IS_FDCAN_RX_FIFO(RxFifo));

  if (RxFifo == FDCAN_RX_FIFO0)
  {
    FillLevel = hfdcan->Instance->RXF0S & FDCAN_RXF0S_F0FL;
  }
  else /* RxFifo == FDCAN_RX_FIFO1 */
  {
    FillLevel = hfdcan->Instance->RXF1S & FDCAN_RXF1S_F1FL;
  }

  /* Return Rx FIFO fill level */
  return FillLevel;
}

/**
  * @brief  Return Tx FIFO free level: number of consecutive free Tx FIFO
  *         elements starting from Tx FIFO GetIndex.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval Tx FIFO free level.
  */
uint32_t HAL_FDCAN_GetTxFifoFreeLevel(const FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t FreeLevel;

  FreeLevel = hfdcan->Instance->TXFQS & FDCAN_TXFQS_TFFL;

  /* Return Tx FIFO free level */
  return FreeLevel;
}

/**
  * @brief  Check if the FDCAN peripheral entered Restricted Operation Mode.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval Status
  *          - 0 : Normal FDCAN operation.
  *          - 1 : Restricted Operation Mode active.
  */
uint32_t HAL_FDCAN_IsRestrictedOperationMode(const FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t OperationMode;

  /* Get Operation Mode */
  OperationMode = ((hfdcan->Instance->CCCR & FDCAN_CCCR_ASM) >> FDCAN_CCCR_ASM_Pos);

  return OperationMode;
}

/**
  * @brief  Exit Restricted Operation Mode.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ExitRestrictedOperationMode(FDCAN_HandleTypeDef *hfdcan)
{
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Exit Restricted Operation mode */
    CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_ASM);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @}
  */

/** @defgroup FDCAN_Exported_Functions_Group4 TT Configuration and control functions
  *  @brief    TT Configuration and control functions
  *
@verbatim
  ==============================================================================
              ##### TT Configuration and control functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) HAL_FDCAN_TT_ConfigOperation                  : Initialize TT operation parameters
      (+) HAL_FDCAN_TT_ConfigReferenceMessage           : Configure the reference message
      (+) HAL_FDCAN_TT_ConfigTrigger                    : Configure the FDCAN trigger
      (+) HAL_FDCAN_TT_SetGlobalTime                    : Schedule global time adjustment
      (+) HAL_FDCAN_TT_SetClockSynchronization          : Schedule TUR numerator update
      (+) HAL_FDCAN_TT_ConfigStopWatch                  : Configure stop watch source and polarity
      (+) HAL_FDCAN_TT_ConfigRegisterTimeMark           : Configure register time mark pulse generation
        (+) HAL_FDCAN_TT_EnableRegisterTimeMarkPulse      : Enable register time mark pulse generation
        (+) HAL_FDCAN_TT_DisableRegisterTimeMarkPulse     : Disable register time mark pulse generation
      (+) HAL_FDCAN_TT_EnableTriggerTimeMarkPulse       : Enable trigger time mark pulse generation
      (+) HAL_FDCAN_TT_DisableTriggerTimeMarkPulse      : Disable trigger time mark pulse generation
      (+) HAL_FDCAN_TT_EnableHardwareGapControl         : Enable gap control by input pin fdcan1_evt
      (+) HAL_FDCAN_TT_DisableHardwareGapControl        : Disable gap control by input pin fdcan1_evt
      (+) HAL_FDCAN_TT_EnableTimeMarkGapControl         : Enable gap control (finish only) by register time mark IT
      (+) HAL_FDCAN_TT_DisableTimeMarkGapControl        : Disable gap control by register time mark interrupt
      (+) HAL_FDCAN_TT_SetNextIsGap                     : Transmit next reference message with Next_is_Gap = "1"
      (+) HAL_FDCAN_TT_SetEndOfGap                      : Finish a Gap by requesting start of reference message
      (+) HAL_FDCAN_TT_ConfigExternalSyncPhase          : Configure target phase used for external synchronization
        (+) HAL_FDCAN_TT_EnableExternalSynchronization    : Synchronize the phase of the FDCAN schedule to an external
                                                            schedule
        (+) HAL_FDCAN_TT_DisableExternalSynchronization   : Disable external schedule synchronization
      (+) HAL_FDCAN_TT_GetOperationStatus               : Get TT operation status

@endverbatim
  * @{
  */

/**
  * @brief  Initialize TT operation parameters.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  pTTParams pointer to a FDCAN_TT_ConfigTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_ConfigOperation(FDCAN_HandleTypeDef *hfdcan, const FDCAN_TT_ConfigTypeDef *pTTParams)
{
  uint32_t tickstart;
  uint32_t RAMcounter;
  uint32_t StartAddress;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_TT_TUR_NUMERATOR(pTTParams->TURNumerator));
  assert_param(IS_FDCAN_TT_TUR_DENOMINATOR(pTTParams->TURDenominator));
  assert_param(IS_FDCAN_TT_TIME_MASTER(pTTParams->TimeMaster));
  assert_param(IS_FDCAN_MAX_VALUE(pTTParams->SyncDevLimit, 7U));
  assert_param(IS_FDCAN_MAX_VALUE(pTTParams->InitRefTrigOffset, 127U));
  assert_param(IS_FDCAN_MAX_VALUE(pTTParams->TriggerMemoryNbr, 64U));
  assert_param(IS_FDCAN_TT_CYCLE_START_SYNC(pTTParams->CycleStartSync));
  assert_param(IS_FDCAN_TT_STOP_WATCH_TRIGGER(pTTParams->StopWatchTrigSel));
  assert_param(IS_FDCAN_TT_EVENT_TRIGGER(pTTParams->EventTrigSel));
  if (pTTParams->TimeMaster == FDCAN_TT_POTENTIAL_MASTER)
  {
    assert_param(IS_FDCAN_TT_BASIC_CYCLES_NUMBER(pTTParams->BasicCyclesNbr));
  }
  if (pTTParams->OperationMode != FDCAN_TT_COMMUNICATION_LEVEL0)
  {
    assert_param(IS_FDCAN_TT_OPERATION(pTTParams->GapEnable));
    assert_param(IS_FDCAN_MAX_VALUE(pTTParams->AppWdgLimit, 255U));
    assert_param(IS_FDCAN_TT_EVENT_TRIGGER_POLARITY(pTTParams->EvtTrigPolarity));
    assert_param(IS_FDCAN_TT_TX_ENABLE_WINDOW(pTTParams->TxEnableWindow));
    assert_param(IS_FDCAN_MAX_VALUE(pTTParams->ExpTxTrigNbr, 4095U));
  }
  if (pTTParams->OperationMode != FDCAN_TT_COMMUNICATION_LEVEL1)
  {
    assert_param(IS_FDCAN_TT_TUR_LEVEL_0_2(pTTParams->TURNumerator, pTTParams->TURDenominator));
    assert_param(IS_FDCAN_TT_EXTERNAL_CLK_SYNC(pTTParams->ExternalClkSync));
    assert_param(IS_FDCAN_TT_GLOBAL_TIME_FILTERING(pTTParams->GlobalTimeFilter));
    assert_param(IS_FDCAN_TT_AUTO_CLK_CALIBRATION(pTTParams->ClockCalibration));
  }
  else
  {
    assert_param(IS_FDCAN_TT_TUR_LEVEL_1(pTTParams->TURNumerator, pTTParams->TURDenominator));
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Stop local time in order to enable write access to the other bits of TURCF register */
    CLEAR_BIT(hfdcan->ttcan->TURCF, FDCAN_TURCF_ELT);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait until the ELT bit into TURCF register is reset */
    while ((hfdcan->ttcan->TURCF & FDCAN_TURCF_ELT) != 0U)
    {
      /* Check for the Timeout */
      if ((HAL_GetTick() - tickstart) > FDCAN_TIMEOUT_VALUE)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }
    }

    /* Configure TUR (Time Unit Ratio) */
    MODIFY_REG(hfdcan->ttcan->TURCF,
               (FDCAN_TURCF_NCL | FDCAN_TURCF_DC),
               (((pTTParams->TURNumerator - 0x10000U) << FDCAN_TURCF_NCL_Pos) |
                (pTTParams->TURDenominator << FDCAN_TURCF_DC_Pos)));

    /* Enable local time */
    SET_BIT(hfdcan->ttcan->TURCF, FDCAN_TURCF_ELT);

    /* Configure TT operation */
    MODIFY_REG(hfdcan->ttcan->TTOCF,
               (FDCAN_TTOCF_OM | FDCAN_TTOCF_TM | FDCAN_TTOCF_LDSDL | FDCAN_TTOCF_IRTO),
               (pTTParams->OperationMode                           | \
                pTTParams->TimeMaster                              | \
                (pTTParams->SyncDevLimit << FDCAN_TTOCF_LDSDL_Pos) | \
                (pTTParams->InitRefTrigOffset << FDCAN_TTOCF_IRTO_Pos)));
    if (pTTParams->OperationMode != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      MODIFY_REG(hfdcan->ttcan->TTOCF,
                 (FDCAN_TTOCF_GEN | FDCAN_TTOCF_AWL | FDCAN_TTOCF_EVTP),
                 (pTTParams->GapEnable                            | \
                  (pTTParams->AppWdgLimit << FDCAN_TTOCF_AWL_Pos) | \
                  pTTParams->EvtTrigPolarity));
    }
    if (pTTParams->OperationMode != FDCAN_TT_COMMUNICATION_LEVEL1)
    {
      MODIFY_REG(hfdcan->ttcan->TTOCF,
                 (FDCAN_TTOCF_EECS | FDCAN_TTOCF_EGTF | FDCAN_TTOCF_ECC),
                 (pTTParams->ExternalClkSync  | \
                  pTTParams->GlobalTimeFilter | \
                  pTTParams->ClockCalibration));
    }

    /* Configure system matrix limits */
    MODIFY_REG(hfdcan->ttcan->TTMLM, FDCAN_TTMLM_CSS, pTTParams->CycleStartSync);
    if (pTTParams->OperationMode != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      MODIFY_REG(hfdcan->ttcan->TTMLM,
                 (FDCAN_TTMLM_TXEW | FDCAN_TTMLM_ENTT),
                 (((pTTParams->TxEnableWindow - 1U) << FDCAN_TTMLM_TXEW_Pos) |
                  (pTTParams->ExpTxTrigNbr << FDCAN_TTMLM_ENTT_Pos)));
    }
    if (pTTParams->TimeMaster == FDCAN_TT_POTENTIAL_MASTER)
    {
      MODIFY_REG(hfdcan->ttcan->TTMLM, FDCAN_TTMLM_CCM, pTTParams->BasicCyclesNbr);
    }

    /* Configure input triggers: Stop watch and Event */
    MODIFY_REG(hfdcan->ttcan->TTTS,
               (FDCAN_TTTS_SWTSEL | FDCAN_TTTS_EVTSEL),
               (pTTParams->StopWatchTrigSel | pTTParams->EventTrigSel));

    /* Configure trigger memory start address */
    StartAddress = (hfdcan->msgRam.EndAddress - SRAMCAN_BASE) / 4U;
    MODIFY_REG(hfdcan->ttcan->TTTMC, FDCAN_TTTMC_TMSA, (StartAddress << FDCAN_TTTMC_TMSA_Pos));

    /* Trigger memory elements number */
    MODIFY_REG(hfdcan->ttcan->TTTMC, FDCAN_TTTMC_TME, (pTTParams->TriggerMemoryNbr << FDCAN_TTTMC_TME_Pos));

    /* Recalculate End Address */
    hfdcan->msgRam.TTMemorySA = hfdcan->msgRam.EndAddress;
    hfdcan->msgRam.EndAddress = hfdcan->msgRam.TTMemorySA + (pTTParams->TriggerMemoryNbr * 2U * 4U);

    if (hfdcan->msgRam.EndAddress > FDCAN_MESSAGE_RAM_END_ADDRESS) /* Last address of the Message RAM */
    {
      /* Update error code.
         Message RAM overflow */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

      return HAL_ERROR;
    }
    else
    {
      /* Flush the allocated Message RAM area */
      for (RAMcounter = hfdcan->msgRam.TTMemorySA; RAMcounter < hfdcan->msgRam.EndAddress; RAMcounter += 4U)
      {
        *(uint32_t *)(RAMcounter) = 0x00000000;
      }
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure the reference message.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  IdType Identifier Type.
  *         This parameter can be a value of @arg FDCAN_id_type.
  * @param  Identifier Reference Identifier.
  *         This parameter must be a number between:
  *           - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
  *           - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID
  * @param  Payload Enable or disable the additional payload.
  *         This parameter can be a value of @arg FDCAN_TT_Reference_Message_Payload.
  *         This parameter is ignored in case of time slaves.
  *         If this parameter is set to FDCAN_TT_REF_MESSAGE_ADD_PAYLOAD, the
  *         following elements are taken from Tx Buffer 0:
  *          - MessageMarker
  *          - TxEventFifoControl
  *          - DataLength
  *          - Data Bytes (payload):
  *             - bytes 2-8, for Level 1
  *             - bytes 5-8, for Level 0 and Level 2
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_ConfigReferenceMessage(FDCAN_HandleTypeDef *hfdcan, uint32_t IdType,
                                                      uint32_t Identifier, uint32_t Payload)
{
  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_ID_TYPE(IdType));
  if (IdType == FDCAN_STANDARD_ID)
  {
    assert_param(IS_FDCAN_MAX_VALUE(Identifier, 0x7FFU));
  }
  else /* IdType == FDCAN_EXTENDED_ID */
  {
    assert_param(IS_FDCAN_MAX_VALUE(Identifier, 0x1FFFFFFFU));
  }
  assert_param(IS_FDCAN_TT_REFERENCE_MESSAGE_PAYLOAD(Payload));

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Configure reference message identifier type, identifier and payload */
    if (IdType == FDCAN_EXTENDED_ID)
    {
      MODIFY_REG(hfdcan->ttcan->TTRMC, (FDCAN_TTRMC_RID | FDCAN_TTRMC_XTD | FDCAN_TTRMC_RMPS),
                 (Payload | IdType | Identifier));
    }
    else /* IdType == FDCAN_STANDARD_ID */
    {
      MODIFY_REG(hfdcan->ttcan->TTRMC, (FDCAN_TTRMC_RID | FDCAN_TTRMC_XTD | FDCAN_TTRMC_RMPS),
                 (Payload | IdType | (Identifier << 18)));
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure the FDCAN trigger according to the specified
  *         parameters in the FDCAN_TriggerTypeDef structure.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  sTriggerConfig pointer to an FDCAN_TriggerTypeDef structure that
  *         contains the trigger configuration information
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_ConfigTrigger(FDCAN_HandleTypeDef *hfdcan, const FDCAN_TriggerTypeDef *sTriggerConfig)
{
  uint32_t CycleCode;
  uint32_t MessageNumber;
  uint32_t TriggerElementW1;
  uint32_t TriggerElementW2;
  uint32_t *TriggerAddress;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_MAX_VALUE(sTriggerConfig->TriggerIndex, 63U));
  assert_param(IS_FDCAN_MAX_VALUE(sTriggerConfig->TimeMark, 0xFFFFU));
  assert_param(IS_FDCAN_TT_REPEAT_FACTOR(sTriggerConfig->RepeatFactor));
  if (sTriggerConfig->RepeatFactor != FDCAN_TT_REPEAT_EVERY_CYCLE)
  {
    assert_param(IS_FDCAN_MAX_VALUE(sTriggerConfig->StartCycle, (sTriggerConfig->RepeatFactor - 1U)));
  }
  assert_param(IS_FDCAN_TT_TM_EVENT_INTERNAL(sTriggerConfig->TmEventInt));
  assert_param(IS_FDCAN_TT_TM_EVENT_EXTERNAL(sTriggerConfig->TmEventExt));
  assert_param(IS_FDCAN_TT_TRIGGER_TYPE(sTriggerConfig->TriggerType));
  assert_param(IS_FDCAN_ID_TYPE(sTriggerConfig->FilterType));
  if ((sTriggerConfig->TriggerType == FDCAN_TT_TX_TRIGGER_SINGLE) ||
      (sTriggerConfig->TriggerType == FDCAN_TT_TX_TRIGGER_CONTINUOUS) ||
      (sTriggerConfig->TriggerType == FDCAN_TT_TX_TRIGGER_ARBITRATION) ||
      (sTriggerConfig->TriggerType == FDCAN_TT_TX_TRIGGER_MERGED))
  {
    assert_param(IS_FDCAN_TX_LOCATION(sTriggerConfig->TxBufferIndex));
  }
  if (sTriggerConfig->TriggerType == FDCAN_TT_RX_TRIGGER)
  {
    if (sTriggerConfig->FilterType == FDCAN_STANDARD_ID)
    {
      assert_param(IS_FDCAN_MAX_VALUE(sTriggerConfig->FilterIndex, 63U));
    }
    else /* sTriggerConfig->FilterType == FDCAN_EXTENDED_ID */
    {
      assert_param(IS_FDCAN_MAX_VALUE(sTriggerConfig->FilterIndex, 127U));
    }
  }

  if (hfdcan->State == HAL_FDCAN_STATE_READY)
  {
    /* Calculate cycle code */
    if (sTriggerConfig->RepeatFactor == FDCAN_TT_REPEAT_EVERY_CYCLE)
    {
      CycleCode = FDCAN_TT_REPEAT_EVERY_CYCLE;
    }
    else /* sTriggerConfig->RepeatFactor != FDCAN_TT_REPEAT_EVERY_CYCLE */
    {
      CycleCode = sTriggerConfig->RepeatFactor + sTriggerConfig->StartCycle;
    }

    /* Build first word of trigger element */
    TriggerElementW1 = ((sTriggerConfig->TimeMark << 16) | \
                        (CycleCode << 8)                 | \
                        sTriggerConfig->TmEventInt       | \
                        sTriggerConfig->TmEventExt       | \
                        sTriggerConfig->TriggerType);

    /* Select message number depending on trigger type (transmission or reception) */
    if (sTriggerConfig->TriggerType == FDCAN_TT_RX_TRIGGER)
    {
      MessageNumber = sTriggerConfig->FilterIndex;
    }
    else if ((sTriggerConfig->TriggerType == FDCAN_TT_TX_TRIGGER_SINGLE) ||
             (sTriggerConfig->TriggerType == FDCAN_TT_TX_TRIGGER_CONTINUOUS) ||
             (sTriggerConfig->TriggerType == FDCAN_TT_TX_TRIGGER_ARBITRATION) ||
             (sTriggerConfig->TriggerType == FDCAN_TT_TX_TRIGGER_MERGED))
    {
      MessageNumber = POSITION_VAL(sTriggerConfig->TxBufferIndex);
    }
    else
    {
      MessageNumber = 0U;
    }

    /* Build second word of trigger element */
    TriggerElementW2 = ((sTriggerConfig->FilterType >> 7) | (MessageNumber << 16));

    /* Calculate trigger address */
    TriggerAddress = (uint32_t *)(hfdcan->msgRam.TTMemorySA + (sTriggerConfig->TriggerIndex * 4U * 2U));

    /* Write trigger element to the message RAM */
    *TriggerAddress = TriggerElementW1;
    TriggerAddress++;
    *TriggerAddress = TriggerElementW2;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Schedule global time adjustment for the next reference message.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TimePreset time preset value.
  *         This parameter must be a number between:
  *           - 0x0000 and 0x7FFF, Next_Master_Ref_Mark = Current_Master_Ref_Mark + TimePreset
  *           or
  *           - 0x8001 and 0xFFFF, Next_Master_Ref_Mark = Current_Master_Ref_Mark - (0x10000 - TimePreset)
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_SetGlobalTime(FDCAN_HandleTypeDef *hfdcan, uint32_t TimePreset)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_TT_TIME_PRESET(TimePreset));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Check that the external clock synchronization is enabled */
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_EECS) != FDCAN_TTOCF_EECS)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }

    /* Check that no global time preset is pending */
    if ((hfdcan->ttcan->TTOST & FDCAN_TTOST_WGTD) == FDCAN_TTOST_WGTD)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PENDING;

      return HAL_ERROR;
    }

    /* Configure time preset */
    MODIFY_REG(hfdcan->ttcan->TTGTP, FDCAN_TTGTP_TP, (TimePreset << FDCAN_TTGTP_TP_Pos));

    /* Wait until the LCKC bit into TTOCN register is reset */
    while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Schedule time preset to take effect by the next reference message */
    SET_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_SGT);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Schedule TUR numerator update for the next reference message.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  NewTURNumerator new value of the TUR numerator.
  *         This parameter must be a number between 0x10000 and 0x1FFFF.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_SetClockSynchronization(FDCAN_HandleTypeDef *hfdcan, uint32_t NewTURNumerator)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_TT_TUR_NUMERATOR(NewTURNumerator));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Check that the external clock synchronization is enabled */
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_EECS) != FDCAN_TTOCF_EECS)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }

    /* Check that no external clock synchronization is pending */
    if ((hfdcan->ttcan->TTOST & FDCAN_TTOST_WECS) == FDCAN_TTOST_WECS)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PENDING;

      return HAL_ERROR;
    }

    /* Configure new TUR numerator */
    MODIFY_REG(hfdcan->ttcan->TURCF, FDCAN_TURCF_NCL, (NewTURNumerator - 0x10000U));

    /* Wait until the LCKC bit into TTOCN register is reset */
    while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Schedule TUR numerator update by the next reference message */
    SET_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_ECS);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure stop watch source and polarity.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  Source stop watch source.
  *         This parameter can be a value of @arg FDCAN_TT_stop_watch_source.
  * @param  Polarity stop watch polarity.
  *         This parameter can be a value of @arg FDCAN_TT_stop_watch_polarity.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_ConfigStopWatch(FDCAN_HandleTypeDef *hfdcan, uint32_t Source, uint32_t Polarity)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_TT_STOP_WATCH_SOURCE(Source));
  assert_param(IS_FDCAN_TT_STOP_WATCH_POLARITY(Polarity));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Wait until the LCKC bit into TTOCN register is reset */
    while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Select stop watch source and polarity */
    MODIFY_REG(hfdcan->ttcan->TTOCN, (FDCAN_TTOCN_SWS | FDCAN_TTOCN_SWP), (Source | Polarity));

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure register time mark pulse generation.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TimeMarkSource time mark source.
  *         This parameter can be a value of @arg FDCAN_TT_time_mark_source.
  * @param  TimeMarkValue time mark value (reference).
  *         This parameter must be a number between 0 and 0xFFFF.
  * @param  RepeatFactor repeat factor of the cycle for which the time mark is valid.
  *         This parameter can be a value of @arg FDCAN_TT_Repeat_Factor.
  * @param  StartCycle index of the first cycle in which the time mark becomes valid.
  *         This parameter is ignored if RepeatFactor is set to FDCAN_TT_REPEAT_EVERY_CYCLE.
  *         This parameter must be a number between 0 and RepeatFactor.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_ConfigRegisterTimeMark(FDCAN_HandleTypeDef *hfdcan,
                                                      uint32_t TimeMarkSource, uint32_t TimeMarkValue,
                                                      uint32_t RepeatFactor, uint32_t StartCycle)
{
  uint32_t Counter = 0U;
  uint32_t CycleCode;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_TT_REGISTER_TIME_MARK_SOURCE(TimeMarkSource));
  assert_param(IS_FDCAN_MAX_VALUE(TimeMarkValue, 0xFFFFU));
  assert_param(IS_FDCAN_TT_REPEAT_FACTOR(RepeatFactor));
  if (RepeatFactor != FDCAN_TT_REPEAT_EVERY_CYCLE)
  {
    assert_param(IS_FDCAN_MAX_VALUE(StartCycle, (RepeatFactor - 1U)));
  }

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Wait until the LCKC bit into TTOCN register is reset */
    while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Disable the time mark compare function */
    CLEAR_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_TMC);

    if (TimeMarkSource != FDCAN_TT_REG_TIMEMARK_DIABLED)
    {
      /* Calculate cycle code */
      if (RepeatFactor == FDCAN_TT_REPEAT_EVERY_CYCLE)
      {
        CycleCode = FDCAN_TT_REPEAT_EVERY_CYCLE;
      }
      else /* RepeatFactor != FDCAN_TT_REPEAT_EVERY_CYCLE */
      {
        CycleCode = RepeatFactor + StartCycle;
      }

      Counter = 0U;

      /* Wait until the LCKM bit into TTTMK register is reset */
      while ((hfdcan->ttcan->TTTMK & FDCAN_TTTMK_LCKM) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Configure time mark value and cycle code */
      hfdcan->ttcan->TTTMK = ((TimeMarkValue << FDCAN_TTTMK_TM_Pos) | (CycleCode << FDCAN_TTTMK_TICC_Pos));

      Counter = 0U;

      /* Wait until the LCKC bit into TTOCN register is reset */
      while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Update the register time mark compare source */
      MODIFY_REG(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_TMC, TimeMarkSource);
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable register time mark pulse generation.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_EnableRegisterTimeMarkPulse(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Wait until the LCKC bit into TTOCN register is reset */
    while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Enable Register Time Mark Interrupt output on fdcan1_rtp */
    SET_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_RTIE);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable register time mark pulse generation.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_DisableRegisterTimeMarkPulse(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Wait until the LCKC bit into TTOCN register is reset */
    while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Disable Register Time Mark Interrupt output on fdcan1_rtp */
    CLEAR_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_RTIE);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable trigger time mark pulse generation.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_EnableTriggerTimeMarkPulse(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_OM) != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      /* Wait until the LCKC bit into TTOCN register is reset */
      while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Enable Trigger Time Mark Interrupt output on fdcan1_tmp */
      SET_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_TTIE);

      /* Return function status */
      return HAL_OK;
    }
    else
    {
      /* Update error code.
         Feature not supported for TT Level 0 */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable trigger time mark pulse generation.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_DisableTriggerTimeMarkPulse(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_OM) != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      /* Wait until the LCKC bit into TTOCN register is reset */
      while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Disable Trigger Time Mark Interrupt output on fdcan1_rtp */
      CLEAR_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_TTIE);

      /* Return function status */
      return HAL_OK;
    }
    else
    {
      /* Update error code.
         Feature not supported for TT Level 0 */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable gap control by input pin fdcan1_evt.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_EnableHardwareGapControl(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_OM) != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      /* Wait until the LCKC bit into TTOCN register is reset */
      while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Enable gap control by pin fdcan1_evt */
      SET_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_GCS);

      /* Return function status */
      return HAL_OK;
    }
    else
    {
      /* Update error code.
         Feature not supported for TT Level 0 */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable gap control by input pin fdcan1_evt.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_DisableHardwareGapControl(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_OM) != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      /* Wait until the LCKC bit into TTOCN register is reset */
      while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Disable gap control by pin fdcan1_evt */
      CLEAR_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_GCS);

      /* Return function status */
      return HAL_OK;
    }
    else
    {
      /* Update error code.
         Feature not supported for TT Level 0 */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable gap control (finish only) by register time mark interrupt.
  *         The next register time mark interrupt (TTIR.RTMI = "1") will finish
  *         the Gap and start the reference message.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_EnableTimeMarkGapControl(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_OM) != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      /* Wait until the LCKC bit into TTOCN register is reset */
      while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Enable gap control by register time mark interrupt */
      SET_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_TMG);

      /* Return function status */
      return HAL_OK;
    }
    else
    {
      /* Update error code.
         Feature not supported for TT Level 0 */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable gap control by register time mark interrupt.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_DisableTimeMarkGapControl(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_OM) != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      /* Wait until the LCKC bit into TTOCN register is reset */
      while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Disable gap control by register time mark interrupt */
      CLEAR_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_TMG);

      /* Return function status */
      return HAL_OK;
    }
    else
    {
      /* Update error code.
         Feature not supported for TT Level 0 */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Transmit next reference message with Next_is_Gap = "1".
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_SetNextIsGap(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Check that the node is configured for external event-synchronized TT operation */
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_GEN) != FDCAN_TTOCF_GEN)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }

    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_OM) != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      /* Wait until the LCKC bit into TTOCN register is reset */
      while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Set Next is Gap */
      SET_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_NIG);

      /* Return function status */
      return HAL_OK;
    }
    else
    {
      /* Update error code.
         Feature not supported for TT Level 0 */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Finish a Gap by requesting start of reference message.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_SetEndOfGap(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Check that the node is configured for external event-synchronized TT operation */
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_GEN) != FDCAN_TTOCF_GEN)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }

    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_OM) != FDCAN_TT_COMMUNICATION_LEVEL0)
    {
      /* Wait until the LCKC bit into TTOCN register is reset */
      while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
      {
        /* Check for the Timeout */
        if (Counter > FDCAN_TIMEOUT_COUNT)
        {
          /* Update error code */
          hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

          /* Change FDCAN state */
          hfdcan->State = HAL_FDCAN_STATE_ERROR;

          return HAL_ERROR;
        }

        /* Increment counter */
        Counter++;
      }

      /* Set Finish Gap */
      SET_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_FGP);

      /* Return function status */
      return HAL_OK;
    }
    else
    {
      /* Update error code.
         Feature not supported for TT Level 0 */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_SUPPORTED;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Configure target phase used for external synchronization by event
  *         trigger input pin fdcan1_evt.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TargetPhase defines target value of cycle time when a rising edge
  *         of fdcan1_evt is expected.
  *         This parameter must be a number between 0 and 0xFFFF.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_ConfigExternalSyncPhase(FDCAN_HandleTypeDef *hfdcan, uint32_t TargetPhase)
{
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_MAX_VALUE(TargetPhase, 0xFFFFU));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Check that no external schedule synchronization is pending */
    if ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_ESCN) == FDCAN_TTOCN_ESCN)
    {
      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PENDING;

      return HAL_ERROR;
    }

    /* Configure cycle time target phase */
    MODIFY_REG(hfdcan->ttcan->TTGTP, FDCAN_TTGTP_CTP, (TargetPhase << FDCAN_TTGTP_CTP_Pos));

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Synchronize the phase of the FDCAN schedule to an external schedule
  *         using event trigger input pin fdcan1_evt.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_EnableExternalSynchronization(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Wait until the LCKC bit into TTOCN register is reset */
    while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Enable external synchronization */
    SET_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_ESCN);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable external schedule synchronization.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_DisableExternalSynchronization(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t Counter = 0U;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Wait until the LCKC bit into TTOCN register is reset */
    while ((hfdcan->ttcan->TTOCN & FDCAN_TTOCN_LCKC) != 0U)
    {
      /* Check for the Timeout */
      if (Counter > FDCAN_TIMEOUT_COUNT)
      {
        /* Update error code */
        hfdcan->ErrorCode |= HAL_FDCAN_ERROR_TIMEOUT;

        /* Change FDCAN state */
        hfdcan->State = HAL_FDCAN_STATE_ERROR;

        return HAL_ERROR;
      }

      /* Increment counter */
      Counter++;
    }

    /* Disable external synchronization */
    CLEAR_BIT(hfdcan->ttcan->TTOCN, FDCAN_TTOCN_ESCN);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Get TT operation status.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TTOpStatus pointer to an FDCAN_TTOperationStatusTypeDef structure.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_GetOperationStatus(const FDCAN_HandleTypeDef *hfdcan,
                                                  FDCAN_TTOperationStatusTypeDef *TTOpStatus)
{
  uint32_t TTStatusReg;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));

  /* Read the TT operation status register */
  TTStatusReg = READ_REG(hfdcan->ttcan->TTOST);

  /* Fill the TT operation status structure */
  TTOpStatus->ErrorLevel = (TTStatusReg & FDCAN_TTOST_EL);
  TTOpStatus->MasterState = (TTStatusReg & FDCAN_TTOST_MS);
  TTOpStatus->SyncState = (TTStatusReg & FDCAN_TTOST_SYS);
  TTOpStatus->GTimeQuality = ((TTStatusReg & FDCAN_TTOST_QGTP) >> FDCAN_TTOST_QGTP_Pos);
  TTOpStatus->ClockQuality = ((TTStatusReg & FDCAN_TTOST_QCS) >> FDCAN_TTOST_QCS_Pos);
  TTOpStatus->RefTrigOffset = ((TTStatusReg & FDCAN_TTOST_RTO) >> FDCAN_TTOST_RTO_Pos);
  TTOpStatus->GTimeDiscPending = ((TTStatusReg & FDCAN_TTOST_WGTD) >> FDCAN_TTOST_WGTD_Pos);
  TTOpStatus->GapFinished = ((TTStatusReg & FDCAN_TTOST_GFI) >> FDCAN_TTOST_GFI_Pos);
  TTOpStatus->MasterPriority = ((TTStatusReg & FDCAN_TTOST_TMP) >> FDCAN_TTOST_TMP_Pos);
  TTOpStatus->GapStarted = ((TTStatusReg & FDCAN_TTOST_GSI) >> FDCAN_TTOST_GSI_Pos);
  TTOpStatus->WaitForEvt = ((TTStatusReg & FDCAN_TTOST_WFE) >> FDCAN_TTOST_WFE_Pos);
  TTOpStatus->AppWdgEvt = ((TTStatusReg & FDCAN_TTOST_AWE) >> FDCAN_TTOST_AWE_Pos);
  TTOpStatus->ECSPending = ((TTStatusReg & FDCAN_TTOST_WECS) >> FDCAN_TTOST_WECS_Pos);
  TTOpStatus->PhaseLock = ((TTStatusReg & FDCAN_TTOST_SPL) >> FDCAN_TTOST_SPL_Pos);

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup FDCAN_Exported_Functions_Group5 Interrupts management
  *  @brief    Interrupts management
  *
@verbatim
  ==============================================================================
                       ##### Interrupts management #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) HAL_FDCAN_ConfigInterruptLines      : Assign interrupts to either Interrupt line 0 or 1
      (+) HAL_FDCAN_TT_ConfigInterruptLines   : Assign TT interrupts to either Interrupt line 0 or 1
      (+) HAL_FDCAN_ActivateNotification      : Enable interrupts
      (+) HAL_FDCAN_DeactivateNotification    : Disable interrupts
      (+) HAL_FDCAN_TT_ActivateNotification   : Enable TT interrupts
      (+) HAL_FDCAN_TT_DeactivateNotification : Disable TT interrupts
      (+) HAL_FDCAN_IRQHandler                : Handles FDCAN interrupt request

@endverbatim
  * @{
  */

/**
  * @brief  Assign interrupts to either Interrupt line 0 or 1.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  ITList indicates which interrupts will be assigned to the selected interrupt line.
  *         This parameter can be any combination of @arg FDCAN_Interrupts.
  * @param  InterruptLine Interrupt line.
  *         This parameter can be a value of @arg FDCAN_Interrupt_Line.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ConfigInterruptLines(FDCAN_HandleTypeDef *hfdcan, uint32_t ITList, uint32_t InterruptLine)
{
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_IT(ITList));
  assert_param(IS_FDCAN_IT_LINE(InterruptLine));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Assign list of interrupts to the selected line */
    if (InterruptLine == FDCAN_INTERRUPT_LINE0)
    {
      CLEAR_BIT(hfdcan->Instance->ILS, ITList);
    }
    else /* InterruptLine == FDCAN_INTERRUPT_LINE1 */
    {
      SET_BIT(hfdcan->Instance->ILS, ITList);
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Assign TT interrupts to either Interrupt line 0 or 1.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TTITList indicates which interrupts will be assigned to the selected interrupt line.
  *         This parameter can be any combination of @arg FDCAN_TTInterrupts.
  * @param  InterruptLine Interrupt line.
  *         This parameter can be a value of @arg FDCAN_Interrupt_Line.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_ConfigInterruptLines(FDCAN_HandleTypeDef *hfdcan, uint32_t TTITList,
                                                    uint32_t InterruptLine)
{
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_TT_IT(TTITList));
  assert_param(IS_FDCAN_IT_LINE(InterruptLine));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Assign list of interrupts to the selected line */
    if (InterruptLine == FDCAN_INTERRUPT_LINE0)
    {
      CLEAR_BIT(hfdcan->ttcan->TTILS, TTITList);
    }
    else /* InterruptLine == FDCAN_INTERRUPT_LINE1 */
    {
      SET_BIT(hfdcan->ttcan->TTILS, TTITList);
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable interrupts.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  ActiveITs indicates which interrupts will be enabled.
  *         This parameter can be any combination of @arg FDCAN_Interrupts.
  * @param  BufferIndexes Tx Buffer Indexes.
  *         This parameter can be any combination of @arg FDCAN_Tx_location.
  *         This parameter is ignored if ActiveITs does not include one of the following:
  *           - FDCAN_IT_TX_COMPLETE
  *           - FDCAN_IT_TX_ABORT_COMPLETE
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_ActivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t ActiveITs,
                                                 uint32_t BufferIndexes)
{
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_IT(ActiveITs));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Enable Interrupt lines */
    if ((ActiveITs & hfdcan->Instance->ILS) == 0U)
    {
      /* Enable Interrupt line 0 */
      SET_BIT(hfdcan->Instance->ILE, FDCAN_INTERRUPT_LINE0);
    }
    else if ((ActiveITs & hfdcan->Instance->ILS) == ActiveITs)
    {
      /* Enable Interrupt line 1 */
      SET_BIT(hfdcan->Instance->ILE, FDCAN_INTERRUPT_LINE1);
    }
    else
    {
      /* Enable Interrupt lines 0 and 1 */
      hfdcan->Instance->ILE = (FDCAN_INTERRUPT_LINE0 | FDCAN_INTERRUPT_LINE1);
    }

    if ((ActiveITs & FDCAN_IT_TX_COMPLETE) != 0U)
    {
      /* Enable Tx Buffer Transmission Interrupt to set TC flag in IR register,
         but interrupt will only occur if TC is enabled in IE register */
      SET_BIT(hfdcan->Instance->TXBTIE, BufferIndexes);
    }

    if ((ActiveITs & FDCAN_IT_TX_ABORT_COMPLETE) != 0U)
    {
      /* Enable Tx Buffer Cancellation Finished Interrupt to set TCF flag in IR register,
         but interrupt will only occur if TCF is enabled in IE register */
      SET_BIT(hfdcan->Instance->TXBCIE, BufferIndexes);
    }

    /* Enable the selected interrupts */
    __HAL_FDCAN_ENABLE_IT(hfdcan, ActiveITs);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable interrupts.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  InactiveITs indicates which interrupts will be disabled.
  *         This parameter can be any combination of @arg FDCAN_Interrupts.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_DeactivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t InactiveITs)
{
  uint32_t ITLineSelection;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_IT(InactiveITs));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Disable the selected interrupts */
    __HAL_FDCAN_DISABLE_IT(hfdcan, InactiveITs);

    if ((InactiveITs & FDCAN_IT_TX_COMPLETE) != 0U)
    {
      /* Disable Tx Buffer Transmission Interrupts */
      CLEAR_REG(hfdcan->Instance->TXBTIE);
    }

    if ((InactiveITs & FDCAN_IT_TX_ABORT_COMPLETE) != 0U)
    {
      /* Disable Tx Buffer Cancellation Finished Interrupt */
      CLEAR_REG(hfdcan->Instance->TXBCIE);
    }

    ITLineSelection = hfdcan->Instance->ILS;

    if ((hfdcan->Instance->IE | ITLineSelection) == ITLineSelection)
    {
      /* Disable Interrupt line 0 */
      CLEAR_BIT(hfdcan->Instance->ILE, FDCAN_INTERRUPT_LINE0);
    }

    if ((hfdcan->Instance->IE & ITLineSelection) == 0U)
    {
      /* Disable Interrupt line 1 */
      CLEAR_BIT(hfdcan->Instance->ILE, FDCAN_INTERRUPT_LINE1);
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Enable TT interrupts.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  ActiveTTITs indicates which TT interrupts will be enabled.
  *         This parameter can be any combination of @arg FDCAN_TTInterrupts.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_ActivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t ActiveTTITs)
{
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_TT_IT(ActiveTTITs));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Enable Interrupt lines */
    if ((ActiveTTITs & hfdcan->ttcan->TTILS) == 0U)
    {
      /* Enable Interrupt line 0 */
      SET_BIT(hfdcan->Instance->ILE, FDCAN_INTERRUPT_LINE0);
    }
    else if ((ActiveTTITs & hfdcan->ttcan->TTILS) == ActiveTTITs)
    {
      /* Enable Interrupt line 1 */
      SET_BIT(hfdcan->Instance->ILE, FDCAN_INTERRUPT_LINE1);
    }
    else
    {
      /* Enable Interrupt lines 0 and 1 */
      hfdcan->Instance->ILE = (FDCAN_INTERRUPT_LINE0 | FDCAN_INTERRUPT_LINE1);
    }

    /* Enable the selected TT interrupts */
    __HAL_FDCAN_TT_ENABLE_IT(hfdcan, ActiveTTITs);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable TT interrupts.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  InactiveTTITs indicates which TT interrupts will be disabled.
  *         This parameter can be any combination of @arg FDCAN_TTInterrupts.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_FDCAN_TT_DeactivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t InactiveTTITs)
{
  uint32_t ITLineSelection;
  HAL_FDCAN_StateTypeDef state = hfdcan->State;

  /* Check function parameters */
  assert_param(IS_FDCAN_TT_INSTANCE(hfdcan->Instance));
  assert_param(IS_FDCAN_TT_IT(InactiveTTITs));

  if ((state == HAL_FDCAN_STATE_READY) || (state == HAL_FDCAN_STATE_BUSY))
  {
    /* Disable the selected TT interrupts */
    __HAL_FDCAN_TT_DISABLE_IT(hfdcan, InactiveTTITs);

    ITLineSelection = hfdcan->ttcan->TTILS;

    if ((hfdcan->ttcan->TTIE | ITLineSelection) == ITLineSelection)
    {
      /* Disable Interrupt line 0 */
      CLEAR_BIT(hfdcan->Instance->ILE, FDCAN_INTERRUPT_LINE0);
    }

    if ((hfdcan->ttcan->TTIE & ITLineSelection) == 0U)
    {
      /* Disable interrupt line 1 */
      CLEAR_BIT(hfdcan->Instance->ILE, FDCAN_INTERRUPT_LINE1);
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Handles FDCAN interrupt request.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
  */
void HAL_FDCAN_IRQHandler(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t ClkCalibrationITs;
  uint32_t TxEventFifoITs;
  uint32_t RxFifo0ITs;
  uint32_t RxFifo1ITs;
  uint32_t Errors;
  uint32_t ErrorStatusITs;
  uint32_t TransmittedBuffers;
  uint32_t AbortedBuffers;
  uint32_t TTSchedSyncITs;
  uint32_t TTTimeMarkITs;
  uint32_t TTGlobTimeITs;
  uint32_t TTDistErrors;
  uint32_t TTFatalErrors;
  uint32_t SWTime;
  uint32_t SWCycleCount;
  uint32_t itsourceIE;
  uint32_t itsourceTTIE;
  uint32_t itflagIR;
  uint32_t itflagTTIR;

  ClkCalibrationITs = (FDCAN_CCU->IR << 30);
  ClkCalibrationITs &= (FDCAN_CCU->IE << 30);
  TxEventFifoITs = hfdcan->Instance->IR & FDCAN_TX_EVENT_FIFO_MASK;
  TxEventFifoITs &= hfdcan->Instance->IE;
  RxFifo0ITs = hfdcan->Instance->IR & FDCAN_RX_FIFO0_MASK;
  RxFifo0ITs &= hfdcan->Instance->IE;
  RxFifo1ITs = hfdcan->Instance->IR & FDCAN_RX_FIFO1_MASK;
  RxFifo1ITs &= hfdcan->Instance->IE;
  Errors = hfdcan->Instance->IR & FDCAN_ERROR_MASK;
  Errors &= hfdcan->Instance->IE;
  ErrorStatusITs = hfdcan->Instance->IR & FDCAN_ERROR_STATUS_MASK;
  ErrorStatusITs &= hfdcan->Instance->IE;
  itsourceIE = hfdcan->Instance->IE;
  itflagIR = hfdcan->Instance->IR;

  /* High Priority Message interrupt management *******************************/
  if (FDCAN_CHECK_IT_SOURCE(itsourceIE, FDCAN_IT_RX_HIGH_PRIORITY_MSG) != RESET)
  {
    if (FDCAN_CHECK_FLAG(itflagIR, FDCAN_FLAG_RX_HIGH_PRIORITY_MSG) != RESET)
    {
      /* Clear the High Priority Message flag */
      __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_RX_HIGH_PRIORITY_MSG);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
      /* Call registered callback*/
      hfdcan->HighPriorityMessageCallback(hfdcan);
#else
      /* High Priority Message Callback */
      HAL_FDCAN_HighPriorityMessageCallback(hfdcan);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
    }
  }

  /* Transmission Abort interrupt management **********************************/
  if (FDCAN_CHECK_IT_SOURCE(itsourceIE, FDCAN_IT_TX_ABORT_COMPLETE) != RESET)
  {
    if (FDCAN_CHECK_FLAG(itflagIR, FDCAN_FLAG_TX_ABORT_COMPLETE) != RESET)
    {
      /* List of aborted monitored buffers */
      AbortedBuffers = hfdcan->Instance->TXBCF;
      AbortedBuffers &= hfdcan->Instance->TXBCIE;

      /* Clear the Transmission Cancellation flag */
      __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_TX_ABORT_COMPLETE);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
      /* Call registered callback*/
      hfdcan->TxBufferAbortCallback(hfdcan, AbortedBuffers);
#else
      /* Transmission Cancellation Callback */
      HAL_FDCAN_TxBufferAbortCallback(hfdcan, AbortedBuffers);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
    }
  }

  /* Clock calibration unit interrupts management *****************************/
  if (ClkCalibrationITs != 0U)
  {
    /* Clear the Clock Calibration flags */
    __HAL_FDCAN_CLEAR_FLAG(hfdcan, ClkCalibrationITs);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
    /* Call registered callback*/
    hfdcan->ClockCalibrationCallback(hfdcan, ClkCalibrationITs);
#else
    /* Clock Calibration Callback */
    HAL_FDCAN_ClockCalibrationCallback(hfdcan, ClkCalibrationITs);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
  }

  /* Tx event FIFO interrupts management **************************************/
  if (TxEventFifoITs != 0U)
  {
    /* Clear the Tx Event FIFO flags */
    __HAL_FDCAN_CLEAR_FLAG(hfdcan, TxEventFifoITs);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
    /* Call registered callback*/
    hfdcan->TxEventFifoCallback(hfdcan, TxEventFifoITs);
#else
    /* Tx Event FIFO Callback */
    HAL_FDCAN_TxEventFifoCallback(hfdcan, TxEventFifoITs);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
  }

  /* Rx FIFO 0 interrupts management ******************************************/
  if (RxFifo0ITs != 0U)
  {
    /* Clear the Rx FIFO 0 flags */
    __HAL_FDCAN_CLEAR_FLAG(hfdcan, RxFifo0ITs);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
    /* Call registered callback*/
    hfdcan->RxFifo0Callback(hfdcan, RxFifo0ITs);
#else
    /* Rx FIFO 0 Callback */
    HAL_FDCAN_RxFifo0Callback(hfdcan, RxFifo0ITs);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
  }

  /* Rx FIFO 1 interrupts management ******************************************/
  if (RxFifo1ITs != 0U)
  {
    /* Clear the Rx FIFO 1 flags */
    __HAL_FDCAN_CLEAR_FLAG(hfdcan, RxFifo1ITs);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
    /* Call registered callback*/
    hfdcan->RxFifo1Callback(hfdcan, RxFifo1ITs);
#else
    /* Rx FIFO 1 Callback */
    HAL_FDCAN_RxFifo1Callback(hfdcan, RxFifo1ITs);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
  }

  /* Tx FIFO empty interrupt management ***************************************/
  if (FDCAN_CHECK_IT_SOURCE(itsourceIE, FDCAN_IT_TX_FIFO_EMPTY) != RESET)
  {
    if (FDCAN_CHECK_FLAG(itflagIR, FDCAN_FLAG_TX_FIFO_EMPTY) != RESET)
    {
      /* Clear the Tx FIFO empty flag */
      __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_TX_FIFO_EMPTY);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
      /* Call registered callback*/
      hfdcan->TxFifoEmptyCallback(hfdcan);
#else
      /* Tx FIFO empty Callback */
      HAL_FDCAN_TxFifoEmptyCallback(hfdcan);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
    }
  }

  /* Transmission Complete interrupt management *******************************/
  if (FDCAN_CHECK_IT_SOURCE(itsourceIE, FDCAN_IT_TX_COMPLETE) != RESET)
  {
    if (FDCAN_CHECK_FLAG(itflagIR, FDCAN_FLAG_TX_COMPLETE) != RESET)
    {
      /* List of transmitted monitored buffers */
      TransmittedBuffers = hfdcan->Instance->TXBTO;
      TransmittedBuffers &= hfdcan->Instance->TXBTIE;

      /* Clear the Transmission Complete flag */
      __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_TX_COMPLETE);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
      /* Call registered callback*/
      hfdcan->TxBufferCompleteCallback(hfdcan, TransmittedBuffers);
#else
      /* Transmission Complete Callback */
      HAL_FDCAN_TxBufferCompleteCallback(hfdcan, TransmittedBuffers);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
    }
  }

  /* Rx Buffer New Message interrupt management *******************************/
  if (FDCAN_CHECK_IT_SOURCE(itsourceIE, FDCAN_IT_RX_BUFFER_NEW_MESSAGE) != RESET)
  {
    if (FDCAN_CHECK_FLAG(itflagIR, FDCAN_FLAG_RX_BUFFER_NEW_MESSAGE) != RESET)
    {
      /* Clear the Rx Buffer New Message flag */
      __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_RX_BUFFER_NEW_MESSAGE);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
      /* Call registered callback*/
      hfdcan->RxBufferNewMessageCallback(hfdcan);
#else
      /* Rx Buffer New Message Callback */
      HAL_FDCAN_RxBufferNewMessageCallback(hfdcan);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
    }
  }

  /* Timestamp Wraparound interrupt management ********************************/
  if (FDCAN_CHECK_IT_SOURCE(itsourceIE, FDCAN_IT_TIMESTAMP_WRAPAROUND) != RESET)
  {
    if (FDCAN_CHECK_FLAG(itflagIR, FDCAN_FLAG_TIMESTAMP_WRAPAROUND) != RESET)
    {
      /* Clear the Timestamp Wraparound flag */
      __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_TIMESTAMP_WRAPAROUND);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
      /* Call registered callback*/
      hfdcan->TimestampWraparoundCallback(hfdcan);
#else
      /* Timestamp Wraparound Callback */
      HAL_FDCAN_TimestampWraparoundCallback(hfdcan);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
    }
  }

  /* Timeout Occurred interrupt management ************************************/
  if (FDCAN_CHECK_IT_SOURCE(itsourceIE, FDCAN_IT_TIMEOUT_OCCURRED) != RESET)
  {
    if (FDCAN_CHECK_FLAG(itflagIR, FDCAN_FLAG_TIMEOUT_OCCURRED) != RESET)
    {
      /* Clear the Timeout Occurred flag */
      __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_TIMEOUT_OCCURRED);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
      /* Call registered callback*/
      hfdcan->TimeoutOccurredCallback(hfdcan);
#else
      /* Timeout Occurred Callback */
      HAL_FDCAN_TimeoutOccurredCallback(hfdcan);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
    }
  }

  /* Message RAM access failure interrupt management **************************/
  if (FDCAN_CHECK_IT_SOURCE(itsourceIE, FDCAN_IT_RAM_ACCESS_FAILURE) != RESET)
  {
    if (FDCAN_CHECK_FLAG(itflagIR, FDCAN_FLAG_RAM_ACCESS_FAILURE) != RESET)
    {
      /* Clear the Message RAM access failure flag */
      __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_RAM_ACCESS_FAILURE);

      /* Update error code */
      hfdcan->ErrorCode |= HAL_FDCAN_ERROR_RAM_ACCESS;
    }
  }

  /* Error Status interrupts management ***************************************/
  if (ErrorStatusITs != 0U)
  {
    /* Clear the Error flags */
    __HAL_FDCAN_CLEAR_FLAG(hfdcan, ErrorStatusITs);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
    /* Call registered callback*/
    hfdcan->ErrorStatusCallback(hfdcan, ErrorStatusITs);
#else
    /* Error Status Callback */
    HAL_FDCAN_ErrorStatusCallback(hfdcan, ErrorStatusITs);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
  }

  /* Error interrupts management **********************************************/
  if (Errors != 0U)
  {
    /* Clear the Error flags */
    __HAL_FDCAN_CLEAR_FLAG(hfdcan, Errors);

    /* Update error code */
    hfdcan->ErrorCode |= Errors;
  }

  if (hfdcan->Instance == FDCAN1)
  {
    if ((hfdcan->ttcan->TTOCF & FDCAN_TTOCF_OM) != 0U)
    {
      TTSchedSyncITs = hfdcan->ttcan->TTIR & FDCAN_TT_SCHEDULE_SYNC_MASK;
      TTSchedSyncITs &= hfdcan->ttcan->TTIE;
      TTTimeMarkITs = hfdcan->ttcan->TTIR & FDCAN_TT_TIME_MARK_MASK;
      TTTimeMarkITs &= hfdcan->ttcan->TTIE;
      TTGlobTimeITs = hfdcan->ttcan->TTIR & FDCAN_TT_GLOBAL_TIME_MASK;
      TTGlobTimeITs &= hfdcan->ttcan->TTIE;
      TTDistErrors = hfdcan->ttcan->TTIR & FDCAN_TT_DISTURBING_ERROR_MASK;
      TTDistErrors &= hfdcan->ttcan->TTIE;
      TTFatalErrors = hfdcan->ttcan->TTIR & FDCAN_TT_FATAL_ERROR_MASK;
      TTFatalErrors &= hfdcan->ttcan->TTIE;
      itsourceTTIE = hfdcan->ttcan->TTIE;
      itflagTTIR = hfdcan->ttcan->TTIR;

      /* TT Schedule Synchronization interrupts management **********************/
      if (TTSchedSyncITs != 0U)
      {
        /* Clear the TT Schedule Synchronization flags */
        __HAL_FDCAN_TT_CLEAR_FLAG(hfdcan, TTSchedSyncITs);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
        /* Call registered callback*/
        hfdcan->TT_ScheduleSyncCallback(hfdcan, TTSchedSyncITs);
#else
        /* TT Schedule Synchronization Callback */
        HAL_FDCAN_TT_ScheduleSyncCallback(hfdcan, TTSchedSyncITs);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
      }

      /* TT Time Mark interrupts management *************************************/
      if (TTTimeMarkITs != 0U)
      {
        /* Clear the TT Time Mark flags */
        __HAL_FDCAN_TT_CLEAR_FLAG(hfdcan, TTTimeMarkITs);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
        /* Call registered callback*/
        hfdcan->TT_TimeMarkCallback(hfdcan, TTTimeMarkITs);
#else
        /* TT Time Mark Callback */
        HAL_FDCAN_TT_TimeMarkCallback(hfdcan, TTTimeMarkITs);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
      }

      /* TT Stop Watch interrupt management *************************************/
      if (FDCAN_CHECK_IT_SOURCE(itsourceTTIE, FDCAN_TT_IT_STOP_WATCH) != RESET)
      {
        if (FDCAN_CHECK_FLAG(itflagTTIR, FDCAN_TT_FLAG_STOP_WATCH) != RESET)
        {
          /* Retrieve Stop watch Time and Cycle count */
          SWTime = ((hfdcan->ttcan->TTCPT & FDCAN_TTCPT_SWV) >> FDCAN_TTCPT_SWV_Pos);
          SWCycleCount = ((hfdcan->ttcan->TTCPT & FDCAN_TTCPT_CCV) >> FDCAN_TTCPT_CCV_Pos);

          /* Clear the TT Stop Watch flag */
          __HAL_FDCAN_TT_CLEAR_FLAG(hfdcan, FDCAN_TT_FLAG_STOP_WATCH);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
          /* Call registered callback*/
          hfdcan->TT_StopWatchCallback(hfdcan, SWTime, SWCycleCount);
#else
          /* TT Stop Watch Callback */
          HAL_FDCAN_TT_StopWatchCallback(hfdcan, SWTime, SWCycleCount);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
        }
      }

      /* TT Global Time interrupts management ***********************************/
      if (TTGlobTimeITs != 0U)
      {
        /* Clear the TT Global Time flags */
        __HAL_FDCAN_TT_CLEAR_FLAG(hfdcan, TTGlobTimeITs);

#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
        /* Call registered callback*/
        hfdcan->TT_GlobalTimeCallback(hfdcan, TTGlobTimeITs);
#else
        /* TT Global Time Callback */
        HAL_FDCAN_TT_GlobalTimeCallback(hfdcan, TTGlobTimeITs);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
      }

      /* TT Disturbing Error interrupts management ******************************/
      if (TTDistErrors != 0U)
      {
        /* Clear the TT Disturbing Error flags */
        __HAL_FDCAN_TT_CLEAR_FLAG(hfdcan, TTDistErrors);

        /* Update error code */
        hfdcan->ErrorCode |= TTDistErrors;
      }

      /* TT Fatal Error interrupts management ***********************************/
      if (TTFatalErrors != 0U)
      {
        /* Clear the TT Fatal Error flags */
        __HAL_FDCAN_TT_CLEAR_FLAG(hfdcan, TTFatalErrors);

        /* Update error code */
        hfdcan->ErrorCode |= TTFatalErrors;
      }
    }
  }

  if (hfdcan->ErrorCode != HAL_FDCAN_ERROR_NONE)
  {
#if USE_HAL_FDCAN_REGISTER_CALLBACKS == 1
    /* Call registered callback*/
    hfdcan->ErrorCallback(hfdcan);
#else
    /* Error Callback */
    HAL_FDCAN_ErrorCallback(hfdcan);
#endif /* USE_HAL_FDCAN_REGISTER_CALLBACKS */
  }
}

/**
  * @}
  */

/** @defgroup FDCAN_Exported_Functions_Group6 Callback functions
  *  @brief   FDCAN Callback functions
  *
@verbatim
  ==============================================================================
                          ##### Callback functions #####
  ==============================================================================
    [..]
    This subsection provides the following callback functions:
      (+) HAL_FDCAN_ClockCalibrationCallback
      (+) HAL_FDCAN_TxEventFifoCallback
      (+) HAL_FDCAN_RxFifo0Callback
      (+) HAL_FDCAN_RxFifo1Callback
      (+) HAL_FDCAN_TxFifoEmptyCallback
      (+) HAL_FDCAN_TxBufferCompleteCallback
      (+) HAL_FDCAN_TxBufferAbortCallback
      (+) HAL_FDCAN_RxBufferNewMessageCallback
      (+) HAL_FDCAN_HighPriorityMessageCallback
      (+) HAL_FDCAN_TimestampWraparoundCallback
      (+) HAL_FDCAN_TimeoutOccurredCallback
      (+) HAL_FDCAN_ErrorCallback
      (+) HAL_FDCAN_ErrorStatusCallback
      (+) HAL_FDCAN_TT_ScheduleSyncCallback
      (+) HAL_FDCAN_TT_TimeMarkCallback
      (+) HAL_FDCAN_TT_StopWatchCallback
      (+) HAL_FDCAN_TT_GlobalTimeCallback

@endverbatim
  * @{
  */

/**
  * @brief  Clock Calibration callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  ClkCalibrationITs indicates which Clock Calibration interrupts are signaled.
  *         This parameter can be any combination of @arg FDCAN_Clock_Calibration_Interrupts.
  * @retval None
  */
__weak void HAL_FDCAN_ClockCalibrationCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ClkCalibrationITs)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(ClkCalibrationITs);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_ClockCalibrationCallback could be implemented in the user file
   */
}

/**
  * @brief  Tx Event callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TxEventFifoITs indicates which Tx Event FIFO interrupts are signaled.
  *         This parameter can be any combination of @arg FDCAN_Tx_Event_Fifo_Interrupts.
  * @retval None
  */
__weak void HAL_FDCAN_TxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TxEventFifoITs)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(TxEventFifoITs);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TxEventFifoCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx FIFO 0 callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  RxFifo0ITs indicates which Rx FIFO 0 interrupts are signaled.
  *         This parameter can be any combination of @arg FDCAN_Rx_Fifo0_Interrupts.
  * @retval None
  */
__weak void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(RxFifo0ITs);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_RxFifo0Callback could be implemented in the user file
   */
}

/**
  * @brief  Rx FIFO 1 callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  RxFifo1ITs indicates which Rx FIFO 1 interrupts are signaled.
  *         This parameter can be any combination of @arg FDCAN_Rx_Fifo1_Interrupts.
  * @retval None
  */
__weak void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(RxFifo1ITs);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_RxFifo1Callback could be implemented in the user file
   */
}

/**
  * @brief  Tx FIFO Empty callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
__weak void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TxFifoEmptyCallback could be implemented in the user file
   */
}

/**
  * @brief  Transmission Complete callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  BufferIndexes Indexes of the transmitted buffers.
  *         This parameter can be any combination of @arg FDCAN_Tx_location.
  * @retval None
  */
__weak void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(BufferIndexes);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TxBufferCompleteCallback could be implemented in the user file
   */
}

/**
  * @brief  Transmission Cancellation callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  BufferIndexes Indexes of the aborted buffers.
  *         This parameter can be any combination of @arg FDCAN_Tx_location.
  * @retval None
  */
__weak void HAL_FDCAN_TxBufferAbortCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(BufferIndexes);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TxBufferAbortCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Buffer New Message callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
__weak void HAL_FDCAN_RxBufferNewMessageCallback(FDCAN_HandleTypeDef *hfdcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_RxBufferNewMessageCallback could be implemented in the user file
   */
}

/**
  * @brief  Timestamp Wraparound callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
__weak void HAL_FDCAN_TimestampWraparoundCallback(FDCAN_HandleTypeDef *hfdcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TimestampWraparoundCallback could be implemented in the user file
   */
}

/**
  * @brief  Timeout Occurred callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
__weak void HAL_FDCAN_TimeoutOccurredCallback(FDCAN_HandleTypeDef *hfdcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TimeoutOccurredCallback could be implemented in the user file
   */
}

/**
  * @brief  High Priority Message callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
__weak void HAL_FDCAN_HighPriorityMessageCallback(FDCAN_HandleTypeDef *hfdcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_HighPriorityMessageCallback could be implemented in the user file
   */
}

/**
  * @brief  Error callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
__weak void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_ErrorCallback could be implemented in the user file
   */
}

/**
  * @brief  Error status callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  ErrorStatusITs indicates which Error Status interrupts are signaled.
  *         This parameter can be any combination of @arg FDCAN_Error_Status_Interrupts.
  * @retval None
  */
__weak void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(ErrorStatusITs);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_ErrorStatusCallback could be implemented in the user file
   */
}

/**
  * @brief  TT Schedule Synchronization callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TTSchedSyncITs indicates which TT Schedule Synchronization interrupts are signaled.
  *         This parameter can be any combination of @arg FDCAN_TTScheduleSynchronization_Interrupts.
  * @retval None
  */
__weak void HAL_FDCAN_TT_ScheduleSyncCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TTSchedSyncITs)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(TTSchedSyncITs);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TT_ScheduleSyncCallback could be implemented in the user file
   */
}

/**
  * @brief  TT Time Mark callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TTTimeMarkITs indicates which TT Schedule Synchronization interrupts are signaled.
  *         This parameter can be any combination of @arg FDCAN_TTTimeMark_Interrupts.
  * @retval None
  */
__weak void HAL_FDCAN_TT_TimeMarkCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TTTimeMarkITs)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(TTTimeMarkITs);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TT_TimeMarkCallback could be implemented in the user file
   */
}

/**
  * @brief  TT Stop Watch callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  SWTime Time Value captured at the Stop Watch Trigger pin (fdcan1_swt) falling/rising
  *         edge (as configured via HAL_FDCAN_TTConfigStopWatch).
  *         This parameter is a number between 0 and 0xFFFF.
  * @param  SWCycleCount Cycle count value captured together with SWTime.
  *         This parameter is a number between 0 and 0x3F.
  * @retval None
  */
__weak void HAL_FDCAN_TT_StopWatchCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t SWTime, uint32_t SWCycleCount)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(SWTime);
  UNUSED(SWCycleCount);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TT_StopWatchCallback could be implemented in the user file
   */
}

/**
  * @brief  TT Global Time callback.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  TTGlobTimeITs indicates which TT Global Time interrupts are signaled.
  *         This parameter can be any combination of @arg FDCAN_TTGlobalTime_Interrupts.
  * @retval None
  */
__weak void HAL_FDCAN_TT_GlobalTimeCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TTGlobTimeITs)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hfdcan);
  UNUSED(TTGlobTimeITs);

  /* NOTE: This function Should not be modified, when the callback is needed,
            the HAL_FDCAN_TT_GlobalTimeCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup FDCAN_Exported_Functions_Group7 Peripheral State functions
  *  @brief   FDCAN Peripheral State functions
  *
@verbatim
  ==============================================================================
                      ##### Peripheral State functions #####
  ==============================================================================
    [..]
    This subsection provides functions allowing to :
      (+) HAL_FDCAN_GetState()  : Return the FDCAN state.
      (+) HAL_FDCAN_GetError()  : Return the FDCAN error code if any.

@endverbatim
  * @{
  */
/**
  * @brief  Return the FDCAN state
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL state
  */
HAL_FDCAN_StateTypeDef HAL_FDCAN_GetState(const FDCAN_HandleTypeDef *hfdcan)
{
  /* Return FDCAN state */
  return hfdcan->State;
}

/**
  * @brief  Return the FDCAN error code
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval FDCAN Error Code
  */
uint32_t HAL_FDCAN_GetError(const FDCAN_HandleTypeDef *hfdcan)
{
  /* Return FDCAN error code */
  return hfdcan->ErrorCode;
}

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup FDCAN_Private_Functions FDCAN Private Functions
  * @{
  */

/**
  * @brief  Calculate each RAM block start address and size
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval HAL status
 */
static HAL_StatusTypeDef FDCAN_CalcultateRamBlockAddresses(FDCAN_HandleTypeDef *hfdcan)
{
  uint32_t RAMcounter;
  uint32_t StartAddress;

  StartAddress = hfdcan->Init.MessageRAMOffset;

  /* Standard filter list start address */
  MODIFY_REG(hfdcan->Instance->SIDFC, FDCAN_SIDFC_FLSSA, (StartAddress << FDCAN_SIDFC_FLSSA_Pos));

  /* Standard filter elements number */
  MODIFY_REG(hfdcan->Instance->SIDFC, FDCAN_SIDFC_LSS, (hfdcan->Init.StdFiltersNbr << FDCAN_SIDFC_LSS_Pos));

  /* Extended filter list start address */
  StartAddress += hfdcan->Init.StdFiltersNbr;
  MODIFY_REG(hfdcan->Instance->XIDFC, FDCAN_XIDFC_FLESA, (StartAddress << FDCAN_XIDFC_FLESA_Pos));

  /* Extended filter elements number */
  MODIFY_REG(hfdcan->Instance->XIDFC, FDCAN_XIDFC_LSE, (hfdcan->Init.ExtFiltersNbr << FDCAN_XIDFC_LSE_Pos));

  /* Rx FIFO 0 start address */
  StartAddress += (hfdcan->Init.ExtFiltersNbr * 2U);
  MODIFY_REG(hfdcan->Instance->RXF0C, FDCAN_RXF0C_F0SA, (StartAddress << FDCAN_RXF0C_F0SA_Pos));

  /* Rx FIFO 0 elements number */
  MODIFY_REG(hfdcan->Instance->RXF0C, FDCAN_RXF0C_F0S, (hfdcan->Init.RxFifo0ElmtsNbr << FDCAN_RXF0C_F0S_Pos));

  /* Rx FIFO 1 start address */
  StartAddress += (hfdcan->Init.RxFifo0ElmtsNbr * hfdcan->Init.RxFifo0ElmtSize);
  MODIFY_REG(hfdcan->Instance->RXF1C, FDCAN_RXF1C_F1SA, (StartAddress << FDCAN_RXF1C_F1SA_Pos));

  /* Rx FIFO 1 elements number */
  MODIFY_REG(hfdcan->Instance->RXF1C, FDCAN_RXF1C_F1S, (hfdcan->Init.RxFifo1ElmtsNbr << FDCAN_RXF1C_F1S_Pos));

  /* Rx buffer list start address */
  StartAddress += (hfdcan->Init.RxFifo1ElmtsNbr * hfdcan->Init.RxFifo1ElmtSize);
  MODIFY_REG(hfdcan->Instance->RXBC, FDCAN_RXBC_RBSA, (StartAddress << FDCAN_RXBC_RBSA_Pos));

  /* Tx event FIFO start address */
  StartAddress += (hfdcan->Init.RxBuffersNbr * hfdcan->Init.RxBufferSize);
  MODIFY_REG(hfdcan->Instance->TXEFC, FDCAN_TXEFC_EFSA, (StartAddress << FDCAN_TXEFC_EFSA_Pos));

  /* Tx event FIFO elements number */
  MODIFY_REG(hfdcan->Instance->TXEFC, FDCAN_TXEFC_EFS, (hfdcan->Init.TxEventsNbr << FDCAN_TXEFC_EFS_Pos));

  /* Tx buffer list start address */
  StartAddress += (hfdcan->Init.TxEventsNbr * 2U);
  MODIFY_REG(hfdcan->Instance->TXBC, FDCAN_TXBC_TBSA, (StartAddress << FDCAN_TXBC_TBSA_Pos));

  /* Dedicated Tx buffers number */
  MODIFY_REG(hfdcan->Instance->TXBC, FDCAN_TXBC_NDTB, (hfdcan->Init.TxBuffersNbr << FDCAN_TXBC_NDTB_Pos));

  /* Tx FIFO/queue elements number */
  MODIFY_REG(hfdcan->Instance->TXBC, FDCAN_TXBC_TFQS, (hfdcan->Init.TxFifoQueueElmtsNbr << FDCAN_TXBC_TFQS_Pos));

  hfdcan->msgRam.StandardFilterSA = SRAMCAN_BASE + (hfdcan->Init.MessageRAMOffset * 4U);
  hfdcan->msgRam.ExtendedFilterSA = hfdcan->msgRam.StandardFilterSA + (hfdcan->Init.StdFiltersNbr * 4U);
  hfdcan->msgRam.RxFIFO0SA = hfdcan->msgRam.ExtendedFilterSA + (hfdcan->Init.ExtFiltersNbr * 2U * 4U);
  hfdcan->msgRam.RxFIFO1SA = hfdcan->msgRam.RxFIFO0SA +
                             (hfdcan->Init.RxFifo0ElmtsNbr * hfdcan->Init.RxFifo0ElmtSize * 4U);
  hfdcan->msgRam.RxBufferSA = hfdcan->msgRam.RxFIFO1SA +
                              (hfdcan->Init.RxFifo1ElmtsNbr * hfdcan->Init.RxFifo1ElmtSize * 4U);
  hfdcan->msgRam.TxEventFIFOSA = hfdcan->msgRam.RxBufferSA +
                                 (hfdcan->Init.RxBuffersNbr * hfdcan->Init.RxBufferSize * 4U);
  hfdcan->msgRam.TxBufferSA = hfdcan->msgRam.TxEventFIFOSA + (hfdcan->Init.TxEventsNbr * 2U * 4U);
  hfdcan->msgRam.TxFIFOQSA = hfdcan->msgRam.TxBufferSA + (hfdcan->Init.TxBuffersNbr * hfdcan->Init.TxElmtSize * 4U);

  hfdcan->msgRam.EndAddress = hfdcan->msgRam.TxFIFOQSA +
                              (hfdcan->Init.TxFifoQueueElmtsNbr * hfdcan->Init.TxElmtSize * 4U);

  if (hfdcan->msgRam.EndAddress > FDCAN_MESSAGE_RAM_END_ADDRESS) /* Last address of the Message RAM */
  {
    /* Update error code.
       Message RAM overflow */
    hfdcan->ErrorCode |= HAL_FDCAN_ERROR_PARAM;

    /* Change FDCAN state */
    hfdcan->State = HAL_FDCAN_STATE_ERROR;

    return HAL_ERROR;
  }
  else
  {
    /* Flush the allocated Message RAM area */
    for (RAMcounter = hfdcan->msgRam.StandardFilterSA; RAMcounter < hfdcan->msgRam.EndAddress; RAMcounter += 4U)
    {
      *(uint32_t *)(RAMcounter) = 0x00000000;
    }
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Copy Tx message to the message RAM.
  * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  pTxHeader pointer to a FDCAN_TxHeaderTypeDef structure.
  * @param  pTxData pointer to a buffer containing the payload of the Tx frame.
  * @param  BufferIndex index of the buffer to be configured.
  * @retval none
 */
static void FDCAN_CopyMessageToRAM(const FDCAN_HandleTypeDef *hfdcan, const FDCAN_TxHeaderTypeDef *pTxHeader,
                                   const uint8_t *pTxData, uint32_t BufferIndex)
{
  uint32_t TxElementW1;
  uint32_t TxElementW2;
  uint32_t *TxAddress;
  uint32_t ByteCounter;

  /* Build first word of Tx header element */
  if (pTxHeader->IdType == FDCAN_STANDARD_ID)
  {
    TxElementW1 = (pTxHeader->ErrorStateIndicator |
                   FDCAN_STANDARD_ID |
                   pTxHeader->TxFrameType |
                   (pTxHeader->Identifier << 18U));
  }
  else /* pTxHeader->IdType == FDCAN_EXTENDED_ID */
  {
    TxElementW1 = (pTxHeader->ErrorStateIndicator |
                   FDCAN_EXTENDED_ID |
                   pTxHeader->TxFrameType |
                   pTxHeader->Identifier);
  }

  /* Build second word of Tx header element */
  TxElementW2 = ((pTxHeader->MessageMarker << 24U) |
                 pTxHeader->TxEventFifoControl |
                 pTxHeader->FDFormat |
                 pTxHeader->BitRateSwitch |
                 (pTxHeader->DataLength << 16U));

  /* Calculate Tx element address */
  TxAddress = (uint32_t *)(hfdcan->msgRam.TxBufferSA + (BufferIndex * hfdcan->Init.TxElmtSize * 4U));

  /* Write Tx element header to the message RAM */
  *TxAddress = TxElementW1;
  TxAddress++;
  *TxAddress = TxElementW2;
  TxAddress++;

  /* Write Tx payload to the message RAM */
  for (ByteCounter = 0; ByteCounter < DLCtoBytes[pTxHeader->DataLength]; ByteCounter += 4U)
  {
    *TxAddress = (((uint32_t)pTxData[ByteCounter + 3U] << 24U) |
                  ((uint32_t)pTxData[ByteCounter + 2U] << 16U) |
                  ((uint32_t)pTxData[ByteCounter + 1U] << 8U)  |
                  (uint32_t)pTxData[ByteCounter]);
    TxAddress++;
  }
}

/**
  * @}
  */
#endif /* HAL_FDCAN_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

#endif /* FDCAN1 */
