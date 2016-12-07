/**
  ******************************************************************************
  * @file    stm32f7xx_hal_pwr.h
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    23-September-2016
  * @brief   Header file of PWR HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#ifndef __STM32F7xx_HAL_PWR_H
#define __STM32F7xx_HAL_PWR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup PWR
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup PWR_Exported_Types PWR Exported Types
  * @{
  */

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

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup PWR_Exported_Constants PWR Exported Constants
  * @{
  */

/** @defgroup PWR_PVD_detection_level PWR PVD detection level
  * @{
  */
#define PWR_PVDLEVEL_0                  PWR_CR1_PLS_LEV0
#define PWR_PVDLEVEL_1                  PWR_CR1_PLS_LEV1
#define PWR_PVDLEVEL_2                  PWR_CR1_PLS_LEV2
#define PWR_PVDLEVEL_3                  PWR_CR1_PLS_LEV3
#define PWR_PVDLEVEL_4                  PWR_CR1_PLS_LEV4
#define PWR_PVDLEVEL_5                  PWR_CR1_PLS_LEV5
#define PWR_PVDLEVEL_6                  PWR_CR1_PLS_LEV6
#define PWR_PVDLEVEL_7                  PWR_CR1_PLS_LEV7/* External input analog voltage
                                                          (Compare internally to VREFINT) */

/**
  * @}
  */

/** @defgroup PWR_PVD_Mode PWR PVD Mode
  * @{
  */
#define PWR_PVD_MODE_NORMAL                 ((uint32_t)0x00000000U)   /*!< basic mode is used */
#define PWR_PVD_MODE_IT_RISING              ((uint32_t)0x00010001U)   /*!< External Interrupt Mode with Rising edge trigger detection */
#define PWR_PVD_MODE_IT_FALLING             ((uint32_t)0x00010002U)   /*!< External Interrupt Mode with Falling edge trigger detection */
#define PWR_PVD_MODE_IT_RISING_FALLING      ((uint32_t)0x00010003U)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection */
#define PWR_PVD_MODE_EVENT_RISING           ((uint32_t)0x00020001U)   /*!< Event Mode with Rising edge trigger detection */
#define PWR_PVD_MODE_EVENT_FALLING          ((uint32_t)0x00020002U)   /*!< Event Mode with Falling edge trigger detection */
#define PWR_PVD_MODE_EVENT_RISING_FALLING   ((uint32_t)0x00020003U)   /*!< Event Mode with Rising/Falling edge trigger detection */
/**
  * @}
  */

/** @defgroup PWR_Regulator_state_in_STOP_mode PWR Regulator state in SLEEP/STOP mode
  * @{
  */
#define PWR_MAINREGULATOR_ON                        ((uint32_t)0x00000000U)
#define PWR_LOWPOWERREGULATOR_ON                    PWR_CR1_LPDS
/**
  * @}
  */

/** @defgroup PWR_SLEEP_mode_entry PWR SLEEP mode entry
  * @{
  */
#define PWR_SLEEPENTRY_WFI              ((uint8_t)0x01U)
#define PWR_SLEEPENTRY_WFE              ((uint8_t)0x02U)
/**
  * @}
  */

/** @defgroup PWR_STOP_mode_entry PWR STOP mode entry
  * @{
  */
#define PWR_STOPENTRY_WFI               ((uint8_t)0x01U)
#define PWR_STOPENTRY_WFE               ((uint8_t)0x02U)
/**
  * @}
  */

/** @defgroup PWR_Regulator_Voltage_Scale PWR Regulator Voltage Scale
  * @{
  */
#define PWR_REGULATOR_VOLTAGE_SCALE1         PWR_CR1_VOS
#define PWR_REGULATOR_VOLTAGE_SCALE2         PWR_CR1_VOS_1
#define PWR_REGULATOR_VOLTAGE_SCALE3         PWR_CR1_VOS_0
/**
  * @}
  */

/** @defgroup PWR_Flag PWR Flag
  * @{
  */
#define PWR_FLAG_WU                     PWR_CSR1_WUIF
#define PWR_FLAG_SB                     PWR_CSR1_SBF
#define PWR_FLAG_PVDO                   PWR_CSR1_PVDO
#define PWR_FLAG_BRR                    PWR_CSR1_BRR
#define PWR_FLAG_VOSRDY                 PWR_CSR1_VOSRDY
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup PWR_Exported_Macro PWR Exported Macro
  * @{
  */

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
#define __HAL_PWR_VOLTAGESCALING_CONFIG(__REGULATOR__) do {                                                     \
                                                            __IO uint32_t tmpreg;                               \
                                                            MODIFY_REG(PWR->CR1, PWR_CR1_VOS, (__REGULATOR__)); \
                                                            /* Delay after an RCC peripheral clock enabling */  \
                                                            tmpreg = READ_BIT(PWR->CR1, PWR_CR1_VOS);           \
                                                            UNUSED(tmpreg);                                     \
				                                                	} while(0)

/** @brief  Check PWR flag is set or not.
  * @param  __FLAG__: specifies the flag to check.
  *           This parameter can be one of the following values:
  *            @arg PWR_FLAG_WU: Wake Up flag. This flag indicates that a wakeup event
  *                  was received on the internal wakeup line in standby mode (RTC alarm (Alarm A or Alarm B),
  *                  RTC Tamper event, RTC TimeStamp event or RTC Wakeup)).
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
#define __HAL_PWR_GET_FLAG(__FLAG__) ((PWR->CSR1 & (__FLAG__)) == (__FLAG__))

/** @brief  Clear the PWR's pending flags.
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be one of the following values:
  *            @arg PWR_FLAG_SB: StandBy flag
  */
#define __HAL_PWR_CLEAR_FLAG(__FLAG__) (PWR->CR1 |=  (__FLAG__) << 2)

/**
  * @brief Enable the PVD Exti Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_IT()   (EXTI->IMR |= (PWR_EXTI_LINE_PVD))

/**
  * @brief Disable the PVD EXTI Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_IT()  (EXTI->IMR &= ~(PWR_EXTI_LINE_PVD))

/**
  * @brief Enable event on PVD Exti Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_EVENT()   (EXTI->EMR |= (PWR_EXTI_LINE_PVD))

/**
  * @brief Disable event on PVD Exti Line 16.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_EVENT()  (EXTI->EMR &= ~(PWR_EXTI_LINE_PVD))

/**
  * @brief Enable the PVD Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_RISING_EDGE()   SET_BIT(EXTI->RTSR, PWR_EXTI_LINE_PVD)

/**
  * @brief Disable the PVD Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_RISING_EDGE()  CLEAR_BIT(EXTI->RTSR, PWR_EXTI_LINE_PVD)

/**
  * @brief Enable the PVD Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_FALLING_EDGE()   SET_BIT(EXTI->FTSR, PWR_EXTI_LINE_PVD)


/**
  * @brief Disable the PVD Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_FALLING_EDGE()  CLEAR_BIT(EXTI->FTSR, PWR_EXTI_LINE_PVD)


/**
  * @brief  PVD EXTI line configuration: set rising & falling edge trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_ENABLE_RISING_FALLING_EDGE()   __HAL_PWR_PVD_EXTI_ENABLE_RISING_EDGE();__HAL_PWR_PVD_EXTI_ENABLE_FALLING_EDGE();

/**
  * @brief Disable the PVD Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_DISABLE_RISING_FALLING_EDGE()  __HAL_PWR_PVD_EXTI_DISABLE_RISING_EDGE();__HAL_PWR_PVD_EXTI_DISABLE_FALLING_EDGE();

/**
  * @brief checks whether the specified PVD Exti interrupt flag is set or not.
  * @retval EXTI PVD Line Status.
  */
#define __HAL_PWR_PVD_EXTI_GET_FLAG()  (EXTI->PR & (PWR_EXTI_LINE_PVD))

/**
  * @brief Clear the PVD Exti flag.
  * @retval None.
  */
#define __HAL_PWR_PVD_EXTI_CLEAR_FLAG()  (EXTI->PR = (PWR_EXTI_LINE_PVD))

/**
  * @brief  Generates a Software interrupt on PVD EXTI line.
  * @retval None
  */
#define __HAL_PWR_PVD_EXTI_GENERATE_SWIT() (EXTI->SWIER |= (PWR_EXTI_LINE_PVD))

/**
  * @}
  */

/* Include PWR HAL Extension module */
#include "stm32f7xx_hal_pwr_ex.h"

/* Exported functions --------------------------------------------------------*/
/** @addtogroup PWR_Exported_Functions PWR Exported Functions
  * @{
  */

/** @addtogroup PWR_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
/* Initialization and de-initialization functions *****************************/
void HAL_PWR_DeInit(void);
void HAL_PWR_EnableBkUpAccess(void);
void HAL_PWR_DisableBkUpAccess(void);
/**
  * @}
  */

/** @addtogroup PWR_Exported_Functions_Group2 Peripheral Control functions
  * @{
  */
/* Peripheral Control functions  **********************************************/
/* PVD configuration */
void HAL_PWR_ConfigPVD(PWR_PVDTypeDef *sConfigPVD);
void HAL_PWR_EnablePVD(void);
void HAL_PWR_DisablePVD(void);

/* WakeUp pins configuration */
void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinPolarity);
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);

/* Low Power modes entry */
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry);
void HAL_PWR_EnterSTANDBYMode(void);

/* Power PVD IRQ Handler */
void HAL_PWR_PVD_IRQHandler(void);
void HAL_PWR_PVDCallback(void);

/* Cortex System Control functions  *******************************************/
void HAL_PWR_EnableSleepOnExit(void);
void HAL_PWR_DisableSleepOnExit(void);
void HAL_PWR_EnableSEVOnPend(void);
void HAL_PWR_DisableSEVOnPend(void);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup PWR_Private_Constants PWR Private Constants
  * @{
  */

/** @defgroup PWR_PVD_EXTI_Line PWR PVD EXTI Line
  * @{
  */
#define PWR_EXTI_LINE_PVD  ((uint32_t)EXTI_IMR_IM16)  /*!< External interrupt line 16 Connected to the PVD EXTI Line */
/**
  * @}
  */

/**
  * @}
  */
/* Private macros ------------------------------------------------------------*/
/** @defgroup PWR_Private_Macros PWR Private Macros
  * @{
  */

/** @defgroup PWR_IS_PWR_Definitions PWR Private macros to check input parameters
  * @{
  */
#define IS_PWR_PVD_LEVEL(LEVEL) (((LEVEL) == PWR_PVDLEVEL_0) || ((LEVEL) == PWR_PVDLEVEL_1)|| \
                                 ((LEVEL) == PWR_PVDLEVEL_2) || ((LEVEL) == PWR_PVDLEVEL_3)|| \
                                 ((LEVEL) == PWR_PVDLEVEL_4) || ((LEVEL) == PWR_PVDLEVEL_5)|| \
                                 ((LEVEL) == PWR_PVDLEVEL_6) || ((LEVEL) == PWR_PVDLEVEL_7))
#define IS_PWR_PVD_MODE(MODE) (((MODE) == PWR_PVD_MODE_IT_RISING)|| ((MODE) == PWR_PVD_MODE_IT_FALLING) || \
                              ((MODE) == PWR_PVD_MODE_IT_RISING_FALLING) || ((MODE) == PWR_PVD_MODE_EVENT_RISING) || \
                              ((MODE) == PWR_PVD_MODE_EVENT_FALLING) || ((MODE) == PWR_PVD_MODE_EVENT_RISING_FALLING) || \
                              ((MODE) == PWR_PVD_MODE_NORMAL))
#define IS_PWR_REGULATOR(REGULATOR) (((REGULATOR) == PWR_MAINREGULATOR_ON) || \
                                     ((REGULATOR) == PWR_LOWPOWERREGULATOR_ON))
#define IS_PWR_SLEEP_ENTRY(ENTRY) (((ENTRY) == PWR_SLEEPENTRY_WFI) || ((ENTRY) == PWR_SLEEPENTRY_WFE))
#define IS_PWR_STOP_ENTRY(ENTRY) (((ENTRY) == PWR_STOPENTRY_WFI) || ((ENTRY) == PWR_STOPENTRY_WFE))
#define IS_PWR_REGULATOR_VOLTAGE(VOLTAGE) (((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE1) || \
                                           ((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE2) || \
                                           ((VOLTAGE) == PWR_REGULATOR_VOLTAGE_SCALE3))

/**
  * @}
  */

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


#endif /* __STM32F7xx_HAL_PWR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
