/**
  ******************************************************************************
  * @file    stm32f4xx_hal.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   This file contains all the functions prototypes for the HAL 
  *          module driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_HAL_H
#define __STM32F4xx_HAL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_conf.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup HAL
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/** @brief  Freeze/Unfreeze Peripherals in Debug mode 
  */
#define __HAL_FREEZE_TIM2_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM2_STOP))
#define __HAL_FREEZE_TIM3_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM3_STOP))
#define __HAL_FREEZE_TIM4_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM4_STOP))
#define __HAL_FREEZE_TIM5_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM5_STOP))
#define __HAL_FREEZE_TIM6_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM6_STOP))
#define __HAL_FREEZE_TIM7_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM7_STOP))
#define __HAL_FREEZE_TIM12_DBGMCU()          (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM12_STOP))
#define __HAL_FREEZE_TIM13_DBGMCU()          (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM13_STOP))
#define __HAL_FREEZE_TIM14_DBGMCU()          (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM14_STOP))
#define __HAL_FREEZE_RTC_DBGMCU()            (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_RTC_STOP))
#define __HAL_FREEZE_WWDG_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_WWDG_STOP))
#define __HAL_FREEZE_IWDG_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_IWDG_STOP))
#define __HAL_FREEZE_I2C1_TIMEOUT_DBGMCU()   (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_I2C1_SMBUS_TIMEOUT))
#define __HAL_FREEZE_I2C2_TIMEOUT_DBGMCU()   (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_I2C2_SMBUS_TIMEOUT))
#define __HAL_FREEZE_I2C3_TIMEOUT_DBGMCU()   (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_I2C3_SMBUS_TIMEOUT))
#define __HAL_FREEZE_CAN1_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_CAN1_STOP))
#define __HAL_FREEZE_CAN2_DBGMCU()           (DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_CAN2_STOP))
#define __HAL_FREEZE_TIM1_DBGMCU()           (DBGMCU->APB2FZ |= (DBGMCU_APB2_FZ_DBG_TIM1_STOP))
#define __HAL_FREEZE_TIM8_DBGMCU()           (DBGMCU->APB2FZ |= (DBGMCU_APB2_FZ_DBG_TIM8_STOP))
#define __HAL_FREEZE_TIM9_DBGMCU()           (DBGMCU->APB2FZ |= (DBGMCU_APB2_FZ_DBG_TIM9_STOP))
#define __HAL_FREEZE_TIM10_DBGMCU()          (DBGMCU->APB2FZ |= (DBGMCU_APB2_FZ_DBG_TIM10_STOP))
#define __HAL_FREEZE_TIM11_DBGMCU()          (DBGMCU->APB2FZ |= (DBGMCU_APB2_FZ_DBG_TIM11_STOP))

#define __HAL_UNFREEZE_TIM2_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_TIM2_STOP))
#define __HAL_UNFREEZE_TIM3_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_TIM3_STOP))
#define __HAL_UNFREEZE_TIM4_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_TIM4_STOP))
#define __HAL_UNFREEZE_TIM5_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_TIM5_STOP))
#define __HAL_UNFREEZE_TIM6_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_TIM6_STOP))
#define __HAL_UNFREEZE_TIM7_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_TIM7_STOP))
#define __HAL_UNFREEZE_TIM12_DBGMCU()          (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_TIM12_STOP))
#define __HAL_UNFREEZE_TIM13_DBGMCU()          (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_TIM13_STOP))
#define __HAL_UNFREEZE_TIM14_DBGMCU()          (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_TIM14_STOP))
#define __HAL_UNFREEZE_RTC_DBGMCU()            (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_RTC_STOP))
#define __HAL_UNFREEZE_WWDG_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_WWDG_STOP))
#define __HAL_UNFREEZE_IWDG_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_IWDG_STOP))
#define __HAL_UNFREEZE_I2C1_TIMEOUT_DBGMCU()   (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_I2C1_SMBUS_TIMEOUT))
#define __HAL_UNFREEZE_I2C2_TIMEOUT_DBGMCU()   (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_I2C2_SMBUS_TIMEOUT))
#define __HAL_UNFREEZE_I2C3_TIMEOUT_DBGMCU()   (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_I2C3_SMBUS_TIMEOUT))
#define __HAL_UNFREEZE_CAN1_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_CAN1_STOP))
#define __HAL_UNFREEZE_CAN2_DBGMCU()           (DBGMCU->APB1FZ &= ~(DBGMCU_APB1_FZ_DBG_CAN2_STOP))
#define __HAL_UNFREEZE_TIM1_DBGMCU()           (DBGMCU->APB2FZ &= ~(DBGMCU_APB2_FZ_DBG_TIM1_STOP))
#define __HAL_UNFREEZE_TIM8_DBGMCU()           (DBGMCU->APB2FZ &= ~(DBGMCU_APB2_FZ_DBG_TIM8_STOP))
#define __HAL_UNFREEZE_TIM9_DBGMCU()           (DBGMCU->APB2FZ &= ~(DBGMCU_APB2_FZ_DBG_TIM9_STOP))
#define __HAL_UNFREEZE_TIM10_DBGMCU()          (DBGMCU->APB2FZ &= ~(DBGMCU_APB2_FZ_DBG_TIM10_STOP))
#define __HAL_UNFREEZE_TIM11_DBGMCU()          (DBGMCU->APB2FZ &= ~(DBGMCU_APB2_FZ_DBG_TIM11_STOP))

/** @brief  Main Flash memory mapped at 0x00000000
  */
#define __HAL_REMAPMEMORY_FLASH()             (SYSCFG->MEMRMP &= ~(SYSCFG_MEMRMP_MEM_MODE))

/** @brief  System Flash memory mapped at 0x00000000
  */
#define __HAL_REMAPMEMORY_SYSTEMFLASH()       do {SYSCFG->MEMRMP &= ~(SYSCFG_MEMRMP_MEM_MODE);\
                                                  SYSCFG->MEMRMP |= SYSCFG_MEMRMP_MEM_MODE_0;\
                                                 }while(0);

/** @brief  Embedded SRAM mapped at 0x00000000
  */
#define __HAL_REMAPMEMORY_SRAM()       do {SYSCFG->MEMRMP &= ~(SYSCFG_MEMRMP_MEM_MODE);\
                                           SYSCFG->MEMRMP |= (SYSCFG_MEMRMP_MEM_MODE_0 | SYSCFG_MEMRMP_MEM_MODE_1);\
                                          }while(0);

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx)|| defined(STM32F417xx)
/** @brief  FSMC Bank1 (NOR/PSRAM 1 and 2) mapped at 0x00000000
  */
#define __HAL_REMAPMEMORY_FSMC()       do {SYSCFG->MEMRMP &= ~(SYSCFG_MEMRMP_MEM_MODE);\
                                           SYSCFG->MEMRMP |= (SYSCFG_MEMRMP_MEM_MODE_1);\
                                          }while(0);
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx */

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx) 
/** @brief  FMC Bank1 (NOR/PSRAM 1 and 2) mapped at 0x00000000
  */
#define __HAL_REMAPMEMORY_FMC()       do {SYSCFG->MEMRMP &= ~(SYSCFG_MEMRMP_MEM_MODE);\
                                          SYSCFG->MEMRMP |= (SYSCFG_MEMRMP_MEM_MODE_1);\
                                         }while(0);

/** @brief  FMC/SDRAM Bank 1 and 2 mapped at 0x00000000
  */
#define __HAL_REMAPMEMORY_FMC_SDRAM()       do {SYSCFG->MEMRMP &= ~(SYSCFG_MEMRMP_MEM_MODE);\
                                                SYSCFG->MEMRMP |= (SYSCFG_MEMRMP_MEM_MODE_2);\
                                               }while(0);
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */ 

/* Exported functions --------------------------------------------------------*/

/* Initialization and de-initialization functions  ******************************/
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority);

/* Peripheral Control functions  ************************************************/
void HAL_IncTick(void);
void HAL_Delay(__IO uint32_t Delay);
uint32_t HAL_GetTick(void);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);
void HAL_EnableDBGSleepMode(void);
void HAL_DisableDBGSleepMode(void);
void HAL_EnableDBGStopMode(void);
void HAL_DisableDBGStopMode(void);
void HAL_EnableDBGStandbyMode(void);
void HAL_DisableDBGStandbyMode(void);
void HAL_EnableCompensationCell(void);
void HAL_DisableCompensationCell(void);
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx)|| defined(STM32F439xx)
void HAL_EnableMemorySwappingBank(void);
void HAL_DisableMemorySwappingBank(void);
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */ 


/**
  * @}
  */ 

/**
  * @}
  */ 
  
#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
