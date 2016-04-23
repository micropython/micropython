/**
  ******************************************************************************
  * @file    stm32f2xx_dbgmcu.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    05-March-2012
  * @brief   This file contains all the functions prototypes for the DBGMCU firmware library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2xx_DBGMCU_H
#define __STM32F2xx_DBGMCU_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/** @addtogroup STM32F2xx_StdPeriph_Driver
  * @{
  */

/** @addtogroup DBGMCU
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup DBGMCU_Exported_Constants
  * @{
  */
#define DBGMCU_SLEEP                 ((uint32_t)0x00000001)
#define DBGMCU_STOP                  ((uint32_t)0x00000002)
#define DBGMCU_STANDBY               ((uint32_t)0x00000004)
#define IS_DBGMCU_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFFF8) == 0x00) && ((PERIPH) != 0x00))

#define DBGMCU_TIM2_STOP             ((uint32_t)0x00000001)
#define DBGMCU_TIM3_STOP             ((uint32_t)0x00000002)
#define DBGMCU_TIM4_STOP             ((uint32_t)0x00000004)
#define DBGMCU_TIM5_STOP             ((uint32_t)0x00000008)
#define DBGMCU_TIM6_STOP             ((uint32_t)0x00000010)
#define DBGMCU_TIM7_STOP             ((uint32_t)0x00000020)
#define DBGMCU_TIM12_STOP            ((uint32_t)0x00000040)
#define DBGMCU_TIM13_STOP            ((uint32_t)0x00000080)
#define DBGMCU_TIM14_STOP            ((uint32_t)0x00000100)
#define DBGMCU_RTC_STOP              ((uint32_t)0x00000400)
#define DBGMCU_WWDG_STOP             ((uint32_t)0x00000800)
#define DBGMCU_IWDG_STOP             ((uint32_t)0x00001000)
#define DBGMCU_I2C1_SMBUS_TIMEOUT    ((uint32_t)0x00200000)
#define DBGMCU_I2C2_SMBUS_TIMEOUT    ((uint32_t)0x00400000)
#define DBGMCU_I2C3_SMBUS_TIMEOUT    ((uint32_t)0x00800000)
#define DBGMCU_CAN1_STOP             ((uint32_t)0x02000000)
#define DBGMCU_CAN2_STOP             ((uint32_t)0x04000000)
#define IS_DBGMCU_APB1PERIPH(PERIPH) ((((PERIPH) & 0xF91FE200) == 0x00) && ((PERIPH) != 0x00))

#define DBGMCU_TIM1_STOP             ((uint32_t)0x00000001)
#define DBGMCU_TIM8_STOP             ((uint32_t)0x00000002)
#define DBGMCU_TIM9_STOP             ((uint32_t)0x00010000)
#define DBGMCU_TIM10_STOP            ((uint32_t)0x00020000)
#define DBGMCU_TIM11_STOP            ((uint32_t)0x00040000)
#define IS_DBGMCU_APB2PERIPH(PERIPH) ((((PERIPH) & 0xFFF8FFFC) == 0x00) && ((PERIPH) != 0x00))
/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
uint32_t DBGMCU_GetREVID(void);
uint32_t DBGMCU_GetDEVID(void);
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB1PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB2PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F2xx_DBGMCU_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
