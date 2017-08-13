/**
  ******************************************************************************
  * @file    stm32l4xx_hal_pwr_ex.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of PWR HAL Extended module.
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
#ifndef __STM32L4xx_HAL_PWR_EX_H
#define __STM32L4xx_HAL_PWR_EX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"

/** @addtogroup STM32L4xx_HAL_Driver
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
  * @brief  PWR PVM configuration structure definition
  */
typedef struct
{
  uint32_t PVMType;   /*!< PVMType: Specifies which voltage is monitored and against which threshold. 
                           This parameter can be a value of @ref PWREx_PVM_Type. 
                           @arg @ref PWR_PVM_1 Peripheral Voltage Monitoring 1 enable: VDDUSB versus 1.2 V (applicable when USB feature is supported). 
                           @arg @ref PWR_PVM_2 Peripheral Voltage Monitoring 2 enable: VDDIO2 versus 0.9 V (applicable when VDDIO2 is present on device). 
                           @arg @ref PWR_PVM_3 Peripheral Voltage Monitoring 3 enable: VDDA versus 1.62 V. 
                           @arg @ref PWR_PVM_4 Peripheral Voltage Monitoring 4 enable: VDDA versus 2.2 V. */

  uint32_t Mode;      /*!< Mode: Specifies the operating mode for the selected pins.
                           This parameter can be a value of @ref PWREx_PVM_Mode. */
}PWR_PVMTypeDef;

/**
  * @}
  */ 

/* Exported constants --------------------------------------------------------*/

/** @defgroup PWREx_Exported_Constants  PWR Extended Exported Constants
  * @{
  */

/** @defgroup PWREx_WUP_Polarity Shift to apply to retrieve polarity information from PWR_WAKEUP_PINy_xxx constants
  * @{
  */
#define PWR_WUP_POLARITY_SHIFT                  0x05   /*!< Internal constant used to retrieve wakeup pin polariry */
/**
  * @}
  */  


/** @defgroup PWREx_WakeUp_Pins  PWR wake-up pins
  * @{
  */                                                      
#define PWR_WAKEUP_PIN1                 PWR_CR3_EWUP1  /*!< Wakeup pin 1 (with high level polarity) */
#define PWR_WAKEUP_PIN2                 PWR_CR3_EWUP2  /*!< Wakeup pin 2 (with high level polarity) */
#define PWR_WAKEUP_PIN3                 PWR_CR3_EWUP3  /*!< Wakeup pin 3 (with high level polarity) */
#define PWR_WAKEUP_PIN4                 PWR_CR3_EWUP4  /*!< Wakeup pin 4 (with high level polarity) */
#define PWR_WAKEUP_PIN5                 PWR_CR3_EWUP5  /*!< Wakeup pin 5 (with high level polarity) */
#define PWR_WAKEUP_PIN1_HIGH            PWR_CR3_EWUP1  /*!< Wakeup pin 1 (with high level polarity) */
#define PWR_WAKEUP_PIN2_HIGH            PWR_CR3_EWUP2  /*!< Wakeup pin 2 (with high level polarity) */
#define PWR_WAKEUP_PIN3_HIGH            PWR_CR3_EWUP3  /*!< Wakeup pin 3 (with high level polarity) */
#define PWR_WAKEUP_PIN4_HIGH            PWR_CR3_EWUP4  /*!< Wakeup pin 4 (with high level polarity) */
#define PWR_WAKEUP_PIN5_HIGH            PWR_CR3_EWUP5  /*!< Wakeup pin 5 (with high level polarity) */
#define PWR_WAKEUP_PIN1_LOW             (uint32_t)((PWR_CR4_WP1<<PWR_WUP_POLARITY_SHIFT) | PWR_CR3_EWUP1) /*!< Wakeup pin 1 (with low level polarity) */
#define PWR_WAKEUP_PIN2_LOW             (uint32_t)((PWR_CR4_WP2<<PWR_WUP_POLARITY_SHIFT) | PWR_CR3_EWUP2) /*!< Wakeup pin 2 (with low level polarity) */
#define PWR_WAKEUP_PIN3_LOW             (uint32_t)((PWR_CR4_WP3<<PWR_WUP_POLARITY_SHIFT) | PWR_CR3_EWUP3) /*!< Wakeup pin 3 (with low level polarity) */
#define PWR_WAKEUP_PIN4_LOW             (uint32_t)((PWR_CR4_WP4<<PWR_WUP_POLARITY_SHIFT) | PWR_CR3_EWUP4) /*!< Wakeup pin 4 (with low level polarity) */
#define PWR_WAKEUP_PIN5_LOW             (uint32_t)((PWR_CR4_WP5<<PWR_WUP_POLARITY_SHIFT) | PWR_CR3_EWUP5) /*!< Wakeup pin 5 (with low level polarity) */
/**
  * @}
  */

/** @defgroup PWREx_PVM_Type Peripheral Voltage Monitoring type
  * @{
  */
#if defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx)
#define PWR_PVM_1                  PWR_CR2_PVME1  /*!< Peripheral Voltage Monitoring 1 enable: VDDUSB versus 1.2 V (applicable when USB feature is supported) */
#endif /* defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) */
#define PWR_PVM_2                  PWR_CR2_PVME2  /*!< Peripheral Voltage Monitoring 2 enable: VDDIO2 versus 0.9 V (applicable when VDDIO2 is present on device) */
#define PWR_PVM_3                  PWR_CR2_PVME3  /*!< Peripheral Voltage Monitoring 3 enable: VDDA versus 1.62 V */
#define PWR_PVM_4                  PWR_CR2_PVME4  /*!< Peripheral Voltage Monitoring 4 enable: VDDA versus 2.2 V  */
/**
  * @}
  */  
  
/** @defgroup PWREx_PVM_Mode  PWR PVM interrupt and event mode
  * @{
  */
#define PWR_PVM_MODE_NORMAL                 ((uint32_t)0x00000000)   /*!< basic mode is used */
#define PWR_PVM_MODE_IT_RISING              ((uint32_t)0x00010001)   /*!< External Interrupt Mode with Rising edge trigger detection */
#define PWR_PVM_MODE_IT_FALLING             ((uint32_t)0x00010002)   /*!< External Interrupt Mode with Falling edge trigger detection */
#define PWR_PVM_MODE_IT_RISING_FALLING      ((uint32_t)0x00010003)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection */
#define PWR_PVM_MODE_EVENT_RISING           ((uint32_t)0x00020001)   /*!< Event Mode with Rising edge trigger detection */
#define PWR_PVM_MODE_EVENT_FALLING          ((uint32_t)0x00020002)   /*!< Event Mode with Falling edge trigger detection */
#define PWR_PVM_MODE_EVENT_RISING_FALLING   ((uint32_t)0x00020003)   /*!< Event Mode with Rising/Falling edge trigger detection */
/**
  * @}
  */
  
  

/** @defgroup PWREx_Regulator_Voltage_Scale  PWR Regulator voltage scale
  * @{
  */
#define PWR_REGULATOR_VOLTAGE_SCALE1       PWR_CR1_VOS_0     /*!< Voltage scaling range 1 */
#define PWR_REGULATOR_VOLTAGE_SCALE2       PWR_CR1_VOS_1     /*!< Voltage scaling range 2 */
/**
  * @}
  */

  
/** @defgroup PWREx_VBAT_Battery_Charging_Selection PWR battery charging resistor selection
  * @{
  */
#define PWR_BATTERY_CHARGING_RESISTOR_5          ((uint32_t)0x00000000) /*!< VBAT charging through a 5 kOhms resistor   */
#define PWR_BATTERY_CHARGING_RESISTOR_1_5         PWR_CR4_VBRS          /*!< VBAT charging through a 1.5 kOhms resistor */
/**
  * @}
  */
  
/** @defgroup PWREx_VBAT_Battery_Charging PWR battery charging
  * @{
  */
#define PWR_BATTERY_CHARGING_DISABLE        ((uint32_t)0x00000000)
#define PWR_BATTERY_CHARGING_ENABLE         PWR_CR4_VBE
/**
  * @}
  */ 
  
/** @defgroup PWREx_GPIO_Bit_Number GPIO bit number for I/O setting in standby/shutdown mode
  * @{
  */
#define PWR_GPIO_BIT_0   PWR_PUCRA_PA0    /*!< GPIO port I/O pin 0  */
#define PWR_GPIO_BIT_1   PWR_PUCRA_PA1    /*!< GPIO port I/O pin 1  */
#define PWR_GPIO_BIT_2   PWR_PUCRA_PA2    /*!< GPIO port I/O pin 2  */
#define PWR_GPIO_BIT_3   PWR_PUCRA_PA3    /*!< GPIO port I/O pin 3  */
#define PWR_GPIO_BIT_4   PWR_PUCRA_PA4    /*!< GPIO port I/O pin 4  */
#define PWR_GPIO_BIT_5   PWR_PUCRA_PA5    /*!< GPIO port I/O pin 5  */
#define PWR_GPIO_BIT_6   PWR_PUCRA_PA6    /*!< GPIO port I/O pin 6  */
#define PWR_GPIO_BIT_7   PWR_PUCRA_PA7    /*!< GPIO port I/O pin 7  */
#define PWR_GPIO_BIT_8   PWR_PUCRA_PA8    /*!< GPIO port I/O pin 8  */
#define PWR_GPIO_BIT_9   PWR_PUCRA_PA9    /*!< GPIO port I/O pin 9  */
#define PWR_GPIO_BIT_10  PWR_PUCRA_PA10   /*!< GPIO port I/O pin 10 */
#define PWR_GPIO_BIT_11  PWR_PUCRA_PA11   /*!< GPIO port I/O pin 11 */
#define PWR_GPIO_BIT_12  PWR_PUCRA_PA12   /*!< GPIO port I/O pin 12 */
#define PWR_GPIO_BIT_13  PWR_PUCRA_PA13   /*!< GPIO port I/O pin 13 */
#define PWR_GPIO_BIT_14  PWR_PDCRA_PA14   /*!< GPIO port I/O pin 14 */
#define PWR_GPIO_BIT_15  PWR_PUCRA_PA15   /*!< GPIO port I/O pin 15 */
/**
  * @}
  */ 
  
/** @defgroup PWREx_GPIO GPIO port
  * @{
  */
#define PWR_GPIO_A   0x00000000      /*!< GPIO port A */
#define PWR_GPIO_B   0x00000001      /*!< GPIO port B */
#define PWR_GPIO_C   0x00000002      /*!< GPIO port C */
#define PWR_GPIO_D   0x00000003      /*!< GPIO port D */
#define PWR_GPIO_E   0x00000004      /*!< GPIO port E */
#define PWR_GPIO_F   0x00000005      /*!< GPIO port F */
#define PWR_GPIO_G   0x00000006      /*!< GPIO port G */
#define PWR_GPIO_H   0x00000007      /*!< GPIO port H */
/**
  * @}
  */ 
  
/** @defgroup PWREx_PVM_EXTI_LINE PWR PVM external interrupts lines
  * @{
  */    
#if defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx)
#define PWR_EXTI_LINE_PVM1  ((uint32_t)0x00000008)  /*!< External interrupt line 35 Connected to the PVM1 EXTI Line   */
#endif /* defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) */
#define PWR_EXTI_LINE_PVM2  ((uint32_t)0x00000010)  /*!< External interrupt line 36 Connected to the PVM2 EXTI Line   */
#define PWR_EXTI_LINE_PVM3  ((uint32_t)0x00000020)  /*!< External interrupt line 37 Connected to the PVM3 EXTI Line   */
#define PWR_EXTI_LINE_PVM4  ((uint32_t)0x00000040)  /*!< External interrupt line 38 Connected to the PVM4 EXTI Line   */ 
/**
  * @}
  */ 
  
/** @defgroup PWREx_PVM_EVENT_LINE PWR PVM event lines
  * @{
  */    
#if defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx)
#define PWR_EVENT_LINE_PVM1 ((uint32_t)0x00000008)  /*!< Event line 35 Connected to the PVM1 EXTI Line */
#endif /* defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) */
#define PWR_EVENT_LINE_PVM2 ((uint32_t)0x00000010)  /*!< Event line 36 Connected to the PVM2 EXTI Line */
#define PWR_EVENT_LINE_PVM3 ((uint32_t)0x00000020)  /*!< Event line 37 Connected to the PVM3 EXTI Line */
#define PWR_EVENT_LINE_PVM4 ((uint32_t)0x00000040)  /*!< Event line 38 Connected to the PVM4 EXTI Line */ 
/**
  * @}
  */ 
  
/** @defgroup PWREx_Flag  PWR Status Flags
  *        Elements values convention: 0000 0000 0XXY YYYYb
  *           - Y YYYY  : Flag position in the XX register (5 bits)
  *           - XX  : Status register (2 bits)
  *                 - 01: SR1 register
  *                 - 10: SR2 register
  *        The only exception is PWR_FLAG_WU, encompassing all
  *        wake-up flags and set to PWR_SR1_WUF.    
  * @{   
  */  
#define PWR_FLAG_WUF1                       ((uint32_t)0x0020)   /*!< Wakeup event on wakeup pin 1 */
#define PWR_FLAG_WUF2                       ((uint32_t)0x0021)   /*!< Wakeup event on wakeup pin 2 */
#define PWR_FLAG_WUF3                       ((uint32_t)0x0022)   /*!< Wakeup event on wakeup pin 3 */
#define PWR_FLAG_WUF4                       ((uint32_t)0x0023)   /*!< Wakeup event on wakeup pin 4 */
#define PWR_FLAG_WUF5                       ((uint32_t)0x0024)   /*!< Wakeup event on wakeup pin 5 */
#define PWR_FLAG_WU                         PWR_SR1_WUF          /*!< Encompass wakeup event on all wakeup pins */
#define PWR_FLAG_SB                         ((uint32_t)0x0028)   /*!< Standby flag */
#define PWR_FLAG_WUFI                       ((uint32_t)0x002F)   /*!< Wakeup on internal wakeup line */

#define PWR_FLAG_REGLPS                     ((uint32_t)0x0048)   /*!< Low-power regulator start flag */
#define PWR_FLAG_REGLPF                     ((uint32_t)0x0049)   /*!< Low-power regulator flag */
#define PWR_FLAG_VOSF                       ((uint32_t)0x004A)   /*!< Voltage scaling flag */
#define PWR_FLAG_PVDO                       ((uint32_t)0x004B)   /*!< Power Voltage Detector output flag */
#if defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx)
#define PWR_FLAG_PVMO1                      ((uint32_t)0x004C)   /*!< Power Voltage Monitoring 1 output flag */
#endif /* defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) */
#define PWR_FLAG_PVMO2                      ((uint32_t)0x004D)   /*!< Power Voltage Monitoring 2 output flag */
#define PWR_FLAG_PVMO3                      ((uint32_t)0x004E)   /*!< Power Voltage Monitoring 3 output flag */
#define PWR_FLAG_PVMO4                      ((uint32_t)0x004F)   /*!< Power Voltage Monitoring 4 output flag */
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

#if defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx)
/**
  * @brief Enable the PVM1 Extended Interrupt Line.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_ENABLE_IT()   SET_BIT(EXTI->IMR2, PWR_EXTI_LINE_PVM1)

/**
  * @brief Disable the PVM1 Extended Interrupt Line.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_DISABLE_IT()  CLEAR_BIT(EXTI->IMR2, PWR_EXTI_LINE_PVM1)

/**
  * @brief Enable the PVM1 Event Line.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_ENABLE_EVENT()   SET_BIT(EXTI->EMR2, PWR_EVENT_LINE_PVM1)

/**
  * @brief Disable the PVM1 Event Line.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_DISABLE_EVENT()  CLEAR_BIT(EXTI->EMR2, PWR_EVENT_LINE_PVM1)

/**
  * @brief Enable the PVM1 Extended Interrupt Rising Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_ENABLE_RISING_EDGE()   SET_BIT(EXTI->RTSR2, PWR_EXTI_LINE_PVM1)

/**
  * @brief Disable the PVM1 Extended Interrupt Rising Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_DISABLE_RISING_EDGE()  CLEAR_BIT(EXTI->RTSR2, PWR_EXTI_LINE_PVM1)

/**
  * @brief Enable the PVM1 Extended Interrupt Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_ENABLE_FALLING_EDGE()   SET_BIT(EXTI->FTSR2, PWR_EXTI_LINE_PVM1)


/**
  * @brief Disable the PVM1 Extended Interrupt Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_DISABLE_FALLING_EDGE()  CLEAR_BIT(EXTI->FTSR2, PWR_EXTI_LINE_PVM1)


/**
  * @brief  PVM1 EXTI line configuration: set rising & falling edge trigger.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_ENABLE_RISING_FALLING_EDGE()  \
  do {                                                    \
    __HAL_PWR_PVM1_EXTI_ENABLE_RISING_EDGE();             \
    __HAL_PWR_PVM1_EXTI_ENABLE_FALLING_EDGE();            \
  } while(0)

/**
  * @brief Disable the PVM1 Extended Interrupt Rising & Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_DISABLE_RISING_FALLING_EDGE()  \
  do {                                                     \
    __HAL_PWR_PVM1_EXTI_DISABLE_RISING_EDGE();             \
    __HAL_PWR_PVM1_EXTI_DISABLE_FALLING_EDGE();            \
  } while(0)

/**
  * @brief  Generate a Software interrupt on selected EXTI line.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER2, PWR_EXTI_LINE_PVM1)

/**
  * @brief Check whether the specified PVM1 EXTI interrupt flag is set or not.
  * @retval EXTI PVM1 Line Status.
  */
#define __HAL_PWR_PVM1_EXTI_GET_FLAG()  (EXTI->PR2 & PWR_EXTI_LINE_PVM1)

/**
  * @brief Clear the PVM1 EXTI flag.
  * @retval None
  */
#define __HAL_PWR_PVM1_EXTI_CLEAR_FLAG()  WRITE_REG(EXTI->PR2, PWR_EXTI_LINE_PVM1)

#endif /* defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) */


/**
  * @brief Enable the PVM2 Extended Interrupt Line.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_ENABLE_IT()   SET_BIT(EXTI->IMR2, PWR_EXTI_LINE_PVM2)

/**
  * @brief Disable the PVM2 Extended Interrupt Line.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_DISABLE_IT()  CLEAR_BIT(EXTI->IMR2, PWR_EXTI_LINE_PVM2)

/**
  * @brief Enable the PVM2 Event Line.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_ENABLE_EVENT()   SET_BIT(EXTI->EMR2, PWR_EVENT_LINE_PVM2)

/**
  * @brief Disable the PVM2 Event Line.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_DISABLE_EVENT()  CLEAR_BIT(EXTI->EMR2, PWR_EVENT_LINE_PVM2)

/**
  * @brief Enable the PVM2 Extended Interrupt Rising Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_ENABLE_RISING_EDGE()   SET_BIT(EXTI->RTSR2, PWR_EXTI_LINE_PVM2)

/**
  * @brief Disable the PVM2 Extended Interrupt Rising Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_DISABLE_RISING_EDGE()  CLEAR_BIT(EXTI->RTSR2, PWR_EXTI_LINE_PVM2)

/**
  * @brief Enable the PVM2 Extended Interrupt Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_ENABLE_FALLING_EDGE()   SET_BIT(EXTI->FTSR2, PWR_EXTI_LINE_PVM2)


/**
  * @brief Disable the PVM2 Extended Interrupt Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_DISABLE_FALLING_EDGE()  CLEAR_BIT(EXTI->FTSR2, PWR_EXTI_LINE_PVM2)


/**
  * @brief  PVM2 EXTI line configuration: set rising & falling edge trigger.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_ENABLE_RISING_FALLING_EDGE()  \
  do {                                                    \
    __HAL_PWR_PVM2_EXTI_ENABLE_RISING_EDGE();             \
    __HAL_PWR_PVM2_EXTI_ENABLE_FALLING_EDGE();            \
  } while(0)
  
/**
  * @brief Disable the PVM2 Extended Interrupt Rising & Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_DISABLE_RISING_FALLING_EDGE()  \
  do {                                                     \
    __HAL_PWR_PVM2_EXTI_DISABLE_RISING_EDGE();             \
    __HAL_PWR_PVM2_EXTI_DISABLE_FALLING_EDGE();            \
  } while(0)

/**
  * @brief  Generate a Software interrupt on selected EXTI line.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER2, PWR_EXTI_LINE_PVM2)

/**
  * @brief Check whether the specified PVM2 EXTI interrupt flag is set or not.
  * @retval EXTI PVM2 Line Status.
  */
#define __HAL_PWR_PVM2_EXTI_GET_FLAG()  (EXTI->PR2 & PWR_EXTI_LINE_PVM2)

/**
  * @brief Clear the PVM2 EXTI flag.
  * @retval None
  */
#define __HAL_PWR_PVM2_EXTI_CLEAR_FLAG()  WRITE_REG(EXTI->PR2, PWR_EXTI_LINE_PVM2)



/**
  * @brief Enable the PVM3 Extended Interrupt Line.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_ENABLE_IT()   SET_BIT(EXTI->IMR2, PWR_EXTI_LINE_PVM3)

/**
  * @brief Disable the PVM3 Extended Interrupt Line.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_DISABLE_IT()  CLEAR_BIT(EXTI->IMR2, PWR_EXTI_LINE_PVM3)

/**
  * @brief Enable the PVM3 Event Line.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_ENABLE_EVENT()   SET_BIT(EXTI->EMR2, PWR_EVENT_LINE_PVM3)

/**
  * @brief Disable the PVM3 Event Line.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_DISABLE_EVENT()  CLEAR_BIT(EXTI->EMR2, PWR_EVENT_LINE_PVM3)

/**
  * @brief Enable the PVM3 Extended Interrupt Rising Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_ENABLE_RISING_EDGE()   SET_BIT(EXTI->RTSR2, PWR_EXTI_LINE_PVM3)

/**
  * @brief Disable the PVM3 Extended Interrupt Rising Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_DISABLE_RISING_EDGE()  CLEAR_BIT(EXTI->RTSR2, PWR_EXTI_LINE_PVM3)

/**
  * @brief Enable the PVM3 Extended Interrupt Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_ENABLE_FALLING_EDGE()   SET_BIT(EXTI->FTSR2, PWR_EXTI_LINE_PVM3)


/**
  * @brief Disable the PVM3 Extended Interrupt Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_DISABLE_FALLING_EDGE()  CLEAR_BIT(EXTI->FTSR2, PWR_EXTI_LINE_PVM3)


/**
  * @brief  PVM3 EXTI line configuration: set rising & falling edge trigger.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_ENABLE_RISING_FALLING_EDGE()  \
  do {                                                    \
    __HAL_PWR_PVM3_EXTI_ENABLE_RISING_EDGE();             \
    __HAL_PWR_PVM3_EXTI_ENABLE_FALLING_EDGE();            \
  } while(0)
  
/**
  * @brief Disable the PVM3 Extended Interrupt Rising & Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_DISABLE_RISING_FALLING_EDGE()  \
  do {                                                     \
    __HAL_PWR_PVM3_EXTI_DISABLE_RISING_EDGE();             \
    __HAL_PWR_PVM3_EXTI_DISABLE_FALLING_EDGE();            \
  } while(0)

/**
  * @brief  Generate a Software interrupt on selected EXTI line.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER2, PWR_EXTI_LINE_PVM3)

/**
  * @brief Check whether the specified PVM3 EXTI interrupt flag is set or not.
  * @retval EXTI PVM3 Line Status.
  */
#define __HAL_PWR_PVM3_EXTI_GET_FLAG()  (EXTI->PR2 & PWR_EXTI_LINE_PVM3)

/**
  * @brief Clear the PVM3 EXTI flag.
  * @retval None
  */
#define __HAL_PWR_PVM3_EXTI_CLEAR_FLAG()  WRITE_REG(EXTI->PR2, PWR_EXTI_LINE_PVM3)




/**
  * @brief Enable the PVM4 Extended Interrupt Line.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_ENABLE_IT()   SET_BIT(EXTI->IMR2, PWR_EXTI_LINE_PVM4)

/**
  * @brief Disable the PVM4 Extended Interrupt Line.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_DISABLE_IT()  CLEAR_BIT(EXTI->IMR2, PWR_EXTI_LINE_PVM4)

/**
  * @brief Enable the PVM4 Event Line.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_ENABLE_EVENT()   SET_BIT(EXTI->EMR2, PWR_EVENT_LINE_PVM4)

/**
  * @brief Disable the PVM4 Event Line.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_DISABLE_EVENT()  CLEAR_BIT(EXTI->EMR2, PWR_EVENT_LINE_PVM4)

/**
  * @brief Enable the PVM4 Extended Interrupt Rising Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_ENABLE_RISING_EDGE()   SET_BIT(EXTI->RTSR2, PWR_EXTI_LINE_PVM4)

/**
  * @brief Disable the PVM4 Extended Interrupt Rising Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_DISABLE_RISING_EDGE()  CLEAR_BIT(EXTI->RTSR2, PWR_EXTI_LINE_PVM4)

/**
  * @brief Enable the PVM4 Extended Interrupt Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_ENABLE_FALLING_EDGE()   SET_BIT(EXTI->FTSR2, PWR_EXTI_LINE_PVM4)


/**
  * @brief Disable the PVM4 Extended Interrupt Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_DISABLE_FALLING_EDGE()  CLEAR_BIT(EXTI->FTSR2, PWR_EXTI_LINE_PVM4)


/**
  * @brief  PVM4 EXTI line configuration: set rising & falling edge trigger.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_ENABLE_RISING_FALLING_EDGE()  \
  do {                                                    \
    __HAL_PWR_PVM4_EXTI_ENABLE_RISING_EDGE();             \
    __HAL_PWR_PVM4_EXTI_ENABLE_FALLING_EDGE();            \
  } while(0)
  
/**
  * @brief Disable the PVM4 Extended Interrupt Rising & Falling Trigger.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_DISABLE_RISING_FALLING_EDGE()  \
  do {                                                     \
    __HAL_PWR_PVM4_EXTI_DISABLE_RISING_EDGE();             \
    __HAL_PWR_PVM4_EXTI_DISABLE_FALLING_EDGE();            \
  } while(0)

/**
  * @brief  Generate a Software interrupt on selected EXTI line.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_GENERATE_SWIT() SET_BIT(EXTI->SWIER2, PWR_EXTI_LINE_PVM4)

/**
  * @brief Check whether or not the specified PVM4 EXTI interrupt flag is set.
  * @retval EXTI PVM4 Line Status.
  */
#define __HAL_PWR_PVM4_EXTI_GET_FLAG()  (EXTI->PR2 & PWR_EXTI_LINE_PVM4)

/**
  * @brief Clear the PVM4 EXTI flag.
  * @retval None
  */
#define __HAL_PWR_PVM4_EXTI_CLEAR_FLAG()  WRITE_REG(EXTI->PR2, PWR_EXTI_LINE_PVM4)


/**
  * @brief Configure the main internal regulator output voltage.
  * @param  __REGULATOR__: specifies the regulator output voltage to achieve
  *         a tradeoff between performance and power consumption.
  *          This parameter can be one of the following values:
  *            @arg @ref PWR_REGULATOR_VOLTAGE_SCALE1  Regulator voltage output range 1 mode,
  *                                                typical output voltage at 1.2 V,  
  *                                                system frequency up to 80 MHz.
  *            @arg @ref PWR_REGULATOR_VOLTAGE_SCALE2  Regulator voltage output range 2 mode,
  *                                                typical output voltage at 1.0 V,                
  *                                                system frequency up to 26 MHz.  
  * @note  This macro is similar to HAL_PWREx_ControlVoltageScaling() API but doesn't check
  *        whether or not VOSF flag is cleared when moving from range 2 to range 1. User
  *        may resort to __HAL_PWR_GET_FLAG() macro to check VOSF bit resetting.   
  * @retval None
  */
#define __HAL_PWR_VOLTAGESCALING_CONFIG(__REGULATOR__) do {                                                     \
                                                            __IO uint32_t tmpreg;                               \
                                                            MODIFY_REG(PWR->CR1, PWR_CR1_VOS, (__REGULATOR__)); \
                                                            /* Delay after an RCC peripheral clock enabling */  \
                                                            tmpreg = READ_BIT(PWR->CR1, PWR_CR1_VOS);           \
                                                            UNUSED(tmpreg);                                     \
                                                          } while(0)

/**
  * @}
  */
  
/* Private macros --------------------------------------------------------*/
/** @addtogroup  PWREx_Private_Macros   PWR Extended Private Macros
  * @{
  */

#define IS_PWR_WAKEUP_PIN(PIN) (((PIN) == PWR_WAKEUP_PIN1) || \
                                ((PIN) == PWR_WAKEUP_PIN2) || \
                                ((PIN) == PWR_WAKEUP_PIN3) || \
                                ((PIN) == PWR_WAKEUP_PIN4) || \
                                ((PIN) == PWR_WAKEUP_PIN5) || \
                                ((PIN) == PWR_WAKEUP_PIN1_HIGH) || \
                                ((PIN) == PWR_WAKEUP_PIN2_HIGH) || \
                                ((PIN) == PWR_WAKEUP_PIN3_HIGH) || \
                                ((PIN) == PWR_WAKEUP_PIN4_HIGH) || \
                                ((PIN) == PWR_WAKEUP_PIN5_HIGH) || \
                                ((PIN) == PWR_WAKEUP_PIN1_LOW) || \
                                ((PIN) == PWR_WAKEUP_PIN2_LOW) || \
                                ((PIN) == PWR_WAKEUP_PIN3_LOW) || \
                                ((PIN) == PWR_WAKEUP_PIN4_LOW) || \
                                ((PIN) == PWR_WAKEUP_PIN5_LOW))
                                
#if defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx)
#define IS_PWR_PVM_TYPE(TYPE) (((TYPE) == PWR_PVM_1) ||\
                               ((TYPE) == PWR_PVM_2) ||\
                               ((TYPE) == PWR_PVM_3) ||\
                               ((TYPE) == PWR_PVM_4))
#elif defined (STM32L471xx)
#define IS_PWR_PVM_TYPE(TYPE) (((TYPE) == PWR_PVM_2) ||\
                               ((TYPE) == PWR_PVM_3) ||\
                               ((TYPE) == PWR_PVM_4))
#endif

#define IS_PWR_PVM_MODE(MODE)  (((MODE) == PWR_PVM_MODE_NORMAL)              ||\
                                ((MODE) == PWR_PVM_MODE_IT_RISING)           ||\
                                ((MODE) == PWR_PVM_MODE_IT_FALLING)          ||\
                                ((MODE) == PWR_PVM_MODE_IT_RISING_FALLING)   ||\
                                ((MODE) == PWR_PVM_MODE_EVENT_RISING)        ||\
                                ((MODE) == PWR_PVM_MODE_EVENT_FALLING)       ||\
                                ((MODE) == PWR_PVM_MODE_EVENT_RISING_FALLING))  
                                
#define IS_PWR_VOLTAGE_SCALING_RANGE(RANGE) (((RANGE) == PWR_REGULATOR_VOLTAGE_SCALE1) || \
                                             ((RANGE) == PWR_REGULATOR_VOLTAGE_SCALE2))
                                             
#define IS_PWR_BATTERY_RESISTOR_SELECT(RESISTOR) (((RESISTOR) == PWR_BATTERY_CHARGING_RESISTOR_5) ||\
                                                  ((RESISTOR) == PWR_BATTERY_CHARGING_RESISTOR_1_5))  
                                                  
#define IS_PWR_BATTERY_CHARGING(CHARGING) (((CHARGING) == PWR_BATTERY_CHARGING_DISABLE) ||\
                                           ((CHARGING) == PWR_BATTERY_CHARGING_ENABLE)) 
                                           
#define IS_PWR_GPIO_BIT_NUMBER(BIT_NUMBER) (((BIT_NUMBER) & GPIO_PIN_MASK) != (uint32_t)0x00)
                                           
                             
#define IS_PWR_GPIO(GPIO) (((GPIO) == PWR_GPIO_A) ||\
                           ((GPIO) == PWR_GPIO_B) ||\
                           ((GPIO) == PWR_GPIO_C) ||\
                           ((GPIO) == PWR_GPIO_D) ||\
                           ((GPIO) == PWR_GPIO_E) ||\
                           ((GPIO) == PWR_GPIO_F) ||\
                           ((GPIO) == PWR_GPIO_G) ||\
                           ((GPIO) == PWR_GPIO_H))


/**
  * @}
  */  
  

/** @addtogroup PWREx_Exported_Functions PWR Extended Exported Functions
  * @{
  */
  
/** @addtogroup PWREx_Exported_Functions_Group1 Extended Peripheral Control functions 
  * @{
  */


/* Peripheral Control functions  **********************************************/
uint32_t HAL_PWREx_GetVoltageRange(void);
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);
void HAL_PWREx_EnableBatteryCharging(uint32_t ResistorSelection);
void HAL_PWREx_DisableBatteryCharging(void);
#if defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx)
void HAL_PWREx_EnableVddUSB(void);
void HAL_PWREx_DisableVddUSB(void);
#endif /* defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) */
void HAL_PWREx_EnableVddIO2(void);
void HAL_PWREx_DisableVddIO2(void);
void HAL_PWREx_EnableInternalWakeUpLine(void);
void HAL_PWREx_DisableInternalWakeUpLine(void);
HAL_StatusTypeDef HAL_PWREx_EnableGPIOPullUp(uint32_t GPIO, uint32_t GPIONumber);
HAL_StatusTypeDef HAL_PWREx_DisableGPIOPullUp(uint32_t GPIO, uint32_t GPIONumber);
HAL_StatusTypeDef HAL_PWREx_EnableGPIOPullDown(uint32_t GPIO, uint32_t GPIONumber);
HAL_StatusTypeDef HAL_PWREx_DisableGPIOPullDown(uint32_t GPIO, uint32_t GPIONumber);
void HAL_PWREx_EnablePullUpPullDownConfig(void);
void HAL_PWREx_DisablePullUpPullDownConfig(void);
void HAL_PWREx_EnableSRAM2ContentRetention(void);
void HAL_PWREx_DisableSRAM2ContentRetention(void);
#if defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx)
void HAL_PWREx_EnablePVM1(void);
void HAL_PWREx_DisablePVM1(void);
#endif /* defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) */
void HAL_PWREx_EnablePVM2(void);
void HAL_PWREx_DisablePVM2(void);
void HAL_PWREx_EnablePVM3(void);
void HAL_PWREx_DisablePVM3(void);
void HAL_PWREx_EnablePVM4(void);
void HAL_PWREx_DisablePVM4(void);
HAL_StatusTypeDef HAL_PWREx_ConfigPVM(PWR_PVMTypeDef *sConfigPVM);


/* Low Power modes configuration functions ************************************/
void HAL_PWREx_EnableLowPowerRunMode(void);
HAL_StatusTypeDef HAL_PWREx_DisableLowPowerRunMode(void);
void HAL_PWREx_EnterSTOP0Mode(uint8_t STOPEntry);
void HAL_PWREx_EnterSTOP1Mode(uint8_t STOPEntry);
void HAL_PWREx_EnterSTOP2Mode(uint8_t STOPEntry);
void HAL_PWREx_EnterSHUTDOWNMode(void);

void HAL_PWREx_PVD_PVM_IRQHandler(void);
#if defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx)
void HAL_PWREx_PVM1Callback(void);
#endif /* defined (STM32L475xx) || defined (STM32L476xx) || defined (STM32L485xx) || defined (STM32L486xx) */
void HAL_PWREx_PVM2Callback(void);
void HAL_PWREx_PVM3Callback(void);
void HAL_PWREx_PVM4Callback(void);


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


#endif /* __STM32L4xx_HAL_PWR_EX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
