/**
  ******************************************************************************
  * @file    stm32n6xx_hal_mmc_ex.h
  * @author  MCD Application Team
  * @brief   Header file of SD HAL extended module.
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
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32N6xx_HAL_MMC_EX_H
#define STM32N6xx_HAL_MMC_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined (SDMMC1) || defined (SDMMC2)
/** @addtogroup MMCEx
  * @brief SD HAL extended module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup MMCEx_Exported_Types MMCEx Exported Types
  * @{
  */

/** @defgroup MMCEx_Exported_Types_Group1 Linked List Wrapper
  * @{
  */
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* -----------------Linked List Wrapper --------------------------------------*/

#define MMC_DMALinkNodeTypeDef        SDMMC_DMALinkNodeTypeDef
#define MMC_DMALinkNodeConfTypeDef    SDMMC_DMALinkNodeConfTypeDef
#define MMC_DMALinkedListTypeDef      SDMMC_DMALinkedListTypeDef
/* ----------------- Linked Aliases ------------------------------------------*/
#define HAL_MMCx_DMALinkedList_WriteCpltCallback HAL_MMC_TxCpltCallback
#define HAL_MMCx_DMALinkedList_ReadCpltCallback  HAL_MMC_RxCpltCallback
/**
  * @}
  */

/**
  * @}
  */
/* Exported functions --------------------------------------------------------*/
/** @defgroup MMCEx_Exported_Functions MMCEx Exported Functions
  * @{
  */

/** @defgroup MMCEx_Exported_Functions_Group1 MultiBuffer functions
  * @{
  */
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_ReadBlocks(MMC_HandleTypeDef *hmmc,
                                                     const MMC_DMALinkedListTypeDef *pLinkedList,
                                                     uint32_t BlockAdd, uint32_t NumberOfBlocks);
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_WriteBlocks(MMC_HandleTypeDef *hmmc,
                                                      const MMC_DMALinkedListTypeDef *pLinkedList,
                                                      uint32_t BlockAdd, uint32_t NumberOfBlocks);

HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_BuildNode(MMC_DMALinkNodeTypeDef *pNode,
                                                    MMC_DMALinkNodeConfTypeDef *pNodeConf);
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_InsertNode(MMC_DMALinkedListTypeDef *pLinkedList,
                                                     MMC_DMALinkNodeTypeDef *pPrevNode,
                                                     MMC_DMALinkNodeTypeDef *pNewNode);
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_RemoveNode(MMC_DMALinkedListTypeDef *pLinkedList,
                                                     MMC_DMALinkNodeTypeDef *pNode);
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_LockNode(MMC_DMALinkNodeTypeDef *pNode);
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_UnlockNode(MMC_DMALinkNodeTypeDef *pNode);
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_EnableCircularMode(SDMMC_DMALinkedListTypeDef *pLinkedList);
HAL_StatusTypeDef HAL_MMCEx_DMALinkedList_DisableCircularMode(SDMMC_DMALinkedListTypeDef *pLinkedList);

void HAL_MMCEx_Read_DMALnkLstBufCpltCallback(MMC_HandleTypeDef *hmmc);
void HAL_MMCEx_Write_DMALnkLstBufCpltCallback(MMC_HandleTypeDef *hmmc);


/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions prototypes ----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @}
  */
#endif /* SDMMC1 || SDMMC2 */

/**
  * @}
  */
#ifdef __cplusplus
}
#endif


#endif /* STM32N6xx_HAL_MMCEx_H */
