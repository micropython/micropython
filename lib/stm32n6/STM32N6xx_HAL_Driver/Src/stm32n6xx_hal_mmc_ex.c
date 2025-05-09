/**
  ******************************************************************************
  * @file    stm32n6xx_hal_mmc_ex.c
  * @author  MCD Application Team
  * @brief   MMC card Extended HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Secure Digital (MMC) peripheral:
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
   The MMC Extension HAL driver can be used as follows:
   (+) Configure Buffer0 and Buffer1 start address and Buffer size using HAL_MMCEx_ConfigDMAMultiBuffer() function.

   (+) Start Read and Write for multibuffer mode using HAL_MMCEx_ReadBlocksDMAMultiBuffer() and
       HAL_MMCEx_WriteBlocksDMAMultiBuffer() functions.

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup MMCEx MMCEx
  * @brief MMC Extended HAL module driver
  * @{
  */

#if defined (SDMMC1) || defined (SDMMC2)
#ifdef HAL_MMC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/** @addtogroup MMCEx_Exported_Functions
  * @{
  */


/** @addtogroup MMCEx_Exported_Functions_Group1
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
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_BuildNode(MMC_DMALinkNodeTypeDef *pNode,
                                                    MMC_DMALinkNodeConfTypeDef *pNodeConf)
{

  if (SDMMC_DMALinkedList_BuildNode(pNode, pNodeConf) != SDMMC_ERROR_NONE)
  {
    return (HAL_ERROR);
  }
  else
  {
    return (HAL_OK);
  }

}
/**
  * @brief  Insert Linked List node.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @param  pPrevNode: Pointer to previous node.
  * @param  pNewNode: Pointer to new node to insert.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_InsertNode(MMC_DMALinkedListTypeDef *pLinkedList,
                                                     MMC_DMALinkNodeTypeDef *pPrevNode,
                                                     MMC_DMALinkNodeTypeDef *pNewNode)
{

  if (SDMMC_DMALinkedList_InsertNode(pLinkedList, pPrevNode, pNewNode) != SDMMC_ERROR_NONE)
  {
    return (HAL_ERROR);
  }
  else
  {
    return (HAL_OK);
  }

}
/**
  * @brief  Remove Linked List node.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @param  pNode: Pointer to node to remove.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_RemoveNode(MMC_DMALinkedListTypeDef *pLinkedList,
                                                     MMC_DMALinkNodeTypeDef *pNode)
{

  if (SDMMC_DMALinkedList_RemoveNode(pLinkedList, pNode) != SDMMC_ERROR_NONE)
  {
    return (HAL_ERROR);
  }
  else
  {
    return (HAL_OK);
  }
}

/**
  * @brief  Lock Linked List node.
  * @param  pNode: Pointer to node to remove.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_LockNode(MMC_DMALinkNodeTypeDef *pNode)
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
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_UnlockNode(MMC_DMALinkNodeTypeDef *pNode)
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
  * @brief  Enable Circular mode for DMA Linked List mode.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_EnableCircularMode(MMC_DMALinkedListTypeDef *pLinkedList)
{

  if (SDMMC_DMALinkedList_EnableCircularMode(pLinkedList) != SDMMC_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}
/**
  * @brief  Disable Circular mode for DMA Linked List mode.
  * @param  pLinkedList: Pointer to the linkedlist that contains transfer nodes
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_DisableCircularMode(MMC_DMALinkedListTypeDef *pLinkedList)
{

  if (SDMMC_DMALinkedList_DisableCircularMode(pLinkedList) != SDMMC_ERROR_NONE)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}


/**
  * @brief  Reads block(s) from a specified address in a card. The received Data will be stored in linked list buffers.
  *         linked list should be prepared before call this function .
  * @param  hmmc: MMC handle
  * @param  pLinkedList: pointer to first linked list node
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Total number of blocks to read
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_ReadBlocks(MMC_HandleTypeDef *hmmc,
                                                     const MMC_DMALinkedListTypeDef *pLinkedList,
                                                     uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
  SDMMC_DataInitTypeDef config;
  uint32_t DmaBase0_reg;
  uint32_t DmaBase1_reg;
  uint32_t errorstate;
  uint32_t add = BlockAdd;

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    if ((BlockAdd + NumberOfBlocks) > (hmmc->MmcCard.LogBlockNbr))
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
    if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS) & 0x000000FFU) != 0x0U)
    {
      if ((NumberOfBlocks % 8U) != 0U)
      {
        /* The number of blocks should be a multiple of 8 sectors of 512 bytes = 4 KBytes */
        hmmc->ErrorCode |= HAL_MMC_ERROR_BLOCK_LEN_ERR;
        return HAL_ERROR;
      }

      if ((BlockAdd % 8U) != 0U)
      {
        /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
        hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
        return HAL_ERROR;
      }
    }

    hmmc->Instance->IDMABASER = (uint32_t) pLinkedList->pHeadNode->IDMABASER;
    hmmc->Instance->IDMABSIZE = (uint32_t) pLinkedList->pHeadNode->IDMABSIZE;
    hmmc->Instance->IDMABAR   = (uint32_t) pLinkedList->pHeadNode;
    hmmc->Instance->IDMALAR   = (uint32_t) SDMMC_IDMALAR_ABR | SDMMC_IDMALAR_ULS | SDMMC_IDMALAR_ULA |
                                sizeof(SDMMC_DMALinkNodeTypeDef) ; /* Initial configuration */

    DmaBase0_reg = hmmc->Instance->IDMABASER;
    DmaBase1_reg = hmmc->Instance->IDMABAR;

    if ((hmmc->Instance->IDMABSIZE == 0U) || (DmaBase0_reg == 0U) || (DmaBase1_reg == 0U))
    {
      hmmc->ErrorCode = HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0;

    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;
    hmmc->State = HAL_MMC_STATE_BUSY;

    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      add *= 512U;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_SDMMC;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);

    hmmc->Instance->DCTRL |= SDMMC_DCTRL_FIFORST;

    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    hmmc->Instance->IDMACTRL = SDMMC_ENABLE_IDMA_DOUBLE_BUFF0;

    /* Read Blocks in DMA mode */
    hmmc->Context = (MMC_CONTEXT_READ_MULTIPLE_BLOCK | MMC_CONTEXT_DMA);

    /* Read Multi Block command */
    errorstate = SDMMC_CmdReadMultiBlock(hmmc->Instance, add);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      hmmc->State = HAL_MMC_STATE_READY;
      hmmc->ErrorCode |= errorstate;
      return HAL_ERROR;
    }

    __HAL_MMC_ENABLE_IT(hmmc, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_RXOVERR | SDMMC_IT_DATAEND |
                               SDMMC_FLAG_IDMATE | SDMMC_FLAG_IDMABTC));

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
  * @param  hmmc: MMC handle
  * @param  pLinkedList: pointer to first linked list node
  * @param  BlockAdd: Block Address from where data is to be read
  * @param  NumberOfBlocks: Total number of blocks to read
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_WriteBlocks(MMC_HandleTypeDef *hmmc,
                                                      const MMC_DMALinkedListTypeDef *pLinkedList,
                                                      uint32_t BlockAdd, uint32_t NumberOfBlocks)
{
  SDMMC_DataInitTypeDef config;
  uint32_t errorstate;
  uint32_t DmaBase0_reg;
  uint32_t DmaBase1_reg;
  uint32_t add = BlockAdd;

  if (hmmc->State == HAL_MMC_STATE_READY)
  {
    if ((BlockAdd + NumberOfBlocks) > (hmmc->MmcCard.LogBlockNbr))
    {
      hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Check the case of 4kB blocks (field DATA SECTOR SIZE of extended CSD register) */
    if (((hmmc->Ext_CSD[(MMC_EXT_CSD_DATA_SEC_SIZE_INDEX / 4)] >> MMC_EXT_CSD_DATA_SEC_SIZE_POS) & 0x000000FFU) != 0x0U)
    {
      if ((NumberOfBlocks % 8U) != 0U)
      {
        /* The number of blocks should be a multiple of 8 sectors of 512 bytes = 4 KBytes */
        hmmc->ErrorCode |= HAL_MMC_ERROR_BLOCK_LEN_ERR;
        return HAL_ERROR;
      }

      if ((BlockAdd % 8U) != 0U)
      {
        /* The address should be aligned to 8 (corresponding to 4 KBytes blocks) */
        hmmc->ErrorCode |= HAL_MMC_ERROR_ADDR_MISALIGNED;
        return HAL_ERROR;
      }
    }

    hmmc->Instance->IDMABASER = (uint32_t) pLinkedList->pHeadNode->IDMABASER;
    hmmc->Instance->IDMABSIZE = (uint32_t) pLinkedList->pHeadNode->IDMABSIZE;

    hmmc->Instance->IDMABAR = (uint32_t)  pLinkedList->pHeadNode;
    hmmc->Instance->IDMALAR = (uint32_t)  SDMMC_IDMALAR_ABR | SDMMC_IDMALAR_ULS | SDMMC_IDMALAR_ULA |
                              sizeof(SDMMC_DMALinkNodeTypeDef) ; /* Initial configuration */

    DmaBase0_reg = hmmc->Instance->IDMABASER;
    DmaBase1_reg = hmmc->Instance->IDMABAR;

    if ((hmmc->Instance->IDMABSIZE == 0U) || (DmaBase0_reg == 0U) || (DmaBase1_reg == 0U))
    {
      hmmc->ErrorCode = HAL_MMC_ERROR_ADDR_OUT_OF_RANGE;
      return HAL_ERROR;
    }

    /* Initialize data control register */
    hmmc->Instance->DCTRL = 0;

    hmmc->ErrorCode = HAL_MMC_ERROR_NONE;

    hmmc->State = HAL_MMC_STATE_BUSY;

    if ((hmmc->MmcCard.CardType) != MMC_HIGH_CAPACITY_CARD)
    {
      add *= 512U;
    }

    /* Configure the MMC DPSM (Data Path State Machine) */
    config.DataTimeOut   = SDMMC_DATATIMEOUT;
    config.DataLength    = MMC_BLOCKSIZE * NumberOfBlocks;
    config.DataBlockSize = SDMMC_DATABLOCK_SIZE_512B;
    config.TransferDir   = SDMMC_TRANSFER_DIR_TO_CARD;
    config.TransferMode  = SDMMC_TRANSFER_MODE_BLOCK;
    config.DPSM          = SDMMC_DPSM_DISABLE;
    (void)SDMMC_ConfigData(hmmc->Instance, &config);

    __SDMMC_CMDTRANS_ENABLE(hmmc->Instance);

    hmmc->Instance->IDMACTRL = SDMMC_ENABLE_IDMA_DOUBLE_BUFF0;

    /* Write Blocks in DMA mode */
    hmmc->Context = (MMC_CONTEXT_WRITE_MULTIPLE_BLOCK | MMC_CONTEXT_DMA);

    /* Write Multi Block command */
    errorstate = SDMMC_CmdWriteMultiBlock(hmmc->Instance, add);
    if (errorstate != HAL_MMC_ERROR_NONE)
    {
      hmmc->State = HAL_MMC_STATE_READY;
      hmmc->ErrorCode |= errorstate;
      return HAL_ERROR;
    }

    __HAL_MMC_ENABLE_IT(hmmc, (SDMMC_IT_DCRCFAIL | SDMMC_IT_DTIMEOUT | SDMMC_IT_TXUNDERR | SDMMC_IT_DATAEND |
                               SDMMC_FLAG_IDMATE | SDMMC_FLAG_IDMABTC));

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

#endif /* HAL_MMC_MODULE_ENABLED */
#endif /* SDMMC1 || SDMMC2 */

/**
  * @}
  */

/**
  * @}
  */
