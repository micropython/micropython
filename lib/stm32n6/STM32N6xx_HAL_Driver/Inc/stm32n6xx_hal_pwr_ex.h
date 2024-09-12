/**
  ******************************************************************************
  * @file    stm32n6xx_hal_pwr_ex.h
  * @author  MCD Application Team
  * @brief   Header file of PWR HAL Extended module.
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
#ifndef STM32N6xx_HAL_PWR_EX_H
#define STM32N6xx_HAL_PWR_EX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal_def.h"


/** @addtogroup STM32N6xx_HAL_Driver
  * @{
  */

/** @addtogroup PWREx
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup PWREx_Exported_Types PWR Extended Exported Types
  * @{
  */
/**
  * @brief  PWREx Wakeup pin configuration structure definition
  */
typedef struct
{
  uint32_t WakeUpPin;   /*!< Specifies the Wake-Up pin to be enabled.
                             This parameter can be a value of @ref PWR_WakeUp_Pins  */

  uint32_t PinPolarity; /*!< Specifies the Wake-Up pin polarity.
                             This parameter can be a value of @ref PWR_PIN_Polarity */

  uint32_t PinPull;     /*!< Specifies the Wake-Up pin pull.
                             This parameter can be a value of @ref PWR_PIN_Pull     */
} PWREx_WakeupPinTypeDef;

/**
  * @brief  PWR PVM configuration structure definition
  */
typedef struct
{
  uint32_t PVMType;   /*!< Specifies which voltage is monitored.
                           This parameter can be a value of
                           @ref PWREx_PVM_Type.                               */

  uint32_t Mode;      /*!< Specifies the operating mode for the selected pins.
                           This parameter can be a value of
                           @ref PWREx_PVM_Mode.                               */
} PWR_PVMTypeDef;


/**
  * @brief  PWR VddCORE monitoring configuration structure definition
  */
typedef struct
{
  uint32_t LowVoltageThreshold;   /*!< Specifies the VDDCORE voltage detector low-level.
                                       This parameter can be a value of
                                       @ref PWREx_VDDCORE_Levels.                          */

  uint32_t Mode;                  /*!< Specifies the operating mode for the selected pins.
                                       This parameter can be a value of
                                       @ref PWREx_VddCOREVM_Mode.                          */
} PWR_VddCOREVMTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup PWREx_Exported_Constants PWR Extended Exported Constants
  * @{
  */

/** @defgroup PWREx_Supply_configuration PWREx Supply configuration
  * @{
  */
#define PWR_SMPS_SUPPLY                PWR_CR1_SDEN                            /*!< VCORE power domains are supplied from SMPS step-down converter according to VOS */
#define PWR_EXTERNAL_SOURCE_SUPPLY     (0U)                                    /*!< SMPS step-down converter disabled. VCORE supplied from external source          */

#define PWR_SUPPLY_CONFIG_MASK         PWR_CR1_SDEN
/**
  * @}
  */

/** @defgroup PWR_Regulator_Voltage_Scale PWR Regulator Voltage Scale
  * @{
  */
#define PWR_REGULATOR_VOLTAGE_SCALE0   PWR_VOSCR_VOS                           /*!< Voltage scaling range 0 (highest frequency) */
#define PWR_REGULATOR_VOLTAGE_SCALE1   (0U)                                    /*!< Voltage scaling range 1 (lowest power)      */
/**
  * @}
  */

/** @defgroup PWREx_Regulator_Voltage_Scale PWREx Regulator Voltage Scale
  * @{
  */
#define PWR_REGULATOR_STOP_VOLTAGE_SCALE3    PWR_CPUCR_SVOS                    /*!< System Stop mode voltage scaling range 3 (highest frequency) */
#define PWR_REGULATOR_STOP_VOLTAGE_SCALE5    (0U)                              /*!< System Stop mode voltage scaling range 5 (lowest power)      */
/**
  * @}
  */

/** @defgroup PWREx_VBAT_Thresholds PWREx VBAT Thresholds
  * @{
  */
#define PWR_VBAT_BETWEEN_HIGH_LOW_THRESHOLD    (0U)
#define PWR_VBAT_BELOW_LOW_THRESHOLD           PWR_BDCR1_VBATL
#define PWR_VBAT_ABOVE_HIGH_THRESHOLD          PWR_BDCR1_VBATH
/**
  * @}
  */

/** @defgroup PWREx_TEMP_Thresholds PWREx Temperature Thresholds
  * @{
  */
#define PWR_TEMP_BETWEEN_HIGH_LOW_THRESHOLD  (0U)
#define PWR_TEMP_BELOW_LOW_THRESHOLD         PWR_BDCR1_TEMPL
#define PWR_TEMP_ABOVE_HIGH_THRESHOLD        PWR_BDCR1_TEMPH
/**
  * @}
  */

/** @defgroup PWREx_VDDCORE_Levels PWREx VDDCORE voltage detector low-level
  * @{
  */
#define PWR_VDDCORE_THRESHOLD_VOS0     PWR_CR3_VCORELLS
#define PWR_VDDCORE_THRESHOLD_VOS1     (0U)
/**
  * @}
  */

/** @defgroup PWREx_VDDCORE_Thresholds PWREx VDDCORE voltage detector low-level Thresholds
  * @{
  */
#define PWR_VDDCORE_BETWEEN_HIGH_LOW_THRESHOLD  (0U)
#define PWR_VDDCORE_BELOW_LOW_THRESHOLD         PWR_CR3_VCOREL
#define PWR_VDDCORE_ABOVE_HIGH_THRESHOLD        PWR_CR3_VCOREH
/**
  * @}
  */

/** @defgroup PWREx_VddCOREVM_Mode PWR Extended VddCORE Monitoring Interrupt and Event Mode
  * @{
  */
#define PWR_VDDCOREVM_MODE_NORMAL               (0x00U)                        /*!< Basic Mode is used                                                 */
#define PWR_VDDCOREVM_MODE_IT_RISING            (0x05U)                        /*!< External Interrupt Mode with Rising edge trigger detection         */
#define PWR_VDDCOREVM_MODE_IT_FALLING           (0x06U)                        /*!< External Interrupt Mode with Falling edge trigger detection        */
#define PWR_VDDCOREVM_MODE_IT_RISING_FALLING    (0x07U)                        /*!< External Interrupt Mode with Rising/Falling edge trigger detection */
#define PWR_VDDCOREVM_MODE_EVENT_RISING         (0x09U)                        /*!< Event Mode with Rising edge trigger detection                      */
#define PWR_VDDCOREVM_MODE_EVENT_FALLING        (0x0AU)                        /*!< Event Mode with Falling edge trigger detection                     */
#define PWR_VDDCOREVM_MODE_EVENT_RISING_FALLING (0x0BU)                        /*!< Event Mode with Rising/Falling edge trigger detection              */
/**
  * @}
  */

/** @defgroup PWREx_VDDIO PWREx Vdd IO selection
  * @{
  */
#define PWR_VDDIO                      (0U)
#define PWR_VDDIO2                     (1U)
#define PWR_VDDIO3                     (2U)
#define PWR_VDDIO4                     (3U)
#define PWR_VDDIO5                     (4U)
/**
  * @}
  */

/** @defgroup PWREx_VDDIO_Range PWREx Vdd IO Range
  * @{
  */
#define PWR_VDDIO_RANGE_3V3            (0U)
#define PWR_VDDIO_RANGE_1V8            (1U)
/**
  * @}
  */

/** @defgroup PWREx_PVM_Mode PWR Extended PVM Interrupt and Event Mode
  * @{
  */
#define PWR_PVM_MODE_NORMAL               (0x00U)                              /*!< Basic Mode is used                                                 */
#define PWR_PVM_MODE_IT_RISING            (0x05U)                              /*!< External Interrupt Mode with Rising edge trigger detection         */
#define PWR_PVM_MODE_IT_FALLING           (0x06U)                              /*!< External Interrupt Mode with Falling edge trigger detection        */
#define PWR_PVM_MODE_IT_RISING_FALLING    (0x07U)                              /*!< External Interrupt Mode with Rising/Falling edge trigger detection */
#define PWR_PVM_MODE_EVENT_RISING         (0x09U)                              /*!< Event Mode with Rising edge trigger detection                      */
#define PWR_PVM_MODE_EVENT_FALLING        (0x0AU)                              /*!< Event Mode with Falling edge trigger detection                     */
#define PWR_PVM_MODE_EVENT_RISING_FALLING (0x0BU)                              /*!< Event Mode with Rising/Falling edge trigger detection              */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/** @defgroup PWREx_Exported_Macros PWR Extended Exported Macros
  * @{
  */

/**
  * @brief  Enable the USBVM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Disable the USBVM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Enable the USBVM Event Line.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Disable the USBVM Event Line.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Enable the USBVM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Disable the USBVM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Enable the USBVM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Disable the USBVM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Enable the USBVM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_ENABLE_RISING_FALLING_EDGE() \
  do                                                    \
  {                                                     \
    __HAL_PWR_USBVM_EXTI_ENABLE_RISING_EDGE();            \
    __HAL_PWR_USBVM_EXTI_ENABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Disable the USBVM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_DISABLE_RISING_FALLING_EDGE() \
  do                                                     \
  {                                                      \
    __HAL_PWR_USBVM_EXTI_DISABLE_RISING_EDGE();            \
    __HAL_PWR_USBVM_EXTI_DISABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Generate a Software Interrupt on USBVM EXTI Line.
  * @retval None
  */
#define __HAL_PWR_USBVM_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Check whether the specified USBVM EXTI flag is set or not.
  * @retval EXTI USBVM Line Status.
  */
#define __HAL_PWR_USBVM_EXTI_GET_FLAG() ((EXTI->RPR3 | EXTI->FPR3) & PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Clear the USBVM interrupt Rising flag.
  * @retval None.
  */
#define __HAL_PWR_USBVM_EXTI_CLEAR_RISING_FLAG() WRITE_REG(EXTI->RPR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Clear the USBVM interrupt Falling flag.
  * @retval None
  */
#define __HAL_PWR_USBVM_EXTI_CLEAR_FALLING_FLAG() WRITE_REG(EXTI->FPR3, PWR_EXTI_LINE_PVM_VDDUSB)

/**
  * @brief  Enable the IO2VM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Disable the IO2VM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Enable the IO2VM Event Line.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Disable the IO2VM Event Line.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Enable the IO2VM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Disable the IO2VM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Enable the IO2VM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Disable the IO2VM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Enable the IO2VM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_ENABLE_RISING_FALLING_EDGE() \
  do                                                      \
  {                                                       \
    __HAL_PWR_IO2VM_EXTI_ENABLE_RISING_EDGE();            \
    __HAL_PWR_IO2VM_EXTI_ENABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Disable the IO2VM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_DISABLE_RISING_FALLING_EDGE() \
  do                                                       \
  {                                                        \
    __HAL_PWR_IO2VM_EXTI_DISABLE_RISING_EDGE();            \
    __HAL_PWR_IO2VM_EXTI_DISABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Generate a Software Interrupt on IO2VM EXTI Line.
  * @retval None
  */
#define __HAL_PWR_IO2VM_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Check whether the specified IO2VM EXTI flag is set or not.
  * @retval EXTI IO2VM Line Status.
  */
#define __HAL_PWR_IO2VM_EXTI_GET_FLAG() ((EXTI->RPR3 | EXTI->FPR3) & PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Clear the IO2VM interrupt Rising flag.
  * @retval None.
  */
#define __HAL_PWR_IO2VM_EXTI_CLEAR_RISING_FLAG() WRITE_REG(EXTI->RPR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Clear the IO2VM interrupt Falling flag.
  * @retval None
  */
#define __HAL_PWR_IO2VM_EXTI_CLEAR_FALLING_FLAG() WRITE_REG(EXTI->FPR3, PWR_EXTI_LINE_PVM_VDDIO2)

/**
  * @brief  Enable the IO3VM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Disable the IO3VM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Enable the IO3VM Event Line.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Disable the IO3VM Event Line.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Enable the IO3VM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Disable the IO3VM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Enable the IO3VM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Disable the IO3VM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Enable the IO3VM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_ENABLE_RISING_FALLING_EDGE() \
  do                                                      \
  {                                                       \
    __HAL_PWR_IO3VM_EXTI_ENABLE_RISING_EDGE();            \
    __HAL_PWR_IO3VM_EXTI_ENABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Disable the IO3VM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_DISABLE_RISING_FALLING_EDGE() \
  do                                                       \
  {                                                        \
    __HAL_PWR_IO3VM_EXTI_DISABLE_RISING_EDGE();            \
    __HAL_PWR_IO3VM_EXTI_DISABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Generate a Software Interrupt on IO3VM EXTI Line.
  * @retval None
  */
#define __HAL_PWR_IO3VM_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Check whether the specified IO3VM EXTI flag is set or not.
  * @retval EXTI IO3VM Line Status.
  */
#define __HAL_PWR_IO3VM_EXTI_GET_FLAG() ((EXTI->RPR3 | EXTI->FPR3) & PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Clear the IO3VM interrupt Rising flag.
  * @retval None.
  */
#define __HAL_PWR_IO3VM_EXTI_CLEAR_RISING_FLAG() WRITE_REG(EXTI->RPR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Clear the IO3VM interrupt Falling flag.
  * @retval None
  */
#define __HAL_PWR_IO3VM_EXTI_CLEAR_FALLING_FLAG() WRITE_REG(EXTI->FPR3, PWR_EXTI_LINE_PVM_VDDIO3)

/**
  * @brief  Enable the IO4VM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Disable the IO4VM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Enable the IO4VM Event Line.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Disable the IO4VM Event Line.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Enable the IO4VM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Disable the IO4VM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Enable the IO4VM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Disable the IO4VM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Enable the IO4VM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_ENABLE_RISING_FALLING_EDGE() \
  do                                                      \
  {                                                       \
    __HAL_PWR_IO4VM_EXTI_ENABLE_RISING_EDGE();            \
    __HAL_PWR_IO4VM_EXTI_ENABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Disable the IO4VM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_DISABLE_RISING_FALLING_EDGE() \
  do                                                       \
  {                                                        \
    __HAL_PWR_IO4VM_EXTI_DISABLE_RISING_EDGE();            \
    __HAL_PWR_IO4VM_EXTI_DISABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Generate a Software Interrupt on IO4VM EXTI Line.
  * @retval None
  */
#define __HAL_PWR_IO4VM_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Check whether the specified IO4VM EXTI flag is set or not.
  * @retval EXTI IO4VM Line Status.
  */
#define __HAL_PWR_IO4VM_EXTI_GET_FLAG() ((EXTI->RPR3 | EXTI->FPR3) & PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Clear the IO4VM interrupt Rising flag.
  * @retval None.
  */
#define __HAL_PWR_IO4VM_EXTI_CLEAR_RISING_FLAG() WRITE_REG(EXTI->RPR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Clear the IO4VM interrupt Falling flag.
  * @retval None
  */
#define __HAL_PWR_IO4VM_EXTI_CLEAR_FALLING_FLAG() WRITE_REG(EXTI->FPR3, PWR_EXTI_LINE_PVM_VDDIO4)

/**
  * @brief  Enable the IO5VM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Disable the IO5VM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Enable the IO5VM Event Line.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Disable the IO5VM Event Line.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Enable the IO5VM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Disable the IO5VM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Enable the IO5VM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Disable the IO5VM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Enable the IO5VM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_ENABLE_RISING_FALLING_EDGE() \
  do                                                      \
  {                                                       \
    __HAL_PWR_IO5VM_EXTI_ENABLE_RISING_EDGE();            \
    __HAL_PWR_IO5VM_EXTI_ENABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Disable the IO5VM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_DISABLE_RISING_FALLING_EDGE() \
  do                                                       \
  {                                                        \
    __HAL_PWR_IO5VM_EXTI_DISABLE_RISING_EDGE();            \
    __HAL_PWR_IO5VM_EXTI_DISABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Generate a Software Interrupt on IO5VM EXTI Line.
  * @retval None
  */
#define __HAL_PWR_IO5VM_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Check whether the specified IO5VM EXTI flag is set or not.
  * @retval EXTI IO5VM Line Status.
  */
#define __HAL_PWR_IO5VM_EXTI_GET_FLAG() ((EXTI->RPR3 | EXTI->FPR3) & PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Clear the IO5VM interrupt Rising flag.
  * @retval None.
  */
#define __HAL_PWR_IO5VM_EXTI_CLEAR_RISING_FLAG() WRITE_REG(EXTI->RPR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Clear the IO5VM interrupt Falling flag.
  * @retval None
  */
#define __HAL_PWR_IO5VM_EXTI_CLEAR_FALLING_FLAG() WRITE_REG(EXTI->FPR3, PWR_EXTI_LINE_PVM_VDDIO5)

/**
  * @brief  Enable the ADCVM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Disable the ADCVM Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Enable the ADCVM Event Line.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Disable the ADCVM Event Line.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Enable the ADCVM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Disable the ADCVM Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Enable the ADCVM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Disable the ADCVM Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Enable the ADCVM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_ENABLE_RISING_FALLING_EDGE() \
  do                                                    \
  {                                                     \
    __HAL_PWR_ADCVM_EXTI_ENABLE_RISING_EDGE();            \
    __HAL_PWR_ADCVM_EXTI_ENABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Disable the ADCVM Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_DISABLE_RISING_FALLING_EDGE() \
  do                                                     \
  {                                                      \
    __HAL_PWR_ADCVM_EXTI_DISABLE_RISING_EDGE();            \
    __HAL_PWR_ADCVM_EXTI_DISABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Generate a Software Interrupt on ADCVM EXTI Line.
  * @retval None
  */
#define __HAL_PWR_ADCVM_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Check whether the specified ADCVM EXTI flag is set or not.
  * @retval EXTI ADCVM Line Status.
  */
#define __HAL_PWR_ADCVM_EXTI_GET_FLAG() ((EXTI->RPR3 | EXTI->FPR3) & PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Clear the ADCVM interrupt Rising flag.
  * @retval None.
  */
#define __HAL_PWR_ADCVM_EXTI_CLEAR_RISING_FLAG() WRITE_REG(EXTI->RPR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Clear the ADCVM interrupt Falling flag.
  * @retval None
  */
#define __HAL_PWR_ADCVM_EXTI_CLEAR_FALLING_FLAG() WRITE_REG(EXTI->FPR3, PWR_EXTI_LINE_PVM_VDDA)

/**
  * @brief  Enable the VddCORE monitoring Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_ENABLE_IT() SET_BIT(EXTI->IMR3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Disable the VddCORE monitoring Extended Interrupt Line.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_DISABLE_IT() CLEAR_BIT(EXTI->IMR3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Enable the VddCORE monitoring Event Line.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_ENABLE_EVENT() SET_BIT(EXTI->EMR3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Disable the VddCORE monitoring Event Line.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_DISABLE_EVENT() CLEAR_BIT(EXTI->EMR3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Enable the VddCORE monitoring Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_ENABLE_RISING_EDGE() SET_BIT(EXTI->RTSR3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Disable the VddCORE monitoring Extended Interrupt Rising Trigger.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_DISABLE_RISING_EDGE() CLEAR_BIT(EXTI->RTSR3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Enable the VddCORE monitoring Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_ENABLE_FALLING_EDGE() SET_BIT(EXTI->FTSR3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Disable the VddCORE monitoring Extended Interrupt Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_DISABLE_FALLING_EDGE() CLEAR_BIT(EXTI->FTSR3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Enable the VddCORE monitoring Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_ENABLE_RISING_FALLING_EDGE() \
  do                                                    \
  {                                                     \
    __HAL_PWR_VCOREVM_EXTI_ENABLE_RISING_EDGE();            \
    __HAL_PWR_VCOREVM_EXTI_ENABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Disable the VddCORE monitoring Extended Interrupt Rising & Falling Trigger.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_DISABLE_RISING_FALLING_EDGE() \
  do                                                     \
  {                                                      \
    __HAL_PWR_VCOREVM_EXTI_DISABLE_RISING_EDGE();            \
    __HAL_PWR_VCOREVM_EXTI_DISABLE_FALLING_EDGE();           \
  } while(0)

/**
  * @brief  Generate a Software Interrupt on VddCORE monitoring EXTI Line.
  * @retval None
  */
#define __HAL_PWR_VCOREVM_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Check whether the specified VddCORE monitoring EXTI flag is set or not.
  * @retval EXTI VddCORE monitoring Line Status.
  */
#define __HAL_PWR_VCOREVM_EXTI_GET_FLAG() ((EXTI->RPR3 | EXTI->FPR3) & PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Clear the VddCORE monitoring interrupt Rising flag.
  * @retval None.
  */
#define __HAL_PWR_VCOREVM_EXTI_CLEAR_RISING_FLAG() WRITE_REG(EXTI->RPR3, PWR_EXTI_LINE_VCOREVM)

/**
  * @brief  Clear the VddCORE monitoring interrupt Falling flag.
  * @retval None
  */
#define __HAL_PWR_VCOREVM_EXTI_CLEAR_FALLING_FLAG() WRITE_REG(EXTI->FPR3, PWR_EXTI_LINE_VCOREVM)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup PWREx_Exported_Functions PWR Extended Exported Functions
  * @{
  */

/** @addtogroup PWREx_Exported_Functions_Group1 Power Supply Control Functions
  * @{
  */
/* Power supply control functions */
HAL_StatusTypeDef HAL_PWREx_ConfigSupply(uint32_t SupplySource);
uint32_t          HAL_PWREx_GetSupplyConfig(void);

/* Power voltage scaling functions */
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);
uint32_t          HAL_PWREx_GetVoltageRange(void);
void              HAL_PWREx_ControlStopModeVoltageScaling(uint32_t VoltageScaling);
uint32_t          HAL_PWREx_GetStopModeVoltageRange(void);


/** @addtogroup PWREx_Exported_Functions_Group2 Wakeup Pins configuration functions
  * @{
  */
/* Wakeup Pins control functions */
void HAL_PWREx_EnableWakeUpPin(const PWREx_WakeupPinTypeDef *sPinParams);
/**
  * @}
  */


/** @addtogroup PWREx_Exported_Functions_Group3 Memories Retention Functions
  * @{
  */
void HAL_PWREx_EnableBkupRAMRetention(void);
void HAL_PWREx_DisableBkupRAMRetention(void);
void HAL_PWREx_EnableTCMRetention(void);
void HAL_PWREx_DisableTCMRetention(void);
void HAL_PWREx_EnableTCMFLXRetention(void);
void HAL_PWREx_DisableTCMFLXRetention(void);
/**
  * @}
  */

/** @addtogroup PWREx_Exported_Functions_Group4 Low Power Control Functions
  * @{
  */
void     HAL_PWREx_SetPulseLow(uint32_t Pulselowtime);
uint32_t HAL_PWREx_GetPulseLow(void);
void     HAL_PWREx_EnableSMPSPWM(void);
void     HAL_PWREx_DisableSMPSPWM(void);
void     HAL_PWREx_EnablePullDownOutput(void);
void     HAL_PWREx_DisablePullDownOutput(void);
/**
  * @}
  */

/** @addtogroup PWREx_Exported_Functions_Group5 Power Monitoring functions
  * @{
  */
/* Power VBAT/Temperature monitoring functions */
void              HAL_PWREx_EnableMonitoring(void);
void              HAL_PWREx_DisableMonitoring(void);
uint32_t          HAL_PWREx_GetTemperatureLevel(void);
uint32_t          HAL_PWREx_GetVBATLevel(void);
void              HAL_PWREx_EnableVDDCOREMonitoring(void);
void              HAL_PWREx_DisableVDDCOREMonitoring(void);
void              HAL_PWREx_ConfigVDDCOREVM(const PWR_VddCOREVMTypeDef *pConfigVddCOREVM);
uint32_t          HAL_PWREx_GetVDDCORELevel(void);
void              HAL_PWREx_ConfigVddIORange(uint32_t VddIOPort, uint32_t VoltageRange);
uint32_t          HAL_PWREx_GetVddIORange(uint32_t VddIOPort);
void              HAL_PWREx_EnableVddIO4RangeSTBY(void);
void              HAL_PWREx_DisableVddIO4RangeSTBY(void);
void              HAL_PWREx_EnableVddIO5RangeSTBY(void);
void              HAL_PWREx_DisableVddIO5RangeSTBY(void);
void              HAL_PWREx_EnableVddUSB(void);
void              HAL_PWREx_DisableVddUSB(void);
void              HAL_PWREx_EnableVddIO2(void);
void              HAL_PWREx_DisableVddIO2(void);
void              HAL_PWREx_EnableVddIO3(void);
void              HAL_PWREx_DisableVddIO3(void);
void              HAL_PWREx_EnableVddIO4(void);
void              HAL_PWREx_DisableVddIO4(void);
void              HAL_PWREx_EnableVddIO5(void);
void              HAL_PWREx_DisableVddIO5(void);
void              HAL_PWREx_EnableVddA(void);
void              HAL_PWREx_DisableVddA(void);
void              HAL_PWREx_EnableVddUSBVMEN(void);
void              HAL_PWREx_DisableVddUSBVMEN(void);
void              HAL_PWREx_EnableVddIO2VMEN(void);
void              HAL_PWREx_DisableVddIO2VMEN(void);
void              HAL_PWREx_EnableVddIO3VMEN(void);
void              HAL_PWREx_DisableVddIO3VMEN(void);
void              HAL_PWREx_EnableVddIO4VMEN(void);
void              HAL_PWREx_DisableVddIO4VMEN(void);
void              HAL_PWREx_EnableVddIO5VMEN(void);
void              HAL_PWREx_DisableVddIO5VMEN(void);
void              HAL_PWREx_EnableVddAVMEN(void);
void              HAL_PWREx_DisableVddAVMEN(void);
HAL_StatusTypeDef HAL_PWREx_ConfigPVM(const PWR_PVMTypeDef *pConfigPVM);
void              HAL_PWREx_PVD_PVM_IRQHandler(void);
void              HAL_PWREx_VDDCORE_Rising_Callback(void);
void              HAL_PWREx_VDDCORE_Falling_Callback(void);
void              HAL_PWREx_USBVM_Rising_Callback(void);
void              HAL_PWREx_USBVM_Falling_Callback(void);
void              HAL_PWREx_IO2VM_Rising_Callback(void);
void              HAL_PWREx_IO2VM_Falling_Callback(void);
void              HAL_PWREx_IO3VM_Rising_Callback(void);
void              HAL_PWREx_IO3VM_Falling_Callback(void);
void              HAL_PWREx_IO4VM_Rising_Callback(void);
void              HAL_PWREx_IO4VM_Falling_Callback(void);
void              HAL_PWREx_IO5VM_Rising_Callback(void);
void              HAL_PWREx_IO5VM_Falling_Callback(void);
void              HAL_PWREx_ADCVM_Rising_Callback(void);
void              HAL_PWREx_ADCVM_Falling_Callback(void);
/**
  * @}
  */

/**
  * @}
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/


/** @defgroup PWREx_VDDCOREVM_EXTI PWR VddCORE monitoring  extended interrupts and event lines defines
  * @{
  */
#define PWR_EXTI_LINE_VCOREVM          EXTI_IMR3_IM68                          /*!< VddCORE monitoring EXTI Line */
/**
  * @}
  */

/** @defgroup PWREx_PVM_EXTI PWR PVM extended interrupts and event lines defines
  * @{
  */
#define PWR_EXTI_LINE_PVM_VDDIO2       EXTI_IMR3_IM69                          /*!< PVM VDDIO2 EXTI Line  */
#define PWR_EXTI_LINE_PVM_VDDIO3       EXTI_IMR3_IM70                          /*!< PVM VDDIO3 EXTI Line  */
#define PWR_EXTI_LINE_PVM_VDDIO4       EXTI_IMR3_IM71                          /*!< PVM VDDIO4 EXTI Line  */
#define PWR_EXTI_LINE_PVM_VDDIO5       EXTI_IMR3_IM72                          /*!< PVM VDDIO5 EXTI Line  */
#define PWR_EXTI_LINE_PVM_VDDUSB       EXTI_IMR3_IM73                          /*!< PVM VDD USB EXTI Line */
#define PWR_EXTI_LINE_PVM_VDDA         EXTI_IMR3_IM74                          /*!< PVM VDD ADC EXTI Line */
/**
  * @}
  */

/** @defgroup PWREx_PVM_Type PWR Extended Voltage Monitoring Type
  * @{
  */
#define PWR_VDDUSB_VM                  (0U)                                    /*!< Independent USB voltage monitor    */
#define PWR_VDDIO2_VM                  (1U)                                    /*!< Independent VDDIO2 voltage monitor */
#define PWR_VDDIO3_VM                  (2U)                                    /*!< Independent VDDIO3 voltage monitor */
#define PWR_VDDIO4_VM                  (3U)                                    /*!< Independent VDDIO4 voltage monitor */
#define PWR_VDDIO5_VM                  (4U)                                    /*!< Independent VDDIO5 voltage monitor */
#define PWR_VDDA_VM                    (5U)                                    /*!< Independent VDDA voltage monitor   */
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup PWREx_Private_Macros PWREx Private Macros
  * @{
  */
/** @defgroup PWREx_IS_PWR_Definitions PWREx Private macros to check input parameters
  * @{
  */
/* Check PWR regulator configuration parameter */
#define IS_PWR_SUPPLY(__SOURCE__) (((__SOURCE__) == PWR_SMPS_SUPPLY)             ||\
                                   ((__SOURCE__) == PWR_EXTERNAL_SOURCE_SUPPLY))

/* Check low power regulator parameter */
#define IS_PWR_REGULATOR(__REGULATOR__) ((__REGULATOR__) == PWR_MAINREGULATOR_ON)

/* Check voltage scale level parameter */
#define IS_PWR_REGULATOR_VOLTAGE(__VOLTAGE__) (((__VOLTAGE__) == PWR_REGULATOR_VOLTAGE_SCALE0) || \
                                               ((__VOLTAGE__) == PWR_REGULATOR_VOLTAGE_SCALE1))

/* Check PWR regulator configuration in STOP mode parameter */
#define IS_PWR_STOP_MODE_REGULATOR_VOLTAGE(__VOLTAGE__) (((__VOLTAGE__) == PWR_REGULATOR_STOP_VOLTAGE_SCALE3)  ||\
                                                         ((__VOLTAGE__) == PWR_REGULATOR_STOP_VOLTAGE_SCALE5))

/* Check PWR pulse low time configuration parameter */
#define IS_PWR_PULSE_LOW_TIME(__LOWTIME__) ((__LOWTIME__) <= 31U)

/* Check VDDCORE voltage detector low-level parameter */
#define IS_PWR_VDDCOREVM_LEVEL(__LEVEL__) (((__LEVEL__) == PWR_VDDCORE_THRESHOLD_VOS0) || \
                                           ((__LEVEL__) == PWR_VDDCORE_THRESHOLD_VOS1))

/* Check VddCORE VM mode check parameter */
#define IS_PWR_VDDCOREVM_MODE(__MODE__)                     \
  (((__MODE__) == PWR_VDDCOREVM_MODE_NORMAL)              ||\
   ((__MODE__) == PWR_VDDCOREVM_MODE_IT_RISING)           ||\
   ((__MODE__) == PWR_VDDCOREVM_MODE_IT_FALLING)          ||\
   ((__MODE__) == PWR_VDDCOREVM_MODE_IT_RISING_FALLING)   ||\
   ((__MODE__) == PWR_VDDCOREVM_MODE_EVENT_RISING)        ||\
   ((__MODE__) == PWR_VDDCOREVM_MODE_EVENT_FALLING)       ||\
   ((__MODE__) == PWR_VDDCOREVM_MODE_EVENT_RISING_FALLING))

/* Check PVM type parameter */
#define IS_PWR_PVM_TYPE(__TYPE__)  \
  (((__TYPE__) == PWR_VDDUSB_VM) ||\
   ((__TYPE__) == PWR_VDDIO2_VM) ||\
   ((__TYPE__) == PWR_VDDIO3_VM) ||\
   ((__TYPE__) == PWR_VDDIO4_VM) ||\
   ((__TYPE__) == PWR_VDDIO5_VM) ||\
   ((__TYPE__) == PWR_VDDA_VM))

/* Check PVM mode check parameter */
#define IS_PWR_PVM_MODE(__MODE__)                   \
  (((__MODE__) == PWR_PVM_MODE_NORMAL)            ||\
   ((__MODE__) == PWR_PVM_MODE_IT_RISING)         ||\
   ((__MODE__) == PWR_PVM_MODE_IT_FALLING)        ||\
   ((__MODE__) == PWR_PVM_MODE_IT_RISING_FALLING) ||\
   ((__MODE__) == PWR_PVM_MODE_EVENT_RISING)      ||\
   ((__MODE__) == PWR_PVM_MODE_EVENT_FALLING)     ||\
   ((__MODE__) == PWR_PVM_MODE_EVENT_RISING_FALLING))

/* Check the VddIO parameter */
#define IS_PWR_VDDIO(__VDDIO__)  (((__VDDIO__) == PWR_VDDIO)  || \
                                  ((__VDDIO__) == PWR_VDDIO2) || \
                                  ((__VDDIO__) == PWR_VDDIO3) || \
                                  ((__VDDIO__) == PWR_VDDIO4) || \
                                  ((__VDDIO__) == PWR_VDDIO5))

/* Check the VddIO Range parameter */
#define IS_PWR_VDDIO_RANGE(__RANGE__)  (((__RANGE__) == PWR_VDDIO_RANGE_3V3) || \
                                        ((__RANGE__) == PWR_VDDIO_RANGE_1V8))

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

/**
  * @}
  */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* STM32N6xx_HAL_PWR_EX_H */

