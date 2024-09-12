/**
  ******************************************************************************
  * @file    stm32n6xx_hal_sd_ex.h
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
#ifndef STM32N6xx_HAL_SD_EX_H
#define STM32N6xx_HAL_SD_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */
#if defined (SDMMC1) || defined (SDMMC2)

/** @addtogroup SDEx
  * @brief SD HAL extended module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SDEx_Exported_Types SDEx Exported Types
  * @{
  */

/** @defgroup SDEx_Exported_Types_Group1 Linked List Wrapper
  * @{
  */
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* -----------------Linked List Wrapper --------------------------------------*/

#define SD_DMALinkNodeTypeDef        SDMMC_DMALinkNodeTypeDef
#define SD_DMALinkNodeConfTypeDef    SDMMC_DMALinkNodeConfTypeDef
#define SD_DMALinkedListTypeDef      SDMMC_DMALinkedListTypeDef
/* ----------------- Linked Aliases ------------------------------------------*/
#define HAL_SDEx_DMALinkedList_WriteCpltCallback HAL_SD_TxCpltCallback
#define HAL_SDEx_DMALinkedList_ReadCpltCallback  HAL_SD_RxCpltCallback
/**
  * @}
  */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SDEx_Exported_Functions SDEx Exported Functions
  * @{
  */
/** @defgroup SDEx_Exported_Functions_Group1 Linked List functions
  * @{
  */
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_ReadBlocks(SD_HandleTypeDef *hsd, const SD_DMALinkedListTypeDef *pLinkedList,
                                                    uint32_t BlockAdd, uint32_t NumberOfBlocks);
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_WriteBlocks(SD_HandleTypeDef *hsd, const SD_DMALinkedListTypeDef *pLinkedList,
                                                     uint32_t BlockAdd, uint32_t NumberOfBlocks);

HAL_StatusTypeDef HAL_SDEx_DMALinkedList_BuildNode(SD_DMALinkNodeTypeDef *pNode, SD_DMALinkNodeConfTypeDef *pNodeConf);
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_InsertNode(SD_DMALinkedListTypeDef *pLinkedList,
                                                    SD_DMALinkNodeTypeDef *pPrevNode, SD_DMALinkNodeTypeDef *pNewNode);
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_RemoveNode(SD_DMALinkedListTypeDef *pLinkedList, SD_DMALinkNodeTypeDef *pNode);
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_LockNode(SD_DMALinkNodeTypeDef *pNode);
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_UnlockNode(SD_DMALinkNodeTypeDef *pNode);
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_EnableCircularMode(SD_DMALinkedListTypeDef *pLinkedList);
HAL_StatusTypeDef HAL_SDEx_DMALinkedList_DisableCircularMode(SD_DMALinkedListTypeDef *pLinkedList);

void HAL_SDEx_Read_DMALnkLstBufCpltCallback(SD_HandleTypeDef *hsd);
void HAL_SDEx_Write_DMALnkLstBufCpltCallback(SD_HandleTypeDef *hsd);


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


#endif /* stm32n6xx_HAL_SD_EX_H */
