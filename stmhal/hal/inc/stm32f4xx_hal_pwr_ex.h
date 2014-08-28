/**
  ******************************************************************************
  * @file    stm32f4xx_hal_pwr_ex.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of PWR HAL Extension module.
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
#ifndef __STM32F4xx_HAL_PWR_EX_H
#define __STM32F4xx_HAL_PWR_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup PWREx
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/ 
/* Exported constants --------------------------------------------------------*/
/* ------------- PWR registers bit address in the alias region ---------------*/
/* --- CR Register ---*/
/* Alias word address of FPDS bit */
#define FPDS_BitNumber  0x09
#define CR_FPDS_BB      (PERIPH_BB_BASE + (CR_OFFSET * 32) + (FPDS_BitNumber * 4))

/* Alias word address of ODEN bit   */
#define ODEN_BitNumber           0x10
#define CR_ODEN_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ODEN_BitNumber * 4))

/* Alias word address of ODSWEN bit */
#define ODSWEN_BitNumber         0x11
#define CR_ODSWEN_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ODSWEN_BitNumber * 4))
    
/* Alias word address of MRLVDS bit */
#define MRLVDS_BitNumber         0x0B
#define CR_MRLVDS_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (MRLVDS_BitNumber * 4))

/* Alias word address of LPLVDS bit */
#define LPLVDS_BitNumber         0x0A
#define CR_LPLVDS_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (LPLVDS_BitNumber * 4))

/* --- CSR Register ---*/
/* Alias word address of BRE bit */
#define BRE_BitNumber   0x09
#define CSR_BRE_BB      (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (BRE_BitNumber * 4))    

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
   
/** @defgroup PWREx_Regulator_state_in_UnderDrive_mode 
  * @{
  */
#define PWR_MAINREGULATOR_UNDERDRIVE_ON                       PWR_CR_MRUDS
#define PWR_LOWPOWERREGULATOR_UNDERDRIVE_ON                   ((uint32_t)(PWR_CR_LPDS | PWR_CR_LPUDS))

#define IS_PWR_REGULATOR_UNDERDRIVE(REGULATOR) (((REGULATOR) == PWR_MAINREGULATOR_UNDERDRIVE_ON) || \
                                                ((REGULATOR) == PWR_LOWPOWERREGULATOR_UNDERDRIVE_ON))
/**
  * @}
  */ 
  
/** @defgroup PWREx_Over_Under_Drive_Flag 
  * @{
  */
#define PWR_FLAG_ODRDY                  PWR_CSR_ODRDY
#define PWR_FLAG_ODSWRDY                PWR_CSR_ODSWRDY
#define PWR_FLAG_UDRDY                  PWR_CSR_UDSWRDY
/**
  * @}
  */
#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */
/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/
  
#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
/** @brief Macros to enable or disable the Over drive mode.
  * @note  These macros can be used only for STM32F42xx/STM3243xx devices.
  */
#define __HAL_PWR_OVERDRIVE_ENABLE() (*(__IO uint32_t *) CR_ODEN_BB = ENABLE)
#define __HAL_PWR_OVERDRIVE_DISABLE() (*(__IO uint32_t *) CR_ODEN_BB = DISABLE)

/** @brief Macros to enable or disable the Over drive switching.
  * @note  These macros can be used only for STM32F42xx/STM3243xx devices. 
  */
#define __HAL_PWR_OVERDRIVESWITCHING_ENABLE() (*(__IO uint32_t *) CR_ODSWEN_BB = ENABLE)
#define __HAL_PWR_OVERDRIVESWITCHING_DISABLE() (*(__IO uint32_t *) CR_ODSWEN_BB = DISABLE)

/** @brief Macros to enable or disable the Under drive mode.
  * @note  This mode is enabled only with STOP low power mode.
  *        In this mode, the 1.2V domain is preserved in reduced leakage mode. This 
  *        mode is only available when the main regulator or the low power regulator 
  *        is in low voltage mode.      
  * @note  If the Under-drive mode was enabled, it is automatically disabled after 
  *        exiting Stop mode. 
  *        When the voltage regulator operates in Under-drive mode, an additional  
  *        startup delay is induced when waking up from Stop mode.
  */
#define __HAL_PWR_UNDERDRIVE_ENABLE() (PWR->CR |= (uint32_t)PWR_CR_UDEN)
#define __HAL_PWR_UNDERDRIVE_DISABLE() (PWR->CR &= (uint32_t)(~PWR_CR_UDEN))

/** @brief  Check PWR flag is set or not.
  * @note   These macros can be used only for STM32F42xx/STM3243xx devices.
  * @param  __FLAG__: specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg PWR_FLAG_ODRDY: This flag indicates that the Over-drive mode
  *                                 is ready 
  *            @arg PWR_FLAG_ODSWRDY: This flag indicates that the Over-drive mode
  *                                   switching is ready  
  *            @arg PWR_FLAG_UDRDY: This flag indicates that the Under-drive mode
  *                                 is enabled in Stop mode
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_PWR_GET_ODRUDR_FLAG(__FLAG__) ((PWR->CSR & (__FLAG__)) == (__FLAG__))

/** @brief Clear the Under-Drive Ready flag.
  * @note  These macros can be used only for STM32F42xx/STM3243xx devices.
  */
#define __HAL_PWR_CLEAR_ODRUDR_FLAG() (PWR->CSR |= PWR_FLAG_UDRDY)

#endif /* STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx */

/* Exported functions --------------------------------------------------------*/
void              HAL_PWREx_EnableFlashPowerDown(void);
void              HAL_PWREx_DisableFlashPowerDown(void); 
HAL_StatusTypeDef HAL_PWREx_EnableBkUpReg(void);
HAL_StatusTypeDef HAL_PWREx_DisableBkUpReg(void); 

#if defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE)
void HAL_PWREx_EnableMainRegulatorLowVoltage(void);
void HAL_PWREx_DisableMainRegulatorLowVoltage(void);
void HAL_PWREx_EnableLowRegulatorLowVoltage(void);
void HAL_PWREx_DisableLowRegulatorLowVoltage(void);
#endif /* STM32F401xC || STM32F401xE || STM32F411xE */

#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
HAL_StatusTypeDef HAL_PWREx_ActivateOverDrive(void);
HAL_StatusTypeDef HAL_PWREx_DeactivateOverDrive(void);
HAL_StatusTypeDef HAL_PWREx_EnterUnderDriveSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
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


#endif /* __STM32F4xx_HAL_PWR_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
