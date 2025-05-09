/**
  **********************************************************************************************************************
  * @file    stm32n6xx_hal_dma.c
  * @author  MCD Application Team
  * @brief   This file provides firmware functions to manage the following functionalities of the Direct Memory Access
  *          (DMA) peripheral:
  *            + Initialization/De-Initialization Functions
  *            + I/O Operation Functions
  *            + State and Errors Functions
  *            + DMA Attributes Functions
  *
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  @verbatim
  ======================================================================================================================
                       ##### How to use this driver #####
  ======================================================================================================================
  [..]
   (#) GPDMA and HPDMA are made available on this series.
       Transfer could be done thanks to AXI or AHB ports.

       HPDMA support transmission through:
         AXI port 0.
         AHB port 1.
       GPDMA allows transmission through:
         GPDMA_P = port-0.
         GPDMA_M = port-1

       The supported configurations for GPDMA and HPDMA can be found
       in the Connectivity matrix table of the reference manual.
       Mind that DTCM and ITCM can be only be accessed thanks to HPDMA
       AXI port 0.
    [..]
      DMA transfer modes are divided to 2 major categories :
          (+) Normal transfers (legacy)
          (+) Linked-list transfers

    [..]
      Normal transfers mode is initialized via the standard module and linked-list mode is configured via the extended
      module.

    [..]
      Additionally to linked-list capability, all advanced DMA features are managed and configured via the extended
      module as extensions to normal mode.
      Advanced features are :
          (+) Repeated block feature.
          (+) Trigger feature.
          (+) Data handling feature.

    [..]
      DMA Legacy circular transfer, is replaced by circular linked-list configuration.


    *** Initialization and De-Initialization ***
    ============================================
    [..]
      For a given channel, enable and configure the peripheral to be connected to the DMA Channel (except for internal
      SRAM/FLASH memories: no initialization is necessary) please refer to Reference manual for connection between
      peripherals and DMA requests.

    [..]
      For a given channel, use HAL_DMA_Init function to program the required configuration for normal transfer through
      the following parameters:

          (+) Request               : Specifies the DMA channel request
              Request parameters    :
              (++) can be a value of DMA_Request_Selection

          (+) BlkHWRequest          : Specifies the Block hardware request mode for DMA channel
              (++) can be a value of DMA_Block_Request

          (+) Direction             : Specifies the transfer direction for DMA channel
              (++) can be a value of DMA_Transfer_Direction

          (+) SrcInc                : Specifies the source increment mode for the DMA channel
              (++) can be a value of DMA_Source_Increment_Mode

          (+) DestInc               : Specifies the destination increment mode for the DMA channel
              (++) can be a value of DMA_Destination_Increment_Mode

          (+) SrcDataWidth          : Specifies the source data width for the DMA channel
              (++) can be a value of DMA_Source_Data_Width

          (+) DestDataWidth         : Specifies the destination data width for the DMA channel
              (++) can be a value of DMA_Destination_Data_Width

          (+) Priority              : Specifies the priority for the DMA channel
              (++) can be a value of DMA_Priority_Level

          (+) SrcBurstLength        : Specifies the source burst length (number of beats) for the DMA channel
              (++) can be a value of between 1 and 64

          (+) DestBurstLength       : Specifies the destination burst length (number of beats) for the DMA channel
              (++) can be a value of between 1 and 64

          (+) TransferAllocatedPort : Specifies the source and destination allocated ports
              (++) can be a value of DMA_Transfer_Allocated_Port

          (+) TransferEventMode     : Specifies the transfer event mode for the DMA channel
              (++) can be a value of DMA_Transfer_Event_Mode

          (+) Mode                  : Specifies the transfer mode for the DMA channel
              (++) can be one of the following modes :
                  (+++) DMA_NORMAL : Normal Mode
                  (+++) DMA_PFCTRL : Peripheral Flow Control (peripheral early termination) Mode

    *** Polling mode IO operation ***
    =================================
    [..]
          (+) Use HAL_DMA_Start() to start a DMA normal transfer after the configuration of source address, destination
              address and the size of data to be transferred.

          (+) Use HAL_DMA_PollForTransfer() to poll for selected transfer level. In this case a fixed Timeout can be
              configured by User depending on his application.
              Transfer level can be :
              (++) HAL_DMA_HALF_TRANSFER
              (++) HAL_DMA_FULL_TRANSFER
              For circular transfer, this API returns an HAL_ERROR with HAL_DMA_ERROR_NOT_SUPPORTED error code.

          (+) Use HAL_DMA_Abort() function to abort any ongoing DMA transfer in blocking mode.
              This API returns HAL_ERROR when there is no ongoing transfer or timeout is reached when disabling the DMA
              channel. (This API should not be called from an interrupt service routine)


    *** Interrupt mode IO operation ***
    ===================================
    [..]
          (+) Configure the DMA interrupt priority using HAL_NVIC_SetPriority()

          (+) Enable the DMA IRQ handler using HAL_NVIC_EnableIRQ()

          (+) Use HAL_DMA_RegisterCallback() function to register user callbacks from the following list :
              (++) XferCpltCallback     : transfer complete callback.
              (++) XferHalfCpltCallback : half transfer complete callback.
              (++) XferErrorCallback    : transfer error callback.
              (++) XferAbortCallback    : transfer abort complete callback.
              (++) XferSuspendCallback  : transfer suspend complete callback.

          (+) Use HAL_DMA_Start_IT() to start the DMA transfer after the enable of DMA interrupts and the configuration
              of source address,destination address and the size of data to be transferred.

          (+) Use HAL_DMA_IRQHandler() called under DMA_IRQHandler() interrupt subroutine to handle any DMA interrupt.

          (+) Use HAL_DMA_Abort_IT() function to abort any on-going DMA transfer in non-blocking mode.
              This API will suspend immediately the DMA channel execution. When the transfer is effectively suspended,
              an interrupt is generated and HAL_DMA_IRQHandler() will reset the channel and execute the callback
              XferAbortCallback. (This API could be called from an interrupt service routine)


    *** State and errors ***
    ========================
    [..]
          (+) Use HAL_DMA_GetState() function to get the DMA state.
          (+) Use HAL_DMA_GetError() function to get the DMA error code.


    *** Security and privilege attributes ***
    =========================================
    [..]
          (+) Use HAL_DMA_ConfigChannelAttributes() function to configure DMA channel security and privilege attributes.
              (++) Security  : at channel level, at source level and at destination level.
              (++) Privilege : at channel level.
          (+) Use HAL_DMA_GetConfigChannelAttributes() function to get the DMA channel attributes.
          (+) Use HAL_DMA_LockChannelAttributes() function to lock the DMA channel security and privilege attributes
              configuration. This API can be called once after each system boot.
              If called again, HAL_DMA_ConfigChannelAttributes() API has no effect.
              Unlock is done either by a system boot or a by an RCC reset.
          (+) Use HAL_DMA_GetLockChannelAttributes() function to get the attributes lock status.
          (+) Use HAL_DMA_SetIsolationAttributes() function to configure the HPDMA channel isolation attribute.
          (+) Use HAL_DMA_GetIsolationAttributes() function to get the HPDMA channel isolation attribute.

    *** Isolation attributes ***
    ==================================
    It is recommended to always enable the isolation feature of the DMA (CFEN = 1) of the HPDMA channel.
    The default isolation shall be 1. i.e. the Cortex CID.
    Indeed, should the isolation bit CFEN be equal to 0, a default isolation CID is still carried out and is 0 (zero).
    the CID generated by a channel access is zero-ed when CFEN=0, even if SCID is different from 000.

    Similarly, it is recommended that all RIMU related IP (SDMMC, OTG, ETH, GPU, DMA2D, ...) to always enable and
    generate (CID = 1).

    *** DMA HAL driver macros list ***
    ==================================
    [..]
      Below the list of most used macros in DMA HAL driver.

          (+) __HAL_DMA_ENABLE        : Enable the specified DMA Channel.
          (+) __HAL_DMA_DISABLE       : Disable the specified DMA Channel.
          (+) __HAL_DMA_GET_FLAG      : Get the DMA Channel pending flags.
          (+) __HAL_DMA_CLEAR_FLAG    : Clear the DMA Channel pending flags.
          (+) __HAL_DMA_ENABLE_IT     : Enable the specified DMA Channel interrupts.
          (+) __HAL_DMA_DISABLE_IT    : Disable the specified DMA Channel interrupts.
          (+) __HAL_DMA_GET_IT_SOURCE : Check whether the specified DMA Channel interrupt has occurred or not.

    *** Valid burst length ***
    ==================================
    [..]
      HPDMA allowed AXI maximum burst length
    (+) The maximum allowed AXI burst length shall not exceed 16.
            (++) If either selected source or destination length is above 16, HAL_DMA_Init will return HAL_ERROR.

    [..]
     (@) You can refer to the header file of the DMA HAL driver for more useful macros.

    @endverbatim
  **********************************************************************************************************************
  */

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup DMA DMA
  * @brief DMA HAL module driver
  * @{
  */

#ifdef HAL_DMA_MODULE_ENABLED

/* Private typedef ---------------------------------------------------------------------------------------------------*/
/* Private constants -------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
/* Private function prototypes ---------------------------------------------------------------------------------------*/
static void DMA_SetConfig(DMA_HandleTypeDef const *const hdma,
                          uint32_t SrcAddress,
                          uint32_t DstAddress,
                          uint32_t SrcDataSize);
static void DMA_Init(DMA_HandleTypeDef const *const hdma);

/* Exported functions ------------------------------------------------------------------------------------------------*/

/** @addtogroup DMA_Exported_Functions DMA Exported Functions
  * @{
  */

/** @addtogroup DMA_Exported_Functions_Group1
  *
@verbatim
  ======================================================================================================================
                       ##### Initialization and de-initialization functions #####
  ======================================================================================================================
    [..]
      This section provides functions allowing to initialize and de-initialize the DMA channel in normal mode.

    [..]
      (+) The HAL_DMA_Init() function follows the DMA channel configuration procedures as described in reference manual.
      (+) The HAL_DMA_DeInit() function allows to de-initialize the DMA channel.

@endverbatim
  * @{
  */

/**
  * @brief  Initialize the DMA channel in normal mode according to the specified parameters in the DMA_InitTypeDef and
  *         create the associated handle.
  * @note   Warning: the maximum allowed AXI burst length shall not exceed 16.
  *         Otherwise, an error will be returned and no initialization performed.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *const hdma)
{
  /* Get tick number */
  uint32_t tickstart = HAL_GetTick();

  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));
  assert_param(IS_DMA_DIRECTION(hdma->Init.Direction));
  if (hdma->Init.Direction != DMA_MEMORY_TO_MEMORY)
  {
    assert_param(IS_DMA_REQUEST(hdma->Init.Request));
  }
  assert_param(IS_DMA_BLOCK_HW_REQUEST(hdma->Init.BlkHWRequest));
  assert_param(IS_DMA_SOURCE_INC(hdma->Init.SrcInc));
  assert_param(IS_DMA_DESTINATION_INC(hdma->Init.DestInc));
  assert_param(IS_DMA_SOURCE_DATA_WIDTH(hdma->Init.SrcDataWidth));
  assert_param(IS_DMA_DESTINATION_DATA_WIDTH(hdma->Init.DestDataWidth));
  assert_param(IS_DMA_PRIORITY(hdma->Init.Priority));
  assert_param(IS_DMA_TCEM_EVENT_MODE(hdma->Init.TransferEventMode));
  assert_param(IS_DMA_MODE(hdma->Init.Mode));
  if (hdma->Init.Mode == DMA_PFCTRL)
  {
    assert_param(IS_DMA_PFREQ_INSTANCE(hdma->Instance));
  }
  /* Check DMA channel instance */
  if ((IS_HPDMA_INSTANCE(hdma->Instance) != 0U) || (IS_GPDMA_INSTANCE(hdma->Instance) != 0U))
  {
    assert_param(IS_DMA_BURST_LENGTH(hdma->Init.SrcBurstLength));
    assert_param(IS_DMA_BURST_LENGTH(hdma->Init.DestBurstLength));
    assert_param(IS_DMA_TRANSFER_ALLOCATED_PORT(hdma->Init.TransferAllocatedPort));
  }

  /* Check if the burst length may face DMA AXI limitation */
  if (IS_HPDMA_INSTANCE(hdma->Instance) != 0U)
  {
    if (((hdma->Init.TransferAllocatedPort & DMA_CTR1_SAP) == DMA_SRC_ALLOCATED_PORT0) &&
        (hdma->Init.SrcBurstLength > 16U))
    {
      return HAL_ERROR;
    }
    if (((hdma->Init.TransferAllocatedPort & DMA_CTR1_DAP) == DMA_DEST_ALLOCATED_PORT0) &&
        (hdma->Init.DestBurstLength > 16U))
    {
      return HAL_ERROR;
    }
  }

  /* Allocate lock resource */
  __HAL_UNLOCK(hdma);

  /* Initialize the callbacks */
  if (hdma->State == HAL_DMA_STATE_RESET)
  {
    /* Clean all callbacks */
    hdma->XferCpltCallback     = NULL;
    hdma->XferHalfCpltCallback = NULL;
    hdma->XferErrorCallback    = NULL;
    hdma->XferAbortCallback    = NULL;
    hdma->XferSuspendCallback  = NULL;
  }

  /* Update the DMA channel state */
  hdma->State = HAL_DMA_STATE_BUSY;

  /* Disable the DMA channel */
  __HAL_DMA_DISABLE(hdma);

  /* Check if the DMA channel is effectively disabled */
  while ((hdma->Instance->CCR & DMA_CCR_EN) != 0U)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
    {
      /* Update the DMA channel error code */
      hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

      /* Update the DMA channel state */
      hdma->State = HAL_DMA_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Initialize the DMA channel registers */
  DMA_Init(hdma);

  /* Update DMA channel operation mode */
  hdma->Mode = hdma->Init.Mode;

  /* Update the DMA channel error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Update the DMA channel state */
  hdma->State = HAL_DMA_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  DeInitialize the DMA channel when it is configured in normal mode.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *const hdma)
{
  uint32_t tickstart = HAL_GetTick();

  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));
  /* Disable the selected DMA Channel */
  __HAL_DMA_DISABLE(hdma);

  /* Check if the DMA channel is effectively disabled */
  while ((hdma->Instance->CCR & DMA_CCR_EN) != 0U)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
    {
      /* Update the DMA channel error code */
      hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

      /* Update the DMA channel state */
      hdma->State = HAL_DMA_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Reset DMA Channel registers */
  hdma->Instance->CLBAR = 0U;
  hdma->Instance->CCR   = 0U;
  /* As secure, privilege and CID configuration, bit field DSEC and SSEC are managed in   */
  /* HAL_DMA_ConfigChannelAttributes function, so it mustn't be cleaned in HAL_DMA_DeInit */
  hdma->Instance->CTR1  = hdma->Instance->CTR1 & (0U | DMA_CTR1_DSEC | DMA_CTR1_SSEC);
  hdma->Instance->CTR2  = 0U;
  hdma->Instance->CBR1  = 0U;
  hdma->Instance->CSAR  = 0U;
  hdma->Instance->CDAR  = 0U;
  hdma->Instance->CLLR  = 0U;

  /* Reset 2D Addressing registers */
  if (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U)
  {
    hdma->Instance->CTR3 = 0U;
    hdma->Instance->CBR2 = 0U;
  }

  /* Clear all flags */
  __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT | DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE | DMA_FLAG_SUSP |
                              DMA_FLAG_TO));

  /* Clean all callbacks */
  hdma->XferCpltCallback     = NULL;
  hdma->XferHalfCpltCallback = NULL;
  hdma->XferErrorCallback    = NULL;
  hdma->XferAbortCallback    = NULL;
  hdma->XferSuspendCallback  = NULL;

  /* Clean DMA queue */
  hdma->LinkedListQueue = NULL;

  /* Clean DMA parent */
  if (hdma->Parent != NULL)
  {
    hdma->Parent = NULL;
  }

  /* Update DMA channel operation mode */
  hdma->Mode = DMA_NORMAL;

  /* Update the DMA channel error code */
  hdma->ErrorCode = HAL_DMA_ERROR_NONE;

  /* Update the DMA channel state */
  hdma->State = HAL_DMA_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hdma);

  return HAL_OK;
}
/**
  * @}
  */

/** @addtogroup DMA_Exported_Functions_Group2
  *
@verbatim
  ======================================================================================================================
                                ##### IO operation functions #####
  ======================================================================================================================
    [..]
      This section provides functions allowing to :
      (+) Configure the source, destination address and data size and Start DMA transfer in normal mode
      (+) Abort DMA transfer
      (+) Poll for transfer complete
      (+) Handle DMA interrupt request
      (+) Register and Unregister DMA callbacks

    [..]
      (+) The HAL_DMA_Start() function allows to start the DMA channel transfer in normal mode (Blocking mode).
      (+) The HAL_DMA_Start_IT() function allows to start the DMA channel transfer in normal mode (Non-blocking mode).
      (+) The HAL_DMA_Abort() function allows to abort any on-going transfer (Blocking mode).
      (+) The HAL_DMA_Abort_IT() function allows to abort any on-going transfer (Non-blocking mode).
      (+) The HAL_DMA_PollForTransfer() function allows to poll on half transfer and transfer complete (Blocking mode).
          This API cannot be used for circular transfers.
      (+) The HAL_DMA_IRQHandler() function allows to handle any DMA channel interrupt (Non-blocking mode).
      (+) The HAL_DMA_RegisterCallback() and HAL_DMA_UnRegisterCallback() functions allow respectively to register and
          unregister user customized callbacks.
          User callbacks are called under HAL_DMA_IRQHandler().

@endverbatim
  * @{
  */

/**
  * @brief  Start the DMA channel transfer in normal mode (Blocking mode).
  * @param  hdma        : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for
  *                       the specified DMA Channel.
  * @param  SrcAddress  : The source data address.
  * @param  DstAddress  : The destination data address.
  * @param  SrcDataSize : The length of data to be transferred from source to destination in bytes.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *const hdma,
                                uint32_t SrcAddress,
                                uint32_t DstAddress,
                                uint32_t SrcDataSize)
{
  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_BLOCK_SIZE(SrcDataSize));

  /* Process locked */
  __HAL_LOCK(hdma);

  /* Check DMA channel state */
  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_BUSY;

    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

    /* Configure the source address, destination address, the data size and clear flags */
    DMA_SetConfig(hdma, SrcAddress, DstAddress, SrcDataSize);

    /* Enable DMA channel */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Starts the DMA channel transfer in normal mode with interrupts enabled (Non-blocking mode).
  * @param  hdma         : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                        specified DMA Channel.
  * @param  SrcAddress   : The source data address.
  * @param  DstAddress   : The destination data address.
  * @param  SrcDataSize  : The length of data to be transferred from source to destination in bytes.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *const hdma,
                                   uint32_t SrcAddress,
                                   uint32_t DstAddress,
                                   uint32_t SrcDataSize)
{
  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_BLOCK_SIZE(SrcDataSize));

  /* Process locked */
  __HAL_LOCK(hdma);

  /* Check DMA channel state */
  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_BUSY;

    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NONE;

    /* Configure the source address, destination address, the data size and clear flags */
    DMA_SetConfig(hdma, SrcAddress, DstAddress, SrcDataSize);

    /* Enable common interrupts: Transfer Complete and Transfer Errors ITs */
    __HAL_DMA_ENABLE_IT(hdma, (DMA_IT_TC | DMA_IT_DTE | DMA_IT_ULE | DMA_IT_USE | DMA_IT_TO));

    /* Check half transfer complete callback */
    if (hdma->XferHalfCpltCallback != NULL)
    {
      /* If Half Transfer complete callback is set, enable the corresponding IT */
      __HAL_DMA_ENABLE_IT(hdma, DMA_IT_HT);
    }

    /* Check Half suspend callback */
    if (hdma->XferSuspendCallback != NULL)
    {
      /* If Transfer suspend callback is set, enable the corresponding IT */
      __HAL_DMA_ENABLE_IT(hdma, DMA_IT_SUSP);
    }

    /* Enable DMA channel */
    __HAL_DMA_ENABLE(hdma);
  }
  else
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_BUSY;

    /* Process unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Abort any on-going DMA channel transfer (Blocking mode).
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @note   After suspending a DMA channel, a wait until the DMA channel is effectively stopped is added. If a channel
  *         is suspended while a data transfer is on-going, the current data will be transferred and the channel will be
  *         effectively suspended only after the transfer of any on-going data is finished.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *const hdma)
{
  /* Get tick number */
  uint32_t tickstart =  HAL_GetTick();

  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check DMA channel state */
  if (hdma->State != HAL_DMA_STATE_BUSY)
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }
  else
  {
    /* Suspend the channel */
    hdma->Instance->CCR |= DMA_CCR_SUSP;

    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_SUSPEND;

    /* Check if the DMA Channel is suspended */
    while ((hdma->Instance->CSR & DMA_CSR_SUSPF) == 0U)
    {
      /* Check for the Timeout */
      if ((HAL_GetTick() - tickstart) > HAL_TIMEOUT_DMA_ABORT)
      {
        /* Update the DMA channel error code */
        hdma->ErrorCode |= HAL_DMA_ERROR_TIMEOUT;

        /* Update the DMA channel state */
        hdma->State = HAL_DMA_STATE_ERROR;

        /* Check DMA channel transfer mode */
        if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
        {
          /* Update the linked-list queue state */
          hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
        }

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        return HAL_ERROR;
      }
    }

    /* Reset the channel */
    hdma->Instance->CCR |= DMA_CCR_RESET;

    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_ABORT;

    /* Clear all status flags */
    __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT | DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE | DMA_FLAG_SUSP |
                                DMA_FLAG_TO));

    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Check DMA channel transfer mode */
    if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      /* Update the linked-list queue state */
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;

      /* Clear remaining data size to ensure loading linked-list from memory next start */
      hdma->Instance->CBR1 = 0U;
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);
  }

  return HAL_OK;
}

/**
  * @brief  Abort any on-going DMA channel transfer in interrupt mode (Non-blocking mode).
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *const hdma)
{
  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check DMA channel state */
  if (hdma->State != HAL_DMA_STATE_BUSY)
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    return HAL_ERROR;
  }
  else
  {
    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_ABORT;

    /* Suspend the channel and activate suspend interrupt */
    hdma->Instance->CCR |= (DMA_CCR_SUSP | DMA_CCR_SUSPIE);
  }

  return HAL_OK;
}

/**
  * @brief  Polling for transfer status (Blocking mode).
  * @param  hdma          : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                         specified DMA Channel.
  * @param  CompleteLevel : Specifies the DMA level complete.
  * @param  Timeout       : Timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *const hdma,
                                          HAL_DMA_LevelCompleteTypeDef CompleteLevel,
                                          uint32_t Timeout)
{
  /* Get tick number */
  uint32_t tickstart = HAL_GetTick();
  uint32_t level_flag;
  uint32_t tmp_csr;

  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_LEVEL_COMPLETE(CompleteLevel));

  /* Check DMA channel state */
  if (hdma->State != HAL_DMA_STATE_BUSY)
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  /* Polling mode is not supported in circular mode */
  if ((hdma->Mode & DMA_LINKEDLIST_CIRCULAR) == DMA_LINKEDLIST_CIRCULAR)
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;

    return HAL_ERROR;
  }

  /* Get the level transfer complete flag */
  level_flag = ((CompleteLevel == HAL_DMA_FULL_TRANSFER) ? DMA_FLAG_IDLE : DMA_FLAG_HT);

  /* Get DMA channel status */
  tmp_csr = hdma->Instance->CSR;

  while ((tmp_csr & level_flag) == 0U)
  {
    /* Check for the timeout */
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
      {
        /* Update the DMA channel error code */
        hdma->ErrorCode |= HAL_DMA_ERROR_TIMEOUT;

        /*
          If timeout, abort the current transfer.
          Note that the Abort function will
          - Clear all transfer flags.
          - Unlock.
          - Set the State.
        */
        (void)HAL_DMA_Abort(hdma);

        return HAL_ERROR;
      }
    }

    /* Get a newer CSR register value */
    tmp_csr = hdma->Instance->CSR;
  }

  /* Check trigger overrun flag */
  if ((tmp_csr & DMA_FLAG_TO) != 0U)
  {
    /* Update the DMA channel error code */
    hdma->ErrorCode |= HAL_DMA_ERROR_TO;

    /* Clear the error flag */
    __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TO);
  }

  /* Check error flags */
  if ((tmp_csr & (DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE)) != 0U)
  {
    /* Check the data transfer error flag */
    if ((tmp_csr & DMA_FLAG_DTE) != 0U)
    {
      /* Update the DMA channel error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_DTE;

      /* Clear the error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_DTE);
    }

    /* Check the update link error flag */
    if ((tmp_csr & DMA_FLAG_ULE) != 0U)
    {
      /* Update the DMA channel error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_ULE;

      /* Clear the error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_ULE);
    }

    /* Check the user setting error flag */
    if ((tmp_csr & DMA_FLAG_USE) != 0U)
    {
      /* Update the DMA channel error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_USE;

      /* Clear the error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_USE);
    }

    /* Reset the channel */
    hdma->Instance->CCR |= DMA_CCR_RESET;

    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Check DMA channel transfer mode */
    if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      /* Update the linked-list queue state */
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    return HAL_ERROR;
  }

  /* Clear the transfer level flag */
  if (CompleteLevel == HAL_DMA_HALF_TRANSFER)
  {
    /* Clear the Half Transfer flag */
    __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_HT);
  }
  else if (CompleteLevel == HAL_DMA_FULL_TRANSFER)
  {
    /* Clear the transfer flags */
    __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT));

    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Check DMA channel transfer mode */
    if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      /* Update the linked-list queue state */
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
    }

    /* Process unlocked */
    __HAL_UNLOCK(hdma);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Handle DMA interrupt request (Non-blocking mode).
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval None.
  */
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *const hdma)
{
  const DMA_TypeDef *p_dma_instance = GET_DMA_INSTANCE(hdma);
  uint32_t global_it_flag =  1UL << (GET_DMA_CHANNEL(hdma) & 0x1FU);
  uint32_t global_active_flag_ns = IS_DMA_GLOBAL_ACTIVE_FLAG_NS(p_dma_instance, global_it_flag);
#if defined (CPU_IN_SECURE_STATE)
  uint32_t global_active_flag_s = IS_DMA_GLOBAL_ACTIVE_FLAG_S(p_dma_instance, global_it_flag);
#endif /* CPU_IN_SECURE_STATE */

  /* Global Interrupt Flag management *********************************************************************************/
#if defined (CPU_IN_SECURE_STATE)
  if ((global_active_flag_s == 0U) && (global_active_flag_ns == 0U))
#else
  if (global_active_flag_ns == 0U)
#endif /* CPU_IN_SECURE_STATE */
  {
    return; /* the global interrupt flag for the current channel is down , nothing to do */
  }

  /* Data Transfer Error Interrupt management *************************************************************************/
  if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_DTE) != 0U)
  {
    /* Check if interrupt source is enabled */
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_DTE) != 0U)
    {
      /* Clear the transfer error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_DTE);

      /* Update the DMA channel error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_DTE;
    }
  }

  /* Update Linked-list Error Interrupt management ********************************************************************/
  if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_ULE) != 0U)
  {
    /* Check if interrupt source is enabled */
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_ULE) != 0U)
    {
      /* Clear the update linked-list error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_ULE);

      /* Update the DMA channel error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_ULE;
    }
  }

  /* User Setting Error Interrupt management **************************************************************************/
  if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_USE) != 0U)
  {
    /* Check if interrupt source is enabled */
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_USE) != 0U)
    {
      /* Clear the user setting error flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_USE);

      /* Update the DMA channel error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_USE;
    }
  }

  /* Trigger Overrun Interrupt management *****************************************************************************/
  if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_TO) != 0U)
  {
    /* Check if interrupt source is enabled */
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TO) != 0U)
    {
      /* Clear the trigger overrun flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TO);

      /* Update the DMA channel error code */
      hdma->ErrorCode |= HAL_DMA_ERROR_TO;
    }
  }

  /* Half Transfer Complete Interrupt management **********************************************************************/
  if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_HT) != 0U)
  {
    /* Check if interrupt source is enabled */
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_HT) != 0U)
    {
      /* Clear the half transfer flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_HT);

      /* Check half transfer complete callback */
      if (hdma->XferHalfCpltCallback != NULL)
      {
        /* Half transfer callback */
        hdma->XferHalfCpltCallback(hdma);
      }
    }
  }

  /* Suspend Transfer Interrupt management ****************************************************************************/
  if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_SUSP) != 0U)
  {
    /* Check if interrupt source is enabled */
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_SUSP) != 0U)
    {
      /* Clear the block transfer complete flag */
      __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_SUSP);

      /* Check DMA channel state */
      if (hdma->State == HAL_DMA_STATE_ABORT)
      {
        /* Disable the suspend transfer interrupt */
        __HAL_DMA_DISABLE_IT(hdma, DMA_IT_SUSP);

        /* Reset the channel internal state and reset the FIFO */
        hdma->Instance->CCR |= DMA_CCR_RESET;

        /* Update the DMA channel state */
        hdma->State = HAL_DMA_STATE_READY;

        /* Check DMA channel transfer mode */
        if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
        {
          /* Update the linked-list queue state */
          hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;

          /* Clear remaining data size to ensure loading linked-list from memory next start */
          hdma->Instance->CBR1 = 0U;
        }

        /* Process Unlocked */
        __HAL_UNLOCK(hdma);

        /* Check transfer abort callback */
        if (hdma->XferAbortCallback != NULL)
        {
          /* Transfer abort callback */
          hdma->XferAbortCallback(hdma);
        }

        return;
      }
      else
      {
        /* Update the DMA channel state */
        hdma->State = HAL_DMA_STATE_SUSPEND;

        /* Check transfer suspend callback */
        if (hdma->XferSuspendCallback != NULL)
        {
          /* Transfer suspend callback */
          hdma->XferSuspendCallback(hdma);
        }
      }
    }
  }

  /* Transfer Complete Interrupt management ***************************************************************************/
  if (__HAL_DMA_GET_FLAG(hdma, DMA_FLAG_TC) != 0U)
  {
    /* Check if interrupt source is enabled */
    if (__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TC) != 0U)
    {
      /* Check DMA channel transfer mode */
      if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
      {
        /* If linked-list transfer */
        if (hdma->Instance->CLLR == 0U)
        {
          if (hdma->Instance->CBR1 == 0U)
          {
            /* Update the DMA channel state */
            hdma->State = HAL_DMA_STATE_READY;

            /* Update the linked-list queue state */
            hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
          }
        }
      }
      else
      {
        /* If normal transfer */
        if (hdma->Instance->CBR1 == 0U)
        {
          /* Update the DMA channel state */
          hdma->State = HAL_DMA_STATE_READY;
        }
      }

      /* Clear TC and HT transfer flags */
      __HAL_DMA_CLEAR_FLAG(hdma, (DMA_FLAG_TC | DMA_FLAG_HT));

      /* Process Unlocked */
      __HAL_UNLOCK(hdma);

      /* Check transfer complete callback */
      if (hdma->XferCpltCallback != NULL)
      {
        /* Channel Transfer Complete callback */
        hdma->XferCpltCallback(hdma);
      }
    }
  }

  /* Manage error case ************************************************************************************************/
  if (hdma->ErrorCode != HAL_DMA_ERROR_NONE)
  {
    /* Reset the channel internal state and reset the FIFO */
    hdma->Instance->CCR |= DMA_CCR_RESET;

    /* Update the DMA channel state */
    hdma->State = HAL_DMA_STATE_READY;

    /* Check DMA channel transfer mode */
    if ((hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      /* Update the linked-list queue state */
      hdma->LinkedListQueue->State = HAL_DMA_QUEUE_STATE_READY;
    }

    /* Process Unlocked */
    __HAL_UNLOCK(hdma);

    /* Check transfer error callback */
    if (hdma->XferErrorCallback != NULL)
    {
      /* Transfer error callback */
      hdma->XferErrorCallback(hdma);
    }
  }
}

/**
  * @brief  Register callback according to specified ID.
  * @note   The HAL_DMA_RegisterCallback() may be called before HAL_DMA_Init() in HAL_DMA_STATE_RESET
  *         to register callbacks for HAL_DMA_MSPINIT_CB_ID and HAL_DMA_MSPDEINIT_CB_ID.
  * @param  hdma       : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                      specified DMA Channel.
  * @param  CallbackID : User Callback identifier which could be a value of HAL_DMA_CallbackIDTypeDef enumeration.
  * @param  pCallback  : Pointer to private callback function.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *const hdma,
                                           HAL_DMA_CallbackIDTypeDef CallbackID,
                                           void (*const pCallback)(DMA_HandleTypeDef *const _hdma))
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check DMA channel state */
  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Check callback ID */
    switch (CallbackID)
    {
      case HAL_DMA_XFER_CPLT_CB_ID:
      {
        /* Register transfer complete callback */
        hdma->XferCpltCallback = pCallback;
        break;
      }

      case HAL_DMA_XFER_HALFCPLT_CB_ID:
      {
        /* Register half transfer callback */
        hdma->XferHalfCpltCallback = pCallback;
        break;
      }

      case HAL_DMA_XFER_ERROR_CB_ID:
      {
        /* Register transfer error callback */
        hdma->XferErrorCallback = pCallback;
        break;
      }

      case HAL_DMA_XFER_ABORT_CB_ID:
      {
        /* Register abort callback */
        hdma->XferAbortCallback = pCallback;
        break;
      }

      case HAL_DMA_XFER_SUSPEND_CB_ID:
      {
        /* Register suspend callback */
        hdma->XferSuspendCallback = pCallback;
        break;
      }

      default:
      {
        /* Update error status */
        status = HAL_ERROR;
        break;
      }
    }
  }
  else
  {
    /* Update error status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister callback according to specified ID.
  * @note   The HAL_DMA_UnRegisterCallback() may be called before HAL_DMA_Init() in HAL_DMA_STATE_RESET
  *         to un-register callbacks for HAL_DMA_MSPINIT_CB_ID and HAL_DMA_MSPDEINIT_CB_ID.
  * @param  hdma       : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                      specified DMA Channel.
  * @param  CallbackID : User Callback identifier which could be a value of HAL_DMA_CallbackIDTypeDef enum.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *const hdma,
                                             HAL_DMA_CallbackIDTypeDef CallbackID)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check DMA channel state */
  if (hdma->State == HAL_DMA_STATE_READY)
  {
    /* Check callback ID */
    switch (CallbackID)
    {
      case HAL_DMA_XFER_CPLT_CB_ID:
      {
        /* UnRegister transfer complete callback */
        hdma->XferCpltCallback = NULL;
        break;
      }

      case HAL_DMA_XFER_HALFCPLT_CB_ID:
      {
        /* UnRegister half transfer callback */
        hdma->XferHalfCpltCallback = NULL;
        break;
      }

      case HAL_DMA_XFER_ERROR_CB_ID:
      {
        /* UnRegister transfer error callback */
        hdma->XferErrorCallback = NULL;
        break;
      }

      case HAL_DMA_XFER_ABORT_CB_ID:
      {
        /* UnRegister abort callback */
        hdma->XferAbortCallback = NULL;
        break;
      }

      case HAL_DMA_XFER_SUSPEND_CB_ID:
      {
        /* UnRegister suspend callback */
        hdma->XferSuspendCallback = NULL;
        break;
      }

      case HAL_DMA_XFER_ALL_CB_ID:
      {
        /* UnRegister all available callbacks */
        hdma->XferCpltCallback     = NULL;
        hdma->XferHalfCpltCallback = NULL;
        hdma->XferErrorCallback    = NULL;
        hdma->XferAbortCallback    = NULL;
        hdma->XferSuspendCallback  = NULL;
        break;
      }

      default:
      {
        /* Update error status */
        status = HAL_ERROR;
        break;
      }
    }
  }
  else
  {
    /* Update error status */
    status = HAL_ERROR;
  }

  return status;
}
/**
  * @}
  */

/** @addtogroup DMA_Exported_Functions_Group3
  *
@verbatim
  ======================================================================================================================
                              ##### State and Errors functions #####
  ======================================================================================================================
    [..]
      This section provides functions allowing to :
      (+) Check the DMA state
      (+) Get error code

    [..]
      (+) The HAL_DMA_GetState() function allows to get the DMA channel state.
      (+) The HAL_DMA_DeInit() function allows to get the DMA channel error code.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the DMA channel state.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval DMA state.
  */
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef const *const hdma)
{
  /* Return the DMA channel state */
  return hdma->State;
}

/**
  * @brief  Return the DMA channel error code.
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval DMA Error Code.
  */
uint32_t HAL_DMA_GetError(DMA_HandleTypeDef const *const hdma)
{
  /* Return the DMA channel error code */
  return hdma->ErrorCode;
}
/**
  * @}
  */

/** @addtogroup DMA_Exported_Functions_Group4
  *
@verbatim
  ======================================================================================================================
                           ##### DMA Attributes functions #####
  ======================================================================================================================
    [..]
      This section provides functions allowing to :
      (+) Configure DMA channel secure and privilege attributes.
      (+) Get DMA channel secure and privilege attributes.
      (+) Lock DMA channel secure and privilege attributes configuration.
      (+) Check whether DMA channel secure and privilege attributes configuration is locked or not.

    [..]
      (+) The HAL_DMA_ConfigChannelAttributes() function allows to configure DMA channel security and privilege
          attributes.
      (+) The HAL_DMA_GetConfigChannelAttributes() function allows to get DMA channel security and privilege attributes
          configuration.
      (+) The HAL_DMA_LockChannelAttributes() function allows to lock the DMA channel security and privilege attributes.
      (+) The HAL_DMA_GetLockChannelAttributes() function allows to get the DMA channel security and privilege
          attributes lock status.

@endverbatim
  * @{
  */

/**
  * @brief  Configure the DMA channel security and privilege attribute(s).
  * @note   These attributes cannot be modified when the corresponding lock state is enabled.
  * @param  hdma              : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for
  *                             the specified DMA Channel.
  * @param  ChannelAttributes : Specifies the DMA channel secure/privilege attributes.
  *                             This parameter can be a one or a combination of @ref DMA_Channel_Attributes.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_DMA_ConfigChannelAttributes(DMA_HandleTypeDef *const hdma, uint32_t ChannelAttributes)
{
  DMA_TypeDef *p_dma_instance;
  uint32_t channel_idx;

  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_DMA_ATTRIBUTES(ChannelAttributes));

  /* Get DMA instance */
  p_dma_instance = GET_DMA_INSTANCE(hdma);

  /* Get channel index */
  channel_idx = 1UL << (GET_DMA_CHANNEL(hdma) & 0x1FU);

  /* Check DMA channel privilege attribute management */
  if ((ChannelAttributes & DMA_CHANNEL_ATTR_PRIV_MASK) == DMA_CHANNEL_ATTR_PRIV_MASK)
  {
    /* Configure DMA channel privilege attribute */
    if ((ChannelAttributes & DMA_CHANNEL_PRIV) == DMA_CHANNEL_PRIV)
    {
      p_dma_instance->PRIVCFGR |= channel_idx;
    }
    else
    {
      p_dma_instance->PRIVCFGR &= (~channel_idx);
    }
  }

#if defined (CPU_IN_SECURE_STATE)
  /* Check DMA channel security attribute management */
  if ((ChannelAttributes & DMA_CHANNEL_ATTR_SEC_MASK) == DMA_CHANNEL_ATTR_SEC_MASK)
  {
    /* Configure DMA channel security attribute */
    if ((ChannelAttributes & DMA_CHANNEL_SEC) == DMA_CHANNEL_SEC)
    {
      p_dma_instance->SECCFGR |= channel_idx;
    }
    else
    {
      p_dma_instance->SECCFGR &= (~channel_idx);
    }
  }

  /* Channel source security attribute management */
  if ((ChannelAttributes & DMA_CHANNEL_ATTR_SEC_SRC_MASK) == DMA_CHANNEL_ATTR_SEC_SRC_MASK)
  {
    /* Configure DMA channel source security attribute */
    if ((ChannelAttributes & DMA_CHANNEL_SRC_SEC) == DMA_CHANNEL_SRC_SEC)
    {
      hdma->Instance->CTR1 |= DMA_CTR1_SSEC;
    }
    else
    {
      hdma->Instance->CTR1 &= (~DMA_CTR1_SSEC);
    }
  }

  /* Channel destination security attribute management */
  if ((ChannelAttributes & DMA_CHANNEL_ATTR_SEC_DEST_MASK) == DMA_CHANNEL_ATTR_SEC_DEST_MASK)
  {
    /* Configure DMA channel destination security attribute */
    if ((ChannelAttributes & DMA_CHANNEL_DEST_SEC) == DMA_CHANNEL_DEST_SEC)
    {
      hdma->Instance->CTR1 |= DMA_CTR1_DSEC;
    }
    else
    {
      hdma->Instance->CTR1 &= (~DMA_CTR1_DSEC);
    }
  }
#endif /* CPU_IN_SECURE_STATE */

  return HAL_OK;
}

/**
  * @brief  Get the DMA channel security and privilege attributes.
  * @param  hdma               : Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                              for the specified DMA Channel.
  * @param  pChannelAttributes : Pointer to the returned attributes.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_DMA_GetConfigChannelAttributes(DMA_HandleTypeDef const *const hdma,
                                                     uint32_t *const pChannelAttributes)
{
  const DMA_TypeDef *p_dma_instance;
  uint32_t attributes;
  uint32_t channel_idx;

  /* Check the DMA peripheral handle and channel attributes parameters */
  if ((hdma == NULL) || (pChannelAttributes == NULL))
  {
    return HAL_ERROR;
  }

  /* Get DMA instance */
  p_dma_instance = GET_DMA_INSTANCE(hdma);

  /* Get channel index */
  channel_idx = 1UL << (GET_DMA_CHANNEL(hdma) & 0x1FU);

  /* Get DMA channel privilege attribute */
  attributes = ((p_dma_instance->PRIVCFGR & channel_idx) == 0U) ? DMA_CHANNEL_NPRIV : DMA_CHANNEL_PRIV;

  /* Get DMA channel security attribute */
  attributes |= ((p_dma_instance->SECCFGR & channel_idx) == 0U) ? DMA_CHANNEL_NSEC : DMA_CHANNEL_SEC;

  /* Get DMA channel source security attribute */
  attributes |= ((hdma->Instance->CTR1 & DMA_CTR1_SSEC) == 0U) ? DMA_CHANNEL_SRC_NSEC : DMA_CHANNEL_SRC_SEC;

  /* Get DMA channel destination security attribute */
  attributes |= ((hdma->Instance->CTR1 & DMA_CTR1_DSEC) == 0U) ? DMA_CHANNEL_DEST_NSEC : DMA_CHANNEL_DEST_SEC;

  /* return value */
  *pChannelAttributes = attributes;

  return HAL_OK;
}

#if defined (CPU_IN_SECURE_STATE)
/**
  * @brief  Set the DMA channel filtering CID (Isolation configuration). It can be
  *             - static: the CID passed as parameter is programmed;
  *             - disabled: the whole register is cleared.
  * @param  hdma    : Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                of the HPDMA channel.
  * @param  pConfig : Pointer on the DMA Isolation structure
  * @retval None
  */
HAL_StatusTypeDef HAL_DMA_SetIsolationAttributes(DMA_HandleTypeDef *const hdma,
                                                 DMA_IsolationConfigTypeDef const *const pConfig)
{
  /* Check the DMA peripheral handle parameter */
  if ((hdma == NULL) || (pConfig == NULL))
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_HPDMA_INSTANCE(hdma->Instance));
  assert_param(IS_DMA_ISOLATION_MODE(pConfig->CidFiltering));
  assert_param(IS_DMA_ISOLATION_STATIC_CID(pConfig->StaticCid));

  /* static CID field value used when filtering is enable */
  if ((pConfig->CidFiltering) == DMA_ISOLATION_ON)
  {
    /* Write static CID configuration */
    hdma->Instance->CCIDCFGR = ((pConfig->StaticCid & DMA_CCIDCFGR_SCID_Msk) | DMA_CCIDCFGR_CFEN);
  }
  else
  {
    /* CID configuration is off */
    hdma->Instance->CCIDCFGR = 0U;
  }

  return HAL_OK;
}
#endif /* CPU_IN_SECURE_STATE */
/**
  * @brief  Read the DMA channel filtering CID (Isolation configuration). It can be
  *             - static: the CID passed as parameter is programmed;
  *             - disabled: the whole register is cleared.
  * @param  hdma    : Pointer to a DMA_HandleTypeDef structure that contains the configuration information
  *                of the HPDMA channel.
  * @param  pConfig : Pointer on the DMA Isolation structure
  * @retval None
  */
HAL_StatusTypeDef HAL_DMA_GetIsolationAttributes(DMA_HandleTypeDef const *const hdma,
                                                 DMA_IsolationConfigTypeDef *const pConfig)
{
  uint32_t ccidcfgr;

  /* Check the DMA peripheral handle parameter */
  if ((hdma == NULL) || (pConfig == NULL))
  {
    return HAL_ERROR;
  }

  ccidcfgr = hdma->Instance->CCIDCFGR;

  /* Check the parameters */
  assert_param(IS_HPDMA_INSTANCE(hdma->Instance));

  if ((ccidcfgr & DMA_CCIDCFGR_CFEN) == DMA_CCIDCFGR_CFEN)
  {
    pConfig->CidFiltering = DMA_ISOLATION_ON;
  }
  else
  {
    pConfig->CidFiltering = DMA_ISOLATION_OFF;
  }
  pConfig->StaticCid = (ccidcfgr & DMA_CCIDCFGR_SCID_Msk);

  return HAL_OK;
}

#if defined (CPU_IN_SECURE_STATE)
/**
  * @brief  Lock the DMA channel security and privilege attribute(s).
  * @param  hdma : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_DMA_LockChannelAttributes(DMA_HandleTypeDef const *const hdma)
{
  DMA_TypeDef *p_dma_instance;
  uint32_t channel_idx;

  /* Check the DMA peripheral handle parameter */
  if (hdma == NULL)
  {
    return HAL_ERROR;
  }

  /* Get DMA instance */
  p_dma_instance = GET_DMA_INSTANCE(hdma);

  /* Get channel index */
  channel_idx = 1UL << (GET_DMA_CHANNEL(hdma) & 0x1FU);

  /* Lock the DMA channel privilege and security attributes */
  p_dma_instance->RCFGLOCKR |= channel_idx;

  return HAL_OK;
}
#endif /* CPU_IN_SECURE_STATE */

/**
  * @brief  Get the security and privilege attribute lock state of a DMA channel.
  * @param  hdma       : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                      specified DMA Channel.
  * @param  pLockState : Pointer to lock state (returned value can be DMA_CHANNEL_ATTRIBUTE_UNLOCKED or
  *                      DMA_CHANNEL_ATTRIBUTE_LOCKED).
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_DMA_GetLockChannelAttributes(DMA_HandleTypeDef const *const hdma, uint32_t *const pLockState)
{
  const DMA_TypeDef *p_dma_instance;
  uint32_t channel_idx;

  /* Check the DMA peripheral handle and lock state parameters */
  if ((hdma == NULL) || (pLockState == NULL))
  {
    return HAL_ERROR;
  }

  /* Get DMA instance */
  p_dma_instance = GET_DMA_INSTANCE(hdma);

  /* Get channel index */
  channel_idx = 1UL << (GET_DMA_CHANNEL(hdma) & 0x1FU);

  /* Get channel lock attribute state */
  *pLockState = ((p_dma_instance->RCFGLOCKR & channel_idx) == 0U) ? DMA_CHANNEL_ATTRIBUTE_UNLOCKED : \
                DMA_CHANNEL_ATTRIBUTE_LOCKED;

  return HAL_OK;
}
/**
  * @}
  */

/**
  * @}
  */


/* Private functions -------------------------------------------------------------------------------------------------*/
/** @defgroup DMA_Private_Functions DMA Private Functions
  * @brief    DMA Private Functions
  * @{
  */

/**
  * @brief  Set the DMA channel normal transfer parameters.
  * @param  hdma        : Pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                       specified DMA Channel.
  * @param  SrcAddress  : The source data address.
  * @param  DstAddress  : The destination data address.
  * @param  SrcDataSize : The length of data to be transferred from source to destination in bytes.
  * @retval None.
  */
static void DMA_SetConfig(DMA_HandleTypeDef const *const hdma,
                          uint32_t SrcAddress,
                          uint32_t DstAddress,
                          uint32_t SrcDataSize)
{
  /* Configure the DMA channel data size */
  MODIFY_REG(hdma->Instance->CBR1, DMA_CBR1_BNDT, (SrcDataSize & DMA_CBR1_BNDT));

  /* Clear all interrupt flags */
  __HAL_DMA_CLEAR_FLAG(hdma, DMA_FLAG_TC | DMA_FLAG_HT | DMA_FLAG_DTE | DMA_FLAG_ULE | DMA_FLAG_USE | DMA_FLAG_SUSP |
                       DMA_FLAG_TO);

  /* Configure DMA channel source address */
  hdma->Instance->CSAR = SrcAddress;

  /* Configure DMA channel destination address */
  hdma->Instance->CDAR = DstAddress;
}

/**
  * @brief  Initialize the DMA channel in normal mode according to the specified parameters in the DMA_InitTypeDef.
  * @param  hdma : pointer to a DMA_HandleTypeDef structure that contains the configuration information for the
  *                specified DMA Channel.
  * @retval None.
  */
static void DMA_Init(DMA_HandleTypeDef const *const hdma)
{
  uint32_t tmpreg;

  /* Prepare DMA Channel Control Register (CCR) value *****************************************************************/
  tmpreg = hdma->Init.Priority;

  /* Write DMA Channel Control Register (CCR) */
  MODIFY_REG(hdma->Instance->CCR, DMA_CCR_PRIO | DMA_CCR_LAP | DMA_CCR_LSM, tmpreg);

  /* Prepare DMA Channel Transfer Register (CTR1) value ***************************************************************/
  tmpreg = hdma->Init.DestInc | hdma->Init.DestDataWidth | hdma->Init.SrcInc | hdma->Init.SrcDataWidth;

  /* Add parameters specific to HPDMA and GPDMA */
  if ((IS_HPDMA_INSTANCE(hdma->Instance) != 0U) || (IS_GPDMA_INSTANCE(hdma->Instance) != 0U))
  {
    tmpreg |= (hdma->Init.TransferAllocatedPort                                             |
               (((hdma->Init.DestBurstLength - 1U) << DMA_CTR1_DBL_1_Pos) & DMA_CTR1_DBL_1) |
               (((hdma->Init.SrcBurstLength - 1U) << DMA_CTR1_SBL_1_Pos) & DMA_CTR1_SBL_1));
  }

  /* Write DMA Channel Transfer Register 1 (CTR1) */
  MODIFY_REG(hdma->Instance->CTR1, ~(DMA_CTR1_SSEC | DMA_CTR1_DSEC), tmpreg);

  /* Prepare DMA Channel Transfer Register 2 (CTR2) value *************************************************************/
  tmpreg = hdma->Init.BlkHWRequest | (hdma->Init.Request & DMA_CTR2_REQSEL) | hdma->Init.TransferEventMode;

  /* Memory to Peripheral Transfer */
  if ((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
    if ((IS_HPDMA_INSTANCE(hdma->Instance) != 0U) || (IS_GPDMA_INSTANCE(hdma->Instance) != 0U))
    {
      tmpreg |= DMA_CTR2_DREQ;
    }
  }
  /* Memory to Memory Transfer */
  else if ((hdma->Init.Direction) == DMA_MEMORY_TO_MEMORY)
  {
    tmpreg |= DMA_CTR2_SWREQ;
  }
  else
  {
    /* Nothing to do */
  }

  /* Set DMA channel operation mode */
  tmpreg |= hdma->Init.Mode;

  /* Write DMA Channel Transfer Register 2 (CTR2) */
  MODIFY_REG(hdma->Instance->CTR2, (DMA_CTR2_TCEM  | DMA_CTR2_TRIGPOL | DMA_CTR2_TRIGSEL | DMA_CTR2_TRIGM |
                                    DMA_CTR2_PFREQ | DMA_CTR2_BREQ  | DMA_CTR2_DREQ    | DMA_CTR2_SWREQ   |
                                    DMA_CTR2_REQSEL), tmpreg);


  /* Write DMA Channel Block Register 1 (CBR1) ************************************************************************/
  WRITE_REG(hdma->Instance->CBR1, 0U);

  /* If 2D Addressing is supported by current channel */
  if (IS_DMA_2D_ADDRESSING_INSTANCE(hdma->Instance) != 0U)
  {
    /* Write DMA Channel Transfer Register 3 (CTR3) *******************************************************************/
    WRITE_REG(hdma->Instance->CTR3, 0U);

    /* Write DMA Channel Block Register 2 (CBR2) **********************************************************************/
    WRITE_REG(hdma->Instance->CBR2, 0U);
  }

  /* Write DMA Channel linked-list address register (CLLR) ************************************************************/
  WRITE_REG(hdma->Instance->CLLR, 0U);
}
/**
  * @}
  */

#endif /* HAL_DMA_MODULE_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
