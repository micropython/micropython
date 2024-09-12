/**
  ******************************************************************************
  * @file    stm32n6xx_hal_iwdg.h
  * @author  MCD Application Team
  * @brief   Header file of IWDG HAL module.
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
#ifndef STM32N6xx_HAL_IWDG_H
#define STM32N6xx_HAL_IWDG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"

/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @defgroup IWDG IWDG
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup IWDG_Exported_Types IWDG Exported Types
  * @{
  */

/**
  * @brief  IWDG Init structure definition
  */
typedef struct
{
  uint32_t Prescaler;  /*!< Select the prescaler of the IWDG.
                            This parameter can be a value of @ref IWDG_Prescaler */

  uint32_t Reload;     /*!< Specifies the IWDG down-counter reload value.
                            This parameter must be a number between Min_Data = 0 and Max_Data = 0x0FFF */

  uint32_t Window;     /*!< Specifies the window value to be compared to the down-counter.
                            This parameter must be a number between Min_Data = 0 and Max_Data = 0x0FFF */

  uint32_t EWI;        /*!< Specifies if IWDG Early Wakeup Interrupt is enable or not and the comparator value.
                            This parameter must be a number between Min_Data = 0 and Max_Data = 0x0FFF
                            value 0 means that EWI is disabled */
} IWDG_InitTypeDef;

/**
  * @brief  IWDG Handle Structure definition
  */
#if (USE_HAL_IWDG_REGISTER_CALLBACKS == 1)
typedef struct __IWDG_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_IWDG_REGISTER_CALLBACKS */
{
  IWDG_TypeDef                 *Instance;  /*!< Register base address    */

  IWDG_InitTypeDef             Init;       /*!< IWDG required parameters */

#if (USE_HAL_IWDG_REGISTER_CALLBACKS == 1)
  void (* EwiCallback)(struct __IWDG_HandleTypeDef *hiwdg);                  /*!< IWDG Early WakeUp Interrupt callback */
  void (* MspInitCallback)(struct __IWDG_HandleTypeDef *hiwdg);              /*!< IWDG Msp Init callback */
#endif /* USE_HAL_IWDG_REGISTER_CALLBACKS */
} IWDG_HandleTypeDef;

#if (USE_HAL_IWDG_REGISTER_CALLBACKS == 1)
/**
  * @brief  HAL IWDG common Callback ID enumeration definition
  */
typedef enum
{
  HAL_IWDG_EWI_CB_ID          = 0x00U,    /*!< IWDG EWI callback ID */
  HAL_IWDG_MSPINIT_CB_ID      = 0x01U,    /*!< IWDG MspInit callback ID */
} HAL_IWDG_CallbackIDTypeDef;

/**
  * @brief  HAL IWDG Callback pointer definition
  */
typedef void (*pIWDG_CallbackTypeDef)(IWDG_HandleTypeDef *hppp);  /*!< pointer to a IWDG common callback functions */
#endif /* USE_HAL_IWDG_REGISTER_CALLBACKS */


/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup IWDG_Exported_Constants IWDG Exported Constants
  * @{
  */

/** @defgroup IWDG_Prescaler IWDG Prescaler
  * @{
  */
#define IWDG_PRESCALER_4                0x00000000u                                     /*!< IWDG prescaler set to 4   */
#define IWDG_PRESCALER_8                IWDG_PR_PR_0                                    /*!< IWDG prescaler set to 8   */
#define IWDG_PRESCALER_16               IWDG_PR_PR_1                                    /*!< IWDG prescaler set to 16  */
#define IWDG_PRESCALER_32               (IWDG_PR_PR_1 | IWDG_PR_PR_0)                   /*!< IWDG prescaler set to 32  */
#define IWDG_PRESCALER_64               IWDG_PR_PR_2                                    /*!< IWDG prescaler set to 64  */
#define IWDG_PRESCALER_128              (IWDG_PR_PR_2 | IWDG_PR_PR_0)                   /*!< IWDG prescaler set to 128 */
#define IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1)                   /*!< IWDG prescaler set to 256 */
#define IWDG_PRESCALER_512              (IWDG_PR_PR_2 | IWDG_PR_PR_1 | IWDG_PR_PR_0)    /*!< IWDG prescaler set to 512 */
#define IWDG_PRESCALER_1024             IWDG_PR_PR_3                                    /*!< IWDG prescaler set to 1024 */
/**
  * @}
  */

/** @defgroup IWDG_Window_option IWDG Window option
  * @{
  */
#define IWDG_WINDOW_DISABLE             IWDG_WINR_WIN
/**
  * @}
  */

/** @defgroup IWDG_EWI_Mode IWDG Early Wakeup Interrupt Mode
  * @{
  */
#define IWDG_EWI_DISABLE                0x00000000u       /*!< EWI Disable */
/**
  * @}
  */

/** @defgroup IWDG_Active_Status IWDG Active Status
  * @{
  */
#define IWDG_STATUS_DISABLE             0x00000000u
#define IWDG_STATUS_ENABLE              IWDG_SR_ONF
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup IWDG_Exported_Macros IWDG Exported Macros
  * @{
  */

/**
  * @brief  Enable the IWDG peripheral.
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
#define __HAL_IWDG_START(__HANDLE__)                WRITE_REG((__HANDLE__)->Instance->KR, IWDG_KEY_ENABLE)

/**
  * @brief  Reload IWDG counter with value defined in the reload register
  *         (write access to IWDG_PR, IWDG_RLR, IWDG_WINR and EWCR registers disabled).
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
#define __HAL_IWDG_RELOAD_COUNTER(__HANDLE__)       WRITE_REG((__HANDLE__)->Instance->KR, IWDG_KEY_RELOAD)

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup IWDG_Exported_Functions  IWDG Exported Functions
  * @{
  */

/** @defgroup IWDG_Exported_Functions_Group1 Initialization and Start functions
  * @{
  */
/* Initialization/Start functions  ********************************************/
HAL_StatusTypeDef     HAL_IWDG_Init(IWDG_HandleTypeDef *hiwdg);
void                  HAL_IWDG_MspInit(IWDG_HandleTypeDef *hiwdg);
/* Callbacks Register/UnRegister functions  ***********************************/
#if (USE_HAL_IWDG_REGISTER_CALLBACKS == 1)
HAL_StatusTypeDef     HAL_IWDG_RegisterCallback(IWDG_HandleTypeDef *hiwdg, HAL_IWDG_CallbackIDTypeDef CallbackID,
                                                pIWDG_CallbackTypeDef pCallback);
HAL_StatusTypeDef     HAL_IWDG_UnRegisterCallback(IWDG_HandleTypeDef *hiwdg, HAL_IWDG_CallbackIDTypeDef CallbackID);
#endif /* USE_HAL_IWDG_REGISTER_CALLBACKS */
/**
  * @}
  */

/** @defgroup IWDG_Exported_Functions_Group2 IO operation functions
  * @{
  */
/* I/O operation functions ****************************************************/
HAL_StatusTypeDef     HAL_IWDG_Refresh(IWDG_HandleTypeDef *hiwdg);
uint32_t              HAL_IWDG_GetActiveStatus(const IWDG_HandleTypeDef *hiwdg);
void                  HAL_IWDG_IRQHandler(IWDG_HandleTypeDef *hiwdg);
void                  HAL_IWDG_EarlyWakeupCallback(IWDG_HandleTypeDef *hiwdg);
/**
  * @}
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/
/** @defgroup IWDG_Private_Constants IWDG Private Constants
  * @{
  */

/**
  * @brief  IWDG Key Register BitMask
  */
#define IWDG_KEY_RELOAD                 0x0000AAAAu  /*!< IWDG Reload Counter Enable   */
#define IWDG_KEY_ENABLE                 0x0000CCCCu  /*!< IWDG Peripheral Enable       */
#define IWDG_KEY_WRITE_ACCESS_ENABLE    0x00005555u  /*!< IWDG KR Write Access Enable  */
#define IWDG_KEY_WRITE_ACCESS_DISABLE   0x00000000u  /*!< IWDG KR Write Access Disable */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup IWDG_Private_Macros IWDG Private Macros
  * @{
  */

/**
  * @brief  Enable write access to IWDG_PR, IWDG_RLR, IWDG_WINR and EWCR registers.
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
#define IWDG_ENABLE_WRITE_ACCESS(__HANDLE__)  WRITE_REG((__HANDLE__)->Instance->KR, IWDG_KEY_WRITE_ACCESS_ENABLE)

/**
  * @brief  Disable write access to IWDG_PR, IWDG_RLR, IWDG_WINR and EWCR registers.
  * @param  __HANDLE__  IWDG handle
  * @retval None
  */
#define IWDG_DISABLE_WRITE_ACCESS(__HANDLE__) WRITE_REG((__HANDLE__)->Instance->KR, IWDG_KEY_WRITE_ACCESS_DISABLE)

/**
  * @brief  Check IWDG prescaler value.
  * @param  __PRESCALER__  IWDG prescaler value
  * @retval None
  */
#define IS_IWDG_PRESCALER(__PRESCALER__)      (((__PRESCALER__) == IWDG_PRESCALER_4)  || \
                                               ((__PRESCALER__) == IWDG_PRESCALER_8)  || \
                                               ((__PRESCALER__) == IWDG_PRESCALER_16) || \
                                               ((__PRESCALER__) == IWDG_PRESCALER_32) || \
                                               ((__PRESCALER__) == IWDG_PRESCALER_64) || \
                                               ((__PRESCALER__) == IWDG_PRESCALER_128)|| \
                                               ((__PRESCALER__) == IWDG_PRESCALER_256)|| \
                                               ((__PRESCALER__) == IWDG_PRESCALER_512)|| \
                                               ((__PRESCALER__) == IWDG_PRESCALER_1024))

/**
  * @brief  Check IWDG reload value.
  * @param  __RELOAD__  IWDG reload value
  * @retval None
  */
#define IS_IWDG_RELOAD(__RELOAD__)            ((__RELOAD__) <= IWDG_RLR_RL)

/**
  * @brief  Check IWDG window value.
  * @param  __WINDOW__  IWDG window value
  * @retval None
  */
#define IS_IWDG_WINDOW(__WINDOW__)            ((__WINDOW__) <= IWDG_WINR_WIN)

/**
  * @brief  Check IWDG ewi value.
  * @param  __EWI__  IWDG ewi value
  * @retval None
  */
#define IS_IWDG_EWI(__EWI__)            ((__EWI__) <= IWDG_EWCR_EWIT)

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* STM32N6xx_HAL_IWDG_H */
