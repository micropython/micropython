/**
  ******************************************************************************
  * @file    stm32n6xx_hal_sd_ex.c
  * @author  MCD Application Team
  * @brief   SD card Extended HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Secure Digital (SD) peripheral:
  *           + Extended features functions
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
   The SD Extension HAL driver can be used as follows:
   (+) Configure Buffer0 and Buffer1 start address and Buffer size using HAL_SDEx_ConfigDMAMultiBuffer() function.
   (+) Start Read and Write for multibuffer mode using HAL_SDEx_ReadBlocksDMAMultiBuffer()
       and HAL_SDEx_WriteBlocksDMAMultiBuffer() functions.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup SDEx SDEx
  * @brief SD Extended HAL module driver
  * @{
  */

#if defined (SDMMC1) || defined (SDMMC2)
#ifdef HAL_SD_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @addtogroup SDEx_Exported_Functions
  * @{
  */


/** @addtogroup SDEx_Exported_Functions_Group1
  *  @brief   Linked List management functions
  *
@verbatim
 ===============================================================================
                   ##### Linked List management functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to manage the needed functions.

@endverbatim
  * @{
  */

/**
  * @brief  Build Linked List node.
  * @param  pNode: Pointer to new node to add.
  * @param  pNodeConf: Pointer to configuration parameters for new node to add.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_BuildNode(SD_DMALinkNodeTypeDef *pNode, SD_DMALinkNodeConfTypeDef *pNodeConf)
{

  (void)SDMMC_DMALinkedList_BuildNode(pNode, pNodeConf);

  return (HAL_OK);

}

/**
  * @brief  Insert new Linked List node.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @param  pPrevNode: Pointer to previous node.
  * @param  pNewNode: Pointer to new node to insert.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_InsertNode(SD_DMALinkedListTypeDef *pLinkedList,
                                                    SD_DMALinkNodeTypeDef *pPrevNode, SD_DMALinkNodeTypeDef *pNewNode)
{

  (void)SDMMC_DMALinkedList_InsertNode(pLinkedList, pPrevNode, pNewNode);

  return (HAL_OK);

}
/**
  * @brief  Remove Linked List node.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @param  pNode: Pointer to node to remove.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_RemoveNode(SD_DMALinkedListTypeDef *pLinkedList, SD_DMALinkNodeTypeDef *pNode)
{

  if (SDMMC_DMALinkedList_RemoveNode(pLinkedList, pNode) != SDMMC_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Lock Linked List node.
  * @param  pNode: Pointer to node to remove.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_LockNode(SD_DMALinkNodeTypeDef *pNode)
{

  if (SDMMC_DMALinkedList_LockNode(pNode) != SDMMC_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Unlock Linked List node.
  * @param  pNode: Pointer to node to remove.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_UnlockNode(SD_DMALinkNodeTypeDef *pNode)
{

  if (SDMMC_DMALinkedList_UnlockNode(pNode) != SDMMC_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Enable Circular mode for DMA Linked List.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_EnableCircularMode(SD_DMALinkedListTypeDef *pLinkedList)
{

  (void)SDMMC_DMALinkedList_EnableCircularMode(pLinkedList);

  return HAL_OK;

}
/**
  * @brief  Disable Circular mode for DMA Linked List.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_DisableCircularMode(SD_DMALinkedListTypeDef *pLinkedList)
{

  (void)SDMMC_DMALinkedList_DisableCircularMode(pLinkedList);

  return HAL_OK;

}


/**
  * @brief  Reads block(s) from a specified address in a card. The received Data will be stored in linked list buffers.
  *         linked list should be prepared before call this function .
  * @param  hsd: SD handle
  * @param  pLinkedList: pointer to first linked list node
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Total number of blocks to read
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_ReadBlocks(SD_HandleTypeDef *hsd,
                                                    const SDMMC_DMALinkedListTypeDef *pLinkedList,
                                                    uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t DmaBase0_reg;
  uint32_t DmaBase1_reg;
  uint32_t add = BlockAdd;

  if (hsd->State == HAL_SD_STATE_READY)
  {
    if ((add + NumberOfBlocks) > (hsd->SdCard.LogBlockNbr))
    {
      hsd->ErrorCode |= HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    hsd->Instance->IDMABASER = (uint32_t) pLinkedList->pHeadNode->IDMABASER;
    hsd->Instance->IDMABSIZE = (uint32_t) pLinkedList->pHeadNode->IDMABSIZE;

    hsd->Instance->IDMABAR   = (uint32_t) pLinkedList->pHeadNode;
    hsd->Instance->IDMALAR   = (uint32_t) SDMMC_IDMALAR_ABR | SDMMC_IDMALAR_ULS | SDMMC_IDMALAR_ULA |
                               sizeof(SDMMC_DMALinkNodeTypeDef) ; /* Initial configuration */

    DmaBase0_reg = hsd->Instance->IDMABASER;
    DmaBase1_reg = hsd->Instance->IDMABAR;

    if ((hsd->Instance->IDMABSIZE == 0U) || (DmaBase0_reg == 0U) || (DmaBase1_reg == 0U))
    {
      hsd->ErrorCode = HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Initialize data control register */
    hsd->Instance->DCTRL = 0;
    /* Clear old Flags*/
    __HAL_SD_CLEAR_FLAG(hsd, SDMMC_STATIC_DATA_FLAGS);

    hsd->ErrorCode = HAL_SD_ERROR_NONE;
    hsd->State = HAL_SD_STATE_BUSY;

    if (hsd->SdCard.CardType != CARD_SDHC_SDXC)
    {
      add *= 512U;
    }

    /* Configure the SD DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hsd->Instance, &config);

    hsd->Instance->DCTRL |= SDMMC_DCTRL_FIFORST;

    __SDMMC_CMDTRANS_ENABLE(hsd->Instance);

    hsd->Instance->IDMACTRL = SDMMC_ENABLE_IDMA_DOUBLE_BUFF0;

    /* Read Blocks in DMA mode */
    hsd->Context = (SD_CONTEXT_READ_MULTIPLE_BLOCK | SD_CONTEXT_DMA);

    /* Read Multi Block command */
    errorstate = SDMMC_CmdReadMultiBlock(hsd->Instance, add);
    if (errorstate != HAL_SD_ERROR_NONE)
    {
      hsd->State = HAL_SD_STATE_READY;
      hsd->ErrorCode |= errorstate;
      return HAL_ERROR;
    }

    __HAL_SD_ENABLE_IT(hsd, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_RXOVERR | SDMMC_IT_DATAEND |
                             SDMMC_IT_IDMABTC));

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }

}

/**
  * @brief  Write block(s) to a specified address in a card. The transferred Data are stored linked list nodes buffers .
  *         linked list should be prepared before call this function .
  * @param  hsd: SD handle
  * @param  pLinkedList: pointer to first linked list node
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Total number of blocks to read
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_WriteBlocks(SD_HandleTypeDef *hsd,
                                                     const SDMMC_DMALinkedListTypeDef *pLinkedList,
                                                     uint32_t BlockAdd, uint32_t NumberOfBlocks)

{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t DmaBase0_reg;
  uint32_t DmaBase1_reg;
  uint32_t add = BlockAdd;

  if (hsd->State == HAL_SD_STATE_READY)
  {
    if ((add + NumberOfBlocks) > (hsd->SdCard.LogBlockNbr))
    {
      hsd->ErrorCode |= HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    hsd->Instance->IDMABASER = (uint32_t) pLinkedList->pHeadNode->IDMABASER;
    hsd->Instance->IDMABSIZE = (uint32_t) pLinkedList->pHeadNode->IDMABSIZE;

    hsd->Instance->IDMABAR = (uint32_t)  pLinkedList->pHeadNode;
    hsd->Instance->IDMALAR = (uint32_t)  SDMMC_IDMALAR_ABR | SDMMC_IDMALAR_ULS | SDMMC_IDMALAR_ULA |
                             sizeof(SDMMC_DMALinkNodeTypeDef) ; /* Initial configuration */

    DmaBase0_reg = hsd->Instance->IDMABASER;
    DmaBase1_reg = hsd->Instance->IDMABAR;

    if ((hsd->Instance->IDMABSIZE == 0U) || (DmaBase0_reg == 0U) || (DmaBase1_reg == 0U))
    {
      hsd->ErrorCode = HAL_SD_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Initialize data control register */
    hsd->Instance->DCTRL = 0;

    hsd->ErrorCode = HAL_SD_ERROR_NONE;

    hsd->State = HAL_SD_STATE_BUSY;

    if (hsd->SdCard.CardType != CARD_SDHC_SDXC)
    {
      add *= 512U;
    }

    /* Configure the SD DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hsd->Instance, &config);

    __SDMMC_CMDTRANS_ENABLE(hsd->Instance);

    hsd->Instance->IDMACTRL = SDMMC_ENABLE_IDMA_DOUBLE_BUFF0;

    /* Write Blocks in DMA mode */
    hsd->Context = (SD_CONTEXT_WRITE_MULTIPLE_BLOCK | SD_CONTEXT_DMA);

    /* Write Multi Block command */
    errorstate = SDMMC_CmdWriteMultiBlock(hsd->Instance, add);
    if (errorstate != HAL_SD_ERROR_NONE)
    {
      hsd->State = HAL_SD_STATE_READY;
      hsd->ErrorCode |= errorstate;
      return HAL_ERROR;
    }

    __HAL_SD_ENABLE_IT(hsd, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR | SDMMC_IT_DATAEND |
                             SDMMC_IT_IDMABTC));

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

#endif /* HAL_SD_MODULE_ENABLED */
#endif /* SDMMC1 || SDMMC2 */

/**
  * @}
  */

/**
  * @}
  */
