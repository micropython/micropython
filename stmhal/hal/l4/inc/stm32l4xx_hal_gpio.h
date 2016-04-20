/**
  ******************************************************************************
  * @file    stm32l4xx_hal_gpio.h
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    29-January-2016
  * @brief   Header file of GPIO HAL module.
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
#ifndef __STM32L4xx_HAL_GPIO_H
#define __STM32L4xx_HAL_GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal_def.h"

/** @addtogroup STM32L4xx_HAL_Driver
  * @{
  */

/** @addtogroup GPIO
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup GPIO_Exported_Types GPIO Exported Types
  * @{
  */
/**
  * @brief   GPIO Init structure definition
  */
typedef struct
{
  uint32_t Pin;        /*!< Specifies the GPIO pins to be configured.
                           This parameter can be any value of @ref GPIO_pins */

  uint32_t Mode;       /*!< Specifies the operating mode for the selected pins.
                           This parameter can be a value of @ref GPIO_mode */

  uint32_t Pull;       /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                           This parameter can be a value of @ref GPIO_pull */

  uint32_t Speed;      /*!< Specifies the speed for the selected pins.
                           This parameter can be a value of @ref GPIO_speed */

  uint32_t Alternate;  /*!< Peripheral to be connected to the selected pins 
                            This parameter can be a value of @ref GPIOEx_Alternate_function_selection */
}GPIO_InitTypeDef;

/**
  * @brief  GPIO Bit SET and Bit RESET enumeration
  */
typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
}GPIO_PinState;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup GPIO_Exported_Constants GPIO Exported Constants
  * @{
  */
/** @defgroup GPIO_pins GPIO pins
  * @{
  */
#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */

#define GPIO_PIN_MASK              ((uint32_t)0x0000FFFF) /* PIN mask for assert test */
/**
  * @}
  */

/** @defgroup GPIO_mode GPIO mode
  * @brief GPIO Configuration Mode
  *        Elements values convention: 0xX0yz00YZ
  *           - X  : GPIO mode or EXTI Mode
  *           - y  : External IT or Event trigger detection
  *           - z  : IO configuration on External IT or Event
  *           - Y  : Output type (Push Pull or Open Drain)
  *           - Z  : IO Direction mode (Input, Output, Alternate or Analog)
  * @{
  */
#define  GPIO_MODE_INPUT                        ((uint32_t)0x00000000)   /*!< Input Floating Mode                   */
#define  GPIO_MODE_OUTPUT_PP                    ((uint32_t)0x00000001)   /*!< Output Push Pull Mode                 */
#define  GPIO_MODE_OUTPUT_OD                    ((uint32_t)0x00000011)   /*!< Output Open Drain Mode                */
#define  GPIO_MODE_AF_PP                        ((uint32_t)0x00000002)   /*!< Alternate Function Push Pull Mode     */
#define  GPIO_MODE_AF_OD                        ((uint32_t)0x00000012)   /*!< Alternate Function Open Drain Mode    */
#define  GPIO_MODE_ANALOG                       ((uint32_t)0x00000003)   /*!< Analog Mode  */
#define  GPIO_MODE_ANALOG_ADC_CONTROL           ((uint32_t)0x0000000B)   /*!< Analog Mode for ADC conversion */
#define  GPIO_MODE_IT_RISING                    ((uint32_t)0x10110000)   /*!< External Interrupt Mode with Rising edge trigger detection          */
#define  GPIO_MODE_IT_FALLING                   ((uint32_t)0x10210000)   /*!< External Interrupt Mode with Falling edge trigger detection         */
#define  GPIO_MODE_IT_RISING_FALLING            ((uint32_t)0x10310000)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
#define  GPIO_MODE_EVT_RISING                   ((uint32_t)0x10120000)   /*!< External Event Mode with Rising edge trigger detection               */
#define  GPIO_MODE_EVT_FALLING                  ((uint32_t)0x10220000)   /*!< External Event Mode with Falling edge trigger detection              */
#define  GPIO_MODE_EVT_RISING_FALLING           ((uint32_t)0x10320000)   /*!< External Event Mode with Rising/Falling edge trigger detection       */
/**
  * @}
  */

/** @defgroup GPIO_speed GPIO speed
  * @brief GPIO Output Maximum frequency
  * @{
  */
#define  GPIO_SPEED_FREQ_LOW        ((uint32_t)0x00000000)   /*!< range up to 5 MHz, please refer to the product datasheet */
#define  GPIO_SPEED_FREQ_MEDIUM     ((uint32_t)0x00000001)   /*!< range  5 MHz to 25 MHz, please refer to the product datasheet */
#define  GPIO_SPEED_FREQ_HIGH       ((uint32_t)0x00000002)   /*!< range 25 MHz to 50 MHz, please refer to the product datasheet */
#define  GPIO_SPEED_FREQ_VERY_HIGH  ((uint32_t)0x00000003)   /*!< range 50 MHz to 80 MHz, please refer to the product datasheet */
/**
  * @}
  */

 /** @defgroup GPIO_pull GPIO pull
   * @brief GPIO Pull-Up or Pull-Down Activation
   * @{
   */
#define  GPIO_NOPULL        ((uint32_t)0x00000000)   /*!< No Pull-up or Pull-down activation  */
#define  GPIO_PULLUP        ((uint32_t)0x00000001)   /*!< Pull-up activation                  */
#define  GPIO_PULLDOWN      ((uint32_t)0x00000002)   /*!< Pull-down activation                */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup GPIO_Exported_Macros GPIO Exported Macros
  * @{
  */
  
/**
  * @brief  Check whether the specified EXTI line flag is set or not.
  * @param  __EXTI_LINE__: specifies the EXTI line flag to check.
  *         This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval The new state of __EXTI_LINE__ (SET or RESET).
  */
#define __HAL_GPIO_EXTI_GET_FLAG(__EXTI_LINE__)       (EXTI->PR1 & (__EXTI_LINE__))

/**
  * @brief  Clear the EXTI's line pending flags.
  * @param  __EXTI_LINE__: specifies the EXTI lines flags to clear.
  *         This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#define __HAL_GPIO_EXTI_CLEAR_FLAG(__EXTI_LINE__)     (EXTI->PR1 = (__EXTI_LINE__))

/**
  * @brief  Check whether the specified EXTI line is asserted or not.
  * @param  __EXTI_LINE__: specifies the EXTI line to check.
  *          This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval The new state of __EXTI_LINE__ (SET or RESET).
  */
#define __HAL_GPIO_EXTI_GET_IT(__EXTI_LINE__)         (EXTI->PR1 & (__EXTI_LINE__))

/**
  * @brief  Clear the EXTI's line pending bits.
  * @param  __EXTI_LINE__: specifies the EXTI lines to clear.
  *          This parameter can be any combination of GPIO_PIN_x where x can be (0..15)
  * @retval None
  */
#define __HAL_GPIO_EXTI_CLEAR_IT(__EXTI_LINE__)       (EXTI->PR1 = (__EXTI_LINE__))

/**
  * @brief  Generate a Software interrupt on selected EXTI line.
  * @param  __EXTI_LINE__: specifies the EXTI line to check.
  *          This parameter can be GPIO_PIN_x where x can be(0..15)
  * @retval None
  */
#define __HAL_GPIO_EXTI_GENERATE_SWIT(__EXTI_LINE__)  (EXTI->SWIER1 |= (__EXTI_LINE__))

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup GPIO_Private_Macros GPIO Private Macros
  * @{
  */
#define IS_GPIO_PIN_ACTION(ACTION)  (((ACTION) == GPIO_PIN_RESET) || ((ACTION) == GPIO_PIN_SET))

#define IS_GPIO_PIN(__PIN__)        (((__PIN__) & GPIO_PIN_MASK) != (uint32_t)0x00)

#define IS_GPIO_MODE(__MODE__)      (((__MODE__) == GPIO_MODE_INPUT)              ||\
                                     ((__MODE__) == GPIO_MODE_OUTPUT_PP)          ||\
                                     ((__MODE__) == GPIO_MODE_OUTPUT_OD)          ||\
                                     ((__MODE__) == GPIO_MODE_AF_PP)              ||\
                                     ((__MODE__) == GPIO_MODE_AF_OD)              ||\
                                     ((__MODE__) == GPIO_MODE_IT_RISING)          ||\
                                     ((__MODE__) == GPIO_MODE_IT_FALLING)         ||\
                                     ((__MODE__) == GPIO_MODE_IT_RISING_FALLING)  ||\
                                     ((__MODE__) == GPIO_MODE_EVT_RISING)         ||\
                                     ((__MODE__) == GPIO_MODE_EVT_FALLING)        ||\
                                     ((__MODE__) == GPIO_MODE_EVT_RISING_FALLING) ||\
                                     ((__MODE__) == GPIO_MODE_ANALOG)             ||\
                                     ((__MODE__) == GPIO_MODE_ANALOG_ADC_CONTROL))

#define IS_GPIO_SPEED(__SPEED__)    (((__SPEED__) == GPIO_SPEED_FREQ_LOW)       ||\
                                     ((__SPEED__) == GPIO_SPEED_FREQ_MEDIUM)    ||\
                                     ((__SPEED__) == GPIO_SPEED_FREQ_HIGH)      ||\
                                     ((__SPEED__) == GPIO_SPEED_FREQ_VERY_HIGH))

#define IS_GPIO_PULL(__PULL__)      (((__PULL__) == GPIO_NOPULL)   ||\
                                     ((__PULL__) == GPIO_PULLUP)   || \
                                     ((__PULL__) == GPIO_PULLDOWN))
/**
  * @}
  */

/* Include GPIO HAL Extended module */
#include "stm32l4xx_hal_gpio_ex.h"

/* Exported functions --------------------------------------------------------*/ 
/** @addtogroup GPIO_Exported_Functions GPIO Exported Functions
  * @{
  */

/** @addtogroup GPIO_Exported_Functions_Group1 Initialization/de-initialization functions 
 *  @brief    Initialization and Configuration functions
 * @{
 */
     
/* Initialization and de-initialization functions *****************************/
void              HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void              HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);

/**
  * @}
  */

/** @addtogroup GPIO_Exported_Functions_Group2 IO operation functions 
 * @{
 */
   
/* IO operation functions *****************************************************/
GPIO_PinState     HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void              HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void              HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

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

#endif /* __STM32L4xx_HAL_GPIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
