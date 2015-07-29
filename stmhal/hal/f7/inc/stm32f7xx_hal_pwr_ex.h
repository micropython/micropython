/**
  ******************************************************************************
  * @file    stm32f7xx_hal_pwr_ex.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    25-June-2015
  * @brief   Header file of PWR HAL Extension module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
#ifndef __STM32F7xx_HAL_PWR_EX_H
#define __STM32F7xx_HAL_PWR_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal_def.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @addtogroup PWREx
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup PWREx_Exported_Constants PWREx Exported Constants
  * @{
  */
/** @defgroup PWREx_WakeUp_Pins PWREx Wake Up Pins
  * @{
  */
#define PWR_WAKEUP_PIN1                PWR_CSR2_EWUP1
#define PWR_WAKEUP_PIN2                PWR_CSR2_EWUP2
#define PWR_WAKEUP_PIN3                PWR_CSR2_EWUP3
#define PWR_WAKEUP_PIN4                PWR_CSR2_EWUP4
#define PWR_WAKEUP_PIN5                PWR_CSR2_EWUP5
#define PWR_WAKEUP_PIN6                PWR_CSR2_EWUP6
#define PWR_WAKEUP_PIN1_HIGH           PWR_CSR2_EWUP1
#define PWR_WAKEUP_PIN2_HIGH           PWR_CSR2_EWUP2
#define PWR_WAKEUP_PIN3_HIGH           PWR_CSR2_EWUP3
#define PWR_WAKEUP_PIN4_HIGH           PWR_CSR2_EWUP4
#define PWR_WAKEUP_PIN5_HIGH           PWR_CSR2_EWUP5
#define PWR_WAKEUP_PIN6_HIGH           PWR_CSR2_EWUP6
#define PWR_WAKEUP_PIN1_LOW            (uint32_t)((PWR_CR2_WUPP1<<6) | PWR_CSR2_EWUP1)
#define PWR_WAKEUP_PIN2_LOW            (uint32_t)((PWR_CR2_WUPP2<<6) | PWR_CSR2_EWUP2)
#define PWR_WAKEUP_PIN3_LOW            (uint32_t)((PWR_CR2_WUPP3<<6) | PWR_CSR2_EWUP3)
#define PWR_WAKEUP_PIN4_LOW            (uint32_t)((PWR_CR2_WUPP4<<6) | PWR_CSR2_EWUP4)
#define PWR_WAKEUP_PIN5_LOW            (uint32_t)((PWR_CR2_WUPP5<<6) | PWR_CSR2_EWUP5)
#define PWR_WAKEUP_PIN6_LOW            (uint32_t)((PWR_CR2_WUPP6<<6) | PWR_CSR2_EWUP6)

/**
  * @}
  */

/** @defgroup PWREx_Regulator_state_in_UnderDrive_mode PWREx Regulator state in UnderDrive mode
  * @{
  */
#define PWR_MAINREGULATOR_UNDERDRIVE_ON                       PWR_CR1_MRUDS
#define PWR_LOWPOWERREGULATOR_UNDERDRIVE_ON                   ((uint32_t)(PWR_CR1_LPDS | PWR_CR1_LPUDS))
/**
  * @}
  */

/** @defgroup PWREx_Over_Under_Drive_Flag PWREx Over Under Drive Flag
  * @{
  */
#define PWR_FLAG_ODRDY                  PWR_CSR1_ODRDY
#define PWR_FLAG_ODSWRDY                PWR_CSR1_ODSWRDY
#define PWR_FLAG_UDRDY                  PWR_CSR1_UDSWRDY
/**
  * @}
  */

/** @defgroup PWREx_Wakeup_Pins_Flag PWREx Wake Up Pin Flags
  * @{
  */
#define PWR_WAKEUP_PIN_FLAG1            PWR_CSR2_WUPF1
#define PWR_WAKEUP_PIN_FLAG2            PWR_CSR2_WUPF2
#define PWR_WAKEUP_PIN_FLAG3            PWR_CSR2_WUPF3
#define PWR_WAKEUP_PIN_FLAG4            PWR_CSR2_WUPF4
#define PWR_WAKEUP_PIN_FLAG5            PWR_CSR2_WUPF5
#define PWR_WAKEUP_PIN_FLAG6            PWR_CSR2_WUPF6
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup PWREx_Exported_Macro PWREx Exported Macro
  *  @{
  */
/** @brief Macros to enable or disable the Over drive mode.
  */
#define __HAL_PWR_OVERDRIVE_ENABLE() (PWR->CR1 |= (uint32_t)PWR_CR1_ODEN)
#define __HAL_PWR_OVERDRIVE_DISABLE() (PWR->CR1 &= (uint32_t)(~PWR_CR1_ODEN))

/** @brief Macros to enable or disable the Over drive switching.
  */
#define __HAL_PWR_OVERDRIVESWITCHING_ENABLE() (PWR->CR1 |= (uint32_t)PWR_CR1_ODSWEN)
#define __HAL_PWR_OVERDRIVESWITCHING_DISABLE() (PWR->CR1 &= (uint32_t)(~PWR_CR1_ODSWEN))

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
#define __HAL_PWR_UNDERDRIVE_ENABLE() (PWR->CR1 |= (uint32_t)PWR_CR1_UDEN)
#define __HAL_PWR_UNDERDRIVE_DISABLE() (PWR->CR1 &= (uint32_t)(~PWR_CR1_UDEN))

/** @brief  Check PWR flag is set or not.
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
#define __HAL_PWR_GET_ODRUDR_FLAG(__FLAG__) ((PWR->CSR1 & (__FLAG__)) == (__FLAG__))

/** @brief Clear the Under-Drive Ready flag.
  */
#define __HAL_PWR_CLEAR_ODRUDR_FLAG() (PWR->CSR1 |= PWR_FLAG_UDRDY)

/** @brief  Check Wake Up flag is set or not.
  * @param  __WUFLAG__: specifies the Wake Up flag to check.
  *          This parameter can be one of the following values:
  *            @arg PWR_WAKEUP_PIN_FLAG1: Wakeup Pin Flag for PA0
  *            @arg PWR_WAKEUP_PIN_FLAG2: Wakeup Pin Flag for PA2
  *            @arg PWR_WAKEUP_PIN_FLAG3: Wakeup Pin Flag for PC1
  *            @arg PWR_WAKEUP_PIN_FLAG4: Wakeup Pin Flag for PC13
  *            @arg PWR_WAKEUP_PIN_FLAG5: Wakeup Pin Flag for PI8
  *            @arg PWR_WAKEUP_PIN_FLAG6: Wakeup Pin Flag for PI11
  */
#define __HAL_PWR_GET_WAKEUP_FLAG(__WUFLAG__) (PWR->CSR2 & (__WUFLAG__))

/** @brief  Clear the WakeUp pins flags.
  * @param  __WUFLAG__: specifies the Wake Up pin flag to clear.
  *          This parameter can be one of the following values:
  *            @arg PWR_WAKEUP_PIN_FLAG1: Wakeup Pin Flag for PA0
  *            @arg PWR_WAKEUP_PIN_FLAG2: Wakeup Pin Flag for PA2
  *            @arg PWR_WAKEUP_PIN_FLAG3: Wakeup Pin Flag for PC1
  *            @arg PWR_WAKEUP_PIN_FLAG4: Wakeup Pin Flag for PC13
  *            @arg PWR_WAKEUP_PIN_FLAG5: Wakeup Pin Flag for PI8
  *            @arg PWR_WAKEUP_PIN_FLAG6: Wakeup Pin Flag for PI11
  */
#define __HAL_PWR_CLEAR_WAKEUP_FLAG(__WUFLAG__) (PWR->CR2 |=  (__WUFLAG__))
/**
  * @}
  */
/* Exported functions --------------------------------------------------------*/
/** @addtogroup PWREx_Exported_Functions PWREx Exported Functions
  *  @{
  */

/** @addtogroup PWREx_Exported_Functions_Group1
  * @{
  */
uint32_t HAL_PWREx_GetVoltageRange(void);
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);

void HAL_PWREx_EnableFlashPowerDown(void);
void HAL_PWREx_DisableFlashPowerDown(void);
HAL_StatusTypeDef HAL_PWREx_EnableBkUpReg(void);
HAL_StatusTypeDef HAL_PWREx_DisableBkUpReg(void);

void HAL_PWREx_EnableMainRegulatorLowVoltage(void);
void HAL_PWREx_DisableMainRegulatorLowVoltage(void);
void HAL_PWREx_EnableLowRegulatorLowVoltage(void);
void HAL_PWREx_DisableLowRegulatorLowVoltage(void);

HAL_StatusTypeDef HAL_PWREx_EnableOverDrive(void);
HAL_StatusTypeDef HAL_PWREx_DisableOverDrive(void);
HAL_StatusTypeDef HAL_PWREx_EnterUnderDriveSTOPMode(uint32_t Regulator, uint8_t STOPEntry);

/**
  * @}
  */

/**
  * @}
  */
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup PWREx_Private_Macros PWREx Private Macros
  * @{
  */

/** @defgroup PWREx_IS_PWR_Definitions PWREx Private macros to check input parameters
  * @{
  */
#define IS_PWR_REGULATOR_UNDERDRIVE(REGULATOR) (((REGULATOR) == PWR_MAINREGULATOR_UNDERDRIVE_ON) || \
                                                ((REGULATOR) == PWR_LOWPOWERREGULATOR_UNDERDRIVE_ON))
#define IS_PWR_WAKEUP_PIN(__PIN__)         (((__PIN__) == PWR_WAKEUP_PIN1)       || \
                                            ((__PIN__) == PWR_WAKEUP_PIN2)       || \
                                            ((__PIN__) == PWR_WAKEUP_PIN3)       || \
                                            ((__PIN__) == PWR_WAKEUP_PIN4)       || \
                                            ((__PIN__) == PWR_WAKEUP_PIN5)       || \
                                            ((__PIN__) == PWR_WAKEUP_PIN6)  		 || \
                                            ((__PIN__) == PWR_WAKEUP_PIN1_HIGH)  || \
                                            ((__PIN__) == PWR_WAKEUP_PIN2_HIGH)  || \
                                            ((__PIN__) == PWR_WAKEUP_PIN3_HIGH)  || \
                                            ((__PIN__) == PWR_WAKEUP_PIN4_HIGH)  || \
                                            ((__PIN__) == PWR_WAKEUP_PIN5_HIGH)  || \
                                            ((__PIN__) == PWR_WAKEUP_PIN6_HIGH)  || \
                                            ((__PIN__) == PWR_WAKEUP_PIN1_LOW)   || \
                                            ((__PIN__) == PWR_WAKEUP_PIN2_LOW)   || \
                                            ((__PIN__) == PWR_WAKEUP_PIN3_LOW)   || \
                                            ((__PIN__) == PWR_WAKEUP_PIN4_LOW)   || \
                                            ((__PIN__) == PWR_WAKEUP_PIN5_LOW)	 || \
                                            ((__PIN__) == PWR_WAKEUP_PIN6_LOW))
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


#endif /* __STM32F7xx_HAL_PWR_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
