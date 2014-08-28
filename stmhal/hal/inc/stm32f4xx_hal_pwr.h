/**
  ******************************************************************************
  * @file    stm32f4xx_hal_pwr.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-June-2014
  * @brief   Header file of PWR HAL module.
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
#ifndef __STM32F4xx_HAL_PWR_H
#define __STM32F4xx_HAL_PWR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_def.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @addtogroup PWR
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  PWR PVD configuration structure definition
  */
typedef struct
{
  uint32_t PVDLevel;   /*!< PVDLevel: Specifies the PVD detection level.
                            This parameter can be a value of @ref PWR_PVD_detection_level */

  uint32_t Mode;      /*!< Mode: Specifies the operating mode for the selected pins.
                           This parameter can be a value of @ref PWR_PVD_Mode */
}PWR_PVDTypeDef;

/* Exported constants --------------------------------------------------------*/
/* ------------- PWR registers bit address in the alias region ---------------*/
#define PWR_OFFSET      (PWR_BASE - PERIPH_BASE)

/* --- CR Register ---*/
/* Alias word address of DBP bit */
#define CR_OFFSET       (PWR_OFFSET + 0x00)
#define DBP_BitNumber   0x08
#define CR_DBP_BB       (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))

/* Alias word address of PVDE bit */
#define PVDE_BitNumber  0x04
#define CR_PVDE_BB      (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PVDE_BitNumber * 4))

/* Alias word address of PMODE bit */
#define PMODE_BitNumber 0x0E
#define CR_PMODE_BB     (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PMODE_BitNumber * 4))

/* --- CSR Register ---*/
/* Alias word address of EWUP bit */
#define CSR_OFFSET      (PWR_OFFSET + 0x04)
#define EWUP_BitNumber  0x08
#define CSR_EWUP_BB     (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP_BitNumber * 4))
   
/** @defgroup PWR_Exported_Constants
  * @{
  */
 
/** @defgroup PWR_WakeUp_Pins
  * @{
  */

#define PWR_WAKEUP_PIN1                 PWR_CSR_EWUP
#define IS_PWR_WAKEUP_PIN(PIN) ((PIN) == PWR_WAKEUP_PIN1)
/**
  * @}
  */

/** @defgroup PWR_PVD_detection_level 
  * @{
  */ 
#define PWR_PVDLEVEL_0                  PWR_CR_PLS_LEV0
#define PWR_PVDLEVEL_1                  PWR_CR_PLS_LEV1
#define PWR_PVDLEVEL_2                  PWR_CR_PLS_LEV2
#define PWR_PVDLEVEL_3                  PWR_CR_PLS_LEV3
#define PWR_PVDLEVEL_4                  PWR_CR_PLS_LEV4
#define PWR_PVDLEVEL_5                  PWR_CR_PLS_LEV5
#define PWR_PVDLEVEL_6                  PWR_CR_PLS_LEV6
#define PWR_PVDLEVEL_7                  PWR_CR_PLS_LEV7
#define IS_PWR_PVD_LEVEL(LEVEL) (((LEVEL) == PWR_PVDLEVEL_0) || ((LEVEL) == PWR_PVDLEVEL_1)|| \
                                 ((LEVEL) == PWR_PVDLEVEL_2) || ((LEVEL) == PWR_PVDLEVEL_3)|| \
                                 ((LEVEL) == PWR_PVDLEVEL_4) || ((LEVEL) == PWR_PVDLEVEL_5)|| \
                                 ((LEVEL) == PWR_PVDLEVEL_6) || ((LEVEL) == PWR_PVDLEVEL_7))
/**
  * @}
  */   
 
/** @defgroup PWR_PVD_Mode 
  * @{
  */
#define  PWR_MODE_EVT                  ((uint32_t)0x00000000)   /*!< No Interrupt */
#define  PWR_MODE_IT_RISING            ((uint32_t)0x00000001)   /*!< External Interrupt Mode with Rising edge trigger detection */
#define  PWR_MODE_IT_FALLING           ((uint32_t)0x00000002)   /*!< External Interrupt Mode with Falling edge trigger detection */
#define  PWR_MODE_IT_RISING_FALLING    ((uint32_t)0x00000003)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection */
#define IS_PWR_PVD_MODE(MODE) (((MODE) == PWR_MODE_EVT) || ((MODE) == PWR_MODE_IT_RISING)|| \
                               ((MODE) == PWR_MODE_IT_FALLING) || ((MODE) == PWR_MODE_IT_RISING_FALLING))
/**
  * @}
  */ 

/** @defgroup PWR_Regulator_state_in_STOP_mode 
  * @{
  */
#define PWR_MAINREGULATOR_ON                        ((uint32_t)0x00000000)
#define PWR_LOWPOWERREGULATOR_ON                    PWR_CR_LPDS

#define IS_PWR_REGULATOR(REGULATOR) (((REGULATOR) == PWR_MAINREGULATOR_ON) || \
                                     ((REGULATOR) == PWR_LOWPOWERREGULATOR_ON))
/**
  * @}
  */
    
/** @defgroup PWR_SLEEP_mode_entry 
  * @{
  */
#define PWR_SLEEPENTRY_WFI              ((uint8_t)0x01)
#define PWR_SLEEPENTRY_WFE              ((uint8_t)0x02)
#define IS_PWR_SLEEP_ENTRY(ENTRY) (((ENTRY) == PWR_SLEEPENTRY_WFI) || ((ENTRY) == PWR_SLEEPENTRY_WFE))
/**
  * @}
  */

/** @defgroup PWR_STOP_mode_entry 
  * @{
  */
#define PWR_STOPENTRY_WFI               ((uint8_t)0x01)
#define PWR_STOPENTRY_WFE               ((uint8_t)0x02)
#define IS_PWR_STOP_ENTRY(ENTRY) (((ENTRY) == PWR_STOPENTRY_WFI) || ((ENTRY) == PWR_STOPENTRY_WFE))
/**
  * @}
  */

/** @defgroup PWR_Regulator_Voltage_Scale 
  * @{
  */
#define PWR_REGULATOR_VOLTAGE_SCALE1    ((uint32_t)0x0000C000)
#define PWR_REGULATOR_VOLTAGE_SCALE2    ((uint32_t)0x00008000)
#define PWR_REGULATOR_VOLTAGE_SCALE3    ((uint32_t)0x00004000)
#define IS_PWR_REGULATOR_VOLTAGE(VOLTAGE) (((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE1) || \
                                           ((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE2) || \
                                           ((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE3))
/**
  * @}
  */

/** @defgroup PWR_Flag 
  * @{
  */
#define PWR_FLAG_WU                     PWR_CSR_WUF
#define PWR_FLAG_SB                     PWR_CSR_SBF
#define PWR_FLAG_PVDO                   PWR_CSR_PVDO
#define PWR_FLAG_BRR                    PWR_CSR_BRR
#define PWR_FLAG_VOSRDY                 PWR_CSR_VOSRDY

/**
  * @}
  */

/**
  * @}
  */ 
  
/* Exported macro ------------------------------------------------------------*/

/** @brief  macros configure the main internal regulator output voltage.
  * @param  __REGULATOR__: specifies the regulator output voltage to achieve
  *         a tradeoff between performance and power consumption when the device does
  *         not operate at the maximum frequency (refer to the datasheets for more details).
  *          This parameter can be one of the following values:
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE1: Regulator voltage output Scale 1 mode
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE2: Regulator voltage output Scale 2 mode
  *            @arg PWR_REGULATOR_VOLTAGE_SCALE3: Regulator voltage output Scale 3 mode
  * @retval None
  */
#define __HAL_PWR_VOLTAGESCALING_CONFIG(__REGULATOR__) (MODIFY_REG(PWR->CR, PWR_CR_VOS, (__REGULATOR__)))

/** @brief  Check PWR flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *           This parameter can be one of the following values:
  *            @arg PWR_FLAG_WU: Wake Up flag. This flag indicates that a wakeup event 
  *                  was received from the WKUP pin or from the RTC alarm (Alarm A 
  *                  or Alarm B), RTC Tamper event, RTC TimeStamp event or RTC Wakeup.
  *                  An additional wakeup event is detected if the WKUP pin is enabled 
  *                  (by setting the EWUP bit) when the WKUP pin level is already high.  
  *            @arg PWR_FLAG_SB: StandBy flag. This flag indicates that the system was
  *                  resumed from StandBy mode.    
  *            @arg PWR_FLAG_PVDO: PVD Output. This flag is valid only if PVD is enabled 
  *                  by the HAL_PWR_EnablePVD() function. The PVD is stopped by Standby mode 
  *                  For this reason, this bit is equal to 0 after Standby or reset
  *                  until the PVDE bit is set.
  *            @arg PWR_FLAG_BRR: Backup regulator ready flag. This bit is not reset 
  *                  when the device wakes up from Standby mode or by a system reset 
  *                  or power reset.  
  *            @arg PWR_FLAG_VOSRDY: This flag indicates that the Regulator voltage 
  *                 scaling output selection is ready.
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define __HAL_PWR_GET_FLAG(__FLAG__) ((PWR->CSR & (__FLAG__)) == (__FLAG__))

/** @brief  Clear the PWR's pending flags.
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be one of the following values:
  *            @arg PWR_FLAG_WU: Wake Up flag
  *            @arg PWR_FLAG_SB: StandBy flag
  */
#define __HAL_PWR_CLEAR_FLAG(__FLAG__) (PWR->CR |=  (__FLAG__) << 2)

#define PWR_EXTI_LINE_PVD  ((uint32_t)0x00010000)  /*!< External interrupt line 16 Connected to the PVD EXTI Line */
/**
  * @brief Enable the PVD Exti Line.
  * @param  __EXTILINE__: specifies the PVD Exti sources to be enabled.
  * This parameter can be:
  *   @arg PWR_EXTI_LINE_PVD     
  * @retval None.
  */
#define __HAL_PVD_EXTI_ENABLE_IT(__EXTILINE__)   (EXTI->IMR |= (__EXTILINE__))

/**
  * @brief Disable the PVD EXTI Line.
  * @param  __EXTILINE__: specifies the PVD EXTI sources to be disabled.
  * This parameter can be:
  *  @arg PWR_EXTI_LINE_PVD    
  * @retval None.
  */
#define __HAL_PVD_EXTI_DISABLE_IT(__EXTILINE__)  (EXTI->IMR &= ~(__EXTILINE__))

/**
  * @brief checks whether the specified PVD Exti interrupt flag is set or not.
  * @param  __EXTILINE__: specifies the PVD Exti sources to be cleared.
  * This parameter can be:
  *   @arg PWR_EXTI_LINE_PVD  
  * @retval EXTI PVD Line Status.
  */
#define __HAL_PVD_EXTI_GET_FLAG(__EXTILINE__)  (EXTI->PR & (__EXTILINE__))

/**
  * @brief Clear the PVD Exti flag.
  * @param  __EXTILINE__: specifies the PVD Exti sources to be cleared.
  * This parameter can be:
  *   @arg PWR_EXTI_LINE_PVD  
  * @retval None.
  */
#define __HAL_PVD_EXTI_CLEAR_FLAG(__EXTILINE__)  (EXTI->PR = (__EXTILINE__))

/**
  * @brief  Generates a Software interrupt on selected EXTI line.
  * @param  __EXTILINE__: specifies the PVD EXTI sources to be disabled.
  * This parameter can be:
  *  @arg PWR_EXTI_LINE_PVD
  * @retval None
  */
#define __HAL_PVD_EXTI_GENERATE_SWIT(__EXTI_LINE__) (EXTI->SWIER |= (__EXTI_LINE__))

/* Include PWR HAL Extension module */
#include "stm32f4xx_hal_pwr_ex.h"

/* Exported functions --------------------------------------------------------*/

/* Initialization and de-initialization functions *****************************/
void HAL_PWR_DeInit(void);
void HAL_PWR_EnableBkUpAccess(void);
void HAL_PWR_DisableBkUpAccess(void);

/* Peripheral Control functions  **********************************************/
/* PVD configuration */
void HAL_PWR_PVDConfig(PWR_PVDTypeDef *sConfigPVD);
void HAL_PWR_EnablePVD(void);
void HAL_PWR_DisablePVD(void);

/* WakeUp pins configuration */
void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinx);
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);

/* Low Power modes entry */
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry);
void HAL_PWR_EnterSTANDBYMode(void);

void HAL_PWR_PVD_IRQHandler(void);
void HAL_PWR_PVDCallback(void);


/**
  * @}
  */ 

/**
  * @}
  */
  
#ifdef __cplusplus
}
#endif


#endif /* __STM32F4xx_HAL_PWR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
