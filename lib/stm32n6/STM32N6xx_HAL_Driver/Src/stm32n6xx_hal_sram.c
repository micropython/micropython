/**
  ******************************************************************************
  * @file    stm32n6xx_hal_sram.c
  * @author  MCD Application Team
  * @brief   SRAM HAL module driver.
  *          This file provides a generic firmware to drive SRAM memories
  *          mounted as external device.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
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
    This driver is a generic layered driver which contains a set of APIs used to
    control SRAM memories. It uses the FMC layer functions to interface
    with SRAM devices.
    The following sequence should be followed to configure the FMC to interface
    with SRAM/PSRAM memories:

   (#) Declare a SRAM_HandleTypeDef handle structure, for example:
          SRAM_HandleTypeDef  hsram; and:

       (++) Fill the SRAM_HandleTypeDef handle "Init" field with the allowed
            values of the structure member.

       (++) Fill the SRAM_HandleTypeDef handle "Instance" field with a predefined
            base register instance for NOR or SRAM device

       (++) Fill the SRAM_HandleTypeDef handle "Extended" field with a predefined
            base register instance for NOR or SRAM extended mode

   (#) Declare two FMC_NORSRAM_TimingTypeDef structures, for both normal and extended
       mode timings; for example:
          FMC_NORSRAM_TimingTypeDef  Timing and FMC_NORSRAM_TimingTypeDef  ExTiming;
      and fill its fields with the allowed values of the structure member.

   (#) Initialize the SRAM Controller by calling the function HAL_SRAM_Init(). This function
       performs the following sequence:

       (##) MSP hardware layer configuration using the function HAL_SRAM_MspInit()
       (##) Control register configuration using the FMC NORSRAM interface function
            FMC_NORSRAM_Init()
       (##) Timing register configuration using the FMC NORSRAM interface function
            FMC_NORSRAM_Timing_Init()
       (##) Extended mode Timing register configuration using the FMC NORSRAM interface function
            FMC_NORSRAM_Extended_Timing_Init()
       (##) Enable the SRAM device using the macro __FMC_NORSRAM_ENABLE()

   (#) At this stage you can perform read/write accesses from/to the memory connected
       to the NOR/SRAM Bank. You can perform either polling or DMA transfer using the
       following APIs:
       (++) HAL_SRAM_Read()/HAL_SRAM_Write() for polling read/write access
       (++) HAL_SRAM_Read_DMA()/HAL_SRAM_Write_DMA() for DMA read/write transfer

   (#) You can also control the SRAM device by calling the control APIs HAL_SRAM_WriteOperation_Enable()/
       HAL_SRAM_WriteOperation_Disable() to respectively enable/disable the SRAM write operation

   (#) You can continuously monitor the SRAM device HAL state by calling the function
       HAL_SRAM_GetState()

       *** Callback registration ***
    =============================================
    [..]
      The compilation define  USE_HAL_SRAM_REGISTER_CALLBACKS when set to 1
      allows the user to configure dynamically the driver callbacks.

      Use Functions HAL_SRAM_RegisterCallback() to register a user callback,
      it allows to register following callbacks:
        (+) MspInitCallback    : SRAM MspInit.
        (+) MspDeInitCallback  : SRAM MspDeInit.
      This function takes as parameters the HAL peripheral handle, the Callback ID
      and a pointer to the user callback function.

      Use function HAL_SRAM_UnRegisterCallback() to reset a callback to the default
      weak (overridden) function. It allows to reset following callbacks:
        (+) MspInitCallback    : SRAM MspInit.
        (+) MspDeInitCallback  : SRAM MspDeInit.
      This function) takes as parameters the HAL peripheral handle and the Callback ID.

      By default, after the HAL_SRAM_Init and if the state is HAL_SRAM_STATE_RESET
      all callbacks are reset to the corresponding legacy weak (overridden) functions.
      Exception done for MspInit and MspDeInit callbacks that are respectively
      reset to the legacy weak (overridden) functions in the HAL_SRAM_Init
      and  HAL_SRAM_DeInit only when these callbacks are null (not registered beforehand).
      If not, MspInit or MspDeInit are not null, the HAL_SRAM_Init and HAL_SRAM_DeInit
      keep and use the user MspInit/MspDeInit callbacks (registered beforehand)

      Callbacks can be registered/unregistered in READY state only.
      Exception done for MspInit/MspDeInit callbacks that can be registered/unregistered
      in READY or RESET state, thus registered (user) MspInit/DeInit callbacks can be used
      during the Init/DeInit.
      In that case first register the MspInit/MspDeInit user callbacks
      using HAL_SRAM_RegisterCallback before calling HAL_SRAM_DeInit
      or HAL_SRAM_Init function.

      When The compilation define USE_HAL_SRAM_REGISTER_CALLBACKS is set to 0 or
      not defined, the callback registering feature is not available
      and weak (overridden) callbacks are used.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"


/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

#ifdef HAL_SRAM_MODULE_ENABLED

/** @defgroup SRAM SRAM
  * @brief SRAM driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup SRAM_Private_Functions SRAM Private Functions
  * @{
  */
static void SRAM_DMACplt(DMA_HandleTypeDef *hdma);
static void SRAM_DMACpltProt(DMA_HandleTypeDef *hdma);
static void SRAM_DMAError(DMA_HandleTypeDef *hdma);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @defgroup SRAM_Exported_Functions SRAM Exported Functions
  * @{
  */

/** @defgroup SRAM_Exported_Functions_Group1 Initialization and de-initialization functions
  * @brief    Initialization and Configuration functions.
  *
  @verbatim
  ==============================================================================
           ##### SRAM Initialization and de_initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to initialize/de-initialize
          the SRAM memory

@endverbatim
  * @{
  */

/**
  * @brief  Performs the SRAM device initialization sequence
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @param  Timing Pointer to SRAM control timing structure
  * @param  ExtTiming Pointer to SRAM extended mode timing structure
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_Init(SRAM_HandleTypeDef *hsram, FMC_NORSRAM_TimingTypeDef *Timing,
                                FMC_NORSRAM_TimingTypeDef *ExtTiming)
{
  /* Check the SRAM handle parameter */
  if (hsram == NULL)
  {
    return HAL_ERROR;
  }

  if (hsram->State == HAL_SRAM_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hsram->Lock = HAL_UNLOCKED;

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
    if (hsram->MspInitCallback == NULL)
    {
      hsram->MspInitCallback = HAL_SRAM_MspInit;
    }
    hsram->DmaXferCpltCallback = HAL_SRAM_DMA_XferCpltCallback;
    hsram->DmaXferErrorCallback = HAL_SRAM_DMA_XferErrorCallback;

    /* Init the low level hardware */
    hsram->MspInitCallback(hsram);
#else
    /* Initialize the low level hardware (MSP) */
    HAL_SRAM_MspInit(hsram);
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS */
  }

  /* Initialize SRAM control Interface */
  (void)FMC_NORSRAM_Init(hsram->Instance, &(hsram->Init));

  /* Initialize SRAM timing Interface */
  (void)FMC_NORSRAM_Timing_Init(hsram->Instance, Timing, hsram->Init.NSBank);

  /* Initialize SRAM extended mode timing Interface */
  (void)FMC_NORSRAM_Extended_Timing_Init(hsram->Extended, ExtTiming, hsram->Init.NSBank,
                                         hsram->Init.ExtendedMode);

  /* Enable the NORSRAM device */
  __FMC_NORSRAM_ENABLE(hsram->Instance, hsram->Init.NSBank);

  /* Enable FMC Peripheral */
  __FMC_ENABLE();

  /* Initialize the SRAM controller state */
  hsram->State = HAL_SRAM_STATE_READY;

  return HAL_OK;
}

/**
  * @brief  Performs the SRAM device De-initialization sequence.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_DeInit(SRAM_HandleTypeDef *hsram)
{
#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
  if (hsram->MspDeInitCallback == NULL)
  {
    hsram->MspDeInitCallback = HAL_SRAM_MspDeInit;
  }

  /* DeInit the low level hardware */
  hsram->MspDeInitCallback(hsram);
#else
  /* De-Initialize the low level hardware (MSP) */
  HAL_SRAM_MspDeInit(hsram);
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS */

  /* Configure the SRAM registers with their reset values */
  (void)FMC_NORSRAM_DeInit(hsram->Instance, hsram->Extended, hsram->Init.NSBank);

  /* Reset the SRAM controller state */
  hsram->State = HAL_SRAM_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hsram);

  return HAL_OK;
}

/**
  * @brief  SRAM MSP Init.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @retval None
  */
__weak void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsram);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SRAM_MspInit could be implemented in the user file
   */
}

/**
  * @brief  SRAM MSP DeInit.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @retval None
  */
__weak void HAL_SRAM_MspDeInit(SRAM_HandleTypeDef *hsram)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsram);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SRAM_MspDeInit could be implemented in the user file
   */
}

/**
  * @brief  DMA transfer complete callback.
  * @param  hdma pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @retval None
  */
__weak void HAL_SRAM_DMA_XferCpltCallback(DMA_HandleTypeDef *hdma)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SRAM_DMA_XferCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  DMA transfer complete error callback.
  * @param  hdma pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @retval None
  */
__weak void HAL_SRAM_DMA_XferErrorCallback(DMA_HandleTypeDef *hdma)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hdma);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_SRAM_DMA_XferErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup SRAM_Exported_Functions_Group2 Input Output and memory control functions
  * @brief    Input Output and memory control functions
  *
  @verbatim
  ==============================================================================
                  ##### SRAM Input and Output functions #####
  ==============================================================================
  [..]
    This section provides functions allowing to use and control the SRAM memory

@endverbatim
  * @{
  */

/**
  * @brief  Reads 8-bit buffer from SRAM memory.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @param  pAddress Pointer to read start address
  * @param  pDstBuffer Pointer to destination buffer
  * @param  BufferSize Size of the buffer to read from memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_Read_8b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pDstBuffer,
                                   uint32_t BufferSize)
{
  uint32_t size;
  __IO uint8_t *psramaddress = (uint8_t *)pAddress;
  uint8_t *pdestbuff = pDstBuffer;
  HAL_SRAM_StateTypeDef state = hsram->State;

  /* Check the SRAM controller state */
  if ((state == HAL_SRAM_STATE_READY) || (state == HAL_SRAM_STATE_PROTECTED))
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Read data from memory */
    for (size = BufferSize; size != 0U; size--)
    {
      *pdestbuff = *psramaddress;
      pdestbuff++;
      psramaddress++;
    }

    /* Update the SRAM controller state */
    hsram->State = state;

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Writes 8-bit buffer to SRAM memory.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @param  pAddress Pointer to write start address
  * @param  pSrcBuffer Pointer to source buffer to write
  * @param  BufferSize Size of the buffer to write to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_Write_8b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint8_t *pSrcBuffer,
                                    uint32_t BufferSize)
{
  uint32_t size;
  __IO uint8_t *psramaddress = (uint8_t *)pAddress;
  uint8_t *psrcbuff = pSrcBuffer;

  /* Check the SRAM controller state */
  if (hsram->State == HAL_SRAM_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Write data to memory */
    for (size = BufferSize; size != 0U; size--)
    {
      *psramaddress = *psrcbuff;
      psrcbuff++;
      psramaddress++;
    }

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Reads 16-bit buffer from SRAM memory.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @param  pAddress Pointer to read start address
  * @param  pDstBuffer Pointer to destination buffer
  * @param  BufferSize Size of the buffer to read from memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_Read_16b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint16_t *pDstBuffer,
                                    uint32_t BufferSize)
{
  uint32_t size;
  __IO uint32_t *psramaddress = pAddress;
  uint16_t *pdestbuff = pDstBuffer;
  uint8_t limit;
  HAL_SRAM_StateTypeDef state = hsram->State;

  /* Check the SRAM controller state */
  if ((state == HAL_SRAM_STATE_READY) || (state == HAL_SRAM_STATE_PROTECTED))
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Check if the size is a 32-bits multiple */
    limit = (((BufferSize % 2U) != 0U) ? 1U : 0U);

    /* Read data from memory */
    for (size = BufferSize; size != limit; size -= 2U)
    {
      *pdestbuff = (uint16_t)((*psramaddress) & 0x0000FFFFU);
      pdestbuff++;
      *pdestbuff = (uint16_t)(((*psramaddress) & 0xFFFF0000U) >> 16U);
      pdestbuff++;
      psramaddress++;
    }

    /* Read last 16-bits if size is not 32-bits multiple */
    if (limit != 0U)
    {
      *pdestbuff = (uint16_t)((*psramaddress) & 0x0000FFFFU);
    }

    /* Update the SRAM controller state */
    hsram->State = state;

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Writes 16-bit buffer to SRAM memory.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @param  pAddress Pointer to write start address
  * @param  pSrcBuffer Pointer to source buffer to write
  * @param  BufferSize Size of the buffer to write to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_Write_16b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint16_t *pSrcBuffer,
                                     uint32_t BufferSize)
{
  uint32_t size;
  __IO uint32_t *psramaddress = pAddress;
  uint16_t *psrcbuff = pSrcBuffer;
  uint8_t limit;

  /* Check the SRAM controller state */
  if (hsram->State == HAL_SRAM_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Check if the size is a 32-bits multiple */
    limit = (((BufferSize % 2U) != 0U) ? 1U : 0U);

    /* Write data to memory */
    for (size = BufferSize; size != limit; size -= 2U)
    {
      *psramaddress = (uint32_t)(*psrcbuff);
      psrcbuff++;
      *psramaddress |= ((uint32_t)(*psrcbuff) << 16U);
      psrcbuff++;
      psramaddress++;
    }

    /* Write last 16-bits if size is not 32-bits multiple */
    if (limit != 0U)
    {
      *psramaddress = ((uint32_t)(*psrcbuff) & 0x0000FFFFU) | ((*psramaddress) & 0xFFFF0000U);
    }

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Reads 32-bit buffer from SRAM memory.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @param  pAddress Pointer to read start address
  * @param  pDstBuffer Pointer to destination buffer
  * @param  BufferSize Size of the buffer to read from memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_Read_32b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pDstBuffer,
                                    uint32_t BufferSize)
{
  uint32_t size;
  __IO uint32_t *psramaddress = pAddress;
  uint32_t *pdestbuff = pDstBuffer;
  HAL_SRAM_StateTypeDef state = hsram->State;

  /* Check the SRAM controller state */
  if ((state == HAL_SRAM_STATE_READY) || (state == HAL_SRAM_STATE_PROTECTED))
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Read data from memory */
    for (size = BufferSize; size != 0U; size--)
    {
      *pdestbuff = *psramaddress;
      pdestbuff++;
      psramaddress++;
    }

    /* Update the SRAM controller state */
    hsram->State = state;

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Writes 32-bit buffer to SRAM memory.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @param  pAddress Pointer to write start address
  * @param  pSrcBuffer Pointer to source buffer to write
  * @param  BufferSize Size of the buffer to write to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_Write_32b(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pSrcBuffer,
                                     uint32_t BufferSize)
{
  uint32_t size;
  __IO uint32_t *psramaddress = pAddress;
  uint32_t *psrcbuff = pSrcBuffer;

  /* Check the SRAM controller state */
  if (hsram->State == HAL_SRAM_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Write data to memory */
    for (size = BufferSize; size != 0U; size--)
    {
      *psramaddress = *psrcbuff;
      psrcbuff++;
      psramaddress++;
    }

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Reads a Words data from the SRAM memory using DMA transfer.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @param  pAddress Pointer to read start address
  * @param  pDstBuffer Pointer to destination buffer
  * @param  BufferSize Size of the buffer to read from memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_Read_DMA(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pDstBuffer,
                                    uint32_t BufferSize)
{
  HAL_StatusTypeDef status;
  HAL_SRAM_StateTypeDef state = hsram->State;
  uint32_t size;
  uint32_t data_width;

  /* Check the SRAM controller state */
  if ((state == HAL_SRAM_STATE_READY) || (state == HAL_SRAM_STATE_PROTECTED))
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Configure DMA user callbacks */
    if (state == HAL_SRAM_STATE_READY)
    {
      hsram->hdma->XferCpltCallback = SRAM_DMACplt;
    }
    else
    {
      hsram->hdma->XferCpltCallback = SRAM_DMACpltProt;
    }
    hsram->hdma->XferErrorCallback = SRAM_DMAError;

    if ((hsram->hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      if ((hsram->hdma->LinkedListQueue != 0U) && (hsram->hdma->LinkedListQueue->Head != 0U))
      {
        /* Check destination data width and set the size to be transferred */
        data_width = hsram->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] & DMA_CTR1_DDW_LOG2;

        if (data_width == DMA_DEST_DATAWIDTH_WORD)
        {
          size = (BufferSize * 4U);
        }
        else if (data_width == DMA_DEST_DATAWIDTH_HALFWORD)
        {
          size = (BufferSize * 2U);
        }
        else
        {
          size = (BufferSize);
        }
        /* Set Source , destination , buffer size */
        /* Set DMA data size */
        hsram->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = size;
        /* Set DMA source address */
        hsram->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)pAddress;
        /* Set DMA destination address */
        hsram->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)pDstBuffer;

        /* Enable the DMA Stream */
        status = HAL_DMAEx_List_Start_IT(hsram->hdma);
      }
      else
      {
        /* Change SRAM state */
        hsram->State = HAL_SRAM_STATE_READY;

        __HAL_UNLOCK(hsram);

        status = HAL_ERROR;
      }
    }
    else
    {
      /* Check destination data width and set the size to be transferred */
      data_width = hsram->hdma->Init.DestDataWidth;

      if (data_width == DMA_DEST_DATAWIDTH_WORD)
      {
        size = (BufferSize * 4U);
      }
      else if (data_width == DMA_DEST_DATAWIDTH_HALFWORD)
      {
        size = (BufferSize * 2U);
      }
      else
      {
        size = (BufferSize);
      }

      /* Enable the DMA Stream */
      status = HAL_DMA_Start_IT(hsram->hdma, (uint32_t)pAddress, (uint32_t)pDstBuffer, size);
    }

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    status = HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Writes a Words data buffer to SRAM memory using DMA transfer.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @param  pAddress Pointer to write start address
  * @param  pSrcBuffer Pointer to source buffer to write
  * @param  BufferSize Size of the buffer to write to memory
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_Write_DMA(SRAM_HandleTypeDef *hsram, uint32_t *pAddress, uint32_t *pSrcBuffer,
                                     uint32_t BufferSize)
{
  HAL_StatusTypeDef status;
  uint32_t size;
  uint32_t data_width;

  /* Check the SRAM controller state */
  if (hsram->State == HAL_SRAM_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Configure DMA user callbacks */
    hsram->hdma->XferCpltCallback = SRAM_DMACplt;
    hsram->hdma->XferErrorCallback = SRAM_DMAError;

    if ((hsram->hdma->Mode & DMA_LINKEDLIST) == DMA_LINKEDLIST)
    {
      if ((hsram->hdma->LinkedListQueue != 0U) && (hsram->hdma->LinkedListQueue->Head != 0U))
      {
        /* Check destination data width and set the size to be transferred */
        data_width = hsram->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CTR1_DEFAULT_OFFSET] & DMA_CTR1_DDW_LOG2;

        if (data_width == DMA_DEST_DATAWIDTH_WORD)
        {
          size = (BufferSize * 4U);
        }
        else if (data_width == DMA_DEST_DATAWIDTH_HALFWORD)
        {
          size = (BufferSize * 2U);
        }
        else
        {
          size = (BufferSize);
        }
        /* Set Source , destination , buffer size */
        /* Set DMA data size */
        hsram->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CBR1_DEFAULT_OFFSET] = size;
        /* Set DMA source address */
        hsram->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CSAR_DEFAULT_OFFSET] = (uint32_t)pSrcBuffer;
        /* Set DMA destination address */
        hsram->hdma->LinkedListQueue->Head->LinkRegisters[NODE_CDAR_DEFAULT_OFFSET] = (uint32_t)pAddress;
        /* Enable the DMA Stream */
        status = HAL_DMAEx_List_Start_IT(hsram->hdma);
      }
      else
      {
        /* Change SRAM state */
        hsram->State = HAL_SRAM_STATE_READY;

        __HAL_UNLOCK(hsram);

        status = HAL_ERROR;
      }
    }
    else
    {
      /* Check destination data width and set the size to be transferred */
      data_width = hsram->hdma->Init.DestDataWidth;

      if (data_width == DMA_DEST_DATAWIDTH_WORD)
      {
        size = (BufferSize * 4U);
      }
      else if (data_width == DMA_DEST_DATAWIDTH_HALFWORD)
      {
        size = (BufferSize * 2U);
      }
      else
      {
        size = (BufferSize);
      }

      /* Enable the DMA Stream */
      status = HAL_DMA_Start_IT(hsram->hdma, (uint32_t)pSrcBuffer, (uint32_t)pAddress, size);
    }

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    status = HAL_ERROR;
  }

  return status;
}

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
/**
  * @brief  Register a User SRAM Callback
  *         To be used to override the weak predefined callback
  * @param hsram : SRAM handle
  * @param CallbackId : ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_SRAM_MSP_INIT_CB_ID       SRAM MspInit callback ID
  *          @arg @ref HAL_SRAM_MSP_DEINIT_CB_ID     SRAM MspDeInit callback ID
  * @param pCallback : pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_SRAM_RegisterCallback(SRAM_HandleTypeDef *hsram, HAL_SRAM_CallbackIDTypeDef CallbackId,
                                            pSRAM_CallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;
  HAL_SRAM_StateTypeDef state;

  if (pCallback == NULL)
  {
    return HAL_ERROR;
  }

  state = hsram->State;
  if ((state == HAL_SRAM_STATE_READY) || (state == HAL_SRAM_STATE_RESET) || (state == HAL_SRAM_STATE_PROTECTED))
  {
    switch (CallbackId)
    {
      case HAL_SRAM_MSP_INIT_CB_ID :
        hsram->MspInitCallback = pCallback;
        break;
      case HAL_SRAM_MSP_DEINIT_CB_ID :
        hsram->MspDeInitCallback = pCallback;
        break;
      default :
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Unregister a User SRAM Callback
  *         SRAM Callback is redirected to the weak predefined callback
  * @param hsram : SRAM handle
  * @param CallbackId : ID of the callback to be unregistered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_SRAM_MSP_INIT_CB_ID       SRAM MspInit callback ID
  *          @arg @ref HAL_SRAM_MSP_DEINIT_CB_ID     SRAM MspDeInit callback ID
  *          @arg @ref HAL_SRAM_DMA_XFER_CPLT_CB_ID  SRAM DMA Xfer Complete callback ID
  *          @arg @ref HAL_SRAM_DMA_XFER_ERR_CB_ID   SRAM DMA Xfer Error callback ID
  * @retval status
  */
HAL_StatusTypeDef HAL_SRAM_UnRegisterCallback(SRAM_HandleTypeDef *hsram, HAL_SRAM_CallbackIDTypeDef CallbackId)
{
  HAL_StatusTypeDef status = HAL_OK;
  HAL_SRAM_StateTypeDef state;

  state = hsram->State;
  if ((state == HAL_SRAM_STATE_READY) || (state == HAL_SRAM_STATE_PROTECTED))
  {
    switch (CallbackId)
    {
      case HAL_SRAM_MSP_INIT_CB_ID :
        hsram->MspInitCallback = HAL_SRAM_MspInit;
        break;
      case HAL_SRAM_MSP_DEINIT_CB_ID :
        hsram->MspDeInitCallback = HAL_SRAM_MspDeInit;
        break;
      case HAL_SRAM_DMA_XFER_CPLT_CB_ID :
        hsram->DmaXferCpltCallback = HAL_SRAM_DMA_XferCpltCallback;
        break;
      case HAL_SRAM_DMA_XFER_ERR_CB_ID :
        hsram->DmaXferErrorCallback = HAL_SRAM_DMA_XferErrorCallback;
        break;
      default :
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else if (state == HAL_SRAM_STATE_RESET)
  {
    switch (CallbackId)
    {
      case HAL_SRAM_MSP_INIT_CB_ID :
        hsram->MspInitCallback = HAL_SRAM_MspInit;
        break;
      case HAL_SRAM_MSP_DEINIT_CB_ID :
        hsram->MspDeInitCallback = HAL_SRAM_MspDeInit;
        break;
      default :
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* update return status */
    status =  HAL_ERROR;
  }

  return status;
}

/**
  * @brief  Register a User SRAM Callback for DMA transfers
  *         To be used to override the weak predefined callback
  * @param hsram : SRAM handle
  * @param CallbackId : ID of the callback to be registered
  *        This parameter can be one of the following values:
  *          @arg @ref HAL_SRAM_DMA_XFER_CPLT_CB_ID  SRAM DMA Xfer Complete callback ID
  *          @arg @ref HAL_SRAM_DMA_XFER_ERR_CB_ID   SRAM DMA Xfer Error callback ID
  * @param pCallback : pointer to the Callback function
  * @retval status
  */
HAL_StatusTypeDef HAL_SRAM_RegisterDmaCallback(SRAM_HandleTypeDef *hsram, HAL_SRAM_CallbackIDTypeDef CallbackId,
                                               pSRAM_DmaCallbackTypeDef pCallback)
{
  HAL_StatusTypeDef status = HAL_OK;
  HAL_SRAM_StateTypeDef state;

  if (pCallback == NULL)
  {
    return HAL_ERROR;
  }

  /* Process locked */
  __HAL_LOCK(hsram);

  state = hsram->State;
  if ((state == HAL_SRAM_STATE_READY) || (state == HAL_SRAM_STATE_PROTECTED))
  {
    switch (CallbackId)
    {
      case HAL_SRAM_DMA_XFER_CPLT_CB_ID :
        hsram->DmaXferCpltCallback = pCallback;
        break;
      case HAL_SRAM_DMA_XFER_ERR_CB_ID :
        hsram->DmaXferErrorCallback = pCallback;
        break;
      default :
        /* update return status */
        status =  HAL_ERROR;
        break;
    }
  }
  else
  {
    /* update return status */
    status =  HAL_ERROR;
  }

  /* Release Lock */
  __HAL_UNLOCK(hsram);
  return status;
}
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS */

/**
  * @}
  */

/** @defgroup SRAM_Exported_Functions_Group3 Control functions
  *  @brief   Control functions
  *
@verbatim
  ==============================================================================
                        ##### SRAM Control functions #####
  ==============================================================================
  [..]
    This subsection provides a set of functions allowing to control dynamically
    the SRAM interface.

@endverbatim
  * @{
  */

/**
  * @brief  Enables dynamically SRAM write operation.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_WriteOperation_Enable(SRAM_HandleTypeDef *hsram)
{
  /* Check the SRAM controller state */
  if (hsram->State == HAL_SRAM_STATE_PROTECTED)
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Enable write operation */
    (void)FMC_NORSRAM_WriteOperation_Enable(hsram->Instance, hsram->Init.NSBank);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Disables dynamically SRAM write operation.
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SRAM_WriteOperation_Disable(SRAM_HandleTypeDef *hsram)
{
  /* Check the SRAM controller state */
  if (hsram->State == HAL_SRAM_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(hsram);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_BUSY;

    /* Disable write operation */
    (void)FMC_NORSRAM_WriteOperation_Disable(hsram->Instance, hsram->Init.NSBank);

    /* Update the SRAM controller state */
    hsram->State = HAL_SRAM_STATE_PROTECTED;

    /* Process unlocked */
    __HAL_UNLOCK(hsram);
  }
  else
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup SRAM_Exported_Functions_Group4 Peripheral State functions
  *  @brief   Peripheral State functions
  *
@verbatim
  ==============================================================================
                      ##### SRAM State functions #####
  ==============================================================================
  [..]
    This subsection permits to get in run-time the status of the SRAM controller
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Returns the SRAM controller state
  * @param  hsram pointer to a SRAM_HandleTypeDef structure that contains
  *                the configuration information for SRAM module.
  * @retval HAL state
  */
HAL_SRAM_StateTypeDef HAL_SRAM_GetState(const SRAM_HandleTypeDef *hsram)
{
  return hsram->State;
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup SRAM_Private_Functions SRAM Private Functions
  * @{
  */

/**
  * @brief  DMA SRAM process complete callback.
  * @param  hdma : DMA handle
  * @retval None
  */
static void SRAM_DMACplt(DMA_HandleTypeDef *hdma)
{
  /* Derogation MISRAC2012-Rule-11.5 */
  SRAM_HandleTypeDef *hsram = (SRAM_HandleTypeDef *)(hdma->Parent);

  /* Disable the DMA channel */
  __HAL_DMA_DISABLE(hdma);

  /* Update the SRAM controller state */
  hsram->State = HAL_SRAM_STATE_READY;

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
  hsram->DmaXferCpltCallback(hdma);
#else
  HAL_SRAM_DMA_XferCpltCallback(hdma);
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA SRAM process complete callback.
  * @param  hdma : DMA handle
  * @retval None
  */
static void SRAM_DMACpltProt(DMA_HandleTypeDef *hdma)
{
  /* Derogation MISRAC2012-Rule-11.5 */
  SRAM_HandleTypeDef *hsram = (SRAM_HandleTypeDef *)(hdma->Parent);

  /* Disable the DMA channel */
  __HAL_DMA_DISABLE(hdma);

  /* Update the SRAM controller state */
  hsram->State = HAL_SRAM_STATE_PROTECTED;

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
  hsram->DmaXferCpltCallback(hdma);
#else
  HAL_SRAM_DMA_XferCpltCallback(hdma);
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS */
}

/**
  * @brief  DMA SRAM error callback.
  * @param  hdma : DMA handle
  * @retval None
  */
static void SRAM_DMAError(DMA_HandleTypeDef *hdma)
{
  /* Derogation MISRAC2012-Rule-11.5 */
  SRAM_HandleTypeDef *hsram = (SRAM_HandleTypeDef *)(hdma->Parent);

  /* Disable the DMA channel */
  __HAL_DMA_DISABLE(hdma);

  /* Update the SRAM controller state */
  hsram->State = HAL_SRAM_STATE_ERROR;

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
  hsram->DmaXferErrorCallback(hdma);
#else
  HAL_SRAM_DMA_XferErrorCallback(hdma);
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS */
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_SRAM_MODULE_ENABLED */

/**
  * @}
  */

